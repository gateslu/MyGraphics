#include "aqp/alt_key.hpp"
#include "aqp/aqp.hpp"
#include "mygraphics.h"
#include "ui_mygraphics.h"
#include "item/MyItem.h"
#include "qtvariantproperty.h"
#include "qttreepropertybrowser.h"
#include <QtGui>
#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QCloseEvent>
#include <QDesktopWidget>
#include <QDockWidget>
#include <QFile>
#include <QFileDialog>
#include <QGraphicsItemGroup>
#include <QGraphicsScene>
#include <QImageWriter>
#include <QMenu>
#include <QMenuBar>
#ifdef ANIMATE_ALIGNMENT
#include <QPropertyAnimation>
#endif
#ifdef ANIMATE_IN_PARALLEL
#include <QParallelAnimationGroup>
#endif
#include <QPrintDialog>
#include <QSettings>
#include <QStatusBar>
#include <QTimer>
#include <QToolBar>
#ifdef USE_STL
#include <algorithm>
#endif
#include <cmath>
#include <limits>
#include <stdlib.h>
#include <QDebug>
#include <QSvgGenerator>
#include "undocommands/commands.h"

const qreal itemzValue = 1.0;
const int OffsetIncrement = 5;
const qint32 MagicNumber = 0x5A93DE5;
const qint16 VersionNumber = 1;
const QString MostRecentFile("MostRecentFile");
const QString MimeType = "application/MyGraphic";

MyGraphics::MyGraphics(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MyGraphics),
    pasteOffset(OffsetIncrement), z_value(itemzValue), maxValue(-10000.0)
{
    ui->setupUi(this);

    undoStack = new QUndoStack(this);

    connect(ui->iRectB, SIGNAL(clicked()), this, SLOT(addCustomItem()));
    connect(ui->iEllipseB, SIGNAL(clicked()), this, SLOT(addCustomItem()));
    connect(ui->iTextB, SIGNAL(clicked()), this, SLOT(addCustomItem()));

    printer = new QPrinter(QPrinter::HighResolution);

    createActions();
    createMenusAndToolBars();
    createConnections();

    createUndoView();

    scene = new MyGraphicsScene(this);
    scene->setSceneRect(QRectF(0, 0, 800, 800));

    originP = QPointF(scene->width()/2, scene->height()/2);

    connect(scene, SIGNAL(itemClicked(QGraphicsItem*)), this, SLOT(itemClicked(QGraphicsItem*)));
    connect(scene, SIGNAL(itemsMoving(QGraphicsItem *)),this, SLOT(itemsMoving(QGraphicsItem *)));
    connect(scene, SIGNAL(itemsMoved(QMap<QGraphicsItem*,QPointF>, QMap<QGraphicsItem*,QPointF>)),
            this, SLOT(itemsMoved(QMap<QGraphicsItem*,QPointF>, QMap<QGraphicsItem*,QPointF>)));
    connect(scene, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));
    connect(this, SIGNAL(firstSelectedItem(QGraphicsItem*)), scene, SLOT(firstSelectedItem(QGraphicsItem*)));

    view = new GluGraphicsView();
    view->setScene(scene);
    setCentralWidget(view);

    currentItem = 0;

    variantManager = new QtVariantPropertyManager(this);

    connect(variantManager, SIGNAL(valueChanged(QtProperty *, const QVariant &)),
            this, SLOT(valueChanged(QtProperty *, const QVariant &)));

    QtVariantEditorFactory *variantFactory = new QtVariantEditorFactory(this);


    QDockWidget *dock = new QDockWidget(tr("属性"),this);
    addDockWidget(Qt::RightDockWidgetArea, dock);

    propertyEditor = new QtTreePropertyBrowser(dock);
    propertyEditor->setFactoryForManager(variantManager, variantFactory);
    dock->setWidget(propertyEditor);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    readSettings();
}

MyGraphics::~MyGraphics()
{
    //    delete printer;
    //    delete ui;
}

//操作历史
void MyGraphics::createUndoView()
{
    undoView = new QUndoView(undoStack);
    undoView->setWindowTitle(tr("Command List"));
    undoView->show();
    undoView->setAttribute(Qt::WA_QuitOnClose, false);
}

void MyGraphics::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);
    //    QSettings settings;
    //    QString filename = settings.value(MostRecentFile).toString();
    //    if (filename.isEmpty() || filename == tr("Unnamed"))
    QTimer::singleShot(0, this, SLOT(fileNew()));
    //    else {
    //        setWindowFilePath(filename);
    //        QTimer::singleShot(0, this, SLOT(loadFile()));
    //    }
}

void MyGraphics::closeEvent(QCloseEvent *event)
{
    if (okToClearData()) {
        writeSettings();
        delete printer;
        delete ui;
        event->accept();
    }
    else
        event->ignore();
}

//新建文件
void MyGraphics::fileNew()
{
    if (!okToClearData())
        return;
    clear();
    this->setWindowFilePath(tr("Unnamed"));
    setDirty(false);
}

//打开文件
void MyGraphics::fileOpen()
{
    if (!okToClearData())
        return;
    const QString &filename = QFileDialog::getOpenFileName(this,
                                                           tr("%1 - Open").arg(QApplication::applicationName()),
                                                           ".", tr("Glu Graphics (*.gg);;All file(*)(*.*)"));
    if (filename.isEmpty())
        return;
    setWindowFilePath(filename);
    loadFile();
}

//添加item
void MyGraphics::addCustomItem()
{
    QToolButton *itemButton = qobject_cast<QToolButton *>(sender());

    QString itemType = itemButton->objectName();

    //    qDebug() << z_value;
    QUndoCommand *addCommand = new AddCommand(itemType, scene ,z_value);
    undoStack->push(addCommand);
    z_value++;

    //    QGraphicsItem *item = 0;
    //    if (itemType == "iRectB")
    //    {
    //        item = new iRect;
    //    }
    //    else if (itemType == "iEllipseB")
    //    {
    //        item = new iEllipse;
    //    }
    //    else if (itemType == "iTextB")
    //    {
    //        item = new iText;
    //    }

    //    scene->addItem(item);
    //    scene->selectedItem(item);
    setDirty(true);
}


bool MyGraphics::okToClearData()
{
    if (isWindowModified())
        return AQP::okToClearData(&MyGraphics::fileSave, this,
                                  tr("Unsaved changes"), tr("Save unsaved changes?"));
    return true;
}

//输出视图
void MyGraphics::fileExport()
{
    QString suffixes = AQP::filenameFilter(tr("Bitmap image"),
                                           QImageWriter::supportedImageFormats());
    suffixes += tr(";;Vector image (*.svg)");
    const QString filename = QFileDialog::getSaveFileName(this,
                                                          tr("%1 - Export").arg(QApplication::applicationName()),
                                                          ".", suffixes);
    if (filename.isEmpty())
        return;
    if (filename.toLower().endsWith(".svg"))
        exportSvg(filename);
    else
        exportImage(filename);
}

//输出scene的内容
void MyGraphics::paintScene(QPainter *painter)
{
    //    bool showGrid = viewShowGridAction->isChecked();
    //    if (showGrid)
    //        viewShowGrid(false);
    QList<QGraphicsItem*> items = scene->selectedItems();
    scene->clearSelection();

    scene->render(painter);

    //    if (showGrid)
    //        viewShowGrid(true);
    foreach (QGraphicsItem *item, items)
        item->setSelected(true);
    selectionChanged();
}

//输出为svg
void MyGraphics::exportSvg(const QString &filename)
{
    QSvgGenerator svg;
    svg.setFileName(filename);
    svg.setSize(printer->paperSize(QPrinter::Point).toSize());
    {
        QPainter painter(&svg);
        paintScene(&painter);
    }
    statusBar()->showMessage(tr("Exported %1").arg(filename),
                             1000);
}

//输出为图片
void MyGraphics::exportImage(const QString &filename)
{
    QImage image(printer->paperSize(QPrinter::Point).toSize(),
                 QImage::Format_ARGB32);
    {
        QPainter painter(&image);
        painter.setRenderHints(QPainter::Antialiasing|
                               QPainter::TextAntialiasing);
        paintScene(&painter);
    }
    if (image.save(filename))
        statusBar()->showMessage(tr("Exported %1").arg(filename),
                                 2000);
    else
        AQP::warning(this, tr("Error"), tr("Failed to export: %1")
                     .arg(filename));
}

//打印
void MyGraphics::filePrint()
{
    QPrintDialog dialog(printer);
    if (dialog.exec()) {
        {
            QPainter painter(printer);
            paintScene(&painter);
        }
        statusBar()->showMessage(tr("Printed %1")
                                 .arg(windowFilePath()), 2000);
    }
}

//复制
void MyGraphics::editCopy()
{
    QList<QGraphicsItem*> items = scene->selectedItems();
    if (items.isEmpty())
        return;
    pasteOffset = OffsetIncrement;
    copyItems(items);
}

//剪切
void MyGraphics::editCut()
{
    QList<QGraphicsItem*> items = scene->selectedItems();
    if (items.isEmpty())
        return;
    copyItems(items);
//    QListIterator<QGraphicsItem*> i(items);
//    while (i.hasNext()) {
//#if QT_VERSION >= 0x040600
//        QScopedPointer<QGraphicsItem> item(i.next());
//        scene->removeItem(item.data());
//#else
//        QGraphicsItem *item = i.next();
//        scene->removeItem(item);
//        delete item;
//#endif
//    }
    QUndoCommand *deleteCommand = new DeleteCommand(Cut,scene);
    undoStack->push(deleteCommand);

    setDirty(true);
}


//复制item实现
void MyGraphics::copyItems(const QList<QGraphicsItem*> &items)
{
    QByteArray copiedItems;
    QDataStream out(&copiedItems, QIODevice::WriteOnly);
    writeItems(out, items);
    QMimeData *mimeData = new QMimeData;
    mimeData->setData(MimeType, copiedItems);
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setMimeData(mimeData);
}

//粘贴
void MyGraphics::editPaste()
{
    QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();
    if (!mimeData)
        return;

    if (mimeData->hasFormat(MimeType)) {
        QByteArray copiedItems = mimeData->data(MimeType);
        QDataStream in(&copiedItems, QIODevice::ReadOnly);
//        readItems(in, pasteOffset);
        undoStack->push(new PasteCommand(readItems(in, pasteOffset), scene, true, maxValue));
        pasteOffset += OffsetIncrement;
    }
    else
        return;
    setDirty(true);
}

//创建Action
void MyGraphics::createActions()
{
    fileNewAction = new QAction(QIcon(":images/filenew.png"),
                                tr("New..."), this);
    fileNewAction->setShortcuts(QKeySequence::New);
    fileOpenAction = new QAction(QIcon(":images/fileopen.png"),
                                 tr("Open..."), this);
    fileOpenAction->setShortcuts(QKeySequence::Open);
    fileSaveAction = new QAction(QIcon(":images/filesave.png"),
                                 tr("Save"), this);
    fileSaveAction->setShortcuts(QKeySequence::Save);
    fileSaveAsAction = new QAction(QIcon(":images/filesave.png"),
                                   tr("Save As..."), this);
    fileSaveAsAction->setShortcuts(QKeySequence::SaveAs);
    fileExportAction = new QAction(QIcon(":images/fileexport.png"),
                                   tr("Export..."), this);
    filePrintAction = new QAction(QIcon(":images/fileprint.png"),
                                  tr("Print..."), this);

    for (int i = 0; i < MaxRecentFiles; ++i) {
        recentFileActions[i] = new QAction(this);
        recentFileActions[i]->setVisible(false);
        connect(recentFileActions[i], SIGNAL(triggered()),
                this, SLOT(openRecentFile()));
    }

    fileQuitAction = new QAction(QIcon(":images/filequit.png"),
                                 tr("Quit"), this);
    fileQuitAction->setShortcuts(QKeySequence::Quit);

    editUndoAction = undoStack->createUndoAction(this, tr("&Undo"));
    editUndoAction->setIcon(QIcon(":images/undo.png"));
    editUndoAction->setShortcuts(QKeySequence::Undo);

    editRedoAction = undoStack->createRedoAction(this, tr("&Redo"));
    editRedoAction->setIcon(QIcon(":images/redo.png"));
    editRedoAction->setShortcuts(QKeySequence::Redo);

    editCopyAction = new QAction(QIcon(":images/editcopy.png"), tr("&Copy"),
                                 this);
    editCopyAction->setShortcuts(QKeySequence::Copy);
    editCutAction = new QAction(QIcon(":images/editcut.png"), tr("Cu&t"),
                                this);
    editCutAction->setShortcuts(QKeySequence::Cut);
    editPasteAction = new QAction(QIcon(":images/editpaste.png"),
                                  tr("&Paste"), this);
    editPasteAction->setShortcuts(QKeySequence::Paste);

    editDeleteAction = new QAction(QIcon(":images/editdelete.png"),
                                   tr("&Delete"), this);
    editDeleteAction->setShortcuts(QKeySequence::Delete);
    editCleanScreenAction  = new QAction(QIcon(":images/editclean.png"),
                                         tr("CleanScreen"), this);

    editAlignmentAction = new QAction(QIcon(":images/align-left.png"),
                                      tr("Alignment"), this);
    editAlignmentAction->setData(Qt::AlignLeft);
    editAlignLeftAction = new QAction(QIcon(":images/align-left.png"),
                                      tr("Left"), this);
    editAlignLeftAction->setData(Qt::AlignLeft);
    editAlignRightAction = new QAction(QIcon(":images/align-right.png"),
                                       tr("Right"), this);
    editAlignRightAction->setData(Qt::AlignRight);
    editAlignTopAction = new QAction(QIcon(":images/align-top.png"),
                                     tr("Top"), this);
    editAlignTopAction->setData(Qt::AlignTop);
    editAlignBottomAction = new QAction(QIcon(":images/align-bottom.png"),
                                        tr("Bottom"), this);
    editAlignBottomAction->setData(Qt::AlignBottom);

    editBringToFrontAction = new QAction(QIcon(":images/bringtofront.png"),
                                         tr("BringToFront"), this);
    editSendToBackAction = new QAction(QIcon(":images/sendtoback.png"),
                                       tr("SentToBack"), this);

    viewZoomInAction = new QAction(QIcon(":images/zoom_in.png"),
                                   tr("Zoom In"), this);
    viewZoomInAction->setShortcut(tr("+"));
    viewZoomOutAction = new QAction(QIcon(":images/zoom_out.png"),
                                    tr("Zoom Out"), this);
    viewZoomOutAction->setShortcut(tr("-"));
    viewRestoreAction = new QAction(QIcon(":images/restore.png"),
                                    tr("View Restore"), this);
}

//创建menu和toolbar
void MyGraphics::createMenusAndToolBars()
{
    QAction *separator = 0;
    setUnifiedTitleAndToolBarOnMac(true);

    QMenu *fileMenu = menuBar()->addMenu(tr("File"));
    QToolBar *fileToolBar = addToolBar(tr("File"));
    populateMenuAndToolBar(fileMenu, fileToolBar, QList<QAction*>()
                           << fileNewAction << fileOpenAction << fileSaveAction
                           << fileExportAction << separator << filePrintAction);
    fileMenu->insertAction(fileExportAction, fileSaveAsAction);
    fileMenu->addSeparator();
    separatorAction = fileMenu->addSeparator();
    for (int i = 0; i < MaxRecentFiles; ++i)
        fileMenu->addAction(recentFileActions[i]);
    separatorAction = fileMenu->addSeparator();
    fileMenu->addAction(fileQuitAction);

    QMenu *alignmentMenu = new QMenu(tr("Align"), this);
    foreach (QAction *action, QList<QAction*>()
             << editAlignLeftAction << editAlignRightAction
             << editAlignTopAction << editAlignBottomAction)
        alignmentMenu->addAction(action);
    editAlignmentAction->setMenu(alignmentMenu);

    QMenu *editMenu = menuBar()->addMenu(tr("Edit"));
    QToolBar *editToolBar = addToolBar(tr("Edit"));
    populateMenuAndToolBar(editMenu, editToolBar, QList<QAction*>()
                           << editUndoAction << editRedoAction << separator
                           << editCopyAction << editCutAction << editPasteAction
                           << separator << editDeleteAction << editCleanScreenAction << separator
                           << editAlignmentAction << editBringToFrontAction << editSendToBackAction);

    QMenu *viewMenu = menuBar()->addMenu(tr("View"));
    QToolBar *viewToolBar = addToolBar(tr("View"));
    populateMenuAndToolBar(viewMenu, viewToolBar, QList<QAction*>()
                           << viewZoomInAction << viewZoomOutAction << viewRestoreAction);

    AQP::accelerateMenu(menuBar());
}

//创建连接
void MyGraphics::createConnections()
{
    connect(fileNewAction, SIGNAL(triggered()),
            this, SLOT(fileNew()));
    connect(fileOpenAction, SIGNAL(triggered()),
            this, SLOT(fileOpen()));
    connect(fileSaveAction, SIGNAL(triggered()),
            this, SLOT(fileSave()));
    connect(fileSaveAsAction, SIGNAL(triggered()),
            this, SLOT(fileSaveAs()));
    connect(fileExportAction, SIGNAL(triggered()),
            this, SLOT(fileExport()));
    connect(filePrintAction, SIGNAL(triggered()),
            this, SLOT(filePrint()));
    connect(fileQuitAction, SIGNAL(triggered()),
            this, SLOT(close()));

    connect(editCopyAction, SIGNAL(triggered()),
            this, SLOT(editCopy()));
    connect(editCutAction, SIGNAL(triggered()),
            this, SLOT(editCut()));
    connect(editPasteAction, SIGNAL(triggered()),
            this, SLOT(editPaste()));
    connect(editDeleteAction, SIGNAL(triggered()),
            this, SLOT(editDelete()));
    connect(editCleanScreenAction, SIGNAL(triggered()),
            this, SLOT(editCleanScreen()));

    foreach (QAction *action, QList<QAction*>()
             << editAlignmentAction << editAlignLeftAction
             << editAlignRightAction << editAlignTopAction
             << editAlignBottomAction)
        connect(action, SIGNAL(triggered()), this, SLOT(editAlign()));
    connect(editBringToFrontAction, SIGNAL(triggered()),
            this, SLOT(editBringToFront()));
    connect(editSendToBackAction, SIGNAL(triggered()),
            this, SLOT(editSendToBack()));

    connect(viewZoomInAction, SIGNAL(triggered()),
            this, SLOT(viewZoomIn()));
    connect(viewZoomOutAction, SIGNAL(triggered()),
            this, SLOT(viewZoomOut()));
    connect(viewRestoreAction, SIGNAL(triggered()),
            this, SLOT(viewRestore()));

}

void MyGraphics::populateCoordinates(const Qt::Alignment &alignment,
                                     QVector<double> *coordinates,
                                     const QList<QGraphicsItem *> &items)
{
    QListIterator<QGraphicsItem*> i(items);
    while (i.hasNext()) {
        QRectF rect = i.next()->sceneBoundingRect();
        switch (alignment) {
        case Qt::AlignLeft:
            coordinates->append(rect.x()); break;
        case Qt::AlignRight:
            coordinates->append(rect.x() + rect.width()); break;
        case Qt::AlignTop:
            coordinates->append(rect.y()); break;
        case Qt::AlignBottom:
            coordinates->append(rect.y() + rect.height()); break;
        }
    }
}

//item对齐
void MyGraphics::editAlign()
{
    QAction *action = qobject_cast<QAction*>(sender());
    if (!action)
        return;

    Qt::Alignment alignment = static_cast<Qt::Alignment>(
                action->data().toInt());
    if (action != editAlignmentAction) {
        editAlignmentAction->setData(action->data());
        editAlignmentAction->setIcon(action->icon());
    }

    QList<QGraphicsItem*> items = scene->selectedItems();
    QVector<double> coordinates;
    populateCoordinates(alignment, &coordinates, items);
    double offset;
    if (alignment == Qt::AlignLeft || alignment == Qt::AlignTop)
        offset = *std::min_element(coordinates.constBegin(),
                                   coordinates.constEnd());
    else
        offset = *std::max_element(coordinates.constBegin(),
                                   coordinates.constEnd());


    QList<QPointF> positions;
    if (alignment == Qt::AlignLeft || alignment == Qt::AlignRight) {
        for (int i = 0; i < items.count(); ++i)
            positions << items.at(i)->pos() +
                         QPointF(offset - coordinates.at(i), 0);
    }
    else {
        for (int i = 0; i < items.count(); ++i)
            positions << items.at(i)->pos() +
                         QPointF(0, offset - coordinates.at(i));
    }

    animateAlignment(items, positions);
    setDirty(true);
}

//创建populateMenuAndToolBar
void MyGraphics::populateMenuAndToolBar(QMenu *menu,
                                        QToolBar *toolBar, QList<QAction*> actions)
{
#ifdef Q_WS_MAC
    toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
#endif
    foreach (QAction *action, actions) {
        if (!action) {
            menu->addSeparator();
            toolBar->addSeparator();
        }
        else {
            menu->addAction(action);
            toolBar->addAction(action);
        }
    }
}

void MyGraphics::animateAlignment(const QList<QGraphicsItem*> &items,
                                  const QList<QPointF> &positions)
{
    int duration = ((qApp->keyboardModifiers() & Qt::ShiftModifier)
                    != Qt::ShiftModifier) ? 1000 : 5000;

#ifdef ANIMATE_IN_PARALLEL
    QParallelAnimationGroup *group = new QParallelAnimationGroup;
#endif
    for (int i = 0; i < items.count(); ++i) {
        QObject *object = dynamic_cast<QObject*>(items.at(i));
        if (!object)
            continue;
        QPropertyAnimation *animation = new QPropertyAnimation(
                    object, "pos", this);
        animation->setDuration(duration);
        animation->setEasingCurve(QEasingCurve::InOutBack);
#ifdef ANIMATE_IN_PARALLEL
        animation->setStartValue(items.at(i)->pos());
        animation->setEndValue(positions.at(i));
        group->addAnimation(animation);
#else
        animation->setKeyValueAt(0.0, items.at(i)->pos());
        animation->setKeyValueAt(1.0, positions.at(i));
        animation->start(QAbstractAnimation::DeleteWhenStopped);
#endif
    }
#ifdef ANIMATE_IN_PARALLEL
    group->start(QAbstractAnimation::DeleteWhenStopped);
#endif
}

void MyGraphics::setCurrentFile(const QString &fileName)
{
    curFile = fileName;
    //    setWindowModified(false);

    QString shownName = tr("Unnamed");
    if (!curFile.isEmpty()) {
        shownName = strippedName(curFile);
        recentFiles.removeAll(curFile);
        recentFiles.prepend(curFile);
        updateRecentFileActions();
    }
}

//更新recentfile列
void MyGraphics::updateRecentFileActions()
{
    QMutableStringListIterator i(recentFiles);
    while (i.hasNext()) {
        if (!QFile::exists(i.next()))
            i.remove();
    }

    for (int j = 0; j < MaxRecentFiles; ++j) {
        if (j < recentFiles.count()) {
            QString text = tr("&%1 %2")
                    .arg(j + 1)
                    .arg(strippedName(recentFiles[j]));
            recentFileActions[j]->setText(text);
            recentFileActions[j]->setData(recentFiles[j]);
            recentFileActions[j]->setVisible(true);
        } else {
            recentFileActions[j]->setVisible(false);
        }
    }
    separatorAction->setVisible(!recentFiles.isEmpty());
}

QString MyGraphics::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

//打开最近打开的文件
void MyGraphics::openRecentFile()
{
    if (!okToClearData())
        return;

    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
    {
        setWindowFilePath(action->data().toString());
        loadFile();
    }

}

//读取设置
void MyGraphics::readSettings()
{
    QSettings settings("Fish.", "Glu Graphics");

    recentFiles = settings.value("recentFiles").toStringList();
    updateRecentFileActions();
}

//保存设置
void MyGraphics::writeSettings()
{
    QSettings settings("Fish.", "Glu Graphics");

    settings.setValue("recentFiles", recentFiles);
}

//更新窗口修改标志*
void MyGraphics::setDirty(bool on)
{
    setWindowModified(on);
}

//保存
bool MyGraphics::fileSave()
{
    const QString filename = windowFilePath();
    if (filename.isEmpty() || filename == tr("Unnamed"))
        return fileSaveAs();
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly))
        return false;
    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_4_5);
    writeItems(out, scene->items());
    file.close();
    setDirty(false);
    setCurrentFile(filename);
    return true;
}

//另存为
bool MyGraphics::fileSaveAs()
{
    QString filename = QFileDialog::getSaveFileName(this,
                                                    tr("%1 - Save As").arg(QApplication::applicationName()),
                                                    ".", tr("Glu Graphics (*.gg)"));
    if (filename.isEmpty())
        return false;
    if (!filename.toLower().endsWith(".gg"))
        filename += ".gg";
    setWindowFilePath(filename);
    return fileSave();
}

//保存item
void MyGraphics::writeItems(QDataStream &out,
                            const QList<QGraphicsItem*> &itemList)
{
    out << MagicNumber << VersionNumber;
    int sceneItemsize = 0;
    for(int i=0;i<itemList.size();i++)
    {
        QGraphicsItem *pParentItem= itemList.at(i)->parentItem();
        if(pParentItem != 0)
            continue;
        sceneItemsize++ ;
    }
    out << sceneItemsize;
    for(int i=0;i<itemList.size();i++)
    {
        //        QGraphicsItem *pParentItem= itemList.at(i)->parentItem();
        //        if(pParentItem != 0)
        //            continue;
        qint32 type = static_cast<qint32>(itemList.at(i)->type());
        switch (type)
        {
        case MG_TYPE_IRECT:
        {
            out << iRect::Type;
            iRect *piRect = dynamic_cast<iRect*>(itemList.at(i));
            out << piRect;
            out << piRect->zValue();
            break;
        }
        case MG_TYPE_IELLIPSE:
        {
            out << iEllipse::Type;
            iEllipse *piEllipse = dynamic_cast<iEllipse*>(itemList.at(i));
            out << piEllipse;
            out << piEllipse->zValue();
            break;
        }
        case MG_TYPE_ITEXT:
        {
            out << iText::Type;
            iText *piText = dynamic_cast<iText*>(itemList.at(i));
            out << piText;
            out << piText->zValue();
            break;
        }
        default:
            Q_ASSERT(false);
        }
    }
}

//读取文件
void MyGraphics::loadFile()
{
    QFile file(windowFilePath());
    file.open(QIODevice::ReadOnly);
    QDataStream in;
    in.setDevice(&file);

    in.setVersion(QDataStream::Qt_4_5);
    clear();
    readItems(in);
    setDirty(false);
    setCurrentFile(windowFilePath());
}

//清空scene
void MyGraphics::clear()
{
    undoStack->clear();
    scene->clear();
    z_value = 1.0;
    maxValue = -10000.0;
    viewRestore();
}

//读取文件中的item
void MyGraphics::readItems(QDataStream &in)
{
    qint32 itemType;
    QGraphicsItem *item = 0;
    qint32 magicNumber;
    qint16 versionNumber;

    in >> magicNumber >> versionNumber;

    int sceneItemsize = 0;
    in >> sceneItemsize;

    while (sceneItemsize > 0 && !in.atEnd())
    {
        sceneItemsize-- ;
        in >> itemType;
        switch (itemType)
        {
        case MG_TYPE_IRECT:
        {
            iRect *it = new iRect();
            in >> it;
            scene->addItem(it);
            qreal zvalue;
            in >> zvalue;
            it->setZValue(zvalue);
            item = it;
            break;
        }
        case MG_TYPE_IELLIPSE:
        {
            iEllipse *it = new iEllipse();
            in >> it;
            scene->addItem(it);
            qreal zvalue;
            in >> zvalue;
            it->setZValue(zvalue);
            item = it;
            break;
        }
        case MG_TYPE_ITEXT:
        {
            iText *it = new iText();
            in >> it;
            scene->addItem(it);
            qreal zvalue;
            in >> zvalue;
            it->setZValue(zvalue);
            item = it;
            break;
        }
        }
    }
}

//剪切板中的item
QList<QGraphicsItem*> MyGraphics::readItems(QDataStream &in, int offset)
{
    QList<QGraphicsItem*> itList;
    QSet<QGraphicsItem*> items;
    qint32 itemType;
    QGraphicsItem *item = 0;
    qint32 magicNumber;
    qint16 versionNumber;

    in >> magicNumber >> versionNumber;

    int sceneItemsize = 0;
    in >> sceneItemsize;

    int count = scene->items().size();

    qDebug() << "count =" << count;

    maxValue = -10000.0;

    if (count > 0)
        foreach (QGraphicsItem *item, scene->items())
        {
            maxzValue(item->zValue());
        }
    else
        maxValue = 0.0;


    qDebug() << "maxValue" << maxValue;

    while (sceneItemsize > 0 && !in.atEnd())
    {
        sceneItemsize--;
        in >> itemType;
        switch (itemType)
        {
        case MG_TYPE_IRECT:
        {
            iRect *it = new iRect();
            in >> it;
//            scene->addItem(it);
            qreal zvalue;
            in >> zvalue;
            it->setZValue(zvalue);
            item = it;
            break;
        }
        case MG_TYPE_IELLIPSE:
        {
            iEllipse *it = new iEllipse();
            in >> it;
//            scene->addItem(it);
            qreal zvalue;
            in >> zvalue;
            it->setZValue(zvalue);
            item = it;
            break;
        }
        case MG_TYPE_ITEXT:
        {
            iText *it = new iText();
            in >> it;
//            scene->addItem(it);
            qreal zvalue;
            in >> zvalue;
            it->setZValue(zvalue);
            item = it;
            break;
        }
        }
        itList.append(item);
        if (item) {
            item->moveBy(offset, offset);
            items << item;
            item = 0;
        }
        //        qDebug() << "itList" << itList.at(0);
        z_value++;
    }
    //    qDebug() << "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^";
    //    qDebug() << "before" << itList;
    //    qDebug() << "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^";
    setItemszValue(itList);
    //    qDebug() << "after" << itList;
    //    qDebug() << "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^";

    return itList;
//    qreal pastezValue = maxValue + 1;
//    for (int m = 0; m < itList.size(); m++)
//    {
//        itList.at(m)->setZValue(pastezValue++);
//        scene->addItem(itList.at(m));
//    }
//    if (select)
//        selectItems(items);
}


//当前最大的zValue
void MyGraphics::maxzValue(qreal mxValue)
{
    maxValue = (mxValue > maxValue ? mxValue : maxValue);
}

//冒泡法排序
void MyGraphics::setItemszValue(QList<QGraphicsItem *> &itList)
{
    int i ,j;                //定义2个变量
    int itemCount = itList.size()-1;
    for (i = 0; i < itemCount; i++)      //进行N轮排序
    {
        for(j = 0; j < itemCount - i; j++) //每轮进行N-i次交换
            if(itList.at(j)->zValue() > itList.at(j + 1)->zValue())
            {
                itList.swap(j, j+1);   //大的沉底，小的上浮
            }
    }
}

//展开
void MyGraphics::updateExpandState()
{
    QList<QtBrowserItem *> list = propertyEditor->topLevelItems();
    QListIterator<QtBrowserItem *> it(list);
    while (it.hasNext()) {
        QtBrowserItem *item = it.next();
        QtProperty *prop = item->property();
        idToExpanded[propertyToId[prop]] = propertyEditor->isExpanded(item);
    }
}

//增加属性
void MyGraphics::addProperty(QtVariantProperty *property, const QString &id)
{
    propertyToId[property] = id;
    idToProperty[id] = property;
    QtBrowserItem *item = propertyEditor->addProperty(property);
    if (idToExpanded.contains(id))
        propertyEditor->setExpanded(item, idToExpanded[id]);
}

//item点击效果实现
void MyGraphics::itemClicked(QGraphicsItem *item)
{
    updateExpandState();

    QMap<QtProperty *, QString>::ConstIterator itProp = propertyToId.constBegin();
    while (itProp != propertyToId.constEnd()) {
        delete itProp.key();
        itProp++;
    }
    propertyToId.clear();
    idToProperty.clear();

    currentItem = item;
    if (!currentItem) {
        //        deleteAction->setEnabled(false);
        return;
    }

    //    deleteAction->setEnabled(true);

    QtVariantProperty *property;

    property = variantManager->addProperty(QVariant::Double, tr("Position X"));
    property->setAttribute(QLatin1String("minimum"), 0);
    property->setAttribute(QLatin1String("maximum"), scene->width());
    property->setValue(item->pos().x());
    addProperty(property, QLatin1String("xpos"));

    property = variantManager->addProperty(QVariant::Double, tr("Position Y"));
    property->setAttribute(QLatin1String("minimum"), 0);
    property->setAttribute(QLatin1String("maximum"), scene->height());
    property->setValue(item->pos().y());
    addProperty(property, QLatin1String("ypos"));

    property = variantManager->addProperty(QVariant::Double, tr("Position Z"));
    property->setAttribute(QLatin1String("minimum"), -20000);
    property->setAttribute(QLatin1String("maximum"), 20000);
    property->setValue(item->zValue());
    addProperty(property, QLatin1String("zpos"));

    property = variantManager->addProperty(QVariant::Double, tr("Angle"));
    property->setAttribute(QLatin1String("minimum"), -360.0);
    property->setAttribute(QLatin1String("maximum"), 360.0);
    property->setValue(item->rotation());
    addProperty(property, QLatin1String("angle"));

    if (item->type() == MG_TYPE_IRECT)
    {
        iRect *i = (iRect *)item;

        property = variantManager->addProperty(QVariant::Color, tr("Brush Color"));
        property->setValue(i->brush().color());
        addProperty(property, QLatin1String("brush"));

        property = variantManager->addProperty(QVariant::Double, tr("Pen Width"));
        property->setAttribute(QLatin1String("minimum"), 1.0);
        property->setAttribute(QLatin1String("maximum"), 10.0);
        property->setValue(i->pen().widthF());
        addProperty(property, QLatin1String("pen_width"));

        property = variantManager->addProperty(QVariant::Color, tr("Pen Color"));
        property->setValue(i->pen().color());
        addProperty(property, QLatin1String("pen"));

        property = variantManager->addProperty(QVariant::SizeF, tr("Size"));
        property->setValue(i->rect().size());
        addProperty(property, QLatin1String("size"));
    }
    else if (item->type() == MG_TYPE_IELLIPSE)
    {
        iEllipse *i = (iEllipse *)item;

        property = variantManager->addProperty(QVariant::Color, tr("Brush Color"));
        property->setValue(i->brush().color());
        addProperty(property, QLatin1String("brush"));

        property = variantManager->addProperty(QVariant::Double, tr("Pen Width"));
        property->setAttribute(QLatin1String("minimum"), 1.0);
        property->setAttribute(QLatin1String("maximum"), 10.0);
        property->setValue(i->pen().widthF());
        addProperty(property, QLatin1String("pen_width"));

        property = variantManager->addProperty(QVariant::Color, tr("Pen Color"));
        property->setValue(i->pen().color());
        addProperty(property, QLatin1String("pen"));

        property = variantManager->addProperty(QVariant::SizeF, tr("Size"));
        property->setValue(i->rect().size());
        addProperty(property, QLatin1String("size"));
    }
    else if (item->type() == MG_TYPE_ITEXT)
    {
        iText *i = (iText *)item;

        property = variantManager->addProperty(QVariant::Color, tr("Color"));
        property->setValue(i->defaultTextColor());
        addProperty(property, QLatin1String("color"));

        property = variantManager->addProperty(QVariant::String, tr("Text"));
        property->setValue(i->toPlainText());
        addProperty(property, QLatin1String("text"));

        property = variantManager->addProperty(QVariant::Font, tr("Font"));
        property->setValue(i->font());
        addProperty(property, QLatin1String("font"));
    }
    //else if (item->rtti() == QtCanvasItem::Rtti_Line) {
    //        QtCanvasLine *i = (QtCanvasLine *)item;

    //        property = variantManager->addProperty(QVariant::Color, tr("Pen Color"));
    //        property->setValue(i->pen().color());
    //        addProperty(property, QLatin1String("pen"));

    //        property = variantManager->addProperty(QVariant::Point, tr("Vector"));
    //        property->setValue(i->endPoint());
    //        addProperty(property, QLatin1String("endpoint"));
    //    }
}

//选取
void MyGraphics::selectionChanged()
{
    QList<QGraphicsItem*> items = scene->selectedItems();
    if (items.count() == 1) {
        itemClicked(items.at(0));
        //        qDebug() << "1 item";
        emit firstSelectedItem(items.at(0));
    }
    else if (items.count() == 0)
    {
        //        qDebug() << "0 item";
        itemClicked(0);
    }
}

//移动item
void MyGraphics::itemsMoving(QGraphicsItem *item)
{
    if (item != currentItem)
        return;

    disconnect(variantManager, SIGNAL(valueChanged(QtProperty *, const QVariant &)),
               this, SLOT(valueChanged(QtProperty *, const QVariant &)));

    variantManager->setValue(idToProperty[QLatin1String("xpos")], item->pos().x());
    variantManager->setValue(idToProperty[QLatin1String("ypos")], item->pos().y());

    connect(variantManager, SIGNAL(valueChanged(QtProperty *, const QVariant &)),
            this, SLOT(valueChanged(QtProperty *, const QVariant &)));
    setDirty(true);
    scene->update();
}

//停止移动
void MyGraphics::itemsMoved(QMap<QGraphicsItem *, QPointF> itemsnewpos, QMap<QGraphicsItem*,QPointF> itemsoldpos)
{
    undoStack->push(new MoveCommand(itemsnewpos, itemsoldpos));
}

//修改item属性
void MyGraphics::valueChanged(QtProperty *property, const QVariant &value)
{
    if (!propertyToId.contains(property))
        return;

    if (!currentItem)
        return;

    QString id = propertyToId[property];
    if (id == QLatin1String("xpos"))
    {
        currentItem->setX(value.toDouble());
    }
    else if (id == QLatin1String("ypos"))
    {
        currentItem->setY(value.toDouble());
    }
    else if (id == QLatin1String("zpos"))
    {
        currentItem->setZValue(value.toDouble());
    }
    else if (id == QLatin1String("angle"))
    {
        currentItem->setRotation(value.toDouble());
    }
    else if (id == QLatin1String("text"))
    {
        if (currentItem->type() == MG_TYPE_ITEXT)
        {
            QGraphicsTextItem *i = (QGraphicsTextItem *)currentItem;  //textitem
            i->setPlainText(qVariantValue<QString>(value));
        }
    }
    else if (id == QLatin1String("brush"))
    {
        if (currentItem->type() == MG_TYPE_IRECT)
        {
            iRect *i = (iRect *)currentItem;
            QBrush b = i->brush();
            b.setColor(qVariantValue<QColor>(value));
            i->setBrush(b);
        }
        else if(currentItem->type() == MG_TYPE_IELLIPSE)
        {
            iEllipse *i = (iEllipse *)currentItem;
            QBrush b = i->brush();
            b.setColor(qVariantValue<QColor>(value));
            i->setBrush(b);
        }
    }
    else if (id == QLatin1String("color"))
    {
        if (currentItem->type() == MG_TYPE_ITEXT)
        {
            iText *i = (iText *)currentItem;
            i->setDefaultTextColor(qVariantValue<QColor>(value));
        }
    }
    else if (id == QLatin1String("pen_width"))
    {
        if (currentItem->type() == MG_TYPE_IRECT)
        {
            iRect *i = (iRect *)currentItem;
            QPen p = i->pen();
            p.setWidthF(value.toDouble());
            i->setPen(p);
        }
        else if(currentItem->type() == MG_TYPE_IELLIPSE)
        {
            iEllipse *i = (iEllipse *)currentItem;
            QPen p = i->pen();
            p.setWidthF(value.toDouble());
            i->setPen(p);
        }
    }
    else if (id == QLatin1String("pen"))
    {
        if (currentItem->type() == MG_TYPE_IRECT)
        {
            iRect *i = (iRect *)currentItem;
            QPen p = i->pen();
            p.setColor(qVariantValue<QColor>(value));
            i->setPen(p);
        }
        else if(currentItem->type() == MG_TYPE_IELLIPSE)
        {
            iEllipse *i = (iEllipse *)currentItem;
            QPen p = i->pen();
            p.setColor(qVariantValue<QColor>(value));
            i->setPen(p);
        }
        else if(currentItem->type() == MG_TYPE_ILINE)
        {
        }
    }
    else if (id == QLatin1String("font"))
    {
        if (currentItem->type() == MG_TYPE_ITEXT)
        {
            iText *i = (iText *)currentItem;
            i->setFont(qVariantValue<QFont>(value));
        }
    }
    //    else if (id == QLatin1String("endpoint"))
    //    {
    //        if (currentItem->type() == MG_TYPE_ILINE)
    //        {
    ////            QtCanvasLine *i = (QtCanvasLine *)currentItem;
    ////            QPoint p = qVariantValue<QPoint>(value);
    ////            i->setPoints(i->startPoint().x(), i->startPoint().y(), p.x(), p.y());
    //        }
    //    }
    else if (id == QLatin1String("size"))
    {
        if (currentItem->type() == MG_TYPE_IRECT)
        {
            iRect *i = (iRect *)currentItem;
            QSizeF s = qVariantValue<QSizeF>(value);
            QPointF p = i->rect().topLeft();
            QRectF r(p, s);
            i->setRect(r);
        }
        else if(currentItem->type() == MG_TYPE_IELLIPSE)
        {
            iEllipse *i = (iEllipse *)currentItem;
            QSizeF s = qVariantValue<QSizeF>(value);
            QPointF p = i->rect().topLeft();
            QRectF r(p, s);
            i->setRect(r);
        }
    }
    setDirty(true);
    scene->update();
}

//重新选中item
void MyGraphics::selectItems(const QSet<QGraphicsItem *> &items)
{
    scene->clearSelection();
    foreach (QGraphicsItem *item, items)
        item->setSelected(true);
}

//清空屏幕
void MyGraphics::editCleanScreen()
{
    //QMessageBox::Yes	0x00004000
    int ret = QMessageBox::warning(this,
                                   "Clean Screen",
                                   "Do you want yo clean all item?",
                                   QMessageBox::Yes,
                                   QMessageBox::No);
    if (ret == 0x00004000)
    {
        clear();
    }
}

//item移到前方
void MyGraphics::editBringToFront()
{
    if (scene->selectedItems().isEmpty())
        return;

    QGraphicsItem *selectedItem = scene->selectedItems().first();
    QList<QGraphicsItem *> overlapItems = selectedItem->collidingItems();

    qreal zValue = 0;
    foreach (QGraphicsItem *item, overlapItems)
    {
        if (item->zValue() >= zValue)
            zValue = item->zValue() + 1.0;
    }
    selectedItem->setZValue(zValue);
    itemClicked(selectedItem);
}

//item移到后方
void MyGraphics::editSendToBack()
{
    if (scene->selectedItems().isEmpty())
        return;

    QGraphicsItem *selectedItem = scene->selectedItems().first();
    QList<QGraphicsItem *> overlapItems = selectedItem->collidingItems();

    qreal zValue = 0;
    foreach (QGraphicsItem *item, overlapItems)
    {
        if (item->zValue() <= zValue)
            zValue = item->zValue() - 1.0;
    }
    selectedItem->setZValue(zValue);
    itemClicked(selectedItem);
}

void MyGraphics::viewZoomIn()
{
    view->zoomIn();
}

void MyGraphics::viewZoomOut()
{
    view->zoomOut();
}

void MyGraphics::viewRestore()
{
    view->restore();
}

//删除选中的item
void MyGraphics::editDelete()
{
    QList<QGraphicsItem*> items = scene->selectedItems();
    if (items.isEmpty())
        return;

    QUndoCommand *deleteCommand = new DeleteCommand(Delete, scene);
    undoStack->push(deleteCommand);
    //    QListIterator<QGraphicsItem*> i(items);
    //    while (i.hasNext()) {
    //#if QT_VERSION >= 0x040600
    //        QScopedPointer<QGraphicsItem> item(i.next());
    //        scene->removeItem(item.data());
    //#else
    //        QGraphicsItem *item = i.next();
    //        scene->removeItem(item);
    //        delete item;
    //#endif
    //    }
    itemClicked(0);
    setDirty(true);             //窗口标题增加*,用来标记文件已被修改
}
