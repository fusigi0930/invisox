#ifndef COSSYSTEM_H
#define COSSYSTEM_H

#include <QObject>

class COSSystem : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString dllname READ slotGetDynamicLibrary)

public:
	COSSystem(QObject *parent = 0);
	~COSSystem();

signals:

public slots:
	QString slotGetDynamicLibrary();
};

#endif // COSSYSTEM_H
