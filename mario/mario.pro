TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt

DEFINES  += __USING_MYSQL__

SOURCES += main.cpp \
    ../src/chinese.c \
    ../src/mario_mysql.cpp \
    ../src/mario_data.cpp \
    ../src/utility.c \
    ../src/salt_api.cpp \
    ../src/http_client.cpp \
    ../src/threadpool.c \
    ../src/pipe.cpp

HEADERS += \
    ../include/chinese.h \
    ../include/mario_mysql.h \
    ../include/utility.h \
    ../include/mario_data.h \
    ../include/salt_api.h \
    ../include/http_client.h \
    ../include/threadpool.h \
    ../include/pipe.h

DISTFILES += \
    ../src/mario-host-ip.py \
    ../src/mario-host-minion.sql \
    ../src/mario.sql \
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

