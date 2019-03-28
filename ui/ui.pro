TEMPLATE = app
TARGET = invisox

QT += qml quick widgets xml

SOURCES += main.cpp \
	cenvstore.cpp \
	cscriptstore.cpp \
	csettingstore.cpp \
	cossystem.cpp

RESOURCES += qml.qrc


INCLUDEPATH += $$_PRO_FILE_PWD_/../inc \
			   $$_PRO_FILE_PWD_/../auto/inc \

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
	cenvstore.h \
	cscriptstore.h \
	csettingstore.h \
	cossystem.h

CONFIG(debug, debug|release) {
	DESTDIR = $$_PRO_FILE_PWD_/../out/debug
	MOC_DIR = $$_PRO_FILE_PWD_/../out/obj/moc
	OBJECTS_DIR = $$_PRO_FILE_PWD_/../out/obj/debug

	INTERP_LIB = -L$$_PRO_FILE_PWD_/../out/debug
} else {
	DESTDIR = $$_PRO_FILE_PWD_/../out/release
	MOC_DIR = $$_PRO_FILE_PWD_/../out/obj/moc
	OBJECTS_DIR = $$_PRO_FILE_PWD_/../out/obj/release

	INTERP_LIB = -L$$_PRO_FILE_PWD_/../out/release
}

GITVER = $$system("genver.bat")
GITVER = $$replace(GITVER, "v", "")
GITVER = $$replace(GITVER, "-", ".")
GITVER = $$split(GITVER, .)
GITVER = $$member(GITVER, 0).$$member(GITVER, 1).$$member(GITVER, 2).$$member(GITVER, 3)

VERSION = $$GITVER
QMAKE_TARGET_PRODUCT = "invisOX"
QMAKE_TARGET_DESCRIPTION = "invisOX - ui interface"
QMAKE_TARGET_COPYRIGHT = "Copyright(c) 2019 ChuYuan Chiang. All rights reserved"
QMAKE_TARGET_COMPANY = "ChuYuan Chiang Personal."

LIBS += $${INTERP_LIB}
LIBS += -lluainterp
