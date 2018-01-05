QT += core gui
QT += network

contains(QT_MAJOR_VERSION, 5): QT += widgets gui-private

CONFIG += c++11

TARGET = RimClient

TEMPLATE = app

RC_ICONS +=RimClient.ico

SOURCES += main.cpp \
    global.cpp \
    datastruct.cpp

DESTDIR = bin

include(Util/Util.pri)
include(Widgets/Widgets.pri)
include(network/network.pri)
include(thread/thread.pri)

HEADERS += \
    head.h \
    global.h \
    datastruct.h \
    constants.h

TRANSLATIONS += $${TARGET}_zh_CN.ts

RESOURCES += \
    resource.qrc

win32:RC_FILE = RimClient_resource.rc

win32-msvc2013{
    LIBS += gdi32.lib dwmapi.lib
}

win32-g++{
QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder
}
