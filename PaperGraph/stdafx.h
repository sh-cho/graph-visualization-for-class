#pragma once
#include <QColor>
#include <QComboBox>
#include <QDebug>
#include <QFrame>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGridLayout>
#include <QKeyEvent>
#include <QList>
#include <QMainWindow>
#include <qmath.h>
#include <QMessageBox>
#include <QtGui>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtWidgets>

#include <boost/algorithm/string.hpp>	//boost::split
#include <boost/bimap.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/circle_layout.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/fruchterman_reingold.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/random_layout.hpp>
#include <boost/graph/topology.hpp>
#include <boost/regex.hpp>

#include <algorithm>
#include <exception>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <string>
#include <vector>

using namespace boost;
using namespace std;

/* constants */
namespace {
	/* enums */
	enum NODE_TYPE {
		NODE_PAPER,
		NODE_AUTHOR
	};

	enum GRAPH_LAYOUT {
		RANDOM_LAYOUT,
		CIRCLE_LAYOUT,
		//KAMADA_KAWAI_LAYOUT,
		FRUCHTERMAN_REINGOLD_LAYOUT	//slow
	};

	/* file io */
	const char* PAPER_FILENAME = "dblp-paper.txt";

	/* visualization */
	const int NODE_SIZE = 4;
	const int LAYOUT_MODE = GRAPH_LAYOUT::RANDOM_LAYOUT;
	const int SCREEN_SIZE = 1000;
	const int READ_LINE_UNIT = 100;	//한 번에 몇 라인을 읽을지

	/* topK */
	const int topK = 10;	//상위 몇 개 아이템에 대해

}

namespace boost {
	const boost::regex paper_reg("(conf|journals).*");
}