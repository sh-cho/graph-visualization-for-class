#include "stdafx.h"
#include "PaperGraphWidget.h"
#include "MainWindow.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	
	MainWindow m;

	try {
		m.make_graph(PAPER_FILENAME);
	} catch (const std::exception& e) {
		qDebug() << "Error: " << e.what();
		return EXIT_FAILURE;
	}

	m.show();

	return app.exec();
}
