#ifndef ITEXT_H
#define ITEXT_H

#include <QGraphicsTextItem>
#include <QFont>

class iText : public QGraphicsTextItem
{
public:
    iText();
    enum { Type = UserType + 52 };
    int type() const
      { return Type;}

public:
    friend QDataStream &operator<<(QDataStream &stream, iText *pItem);
    friend QDataStream &operator>>(QDataStream &stream, iText *pItem);
};

#endif // ITEXT_H
