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

	void print_graph(std::ifstream& fin);
	/*void test_highlighting();*/

private:
	PaperGraphWidget *graphWidget;

	QMenu *actionMenu;
	QAction *testHighlightAct;
	QAction *resetColorAct;

private:
	void createActions();
	void createMenus();

private slots:
	void test_highlighting();
	void reset_color();
};

#endif // MAINWINDOW_H
