#ifndef COAUTHORSHIPGRAPHWIDGET_H
#define COAUTHORSHIPGRAPHWIDGET_H

#include <QtWidgets/QWidget>
#include <QGraphicsScene>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/topology.hpp>
#include <boost/graph/graph_traits.hpp>

#include "ui_CoauthorshipGraphWidget.h"


using namespace boost;


enum vertex_position_t { vertex_position };
namespace boost {
	BOOST_INSTALL_PROPERTY(vertex, position);
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


class CoauthorshipGraphWidget : public QWidget
{
	Q_OBJECT

public:
	CoauthorshipGraphWidget(QWidget *parent = 0);
	void print_graph(const Graph& graph);

private:
	void initscene();

	Ui::CoauthorshipGraphWidgetClass ui;
	QGraphicsScene *scene;
};

#endif // COAUTHORSHIPGRAPHWIDGET_H
