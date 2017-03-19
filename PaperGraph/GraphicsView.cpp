#include "GraphicsView.h"
#include <qmath.h>

//View
View::View(const QString& name, QWidget *parent)
	: QFrame(parent)
{
	//GraphicsView settings
	graphicsView = new GraphicsView(this);
	graphicsView->setDragMode(QGraphicsView::RubberBandDrag);
	graphicsView->setOptimizationFlags(QGraphicsView::DontSavePainterState);
	graphicsView->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
	graphicsView->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

	//connect

	//layout
	QGridLayout *topLayout = new QGridLayout;
	topLayout->addWidget(graphicsView, 0, 0);
	setLayout(topLayout);

	setupMatrix();
}

QGraphicsView* View::view() const
{
	return static_cast<QGraphicsView *>(graphicsView);
}

void View::setupMatrix()
{
	qreal scale = qPow(qreal(2), qreal(2));

	QMatrix matrix;
	matrix.scale(scale, scale);
	matrix.rotate(qreal(0));

	graphicsView->setMatrix(matrix);
}