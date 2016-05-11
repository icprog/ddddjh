/***********************************************************
*note: dm database operater
*write by changxing, 2013.5
************************************************************/

#ifndef DM_OPERATOR_H
#define DM_OPERATOR_H

#include <stdio.h>  
#include <stdlib.h>  
#include <api.h>
#include "log.h"
#include "inifile.h"

dm_henv henv;   //environment handle
dm_hdbc hdbc;   //connection handle
dm_hstmt hsmt;  // statement handle
dm_bool sret;   //return code
#define  PERROR_CLOSEDATABASE_EXIT(ret,str)\
        {fprintf(stderr,"error in: %s,file:%s,line:%d\n",str,__FILE__,__LINE__);write_act_info(-1,str);close_database();exit(ret);}

#define  PERROR_CLOSEDATABASE(ret,str)\
        {fprintf(stderr,"error in: %s,file:%s,line:%d\n",str,__FILE__,__LINE__);write_act_info(ret,str);}

typedef struct
{
	char id[20];
	char prog_id[20];
	char prog_name[40];
	int  result;
	char result_desc[250];
	char date[30];
	char time[30];
}act_info_stru;
act_info_stru act_info;
long long row_num;

char ipstr[MAXSIZE];
char usrstr[MAXSIZE];
char pwdstr[MAXSIZE];

act_info_stru act_info_to_write;//程序执行信息

#define INI_PATH "./db.ini"
#define		 COLUMN_LEN		100


int init_database();

int exec_sql(char *sql);  //execute insert procedure

int exec_select(char *sql_string, int column_num, char *desc, void **db_data);

int close_database();

int write_act_info(int sresult,char *note);

#endif
