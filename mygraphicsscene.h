#ifndef MYGRAPHICSSCENE_H
#define MYGRAPHICSSCENE_H

#include <QGraphicsSceneMouseEvent>
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

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
    QGraphicsItem *moving;
};

#endif // MYGRAPHICSSCENE_H
