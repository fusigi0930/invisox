#ifndef CCINTERPRETER_H
#define CCINTERPRETER_H

#include "luainterp_global.h"
#include "cbaseinterp.h"
#include "clua.h"
#include "debug.h"
#include <QObject>

class LUAINTERPSHARED_EXPORT CLuaInterpreter : public CBasedInterpreter
{
	Q_OBJECT

private:
	CLua mLua;

public:
	CLuaInterpreter();
	virtual ~CLuaInterpreter();



protected:
	virtual int run(QString szFile);

public:
	QString getStdout();
	virtual QString genScript(std::vector<SEvent> &events);

};

#endif // CCINTERPRETER_H
