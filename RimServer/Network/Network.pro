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
    win32net/SharedIocpData.h \
    head.h \
    win32net/workthread.h \
    win32net/netutils.h \
    win32net/iopacket.h \
    win32net/iocpcontext.h \
    socket.h \
    abstractserver.h \
    multitransmits/basetransmit.h \
    multitransmits/tcptransmit.h \
    dataprocess/handler.h \
    dataprocess/sockdatahandler.h \
    connection/seriesconnection.h \
    connection/tcpclient.h \
    wraprule/qdb21_wraprule.h \
    wraprule/qdb61a_wraprule.h \
    wraprule/qdb495_wraprule.h \
    wraprule/qdb2048_wraprule.h \
    wraprule/qdb2051_wraprule.h \
    wraprule/tcp_wraprule.h \
    wraprule/tcp495datapacketrule.h \
    wraprule/tcpdatapacketrule.h \
    wraprule/tk205_wraprule.h \
    wraprule/udp_wraprule.h \
    wraprule/wraprule.h

SOURCES += \
    netglobal.cpp \
    win32net/SharedIocpData.cpp \
    win32net/workthread.cpp \
    win32net/netutils.cpp \
    win32net/tcpserver.cpp \
    win32net/iopacket.cpp \
    win32net/iocpcontext.cpp \
    socket.cpp \
    abstractserver.cpp \
    multitransmits/basetransmit.cpp \
    multitransmits/tcptransmit.cpp \
    dataprocess/handler.cpp \
    dataprocess/sockdatahandler.cpp \
    connection/seriesconnection.cpp \
    connection/tcpclient.cpp \
    wraprule/qdb21_wraprule.cpp \
    wraprule/qdb61a_wraprule.cpp \
    wraprule/qdb495_wraprule.cpp \
    wraprule/qdb2048_wraprule.cpp \
    wraprule/qdb2051_wraprule.cpp \
    wraprule/tcp_wraprule.cpp \
    wraprule/tcp495datapacketrule.cpp \
    wraprule/tcpdatapacketrule.cpp \
    wraprule/TK205_WrapRule.cpp \
    wraprule/udp_wraprule.cpp \
    wraprule/wraprule.cpp

INCLUDEPATH += $$PWD/../../RimClient/

win32-msvc2013{
    LIBS += ../Lib/Util.lib
}

unix{
    LIBS+= -L../Lib/ -lNetwork -lUtil
}
