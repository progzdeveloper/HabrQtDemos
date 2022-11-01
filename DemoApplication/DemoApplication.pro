#-------------------------------------------------
#
# Project created by QtCreator 2022-07-01T00:11:14
#
#-------------------------------------------------

CONFIG += c++1z
QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DemoApplication
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

INCLUDEPATH += \
    ../MultiLayerWindow \
    ../BlurBehindEffect

SOURCES += \
    ../BlurBehindEffect/blurbehindeffect.cpp \
    ../BlurBehindEffect/glblurfunctions.cpp \
    ../BlurBehindEffect/boxblur.cpp \
    ../BlurBehindEffect/stackblur.cpp \
    ../ShapedWidget/shapedwidget.cpp \
    ../CustomButton/custombutton.cpp \
    ../MultiLayerWindow/contentwidget.cpp \
    ../MultiLayerWindow/painttool.cpp \
    ../MultiLayerWindow/brushpreview.cpp \
    ../MultiLayerWindow/coloredit.cpp \
    controlpanel.cpp \
    popuppanel.cpp \
    overlaypanel.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    ../BlurBehindEffect/blurbehindeffect.h \
    ../BlurBehindEffect/glblurfunctions.h \
    ../BlurBehindEffect/boxblur.h \
    ../BlurBehindEffect/stackblur.h \
    ../BlurBehindEffect/vertex.h \
    ../ShapedWidget/shapedwidget.h \
    ../CustomButton/custombutton.h \
    ../MultiLayerWindow/contentwidget.h \
    ../MultiLayerWindow/painttool.h \
    ../MultiLayerWindow/brushpreview.h \
    ../MultiLayerWindow/coloredit.h \
    controlpanel.h \
    popuppanel.h \
    overlaypanel.h \
    mainwindow.h


RESOURCES += \
    resources.qrc
