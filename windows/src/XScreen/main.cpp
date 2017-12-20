#include "xscreen.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	XScreen w;
	w.show();
	return a.exec();
}
