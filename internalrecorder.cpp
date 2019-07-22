#include "internalrecorder.h"

#include "helper/filewriter.h"

#include <QTimer>

InternalRecorder::InternalRecorder()
	: writer(new FileWriter("/tmp/")),
	  timer(new QTimer)
{
	connect(timer.get(), &QTimer::timeout, this, &InternalRecorder::finishRecord);
}

InternalRecorder::~InternalRecorder()
{

}

void InternalRecorder::start()
{
	timer->start(recordLengthSec * 1000);
	status.store(Status::Recording);
}

void InternalRecorder::startRecord(char const* data, int size)
{
	if (status.load() == Status::Recording)
		writer->addData(data, size);
}

void InternalRecorder::finishRecord()
{
	qDebug() << "In timeout";
	if (status.load() == Status::Recording)
		writer->save();

}
