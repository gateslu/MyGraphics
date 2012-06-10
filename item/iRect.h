#ifndef IRECT_H
#define IRECT_H

#include <QGraphicsRectItem>
#include <QObject>
#include <QPen>

class iRect : public QObject, public QGraphicsRectItem
{
    Q_OBJECT
    Q_PROPERTY(QPointF pos READ pos WRITE setPos)
public:
    explicit iRect();
    enum { Type = UserType + 50 };
    int type() const
      { return Type;}

private:

public:
    friend QDataStream &operator<<(QDataStream &stream, iRect *pItem);
    friend QDataStream &operator>>(QDataStream &stream, iRect *pItem);
};

#endif // IRECT_H
