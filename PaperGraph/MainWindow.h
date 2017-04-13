#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "stdafx.h"
//#include "ui_MainWindow.h"
#include "PaperGraphWidget.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();

	void print_graph(std::ifstream& fin);
	void test_func();

private:
	//Ui::MainWindow ui;
	PaperGraphWidget *graphWidget;

	QMenu *fileMenu;
	QAction *testAct;
};

#endif // MAINWINDOW_H
