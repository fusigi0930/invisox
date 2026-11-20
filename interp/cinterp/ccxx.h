#ifndef CCXX_H
#define CCXX_H

#include <QString>
#include <map>
#include <vector>
#include <angelscript.h>

#define GLOBAL_CXXOBJ "__CXX_OBJECT__"

class QThread;

class CCxx
{
private:

protected:
    asIScriptEngine *mAS;
    QString m_szPrintOut;
    std::map<QString, QThread*> m_mapCxxThread;
public:
    CCxx();
    virtual ~CCxx();

    void init();
    void close();

    void runString(QString szScript);
    void runFile(QString szFile);
    QString getResult();

    // additional functions
};

#endif // CCXX_H
