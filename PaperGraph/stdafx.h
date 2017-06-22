#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <QColor>
#include <QComboBox>
#include <QDebug>
#include <QFrame>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGridLayout>
#include <QInputDialog>
#include <QKeyEvent>
#include <QList>
#include <QMainWindow>
#include <qmath.h>
#include <QMessageBox>
#include <QtGui>
#include <QtWidgets>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QLegend>
#include <QtCharts/QBarCategoryAxis>

#include <boost/algorithm/string.hpp>	//boost::split
#include <boost/bimap.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/circle_layout.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/fruchterman_reingold.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/page_rank.hpp>
#include <boost/graph/random_layout.hpp>
#include <boost/graph/topology.hpp>
#include <boost/regex.hpp>

#include <curl/curl.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <algorithm>
#include <exception>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <map>
#include <queue>
#include <string>
#include <utility>
#include <vector>

#include <bibtexreader.hpp>

#include <rapidjson/document.h>

#include "bibtex_processor.h"
#include "curl_processor.h"
#include "json_processor.h"

using bibtex::BibTeXEntry;
using namespace boost;
using namespace std;

/* Qt namespaces */

/* function declaration */
size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp);

/* enums */
enum vertex_position_t { vertex_position };
enum vertex_type_t { vertex_type };
enum vertex_record_t { vertex_record };
enum vertex_citation_t { vertex_citation };						//for paper
enum vertex_pagerank_t { vertex_pagerank };
enum vertex_category_t { vertex_category };						//for paper
enum vertex_title_t { vertex_title };							//for paper
enum vertex_year_t { vertex_year };								//for paper
enum vertex_category_accuracy_t { vertex_category_accuracy };	//for paper
namespace boost {
	BOOST_INSTALL_PROPERTY(vertex, position);
	BOOST_INSTALL_PROPERTY(vertex, type);
	BOOST_INSTALL_PROPERTY(vertex, record);
	BOOST_INSTALL_PROPERTY(vertex, citation);
	BOOST_INSTALL_PROPERTY(vertex, pagerank);
	BOOST_INSTALL_PROPERTY(vertex, category);
	BOOST_INSTALL_PROPERTY(vertex, title);
	BOOST_INSTALL_PROPERTY(vertex, year);
	BOOST_INSTALL_PROPERTY(vertex, category_accuracy);
}
enum PAPER_CATEGORY {
	CS_AI,
	CS_CL,
	CS_CC,
	CS_CE,
	CS_CG,
	CS_GT,
	CS_CV,
	CS_CY,
	CS_CR,
	CS_DS,
	CS_DB,
	CS_DL,
	CS_DM,
	CS_DC,
	CS_ET,
	CS_FL,
	CS_GL,
	CS_GR,
	CS_AR,
	CS_HC,
	CS_IR,
	CS_IT,
	CS_LG,
	CS_LO,
	CS_MS,
	CS_MA,
	CS_MM,
	CS_NI,
	CS_NE,
	CS_NA,
	CS_OS,
	CS_OH,
	CS_PF,
	CS_PL,
	CS_RO,
	CS_SI,
	CS_SE,
	CS_SD,
	CS_SC,
	CS_SY
};
enum NODE_TYPE {
	NODE_PAPER,
	NODE_AUTHOR
};
enum GRAPH_LAYOUT {
	RANDOM_LAYOUT,
	CIRCLE_LAYOUT,
	//KAMADA_KAWAI_LAYOUT,
	FRUCHTERMAN_REINGOLD_LAYOUT	//slow
};

/* typedef */
typedef boost::bimap<string, int> bm_type;
typedef square_topology<>::point_type point;
typedef boost::property<vertex_index_t, int,
	boost::property<vertex_name_t, std::string,
	boost::property<vertex_position_t, point,	//좌표값
	boost::property<vertex_type_t, int,			//타입. enum NODE_TYPE에 정의됨
	boost::property<vertex_record_t, int,		//이웃 노드 개수
	boost::property<vertex_citation_t, int,		//피인용수
	boost::property<vertex_pagerank_t, double,	//페이지랭크값 계산해서 저장할 property
	boost::property<vertex_category_t, int,		//논문 주제
	boost::property<vertex_title_t, std::string,	//논문 제목
	boost::property<vertex_year_t, int,
	boost::property<vertex_category_accuracy_t, double>
	>>>>>>>>>
	> VertexProperties;
typedef boost::adjacency_list<
	listS,	//outEdgeList
	listS,	//VertexList
	undirectedS,
	//vertex properties
	VertexProperties,
	//edge properties
	boost::property<edge_weight_t, double>
> Graph;
typedef typename graph_traits<Graph>::edge_iterator edge_iterator;
typedef typename graph_traits<Graph>::vertex_iterator vertex_iterator;
typedef boost::graph_traits<Graph>::adjacency_iterator adjacencyIterator;
typedef boost::graph_traits<Graph>::vertex_descriptor vertex_descriptor;
typedef square_topology<> Topology;
typedef typename Topology::point_type Point;

/* constants */
namespace {
	/* file io */
	const char* PAPER_FILENAME = "dblp-paper.txt";

	/* visualization */
	const int NODE_SIZE = 4;
	const int LAYOUT_MODE = GRAPH_LAYOUT::RANDOM_LAYOUT;
	//const int SCREEN_SIZE = 3000;
	const int SCREEN_SIZE = 500;
	const int READ_LINE_UNIT = 100;	//한 번에 몇 라인을 읽을지

	/* curl processor */
	curl_processor _curl_processor;

	/* bibtex processor */
	bibtex_processor _bibtex_processor;

	/* json processor */
	json_processor _json_processor;

	/* category keywords */
	vector<vector<string>> keywords{
		{ "LANGUAGE", "TRANSLATION", "TEXT", "PHRASE", "PARAGRAPH" },
		{ "COMPLEXITY", "EXPONENTIALLY", "FOURIER", "CASCADE", "NP-COMPLETE" },
		{ "ENGINEERING", "FINANCE", "STOCHASTIC", "BUSINESS", "DAMAGE" },
		{ "GEOMETRY", "COLLINEAR", "DISTANCE", "DIMENSION", "GEOMETRIC" },
		{ "GAME", "GAMES", "INTERFERENCE", "REVENUE", "AGGREGATIVE" },
		{ "VISION", "PATTERN", "RECOGNITION", "FEATURE", "FIXATION" },
		{ "SOCIETY", "MEDIA", "SOCIAL", "MOBILE", "EVENT" },
		{ "CRYPTOGRAPHY", "SECURITY", "CODE", "CRYPTOSYSTEM" },
		{ "GRAPH", "POLYNOMIAL" },
		{ "DATABASE", "DATA", "JOIN", "TRANSACTION" },
		{ "LIBRARY", "PREPRINT", "RESEARCH" },
		{ "DISCRETE", "COMBINATORIC" },
		{ "DISTRIBUTED", "PARALLEL", "CLUSTER", "CLOUD" },
		{ "NEUTRON", "QUANTUM" },
		{ "LANGUAGE", "AUTOMATA" },
		{ "LITERATURE" },
		{ "GAUSSIAN", "3D", "SHAPE", "GRAPHIC" },
		{ "HARDWARE", "CAMERA", "CACHE", "ENERGY", "HYBRID" },
		{ "PERSON", "INDOOR", "HUMAN" },
		{ "MINING", "MICROBLOG", "METADATA" },
		{ "UPLINK", "INFORMATION" },
		{ "LEARNING", "DEEP", "NEURAL", "GAN", "CNN", "RNN" },
		{ "LOGIC", "HEURISTIC" },
		{ "MATHE", "MATRIX", "LINEAR", "ALGEBRA" },
		{ "MULTI-AGENT", "AGENT", "COLLABORATIVE", "CONVERGENCE" },
		{ "CODEC", "MEDIA", "AUDIO", "IMAGE" },
		{ "NETWORK", "INTERNET", "CLOUD", "SDN" },
		{ "NEURAL", "EVOLUTION", "LEARNING" },
		{ "NUMERICALLY", "NUMERICAL" },
		{ "SCHEDULING", "PREFETCHING", "VM", "SYSTEM", "LINUX" },
		{},
		{ "PERFORMANCE", "HIGH", "ADAPTIVE" },
		{ "GRAMMAR", "PROGRAM" },
		{ "ROBOT", "ROBOTIC", "HARVESTER", "CRAWLER" },
		{ "SOCIAL", "INFORMATION", "LOCATION", "EVENT", "MICROBLOG" },
		{ "SOFTWARE", "ENGINEERING", "PROTOTYPE", "DEVELOPMENT" },
		{ "MULTI-CHANNEL", "ACOUSTIC", "AUDIO", "AUTOENCODER" },
		{ "SYMBOLIC", "FUNCTION", "COEFFICIENT" },
		{ "SYSTEM", "SENSORY", "TRANSFORMER", "CONTROL" }
	};

	vector<string> category_colors{
		"#694842","#325eee","#38cff1","#0b7d18","#20a920",
		"#ce112f","#479c12","#7cb382","#c4d5cb","#d52840",
		"#b6070e","#f1c50b","#181ce4","#6ed976","#abecdc",
		"#ddb390","#298d3e","#e48b31","#183083","#a03350",
		"#309c0c","#75fa48","#6ce15c","#82dee1","#845576",
		"#b9c3fb","#e59908","#30827a","#0658d1","#8b921c",
		"#fa7529","#b91ad2","#545e87","#cb6eae","#c2d4ba",
		"#dffa88","#5942d5","#32add6","#99443f","#c85b70"
	};
}

/* boost */
namespace boost {
	const boost::regex paper_reg("(conf|journals).*");
}

/* topK heap */
template <typename T>
class TopKHeap {
private:
	int k;		//max size
	int size;	//current size
	T *heap;

private:
	void reHeapDown(int root) {
		//after pop
		int minIdx, left, right;

		left = root * 2 + 1;
		right = root * 2 + 2;

		if (left <= (size - 1)) {
			if (left == (size - 1))
				minIdx = left;
			else {
				minIdx = ((heap[left] <= heap[right]) ? left : right);
			}

			if (heap[root] > heap[minIdx]) {
				swap(heap[root], heap[minIdx]);
				reHeapDown(minIdx);
			}
		}
	}
	void reHeapUp(int root, int bottom) {
		//bottom: 위로 올릴 노드 idx
		//after push
		if (root > bottom) {
			int parent = (bottom - 1) / 2;
			if (heap[parent] < heap[bottom]) {
				swap(heap[parent], heap[bottom]);
				reHeapUp(root, parent);
			}
		}
	}

public:
	TopKHeap(int _k) : k(_k), size(0) {
		if (_k <= 0) abort();
		heap = new T[_k];
		memset(heap, 0, sizeof(heap));
	}
	~TopKHeap() {
		if (heap)	delete[] heap;
	}

public:
	void push(T elem) {
		if (size < k) {
			heap[size++] = elem;
		}
		else {
			if (elem < heap[0]) {
				//less than minimum
			}
			else {
				pop();
				heap[size++] = elem;
			}
		}

		reHeapUp(0, size - 1);
	}
	T pop() {
		if (size <= 0)
			abort();
		else if (size == 1) {
			T ret = heap[--size];
			return ret;
		}
		else {
			T ret = heap[0];
			heap[0] = heap[--size];
			reHeapDown(0);
			return ret;
		}
	}
	int getSize() {return size;}
	bool isFull() {return k==size;}
};
