TEMPLATE = subdirs

SUBDIRS += \
#	ui
	ui \
	interp/luainterp

ui.depends = interp/luainterp
