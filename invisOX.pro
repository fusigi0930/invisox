TEMPLATE = subdirs

SUBDIRS += ui \
    engine

SUBDIRS += interp/luainterp \
           interp/cinterp \
		   interp/interp_comm

ui.depends = interp/luainterp
ui.depends += interp/cinterp
ui.depends += interp/interp_comm
ui.depends += engine

luainterp.depends = interp/interp_common
cinterp.depends = interp/interp_common
