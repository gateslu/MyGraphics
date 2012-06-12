#ifndef MYGRAPHICS_H
#define MYGRAPHICS_H

#include <QMainWindow>
#include <QtGui>
#include <QtCore>
#include "mygraphicsscene.h"
#include "GluGraphicsView.h"
#include "global_types.h"
#include <QGraphicsItem>
#include <QShowEvent>

QT_BEGIN_NAMESPACE
class QAction;
class QToolBar;
class QMenu;
class QUndoStack;
class QUndoView;
QT_END_NAMESPACE
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
    
    void writeItems(QDataStream &out, const QList<QGraphicsItem *> &itemList);

    void setDirty(bool on);
    void loadFile();
    void clear();
    void readItems(QDataStream &in, int offset=0, bool select=false);

    void updateExpandState();

    void addProperty(QtVariantProperty *property, const QString &id);

protected:
    void showEvent(QShowEvent *event);
    void closeEvent(QCloseEvent *event);

private slots:
    void createUndoView();

    void addCustomItem();
    void itemClicked(QGraphicsItem *item);
    void itemMoved(QGraphicsItem *item, const QPointF &oldPosition);
    void selectionChanged();
    void valueChanged(QtProperty *property, const QVariant &value);

    void fileNew();
    void fileOpen();
    bool fileSave();
    bool fileSaveAs();
    void fileExport();
    void filePrint();
    void editCopy();
    void editCut();
    void editPaste();
    void editDelete();
    void editCleanScreen();
    void editAlign();
    void editBringToFront();
    void editSendToBack();
    void viewZoomIn();
    void viewZoomOut();
    void viewRestore();

    void openRecentFile();

private:
    void readSettings();
    void writeSettings();
    bool okToClearData();
    void copyItems(const QList<QGraphicsItem*> &items);
    void selectItems(const QSet<QGraphicsItem*> &items);
    void createActions();
    void createMenusAndToolBars();
    void createConnections();
    void exportImage(const QString &filename);
    void exportSvg(const QString &filename);
    void paintScene(QPainter *painter);
    void populateMenuAndToolBar(QMenu *menu,
                                QToolBar *toolBar,
                                QList<QAction *> actions);
    void populateCoordinates(const Qt::Alignment &alignment,
                             QVector<double> *coordinates,
                             const QList<QGraphicsItem*> &items);
    void animateAlignment(const QList<QGraphicsItem *> &items,
                          const QList<QPointF> &positions);

    void setCurrentFile(const QString &fileName);
    void updateRecentFileActions();
    QString strippedName(const QString &fullFileName);

private:
    Ui::MyGraphics *ui;
    GluGraphicsView *view;
    MyGraphicsScene *scene;
    QPrinter *printer;

    QAction *fileNewAction;
    QAction *fileOpenAction;
    QAction *fileSaveAction;
    QAction *fileSaveAsAction;
    QAction *fileExportAction;
    QAction *filePrintAction;
    QAction *fileQuitAction;
    QAction *editUndoAction;
    QAction *editRedoAction;
    QAction *editCopyAction;
    QAction *editCutAction;
    QAction *editPasteAction;
    QAction *editDeleteAction;
    QAction *editCleanScreenAction;
    QAction *editAlignmentAction;
    QAction *editAlignLeftAction;
    QAction *editAlignRightAction;
    QAction *editAlignTopAction;
    QAction *editAlignBottomAction;
    QAction *editBringToFrontAction;
    QAction *editSendToBackAction;
    QAction *viewZoomInAction;
    QAction *viewZoomOutAction;
    QAction *viewRestoreAction;

    QStringList recentFiles;
    QString curFile;

    enum { MaxRecentFiles = 5 };
    QAction *recentFileActions[MaxRecentFiles];
    QAction *separatorAction;

    QPointF originP;
    class QtVariantPropertyManager *variantManager;
    class QtTreePropertyBrowser *propertyEditor;
    QGraphicsItem *currentItem;
    QMap<QtProperty *, QString> propertyToId;
    QMap<QString, QtVariantProperty *> idToProperty;
    QMap<QString, bool> idToExpanded;

    QUndoStack *undoStack;
    QUndoView *undoView;

    int pasteOffset;
};

#endif // MYGRAPHICS_H
