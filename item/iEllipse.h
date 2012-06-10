#ifndef IELLIPSE_H
#define IELLIPSE_H

#include <QGraphicsEllipseItem>
#include <QObject>
#include <QPen>

class iEllipse : public QObject, public QGraphicsEllipseItem
{
    Q_OBJECT
    Q_PROPERTY(QPointF pos READ pos WRITE setPos)
public:
    explicit iEllipse();
    enum { Type = UserType + 51 };
    int type() const
      { return Type;}

public:
    friend QDataStream &operator<<(QDataStream &stream, iEllipse *pItem);
    friend QDataStream &operator>>(QDataStream &stream, iEllipse *pItem);
};

#endif // IELLIPSE_H
