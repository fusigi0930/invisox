#ifndef __INVISOX_DEBUG__
#define __INVISOX_DEBUG__
#include <QDebug>
#include <time.h>

#ifdef _FULL_FUNCTION_NAME
#	define _INVISOX_FUNC __PRETTY_FUNCTION__
#else
#	define _INVISOX_FUNC __FUNCTION__
#endif

#ifdef _INVISOX_DISP_CLOCK
#   define _DMSG(e,...) \
{ \
    char sz[512]; \
    ::sprintf(sz, "%d-[%s:%d] " e, (clock()/(CLOCKS_PRE_SEC/1000)), _INVISOX_FUNC, __LINE__, ##__VA_ARGS__); \
    qDebug() << sz; \
}
#else
#   define _DMSG(e,...) \
{ \
    char sz[512]; \
    ::sprintf(sz, "[%s:%d] " e, _INVISOX_FUNC, __LINE__, ##__VA_ARGS__); \
    qDebug() << sz; \
}
#endif

#ifndef DMSG
#	define DMSG _DMSG
#endif

#define QSZ(e) e.toUtf8().data()

#endif
