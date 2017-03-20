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

#include <boost/algorithm/string.hpp>	//boost::split
//#include <boost/bind.hpp>
#include <boost/bimap.hpp>

using namespace std;


const string PAPER_FILENAME = "dblp-paper.txt";

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

	typedef boost::bimap<string, int> bm_type;
	bm_type node_ids;
	vector<simple_edge> edges_indexes;
	int node_cnt = 0;
	qDebug() << "* graph reading start" << endl;
	while (std::getline(in, line) && !line.empty()) {
		boost::split(tokens, line, boost::is_any_of("||"), boost::token_compress_on);
		boost::split(authors, tokens[1], boost::is_any_of("&&"), boost::token_compress_on);
		
		const string& paper_key = tokens[0];
		if (node_ids.left.find(paper_key) == node_ids.left.end()) {
			//node_ids[paper_key] = node_cnt++;
			node_ids.insert(bm_type::value_type(paper_key, node_cnt++));
			//qDebug() << paper_key.c_str() << " ";
		}

		for (auto author : authors) {
			edges.push_back(pair<string, string>(paper_key, author));
			if (node_ids.left.find(author) == node_ids.left.end()) {
				//node_ids[author] = node_cnt++;
				node_ids.insert(bm_type::value_type(author, node_cnt++));
				//qDebug() << author.c_str() << " ";

			}
		}


		//debug
		if (node_cnt > 100) break;
	}
	qDebug() << "* graph reading complete" << endl;

	//std::sort(node_names.begin(), node_names.end());

	//Make graph
	//Graph --> defined in "PaperGraphWidget.h"
	
	for (auto edge : edges) {
		edges_indexes.push_back({
			node_ids.left.find(edge.first)->get_right(),
			node_ids.left.find(edge.second)->get_right()
		});
	}
	Graph graph(edges_indexes.begin(), edges_indexes.end(), node_ids.size());

	//print map
	//for (auto it=node_ids.left.begin(), itend=node_ids.left.end();
	//	it!=itend; ++it) {
	//	qDebug() << it->first.c_str() << " " << it->second << endl;
	//}

	//set index property
	typedef typename graph_traits<Graph>::edge_iterator edge_iterator;
	typedef typename graph_traits<Graph>::vertex_iterator vertex_iterator;
	vertex_iterator vi, vi_end;
	int i = 0;
	for (boost::tie(vi, vi_end)=vertices(graph); vi!=vi_end; ++vi) {
		boost::put(vertex_index, graph, *vi, i);
		boost::put(vertex_name, graph, *vi,
			node_ids.right.find(i)->get_left());
		//VertexProperties prop = VertexProperties(i,
		//	boost::property<vertex_name_t, std::string,
		//	boost::property<vertex_position_t, point>>());

		++i;
	}


	//for (auto edge : edges) {
	//	//add edge
	//	//VertexProperties prop = VertexProperties()
	//	//add_edge(node_ids[edge.first], node_ids[edge.second],);
	//}

	//make graph layout


	return graph;
}

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	app.setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);
	
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
