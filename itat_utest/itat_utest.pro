TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp

INCLUDEPATH += ../include

INCLUDEPATH += $$PWD/../itat
DEPENDPATH += $$PWD/../itat

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
        TARGET = itat_utest_d
        LIBS += -L$$OUT_PWD/../itat/ -litat-d
        message("Build for Debug version")
    }
    CONFIG(release, debug|release) {
        TARGET = itat_utest
        LIBS += -L$$OUT_PWD/../itat/ -litat
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


