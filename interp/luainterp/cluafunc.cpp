#include "clua.h"
#include <typeinfo>

static const struct luaL_Reg override_func[] = {
	{ "print", CLua::luaPrint },
	{ NULL, NULL},
};

static const struct luaL_Reg additional_func[] = {
	{ "run_thread", CLua::luaRunThread },
	{ "send_event", CLua::luaSendEvent },
	{ "set_event_mode", CLua::luaSetEventMode },
	{ "wait", CLua::luaSleep },
	{ NULL, NULL},
};

struct SConst {
	char *name;
	int value;
};

static const struct SConst const_data[] = {
	{ "CONST_VK_BACK",     0x08 },
	{ "CONST_VK_TAB",     0x09 },
	{ "CONST_VK_CLEAR",     0x0C },
	{ "CONST_VK_RETURN",     0x0D },
	{ "CONST_VK_SHIFT",     0x10 },
	{ "CONST_VK_CONTROL",     0x11  },
	{ "CONST_VK_MENU",     0x12  },
	{ "CONST_VK_PAUSE",     0x13  },
	{ "CONST_VK_CAPITAL",     0x14  },
	{ "CONST_VK_KANA",     0x15  },
	{ "CONST_VK_HANGUL",     0x15  },
	{ "CONST_VK_JUNJA",     0x17  },
	{ "CONST_VK_FINAL",     0x18  },
	{ "CONST_VK_HANJA",     0x19  },
	{ "CONST_VK_KANJI",     0x19  },
	{ "CONST_VK_ESCAPE",     0x1B  },
	{ "CONST_VK_CONVERT",     0x1C  },
	{ "CONST_VK_NONCONVERT",     0x1D  },
	{ "CONST_VK_ACCEPT",     0x1E  },
	{ "CONST_VK_MODECHANGE",     0x1F  },
	{ "CONST_VK_SPACE",     0x20  },
	{ "CONST_VK_PRIOR",     0x21  },
	{ "CONST_VK_NEXT",     0x22  },
	{ "CONST_VK_END",     0x23  },
	{ "CONST_VK_HOME",     0x24  },
	{ "CONST_VK_LEFT",     0x25  },
	{ "CONST_VK_UP",     0x26  },
	{ "CONST_VK_RIGHT",     0x27  },
	{ "CONST_VK_DOWN",     0x28  },
	{ "CONST_VK_SELECT",     0x29  },
	{ "CONST_VK_PRINT",     0x2A  },
	{ "CONST_VK_EXECUTE",     0x2B  },
	{ "CONST_VK_SNAPSHOT",     0x2C  },
	{ "CONST_VK_INSERT",     0x2D  },
	{ "CONST_VK_DELETE",     0x2E  },
	{ "CONST_VK_HELP",     0x2F  },
	{ "CONST_VK_0",     0x30 },
	{ "CONST_VK_1",     0x31 },
	{ "CONST_VK_2",     0x32 },
	{ "CONST_VK_3",     0x33 },
	{ "CONST_VK_4",     0x34 },
	{ "CONST_VK_5",     0x35 },
	{ "CONST_VK_6",     0x36 },
	{ "CONST_VK_7",     0x37 },
	{ "CONST_VK_8",     0x38 },
	{ "CONST_VK_9",     0x39 },
	{ "CONST_VK_A",     0x41 },
	{ "CONST_VK_B",     0x42 },
	{ "CONST_VK_C",     0x43 },
	{ "CONST_VK_D",     0x44 },
	{ "CONST_VK_E",     0x45 },
	{ "CONST_VK_F",     0x46 },
	{ "CONST_VK_G",     0x47 },
	{ "CONST_VK_H",     0x48 },
	{ "CONST_VK_I",     0x49 },
	{ "CONST_VK_J",     0x4A },
	{ "CONST_VK_K",     0x4B },
	{ "CONST_VK_L",     0x4C },
	{ "CONST_VK_M",     0x4D },
	{ "CONST_VK_N",     0x4E },
	{ "CONST_VK_O",     0x4F },
	{ "CONST_VK_P",     0x50 },
	{ "CONST_VK_Q",     0x51 },
	{ "CONST_VK_R",     0x52 },
	{ "CONST_VK_S",     0x53 },
	{ "CONST_VK_T",     0x54 },
	{ "CONST_VK_U",     0x55 },
	{ "CONST_VK_V",     0x56 },
	{ "CONST_VK_W",     0x57 },
	{ "CONST_VK_X",     0x58 },
	{ "CONST_VK_Y",     0x59 },
	{ "CONST_VK_Z",     0x5A },
	{ "CONST_VK_LWIN",     0x5B  },
	{ "CONST_VK_RWIN",     0x5C  },
	{ "CONST_VK_APPS",     0x5D  },
	{ "CONST_VK_SLEEP",     0x5F  },
	{ "CONST_VK_NUMPAD0",     0x60 },
	{ "CONST_VK_NUMPAD1",     0x61 },
	{ "CONST_VK_NUMPAD2",     0x62 },
	{ "CONST_VK_NUMPAD3",     0x63 },
	{ "CONST_VK_NUMPAD4",     0x64 },
	{ "CONST_VK_NUMPAD5",     0x65 },
	{ "CONST_VK_NUMPAD6",     0x66 },
	{ "CONST_VK_NUMPAD7",     0x67 },
	{ "CONST_VK_NUMPAD8",     0x68 },
	{ "CONST_VK_NUMPAD9",     0x69 },
	{ "CONST_VK_MULTIPLY",     0x6A },
	{ "CONST_VK_ADD",     0x6B },
	{ "CONST_VK_SEPARATOR",     0x6C },
	{ "CONST_VK_SUBTRACT",     0x6D },
	{ "CONST_VK_DECIMAL",     0x6E },
	{ "CONST_VK_DIVIDE",     0x6F },
	{ "CONST_VK_F1",     0x70 },
	{ "CONST_VK_F2",     0x71 },
	{ "CONST_VK_F3",     0x72 },
	{ "CONST_VK_F4",     0x73 },
	{ "CONST_VK_F5",     0x74 },
	{ "CONST_VK_F6",     0x75 },
	{ "CONST_VK_F7",     0x76 },
	{ "CONST_VK_F8",     0x77 },
	{ "CONST_VK_F9",     0x78 },
	{ "CONST_VK_F10",     0x79 },
	{ "CONST_VK_F11",     0x7A },
	{ "CONST_VK_F12",     0x7B },
	{ "CONST_VK_F13",     0x7C },
	{ "CONST_VK_F14",     0x7D },
	{ "CONST_VK_F15",     0x7E },
	{ "CONST_VK_F16",     0x7F },
	{ "CONST_VK_F17",     0x80 },
	{ "CONST_VK_F18",     0x81 },
	{ "CONST_VK_F19",     0x82 },
	{ "CONST_VK_F20",     0x83 },
	{ "CONST_VK_F21",     0x84 },
	{ "CONST_VK_F22",     0x85 },
	{ "CONST_VK_F23",     0x86 },
	{ "CONST_VK_F24",     0x87 },
	{ "CONST_VK_NUMLOCK",     0x90 },
	{ "CONST_VK_SCROLL",     0x91 },
	{ "CONST_VK_LSHIFT",     0xA0 },
	{ "CONST_VK_RSHIFT",     0xA1 },
	{ "CONST_VK_LCONTROL",     0xA2 },
	{ "CONST_VK_RCONTROL",     0xA3 },
	{ "CONST_VK_LMENU",     0xA4 },
	{ "CONST_VK_RMENU",     0xA5 },
	{ "CONST_VK_BROWSER_BACK",     0xA6  },
	{ "CONST_VK_BROWSER_FORWARD",     0xA7  },
	{ "CONST_VK_BROWSER_REFRESH",     0xA8  },
	{ "CONST_VK_BROWSER_STOP",     0xA9  },
	{ "CONST_VK_BROWSER_SEARCH",     0xAA  },
	{ "CONST_VK_BROWSER_FAVORITES",     0xAB  },
	{ "CONST_VK_BROWSER_HOME",     0xAC  },
	{ "CONST_VK_VOLUME_MUTE",     0xAD  },
	{ "CONST_VK_VOLUME_DOWN",     0xAE  },
	{ "CONST_VK_VOLUME_UP",     0xAF  },
	{ "CONST_VK_MEDIA_NEXT_TRACK",     0xB0  },
	{ "CONST_VK_MEDIA_PREV_TRACK",     0xB1  },
	{ "CONST_VK_MEDIA_STOP",     0xB2  },
	{ "CONST_VK_MEDIA_PLAY_PAUSE",     0xB3  },
	{ "CONST_VK_MEDIA_LAUNCH_MAIL",     0xB4  },
	{ "CONST_VK_MEDIA_LAUNCH_MEDIA_SELECT",     0xB5  },
	{ "CONST_VK_MEDIA_LAUNCH_APP1",     0xB6  },
	{ "CONST_VK_MEDIA_LAUNCH_APP2",     0xB7  },
	{ "CONST_VK_OEM_1",     0xBA },
	{ "CONST_VK_OEM_PLUS",     0xBB },
	{ "CONST_VK_OEM_COMMA",     0xBC },
	{ "CONST_VK_OEM_MINUS",     0xBD },
	{ "CONST_VK_OEM_PERIOD",     0xBE },
	{ "CONST_VK_OEM_2",     0xBF },
	{ "CONST_VK_OEM_3",     0xC0 },
	{ "CONST_VK_OEM_4",     0xDB },
	{ "CONST_VK_OEM_5",     0xDC },
	{ "CONST_VK_OEM_6",     0xDD },
	{ "CONST_VK_OEM_7",     0xDE },
	{ "CONST_VK_OEM_8",     0xDF },
	{ "CONST_VK_OEM_102",     0xE2 },
	{ "CONST_VK_PROCESSKEY",     0xE5 },
	{ "CONST_VK_PACKET",     0xE7 },
	{ "CONST_VK_ATTN",     0xF6  },
	{ "CONST_VK_CRSEL",     0xF7  },
	{ "CONST_VK_EXSEL",     0xF8  },
	{ "CONST_VK_EREOF",     0xF9  },
	{ "CONST_VK_PLAY",     0xFA  },
	{ "CONST_VK_ZOOM",     0xFB  },
	{ "CONST_VK_NONAME",     0xFC  },
	{ "CONST_VK_PA1",     0xFD  },
	{ "CONST_VK_OEM_CLEAR",     0xFE  },

	{ "CONST_EVENT_KEY",		0 },
	{ "CONST_EVENT_MOUSE",		1 },

	{ "CONST_EVENT_ACTION_DOWN",	0 },
	{ "CONST_EVENT_ACTION_UP",		1 },
	{ "CONST_EVENT_ACTION_CLICK",	2 },
	{ "CONST_EVENT_ACTION_RDOWN",	3 },
	{ "CONST_EVENT_ACTION_RUP",		4 },
	{ "CONST_EVENT_ACTION_RCLICK",	5 },
	{ "CONST_EVENT_ACTION_MOVE",	6 },

	{ "CONST_MODE_EVENT_WM",	1 },
	{ "CONST_MODE_EVENT_FUNC",	0 },

	{ NULL, NULL},
};

void CLua::registerConst() {
	if (NULL == mLua) return;

	for (int i=0; NULL != const_data[i].name; i++) {
		lua_pushnumber(mLua, const_data[i].value);
		lua_setglobal(mLua, const_data[i].name);
	}
}

void CLua::registerFunc() {
	if (NULL == mLua) return;

	for (int i=0; NULL != additional_func[i].func; i++) {
		lua_pushcfunction(mLua, additional_func[i].func);
		lua_setglobal(mLua, additional_func[i].name);
	}
}

void CLua::registerOverrideFunc() {
	if (NULL == mLua) return;

	for (int i=0; NULL != override_func[i].func; i++) {
		lua_pushcfunction(mLua, override_func[i].func);
		lua_setglobal(mLua, override_func[i].name);
	}
}

int CLua::luaPrint(lua_State *L) {
	CLua *pLua=NULL;
	CLua **p;
	QString outString;
	lua_getglobal(L, GLOBAL_LUAOBJ);

	for (int i=1; i<=lua_gettop(L); i++) {
		switch (lua_type(L, i)) {
			case LUA_TUSERDATA:
				p=reinterpret_cast<CLua**>(lua_touserdata(L, i));
				if (NULL != p && typeid(CLua) == typeid(**p))
					pLua=*p;
				break;
			case LUA_TSTRING:
				outString.append(lua_tostring(L, i));
				break;
			case LUA_TNUMBER:
				outString.append(QString().sprintf("%d",lua_tointeger(L,i)));
				break;
			default:
				//outString.append(lua_tostring(L, i));
				break;
		}
	}

	if (pLua) {
		pLua->setResult(outString);
	}

	return 0;
}

int CLua::luaRunThread(lua_State *L) {
	return 0;
}

int CLua::luaSendEvent(lua_State *L) {
	return 0;
}

int CLua::luaSetEventMode(lua_State *L) {
	return 0;
}

int CLua::luaSleep(lua_State *L) {
	return 0;
}
