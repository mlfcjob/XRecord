#include "XScreenRecord.h"
#include "XCaptureThread.h"
#include "XAudioThread.h"
#include "XVideoWriter.h"
#include <iostream>

using namespace std;

XScreenRecord::XScreenRecord()
{
}


XScreenRecord::~XScreenRecord()
{
}



bool XScreenRecord::Start(const char* filename)
{
	Stop();
	if (!filename) {
		return false;
	}

	mutex.lock();
	isExit = false;

	//init video record
	XCaptureThread::Get()->Start();

	//init audio record
	XAudioThread::Get()->Start();

	// init encoder
	XVideoWriter::Get(0)->inWidth = XCaptureThread::Get()->width;
	XVideoWriter::Get(0)->inHeight = XCaptureThread::Get()->height;
	XVideoWriter::Get(0)->outWidth = outWidth;
	XVideoWriter::Get(0)->outHeight = outHeight;
	XVideoWriter::Get(0)->outFps = fps;

	XVideoWriter::Get(0)->Init(filename);
	XVideoWriter::Get(0)->AddVideoStream();
	XVideoWriter::Get(0)->AddAudioStream();
	if (!XVideoWriter::Get(0)->WriteHeader()) {
		mutex.unlock();
		Stop();
		return false;
	}

	mutex.unlock();
	start();
}

void XScreenRecord::run()
{
	while (!isExit)
	{
		mutex.lock();

		if (XVideoWriter::Get(0)->IsVideoBefore()) {
			// write video
			char *rgb = XCaptureThread::Get()->GetRGB();
			if (rgb)
			{
				AVPacket *p = XVideoWriter::Get(0)->EncodeVideo((unsigned char*)rgb);
				delete rgb;
				XVideoWriter::Get(0)->WriteFrame(p);
				cout << " @ ";
			}
		}
		else {
			// write audio
			char *pcm = XAudioThread::Get()->GetPcm();
			if (pcm)
			{
				AVPacket *p = XVideoWriter::Get(0)->EncodeAudio((unsigned char*)pcm);
				delete pcm;
				XVideoWriter::Get(0)->WriteFrame(p);
				cout << " # ";
			}
		}
		
		msleep(10);
		mutex.unlock();
	}
}

void XScreenRecord::Stop()
{
	mutex.lock();
	isExit = true;
	mutex.unlock();
	wait();

	mutex.lock();
	XCaptureThread::Get()->Stop();
	XAudioThread::Get()->Stop();
	XVideoWriter::Get(0)->WriteEnd();
	XVideoWriter::Get(0)->Close();
	mutex.unlock();
}