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
	QAction *topkAct;
	QAction *resetColorAct;

private:
	void createActions();
	void createMenus();

private slots:
	void read_more();
	void might_know();
	void topk();
	void reset_color();
};

#endif // MAINWINDOW_H
