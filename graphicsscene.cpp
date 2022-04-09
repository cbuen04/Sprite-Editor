#include "graphicsscene.h"

GraphicsScene::GraphicsScene(QObject *parent)
    : QGraphicsScene(parent)
{

}

void GraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{

      if (mouseEvent->button() == Qt::LeftButton)
      {
          double rad = 1;
          QPointF pt = mouseEvent->scenePos();
          this->addRect(pt.x()-rad, pt.y()-rad, rad*2.0, rad*2.0,QPen(),
          QBrush(Qt::SolidPattern));
      }

    QGraphicsScene::mousePressEvent(mouseEvent);
}
