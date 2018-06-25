#-------------------------------------------------
#
# Project created by QtCreator 2018-01-08T17:23:21
#
#-------------------------------------------------

QT       += core gui
QT       += sql
QT       += xml

CONFIG+= console

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RimServer
TEMPLATE = app

#读取本地联系人列表，为了解决并需求变动带来的冲突问题
DEFINES += __LOCAL_CONTACT__

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
    thread/recvtextprocessthread.cpp \
    thread/dataprocess.cpp \
    thread/sendtextprocessthread.cpp \
    jsonkey.cpp \
    sql/rpersistence.cpp \
    global.cpp \
    sql/autotransaction.cpp \
    file/xmlparse.cpp \
    Network/msgwrap/binary_wrapformat.cpp \
    Network/msgwrap/binarywrapfactory.cpp \
    Network/msgwrap/json_wrapformat.cpp \
    Network/msgwrap/msgwrap.cpp \
    Network/msgwrap/textwrapfactory.cpp \
    Network/wraprule/qdb21_wraprule.cpp \
    Network/wraprule/qdb61a_wraprule.cpp \
    Network/wraprule/qdb495_wraprule.cpp \
    Network/wraprule/qdb2051_wraprule.cpp \
    Network/wraprule/tcp_wraprule.cpp \
    Network/wraprule/TK205_WrapRule.cpp \
    Network/wraprule/udp_wraprule.cpp \
    Network/msgparse/dataparse.cpp \
    Network/msgparse/json_msgparse.cpp \
    Network/msgparse/msgparsefactory.cpp \
    protocol/datastruct.cpp \
    protocol/localprotocoldata.cpp \
    protocol/protocoldata.cpp
HEADERS  += widget.h \
    rsingleton.h \
    constants.h \
    sql/sqlprocess.h \
    sql/databasemanager.h \
    sql/database.h \
    sql/datatable.h \
    thread/recvtextprocessthread.h \
    thread/dataprocess.h \
    thread/sendtextprocessthread.h \
    jsonkey.h \
    sql/rpersistence.h \
    global.h \
    sql/autotransaction.h \
    file/xmlparse.h \
    Network/msgwrap/binary_wrapformat.h \
    Network/msgwrap/binarywrapfactory.h \
    Network/msgwrap/json_wrapformat.h \
    Network/msgwrap/msgwrap.h \
    Network/msgwrap/textwrapfactory.h \
    Network/wraprule/qdb21_wraprule.h \
    Network/wraprule/qdb61a_wraprule.h \
    Network/wraprule/qdb495_wraprule.h \
    Network/wraprule/qdb2051_wraprule.h \
    Network/wraprule/tcp_wraprule.h \
    Network/wraprule/tk205_wraprule.h \
    Network/wraprule/udp_wraprule.h \
    Network/msgparse/dataparse.h \
    Network/msgparse/json_msgparse.h \
    Network/msgparse/msgparsefactory.h \
    protocol/datastruct.h \
    protocol/localprotocoldata.h \
    protocol/protocoldata.h

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
