#-------------------------------------------------
#
# Project created by QtCreator 2012-06-01T12:50:47
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MyGraphics
TEMPLATE = app

include(src/qtpropertybrowser.pri)

SOURCES += main.cpp\
        mygraphics.cpp \
    mygraphicsscene.cpp \
    item/iRect.cpp \
    item/iEllipse.cpp \
    item/iText.cpp

HEADERS  += mygraphics.h \
    mygraphicsscene.h \
    global_types.h \
    item/iRect.h \
    item/iEllipse.h \
    item/MyItem.h \
    item/iText.h

FORMS    += mygraphics.ui
