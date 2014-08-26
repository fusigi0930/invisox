#include "cscriptstore.h"

CScriptStore::CScriptStore(QObject *parent) :
    CEnvStore(parent)
{

}

CScriptStore::~CScriptStore() {

}

bool CScriptStore::parser() {
    if (!CEnvStore::parser())
        return false;

    return true;
}

bool CScriptStore::slotStore() {
    if (!CEnvStore::slotStore())
        return false;

    return true;
}
