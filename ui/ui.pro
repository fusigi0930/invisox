TEMPLATE = app

QT += qml quick widgets xml

SOURCES += main.cpp \
    cenvstore.cpp \
    cscriptstore.cpp

RESOURCES += qml.qrc

INCLUDEPATH += ../inc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    cenvstore.h \
    cscriptstore.h
