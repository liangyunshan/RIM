QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

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
