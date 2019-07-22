#ifndef FILEWRITER_H
#define FILEWRITER_H

#include <QString>
#include <mutex>

#include "simplebuffer.h"

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
	void addData(char const* data, int size);
	void save();
private:
	QString folderPath = "/tmp/";
	QString fileName = "trial.h264";
	simple_buffer<char> buffer;
	Status status = Status::NotStarted;
	std::mutex mutex;
};

#endif // FILEWRITER_H
