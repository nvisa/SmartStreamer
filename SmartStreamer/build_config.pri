CUTEVIDEO=$$PWD/..
INCLUDEPATH += $$CUTEVIDEO/lmm
INCLUDEPATH += $$CUTEVIDEO/ptzp

INSTALL_PREFIX=$$OUT_PWD/..
LIBS += $$INSTALL_PREFIX/lmm/lmm/liblmm.a
LIBS += $$INSTALL_PREFIX/ptzp/libptzp.a

LIBS += -lx264 -lyuv
LIBS += -L$$INSTALL_PREFIX/usr/local/lib/ -lavformat -lswscale -lavcodec -lavutil -lswresample
LIBS += -lgstreamer-1.0 -lgobject-2.0 -lglib-2.0 -lgstbase-1.0 -lgstapp-1.0 -lz -llzma -lva
LIBS += /usr/local/lib/libgpu.a
LIBS += /usr/local/lib/lib_smart_gpu.a
LIBS += -ldl
LIBS += -L/usr/local/cuda/lib64 -lcudart -lcufft -lcublas
LIBS += -lopencv_core \
    -lopencv_imgproc \
    -lopencv_highgui \
    -lopencv_ml \
    -lopencv_video \
    -lopencv_imgcodecs

DEFINES += HAVE_TX1 HAVE_LIBYUV
DEFINES += DEBUG INFO LOG LOGV DEBUG_TIMING

CONFIG += smart-grpc c++11 tx1 third-party
