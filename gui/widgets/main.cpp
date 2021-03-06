#include <QtWidgets/QApplication>
#include <QtMvvmWidgets/WidgetsPresenter>
#include <QtMvvmDataSyncWidgets/qtmvvmdatasyncwidgets_global.h>
#include <QSvgRenderer>
#include <aniremapp.h>
#include <qsingleinstance.h>

#include "mainwindow.h"
#include "addanimedialog.h"
#include "detailsdockwidget.h"
#include "entrydialog.h"
#include "logindialog.h"
#include "widgetsupdatenotifier.h"
#include "instancesetup.h"

// Register the core app
QTMVVM_REGISTER_CORE_APP(AniRemApp)

namespace {

QPointer<QMainWindow> currentMainWindow;
QPointer<QSingleInstance> currentInstance;

}

int main(int argc, char *argv[])
{
	QtMvvm::CoreApp::disableAutoBoot();

	QApplication a(argc, argv);

	QSingleInstance instance;
	currentInstance = &instance;
	instance.setStartupFunction([&](){
		QtMvvm::ServiceRegistry::instance()->registerInterface<IUpdateNotifier, WidgetsUpdateNotifier>();

		QtMvvm::registerDataSyncWidgets();
		QtMvvm::WidgetsPresenter::registerView<MainWindow>();
		QtMvvm::WidgetsPresenter::registerView<AddAnimeDialog>();
		QtMvvm::WidgetsPresenter::registerView<DetailsDockWidget>();
		QtMvvm::WidgetsPresenter::registerView<EntryDialog>();
		QtMvvm::WidgetsPresenter::registerView<LoginDialog>();

		QObject::connect(&instance, &QSingleInstance::instanceMessage,
						 coreApp, [](){
			if(!currentMainWindow)
				coreApp->show<MainViewModel>();
		});

		//workaround to force svg deployment
		QSvgRenderer _unused;

		coreApp->bootApp();
		return EXIT_SUCCESS;
	});

	return instance.singleExec();
}

void setMainWindow(QMainWindow *window)
{
	currentMainWindow = window;
	if(currentInstance)
		currentInstance->setNotifyWindow(window);
}

QMainWindow *getMainWindow()
{
	return currentMainWindow;
}
