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
	resize(800, 600);
}

MainWindow::~MainWindow()
{

}

void MainWindow::print_graph(const char * filename)
{
	graphWidget->print_graph(filename);
}

//void MainWindow::print_graph(std::ifstream *pFin)
//{
//	graphWidget->print_graph(pFin);
//}

//////////////////////////////////////////////////////////////////
// private methods
//////////////////////////////////////////////////////////////////
void MainWindow::createActions()
{
	readMoreAct = new QAction(tr("Read more"), this);
	readMoreAct->setStatusTip(tr("read more lines from file"));
	connect(readMoreAct, &QAction::triggered, this, &MainWindow::read_more);

	testHighlightAct = new QAction(tr("Highlight"), this);
	testHighlightAct->setStatusTip(tr("Highlighting node"));
	connect(testHighlightAct, &QAction::triggered, this, &MainWindow::test_highlighting);
	topkAct = new QAction(tr("topK"), this);
	topkAct->setStatusTip(tr("highlight who was top k papers"));
	connect(topkAct, &QAction::triggered, this, &MainWindow::topk);
	resetColorAct = new QAction(tr("Reset colors"), this);
	resetColorAct->setStatusTip(tr("Reset all node's color"));
	connect(resetColorAct, &QAction::triggered, this, &MainWindow::reset_color);
}

void MainWindow::createMenus()
{
	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(readMoreAct);

	actionMenu = menuBar()->addMenu(tr("&Actions"));
	actionMenu->addAction(testHighlightAct);
	actionMenu->addAction(topkAct);
	actionMenu->addAction(resetColorAct);
}

//////////////////////////////////////////////////////////////////
// slots
//////////////////////////////////////////////////////////////////
void MainWindow::read_more()
{
	qDebug("* MainWindow::read_more");
	graphWidget->read_more();
}

void MainWindow::test_highlighting()
{
	/*QMessageBox::information(this, "test", 
	"test: "+QString::number(11));*/
	graphWidget->path_highlight();
	//graphWidget->update();
}

void MainWindow::topk()
{
	graphWidget->topk();
}

void MainWindow::reset_color()
{
	graphWidget->reset_color();
}
