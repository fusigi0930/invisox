#include "clua.h"
#include "luafunc.h"

CLua::CLua()
{
	mLua = NULL;
	init();
}

CLua::~CLua() {
	close();
}

void CLua::init() {
	if (NULL == mLua) {
		mLua = luaL_newstate();
	}
	luaL_openlibs(mLua);
	registerOverrideFunc();
	registerFunc();
	registerConst();

	CLua **pUserdata = reinterpret_cast<CLua**>(lua_newuserdata(mLua, sizeof(CLua*)));
	*pUserdata = this;
	lua_setglobal(mLua, GLOBAL_LUAOBJ);
}

void CLua::close() {
	if (mLua) {
		lua_close(mLua);
		mLua=NULL;
	}
}

void CLua::setResult(QString szResult) {
	m_szPrintOut = szResult;
}
