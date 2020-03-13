#include "usersettings.h"

#include <debug.h>

#include <QFile>
#include <QDebug>
#include <QFileInfo>
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

class OverlayedSetting
{
public:
	OverlayedSetting()
	{
	}

	void setPlatform(const QString &plat)
	{
		platform = plat;
	}

	QJsonDocument GetOverlayed(const QJsonDocument &doc)
	{
		QJsonDocument final = doc;

		QFileInfo finfo(GetFileName());
		/* first check and overlay platform settings */
		if (!platform.isEmpty()) {
			QString platfile = QString("/etc/smartstreamer/platforms/%2/%1").arg(finfo.fileName()).arg(platform);
			if (QFile::exists(platfile)) {
				QJsonDocument overlay = readJson(platfile);
				final = Overlay(final, overlay);
			}
		}

		/* then use user settings */
		QString userfile = QString("/etc/smartstreamer_user/%1").arg(finfo.fileName());
		if (QFile::exists(userfile)) {
			QJsonDocument overlay = readJson(userfile);
			final = Overlay(final, overlay);
		}

		return final;
	}

	int WriteOverlay(const QJsonDocument &doc)
	{
		QString userfile = GetFileName().replace("/etc/smartstreamer/", "/etc/smartstreamer_user/");
		return saveJson(userfile, doc);
	}

	virtual QString GetFileName() = 0;

protected:
	/**
	 * @brief Overlay
	 * @param src
	 * @param overlay
	 * @return
	 *
	 * Overlays given overlay json into given source file.
	 *
	 * If documents are objects, they are merged key-by-key.
	 *
	 * If given documents are arrays, it works on element wise.
	 * This means i'th element in overlay file is overlayed onto
	 * i'th element in source file. If array indexes doesn't
	 * correspond each other, results may be undefined. For best results, in array
	 * case both documents should contain identical number of elements.
	 *
	 */
	QJsonDocument Overlay(const QJsonDocument &src, const QJsonDocument &overlay)
	{
		if (overlay.isArray()) {
			QJsonArray arr = overlay.array();
			QJsonArray overlayed = src.array();
			for (int i = 0; i < arr.size(); i++) {
				const QJsonObject obj = arr[i].toObject();
				QJsonObject objo = overlayed[i].toObject();
				overlayObjects(objo, obj, keys);
				overlayed[i] = objo;
			}
			return QJsonDocument(overlayed);
		}

		QJsonObject obj = src.object();
		const QJsonObject objo = overlay.object();
		overlayObjects(obj, objo, keys);
		return QJsonDocument(obj);
	}

	void overlayObjects(QJsonObject &src, const QJsonObject &overlay, const QStringList &keys = QStringList())
	{
		QStringList keylist = keys;
		if (keylist.isEmpty())
			keylist = overlay.keys();
		foreach (const QString &key, keylist)
			src[key] = overlay[key];
	}

	QStringList keys; //TODO: It seems like we can get rid of this completely
	QString platform;
};

class EncoderOverlayedSetting : public OverlayedSetting
{
public:
	EncoderOverlayedSetting()
	{
		keys << "bitrate";
		keys << "frameRate";
		keys << "shaping";
		keys << "resolution";
		keys << "multicast_address_base";
	}

	QString GetFileName()
	{
		/* TODO: embedding config filename here is not nice */
		return "/etc/smartstreamer/encoders.json";
	}
};

UserSettings::UserSettings()
{
	auto s = new EncoderOverlayedSetting;
	overlays.insert(s->GetFileName(), s);
}

UserSettings &UserSettings::instance()
{
	static UserSettings s;
	return s;
}

QJsonDocument UserSettings::GetOverlayed(const QString &filename, const QJsonDocument &doc)
{
	QJsonDocument doc2 = doc;
	if (doc2.isEmpty())
		doc2 = readJson(filename);
	if (overlays.contains(filename))
		return overlays[filename]->GetOverlayed(doc2);
	return doc2;
}

int UserSettings::WriteOverlay(const QString &filename, const QJsonDocument &doc)
{
	if (overlays.contains(filename))
		return overlays[filename]->WriteOverlay(doc);
	return -ENOENT;
}

void UserSettings::setPlatform(const QString &plat)
{
	platform = plat;
	QHashIterator<QString, OverlayedSetting *> hi(overlays);
	while (hi.hasNext()) {
		hi.next();
		hi.value()->setPlatform(platform);
	}
}
