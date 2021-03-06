TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    ../src/chinese.c \
    ../mario/mario_data.cpp \
    ../mario/mario_mysql.cpp \
    ../mario/utility.c \
    ../mario/salt_api.cpp \
    ../mario/pipe.cpp \
    ../mario/http_client.cpp \
    ../mario/threadpool.c


INCLUDEPATH += ../include
INCLUDEPATH += ../mario
INCLUDEPATH += /usr/local/include/igraph


DEFINES  += __USING_MYSQL__


###################### unix ############################
unix {
    DEFINES += _UNIX_

    message("Building for unix")
    INCLUDEPATH += /usr/local/include

    LIBS += -lpthread -lrt -ligraph -L/usr/lib64/mysql -lmysqlclient
    LIBS += -L/usr/local/lib -lgtest -lgtest_main

    target.path = /usr/local/bin/mario
    INSTALLS += target

    CONFIG(debug, debug|release) {
        DEFINES += _DEBUG_
        TARGET = utest-1d
        message("Build for Debug version")
    }
    CONFIG(release, debug|release) {
        TARGET = utest-1
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

