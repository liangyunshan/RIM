#-------------------------------------------------
#
# Project created by QtCreator 2018-01-08T14:41:47
#
#-------------------------------------------------

QT  += core gui
QT  += xml

contains(QT_MAJOR_VERSION, 5): QT += widgets gui-private

#CONFIG += build_all


TARGET = Util
TEMPLATE = lib

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

DEFINES += UTIL_LIBRARY

win32-g++{
    QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder
}

INCLUDEPATH += $$PWD/../

SOURCES += \
    fileutils.cpp \
    imagemanager.cpp \
    rlog.cpp \
    rutil.cpp \
    regexp.cpp \
    rbuffer.cpp

HEADERS +=\
        util_global.h \
    fileutils.h \
    imagemanager.h \
    rlog.h \
    rutil.h \
    regexp.h \
    rbuffer.h
