#ifndef __BASED_INTERPRET_ENGINE__
#define __BASED_INTERPRET_ENGINE__

#include <QString>
#include <QObject>
#include <QMap>
#include <QFuture>
#include <QFutureWatcher>
#include <QThread>
#include <vector>
#include "invisox_common.h"

#ifdef Q_OS_WIN
#include <windows.h>
#endif

struct SEvent {
	int type;
	union UNValue {
		struct SKey {
			int keyvalue;
			int multiple;
			int keyAction;
		} key;
		struct SMouse {
			int x;
			int y;
			int mouseEvent;
		} mouse;
	} o;
	unsigned long long timeTick;

	bool operator==(const SEvent &event) {
		if (timeTick == event.timeTick && type == event.type && 0 == memcmp(&o, &event.o, sizeof(o))) {
			return true;
		}
		return false;
	}

	bool operator<=(const SEvent &event) {
		if (type == event.type && 0 == memcmp(&o, &event.o, sizeof(o) - sizeof(int)) && o.key.keyAction != event.o.key.keyAction) {
			return true;
		}
		return false;
	}

	bool operator<(const SEvent &event) {
		if (type == event.type && 0 != memcmp(&o, &event.o, sizeof(o))) {
			return true;
		}
		return false;
	}

	bool operator!=(const SEvent &event) {
		if (type != event.type) {
			return true;
		}
		return false;
	}
};

class CBasedInterpreter;

class CBasedRunThread : public QThread {
	Q_OBJECT

protected:
	CBasedInterpreter *m_interp;

#ifdef Q_OS_WIN
	HANDLE m_hThread;
#endif
public:
	CBasedRunThread();
	CBasedRunThread(CBasedInterpreter *p);
	virtual ~CBasedRunThread();

	void pause();
	void resume();
	void stop();

private slots:
	void slotBasedFinished();

protected:
	virtual void run();
};

class CBasedInterpreter : public QObject {
	Q_OBJECT
	friend class CBasedRunThread;
public:
	CBasedInterpreter();

	virtual ~CBasedInterpreter();

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
	QString m_szFile;
	QString m_szAction;
	CBasedRunThread m_thread;

	static int runThread(CBasedInterpreter *interp, QString szFile);

	virtual int run(QString szFile);

	virtual int finishRun(QString szFile);

signals:
	void sigThreadFinished(QString szFile);
	void sigThreadError(QString szFile);
	
public slots:
	virtual int slotRun(QString szFile);
	virtual void slotPause();
	virtual void slotResume();

public:
	virtual void setAction(QString action);
	virtual QString getAction();
	virtual void Stop();
	virtual QString genScript(std::vector<SEvent> &events);
};

#endif
