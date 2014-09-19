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
