#include "xscreen.h"
#include <QtWidgets/QApplication>
#include "XCaptureThread.h"
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
	XCaptureThread::Get()->Start();

	//QThread::msleep(3000);
	for (;;)
	{
		char *data = XCaptureThread::Get()->GetRGB();
		if (data) {
			cout << " * ";
		}
	}

	XCaptureThread::Get()->Stop();

	QApplication a(argc, argv);
	XScreen w;
	w.show();
	return a.exec();
}
