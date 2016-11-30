TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    ../src/mario_data.cpp \
    ../src/salt_api.cpp \
    ../src/utility.c \
    ../src/mario_mysql.cpp \
    ../src/http_client.cpp



INCLUDEPATH += ../include

INCLUDEPATH += /usr/local/include/igraph


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
        TARGET = events_lestener-d
        message("Build for Debug version")
    }
    CONFIG(release, debug|release) {
        TARGET = events_lestener
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

HEADERS += \
    ../include/chinese.h \
    ../include/http_client.h \
    ../include/mario_data.h \
    ../include/mario_mysql.h \
    ../include/salt_api.h \
    ../include/utility.h \
    ../include/pipeline.h

