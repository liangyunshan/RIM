TEMPLATE = subdirs

CONFIG += ordered console debug

SUBDIRS += \
    Network \
    Core

win32-g++{
    QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder
}
