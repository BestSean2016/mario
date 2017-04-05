#-------------------------------------------------
#
# Project created by QtCreator 2016-12-06T16:40:42
#
#-------------------------------------------------

QT     -= core gui
CONFIG += c++14

INCLUDEPATH += ../include
INCLUDEPATH += /app/boost/include

TARGET   = itat
TEMPLATE = lib

DEFINES += ITAT_LIBRARY

SOURCES += \
    ../src/str.c \
    ../src/threadpool.c \
    ../src/saltapi.cpp \
    ../src/httpapi.cpp \
    ../src/mylog.cpp \
    ../src/node.cpp \
    ../src/state.cpp \
    ../src/graph.cpp \
    ../src/mario.cpp

HEADERS += \
    ../include/itat_global.h \
    ../include/mylog.h \
    ../include/str.h \
    ../include/threadpool.h \
    ../include/saltapi.hpp \
    ../include/httpapi.hpp \
    ../include/node.hpp \
    ../include/state.hpp \
    ../include/edge.hpp \
    ../include/graph.hpp \
    ../include/itat.h \
    ../include/mario.hpp



###################### unix ############################
unix {
    DEFINES += _UNIX

    message("Building for unix")
    INCLUDEPATH += /usr/local/include

    LIBS += -lpthread -lrt -ligraph

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
    DEFINES += _WINDOWS

    message("Building for Windows")


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

DISTFILES +=


