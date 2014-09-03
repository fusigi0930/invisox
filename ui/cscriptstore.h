#ifndef CSCRIPTSTORE_H
#define CSCRIPTSTORE_H
#include "cenvstore.h"
#include <map>

class CScriptStore : public CEnvStore
{
    Q_OBJECT
    Q_PROPERTY(QVariant item READ getCurItem NOTIFY sigAddListItem)

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
        _INTERP_UNKNOW
    };

    struct SScriptInfo {
        EScriptCmd type;
        QString action;
        QString scriptFile;
        QString desc;
        EScriptInterp interp;
    };

    struct SKeyMap {
        int nKey;
        QString szKey;
    };


private:
    QVariantMap m_currentItem;
    std::map<QString, CScriptStore::SScriptInfo> m_mapScriptInfos;
    bool getItems();
    bool getItem(CScriptStore::SScriptInfo &info);

    bool writeItems();
    bool writeItem(CScriptStore::SScriptInfo &info);

public:
    QVariant getCurItem();

protected:
    virtual bool parser();
    virtual bool store();

signals:
    void sigAddListItem();

public slots:
    bool slotStore();
    bool slotParser();
    bool slotAddItem(QVariant item);
    bool slotEditItem(QVariant item);
    bool slotRemoveItem(QVariant item);
    QString slotActionToUi(QString szOri);
    QString slotActionToXml(QString szOri);

};

#endif // CSCRIPTSTORE_H
