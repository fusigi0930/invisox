#-------------------------------------------------
#
# Project created by QtCreator 2019-03-28T22:34:03
#
#-------------------------------------------------

QT       -= gui qt core

TARGET = engine
TEMPLATE = lib

DEFINES += ENGINE_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += ../inc

SOURCES += \
        engine.cpp

HEADERS += \
        engine.h \
        engine_global.h 

unix {
    target.path = /usr/lib
    INSTALLS += target
}

CONFIG(debug, debug|release) {
    DESTDIR = $$_PRO_FILE_PWD_/../out/debug
    MOC_DIR = $$_PRO_FILE_PWD_/../out/obj/moc
    OBJECTS_DIR = $$_PRO_FILE_PWD_/../out/obj/debug
} else {
    DESTDIR = $$_PRO_FILE_PWD_/../out/release
    MOC_DIR = $$_PRO_FILE_PWD_/../out/obj/moc
    OBJECTS_DIR = $$_PRO_FILE_PWD_/../out/obj/release
}

win32 | win64 {
    CONFIG += dll warn_off
    QMAKE_POST_LINK += $$_PRO_FILE_PWD_/post_build.bat $$replace(_PRO_FILE_PWD_, /, \\) $$replace(DESTDIR, /, \\)
}

GITVER = $$system("genver.bat")
GITVER = $$replace(GITVER, "v", "")
GITVER = $$replace(GITVER, "-", ".")
GITVER = $$split(GITVER, .)
GITVER = $$member(GITVER, 0).$$member(GITVER, 1).$$member(GITVER, 2).$$member(GITVER, 3)

VERSION = $$GITVER
CONFIG += skip_target_version_ext
QMAKE_TARGET_PRODUCT = "invisOX"
QMAKE_TARGET_DESCRIPTION = "invisOX - engine"
QMAKE_TARGET_COPYRIGHT = "Copyright(c) 2019 ChuYuan Chiang. All rights reserved"
QMAKE_TARGET_COMPANY = "ChuYuan Chiang Personal."
