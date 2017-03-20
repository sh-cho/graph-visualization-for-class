#include "PaperGraphWidget.h"
#include "NodeItem.h"
#include "EdgeItem.h"
#include "GraphicsView.h"

#include <string>

PaperGraphWidget::PaperGraphWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	initscene();

	View *view = new View("temp view");
	view->view()->setScene(scene);
	QHBoxLayout *layout = new QHBoxLayout;
	layout->addWidget(view);
	setLayout(layout);

	setWindowTitle(tr("dblp paper graph visualization"));
}

void PaperGraphWidget::print_graph(const Graph & graph)
{
	//print graph
	typedef square_topology<> Topology;
	typedef typename Topology::point_type Point;
	const int rect_sz = 4;
	auto position = get(vertex_position, graph);
	auto label = get(vertex_name, graph);
	


	//print edges
	typename graph_traits<Graph>::edge_iterator ei, ei_end;
	typedef boost::graph_traits<Graph>::vertex_descriptor VertexDescriptor;
	VertexDescriptor u, v;
	for (boost::tie(ei, ei_end)=edges(graph); ei!=ei_end; ++ei) {
		u = source(*ei, graph);
		v = target(*ei, graph);

		Point p1 = position[u];
		Point p2 = position[v];

		/*line = scene->addLine(
			p1[0], p1[1],
			p2[0], p2[1],
			QPen(Qt::black)
		);*/
		//QGraphicsItem *edge =
		//	new EdgeItem(p1[0], p1[1], p2[0], p2[1]);
		//edge->setPos(QPointF(p1[0], p1[1]));
		//scene->addItem(edge);

		scene->addLine(p1[0], p1[1], p2[0], p2[1], QPen(Qt::black, 0));
	}

	//print nodes
	typename graph_traits<Graph>::vertex_iterator vi, vi_end;
	for (boost::tie(vi, vi_end)=vertices(graph); vi!=vi_end; ++vi) {
		//Point p = position[*vi];
		//rectangle = scene->addRect(
		//	p[0], p[1], rect_sz, rect_sz,
		//	QPen(Qt::black),
		//	QBrush(Qt::green));
		Point p = position[*vi];
		std::string name = label[*vi];
		QGraphicsItem *node = new NodeItem(p[0], p[1], QString(name.c_str()));
		node->setPos(QPointF(p[0], p[1]));
		scene->addItem(node);
	}
}

void PaperGraphWidget::initscene()
{
	scene = new QGraphicsScene(this);

	//int x = 0, y;
	//for (int i=-11000; i<11000; i+=110) {
	//	++x;
	//	y = 0;
	//	for (int j=-7000; j<7000; j+=70) {
	//		++y;
	//		QGraphicsItem *item = new NodeItem(x, y);
	//		item->setPos(QPointF(i, j));
	//		scene->addItem(item);
	//	}
	//}
}
