
extern "C"
{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}

#include <iostream>
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avutil.lib")

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
		cout << inFile << "open failed" << endl;
		getchar();
		return -1;
	}

	int width  = 848;
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
	st->codec = c;
	st->id = 0;
	st->codecpar->codec_tag = 0;
	avcodec_parameters_from_context(st->codecpar, c);

	cout << "=========================================" << endl;
	av_dump_format(oc, 0, outFile, 1);
	cout << "=========================================" << endl;

	// 4 rgb to yuv
	//SwsContext *ctx = NULL;

	//AVFormatContext *ic = NULL;

	//// 1 打开输入文件
	//avformat_open_input(&ic, inFile, 0, 0);
	//if (ic == NULL)
	//{
	//	cout<<" 1 avformat_open input failed"<<endl;
	//	getchar();
	//	return -1;
	//}

	//cout << "open infile success" << endl;

	//// 2 create output context
	//AVFormatContext *oc;
	//avformat_alloc_output_context2(&oc, NULL , NULL/* mp4*/, outFile);
	//if (!oc)
	//{
	//	cerr << "avformat_alloc_output_context2 faield" << endl;
	//	getchar();
	//	return -1;
	//}

	//// 3  add thes stream 
	//AVStream *videoStream = avformat_new_stream(oc, NULL);
	//AVStream *audioStream = avformat_new_stream(oc, NULL);
 //
	//for (int i = 0; i < ic->nb_streams; i++)
	//{
	//	if (ic->streams[i]->index == AVMEDIA_TYPE_VIDEO)
	//	{
	//		avcodec_parameters_copy(videoStream->codecpar, ic->streams[i]->codecpar);
	//	}

	//	if (ic->streams[i]->index == AVMEDIA_TYPE_AUDIO)
	//	{ 
	//		avcodec_parameters_copy(audioStream->codecpar, ic->streams[i]->codecpar);
	//	}
	//}

	//// 4 copy parameters
	//// 不编码  ??  -c copy
	//videoStream->codecpar->codec_tag = 0;
	//audioStream->codecpar->codec_tag = 0;

	//av_dump_format(ic, 0, inFile, 0);
	//cout << "=======================================================================" << endl;
	//av_dump_format(oc, 0, outFile, 1);


	//// 5 open out file io, write file header
	//char buf[156] = {0};
	//int ret = avio_open(&oc->pb, outFile, AVIO_FLAG_WRITE);
	//if (ret < 0)
	//{
	//	cout << "avio open failed" << endl;
	//	getchar();
	//	return -1;
	//}

	//ret = avformat_write_header(oc, NULL);
	//if (ret < 0)
	//{
	//	cout << "avformat write header failed" << endl;
	//	getchar();
	//	return -1;
	//}


	//AVPacket pkt;
	//for(;;)
	//{ 
	//	int re = av_read_frame(ic, &pkt);
	//	if (re < 0)   //end of file
	//		break;

	//	pkt.pts = av_rescale_q_rnd(pkt.pts, ic->streams[pkt.stream_index]->time_base, oc->streams[pkt.stream_index]->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
	//	pkt.dts = av_rescale_q_rnd(pkt.dts, ic->streams[pkt.stream_index]->time_base, oc->streams[pkt.stream_index]->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
	//	pkt.pos = -1;
	//	pkt.duration = av_rescale_q_rnd(pkt.duration, ic->streams[pkt.stream_index]->time_base, oc->streams[pkt.stream_index]->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
	//	av_write_frame(oc, &pkt);
	//	av_packet_unref(&pkt);
	//	cout << "# ";
	//}

	//av_write_trailer(oc);

	//avio_close(oc->pb);

	//cout << "avformat write header success" << endl;

	getchar();
	return 0;
}
