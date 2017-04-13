#pragma once
#include <QColor>
#include <QComboBox>
#include <QDebug>
#include <QFrame>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGridLayout>
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

#include <algorithm>
#include <exception>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <string>
#include <vector>

using namespace boost;
using namespace std;