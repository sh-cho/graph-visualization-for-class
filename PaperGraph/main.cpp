#include "stdafx.h"
#include "PaperGraphWidget.h"
#include "MainWindow.h"

/**
 *	Constants
 */
const char* PAPER_FILENAME = "dblp-paper.txt";

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	
	//PaperGraphWidget w;
	MainWindow m;

	try {
		ifstream fin(PAPER_FILENAME);
		//w.print_graph(fin);
		m.print_graph(fin);
		fin.close();
	} catch (const std::exception& e) {
		qDebug() << "Error: " << e.what();
		return EXIT_FAILURE;
	}

	//w.show();
	m.show();

	return app.exec();
}
