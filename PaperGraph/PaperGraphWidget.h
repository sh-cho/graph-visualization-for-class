#ifndef PAPERGRAPHWIDGET_H
#define PAPERGRAPHWIDGET_H

#include <QtWidgets/QWidget>
#include <QGraphicsScene>

#include <fstream>

#include "GraphItem.h"
#include "ui_PaperGraphWidget.h"


class PaperGraphWidget : public QWidget
{
	Q_OBJECT

public:
	PaperGraphWidget(QWidget *parent = 0);
	void print_graph(ifstream& fin);

private slots:
	void handleSelectionChanged(int idx);

private:
	void initscene();

	Ui::PaperGraphWidgetClass ui;
	QGraphicsScene *scene;
};

#endif // PAPERGRAPHWIDGET_H
