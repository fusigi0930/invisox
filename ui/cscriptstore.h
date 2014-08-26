#ifndef CSCRIPTSTORE_H
#define CSCRIPTSTORE_H
#include "cenvstore.h"
#include <map>

class CScriptStore : public CEnvStore
{
    Q_OBJECT
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
    std::map<QString, CScriptStore::SScriptInfo> m_mapScriptInfos;
    virtual bool parser();

signals:

public slots:
    virtual bool slotStore();

};

#endif // CSCRIPTSTORE_H
