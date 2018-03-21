QT += core network gui widgets websockets

TARGET = SmartStreamer
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    smartstreamer.cpp \
    websocketstreamer.cpp \
    moxadriver.cpp

HEADERS += \
    smartstreamer.h \
    websocketstreamer.h \
    moxadriver.h

include (build_config.pri)
