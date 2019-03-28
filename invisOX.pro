TEMPLATE = subdirs

SUBDIRS += ui \
    engine
SUBDIRS += interp/luainterp

ui.depends = interp/luainterp
ui.depends += engine
