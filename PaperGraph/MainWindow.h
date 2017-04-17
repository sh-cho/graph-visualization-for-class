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

	//void print_graph(std::ifstream *pFin);
	void print_graph(const char* filename);


private:
	PaperGraphWidget *graphWidget;

	QMenu *fileMenu;
	QAction *readMoreAct;
	QMenu *actionMenu;
	QAction *testHighlightAct;
	QAction *topkAct;
	QAction *resetColorAct;

private:
	void createActions();
	void createMenus();

private slots:
	void read_more();
	void test_highlighting();
	void topk();
	void reset_color();
};

#endif // MAINWINDOW_H
