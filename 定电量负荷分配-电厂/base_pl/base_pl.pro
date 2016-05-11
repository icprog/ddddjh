OTHER_FILES += \
    db.ini

HEADERS += \
    log.h \
    inifile.h \
    dm_operater.h \
    common.h \
    base_plan.h

SOURCES += \
    log.c \
    inifile.c \
    dm_operater.c \
    common.c \
    base_plan.c

INCLUDEPATH += /opt/dmdbms/bin/include/

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../opt/dmdbms/bin/release/ -ldmapi
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../opt/dmdbms/bin/debug/ -ldmapi
else:unix: LIBS += -L$$PWD/../../../../../opt/dmdbms/bin/ -ldmapi

INCLUDEPATH += $$PWD/../../../../../opt/dmdbms/bin
DEPENDPATH += $$PWD/../../../../../opt/dmdbms/bin
