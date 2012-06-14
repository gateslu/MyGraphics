#include "mygraphicsscene.h"
#include <QDebug>

MyGraphicsScene::MyGraphicsScene(QObject *parent)
    : QGraphicsScene(parent), isPressing(false)
{
    moving = 0;
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
    itemsOldPos.clear();

//    qDebug() << "size of l" << l.size();

    if (!l.isEmpty())
    {
        for (int i = 0; i < l.size(); i++)
        {
            itemsOldPos.insert(l.at(i), l.at(i)->pos());
            //            qDebug() << itemsOldPos;
        }
    }
    else
    {
        moving = 0;
    }
    //    emit itemClicked(moving);
}

void MyGraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (moving && isPressing) {
        //        QPointF p = event->scenePos();
        //        moving->moveBy(p.x() - moving_start.x(), p.y() - moving_start.y());
        //        moving_start = p;
        //        this->update();
        //        qDebug() << moving;
        emit itemsMoving(moving);
    }
    QGraphicsScene::mouseMoveEvent(event);
}

void MyGraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() != Qt::LeftButton)
        return;

    isPressing = false;

    if (moving != 0) {
        if (oldPos != moving->pos())
        {
            QList<QGraphicsItem*> l = this->selectedItems();
            itemsNewPos.clear();
            for (int i = 0; i < l.size(); i++)
            {
                itemsNewPos.insert(l.at(i), l.at(i)->pos());
                //            qDebug() << itemsOldPos;
            }
            emit itemsMoved(itemsNewPos, itemsOldPos);
        }
    }

    QGraphicsScene::mouseReleaseEvent(event);
}

void MyGraphicsScene::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    QGraphicsScene::dragMoveEvent(event);
}

void MyGraphicsScene::firstSelectedItem(QGraphicsItem *item)
{
    moving = 0;
    moving = item;
//    qDebug() << moving;
    oldPos = moving->pos();
}
