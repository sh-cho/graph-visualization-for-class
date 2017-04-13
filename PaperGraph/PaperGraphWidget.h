#ifndef PAPERGRAPHWIDGET_H
#define PAPERGRAPHWIDGET_H

#include "stdafx.h"

#include "GraphItem.h"

class PaperGraphWidget : public QWidget
{
	Q_OBJECT

public:
	PaperGraphWidget(QWidget *parent = 0);
	void print_graph(ifstream& fin);

	void path_highlight();
	void reset_color();

private slots:
	void handleSelectionChanged(int idx);

private:
	void initscene();

	QGraphicsScene *scene;
	GraphItem *graphItem = nullptr;
};

#endif // PAPERGRAPHWIDGET_H
