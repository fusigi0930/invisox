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

INCLUDEPATH += ../inc

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
