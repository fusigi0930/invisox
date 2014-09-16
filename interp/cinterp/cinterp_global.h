#ifndef CINTERP_GLOBAL_H
#define CINTERP_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(CINTERP_LIBRARY)
#  define CINTERPSHARED_EXPORT Q_DECL_EXPORT
#else
#  define CINTERPSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // CINTERP_GLOBAL_H
