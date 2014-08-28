#ifndef CENVSTORE_H
#define CENVSTORE_H

#include <QObject>
#include <QtXml/QtXml>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QVariant>
#include <QFile>

class CEnvStore : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString xmlName READ slotGetFile() WRITE slotSetFile)


public:
    CEnvStore(QObject *parent = 0);
    virtual ~CEnvStore();

protected:
    QString m_szXmlFile;
    QFile m_file;
    QXmlStreamReader m_xmlReader;
    QXmlStreamWriter m_xmlWriter;

protected:
    virtual bool parser();
    virtual bool closeParserWrite();


signals:
    void sigInit();

public slots:
    void slotSetFile(QString szName);
    QString slotActionToUi(QString szOri);
    QString slotActionToXml(QString szOri);
    QString slotGetFile();
    virtual bool slotStore();

};

#endif // CENVSTORE_H