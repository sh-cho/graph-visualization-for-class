#include "PaperGraphWidget.h"
#include <QtWidgets/QApplication>
#include <QDebug>

#include <algorithm>
#include <exception>
#include <fstream>
#include <iterator>
#include <string>
#include <map>
#include <vector>

#include <boost/graph/fruchterman_reingold.hpp>
//#include <boost/graph/kamada_kawai_spring_layout.hpp>
#include <boost/graph/random_layout.hpp>
#include <boost/graph/circle_layout.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/algorithm/string.hpp>	//boost::split
#include <boost/bimap.hpp>

using namespace std;

/**
 *	Constants
 */
const char* PAPER_FILENAME = "dblp-paper.txt";
enum GRAPH_LAYOUT {
	RANDOM_LAYOUT,
	CIRCLE_LAYOUT,
	//KAMADA_KAWAI_LAYOUT,
	FRUCHTERMAN_REINGOLD_LAYOUT	//slow
};
const int LAYOUT_MODE = GRAPH_LAYOUT::RANDOM_LAYOUT;
const int SCREEN_SIZE = 500;
const int NODE_LIMIT = 100;

Graph read_graph(ifstream& in) {
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
	typedef boost::bimap<string, int> bm_type;
	bm_type node_ids;
	vector<simple_edge> edges_indexes;	//int로 변환된 edge

	int node_cnt = 0;
	qDebug() << "* graph reading start";

	//한 줄씩 읽어서 Parse
	while (std::getline(in, line) && !line.empty()) {
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
	Graph graph(edges_indexes.begin(), edges_indexes.end(), node_ids.size());

	//set index property
	qDebug() << "* set vertex property start";
	typedef typename graph_traits<Graph>::edge_iterator edge_iterator;
	typedef typename graph_traits<Graph>::vertex_iterator vertex_iterator;
	vertex_iterator vi, vi_end;
	int i = 0;
	for (boost::tie(vi, vi_end)=vertices(graph); vi!=vi_end; ++vi) {
		//Vertex Property 설정
		//index: 0 ~ ...
		//name : map의 value(i) 기준으로 찾은 Key
		//		map --> map<string, int> (boost bidirectional map)
		boost::put(vertex_index, graph, *vi, i);
		boost::put(vertex_name, graph, *vi,
			node_ids.right.find(i)->get_left());

		++i;
	}
	qDebug() << "* set vertex property end";


	//모든 edge weight 1로 만들기
	edge_iterator ei, ei_end;
	for (boost::tie(ei, ei_end)=boost::edges(graph); ei!=ei_end; ++ei) {
		boost::put(edge_weight, graph, *ei, 1);
	}


	//path finding between two vertices
	//using dijkstra algorithm
	//ex) "Werner Keim" ---> "Arpad L. Scholtz" (in dataset 2nd line)
	qDebug() << "* path finding start";
	typedef graph_traits<Graph>::vertex_descriptor vertex_descriptor;
	std::vector<vertex_descriptor> parent(num_vertices(graph));
	std::vector<int> distance(num_vertices(graph));
	int start_idx = node_ids.left.find("Werner Keim")->get_right();
	vertex_descriptor start_v = vertex(start_idx, graph);

	dijkstra_shortest_paths(
		graph,
		start_v,
		predecessor_map(
			boost::make_iterator_property_map(parent.begin(), get(boost::vertex_index, graph))
		).distance_map(boost::make_iterator_property_map(distance.begin(), get(boost::vertex_index, graph)))
	);
	qDebug() << "* path finding end";

	int aaaa = 3;


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
	std::vector<Topology::point_difference_type> displacements(num_vertices(graph));
	
	switch (LAYOUT_MODE) {
	case GRAPH_LAYOUT::RANDOM_LAYOUT:
		random_graph_layout(graph, get(vertex_position, graph), rect_top);
		break;

	case GRAPH_LAYOUT::CIRCLE_LAYOUT:
		circle_graph_layout(graph, get(vertex_position, graph), SCREEN_SIZE/2);
		break;

	//case GRAPH_LAYOUT::KAMADA_KAWAI_LAYOUT:
	//	kamada_kawai_spring_layout(graph,
	//		get(vertex_position, graph),
	//		get(edge_weight, graph),
	//		topology,
	//		side_length((double)SCREEN_SIZE)
	//	);
	//	break;

	case GRAPH_LAYOUT::FRUCHTERMAN_REINGOLD_LAYOUT:
		//fruchterman_reingold_force_directed_layout(graph,
		//	get(vertex_position, graph),
		//	topology,
		//	square_distance_attractive_force(),
		//	square_distance_repulsive_force(),
		//	all_force_pairs(),
		//	linear_cooling<double>(SCREEN_SIZE/2),
		//	make_iterator_property_map(displacements.begin(),
		//		get(vertex_index, graph),
		//		Topology::point_difference_type())
		//);
		fruchterman_reingold_force_directed_layout(graph,
			get(vertex_position, graph),
			topology,
			attractive_force(square_distance_attractive_force())
			.cooling(linear_cooling<double>(50))
		);
		break;
	}
	qDebug() << "* make graph layout end";

	return graph;
}

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	//app.setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);
	
	PaperGraphWidget w;

	try {
		ifstream fin(PAPER_FILENAME);
		w.print_graph(read_graph(fin));
		fin.close();
	} catch (const std::exception& e) {
		qDebug() << "Error: " << e.what() << endl;
		return -1;
	}


	w.show();

	return app.exec();
}
