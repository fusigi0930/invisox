TEMPLATE = subdirs

SUBDIRS += \
    ui \
    interp/cinterp

ui.depends = interp/cinterp
