#ifndef GRAPHITEM_H
#define GRAPHITEM_H

#include "stdafx.h"
#include "NodeItem.h"
#include "EdgeItem.h"

using namespace std;
using namespace boost;


enum GRAPH_LAYOUT {
	RANDOM_LAYOUT,
	CIRCLE_LAYOUT,
	//KAMADA_KAWAI_LAYOUT,
	FRUCHTERMAN_REINGOLD_LAYOUT	//slow
};
/**
 *	Constants
 */
const int LAYOUT_MODE = GRAPH_LAYOUT::RANDOM_LAYOUT;
const int SCREEN_SIZE = 300;
const int NODE_LIMIT = 100;


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


class GraphItem
	: public QGraphicsItem
{
public:
	GraphItem(ifstream& fin);

	//overrides
	QRectF boundingRect() const override;
	QPainterPath shape() const override;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

	//methods
	void path_highlighting(std::string start, std::string end);
	void reset_color();

protected:
	void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:
	Graph *graph = nullptr;
	QList<NodeItem *> nodeList;
	QList<EdgeItem *> edgeList;
};

#endif // GRAPHITEM_H
