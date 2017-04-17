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

protected:
	void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

public:
	NodeItem(double x, double y, QColor color, QString label, int type);

	//getter setter
	QString getLabel() {return label;}
	int getType() { return type; }
	void setColor(QColor color) {this->color=color;}

	QRectF boundingRect() const override;
	QPainterPath shape() const override;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
};

#endif // NODEITEM_H
