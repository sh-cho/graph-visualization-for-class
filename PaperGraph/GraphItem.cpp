#include "stdafx.h"
#include "GraphItem.h"

GraphItem::GraphItem(const char *filename)
{
	fin.open(filename);
	if (!fin)
		throw std::exception("graph file is not available");

	graph = new Graph();
	read_more();
}

GraphItem::~GraphItem()
{
	fin.close();
}

void GraphItem::read_more()
{
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
	vector<pair<int, int>> edges_indexes;

	//property maps
	auto node_position_map = boost::get(vertex_position, *graph);
	auto node_label_map = boost::get(vertex_name, *graph);
	auto node_type_map = boost::get(vertex_type, *graph);

	int line_cnt = 0;
	qDebug() << "* graph reading start";

	//한 줄씩 읽어서 Parse
	int node_cnt = 0;
	while (std::getline(fin, line) && !line.empty()) {
		//boost::split 이용해 문자열 분리
		//tokens[0]: Paper-key.	ex) conf/iastedCSN/KeimS06
		//tokens[1]: Authors.	ex) Werner Keim&&Arpad L. Scholtz
		//tokens[2]: Published year.
		boost::split(tokens, line, boost::is_any_of("||"), boost::token_compress_on);
		boost::split(authors, tokens[1], boost::is_any_of("&&"), boost::token_compress_on);

		const string& paper_key = tokens[0];
		if (node_ids.left.find(paper_key) == node_ids.left.end()) {
			node_ids.insert(bm_type::value_type(paper_key, node_cnt + whole_node_cnt));
			++node_cnt;
		}

		for (auto author : authors) {
			edges.push_back(pair<string, string>(paper_key, author));
			if (node_ids.left.find(author) == node_ids.left.end()) {
				node_ids.insert(bm_type::value_type(author, node_cnt + whole_node_cnt));
				++node_cnt;
			}
		}

		//debug
		++line_cnt;
		if (line_cnt >= READ_LINE_UNIT) break;
	}
	qDebug() << "* graph reading complete";
	qDebug() << "* # of read line: " << line_cnt;
	qDebug() << "* # of nodes: " << node_cnt;
	//qDebug() << "* # of edges: " << edges.size();

	//edge conversion
	//<string, string> to <int, int>
	//using boost::bimap (bidirectional map)
	for (auto edge : edges) {
		edges_indexes.push_back({
			node_ids.left.find(edge.first)->get_right(),
			node_ids.left.find(edge.second)->get_right()
		});
	}


	// add new vertices
	for (int i = 0; i < node_cnt; ++i) {
		vdes.push_back(add_vertex(static_cast<Graph&>(*graph)));
	}

	// add new edges
	for (auto e_i: edges_indexes) {
		add_edge(vdes[e_i.first], vdes[e_i.second], *graph);
	}
	

	//set index property
	qDebug() << "* set vertex property start";
	vertex_iterator vi, vi_end;
	int i = 0;
	for (boost::tie(vi, vi_end) = vertices(*graph); vi != vi_end; ++vi) {
		//Vertex Property 설정
		//index: n ~ ...
		//name : map의 value(i) 기준으로 찾은 Key
		//		 map --> map<string, int> (boost bidirectional map)
		std::string node_label = node_ids.right.find(i)->get_left();
		boost::put(vertex_index, *graph, *vi, i);
		boost::put(vertex_name, *graph, *vi, node_label);
		boost::put(vertex_record, *graph, *vi, 0);

		//qDebug() << "** index: " << i << ", name: " << node_label.c_str();

		//node type 설정
		if (boost::regex_match(node_label, paper_reg)) {
			//Paper
			boost::put(vertex_type, *graph, *vi, NODE_TYPE::NODE_PAPER);
		} else {
			//Author
			boost::put(vertex_type, *graph, *vi, NODE_TYPE::NODE_AUTHOR);
		}

		++i;
	}
	qDebug() << "* set vertex property end";
	whole_node_cnt += node_cnt;

	qDebug("* set edges weight start");
	//모든 edge의 weight를 1로 설정
	typename graph_traits<Graph>::edge_iterator ei, ei_end;
	for (boost::tie(ei, ei_end)=boost::edges(*graph); ei!=ei_end; ++ei) {
		boost::put(edge_weight, *graph, *ei, 1.0);
	}
	qDebug("* set edges weight end");
	
	//graph layout calculation
	//using boost::random_graph_layout and boost::kamada_kawai_spring_layout
	//vertex마다 계산된 좌표를 property에 적용
	//예제 코드: http://www.boost.org/doc/libs/1_63_0/libs/graph/test/layout_test.cpp
	//(-> 콘솔 기반)
	qDebug() << "* make graph layout start";
	minstd_rand gen;
	Topology topology(gen, (double)SCREEN_SIZE);
	Topology::point_type origin;
	origin[0] = origin[1] = (double)SCREEN_SIZE;
	Topology::point_difference_type extent;
	extent[0] = extent[1] = (double)SCREEN_SIZE;
	rectangle_topology<> rect_top(gen,
		-SCREEN_SIZE / 2, -SCREEN_SIZE / 2,
		SCREEN_SIZE / 2, SCREEN_SIZE / 2);

	switch (LAYOUT_MODE) {
	case GRAPH_LAYOUT::RANDOM_LAYOUT:
		random_graph_layout(*graph, node_position_map, rect_top);
		break;

	case GRAPH_LAYOUT::CIRCLE_LAYOUT:
		circle_graph_layout(*graph, node_position_map, SCREEN_SIZE / 2);
		break;

	case GRAPH_LAYOUT::FRUCHTERMAN_REINGOLD_LAYOUT:
		fruchterman_reingold_force_directed_layout(*graph,
			node_position_map,
			topology,
			attractive_force(square_distance_attractive_force())
			.cooling(linear_cooling<double>(50))
		);
		break;
	}
	qDebug() << "* make graph layout end";


	//clear lists
	nodeList.clear();
	edgeList.clear();
	
	

	//edge_iterator ei, ei_end;
	vertex_descriptor u, v;
	for (boost::tie(ei, ei_end) = boost::edges(*graph); ei != ei_end; ++ei) {
		u = source(*ei, *graph);
		v = target(*ei, *graph);
		Point p1 = node_position_map[u];
		Point p2 = node_position_map[v];

		//make edge item and push it to list
		EdgeItem *edge;

		edge = new EdgeItem(p1[0], p1[1], p2[0], p2[1], QColor(Qt::black), 0,
			QString(node_label_map[u].c_str()), QString(node_label_map[v].c_str()));
		edge->setPos(p1[0], p1[1]);
		edgeList << edge;
	}

	//add nodes
	for (boost::tie(vi, vi_end) = vertices(*graph); vi != vi_end; ++vi) {
		Point p = node_position_map[*vi];
		auto nt = node_type_map[*vi];
		std::string name = node_label_map[*vi];

		//make node item and push it to list
		NodeItem *node;
		if (nt == NODE_TYPE::NODE_PAPER) {
			node = new NodeItem(p[0], p[1], QColor(Qt::darkGreen), QString(name.c_str()), nt);
		}
		else {
			node = new NodeItem(p[0], p[1], QColor(Qt::green), QString(name.c_str()), nt);
		}
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

void GraphItem::might_know()
{
	// 알 수도 있는 연구원 찾기
	vertex_iterator vi, vi_end, vtarget;
	Graph::adjacency_iterator ai, ai_end;
	vector<string> might_know_vec;
	
	auto node_label_map = get(vertex_name, *graph);
	auto node_type_map = get(vertex_type, *graph);

	// 회색 색칠
	for (auto& n : nodeList) {
		if (n->getLabel().toStdString() != TARGET_AUTHOR_NAME) {
			n->setColor(QColor(Qt::lightGray));
		} else {
			n->setColor(QColor(Qt::blue));
		}
	}

	// find target node
	for (boost::tie(vi, vi_end) = boost::vertices(*graph); vi!=vi_end; ++vi) {
		if (node_label_map[*vi] == std::string(TARGET_AUTHOR_NAME)) {
			vtarget = vi;
			break;
		}
	}

	// bfs
	//std::queue<vertex_iterator> q;
	//q.push(vtarget);
	//while (!q.empty()) {
	//	/*auto next_vi = q.front();
	//	q.pop();

	//	if (nodeType[*next_vi] == NODE_TYPE::NODE_PAPER)
	//		continue;

	//	for (boost::tie(ai, ai_end) = boost::adjacent_vertices(*next_vi, *graph);
	//		ai != ai_end; ++ai) {
	//		if (nodeType[*ai] == NODE_TYPE::NODE_PAPER)
	//			continue;
	//		else
	//			q.push(ai);
	//	}*/
	//}

	for (boost::tie(ai, ai_end) = boost::adjacent_vertices(*vtarget, *graph);
		ai != ai_end;
		++ai) {
		might_know_vec.push_back(node_label_map[*ai]);
	}

	// highlight
	for (auto& n: nodeList) {
		if (std::find(might_know_vec.begin(), might_know_vec.end(), n->getLabel().toStdString())
			!= might_know_vec.end()) {
			//found
			n->setColor(Qt::red);
		}
	}
}

void GraphItem::reset_color()
{
	for (auto& n: nodeList) {
		if (n->getType() == NODE_PAPER) {
			n->setColor(QColor(Qt::darkGreen));
		} else {
			n->setColor(QColor(Qt::green));
		}
	}
}

void GraphItem::topK_highlight_with_total()
{
	// 전체 그래프 기준 topK highlight

	// 저자 노드별 실적 계산
	vertex_iterator vi, vi_end;
	Graph::adjacency_iterator ai, ai_end;

	auto node_label_map = boost::get(vertex_name, *graph);
	auto node_type_map = boost::get(vertex_type, *graph);
	auto node_records_map = boost::get(vertex_record, *graph);

	//실적 count 초기화
	//for (boost::tie(vi, vi_end) = boost::vertices(*graph); vi != vi_end; ++vi) {
	//	if (nodeType[*vi] != NODE_TYPE::NODE_AUTHOR) {
	//		continue;
	//	}
	//	boost::put(vertex_record, *graph, *vi, 0);
	//}

	// <record, label>
	TopKHeap<pair<int, string>> heap(TOP_K);
	for (boost::tie(vi, vi_end) = boost::vertices(*graph); vi != vi_end; ++vi) {
		if (node_type_map[*vi] != NODE_TYPE::NODE_AUTHOR) {
			continue;
		}
		
		int record_cnt = 0;
		for (boost::tie(ai, ai_end) = boost::adjacent_vertices(*vi, *graph);
			ai != ai_end; ++ai) {
			if (node_type_map[*ai] == NODE_TYPE::NODE_PAPER) {
				++record_cnt;
			}
		}

		boost::put(vertex_record, *graph, *vi, record_cnt);
		heap.push(make_pair(record_cnt, node_label_map[*vi]));

		//qDebug() << record_cnt;
	}
	
	//get top K records
	pair<int, string> topk_arr[TOP_K];
	for (int i = 0; i < TOP_K; ++i) {
		topk_arr[i] = heap.pop();
		qDebug() << "topk["<<i<<"] = " << topk_arr[i].first << ", " << QString::fromStdString(topk_arr[i].second);
	}
	

	for (auto& n: nodeList) {
		auto label = n->getLabel();
		n->setColor(QColor(Qt::lightGray));
		for (auto& p: topk_arr) {
			if (label.toStdString() == p.second) {
				n->setColor(QColor(Qt::red));
				break;
			}
		}
	}
}

void GraphItem::find_shortest_path()
{
	qDebug("* path highlighting start");
	vertex_iterator vi, vi_end;

	auto node_idx_map = boost::get(vertex_index, *graph);
	auto node_label_map = boost::get(vertex_name, *graph);

	string target_start,
		target_end;

	bool isok = false;

	QInputDialog *inputDialog = new QInputDialog();

	target_start = inputDialog->getText(nullptr, "Enter target's name", "Start node's name:",
		QLineEdit::Normal, "Akira Idoue", &isok).toStdString();
	if (!isok) {
		qDebug("input cancelled");
		return;
	}
	qDebug() << target_start.c_str();

	target_end = inputDialog->getText(nullptr, "Enter target's name", "End node's name:",
		QLineEdit::Normal, "Kayo Kurosaki", &isok).toStdString();
	if (!isok) {
		qDebug("input cancelled");
		return;
	}
	qDebug() << target_end.c_str();




	int start_idx = -1, end_idx = -1;
	for (boost::tie(vi, vi_end) = vertices(*graph); vi != vi_end; ++vi) {
		const string& node_name = node_label_map[*vi];
		if (node_name == target_start) {
			start_idx = node_idx_map[*vi];
		}
		else if (node_name == target_end) {
			end_idx = node_idx_map[*vi];
		}
	}


	if (start_idx == -1 || end_idx == -1) {
		qDebug() << start_idx << " " << end_idx;
		qDebug("no target node");
		return;
	}
	else if (start_idx == end_idx) {
		qDebug("start and end node are same!");
		return;
	}

	/*QMessageBox msgBox;
	msgBox.setText("The document has been modified.");
	msgBox.setInformativeText("Do you want to save your changes?");
	msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
	msgBox.setDefaultButton(QMessageBox::Save);
	int ret = msgBox.exec();
	qDebug() << "ret: " << ret;
	if (ret == QMessageBox::Save) {
	qDebug() << "save";
	}*/

	vector<vertex_descriptor> parents(num_vertices(*graph));
	vector<double> distances(num_vertices(*graph));
	vertex_descriptor start_vertex = boost::vertex(start_idx, *graph);

	//shortest path using dijkstra
	boost::dijkstra_shortest_paths(*graph, start_vertex,
		predecessor_map(boost::make_iterator_property_map(parents.begin(), node_idx_map)).
		distance_map(boost::make_iterator_property_map(distances.begin(), node_idx_map)));

	//check distances
	//qDebug() << "dist: " << distances[end_idx];
	//qDebug();
	//for (int i = 0; i < distances.size(); ++i) {
	//	qDebug() << "dist[" << i << "]: " << distances[i];
	//}
	//qDebug();
	if (distances[end_idx] >= whole_node_cnt) {
		//no path (dist == 0)
		qDebug() << "no path!";
		return;
	}

	//path finding
	qDebug("* path finding start");
	vector<string> paths;
	vertex_descriptor current = boost::vertex(end_idx, *graph);
	paths.push_back(node_label_map[current]);
	qDebug() << "end: " << node_label_map[current].c_str();
	while (1) {
		current = parents[node_idx_map[current]];
		paths.push_back(node_label_map[current]);

		qDebug() << node_label_map[current].c_str();
		if (current == start_vertex) break;
	}
	qDebug("* path finding end");


	qDebug("* path highlighting start");
	for (auto& n : nodeList) {
		if (find(paths.begin(), paths.end(), n->getLabel().toStdString())
			!= paths.end()) {
			n->setColor(QColor(Qt::blue));
		}
	}
	size_t paths_sz = paths.size();
	for (int i = 0; i < paths_sz - 1; ++i) {
		for (auto& e : edgeList) {
			if ((e->getFrom().toStdString() == paths[i] && e->getTo().toStdString() == paths[i + 1])
				|| (e->getFrom().toStdString() == paths[i + 1] && e->getTo().toStdString() == paths[i])) {
				e->setColor(QColor(Qt::blue));
				e->setWidth(3);
			}
		}
	}
	qDebug("* path highlighting end");
}

void GraphItem::test()
{
	qDebug("* test action start");



	qDebug("* test action end");
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
