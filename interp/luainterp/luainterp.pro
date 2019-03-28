#-------------------------------------------------
#
# Project created by QtCreator 2014-09-16T14:44:11
#
#-------------------------------------------------

QT       += network opengl svg multimedia xml sql

QT       -= gui

TARGET = luainterp
TEMPLATE = lib

INCLUDEPATH += ../../inc \
        $$_PRO_FILE_PWD_/lua-5.3.2/src \
        $$_PRO_FILE_PWD_/opencv/inc

DEFINES += LUAINTERP_LIBRARY

SOURCES += cluainterpreter.cpp \
    ../../inc/cbaseinterp.cpp \
    clua.cpp \
    cluafunc.cpp \
    copencv.cpp \
    crunluathread.cpp

HEADERS += cluainterpreter.h\
        luainterp_global.h \
    ../../inc/cbaseinterp.h \
    clua.h \
    crunluathread.h \
    copencv.h

INCLUDEPATH += \
    $$_PRO_FILE_PWD_/lua-5.3.2/src \
    $$_PRO_FILE_PWD_/opencv/inc

QMAKE_CXXFLAGS += \
    -fpermissive

CONFIG(debug, debug|release) {
	DESTDIR = $$_PRO_FILE_PWD_/../../out/debug
	MOC_DIR = $$_PRO_FILE_PWD_/../../out/obj/moc
	OBJECTS_DIR = $$_PRO_FILE_PWD_/../../out/obj/debug
} else {
	DESTDIR = $$_PRO_FILE_PWD_/../../out/release
	MOC_DIR = $$_PRO_FILE_PWD_/../../out/obj/moc
	OBJECTS_DIR = $$_PRO_FILE_PWD_/../../out/obj/release
}


unix {
    target.path = /usr/lib
    INSTALLS += target

}
win32 | win64 {
	CONFIG += dll warn_off
#	QMAKE_LFLAGS+=-Wl,-e,_DllMainCRTStartup@12
	LIBS += \
    -L$$_PRO_FILE_PWD_/lua-5.3.2/src \
    -L$$_PRO_FILE_PWD_/opencv/lib64 \
    -L$$DESTDIR \
    -llua

	LIBS += \
		-llibpng \
		-llibjpeg \
		-lzlib \
		-llibjasper \
		-llibtiff \
		-lIlmImf \

# remove 32 bits libraries
#	LIBS += \
#		-lopencv_core2413.dll \
#		-lopencv_highgui2413.dll \
#		-lopencv_imgproc2413.dll \
#		-lopencv_objdetect2413.dll

# add 64 bits
    LIBS += \
        -lopencv_core243.dll \
        -lopencv_highgui243.dll \
        -lopencv_imgproc243.dll \
        -lopencv_objdetect243.dll


	LIBS += \
		-lgdi32

	QMAKE_POST_LINK += $$_PRO_FILE_PWD_/post_build.bat $$replace(_PRO_FILE_PWD_, /, \\) $$replace(DESTDIR, /, \\)
    lua_src.commands = mingw32-make -C $$_PRO_FILE_PWD_/lua-5.3.2 mingw
    lua_inst.commands = mingw32-make -C $$_PRO_FILE_PWD_/lua-5.3.2 mingw
}

GITVER = $$system("genver.bat")
GITVER = $$replace(GITVER, "v", "")
GITVER = $$replace(GITVER, "-", ".")
GITVER = $$split(GITVER, .)
GITVER = $$member(GITVER, 0).$$member(GITVER, 1).$$member(GITVER, 2).$$member(GITVER, 3)

VERSION = $$GITVER
CONFIG += skip_target_version_ext
QMAKE_TARGET_PRODUCT = "invisOX"
QMAKE_TARGET_DESCRIPTION = "invisOX - lua interpreter"
QMAKE_TARGET_COPYRIGHT = "Copyright(c) 2019 ChuYuan Chiang. All rights reserved"
QMAKE_TARGET_COMPANY = "ChuYuan Chiang Personal."

lua_src.target = lua_src
lua_inst.target = lua_inst

QMAKE_EXTRA_TARGETS += lua_src
PRE_TARGETDEPS = lua_src

