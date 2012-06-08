#ifndef GLUGRAPHICSVIEW_H
#define GLUGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QPainter>
#include <QWheelEvent>
#include <cmath>

class GluGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit GluGraphicsView(QWidget *parent = 0);

public slots:
    void zoomIn() { scaleBy(1.1); }
    void zoomOut() { scaleBy(1.0 / 1.1); }

protected:
    void wheelEvent(QWheelEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);

private:
    void scaleBy(double factor);
};

#endif // GLUGRAPHICSVIEW_H
