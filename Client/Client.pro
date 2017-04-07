#-------------------------------------------------
#
# Project created by QtCreator 2016-09-19T19:43:46
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Client
TEMPLATE = app


SOURCES += main.cpp \
    start.cpp \
    admin.cpp \
    worker.cpp \
    addworker.cpp

HEADERS  += \
    start.h \
    admin.h \
    worker.h \
    person.h \
    addworker.h \
    mytabbar.h

FORMS    += \
    start.ui \
    admin.ui \
    worker.ui \
    addworker.ui
