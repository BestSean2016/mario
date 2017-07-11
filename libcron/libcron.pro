#-------------------------------------------------
#
# Project created by QtCreator 2017-06-16T09:00:34
#
#-------------------------------------------------

QT       -= core gui

TEMPLATE = lib

DEFINES += LIBCRON_LIBRARY

SOURCES += \
    database.c \
    cron.c \
    env.c \
    job.c \
    misc.c \
    do_command.c \
    user.c \
    pw_dup.c \
    popen.c \
    entry.c \
    security.c

HEADERS +=\
        config.h \
        structs.h \
        libcron_global.h \
        bitstring.h \
        cron-paths.h \
        cronie_common.h \
        externs.h \
        funcs.h \
        globals.h \
        macros.h \
        pathnames.h

unix {
    DEFINES += _UNIX

    message("Building for unix")

    target.path = /usr/local/lib
    INSTALLS += target

    CONFIG(debug, debug|release) {
        DEFINES += _DEBUG_
        TARGET = cron-d
        message("Build for Debug version")
    }
    CONFIG(release, debug|release) {
        TARGET = cron
        message("Build for release version")
    }
}

###################### windows #########################
windows {
    DEFINES += _WINDOWS

    message("Building for Windows")


    CONFIG(debug, debug|release) {
        DEFINES += _DEBUG_
        TARGET = cron-d
        message("Build for Debug version")
    }
    CONFIG(release, debug|release) {
        TARGET = cron
        message("Build for release version")
    }

}
