TEMPLATE = subdirs

SUBDIRS += ui
SUBDIRS += interp/luainterp

ui.depends = interp/luainterp
