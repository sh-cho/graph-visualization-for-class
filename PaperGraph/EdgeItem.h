#ifndef EDGEITEM_H
#define EDGEITEM_H

#include "stdafx.h"


class EdgeItem
	: public QGraphicsItem
{
private:
	double x1, y1, x2, y2;
	QString from, to;
	int width;
	QColor color;

protected:
	void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

public:
	EdgeItem(double x1, double y1, double x2, double y2, QColor color, int width,
		QString from, QString to);

	//getter
	QString getFrom() { return from; }
	QString getTo() { return to; }
	
	//setter
	void setWidth(int w) { width = w; }
	void setColor(QColor color) { this->color = color; }

	QRectF boundingRect() const override;
	QPainterPath shape() const override;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
};

#endif // EDGEITEM_H