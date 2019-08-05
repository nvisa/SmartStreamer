#include "internalrecorder.h"

#include "helper/filewriter.h"
#include "helper/nvrchecker.h"

#include <QTimer>
#include <QDebug>

InternalRecorder::InternalRecorder()
	: writer(new FileWriter("/tmp/")),
	  timer(new QTimer),
	  nvrChecker(new NvrChecker)
{
	nvrChecker->setNvrIp("192.168.50.89");
	connect(timer.get(), &QTimer::timeout, this, &InternalRecorder::finishRecord);
	connect(nvrChecker.get(), &NvrChecker::nvrOnline, this, &InternalRecorder::onNvrOnline);
}

InternalRecorder::~InternalRecorder()
{

}

void InternalRecorder::start()
{
	nvrChecker->startPing();
}

bool InternalRecorder::isNvrDead()
{
	return nvrChecker->isDead();
}

void InternalRecorder::record(char const* data, int size)
{
	if (!recordingNow) {
		recordingNow = true;
		QTimer::singleShot(recordLengthSec * 1000, this, &InternalRecorder::finishRecord);
	}
	writer->addData(data, size);
}

void InternalRecorder::finishRecord()
{
	if (recordingNow) {
		writer->save();
		recordingNow = false;
	}

}

void InternalRecorder::onNvrOnline()
{
	finishRecord();
}
