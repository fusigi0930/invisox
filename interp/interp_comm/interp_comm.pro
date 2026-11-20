QT       += network
QT       += core gui widgets opengl testlib

TARGET = interp_comm
TEMPLATE = lib

INCLUDEPATH += ../../inc

DEFINES += INTERP_COMM_LIBRARY

SOURCES += interp_comm.cpp

HEADERS += interp_comm.h \
        interp_comm_global.h

CONFIG += c++17

QMAKE_CXXFLAGS += \
    -fpermissive


unix {
    target.path = /usr/lib
    INSTALLS += target

}
win32 {
	CONFIG += dll
	#QMAKE_LFLAGS+=-Wl,-e,_DllMainCRTStartup@12
}

CONFIG(debug, debug|release) {
	DESTDIR = $$_PRO_FILE_PWD_/../../out/debug
	MOC_DIR = $$_PRO_FILE_PWD_/../../out/obj/moc
	OBJECTS_DIR = $$_PRO_FILE_PWD_/../../out/obj/debug
} else {
	DESTDIR = $$_PRO_FILE_PWD_/../../out/release
	MOC_DIR = $$_PRO_FILE_PWD_/../../out/obj/moc
	OBJECTS_DIR = $$_PRO_FILE_PWD_/../../out/obj/release
}

CV_INC = $$_PRO_FILE_PWD_/opencv/include
CV_LIB = -L$$_PRO_FILE_PWD_/opencv/x64/mingw/staticlib \
		-lopencv_highgui460 \
		-lopencv_objdetect460 \
		-lopencv_imgcodecs460 \
		-lopencv_imgproc460 \
		-lopencv_core460 \
		-llibpng \
		-lliblibjpeg-turbo \
		-llibopenjp2 \
		-lIlmImf \
		-llibwebp \
		-lzlib \
		-llibtiff

DXGI_LIB = -ld3d11 -ldxgi

INCLUDEPATH += $$CV_INC
win32 {
	LIBS += $${CV_LIB} \
		-L"$$[QT_INSTALL_LIBS]" -lQt5OpenGL -lQt5Widgets -lQt5Gui -lQt5Test -lQt5Core \
		$${DXGI_LIB} \
		-lgdi32 -lopengl32
} else {
	LIBS += $${CV_LIB}
}

GITVER = $$system("genver.bat")
GITVER = $$replace(GITVER, "v", "")
GITVER = $$replace(GITVER, "-", ".")
GITVER = $$split(GITVER, .)
GITVER = $$member(GITVER, 0).$$member(GITVER, 1).$$member(GITVER, 2).$$member(GITVER, 3)

VERSION = $$GITVER
CONFIG += skip_target_version_ext
QMAKE_TARGET_PRODUCT = "invisOX"
QMAKE_TARGET_DESCRIPTION = "invisOX - interpreter common libery"
QMAKE_TARGET_COPYRIGHT = "Copyright(c) 2026 ChuYuan Chiang. All rights reserved"
QMAKE_TARGET_COMPANY = "ChuYuan Chiang Personal."

QMAKE_POST_LINK += $$_PRO_FILE_PWD_/post_build.bat $$replace(_PRO_FILE_PWD_, /, \\) $$replace(DESTDIR, /, \\)
