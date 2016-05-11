.SUFFIXES: .cxx .o
CC=cxx

SYBASE=/usr/sybase
LIB=$(SYBASE)/OCS-12_0/lib
BLKLIB=$(LIB)/libblk.a
SYBDBLIB=$(LIB)/libsybdb.a
SRVLIB=$(LIB)/libsrv.a
CTLIB=$(LIB)/libct.a
CSLIB=$(LIB)/libcs.a
COMNLIB=$(LIB)/libcomn.a
INTLIB=$(LIB)/libintl.a
NETSCKLIBS=$(LIB)/libtcl.a
NETTLILIBS=$(LIB)/libtcl.a 
NETL1=$(LIB)/libinsck.a


LIBS= -lm -lpthreads -lmach -lc_r -lrt $(BLKLIB) $(CTLIB) $(CSLIB) $(COMNLIB) $(SYBDBLIB) $(SRVLIB) $(NETSCKLIBS) $(NETTLILIBS) $(NETL1)

FLAGS=-I. -I$(SYBASE)/OCS-12_0/include -I/usr/OCS-12_0/include -I/app/users/osa/OCS-12_0/include -g -gall -D_PTHREAD_USE_D4 -c

.cxx.o:
	$(CC) $(FLAGS) $*.cxx

all: load_dispatch 



OBJS=load_dispatch.o\
	Get_Price_struct.o\
	Get_gen_state.o\
	Get_single_attribute.o\
	Inside_Query.o\
	SQL_Operate.o\
	Set_Opp_Relation.o\
	SQL_Operate_For_Price.o\
	Inside_Query_For_Price.o\
	Get_gen_un_struct.o\
	SQL_Operate_for_run_state.o\
	Inside_Query_for_run_state.o\
	Set_edge_price.o\
	Get_loss_factor.o\
	Get_defined_amount_gen.o\
	Get_up_down_limit.o\
	get_date_and_time.o\
	Set_result.o\
	Get_qt_curve_load.o\
	Set_gen_load.o


load_dispatch: $(OBJS)
	$(CC) -g -o load_dispatch $(OBJS) $(LIBS)

clobber:
	rm -f *.o *~*
	rm -f load_dispatch
