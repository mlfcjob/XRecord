#include "XVideoWriter.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/opt.h>
#include <libswresample/swresample.h>
}
#include <iostream>

#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "swscale.lib")
#pragma comment(lib, "swresample.lib")


using namespace std;

class CXVideoWriter :public XVideoWriter
{
public:
	AVFormatContext *ic = NULL;  // 封装mp4上下文
	AVCodecContext *vc = NULL;   // 视频编码器上下文
	AVCodecContext *ac = NULL;   // 音频编码器上下文
	AVStream    *vs = NULL;      // 视频流
	AVStream    *as = NULL;      // 音频流
	SwsContext  *vsc = NULL;     // 像素转换上下文
	SwrContext  *asc = NULL;     // 音频重采样上下文
	AVFrame     *yuv = NULL;     // 输出yuv
	AVFrame     *pcm = NULL;     // 输出pcm
	int         vpts = 0;        // 视频的pts
	int         apts = 0;        // 音频的pts

	void Close()
	{
		if (ic) {
			avformat_close_input(&ic);
		}

		if (vc) {
			avcodec_close(vc);
			avcodec_free_context(&vc);
		}

		if (ac) {
			avcodec_close(ac);
			avcodec_free_context(&ac);
		}

		if (yuv) {
			av_frame_free(&yuv);
		}

		if (vsc)
		{
			sws_freeContext(vsc);
			vsc = NULL;
		}

		if (asc)
		{
			swr_free(&asc);
		}
	}

	bool Init(const char *file)
	{
		Close();

		// muxer output context
		ret = avformat_alloc_output_context2(&ic, NULL, NULL, file);
		if (!ic) {
			cerr << "avformat_alloc_output_context2 failed " << endl;
			return false;
		}

		//filename = string(file);
		filename = file;

		return true;
	}

	bool AddVideoStream()
	{
		if (!ic) {
			return false;
		}

		// 1 create video encode
		AVCodec *codec = avcodec_find_encoder(AV_CODEC_ID_H264);
		if (!codec) {
			cerr << "find encoder failed" << endl;
			return false;
		}

		// 2 open encoder
		vc = avcodec_alloc_context3(codec);
		if (!vc) {
			cerr << "avcodec_allo_context3 failed" << endl;
			return false;
		}


		// bitrate, bytes per sec after encode
		vc->bit_rate = vBitrate;
		vc->width = outWidth;
		vc->height = outHeight;
		// timebase
		vc->time_base = {1, outFps};
		vc->framerate = {outFps, 1};

		vc->gop_size = 50; // IDR ?? 多少帧一个关键帧
		vc->max_b_frames = 0;  // no B frames

		vc->pix_fmt = AV_PIX_FMT_YUV420P;
		vc->codec_id = AV_CODEC_ID_H264;

		av_opt_set(vc->priv_data, "preset", "superfast", 0);
		vc->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

		ret = avcodec_open2(vc, codec, NULL);
		if (ret != 0)
		{
			cout << "avcodec_open2 failed" << endl;
			return false;
		}
		cout << "avcodec_open2 success" << endl;

		// add video stream to output context
		vs = avformat_new_stream(ic, NULL);
		vs->codecpar->codec_tag = 0;   //流内部不包含编码信息  no repeat SPS/PPS header
		avcodec_parameters_from_context(vs->codecpar, vc);

		av_dump_format(ic, 0, filename.c_str(), 1);


		// scale context init
		vsc = sws_getCachedContext(vsc,
			              inWidth, inHeight, (AVPixelFormat)inPixFmt,  // input param
			              outWidth, outHeight, AV_PIX_FMT_YUV420P, // output param
			              SWS_BICUBIC, NULL, NULL, NULL);

		if (!vsc) {
			cerr << "sws_getCachedContext failed" << endl;
			return false;
		}

		// 
		if (!yuv) {
			yuv = av_frame_alloc();
			yuv->format = AV_PIX_FMT_YUV420P;
			yuv->width = outWidth;
			yuv->height = outHeight;
			yuv->pts = 0;
			ret = av_frame_get_buffer(yuv, 32);
			if (ret != 0) {
				cerr << "av_frame_get_buffer failed" << endl;
				return false;
			}
		}


		return true;
	}

	AVPacket * EncodeVideo(const unsigned char *data)
	{
		AVPacket *p = NULL;
		uint8_t *indata[AV_NUM_DATA_POINTERS] = { 0 };
		indata[0] = (uint8_t *)data;
		int inlinsize[AV_NUM_DATA_POINTERS] = { 0 };
		inlinsize[0] = inWidth * 4;

		// rgb to yuv
		int h = sws_scale(vsc, indata, inlinsize, 0, inHeight, yuv->data, yuv->linesize);
		if (h < 0) {
			cout << "sws scale error" << endl;
			return p;
		}

		// encode
		p = av_packet_alloc();
		yuv->pts = vpts++;
		av_init_packet(p);

		ret = avcodec_send_frame(vc, yuv);
		if (ret != 0) {
			cout << "avcodec_send_frame failed" << endl;
			return NULL;
		}

		ret = avcodec_receive_packet(vc, p);
		if (ret != 0 || p->size <= 0)
		{
			cout << "avcodec_receieve packet failed" << endl;
			av_packet_free(&p);
			return false;
		}

		av_packet_rescale_ts(p, vc->time_base, vs->time_base);
		return p;
	}

	bool WriteHeader()
	{
		if (!ic)
		{
			return false;
		}

		// open io
		ret = avio_open(&ic->pb, filename.c_str(), AVIO_FLAG_WRITE);

		//write header
		ret = avformat_write_header(ic, NULL);
		if (ret != 0)
		{
			cout << "avformt_write_header failed" << endl;
			return false;
		}

		return true;
	}

	// 会释放pkt的空间
	bool WriteFrame(AVPacket *pkt)
	{
		if (!ic || !pkt || pkt->size <= 0)
		{
			return false;
		}

		//ret = av_write_frame(ic, pkt);
		//if (ret != 0)
		//{
		//	cout << "av_write_frame failed " << endl;
		//	return false;
		//}
		//av_packet_unref(pkt);

		ret = av_interleaved_write_frame(ic,pkt);
		if (ret != 0)
		{
			cout << "av_interleaved_write_frame failed " << endl;
			return false;
		}

		return true;
	}

	bool WriteEnd() {
		if (!ic || !ic->pb) {
			return false;
		}

		ret = av_write_trailer(ic);
		if (ret != 0)
		{
			cout << "av_write_trailer failed" << endl;
			return false;
		}

		avio_close(ic->pb);
		return true;
	}


	bool AddAudioStream()
	{
		// 1 find aac encoder
		AVCodec *codec = avcodec_find_encoder(AV_CODEC_ID_AAC);
		if (!codec) {
			cout << "avcodec_find_encoder AAC failed" << endl;
			return false;
		}

		// 2 create and open aac encdoer
		ac = avcodec_alloc_context3(codec);
		if (!ac)
		{
			cout << "avcodec_alloc_context3 failed" << endl;
			return false;
		}

		ac->codec_id = AV_CODEC_ID_AAC;
		ac->bit_rate = aBitrate;
		ac->sample_rate = outSampleRate;
		ac->sample_fmt = (AVSampleFormat)outSampleFmt;
		ac->channels = outChannels;
		ac->channel_layout = av_get_default_channel_layout(outChannels);
		ac->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
		ac->codec_tag = 0;

		ret = avcodec_open2(ac, codec, NULL);
		if (ret != 0)
		{
			avcodec_free_context(&ac);
			cout << "avcodec_open2 failed audio" << endl;
			return false;
		}

		cout << "avcodec open aac success " << endl;

		// add audio stream
		as = avformat_new_stream(ic, codec);
		if (!as)
		{
			cout << "avfomat_new_stream(Audio) failed" << endl;
			return false;
		}

		as->codecpar->codec_tag = 0;
		avcodec_parameters_from_context(as->codecpar, ac);

		av_dump_format(ic, 0, filename.c_str(), 1);


		// auido resample context init 
		asc = swr_alloc_set_opts(asc, ac->channel_layout, ac->sample_fmt, ac->sample_rate,   // output format 
			                     av_get_default_channel_layout(inChannels), (AVSampleFormat)inSampleFmt, inSampleRate,// input format
								0, 0);
		if (!asc)
		{
			cout << "swr_alloc set opts failed" << endl;
			return false;
		}

		ret = swr_init(asc);
		if (ret != 0)
		{
			cout << "swr init failed" << endl;
			return false;
		}

		// pcm after resample
		if (!pcm)
		{
			pcm = av_frame_alloc();
			pcm->format = ac->sample_fmt;
			pcm->channels = ac->channels;
			pcm->channel_layout = ac->channel_layout;
			pcm->nb_samples = outNBSample; // samples per sec

			ret = av_frame_get_buffer(pcm, 0);
			if (ret < 0)
			{
				cout << "av_frame_get buffer failed (audio pcm)" << endl;
				return false;
			}
		}

		cout << "audio avframe create success" << endl;
		return true;
	}

	AVPacket *EncodeAudio(const unsigned char *data)
	{
		if (data == NULL)
		{
			cout << "input data is null" << endl;
			return false;
		}

		AVPacket *p = NULL;

		const uint8_t *indata[AV_NUM_DATA_POINTERS] = { 0 };
		indata[0] = (const uint8_t*)data;

		int len = swr_convert(asc, pcm->data, pcm->nb_samples, indata, pcm->nb_samples);
		cout << " [" << len << "] ";
		return p;
	}

private:
	int ret;
};

XVideoWriter::XVideoWriter()
{
}


XVideoWriter::~XVideoWriter()
{
}

XVideoWriter *XVideoWriter::Get(unsigned short index)
{
	static bool isFirst = true;
	if (isFirst) {
		av_register_all();
		avcodec_register_all();
		isFirst = false;
	}
	static CXVideoWriter wrs[65535];
	return &wrs[index];
}
