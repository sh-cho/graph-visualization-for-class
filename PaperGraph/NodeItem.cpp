#include "stdafx.h"
#include "NodeItem.h"

void NodeItem::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
	/*if (event->button() == Qt::LeftButton) {
		QMessageBox msgbox;
		msgbox.setText("hi");
		msgbox.setInformativeText("hi2");
		msgbox.setStandardButtons(QMessageBox::Ok);
		msgbox.setDefaultButton(QMessageBox::Ok);

		msgbox.exec();
	}*/
}

void NodeItem::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{
}

void NodeItem::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
}

NodeItem::NodeItem(double x, double y, QColor color, QString label, int type, int category,
	double _accuracy, int year)
{
	//node constructor
	this->x = x;
	this->y = y;
	this->color = color;
	this->label = label;
	this->type = type;
	this->category = category;
	this->category_accuracy = _accuracy;
	this->year = year;
	setZValue(1);
	
	//setFlags(ItemIsSelectable | ItemIsMovable);
	//setAcceptHoverEvents(true);
}

QRectF NodeItem::boundingRect() const
{
	return QRectF(0, 0, NODE_SIZE, NODE_SIZE);
}

QPainterPath NodeItem::shape() const
{
	QPainterPath path;
	path.addRect(0, 0, NODE_SIZE, NODE_SIZE);
	return path;
}

void NodeItem::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
	Q_UNUSED(widget);

	//set node pen style
	QPen oldPen = painter->pen();
	QPen pen = oldPen;
	pen.setWidth(0);
	pen.setColor(QColor(Qt::black));
	painter->setPen(pen);

	//label 출력
	QFont font("Gulim", 3);	//set font, font size
	painter->setFont(font);
	painter->save();
	painter->drawText(x, y, label);
	painter->restore();


	//node rectangle 출력
	QColor fillColor = (option->state & QStyle::State_Selected) ? color.dark(150) : color;
	if (option->state & QStyle::State_MouseOver) fillColor = fillColor.light(125);
	painter->setBrush(QBrush(fillColor));
	painter->drawRect(x, y, NODE_SIZE, NODE_SIZE);
}
