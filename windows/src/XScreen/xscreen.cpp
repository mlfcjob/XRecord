#include "xscreen.h"

#include <iostream>
#include <QTime>

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

	startTimer(100);
}

void XScreen::Record()
{
	isRecord = !isRecord;
	if (isRecord) {
		rtime.restart();
		ui.recorButton->setStyleSheet("background-color:rgba(255, 255, 255, 0);background-image:url(:/XScreen/Resources/stop_96.ico)");
	}
	else {
		ui.recorButton->setStyleSheet(RECORD_QSS);
	}
}


void XScreen::timerEvent(QTimerEvent *e)
{
	int es = rtime.elapsed() / 1000;  // second
	char buf[1024] = {0};
	sprintf(buf, "%03d:%02d", es / 60, es % 60);
	ui.timelabel->setText(buf);
}