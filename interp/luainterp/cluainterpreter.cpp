#include "cluainterpreter.h"

#include "debug.h"
#include "invisox_common.h"
#include <iostream>
#include <sstream>

#ifdef Q_OS_WIN
#include <windows.h>

static std::streambuf *g_oriStdout=NULL;
static std::streambuf *g_oriStderr=NULL;
static std::stringstream g_coutBuf;
static std::stringstream g_cerrBuf;


extern "C" BOOL WINAPI DllMain(
  HANDLE hinstDLL,
  DWORD fdwReason,
  LPVOID lpvReserved
)
{
	switch (fdwReason)
	{
		case DLL_PROCESS_ATTACH:
			_DMSG("%s dll attached", __FILE__);

			break;
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			_DMSG("%s dll deattached", __FILE__);

			break;
	}
	return true;
}

#endif


CLuaInterpreter::CLuaInterpreter() : CBasedInterpreter(), mLua()
{
}

CLuaInterpreter::~CLuaInterpreter() {
	mLua.close();
}

int CLuaInterpreter::run(QString szFile) {
	if (szFile.isEmpty()) {
		return -3;
	}

	mLua.runFile(szFile);
	_DMSG("%s", QSZ(mLua.getResult()));

	return 0;
}

QString CLuaInterpreter::getStdout() {

	return "";
}

QString CLuaInterpreter::genScript(std::vector<SEvent> &events) {
	if (0 == events.size()) {
		return CBasedInterpreter::genScript(events);
	}

	QString szScript;
	std::vector<SEvent>::iterator pEvent = events.begin();
	std::vector<SEvent>::iterator pPreEvent;
	while (pEvent != events.end()) {
		pPreEvent = pEvent++;
		switch (pEvent->type) {
			case _INVISOX_EVENT_TYPE_KEY:
				if (pEvent == events.end()) {
					genScriptKey(*pPreEvent, *pPreEvent, szScript);
				}
				else {
					if (0 == genScriptKey(*pPreEvent, *pEvent, szScript))
						pEvent++;
				}
				break;
			case _INVISOX_EVENT_TYPE_MOUSE:
				if (pEvent == events.end()) {
					genScriptMouse(*pPreEvent, *pPreEvent, szScript);
				}
				else {
					if (0 == genScriptMouse(*pPreEvent, *pEvent, szScript))
						pEvent++;
				}
				break;
		}
	}
	return szScript;
}

static QString g_szMappingKey[] = {
	"",									// 0
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"CONST_VK_BACK",					// 0x08
	"CONST_VK_TAB",						// 0x09
	"",
	"",
	"CONST_VK_CLEAR",
	"CONST_VK_RETURN",
	"",
	"",
	"CONST_VK_SHIFT",
	"CONST_VK_CONTROL",
	"CONST_VK_MENU",
	"CONST_VK_PAUSE",
	"CONST_VK_CAPITAL",
	"CONST_VK_KANA",
	"",
	"CONST_VK_JUNJA",
	"CONST_VK_FINAL",
	"CONST_VK_HANJA",
	"",
	"CONST_VK_ESCAPE",					// 0x1b
	"CONST_VK_CONVERT",
	"CONST_VK_NONCONVERT",
	"CONST_VK_ACCEPT",
	"CONST_VK_MODECHANGE",
	"CONST_VK_SPACE",
	"CONST_VK_PRIOR",
	"CONST_VK_NEXT",
	"CONST_VK_END",
	"CONST_VK_HOME",
	"CONST_VK_LEFT",
	"CONST_VK_UP",
	"CONST_VK_RIGHT",
	"CONST_VK_DOWN",
	"CONST_VK_SELECT",
	"CONST_VK_PRINT",
	"CONST_VK_EXECUTE",
	"CONST_VK_SNAPSHOT",
	"CONST_VK_INSERT",
	"CONST_VK_DELETE",
	"CONST_VK_HELP",
	"CONST_VK_0",
	"CONST_VK_1",
	"CONST_VK_2",
	"CONST_VK_3",
	"CONST_VK_4",
	"CONST_VK_5",
	"CONST_VK_6",
	"CONST_VK_7",
	"CONST_VK_8",
	"CONST_VK_9",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"CONST_VK_A",
	"CONST_VK_B",
	"CONST_VK_C",
	"CONST_VK_D",
	"CONST_VK_E",
	"CONST_VK_F",
	"CONST_VK_G",
	"CONST_VK_H",
	"CONST_VK_I",
	"CONST_VK_J",
	"CONST_VK_K",
	"CONST_VK_L",
	"CONST_VK_M",
	"CONST_VK_N",
	"CONST_VK_O",
	"CONST_VK_P",
	"CONST_VK_Q",
	"CONST_VK_R",
	"CONST_VK_S",
	"CONST_VK_T",
	"CONST_VK_U",
	"CONST_VK_V",
	"CONST_VK_W",
	"CONST_VK_X",
	"CONST_VK_Y",
	"CONST_VK_Z",
	"CONST_VK_LWIN",
	"CONST_VK_RWIN",
	"CONST_VK_APPS",
	"",
	"CONST_VK_SLEEP",
	"CONST_VK_NUMPAD0",
	"CONST_VK_NUMPAD1",
	"CONST_VK_NUMPAD2",
	"CONST_VK_NUMPAD3",
	"CONST_VK_NUMPAD4",
	"CONST_VK_NUMPAD5",
	"CONST_VK_NUMPAD6",
	"CONST_VK_NUMPAD7",
	"CONST_VK_NUMPAD8",
	"CONST_VK_NUMPAD9",
	"CONST_VK_MULTIPLY",
	"CONST_VK_ADD",
	"CONST_VK_SEPARATOR",
	"CONST_VK_SUBTRACT",
	"CONST_VK_DECIMAL",
	"CONST_VK_DIVIDE",
	"CONST_VK_F1",
	"CONST_VK_F2",
	"CONST_VK_F3",
	"CONST_VK_F4",
	"CONST_VK_F5",
	"CONST_VK_F6",
	"CONST_VK_F7",
	"CONST_VK_F8",
	"CONST_VK_F9",
	"CONST_VK_F10",
	"CONST_VK_F11",
	"CONST_VK_F12",
	"CONST_VK_F13",
	"CONST_VK_F14",
	"CONST_VK_F15",
	"CONST_VK_F16",
	"CONST_VK_F17",
	"CONST_VK_F18",
	"CONST_VK_F19",
	"CONST_VK_F20",
	"CONST_VK_F21",
	"CONST_VK_F22",
	"CONST_VK_F23",
	"CONST_VK_F24",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"CONST_VK_NUMLOCK",
	"CONST_VK_SCROLL",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"CONST_VK_LSHIFT",
	"CONST_VK_RSHIFT",
	"CONST_VK_LCONTROL",
	"CONST_VK_RCONTROL",
	"CONST_VK_LMENU",
	"CONST_VK_RMENU",
	"CONST_VK_BROWSER_BACK",
	"CONST_VK_BROWSER_FORWARD",
	"CONST_VK_BROWSER_REFRESH",
	"CONST_VK_BROWSER_STOP",
	"CONST_VK_BROWSER_SEARCH",
	"CONST_VK_BROWSER_FAVORITES",
	"CONST_VK_BROWSER_HOME",
	"CONST_VK_VOLUME_MUTE",
	"CONST_VK_VOLUME_DOWN",
	"CONST_VK_VOLUME_UP",
	"CONST_VK_MEDIA_NEXT_TRACK",
	"CONST_VK_MEDIA_PREV_TRACK",
	"CONST_VK_MEDIA_STOP",
	"CONST_VK_MEDIA_PLAY_PAUSE",
	"CONST_VK_MEDIA_LAUNCH_MAIL",
	"CONST_VK_MEDIA_LAUNCH_MEDIA_SELECT",
	"CONST_VK_MEDIA_LAUNCH_APP1",
	"CONST_VK_MEDIA_LAUNCH_APP2",
	"",
	"",
	"CONST_VK_OEM_1",
	"CONST_VK_OEM_PLUS",
	"CONST_VK_OEM_COMMA",
	"CONST_VK_OEM_MINUS",
	"CONST_VK_OEM_PERIOD",
	"CONST_VK_OEM_2",
	"CONST_VK_OEM_3",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"CONST_VK_OEM_4",
	"CONST_VK_OEM_5",
	"CONST_VK_OEM_6",
	"CONST_VK_OEM_7",
	"CONST_VK_OEM_8",
	"",
	"",
	"CONST_VK_OEM_102",
	"",
	"",
	"CONST_VK_PROCESSKEY",
	"",
	"CONST_VK_PACKET",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"CONST_VK_ATTN",
	"CONST_VK_CRSEL",
	"CONST_VK_EXSEL",
	"CONST_VK_EREOF",
	"CONST_VK_PLAY",
	"CONST_VK_ZOOM",
	"CONST_VK_NONAME",
	"CONST_VK_PA1",
	"CONST_VK_OEM_CLEAR",
};

int CLuaInterpreter::genScriptKey(SEvent &event1, SEvent &event2, QString &script) {
	if (g_szMappingKey[event1.o.key.keyvalue].isEmpty()) {
		_DMSG("not support key");
		return 1;
	}
	unsigned long long nDuration = event2.timeTick - event1.timeTick;
	// last action
	QString szAppend;
	if (event1 == event2) {
		switch (event1.o.key.keyAction) {
			case _INVISOX_EVENT_ACTION_KEYDOWN:
				szAppend.sprintf("send_event(CONST_EVENT_KEY, %s, CONST_EVENT_ACTION_CLICK, %d)",
								 QSZ(g_szMappingKey[event1.o.key.keyvalue]), _INVISOX_DEFAULT_EVENT_DURATION);
				break;
			case _INVISOX_EVENT_ACTION_KEYUP:
				szAppend.sprintf("send_event(CONST_EVENT_KEY, %s, CONST_EVENT_ACTION_KEYUP, %d)",
								 QSZ(g_szMappingKey[event1.o.key.keyvalue]), _INVISOX_DEFAULT_EVENT_DURATION);
				break;
		}
		script.append("\r\n").append(szAppend);
		return 1;
	}
	// event1 <= event2 is only keyaction is different
	// event1 < event2 is keyvalue and others are different
	// event1 != event2 is type and others are different
	else if (event1 <= event2){
		switch(event1.o.key.keyAction) {
			case _INVISOX_EVENT_ACTION_KEYDOWN:
				if (_INVISOX_DEFAULT_EVENT_DURATION < nDuration) {
						szAppend.sprintf("send_event(CONST_EVENT_KEY, %s, CONST_EVENT_ACTION_KEYDOWN, %d)",
										 QSZ(g_szMappingKey[event1.o.key.keyvalue]), _INVISOX_DEFAULT_EVENT_DURATION);
						script.append("\r\n").append(szAppend);

						szAppend.sprintf("wait(%d)", nDuration - (2 * _INVISOX_DEFAULT_EVENT_DURATION));
						script.append("\r\n").append(szAppend);

						szAppend.sprintf("send_event(CONST_EVENT_KEY, %s, CONST_EVENT_ACTION_KEYUP, %d)",
										 QSZ(g_szMappingKey[event2.o.key.keyvalue]), _INVISOX_DEFAULT_EVENT_DURATION);
						script.append("\r\n").append(szAppend);
				}
				else {
						szAppend.sprintf("send_event(CONST_EVENT_KEY, %s, CONST_EVENT_ACTION_CLICK, %d)",
										 QSZ(g_szMappingKey[event2.o.key.keyvalue]), nDuration);
						script.append("\r\n").append(szAppend);
				}
				return 0;
			case _INVISOX_EVENT_ACTION_KEYUP:
				if (_INVISOX_DEFAULT_EVENT_DURATION < nDuration) {
					szAppend.sprintf("send_event(CONST_EVENT_KEY, %s, CONST_EVENT_ACTION_KEYUP, %d)",
									 QSZ(g_szMappingKey[event1.o.key.keyvalue]), _INVISOX_DEFAULT_EVENT_DURATION);
					script.append("\r\n").append(szAppend);
					szAppend.sprintf("wait(%d)", nDuration - (2 * _INVISOX_DEFAULT_EVENT_DURATION));
				}
				else {
					szAppend.sprintf("send_event(CONST_EVENT_KEY, %s, CONST_EVENT_ACTION_KEYUP, %d)",
									 QSZ(g_szMappingKey[event1.o.key.keyvalue]), nDuration);
				}
				break;
		}
		script.append("\r\n").append(szAppend);
		return 1;
	}
	// event1 < event2 is keyvalue and others are different
	// event1 != event2 is type and others are different
	else if (event1 < event2 || event1 != event2) {
		switch(event1.o.key.keyAction) {
			case _INVISOX_EVENT_ACTION_KEYDOWN:
				if (_INVISOX_DEFAULT_EVENT_DURATION < nDuration) {
						szAppend.sprintf("send_event(CONST_EVENT_KEY, %s, CONST_EVENT_ACTION_KEYDOWN, %d)",
										 QSZ(g_szMappingKey[event1.o.key.keyvalue]), _INVISOX_DEFAULT_EVENT_DURATION);
						script.append("\r\n").append(szAppend);

						szAppend.sprintf("wait(%d)", nDuration - (2 * _INVISOX_DEFAULT_EVENT_DURATION));
				}
				else {
						szAppend.sprintf("send_event(CONST_EVENT_KEY, %s, CONST_EVENT_ACTION_CLICK, %d)",
										 QSZ(g_szMappingKey[event2.o.key.keyvalue]), nDuration);
						script.append("\r\n").append(szAppend);
				}
				break;
			case _INVISOX_EVENT_ACTION_KEYUP:
				if (_INVISOX_DEFAULT_EVENT_DURATION < nDuration) {
					szAppend.sprintf("send_event(CONST_EVENT_KEY, %s, CONST_EVENT_ACTION_KEYUP, %d)",
									 QSZ(g_szMappingKey[event1.o.key.keyvalue]), _INVISOX_DEFAULT_EVENT_DURATION);
					script.append("\r\n").append(szAppend);

					szAppend.sprintf("wait(%d)", nDuration - (2 * _INVISOX_DEFAULT_EVENT_DURATION));
				}
				else {
					szAppend.sprintf("send_event(CONST_EVENT_KEY, %s, CONST_EVENT_ACTION_KEYUP, %d)",
									 QSZ(g_szMappingKey[event1.o.key.keyvalue]), nDuration);
				}
				break;
		}
		script.append("\r\n").append(szAppend);
		return 1;
	}

	return 0;
}

int CLuaInterpreter::genScriptMouse(SEvent &event1, SEvent &event2, QString &script) {
	// last action
	if (event1.timeTick == event2.timeTick) {

	}
	return 0;
}
