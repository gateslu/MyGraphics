#include "mygraphicsscene.h"
#include <QDebug>

MyGraphicsScene::MyGraphicsScene(QObject *parent)
    : QGraphicsScene(parent), isPressing(false)
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
    isPressing = true;
    QList<QGraphicsItem*> l = this->selectedItems();
    moving = 0;
    if (!l.isEmpty())
    {
        moving = l.first();
        oldPos = moving->pos();
    }
    emit itemClicked(moving);
}

void MyGraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (moving && isPressing) {
//        QPointF p = event->scenePos();
//        moving->moveBy(p.x() - moving_start.x(), p.y() - moving_start.y());
//        moving_start = p;
//        this->update();
        emit itemMoved(moving, oldPos);
    }
    QGraphicsScene::mouseMoveEvent(event);
}

void MyGraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    isPressing = false;
    QGraphicsScene::mouseReleaseEvent(event);
}

void MyGraphicsScene::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    QGraphicsScene::dragMoveEvent(event);
}
