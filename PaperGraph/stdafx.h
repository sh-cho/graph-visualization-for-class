#pragma once
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

using namespace boost;
using namespace std;

/* enums */
enum vertex_position_t { vertex_position };
enum vertex_type_t { vertex_type };
enum vertex_record_t { vertex_record };
namespace boost {
	BOOST_INSTALL_PROPERTY(vertex, position);
	BOOST_INSTALL_PROPERTY(vertex, type);
	BOOST_INSTALL_PROPERTY(vertex, record);
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
	boost::property<vertex_position_t, point,
	boost::property<vertex_type_t, int,
	boost::property<vertex_record_t, int>>>>
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

	/* topK */
	//const int TOP_K = 5;	//상위 몇 개 아이템에 대해 highlight 할 지

	/* a research you might know */
	//const char* TARGET_AUTHOR_NAME = "Shuichi Itoh";
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