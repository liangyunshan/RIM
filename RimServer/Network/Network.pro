QT += core network

CONFIG += c++11

TARGET = Network
CONFIG += console
CONFIG -= app_bundle

DEFINES += NETWORK_LIBRARY

#读取本地联系人列表，为了解决并需求变动带来的冲突问题
DEFINES += __LOCAL_CONTACT__

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
    abstractserver.h \
    wraprule/wraprule.h \
    multitransmits/basetransmit.h \
    multitransmits/tcptransmit.h \
    wraprule/tcp495datapacketrule.h \
    wraprule/tcpdatapacketrule.h \
    dataprocess/handler.h \
    dataprocess/sockdatahandler.h

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
    abstractserver.cpp \
    wraprule/wraprule.cpp \
    multitransmits/basetransmit.cpp \
    multitransmits/tcptransmit.cpp \
    wraprule/tcp495datapacketrule.cpp \
    wraprule/tcpdatapacketrule.cpp \
    dataprocess/handler.cpp \
    dataprocess/sockdatahandler.cpp

INCLUDEPATH += $$PWD/../../RimClient/

win32-msvc2013{
    LIBS += ../Lib/Util.lib
}

unix{
    LIBS+= -L../Lib/ -lNetwork -lUtil
}
