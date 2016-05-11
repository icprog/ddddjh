/*--------------------------------------------------------------
**   FILE NAME: Get_gen_un_struct.cxx
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
#include "var_def.h"

void get_gen_un_struct()
{
	gen_to_unit = new gen_to_unit_struct[total_gen_unit_num];

    char sql_str[SQL_STRING_LENGTH];
    char buf[100*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)];

	sprintf(sql_str,"xtgl@^$$^@select JZID,SZDCID,SSJBDYID,SSJBDYMC,JZMC from xtgl..T_JZBM where SSJBDYID not in ('qt') and SFJJ = 1 order by SSJBDYID");
//	printf("sql=%s\n",sql_str);

	sql_operate(sql_str,buf);
	for(int i=0;i<total_gen_unit_num;i++)
	{
		strcpy(gen_to_unit[i].gen_id,&buf[i*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)+0*(MAX_COLUMN_SIZE+1)]);
		strcpy(gen_to_unit[i].fac_id,&buf[i*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)+1*(MAX_COLUMN_SIZE+1)]);
		strcpy(gen_to_unit[i].unit_id,&buf[i*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)+2*(MAX_COLUMN_SIZE+1)]);
		strcpy(gen_to_unit[i].unit_name,&buf[i*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)+3*(MAX_COLUMN_SIZE+1)]);
		strcpy(gen_to_unit[i].gen_name,&buf[i*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)+4*(MAX_COLUMN_SIZE+1)]);
	}
}


void free_gen_un_struct()
{
	delete[] gen_to_unit;
}

