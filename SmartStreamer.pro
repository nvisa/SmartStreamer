QT += core network gui widgets

TARGET = SmartStreamer
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    smartstreamer.cpp \
    moxadriver.cpp \
    mjpegserver.cpp \
    seiinserter.cpp

HEADERS += \
    smartstreamer.h \
    moxadriver.h \
    mjpegserver.h \
    seiinserter.h

websockets {
    SOURCES += websocketstreamer.cpp
    HEADERS += websocketstreamer.h
}

include (build_config.pri)
