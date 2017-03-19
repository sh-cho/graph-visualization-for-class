#include "PaperGraphWidget.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	app.setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);

	PaperGraphWidget w;
	w.show();

	return app.exec();
}
