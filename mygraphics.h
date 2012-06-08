#ifndef MYGRAPHICS_H
#define MYGRAPHICS_H

#include <QMainWindow>
#include <QtGui>
#include <QtCore>
#include "mygraphicsscene.h"
#include "global_types.h"

class QtProperty;

namespace Ui {
class MyGraphics;
}

class MyGraphics : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MyGraphics(QWidget *parent = 0);
    ~MyGraphics();

    bool fileSave();
    bool fileSaveAs();
    
    void writeItems(QDataStream &out, const QList<QGraphicsItem *> &itemList);

    void setDirty(bool on);
    void loadFile();
    void clear();
    void readItems(QDataStream &in);

    void updateExpandState();

    void addProperty(QtProperty *property, const QString &id);

private slots:
    void on_toolButton_clicked();

    void on_actionOpen_triggered();

    void on_actionSave_triggered();

    void on_actionClear_scene_triggered();

    void on_toolButton_2_clicked();

    void on_actionSendtoback_triggered();

    void on_actionBringtofront_triggered();

    void itemClicked(QGraphicsItem *item);

    void valueChanged(QtProperty *property, double value);
//    void valueChanged(QtProperty *property, const QString &value);
//    void valueChanged(QtProperty *property, const QColor &value);
//    void valueChanged(QtProperty *property, const QFont &value);
//    void valueChanged(QtProperty *property, const QPoint &value);
//    void valueChanged(QtProperty *property, const QSize &value);

private:
    Ui::MyGraphics *ui;
    QGraphicsView *view;
    MyGraphicsScene *scene;

    QGraphicsItem *currentItem;
    QMap<QtProperty *, QString> propertyToId;
    QMap<QString, QtProperty *> idToProperty;
    QMap<QString, bool> idToExpanded;

    class QtDoublePropertyManager *doubleManager;
    class QtStringPropertyManager *stringManager;
    class QtColorPropertyManager *colorManager;
    class QtFontPropertyManager *fontManager;
    class QtPointFPropertyManager *pointfManager;
    class QtSizeFPropertyManager *sizefManager;

    class QtTreePropertyBrowser *propertyEditor;
};

#endif // MYGRAPHICS_H
