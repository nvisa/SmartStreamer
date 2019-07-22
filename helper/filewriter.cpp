#include "filewriter.h"

#include <algorithm>
#include <utility>

#include <QDebug>
#include <QFile>

#include "datetime.h"

#define BUFFER_SEGMENT_SIZE 8192

namespace helper
{

static void write(simple_buffer<char>& buffer, QString const& outputFile)
{
	QFile file(outputFile);
	if (file.open(QIODevice::Append)) {
		auto data = buffer.flush();
		file.write(data.first.get(), data.second);
	} else {
		qDebug() << "Can't open file" << outputFile;
	}
}

static void addData(simple_buffer<char>& buffer, char const * data, unsigned long long size, QString const& outputFile)
{
	auto trial = buffer.try_add(data, size);
	if (!trial.first) {
		write(buffer, outputFile);
		addData(buffer, trial.second, size, outputFile);
	}
}

}

FileWriter::FileWriter(QString saveFolderPath)
	: folderPath(std::move(saveFolderPath)),
	  buffer(BUFFER_SEGMENT_SIZE),
	  mutex() {}

void FileWriter::addData(char const* data, int size)
{
	std::lock_guard<std::mutex> lock(mutex);
	if (status != Status::Recording) {
		fileName = DateTime::currentDateTime();
		status = Status::Recording;
	}
	helper::addData(buffer, data, static_cast<unsigned long long>(size), folderPath + fileName);
}

void FileWriter::save()
{
	std::lock_guard<std::mutex> lock(mutex);
	qDebug() << "In save";
	status = Status::Finished;
	QFile file(folderPath + fileName);
	file.rename(folderPath + fileName + " - " + DateTime::currentDateTime() + ".h264");
}
