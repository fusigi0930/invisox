#include "cluainterpreter.h"

#include "debug.h"
#include "invisox_common.h"
#include <iostream>
#include <sstream>

#ifdef Q_OS_WIN
#include <windows.h>

static std::streambuf *g_oriStdout=NULL;
static std::streambuf *g_oriStderr=NULL;
static std::stringstream g_coutBuf;
static std::stringstream g_cerrBuf;


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

			break;
	}
	return true;
}

#endif


CLuaInterpreter::CLuaInterpreter() : CBasedInterpreter(), mLua()
{
}

CLuaInterpreter::~CLuaInterpreter() {
	mLua.close();
}

int CLuaInterpreter::run(QString szFile) {
	if (szFile.isEmpty()) {
		return -3;
	}

	mLua.runFile(szFile);
	_DMSG("%s", QSZ(mLua.getResult()));

	return 0;
}

QString CLuaInterpreter::getStdout() {

	return "";
}
