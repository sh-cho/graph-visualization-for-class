#include "stdafx.h"
#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	graphWidget = new PaperGraphWidget;
	setCentralWidget(graphWidget);

	/*QVBoxLayout *layout = new QVBoxLayout;
	layout->setMargin(5);
	graphWidget->setLayout(layout);*/
	createActions();
	createMenus();

	QString message = tr("test message");
	statusBar()->showMessage(message);

	setMinimumSize(160, 160);
	resize(1200, 650);
}

MainWindow::~MainWindow()
{

}

void MainWindow::make_graph(const char* filename)
{
	graphWidget->make_graph(filename);
}

//////////////////////////////////////////////////////////////////
// private methods
//////////////////////////////////////////////////////////////////
void MainWindow::createActions()
{
	readMoreAct = new QAction(tr("Read more"), this);
	readMoreAct->setStatusTip(tr("read more lines from file"));
	connect(readMoreAct, &QAction::triggered, this, &MainWindow::read_more);


	mightKnowAct = new QAction(tr("Might know"), this);
	mightKnowAct->setStatusTip(tr("highlight a research you might know"));
	connect(mightKnowAct, &QAction::triggered, this, &MainWindow::might_know);
	topkWithTotalAct = new QAction(tr("topK with total"), this);
	topkWithTotalAct->setStatusTip(tr("highlight who was top k papers in whole graph"));
	connect(topkWithTotalAct, &QAction::triggered, this, &MainWindow::topK_with_total);
	topKWithTargetAct = new QAction(tr("topK with target"), this);
	topKWithTargetAct->setStatusTip(tr("highlight who was top k papers in particular range"));
	connect(topKWithTargetAct, &QAction::triggered, this, &MainWindow::topK_with_target);
	findShortestPathAct = new QAction(tr("Find Shortest Path"), this);
	findShortestPathAct->setStatusTip("Find shortest path between two node");
	connect(findShortestPathAct, &QAction::triggered, this, &MainWindow::find_shortest_path);
	topkWithPagerankAct = new QAction(tr("topK with pagerank"), this);
	topkWithPagerankAct->setStatusTip(tr("highlight which is in top k pagerank in whole graph"));
	connect(topkWithPagerankAct, &QAction::triggered, this, &MainWindow::topk_with_pagerank);

	resetColorAct = new QAction(tr("Reset colors"), this);
	resetColorAct->setStatusTip(tr("Reset all node's color"));
	connect(resetColorAct, &QAction::triggered, this, &MainWindow::reset_color);


	testAct = new QAction(tr("test action"), this);
	testAct->setStatusTip(tr("test test"));
	connect(testAct, &QAction::triggered, this, &MainWindow::test);
}

void MainWindow::createMenus()
{
	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(readMoreAct);

	actionMenu = menuBar()->addMenu(tr("&Actions"));
	actionMenu->addAction(mightKnowAct);
	actionMenu->addAction(topkWithTotalAct);
	actionMenu->addAction(topKWithTargetAct);
	actionMenu->addAction(findShortestPathAct);
	actionMenu->addAction(topkWithPagerankAct);

	actionMenu->addAction(resetColorAct);

	testMenu = menuBar()->addMenu(tr("&Test"));
	testMenu->addAction(testAct);
}

//////////////////////////////////////////////////////////////////
// slots
//////////////////////////////////////////////////////////////////
void MainWindow::read_more()
{
	qDebug("* MainWindow::read_more");
	graphWidget->read_more();
}

void MainWindow::might_know()
{
	graphWidget->might_know();
}

void MainWindow::topK_with_total()
{
	graphWidget->topk_with_total();
}

void MainWindow::topK_with_target()
{
	graphWidget->topk_with_target();
}

void MainWindow::find_shortest_path()
{
	graphWidget->find_shortest_path();
}

void MainWindow::topk_with_pagerank() {
	graphWidget->topk_with_pagerank();
}

void MainWindow::reset_color()
{
	graphWidget->reset_color();
}

void MainWindow::test()
{
	graphWidget->test();
}