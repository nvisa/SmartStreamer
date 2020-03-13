#ifndef SIMPLEIPSTREAMER_H
#define SIMPLEIPSTREAMER_H

#include <lmm/players/basestreamer.h>

class SimpleIPStreamer : public BaseStreamer
{
public:
	SimpleIPStreamer(QObject *parent = 0);
};

#endif // SIMPLEIPSTREAMER_H
