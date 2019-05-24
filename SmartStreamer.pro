QT += core network gui widgets

TARGET = SmartStreamer
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    moxadriver.cpp \
    mjpegserver.cpp \
    seiinserter.cpp \
    peercheck.cpp \
    algorithmmanager.cpp \
    ipstreamer.cpp \
    algorithmelement.cpp \
    usbstreamer.cpp \
    yamgozstreamer.cpp \
    analogstreamer.cpp \
    streamercommon.cpp \
    applicationinfo.cpp \
    diagnosticserviceimpl.cpp \
    simpleipstreamer.cpp

HEADERS += \
    moxadriver.h \
    mjpegserver.h \
    seiinserter.h \
    peercheck.h \
    algorithmmanager.h \
    algorithmfunctions.h \
    ipstreamer.h \
    algorithmelement.h \
    usbstreamer.h \
    commoninformationinterface.h \
    yamgozstreamer.h \
    analogstreamer.h \
    streamercommon.h \
    applicationinfo.h \
    diagnosticserviceimpl.h \
    simpleipstreamer.h

websockets {
    SOURCES += websocketstreamer.cpp
    HEADERS += websocketstreamer.h
}

include (build_config.pri)
include (proto/grpc.pri)
include (algorithm/algorithm.pri)

tx1 {
    LIBS += -L/usr/local/cuda/lib64 -lcudart -lcufft
    LIBS += -L/usr/lib/aarch64-linux-gnu/tegra/ -lv4l2 -lnvbuf_utils -lnvinfer -lnvparsers -lnvjpeg -lEGL -lX11
}

third-party {
    LIBS += -L/home/nvidia/workspace/xDNN/Release -lxDNN
    LIBS += -L/opt/caffe-dev/lib -lcaffe
    LIBS += -lboost_system -lboost_filesystem
}

#Add make targets for checking version info
VersionCheck.commands = @$$PWD/checkversion.sh $$PWD
VersionCheck.depends = FORCE
VersionCheck.target = version.h
QMAKE_EXTRA_TARGETS += VersionCheck
PRE_TARGETDEPS += version.h

target.path = /home/ubuntu/bin/
INSTALLS += target

RESOURCES += \
    resources.qrc

DISTFILES += \
    data/algodesc.json

