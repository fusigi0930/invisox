
#include <windows.h>
#include <QFile>
#include "copencv.h"
#include "debug.h"
#include "invisox_common.h"
#include "cluainterpreter.h"
#include "interp_comm.h"

#include <QRect>

#define TEST_CV_WIN "__CVWin__"

char COpenCV::className[] = "opencv";
Luna<COpenCV>::FunctionType COpenCV::methods[] = {
	{ "set_pattern", &COpenCV::setPattern },
	{ "cap_screen", &COpenCV::captureScreen },
	{ "show_screen", &COpenCV::showScreen },
	{ "match", &COpenCV::match },
	{ "rect", &COpenCV::rect },
	{ nullptr, nullptr }
};

Luna<COpenCV>::PropertyType COpenCV::properties[] = {
	{ "prop", &COpenCV::getProp, &COpenCV::setProp },
	{ nullptr, nullptr, nullptr }
};

#if 0
static HBITMAP CreateBMP(int nBit, int nWidth, int nHeight) {
	HDC hdc=::GetDC(nullptr);
	HBITMAP hBmp=nullptr;
	BITMAPINFOHEADER binfo={0};
	binfo.biBitCount=nBit;
	binfo.biSize=sizeof(BITMAPINFOHEADER);
	binfo.biHeight=nHeight;
	binfo.biWidth=nWidth;
	binfo.biPlanes=1;
	binfo.biCompression=BI_RGB;

	unsigned char *pDIB=nullptr;

	hBmp=CreateDIBSection(hdc, reinterpret_cast<BITMAPINFO*>(&binfo), DIB_RGB_COLORS, reinterpret_cast<void **>(&pDIB), nullptr, nullptr);
	::ReleaseDC(nullptr, hdc);
	return hBmp;
}
#endif

COpenCV::COpenCV(lua_State *L) {
    //_DMSG("");
	m_LuaState=L;
    m_capScreen = nullptr;
}

COpenCV::~COpenCV() {
#if defined(DEBUG)
	cv::destroyAllWindows();
#endif
    if (m_capScreen) {
        cmCvFreeMem(m_capScreen);
        m_capScreen = nullptr;
    }
    //_DMSG("");
}

int COpenCV::getProp(lua_State *L) {
	return 0;
}

int COpenCV::setProp(lua_State *L) {
	return 0;
}

int COpenCV::setPattern(lua_State *L) {
	if (nullptr == L) {
		return 0;
	}

	lua_gettop(L);
	m_szPattern=const_cast<char*>(lua_tostring(L, -1));

	return 0;
}

void COpenCV::getScreen() {
    //_DMSG("");
    if (m_capScreen) {
        cmCvFreeMem(m_capScreen);
        m_capScreen = nullptr;
    }
    m_capScreen = cmCvCapScreen();
}

int COpenCV::captureScreen(lua_State *L) {
	_DMSG("");
	if (nullptr == L) {
		return 0;
	}

	lua_gettop(L);
	int x=lua_tointeger(L, 1);
	int y=lua_tointeger(L, 2);
	int width=lua_tointeger(L, 3);
	int height=lua_tointeger(L, 4);

	_DMSG("%d, %d, %d, %d", x, y, width, height);

    getScreen();

	return 0;
}

int COpenCV::showScreen(lua_State *L) {
	_DMSG("");
	if (nullptr == L) {
		return 0;
	}

    if (nullptr == m_capScreen) {
		return 0;
	}

    cmCvShow("show window", m_capScreen);

	return 0;
}

int COpenCV::match(lua_State *L) {
	//////////////////////////////////////
	//
	// argument 1: image file name, use screen if it is null
	// argument 2: pattern file name
	// argument 3-6: x, y, w, h in argument 1
	//
	// return x, y
	//
	if (nullptr == L) return 0;

	_DMSG("");
	lua_gettop(L);
	// get argument 3-6
	int x=lua_tointeger(L, 3);
	int y=lua_tointeger(L, 4);
	int w=lua_tointeger(L, 5);
	int h=lua_tointeger(L, 6);

	// get argument 2
	m_szPattern=lua_tostring(L, 2);
	_DMSG("pattern file: %s", QSZ(m_szPattern));
    SMatchData data = {0};
    cmCvMatch("", m_szPattern, x, y, w, h, &data);

    _DMSG("match out: %d %d", data.x, data.y);
    lua_pushinteger(L, data.x);
    lua_pushinteger(L, data.y);
    return 2;
}

int COpenCV::rect(lua_State *L) {
	//////////////////////////////////////
	//
	// argument 1-: x, y, w, h
	//
	if (nullptr == L) return 0;

	lua_gettop(L);
	// get argument
    QRect rect;
    rect.setX(lua_tointeger(L, 1));
    rect.setY(lua_tointeger(L, 2));
    rect.setWidth(lua_tointeger(L, 3));
    rect.setHeight(lua_tointeger(L, 4));

    if (nullptr == m_capScreen) return 0;

    cmCvRect(m_capScreen, rect);

	return 0;
}

void luaTest() {

}
