#ifndef PAPERGRAPHWIDGET_H
#define PAPERGRAPHWIDGET_H

#include <QtWidgets/QWidget>
#include <QGraphicsScene>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/topology.hpp>
#include <boost/graph/graph_traits.hpp>

#include "ui_PaperGraphWidget.h"


using namespace boost;


enum vertex_position_t { vertex_position };
//enum vertex_type_t { vertex_type };
namespace boost {
	BOOST_INSTALL_PROPERTY(vertex, position);
	//BOOST_INSTALL_PROPERTY(vertex, type);
}
typedef square_topology<>::point_type point;
struct simple_edge {
	int first, second;
};
typedef boost::property<vertex_index_t, int,
	boost::property<vertex_name_t, std::string,
	boost::property<vertex_position_t, point>>
> VertexProperties;
typedef adjacency_list<
	listS,	//outEdgeList
	listS,	//VertexList
	undirectedS,
	//vertex properties
	VertexProperties,
	//edge properties
	boost::property<edge_weight_t, double>
> Graph;


class PaperGraphWidget : public QWidget
{
	Q_OBJECT

public:
	PaperGraphWidget(QWidget *parent = 0);
	void print_graph(const Graph& graph);

private:
	void initscene();

	Ui::PaperGraphWidgetClass ui;
	QGraphicsScene *scene;
};

#endif // PAPERGRAPHWIDGET_H
