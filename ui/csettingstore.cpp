#include "csettingstore.h"
#include "cscriptstore.h"

/*
 * <?xml version="1.0" encoding="utf-8"?>
 * <settings>
 *      <generic>
 *          <script>c:\xxx\xxx\xxx.xml></script> <!-- the script list file path -->
 *          <stop_key>ctrl,22</stop_key> <!-- stop all of the running interpreter thread -->
 *          <method>0</method> <!-- send i/o command method -->
 *      </generic>
 *      <addition>
 *          <plug_in>c:\xxx\xxx.dll</plug_in> <!-- or *.so, to create some additional feature to invisOX -->
 *      </addition>
 * </settings>
 *
 *
 */

 #define XML_MAIN                           "settings"
 #define XML_GENERIC                        "generic"
 #define XML_GENERIC_SCRIPT                 "script"
 #define XML_GENERIC_STOP                   "stop_key"
 #define XML_GENERIC_METHOD                 "method"

 #define XML_ADD                            "addition"
 #define XML_ADD_PLUGINS                    "plug_in"

CSettingStore::CSettingStore()
{
}

CSettingStore::~CSettingStore() {

}

CSettingStore::SSettingInfo& CSettingStore::SSettingInfo::operator=(const CSettingStore::SSettingInfo &info) {
    return *this;
}

CSettingStore::SSettingInfo& CSettingStore::SSettingInfo::operator=(const QVariant &info) {
    return *this;
}

bool CSettingStore::parser() {
    if (!CEnvStore::parser())
        return false;

    return true;
}

bool CSettingStore::store() {
    if (!CEnvStore::store())
        return false;

    return true;
}
