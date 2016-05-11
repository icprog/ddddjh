/*--------------------------------------------------------------
**   FILE NAME: Get_gen_state.cxx
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


int get_num_of_defined_amount_gen()
{
        int load_value;
        char sql_str[SQL_STRING_LENGTH];
        char buf[1*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)];


        sprintf(sql_str,"jhjygl@^$$^@select count(*) from jygl%s..T_DDLJZRFDJH_%s_%s where SD=%d",year,month,day,sd_id);
//      printf("sql=%s\n",sql_str);

        sql_operate(sql_str,buf);

        load_value=atoi(&buf[0]);

        return load_value;
}

void get_defined_amount_gen_array()
{
        char sql_str[SQL_STRING_LENGTH];
        char buf[50*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)];
	//strcpy(buf,"");

	sprintf(sql_str,"jhjygl@^$$^@select JZID,YGGL,ZKDRL,FKDRL,FDL from jygl%s..T_DDLJZRFDJH_%s_%s where SD=%d",year,month,day,sd_id);
//	printf("sql=%s\n",sql_str);

	sql_operate(sql_str,buf);

	
	for(int i=0;i < num_of_defined_amount_gen;i++)
		{
			strcpy(defined_amount_gen_array[i].un_id,&buf[i*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)]);
			defined_amount_gen_array[i].real_power = atoi(&buf[i*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)+1*(MAX_COLUMN_SIZE+1)]);
			defined_amount_gen_array[i].plus_adjust_power = atoi(&buf[i*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)+2*(MAX_COLUMN_SIZE+1)]);
			defined_amount_gen_array[i].minus_adjust_power = atoi(&buf[i*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)+3*(MAX_COLUMN_SIZE+1)]);
			defined_amount_gen_array[i].gen_power = atof(&buf[i*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)+4*(MAX_COLUMN_SIZE+1)]);
		}
}

