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
    streamercommon.cpp \
    applicationinfo.cpp \
    diagnosticserviceimpl.cpp \
    snapshotelement.cpp \
    indevicetest.cpp \
    alarmgeneratorelement.cpp \
    kardelenapi.cpp \
    usersettings.cpp
HEADERS += \
    moxadriver.h \
    mjpegserver.h \
    seiinserter.h \
    peercheck.h \
    commoninformationinterface.h \
    streamercommon.h \
    applicationinfo.h \
    diagnosticserviceimpl.h \
    snapshotelement.h \
    indevicetest.h \
    alarmgeneratorelement.h \
    kardelenapi.h \
    algorithmparameters.h \
    usersettings.h

websockets {
    SOURCES += websocketstreamer.cpp
    HEADERS += websocketstreamer.h
}

include (platform.pri)
include (build_config.pri)
include (proto/grpc.pri)
include (algorithm/algorithm.pri)

videoRecorder {
	SOURCES += helper/datetime.cpp \
		helper/filewriter.cpp \
		helper/nvrchecker.cpp \
		internalrecorder.cpp

	HEADERS += helper/datetime.h \
		helper/filewriter.h \
		helper/simplebuffer.h \
		helper/nvrchecker.h \
		internalrecorder.h
}

tx1 {
	SOURCES += \
		ipstreamer.cpp \
		usbstreamer.cpp \
		yamgozstreamer.cpp \
		analogstreamer.cpp \
		simpleipstreamer.cpp \
		simpleapiserver.cpp \
		tx1streamer.cpp \
		tbgthstreamer.cpp \
		videotestsourcestreamer.cpp \

	HEADERS += \
		ipstreamer.h \
		usbstreamer.h \
		yamgozstreamer.h \
		analogstreamer.h \
		simpleipstreamer.h \
		simpleapiserver.h \
		tx1streamer.h \
		tbgthstreamer.h \
		videotestsourcestreamer.h \

	LIBS += -L/usr/local/cuda/lib64 -lcudart -lcufft
	LIBS += -L/usr/lib/aarch64-linux-gnu/tegra/ -lv4l2 -lnvbuf_utils -lnvinfer -lnvparsers -lnvjpeg -lEGL -lX11

	LIBS += -lopencv_imgcodecs -lboost_system -lboost_filesystem
	CONFIG += third-party
	DEFINES += HAVE_TX1 HAVE_VIA_MOTION HAVE_VIA_STABILIZATION HAVE_VIA_TRACK
}

tk1 {
	SOURCES += \
		aryastreamer.cpp \
		orioncommunicationserver.cpp \
		flirstreamer.cpp \
		tk1omxpipeline.cpp \
		moxak1streamer.cpp \
		tk1streamer.cpp


	HEADERS += \
		aryastreamer.h \
		orioncommunicationserver.h \
		flirstreamer.h \
		tk1omxpipeline.h \
		moxak1streamer.h \
		tk1streamer.h

	LIBS += -lvdpau -lX11 -lXv -lva-drm -lva-x11
	INCLUDEPATH += /usr/include/gstreamer-1.0
	INCLUDEPATH += /usr/include/glib-2.0
	INCLUDEPATH += /usr/lib/arm-linux-gnueabihf/glib-2.0/include
	DEFINES += HAVE_TK1 HAVE_VIA_MOTION HAVE_VIA_PANAROMA
}

metrics {
    include($$PWD/../../metricslib/metrics.pri)
    DEFINES += HAVE_METRICS_PROMETHEUS
}

deprecated {
    SOURCES += \
        algorithmmanager.cpp \
        algorithmelement.cpp \

    HEADERS += \
        algorithmmanager.h \
        algorithmelement.h \
        algorithmfunctions.h \
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

target.path = /usr/local/bin/
INSTALLS += target

RESOURCES += \
    resources.qrc

DISTFILES += \
    data/algodesc.json \
    build_config.pri

