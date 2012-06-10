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
    setRotation(45);
    setZValue(0.0);
}

QDataStream &operator<<(QDataStream &stream, iRect *pItem)
{
    stream << pItem->rect();
    stream << pItem->scenePos();
    stream << pItem->brush();
    stream << pItem->rotation();
    return stream ;
}

QDataStream &operator>>(QDataStream &stream, iRect *pItem)
{
    QRectF rectF;
    QPointF pointF;
    QBrush brush;
    qreal rotation;

    stream >> rectF >> pointF;
    stream >> brush >> rotation;

    pItem->setRect(rectF);
    pItem->setPos(pointF);
    pItem->setBrush(brush);
    pItem->setRotation(rotation);

    return stream;
}
