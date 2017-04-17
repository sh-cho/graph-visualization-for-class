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

void PaperGraphWidget::print_graph(ifstream& fin)
{
	//QGraphicsItem *graph_item = new GraphItem(fin);
	if (graphItem)
		throw std::exception("already have graph item");

	graphItem = new GraphItem(fin, READ_LINE_UNIT);
	graphItem->setPos(0, 0);
	scene->addItem(graphItem);
}

void PaperGraphWidget::path_highlight()
{
	graphItem->path_highlighting(std::string(""), std::string(""));
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
