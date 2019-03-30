#ifndef CHOOKTHREAD_H
#define CHOOKTHREAD_H

#include <QThread>
#include "cscriptstore.h"

class CHookThread : public QThread
{
private:
	CScriptStore *m_pScriptStore;
public:
	CHookThread();
	CHookThread(CScriptStore *scriptStore);

	virtual ~CHookThread();

protected:
	virtual void run();
};

#endif // CHOOKTHREAD_H
