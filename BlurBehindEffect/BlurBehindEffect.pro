#-------------------------------------------------
#
# Project created by QtCreator 2022-04-26T12:39:01
#
#-------------------------------------------------

CONFIG += c++1z
QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BlurBehindEffect
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

QMAKE_CXXFLAGS += -mtune=native -march=native

SOURCES += \
    main.cpp \
    widget.cpp \
    wigglywidget.cpp \
    boxblur.cpp \
    stackblur.cpp \
    glblurfunctions.cpp \
    blurbehindeffect.cpp

HEADERS += \
    widget.h \
    wigglywidget.h \
    blur.h \
    glblurfunctions.h \
    vertex.h \
    blurbehindeffect.h

RESOURCES += \
    resources.qrc
