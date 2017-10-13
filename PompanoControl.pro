#-------------------------------------------------
#
# Project created by QtCreator 2017-07-02T13:51:43
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = PompanoControl
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    g.cpp \
    tcpctrl.cpp \
    canctrl.cpp \
    kellerctrl.cpp \
    lightctrl.cpp \
    PID.cpp \
    xsensctrl.cpp \
    altctrl.cpp \
    platformctrl.cpp

#remote debugging, copy program to pompanoo /test directory
target.path = /test
INSTALLS += target

HEADERS += \
    g.h \
    tcpctrl.h \
    canctrl.h \
    kellerctrl.h \
    lightctrl.h \
    PID.h \
    xsensctrl.h \
    altctrl.h \
    platformctrl.h
