#include "proxerapp.h"
#include <QtRestClient>
#include <QDate>
#include <wsauthenticator.h>
#include "core.h"
#include "coremessage.h"
#include "animeinfo.h"
#include "proxer-api-key.h"
#include "jsonserializer.h"
#include "proxerapi.h"
#ifdef Q_OS_ANDROID
#include <QtAndroidExtras>
#endif

REGISTER_CORE_APP(ProxerApp)

ProxerApp::ProxerApp(QObject *parent) :
	CoreApp(parent),
	store(nullptr),
	loader(nullptr),
	mainControl(nullptr),
	statusControl(nullptr),
	passiveUpdate(false),
	showNoUpdatesInfo(false)
{
	//qRegisterMetaType<QList<AnimeInfo*>>();
	qRegisterMetaType<QMap<AnimeInfo::SeasonType, AnimeInfo::SeasonInfo>>("QMap<AnimeInfo::SeasonType, AnimeInfo::SeasonInfo>");
	QJsonSerializer::registerListConverters<AnimeInfo*>();
}

void ProxerApp::checkForSeasonUpdate(AnimeInfo *animeInfo)
{
	mainControl->updateLoadStatus(true);
	loader->checkForUpdates({animeInfo});
}

void ProxerApp::checkForSeasonUpdates()
{
	mainControl->updateLoadStatus(true);
	showNoUpdatesInfo = true;
	loader->checkForUpdates(store->loadAll());
}

void ProxerApp::showMainControl()
{
	showControl(mainControl);
	if(statusControl)
		closeControl(statusControl);
}

void ProxerApp::quitApp()
{
#ifdef Q_OS_ANDROID
	auto service = QtAndroid::androidService();
	if(service.isValid())
		service.callMethod<void>("stopSelf");
	else
#endif
	qApp->quit();
}

void ProxerApp::setupParser(QCommandLineParser &parser, bool &allowInvalid) const
{
	CoreApp::setupParser(parser, allowInvalid);
	parser.addOption({
						 {"u", "update"},
						 "SeasonProxer will start with a GUI, checking for updates passively"
					 });
}

bool ProxerApp::startApp(const QCommandLineParser &parser)
{
	if(autoShowHelpOrVersion(parser))
		return true;

	//datasync setup
	QtDataSync::Setup()
			.setSerializer(new JsonSerializer())
			.create();
	auto auth = QtDataSync::Setup::authenticatorForSetup<QtDataSync::WsAuthenticator>(this);
#ifndef QT_NO_DEBUG
	auth->setServerSecret(QStringLiteral("baum42"));
	auth->setRemoteUrl(QStringLiteral("ws://localhost:8080"));
#else
	auth->setServerSecret(SERVER_SECRET);
	auth->setRemoteUrl(QStringLiteral("wss://apps.skycoder42.de/seasonproxer"));
#endif
	auth->deleteLater();

	//anime data store
	store = new AnimeStore(this);
	connect(store, &AnimeStore::storeLoaded,
			this, &ProxerApp::storeLoaded,
			Qt::QueuedConnection);

	//fixup with additional setup for proxer rest api
	ProxerApi api;
	api.restClient()->addGlobalHeader("proxer-api-key", PROXER_API_KEY);
	api.restClient()->serializer()->setAllowDefaultNull(true);//DEBUG use this to provoke an error to test error handling

	//updater
	loader = new SeasonStatusLoader(this);
	connect(loader, &SeasonStatusLoader::animeInfoUpdated,
			store, &AnimeStore::save);

	//main control
	mainControl = new MainControl(store, this);
	connect(loader, &SeasonStatusLoader::updated,
			mainControl, &MainControl::setProgress);
	connect(loader, &SeasonStatusLoader::completed,
			this, &ProxerApp::updateDone);

	//init control flow
	passiveUpdate = parser.isSet("update");
	if(passiveUpdate) {
		statusControl = new StatusControl(this);
		connect(loader, &SeasonStatusLoader::updated,
				statusControl, &StatusControl::updateProgress);
		showControl(statusControl);
	} else
		showControl(mainControl);
	return true;
}

void ProxerApp::aboutToQuit() {}

void ProxerApp::storeLoaded()
{
	if(passiveUpdate)
		automaticUpdateCheck();
	else
		mainControl->updateLoadStatus(false);
}

void ProxerApp::updateDone(bool hasUpdates, QString errorString)
{
	mainControl->updateLoadStatus(false);

	if(passiveUpdate) {
		passiveUpdate = false;
		if(hasUpdates || !errorString.isNull()) {
			if(errorString.isNull())
				statusControl->loadUpdateStatus(store->loadAll());
			else
				statusControl->loadErrorStatus(errorString);
		} else
			quitApp();
	} else {
		if(!errorString.isNull())
			CoreMessage::critical(tr("Season check failed"), errorString);
		else if(hasUpdates)
			CoreMessage::information(tr("Season check completed"), tr("New Seasons are available!"));
		else if(showNoUpdatesInfo) {
			showNoUpdatesInfo = false;
			CoreMessage::information(tr("Season check completed"), tr("No seasons changed."));
		}
	}
}

void ProxerApp::automaticUpdateCheck()
{
	QSettings settings;
	settings.beginGroup("updates");

	auto updateList = store->loadAll();

	//check if there are still animes with updates
	bool hasUpdates = false;
	foreach (auto info, updateList) {
		if(info->hasNewSeasons()) {
			hasUpdates = true;
			break;
		}
	}

	//sort by last update check time
	std::sort(updateList.begin(), updateList.end(), [](AnimeInfo *left, AnimeInfo *right){
		return left->lastUpdateCheck() < right->lastUpdateCheck();
	});

	//reduce to allowed list size
	auto maxSize = settings.value("checkLimit", 10).toInt();
	if(maxSize > 0)
		updateList = updateList.mid(0, maxSize);

	//reduce to allowed interval
	auto interval = settings.value("autoCheck", 7).toInt();
	for(auto i = 0; i < updateList.size(); i++) {
		if(updateList[i]->lastUpdateCheck().daysTo(QDate::currentDate()) < interval) {
			updateList = updateList.mid(0, i);
			break;
		}
	}

	//quit if none have to be updated or show already updated
	if(updateList.isEmpty() || interval == 0) {
		if(hasUpdates)
			updateDone(true, {});
		else
			quitApp();
		return;
	}

	//begin the update
	qInfo() << "checking for updates of" << updateList.size() << "animes";
	loader->checkForUpdates(updateList, hasUpdates);

	settings.endGroup();
}
