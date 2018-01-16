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
    tcpserver.h \
    tcpclient.h \
    SharedIocpData.h \
    head.h \
    workthread.h \
    netutils.h \
    iopacket.h \
    iocpcontext.h \
    socket.h

SOURCES += \
    netglobal.cpp \
    tcpclient.cpp \
    SharedIocpData.cpp \
    workthread.cpp \
    netutils.cpp \
    tcpserver.cpp \
    iopacket.cpp \
    iocpcontext.cpp \
    socket.cpp

INCLUDEPATH += $$PWD/../../RimClient/

win32-msvc2013{
    LIBS += ../Lib/Util.lib
}

unix{
    LIBS+= -L../Lib/ -lNetwork -lUtil
}
