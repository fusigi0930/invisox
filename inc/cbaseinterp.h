#ifndef __BASED_INTERPRET_ENGINE__
#define __BASED_INTERPRET_ENGINE__

#include <QString>
#include <QObject>
#include <QMap>
#include <QFuture>
#include <QFutureWatcher>

class CBasedInterpreter : public QObject {
	Q_OBJECT

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

	static int runThread(CBasedInterpreter *interp, QString szFile);

	virtual int run(QString szFile);

	virtual int finishRun(QString szFile);

signals:
	void sigThreadFinished(QString szFile);
	void sigThreadError(QString szFile);

	
public slots:
	virtual int slotRun(QString szFile);

};

#endif
