#ifndef INTERP_COMM_H
#define INTERP_COMM_H

#include "interp_comm_global.h"
#include "debug.h"
#include <QString>

struct SKeyEvent {
	int key;
	int action;
	int miniSec;
};

struct SMouseEvent {
	int x;
	int y;
	int action;
	int miniSec;
};

struct SRpiEvent {
	
};

struct SMatchData {
	int x;
	int y;
};

#define _MODE_INPUT_EVENT_WM	1
#define _MODE_INPUT_EVENT_FUNC	0
#define _MODE_INPUT_EVENT_RASPBERRY 2

#define _TYPE_EVENT_KEY		0
#define _TYPE_EVENT_MOUSE	1
#define _TYPE_EVENT_CUSTOM	2
#define _TYPE_EVENT_RPIHID	3

#define _EVENT_ACTION_DOWN		0
#define _EVENT_ACTION_UP		1
#define _EVENT_ACTION_CLICK		2
#define _EVENT_ACTION_RDOWN		3
#define _EVENT_ACTION_RUP		4
#define _EVENT_ACTION_RCLICK	5
#define _EVENT_ACTION_MOVE		6

extern "C"
{
    INTERP_COMMSHARED_EXPORT int cmCvMatch(QString img, QString patFile, int left, int top, int width, int height, SMatchData *out, int disp_id = 0);
	INTERP_COMMSHARED_EXPORT void* cmCvCapScreen(int disp_id = 0);
	INTERP_COMMSHARED_EXPORT void cmCvFreeMem(void *data);
	INTERP_COMMSHARED_EXPORT void cmCvShow(QString name, void *data);
	INTERP_COMMSHARED_EXPORT void cmCvSave(QString file, void *data);
	INTERP_COMMSHARED_EXPORT void cmCvRect(void *data, QRect &rect);
    INTERP_COMMSHARED_EXPORT int cmSendEvent(int type, int mode, void *event);
    INTERP_COMMSHARED_EXPORT int cmInitCV(int disp_id = 0);
    INTERP_COMMSHARED_EXPORT int cmTest(void *p = nullptr);
}

#endif // CCINTERPRETER_H
