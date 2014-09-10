#ifndef __BASED_INTERPRET_ENGINE__
#define __BASED_INTERPRET_ENGINE__

#include "debug.h"
#include <QString>
#include <QObject>
#include <QMap>
#include <QtConcurrent/QtConcurrent>
#include <QFuture>
#include <QFutureWatcher>

class CBasedInterpreter : public QObject {
	Q_OBJECT

public:
	CBasedInterpreter() : QObject(NULL), m_status(_STATUS_NORMAL) {
	
	}

	virtual ~CBasedInterpreter() {

	}

	enum EInterpStatus {
		_STATUS_NORMAL,
		_STATUS_RUNNING,
		_STATUS_TERMINATED,
		_STATUS_STOP,
		_STATUS_UNKNOW
	};

protected:
	EInterpStatus m_status;
	typedef QMap<QString, QFuture<int> > TInterpThreadMap;
	TInterpThreadMap m_mapThread;

	static int runThread(CBasedInterpreter *interp, QString szFile) {
		if (NULL == interp || szFile.isEmpty()) {
			return -1;
		}

		interp->run(szFile);
		return interp->finishRun(szFile);		
	}

	virtual int run(QString szFile) {
		return 0;
	}

	virtual int finishRun(QString szFile) {
		emit sigThreadFinished(szFile);
		// remove the thread from map
		m_mapThread.remove(szFile);
		return 0;
	}

signals:
	void sigThreadFinished(QString szFile);
	void sigThreadError(QString szFile);

	
public slots:
	virtual int slotRun(QString szFile) {
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

};

#endif
