QT += core network gui widgets

TARGET = SmartStreamer
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    moxadriver.cpp \
    mjpegserver.cpp \
    seiinserter.cpp \
    proto/OrionCommunication.grpc.pb.cc \
    proto/OrionCommunication.pb.cc \
    proto/AlgorithmCommunication.grpc.pb.cc \
    proto/AlgorithmCommunication.pb.cc \
    peercheck.cpp \
    algorithmmanager.cpp \
    ipstreamer.cpp \
    algorithmelement.cpp \
    usbstreamer.cpp

HEADERS += \
    moxadriver.h \
    mjpegserver.h \
    seiinserter.h \
    proto/OrionCommunication.grpc.pb.h \
    proto/OrionCommunication.pb.h \
    proto/AlgorithmCommunication.grpc.pb.h \
    proto/AlgorithmCommunication.pb.h \
    peercheck.h \
    algorithmmanager.h \
    algorithmfunctions.h \
    ipstreamer.h \
    algorithmelement.h \
    usbstreamer.h \
    commoninformationinterface.h

websockets {
    SOURCES += websocketstreamer.cpp
    HEADERS += websocketstreamer.h
}

include (build_config.pri)

tx1 {
    LIBS += -L/usr/local/cuda/lib64 -lcudart -lcufft
    LIBS += -L/usr/lib/aarch64-linux-gnu/tegra/ -lv4l2 -lnvbuf_utils -lnvinfer -lnvparsers
}

#Add make targets for checking version info
VersionCheck.commands = @$$PWD/checkversion.sh $$PWD
VersionCheck.depends = FORCE
VersionCheck.target = version.h
QMAKE_EXTRA_TARGETS += VersionCheck
PRE_TARGETDEPS += version.h
