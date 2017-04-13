#include "GraphItem.h"

#include <boost/graph/fruchterman_reingold.hpp>
#include <boost/graph/random_layout.hpp>
#include <boost/graph/circle_layout.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/algorithm/string.hpp>	//boost::split
#include <boost/bimap.hpp>
#include <boost/regex.hpp>

#include <exception>
#include <iterator>
#include <string>
#include <map>
#include <vector>

#include <QDebug>
#include <QtWidgets>


GraphItem::GraphItem(ifstream& fin)
{
	if (!fin)
		throw std::exception("graph file input is invalid");

	/**
	 *	Parse Paper dataset
	 *	- paper_key, [author_list], publish_year
	 *	Column Delimiter:		||
	 *	Author list Delimiter:	&&
	 */
	std::string line;
	vector<std::string> tokens;
	vector<std::string> authors;
	vector<pair<string, string>> edges;
	
	//String <--> int 양방향 변환을 위해 bidirectional map 상숑
	//map<string, int> -> <vertex label, vertex index>
	typedef boost::bimap<string, int> bm_type;
	bm_type node_ids;
	vector<simple_edge> edges_indexes;	//int로 변환된 edge
	
	int node_cnt = 0;
	qDebug() << "* graph reading start";
	
	//한 줄씩 읽어서 Parse
	while (std::getline(fin, line) && !line.empty()) {
		//boost::split 이용해 문자열 분리
		//tokens[0]: Paper-key.	ex) conf/iastedCSN/KeimS06
		//tokens[1]: Authors.	ex) Werner Keim&&Arpad L. Scholtz
		//tokens[2]: Published year.
		boost::split(tokens, line, boost::is_any_of("||"), boost::token_compress_on);
		boost::split(authors, tokens[1], boost::is_any_of("&&"), boost::token_compress_on);
			
		const string& paper_key = tokens[0];
		if (node_ids.left.find(paper_key) == node_ids.left.end()) {
			node_ids.insert(bm_type::value_type(paper_key, node_cnt++));
		}
	
		for (auto author : authors) {
			edges.push_back(pair<string, string>(paper_key, author));
			if (node_ids.left.find(author) == node_ids.left.end()) {
				node_ids.insert(bm_type::value_type(author, node_cnt++));
			}
		}
	
		//debug
		if (node_cnt > NODE_LIMIT) break;
	}
	qDebug() << "* graph reading complete";
	qDebug() << "* # of nodes: " << node_cnt;
	qDebug() << "* # of edges: " << edges.size();
	
	//edge conversion
	//<string, string> to <int, int>
	//using boost::bimap (bidirectional map)
	for (auto edge : edges) {
		edges_indexes.push_back({
			node_ids.left.find(edge.first)->get_right(),
			node_ids.left.find(edge.second)->get_right()
		});
	}
	//Graph --> defined in "PaperGraphWidget.h"
	//Graph graph(edges_indexes.begin(), edges_indexes.end(), node_ids.size());
	graph = new Graph(edges_indexes.begin(), edges_indexes.end(), node_ids.size());
	
	//set index property
	qDebug() << "* set vertex property start";
	typedef typename graph_traits<Graph>::edge_iterator edge_iterator;
	typedef typename graph_traits<Graph>::vertex_iterator vertex_iterator;
	vertex_iterator vi, vi_end;
	int i = 0;
	for (boost::tie(vi, vi_end)=vertices(*graph); vi!=vi_end; ++vi) {
		//Vertex Property 설정
		//index: 0 ~ ...
		//name : map의 value(i) 기준으로 찾은 Key
		//		map --> map<string, int> (boost bidirectional map)
		boost::put(vertex_index, *graph, *vi, i);
		boost::put(vertex_name, *graph, *vi,
			node_ids.right.find(i)->get_left());
	
		++i;
	}
	qDebug() << "* set vertex property end";


	//qDebug("* set edges weight start");
	////모든 edge의 weight를 1로 설정
	//typename graph_traits<Graph>::edge_iterator ei, ei_end;
	//for (boost::tie(ei, ei_end)=boost::edges(*graph); ei!=ei_end; ++ei) {
	//	boost::put(edge_weight, *graph, *ei, 1);
	//}
	//qDebug("* set edges weight end");
	//
	//
	//qDebug("* path highlighting start");
	////find start, end node's id
	//int start_idx, end_idx;
	//for (boost::tie(vi, vi_end)=vertices(*graph); vi!=vi_end; ++vi) {
	//	string node_name = boost::get(vertex_name, *graph, *vi);
	//	if (node_name == "Seong Chul Cho") {
	//		start_idx = boost::get(vertex_index, *graph, *vi);
	//	} else if (node_name == "Hyung Jin Kim") {
	//		end_idx = boost::get(vertex_index, *graph, *vi);
	//	}
	//}

	//typedef boost::graph_traits<Graph>::vertex_descriptor vertex_descriptor;
	//vector<vertex_descriptor> parents(num_vertices(*graph));
	//vector<int> distances(num_vertices(*graph));
	//vertex_descriptor start_vertex = vertex(start_idx, *graph);
	//dijkstra_shortest_paths(*graph, start_vertex,
	//	predecessor_map(&parents[0]).distance_map(&distances[0]));
	////path finding
	//vertex_descriptor current = boost::vertex(end_idx, *graph);
	//while (current != boost::vertex(start_idx, *graph)) {
	//}
	//qDebug("* path highlighting end");
	

	//graph layout calculation
	//using boost::random_graph_layout and boost::kamada_kawai_spring_layout
	//vertex마다 계산된 좌표를 property에 적용
	//예제 코드: http://www.boost.org/doc/libs/1_63_0/libs/graph/test/layout_test.cpp
	//(-> 콘솔 기반)
	qDebug() << "* make graph layout start";
	typedef square_topology<> Topology;
	minstd_rand gen;
	Topology topology(gen, (double)SCREEN_SIZE);
	Topology::point_type origin;
	origin[0] = origin[1] = (double)SCREEN_SIZE;
	Topology::point_difference_type extent;
	extent[0] = extent[1] = (double)SCREEN_SIZE;
	rectangle_topology<> rect_top(gen,
		-SCREEN_SIZE/2, -SCREEN_SIZE/2,
		SCREEN_SIZE/2, SCREEN_SIZE/2);
		
	switch (LAYOUT_MODE) {
	case GRAPH_LAYOUT::RANDOM_LAYOUT:
		random_graph_layout(*graph, get(vertex_position, *graph), rect_top);
		break;
	
	case GRAPH_LAYOUT::CIRCLE_LAYOUT:
		circle_graph_layout(*graph, get(vertex_position, *graph), SCREEN_SIZE/2);
		break;
	
	case GRAPH_LAYOUT::FRUCHTERMAN_REINGOLD_LAYOUT:
		fruchterman_reingold_force_directed_layout(*graph,
			get(vertex_position, *graph),
			topology,
			attractive_force(square_distance_attractive_force())
			.cooling(linear_cooling<double>(50))
		);
		break;
	}
	qDebug() << "* make graph layout end";


	//add edges
	typedef square_topology<> Topology;
	typedef typename Topology::point_type Point;
	auto position = get(vertex_position, *graph);
	auto label = get(vertex_name, *graph);

	typedef boost::graph_traits<Graph>::vertex_descriptor vertex_descriptor;
	typename graph_traits<Graph>::edge_iterator ei, ei_end;
	vertex_descriptor u, v;
	for (boost::tie(ei, ei_end)=boost::edges(*graph); ei!=ei_end; ++ei) {
		u = source(*ei, *graph);
		v = target(*ei, *graph);
		Point p1 = position[u];
		Point p2 = position[v];

		//make edge item and push it to list
		EdgeItem *edge;
		
		//if (label[u] == "conf/sbrn/GomesPSRC10" ||
		//	label[u] == "conf/iastedCSN/KeimS06" ||
		//	label[v] == "conf/sbrn/GomesPSRC10" ||
		//	label[v] == "conf/iastedCSN/KeimS06") {
		//	//highlight
		//	edge = new EdgeItem(p1[0], p1[1], p2[0], p2[1], QColor(Qt::blue), 3);
		//} else {
		//	edge = new EdgeItem(p1[0], p1[1], p2[0], p2[1], QColor(Qt::black), 0);
		//}
		edge = new EdgeItem(p1[0], p1[1], p2[0], p2[1], QColor(Qt::black), 0);
		edge->setPos(p1[0], p1[1]);
		edgeList << edge;
	}

	//add nodes
	for (boost::tie(vi, vi_end)=vertices(*graph); vi!=vi_end; ++vi) {
		Point p = position[*vi];
		std::string name = label[*vi];

		//make node item and push it to list
		NodeItem *node;
		//if (name == "conf/sbrn/GomesPSRC10" ||
		//	name == "conf/iastedCSN/KeimS06") {
		//	//highlight
		//	node = new NodeItem(p[0], p[1], QColor(Qt::blue), QString(name.c_str()));
		//} else {
		//	node = new NodeItem(p[0], p[1], QColor(Qt::green), QString(name.c_str()));
		//}
		node = new NodeItem(p[0], p[1], QColor(Qt::green), QString(name.c_str()));
		node->setPos(QPointF(p[0], p[1]));
		nodeList << node;
	}
}

//override
QRectF GraphItem::boundingRect() const
{
	//TODO
	return QRectF(-SCREEN_SIZE/2, -SCREEN_SIZE/2, SCREEN_SIZE, SCREEN_SIZE);
}

QPainterPath GraphItem::shape() const
{
	QPainterPath path;
	return path;
}

void GraphItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	if (!graph)
		return;

	//print edges
	for (auto edge: edgeList) {
		edge->paint(painter, option, widget);
	}
	
	//print nodes
	for (auto node: nodeList) {
		node->paint(painter, option, widget);
	}
}

void GraphItem::path_highlighting(std::string start, std::string end)
{
	//path highlight
	//nodeList, edgeList 속성을 수정
	for (auto& n: nodeList) {
		if (n->getLabel() == QString("Seongsoo Park")) {
			n->setColor(QColor(255, 0, 0));
		}
	}

	
}

//event handler
void GraphItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
}

void GraphItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
}

void GraphItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
}
