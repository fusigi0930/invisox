#include "clua.h"
#include "luafunc.h"
#include <QFile>
#include "debug.h"
#include <QFileInfo>
#include "invisox_common.h"

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

void CLua::adjustLuaPath() {
	// compare path environment
	lua_getglobal(mLua, "package");
	lua_getfield(mLua, -1, "path");
	QString szEnvPath=lua_tostring(mLua, -1);
	DMSG("LUA PATH: %s", QSZ(szEnvPath));
	QFileInfo fi(m_szFile);

	if (!szEnvPath.contains(fi.absolutePath(), Qt::CaseInsensitive)) {
		QString szBuf;
		szBuf.sprintf("%s;%s/?.lua;", QSZ(szEnvPath), QSZ(fi.absolutePath()));
		lua_pop(mLua, 1);
		lua_pushstring(mLua, QSZ(szBuf));
		lua_setfield(mLua, -2, "path");
		lua_pop(mLua, 1);
	}
}

void CLua::runFile(QString szFile) {
	if (NULL == mLua) return;
	if (szFile.isEmpty()) return;
	if (!QFile::exists(szFile)) return;

	m_szFile=szFile;

	adjustLuaPath();
	if (luaL_loadfile(mLua, QSZ(m_szFile))) {
		DMSG("load %s failed!", QSZ(m_szFile));
		return;
	}

	if (lua_pcall(mLua, 0, LUA_MULTRET, 0)) {
		DMSG("run lua script failed");
		return;
	}

	DMSG("run lua script finished");
}

void CLua::runString(QString szScript) {
	if (NULL == mLua) return;
	if (szScript.isEmpty()) return;

	luaL_dostring(mLua, QSZ(szScript));
}
