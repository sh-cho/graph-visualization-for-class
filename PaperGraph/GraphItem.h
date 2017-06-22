#ifndef GRAPHITEM_H
#define GRAPHITEM_H

#include "stdafx.h"
#include "NodeItem.h"
#include "EdgeItem.h"

using namespace std;
using namespace boost;

#define	CITATION_COUNT

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
	void might_know();
	void topK_highlight_with_total();
	void topK_highlight_with_target();
	//void topK_using_custom_score();
	void find_shortest_path();
	void topk_with_pagerank();
	void category_visualize();
	void reset_color();

	//test
	void test();

protected:
	void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:
	ifstream fin;
	bm_type node_ids;
	vector<vertex_descriptor> vdes;
	int whole_node_cnt = 0;

	Graph *graph = nullptr;
	QList<NodeItem *> nodeList;
	QList<EdgeItem *> edgeList;
};

#endif // GRAPHITEM_H
