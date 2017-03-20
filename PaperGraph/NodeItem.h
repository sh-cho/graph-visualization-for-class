#ifndef NODEITEM_H
#define NODEITEM_H

#include <QColor>
#include <QGraphicsItem>


const int NODE_SIZE = 4;

class NodeItem
	: public QGraphicsItem
{
private:
	int x;
	int y;
	QColor color;

protected:
	void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

public:
	NodeItem(int x, int y);

	QRectF boundingRect() const override;
	QPainterPath shape() const override;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
};

#endif // NODEITEM_H
