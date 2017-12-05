
extern "C"
{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
}

#include <iostream>
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "swresample.lib")
#pragma comment(lib, "swscale.lib")

using namespace std;

int main()
{
	//const char *inFile = "F:\\FFOutput\\swxfdemo.mp4";
	//const char *inFile = "F:\\FFOutput\\zVision Demo Creo3.0 摩托车.mp4";
	const char *inFile = "out.rgb";
	char outFile[] = "rgb.mp4";
	//muxter and demuxers
	av_register_all();

	avcodec_register_all();

	FILE *fp = fopen(inFile, "rb");
	if (!fp)
	{
		cout << inFile << " open failed" << endl;
		cout << strerror(errno) << endl;
		getchar();
		return -1;
	}

	int width  = 640;
	int height = 480;
	int fps = 25;

	// 1 create codec
	AVCodec *codec = avcodec_find_encoder(AV_CODEC_ID_H264);
	if (!codec)
	{
		cout << "avcodec_find_encoder AV_CODEC_ID_H264 failed" << endl;
		getchar();
		return -1;
	}


	AVCodecContext *c = avcodec_alloc_context3(codec);
	if (!c)
	{
		cout << "avcodec_alloc_context3 failed" << endl;
		getchar();
		return -1;
	}

	//bitrate
	c->bit_rate = 4000000;

	c->width = width;
	c->height = height;
	c->time_base = {1, fps};
	c->framerate = {fps, 1};

	// 画面大小、关键帧
	c->gop_size = 50;
	c->max_b_frames = 0;
	c->pix_fmt = AV_PIX_FMT_YUV420P;
	c->codec_id = AV_CODEC_ID_H264;
	c->thread_count = 8;

	//全局的编码信息
	c->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

	int ret = avcodec_open2(c, codec, NULL);
	if (ret < 0)
	{
		cout << "avcodec_open2 failed" << endl;
		getchar();
		return -1;
	}

	cout << "avcodec_open2 success" << endl;

	// 2 create out context
	AVFormatContext *oc = NULL;
	avformat_alloc_output_context2(&oc, 0, 0, outFile);

	// 3 add video stream
	AVStream *st = avformat_new_stream(oc, NULL);
	//st->codec = c;
	st->id = 0;
	st->codecpar->codec_tag = 0;
	avcodec_parameters_from_context(st->codecpar, c);

	cout << "=========================================" << endl;
	av_dump_format(oc, 0, outFile, 1);
	cout << "=========================================" << endl;

	// 4 rgb to yuv (init context)
	SwsContext *ctx = NULL;
	ctx = sws_getCachedContext(ctx, width, height, AV_PIX_FMT_RGB24,
		width, height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, 0, 0, 0);

	// input space
	unsigned char *rgb = new unsigned char[width * height * 3];

	// output space
	AVFrame *yuv = av_frame_alloc();
	yuv->format = AV_PIX_FMT_YUV420P;
	yuv->width = width;
	yuv->height = height;
	ret = av_frame_get_buffer(yuv, 24);
	if (ret < 0)
	{
		cout << "av_frame_get_buffer failed" << endl;
		getchar();
		return -1;
	}

	// 5 write mp4 header
	ret = avio_open(&oc->pb, outFile, AVIO_FLAG_WRITE);
	if (ret < 0)
	{
		cout << "avio_open failed " << endl;
		getchar();
		return -1;
	}

	ret = avformat_write_header(oc, NULL);
	if (ret < 0) {
		cout << "avformat wrte header failed!" << endl;
		getchar();
		return -1;
	}

	static int i = 0;
	int p = 0;
	for (;;)
	{
		int len = fread(rgb, 1, width * height * 3, fp);
		if (len <= 0)
		{
			break;
		}

		uint8_t *indata[AV_NUM_DATA_POINTERS] = { 0 };
		indata[0] = rgb;

		int inlinesize[AV_NUM_DATA_POINTERS] = {0};
		inlinesize[0] = width * 3;

		int h = sws_scale(ctx, indata, inlinesize, 0, height, yuv->data, yuv->linesize);
		if (h <= 0)
		{
			break;
		}

		// 6 encode frame
		yuv->pts = p;
		//yuv->pict_type = AV_PICTURE_TYPE_I;
		p = p + 3600;  //: 90000 / fps  timebase {1, 90000}
		ret = avcodec_send_frame(c, yuv);

		AVPacket pkt;
		av_init_packet(&pkt);
		ret = avcodec_receive_packet(c, &pkt);
		if (ret != 0) {
			continue;
		}

		cout << "<" << pkt.size << ">"<< endl;

	/*	av_write_frame(oc, &pkt);
		av_packet_unref(&pkt);*/

		av_interleaved_write_frame(oc, &pkt);
	}

	// write video index
	av_write_trailer(oc);

	// close output io
	avio_close(oc->pb);

	// clean output context
	avformat_free_context(oc);

	// close encode
	avcodec_close(c);

	// clean encode context
	avcodec_free_context(&c);

	// clean scale context
	sws_freeContext(ctx);

	delete rgb;

	cout << "========================= end======================" << endl;
	getchar();
	return 0;
}
