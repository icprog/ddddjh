OTHER_FILES += \
    ../db.ini

HEADERS += \
    ../base_plan.h \
    ../common.h \
    ../dm_operater.h \
    ../inifile.h \
    ../log.h \
    ../bool.h \
    ../tables.h \
    ../des_encode.h

SOURCES += \
    ../base_plan.c \
    ../common.c \
    ../dm_operater.c \
    ../inifile.c \
    ../log.c \
    ../des_encode.c

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../../opt/dmdbms/bin/release/ -ldmapi
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../../opt/dmdbms/bin/debug/ -ldmapi
else:unix: LIBS += -L$$PWD/../../../../../../opt/dmdbms/bin/ -ldmapi
INCLUDEPATH += /opt/dmdbms/bin/include/
INCLUDEPATH += $$PWD/../../../../../../opt/dmdbms/bin
DEPENDPATH += $$PWD/../../../../../../opt/dmdbms/bin
