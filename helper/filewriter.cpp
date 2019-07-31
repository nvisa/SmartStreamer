#include "filewriter.h"

#include <algorithm>
#include <utility>

#include <QElapsedTimer>
#include <QDataStream>
#include <QObject>
#include <QTimer>
#include <QDebug>
#include <QFile>

#include "datetime.h"
#include "videoinfo.h"

#define BUFFER_SEGMENT_SIZE 1024000

namespace helper
{

static void write(simple_buffer<char>& buffer, QString const& outputFile)
{
	QFile file(outputFile);
	if (file.open(QIODevice::Append)) {
		auto data = buffer.flush();
		file.write(data.first, data.second);
	} else {
		qDebug() << "Can't open file" << outputFile;
	}
}

static void addData(simple_buffer<char>& buffer, char const * data, unsigned long long size, QString const& outputFile)
{
	auto dataLeft = buffer.try_add(data, size);
	if (size != 0) {
		write(buffer, outputFile);
		addData(buffer, dataLeft, size, outputFile);
	}
}

}

FileWriter::FileWriter(QString saveFolderPath)
	: folderPath(std::move(saveFolderPath)),
	  buffer(BUFFER_SEGMENT_SIZE),
	  mutex(),
	  fpsCounter(0),
	  timer(new QTimer)
{
	timer->start(1000);
	QObject::connect(timer.get(), &QTimer::timeout, [this]()
	{
		qDebug() << "fps_counter:" << fpsCounter;
		fpsCounter = 0;
	});
}

FileWriter::~FileWriter()
{

}

void FileWriter::addData(char const* data, int size)
{
	std::lock_guard<std::mutex> lock(mutex);
	QFile file("/tmp/test");
	file.open(QIODevice::Append);
	QDataStream stream(&file);
	stream << QByteArray(data, size);
	++fpsCounter;
	if (status != Status::Recording) {
		fileName = DateTime::currentDateTime();
		status = Status::Recording;
	}
	helper::addData(buffer, data, static_cast<unsigned long long>(size), folderPath + fileName);
}

void FileWriter::addData(char const* data, unsigned long long size)
{
	std::lock_guard<std::mutex> lock(mutex);
	QFile file("/tmp/test");
	file.open(QIODevice::Append);
	QDataStream stream(&file);
	stream << QByteArray(data, size);
	++fpsCounter;
	if (status != Status::Recording) {
		fileName = DateTime::currentDateTime();
		status = Status::Recording;
	}
	helper::addData(buffer, data, size, folderPath + fileName);
}

void FileWriter::save()
{
	std::lock_guard<std::mutex> lock(mutex);
	status = Status::Finished;
	QString oldFileName = folderPath + fileName;
	helper::write(buffer, oldFileName);
	QFile file(oldFileName);
	QString newFileName = folderPath + fileName + " - " + DateTime::currentDateTime() + ".h264";
	file.rename(newFileName);
	file.close();
}
