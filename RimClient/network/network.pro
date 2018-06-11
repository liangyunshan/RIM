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
    netglobal.cpp \
    rsocket.cpp \
    rtask.cpp \
    aes/AES.cpp \
    aes/raes.cpp \
    wraprule/wraprule.cpp \
    wraprule/datapacketrule.cpp \
    win32net/tcpmsgreceive.cpp \
    win32net/tcpmsgsender.cpp \
    win32net/rudpsocket.cpp

HEADERS +=\
        network_global.h \
    netglobal.h \
    rsocket.h \
    rtask.h \
    aes/AES.h \
    aes/raes.h \
    wraprule/wraprule.h \
    wraprule/datapacketrule.h \
    win32net/tcpmsgreceive.h \
    win32net/tcpmsgsender.h \
    win32net/rudpsocket.h

