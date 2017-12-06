
#include <iostream>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
}

#pragma comment(lib , "avcodec.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "swresample.lib")
#pragma comment(lib, "avutil.lib")

using namespace std;

int main(int argc, char *argv[])
{
	char inFile[] = "test.wav";
	char outFile[] = "out.aac";

	// muxer demuxers
	av_register_all();
	avcodec_register_all();

	int ret;

	//1 open audio encoder
	AVCodec *codec = avcodec_find_encoder(AV_CODEC_ID_AAC);
	if (!codec) {
		cout << "av_find_encoder error " << endl;
		getchar();
		return -1;
	}

	// create encode context
	AVCodecContext *c = avcodec_alloc_context3(codec);
	if (!c) {
		cout << "avcodec_alloc_context3 failed " << endl;
		getchar();
		return -1;
	}

	// set encoder's param
	c->bit_rate = 6400;
	c->sample_rate = 44100;
	c->channel_layout = AV_CH_LAYOUT_STEREO;
	c->channels = 2;
	c->sample_fmt = AV_SAMPLE_FMT_FLTP;
	c->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

	// open encoder
	ret = avcodec_open2(c, codec, NULL);
	if (ret < 0) {
		cout << "avcodec_open2 eror" << endl;
		getchar();
		return -1;
	}

	cout << "avcodec_open2 success" << endl;

	// 2 open output muxer context
	AVFormatContext *oc = NULL;
	avformat_alloc_output_context2(&oc, NULL, NULL, outFile);
	if (!oc)
	{
		cout << "avformat_alloc_output_context2 failed" << endl;
		return -1;
	}

	AVStream *st = avformat_new_stream(oc, NULL);
	st->codecpar->codec_tag = 0;
	avcodec_parameters_from_context(st->codecpar, c);

	av_dump_format(oc, 0, outFile, 1);


	// 3 open io and write header
	ret = avio_open(&oc->pb, outFile, AVIO_FLAG_WRITE);
	if (ret < 0)
	{
		cout << "avio_open failed" << endl;
		getchar();
		return -1;
	}


	ret = avformat_write_header(oc, NULL);
	if (ret < 0)
	{
		cout << "avformat_write_header failed" << endl;
		getchar();
		return -1;
	}


	// 4 create audio resample context  44100 16 2
	SwrContext *actx = NULL;
	actx = swr_alloc_set_opts(actx, c->channel_layout, c->sample_fmt, c->sample_rate,  // output format
		                      AV_CH_LAYOUT_STEREO, AV_SAMPLE_FMT_S16, 44100,   // in format 
		                      0, NULL);
	if (!actx) {
		cout << "swr_alloc_set_opts failed" << endl;
		getchar();
		return -1;
	}

	ret = swr_init(actx);
	if (ret < 0) {
		cout << "swr_init failed" << endl;
		getchar();
		return -1;
	}

	// 5 open input and resample
	AVFrame *frame = av_frame_alloc();
	frame->format = AV_SAMPLE_FMT_FLTP;
	frame->channels = 2;
	frame->channel_layout = AV_CH_LAYOUT_STEREO;
	frame->nb_samples = 1024;  // 一帧音频存放的样本数量
	ret = av_frame_get_buffer(frame, 0);
	if (ret < 0) {
		cout << "av_frame_get_buffer failed" << endl;
		getchar();
		return -1;
	}

	int readSize = frame->nb_samples * 2 * 2;
	char *pcm = new char[readSize];

	FILE *fp = fopen(inFile, "rb");
	if (!fp){
		cout << "open file failed : " << strerror(errno) << endl;
		getchar();
		return -1;
	}

	for (;;)
	{
		int len = fread(pcm, 1, readSize, fp);
		if (len <= 0)
		{
			break;
		}


		const uint8_t *data[1];
		data[0] = (uint8_t*)pcm;

		len = swr_convert(actx, frame->data, frame->nb_samples, data, frame->nb_samples);
		if (len <= 0) {
			break;
		}


		// 6 encode audio
		AVPacket pkt;
		av_init_packet(&pkt);
		ret = avcodec_send_frame(c, frame);
		if (ret != 0) {
			continue;
		}
		ret = avcodec_receive_packet(c, &pkt);
		//cout << "[" << pkt.size << "]";

		// 7 muxer audio to aac file
		pkt.stream_index = 0;
		pkt.pts = 0;
		pkt.dts = 0;

		ret = av_interleaved_write_frame(oc, &pkt);
	/*	av_write_frame(oc, &pkt);
		av_packet_unref(&pkt);*/


	}

	//	// wirte audio index 
	av_write_trailer(oc);

	// close audio output io 
	avio_close(oc->pb);

	// clean muxer context
	avformat_free_context(oc);
	
	// close endcoder
	avcodec_close(c);

	//clean encoder
	avcodec_free_context(&c);

	delete pcm;

	cout << "==========================end =========================== " << endl;
	return 0;
}