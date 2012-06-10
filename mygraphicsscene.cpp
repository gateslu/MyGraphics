#include "mygraphicsscene.h"
#include <QDebug>

MyGraphicsScene::MyGraphicsScene(QObject *parent)
    : QGraphicsScene(parent)
{
}

void MyGraphicsScene::selectedItem(QGraphicsItem *item)
{
    QList<QGraphicsItem*> list = this->items();
    for (int i = 0; i < list.size(); ++i) {
        list.at(i)->setSelected(false);
    }
    item->setSelected(true);
    emit itemClicked(item);
}

void MyGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() != Qt::LeftButton)
        return;
    QGraphicsScene::mousePressEvent(event);
    //    QPoint p = inverseWorldMatrix().map(event->pos());
//    QPointF p = event->scenePos();
    QList<QGraphicsItem*> l = this->selectedItems();
    moving = 0;
    //    qDebug() << l.size();
    if (!l.isEmpty())
    {
        moving = l.first();
    }
//    moving_start = p;
//    qDebug() << p;
    emit itemClicked(moving);
}

void MyGraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (moving) {
//        QPointF p = event->scenePos();
//        moving->moveBy(p.x() - moving_start.x(), p.y() - moving_start.y());
//        moving_start = p;
//        this->update();
        emit itemMoved(moving);
    }
    QGraphicsScene::mouseMoveEvent(event);
}

void MyGraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mouseReleaseEvent(event);
}

void MyGraphicsScene::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    QGraphicsScene::dragMoveEvent(event);
}
