#include "engine.h"
#include "debug.h"

#ifdef Q_OS_WIN
#include <windows.h>

static HINSTANCE g_hInst = nullptr;
static HMODULE g_hLib = nullptr;
static HHOOK g_hHookKey = nullptr;

typedef HHOOK (*fnHooking)(int, HOOKPROC, HINSTANCE, DWORD);
typedef BOOL (*fnUnHooking)(HHOOK);
typedef LRESULT (*fnCallNextHooking)(HHOOK, int, WPARAM, LPARAM);

static fnHooking hooking = nullptr;
static fnUnHooking unHooking = nullptr;
static fnCallNextHooking callNextHooking = nullptr;

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
				callNextHooking = reinterpret_cast<fnCallNextHooking>(GetProcAddress(g_hLib, "CallNextHookEx"));
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
			break;
	}
	return true;
}

static LRESULT CALLBACK monitorKeyEventProc(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nullptr == callNextHooking) {
		return callNextHooking(g_hHookKey, nCode, wParam, lParam);;
	}

	if (lParam & 0x8000000) {
		LPARAM lp = 0;
		if (0 != ::GetAsyncKeyState(VK_LSHIFT) || 0 != ::GetAsyncKeyState(VK_RSHIFT)) {
			lp |= INVISOX_KF_SHFIT;
		}
		if (0 != ::GetAsyncKeyState(VK_LCONTROL) || 0 != ::GetAsyncKeyState(VK_RCONTROL)) {
			lp |= INVISOX_KF_CTRL;
		}
		if (0 != ::GetAsyncKeyState(VK_LMENU) || 0 != ::GetAsyncKeyState(VK_RMENU)) {
			lp |= INVISOX_KF_ALT;
		}

		// send information to invisox ui
	}

	return callNextHooking(g_hHookKey, nCode, wParam, lParam);
}

int engStart() {
	if (hooking != nullptr && g_hInst != nullptr) {
		_DMSG("start hooking");
		g_hHookKey = hooking(WH_KEYBOARD, monitorKeyEventProc, g_hInst, 0);
	}

	if (nullptr == g_hHookKey)
		return -1;
	return 0;
}

int engEnd() {
	if (unHooking != nullptr && g_hHookKey != nullptr) {
		unHooking(g_hHookKey);
		g_hHookKey = nullptr;
	}
}

#endif

CEngine::CEngine()
{
}
