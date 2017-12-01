
#include <iostream>
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}


#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avformat.lib")

int main(int argc, char *argv[])
{
	av_register_all();

	avformat_network_init();

	avcodec_register_all();

	printf("start ffmpeg \n");

	getchar();
	return 0;
}


