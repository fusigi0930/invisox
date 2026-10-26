#include "engine.h"
#include "invisox_common.h"
#include <windows.h>
#include <cstdio>
#include <commctrl.h>

#define _DMSG(e,...) \
{ \
	char string[512] = {0}; \
	sprintf(string, "[%s:%d] " e "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__); \
	OutputDebugStringA(string) ; \
}

__attribute__((section("invisox_shared"), shared)) HINSTANCE g_hInst = nullptr;
__attribute__((section("invisox_shared"), shared)) HMODULE g_hLib = nullptr;
__attribute__((section("invisox_shared"), shared)) HHOOK g_hHookKey = nullptr;
__attribute__((section("invisox_shared"), shared)) HHOOK g_hHookMouse = nullptr;
__attribute__((section("invisox_shared"), shared)) HANDLE g_hSharedMem = nullptr;
__attribute__((section("invisox_shared"), shared)) HANDLE g_hReadEvent = nullptr;

static const UINT_PTR SUBCLS_ID = 50666;
static bool bRecording = false;

extern "C" BOOL WINAPI DllMain(
  HANDLE hinstDLL,
  DWORD fdwReason,
  LPVOID lpvReserved
)
{
	switch (fdwReason)
	{
		case DLL_PROCESS_ATTACH:
			_DMSG("%s dll attached", __FILE__);
			break;
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			_DMSG("%s dll deattached", __FILE__);
			engEnd();
			if (g_hReadEvent) {
				::CloseHandle(g_hReadEvent);
				g_hReadEvent = nullptr;
			}
			if (g_hSharedMem) {
				::CloseHandle(g_hSharedMem);
				g_hSharedMem = nullptr;

			}
			break;
	}
	return true;
}

int engInit() {
	_DMSG("create share memory");
	g_hSharedMem = ::CreateFileMappingA(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, _INVISOX_SHARED_MEM_SIZE, _INVISOX_SHARED_MEM_NAME);
	_DMSG("create global event");
	g_hReadEvent = ::CreateEventA(nullptr, FALSE, FALSE, _INVISOX_SHARED_EVENT_NAME);
}

static void rawKeyProc(RAWINPUT *raw) {
    if (bRecording) {
        unsigned long long keyData[3] = {0};
        keyData[0] = static_cast<unsigned long long>(raw->data.keyboard.VKey);
        keyData[1] = static_cast<unsigned long long>(0);
        keyData[2] = ((raw->data.keyboard.Flags & RI_KEY_BREAK) == RI_KEY_BREAK ? _INVISOX_EVENT_ACTION_KEYUP : _INVISOX_EVENT_ACTION_KEYDOWN);
        engWriteSharedMemory(reinterpret_cast<char*>(keyData), sizeof(keyData));
    }
    else {
        USHORT vKey = static_cast<USHORT>(raw->data.keyboard.VKey);
        switch(vKey) {
        default: break;
        case VK_LSHIFT: case VK_RSHIFT: case VK_LCONTROL: case VK_RCONTROL: case VK_LMENU: case VK_RMENU:
        case VK_CONTROL: case VK_MENU: case VK_SHIFT:
            return;
        }

        if (raw->data.keyboard.Flags & RI_KEY_BREAK) {
            unsigned long long keyData[2] = {0};
            if (0 != ::GetAsyncKeyState(VK_LSHIFT) || 0 != ::GetAsyncKeyState(VK_RSHIFT)) {
                keyData[1] |= _INVISOX_KF_SHIFT;
            }
            if (0 != ::GetAsyncKeyState(VK_LCONTROL) || 0 != ::GetAsyncKeyState(VK_RCONTROL)) {
                keyData[1] |= _INVISOX_KF_CTRL;
            }
            if (0 != ::GetAsyncKeyState(VK_LMENU) || 0 != ::GetAsyncKeyState(VK_RMENU)) {
                keyData[1] |= _INVISOX_KF_ALT;
            }
            keyData[0] = raw->data.keyboard.VKey;
            engWriteSharedMemory(reinterpret_cast<char*>(keyData), sizeof(keyData));
        }
    }
}

static void rawMouseProc(RAWINPUT *raw) {
    if (bRecording) {
        unsigned long long mouseData[3] = {0};
        POINT pt;
        GetCursorPos(&pt);
        if (raw->data.mouse.usFlags & MOUSE_MOVE_RELATIVE || raw->data.mouse.usFlags & MOUSE_MOVE_ABSOLUTE) {
            mouseData[0] = _INVISOX_EVENT_ACTION_MOUSEMOVE;
        }
        else if (raw->data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN) {
            mouseData[0] = _INVISOX_EVENT_ACTION_LBDOWN;
        }
        else if (raw->data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_UP) {
            mouseData[0] = _INVISOX_EVENT_ACTION_LBUP;
        }
        else if (raw->data.mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN) {
            mouseData[0] = _INVISOX_EVENT_ACTION_RBDOWN;
        }
        else if (raw->data.mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_UP) {
            mouseData[0] = _INVISOX_EVENT_ACTION_RBUP;
        }
        else {
           mouseData[0] = _INVISOX_EVENT_ACTION_MOUSEMOVE;
        }
        mouseData[1] = static_cast<unsigned long long>(pt.x);
        mouseData[2] = static_cast<unsigned long long>(pt.y);
        engWriteSharedMemory(reinterpret_cast<char*>(mouseData), sizeof(mouseData), _INVISOX_SHARED_MEM_MOUES_INDEX);
    }

}

static LRESULT CALLBACK rawInputSubClassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    if (uMsg == WM_INPUT) {
        UINT dwSize = 0;
        GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
        if (0 < dwSize) {
            BYTE *buf = new BYTE[dwSize];
            if (dwSize == GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, buf, &dwSize, sizeof(RAWINPUTHEADER))) {
                RAWINPUT *raw = reinterpret_cast<RAWINPUT*>(buf);
                switch(raw->header.dwType) {
                case RIM_TYPEKEYBOARD:
                    rawKeyProc(raw);
                    break;
                case RIM_TYPEMOUSE:
                    rawMouseProc(raw);
                    break;
                }

            }
            delete [] buf;
        }
        return 0;
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

int engRegisterClass(void* hwnd) {
    if (nullptr == hwnd) {
        _DMSG("hwnd is null");
        return 1;
    }
    _DMSG("register sub class: %p", hwnd);
    HWND hWnd = reinterpret_cast<HWND>(hwnd);
    int ret = SetWindowSubclass(hWnd, rawInputSubClassProc, SUBCLS_ID, 0) != FALSE ? 0 : 1;

    RAWINPUTDEVICE rd[2];
    rd[0].usUsagePage = 0x01;
    rd[0].usUsage = 0x06; //keyboard
    rd[0].dwFlags = RIDEV_INPUTSINK;
    rd[0].hwndTarget = hWnd;

    rd[1].usUsagePage = 0x01;
    rd[1].usUsage = 0x02; //mouse
    rd[1].dwFlags = RIDEV_INPUTSINK;
    rd[1].hwndTarget = hWnd;

    RegisterRawInputDevices(rd, 2, sizeof(RAWINPUTDEVICE));

    return ret;
}

void engUnregisterClass(void* hwnd) {
    if (nullptr == hwnd) {
        _DMSG("hwnd is null");
        return;
    }
    _DMSG("unregister sub class")
    HWND hWnd = reinterpret_cast<HWND>(hwnd);
    RemoveWindowSubclass(hWnd, rawInputSubClassProc, SUBCLS_ID);
}

int engStart() {
	_DMSG("exit");
	return 0;
}

int engEnd() {
	char *buffer = reinterpret_cast<char *>(::MapViewOfFile(g_hSharedMem, FILE_MAP_ALL_ACCESS, 0, 0, _INVISOX_SHARED_MEM_SIZE));
	HANDLE hReadEvent = ::OpenEventA(EVENT_ALL_ACCESS, FALSE, _INVISOX_SHARED_EVENT_NAME);
	if (buffer && g_hReadEvent) {
		buffer[_INVISOX_SHARED_MEM_SIZE - 1] = static_cast<char>(_INVISOX_EXIT_CODE_HOOING);
		_DMSG("trigger the write event for ready to exist");
		::SetEvent(hReadEvent);
	}
	::CloseHandle(hReadEvent);
	_DMSG("exit");
	return 0;
}

int recStart() {
    bRecording = true;
	_DMSG("exit");
	return 0;
}

int recEnd() {
    bRecording = false;
	char *buffer = reinterpret_cast<char *>(::MapViewOfFile(g_hSharedMem, FILE_MAP_ALL_ACCESS, 0, 0, _INVISOX_SHARED_MEM_SIZE));
	HANDLE hReadEvent = ::OpenEventA(EVENT_ALL_ACCESS, FALSE, _INVISOX_SHARED_EVENT_NAME);
	if (buffer && g_hReadEvent) {
		buffer[_INVISOX_SHARED_MEM_SIZE - 1] = static_cast<char>(_INVISOX_EXIT_CODE_HOOING);
		_DMSG("trigger the write event for ready to exist");
		::SetEvent(hReadEvent);
	}
	::CloseHandle(hReadEvent);
	_DMSG("exit");
	return 0;
}

int engReadSharedMemory(char *buffer, int nLeng) {
	if (nullptr == buffer || 0 >= nLeng)
		return -1;

	HANDLE hReadEvent = ::OpenEventA(EVENT_ALL_ACCESS, FALSE, _INVISOX_SHARED_EVENT_NAME);
	if (hReadEvent) {
		::ResetEvent(hReadEvent);
		_DMSG("get event and wait for event signal");
		::WaitForSingleObject(hReadEvent, INFINITE);
		_DMSG("wait for read event");
		char *shareMem = reinterpret_cast<char *>(::MapViewOfFile(g_hSharedMem, FILE_MAP_ALL_ACCESS, 0, 0, _INVISOX_SHARED_MEM_SIZE));
		_DMSG("start copy share memory");
		memcpy(buffer, shareMem, (nLeng > _INVISOX_SHARED_MEM_SIZE ? _INVISOX_SHARED_MEM_SIZE : nLeng));
		memset(shareMem, 0, _INVISOX_SHARED_MEM_SIZE);
		::CloseHandle(hReadEvent);
	}
	return 0;
}
int engWriteSharedMemory(char *buffer, int nLeng, int offset) {
	if (nullptr == buffer || 0 >= nLeng)
		return -1;

	HANDLE hReadEvent = ::OpenEventA(EVENT_ALL_ACCESS, FALSE, _INVISOX_SHARED_EVENT_NAME);
	if (hReadEvent) {
		char *shareMem = reinterpret_cast<char *>(::MapViewOfFile(g_hSharedMem, FILE_MAP_ALL_ACCESS, 0, 0, _INVISOX_SHARED_MEM_SIZE));
        memcpy(shareMem + offset, buffer, (nLeng > _INVISOX_SHARED_MEM_SIZE ? _INVISOX_SHARED_MEM_SIZE : nLeng));
		::SetEvent(hReadEvent);
		::CloseHandle(hReadEvent);
	}
	return 0;
}

int engClearSharedMemory() {
	char *shareMem = reinterpret_cast<char *>(::MapViewOfFile(g_hSharedMem, FILE_MAP_ALL_ACCESS, 0, 0, _INVISOX_SHARED_MEM_SIZE));
	memset(shareMem, 0, _INVISOX_SHARED_MEM_SIZE);
}

CEngine::CEngine()
{
}
