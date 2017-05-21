#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "stdafx.h"
#include "PaperGraphWidget.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();

	void make_graph(const char* filename);


private:
	PaperGraphWidget *graphWidget;

	QMenu *fileMenu;
	QAction *readMoreAct;
	QMenu *actionMenu;
	QAction *mightKnowAct;
	QAction *topkWithTotalAct;
	QAction *topKWithTargetAct;
	QAction *resetColorAct;
	QAction *findShortestPathAct;
	
	//test
	QMenu *testMenu;
	QAction *testAct;

private:
	void createActions();
	void createMenus();

private slots:
	void read_more();
	void might_know();
	void topK_with_total();
	void topK_with_target();
	void find_shortest_path();
	void reset_color();
	//test
	void test();
};

#endif // MAINWINDOW_H
