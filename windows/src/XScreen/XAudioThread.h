#pragma once

#include <QThread>
#include <QMutex>

class XAudioThread:protected  QThread
{
public:
	int sampleRate = 44100;
	int channels = 2;
	int sampleByte = 2;
	int cacheSize = 10;
	int nbSample = 1024;

	static XAudioThread *Get() {
		static XAudioThread xa;
		return &xa;
	}
	virtual ~XAudioThread();

	// thread safe, the space need to be freed by user
	char *GetPcm();
	void Start();
	void Stop();
	void run();
protected:
	QMutex mutex;
	bool isExit = false;
	std::list<char*> pcms;
 	XAudioThread();
};

