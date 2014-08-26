#include "cenvstore.h"
#include "debug.h"

CEnvStore::CEnvStore(QObject *parent) :
    QObject(parent)
{

}

CEnvStore::~CEnvStore() {
    if (m_file.isOpen()) {
        m_file.close();
    }

}

bool CEnvStore::parser() {
    if (m_szXmlFile.isEmpty())
        return false;

    if (!m_file.isOpen()) {
        m_file.setFileName(m_szXmlFile);
        if (!m_file.open(QFile::ReadWrite)) {
            return false;
        }
    }

    m_xmlReader.setDevice(&m_file);

    return true;
}

void CEnvStore::slotSetFile(QString szName) {
    m_szXmlFile=szName;
    _DMSG("xml name: %s", m_szXmlFile.toUtf8().data());
}

QString CEnvStore::slotGetFile() {
    return m_szXmlFile;
}

bool CEnvStore::slotStore() {
    if (m_szXmlFile.isEmpty())
        return false;

    if (!m_file.isOpen()) {
        m_file.setFileName(m_szXmlFile);
        if (!m_file.open(QFile::ReadWrite)) {
            return false;
        }
    }

    m_xmlWriter.setDevice(&m_file);

    return true;
}

