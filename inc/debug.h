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
	qDebug("%ulld-[%s:%d] " e "\n", (clock()/(CLOCKS_PRE_SEC/1000)), _INVISOX_FUNC, __LINE__, ##__VA_ARGS__)
#else
#   define _DMSG(e,...) \
	qDebug("[%s:%d] " e "\n", _INVISOX_FUNC, __LINE__, ##__VA_ARGS__)
#endif

#ifndef DMSG
#	define DMSG _DMSG
#endif

#endif
