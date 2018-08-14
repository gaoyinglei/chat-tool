#-------------------------------------------------
#
# Project created by QtCreator 2016-12-20T15:33:22
#
#-------------------------------------------------

QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Client
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    logindlg.cpp \
    chat.cpp

HEADERS  += widget.h \
    logindlg.h \
    chat.h

FORMS    += widget.ui \
    logindlg.ui \
    chat.ui
