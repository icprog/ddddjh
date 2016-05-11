/*--------------------------------------------------------------
**   FILE NAME: Get_single_attribute.cxx
**--------------------------------------------------------------
**   PURPOSE:
**      This rountine receive sql string from socket class,link to
**       pointed database and query data qualifyed by sql string,
**       write data to a common string.
**--------------------------------------------------------------
**   PARAMETERS:
**      INPUT
**      OUTPUT
**         no returns
**   Author:
**      Zhang zhigang     26/11/99
**
**--------------------------------------------------------------*/

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctpublic.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <time.h>
#include <locale.h>
#include "global_var.h"
#include "load_dispatch_db.h"



int get_time_num(char *year)
{
	int sd_num;
	char sql_str[SQL_STRING_LENGTH];
	char buf[1*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)];

	sprintf(sql_str,"xtgl@^$$^@select CSSZ from xtgl..T_XTKZCS where CSMC='mrsds' and convert(datetime,BZRQ)=(select max(convert(datetime,BZRQ)) from xtgl..T_XTKZCS where CSMC='mrsds')");
	//printf("sql=%s\n",sql_str);

//	strcpy(col_datatype[0],"int");

	sql_operate(sql_str,buf);
	
	sd_num = atoi(&buf[0]);

	return sd_num;

}

int get_num_of_prior_hydron_plant()
{
	int num;
	char sql_str[SQL_STRING_LENGTH];
	char buf[1*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)];

	sprintf(sql_str,"xtgl@^$$^@select count(*) from xtgl..T_SDCCS where SFTJ=1 and TJWZ=1");
	//printf("sql=%s\n",sql_str);


	sql_operate(sql_str,buf);
	
	num = atoi(&buf[0]);

	return num;

}

int get_num_of_lower_hydron_plant(char *un_id)
{
	int num;
	char sql_str[SQL_STRING_LENGTH];
	char buf[1*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)];

	sprintf(sql_str,"xtgl@^$$^@select count(*) from xtgl..T_SDCCS where SFTJ=1 and TJWZ<>1 and SFSSYLSXZ=1 and SSLY in (select SSLY from xtgl..T_SDCCS where DCID='%s')",un_id);
	//printf("sql=%s\n",sql_str);


	sql_operate(sql_str,buf);
	
	num = atoi(&buf[0]);

	return num;

}

int get_pcount_of_prior_hydron_plant(char *un_id)
{
	int value;
	char sql_str[SQL_STRING_LENGTH];
	char buf[1*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)];

	sprintf(sql_str,"jhjygl@^$$^@select RFDL from jygl%s..T_DCRDDL where DCID='%s' and RQ='%s'",year,un_id,present_date);
	//printf("sql=%s\n",sql_str);


	sql_operate(sql_str,buf);
	
	value = atoi(&buf[0]);

	return value;

}

int get_num_of_thermal_unit(char *year_str)
{
	int num;
	char sql_str[SQL_STRING_LENGTH];
	char buf[1*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)];

	//sprintf(sql_str,"jhjygl@^$$^@select count(*) from jygl%s..T_DCRDDL where RQ='%s' and DCSX <> 'hydron'",year,present_date);

        sprintf(sql_str,"jhjygl@^$$^@select count(*) from jygl%s..T_DCRDDL where RQ='%s' and (DCSX = NULL or DCSX != 'hydron')",year,present_date);
        //sprintf(sql_str,"jhjygl@^$$^@select count(*) from jygl%s..T_DCRDDL where RQ='%s' and DCSX <> 'thermal'",year,present_date);
	//printf("sql=%s\n",sql_str);


	sql_operate(sql_str,buf);
	
	num = atoi(&buf[0]);

	return num;

}

int get_num_of_hydron_unit(char *year_str)
{
	int num;
	char sql_str[SQL_STRING_LENGTH];
	char buf[1*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)];

	sprintf(sql_str,"jhjygl@^$$^@select count(*) from jygl%s..T_DCRDDL where RQ='%s' and DCSX = 'hydron'",year,present_date);
	//printf("sql=%s\n",sql_str);


	sql_operate(sql_str,buf);
	
	num = atoi(&buf[0]);

	return num;

}

int get_num_of_generator(char *year_str)

{       int num;
        char sql_str[SQL_STRING_LENGTH];
        char buf[1*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)];

        sprintf(sql_str,"jhjygl@^$$^@select count(*) from jygl%s..T_JZRDDL where RQ='%s'",year,present_date);
        //printf("sql=%s\n",sql_str);

        sql_operate(sql_str,buf);

        num = atoi(&buf[0]);

        return num;

}
  
float get_net_loss_factor()
{
	float loss;
	char sql_str[SQL_STRING_LENGTH];
	char buf[1*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)];

	sprintf(sql_str,"xtgl@^$$^@select CSSZ from xtgl..T_XTKZCS where CSMC='wsxs' and convert(datetime,BZRQ)=(select max(convert(datetime,BZRQ)) from xtgl..T_XTKZCS where CSMC='wsxs')");

	//printf("sql=%s\n",sql_str);


	sql_operate(sql_str,buf);
	
	loss = atof(&buf[0]);

	return loss;

}
