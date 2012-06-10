#include "iEllipse.h"

iEllipse::iEllipse(): QObject(), QGraphicsEllipseItem()
{
    setFlags(QGraphicsItem::ItemIsSelectable|
             QGraphicsItem::ItemSendsGeometryChanges|
             QGraphicsItem::ItemIsMovable|
             QGraphicsItem::ItemIsFocusable);
}

QDataStream &operator<<(QDataStream &stream, iEllipse *pItem)
{
    stream << pItem->rect();
    stream << pItem->scenePos();
    stream << pItem->brush();
    stream << pItem->rotation();
    return stream ;
}

QDataStream &operator>>(QDataStream &stream, iEllipse *pItem)
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

    return stream ;
}
