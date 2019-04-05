#ifndef __BASED_INTERPRET_ENGINE__
#define __BASED_INTERPRET_ENGINE__

#include <QString>
#include <QObject>
#include <QMap>
#include <QFuture>
#include <QFutureWatcher>
#include <QThread>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

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
};

#endif
