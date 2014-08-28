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


private:
    QVariantMap m_currentItem;
    std::map<QString, CScriptStore::SScriptInfo> m_mapScriptInfos;
    bool getItems();
    bool getItem(CScriptStore::SScriptInfo &info);

public:
    QVariant getCurItem();

protected:
    virtual bool parser();

signals:
    void sigAddListItem();

public slots:
    virtual bool slotStore();
    bool slotParser();

};

#endif // CSCRIPTSTORE_H