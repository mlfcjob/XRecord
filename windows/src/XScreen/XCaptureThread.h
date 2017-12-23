#pragma once

#include <QThread>
#include <QMutex>

class XCaptureThread:protected QThread
{
public:
	// out parameter
	int width = 1280;
	int height = 720;

	// in parameter
	int fps = 10;
	int cacheSize = 3;

	void Start();
	void run();
	void Stop();
	static XCaptureThread *Get()
	{
		static XCaptureThread ct;
		return &ct;
	}

	virtual ~XCaptureThread();

protected:
	QMutex mutex;
	bool isExit = false;
	std::list <char*> rgbs;
	XCaptureThread();
};

