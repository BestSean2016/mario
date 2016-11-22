TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    ../src/chinese.c \
    ../src/mario_mysql.c

HEADERS += \
    ../include/chinese.h \
    ../include/mario_mysql.h \
    ../include/mario_types.h

DISTFILES += \
    ../src/mario-host-ip.py \
    ../src/mario-host-minion.sql



INCLUDEPATH += ../include

###################### unix ############################
unix {
    DEFINES += _UNIX_

    message("Building for unix")
    INCLUDEPATH += /usr/local/include

    LIBS += -lpthread -lrt -ligraph -lcurl
    LIBS += -L/usr/local/lib -lgtest -lgtest_main

    target.path = /usr/local/bin/mario
    INSTALLS += target

    CONFIG(debug, debug|release) {
        DEFINES += _DEBUG_
        TARGET = mario_d
        message("Build for Debug version")
    }
    CONFIG(release, debug|release) {
        TARGET = mario
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
        TARGET = mario_d
        LIBS += -LD:/projects/md/gtest/win64 -lgtest-d -lgtest_main-d
        message("Build for Debug version")
    }
    CONFIG(release, debug|release) {
        TARGET = mariods
        LIBS += -LD:/projects/md/gtest/win64 -lgtest -lgtest_main
        message("Build for release version")
    }

}

