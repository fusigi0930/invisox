#-------------------------------------------------
#
# Project created by QtCreator 2014-09-16T14:44:11
#
#-------------------------------------------------

QT       += network

QT       -= gui

TARGET = cinterp
TEMPLATE = lib

INCLUDEPATH += ../../inc

DEFINES += CINTERP_LIBRARY

SOURCES += ccinterpreter.cpp \
    ../../inc/cbaseinterp.cpp

HEADERS += ccinterpreter.h\
        cinterp_global.h \
    ../../inc/cbaseinterp.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
