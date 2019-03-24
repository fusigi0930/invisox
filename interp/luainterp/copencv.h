#ifndef __COPENCV_H__
#define __COPENCV_H__

#include "opencv2/opencv.hpp"
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "luna_four_code.h"
#include <QString>

class COpenCV
{
private:
	lua_State *m_LuaState;
	cv::Mat m_matScreen;
	QString m_szPattern;

	void getScreen(int x=0, int y=0, int w=0, int h=0);

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
