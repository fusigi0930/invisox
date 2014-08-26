#include "cscriptstore.h"

/*
 * <?xml version="1.0" encoding="utf-8"?>
 * <scripts>
 *      <item>
 *          <action type="hotkey">ctrl,49</action> <!-- 49 is the key value for decide -->
 *          <file>c:\xxx\xxx\test.cxx</file>
 *          <desc>
 *              test for the xml parser
 *          </desc>
 *          <interp>0</interp>
 *      </item>
 *      <item>
 *          <action type="netcmd">working</action>
 *          <file>c:\xfdsf\sfddsfd\qq.cxx</file>
 *          <desc>
 *              qkqkqkqqkqk
 *          </desc>
 *          <interp>0</interp>
 *      </item>
 * </scripts>
 *
 *
 */

#include "debug.h"

#define XML_MAIN                                "scripts"
#define XML_ITEM                                "item"
#define XML_ITEM_ACTION                         "action"
#define XML_ITEM_ACTION_ATTR_TYPE               "type"
#define XML_ITEM_ACTION_ATTR_TYPE_HOTKEY        "hotkey"
#define XML_ITEM_ACTION_ATTR_TYPE_NETCMD        "netcmd"
#define XML_ITEM_FILE                           "file"
#define XML_ITEM_DESC                           "desc"
#define XML_ITEM_INTERPRETER                    "interp"



CScriptStore::CScriptStore(QObject *parent) :
    CEnvStore(parent)
{

}

CScriptStore::~CScriptStore() {
    m_mapScriptInfos.clear();
}

bool CScriptStore::getItem(CScriptStore::SScriptInfo &info) {
    while (!m_xmlReader.atEnd() && !m_xmlReader.hasError()) {
         QXmlStreamReader::TokenType readToken=m_xmlReader.readNext();

        switch(readToken) {
            default:
                continue;
                break;
            case QXmlStreamReader::StartElement:
                _DMSG("element name: %s, text: %s", m_xmlReader.name().toString().toUtf8().data(), m_xmlReader.readElementText().toUtf8().data());
                if (0 == m_xmlReader.name().toString().compare(XML_ITEM_ACTION)) {

                }
                else if (0 == m_xmlReader.name().toString().compare(XML_ITEM_FILE)) {

                }
                else if (0 == m_xmlReader.name().toString().compare(XML_ITEM_DESC)) {

                }
                else if (0 == m_xmlReader.name().toString().compare(XML_ITEM_INTERPRETER)) {

                }
                break;
            case QXmlStreamReader::EndElement:
                if (0 == m_xmlReader.name().toString().compare(XML_ITEM))
                    return true;
                break;
        }


    }
    return true;
}

bool CScriptStore::getItems() {
    while (!m_xmlReader.atEnd() && !m_xmlReader.hasError()) {
        QXmlStreamReader::TokenType readToken=m_xmlReader.readNext();
        switch (readToken) {
            default:
                continue;
                break;
            case QXmlStreamReader::StartDocument:
                continue;
                break;
            case QXmlStreamReader::StartElement:
                _DMSG("element name: %s", m_xmlReader.name().toString().toUtf8().data());
                if (0 == m_xmlReader.name().toString().compare(XML_ITEM)) {
                    CScriptStore::SScriptInfo info;
                    if (!getItem(info))
                        continue;

                }
                break;
        }

    }

    return true;
}

bool CScriptStore::parser() {
    if (!CEnvStore::parser())
        return false;

    m_mapScriptInfos.clear();

    while (!m_xmlReader.atEnd() && !m_xmlReader.hasError()) {
        QXmlStreamReader::TokenType readToken=m_xmlReader.readNext();
        switch (readToken) {
            default:
                continue;
                break;
            case QXmlStreamReader::StartDocument:
                continue;
                break;
            case QXmlStreamReader::StartElement:
                _DMSG("element name: %s", m_xmlReader.name().toString().toUtf8().data());
                if (0 == m_xmlReader.name().toString().compare(XML_MAIN)) {
                    if (!getItems())
                        continue;
                }
                break;
        }

    }
    return true;
}

bool CScriptStore::slotStore() {
    if (!CEnvStore::slotStore())
        return false;

    return true;
}

bool CScriptStore::slotParser() {
    return parser();
}
