CC = gcc
DM_PATH=/opt/dmdbms
INCLUDEFLAGS=-I $(DM_PATH)/bin/include/
LDFLAGS=-L $(DM_PATH)/bin/ -ldmapi
OBJECTS=base_plan.o common.o dm_operater.o inifile.o log.o des_encode.o
CFLAGS=-g -I $(DM_PATH)/bin/include/


base_plan:$(OBJECTS)
	$(CC) -o $@ $(OBJECTS) $(LDFLAGS)
base_plan.o:base_plan.h log.h dm_operater.h inifile.h common.h
common.o:common.h
dm_operater.o:dm_operater.h log.h inifile.h
des_encode.o:des_encode.h bool.h tables.h inifile.h
inifile.o:inifile.h
log.o:log.h
clean:
	-rm *.o base_plan
