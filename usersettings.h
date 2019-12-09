#ifndef USERSETTINGS_H
#define USERSETTINGS_H

#include <QHash>
#include <QStringList>
#include <QJsonDocument>

class OverlayedSetting;

class UserSettings
{
public:
	static UserSettings & instance();

	QJsonDocument GetOverlayed(const QString &filename, const QJsonDocument &doc = QJsonDocument());
	int WriteOverlay(const QString &filename, const QJsonDocument &doc);
	void setPlatform(const QString &plat);

protected:
	UserSettings();

	int WriteOverlayEncoders(const QJsonDocument &doc);

private:
	QString platform;
	QHash<QString, OverlayedSetting *> overlays;
};

#endif // USERSETTINGS_H
