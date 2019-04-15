#include "cscriptstore.h"
#include "cluainterpreter.h"
#include "engine.h"
#include "debug.h"
#include "invisox_common.h"
#include "chookthread.h"
#include "crecordthread.h"
#include <QDateTime>

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
#define XML_ITEM_INTERPRETER_LUA                "lua"

#define LIST_ACTION                             "actions"
#define LIST_DESC                               "desc"
#define LIST_FILE                               "script"
#define LIST_INTERPRETER                        "lang"
#define LIST_STATUS                             "status"

#define LIST_ORI_KEY                            "ori_key"


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
		{ 0x3d, "=" },
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
		{ 0x3d, "=" },
#endif
		{ -1, "UNKNOW" }
};

//CCInterpreter g_cinterp;

CScriptStore::SScriptInfo& CScriptStore::SScriptInfo::operator=(const CScriptStore::SScriptInfo &info) {
	action=info.action;
	type=info.type;
	scriptFile=info.scriptFile;
	desc=info.desc;
	interp=info.interp;
	return *this;
}

CScriptStore::SScriptInfo& CScriptStore::SScriptInfo::operator=(const QVariant &info) {
	QVariantMap map=info.toMap();
	for (QVariantMap::iterator pMap=map.begin(); pMap != map.end(); pMap++) {
		if (0 == pMap.key().compare(LIST_ACTION)) {
			// is the network command?
			if (0 == pMap.value().toString().left(3).compare("nc:")) {
				type=_CMD_TYPE_NETCMD;
				action=pMap.value().toString().mid(3);
			}
			else {
				type=_CMD_TYPE_HOTKEY;
				action=CScriptStore::actionToXml(pMap.value().toString());
			}

		}
		else if (0 == pMap.key().compare(LIST_FILE)) {
			scriptFile=pMap.value().toString();
		}
		else if (0 == pMap.key().compare(LIST_DESC)) {
			desc=pMap.value().toString();
		}
		else if (0 == pMap.key().compare(LIST_INTERPRETER)) {
			if (0 == pMap.value().toString().compare(XML_ITEM_INTERPRETER_CLING)) {
				interp=_INTERP_CPP;
			}
			else if (0 == pMap.value().toString().compare(XML_ITEM_INTERPRETER_PHP)) {
				interp=_INTERP_PHP;
			}
			else if (0 == pMap.value().toString().compare(XML_ITEM_INTERPRETER_BASIC)) {
				interp=_INTERP_BASIC;
			}
			else if (0 == pMap.value().toString().compare(XML_ITEM_INTERPRETER_JAVASCRIPT)) {
				interp=_INTERP_JS;
			}
			else if (0 == pMap.value().toString().compare(XML_ITEM_INTERPRETER_LUA)) {
				interp=_INTERP_LUA;
			}
		}
	}
	return *this;
}

CScriptStore::CScriptStore(QObject *parent) :
	CEnvStore(parent)
{
	m_pHookThread = nullptr;
}

CScriptStore::~CScriptStore() {
	m_mapScriptInfos.clear();
	for(std::map<QString, CBasedInterpreter*>::iterator p=m_mapRunningInterp.begin(); p!=m_mapRunningInterp.end(); p++) {
		if (p->second) {
			delete p->second;
			p->second = nullptr;
		}
	}
	m_mapRunningInterp.clear();
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
			case QXmlStreamReader::StartDocument:
				continue;
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
					m_currentItem.insert(LIST_STATUS, "");

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
						case _INTERP_LUA:
							m_currentItem.insert(LIST_INTERPRETER, XML_ITEM_INTERPRETER_LUA);
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
			case QXmlStreamReader::StartDocument:
				continue;
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

bool CScriptStore::writeItems() {
	for (std::map<QString, CScriptStore::SScriptInfo>::iterator p=m_mapScriptInfos.begin(); p!=m_mapScriptInfos.end(); p++) {
		m_xmlWriter.writeStartElement(XML_ITEM);
		if (!writeItem(p->second))
			return false;

		m_xmlWriter.writeEndElement();
	}
	return true;
}

bool CScriptStore::writeItem(CScriptStore::SScriptInfo &info) {
	// action
	m_xmlWriter.writeStartElement(XML_ITEM_ACTION);
	m_xmlWriter.writeAttribute(XML_ITEM_ACTION_ATTR_TYPE, _CMD_TYPE_NETCMD == info.type ? XML_ITEM_ACTION_ATTR_TYPE_NETCMD : XML_ITEM_ACTION_ATTR_TYPE_HOTKEY);
	m_xmlWriter.writeCharacters(slotActionToXml(info.action));
	//_DMSG("action: %s", slotActionToXml(info.action).toUtf8().data());
	m_xmlWriter.writeEndElement();

	// file
	m_xmlWriter.writeStartElement(XML_ITEM_FILE);
	m_xmlWriter.writeCharacters(info.scriptFile);
	//_DMSG("file: %s", info.scriptFile.toUtf8().data());
	m_xmlWriter.writeEndElement();

	// desc
	m_xmlWriter.writeStartElement(XML_ITEM_DESC);
	m_xmlWriter.writeCharacters(info.desc);
	//_DMSG("desc: %s", info.desc.toUtf8().data());
	m_xmlWriter.writeEndElement();

	// interp
	m_xmlWriter.writeStartElement(XML_ITEM_INTERPRETER);
	m_xmlWriter.writeCharacters(QString().sprintf("%d", static_cast<int>(info.interp)));
	//_DMSG("interp: %d", static_cast<int>(info.interp));
	m_xmlWriter.writeEndElement();
	return true;
}

bool CScriptStore::store() {
	if (!CEnvStore::store())
		return false;

	m_xmlWriter.writeStartDocument();
	m_xmlWriter.writeStartElement(XML_MAIN);
	if (!writeItems()) {
		return false;
	}

	m_xmlWriter.writeEndElement();
	m_xmlWriter.writeEndDocument();

	return true;
}

bool CScriptStore::slotStore() {
	return store();
}

bool CScriptStore::slotParser() {
	return parser();
}

QString CScriptStore::actionToUi(QString szOri) {
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
		szRet+=QString().sprintf("F%d", nKey-0x70+1);
	}
	else {
		for (int i=0; -1 != g_keymap[i].nKey; ++i) {
			if (g_keymap[i].nKey == nKey) {
				szRet+= g_keymap[i].szKey;
			}
		}
	}
	return szRet;
}

QString CScriptStore::actionToXml(QString szOri) {
	/*
	 * convert from the Ui to XML type
	 *
	 */

	_DMSG("the ori string: %s", QSZ(szOri));
	QString szRet;
	// is the action network command?
	if (0 == szOri.left(3).compare("nc:")) {
		return szOri.mid(3);
	}

	int nSep=szOri.indexOf("+", 0);
	if (-1 == nSep) {
		return szOri;
	}

	while (-1 != nSep) {
		szRet+=szOri.left(nSep)+",";
		szOri=szOri.mid(nSep+1);
		nSep=szOri.indexOf("+", 0);
	}

	if (1 == szOri.length()) {
		_DMSG("char value: %d", szOri.at(0).unicode());
		szRet+=QString().sprintf("%d",static_cast<int>(szOri.at(0).unicode()));
	}
	else {
		for (int i=0; -1 != g_keymap[i].nKey; ++i) {
			if (0 == g_keymap[i].szKey.compare(szOri)) {
				szRet+=QString().sprintf("%d",g_keymap[i].nKey);
				break;
			}
		}
	}
	return szRet;
}

QString CScriptStore::slotActionToUi(QString szOri) {
	return CScriptStore::actionToUi(szOri);
}

QString CScriptStore::slotActionToXml(QString szOri) {
	return CScriptStore::actionToXml(szOri);
}

int CScriptStore::slotAddItem(QVariant item) {
	CScriptStore::SScriptInfo info;
	info=item;

	std::map<QString, CScriptStore::SScriptInfo>::iterator pFind=m_mapScriptInfos.find(_CMD_TYPE_NETCMD == info.type ? "nc:"+info.action : info.action );
	if (m_mapScriptInfos.end() != pFind) {
		_DMSG("the action has been used!");
		return -1;
	}

	switch (info.type) {
		default:
		case _CMD_TYPE_HOTKEY:
			m_mapScriptInfos[info.action]=info;
			break;
		case _CMD_TYPE_NETCMD:
			m_mapScriptInfos["nc:"+info.action]=info;
			break;
	}

	return 0;
}

int CScriptStore::slotEditItem(QVariant item) {
	CScriptStore::SScriptInfo info;
	info=item;

	QVariantMap map=item.toMap();
	QVariantMap::iterator pOriFind=map.find(LIST_ORI_KEY);
	if (map.end() == pOriFind) {
		_DMSG("the ori_key is not found!");
		return -1;
	}

	_DMSG("ori_key: %s", pOriFind.value().toString().toUtf8().data());

	std::map<QString, CScriptStore::SScriptInfo>::iterator pFind, pReplace;
	if (0 == pOriFind.value().toString().left(3).compare("nc:")) {
		pFind=m_mapScriptInfos.find(pOriFind.value().toString());
	}
	else {
		pFind=m_mapScriptInfos.find(slotActionToXml(pOriFind.value().toString()));
	}
	if (m_mapScriptInfos.end() == pFind) {
		_DMSG("the action is not found!");
		return -2;
	}

	// it has only update filename scenario
#if 0
	pReplace=m_mapScriptInfos.find(_CMD_TYPE_NETCMD == info.type ? "nc:"+info.action : info.action);
	if (m_mapScriptInfos.end() != pReplace) {
		_DMSG("the action is duplicated!");
		return -3;
	}
#endif

	m_mapScriptInfos.erase(pFind);

	switch (info.type) {
		default:
		case _CMD_TYPE_HOTKEY:
			m_mapScriptInfos[info.action]=info;
			break;
		case _CMD_TYPE_NETCMD:
			m_mapScriptInfos["nc:"+info.action]=info;
			break;
	}

	_DMSG("update success!");
	return 0;
}

int CScriptStore::slotRemoveItem(QVariant item) {
	CScriptStore::SScriptInfo info;
	info=item;

	std::map<QString, CScriptStore::SScriptInfo>::iterator pFind=m_mapScriptInfos.find(_CMD_TYPE_NETCMD == info.type ? "nc:"+info.action : info.action );
	if (m_mapScriptInfos.end() == pFind) {
		_DMSG("the action is not exist!");
		return -1;
	}

	m_mapScriptInfos.erase(pFind);

	return 0;
}

static QString transToUiAction(QString action) {
	QStringList element = action.split(",");
	int key = element[element.size()-1].toInt();
	QString uiAction;
	for (int i=0; i<element.size()-1; i++) {
		uiAction.isEmpty() ? uiAction.append(element[i]) : uiAction.append("+"+element[i]);
	}
	// keyvalue
	if (key >= VK_F1 && key <= VK_F12) {
		QString keyvalue;
		keyvalue.sprintf("+F%d", key - VK_F1);
		uiAction.append(keyvalue);
	}
	else if (key >= '0' && key <= '9') {
		QString keyvalue;
		keyvalue.sprintf("+%c", key);
		uiAction.append(keyvalue);
	}
	else if (key >= 'A' && key <= 'Z') {
		QString keyvalue;
		keyvalue.sprintf("+%c", key);
		uiAction.append(keyvalue);
	}

	return uiAction;
}

int CScriptStore::runItem(QString action) {
	std::map<QString, CScriptStore::SScriptInfo>::iterator pFind=m_mapScriptInfos.find(action);
	if (m_mapScriptInfos.end() == pFind) {
		_DMSG("the action is not exist!");
		return -1;
	}

	CBasedInterpreter *base = nullptr;

	switch (pFind->second.interp) {
		case _INTERP_LUA:
			base = new CLuaInterpreter();
			break;
		default:
			_DMSG("not support script now!");
			return 1;
	}

	// do running thread actions
	connect(base, SIGNAL(sigThreadFinished(QString)), this, SLOT(slotInterpThreadFinished(QString)));
	connect(base, SIGNAL(sigThreadError(QString)), this, SLOT(slotInterpThreadError(QString)));

	std::map<QString, CBasedInterpreter *>::iterator pRunFile = m_mapRunningInterp.find(translatePath(pFind->second.scriptFile));
	if (m_mapRunningInterp.end() == pRunFile) {
		m_mapRunningInterp[translatePath(pFind->second.scriptFile)]=base;
		QVariantMap item;

		item.insert(LIST_ACTION, transToUiAction(action));
		item.insert(LIST_STATUS, "run");
		emit sigUpdateItemStatus(QVariant::fromValue(item));
		base->setAction(action);
		base->slotRun(translatePath(pFind->second.scriptFile));
	}
	return 0;
}

int CScriptStore::forceStopAll() {
	_DMSG("force stop all running script threads");
	std::map<QString, CBasedInterpreter *>::iterator pRunFile;
	for (pRunFile = m_mapRunningInterp.begin(); pRunFile != m_mapRunningInterp.end(); pRunFile++) {
		QVariantMap item;
		pRunFile->second->slotPause();
		item.insert(LIST_ACTION, transToUiAction(pRunFile->second->getAction()));
		item.insert(LIST_STATUS, "force stop");
		emit sigUpdateItemStatus(QVariant::fromValue(item));
		pRunFile->second->Stop();
		delete pRunFile->second;
	}
	m_mapRunningInterp.clear();
	return 0;
}

int CScriptStore::slotRunItem(QVariant item) {
	CScriptStore::SScriptInfo info;
	info=item;
	QString actionKey = (_CMD_TYPE_NETCMD == info.type ? "nc:"+info.action : info.action);

	return runItem(actionKey);
}

int CScriptStore::slotEngineReady() {
	::engStart();
	if (nullptr != m_pHookThread) {
		delete m_pHookThread;
		m_pHookThread = nullptr;
	}
	m_pHookThread = new CHookThread(this);

	if (m_pHookThread) {
		m_pHookThread->start();
	}
	return 0;
}

int CScriptStore::slotEngineStop() {
	::engEnd();
	if (nullptr != m_pHookThread) {
		m_pHookThread->quit();
		m_pHookThread->wait();
		delete m_pHookThread;
		m_pHookThread = nullptr;
	}
	return 0;
}

int CScriptStore::slotEngineRecReady() {
	::engEnd();
	if (nullptr != m_pHookThread) {
		m_pHookThread->quit();
		m_pHookThread->wait();
		delete m_pHookThread;
		m_pHookThread = nullptr;
	}

	::recStart();
	m_vtEvents.clear();
	m_szGenScript.clear();
	m_pHookThread = new CRecordThread(this);
	if (m_pHookThread) {
		m_pHookThread->start();
	}
	return 0;
}

int CScriptStore::slotEngineRecStop() {
	::recEnd();
	if (nullptr != m_pHookThread) {
		m_pHookThread->quit();
		m_pHookThread->wait();
		delete m_pHookThread;
		m_pHookThread = nullptr;
	}

	// gen script
	CBasedInterpreter *pInterp = new CLuaInterpreter();
	if (pInterp) {
		m_szGenScript = pInterp->genScript(m_vtEvents);
		delete pInterp;
		emit sigUpdateGenScript();
	}
	else {
		m_szGenScript.clear();
	}

	::engStart();
	return 0;
}

int CScriptStore::slotTest() {
	return 0;
}

static QString transKeyData(unsigned long long *keyData) {
	if (nullptr == keyData) {
		return "";
	}

	QString ret;
	if (keyData[1] & _INVISOX_KF_CTRL) {
		ret.append("ctrl");
	}
	if (keyData[1] & _INVISOX_KF_ALT) {
		ret.isEmpty() ? ret.append("alt") : ret.append(",alt");
	}
	if (keyData[1] & _INVISOX_KF_SHIFT) {
		ret.isEmpty() ? ret.append("shift") : ret.append(",shift");
	}
	QString keyNum;
	switch (keyData[0]) {
	default:
		break;
	case VK_OEM_PLUS:
		keyData[0] = '=';
		break;
	case VK_OEM_MINUS:
		keyData[0] = '-';
		break;
	}
	keyNum.sprintf(",%d", static_cast<int>(keyData[0]));
	ret.append(keyNum);

	return ret;
}

void CScriptStore::processHookingSignal() {
	while (1) {
		char buffer[_INVISOX_SHARED_MEM_SIZE] = {0};
		_DMSG("waiting for hooking key event");
		engReadSharedMemory(buffer, sizeof(buffer));

		unsigned long long *pKeyData = reinterpret_cast<unsigned long long *>(buffer);
		_DMSG("hook key: 0x%x", static_cast<int>(pKeyData[0]));
		_DMSG("hook multiple key: 0x%x", static_cast<int>(pKeyData[1]));
		if (static_cast<char>(_INVISOX_EXIT_CODE_HOOING) == buffer[_INVISOX_SHARED_MEM_SIZE - 1]) {
			_DMSG("exit from the infinite loop");
			break;
		}
		// process monitor keys detect
		QString actionKey = transKeyData(pKeyData);
		_DMSG("stop key: %s, current key: %s", QSZ(m_szStopKey), QSZ(actionKey));
		if (0 == m_szStopKey.compare(actionKey)) {
			forceStopAll();
		}
		else {
			runItem(actionKey);
		}
	}
	_DMSG("exit the process hooking");
}

void CScriptStore::processRecordingSignal() {
	while (1) {
		char buffer[_INVISOX_SHARED_MEM_SIZE] = {0};
		_DMSG("waiting for hooking key event");
		engReadSharedMemory(buffer, sizeof(buffer));

		// is exist?
		if (static_cast<char>(_INVISOX_EXIT_CODE_HOOING) == buffer[_INVISOX_SHARED_MEM_SIZE - 1]) {
			_DMSG("exit from the infinite loop");
			break;
		}

		// get key data
		unsigned long long *pData = reinterpret_cast<unsigned long long *>(buffer + _INVISOX_SHARED_MEM_KEY_INDEX);
		if (0 != pData[0]) {
			SEvent event;
			event.type = _INVISOX_EVENT_TYPE_KEY;
			event.o.key.keyvalue = static_cast<int>(pData[0]);
			event.o.key.multiple = static_cast<int>(pData[1]);
			event.o.key.keyAction = static_cast<int>(pData[2]);
			_DMSG("kv: 0x%x, action: %d", event.o.key.keyvalue, event.o.key.keyAction);
			event.timeTick = static_cast<unsigned long long>(QDateTime::currentMSecsSinceEpoch());
			m_vtEvents.push_back(event);
		}

		// get mouse data
		pData = reinterpret_cast<unsigned long long *>(buffer + _INVISOX_SHARED_MEM_MOUES_INDEX);
		if (0 != pData[0]) {
			SEvent event;
			event.type = _INVISOX_EVENT_TYPE_MOUSE;
			event.o.mouse.mouseEvent = static_cast<int>(pData[0]);
			event.o.mouse.x = static_cast<int>(pData[1]);
			event.o.mouse.y = static_cast<int>(pData[2]);
			event.timeTick = static_cast<unsigned long long>(QDateTime::currentMSecsSinceEpoch());
			m_vtEvents.push_back(event);
		}
	}
	_DMSG("exit the process hooking");
}

QString CScriptStore::translatePath(QString uri) {
	if (uri.length() < 8)
		return uri;
	return uri.mid(8).replace('/', '\\');
}

void CScriptStore::slotInterpThreadFinished(QString szFile) {
	std::map<QString, CBasedInterpreter *>::iterator pFind = m_mapRunningInterp.find(szFile);
	if (m_mapRunningInterp.end() != pFind) {
		_DMSG("file: %s run finished", QSZ(szFile));
		QVariantMap item;

		item.insert(LIST_ACTION, transToUiAction(pFind->second->getAction()));
		item.insert(LIST_STATUS, "done");
		emit sigUpdateItemStatus(QVariant::fromValue(item));
		delete pFind->second;
		m_mapRunningInterp.erase(pFind);
	}
}

void CScriptStore::slotInterpThreadError(QString szFile) {
	std::map<QString, CBasedInterpreter *>::iterator pFind = m_mapRunningInterp.find(szFile);
	if (m_mapRunningInterp.end() != pFind) {
		_DMSG("file: %s run error", QSZ(szFile));
		QVariantMap item;

		item.insert(LIST_ACTION, transToUiAction(pFind->second->getAction()));
		item.insert(LIST_STATUS, "error");
		emit sigUpdateItemStatus(QVariant::fromValue(item));
		delete pFind->second;
		m_mapRunningInterp.erase(pFind);
	}
}

void CScriptStore::slotSetStopKey(QString szKey) {
	m_szStopKey = szKey;
}

QString CScriptStore::slotGetStopKey() {
	return m_szStopKey;
}

QString CScriptStore::slotGetGenScript() {
	return m_szGenScript;
}
