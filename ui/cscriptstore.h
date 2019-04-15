#ifndef CSCRIPTSTORE_H
#define CSCRIPTSTORE_H
#include "cenvstore.h"
#include "cbaseinterp.h"
#include <map>
#include <QThread>

class CScriptStore : public CEnvStore
{
	Q_OBJECT
	Q_PROPERTY(QVariant item READ getCurItem NOTIFY sigAddListItem)
	Q_PROPERTY(QString stopKey READ slotGetStopKey() WRITE slotSetStopKey)
	Q_PROPERTY(QString gen_script READ slotGetGenScript() NOTIFY sigUpdateGenScript)

public:
	CScriptStore(QObject *parent = 0);
	virtual ~CScriptStore();

	enum EScriptCmd {
		_CMD_TYPE_HOTKEY,
		_CMD_TYPE_NETCMD,
		_CMD_TYPE_UNKNOW
	};

	enum EScriptInterp {
		_INTERP_CPP,
		_INTERP_PHP,
		_INTERP_JS,
		_INTERP_BASIC,
		_INTERP_LUA,
		_INTERP_UNKNOW
	};

	struct SScriptInfo {
		EScriptCmd type;
		QString action;
		QString scriptFile;
		QString desc;
		EScriptInterp interp;

		SScriptInfo& operator=(const CScriptStore::SScriptInfo &info);
		SScriptInfo& operator=(const QVariant &info);
	};

	struct SKeyMap {
		int nKey;
		QString szKey;
	};


private:
	QVariantMap m_currentItem;
	std::map<QString, CScriptStore::SScriptInfo> m_mapScriptInfos;
	std::map<QString, CBasedInterpreter *> m_mapRunningInterp;
	QThread *m_pHookThread;
	QString m_szStopKey;
	QString m_szGenScript;

	bool getItems();
	bool getItem(CScriptStore::SScriptInfo &info);

	bool writeItems();
	bool writeItem(CScriptStore::SScriptInfo &info);

	int runItem(QString action);
	int forceStopAll();
	QString translatePath(QString uri);
	std::vector<SEvent> m_vtEvents;

public:
	QVariant getCurItem();
	static QString actionToUi(QString szOri);
	static QString actionToXml(QString szOri);
	void processHookingSignal();
	void processRecordingSignal();

protected:
	virtual bool parser();
	virtual bool store();

signals:
	void sigAddListItem();
	void sigUpdateItemStatus(QVariant updateItem);
	void sigUpdateGenScript();

public slots:
	void slotInterpThreadFinished(QString szFile);
	void slotInterpThreadError(QString szFile);

public slots:
	bool slotStore();
	bool slotParser();
	int slotAddItem(QVariant item);
	int slotEditItem(QVariant item);
	int slotRemoveItem(QVariant item);
	int slotRunItem(QVariant item);
	int slotEngineReady();
	int slotEngineStop();
	int slotEngineRecReady();
	int slotEngineRecStop();
	int slotTest();
	void slotSetStopKey(QString szKey);
	QString slotGetStopKey();

	QString slotActionToUi(QString szOri);
	QString slotActionToXml(QString szOri);
	QString slotGetGenScript();
};

#endif // CSCRIPTSTORE_H
