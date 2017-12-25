QT += core gui

contains(QT_MAJOR_VERSION, 5): QT += widgets gui-private

CONFIG += c++11

TARGET = RimClient

TEMPLATE = app

SOURCES += main.cpp \
    global.cpp \
    datastruct.cpp

DESTDIR = bin

include(Util/Util.pri)
include(Widgets/Widgets.pri)

HEADERS += \
    head.h \
    global.h \
    datastruct.h \
    constants.h


TRANSLATIONS += $${TARGET}_zh_CN.ts

RESOURCES += \
    resource.qrc

win32-msvc2013{
    LIBS += gdi32.lib dwmapi.lib
}

