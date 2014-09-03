#ifndef CSETTINGSTORE_H
#define CSETTINGSTORE_H

#include "cenvstore.h"
#include <map>
#include <list>

class CSettingStore : public CEnvStore
{
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

protected:
    virtual bool parser();
    virtual bool store();
};

#endif // CSETTINGSTORE_H
