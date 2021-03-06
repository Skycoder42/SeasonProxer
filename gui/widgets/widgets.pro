TEMPLATE = app

QT += widgets mvvmwidgets mvvmdatasyncwidgets svg concurrent

!qtHaveModule(autoupdatergui): CONFIG += no_updater
!no_updater:QT += autoupdatergui
else: DEFINES += NO_AUTO_UPDATER

TARGET = Ani-Rem

QMAKE_TARGET_PRODUCT = "$$TARGET"
RC_ICONS += ../../icn/anirem.ico
ICON = ../../icn/anirem.icns

HEADERS += mainwindow.h \
	animemodel.h \
	addanimedialog.h \
	detailsdockwidget.h \
	widgetsupdatenotifier.h \
	instancesetup.h \
	entrydialog.h \
	logindialog.h

SOURCES += main.cpp \
	mainwindow.cpp \
	animemodel.cpp \
	addanimedialog.cpp \
	detailsdockwidget.cpp \
	widgetsupdatenotifier.cpp \
	entrydialog.cpp \
	logindialog.cpp

FORMS += mainwindow.ui \
	addanimedialog.ui \
	detailsdockwidget.ui \
	entrydialog.ui \
	logindialog.ui

RESOURCES += \
	anirem-widgets.qrc

TRANSLATIONS += anirem_widgets_de.ts

no_updater: EXTRA_TRANSLATIONS += anirem_de.ts
else: EXTRA_TRANSLATIONS += installer/anirem_de.ts

DISTFILES += $$TRANSLATIONS \
	$$EXTRA_TRANSLATIONS \
	installer/config.xml \
	installer/meta/package.xml

# install / qtifw
mac: target.path = $$INSTALL_APPS
else: target.path = $$INSTALL_BINS
qpmx_ts_target.path = $$INSTALL_TRANSLATIONS
extra_ts_target.path = $$INSTALL_TRANSLATIONS
INSTALLS += target qpmx_ts_target extra_ts_target

win32 {
	for(tsfile, EXTRA_TRANSLATIONS) {
		tsBase = $$basename(tsfile)
		qpmx_ts_target_fixed.files += "$$OUT_PWD/$$replace(tsBase, \.ts, .qm)"
	}
	qpmx_ts_target_fixed.path = $$INSTALL_TRANSLATIONS
	qpmx_ts_target_fixed.CONFIG += no_check_exist
	INSTALLS += qpmx_ts_target_fixed
}

create_installer {
	QTIFW_MODE = online_all

	QTIFW_QM_DEPS = qtbase qtwebsockets qtdatasync \
		qtmvvmcore qtmvvmdatasynccore qtmvvmwidgets qtmvvmdatasyncwidgets \
		qtautoupdatergui

	qtifw_advanced_config.files += $$PWD/installer/config.xml
	CONFIG += qtifw_target qtifw_deploy_no_install

	mac: qtifw_deploy_target.path = $${INSTALL_PREFIX}

	anirem_qtifw_meta.path = $$QTIFW_PKG_ROOT
	anirem_qtifw_meta.files += $$PWD/installer/meta/
	INSTALLS += anirem_qtifw_meta

	install_icons.path = $$INSTALL_PREFIX
	install_icons.files = installer/main.png
	INSTALLS += install_icons

	# deploy extra plugins
	anirem_ds_plugins.path = $$INSTALL_PLUGINS
	anirem_ds_plugins.files += $$[QT_INSTALL_PLUGINS]/keystores
	INSTALLS += anirem_ds_plugins
} else {
	desktop_install.files = anirem.desktop
	desktop_install.path = $$INSTALL_SHARE/applications/
	install_icons.path = $$INSTALL_SHARE
	install_icons.files = icons
	linux:!android: INSTALLS += desktop_install install_icons
}

# Link with core project
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../core/release/ -lanirem-core
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../core/debug/ -lanirem-core
else:unix: LIBS += -L$$OUT_PWD/../core/ -lanirem-core

INCLUDEPATH += $$PWD/../core
DEPENDPATH += $$PWD/../core

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../core/release/libanirem-core.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../core/debug/libanirem-core.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../core/release/anirem-core.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../core/debug/anirem-core.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../core/libanirem-core.a

include(../../lib.pri)

!ReleaseBuild:!DebugBuild:!system(qpmx -d $$shell_quote($$_PRO_FILE_PWD_) --qmake-run init $$QPMX_EXTRA_OPTIONS $$shell_quote($$QMAKE_QMAKE) $$shell_quote($$OUT_PWD)): error(qpmx initialization failed. Check the compilation log for details.)
else: include($$OUT_PWD/qpmx_generated.pri)
