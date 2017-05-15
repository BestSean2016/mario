TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt

DEFINES  += __USING_MYSQL__

SOURCES += main.cpp \
    ../src/chinese.c \
    mario_mysql.cpp \
    mario_data.cpp \
    utility.c \
    salt_api.cpp \
    http_client.cpp \
    threadpool.c \
    pipe.cpp

HEADERS += \
    ../include/chinese.h \
    mario_mysql.h \
    utility.h \
    mario_data.h \
    salt_api.h \
    http_client.h \
    threadpool.h \
    pipe.h

DISTFILES += \
    mario-host-ip.py \
    mario-host-minion.sql \
    mario.sql \
    run_mario_run.sh



INCLUDEPATH += ../include
INCLUDEPATH += /usr/local/include/igraph



###################### unix ############################
unix {
    DEFINES += _UNIX_

    message("Building for unix")
    INCLUDEPATH += /usr/local/include

    LIBS += -L/usr/local/lib -L/usr/lib64/mysql
    LIBS += -lpthread -lrt -ligraph -lmysqlclient

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

