#include "engine.h"
#include "invisox_common.h"
#include <windows.h>
#include <cstdio>

#define _DMSG(e,...) \
{ \
	char string[512] = {0}; \
	sprintf(string, "[%s:%d] " e "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__); \
	OutputDebugStringA(string) ; \
}

__attribute__((section("invisox_shared"), shared)) HINSTANCE g_hInst = nullptr;
__attribute__((section("invisox_shared"), shared)) HMODULE g_hLib = nullptr;
__attribute__((section("invisox_shared"), shared)) HHOOK g_hHookKey = nullptr;
__attribute__((section("invisox_shared"), shared)) HANDLE g_hSharedMem = nullptr;
__attribute__((section("invisox_shared"), shared)) HANDLE g_hReadEvent = nullptr;

typedef HHOOK (*fnHooking)(int, HOOKPROC, HINSTANCE, DWORD);
typedef BOOL (*fnUnHooking)(HHOOK);

static fnHooking hooking = nullptr;
static fnUnHooking unHooking = nullptr;

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
			g_hInst = reinterpret_cast<HINSTANCE>(hinstDLL);
			g_hLib = ::LoadLibraryW(L"user32.dll");
			if (g_hLib) {
				hooking = reinterpret_cast<fnHooking>(GetProcAddress(g_hLib, "SetWindowsHookExW"));
				unHooking = reinterpret_cast<fnUnHooking>(GetProcAddress(g_hLib, "UnhookWindowsHookEx"));
			}
			break;
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			_DMSG("%s dll deattached", __FILE__);
			engEnd();
			if (g_hLib) {
				::FreeLibrary(g_hLib);
				g_hLib = nullptr;
				hooking = nullptr;
				unHooking = nullptr;
			}
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

static LRESULT CALLBACK monitorKeyEventProc(int nCode, WPARAM wParam, LPARAM lParam) {
	//_DMSG("enter hooking process");

	if (wParam == VK_LSHIFT || wParam == VK_RSHIFT ||
		wParam == VK_LCONTROL || wParam == VK_RCONTROL ||
			wParam == VK_LMENU || wParam == VK_RMENU ||
			wParam == VK_CONTROL || wParam == VK_MENU || wParam == VK_SHIFT) {

		//_DMSG("unnecessary hooking keys");
		return ::CallNextHookEx(g_hHookKey, nCode, wParam, lParam);
	}

	if (lParam & 0x80000000) {
		LPARAM lp = 0;
		if (0 != ::GetAsyncKeyState(VK_LSHIFT) || 0 != ::GetAsyncKeyState(VK_RSHIFT)) {
			lp |= _INVISOX_KF_SHIFT;
		}
		if (0 != ::GetAsyncKeyState(VK_LCONTROL) || 0 != ::GetAsyncKeyState(VK_RCONTROL)) {
			lp |= _INVISOX_KF_CTRL;
		}
		if (0 != ::GetAsyncKeyState(VK_LMENU) || 0 != ::GetAsyncKeyState(VK_RMENU)) {
			lp |= _INVISOX_KF_ALT;
		}

		// send information to invisox ui
		HANDLE hSharedMem = ::OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, _INVISOX_SHARED_MEM_NAME);
		char *buffer = reinterpret_cast<char *>(::MapViewOfFile(hSharedMem, FILE_MAP_ALL_ACCESS, 0, 0, _INVISOX_SHARED_MEM_SIZE));
		HANDLE hReadEvent = ::OpenEventA(EVENT_ALL_ACCESS, FALSE, _INVISOX_SHARED_EVENT_NAME);

		if (buffer && g_hReadEvent) {
			unsigned long long keyData[2];
			keyData[0] = wParam;
			keyData[1] = static_cast<unsigned long long>(lp);
			memcpy(buffer, reinterpret_cast<char*>(keyData), sizeof(keyData));

			//_DMSG("trigger the write event");
			::SetEvent(hReadEvent);
		}
		::CloseHandle(hReadEvent);
		::CloseHandle(hSharedMem);
	}

	//_DMSG("exit hooking process");
	return ::CallNextHookEx(g_hHookKey, nCode, wParam, lParam);
}

int engStart() {
	if (hooking != nullptr && g_hInst != nullptr) {
		_DMSG("start hooking");
		g_hHookKey = hooking(WH_KEYBOARD, monitorKeyEventProc, g_hInst, 0);
	}

	if (nullptr == g_hHookKey)
		return -1;

	_DMSG("create share memory");
	g_hSharedMem = ::CreateFileMappingA(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, _INVISOX_SHARED_MEM_SIZE, _INVISOX_SHARED_MEM_NAME);
	_DMSG("create global event");
	g_hReadEvent = ::CreateEventA(nullptr, FALSE, FALSE, _INVISOX_SHARED_EVENT_NAME);

	_DMSG("exit");
	return 0;
}

int engEnd() {
	if (unHooking != nullptr && g_hHookKey != nullptr) {
		unHooking(g_hHookKey);
		g_hHookKey = nullptr;
	}

	char *buffer = reinterpret_cast<char *>(::MapViewOfFile(g_hSharedMem, FILE_MAP_ALL_ACCESS, 0, 0, _INVISOX_SHARED_MEM_SIZE));
	if (buffer && g_hReadEvent) {
		buffer[_INVISOX_SHARED_MEM_SIZE - 1] = static_cast<char>(_INVISOX_EXIT_CODE_HOOING);
		_DMSG("trigger the write event for ready to exist");
		::SetEvent(g_hReadEvent);
	}

	_DMSG("exit");
	return 0;
}

int engReadSharedMemory(char *buffer, int nLeng) {
	if (nullptr == buffer || 0 >= nLeng)
		return -1;

	HANDLE hReadEvent = ::OpenEventA(EVENT_ALL_ACCESS, FALSE, _INVISOX_SHARED_EVENT_NAME);
	if (hReadEvent) {
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
int engWriteSharedMemory(char *buffer, int nLeng) {
	if (nullptr == buffer || 0 >= nLeng)
		return -1;

	HANDLE hReadEvent = ::OpenEventA(EVENT_ALL_ACCESS, FALSE, _INVISOX_SHARED_EVENT_NAME);
	if (hReadEvent) {
		char *shareMem = reinterpret_cast<char *>(::MapViewOfFile(g_hSharedMem, FILE_MAP_ALL_ACCESS, 0, 0, _INVISOX_SHARED_MEM_SIZE));
		memcpy(shareMem, buffer, (nLeng > _INVISOX_SHARED_MEM_SIZE ? _INVISOX_SHARED_MEM_SIZE : nLeng));
		::SetEvent(hReadEvent);
		::CloseHandle(hReadEvent);
	}
	return 0;
}

CEngine::CEngine()
{
}
