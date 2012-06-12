#include "iEllipse.h"

iEllipse::iEllipse(): QObject(), QGraphicsEllipseItem()
{
    setFlags(QGraphicsItem::ItemIsSelectable|
             QGraphicsItem::ItemSendsGeometryChanges|
             QGraphicsItem::ItemIsMovable|
             QGraphicsItem::ItemIsFocusable);
    setRect(0,0,100,100);
    setBrush(QBrush(QColor(Qt::darkGreen)));
    //    ellipse->setOpacity(0.7);
    setPos(QPointF(400,400));
}

QDataStream &operator<<(QDataStream &stream, iEllipse *pItem)
{
    stream << pItem->rect();
    stream << pItem->scenePos();
    stream << pItem->rotation();
    stream << pItem->brush();
    stream << pItem->pen();
    return stream ;
}

QDataStream &operator>>(QDataStream &stream, iEllipse *pItem)
{
    QRectF rectF;
    QPointF pointF;
    qreal rotation;
    QBrush brush;
    QPen pen;

    stream >> rectF >> pointF >> rotation;
    stream >> brush >> pen;

    pItem->setRect(rectF);
    pItem->setPos(pointF);
    pItem->setRotation(rotation);
    pItem->setBrush(brush);
    pItem->setPen(pen);

    return stream ;
}
