#include "PaperGraphWidget.h"
#include <QtWidgets/QApplication>
#include <QDebug>

#include <algorithm>
#include <exception>
#include <fstream>
#include <iterator>
#include <string>
#include <map>
#include <vector>

#include <boost/graph/fruchterman_reingold.hpp>
//#include <boost/graph/kamada_kawai_spring_layout.hpp>
#include <boost/graph/random_layout.hpp>
#include <boost/graph/circle_layout.hpp>
#include <boost/algorithm/string.hpp>	//boost::split
#include <boost/bimap.hpp>

using namespace std;

/**
 *	Constants
 */
const char* PAPER_FILENAME = "dblp-paper.txt";

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	
	PaperGraphWidget w;

	try {
		ifstream fin(PAPER_FILENAME);
		w.print_graph(fin);
		fin.close();
	} catch (const std::exception& e) {
		qDebug() << "Error: " << e.what();
		return EXIT_FAILURE;
	}

	w.show();

	return app.exec();
}
