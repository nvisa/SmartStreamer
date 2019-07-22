#include "datetime.h"

#include <QDateTime>

QString DateTime::addSecsToDateTime(QString date, int secs)
{
	return QDateTime::fromString(date, QString("dd-MM-yyyy HH:mm:ss")).addMSecs(static_cast<qint64>(secs) * 1000).toString(QString("dd-MM-yyyy HH:mm:ss"));
}

QString DateTime::currentDateTime(int addSec)
{
	return QDateTime::currentDateTimeUtc().addSecs(addSec).toString(QString("dd-MM-yyyy HH:mm:ss"));
}

QString DateTime::currentDate()
{
	return QDateTime::currentDateTimeUtc().date().toString(QString("dd-MM-yyyy"));
}

QString DateTime::currentTime()
{
	return QDateTime::currentDateTimeUtc().time().toString(QString("HH:mm:ss"));
}
