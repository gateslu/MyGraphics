#include "mygraphics.h"
#include "ui_mygraphics.h"
#include "item/MyItem.h"
#include "src/qtpropertymanager.h"
#include "src/qteditorfactory.h"
#include "src/qttreepropertybrowser.h"
#include <QtGui>
#include <QDebug>

const int OffsetIncrement = 5;
const qint32 MagicNumber = 0x5A93DE5;
const qint16 VersionNumber = 1;
const QString MostRecentFile("MostRecentFile");
const QString MimeType = "application/vnd.qtrac.pagedesigner";

MyGraphics::MyGraphics(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MyGraphics)
{
    ui->setupUi(this);
    scene = new MyGraphicsScene(this);
    scene->setSceneRect(QRectF(0, 0, 800, 800));

    connect(scene, SIGNAL(itemClicked(QGraphicsItem*)), this, SLOT(itemClicked(QGraphicsItem*)));

    view = new QGraphicsView;
    view->setScene(scene);
    view->setDragMode(QGraphicsView::RubberBandDrag);     //可多选
    view->setBackgroundBrush(QBrush(Qt::white));
    setCentralWidget(view);

    currentItem = 0;

    doubleManager = new QtDoublePropertyManager(this);
    stringManager = new QtStringPropertyManager(this);
    colorManager = new QtColorPropertyManager(this);
    fontManager = new QtFontPropertyManager(this);
    pointfManager = new QtPointFPropertyManager(this);
    sizefManager = new QtSizeFPropertyManager(this);

    connect(doubleManager, SIGNAL(valueChanged(QtProperty *, double)),
                this, SLOT(valueChanged(QtProperty *, double)));
    connect(stringManager, SIGNAL(valueChanged(QtProperty *, const QString &)),
                this, SLOT(valueChanged(QtProperty *, const QString &)));
    connect(colorManager, SIGNAL(valueChanged(QtProperty *, const QColor &)),
                this, SLOT(valueChanged(QtProperty *, const QColor &)));
    connect(fontManager, SIGNAL(valueChanged(QtProperty *, const QFont &)),
                this, SLOT(valueChanged(QtProperty *, const QFont &)));
    connect(pointfManager, SIGNAL(valueChanged(QtProperty *, const QPoint &)),
                this, SLOT(valueChanged(QtProperty *, const QPoint &)));
    connect(sizefManager, SIGNAL(valueChanged(QtProperty *, const QSize &)),
                this, SLOT(valueChanged(QtProperty *, const QSize &)));

    QtDoubleSpinBoxFactory *doubleSpinBoxFactory = new QtDoubleSpinBoxFactory(this);
    QtCheckBoxFactory *checkBoxFactory = new QtCheckBoxFactory(this);
    QtSpinBoxFactory *spinBoxFactory = new QtSpinBoxFactory(this);
    QtLineEditFactory *lineEditFactory = new QtLineEditFactory(this);
    QtEnumEditorFactory *comboBoxFactory = new QtEnumEditorFactory(this);

    QDockWidget *dock = new QDockWidget(this);
    addDockWidget(Qt::RightDockWidgetArea, dock);

    propertyEditor = new QtTreePropertyBrowser(dock);
    propertyEditor->setFactoryForManager(doubleManager, doubleSpinBoxFactory);
    propertyEditor->setFactoryForManager(stringManager, lineEditFactory);
    propertyEditor->setFactoryForManager(colorManager->subIntPropertyManager(), spinBoxFactory);
    propertyEditor->setFactoryForManager(fontManager->subIntPropertyManager(), spinBoxFactory);
    propertyEditor->setFactoryForManager(fontManager->subBoolPropertyManager(), checkBoxFactory);
    propertyEditor->setFactoryForManager(fontManager->subEnumPropertyManager(), comboBoxFactory);
    propertyEditor->setFactoryForManager(pointfManager->subDoublePropertyManager(), doubleSpinBoxFactory);
    propertyEditor->setFactoryForManager(sizefManager->subDoublePropertyManager(), doubleSpinBoxFactory);
    dock->setWidget(propertyEditor);
}

MyGraphics::~MyGraphics()
{
    delete ui;
}

void MyGraphics::setDirty(bool on)
{
    setWindowModified(on);
}

//保存
bool MyGraphics::fileSave()
{
    //    const QString filename = windowFilePath();
    //    if (filename.isEmpty() || filename == tr("Unnamed"))
    //        return fileSaveAs();

    QString filenameas = QFileDialog::getSaveFileName(this,
                                                      tr("%1 - Save As").arg(QApplication::applicationName()),
                                                      "test.pd", tr("Page Designer (*.pd)"));
    if (filenameas.isEmpty())
        return false;
    if (!filenameas.toLower().endsWith(".pd"))
        filenameas += ".pd";
    setWindowFilePath(filenameas);

    const QString filename = windowFilePath();
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly))
        return false;
    QDataStream out(&file);
    out << MagicNumber << VersionNumber;
    out.setVersion(QDataStream::Qt_4_5);
    writeItems(out, scene->items());
    file.close();
    setDirty(false);
    return true;
}

//另存为
bool MyGraphics::fileSaveAs()
{
    QString filename = QFileDialog::getSaveFileName(this,
                                                    tr("%1 - Save As").arg(QApplication::applicationName()),
                                                    ".", tr("Page Designer (*.pd)"));
    if (filename.isEmpty())
        return false;
    if (!filename.toLower().endsWith(".pd"))
        filename += ".pd";
    setWindowFilePath(filename);
    return fileSave();
}

//保存item
void MyGraphics::writeItems(QDataStream &out,
                            const QList<QGraphicsItem*> &itemList)
{
    //    foreach (QGraphicsItem *item, itemList) {
    //        //        if (item == gridGroup || item->group() == gridGroup)
    //        //            continue;
    //        qint32 type = static_cast<qint32>(item->type());
    //        out << type;
    //        switch (type) {
    //        case MG_TYPE_IRECT:
    //            out << dynamic_cast<iRect*>(item);
    //            qreal zvalue;
    //            zvalue = item->zValue();
    //            out << zvalue;
    //            break;
    //        default: Q_ASSERT(false);
    //        }
    //    }

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
            piRect->setZValue(0.0-i);
            out << piRect->zValue();
            break;
        }
        case MG_TYPE_IELLIPSE:
        {
            out << iEllipse::Type;
            iEllipse *piEllipse = dynamic_cast<iEllipse*>(itemList.at(i));
            out << piEllipse;
            piEllipse->setZValue(0.0-i);
            out << piEllipse->zValue();
            break;
        }
        default:
            Q_ASSERT(false);
        }
    }
}

//打开文件
void MyGraphics::on_actionOpen_triggered()
{
    const QString &filename = QFileDialog::getOpenFileName(this,
                                                           tr("%1 - Open").arg(QApplication::applicationName()),
                                                           ".", tr("Page Designer (*.pd)"));
    if (filename.isEmpty())
        return;
    setWindowFilePath(filename);
    loadFile();
}

//保存文件
void MyGraphics::on_actionSave_triggered()
{
    fileSave();
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
}

//清空scene
void MyGraphics::clear()
{
//    QList<QGraphicsItem*> list = scene->items();
//    for (int i = 0; i < list.size(); ++i) {
//        scene->removeItem(list.at(i));
//    }
    scene->clear();
}

//读取文件中的item
void MyGraphics::readItems(QDataStream &in)
{
    qint32 itemType;

    qint32 magicNumber;
    qint16 versionNumber;

    in >> magicNumber >> versionNumber;

    int sceneItemsize = 0;
    in >> sceneItemsize;
    qDebug() << sceneItemsize;

    while (sceneItemsize > 0 && !in.atEnd())
    {
        sceneItemsize-- ;
        in >> itemType;
        switch (itemType)
        {
        case MG_TYPE_IRECT:
        {
            iRect *irect = new iRect();
            in >> irect;
            scene->addItem(irect);
            qreal zvalue;
            in >> zvalue;
            qDebug() << zvalue;
            irect->setZValue(zvalue);
            break;
        }
        case MG_TYPE_IELLIPSE:
        {
            iEllipse *iellipse = new iEllipse();
            in >> iellipse;
            scene->addItem(iellipse);
            qreal zvalue;
            in >> zvalue;
            qDebug() << zvalue;
            iellipse->setZValue(zvalue);
            break;
        }
        }
    }
}

//清空scene
void MyGraphics::on_actionClear_scene_triggered()
{
    clear();
}

//item移到后方
void MyGraphics::on_actionSendtoback_triggered()
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

//item移到前方
void MyGraphics::on_actionBringtofront_triggered()
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

void MyGraphics::addProperty(QtProperty *property, const QString &id)
{
    propertyToId[property] = id;
    idToProperty[id] = property;
    QtBrowserItem *item = propertyEditor->addProperty(property);
    if (idToExpanded.contains(id))
        propertyEditor->setExpanded(item, idToExpanded[id]);
}

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

    //    qDebug() << item;
    //    qDebug() << "scenewidth:" << scene->width();
    //    qDebug() << "sceneheight:" << scene->height();
    //    qDebug() << "x:" << item->scenePos().x();
    //    qDebug() << "y:" << item->scenePos().y();
    //    qDebug() << "z:" << item->scenePos().y();

//    deleteAction->setEnabled(true);

    QtProperty *property;

    property = doubleManager->addProperty(tr("Position X"));
    doubleManager->setRange(property, 0-scene->width()/2, scene->width()/2);
    doubleManager->setValue(property, item->pos().x());
    addProperty(property, QLatin1String("xpos"));

    property = doubleManager->addProperty(tr("Position Y"));
    doubleManager->setRange(property, 0-scene->height()/2, scene->height()/2);
    doubleManager->setValue(property, item->pos().y());
    addProperty(property, QLatin1String("ypos"));

    property = doubleManager->addProperty(tr("Position Z"));
    doubleManager->setRange(property, -10000, 10000);
    doubleManager->setValue(property, item->zValue());
    addProperty(property, QLatin1String("zpos"));

    if (item->type() == MG_TYPE_IRECT) {
        iRect *i = (iRect *)item;

        property = colorManager->addProperty(tr("Brush Color"));
        colorManager->setValue(property, i->brush().color());
        addProperty(property, QLatin1String("brush"));

        property = colorManager->addProperty(tr("Pen Color"));
        colorManager->setValue(property, i->pen().color());
        addProperty(property, QLatin1String("pen"));

        property = sizefManager->addProperty(tr("Size"));
        sizefManager->setValue(property, i->rect().size());
        addProperty(property, QLatin1String("size"));
    } else if (item->type() == MG_TYPE_IELLIPSE) {
        iEllipse *i = (iEllipse *)item;

        property = colorManager->addProperty(tr("Brush Color"));
        colorManager->setValue(property, i->brush().color());
        addProperty(property, QLatin1String("brush"));

        property = colorManager->addProperty(tr("Pen Color"));
        colorManager->setValue(property, i->pen().color());
        addProperty(property, QLatin1String("pen"));

        property = sizefManager->addProperty(tr("Size"));
        sizefManager->setValue(property, i->rect().size());
        addProperty(property, QLatin1String("size"));
    }
//    else if (item->rtti() == QtCanvasItem::Rtti_Ellipse) {
//        QtCanvasEllipse *i = (QtCanvasEllipse *)item;

//        property = colorManager->addProperty(tr("Brush Color"));
//        colorManager->setValue(property, i->brush().color());
//        addProperty(property, QLatin1String("brush"));

//        property = sizeManager->addProperty(tr("Size"));
//        sizeManager->setValue(property, QSize(i->width(), i->height()));
//        sizeManager->setRange(property, QSize(0, 0), QSize(1000, 1000));
//        addProperty(property, QLatin1String("size"));
//    } else if (item->rtti() == QtCanvasItem::Rtti_Text) {
//        QtCanvasText *i = (QtCanvasText *)item;

//        property = colorManager->addProperty(tr("Color"));
//        colorManager->setValue(property, i->color());
//        addProperty(property, QLatin1String("color"));

//        property = stringManager->addProperty(tr("Text"));
//        stringManager->setValue(property, i->text());
//        addProperty(property, QLatin1String("text"));

//        property = fontManager->addProperty(tr("Font"));
//        fontManager->setValue(property, i->font());
//        addProperty(property, QLatin1String("font"));
//    }
}

void MyGraphics::valueChanged(QtProperty *property, double value)
{
    if (!propertyToId.contains(property))
        return;

    if (!currentItem)
        return;

    QString id = propertyToId[property];
    if (id == QLatin1String("xpos")) {
        currentItem->setX(value);
        qDebug() << "xpos" << value;
    } else if (id == QLatin1String("ypos")) {
        currentItem->setY(value);
    } else if (id == QLatin1String("zpos")) {
        currentItem->setZValue(value);
    }
    scene->update();
}

void MyGraphics::on_toolButton_clicked()
{
    iRect *rect = new iRect;
    rect->setRect(350,350,110,110);
    rect->setBrush(QBrush(QColor(Qt::blue)));
    scene->addItem(rect);
    scene->selectedItem(rect);
    rect->setZValue(0.0);
    qDebug() << rect->zValue();
}

void MyGraphics::on_toolButton_2_clicked()
{
    iEllipse *ellipse = new iEllipse;
    ellipse->setRect(350,350,100,100);
    ellipse->setBrush(QBrush(QColor(Qt::darkGreen)));
//    ellipse->setOpacity(0.7);
    scene->addItem(ellipse);
    scene->selectedItem(ellipse);
    ellipse->setZValue(0.0);
    qDebug() << ellipse->zValue();
}
