#include "iText.h"

iText::iText()
{
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
}

QDataStream &operator<<(QDataStream &stream, iText *pItem)
{
    stream << pItem->toPlainText();
    stream << pItem->scenePos();
    stream << pItem->defaultTextColor();
    stream << pItem->font();
    stream << pItem->rotation();
    return stream ;
}

QDataStream &operator>>(QDataStream &stream, iText *pItem)
{
    QString text;
    QPointF pointF;
    QColor color;
    QFont font;
    qreal rotation;

    stream >> text >> pointF >> color >> font >> rotation;

    pItem->setPlainText(text);
    pItem->setPos(pointF);
    pItem->setDefaultTextColor(color);
    pItem->setFont(font);
    pItem->setRotation(rotation);

    return stream;
}
