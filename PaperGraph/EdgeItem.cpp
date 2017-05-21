#include "stdafx.h"
#include "EdgeItem.h"

void EdgeItem::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
}

void EdgeItem::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{
}

void EdgeItem::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
}

EdgeItem::EdgeItem(double x1, double y1, double x2, double y2, QColor color, int width,
	QString from, QString to)
{
	this->x1 = x1;
	this->y1 = y1;
	this->x2 = x2;
	this->y2 = y2;

	this->color = color;
	this->width = width;

	this->from = from;
	this->to = to;
	setZValue(0);	//노드 앞 가리지 않도록 ZValue 설정

	/*setFlags(ItemIsSelectable | ItemIsMovable);
	setAcceptHoverEvents(true);*/
}

QRectF EdgeItem::boundingRect() const
{
	return QRectF(0,0,0,0);
}

QPainterPath EdgeItem::shape() const
{
	QPainterPath path;
	return path;
}

void EdgeItem::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
	Q_UNUSED(widget);

	QPen oldPen = painter->pen();
	QPen pen = oldPen;
	pen.setWidth(width);
	pen.setColor(color);
	painter->setPen(pen);
	painter->drawLine(QLineF(x1, y1, x2, y2));
}
