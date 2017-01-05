#ifndef PROXERCONNECTOR_H
#define PROXERCONNECTOR_H

#include <QObject>
#include <QPixmap>
#include <restclient.h>
#include "ProxerApi/proxerentry.h"

struct MetaRequest;
class ProxerConnector : public QObject
{
	Q_OBJECT

public:
	explicit ProxerConnector(QObject *parent = nullptr);

public slots:
	void loadMetaData(int id);
	void loadSeasonCount(int id);

signals:
	void metaDataLoaded(int id, QString title, QPixmap preview);
	void seasonsLoaded(int id, int seasonCount);
	void apiError(const QString &error);

private slots:
	void imageReplyFinished();
	void tryFinishMetaRequest(MetaRequest *entry);

private:
	QtRestClient::RestClient *client;	
	QtRestClient::RestClass *infoClass;
};

#endif // PROXERCONNECTOR_H
