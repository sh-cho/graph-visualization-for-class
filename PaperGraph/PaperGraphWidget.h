#ifndef PAPERGRAPHWIDGET_H
#define PAPERGRAPHWIDGET_H

#include <QtWidgets/QWidget>
#include "ui_PaperGraphWidget.h"

class PaperGraphWidget : public QWidget
{
	Q_OBJECT

public:
	PaperGraphWidget(QWidget *parent = 0);
	~PaperGraphWidget();

private:
	Ui::PaperGraphWidgetClass ui;
};

#endif // PAPERGRAPHWIDGET_H
