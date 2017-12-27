#include "xscreen.h"

#include <iostream>
#include <QTime>
#include "XScreenRecord.h"

using namespace std;

static bool isRecord = false;
static QTime rtime;

#define RECORD_QSS " \
QPushButton:!hover{background-image:url(:/XScreen/Resources/Record_Normal_96.ico);} \
QPushButton:hover{background-image:url(:/XScreen/Resources/Record_Hot_96.ico);}\
QPushButton:pressed{background-color:rgba(255, 255, 255, 0); \
background-image:url(:/XScreen/Resources/record_pess_96.ico);}"

XScreen::XScreen(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	setWindowFlags(Qt::FramelessWindowHint);
	setAttribute(Qt::WA_TranslucentBackground);

	timerId = startTimer(100);
}

void XScreen::Record()
{

	

	isRecord = !isRecord;
	if (isRecord) {
		rtime.restart();
		ui.recorButton->setStyleSheet("background-color:rgba(255, 255, 255, 0);background-image:url(:/XScreen/Resources/stop_96.ico)");

		QDateTime t = QDateTime::currentDateTime();
		QString filename = t.toString("yyyyMMdd_hhmmss");
		filename = "xscreen_" + filename;
		filename += ".mp4";
		filename = ui.urlEdit->text() + "\\" + filename;

		XScreenRecord::Get()->outWidth = ui.widthEdit->text().toInt();
		XScreenRecord::Get()->outHeight = ui.heightEdit->text().toInt();
		XScreenRecord::Get()->fps = ui.fpsEdit->text().toInt();


		if (XScreenRecord::Get()->Start(filename.toLocal8Bit()))
		{
			return;
		}
		else {
			cout << "start record failed! " << endl;
			isRecord = false;
			ui.recorButton->setStyleSheet(RECORD_QSS);
		}
	}
	else {
		ui.recorButton->setStyleSheet(RECORD_QSS);
		XScreenRecord::Get()->Stop();
	}
}


void XScreen::timerEvent(QTimerEvent *e)
{
	if (isRecord) {
		int es = rtime.elapsed() / 1000;  // second
		char buf[1024] = { 0 };
		sprintf(buf, "%03d:%02d", es / 60, es % 60);
		ui.timelabel->setText(buf);
	}
}