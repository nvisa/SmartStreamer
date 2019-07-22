#ifndef DATETIME_H
#define DATETIME_H

#include <QString>

class DateTime
{
public:
	static QString addSecsToDateTime(QString date, int secs);
	static QString currentDateTime(int addSecs = 0);
	static QString currentDate();
	static QString currentTime();
};

#endif // DATETIME_H
