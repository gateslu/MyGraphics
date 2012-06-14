#ifndef MYGRAPHICSSCENE_H
#define MYGRAPHICSSCENE_H

#include <QtCore>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneDragDropEvent>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QObject>
#include <QList>
#include <QMap>

class MyGraphicsScene : public QGraphicsScene
{
    Q_OBJECT                        //使用信号和槽类所必须加上的宏定义
public:
    MyGraphicsScene(QObject *parent);
    void selectedItem(QGraphicsItem *item);

signals:
    void itemClicked(QGraphicsItem *item);
    void itemsMoving(QGraphicsItem *item);
    void itemsMoved(QMap<QGraphicsItem*, QPointF> itemsnewpos,
                    QMap<QGraphicsItem*, QPointF> itemsoldpos);
    void itemStopMoving(bool released);

public slots:
    void firstSelectedItem(QGraphicsItem *item);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void dragMoveEvent(QGraphicsSceneDragDropEvent *event);

private:
    QList<QGraphicsItem*> itemlist;
    QMap<QGraphicsItem*, QPointF> itemsOldPos;
    QMap<QGraphicsItem*, QPointF> itemsNewPos;
    QGraphicsItem *moving;
    QPointF moving_start;
    bool isPressing;
    QPointF oldPos;
};

#endif // MYGRAPHICSSCENE_H
