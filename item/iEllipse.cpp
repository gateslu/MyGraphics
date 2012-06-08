#include "iEllipse.h"

iEllipse::iEllipse()
{
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
}

QDataStream &operator<<(QDataStream &stream, iEllipse *pItem)
{
    stream << pItem->rect();
    stream << pItem->scenePos();
    stream << pItem->brush();
    return stream ;
}

QDataStream &operator>>(QDataStream &stream, iEllipse *pItem)
{
    QRectF rectF;
    QPointF pointF;
    QBrush brush;

    stream >> rectF >> pointF;
    stream >> brush;

    pItem->setRect(rectF);
    pItem->setPos(pointF);
    pItem->setBrush(brush);

    return stream ;
}
