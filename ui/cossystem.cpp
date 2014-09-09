#include "cossystem.h"

COSSystem::COSSystem(QObject *parent) :
    QObject(parent)
{
}

COSSystem::~COSSystem() {

}

QString COSSystem::slotGetDynamicLibrary() {
#if defined(Q_OS_WIN)
    return "dll files (*.dll)";
#elif defined(Q_OS_LINUX)
    return "so files (*.so)";
#endif
}
