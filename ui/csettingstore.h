#ifndef CSETTINGSTORE_H
#define CSETTINGSTORE_H

#include "cenvstore.h"
#include <map>
#include <list>

class CSettingStore : public CEnvStore
{
	Q_OBJECT
	Q_PROPERTY(QString xml_script READ slotGetScriptFile NOTIFY sigChangedScriptFile)
	Q_PROPERTY(QString xml_stopKey READ slotGetStopKey NOTIFY sigChangedStopKey)

public:
	CSettingStore();
	virtual ~CSettingStore();

	enum EActionMethod {
		_METHOD_NORMAL,
		_METHOD_SENDMESSAGE,
	};

	struct SSettingInfo {
		QString szFile;
		QString szStopKey;
		EActionMethod actionMethod;

		std::list<QString> listPlugins;

		SSettingInfo& operator=(const SSettingInfo& info);
		SSettingInfo& operator=(const QVariant& info);
	};

private:
	CSettingStore::SSettingInfo m_settingInfo;

	void initGenericDefault();
	void initAdditionDefault();
	void initDefault();
	bool getGeneric();
	bool getGenericSettings();
	bool getAddition();
	bool getAdditionSettings();
	bool writeGeneric();
	bool writeGenericSettings();
	bool writeAddition();
	bool writeAdditionSettings();

protected:
	virtual bool parser();
	virtual bool store();

signals:
	void sigChangedScriptFile();
	void sigChangedStopKey();

public slots:
	bool slotStore();
	bool slotParser();
	QString slotGetScriptFile();
	QString slotGetStopKey();
	void slotSetSettingInfo(QVariant info);
	QVariant slotGetSettings();
	bool slotRemovePlugin(QVariant info);
	bool slotInsertPlugin(QVariant info);
};

#endif // CSETTINGSTORE_H
