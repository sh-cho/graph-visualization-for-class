#include "stdafx.h"
#include "PaperGraphWidget.h"
#include "MainWindow.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	
	MainWindow m;

	try {
		ifstream fin(PAPER_FILENAME);
		m.print_graph(fin);
		fin.close();
	} catch (const std::exception& e) {
		qDebug() << "Error: " << e.what();
		return EXIT_FAILURE;
	}

	m.show();

	return app.exec();
}
