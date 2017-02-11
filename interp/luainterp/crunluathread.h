#ifndef CRUNLUATHREAD_H
#define CRUNLUATHREAD_H
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "clua.h"
#include <QThread>

class CRunLuaThread : public QThread
{
private:
	lua_State *mLua;
	QString m_szLuaFunc;
	CLua *m_pLua;

public:
	CRunLuaThread(lua_State* L=NULL);
	virtual ~CRunLuaThread();

	void setFunc(char *szFunc);
	virtual void run();
};

#endif // CRUNLUATHREAD_H
