----------------------------------------
-- function: set_event_mode(mode)
-- comment: set the send_event function use event function or windows message
-- mode: CONST_MODE_EVENT_FUNC is using event function
--       CONST_MODE_EVENT_WM is sending windows message to simulate input event
----------------------------------------
set_event_mode(CONST_MODE_EVENT_FUNC)

----------------------------------------
-- object: opencv()
-- comment: create a opencv lua object, it has many methods to use
----------------------------------------
cv=opencv()
x = 10
y = 10

----------------------------------------
-- method: opencv().match(img, pattern, x, y, width, height)
-- comment: find the match pattern from img
-- img: image file name or input nil to use current screen
-- pattern: be finding pattern (png or jpg file is ok)
-- x: the finding rectangle from img (or screen) whose left top x position
-- y: the finding rectangle from img (or screen) whose left top y position
-- width: the finding rectangle from img (or screen) whose width
-- height: the finding rectangle from img (or screen) whose height
----------------------------------------
x,y=cv.match(nil, "X:/ProjectCode/AppCode/invisox/out/debug/test.png",0,0,800,800)

----------------------------------------
-- function: print(...)
-- comment: print message to invisox ui, now only be shown in the debug output
-- argument: strings or numerics
----------------------------------------
print("x: ",x,", y:",y)

----------------------------------------
-- function: send_event(event, ...)
-- comment: for mouse event, it should have 4 arguments after event argument
-- event: CONST_EVENT_MOUSE (in this case), send mouse event
-- arg1: x position
-- arg2: y position
-- arg3: action for mouse event
--       CONST_EVENT_ACTION_DOWN: mouse left button down
--       CONST_EVENT_ACTION_UP: mouse left button up
--       CONST_EVENT_ACTION_CLICK: mouse left button click
--       CONST_EVENT_ACTION_RDOWN: mouse right button down
--       CONST_EVENT_ACTION_RUP: mouse right button up
--       CONST_EVENT_ACTION_RCLICK: mouse right button click
--       CONST_EVENT_ACTION_MOVE: move mouse cursor to x, y position
-- arg4: action druation in (mini seconds)
----------------------------------------
send_event(CONST_EVENT_MOUSE, x, y, CONST_EVENT_ACTION_MOVE, 100);