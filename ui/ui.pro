TEMPLATE = app
TARGET = invisox

QT += qml quick widgets xml

SOURCES += main.cpp \
	cenvstore.cpp \
	cscriptstore.cpp \
	csettingstore.cpp \
	cossystem.cpp

win32 {
	SOURCES += cinterpenvstore_win.cpp
}


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

win32 {
	HEADERS += cinterpenvstore_win.h
}

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

LIBS += $${INTERP_LIB}
