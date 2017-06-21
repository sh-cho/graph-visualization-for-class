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
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtWidgets>

#include <boost/algorithm/string.hpp>	//boost::split
#include <boost/bimap.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/circle_layout.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/fruchterman_reingold.hpp>
#include <boost/graph/graph_traits.hpp>
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

using bibtex::BibTeXEntry;
using namespace boost;
using namespace std;

/* function declaration */
size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp);

/* enums */
enum vertex_position_t { vertex_position };
enum vertex_type_t { vertex_type };
enum vertex_record_t { vertex_record };
enum vertex_citation_t { vertex_citation };
namespace boost {
	BOOST_INSTALL_PROPERTY(vertex, position);
	BOOST_INSTALL_PROPERTY(vertex, type);
	BOOST_INSTALL_PROPERTY(vertex, record);
	BOOST_INSTALL_PROPERTY(vertex, citation);
}
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
	boost::property<vertex_citation_t, int>
	>>>>
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
	//const int READ_LINE_UNIT = 20;	//한 번에 몇 라인을 읽을지
	const int READ_LINE_UNIT = 100;

	/* curl processor */
	curl_processor _curl_processor;

	/* bibtex processor */
	bibtex_processor _bibtex_processor;
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
