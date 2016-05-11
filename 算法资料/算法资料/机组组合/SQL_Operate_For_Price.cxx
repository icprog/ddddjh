/*--------------------------------------------------------------
**   FILE NAME: SQL_Operate_For_Price.cxx
**--------------------------------------------------------------
**   PURPOSE:
**	create multiple thread to process client query
**--------------------------------------------------------------
**   PARAMETERS:
**      INPUT
**      OUTPUT
**         no returns
**   Author:
**	  shao bin 10/10/99
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
#include "gen_combination_function.h"

void sql_operate_for_price(char *sql_str)
{
	CS_RETCODE       rcode=0;

//	set_opp_rela();
	
	rcode=inside_query_for_price(sql_str);
	if(rcode!=CS_SUCCEED) 
	{
		printf("database link failed!!\n");
                sprintf(result_info,"错误1");
                sprintf(result_info_explain,"数据库连接失败");
                write_execute_info();
		exit(-1);
	}
	//RETURN_IF(rcode,"inside_query");
}
