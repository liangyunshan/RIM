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

DESTDIR += ../Bin
INCLUDEPATH += $$PWD/../

win32-msvc2013{
    LIBS+= ../Bin/Util.lib
}

win32-g++{
    LIBS+= -L../Bin -lUtil
    QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder
}

unix {
    target.path = /usr/lib
    INSTALLS += target
}

SOURCES += \
    msgreceive.cpp \
    netglobal.cpp \
    socket.cpp

HEADERS +=\
        network_global.h \
    msg.h \
    msgreceive.h \
    netglobal.h \
    socket.h

