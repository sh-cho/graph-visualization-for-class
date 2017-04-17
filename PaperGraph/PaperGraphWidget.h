#ifndef PAPERGRAPHWIDGET_H
#define PAPERGRAPHWIDGET_H

#include "stdafx.h"

#include "GraphItem.h"

class PaperGraphWidget : public QWidget
{
	Q_OBJECT

public:
	PaperGraphWidget(QWidget *parent = 0);
	//void print_graph(ifstream* pFin);
	void print_graph(const char* filename);

//main window slots
	void read_more();
	void path_highlight();
	void topk();
	void reset_color();

private slots:
	void handleSelectionChanged(int idx);

private:
	void initscene();

	QGraphicsScene *scene;
	GraphItem *graphItem = nullptr;
};

#endif // PAPERGRAPHWIDGET_H
