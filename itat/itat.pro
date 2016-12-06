#-------------------------------------------------
#
# Project created by QtCreator 2016-12-06T16:40:42
#
#-------------------------------------------------

QT       -= core gui

INCLUDEPATH += ../include

TARGET = itat
TEMPLATE = lib

DEFINES += ITAT_LIBRARY

SOURCES += \
    ../src/itat.cpp

HEADERS += \
    ../include/itat.h \
    ../include/itat_global.h



###################### unix ############################
unix {
    DEFINES += _UNIX_

    message("Building for unix")
    INCLUDEPATH += /usr/local/include

    LIBS += -lpthread -lrt

    target.path = /usr/local/bin/mario
    INSTALLS += target

    CONFIG(debug, debug|release) {
        DEFINES += _DEBUG_
        TARGET = itat-d
        message("Build for Debug version")
    }
    CONFIG(release, debug|release) {
        TARGET = itat
        message("Build for release version")
    }
}

###################### windows #########################
windows {
    DEFINES += _WINDOWS_

    message("Building for Windows")

    INCLUDEPATH += D:/projects/md/gtest

    CONFIG(debug, debug|release) {
        DEFINES += _DEBUG_
        TARGET = utest-1d
        LIBS += -LD:/projects/md/gtest/win64 -lgtest-d -lgtest_main-d
        message("Build for Debug version")
    }
    CONFIG(release, debug|release) {
        TARGET = utest-1
        LIBS += -LD:/projects/md/gtest/win64 -lgtest -lgtest_main
        message("Build for release version")
    }

}


