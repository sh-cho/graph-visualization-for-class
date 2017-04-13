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

void MainWindow::print_graph(std::ifstream& fin)
{
	graphWidget->print_graph(fin);
}

//////////////////////////////////////////////////////////////////
// private methods
//////////////////////////////////////////////////////////////////
void MainWindow::createActions()
{
	testHighlightAct = new QAction(tr("&Highlight"), this);
	testHighlightAct->setStatusTip(tr("Highlighting node"));
	connect(testHighlightAct, &QAction::triggered, this, &MainWindow::test_highlighting);

	resetColorAct = new QAction(tr("&Reset"), this);
	resetColorAct->setStatusTip(tr("Reset all node's color"));
	connect(resetColorAct, &QAction::triggered, this, &MainWindow::reset_color);
}

void MainWindow::createMenus()
{
	actionMenu = menuBar()->addMenu(tr("&Actions"));
	actionMenu->addAction(testHighlightAct);
	actionMenu->addAction(resetColorAct);
}

//////////////////////////////////////////////////////////////////
// slots
//////////////////////////////////////////////////////////////////
void MainWindow::test_highlighting()
{
	/*QMessageBox::information(this, "test", 
	"test: "+QString::number(11));*/
	graphWidget->path_highlight();
	//graphWidget->update();
}

void MainWindow::reset_color()
{
	graphWidget->reset_color();
}