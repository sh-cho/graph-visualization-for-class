#include "NodeItem.h"
#include <QtWidgets>

void NodeItem::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
}

void NodeItem::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{
}

void NodeItem::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
}

NodeItem::NodeItem(int x, int y, QString label)
{
	this->x = x;
	this->y = y;
	this->color = QColor(Qt::green);
	this->label = label;
	setZValue(1);
	
	setFlags(ItemIsSelectable | ItemIsMovable);
	setAcceptHoverEvents(true);
}

QRectF NodeItem::boundingRect() const
{
	return QRectF(0, 0, NODE_SIZE, NODE_SIZE);
}

QPainterPath NodeItem::shape() const
{
	QPainterPath path;
	//path.addRect(14, 14, 82, 42);
	path.addRect(0, 0, NODE_SIZE, NODE_SIZE);
	return path;
}

void NodeItem::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
	Q_UNUSED(widget);

	//label 출력
	QFont font("Gulim", 10);
	font.setStyleStrategy(QFont::ForceOutline);
	painter->setFont(font);
	painter->save();
	painter->scale(0.3, 0.3);
	painter->drawText(0, 0, label);
	painter->restore();


	//Rectangle 출력
	QColor fillColor = (option->state & QStyle::State_Selected) ? color.dark(150) : color;
	if (option->state & QStyle::State_MouseOver) fillColor = fillColor.light(125);
	QPen pen = painter->pen();
	pen.setWidth(0);
	pen.setColor(QColor(Qt::black));
	painter->setPen(pen);
	painter->setBrush(QBrush(fillColor));
	painter->drawRect(0, 0, NODE_SIZE, NODE_SIZE);
}
