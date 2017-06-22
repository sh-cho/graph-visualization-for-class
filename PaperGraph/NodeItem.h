#ifndef NODEITEM_H
#define NODEITEM_H

#include "stdafx.h"

class NodeItem
	: public QGraphicsItem
{
private:
	double x;
	double y;
	QColor color;
	QString label;
	int type;
	int category;
	double category_accuracy;
	int year;

protected:
	void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

public:
	NodeItem(double x, double y, QColor color, QString label, int type, int category,
		double _accuracy, int year);

	//getter setter
	QString getLabel() {return label;}
	int getType() { return type; }
	int getCategory() { return category; }
	double getAccuracy() { return category_accuracy; }
	int getYear() { return year; }
	void setColor(QColor color) {this->color=color;}

	QRectF boundingRect() const override;
	QPainterPath shape() const override;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
};

#endif // NODEITEM_H
