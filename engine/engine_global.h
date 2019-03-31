#ifndef ENGINE_GLOBAL_H
#define ENGINE_GLOBAL_H

//#include <QtCore/qglobal.h>

#if defined(ENGINE_LIBRARY)
#  define ENGINESHARED_EXPORT __declspec(dllexport)
#else
#  define ENGINESHARED_EXPORT __declspec(dllimport)
#endif

#endif // ENGINE_GLOBAL_H
