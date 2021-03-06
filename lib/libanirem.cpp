#include "libanirem.h"

#include <QCoreApplication>
#include <QJsonSerializer>
#include <QLibraryInfo>
#include <QLocale>
#include <QTranslator>
#include <QtMvvmCore/ServiceRegistry>
#include <datasyncsettingsaccessor.h>

#include "jsonseasondataconverter.h"
#include "animeinfo.h"
#include "proxerapi.h"
#include "syncedsettings.h"
#include "iupdatenotifier.h"
#include "seasonstatusloader.h"
#include "storedcookiejar.h"

void AniRem::prepareTranslations()
{
	auto translator = new QTranslator(qApp);
	if(translator->load(QLocale(),
						QStringLiteral("anirem"),
						QStringLiteral("_"),
						QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
		qApp->installTranslator(translator);
}

void AniRem::setup(QtDataSync::Setup &setup, bool passive)
{
	setup.setRemoteConfiguration({QStringLiteral("wss://apps.skycoder42.de/datasync/")})
			.serializer()->addJsonTypeConverter<JsonSeasonDataConverter>();
	if(passive)
		setup.setRemoteObjectHost(QStringLiteral("local:de.skycoder42.anirem.daemon"));

	//also: setup API
	ProxerApi api;
	api.restClient()->serializer()->setAllowDefaultNull(true);
	api.restClient()->manager()->setCookieJar(new StoredCookieJar());
	if(LocalSettings::instance()->account.token.isSet())
		api.restClient()->addGlobalHeader("proxer-api-token", LocalSettings::instance()->account.token.get().toUtf8());
}

void AniRem::setProxerToken(const QString &token, LocalSettings *settings)
{
	if(token.isEmpty()) {
		settings->account.token.reset();
		ProxerApi api;
		api.restClient()->removeGlobalHeader("proxer-api-token");
	} else {
		settings->account.token = token;
		ProxerApi api;
		api.restClient()->addGlobalHeader("proxer-api-token", token.toUtf8());
	}
}



namespace {

void cleanSettings()
{
	try {
		//WORKAROUND for settings destruction bug
		auto accessor = dynamic_cast<DataSyncSettingsAccessor*>(SyncedSettings::instance()->accessor());
		if(accessor)
			delete accessor;
		qDebug() << "Cleaned settings";
	} catch(QException &e) {
		qCritical() << "Failed to clean settings:" << e.what();
	}

	try {
		//WORKAROUND for settings destruction bug
		auto loader = QtMvvm::ServiceRegistry::instance()->service<SeasonStatusLoader>();
		if(loader)
			loader->preClean();
		qDebug() << "Cleaned status loader";
	} catch(QException &e) {
		qCritical() << "Failed to status loader:" << e.what();
	}
}

void setupAniRemLib()
{
	qRegisterMetaType<AnimeInfo::SeasonMap>("AnimeInfo::SeasonMap");

	QJsonSerializer::registerListConverters<AnimeInfo>();

	QtMvvm::registerInterfaceConverter<IUpdateNotifier>();
	QtMvvm::ServiceRegistry::instance()->registerObject<SeasonStatusLoader>();

	qAddPostRoutine(cleanSettings);
}

}
Q_COREAPP_STARTUP_FUNCTION(setupAniRemLib)
