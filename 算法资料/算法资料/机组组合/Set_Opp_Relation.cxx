/*--------------------------------------------------------------
**   FILE NAME: Set_Opp_Relation.cxx
**--------------------------------------------------------------
**   PURPOSE:
**      set realtionship of system id and database link, 
**	and the mutex of thread
**--------------------------------------------------------------
**   PARAMETERS:
**      INPUT
**      OUTPUT
**         no returns
**   Author:
**        shao bin 10/22/99
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

db_status_tab st_tab[SYSTEM_NUMBER];


void set_opp_rela()
{

	FILE *fp;
	char read_str[100],*str_h,*str_t;
	int count=0,i;
	char temp_str[5][100];
	if(fp=fopen(getenv("SRVLIST"),"r")) //read config file and set struct db_status_tab
	{
		while(fgets(read_str,95,fp)) 
		{
			if(read_str[0]=='#') continue;
			else 
			{
				str_h=str_t=read_str;	
				for(i=0;i<5;i++)
				{
					while(strncmp(str_t,"$$",2)) str_t++;
					*str_t='\0';
					strcpy(temp_str[i],str_h);
					str_t+=2;
					str_h=str_t;
				}
				strcpy(st_tab[count].sys_id,temp_str[0]);
				strcpy(st_tab[count].serv_nm,temp_str[1]);
				strcpy(st_tab[count].IP_addr,temp_str[2]);
				strcpy(st_tab[count].usr_name,temp_str[3]);
				strcpy(st_tab[count].passwd,temp_str[4]);
				count++;
				//printf("reading string =%s\n",read_str);
			}
		}
		fclose(fp);
	}
	else printf("open file error!\n");

	for(i=0;i<SYSTEM_NUMBER;i++) //create thread mutex for thread to lock
	{
		st_tab[i].cntx_ptr=NULL;
		st_tab[i].conn_ptr=NULL;
		st_tab[i].cmd_ptr=NULL;

		if(pthread_mutex_init(&(st_tab[i].lock_mutex),pthread_mutexattr_default))
		{
                	perror("Init point_mutex error");
                	exit(-1);
        	}

	}
}
