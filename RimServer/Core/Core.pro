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

TRANSLATIONS += $${TARGET}_zh_CN.ts
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
    rsingleton.cpp \
    sql/sqlprocess.cpp \
    sql/databasemanager.cpp \
    sql/database.cpp \
    sql/datatable.cpp \
    thread/recvtextprocessthread.cpp \
    thread/sendtextprocessthread.cpp \
    jsonkey.cpp \
    sql/rpersistence.cpp \
    global.cpp \
    sql/autotransaction.cpp \
    file/xmlparse.cpp \
    Network/msgwrap/binary_wrapformat.cpp \
    Network/msgwrap/json_wrapformat.cpp \
    Network/msgwrap/msgwrap.cpp \
    Network/msgparse/dataparse.cpp \
    Network/msgparse/json_msgparse.cpp \
    Network/msgparse/msgparsefactory.cpp \
    protocol/datastruct.cpp \
    protocol/localprotocoldata.cpp \
    protocol/protocoldata.cpp \
    Network/msgprocess/dataprocess.cpp \
    Network/msgwrap/wrapformat.cpp \
    Network/msgwrap/basemsgwrap.cpp \
    Network/msgwrap/wrapfactory.cpp \
    Network/msgwrap/localmsgwrap.cpp \
    Network/msgwrap/binary716_wrapfromat.cpp \
    Network/msgparse/binary716_msgparse.cpp \
    Network/msgprocess/data716process.cpp \
    thread/netconnector.cpp \
    thread/filesendqueuethread.cpp \
    broadcastnode.cpp \
    file/globalconfigfile.cpp \
    config/routetable.cpp \
    config/routetableview.cpp \
    config/routetableclientmodel.cpp \
    config/routetableservermodel.cpp \
    config/routetableserverdelegate.cpp \
    config/routetableclientdelegate.cpp \
    config/routetablehead.cpp \
    config/routesettings.cpp \
    actionmanager/actionmanager.cpp \
    actionmanager/id.cpp \
    mainwindow.cpp

HEADERS  += \
    rsingleton.h \
    constants.h \
    sql/sqlprocess.h \
    sql/databasemanager.h \
    sql/database.h \
    sql/datatable.h \
    thread/recvtextprocessthread.h \
    thread/sendtextprocessthread.h \
    jsonkey.h \
    sql/rpersistence.h \
    global.h \
    sql/autotransaction.h \
    file/xmlparse.h \
    Network/msgwrap/binary_wrapformat.h \
    Network/msgwrap/json_wrapformat.h \
    Network/msgwrap/msgwrap.h \
    Network/msgparse/dataparse.h \
    Network/msgparse/json_msgparse.h \
    Network/msgparse/msgparsefactory.h \
    protocol/datastruct.h \
    protocol/localprotocoldata.h \
    protocol/protocoldata.h \
    Network/msgprocess/dataprocess.h \
    Network/msgwrap/wrapformat.h \
    Network/msgwrap/basemsgwrap.h \
    Network/msgwrap/wrapfactory.h \
    Network/msgwrap/localmsgwrap.h \
    Network/msgwrap/binary716_wrapfromat.h \
    Network/msgparse/binary716_msgparse.h \
    Network/msgprocess/data716process.h \
    thread/netconnector.h \
    thread/filesendqueuethread.h \
    broadcastnode.h \
    file/globalconfigfile.h \
    config/routetable.h \
    config/routetableview.h \
    config/routetableclientmodel.h \
    config/routetableservermodel.h \
    config/routetableserverdelegate.h \
    config/routetableclientdelegate.h \
    config/routetablehead.h \
    config/routesettings.h \
    actionmanager/actionmanager.h \
    actionmanager/id.h \
    mainwindow.h

FORMS    += \
    config/routetable.ui \
    mainwindow.ui

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
