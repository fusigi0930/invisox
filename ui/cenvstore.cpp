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

bool CEnvStore::closeParserWrite() {
    m_xmlReader.clear();
    if (m_file.isOpen())
        m_file.close();

    return true;
}

bool CEnvStore::store() {
    if (m_szXmlFile.isEmpty())
        return false;

    closeParserWrite();

    if (!m_file.isOpen()) {
        m_file.setFileName(m_szXmlFile);
        if (!m_file.open(QFile::ReadWrite | QFile::Truncate)) {
            return false;
        }
    }

    m_xmlWriter.setDevice(&m_file);
    m_xmlWriter.setAutoFormatting(true);
    return true;
}

void CEnvStore::slotSetFile(QString szName) {
    m_szXmlFile=szName;
    _DMSG("xml name: %s", m_szXmlFile.toUtf8().data());
}

QString CEnvStore::slotGetFile() {
    return m_szXmlFile;
}


