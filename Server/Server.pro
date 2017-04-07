#-------------------------------------------------
#
# Project created by QtCreator 2016-09-19T12:29:55
#
#-------------------------------------------------

QT       += core network xmlpatterns

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Server
TEMPLATE = app


SOURCES += main.cpp\
        myserver.cpp

HEADERS  += myserver.h \
    person.h

RESOURCES += \
    res.qrc
