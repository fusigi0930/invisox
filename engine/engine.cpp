#include "engine.h"
#include "debug.h"

#ifdef Q_OS_WIN
#include <windows.h>

static HINSTANCE g_hInst = nullptr;
static HMODULE g_hLib = nullptr;
static HHOOK g_hHookKey = nullptr;

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
			if (g_hLib) {
				::FreeLibrary(g_hLib);
				g_hLib = nullptr;
			}
			break;
	}
	return true;
}

static LRESULT CALLBACK monitorKeyEventProc(int nCode, WPARAM wParam, LPARAM lParam) {
	LRESULT ret = 0;
	return ret;
}

int engStart() {
	if (hooking != nullptr && g_hInst != nullptr) {
		_DMSG("start hooking");
		g_hHookKey = hooking(WH_KEYBOARD, monitorKeyEventProc, g_hInst, 0);
	}

	return 0;
}

#endif

CEngine::CEngine()
{
}
