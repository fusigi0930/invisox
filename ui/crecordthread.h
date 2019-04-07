#ifndef CRECORDTHREAD_H
#define CRECORDTHREAD_H

#include <QThread>
#include "cscriptstore.h"

class CRecordThread : public QThread
{
private:
	CScriptStore *m_pScriptStore;
public:
	CRecordThread();
	CRecordThread(CScriptStore *store);

	virtual ~CRecordThread();

protected:
	virtual void run();
};

#endif // CRECORDTHREAD_H
