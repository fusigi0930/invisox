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
#define XML_ITEM_INTERPRETER_CLING              "c/c++"
#define XML_ITEM_INTERPRETER_PHP                "php"
#define XML_ITEM_INTERPRETER_BASIC              "basic"
#define XML_ITEM_INTERPRETER_JAVASCRIPT         "javascript"

#define LIST_ACTION                             "actions"
#define LIST_DESC                               "desc"
#define LIST_FILE                               "script"
#define LIST_INTERPRETER                        "lang"


static CScriptStore::SKeyMap g_keymap[] ={
#ifdef _WIN32
        { 0x70, "F1" },
        { 0x71, "F2" },
        { 0x72, "F3" },
        { 0x73, "F4" },
        { 0x74, "F5" },
        { 0x75, "F6" },
        { 0x76, "F7" },
        { 0x77, "F8" },
        { 0x78, "F9" },
        { 0x79, "F10" },
        { 0x7a, "F11" },
        { 0x7b, "F12" },
#else
        { 0x70, "F1" },
        { 0x71, "F2" },
        { 0x72, "F3" },
        { 0x73, "F4" },
        { 0x74, "F5" },
        { 0x75, "F6" },
        { 0x76, "F7" },
        { 0x77, "F8" },
        { 0x78, "F9" },
        { 0x79, "F10" },
        { 0x7a, "F11" },
        { 0x7b, "F12" },
#endif
        { -1, "UNKNOW" }
    };

CScriptStore::CScriptStore(QObject *parent) :
    CEnvStore(parent)
{

}

CScriptStore::~CScriptStore() {
    m_mapScriptInfos.clear();
}

QVariant CScriptStore::getCurItem() {
    //qDebug() << QVariant::fromValue(m_currentItem);
    return QVariant::fromValue(m_currentItem);
}

bool CScriptStore::getItem(CScriptStore::SScriptInfo &info) {
    while (!m_xmlReader.atEnd() && !m_xmlReader.hasError()) {
         QXmlStreamReader::TokenType readToken=m_xmlReader.readNext();

        switch(readToken) {
            default:
                continue;
                break;
            case QXmlStreamReader::StartElement:
                if (0 == m_xmlReader.name().toString().compare(XML_ITEM_ACTION)) {
                    QXmlStreamAttributes attri=m_xmlReader.attributes();
                    if (!attri.hasAttribute(XML_ITEM_ACTION_ATTR_TYPE)) {
                        info.type=_CMD_TYPE_HOTKEY;
                    }
                    else if (0 == attri.value(XML_ITEM_ACTION_ATTR_TYPE).toString().compare(XML_ITEM_ACTION_ATTR_TYPE_HOTKEY)) {
                        info.type=_CMD_TYPE_HOTKEY;
                    }
                    else if (0 == attri.value(XML_ITEM_ACTION_ATTR_TYPE).toString().compare(XML_ITEM_ACTION_ATTR_TYPE_NETCMD)) {
                        info.type=_CMD_TYPE_NETCMD;
                    }
                    else {
                        info.type=_CMD_TYPE_HOTKEY;
                    }
                    info.action=m_xmlReader.readElementText();
                }
                else if (0 == m_xmlReader.name().toString().compare(XML_ITEM_FILE)) {
                    info.scriptFile=m_xmlReader.readElementText();
                }
                else if (0 == m_xmlReader.name().toString().compare(XML_ITEM_DESC)) {
                    info.desc=m_xmlReader.readElementText();
                }
                else if (0 == m_xmlReader.name().toString().compare(XML_ITEM_INTERPRETER)) {
                    info.interp=static_cast<CScriptStore::EScriptInterp>(
                                m_xmlReader.readElementText().toInt());
                }
                break;
            case QXmlStreamReader::EndElement:
                if (0 == m_xmlReader.name().toString().compare(XML_ITEM)) {
                    return true;
                }
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
                //_DMSG("element name: %s", m_xmlReader.name().toString().toUtf8().data());
                if (0 == m_xmlReader.name().toString().compare(XML_ITEM)) {
                    CScriptStore::SScriptInfo info;
                    if (!getItem(info))
                        continue;

                    m_currentItem.clear();
                    switch (info.type) {
                        default:
                        case _CMD_TYPE_HOTKEY:
                            m_mapScriptInfos[info.action]=info;
                            info.action=slotActionToUi(info.action);
                            m_currentItem.insert(LIST_ACTION, info.action);
                            break;
                        case _CMD_TYPE_NETCMD:
                            m_mapScriptInfos["nc:"+info.action]=info;
                            m_currentItem.insert(LIST_ACTION, "nc:"+info.action);
                            break;
                    }

                    QString szTemp=info.desc;
                    szTemp.replace("\n", "");
                    szTemp.replace("\r", "");
                    m_currentItem.insert(LIST_DESC, szTemp);
                    m_currentItem.insert(LIST_FILE, info.scriptFile);

                    switch (info.interp) {
                        default:
                        case _INTERP_CPP:
                            m_currentItem.insert(LIST_INTERPRETER, XML_ITEM_INTERPRETER_CLING);
                            break;
                        case _INTERP_BASIC:
                            m_currentItem.insert(LIST_INTERPRETER, XML_ITEM_INTERPRETER_BASIC);
                            break;
                        case _INTERP_PHP:
                            m_currentItem.insert(LIST_INTERPRETER, XML_ITEM_INTERPRETER_PHP);
                            break;
                        case _INTERP_JS:
                            m_currentItem.insert(LIST_INTERPRETER, XML_ITEM_INTERPRETER_JAVASCRIPT);
                            break;
                    }

                    emit sigAddListItem();
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
                //_DMSG("element name: %s", m_xmlReader.name().toString().toUtf8().data());
                if (0 == m_xmlReader.name().toString().compare(XML_MAIN)) {
                    if (!getItems())
                        continue;
                }
                break;
        }

    }
    closeParserWrite();
    return true;
}

bool CScriptStore::store() {
    if (!CEnvStore::store())
        return false;
    return true;
}

bool CScriptStore::slotStore() {
    if (!CEnvStore::slotStore())
        return false;

    closeParserWrite();
    return true;
}

bool CScriptStore::slotParser() {
    return parser();
}


QString CScriptStore::slotActionToUi(QString szOri) {
    /*
     * the original action text should be nc:xxxxxxx,
     * ctrl,<keycode>
     * ctrl,alt,<keycode>
     * ctrl,alt,shift,<keycode>
     *
     * it should be formatted to the human format like:
     * nc:xxxxxxxx
     * ctrl+<keychar>
     * ctrl+alt+<keychar>
     * ctrl+alt+shift+<keychar>
     *
     * and the human's
     *
     */
    QString szRet;
    int nSep=szOri.indexOf(",", 0);
    while (-1 != nSep) {
        szRet+=szOri.left(nSep)+"+";
        szOri=szOri.mid(nSep+1);
        nSep=szOri.indexOf(",", 0);
    }

    int nKey=szOri.mid(-1 == nSep ? 0 : nSep+1).toInt();
    if ((0x30 <= nKey && 0x39 >= nKey) || ('A' <= nKey && 'Z' >= nKey)) {
        szRet+=QChar(nKey);
    }
    else if (0x70 <= nKey && 0x7b >= nKey) {
        szRet.sprintf("F%d", nKey-0x70+1);
    }

    return szRet;
}

QString CScriptStore::slotActionToXml(QString szOri) {
    /*
     * convert from the Ui to XML type
     *
     */
    QString szRet;
    // is the action network command?
    if (0 == szOri.left(3).compare("nc:")) {
        return szOri.mid(4);
    }

    int nSep=szOri.indexOf("+", 0);
    while (-1 != nSep) {
        szRet+=szOri.left(nSep)+",";
        szOri=szOri.mid(nSep+1);
        nSep=szOri.indexOf("+", 0);
    }

    if (1 == szOri.length()) {
        szRet+=static_cast<int>(szOri.at(0).toLatin1());
    }
    else {
        for (int i=0; -1 != g_keymap[i].nKey; ++i) {
            if (0 == g_keymap[i].szKey.compare(szOri)) {
                szRet+=g_keymap[i].nKey;
                break;
            }
        }
    }
    return szRet;
}
