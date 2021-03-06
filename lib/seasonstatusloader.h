#ifndef SEASONSTATUSLOADER_H
#define SEASONSTATUSLOADER_H

#include <QObject>
#include <QQueue>
#include <QtMvvmCore/Injection>
#include "lib_anirem_global.h"
#include "libanirem.h"
#include "infoclass.h"
#include "syncedsettings.h"

class LIB_ANIREM_EXPORT SeasonStatusLoader : public QObject
{
	Q_OBJECT

	QTMVVM_INJECT_PROP(SyncedSettings*, settings, _settings)

public:
	Q_INVOKABLE explicit SeasonStatusLoader(QObject *parent = nullptr);

	bool isUpdating() const;
	void preClean();

public slots:
	void checkForUpdates(bool useInterval = false);
	void checkForUpdates(const AnimeInfo &info);

signals:
	void started();
	void updated(int value, int max);
	void completed(bool hasUpdates, const QString &errorString = {});

private slots:
	void checkNext();
	void error(const QString &errorString, int errorCode, QtRestClient::RestReply::ErrorType errorType);

private:
	AniremStore *_store;
	InfoClass *_infoClass;
	SyncedSettings *_settings;

	QQueue<AnimeInfo> _updateQueue;
	int _lastMax;
	bool _anyUpdated;

	void addInfos(const QList<AnimeInfo> &infos);
};

#endif // SEASONSTATUSLOADER_H
