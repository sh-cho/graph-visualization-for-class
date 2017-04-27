#include "stdafx.h"
#include "PaperGraphWidget.h"
#include "NodeItem.h"
#include "GraphicsView.h"

PaperGraphWidget::PaperGraphWidget(QWidget *parent)
	: QWidget(parent)
{
	initscene();

	View *view = new View("temp view");
	view->view()->setScene(scene);

	QVBoxLayout *layout = new QVBoxLayout;
	//QComboBox *combo = new QComboBox;
	//combo->addItem("conf/iastedCSN/KeimS06");
	//combo->addItem("conf/iastedCSN/Mojumdar06");
	//combo->addItem("conf/iastedCSN/PourKKI06");
	//connect(combo, SIGNAL(currentIndexChanged(int)),
	//	this, SLOT(handleSelectionChanged(int)));
	//layout->addWidget(combo);
	layout->addWidget(view);
	setLayout(layout);

	setWindowTitle(tr("dblp paper graph visualization"));
}

void PaperGraphWidget::make_graph(const char* filename)
{
	if (graphItem)
		throw std::exception("graph is already exist");

	graphItem = new GraphItem(filename);
	graphItem->setPos(0, 0);
	scene->addItem(graphItem);
}

void PaperGraphWidget::read_more()
{
	qDebug("* PaperGraphWidget::read_more");
	graphItem->read_more();
	scene->update();
}

void PaperGraphWidget::might_know()
{
	graphItem->might_know();
	scene->update();
}

void PaperGraphWidget::topk()
{
	graphItem->topK_highlight();
	scene->update();
}

void PaperGraphWidget::reset_color()
{
	graphItem->reset_color();
	scene->update();
}

void PaperGraphWidget::handleSelectionChanged(int idx)
{
	QMessageBox::information(this, "QCombobox", 
		"idx: "+QString::number(idx));
	/*if (idx==0) {
	} else if (idx==1) {
	} else {
	}*/
}

void PaperGraphWidget::initscene()
{
	scene = new QGraphicsScene(this);
}
