#include "xscreen.h"
#include <QtWidgets/QApplication>
#include "XCaptureThread.h"

int main(int argc, char *argv[])
{
	XCaptureThread::Get()->Start();
	QThread::msleep(3000);
	XCaptureThread::Get()->Stop();

	QApplication a(argc, argv);
	XScreen w;
	w.show();
	return a.exec();
}
