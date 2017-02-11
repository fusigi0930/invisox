#ifndef CLUA_H
#define CLUA_H
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include <QString>
#include <map>
#include <vector>

#define GLOBAL_LUAOBJ "__LUA_OBJECT__"

class QThread;

class CLua
{
private:
	void registerOverrideFunc();
	void registerFunc();
	void registerConst();

	static void sendInputFuncEvent(std::vector<int> *vt);
	static void sendInputWmEvent(std::vector<int> *vt);

protected:
	lua_State *mLua;
	QString m_szPrintOut;
	std::map<QString, QThread*> m_mapLuaThread;

public:
	CLua();
	virtual ~CLua();

	void init();
	void close();
	void setResult(QString szResult);

	// override original Lua function
	static int luaPrint(lua_State* L);

	// additional functions
	static int luaRunThread(lua_State *L);
	static int luaSendEvent(lua_State *L);
	static int luaSetEventMode(lua_State *L);
	static int luaSleep(lua_State *L);

};

#endif // CLUA_H
