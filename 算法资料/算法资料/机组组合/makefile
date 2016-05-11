.SUFFIXES: .cxx .o
CC=cxx

SYBASE=/usr/sybase
LIB=$(SYBASE)/OCS-12_0/lib
BLKLIB=$(LIB)/libblk.a
SYBDBLIB=$(LIB)/libsybdb.a
CTLIB=$(LIB)/libct.a
CSLIB=$(LIB)/libcs.a
COMNLIB=$(LIB)/libcomn.a
INTLIB=$(LIB)/libintl.a
NETSCKLIBS=$(LIB)/libtcl.a
NETTLILIBS=$(LIB)/libtcl.a 
NETL1=$(LIB)/libinsck.a

LIBS= $(BLKLIB) $(CTLIB) $(CSLIB) $(COMNLIB) $(INTLIB) $(NETSCKLIBS) $(NETTLILIBS) $(NETL1)  -lm -lpthreads -lmach -lc_r -lrt

FLAGS=-I. -I$(SYBASE)/OCS-12_0/include -I/usr/OCS-12_0/include -I/usr/OCS-12_0/include -g -gall -D_PTHREAD_USE_D4 -c

.cxx.o:
	$(CC) $(FLAGS) $*.cxx

all: MyOrder 



OBJS=MyOrder.o\
	Get_gen_mw_scape.o\
	Get_mannul_state.o\
	Get_Price_struct.o\
	Get_single_attribute.o\
	Inside_Query.o\
	SQL_Operate.o\
	Set_Opp_Relation.o\
	SQL_Operate_For_Price.o\
	Inside_Query_For_Price.o\
	Write_database.o\
	Get_technical_data.o\
	Get_gen_num.o\
	Get_gen_state.o\
	Get_safe_index.o\
	Get_gen_id.o\
	Get_weight.o\
	Get_real_time_state.o\
	Get_mini_mode.o\
	Set_result.o\
	clear_database.o\
	get_date_and_time.o\
	Get_ZKTRL.o\
	Get_loss_factor.o

MyOrder: $(OBJS)
	$(CC) -g -o MyOrder $(OBJS) $(LIBS)

clobber:
	rm -f *.o *~*
	rm -f MyOrder
