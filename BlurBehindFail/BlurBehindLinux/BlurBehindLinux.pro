#-------------------------------------------------
#
# Project created by QtCreator 2023-05-14T19:06:21
#
#-------------------------------------------------

CONFIG += c++1z

QT       += core gui x11extras

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BlurBehindLinux
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

LIBS += -lxcb -lxcb-util -lxcb-ewmh -lxcb-icccm -lX11 -ljpeg

INCLUDEPATH += ../../BlurBehindEffect

SOURCES += \
    main.cpp \
    widget.cpp \
    windowfunctions.cpp \
    xcbwindowmanager.cpp \
    ../../BlurBehindEffect/boxblur.cpp \
    ../../BlurBehindEffect/stackblur.cpp

HEADERS += \
    widget.h \
    windowfunctions.h \
    xcbwindowmanager.h \
    blur.h

