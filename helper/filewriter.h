#ifndef FILEWRITER_H
#define FILEWRITER_H

#include <QString>
#include <mutex>

#include <memory>

#include "simplebuffer.h"

class QTimer;

class FileWriter
{
public:
	enum class IOType
	{
		Append,
		StartNew,
		Finish
	};
	enum class Status
	{
		NotStarted,
		Recording,
		Finished
	};
	explicit FileWriter(QString saveFolderPath);
	~FileWriter();
	void addData(const char *data, int size);
	void addData(const char *data, unsigned long long size);
	void save();
private:
	QString folderPath = "/tmp/";
	QString fileName = "trial.h264";
	simple_buffer<char> buffer;
	Status status = Status::NotStarted;
	std::mutex mutex;
	unsigned int fpsCounter;
	std::unique_ptr<QTimer> timer;
};

#endif // FILEWRITER_H
