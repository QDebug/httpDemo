#-------------------------------------------------
#
# Project created by QtCreator 2018-11-27T16:45:46
#
#-------------------------------------------------

QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = httpDemo
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    httpclient.cpp

HEADERS  += mainwindow.h \
    httpclient.h

FORMS    += mainwindow.ui

RESOURCES += \
    Resources.qrc


