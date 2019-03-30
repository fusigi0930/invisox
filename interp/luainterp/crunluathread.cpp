#include "crunluathread.h"
#include "debug.h"
#include "invisox_common.h"

CRunLuaThread::CRunLuaThread(lua_State *L)
{
	mLua=L;
}

CRunLuaThread::~CRunLuaThread() {

}

void CRunLuaThread::setFunc(char *szFunc) {
	m_szLuaFunc=szFunc;
}

void CRunLuaThread::run() {
	if (NULL == mLua) return;

	if (m_szLuaFunc.isEmpty()) return;

	lua_getglobal(mLua, QSZ(m_szLuaFunc));

	lua_call(mLua, 0, 0);


}
