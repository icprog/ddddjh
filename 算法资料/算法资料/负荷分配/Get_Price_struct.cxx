/*--------------------------------------------------------------
**   FILE NAME: Get_Price_Order.cxx
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

void get_price_struct(char *year_str,char *mon_str,char *day_str,int time_block)
{
	seq_price = new bid_price_struct[seq_price_num];

	char sql_str[SQL_STRING_LENGTH];

	//sprintf(sql_str,"fdbj@^$$^@select JZID,JZMC,CL,ZSHDJ from fdbj%s..T_XHSCJJSB_%s_%s group by JZID having SD=%d",year_str,mon_str,day_str,time_block);
	sprintf(sql_str,"fdbj@^$$^@select JZID,JZMC,CL,ZSHDJ,CYL from fdbj%s..T_XHSCJJSB_%s_%s where  SD=%d order by JZID,CL",year_str,mon_str,day_str,time_block);

//	printf("%s\n",sql_str);

	sql_operate_for_price(sql_str);
	
}


void free_price_struct()
{
	delete[] seq_price;
}

