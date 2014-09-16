#include "cbaseinterp.h"
#include "debug.h"
#include <QtConcurrent/QtConcurrent>

CBasedInterpreter::CBasedInterpreter() : QObject(NULL), m_status(_STATUS_NORMAL) {

}

CBasedInterpreter::~CBasedInterpreter() {

}

int CBasedInterpreter::runThread(CBasedInterpreter *interp, QString szFile) {
	if (NULL == interp || szFile.isEmpty()) {
		return -1;
	}

	interp->run(szFile);
	return interp->finishRun(szFile);
}

int CBasedInterpreter::run(QString szFile) {
	return 0;
}

int CBasedInterpreter::finishRun(QString szFile) {
	emit sigThreadFinished(szFile);
	// remove the thread from map
	m_mapThread.remove(szFile);
	return 0;
}
int CBasedInterpreter::slotRun(QString szFile) {
	if (szFile.isEmpty()) {
		return -1;
	}

	// is the file already running?
	TInterpThreadMap::iterator pFind=m_mapThread.find(szFile);
	if (m_mapThread.end() != pFind) {
		return -2;
	}
	QFuture<int> thread=QtConcurrent::run(
			CBasedInterpreter::runThread, this, szFile);

	m_mapThread[szFile]=thread;
	return 0;
}
