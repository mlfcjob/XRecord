#pragma once
#include <QMutex>
#include <QThread>

class XScreenRecord: protected QThread
{
public:
	int outWidth = 1280;
	int outHeight = 720;
	int fps = 10;

	static XScreenRecord *Get()
	{
		static XScreenRecord sr;
		return &sr;
	}

	bool Start(const char *filename);
	void run();
	void Stop();
	virtual ~XScreenRecord();

protected:
	bool isExit = false;
	QMutex mutex;
	XScreenRecord();
};

