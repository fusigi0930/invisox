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

# cling environment
CLING_ROOT = $$_PRO_FILE_PWD_/../../lib/cling
CLING_INC = $${CLING_ROOT}/include
CLING_LIB = $${CLING_ROOT}/lib
CLING_BIN = $${CLING_ROOT}/bin
CLING_LIBS = -L$${CLING_LIB} \
			-lclingUserInterface \
			-lclingMetaProcessor \
			-lclingInterpreter \
			-lclingUITextInput \
			-lclingUtils \
			-lclangFrontend \
			-lclangSerialization \
			-lclangDriver \
			-lclangCodeGen \
			-lclangParse \
			-lclangSema \
			-lclangEdit \
			-lclangAnalysis \
			-lclangAST \
			-lclangLex \
			-lclangBasic

# llvm environment
LLVM_LIB = $$system($${CLING_BIN}/llvm-config --libs) \
		   $$system($${CLING_BIN}/llvm-config --system-libs)

LLVM_CXXFLAGS = $$system($${CLING_BIN}/llvm-config --cxxflags) -Wno-unused-parameter -Wno-strict-aliasing

# clang environment
CLANG_INC = #$${CLING_LIB}/clang/3.6.0/include

unix {
    target.path = /usr/lib
    INSTALLS += target

}
win32 {

}

INCLUDEPATH += $${CLANG_INC} \
				$${CLING_INC}

QMAKE_CXXFLAGS += $${LLVM_CXXFLAGS}

LIBS += $${CLING_LIBS}
LIBS += $${LLVM_LIB}
