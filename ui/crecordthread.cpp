#include "crecordthread.h"

CRecordThread::CRecordThread() : QThread()
{

}

CRecordThread::CRecordThread(CScriptStore *store) :
	QThread()
{
	m_pScriptStore = store;
}

CRecordThread::~CRecordThread() {

}

void CRecordThread::run() {
	if (nullptr == m_pScriptStore) {
		return;
	}

	m_pScriptStore->processRecordingSignal();
}
