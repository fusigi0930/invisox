----------------------------------------
-- function: set_event_mode(mode)
-- comment: set the send_event function use event function or windows message
-- mode: CONST_MODE_EVENT_FUNC is using event function
--       CONST_MODE_EVENT_WM is sending windows message to simulate input event
----------------------------------------
set_event_mode(CONST_MODE_EVENT_FUNC)

----------------------------------------
-- function: send_event(event, ...)
-- comment: for key event, it should have 3 arguments after event argument
-- event: CONST_EVENT_KEY (in this case), send mouse event
-- arg1: virtual key value
-- arg2: action for key event
--       CONST_EVENT_ACTION_DOWN: key down
--       CONST_EVENT_ACTION_UP: key up
--       CONST_EVENT_ACTION_CLICK: key press
-- arg3: action druation in (mini seconds)
----------------------------------------
send_event(CONST_EVENT_KEY, CONST_VK_0, CONST_EVENT_ACTION_CLICK, 100)
