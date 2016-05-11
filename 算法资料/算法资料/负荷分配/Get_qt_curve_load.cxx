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
**      Zhang zhigang     01/13/2000
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


void get_unit_run_state()
{
	char sql_str[SQL_STRING_LENGTH];
        char buf[1*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)];
	char *buf1;

	buf1 = (char *)malloc(5000*3*22);
	sprintf(sql_str,"jhjygl@^$$^@select count(*) from jygl%s..T_YYSJZKTJ_%s_%s",year,month,day);
//	printf("sql=%s\n",sql_str);

	sql_operate(sql_str,buf);
	
	num_of_unit_run_state_array = atoi(&buf[0]);

	sprintf(sql_str,"jhjygl@^$$^@select JZID,SD,KTZT from jygl%s..T_YYSJZKTJ_%s_%s order by JZID,SD",year,month,day);
//	printf("sql=%s\n",sql_str);

	sql_operate_for_run_state(sql_str,buf1);
	
	for(int i = 0; i < num_of_unit_run_state_array; i++)
	{
		strcpy(unit_run_state_array[i].un_id,&buf1[i*3*22+0*22]);
		unit_run_state_array[i].sd_id = atoi(&buf1[i*3*22+1*22]);
		unit_run_state_array[i].run_state = atoi(&buf1[i*3*22+2*22]);
	}
	free(buf1);
	
}

void get_unit_run_state_change_info()
{
	num_of_unit_run_state_change_info = 0;
        for(int i = 0; i < num_of_unit_run_state_array - 1; i++)
	{
		if(strcmp(unit_run_state_array[i].un_id, unit_run_state_array[i+1].un_id) == 0 && ((unit_run_state_array[i].run_state == 0 && unit_run_state_array[i+1].run_state ==1) || (unit_run_state_array[i].run_state == 0 && unit_run_state_array[i+1].run_state == 2) || (unit_run_state_array[i].run_state == 3 && unit_run_state_array[i+1].run_state == 1) || (unit_run_state_array[i].run_state == 3 && unit_run_state_array[i+1].run_state == 2)))
		{
			strcpy(unit_run_state_change_info[num_of_unit_run_state_change_info].un_id,unit_run_state_array[i].un_id);
			unit_run_state_change_info[num_of_unit_run_state_change_info].sd_id = unit_run_state_array[i+1].sd_id;
			unit_run_state_change_info[num_of_unit_run_state_change_info].start_or_shut_flag = 0;
			num_of_unit_run_state_change_info ++;
		}
		else if(strcmp(unit_run_state_array[i].un_id, unit_run_state_array[i+1].un_id) == 0 && ((unit_run_state_array[i].run_state == 1 && unit_run_state_array[i+1].run_state ==0) || (unit_run_state_array[i].run_state == 1 && unit_run_state_array[i+1].run_state == 3) || (unit_run_state_array[i].run_state == 2 && unit_run_state_array[i+1].run_state == 0) || (unit_run_state_array[i].run_state == 2 && unit_run_state_array[i+1].run_state == 3)))
		{
			strcpy(unit_run_state_change_info[num_of_unit_run_state_change_info].un_id,unit_run_state_array[i].un_id);
			unit_run_state_change_info[num_of_unit_run_state_change_info].sd_id = unit_run_state_array[i+1].sd_id;
			unit_run_state_change_info[num_of_unit_run_state_change_info].start_or_shut_flag = 1;
			num_of_unit_run_state_change_info ++;
		}
	}
}

void get_start_shut_curve_load()
{
	char sql_str[SQL_STRING_LENGTH];
    char buf[1*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)];
    char buf1[48*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)];
	int  num_of_points_in_curve;
	int  out_power_array[48];
//****************************************
	int ok_flag;
	char type_flag[22];
	char start_curve_type[10][22];
	int num_of_start_curve_type;
	num_of_start_curve_type = get_num_of_start_curve_type();

	sprintf(sql_str,"xtgl@^$$^@select LXID from xtgl..T_JZKTJQXLX where KTJBSID='kj'");
	//	printf("sql=%s\n",sql_str);
	sql_operate(sql_str,buf1);
	for(int i = 0; i < num_of_start_curve_type; i++)
		strcpy(start_curve_type[i],&buf1[i*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)]);
	
//****************************************

	num_of_start_shut_curve_array = 0;
	start_shut_curve_load_sum = 0;

	get_unit_run_state();
	get_unit_run_state_change_info();

	for(int i = 0; i < num_of_unit_run_state_change_info; i++)
	{
		if(unit_run_state_change_info[i].start_or_shut_flag == 0)
		{
//***************************************************************************
		sprintf(sql_str,"fdbj@^$$^@select KJLXID from fdbj%s..T_JZZTJJJQR where RQ='%s' and JZID='%s'",year,present_date,unit_run_state_change_info[i].un_id);
	//	printf("sql=%s\n",sql_str);

		sql_operate(sql_str,buf);
		strcpy(type_flag,&buf[0]);
		//printf("type_flag=%s\n",type_flag);

		ok_flag = 0;
		for(int n = 0; n < num_of_start_curve_type; n++)
		{
			if(!strcmp(type_flag,start_curve_type[n]))
				ok_flag = 1;
		}
		if(ok_flag == 0)
			strcpy(type_flag,"zc");
//*********************************************************************end

		sprintf(sql_str,"fdbj@^$$^@select count(SD) from fdbj%s..T_JZKJQX where convert(datetime,ZXRQ) in (select max(convert(datetime,ZXRQ)) from fdbj%s..T_JZKJQX where convert(datetime,ZXRQ)<=convert(datetime,'%s') and JZID='%s' and LXID='%s') and JZID='%s' and LXID='%s'",year,year,present_date,unit_run_state_change_info[i].un_id,type_flag,unit_run_state_change_info[i].un_id,type_flag);
	//	printf("sql=%s\n",sql_str);

		sql_operate(sql_str,buf);

		num_of_points_in_curve = atoi(&buf[0]);
		
		if(num_of_points_in_curve == 0)
		{
			strcpy(type_flag,"zc");
			sprintf(sql_str,"fdbj@^$$^@select count(SD) from fdbj%s..T_JZKJQX where convert(datetime,ZXRQ) in (select max(convert(datetime,ZXRQ)) from fdbj%s..T_JZKJQX where convert(datetime,ZXRQ)<=convert(datetime,'%s') and JZID='%s' and LXID='%s') and JZID='%s' and LXID='%s'",year,year,present_date,unit_run_state_change_info[i].un_id,type_flag,unit_run_state_change_info[i].un_id,type_flag);
		//	printf("sql=%s\n",sql_str);

			sql_operate(sql_str,buf);

			num_of_points_in_curve = atoi(&buf[0]);
			
		}

		sprintf(sql_str,"fdbj@^$$^@select SD,CL from fdbj%s..T_JZKJQX where convert(datetime,ZXRQ) in (select max(convert(datetime,ZXRQ)) from fdbj%s..T_JZKJQX where convert(datetime,ZXRQ)<=convert(datetime,'%s') and JZID='%s' and LXID='%s') and JZID='%s' and LXID='%s'",year,year,present_date,unit_run_state_change_info[i].un_id,type_flag,unit_run_state_change_info[i].un_id,type_flag);
	//	printf("sql=%s\n",sql_str);

		sql_operate(sql_str,buf1);
		
		for(int num = 0; num < num_of_points_in_curve; num++)
			out_power_array[num] = atoi(&buf1[num*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)+1*(MAX_COLUMN_SIZE+1)]);
		
//*************the following is to fill the start_shut_curve_array  ***************
		for(int num = 0; num < num_of_points_in_curve; num++)
		{
			strcpy(start_shut_curve_array[num_of_start_shut_curve_array].un_id,unit_run_state_change_info[i].un_id);
			start_shut_curve_array[num_of_start_shut_curve_array].out_power = out_power_array[num];
			start_shut_curve_array[num_of_start_shut_curve_array].sd_id = unit_run_state_change_info[i].sd_id  + num;
			num_of_start_shut_curve_array++;
		}
//*********************************************************************************
		}
		else if(unit_run_state_change_info[i].start_or_shut_flag == 1)
		{
//**************************************************************************tjqx*
			sprintf(sql_str,"fdbj@^$$^@select TJLXID from fdbj%s..T_JZZTJJJQR where RQ='%s' and JZID='%s'",year,present_date,unit_run_state_change_info[i].un_id);
		//	printf("sql=%s\n",sql_str);

			sql_operate(sql_str,buf);
			strcpy(type_flag,&buf[0]);
			//printf("type_flag=%s\n",type_flag);

			ok_flag = 0;
			for(int n = 0; n < num_of_start_curve_type; n++)
			{
				if(!strcmp(type_flag,start_curve_type[n]))
					ok_flag = 1;
			}
			if(ok_flag == 0)
				strcpy(type_flag,"zc");
//*********************************************************************end
			sprintf(sql_str,"fdbj@^$$^@select count(SD) from fdbj%s..T_JZTJQX where convert(datetime,ZXRQ) in (select max(convert(datetime,ZXRQ)) from fdbj%s..T_JZTJQX where convert(datetime,ZXRQ)<=convert(datetime,'%s') and JZID='%s' and LXID='%s') and JZID='%s' and LXID='%s'",year,year,present_date,unit_run_state_change_info[i].un_id,type_flag,unit_run_state_change_info[i].un_id,type_flag);
		//	printf("sql=%s\n",sql_str);

			sql_operate(sql_str,buf);

			num_of_points_in_curve = atoi(&buf[0]);

			if(num_of_points_in_curve == 0)
			{
				strcpy(type_flag,"zc");
				sprintf(sql_str,"fdbj@^$$^@select count(SD) from fdbj%s..T_JZTJQX where convert(datetime,ZXRQ) in (select max(convert(datetime,ZXRQ)) from fdbj%s..T_JZTJQX where convert(datetime,ZXRQ)<=convert(datetime,'%s') and JZID='%s' and LXID='%s') and JZID='%s' and LXID='%s'",year,year,present_date,unit_run_state_change_info[i].un_id,type_flag,unit_run_state_change_info[i].un_id,type_flag);
				//	printf("sql=%s\n",sql_str);

				sql_operate(sql_str,buf);

				num_of_points_in_curve = atoi(&buf[0]);
			}

			sprintf(sql_str,"fdbj@^$$^@select SD,CL from fdbj%s..T_JZTJQX where convert(datetime,ZXRQ) in (select max(convert(datetime,ZXRQ)) from fdbj%s..T_JZTJQX where convert(datetime,ZXRQ)<=convert(datetime,'%s') and JZID='%s' and LXID='%s') and JZID='%s' and LXID='%s'",year,year,present_date,unit_run_state_change_info[i].un_id,type_flag,unit_run_state_change_info[i].un_id,type_flag);
			//	printf("sql=%s\n",sql_str);

			sql_operate(sql_str,buf1);

			for(int num = 0; num < num_of_points_in_curve; num++)
				out_power_array[num] = atoi(&buf1[num*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)+1*(MAX_COLUMN_SIZE+1)]);
		
//*************the following is to fill the start_shut_curve_array  ***************
			for(int num = 0; num < num_of_points_in_curve; num++)
			{
				strcpy(start_shut_curve_array[num_of_start_shut_curve_array].un_id,unit_run_state_change_info[i].un_id);
				start_shut_curve_array[num_of_start_shut_curve_array].out_power = out_power_array[num];
				start_shut_curve_array[num_of_start_shut_curve_array].sd_id = unit_run_state_change_info[i].sd_id - num_of_points_in_curve + num + 1;
				num_of_start_shut_curve_array++;
			}
//*********************************************************************************
		}
	}
	
	for(int j = 0; j < num_of_start_shut_curve_array; j++)
	{
		if(start_shut_curve_array[j].sd_id == sd_id)
			start_shut_curve_load_sum += start_shut_curve_array[j].out_power;
	}

}

