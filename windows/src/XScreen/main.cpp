#include "xscreen.h"
#include <QtWidgets/QApplication>
#include "XCaptureThread.h"
#include "XAudioThread.h"
#include "XScreenRecord.h"
#include <qdatetime.h>
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
	//////////////////////////////////////////////////////////////////
	/////// XScreenRecord Test
	//////////////////////////////////////////////////////////////////
	/*QDateTime t = QDateTime::currentDateTime();
	QString filename = t.toString("yyyyMMdd_hhmmss");
	filename = "xscreen_" + filename;
	filename += ".mp4";
*/
	//XScreenRecord::Get()->Start(filename.toLocal8Bit());

	//getchar();
	//XScreenRecord::Get()->Stop();

	/////////////////////////////////////////////////////////////////
	////// AudioRecord class Test
	/////////////////////////////////////////////////////////////////
	//XAudioThread::Get()->Start();
	//for (;;)
	//{
	//	char *pcm = XAudioThread::Get()->GetPcm();
	//	if (pcm)
	//	{
	//		cout << " * ";
	//	}
	//}

	//XAudioThread::Get()->Stop();

	/////////////////////////////////////////////////////////////////
	////// VideoRecord class Test
	/////////////////////////////////////////////////////////////////
	//XCaptureThread::Get()->Start();
	////QThread::msleep(3000);
	//for (;;)
	//{
	//	char *data = XCaptureThread::Get()->GetRGB();
	//	if (data) {
	//		cout << " * ";
	//	}
	//}
	//XCaptureThread::Get()->Stop();

	QApplication a(argc, argv);
	XScreen w;
	w.show();
	return a.exec();
}
