//sybase.h by Dingpeng, KD
////////////////////////////////

#ifndef __SYBASE_H__
#define __SYBASE_H__

#include"opf.h"
#include <ctpublic.h>
#include <pthread.h>
/////////////////////////////////////////////////

#define ON_LINE		1
#define OFFLINE		0

#define NUM_FLAT 	400
#define UN_ID_LEN 	22
#define UN_NAME_LEN 	42
#define FAC_ID_LEN 	22

#define SD_LEN 		10	//for 144sd
#define RQ_LEN          12
#define TIME_LEN	10

#define SQL_STRING_LENGTH 500
#define RESULT_STRING_LENGTH 20000

#define MAX_COLNUM 15
#define MAX_COLSIZE 256

#define BUF1_LEN    	MAX_COLNUM*(MAX_COLSIZE+1)
#define BUF10_LEN    	10*BUF1_LEN
#define BUF20_LEN    	20*BUF1_LEN
#define BUF50_LEN    	50*BUF1_LEN
#define BUF60_LEN    	60*BUF1_LEN
#define BUF100_LEN   	100*BUF1_LEN
#define BUF500_LEN	500*BUF1_LEN

#define SERVER_NUMBER 2
#define SYSTEM_NUMBER 9
#define LOAD_FORCAST_LEN 22
#define RETURN_IF(a,b) if(a!=CS_SUCCEED){fprintf(stderr,"error in: %s,line:%d\n",b,__LINE__);return a;}
#define EXIT_IF(a) if(a!=CS_SUCCEED){fprintf(stderr,"FATAL ERROR! line:%d\n",__LINE__);exit(-1);}

#define COL_END_FIX "@^$$^@"
#define COL_END_FIX_LEN 6
#define ROW_END_FIX "\n"
#define RES_END_FIX "@^&&^@"
#define COM_AFFACT_LINE_FIX "@^~~^@"
#define COM_AFFACT_LINE_FIX_LEN 6
#define DB_OPERATE_FAIL "ff@^$$^@"
//////////////////////////////////////////////////////////

char exe_date[12];
char exe_time[12];
char program_id[22];
char program_name[42];
char result_info[10];
char result_info_explain[252];

typedef struct{
        char sys_id[25];   //system id which have relationship with database name, databasename=sysid+year
        char serv_nm[25];  //the server name which database locate on it
        char IP_addr[40];  //the IP address of serv_nm
        char usr_name[20]; //usr name of database
        char passwd[50];   //pass word of usr_name
        int maj_status;    //database status, 0:major database 1:standby database
        CS_CONTEXT *cntx_ptr;   //database link of database
        CS_CONNECTION *conn_ptr;
        CS_COMMAND *cmd_ptr;
        pthread_mutex_t lock_mutex; //thread mutex for thread to lock
} db_status_tab;

db_status_tab st_tab[SYSTEM_NUMBER];

int connect_status;    //socket operation status

typedef struct {
        char uni_id[UN_ID_LEN];
        char un_name[UN_NAME_LEN];
        int out_power;
        double price;
        float fac_used_ratio;
}bid_price_struct;
bid_price_struct *seq_price;

typedef struct {
	char RQ[21];
	char LLXZID[UN_ID_LEN];
	int  YGXZ;
	int  LLXZFSID;
	char TJXZXLID1[UN_ID_LEN];
	char TJXZXLID2[UN_ID_LEN];
	char TJXZXLID3[UN_ID_LEN];
}tie_group_struct;
tie_group_struct * tieGroupPtr;

int seq_price_num;
int *point_num;
/////////////////////////////////////////////////////////////

long lookup(char *sybbuf,char *id,long m,long nfield);
int get_un_num();
int get_seq_price_num(char *year_str,char *mon_str,char *day_str,int time_block);
int modeID(char *mode_buf,char *outage_buf,long mode_num,long outage_num,long mode_size);
int ret_pos(char *sysid);

void get_date_and_time();
void clear_execute_info();
void write_execute_info();

void consoleMsg(char *msg,char *msg_ex);
void consoleMsg(char *msg,char *msg_ex,char *file,long line);

void form_point_num(int *point_num,int un_num);
void form_rq_str(char *rq_str,char *month,char *day,char *year);
void form_time_str(char *time_str,long sd);
void get_price_struct(char *year_str,char *mon_str,char *day_str,int time_block);

void OPF_readSybase(char *year,char *month,char *day,int sd,long prnt);
void OPF_writeSybase(char *year,char *month,char *day,long sd,struct genrstr *genrData,
		struct groupstr *groupData,double *grp_w0,double *grp_w,long groupNum,long prnt);

void reguExplain(char *expln,long ibs,double inc,struct genrstr *genrData,struct groupstr *groupData);

double getRamp(char *gen_id,char *RLSB_tb,long un_num);
double getFall(char *gen_id,char *RLSB_tb,long un_num);

double getLinePf(char *mode_buf,char *ln_id,long imode,long mode_size);
double getLineWt(char *mode_buf,char *ln_id,long imode,long mode_size);

void set_opp_rela();
void sql_operate(char *sql_str,char *buf_str);
void sql_operate_for_price(char *sql_str);

CS_RETCODE close_connect_with_server();
CS_RETCODE inside_query(char *sq_str,char *buf_str);
CS_RETCODE inside_send_sql(CS_COMMAND *cmd_ptr,CS_CHAR *query_str);
CS_RETCODE inside_handle_returns(CS_COMMAND *cmd_ptr,char *buf_str);
CS_RETCODE inside_query_for_price(char *sq_str);
CS_RETCODE inside_send_sql_for_price(CS_COMMAND *cmd_ptr,CS_CHAR *query_str);
CS_RETCODE inside_handle_returns_for_price(CS_COMMAND *cmd_ptr);
//////////////////////////////////////////////////////////////////////////

#endif
