#ifndef CCINTERPRETER_H
#define CCINTERPRETER_H

#include "cinterp_global.h"
#include "cbaseinterp.h"
#include "debug.h"
#include <QObject>

class CINTERPSHARED_EXPORT CCInterpreter : public CBasedInterpreter
{
	Q_OBJECT
public:
	CCInterpreter();
	virtual ~CCInterpreter();

protected:
	virtual int run(QString szFile);
};

#endif // CCINTERPRETER_H
