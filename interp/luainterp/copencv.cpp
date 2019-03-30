#include "copencv.h"
#include <windows.h>
#include <QFile>
#include <opencv2/highgui/highgui_c.h>
#include "debug.h"
#include "invisox_common.h"

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

COpenCV::COpenCV(lua_State *L) {
	_DMSG("");
	m_LuaState=L;

}

COpenCV::~COpenCV() {
#if defined(DEBUG)
	cv::destroyAllWindows();
#endif
	_DMSG("");
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

void COpenCV::getScreen(int x, int y, int w, int h) {
	_DMSG("");
	HDC hdc=::GetDC(nullptr);
	if (nullptr == hdc) return;

	DEVMODE devMode={0};
	EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &devMode);

	_DMSG("bits: %d", devMode.dmBitsPerPel);
	HDC hCompDC=::CreateCompatibleDC(hdc);
	HBITMAP hDestBmp=CreateBMP(24 /*devMode.dmBitsPerPel*/, w, h);
	HBITMAP hOldBmp=reinterpret_cast<HBITMAP>(::SelectObject(hCompDC, hDestBmp));

	::BitBlt(hCompDC, 0, 0, w, h, hdc, x, y, SRCCOPY);
	m_matScreen.create(h, w, CV_8UC3 /*(devMode.dmBitsPerPel/8)*/);

	BITMAP bmp;
	::GetObject(hDestBmp, sizeof(BITMAP), &bmp);
	::GetBitmapBits(hDestBmp, bmp.bmWidthBytes * bmp.bmHeight, m_matScreen.data);

	if (hOldBmp && hCompDC) ::SelectObject(hCompDC, hOldBmp);
	if (hCompDC) ::DeleteDC(hCompDC);
	if (hdc) ::ReleaseDC(nullptr, hdc);
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

	getScreen(x, y, width, height);

	return 0;
}

int COpenCV::showScreen(lua_State *L) {
	_DMSG("");
	if (nullptr == L) {
		return 0;
	}

	if (!m_matScreen.data) {
		return 0;
	}

	cv::namedWindow(TEST_CV_WIN, cv::WINDOW_AUTOSIZE);

	cv::imshow(TEST_CV_WIN, m_matScreen);
	cv::waitKey(0);

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

	// get arguments 1
	if (LUA_TNIL == lua_type(L, 1)) {
		getScreen(x, y, w, h);
	}
	else {
		if (!QFile::exists(lua_tostring(L, 1))) {
			DMSG("file file is not exist");
			return 0;
		}
		m_matScreen=cv::imread(lua_tostring(L, 1), CV_LOAD_IMAGE_COLOR);
	}

	// detect matrix data
	if (!m_matScreen.data) {
		return 0;
	}

	if (!QFile::exists(m_szPattern)) {
		DMSG("pattern file is not exist");
		return 0;
	}
	cv::Mat matPattern=cv::imread(QSZ(m_szPattern), CV_LOAD_IMAGE_ANYCOLOR | CV_LOAD_IMAGE_ANYDEPTH);
	//cv::Mat matPattern;


	DMSG("screen: %d,%d, pattern: %d,%d -- %d %d ", m_matScreen.depth(), m_matScreen.type(), matPattern.depth(), matPattern.type(),
												CV_8UC3, CV_8UC4);

	cv::Mat cvImgResult;
	cvImgResult.create(m_matScreen.rows - matPattern.rows +1,
					   m_matScreen.cols - matPattern.cols + 1, m_matScreen.channels());

	cv::matchTemplate(m_matScreen, matPattern, cvImgResult, CV_TM_CCOEFF);

	cv::Point minPos, maxPos;
	double fMin, fMax;
	cv::minMaxLoc(cvImgResult, &fMin, &fMax, &minPos, &maxPos, cv::Mat());
	cvImgResult.release();
	matPattern.release();
	//mat.release();

	lua_pushinteger(L, maxPos.x);
	lua_pushinteger(L, maxPos.y);
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
	cv::Rect rect;
	rect.x=lua_tointeger(L, 1);
	rect.y=lua_tointeger(L, 2);
	rect.width=lua_tointeger(L, 3);
	rect.height=lua_tointeger(L, 4);

	if (!m_matScreen.data) return 0;

	cv::rectangle(m_matScreen, rect, cvScalar(70,70,200), 2, 8, 0);

	return 0;
}
