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

/* enums */
enum vertex_position_t { vertex_position };
enum vertex_type_t { vertex_type };
namespace boost {
	BOOST_INSTALL_PROPERTY(vertex, position);
	BOOST_INSTALL_PROPERTY(vertex, type);
}
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

/* typedef */
typedef boost::bimap<string, int> bm_type;
typedef square_topology<>::point_type point;
typedef boost::property<vertex_index_t, int,
	boost::property<vertex_name_t, std::string,
	boost::property<vertex_position_t, point,
	boost::property<vertex_type_t, int>>>
	> VertexProperties;
typedef boost::adjacency_list<
	listS,	//outEdgeList
	listS,	//VertexList
	undirectedS,
	//vertex properties
	VertexProperties,
	//edge properties
	boost::property<edge_weight_t, double>
> Graph;
typedef typename graph_traits<Graph>::edge_iterator edge_iterator;
typedef typename graph_traits<Graph>::vertex_iterator vertex_iterator;
typedef boost::graph_traits<Graph>::vertex_descriptor vertex_descriptor;
typedef square_topology<> Topology;
typedef typename Topology::point_type Point;

/* structs */
struct simple_edge {
	int first, second;
};

/* constants */
namespace {
	/* file io */
	const char* PAPER_FILENAME = "dblp-paper.txt";

	/* visualization */
	const int NODE_SIZE = 4;
	const int LAYOUT_MODE = GRAPH_LAYOUT::RANDOM_LAYOUT;
	const int SCREEN_SIZE = 1000;
	const int READ_LINE_UNIT = 100;	//한 번에 몇 라인을 읽을지

	/* topK */
	const int TOP_K = 10;	//상위 몇 개 아이템에 대해 할 지

}


namespace boost {
	const boost::regex paper_reg("(conf|journals).*");
}