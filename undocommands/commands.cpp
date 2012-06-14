/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtGui>

#include "commands.h"
#include "item/MyItem.h"
#include <QGraphicsItem>
#include <QDebug>

//! [0]
MoveCommand::MoveCommand(QGraphicsItem *graphicsItem, const QPointF &oldPos,
                         QUndoCommand *parent)
    : QUndoCommand(parent)
{
    myGraphicsItem = graphicsItem;
    newPos = graphicsItem->pos();
    myOldPos = oldPos;
}

MoveCommand::MoveCommand(QMap<QGraphicsItem *, QPointF> itemsnewpos, QMap<QGraphicsItem *, QPointF> itemsoldpos,
                         QUndoCommand *parent)
    : QUndoCommand(parent)
{
    myItemsPos = itemsnewpos;
    newPosList = itemsnewpos.values();
    myOldPosList = itemsoldpos.values();
}
//! [0]

//! [1]
bool MoveCommand::mergeWith(const QUndoCommand *command)
{
    //    const MoveCommand *moveCommand = static_cast<const MoveCommand *>(command);
    //    QGraphicsItem *item = moveCommand->myGraphicsItem;

    //    if (myGraphicsItem != item)
    //        return false;

    //    newPos = item->pos();
    //    //    setText(QObject::tr("Move %1")
    //    //        .arg(createCommandString(myGraphicsItem, newPos)));


    const MoveCommand *moveCommand = static_cast<const MoveCommand *>(command);
    QMap<QGraphicsItem *, QPointF> itemsPos = moveCommand->myItemsPos;

    if (myItemsPos != itemsPos)
        return false;

    newPosList = itemsPos.values();

    setText(QObject::tr("ÒÆ¶¯"));
    return true;
}
//! [1]

//! [2]
void MoveCommand::undo()
{
//    myGraphicsItem->setPos(myOldPos);
//    myGraphicsItem->scene()->update();
    //    setText(QObject::tr("Move %1")
    //        .arg(createCommandString(myGraphicsItem, newPos)));

    for (int i = 0; i < myItemsPos.size(); i++)
    {
        QGraphicsItem *graphicsItem;
        graphicsItem = myItemsPos.keys().at(i);
        graphicsItem->setPos(myOldPosList.at(i));
        graphicsItem->scene()->update();
    }

    setText(QObject::tr("³·ÏúÒÆ¶¯"));
}
//! [2]

//! [3]
void MoveCommand::redo()
{
//    myGraphicsItem->setPos(newPos);
    //    setText(QObject::tr("Move %1")
    //        .arg(createCommandString(myGraphicsItem, newPos)));

    for (int i = 0; i < myItemsPos.size(); i++)
    {
        QGraphicsItem *graphicsItem;
        graphicsItem = myItemsPos.keys().at(i);
        graphicsItem->setPos(newPosList.value(i));
    }

    setText(QObject::tr("ÒÆ¶¯"));
}
//! [3]

//! [4]
DeleteCommand::DeleteCommand(int etype, MyGraphicsScene *scene, QUndoCommand *parent)
    : QUndoCommand(parent)
{
    if (etype == Delete)
    {
        undoActinText = QObject::tr("³·ÏúÉ¾³ý");
        redoActinText = QObject::tr("É¾³ý");
    }
    else if (etype == Cut)
    {
        undoActinText = QObject::tr("³·Ïú¼ôÇÐ");
        redoActinText = QObject::tr("¼ôÇÐ");
    }

    myGraphicsScene = scene;
    QList<QGraphicsItem *> list = myGraphicsScene->selectedItems();
    list.first()->setSelected(false);
    myDiagramItem = static_cast<QGraphicsItem *>(list.first());

    myDiagramItemList = list;

//    setText(QObject::tr("Delete %1")
//        .arg(createCommandString(myDiagramItem, myDiagramItem->pos())));
}
//! [4]

//! [5]
void DeleteCommand::undo()
{
    for (int i = 0; i < myDiagramItemList.size(); i++)
    {
        myGraphicsScene->addItem(myDiagramItemList.at(i));
    }
    myGraphicsScene->update();
    setText(undoActinText);
}
//! [5]

//! [6]
void DeleteCommand::redo()
{
    for (int i = 0; i < myDiagramItemList.size(); i++)
        myGraphicsScene->removeItem(myDiagramItemList.at(i));
    myGraphicsScene->clearSelection();
    setText(redoActinText);
}
//! [6]

//! [7]
AddCommand::AddCommand(QString itemType, MyGraphicsScene *scene,
                       qreal value, QUndoCommand *parent)
    : QUndoCommand(parent)
{
    static int itemCount = 0;

    myGraphicsScene = scene;

    myDiagramItem = 0;
    if (itemType == "iRectB")
    {
        myDiagramItem = new iRect;
    }
    else if (itemType == "iEllipseB")
    {
        myDiagramItem = new iEllipse;
    }
    else if (itemType == "iTextB")
    {
        myDiagramItem = new iText;
    }

    z_value = value;

    initialPosition = QPointF((itemCount * 15) % int(scene->width()),
                              (itemCount * 15) % int(scene->height()));
    scene->update();
    ++itemCount;
//    setText(QObject::tr("·ÅÖÃ"));
//        .arg(createCommandString(myDiagramItem, initialPosition)));
}
//! [7]

AddCommand::~AddCommand()
{
    if (!myDiagramItem->scene())
        delete myDiagramItem;
}

//! [8]
void AddCommand::undo()
{
    myGraphicsScene->removeItem(myDiagramItem);
    myGraphicsScene->update();
    setText(QObject::tr("³·Ïú·ÅÖÃ"));
}
//! [8]

//! [9]
void AddCommand::redo()
{
    myGraphicsScene->addItem(myDiagramItem);
    myDiagramItem->setZValue(z_value);
//    myDiagramItem->setPos(initialPosition);
    myGraphicsScene->clearSelection();
//    myGraphicsScene->selectedItem(myDiagramItem);
    myGraphicsScene->update();
    setText(QObject::tr("·ÅÖÃ"));
}
//! [9]

QString createCommandString(QGraphicsItem *item, const QPointF &pos)
{
    return QObject::tr("%1 at (%2, %3)")
            .arg(item->type())
            .arg(pos.x()).arg(pos.y());
}

//! [10]
PasteCommand::PasteCommand(QList<QGraphicsItem *> pasteList,
                           MyGraphicsScene *graphicsScene,
                           bool select,
                           qreal maxzValue, QUndoCommand *parent)
: QUndoCommand(parent)
{
    itemList = pasteList;
     myGraphicsScene = graphicsScene;
     isSeletctItem = select;
     maxZValue = maxzValue;
}
//! [10]

//! [11]
PasteCommand::~PasteCommand()
{
}
//! [11]

//! [12]
void PasteCommand::undo()
{
    for (int m = 0; m < itemList.size(); m++)
    {
        myGraphicsScene->removeItem(itemList.at(m));
    }
    myGraphicsScene->update();
    setText(QObject::tr("³·ÏúÕ³Ìù"));
}
//! [12]

//! [13]
void PasteCommand::redo()
{
    qreal pastezValue = maxZValue + 1;
    for (int m = 0; m < itemList.size(); m++)
    {
        itemList.at(m)->setZValue(pastezValue++);
        myGraphicsScene->addItem(itemList.at(m));
    }
    if (isSeletctItem)
    {
        myGraphicsScene->clearSelection();
        foreach (QGraphicsItem *item, itemList)
            item->setSelected(true);
    }
    setText(QObject::tr("Õ³Ìù"));
}
//! [13]


