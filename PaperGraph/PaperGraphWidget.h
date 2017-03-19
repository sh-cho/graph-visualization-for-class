#ifndef PAPERGRAPHWIDGET_H
#define PAPERGRAPHWIDGET_H

#include <QtWidgets/QWidget>
#include <QGraphicsScene>
#include "ui_PaperGraphWidget.h"

//class QGraphicsScene;

class PaperGraphWidget : public QWidget
{
	Q_OBJECT

public:
	PaperGraphWidget(QWidget *parent = 0);

private:
	void initscene();

	Ui::PaperGraphWidgetClass ui;
	QGraphicsScene *scene;
};

#endif // PAPERGRAPHWIDGET_H
