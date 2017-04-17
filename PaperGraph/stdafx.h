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

#define	NODE_PAPER	1
#define	NODE_AUTHOR	2

namespace {
	enum GRAPH_LAYOUT {
		RANDOM_LAYOUT,
		CIRCLE_LAYOUT,
		//KAMADA_KAWAI_LAYOUT,
		FRUCHTERMAN_REINGOLD_LAYOUT	//slow
	};

	const int LAYOUT_MODE = GRAPH_LAYOUT::RANDOM_LAYOUT;
	const int SCREEN_SIZE = 300;
	const int NODE_LIMIT = 100;
	
	const char* PAPER_FILENAME = "dblp-paper.txt";
}

namespace boost {
	const regex paper_reg("(conf|journals).*");
}