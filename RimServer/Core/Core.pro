#-------------------------------------------------
#
# Project created by QtCreator 2018-01-08T17:23:21
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Core
TEMPLATE = app

DESTDIR = ../Bin

SOURCES += main.cpp\
        widget.cpp \
    rsingleton.cpp

HEADERS  += widget.h \
    rsingleton.h \
    constants.h

FORMS    += widget.ui

win32-msvc2013{
    LIBS += E:/Git/build-RimClient-Qt570vs64-Debug/Bin/Util.lib
    LIBS+= ../Bin/network.lib
}

win32-g++{
    LIBS+= -LE:/Git/build-RimClient-Qt570vs64-Debug/Bin/ -lUtil
    QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder
}

unix{
    LIBS+= -L../Bin/ -lNetwork -lUtil
}

INCLUDEPATH += $$PWD/../
INCLUDEPATH += $$PWD/../../RimClient/
