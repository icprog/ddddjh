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

void get_prior_hydron_plant()
{

        char sql_str[SQL_STRING_LENGTH];
        char buf[10*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)];

	sprintf(sql_str,"xtgl@^$$^@select DCID,DCMC,HSL from xtgl..T_SDCCS where SFTJ=1 and TJWZ=1");
//	printf("sql=%s\n",sql_str);

	sql_operate(sql_str,buf);
	for(int i=0;i<num_of_prior_hydron_plant;i++)
		{
			strcpy(prior_hydron_plant[i].un_id,&buf[i*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)]);
			strcpy(prior_hydron_plant[i].un_name,&buf[i*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)+1*(MAX_COLUMN_SIZE+1)]);
			prior_hydron_plant[i].consume_water = atof(&buf[i*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)+2*(MAX_COLUMN_SIZE+1)]);
		}
}


void free_prior_hydron_plant()
{
	delete[] prior_hydron_plant;
}

void get_lower_hydron_plant(char *un_id)
{

        char sql_str[SQL_STRING_LENGTH];
        char buf[20*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)];


	sprintf(sql_str,"xtgl@^$$^@select DCID,DCMC,TJWZ,HSL from xtgl..T_SDCCS where SFTJ=1 and TJWZ<>1 and SFSSYLSXZ=1 and SSLY in (select SSLY from xtgl..T_SDCCS where DCID='%s') order by TJWZ",un_id);
//	printf("sql=%s\n",sql_str);

	sql_operate(sql_str,buf);
	//printf("res is %s\n",buf);
	for(int i=0;i<num_of_lower_hydron_plant;i++)
		{
			strcpy(lower_hydron_plant[i].un_id,&buf[i*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)]);
			strcpy(lower_hydron_plant[i].un_name,&buf[i*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)+1*(MAX_COLUMN_SIZE+1)]);
			lower_hydron_plant[i].chain_position = atoi(&buf[i*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)+2*(MAX_COLUMN_SIZE+1)]);
			lower_hydron_plant[i].consume_water = atof(&buf[i*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)+3*(MAX_COLUMN_SIZE+1)]);
		}
}

void get_gen_fix_thermal_amount(char *year)
{

        char sql_str[SQL_STRING_LENGTH];
        char buf[100*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)];
        char buf1[10*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)];
        char buf2[1*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)];

	sprintf(sql_str,"jhjygl@^$$^@select jygl%s..T_DCRDDL.DCID,jygl%s..T_DCRDDL.DCMC,jygl%s..T_DCRDDL.DLSXID,jygl%s..T_DCRDDL.DLSX,RFDL from jygl%s..T_DCRDDL where jygl%s..T_DCRDDL.RQ='%s' and (jygl%s..T_DCRDDL.DCSX = NULL or jygl%s..T_DCRDDL.DCSX != 'hydron') order by DCID,DLSXID",year,year,year,year,year,year,present_date,year,year);

	//printf("sql=%s\n",sql_str);

	sql_operate(sql_str,buf);
	//printf(" ret is %s\n",buf);
	for(int i=0;i<num_of_thermal_unit;i++)
		{
			strcpy(gen_fix_thermal_amount[i].un_id,&buf[i*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)]);
			strcpy(gen_fix_thermal_amount[i].un_name,&buf[i*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)+1*(MAX_COLUMN_SIZE+1)]);
			strcpy(gen_fix_thermal_amount[i].pcount_attr_id,&buf[i*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)+2*(MAX_COLUMN_SIZE+1)]);
			strcpy(gen_fix_thermal_amount[i].pcount_attr,&buf[i*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)+3*(MAX_COLUMN_SIZE+1)]);
			gen_fix_thermal_amount[i].pcount = atoi(&buf[i*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)+4*(MAX_COLUMN_SIZE+1)]);
			
			sprintf(sql_str,"jhjygl@^$$^@select count(*) from jygl%s..T_DCRDDLCLFW where DCID='%s' and DLSXID='%s' and RQ='%s'",year,gen_fix_thermal_amount[i].un_id,gen_fix_thermal_amount[i].pcount_attr_id,present_date);
			//printf("sql=%s\n",sql_str);
			sql_operate(sql_str,buf2);
			//printf("res is %s\n",buf1);

			gen_fix_thermal_amount[i].num_of_sd = atoi(&buf2[0]);

			sprintf(sql_str,"jhjygl@^$$^@select QSSD,ZZSD,ZDCL,ZXCL from jygl%s..T_DCRDDLCLFW where DCID='%s' and DLSXID='%s' and RQ='%s'",year,gen_fix_thermal_amount[i].un_id,gen_fix_thermal_amount[i].pcount_attr_id,present_date);
			//printf("sql=%s\n",sql_str);
			sql_operate(sql_str,buf1);
			//printf("res is %s\n",buf1);

			for(int j = 0; j < gen_fix_thermal_amount[i].num_of_sd; j++)
			{
			gen_fix_thermal_amount[i].starttime[j] = atoi(&buf1[j*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)+0*(MAX_COLUMN_SIZE+1)]);
			gen_fix_thermal_amount[i].endtime[j] = atoi(&buf1[j*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)+1*(MAX_COLUMN_SIZE+1)]);
			gen_fix_thermal_amount[i].uplimit[j] = atoi(&buf1[j*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)+2*(MAX_COLUMN_SIZE+1)]);
			gen_fix_thermal_amount[i].downlimit[j] = atoi(&buf1[j*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)+3*(MAX_COLUMN_SIZE+1)]);
			}
		}
}

void get_gen_fix_hydron_amount(char *year)
{
        char sql_str[SQL_STRING_LENGTH];
        char buf[100*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)];
        char buf1[10*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)];
        char buf2[1*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)];

	sprintf(sql_str,"jhjygl@^$$^@select jygl%s..T_DCRDDL.DCID,jygl%s..T_DCRDDL.DCMC,jygl%s..T_DCRDDL.DLSXID,jygl%s..T_DCRDDL.DLSX,RFDL from jygl%s..T_DCRDDL where jygl%s..T_DCRDDL.RQ='%s' and jygl%s..T_DCRDDL.DCSX = 'hydron' order by DCID,DLSXID",year,year,year,year,year,year,present_date,year);

	//printf("sql=%s\n",sql_str);

	sql_operate(sql_str,buf);
	//printf("res is %s\n",buf);
	for(int i=0;i<num_of_hydron_unit;i++)
		{
			strcpy(gen_fix_hydron_amount[i].un_id,&buf[i*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)]);
			strcpy(gen_fix_hydron_amount[i].un_name,&buf[i*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)+1*(MAX_COLUMN_SIZE+1)]);
			strcpy(gen_fix_hydron_amount[i].pcount_attr_id,&buf[i*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)+2*(MAX_COLUMN_SIZE+1)]);
			strcpy(gen_fix_hydron_amount[i].pcount_attr,&buf[i*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)+3*(MAX_COLUMN_SIZE+1)]);
			gen_fix_hydron_amount[i].pcount = atoi(&buf[i*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)+4*(MAX_COLUMN_SIZE+1)]);
			
			sprintf(sql_str,"jhjygl@^$$^@select count(*) from jygl%s..T_DCRDDLCLFW where DCID='%s' and DLSXID='%s' and RQ='%s'",year,gen_fix_hydron_amount[i].un_id,gen_fix_hydron_amount[i].pcount_attr_id,present_date);
			//printf("sql=%s\n",sql_str);
			sql_operate(sql_str,buf2);
			//printf("res is %s\n",buf1);

			gen_fix_hydron_amount[i].num_of_sd = atoi(&buf2[0]);

			sprintf(sql_str,"jhjygl@^$$^@select QSSD,ZZSD,ZDCL,ZXCL from jygl%s..T_DCRDDLCLFW where DCID='%s' and DLSXID='%s' and RQ='%s'",year,gen_fix_hydron_amount[i].un_id,gen_fix_hydron_amount[i].pcount_attr_id,present_date);
			//printf("sql=%s\n",sql_str);
			sql_operate(sql_str,buf1);
			//printf("res is %s\n",buf1);

			for(int j = 0; j < gen_fix_hydron_amount[i].num_of_sd; j++)
			{
			gen_fix_hydron_amount[i].starttime[j] = atoi(&buf1[j*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)+0*(MAX_COLUMN_SIZE+1)]);
			gen_fix_hydron_amount[i].endtime[j] = atoi(&buf1[j*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)+1*(MAX_COLUMN_SIZE+1)]);
			gen_fix_hydron_amount[i].uplimit[j] = atoi(&buf1[j*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)+2*(MAX_COLUMN_SIZE+1)]);
			gen_fix_hydron_amount[i].downlimit[j] = atoi(&buf1[j*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)+3*(MAX_COLUMN_SIZE+1)]);
			}
		}

}

void get_gen_fix_generator_amount(char *year)
{

        char sql_str[SQL_STRING_LENGTH];
        char buf[100*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)];
        char buf1[10*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)];
        char buf2[1*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)];

        sprintf(sql_str,"jhjygl@^$$^@select jygl%s..T_JZRDDL.JZID,jygl%s..T_JZRDDL.JZMC,jygl%s..T_JZRDDL.DLSXID,jygl%s..T_JZRDDL.DLSX,RFDL from jygl%s..T_JZRDDL where jygl%s..T_JZRDDL.RQ='%s'",year,year,year,year,year,year,present_date);


        //printf("sql=%s\n",sql_str);

        sql_operate(sql_str,buf);
        //printf(" ret is %s\n",buf);
        for(int i=0;i<num_of_generator_unit;i++)
                {
                        strcpy(gen_fix_generator_amount[i].un_id,&buf[i*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)]);
                        strcpy(gen_fix_generator_amount[i].un_name,&buf[i*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)+1*(MAX_COLUMN_SIZE+1)]);
                        strcpy(gen_fix_generator_amount[i].pcount_attr_id,&buf[i*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)+2*(MAX_COLUMN_SIZE+1)]);
                        strcpy(gen_fix_generator_amount[i].pcount_attr,&buf[i*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)+3*(MAX_COLUMN_SIZE+1)]);
                        gen_fix_generator_amount[i].pcount = atoi(&buf[i*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)+4*(MAX_COLUMN_SIZE+1)]);

                        sprintf(sql_str,"jhjygl@^$$^@select count(*) from jygl%s..T_JZRDDLCLFW where JZID='%s' and DLSXID='%s' and RQ='%s'",year,gen_fix_generator_amount[i].un_id,gen_fix_generator_amount[i].pcount_attr_id,present_date);
                        //printf("sql=%s\n",sql_str);
                        sql_operate(sql_str,buf2);
                        //printf("res is %s\n",buf1);

                        gen_fix_generator_amount[i].num_of_sd = atoi(&buf2[0]);

                        sprintf(sql_str,"jhjygl@^$$^@select QSSD,ZZSD,ZDCL,ZXCL from jygl%s..T_JZRDDLCLFW where JZID='%s' and DLSXID='%s' and RQ='%s'",year,gen_fix_generator_amount[i].un_id,gen_fix_generator_amount[i].pcount_attr_id,present_date);
                        //printf("sql=%s\n",sql_str);
                        sql_operate(sql_str,buf1);
                        //printf("res is %s\n",buf1);

                        for(int j = 0; j < gen_fix_generator_amount[i].num_of_sd; j++)
                        {
                        gen_fix_generator_amount[i].starttime[j] = atoi(&buf1[j*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)+0*(MAX_COLUMN_SIZE+1)]);
                        gen_fix_generator_amount[i].endtime[j] = atoi(&buf1[j*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)+1*(MAX_COLUMN_SIZE+1)]);
                        gen_fix_generator_amount[i].uplimit[j] = atoi(&buf1[j*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)+2*(MAX_COLUMN_SIZE+1)]);
                        gen_fix_generator_amount[i].downlimit[j] = atoi(&buf1[j*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)+3*(MAX_COLUMN_SIZE+1)]);
                        }
                }
}


