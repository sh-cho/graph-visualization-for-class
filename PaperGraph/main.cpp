#include "PaperGraphWidget.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	PaperGraphWidget w;
	w.show();
	return a.exec();
}
