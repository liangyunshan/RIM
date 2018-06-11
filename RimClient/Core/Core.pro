#-------------------------------------------------
#
# Project created by QtCreator 2018-01-08T14:42:28
#
#-------------------------------------------------

QT       += core gui
QT       += xml
QT       += sql
QT       += multimedia
QT       += webenginewidgets

contains(QT_MAJOR_VERSION, 5): QT += widgets gui-private

QMAKE_LFLAGS_RELEASE += /MAP
QMAKE_CFLAGS_RELEASE += /Zi
QMAKE_LFLAGS_RELEASE += /debug /opt:ref

#是否开启Server:DEFINES += __NO_SERVER__
DEFINES += __NO_SQL_PRINT__

#读取本地联系人列表，为了解决并需求变动带来的冲突问题
#DEFINES += __LOCAL_CONTACT__

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
} else {__NO_SERVER__
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
    Network/netconnector.cpp \
    Widgets/registdialog.cpp \
    Widgets/netsettings.cpp \
    thread/dataprocess.cpp \
    messdiapatch.cpp \
    sql/database.cpp \
    sql/databasemanager.cpp \
    thread/databasethread.cpp \
    jsonkey.cpp \
    Widgets/widget/rlabel.cpp \
    Widgets/setkeysequencedialog.cpp \
    Widgets/screenshot.cpp \
    application.cpp \
    media/mediaplayer.cpp \
    Widgets/notifywindow.cpp \
    Widgets/systemnotifyview.cpp \
    Widgets/textedit/chattextframe.cpp \
    sql/sqlprocess.cpp \
    user/userclient.cpp \
    json/jsonresolver.cpp \
    Widgets/widget/rcombobox.cpp \
    Widgets/widget/rlineedit.cpp \
    Widgets/modifyremarkwindow.cpp \
    Widgets/contactdetailwindow.cpp \
    others/msgqueuemanager.cpp \
    thread/filereceiveproctask.cpp \
    thread/filerecvtask.cpp \
    file/filedesc.cpp \
    file/filemanager.cpp \
    user/user.cpp \
    sql/rpersistence.cpp \
    user/userfriendcontainer.cpp \
    sql/datatable.cpp \
    user/userchatcontainer.cpp \
    user/groupclient.cpp \
    Widgets/registgroupdialog.cpp \
    media/audioinput.cpp \
    media/audiooutput.cpp \
    Widgets/chataudioarea.cpp \
    Widgets/document.cpp \
    Widgets/previewpage.cpp\
    Widgets/widget/rcomboboxitem.cpp \
    thread/chatmsgprocess.cpp \
    Widgets/abstractchatmainwidget.cpp \
    Widgets/setfontwidget.cpp \
    thread/historyrecordtask.cpp \
    file/xmlparse.cpp \
    Widgets/splashlogindialog.cpp \
    Network/msgwrap/localmsgwrap.cpp \
    localprotocoldata.cpp \
    Network/wraprule/qdb61a_wraprule.cpp \
    Network/wraprule/qdb21_wraprule.cpp \
    Network/wraprule/qdb495_wraprule.cpp \
    Network/wraprule/udp_wraprule.cpp \
    Network/wraprule/tcp_wraprule.cpp \
    Network/wraprule/tk205_wraprule.cpp \
    Network/msgwrap/msgwrap.cpp \
    Network/msgwrap/wrapfactory.cpp \
    Network/msgwrap/basemsgwrap.cpp \
    Network/msgwrap/wrapformat.cpp \
    Network/msgwrap/binary_wrapformat.cpp \
    Network/msgwrap/json_wrapformat.cpp \
    Widgets/chatpersonwidget.cpp


HEADERS  += \
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
    Network/netconnector.h \
    Widgets/registdialog.h \
    Widgets/netsettings.h \
    thread/dataprocess.h \
    messdiapatch.h \
    sql/database.h \
    sql/databasemanager.h \
    thread/databasethread.h \
    jsonkey.h \
    Widgets/widget/rlabel.h \
    Widgets/setkeysequencedialog.h \
    Widgets/screenshot.h \
    application.h \
    media/mediaplayer.h \
    Widgets/notifywindow.h \
    Widgets/systemnotifyview.h \
    Widgets/textedit/chattextframe.h \
    sql/sqlprocess.h \
    sql/sqlheader.h \
    user/userclient.h \
    json/jsonresolver.h \
    Widgets/widget/rcombobox.h \
    Widgets/widget/rlineedit.h \
    others/msgqueuemanager.h \
    Widgets/modifyremarkwindow.h \
    Widgets/contactdetailwindow.h \ 
    thread/filereceiveproctask.h \
    thread/filerecvtask.h \
    file/filedesc.h \
    file/filemanager.h \
    user/user.h \
    sql/rpersistence.h \
    user/userfriendcontainer.h \
    sql/datatable.h \
    user/userchatcontainer.h \
    user/groupclient.h \
    Widgets/registgroupdialog.h \
    media/audioinput.h \
    media/audiooutput.h \
    Widgets/chataudioarea.h \
    Widgets/document.h \
    Widgets/previewpage.h\
    Widgets/widget/rcomboboxitem.h \
    thread/chatmsgprocess.h \
    Widgets/abstractchatmainwidget.h \
    Widgets/setfontwidget.h \
    thread/historyrecordtask.h \
    file/xmlparse.h \
    Widgets/splashlogindialog.h \
    Network/msgwrap/localmsgwrap.h \
    localprotocoldata.h \
    Network/wraprule/qdb61a_wraprule.h \
    Network/wraprule/qdb21_wraprule.h \
    Network/wraprule/qdb495_wraprule.h \
    Network/wraprule/udp_wraprule.h \
    Network/wraprule/tcp_wraprule.h \
    Network/wraprule/tk205_wraprule.h \
    Network/msgwrap/msgwrap.h \
    Network/msgwrap/wrapfactory.h \
    Network/msgwrap/basemsgwrap.h \
    Network/msgwrap/wrapformat.h \
    Network/msgwrap/json_wrapformat.h \
    Network/msgwrap/binary_wrapformat.h \
    Widgets/chatpersonwidget.h

DISTFILES += \
    Widgets/Widgets.pri


RESOURCES += \
    resource.qrc
