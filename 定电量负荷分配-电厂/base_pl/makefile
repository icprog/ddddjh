base_plan: base_plan.o log.o dm_operater.o inifile.o common.o 
	gcc -o base_plan base_plan.o log.o dm_operater.o inifile.o common.o -I /opt/dmdbms/bin/include/ -L /opt/dmdbms/bin/ -ldmapi
base_plan.o: base_plan.c log.h dm_operater.h base_plan.h
	gcc -c -g base_plan.c -I /opt/dmdbms/bin/include/
common.o: common.c common.h
	gcc -c -g common.c -o common.o
dm_operater.o: dm_operater.c log.h inifile.h
	gcc -c -g dm_operater.c -I /opt/dmdbms/bin/include/ 
inifile.o:inifile.c inifile.h
	gcc -c -g inifile.c -o inifile.o
log.o: log.c
	gcc -c -g log.c -I /opt/dmdbms/bin/include/ 

clean:
	rm -rf *.o
