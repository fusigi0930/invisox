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
    ../../inc/cbaseinterp.cpp \
    ccxx.cpp

HEADERS += ccinterpreter.h\
	ccxx.h \
        cinterp_global.h \
    ../../inc/cbaseinterp.h

# angelscript environment
AS_ROOT = $$_PRO_FILE_PWD_/angelscript
AS_INC = $${AS_ROOT}
AS_LIB = $${AS_ROOT}
AS_BIN = $${AS_ROOT}
AS_LIBS = -L$${AS_LIB} \
			-langelscript

unix {
    target.path = /usr/lib
    INSTALLS += target

}
win32 {
	CONFIG += dll
	QMAKE_LFLAGS+=-Wl,-e,_DllMainCRTStartup@12
}

CONFIG(debug, debug|release) {
	DESTDIR = $$_PRO_FILE_PWD_/../../out/debug
	MOC_DIR = $$_PRO_FILE_PWD_/../../out/obj/moc
	OBJECTS_DIR = $$_PRO_FILE_PWD_/../../out/obj/cinterp/debug
} else {
	DESTDIR = $$_PRO_FILE_PWD_/../../out/release
	MOC_DIR = $$_PRO_FILE_PWD_/../../out/obj/moc
	OBJECTS_DIR = $$_PRO_FILE_PWD_/../../out/obj/cinterp/release
}

INCLUDEPATH += $${AS_INC}
LIBS += $${AS_LIBS}

GITVER = $$system("genver.bat")
GITVER = $$replace(GITVER, "v", "")
GITVER = $$replace(GITVER, "-", ".")
GITVER = $$split(GITVER, .)
GITVER = $$member(GITVER, 0).$$member(GITVER, 1).$$member(GITVER, 2).$$member(GITVER, 3)

VERSION = $$GITVER
CONFIG += skip_target_version_ext
QMAKE_TARGET_PRODUCT = "invisOX"
QMAKE_TARGET_DESCRIPTION = "invisOX - C++ interpreter"
QMAKE_TARGET_COPYRIGHT = "Copyright(c) 2026 ChuYuan Chiang. All rights reserved"
QMAKE_TARGET_COMPANY = "ChuYuan Chiang Personal."

QMAKE_POST_LINK += $$_PRO_FILE_PWD_/post_build.bat $$replace(_PRO_FILE_PWD_, /, \\) $$replace(DESTDIR, /, \\)
