#include <QtWidgets/QApplication>
#include <QtMvvmWidgets/WidgetsPresenter>
#include <QtMvvmDataSyncWidgets/qtmvvmdatasyncwidgets_global.h>
#include <aniremapp.h>

#include "mainwindow.h"

// Register the core app
QTMVVM_REGISTER_CORE_APP(AniRemApp)

int main(int argc, char *argv[])
{
	//TODO test without icon theme
	QApplication a(argc, argv);

	// Automatically sets "WidgetPresenter" as presenter and registers the %{WindowName} class as a widget
	// The viewmodel this widget belongs to is detected automatically via naming conventions
	QtMvvm::WidgetsPresenter::registerView<MainWindow>();
	QtMvvm::registerDataSyncWidgets();

	return a.exec();
}