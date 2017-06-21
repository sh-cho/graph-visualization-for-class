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
	auto node_citation_map = boost::get(vertex_citation, *graph);

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

		//node type에 따라
		if (boost::regex_match(node_label, paper_reg)) {
			//Paper일 경우
			//vertex type 설정
			boost::put(vertex_type, *graph, *vi, NODE_TYPE::NODE_PAPER);

			//citation counting
			//bibtex 다운로드
			std::string dblp_url = std::string("http://dblp.uni-trier.de/rec/bib1/") + node_label;
			_curl_processor.set_url(dblp_url.c_str());
			_curl_processor.perform();

			//bibtex 파싱
			_bibtex_processor.read(_curl_processor.get_buffer());
			
			std::string doi;
			if (!_bibtex_processor.get_value("doi", doi)) {
				//doi가 없는 경우
				node_citation_map[*vi] = 0;	//citation count 0으로 설정
			}
			else {
				//doi가 존재하는 경우
				std::string json_address = std::string("http://api.crossref.org/works/") + doi;
				_curl_processor.set_url(json_address.c_str());
				_curl_processor.perform();

				_json_processor.read_json(_curl_processor.get_buffer());
				if (!_json_processor.is_ok()) {
					node_citation_map[*vi] = 0;
				}
				else {
					node_citation_map[*vi] = _json_processor.get_citation_count();
				}
			}
		} else {
			//Author
			boost::put(vertex_type, *graph, *vi, NODE_TYPE::NODE_AUTHOR);
		}

		

		//counter
		//printf("%d end: %s\n", i, node_label.c_str());
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
	vertex_iterator vi, vi_end/*, vtarget*/;
	vertex_descriptor vtarget;
	/*Graph::adjacency_iterator ai, ai_end,
		ai2, ai2_end;*/
	adjacencyIterator ai, ai_end,
		ai2, ai2_end;
	vector<string> coauthor_name_vec, might_know_name_vec,
		visited;
	vector<string> paper_name_vec, coauthor_paper_name_vec;
	vector<vertex_descriptor> paper_vec, coauthor_paper_vec,
		might_know_paper_vec;
	
	auto node_idx_map = boost::get(vertex_index, *graph);
	auto node_label_map = get(vertex_name, *graph);
	auto node_type_map = get(vertex_type, *graph);
	
	//vertex_descriptor vvv = boost::vertex(node_idx_map[*vi], *graph);
	//vertex_descriptor vvv = boost::vertex(node_idx_map[*ai], *graph);

	//사람 지정
	bool isok = false;
	string target_name;
	QInputDialog *inputDialog = new QInputDialog();
	target_name = inputDialog->getText(nullptr, "Enter target's name", "Target node's name:",
		QLineEdit::Normal, "Akira Idoue", &isok).toStdString();
	if (!isok) {
		qDebug("input cancelled");
		return;
	}
	qDebug() << target_name.c_str();


	// 회색 색칠
	for (auto& n : nodeList) {
		if (n->getLabel().toStdString() != target_name) {
			n->setColor(QColor(Qt::lightGray));
		} else {
			n->setColor(QColor(Qt::blue));
		}
	}

	// find target node
	for (boost::tie(vi, vi_end) = boost::vertices(*graph); vi!=vi_end; ++vi) {
		if (node_label_map[*vi] == target_name) {
			vtarget = vertex(node_idx_map[*vi], *graph);
			break;
		}
	}

	visited.push_back(target_name);
	
	//push target's papers
	for (boost::tie(ai, ai_end) = boost::adjacent_vertices(vtarget, *graph);
		ai != ai_end;
		++ai) {
		//ai: 타겟 노드의 이웃 노드(paper)
		const string& node_label = node_label_map[*ai];
		//qDebug() << "ai: " << node_label.c_str();
		
		paper_vec.push_back(vertex(node_idx_map[*ai], *graph));
		paper_name_vec.push_back(node_label);
		
		visited.push_back(node_label);
	}

	// find coauthor
	for (auto paper: paper_vec) {
		//paper: 타겟 저자의 논문
		for (boost::tie(ai, ai_end)=boost::adjacent_vertices(paper, *graph);
			ai!=ai_end;
			++ai) {
			//ai: paper의 이웃. = 저자들
			//                  == coauthors
			const string& node_label = node_label_map[*ai];
			//if (node_label == target_name) {
			//	// 중복체크
			//	continue;
			//}
			if (find(visited.begin(), visited.end(), node_label) != visited.end()) {
				continue;
			}
			//qDebug() << "_ai: "<< node_label.c_str();

			coauthor_name_vec.push_back(node_label_map[*ai]);
			coauthor_paper_vec.push_back(vertex(node_idx_map[*ai], *graph));
			visited.push_back(node_label);
		}
	}

	//push coauthors' papers
	for (auto coauthor_paper: coauthor_paper_vec) {
		//coauthor_paper: coauthor의 논문
		for (boost::tie(ai, ai_end) = boost::adjacent_vertices(coauthor_paper, *graph);
			ai != ai_end;
			++ai) {
			//ai: coauthor_paper의 저자들 = might know
			//if () {
			//	continue;
			//}

			const string& node_label = node_label_map[*ai];
			if (find(visited.begin(), visited.end(), node_label) != visited.end()) {
				continue;
			}
			//qDebug() << "_ai2: " << node_label.c_str();

			might_know_paper_vec.push_back(vertex(node_idx_map[*ai], *graph));
			coauthor_paper_name_vec.push_back(node_label);
			visited.push_back(node_label);
		}
	}

	// find "might know" author
	for (auto paper: might_know_paper_vec) {
		for (boost::tie(ai, ai_end)=boost::adjacent_vertices(paper, *graph);
			ai!=ai_end;
			++ai) {
			const string& node_label = node_label_map[*ai];
			if (find(visited.begin(), visited.end(), node_label) != visited.end()) {
				continue;
			}
			//qDebug() << "_ai3: " << node_label.c_str();

			might_know_name_vec.push_back(node_label);
			visited.push_back(node_label);
		}
	}


	// highlight
	//1. might know
	//2. coauthor
	for (auto& n: nodeList) {
		if (n->getLabel().toStdString() == target_name) {
			n->setColor(Qt::red);
		}
		else if (std::find(paper_name_vec.begin(), paper_name_vec.end(), n->getLabel().toStdString())
			!= paper_name_vec.end()) {
			n->setColor("#ff8c00");
		}
		else if (std::find(coauthor_name_vec.begin(), coauthor_name_vec.end(), n->getLabel().toStdString())
			!= coauthor_name_vec.end()) {
			//coauthor
			n->setColor(Qt::yellow);
		}
		else if (std::find(coauthor_paper_name_vec.begin(), coauthor_paper_name_vec.end(), n->getLabel().toStdString())
			!= coauthor_paper_name_vec.end()) {
			n->setColor(Qt::green);
		}
		else if (std::find(might_know_name_vec.begin(), might_know_name_vec.end(), n->getLabel().toStdString())
			!= might_know_name_vec.end()) {
			//might know 
			n->setColor(Qt::blue);
		}
	}
	//highlight nodes
	//1. target - paper
	//2. paper - coauthor
	//3. coauthor - coauthor's paper
	//4. coauthor's paper - might know
	for (auto& to: paper_name_vec) {
		//1.
		for (auto& e: edgeList) {
			if (e->match(to, target_name)) {
				e->setColor(Qt::red);
				e->setWidth(3);
			}
		}
	}
	for (auto& from: paper_name_vec) {
		for (auto& to: coauthor_name_vec) {
			for (auto& e : edgeList) {
				if (e->match(from, to)) {
					e->setColor("#ff8c00");
					e->setWidth(3);
				}
			}
		}
	}
	for (auto& from : coauthor_name_vec) {
		for (auto& to : coauthor_paper_name_vec) {
			for (auto& e : edgeList) {
				if (e->match(from, to)) {
					e->setColor(Qt::yellow);
					e->setWidth(3);
				}
			}
		}
	}
	for (auto& from : coauthor_paper_name_vec) {
		for (auto& to : might_know_name_vec) {
			for (auto& e : edgeList) {
				if (e->match(from, to)) {
					e->setColor(Qt::green);
					e->setWidth(3);
				}
			}
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

	//k 입력
	bool isok = false;

	QInputDialog *inputDialog = new QInputDialog();
	//int inputK = inputDialog->getText(nullptr, "Enter target's name", "Start node's name:",
	//	QLineEdit::Normal, "Akira Idoue", &isok).toStdString();
	int inputK = inputDialog->getInt(nullptr, "Enter K", "K:", 3,
		1, whole_node_cnt, 1, &isok);
	if (!isok) {
		qDebug("input cancelled");
		return;
	}

	//저자별 논문 수 계산 + TopK Heap 사용
	//pair -> <num_of_record, label>
	TopKHeap<pair<int, string>> heap(inputK);
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
	//pair<int, string> topk_arr[inputK];
	pair<int, string> *topk_arr = new pair<int, string>[inputK];
	for (int i = 0; i < inputK; ++i) {
		topk_arr[i] = heap.pop();
		qDebug() << "topk["<<i<<"] = " << topk_arr[i].first << ", " << QString::fromStdString(topk_arr[i].second);
	}
	

	for (auto& n: nodeList) {
		auto label = n->getLabel();
		n->setColor(QColor(Qt::lightGray));
		for (int i = 0; i < inputK; ++i) {
			auto& p = topk_arr[i];
			if (label.toStdString() == p.second) {
				n->setColor(QColor(Qt::red));
				break;
			}
		}
	}
	delete[] topk_arr;
}

void GraphItem::topK_highlight_with_target()
{
	// 선택한 사람 주변 topK highlight

	// 저자 노드별 실적 계산
	vertex_iterator vi, vi_end;
	vertex_descriptor vtarget;
	Graph::adjacency_iterator ai, ai_end,
		ai2, ai2_end;

	auto node_idx_map = boost::get(vertex_index, *graph);
	auto node_label_map = boost::get(vertex_name, *graph);
	auto node_type_map = boost::get(vertex_type, *graph);
	auto node_records_map = boost::get(vertex_record, *graph);

	vector<vertex_descriptor> paper_vec;
	vector<string> visited;

	vector<vertex_descriptor> target_desc_range_vec;

	//k 입력
	bool isok = false;

	QInputDialog *inputDialog = new QInputDialog();
	//int inputK = inputDialog->getText(nullptr, "Enter target's name", "Start node's name:",
	//	QLineEdit::Normal, "Akira Idoue", &isok).toStdString();
	int inputK = inputDialog->getInt(nullptr, "Enter K", "K:", 3,
		1, whole_node_cnt, 1, &isok);
	if (!isok) {
		qDebug("input cancelled");
		return;
	}

	//std::string target_name = inputDialog->getText
	std::string target_name = inputDialog->getText(nullptr, "Enter target's name",
		"target node's name:", QLineEdit::Normal, "Tobias Scholand", &isok).toStdString();
	if (!isok) {
		qDebug("input cancelled");
		return;
	}
	qDebug() << target_name.c_str();


	// find target node
	for (boost::tie(vi, vi_end) = boost::vertices(*graph); vi != vi_end; ++vi) {
		if (node_label_map[*vi] == target_name) {
			target_desc_range_vec.push_back(*vi);
			vtarget = vertex(node_idx_map[*vi], *graph);
			break;
		}
	}
	visited.push_back(target_name);
	
	//push target's papers
	for (boost::tie(ai, ai_end) = boost::adjacent_vertices(vtarget, *graph);
		ai != ai_end;
		++ai) {
		//ai: 타겟 노드의 이웃 노드(paper)
		const string& node_label = node_label_map[*ai];
		paper_vec.push_back(vertex(node_idx_map[*ai], *graph));
		visited.push_back(node_label);
	}

	// find coauthor
	for (auto paper : paper_vec) {
		//paper: 타겟 저자의 논문
		for (boost::tie(ai, ai_end) = boost::adjacent_vertices(paper, *graph);
			ai != ai_end;
			++ai) {

			//ai: paper의 이웃. = 저자들
			//                  == coauthors
			const string& node_label = node_label_map[*ai];
			if (find(visited.begin(), visited.end(), node_label) != visited.end()) {
				continue;
			}

			target_desc_range_vec.push_back(vertex(node_idx_map[*ai], *graph));
			visited.push_back(node_label);
		}
	}


	//저자별 논문 수 계산 + TopK Heap 사용
	//target range에 대해 topK 구하기
	//pair -> <num_of_record, label>
	TopKHeap<pair<int, string>> heap(inputK);
	for (auto& n: target_desc_range_vec) {
		//if not author --> skip
		if (node_type_map[n] != NODE_TYPE::NODE_AUTHOR) {
			continue;
		}

		const string& node_label = node_label_map[n];
		int record_cnt = 0;
		//cnt
		for (boost::tie(ai, ai_end) = boost::adjacent_vertices(n, *graph);
			ai != ai_end;
			++ai) {
			++record_cnt;
		}

		boost::put(vertex_record, *graph, n, record_cnt);
		heap.push(make_pair(record_cnt, node_label));
	}

	//get top K records
	int heap_sz = std::min(inputK, heap.getSize());
	pair<int, string> *topk_arr = new pair<int, string>[heap_sz];
	for (int i = 0; i < heap_sz; ++i) {
		topk_arr[i] = heap.pop();
		qDebug() << "topk[" << i << "] = " << topk_arr[i].first << ", " << QString::fromStdString(topk_arr[i].second);
	}


	for (auto& n : nodeList) {
		auto label = n->getLabel();
		if (label.toStdString() != target_name) {
			n->setColor(QColor(Qt::lightGray));
		}
		else {
			n->setColor(QColor(Qt::blue));
		}

		//in target range
		if (find(visited.begin(), visited.end(), label.toStdString()) != visited.end()) {
			n->setColor(QColor(Qt::green));
		}

		//if topK
		for (int i = 0; i < heap_sz; ++i) {
			auto& p = topk_arr[i];
			if (label.toStdString() == p.second) {
				n->setColor(QColor(Qt::red));
				break;
			}
		}
	}

	delete[] topk_arr;
}

void GraphItem::topK_using_custom_score()
{
	// 전체 그래프 기준 topK highlight

	// 저자 노드별 실적 계산
	vertex_iterator vi, vi_end;
	Graph::adjacency_iterator ai, ai_end;

	auto node_label_map = boost::get(vertex_name, *graph);
	auto node_type_map = boost::get(vertex_type, *graph);
	auto node_records_map = boost::get(vertex_record, *graph);

	//k 입력
	bool isok = false;

	QInputDialog *inputDialog = new QInputDialog();
	//int inputK = inputDialog->getText(nullptr, "Enter target's name", "Start node's name:",
	//	QLineEdit::Normal, "Akira Idoue", &isok).toStdString();
	int inputK = inputDialog->getInt(nullptr, "Enter K", "K:", 3,
		1, whole_node_cnt, 1, &isok);
	if (!isok) {
		qDebug("input cancelled");
		return;
	}

	//저자별 논문 수 계산 + TopK Heap 사용
	//pair -> <num_of_record, label>
	TopKHeap<pair<int, string>> heap(inputK);
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
	//pair<int, string> topk_arr[inputK];
	pair<int, string> *topk_arr = new pair<int, string>[inputK];
	for (int i = 0; i < inputK; ++i) {
		topk_arr[i] = heap.pop();
		qDebug() << "topk[" << i << "] = " << topk_arr[i].first << ", " << QString::fromStdString(topk_arr[i].second);
	}


	for (auto& n : nodeList) {
		auto label = n->getLabel();
		n->setColor(QColor(Qt::lightGray));
		for (int i = 0; i < inputK; ++i) {
			auto& p = topk_arr[i];
			if (label.toStdString() == p.second) {
				n->setColor(QColor(Qt::red));
				break;
			}
		}
	}
	delete[] topk_arr;
}

void GraphItem::find_shortest_path()
{
	qDebug("* path highlighting start");
	vertex_iterator vi, vi_end;

	auto node_idx_map = boost::get(vertex_index, *graph);
	auto node_label_map = boost::get(vertex_name, *graph);

	string target_start, target_end;

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
			if (e->match(paths[i], paths[i+1])) {
				e->setColor(QColor(Qt::blue));
				e->setWidth(3);
			}
		}
	}
	qDebug("* path highlighting end");
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
	for (auto& e: edgeList) {
		e->setColor(Qt::black);
		e->setWidth(0);
	}
}

void GraphItem::test()
{
	qDebug("* test action start");


	//전체노드 색 변경
	for (auto& n: nodeList) {
		n->setColor(Qt::lightGray);
	}

	//citation 디버그
	auto node_citation_map = boost::get(vertex_citation, *graph);
	auto node_label_map = boost::get(vertex_name, *graph);
	auto node_type_map = boost::get(vertex_type, *graph);
	auto node_pagerank_map = boost::get(vertex_pagerank, *graph);
	vertex_iterator vi, vi_end;
	for (boost::tie(vi, vi_end) = vertices(*graph); vi != vi_end; ++vi) {
		if (node_type_map[*vi] != NODE_TYPE::NODE_PAPER) continue;
		printf("%s, %d\n", node_label_map[*vi].c_str(),
			node_citation_map[*vi]);
	}

	//page_rank();
	qDebug("* pagerank start");
	boost::graph::page_rank(*graph, node_pagerank_map);
	for (boost::tie(vi, vi_end) = vertices(*graph); vi != vi_end; ++vi) {
		printf("%s\t\t%f\n", node_label_map[*vi].c_str(),
			node_pagerank_map[*vi]);
	}
	qDebug("* pagerank end");



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
