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
    ../src/saltapi.cpp \
    ../src/httpapi.cpp \
    ../src/mylog.cpp \
    ../src/node.cpp \
    ../src/mario.cpp \
    ../src/state.cpp \
    ../src/pipeline.cpp \
    ../src/itat-python.cpp \
    ../src/djangoapi.cpp \
    ../src/mario_sql.cpp

HEADERS += \
    ../include/itat_global.h \
    ../include/mylog.h \
    ../include/str.h \
    ../include/httpapi.hpp \
    ../include/node.hpp \
    ../include/edge.hpp \
    ../include/itat.h \
    ../include/mario.hpp \
    ../include/saltapi.hpp \
    ../include/state.hpp \
    ../include/pipeline.hpp \
    ../include/djangoapi.hpp \
    ../include/mario_sql.h



###################### unix ############################
unix {
    DEFINES += _UNIX

    message("Building for unix")
    INCLUDEPATH += /usr/local/include
    #INCLUDEPATH += /usr/include/python2.7
    INCLUDEPATH += /data/py2713/include/python2.7

    LIBS += -lpthread -lrt -ligraph -lboost_python
    LIBS += -L/data/py2713/lib -lpython2.7
    LIBS += -L/usr/lib64/mysql -lmysqlclient

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

DISTFILES += \
    itat.py \
    bill_message.py \
    install.sh \
    transfers.py


