#ifndef IRECT_H
#define IRECT_H

#include <QGraphicsRectItem>
#include <QPen>

class iRect : public QGraphicsRectItem
{
public:
    iRect();
    enum { Type = UserType + 50 };
    int type() const
      { return Type;}

private:

public:
    friend QDataStream &operator<<(QDataStream &stream, iRect *pItem);
    friend QDataStream &operator>>(QDataStream &stream, iRect *pItem);
};

#endif // IRECT_H
