#include "cbaseinterp.h"
#include "debug.h"
#include <QtConcurrent/QtConcurrent>

CBasedRunThread::CBasedRunThread() {
	m_interp = NULL;
#ifdef Q_OS_WIN
	m_hThread = NULL;
#endif
}

CBasedRunThread::CBasedRunThread(CBasedInterpreter *p) {
	m_interp = p;
#ifdef Q_OS_WIN
	m_hThread = NULL;
#endif
}

CBasedRunThread::~CBasedRunThread() {
	stop();
}

void CBasedRunThread::stop() {
#ifdef Q_OS_WIN
	if (m_hThread) {
		DWORD ddExitCode=0;
		::GetExitCodeProcess(m_hThread, &ddExitCode);
		::TerminateThread(m_hThread, ddExitCode);
		m_hThread=NULL;
	}
#endif
}

void CBasedRunThread::pause() {
#ifdef Q_OS_WIN
	if (m_hThread) {
		::SuspendThread(m_hThread);
	}
#endif
}

void CBasedRunThread::resume() {
#ifdef Q_OS_WIN
	if (m_hThread) {
		::ResumeThread(m_hThread);
	}
#endif
}

void CBasedRunThread::run() {
#ifdef Q_OS_WIN
	m_hThread = ::GetCurrentThread();
	_DMSG("thread id: 0x%x", m_hThread);
	if (m_interp) {
		CBasedInterpreter::runThread(m_interp, m_interp->m_szFile);
	}
#else
	m_hThread=NULL;
#endif
}

////////////////////////////////////////////////
/// \brief CBasedInterpreter::CBasedInterpreter
///

CBasedInterpreter::CBasedInterpreter() : QObject(NULL), m_status(_STATUS_NORMAL), m_thread(this) {

}

CBasedInterpreter::~CBasedInterpreter() {
	if (m_thread.isRunning()) {
		m_thread.stop();
	}
}

int CBasedInterpreter::runThread(CBasedInterpreter *interp, QString szFile) {
	if (NULL == interp || szFile.isEmpty()) {
		return -1;
	}

	interp->m_status = CBasedInterpreter::_STATUS_RUNNING;
	if (0 != interp->run(szFile)) {
		emit interp->sigThreadError(szFile);
		interp->m_status = CBasedInterpreter::_STATUS_STOP;
	}
	else {
		interp->m_status = CBasedInterpreter::_STATUS_STOP;
	}
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

	m_szFile=szFile;
	// is the file already running?
	if (!m_thread.isRunning()) {
		m_thread.start();
	}
	return 0;
}

void CBasedInterpreter::slotPause() {
	if (m_thread.isRunning()) {
		m_thread.pause();
	}
}

void CBasedInterpreter::slotResume() {
	m_thread.resume();
}

void CBasedInterpreter::setAction(QString action) {
	m_szAction = action;
}

QString CBasedInterpreter::getAction() {
	return m_szAction;
}

void CBasedInterpreter::Stop() {
	if (m_thread.isRunning()) {
		m_thread.stop();
	}
}
