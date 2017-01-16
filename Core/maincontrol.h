#ifndef MAINCONTROL_H
#define MAINCONTROL_H

#include "animestore.h"
#include "control.h"
#include <objectlistmodel.h>

class MainControl : public Control
{
	Q_OBJECT

	Q_PROPERTY(GenericListModel<AnimeInfo>* animeModel READ animeModel CONSTANT)

public:
	explicit MainControl(AnimeStore *store, QObject *parent = nullptr);

	GenericListModel<AnimeInfo>* animeModel() const;

public slots:
	void reload();

	void uncheckAnime(const QModelIndex index);

	void addAnime();
	void addAnimeFromClipboard();
	void showDetails(int id);

	void removeAnime(QModelIndex index);

signals:
	void showStatus(QString message, bool permanent = false);
	void setProgress(int value, int max);

	void updateLoadStatus(bool isFinished);

private slots:
	void storeListLoaded(AnimeList list);

protected:
	void onShow();

private:
	AnimeStore *store;
	GenericListModel<AnimeInfo> *model;

	void internalSave(AnimeInfo *info);
};

#endif // MAINCONTROL_H
