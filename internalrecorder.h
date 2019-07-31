#ifndef INTERNALRECORDER_H
#define INTERNALRECORDER_H

#include <QObject>

#include <atomic>
#include <memory>

class FileWriter;
class NvrChecker;

class QTimer;

class InternalRecorder final : public QObject
{
	Q_OBJECT
public:
	InternalRecorder();
	~InternalRecorder();
	void start();
	bool isNvrDead();
	void record(const char *data, int size);
	void finishRecord();
private slots:
	void onNvrOnline();
private:
	std::unique_ptr<FileWriter> writer;
	std::unique_ptr<QTimer> timer;
	std::unique_ptr<NvrChecker> nvrChecker;
	int recordLengthSec = 10;
	bool recordingNow = false;
};

#endif // INTERNALRECORDER_H
