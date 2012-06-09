#include "GluGraphicsView.h"
#include <QApplication>
#include <QDebug>

GluGraphicsView::GluGraphicsView(QWidget *parent) :
    QGraphicsView(parent)
{
    setDragMode(RubberBandDrag);            //可多选
    setBackgroundBrush(QBrush(Qt::lightGray));
    setRenderHints(QPainter::Antialiasing|
                   QPainter::TextAntialiasing);
}

void GluGraphicsView::wheelEvent(QWheelEvent *event)
{
    if (QApplication::keyboardModifiers () == Qt::ControlModifier)
    {
//        qDebug() << event->delta();
        //        scaleBy(std::pow(4.0 / 3.0, (-event->delta() / 240.0))); //原方法
        QTransform transform;
        qreal tmp;
        tmp = (event->delta()>0 ? this->transform().m22()+0.1 : this->transform().m22()-0.1);
        if (tmp > 3.0 || tmp < 0.1)         //缩放范围0.1-3.0
            return;
        transform.scale(tmp, tmp);
        setTransform(transform);
    }
    else {
        QGraphicsView::wheelEvent(event);
    }
//    else if (QApplication::keyboardModifiers () == Qt::AltModifier)
//    {
//        event->delta()>0 ? this->scroll(0, 50) : this->scroll(0, -50);
//    }
//    else
//    {
//        event->delta()>0 ? this->scroll(50, 0) : this->scroll(-50, 0);
//    }
}

void GluGraphicsView::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(event->button()==Qt::MidButton)
    {
//        resetCachedContent () ;
//        resetMatrix () ;
        resetTransform();           //重置缩放
    }
    QGraphicsView::mouseDoubleClickEvent(event);
}

void GluGraphicsView::scaleBy(double factor)
{
    scale(factor, factor);
}
