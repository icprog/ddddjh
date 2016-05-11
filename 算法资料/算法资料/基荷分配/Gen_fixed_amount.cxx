/*--------------------------------------------------------------
**   FILE NAME: qe.cxx
**--------------------------------------------------------------
**   PURPOSE:
**      This rountine is a independant programme that dispatch the
**      load between the units that has already determined its
**      running status. All the information of the units and data 
**      must be obtained from the database and the results must 
**      write to the corresponding database.
**--------------------------------------------------------------
**   Author:
**      Zhang Zhigang     23/11/99
**
**--------------------------------------------------------------*/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <algorithm>
#include <iostream.h>
#include <fstream.h>
#include <math.h>
#include <stdlib.h>
#include "var_def.h"
#include "load_dispatch_db.h"
#include "global_var.h"

void deletenew()
{
	delete[] gen_fix_hydron_amount;
	delete[] gen_fix_thermal_amount;
	delete[] load_forecast;
	delete[] interconnect_line_load;
}

void main(int argc, char** argv)
{
	correct_result = 0;
        write_database_flag = 1;
	get_date_and_time();
	set_opp_rela();
	sprintf(program_id,"jhfhfp");
	sprintf(program_name,"基荷负荷分配");
	//clear_execute_info();
//******************this part read in the date and time block**************
	if(argc != 2 && argc != 3 && argc != 4)
		{
		printf("please input the correct parameter.");
		sprintf(result_info,"错误");
		sprintf(result_info_explain,"请输入正确的日期参数mm/dd/yyyy");
		write_execute_info();
		return;
		}
	else
	{
	char *p;
	p = *(argv+1);
	strcpy(present_date, p);
	if(p[0] == '0')
        {
        p += 1;
        strncpy(month,p,1);
        p += 2;
        month[1] = '\0';
        }
        else
        {
        strncpy(month,p,2);
        p += 3;
        month[2] = '\0';
        }
	if(p[0] == '0')
        {
        p += 1;
        strncpy(day,p,1);
        p += 2;
        day[1] = '\0';
        }
        else
        {
        strncpy(day,p,2);
        p += 3;
        day[2] = '\0';
        }
	strncpy(year,p,4);
	year[4] = '\0';
	strcpy(user_id,"");
	strcpy(user_name,"");
	if(argc == 3)
                {
                if(strcmp(*(argv+2),"0") != 0)
                        {
                        printf("please input the correct parameter.");
                        return;
                        }
                write_database_flag = atoi(*(argv+2));
                }

	if(argc == 4)
	{
	strcpy(user_id,*(argv+2));
	strcpy(user_name,*(argv+3));
	}
	}

//********date is in the form of 11/24/1999. if readin_sd <= 0 then calculate all the time blocks,or calculate the specefied time block***************************************************


	if(write_database_flag !=0)
	clear_execute_info();
	total_sd = get_time_num(year);
	get_hydron_plant();
	
	load_forecast = new float[total_sd];
	get_load_forecast();
	interconnect_line_load = new float[total_sd];
	get_interconnect_line_load();
	float wsxs = get_net_loss_factor();
	for(int i = 0; i < total_sd; i++)
		load_forecast[i] = load_forecast[i] / (1 - wsxs / 100) + interconnect_line_load[i];

/*
	num_of_thermal_unit = get_num_of_thermal_unit(year);
	gen_fix_thermal_amount = new fix_amount_struct[num_of_thermal_unit];
	get_gen_fix_thermal_amount(year);//here to discuss

	for(int i = 0; i < num_of_thermal_unit; i++)
	{
		tamount_thermal[i] = gen_fix_thermal_amount[i].pcount;
		num_of_block[i] = gen_fix_thermal_amount[i].num_of_sd;
		strcpy(un_id_str_thermal[i],gen_fix_thermal_amount[i].un_id);
		for(int j = 0; j < gen_fix_thermal_amount[i].num_of_sd; j++)
		{
		uplimit_thermal[i][j] = gen_fix_thermal_amount[i].uplimit[j];
		downlimit_thermal[i][j] = gen_fix_thermal_amount[i].downlimit[j];
		starttime_thermal[i][j] = gen_fix_thermal_amount[i].starttime[j];
		endtime_thermal[i][j] = gen_fix_thermal_amount[i].endtime[j];
		}
	}


	gen_fixed_amount(total_sd,num_of_thermal_unit,load_forecast,tamount_thermal,num_of_block,uplimit_thermal,downlimit_thermal,starttime_thermal,endtime_thermal,"thermal",un_id_str_thermal,mw_un_thermal);
*/

	num_of_hydron_unit = get_num_of_hydron_unit(year);
	gen_fix_hydron_amount = new fix_amount_struct[num_of_hydron_unit];
	get_gen_fix_hydron_amount(year);

	for(int i = 0; i < num_of_hydron_unit; i++)
	{
		tamount_hydron[i] = gen_fix_hydron_amount[i].pcount;
		num_of_block[i] = gen_fix_hydron_amount[i].num_of_sd;
		strcpy(un_id_str_hydron[i],gen_fix_hydron_amount[i].un_id);
		for(int j = 0; j < gen_fix_hydron_amount[i].num_of_sd; j++)
		{
		uplimit_hydron[i][j] = gen_fix_hydron_amount[i].uplimit[j];
		downlimit_hydron[i][j] = gen_fix_hydron_amount[i].downlimit[j];
		starttime_hydron[i][j] = gen_fix_hydron_amount[i].starttime[j];
		endtime_hydron[i][j] = gen_fix_hydron_amount[i].endtime[j];
		}
	}

	gen_fixed_amount(total_sd,num_of_hydron_unit,load_forecast,tamount_hydron,num_of_block,uplimit_hydron,downlimit_hydron,starttime_hydron,endtime_hydron,"hydron",un_id_str_hydron,mw_un_hydron);


	num_of_thermal_unit = get_num_of_thermal_unit(year);
	gen_fix_thermal_amount = new fix_amount_struct[num_of_thermal_unit];
	get_gen_fix_thermal_amount(year);//here to discuss

	for(int i = 0; i < num_of_thermal_unit; i++)
	{
		tamount_thermal[i] = gen_fix_thermal_amount[i].pcount;
		num_of_block[i] = gen_fix_thermal_amount[i].num_of_sd;
		strcpy(un_id_str_thermal[i],gen_fix_thermal_amount[i].un_id);
		for(int j = 0; j < gen_fix_thermal_amount[i].num_of_sd; j++)
		{
		uplimit_thermal[i][j] = gen_fix_thermal_amount[i].uplimit[j];
		downlimit_thermal[i][j] = gen_fix_thermal_amount[i].downlimit[j];
		starttime_thermal[i][j] = gen_fix_thermal_amount[i].starttime[j];
		endtime_thermal[i][j] = gen_fix_thermal_amount[i].endtime[j];
		}
	}


	gen_fixed_amount(total_sd,num_of_thermal_unit,load_forecast,tamount_thermal,num_of_block,uplimit_thermal,downlimit_thermal,starttime_thermal,endtime_thermal,"thermal",un_id_str_thermal,mw_un_thermal);


        num_of_generator_unit = get_num_of_generator(year);
        gen_fix_generator_amount = new fix_amount_struct[num_of_generator_unit];
        get_gen_fix_generator_amount(year);//here to discuss

        for(int i = 0; i < num_of_generator_unit; i++)
        {
                tamount_generator[i] = gen_fix_generator_amount[i].pcount;
                num_of_block[i] = gen_fix_generator_amount[i].num_of_sd;
                strcpy(un_id_str_generator[i],gen_fix_generator_amount[i].un_id);
                for(int j = 0; j < gen_fix_generator_amount[i].num_of_sd; j++)
                {
                uplimit_generator[i][j] = gen_fix_generator_amount[i].uplimit[j];
                downlimit_generator[i][j] = gen_fix_generator_amount[i].downlimit[j];
                starttime_generator[i][j] = gen_fix_generator_amount[i].starttime[j];
                endtime_generator[i][j] = gen_fix_generator_amount[i].endtime[j];
                }
        }


        gen_fixed_amount(total_sd,num_of_generator_unit,load_forecast,tamount_generator,num_of_block,uplimit_generator,downlimit_generator,starttime_generator,endtime_generator,"thermal",un_id_str_generator,mw_un_generator); 
        if(write_database_flag !=0)
	set_all_plant_fixed_amount();

	deletenew();
	if(write_database_flag !=0)
	{
	if(correct_result != 1)
	{
	sprintf(result_info,"正确");
	sprintf(result_info_explain,"");
	write_execute_info();
	correct_result = 1;
	}
	}
	CS_RETCODE       rcode=0;
	rcode = close_connect_with_server();
	if(rcode!=CS_SUCCEED)
        {
                printf("database close failed!!\n");
                exit(-1);
        }
}

