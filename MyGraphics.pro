#-------------------------------------------------
#
# Project created by QtCreator 2012-06-01T12:50:47
#
#-------------------------------------------------

QT       += core gui svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MyGraphics
TEMPLATE = app

include(src/qtpropertybrowser.pri)

SOURCES += main.cpp\
        mygraphics.cpp \
    mygraphicsscene.cpp \
    item/iRect.cpp \
    item/iEllipse.cpp \
    item/iText.cpp \
    GluGraphicsView.cpp \
    aqp/kuhn_munkres.cpp \
    aqp/aqp.cpp \
    aqp/alt_key.cpp \
    undocommands/commands.cpp

HEADERS  += mygraphics.h \
    mygraphicsscene.h \
    global_types.h \
    item/iRect.h \
    item/iEllipse.h \
    item/MyItem.h \
    item/iText.h \
    GluGraphicsView.h \
    aqp/kuhn_munkres.hpp \
    aqp/aqp.hpp \
    aqp/alt_key.hpp \
    undocommands/commands.h

FORMS    += mygraphics.ui

RESOURCES += \
    qrc.qrc \
    aqp/aqp.qrc

OTHER_FILES += \
    aqp/mime.types \
    aqp/aqp.pro
