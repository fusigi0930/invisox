#ifndef ENGINE_H
#define ENGINE_H

#include "engine_global.h"

ENGINESHARED_EXPORT int engStart();
ENGINESHARED_EXPORT int engEnd();
ENGINESHARED_EXPORT int engReadSharedMemory(char *buffer, int nLeng);
ENGINESHARED_EXPORT int engWriteSharedMemory(char *buffer, int nLeng);

class ENGINESHARED_EXPORT CEngine
{

public:
	CEngine();
};

#endif // ENGINE_H
