#include "ccinterpreter.h"

#include "debug.h"
#include <iostream>
#include <sstream>

#ifdef Q_OS_WIN
#include <windows.h>

static std::streambuf *g_oriStdout=NULL;
static std::streambuf *g_oriStderr=NULL;
static std::stringstream g_coutBuf;
static std::stringstream g_cerrBuf;

static void initStdOut(bool bInit) {
	if (bInit) {
		g_oriStdout=std::cout.rdbuf();
		g_oriStderr=std::cerr.rdbuf();
		g_coutBuf.str("");
		g_cerrBuf.str("");
		std::cout.rdbuf(g_coutBuf.rdbuf());
		std::cerr.rdbuf(g_cerrBuf.rdbuf());
	}
	else {
		if (g_oriStdout) {
			std::cout.rdbuf(g_oriStdout);
			g_oriStderr=NULL;
		}
		if (g_oriStderr) {
			std::cerr.rdbuf(g_oriStderr);
			g_oriStderr=NULL;
		}
	}
}

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
			initStdOut(true);
			break;
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			_DMSG("dll deattached");
			initStdOut(false);
			break;
	}
	return true;
}

#endif

char *g_value[]={
	"go",
};

CCInterpreter::CCInterpreter()
{
}

CCInterpreter::~CCInterpreter() {

}

int CCInterpreter::run(QString szFile) {
	if (szFile.isEmpty()) {
		return -3;
	}

	return 0;
}

QString CCInterpreter::getStdout() {
	QString szOut=g_coutBuf.str().c_str();
	szOut.append(g_cerrBuf.str().c_str());
	return szOut;
}
