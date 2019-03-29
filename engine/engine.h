#ifndef ENGINE_H
#define ENGINE_H

#include "engine_global.h"

#define INVISOX_KF_SHFIT		0x01
#define INVISOX_KF_ALT			0x02
#define INVISOX_KF_CTRL			0x04


ENGINESHARED_EXPORT int engStart();
ENGINESHARED_EXPORT int engEnd();

class ENGINESHARED_EXPORT CEngine
{

public:
	CEngine();
};

#endif // ENGINE_H
