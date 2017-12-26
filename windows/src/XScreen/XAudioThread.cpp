#include "XAudioThread.h"
#include <QTime>
#include <iostream>
#include <QAudioInput>
#include <QIODevice>
#include <QAudioFormat>

using namespace std;

static QAudioInput *input = NULL;
static QIODevice *io = NULL;

XAudioThread::XAudioThread()
{
}


XAudioThread::~XAudioThread()
{
}


void XAudioThread::Start()
{
	Stop();
	mutex.lock();
	isExit = false;

	QAudioFormat fmt;
	fmt.setSampleRate(sampleRate);
	fmt.setChannelCount(channels);
	fmt.setSampleSize(sampleByte * 8);
	fmt.setSampleType(QAudioFormat::UnSignedInt);
	fmt.setByteOrder(QAudioFormat::LittleEndian);
	fmt.setCodec("audio/pcm");

	input = new QAudioInput(fmt);
	io = input->start();

	mutex.unlock();
	start();
}

void XAudioThread::Stop()
{
	mutex.lock();
	isExit = true;

	while (!pcms.empty()) {
		delete pcms.front();
		pcms.pop_front();
	}
	if (input) {
		io->close();
		input->stop();
		delete input;
		input = NULL;
		delete io;
		io = NULL;
	}

	mutex.unlock();
	wait();

}
void XAudioThread::run()
{
	QTime t;
	int size = nbSample * channels * sampleByte;
	while (!isExit) {
		mutex.lock();
		if (pcms.size() > cacheSize) {
			mutex.unlock();
			msleep(5);
			continue;
		}

		char *data = new char[size];
		int readedSize = 0;
		while (readedSize < size) {
			int br = input->bytesReady();
			if (br < 1024) {
				msleep(1);
				continue;
			}

			int s = 1024;
			// last one
			if (size - readedSize < 1024) {
				s = size - readedSize;
			}

			int len = io->read(data + readedSize, s);
			readedSize += len;
		}

		pcms.push_back(data);
		mutex.unlock();
	}
}

char *XAudioThread::GetPcm()
{
	mutex.lock();
	if (pcms.empty()) {
		mutex.unlock();
		return NULL;
	}

	char *re = pcms.front();
	pcms.pop_front();
	mutex.unlock();
	return re;
}