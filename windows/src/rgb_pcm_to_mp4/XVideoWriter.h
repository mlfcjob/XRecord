#pragma once
#include <string>

class AVPacket;

enum XSAMPLEFMT
{
	X_S16 = 1,
	X_FLATP = 8,
};

class XVideoWriter
{
public:
	// video input parameter
	std::string filename;
	int inWidth = 1920;
	int inHeight = 1080;
	int inPixFmt = 30;  // AV_PIX_FMT_BGRA

	// video output parameter
	int vBitrate = 4000000;
	int outWidth = 1280;
	int outHeight = 720;
	int outFps = 25;

    // audio input parameter
	XSAMPLEFMT inSampleFmt = X_S16;
	int  inChannels = 2;
	int  inSampleRate = 48000;

	// audio output paramter
	int aBitrate = 6400;
	int outChannels = 2;
	int outSampleRate = 48000;
	XSAMPLEFMT outSampleFmt = X_FLATP;
	int outNBSample = 1024;


	virtual bool Init(const char *file) = 0;
	virtual void Close() = 0;

	virtual bool AddVideoStream() = 0;
	virtual AVPacket *EncodeVideo(const unsigned char *rgb) = 0;
	virtual bool WriteHeader() = 0;
	virtual bool WriteFrame(AVPacket *pkt) = 0;
	virtual bool WriteEnd() = 0;

	virtual bool AddAudioStream() = 0;
	virtual AVPacket *EncodeAudio(const unsigned char *pcm) = 0;

	static XVideoWriter *Get(unsigned short index = 0);

	virtual ~XVideoWriter();
protected:
	XVideoWriter();
};
