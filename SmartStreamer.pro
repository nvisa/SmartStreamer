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
    proto/config.pb.cc \
    proto/camback.pb.cc \
    proto/camback.grpc.pb.cc

HEADERS += \
    smartstreamer.h \
    moxadriver.h \
    mjpegserver.h \
    seiinserter.h \
    proto/config.grpc.pb.h \
    proto/config.pb.h \
    proto/camback.pb.h \
    proto/camback.grpc.pb.h

websockets {
    SOURCES += websocketstreamer.cpp
    HEADERS += websocketstreamer.h
}

include (build_config.pri)
