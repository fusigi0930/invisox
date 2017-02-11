TEMPLATE = subdirs

SUBDIRS += \
	ui \
	interp/luainterp

ui.depends = interp/luainterp
