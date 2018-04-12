#ifndef ENTRYVIEWMODEL_H
#define ENTRYVIEWMODEL_H

#include <QtMvvmCore/ViewModel>
#include <proxerentrymodel.h>

class EntryViewModel : public QtMvvm::ViewModel
{
	Q_OBJECT

	Q_PROPERTY(ProxerEntryModel* model READ model NOTIFY modelChanged)

	QTMVVM_INJECT_PROP(SyncedSettings*, settings, _settings)

public:
	Q_INVOKABLE explicit EntryViewModel(QObject *parent = nullptr);

	ProxerEntryModel *model() const;

public slots:
	void addAnime(int id);

signals:
	void modelChanged(ProxerEntryModel *model);
	void close();

protected:
	void onInit(const QVariantHash &params) override;
	void onResult(quint32 requestCode, const QVariant &result) override;

private slots:
	void loginNeeded();
	void apiError(const QString &errorString, int errorCode, QtRestClient::RestReply::ErrorType errorType);

private:
	static const int LoginRequestCode = 0x4201;
	static const int AddRequestCode = 0x4202;

	SyncedSettings *_settings;
	ProxerEntryModel *_model;
	bool _showingChild;
};

#endif // ENTRYVIEWMODEL_H
