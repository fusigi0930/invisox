#ifndef CINTERP_GLOBAL_H
#define CINTERP_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(LUAINTERP_LIBRARY)
#  define LUAINTERPSHARED_EXPORT Q_DECL_EXPORT
#else
#  define LUAINTERPSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // LUAINTERP_GLOBAL_H
