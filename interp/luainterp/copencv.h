#ifndef __COPENCV_H__
#define __COPENCV_H__

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "luna_four_code.h"
#include <QString>

class COpenCV
{
#ifdef TESTCODE
public:
#else
private:
#endif
	lua_State *m_LuaState;
	QString m_szPattern;
    void *m_capScreen;
    void getScreen();

public:
	static char className[];
	static Luna<COpenCV>::FunctionType methods[];
	static Luna<COpenCV>::PropertyType properties[];

	COpenCV(lua_State *L);
	virtual ~COpenCV();

	int getProp(lua_State *L);
	int setProp(lua_State *L);

	int setPattern(lua_State *L);
	int captureScreen(lua_State *L);
	int showScreen(lua_State *L);
	int match(lua_State *L);
	int rect(lua_State *L);

};

#endif // __COPENCV_H__
