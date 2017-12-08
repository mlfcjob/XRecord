#pragma once
#include <string>

class AVPacket;

class XVideoWriter
{
public:
	// input parameter
	std::string filename;
	int inWidth = 1920;
	int inHeight = 1080;
	int inPixFmt = 30;  // AV_PIX_FMT_BGRA

	// output parameter
	int vBitrate = 4000000;
	int outWidth = 1280;
	int outHeight = 720;
	int outFps = 25;

	int aBitrate = 1000;

	virtual bool Init(const char *file) = 0;
	virtual void Close() = 0;

	virtual bool AddVideoStream() = 0;
	virtual AVPacket *EncodeVideo(const unsigned char *rgb) = 0;
	virtual bool WriteHeader() = 0;
	virtual bool WriteFrame(AVPacket *pkt) = 0;
	virtual bool WriteEnd() = 0;

	static XVideoWriter *Get(unsigned short index = 0);

	virtual ~XVideoWriter();


protected:
	XVideoWriter();
};
