#-------------------------------------------------
#
# Project created by QtCreator 2018-01-08T15:00:17
#
#-------------------------------------------------

QT       += network

QT       -= gui

TARGET = network
TEMPLATE = lib

DEFINES += NETWORK_LIBRARY

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

INCLUDEPATH += $$PWD/../

win32-msvc2013{
    LIBS+= ../Lib/Util.lib
}

win32-g++{
    LIBS+= -L../Lib -lUtil
    QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder
}

unix {
    target.path = /usr/lib
    INSTALLS += target
}

SOURCES += \
    msgreceive.cpp \
    netglobal.cpp \
    msgsender.cpp \
    rsocket.cpp

HEADERS +=\
        network_global.h \
    msgreceive.h \
    netglobal.h \
    msgsender.h \
    rsocket.h

