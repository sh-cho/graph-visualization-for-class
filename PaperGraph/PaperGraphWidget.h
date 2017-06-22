#ifndef PAPERGRAPHWIDGET_H
#define PAPERGRAPHWIDGET_H

#include "stdafx.h"

#include "GraphItem.h"

class PaperGraphWidget : public QWidget
{
	Q_OBJECT

public:
	PaperGraphWidget(QWidget *parent = 0);
	void make_graph(const char* filename);

//main window slots
	void read_more();
	void might_know();
	void topk_with_total();
	void topk_with_target();
	void find_shortest_path();
	void topk_with_pagerank();
	void category_visualize();
	void reset_color();

	//test
	void test();

private slots:
	void handleSelectionChanged(int idx);

private:
	void initscene();

	QGraphicsScene *scene;
	GraphItem *graphItem = nullptr;

	QComboBox *testCombo;
};

#endif // PAPERGRAPHWIDGET_H
