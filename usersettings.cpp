#include "usersettings.h"

#include <QFile>
#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>

static QJsonDocument readJson(const QString &filename)
{
	QFile f(filename);
	if (!f.open(QIODevice::ReadOnly))
		return QJsonDocument();
	QByteArray ba = f.readAll();
	f.close();
	QJsonDocument doc = QJsonDocument::fromJson(ba);
	return doc;
}

static int saveJson(const QString &filename, const QJsonDocument &doc)
{
	QFile f(filename);
	if (!f.open(QIODevice::WriteOnly)) {
		qDebug() << "File opening error: " << errno << filename;
		return -1;
	}
	f.write(doc.toJson());
	f.close();
	return 0;
}

UserSettings::UserSettings()
{
	encoderOverlayKeys << "bitrate";
	encoderOverlayKeys << "frameRate";
	encoderOverlayKeys << "shaping";
	encoderOverlayKeys << "resolution";
	encoderOverlayKeys << "multicast_address_base";
}

UserSettings &UserSettings::instance()
{
	static UserSettings s;
	return s;
}

QJsonDocument UserSettings::GetOverlayed(const QString &filename, const QJsonDocument &doc)
{
	if (filename.endsWith("encoders.json"))
		return GetOverlayedEncoders(doc);
	return doc;
}

int UserSettings::WriteOverlay(const QString &filename, const QJsonDocument &doc)
{
	if (filename.endsWith("encoders.json"))
		return WriteOverlayEncoders(doc);
	return -ENOENT;
}

QJsonDocument UserSettings::GetOverlayedEncoders(const QJsonDocument &doc)
{
	QJsonDocument overlay = readJson("/etc/smartstreamer_user/encoders.json");
	QJsonArray arr = overlay.array();
	QJsonArray overlayed = doc.array();
	for (int i = 0; i < arr.size(); i++) {
		const QJsonObject obj = arr[i].toObject();
		QJsonObject objo = overlayed[i].toObject();
		foreach (const QString &key, encoderOverlayKeys)
			objo[key] = obj[key];
	}

	return doc;
}

int UserSettings::WriteOverlayEncoders(const QJsonDocument &doc)
{
	QJsonArray overlayed;
	foreach (const QJsonValue &v, doc.array()) {
		const QJsonObject obj = v.toObject();
		QJsonObject objo;
		foreach (const QString &key, encoderOverlayKeys)
			objo.insert(key, obj[key]);
		overlayed.append(objo);
	}

	return saveJson("/etc/smartstreamer_user/encoders.json", QJsonDocument(overlayed));
}

