#-------------------------------------------------
#
# Project created by QtCreator 2018-01-08T17:23:21
#
#-------------------------------------------------

QT       += core gui
QT       += sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RimServer
TEMPLATE = app

RC_ICONS += $${TARGET}.ico

CONFIG(debug, debug|release) {
#  TARGET = $$join(TARGET,,,d)           #为debug版本生成的文件增加d的后缀

  contains(TEMPLATE, "lib") {
    DESTDIR = ../Lib
    DLLDESTDIR = ../Bin
  } else {
    DESTDIR = ../Bin
  }
} else {
  contains(TEMPLATE, "lib") {
    DESTDIR = ../Lib
    DLLDESTDIR = ../Bin
  } else {
    DESTDIR = ../Bin
  }
}

SOURCES += main.cpp\
        widget.cpp \
    rsingleton.cpp \
    sql/sqlprocess.cpp \
    sql/databasemanager.cpp \
    sql/database.cpp \
    sql/datatable.cpp \
    thread/recvtextprocessthread.cpp

HEADERS  += widget.h \
    rsingleton.h \
    constants.h \
    sql/sqlprocess.h \
    sql/databasemanager.h \
    sql/database.h \
    sql/datatable.h \
    thread/recvtextprocessthread.h

FORMS    += widget.ui

win32-msvc2013{
    LIBS += ../Lib/Util.lib
    LIBS+= ../Lib/network.lib
}

win32-g++{
    LIBS+= -L../Lib/ -lUtil
    QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder
}

unix{
    LIBS+= -L../Lib/ -lNetwork -lUtil
}

INCLUDEPATH += $$PWD/../
INCLUDEPATH += $$PWD/../../RimClient/
