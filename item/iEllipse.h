#ifndef IELLIPSE_H
#define IELLIPSE_H

#include <QGraphicsEllipseItem>
#include <QPen>
#include <QGraphicsItem>

class iEllipse : public QGraphicsEllipseItem
{
public:
    iEllipse();
    enum { Type = UserType + 51 };
    int type() const
      { return Type;}

public:
    friend QDataStream &operator<<(QDataStream &stream, iEllipse *pItem);
    friend QDataStream &operator>>(QDataStream &stream, iEllipse *pItem);
};

#endif // IELLIPSE_H
