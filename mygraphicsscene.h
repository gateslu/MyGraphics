#ifndef MYGRAPHICSSCENE_H
#define MYGRAPHICSSCENE_H

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneDragDropEvent>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QObject>

class MyGraphicsScene : public QGraphicsScene
{
    Q_OBJECT                        //使用信号和槽类所必须加上的宏定义
public:
    MyGraphicsScene(QObject *parent);
    void selectedItem(QGraphicsItem *item);

signals:
    void itemClicked(QGraphicsItem *item);
    void itemMoved(QGraphicsItem *item);
    void itemStopMoving(bool released);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void dragMoveEvent(QGraphicsSceneDragDropEvent *event);

private:
    QGraphicsItem *moving;
    QPointF moving_start;
    bool isPressing;
};

#endif // MYGRAPHICSSCENE_H
