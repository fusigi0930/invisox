/*
 * <?xml version="1.0" encoding="utf-8"?>
 * <env>
 *		<inc>xxxxxxxxxxx</inc>
 * </env>
 */

#include "cinterpenvstore_win.h"
#include "debug.h"
#include <stdlib.h>
#include <windows.h>

#define XML_FILE		"env.xml"
#define XML_MAIN		"env"
#define XML_INC			"inc"

#define ENV_INC			"INCLUDE"

CInterpEnvStore::CInterpEnvStore()
{
	m_szXmlFile=XML_FILE;
}

CInterpEnvStore::~CInterpEnvStore() {

}

bool CInterpEnvStore::parser() {
	if (!CEnvStore::parser())
		return false;

	while (!m_xmlReader.atEnd() && !m_xmlReader.hasError()) {
		QXmlStreamReader::TokenType readToken=m_xmlReader.readNext();
		switch (readToken) {
			default:
			case QXmlStreamReader::StartDocument:
				continue;
			case QXmlStreamReader::StartElement:
				if (0 == m_xmlReader.name().toString().compare(XML_MAIN)) {
					if (!getInclude())
						continue;
				}
		}
	}
	closeParserWrite();
	return true;
}

bool CInterpEnvStore::slotParser() {
	return parser();
}

bool CInterpEnvStore::getInclude() {
	while (!m_xmlReader.atEnd() && !m_xmlReader.hasError()) {
		QXmlStreamReader::TokenType readToken=m_xmlReader.readNext();
		switch (readToken) {
			default:
			case QXmlStreamReader::StartDocument:
				continue;
			case QXmlStreamReader::StartElement:
				if (0 == m_xmlReader.name().toString().compare(XML_INC)) {
					// set windows environment
					QString szInc;
					szInc.sprintf("%s=%s", ENV_INC, m_xmlReader.readElementText().toUtf8().data());
					putenv(szInc.toUtf8().data());
				}
			case QXmlStreamReader::EndElement:
				if (0 == m_xmlReader.name().toString().compare(XML_INC)) {
					return true;
				}
		}
	}
	return true;
}
