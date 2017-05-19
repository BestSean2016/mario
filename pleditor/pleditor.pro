#-------------------------------------------------
#
# Project created by QtCreator 2017-05-19T18:30:54
#
#-------------------------------------------------

QT       -= core gui
CONFIG += c++14

INCLUDEPATH += ../include
INCLUDEPATH += /app/boost/include

TEMPLATE = lib

DEFINES += PLEDITOR_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += pleditor.cpp

HEADERS += pleditor.h\
        pleditor_global.h


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
        TARGET = pleditor-d

        message("Build for Debug version")
    }
    CONFIG(release, debug|release) {
        TARGET = pleditor
        message("Build for release version")
    }
}


###################### windows #########################
windows {
    DEFINES += _WINDOWS

    message("Building for Windows")


    CONFIG(debug, debug|release) {
        DEFINES += _DEBUG_
        TARGET = pleditor-d
        message("Build for Debug version")
    }
    CONFIG(release, debug|release) {
        TARGET = pleditor
        message("Build for release version")
    }

}
