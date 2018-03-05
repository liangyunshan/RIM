QT += core network

CONFIG += c++11

TARGET = Network
CONFIG += console
CONFIG -= app_bundle

DEFINES += NETWORK_LIBRARY

TEMPLATE = lib

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

HEADERS += \
    netglobal.h \
    network_global.h \
    win32net/tcpserver.h \
    tcpclient.h \
    win32net/SharedIocpData.h \
    head.h \
    win32net/workthread.h \
    win32net/netutils.h \
    win32net/iopacket.h \
    win32net/iocpcontext.h \
    socket.h \
    abstractserver.h

SOURCES += \
    netglobal.cpp \
    tcpclient.cpp \
    win32net/SharedIocpData.cpp \
    win32net/workthread.cpp \
    win32net/netutils.cpp \
    win32net/tcpserver.cpp \
    win32net/iopacket.cpp \
    win32net/iocpcontext.cpp \
    socket.cpp \
    abstractserver.cpp

INCLUDEPATH += $$PWD/../../RimClient/

win32-msvc2013{
    LIBS += ../Lib/Util.lib
}

unix{
    LIBS+= -L../Lib/ -lNetwork -lUtil
}
