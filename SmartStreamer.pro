QT += core network gui widgets

TARGET = SmartStreamer
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    smartstreamer.cpp \
    moxadriver.cpp \
    mjpegserver.cpp \
    seiinserter.cpp \
    proto/config.grpc.pb.cc \
    proto/config.pb.cc

HEADERS += \
    smartstreamer.h \
    moxadriver.h \
    mjpegserver.h \
    seiinserter.h \
    proto/config.grpc.pb.h \
    proto/config.pb.h

websockets {
    SOURCES += websocketstreamer.cpp
    HEADERS += websocketstreamer.h
}

include (build_config.pri)
