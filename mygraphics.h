#ifndef MYGRAPHICS_H
#define MYGRAPHICS_H

#include <QMainWindow>
#include <QtGui>
#include <QtCore>
#include "mygraphicsscene.h"
#include "GluGraphicsView.h"
#include "global_types.h"
#include <QGraphicsItem>

class QtVariantProperty;
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
    void readItems(QDataStream &in, int offset=0, bool select=false);

    void updateExpandState();

    void addProperty(QtVariantProperty *property, const QString &id);

private slots:
    void on_toolButton_clicked();

    void on_actionOpen_triggered();

    void on_actionSave_triggered();

    void on_actionClear_scene_triggered();

    void on_toolButton_2_clicked();

    void on_actionSendtoback_triggered();

    void on_actionBringtofront_triggered();

    void itemClicked(QGraphicsItem *item);

    void valueChanged(QtProperty *property, const QVariant &value);

    void on_textiTemButton_clicked();

    void on_actionDelete_item_triggered();

    void on_actionCopy_item_triggered();

    void on_actionCut_item_triggered();

    void on_actionPaste_item_triggered();

private:
    void copyItems(const QList<QGraphicsItem*> &items);
    void selectItems(const QSet<QGraphicsItem*> &items);

private:
    Ui::MyGraphics *ui;
    GluGraphicsView *view;
    MyGraphicsScene *scene;

    QPointF originP;
    class QtVariantPropertyManager *variantManager;
    class QtTreePropertyBrowser *propertyEditor;
    QGraphicsItem *currentItem;
    QMap<QtProperty *, QString> propertyToId;
    QMap<QString, QtVariantProperty *> idToProperty;
    QMap<QString, bool> idToExpanded;

    int pasteOffset;
};

#endif // MYGRAPHICS_H
