#ifndef INTERNALRECORDER_H
#define INTERNALRECORDER_H

#include <QObject>

#include <atomic>
#include <memory>

class FileWriter;

class QTimer;

class InternalRecorder final : public QObject
{
	Q_OBJECT
public:
	InternalRecorder();
	~InternalRecorder();
	void start();
	void startRecord(const char *data, int size);
	void finishRecord();
private slots:
private:
	enum class Status : int
	{
		NotStarted,
		Started,
		Recording,
		Stopped,
		NoRawData
	};
	std::atomic<Status> status{Status::NotStarted};
	std::unique_ptr<FileWriter> writer;
	std::unique_ptr<QTimer> timer;
	int recordLengthSec = 10;
};

#endif // INTERNALRECORDER_H
