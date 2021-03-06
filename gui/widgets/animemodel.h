#ifndef ANIMEMODEL_H
#define ANIMEMODEL_H

#include <QObject>
#include <QtDataSync/DataStoreModel>
#include <qobjectproxymodel.h>
#include <animeinfo.h>

class AnimeModel : public QObjectProxyModel
{
	Q_OBJECT

public:
	explicit AnimeModel(QtDataSync::DataStoreModel *srcModel, QObject *parent = nullptr);

	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
};

#endif // ANIMEMODEL_H
