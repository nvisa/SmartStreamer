#ifndef USERSETTINGS_H
#define USERSETTINGS_H

#include <QStringList>
#include <QJsonDocument>

class UserSettings
{
public:
	static UserSettings & instance();

	QJsonDocument GetOverlayed(const QString &filename, const QJsonDocument &doc);
	int WriteOverlay(const QString &filename, const QJsonDocument &doc);

protected:
	UserSettings();
	QJsonDocument GetOverlayedEncoders(const QJsonDocument &doc);

	int WriteOverlayEncoders(const QJsonDocument &doc);

private:
	QStringList encoderOverlayKeys;
};

#endif // USERSETTINGS_H
