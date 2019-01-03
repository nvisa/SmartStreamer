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
    proto/OrionCommunication.grpc.pb.cc \
    proto/OrionCommunication.pb.cc

HEADERS += \
    smartstreamer.h \
    moxadriver.h \
    mjpegserver.h \
    seiinserter.h \
    proto/OrionCommunication.grpc.pb.h \
    proto/OrionCommunication.pb.h

websockets {
    SOURCES += websocketstreamer.cpp
    HEADERS += websocketstreamer.h
}

include (build_config.pri)

tx1 {
    LIBS += -L/usr/local/cuda/lib64 -lcudart -lcufft
    LIBS += -L/usr/lib/aarch64-linux-gnu/tegra/ -lv4l2 -lnvbuf_utils -lnvinfer -lnvparsers
}
