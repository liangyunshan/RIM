TEMPLATE = subdirs

CONFIG += ordered

SUBDIRS += \
    Network \
    Core

win32-g++{
    QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder
}

#在Server2003R2下运行
win32 {
    QMAKE_LFLAGS_WINDOWS = /SUBSYSTEM:WINDOWS,5.01
    QMAKE_LFLAGS_CONSOLE = /SUBSYSTEM:CONSOLE,5.01

    DEFINES += _ATL_XP_TARGETING
    QMAKE_CFLAGS += /D_USING_V140_SDK71_
    QMAKE_CXXFLAGS += /D_USING_V140_SDK71_
    LIBS += -L$$quote(C:\Program Files (x86)\Microsoft SDKs\Windows\v7.1A\Lib)
    INCLUDEPATH += $$quote(C:\Program Files (x86)\Microsoft SDKs\Windows\v7.1A\Include)
}
