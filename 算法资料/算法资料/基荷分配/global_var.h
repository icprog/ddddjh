/*--------------------------------------------------------------
**   GlobalValues.h
**--------------------------------------------------------------
**   PURPOSE:
**      This  head file include global values and functions
**--------------------------------------------------------------
**   Author:
**      Shao Bin     8/11/99
**
**--------------------------------------------------------------*/

#ifndef GLOBALVALUES
#define GLOBALVALUES

#include <ctpublic.h>
#include <pthread.h>
#include "var_def.h"

#define SQL_STRING_LENGTH 500
#define RESULT_STRING_LENGTH 20000

#define MAX_COLUMN_NUM 40
#define MAX_COLUMN_SIZE 256

#define SERVER_NUMBER 2

#define SYSTEM_NUMBER 9 

#define LOAD_FORCAST_LEN 22

#define  RETURN_IF(a,b) if(a!=CS_SUCCEED)\
        {fprintf(stderr,"error in: %s,line:%d\n",b,__LINE__);return a;}


#define  EXIT_IF(a) if(a!=CS_SUCCEED)\
          {fprintf(stderr,"FATAL ERROR! line:%d\n",__LINE__);exit(-1);}

#define COL_END_FIX "@^$$^@"
#define COL_END_FIX_LEN 6
#define ROW_END_FIX "\n"
#define RES_END_FIX "@^&&^@"
#define COM_AFFACT_LINE_FIX "@^~~^@"
#define COM_AFFACT_LINE_FIX_LEN 6
#define DB_OPERATE_FAIL "ff@^$$^@"


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

extern db_status_tab st_tab[SYSTEM_NUMBER];

extern int connect_status;    //socket operation status


#endif

