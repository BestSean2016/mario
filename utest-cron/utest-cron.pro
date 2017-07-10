TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp


INCLUDEPATH += $$PWD/../libcron
DEPENDPATH += $$PWD/../libcron


###################### unix ############################
unix {
    DEFINES += _UNIX_

    message("Building for unix")
    INCLUDEPATH += /usr/local/include

    LIBS += -lpthread -lrt
    LIBS += -L/usr/local/lib -lgtest -lgtest_main

    target.path = /usr/local/bin/mario
    INSTALLS += target

    CONFIG(debug, debug|release) {
        DEFINES += _DEBUG_
        TARGET = utest-cron-d
        LIBS += -L$$OUT_PWD/../libcron/ -lcron-d
        message("Build for Debug version")
    }
    CONFIG(release, debug|release) {
        TARGET = utest-cron
        LIBS += -L$$OUT_PWD/../libcron/ -lcron
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
        TARGET = utest-cron-d
        LIBS += -LD:/projects/md/gtest/win64 -lgtest-d -lgtest_main-d
        LIBS += -L$$OUT_PWD/../libcron/debug/ -lcron-d
        message("Build for Debug version")
    }
    CONFIG(release, debug|release) {
        TARGET = utest-cron
        LIBS += -LD:/projects/md/gtest/win64 -lgtest -lgtest_main
        LIBS += -L$$OUT_PWD/../libcron/release/ -lcron-d
        message("Build for release version")
    }

}

