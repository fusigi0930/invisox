#-------------------------------------------------
#
# Project created by QtCreator 2014-09-16T14:44:11
#
#-------------------------------------------------

QT       += network

QT       -= gui

TARGET = luainterp
TEMPLATE = lib

INCLUDEPATH += ../../inc \
        $$_PRO_FILE_PWD_/lua-5.3.2/src \
        $$_PRO_FILE_PWD_/opencv/inc

DEFINES += LUAINTERP_LIBRARY

SOURCES += cluainterpreter.cpp \
    ../../inc/cbaseinterp.cpp

HEADERS += cluainterpreter.h\
        luainterp_global.h \
    ../../inc/cbaseinterp.h

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
win32 {
	CONFIG += dll warn_off
	QMAKE_LFLAGS+=-Wl,-e,_DllMainCRTStartup@12
	LIBS += \
    -L$$_PRO_FILE_PWD_/lua-5.3.2/src \
    -L$$_PRO_FILE_PWD_/opencv/lib \
    -llua

	LIBS += \
		-llibpng \
		-llibjpeg \
		-lzlib \
		-llibjasper \
		-llibtiff \
		-lIlmImf \

	LIBS += \
		-lopencv_core2413.dll \
		-lopencv_highgui2413.dll \
		-lopencv_imgproc2413.dll \
		-lopencv_objdetect2413.dll

	LIBS += \
		-lgdi32

	QMAKE_POST_LINK += $$_PRO_FILE_PWD_/post_build.bat $$replace(_PRO_FILE_PWD_, /, \\) $$replace(DESTDIR, /, \\)
    lua_src.commands = mingw32-make -C $$_PRO_FILE_PWD_/lua-5.3.2 mingw
    lua_inst.commands = mingw32-make -C $$_PRO_FILE_PWD_/lua-5.3.2 mingw		
}


lua_src.target = lua_src
lua_inst.target = lua_inst


