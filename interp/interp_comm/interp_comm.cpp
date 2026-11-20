#include "interp_comm.h"

#include "debug.h"
#include <iostream>
#include <sstream>
#include <vector>

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>

#include <d3d11.h>
#include <dxgi1_2.h>
#include <mutex>

#include <QRect>
#include <QThread>
#include "invisox_common.h"

#ifdef Q_OS_WIN
#include <windows.h>
//#ifdef NULL
//#undef NULL
//#define NULL nullptr
//#endif

static ID3D11Device* d3dDevice = nullptr;
static ID3D11DeviceContext* d3dContext = nullptr;
static IDXGIOutputDuplication* deskDupl = nullptr;
static ID3D11Texture2D* stagingTexture = nullptr;
static int screenWidth = 0;
static int screenHeight = 0;
static bool isInit = false;
static std::mutex capMutex;
static cv::Mat cacheFrame;

static bool scInit(int disp_id) {
	std::lock_guard<std::mutex> lock(capMutex);

	if (isInit) return true;

	HRESULT hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, 
                                   nullptr, 0, D3D11_SDK_VERSION, &d3dDevice, nullptr, &d3dContext);
	if (FAILED(hr)) {
		_DMSG("d3d11create device failed!");
		return false;
	}

	IDXGIDevice* dxgiDevice = nullptr;
    hr = d3dDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);
    if (FAILED(hr)) {
		_DMSG("query interface dxgi failed!");
		return false;
	}

	IDXGIAdapter* dxgiAdapter = nullptr;
    hr = dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter);
    dxgiDevice->Release();
    if (FAILED(hr)) {
		_DMSG("get parent adapter failed!");
		return false;
	}

	IDXGIOutput* dxgiOutput = nullptr;
    hr = dxgiAdapter->EnumOutputs(disp_id, &dxgiOutput);
    dxgiAdapter->Release();
    if (FAILED(hr)) {
		_DMSG("enum adapter output failed!");
		return false;
	}

	IDXGIOutput1* dxgiOutput1 = nullptr;
    hr = dxgiOutput->QueryInterface(__uuidof(IDXGIOutput1), (void**)&dxgiOutput1);
    dxgiOutput->Release();
    if (FAILED(hr)) {
		_DMSG("query dxgi interface failed!");
		return false;
	}

	hr = dxgiOutput1->DuplicateOutput(d3dDevice, &deskDupl);
    dxgiOutput1->Release();
    if (FAILED(hr)) {
		_DMSG("duplicate output failed!");
		return false;
	}

	DXGI_OUTDUPL_DESC duplDesc;
    deskDupl->GetDesc(&duplDesc);
    screenWidth = duplDesc.ModeDesc.Width;
    screenHeight = duplDesc.ModeDesc.Height;

    D3D11_TEXTURE2D_DESC stagingDesc;
    ZeroMemory(&stagingDesc, sizeof(stagingDesc));
    stagingDesc.Width = screenWidth;
    stagingDesc.Height = screenHeight;
    stagingDesc.MipLevels = 1;
    stagingDesc.ArraySize = 1;
    stagingDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    stagingDesc.SampleDesc.Count = 1;
    stagingDesc.Usage = D3D11_USAGE_STAGING;
    stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    
    hr = d3dDevice->CreateTexture2D(&stagingDesc, nullptr, &stagingTexture);
    
    if (FAILED(hr)) {
		_DMSG("create texture 2d failed!");
		return false;
    }

    isInit= true;
    return true;
}

static void scDeinit() {
	std::lock_guard<std::mutex> lock(capMutex);

	if (stagingTexture) { stagingTexture->Release(); stagingTexture = nullptr; }
    if (deskDupl) { deskDupl->Release(); deskDupl = nullptr; }
    if (d3dContext) { d3dContext->Release(); d3dContext = nullptr; }
    if (d3dDevice) { d3dDevice->Release(); d3dDevice = nullptr; }

	cacheFrame.release();
	isInit = false;
}

#if 0
extern "C" BOOL WINAPI DllMain(
  [[maybe_unused]] HANDLE hinstDLL,
  [[maybe_unused]] DWORD fdwReason,
  [[maybe_unused]] LPVOID lpvReserved
)
{
	switch (fdwReason)
	{
		case DLL_PROCESS_ATTACH:
			_DMSG("dll attached");
			break;
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			_DMSG("dll deattached");
			break;
	}
	return true;
}
#endif


static cv::Mat cropFrame(QRect *rect) {
    if (cacheFrame.empty()) return cv::Mat();

	if (nullptr == rect || rect->isNull()) {
		return cacheFrame.clone();
	}
	else {
		QRect safeRect = rect->intersected(QRect(0, 0, screenWidth, screenHeight));
		if (safeRect.isEmpty()) return cv::Mat();
		cv::Rect cvSafeRect(safeRect.x(), safeRect.y(), safeRect.width(), safeRect.height());
		return cacheFrame(cvSafeRect).clone();
	}

	return cv::Mat();
}

static cv::Mat getScreen(QRect *rect, int disp_id) {
	if (!isInit) {
        if (!scInit(disp_id)) {
			_DMSG("init failed!");
			return cv::Mat();
		}
	}

    std::lock_guard<std::mutex> lock(capMutex);
	IDXGIResource* desktopResource = nullptr;
    DXGI_OUTDUPL_FRAME_INFO frameInfo;
    HRESULT hr = E_FAIL;

    hr = deskDupl->AcquireNextFrame(0, &frameInfo, &desktopResource);
    
    if (hr == DXGI_ERROR_ACCESS_LOST) {
        capMutex.unlock();
        scDeinit();
        scInit(disp_id);
        return cv::Mat();
	}

	if (hr == DXGI_ERROR_WAIT_TIMEOUT || FAILED(hr)) {
        return cropFrame(rect);
    }


	ID3D11Texture2D* desktopTexture = nullptr;
    hr = desktopResource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&desktopTexture);
    desktopResource->Release();

    if (SUCCEEDED(hr)) {
        d3dContext->CopyResource(stagingTexture, desktopTexture);
        desktopTexture->Release();
    }
    deskDupl->ReleaseFrame();

    D3D11_MAPPED_SUBRESOURCE mapped;
    hr = d3dContext->Map(stagingTexture, 0, D3D11_MAP_READ, 0, &mapped);
    
    if (FAILED(hr)) {
		_DMSG("d3d context map get failed!");
		return cropFrame(rect);
	}

    cv::Mat fullScreenMat(screenHeight, screenWidth, CV_8UC4);
    uchar* sourceData = static_cast<uchar*>(mapped.pData);
    for (int y = 0; y < screenHeight; ++y) {
        memcpy(fullScreenMat.ptr(y), sourceData + y * mapped.RowPitch, screenWidth * 4);
    }

    d3dContext->Unmap(stagingTexture, 0);
    cv::cvtColor(fullScreenMat, cacheFrame, cv::COLOR_BGRA2BGR);
    return cropFrame(rect);
}

static int cmEventKeyInputFunc(SKeyEvent *event) {
	INPUT in;
	in.type = INPUT_KEYBOARD;
	in.ki.wVk=event->key;
	in.ki.wScan=0;
	in.ki.time=0;
	in.ki.dwExtraInfo=NULL;
	_DMSG("in");

	switch (event->action) {
		case _EVENT_ACTION_DOWN:
			in.ki.dwFlags=0;
			SendInput(1, &in, sizeof(in));
			QThread::msleep(event->miniSec);
			break;
		case _EVENT_ACTION_UP:
			in.ki.dwFlags=KEYEVENTF_KEYUP;
			SendInput(1, &in, sizeof(in));
			QThread::msleep(event->miniSec);
			break;
		case _EVENT_ACTION_CLICK:
			in.ki.dwFlags=0;
			SendInput(1, &in, sizeof(in));
			QThread::msleep(50);
			in.ki.dwFlags=KEYEVENTF_KEYUP;
			SendInput(1, &in, sizeof(in));
			QThread::msleep(50);
			break;
		default:
			break;
	}

	return 0;
}

static int cmEventKeyWM(SKeyEvent *event) {
	_DMSG("in");
	switch (event->action) {
		case _EVENT_ACTION_DOWN:
			SendMessage(GetForegroundWindow(), WM_KEYDOWN, static_cast<WPARAM>(event->key), 1);
			QThread::msleep(event->miniSec);
			break;
		case _EVENT_ACTION_UP:
			SendMessage(GetForegroundWindow(), WM_KEYUP, static_cast<WPARAM>(event->key), 1);
			QThread::msleep(event->miniSec);
			break;
		case _EVENT_ACTION_CLICK:
			SendMessage(GetForegroundWindow(), WM_KEYDOWN, static_cast<WPARAM>(event->key), 1);
			QThread::msleep(50);
			SendMessage(GetForegroundWindow(), WM_KEYUP, static_cast<WPARAM>(event->key), 1);
			QThread::msleep(50);
			break;
	}

	return 0;
}

static int cmEventKey(int mode, SKeyEvent *event) {
	int ret = -1;
	switch(mode) {
		default: 
			_DMSG("unsupport mode!");
			break;
		case _MODE_INPUT_EVENT_WM:
			ret = cmEventKeyWM(event);
			break;
		case _MODE_INPUT_EVENT_FUNC:
			ret = cmEventKeyInputFunc(event);
			break;
		case _MODE_INPUT_EVENT_RASPBERRY:
			_DMSG("coming soon");
			break;
	}
	return ret;
}

static int cmEventMouseInputFunc(SMouseEvent *event) {
	INPUT in;
	in.type = INPUT_MOUSE;
	in.mi.dx=event->x*(65535/::GetSystemMetrics(SM_CXSCREEN));
	in.mi.dy=event->y*(65535/::GetSystemMetrics(SM_CYSCREEN));
	in.mi.mouseData=0;
	in.mi.time=0;
	in.mi.dwExtraInfo=NULL;

	_DMSG("in");

	switch (event->action) {
		default: break;
		case _EVENT_ACTION_DOWN:
			in.mi.dwFlags=MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_LEFTDOWN;
			::SendInput(1, &in, sizeof(in));
			QThread::msleep(event->miniSec);
			break;
		case _EVENT_ACTION_UP:
			in.mi.dwFlags=MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_LEFTUP;
			::SendInput(1, &in, sizeof(in));
			QThread::msleep(event->miniSec);
			break;
		case _EVENT_ACTION_CLICK:
			for(int i=0; i<event->miniSec; i++) {
				in.mi.dwFlags=MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_LEFTDOWN;
				::SendInput(1, &in, sizeof(in));
				QThread::msleep(50);
				in.mi.dwFlags=MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_LEFTUP;
				::SendInput(1, &in, sizeof(in));
				QThread::msleep(50);
			}
			break;
		case _EVENT_ACTION_RDOWN:
			in.mi.dwFlags=MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_RIGHTDOWN;
			::SendInput(1, &in, sizeof(in));
			QThread::msleep(event->miniSec);
			break;
		case _EVENT_ACTION_RUP:
			in.mi.dwFlags=MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_RIGHTUP;
			::SendInput(1, &in, sizeof(in));
			QThread::msleep(event->miniSec);
			break;
		case _EVENT_ACTION_RCLICK:
			for(int i=0; i<event->miniSec; i++) {
				in.mi.dwFlags=MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_RIGHTDOWN;
				::SendInput(1, &in, sizeof(in));
				QThread::msleep(50);
				in.mi.dwFlags=MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_RIGHTUP;
				::SendInput(1, &in, sizeof(in));
				QThread::msleep(50);
			}
			break;
		case _EVENT_ACTION_MOVE:
			in.mi.dwFlags=MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_MOVE;
			::SendInput(1, &in, sizeof(in));
			QThread::msleep(event->miniSec);
			break;
	}
	return 0;
}

static int cmEventMouseWM(SMouseEvent *event) {
	_DMSG("in");
	switch (event->action) {
		default: break;
		case _EVENT_ACTION_DOWN:
			::SendMessage(NULL, WM_LBUTTONDOWN, 0, static_cast<LPARAM>(event->x | event->y << 16));
			QThread::msleep(event->miniSec);
			break;
		case _EVENT_ACTION_UP:
			::SendMessage(NULL, WM_LBUTTONUP, 0, static_cast<LPARAM>(event->x | event->y << 16));
			QThread::msleep(event->miniSec);
			break;
		case _EVENT_ACTION_CLICK:
			for(int i=0; i<event->miniSec; i++) {
				::SendMessage(NULL, WM_LBUTTONDOWN, 0, static_cast<LPARAM>(event->x | event->y << 16));
				QThread::msleep(50);
				::SendMessage(NULL, WM_LBUTTONUP, 0, static_cast<LPARAM>(event->x | event->y << 16));
				QThread::msleep(50);
			}
			break;
		case _EVENT_ACTION_RDOWN:
			::SendMessage(NULL, WM_RBUTTONDOWN, 0, static_cast<LPARAM>(event->x | event->y << 16));
			QThread::msleep(event->miniSec);
			break;
		case _EVENT_ACTION_RUP:
			::SendMessage(NULL, WM_RBUTTONDOWN, 0, static_cast<LPARAM>(event->x | event->y << 16));
			QThread::msleep(event->miniSec);
			break;
		case _EVENT_ACTION_RCLICK:
			for(int i=0; i<event->miniSec; i++) {
				::SendMessage(NULL, WM_RBUTTONDOWN, 0, static_cast<LPARAM>(event->x | event->y << 16));
				QThread::msleep(50);
				::SendMessage(NULL, WM_RBUTTONUP, 0, static_cast<LPARAM>(event->x | event->y << 16));
				QThread::msleep(50);
			}
			break;
		case _EVENT_ACTION_MOVE:
			::SendMessage(::GetForegroundWindow(), WM_MOUSEMOVE, 0, static_cast<LPARAM>(event->x | event->y << 16));
			QThread::msleep(event->miniSec);
			break;
	}
	return 0;
}

static int cmEventMouse(int mode, SMouseEvent *event) {
	int ret = -1;
	switch(mode) {
		default: 
			_DMSG("unsupport mode!");
			break;
		case _MODE_INPUT_EVENT_WM:
			ret = cmEventMouseWM(event);
			break;
		case _MODE_INPUT_EVENT_FUNC:
			ret = cmEventMouseInputFunc(event);
			break;
		case _MODE_INPUT_EVENT_RASPBERRY:
			_DMSG("coming soon");
			break;
	}
	return ret;
}

static int cmEventRpiHid(SRpiEvent *event) {
	int ret = -1;
	_DMSG("coming soon");
	return ret;
}

int cmSendEvent(int type, int mode, void *event) {
	if (nullptr == event) {
		_DMSG("event is null!");
		return 1;
	}

	int ret = -1;
	switch (type) {
		default:
			_DMSG("unsupport type!");
			break;
		case _TYPE_EVENT_KEY: {
			SKeyEvent *e = reinterpret_cast<SKeyEvent*>(event);
			ret = cmEventKey(mode, e);
		} break;
		case _TYPE_EVENT_MOUSE: {
			SMouseEvent *e = reinterpret_cast<SMouseEvent*>(event);
			ret = cmEventMouse(mode, e);			
		} break;
		case _TYPE_EVENT_RPIHID: {
			SRpiEvent *e = reinterpret_cast<SRpiEvent*>(event);
			ret = cmEventRpiHid(e);			
		} break;
		case _TYPE_EVENT_CUSTOM:
			_DMSG("reserve type!!");
			break;
	}
	return ret;
}

static double fThreshold = 0.85;

int cmCvMatch(QString img, QString patFile, int left, int top, int width, int height, SMatchData *out, int disp_id) {
	if (nullptr == out) {
		_DMSG("out data is null");
		return 3;
	}
	if(patFile.isEmpty()) {
		_DMSG("pattern file is null");
		return 1;
	}
    cv::Mat matScr;
	if (img.isEmpty()) {
		if (0 <= left && 0 <= top && 0 <= width && 0 <= height) {
			QRect rect(left, top, width, height);
			matScr = getScreen(&rect, disp_id);
		}
		else {
			matScr = getScreen(nullptr, disp_id);
		}
	}
	else {
		matScr = cv::imread(QSZ(img), cv::IMREAD_UNCHANGED);
	}
	cv::Mat matPat = cv::imread(QSZ(patFile), cv::IMREAD_UNCHANGED);
	if (true == matPat.empty() || true == matScr.empty()) {
		_DMSG("cv matrix object read failed!");
		return 2;
	}

	cv::Mat matPatBGR, matPatAlpha;
	if (4 == matPat.channels()) {
		std::vector<cv::Mat> ch;
		cv::split(matPat, ch);
		matPatAlpha = ch[3];
		cv::merge(std::vector<cv::Mat>{ch[0], ch[1], ch[2]}, matPatBGR);
	}
	else {
		matPatBGR = matPat;
	}

	cv::Mat matPatGray, matScrGray;
	cv::cvtColor(matPatBGR, matPatGray, cv::COLOR_BGR2GRAY);

	cv::cvtColor(matScr, matScrGray, cv::COLOR_BGR2GRAY);

	cv::Mat result;
	cv::matchTemplate(matScrGray, matPatGray, result, cv::TM_CCORR_NORMED, matPatAlpha);

	double maxVal;
	cv::Point maxLoc;
	cv::minMaxLoc(result, nullptr, &maxVal, nullptr, &maxLoc);
    _DMSG("max: %f, x: %d, y: %d", maxVal, maxLoc.x, maxLoc.y);
	out->x = maxLoc.x;
	out->y = maxLoc.y;

	return 0;
}

int cmInitCV(int disp_id) {
    cv::Mat scn = getScreen(nullptr, disp_id);
    scn.release();
    return 0;
}

void* cmCvCapScreen(int disp_id) {
	cv::Mat *p = new cv::Mat;
	*p = getScreen(nullptr, disp_id);
	return reinterpret_cast<void*>(p);
}

void cmCvFreeMem(void *data) {
	if (nullptr == data) {
		return;
	}
	cv::Mat *p = reinterpret_cast<cv::Mat *>(data);
	delete p;
}

void cmCvShow(QString name, void *data) {
	if (nullptr == data) {
		return;
	}
	cv::Mat *p = reinterpret_cast<cv::Mat *>(data);
	cv::namedWindow(QSZ(name), cv::WINDOW_AUTOSIZE);
	cv::imshow(QSZ(name), *p);
	cv::waitKey(0);
}

void cmCvSave(QString file, void *data) {
	if (nullptr == data) {
		return;
	}
	if (file.isEmpty()) {
		return;
	}
	cv::Mat *p = reinterpret_cast<cv::Mat *>(data);
	
}

void cmCvRect(void *data, QRect &rect) {
	if (nullptr == data) {
		return;
	}
	cv::Mat *p = reinterpret_cast<cv::Mat *>(data);
	cv::Rect cvRect(rect.x(), rect.y(), rect.width(), rect.height());
	cv::rectangle(*p, cvRect, cv::Scalar(70,70,200), 2, cv::LINE_AA);
}

int cmTest(void *p) {
#ifdef XDGI_TEST
    cv::Mat scn = getScreen(nullptr, 0);
#endif
#ifdef CVMATCH_TEST
	SMatchData data = {0};
	int ret = cmCvMatch("", "x:/temp/pattern.png", -1, -1, -1, -1, &data);
	_DMSG("ret: %d, x: %d, y: %d", ret, data.x, data.y);
#endif
    return 0;
}

#endif
