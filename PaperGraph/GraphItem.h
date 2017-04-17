#ifndef GRAPHITEM_H
#define GRAPHITEM_H

#include "stdafx.h"
#include "NodeItem.h"
#include "EdgeItem.h"

using namespace std;
using namespace boost;


class GraphItem
	: public QGraphicsItem
{
public:
	//GraphItem(ifstream* pFin);
	GraphItem(const char* filename);
	virtual ~GraphItem();

	//io
	void read_more();

	//overrides
	QRectF boundingRect() const override;
	QPainterPath shape() const override;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

	//methods
	void path_highlighting(std::string start, std::string end);
	void reset_color();
	void topK_highlight();

protected:
	void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:
	ifstream fin;
	bm_type node_ids;
	vector<pair<string, string>> edges;
	vector<simple_edge> edges_indexes;
	int whole_node_cnt = 0;

	Graph *graph = nullptr;
	QList<NodeItem *> nodeList;
	QList<EdgeItem *> edgeList;
};

#endif // GRAPHITEM_H
