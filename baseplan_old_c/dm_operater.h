/***********************************************************
*note: dm database operater
*write by changxing, 2013.5
************************************************************/

#ifndef _DM_OPERATOR_H
#define _DM_OPERATOR_H

#include <stdio.h>  
#include <stdlib.h>  
#include <api.h>
#include "log.h"
#include "inifile.h"

dm_henv henv;   //environment handle
dm_hdbc hdbc;   //connection handle
dm_hstmt hsmt;  // statement handle
dm_bool sret;   //return code
#define MAX_BUFFER 1024
char mess_error[MAX_BUFFER];


typedef struct
{
    char id[50];
    char prog_id[50];
    char prog_name[50];
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
char db_name_str[MAXSIZE];

act_info_stru act_info_to_write;//程序执行信息

#define INI_PATH "./db.ini"
#define		 COLUMN_LEN		100

int PERROR_CLOSEDATABASE_EXIT(int ret,char *str);
int init_database();

int exec_sql(char *sql);  //execute insert procedure

int exec_select(char *sql_string, int column_num, char *desc, void **db_data);

int close_database();

int write_act_info(int sresult,char *note);

#endif
