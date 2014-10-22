#include "ccinterpreter.h"

#include "debug.h"
#include <iostream>
#include <sstream>

#include <cling/Interpreter/Interpreter.h>
#include <cling/MetaProcessor/MetaProcessor.h>
#include <cling/UserInterface/UserInterface.h>

#include <clang/Basic/LangOptions.h>
#include <clang/Frontend/CompilerInstance.h>

#include <llvm/Support/Signals.h>
#include <llvm/Support/PrettyStackTrace.h>
#include <llvm/Support/ManagedStatic.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>

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
  HANDLE hinstDLL,
  DWORD fdwReason,
  LPVOID lpvReserved
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

	cling::Interpreter interp(1, g_value);
	clang::CompilerInstance *compiler;
	clang::DiagnosticConsumer *client;

	compiler=interp.getCI();

	if (NULL == compiler) {
		_DMSG("get compiler failed");
		return -3;
	}

	interp.AddIncludePath(".");
	client=compiler->getDiagnostics().getClient();

	if (cling::Interpreter::kSuccess != interp.loadFile(szFile.toLocal8Bit().data())) {
		return -2;
	}

	cling::Interpreter::CompilationResult compResult;
	// for the windows version isssue;
#ifdef Q_OS_WIN
	compResult=interp.execute("printf(\"%d\\n\", 100);");
#endif
	compResult=interp.execute("main(0, 0)");

	return 0;
}

QString CCInterpreter::getStdout() {
	QString szOut=g_coutBuf.str().c_str();
	szOut.append(g_cerrBuf.str().c_str());
	return szOut;
}
