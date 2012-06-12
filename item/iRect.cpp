#include "iRect.h"

iRect::iRect(): QObject(), QGraphicsRectItem()
{
    setFlags(QGraphicsItem::ItemIsSelectable|
             QGraphicsItem::ItemSendsGeometryChanges|
             QGraphicsItem::ItemIsMovable|
             QGraphicsItem::ItemIsFocusable);
    setRect(-55,-55,110,110);
    setBrush(QBrush(QColor(Qt::blue)));
    setPos(QPointF(400,400));
    setRotation(0);
    setZValue(0.0);
}

QDataStream &operator<<(QDataStream &stream, iRect *pItem)
{
    stream << pItem->rect();
    stream << pItem->scenePos();
    stream << pItem->rotation();
    stream << pItem->brush();
    stream << pItem->pen();
    return stream ;
}

QDataStream &operator>>(QDataStream &stream, iRect *pItem)
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

    return stream;
}
