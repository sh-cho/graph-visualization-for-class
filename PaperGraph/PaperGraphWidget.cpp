#include "PaperGraphWidget.h"
#include "NodeItem.h"
#include "GraphicsView.h"

PaperGraphWidget::PaperGraphWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	initscene();

	View *view = new View("temp view");
	view->view()->setScene(scene);
	QHBoxLayout *layout = new QHBoxLayout;
	layout->addWidget(view);
	setLayout(layout);

	setWindowTitle(tr("Paper Graph Visualization"));
}

void PaperGraphWidget::initscene()
{
	scene = new QGraphicsScene(this);

	int x = 0, y;
	for (int i=-11000; i<11000; i+=110) {
		++x;
		y = 0;
		for (int j=-7000; j<7000; j+=70) {
			++y;
			QGraphicsItem *item = new NodeItem(x, y);
			item->setPos(QPointF(i, j));
			scene->addItem(item);
		}
	}
}
