----------------------------------------
-- function: set_event_mode(mode)
-- comment: set the send_event function use event function or windows message
-- mode: CONST_MODE_EVENT_FUNC is using event function
--       CONST_MODE_EVENT_WM is sending windows message to simulate input event
----------------------------------------
set_event_mode(CONST_MODE_EVENT_FUNC)

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
send_event(CONST_EVENT_MOUSE, 100, 500, CONST_EVENT_ACTION_MOVE, 100)
