#-------------------------------------------------
#
# Project created by QtCreator 2016-11-13T23:52:38
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Thrillide
TEMPLATE = app

QMAKE_LIBS+=-lgomp
QMAKE_CXXFLAGS+=-fopenmp

SOURCES += main.cpp\
        mainwindow.cpp \
    model.cpp \
    geom.cpp \
    game.cpp \
    render.cpp \
    tga.cpp

HEADERS  += mainwindow.h \
    model.h \
    geom.h \
    game.h \
    render.h \
    tga.h

FORMS    += mainwindow.ui
