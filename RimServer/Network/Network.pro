QT += core network

CONFIG += c++11

TARGET = Network
CONFIG += console
CONFIG -= app_bundle

DEFINES += NETWORK_LIBRARY

TEMPLATE = lib
DESTDIR += ../Bin

HEADERS += \
    msg.h \
    msgreceive.h \
    netglobal.h \
    network_global.h \
    msgsend.h \
    tcpsocket.h

SOURCES += \
    msgreceive.cpp \
    netglobal.cpp \
    msgsend.cpp \
    tcpsocket.cpp

INCLUDEPATH += $$PWD/../../RimClient/

win32-msvc2013{
    LIBS += E:\Git/build-RimClient-Qt570vs64-Debug/Bin/Util.lib
}
