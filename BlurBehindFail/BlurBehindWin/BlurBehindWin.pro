#-------------------------------------------------
#
# Project created by QtCreator 2021-06-11T10:36:09
#
#-------------------------------------------------

QT       += core gui
win32 {
    QT += winextras
}

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BlurBehindWin
TEMPLATE = app


SOURCES += main.cpp\
    widget.cpp \
    dwmaccentpolicycontroller.cpp

HEADERS  += widget.h \
    dwmaccentpolicycontroller.h

