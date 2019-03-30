#include "chookthread.h"

CHookThread::CHookThread()
{
	m_pScriptStore = nullptr;
}

CHookThread::CHookThread(CScriptStore *scriptStore) {
	m_pScriptStore = scriptStore;
}

CHookThread::~CHookThread() {

}

void CHookThread::run() {
	if (nullptr == m_pScriptStore) {
		return;
	}
	m_pScriptStore->processHookingSignal();
}
