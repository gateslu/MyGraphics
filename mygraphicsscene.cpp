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
    QList<QGraphicsItem*> l = this->selectedItems();
    moving = 0;
//    qDebug() << l.size();
    if (!l.isEmpty())
    {
        moving = l.first();
    }
    emit itemClicked(moving);

//    moving_start = p;
}

void MyGraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mouseMoveEvent(event);
}

void MyGraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mouseReleaseEvent(event);
}
