#-------------------------------------------------
#
# Project created by QtCreator 2018-01-08T14:42:28
#
#-------------------------------------------------

QT       += core gui
QT       += xml
QT       += sql

contains(QT_MAJOR_VERSION, 5): QT += widgets gui-private

TARGET = RimClient
TEMPLATE = app

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

win32:RC_FILE = $${TARGET}_resource.rc
RC_ICONS += $${TARGET}.ico

INCLUDEPATH += $$PWD/../

TRANSLATIONS += $${TARGET}_zh_CN.ts

win32-msvc2013{
    LIBS+= ../Lib/Util.lib
    LIBS+= ../Lib/network.lib
    LIBS += gdi32.lib dwmapi.lib
}

win32-g++{
    LIBS+= -L../Lib -lUtil
    LIBS+= -L../Lib -lnetwork
    QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder
}

unix{
    LIBS+= -L../Lib -lUtil
    LIBS+= -L../Lib -lnetwork
    QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder
}

SOURCES += \
    Widgets/abstractchatwidget.cpp \
    Widgets/addfriend.cpp \
    Widgets/editpersoninfowindow.cpp \
    Widgets/itemhoverinfo.cpp \
    Widgets/logindialog.cpp \
    Widgets/maindialog.cpp \
    Widgets/observer.cpp \
    Widgets/onlinestate.cpp \
    Widgets/panelbottomtoolbar.cpp \
    Widgets/panelcontentarea.cpp \
    Widgets/panelcontentpage.cpp \
    Widgets/panelgrouppage.cpp \
    Widgets/panelhistorypage.cpp \
    Widgets/panelpersonpage.cpp \
    Widgets/paneltoparea.cpp \
    Widgets/slidebar.cpp \
    Widgets/subject.cpp \
    Widgets/systemsettings.cpp \
    Widgets/systemtrayicon.cpp \
    Widgets/systemuserimagewindow.cpp \
    Widgets/toolbar.cpp \
    Widgets/widget.cpp \
    Widgets/actionmanager/actionmanager.cpp \
    Widgets/actionmanager/commandfile.cpp \
    Widgets/actionmanager/id.cpp \
    Widgets/actionmanager/shortcutsettings.cpp \
    Widgets/nativewindow/MainWindow.cpp \
    Widgets/nativewindow/QMainPanel.cpp \
    Widgets/nativewindow/QWinWidget.cpp \
    Widgets/textedit/basetextedit.cpp \
    Widgets/textedit/complextextedit.cpp \
    Widgets/textedit/simpletextedit.cpp \
    Widgets/toolbox/listbox.cpp \
    Widgets/toolbox/toolbox.cpp \
    Widgets/toolbox/toolitem.cpp \
    Widgets/toolbox/toolpage.cpp \
    Widgets/widget/rbutton.cpp \
    Widgets/widget/rmessagebox.cpp \
    Widgets/widget/rtoolbutton.cpp \
    datastruct.cpp \
    global.cpp \
    main.cpp \
    thread/msgreceiveproctask.cpp \
    thread/taskmanager.cpp \
    rsingleton.cpp \
    user/userinfofile.cpp \
    protocoldata.cpp \
    Network/msgwrap.cpp \
    Network/netconnector.cpp \
    Widgets/registdialog.cpp \
    Widgets/netsettings.cpp \
    thread/dataprocess.cpp \
    messdiapatch.cpp \
    sql/database.cpp \
    sql/databasemanager.cpp \
    thread/databasethread.cpp
    jsonkey.cpp \
    Widgets/widget/rlabel.cpp

HEADERS  += \
    Widgets/abstractchatwidget.h \
    Widgets/addfriend.h \
    Widgets/editpersoninfowindow.h \
    Widgets/itemhoverinfo.h \
    Widgets/logindialog.h \
    Widgets/maindialog.h \
    Widgets/observer.h \
    Widgets/onlinestate.h \
    Widgets/panelbottomtoolbar.h \
    Widgets/panelcontentarea.h \
    Widgets/panelcontentpage.h \
    Widgets/panelgrouppage.h \
    Widgets/panelhistorypage.h \
    Widgets/panelpersonpage.h \
    Widgets/paneltoparea.h \
    Widgets/slidebar.h \
    Widgets/subject.h \
    Widgets/systemsettings.h \
    Widgets/systemtrayicon.h \
    Widgets/systemuserimagewindow.h \
    Widgets/toolbar.h \
    Widgets/widget.h \
    Widgets/actionmanager/actionmanager.h \
    Widgets/actionmanager/commandfile.h \
    Widgets/actionmanager/id.h \
    Widgets/actionmanager/shortcutsettings.h \
    Widgets/nativewindow/MainWindow.h \
    Widgets/nativewindow/QMainPanel.h \
    Widgets/nativewindow/QWinWidget.h \
    Widgets/textedit/basetextedit.h \
    Widgets/textedit/complextextedit.h \
    Widgets/textedit/simpletextedit.h \
    Widgets/toolbox/listbox.h \
    Widgets/toolbox/toolbox.h \
    Widgets/toolbox/toolitem.h \
    Widgets/toolbox/toolpage.h \
    Widgets/toolbox/usertoolitem.h \
    Widgets/widget/rbutton.h \
    Widgets/widget/rmessagebox.h \
    Widgets/widget/rtoolbutton.h \
    constants.h \
    datastruct.h \
    global.h \
    head.h \
    thread/msgreceiveproctask.h \
    thread/taskmanager.h \
    rsingleton.h \
    user/userinfofile.h \
    protocoldata.h \
    Network/msgwrap.h \
    Network/netconnector.h \
    Widgets/registdialog.h \
    Widgets/netsettings.h \
    thread/dataprocess.h \
    messdiapatch.h \
    sql/database.h \
    sql/databasemanager.h \
    thread/databasethread.h
    jsonkey.h \
    Widgets/widget/rlabel.h

DISTFILES += \
    Widgets/Widgets.pri


RESOURCES += \
    resource.qrc
