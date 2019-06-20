tx1 {
    HEADERS += \
	$$PWD/facealgorithmelement.h \
    SOURCES += \
	$$PWD/facealgorithmelement.cpp \
}

HEADERS += \
	$$PWD/basealgorithmelement.h \
	$$PWD/motionalgorithmelement.h \
	$$PWD/algorithmgrpcserver.h \
	$$PWD/stabilizationalgorithmelement.h \
	$$PWD/panaromaalgorithmelement.h \
	$$PWD/trackalgorithmelement.h

SOURCES += \
	$$PWD/basealgorithmelement.cpp \
	$$PWD/motionalgorithmelement.cpp \
	$$PWD/algorithmgrpcserver.cpp \
	$$PWD/stabilizationalgorithmelement.cpp \
	$$PWD/panaromaalgorithmelement.cpp \
	$$PWD/trackalgorithmelement.cpp
