#ifndef MYGRAPHICSSCENE_H
#define MYGRAPHICSSCENE_H

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneDragDropEvent>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QObject>

class MyGraphicsScene : public QGraphicsScene
{
    Q_OBJECT                        //ʹ���źźͲ�����������ϵĺ궨��
public:
    MyGraphicsScene(QObject *parent);
    void selectedItem(QGraphicsItem *item);

signals:
    void itemClicked(QGraphicsItem *item);
    void itemMoved(QGraphicsItem *item, const QPointF &movedFromPosition);
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
    QGraphicsItem *movingItem;
    QPointF oldPos;
};

#endif // MYGRAPHICSSCENE_H
