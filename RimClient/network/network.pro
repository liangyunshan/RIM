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

#读取本地联系人列表，为了解决并需求变动带来的冲突问题
DEFINES += __LOCAL_CONTACT__

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
INCLUDEPATH += $$PWD/multitransmits/

win32-msvc2013{
    LIBS+= ../Lib/Util.lib
    LIBS+= ../Lib/DDS.lib
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
    win32net/rudpsocket.cpp \
    multitransmits/basetransmit.cpp \
    multitransmits/tcptransmit.cpp \
    win32net/msgsender.cpp \
    win32net/msgreceive.cpp \
    multitransmits/ddstransmit.cpp \
    wraprule/dds_wraprule.cpp \
    wraprule/qdb21_wraprule.cpp \
    wraprule/qdb61a_wraprule.cpp \
    wraprule/qdb2048_wraprule.cpp \
    wraprule/qdb2051_wraprule.cpp \
    wraprule/tcp_wraprule.cpp \
    wraprule/tcp495datapacketrule.cpp \
    wraprule/tcpdatapacketrule.cpp \
    wraprule/TK205_WrapRule.cpp \
    wraprule/udp_wraprule.cpp \
    wraprule/wraprule.cpp \
    serialno.cpp

HEADERS +=\
        network_global.h \
    netglobal.h \
    rsocket.h \
    rtask.h \
    aes/AES.h \
    aes/raes.h \
    win32net/rudpsocket.h \
    multitransmits/basetransmit.h \
    multitransmits/tcptransmit.h \
    win32net/msgreceive.h \
    win32net/msgsender.h \
    multitransmits/ddstransmit.h \
    3rdhead/DDS.h \
    wraprule/dds_wraprule.h \
    wraprule/qdb21_wraprule.h \
    wraprule/qdb61a_wraprule.h \
    wraprule/qdb2048_wraprule.h \
    wraprule/qdb2051_wraprule.h \
    wraprule/tcp_wraprule.h \
    wraprule/tcp495datapacketrule.h \
    wraprule/tcpdatapacketrule.h \
    wraprule/tk205_wraprule.h \
    wraprule/udp_wraprule.h \
    wraprule/wraprule.h \
    serialno.h
