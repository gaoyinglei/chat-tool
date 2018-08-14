#-------------------------------------------------
#
# Project created by QtCreator 2016-12-21T09:32:34
#
#-------------------------------------------------

QT       += core gui
QT       +=network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Server
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    userinfo.cpp

HEADERS  += widget.h \
    userinfo.h

FORMS    += widget.ui
