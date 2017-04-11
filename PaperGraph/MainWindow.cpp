#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	//ui.setupUi(this);
	graphWidget = new PaperGraphWidget;
	setCentralWidget(graphWidget);

	/*QVBoxLayout *layout = new QVBoxLayout;
	layout->setMargin(5);
	graphWidget->setLayout(layout);*/

	//create menu
	testAct = new QAction(tr("&Test"), this);
	testAct->setStatusTip(tr("Test Action"));
	connect(testAct, &QAction::triggered, this, &MainWindow::test_func);
	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(testAct);

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

void MainWindow::test_func()
{
	/*QMessageBox::information(this, "test", 
		"test: "+QString::number(11));*/
	graphWidget->path_highlight();
}