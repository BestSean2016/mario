TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    pyaxa-mysql.cpp


INCLUDEPATH += ../include


###################### unix ############################
unix {
    DEFINES += _UNIX_

    message("Building for unix")
    INCLUDEPATH += /usr/local/include

    LIBS += -L/usr/local/lib -lpthread -lrt
    LIBS += -L/usr/lib64/mysql -lmysqlclient

    target.path = /usr/local/bin/mario
    INSTALLS += target

    CONFIG(debug, debug|release) {
        DEFINES += _DEBUG_
        TARGET = py-axa_d
        LIBS += -L$$OUT_PWD/../itat -litat-d
        message("Build for Debug version")
    }
    CONFIG(release, debug|release) {
        TARGET = py-axa
        LIBS += -L$$OUT_PWD/../itat -litat
        message("Build for release version")
    }
}

###################### windows #########################
windows {
    DEFINES += _WINDOWS_

    message("Building for Windows")

    CONFIG(debug, debug|release) {
        DEFINES += _DEBUG_
        TARGET = py-axa_d
        LIBS += -L$$OUT_PWD/../itat/debug -litat-d
        message("Build for Debug version")
    }
    CONFIG(release, debug|release) {
        TARGET = py-axa
        LIBS += -L$$OUT_PWD/../itat/release -litat
        message("Build for release version")
    }
}

HEADERS += \
    config.h \
    pyaxa-mysql.hpp

DISTFILES += \
    capturing
