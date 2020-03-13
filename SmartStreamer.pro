TEMPLATE = subdirs
CONFIG += ordered # not recommended option
SUBDIRS = \

#exists(ptzp) {
#    SUBDIRS += ptzp \
#}

#exists(lmm) {
#    SUBDIRS += lmm/lmm-demo.pro \
#}

exists(SmartStreamer) {
    SUBDIRS += SmartStreamer
}
