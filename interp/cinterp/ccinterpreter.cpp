#include "ccinterpreter.h"

#include <cling/Interpreter/Interpreter.h>
#include <cling/MetaProcessor/MetaProcessor.h>
#include <cling/UserInterface/UserInterface.h>

#include <clang/Basic/LangOptions.h>
#include <clang/Frontend/CompilerInstance.h>

#include <llvm/Support/Signals.h>
#include <llvm/Support/PrettyStackTrace.h>
#include <llvm/Support/ManagedStatic.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>

#include "debug.h"
#include <iostream>
#include <sstream>

#ifdef Q_OS_WIN
#include <windows.h>

static std::streambuf *g_oriStdout=NULL;
static std::stringstream g_coutBuf;

static void initStdOut(bool bInit) {
	if (bInit) {
		g_oriStdout=std::cout.rdbuf();
		g_coutBuf.str("");
		std::cout.rdbuf(g_coutBuf.rdbuf());
	}
	else {
		if (g_oriStdout) {
			std::cout.rdbuf(g_oriStdout);
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

CCInterpreter::CCInterpreter()
{
}

CCInterpreter::~CCInterpreter() {

}

int CCInterpreter::run(QString szFile) {
	if (szFile.isEmpty()) {
		return -3;
	}
	char *value[]={
		"go",
	};
	cling::Interpreter cinterp(1,value);
	clang::CompilerInstance *compiler=cinterp.getCI();

	if (NULL == compiler) {
		return -1;
	}

	cinterp.AddIncludePath(".");
	if (cling::Interpreter::kSuccess != cinterp.loadFile(szFile.toLocal8Bit().data())) {
		return -2;
	}

	clang::DiagnosticConsumer *client=compiler->getDiagnostics().getClient();

	cling::Interpreter::CompilationResult compResult;

	compResult=cinterp.execute("main(0, 0)");

	return 0;
}
