#include "csettingstore.h"
#include "cscriptstore.h"

#include <QFileInfo>

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

 #define XML_FILENAME                       "settings.xml"

 #define DEFAULT_SCRIPT_FILE                "script.xml"
 #define DEFAULT_STOP_KEY                   "alt,61"            // alt+=

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
    szStopKey=info.szStopKey;
    szFile=info.szFile;
    actionMethod=info.actionMethod;
    listPlugins.clear();
    listPlugins=info.listPlugins;

    return *this;
}

CSettingStore::SSettingInfo& CSettingStore::SSettingInfo::operator=(const QVariant &info) {
    return *this;
}

void CSettingStore::initGenericDefault() {
    m_settingInfo.szStopKey=DEFAULT_STOP_KEY;
    m_settingInfo.szFile=DEFAULT_SCRIPT_FILE;
    m_settingInfo.actionMethod=_METHOD_NORMAL;
}

void CSettingStore::initAdditionDefault() {

}

void CSettingStore::initDefault() {
    initGenericDefault();
    initAdditionDefault();
}

bool CSettingStore::getGenericSettings() {
    while (!m_xmlReader.atEnd() && !m_xmlReader.hasError()) {
         QXmlStreamReader::TokenType readToken=m_xmlReader.readNext();

        switch(readToken) {
            default:
                continue;
            case QXmlStreamReader::StartElement:
                if (0 == m_xmlReader.name().toString().compare(XML_GENERIC_SCRIPT)) {
                    m_settingInfo.szFile=m_xmlReader.readElementText();
                }
                else if (0 == m_xmlReader.name().toString().compare(XML_GENERIC_STOP)) {
                    m_settingInfo.szStopKey=m_xmlReader.readElementText();
                }
                else if (0 == m_xmlReader.name().toString().compare(XML_GENERIC_METHOD)) {
                    m_settingInfo.actionMethod=static_cast<CSettingStore::EActionMethod>(
                                                m_xmlReader.readElementText().toInt());
                }
                break;
            case QXmlStreamReader::EndElement:
                if (0 == m_xmlReader.name().toString().compare(XML_GENERIC)) {
                    if (0 == m_settingInfo.szFile.length() || 0 == m_settingInfo.szStopKey.length())
                        return false;
                    return true;
                }
                break;
        }
    }
    return true;
}

bool CSettingStore::getGeneric() {
    while (!m_xmlReader.atEnd() && !m_xmlReader.hasError()) {
         QXmlStreamReader::TokenType readToken=m_xmlReader.readNext();

        switch(readToken) {
            default:
                continue;
            case QXmlStreamReader::StartElement:
                if (0 == m_xmlReader.name().toString().compare(XML_GENERIC)) {
                    return getGenericSettings();
                }
                break;
        }
    }
    return true;
}

bool CSettingStore::getAdditionSettings() {
    while (!m_xmlReader.atEnd() && !m_xmlReader.hasError()) {
         QXmlStreamReader::TokenType readToken=m_xmlReader.readNext();

        switch(readToken) {
            default:
                continue;
            case QXmlStreamReader::StartElement:
                if (0 == m_xmlReader.name().toString().compare(XML_ADD_PLUGINS)) {
                    m_settingInfo.listPlugins.push_back(m_xmlReader.readElementText());
                }
                break;
            case QXmlStreamReader::EndElement:
                if (0 == m_xmlReader.name().toString().compare(XML_ADD)) {
                    return true;
                }
                break;
        }
    }
    return true;
}

bool CSettingStore::getAddition() {
    while (!m_xmlReader.atEnd() && !m_xmlReader.hasError()) {
         QXmlStreamReader::TokenType readToken=m_xmlReader.readNext();

        switch(readToken) {
            default:
                continue;
            case QXmlStreamReader::StartElement:
                if (0 == m_xmlReader.name().toString().compare(XML_ADD)) {
                    return getAdditionSettings();
                }
                break;
        }
    }
    return true;
}

bool CSettingStore::parser() {
    if (m_szXmlFile.isEmpty()) {
        m_szXmlFile=XML_FILENAME;
    }

    if (!QFile::exists(m_szXmlFile)) {
        initDefault();
        return true;
    }

    if (m_file.isOpen()) {
        m_file.close();
    }

    m_file.setFileName(m_szXmlFile);
    if (!m_file.open(QFile::ReadWrite)) {
        initDefault();
        return true;
    }

    m_xmlReader.setDevice(&m_file);

    // start parser
    while (!m_xmlReader.atEnd() && !m_xmlReader.hasError()) {
        QXmlStreamReader::TokenType readToken=m_xmlReader.readNext();
        switch (readToken) {
            default:
            case QXmlStreamReader::StartDocument:
                continue;
            case QXmlStreamReader::StartElement:
                //_DMSG("element name: %s", m_xmlReader.name().toString().toUtf8().data());
                if (0 == m_xmlReader.name().toString().compare(XML_MAIN)) {
                    if (!getGeneric()) {
                        initGenericDefault();
                        continue;
                    }
                    if (!getAddition())
                        m_settingInfo.listPlugins.clear();
                        initAdditionDefault();
                        continue;
                }
                break;
        }
    }

    if (0 == m_settingInfo.szFile.length() || 0 == m_settingInfo.szStopKey.length()) {
        initGenericDefault();
    }

    sigChangedScriptFile();
    closeParserWrite();
    return true;
}

bool CSettingStore::writeGeneric() {
    m_xmlWriter.writeStartElement(XML_GENERIC);
    if (!writeGenericSettings())
        return false;

    m_xmlWriter.writeEndElement();
    return true;
}

bool CSettingStore::writeGenericSettings() {
    // script
    m_xmlWriter.writeStartElement(XML_GENERIC_SCRIPT);
    m_xmlWriter.writeCharacters(m_settingInfo.szFile);
    m_xmlWriter.writeEndElement();

    // hotkey
    m_xmlWriter.writeStartElement(XML_GENERIC_STOP);
    m_xmlWriter.writeCharacters(m_settingInfo.szStopKey);
    m_xmlWriter.writeEndElement();

    // method
    m_xmlWriter.writeStartElement(XML_GENERIC_METHOD);
    m_xmlWriter.writeCharacters(QString().sprintf("%d", m_settingInfo.actionMethod));
    m_xmlWriter.writeEndElement();
    return true;
}

bool CSettingStore::writeAddition() {
    m_xmlWriter.writeStartElement(XML_ADD);
    if (!writeAdditionSettings())
        return false;

    m_xmlWriter.writeEndElement();
    return true;
}

bool CSettingStore::writeAdditionSettings() {
    std::list<QString>::iterator pList;
    for (pList=m_settingInfo.listPlugins.begin(); pList!=m_settingInfo.listPlugins.end(); pList++) {
        // plugin
        m_xmlWriter.writeStartElement(XML_ADD_PLUGINS);
        m_xmlWriter.writeCharacters(*pList);
        m_xmlWriter.writeEndElement();
    }
    return true;
}

bool CSettingStore::store() {
    if (!CEnvStore::store())
        return false;

    m_xmlWriter.writeStartDocument();
    m_xmlWriter.writeStartElement(XML_MAIN);
    if (!writeGeneric()) {
        return false;
    }
    if (!writeAddition()) {
        return false;
    }

    m_xmlWriter.writeEndElement();
    m_xmlWriter.writeEndDocument();

    return true;
}

bool CSettingStore::slotStore() {
    return store();
}

bool CSettingStore::slotParser() {
    return parser();
}

QString CSettingStore::slotGetScriptFile() {
    return m_settingInfo.szFile;
}

void CSettingStore::slotSetSettingInfo(QVariant info) {
    QVariantMap infoMap=info.toMap();
    m_settingInfo.listPlugins.clear();
    for (QVariantMap::iterator pMap=infoMap.begin(); pMap != infoMap.end(); pMap++) {
        if (0 == pMap.key().compare(XML_GENERIC_SCRIPT)) {
            m_settingInfo.szFile=pMap.value().toString();
        }
        else if (0 == pMap.key().compare(XML_GENERIC_STOP)) {
            m_settingInfo.szStopKey=CScriptStore::actionToXml(pMap.value().toString());
        }
        else if (0 == pMap.key().compare(XML_GENERIC_METHOD)) {
            m_settingInfo.actionMethod=static_cast<CSettingStore::EActionMethod>(
                                        pMap.value().toInt());
        }
        else if (0 == pMap.key().left(7).compare(XML_ADD_PLUGINS)) {
            m_settingInfo.listPlugins.push_back(pMap.value().toString());
        }
    }
}

QVariant CSettingStore::slotGetSettings() {
    QVariantMap info;
    info.insert(XML_GENERIC_SCRIPT, m_settingInfo.szFile);
    info.insert(XML_GENERIC_STOP, CScriptStore::actionToUi(m_settingInfo.szStopKey));
    info.insert(XML_GENERIC_METHOD, static_cast<int>(m_settingInfo.actionMethod));

    int i=0;
    for (std::list<QString>::iterator pList=m_settingInfo.listPlugins.begin(); pList != m_settingInfo.listPlugins.end(); pList++) {
        info.insert(QString().sprintf("%s%d",XML_ADD_PLUGINS, i++), *pList);
    }
    return QVariant::fromValue(info);
}
