/*---------------------------------------------------------------------------------------
**   FILE NAME: MyOrder.cxx
**---------------------------------------------------------------------------------------
**   PURPOSE:
**      This rountine is a independant programme that dispatch the
**      load between the units that has already determined its
**      running status. All the information of the units and data
**      must be obtained from the database and the results must
**      write to the corresponding database.
**----------------------------------------------------------------------------------------
**   Author:
**      Zhang Yang    01/07/2000
**
**--------------------------------------------------------------------------------------*/
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <algorithm>
#include <iostream.h>
#include <fstream.h>
#include <math.h>

#include "price.h"
#include "var_def.h"
#include "gen_combination_function.h"
#include "global_var.h"

/*********************************************************************************************
**
**         this function caculate generators'price for order 
**
**********************************************************************************************/
//该函数计算机组在一个时段的平均报价，采用的方法是计算报价曲线覆盖的面积，除以机组出力范围。
float tran_price(int i)
{
	float temprice = 0.0;
	for(int j = 0 ; j < icc_num[i] - 1 ; j ++)//机组i的报价点j
	{
		temprice = temprice + 0.5 * (UI[i * 10 + j].price + UI[i * 10 + j + 1].price) * (UI[i * 10 + j + 1].mw - UI[i * 10 + j].mw);
	}
	temprice = temprice/(UI[i * 10 + icc_num[i] - 1].mw - UI[i * 10].mw);	
	return temprice;
}

/**********************************************************************************************
**
**		print the scapes of all the unit powers
**
**********************************************************************************************/
void print_mw()
{
        for(int i = 0 ; i < total_gen ; i ++)
        {
                printf("gen is %s, min is %d, max is %d\n",gen_ptr[i].gen_id,gen_ptr[i].mw_min[0],gen_ptr[i].mw_max[0]);
        }
}

/**********************************************************************************************
**
**         this function sets the up and down limits of units'generation
**
**********************************************************************************************/
//将机组的上网上下限（出力范围）从数据库中取出。
void set_gen_mw_scape()
{
	for(int i = 0 ; i < total_gen ; i ++)
	{
		if(gen_ptr[i].sfjj == 1 && gen_ptr[i].sfky ==1)
		{
			for(int sd = 1 ; sd < total_sd + 1 ; sd ++)
			{
				get_gen_mw_scape(year,month,day,sd,i);
			}
		}
	}
}


/**********************************************************************************************
**
**         this function sets real time states of the generators 
**
**********************************************************************************************/
//将机组在第二天前的状态及其发生时间记录下来。
void set_real_time_state()
{
	printf("now I am in set_real_time_state()\n");
	int flag = 0;
        for(int i = 0 ; i < total_gen ; i ++)
        {
		//printf("gen is %s\n",gen_ptr[i].gen_id);
                for(int j = 0 ; j < gen_real_time_state_num ; j++)
                {
                        if(!strcmp(gen_ptr[i].gen_id , real_time_state_ptr[j].gen_id))
			{
				flag = 1;
                        	gen_ptr[i].last_state = real_time_state_ptr[j].present_state;
				gen_ptr[i].sfjj = real_time_state_ptr[j].sfjj;
				gen_ptr[i].sfky = real_time_state_ptr[j].sfky;
                        	strcpy(gen_ptr[i].last_on_off_time , real_time_state_ptr[j].state_occuring_time);
				//printf("I am here\n");
				//printf("gen is %s,last_state is %d,last_on_off_time is %s\n",gen_ptr[i].gen_id,gen_ptr[i].last_state,gen_ptr[i].last_on_off_time );
			}
                }
		if(flag == 0)
		{
			printf("there are no real_time_state records of generator %s",gen_ptr[i].gen_id);
                	sprintf(result_info,"错误");
                        sprintf(result_info_explain,"系统管理库中无机组%s的实时状态记录",gen_ptr[i].gen_id);
                	write_execute_info();
			exit(-1);
		}
		flag = 0;
        }
/*
	
	for(int i = 0 ; i < total_gen ; i ++)
	{
		printf("gen[%d].id is %s ,gen[%d].state is %d,gen[%d].time is %s\n",i,gen_ptr[i].gen_id,i,gen_ptr[i].last_state,i,gen_ptr[i].last_on_off_time);
	}

*/
}

	
/**************************************************************************************************
**
**          this function initiate all the structs 
**          that were used in the program  in one sd
**
**************************************************************************************************/
//初始化与单个时段相关的数据。price_flag在该函数中没有作用。
void initdata(int price_flag, int sd_num)
{
        sd_id = sd_num;
	//printf("sd is %d\n",sd_id);
        un_num = 0;
        basic_load = get_basic_load(year,month,day,sd_num);//基荷
        interconnect_line_load = get_interconnect_line_load(year,month,day,sd_num);//联络线
        float load_forecasting ;
	//load_forecasting = get_load(year,month,day,sd_id);
        //无约束机组开停与有约束机组开停采用的负荷预测值不相同，主要区别是有约束采用的负荷预测的最后一个时段的负荷预测为调度员指定，确保后天的凌晨低谷下备用满足。
 	if(!strcmp(constrain_flag,"yys"))
	load_forecasting = get_load(year,month,day,sd_id);
	else
	load_forecasting = get_wys_load(year,month,day,sd_id);	

	load_forecasting_ptr[sd_id - 1] = load_forecasting;
        float totalload = load_forecasting / (1 - wsxs / 100) + interconnect_line_load - basic_load * (1 - pjcyl);//竞价空间，即机组总上网电量。
	//printf("load_forecasting is %f,basic_load is %d, interconnect_line_load is %d,totalload is %f\n",load_forecasting ,basic_load ,interconnect_line_load,totalload);
	totalload_ptr[sd_id - 1] = totalload;
        //以下几个函数将各个机组的报价数据从数据库中取出。
        seq_price_num = get_bid_price_num(year,month,day,sd_num);//所有机组的该时段内的所有报价点数。
        total_bid_un_num = get_gen_num(year,month,day,sd_num);//报价的机组数。
        total_un_num = get_qt_gen_num(year,month,day,sd_num);//有报价和无报价的机组数。
        get_price_struct(year,month,day,sd_num);

	/******* initiate the struct that all the biding generators *********/

        int temp_bid_gen_num = 0;
        icc_num = new int[total_bid_un_num];//记录各个机组的报价点数。
        UI =new UN_ICC[total_bid_un_num * 10];//记录各个机组报价点的数据。由于每个机组的最大报价点数为10，所以每个机组可分配10个UN_ICC结构单元。

	/*****************************************************************************************
         **      compute how many biding points that every operating generator has
 	 **        and record all the generators' number of biding points 
         ****************************************************************************************/

        int temp_un_num = 0;
        int temp_point_num = 0;
/*
	for(int i = 0 ; i < seq_price_num ; i ++)
	{
		printf("gen_id is %s,mw is %d\n",seq_price[i].uni_id,seq_price[i].out_power);
	}
*/
        for(int n = 0; n  < seq_price_num ; n  ++)
        {
        	if(n == 0||(temp_un_num==0&&temp_point_num==0))// 如果是第一个机组的第一个报价点 
                {
              		strcpy(UI[temp_un_num*10+temp_point_num].un_id,seq_price[n].uni_id);
			strcpy(UI[temp_un_num*10+temp_point_num].un_name,seq_price[n].un_name);
                        UI[temp_un_num*10+temp_point_num].mw = seq_price[n].out_power;
                        UI[temp_un_num*10+temp_point_num].price = seq_price[n].price;
                        temp_point_num++;//该机组的报价点数+1
                }
                else if(!strcmp(seq_price[n].uni_id, seq_price[n-1].uni_id))//如果是同一台机组的报价点.
                {
                        strcpy(UI[temp_un_num*10+temp_point_num].un_id,seq_price[n].uni_id);
                        strcpy(UI[temp_un_num*10+temp_point_num].un_name,seq_price[n].un_name);
                        UI[temp_un_num*10+temp_point_num].mw = seq_price[n].out_power;
                        UI[temp_un_num*10+temp_point_num].price = seq_price[n].price;
                        temp_point_num++;
                }
                else//from now on to record a different generator.
                {
                        icc_num[temp_un_num] = temp_point_num;//由于上一台机组的数据记录完毕，将该机组的报价点数记录。
                        temp_un_num++;//机组数+1
                        temp_point_num = 0;//初始化该机组的报价点数。
                        strcpy(UI[temp_un_num*10+temp_point_num].un_id,seq_price[n].uni_id);
                        strcpy(UI[temp_un_num*10+temp_point_num].un_name,seq_price[n].un_name);
                        UI[temp_un_num*10+temp_point_num].mw = seq_price[n].out_power;
                        UI[temp_un_num*10+temp_point_num].price = seq_price[n].price;
                        temp_point_num++;
                }
        }
        icc_num[temp_un_num] = temp_point_num;//record the last operating generator's biding point number.
        //将所有机组的报价数据转移至主结构gen_ptr[].
	for(int i = 0 ; i < total_bid_un_num ; i ++)
	{
		for(int j = 0 ; j < total_gen ; j ++)
		{
			if(!strcmp(gen_ptr[j].gen_id,UI[i*10].un_id))
			{
				//gen_ptr[j].mw_min[sd_id - 1] = UI[i * 10].mw;
				//gen_ptr[j].mw_max[sd_id - 1] = UI[i * 10 + icc_num[i] - 1].mw; 
				//printf("id is %s min is %d max is %d\n",UI[i*10].un_id,UI[i * 10].mw,UI[i * 10 + icc_num[i] - 1].mw);
				gen_ptr[j].price_for_order[sd_id - 1] = tran_price(i);//计算该时段平均报价。
				//printf("gen_id is %s,price_for_order id %f\n",gen_ptr[j].gen_id,gen_ptr[j].price_for_order[sd_id - 1]);
				gen_ptr[j].gen_state[sd_id - 1] = 0;
				gen_ptr[j].biding_flag[sd_id - 1] = 1;
				strcpy(gen_ptr[j].gen_name , UI[i * 10].un_name);
			}
		}
	}
/*
	for(int i = 0 ; i < total_gen ; i ++)
	{
		for(int j = 0 ; j < gen_tech_num ; j++)
		{
			if(!strcmp(gen_ptr[i].gen_id , gen_tech_ptr[j].gen_id))
			{
				gen_ptr[i].min_on_time = gen_tech_ptr[j].min_on_time;
				gen_ptr[i].min_off_time = gen_tech_ptr[j].min_off_time;
			}
		}
	}
*/	
/*
	for(int i = 0 ; i < total_bid_un_num ; i ++)
	{
		printf("ui[%d] id  is %s num is %d\n",i,UI[i * 10].un_id,icc_num[i]);
	}
*/
/*
	
	for(int i = 0 ; i < total_gen ; i ++)
	{
		printf("gen[%d].id is %s,gen[%d].min_on_time is %d,gen[%d].min_off_time is %d\n",i , gen_ptr[i].gen_id,i , gen_ptr[i].min_on_time,i , gen_ptr[i].min_off_time);
	}
*/
	
	delete[] icc_num;
	delete[] UI;
	free_gen_tech_ptr();	
}

/********************************************************************************************
**
**           allocate memory for the struct that was used in the program
**
*********************************************************************************************/

void allocate_memory()
{
	for(int i = 0 ; i < total_gen ; i ++)
        {
                gen_ptr[i].mw_min = new int[total_sd];
                gen_ptr[i].mw_max = new int[total_sd];
                gen_ptr[i].price_for_order = new float[total_sd];
                gen_ptr[i].gen_state = new int[total_sd];
		gen_ptr[i].biding_flag = new int[total_sd];
                if(gen_ptr[i].mw_min == (int *)0||gen_ptr[i].mw_max == (int *)0||gen_ptr[i].price_for_order == (float *)0||gen_ptr[i].gen_state == (int *)0||gen_ptr[i].biding_flag == (int *)0)
                {
                printf("it is wrong %d\n",i);
                sprintf(result_info,"错误3");
		sprintf(result_info_explain,"程序无法分配所需内存");
                write_execute_info();
                exit(-1);
                }
//for(int j=0;j<48;j++)gen_ptr[i].mw_max[j]=0;
        }
}

/*********************************************************************************************
**
**              free memory that were allocated in the program
**
**********************************************************************************************/
//该函数里的有些结构是在数据库接口函数里进行的内存分配，所以在主函数里找不到内存分配语句。
void free_new_memory()
{
	for(int i = 0 ; i < total_gen ; i ++)
	{
		delete[] gen_ptr[i].mw_min;
		delete[] gen_ptr[i].mw_max;
		delete[] gen_ptr[i].price_for_order;
		delete[] gen_ptr[i].gen_state;
		delete[] gen_ptr[i].biding_flag;
	}
        free_gen_real_time_state_ptr();
        delete[] gen_ptr;
        delete[] totalload_ptr;
	delete[] load_forecasting_ptr;
	delete[] fac_mini_mode_ptr;
}

/*******************************************************************************************************
**
**           this function decides the generators'price order 
**           without time constraint
**
********************************************************************************************************/
//按时段平均报价进行排队，采用的排队算法是冒泡法。
void bubble_order(int sd )
{
	GENERATOR t;//声名临时结构变量。
	
	for(int i = 0 ; i < total_gen ; i ++)
	{
		for(int j = 0 ; j < total_gen - i - 1; j ++)
		{
			if(gen_ptr[j].price_for_order[sd] > gen_ptr[j + 1].price_for_order[sd])
			{
				t = gen_ptr[j];
				gen_ptr[j] = gen_ptr[j + 1];
				gen_ptr[j + 1] = t;      
			}
			if((gen_ptr[j].price_for_order[sd] == gen_ptr[j + 1].price_for_order[sd])&&(gen_ptr[j].weight > gen_ptr[j + 1].weight))//报价相同，权重小的先开机。
			{
				t = gen_ptr[j];
                 	        gen_ptr[j] = gen_ptr[j + 1];
                                gen_ptr[j + 1] = t;
			}
		}	
	}
	
/*
	for(int i = 0 ; i < total_gen ; i ++)
	{
		printf("gen[%d].id is %s,nid_flag is %d\n",i,gen_ptr[i].gen_id,gen_ptr[i].biding_flag[sd]);
	}
*/
}
			
/**************************************************************************************************
**
**           this function decides which generator to operate
**           according to their price without time constraint 
**
***************************************************************************************************/

void wys_order(int sd)
{
	//printf("I am in wys_order\n");
        
	int tempload_up = 0;

	int tempload_down = 0;

	for(int i = 0 ; i < total_gen ; i ++)
	{
		if(gen_ptr[i].biding_flag[sd] == 1 && gen_ptr[i].sfjj == 1 && gen_ptr[i].sfky == 1)//如果该机组在该时段有报价
		{
			tempload_up = tempload_up + gen_ptr[i].mw_max[sd];
			tempload_down = tempload_down + gen_ptr[i].mw_min[sd];

			if(tempload_up <= (totalload_ptr[sd] + load_forecasting_ptr[sd] * WYS_UP_ROTARY_RATE/100) && tempload_down <= (totalload_ptr[sd] - load_forecasting_ptr[sd] * WYS_DOWN_ROTARY_RATE/100))
			{
				gen_ptr[i].gen_state[sd] = 1;
			}
                        if(tempload_up >= (totalload_ptr[sd] + load_forecasting_ptr[sd] * WYS_UP_ROTARY_RATE/100) && tempload_down <= (totalload_ptr[sd] - load_forecasting_ptr[sd] * WYS_DOWN_ROTARY_RATE/100))
			{
				gen_ptr[i].gen_state[sd] = 1;
        			printf("tempload_up is %d,tempload_down is %d\n",tempload_up ,tempload_down);
				return;
			}
                        if(tempload_up >= (totalload_ptr[sd] + load_forecasting_ptr[sd] * WYS_UP_ROTARY_RATE/100) && tempload_down >= (totalload_ptr[sd] - load_forecasting_ptr[sd] * WYS_DOWN_ROTARY_RATE/100))//满足上备用，不满足下备用。
			{
				continue;//继续寻找合适的机组
			}
		}
	}
/*
	int temppower = 0;
	for(int i = 0 ; i < total_gen; i ++)
	{
		if(gen_ptr[i].gen_state[47] == 1)
		{
		printf("I am here\n");
		printf("gen is %s,max is %d\n",gen_ptr[i].gen_id,gen_ptr[i].mw_max[47]);
		temppower = temppower + gen_ptr[i].mw_max[47];
		printf("up is %d\n",temppower);
		}
	}
*/
}

/****************************************************************************************
**
**                    modify generators' weight 
**
*****************************************************************************************/
//当两台机组的平均报价相同时，一台开机而另一台停机，则开机的机组权重+1
void modify_weight(int sd)
{
/////////////////////   modify generators' weight   ////////////////////////////

	for(int i = 0 ; i < total_gen - 1 ; i ++)//same price same capacity , weight will ascend if it is on while others are off;
	{
		if(gen_ptr[i].price_for_order[sd] == gen_ptr[i + 1].price_for_order[sd] && gen_ptr[i].mw_min[sd] == gen_ptr[i + 1].mw_min[sd] && gen_ptr[i].mw_max[sd] == gen_ptr[i + 1].mw_max[sd])
		{
			if(gen_ptr[i].gen_state[sd] == 1 && gen_ptr[i + 1].gen_state[sd] == 0)
			{
				gen_ptr[i].weight += 1;
			}
		}
	}
/*
	
                        if(tempload_up >= (totalload_ptr[sd] + load_forecasting_ptr[sd] * UP_ROTARY_RATE/100) && tempload_down > (totalload_ptr[sd] - load_forecasting_ptr[sd] * DOWN_ROTARY_RATE/100))
	{
		printf("sd%d down rotary reserve can't be satisfied,load is %f,reserve is %d\n",sd,totalload_ptr[sd],tempload_down);
	}
                        if(tempload_up < (totalload_ptr[sd] + load_forecasting_ptr[sd] * UP_ROTARY_RATE/100) && tempload_down <= (totalload_ptr[sd] - load_forecasting_ptr[sd] * DOWN_ROTARY_RATE/100))	 	
	{
		printf("sd%d up_rotary reserve can't be satisfied,load is %f,reserve is %d,up is %f\n",sd,totalload_ptr[sd],tempload_up,totalload_ptr[sd] + load_forecasting_ptr[sd] * UP_ROTARY_RATE/100);
	}

                        if(tempload_up < (totalload_ptr[sd] + load_forecasting_ptr[sd] * UP_ROTARY_RATE/100) && tempload_down > (totalload_ptr[sd] - load_forecasting_ptr[sd] * DOWN_ROTARY_RATE/100))	  
        {
                printf("sd%d up_rotary and down_rotary reserve can't be satisfied\n",sd);
        }
*/
}

/*************************************************************************************************************************
**
**                        this function print generators'states for 48 time_seg
**
**************************************************************************************************************************/
	
void print_state()
{
        for(int i = 0 ;  i < total_gen ; i ++)
        {
                printf("gen is %s\t",gen_ptr[i].gen_id);
                for(int j = 0 ; j < total_sd ; j ++)
                {
                        printf("%d",gen_ptr[i].gen_state[j]);
                }
                printf("\n");
        }
/*
	for(int i = 0 ; i < total_gen ; i ++)
	{
		for(int j = 0 ; j < total_sd ; j ++)
		{
			printf("gen_ptr[%d].id is %s,gen_ptr[%d].gen_state[%d] is %d\n",i,gen_ptr[i].gen_id,i,j,gen_ptr[i].gen_state[j]);
		}
	}
*/
}

				

/*****************************************************************************************************
**
**        this function get preparing data for the progran 
**
******************************************************************************************************/
//准备与时段无关的数据，调用init_data(int,int)函数
void preparing_data() 
{
	int temp_sd = 0;
	int temp_inter_line_load = 0;
	peak_load = 0 ; 
	lowest_load = 0 ; 
/*
	get_appointed_lowest_load_forecasting(year,month,day);
	get_appointed_peak_load_forecasting(year,month,day);
	if(strcmp(lowest_load_date,""))
	{
		get_lowest_load(year);
                char *p;
		p = lowest_load_date;	
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
		temp_sd = get_next_lowest_load_sd(year,month,day);
		temp_inter_line_load = get_appointed_interconnect_line_load(year,month,day,temp_sd);	
	}
	if(strcmp(peak_load_date,""))
	{
		get_peak_load(year);
                char *p1;
                p1 = peak_load_date;
                if(p1[0] == '0')
                {
                        p1 += 1;
                        strncpy(month,p1,1);
                        p1 += 2;
                        month[1] = '\0';
                }
                else
                {
                        strncpy(month,p1,2);
                        p1 += 3;
                        month[2] = '\0';
                }
                if(p1[0] == '0')
                {
                        p1 += 1;
                        strncpy(day,p1,1);
                        p1 += 2;
                        day[1] = '\0';
                }
                else
                {
                        strncpy(day,p1,2);
                        p1 += 3;
                        day[2] = '\0';
                }
                strncpy(year,p1,4);
                year[4] = '\0';
	}
*/
	DOWN_ROTARY_RATE = get_down_rotary_rate(year);
	UP_ROTARY_RATE = get_up_rotary_rate(year);
	WYS_DOWN_ROTARY_RATE = get_wys_down_rotary_rate(year);
	printf("wysrot_rate is %f\n",WYS_DOWN_ROTARY_RATE);
	WYS_UP_ROTARY_RATE = get_wys_up_rotary_rate(year);
	printf("wysrot_rate is %f\n",WYS_UP_ROTARY_RATE);   

	PEAK_LOAD_ROTARY_RATE = get_peak_load_rotary_rate(year);
	if(PEAK_LOAD_ROTARY_RATE == 1000)//如果没有指定尖峰负荷采用备用率，采用UP_ROTARY_RATE 
	PEAK_LOAD_ROTARY_RATE = UP_ROTARY_RATE;

	LOWEST_LOAD_ROTARY_RATE = get_lowest_load_rotary_rate(year);
	printf("LOWEST_LOAD_ROTARY_RATE is %f\n",LOWEST_LOAD_ROTARY_RATE);
	if(LOWEST_LOAD_ROTARY_RATE == 1000)//如果没有指定第45到48时段的备用率，采用DOWN_ROTARY_RATE 
	LOWEST_LOAD_ROTARY_RATE = DOWN_ROTARY_RATE;

	FIRST_SEG_DOWN_ROTARY_RATE = get_hy_down_rotary_rate(year);
	if(FIRST_SEG_DOWN_ROTARY_RATE == 1000)//如果没有指定第1到14时段的备用率，采用LOWEST_LOAD_ROTARY_RATE 
	FIRST_SEG_DOWN_ROTARY_RATE = LOWEST_LOAD_ROTARY_RATE;

        THRESHOLD_ON_GEN = get_threshold_on_gen(year);//开机阈值

        total_sd = get_time_num(year);
        totalload_ptr = new float[total_sd];
        load_forecasting_ptr = new float[total_sd];
        get_gen_num();
        gen_ptr = new GENERATOR[total_gen];
        get_gen_id();
        get_weight();

        allocate_memory();

        get_gen_real_time_state(year,month,day);

	oper_sd = get_wksd(year) - 1;//晚开机组的开机时段
	stop_sd = get_wtsd(year) - 1;//晚停机组的停机时段
	//printf("ROTSARY IS %f,%f\n",DOWN_ROTARY_RATE,UP_ROTARY_RATE);	
        wsxs = get_net_loss_factor();//网损系数
	printf("wsxs is %d\n",wsxs);
	pjcyl = get_pjcyl();//平均厂用率
	printf("pjcyl is %f\n",pjcyl);

	int must_on_time = get_must_on_time();//取公用机组最小开机时间
	int must_off_time = get_must_off_time();//取公用机组最小停机时间
	printf("must_on_time must_off_time %d,%d\n",must_on_time ,must_off_time);
	if(must_on_time == 0 && must_off_time == 0)//如果没有指定公用最小开机和停机时间
	{
		get_gen_tech_data(year);//取各个机组最小开停机时间
	        for(int i = 0 ; i < total_gen ; i ++)
        	{
                	for(int j = 0 ; j < gen_tech_num ; j++)
                	{
                        	if(!strcmp(gen_ptr[i].gen_id , gen_tech_ptr[j].gen_id))
                        	{
                                	gen_ptr[i].min_on_time = gen_tech_ptr[j].min_on_time;
                                	gen_ptr[i].min_off_time = gen_tech_ptr[j].min_off_time;
					//printf("gen_ptr[i].min_on_time  gen_ptr[i].min_off_time%d,%d\n",gen_ptr[i].min_off_time,gen_ptr[i].min_on_time);
                        	}
                	}
        	}
	}
	else if(must_on_time == 0 )//如果没有指定公用最小开机时间
	{
		get_gen_tech_data(year);
		for(int i = 0 ; i < total_gen ; i ++)
                {
                        for(int j = 0 ; j < gen_tech_num ; j++)
                        {
                                if(!strcmp(gen_ptr[i].gen_id , gen_tech_ptr[j].gen_id))
                                {
                                        gen_ptr[i].min_on_time = gen_tech_ptr[j].min_on_time;
                                }
                        }
			gen_ptr[i].min_off_time = must_off_time;
                }
        }
	else if(must_off_time == 0)//如果没有指定公用最小停机时间
	{
                get_gen_tech_data(year);
                for(int i = 0 ; i < total_gen ; i ++)
                {
                        for(int j = 0 ; j < gen_tech_num ; j++)
                        {
                                if(!strcmp(gen_ptr[i].gen_id , gen_tech_ptr[j].gen_id))
                                {
                                        gen_ptr[i].min_off_time = gen_tech_ptr[j].min_off_time;
                                }
                        }
                        gen_ptr[i].min_on_time = must_on_time;
                }
        }
	else
	{
                for(int i = 0 ; i < total_gen ; i ++)
		{
			gen_ptr[i].min_off_time = must_off_time;
			gen_ptr[i].min_on_time = must_on_time;
			//printf("gen_ptr[i].min_on_time  gen_ptr[i].min_off_time%d,%d\n",gen_ptr[i].min_off_time,gen_ptr[i].min_on_time);

		}
	}

        for(int sd = 1 ; sd <= total_sd; sd++)
        {
              initdata(0,sd);
        }
/*
/////// set lowest_load with appointed lowest_load,set peak_load with appointed peak_load  ///////

	float temp_total_load =	100000.0;
        float temp_load_forecasting = 0.0;
	float appointed_lowest_total_load = 0.0;
	int tempory_lowest_sd = 0;
	int tempory_peak_sd = 0;
	if(strcmp(lowest_load_date,""))	
	{
		appointed_lowest_total_load = lowest_load * (1 + wsxs / 100) + temp_inter_line_load - basic_load[47];		
		for(int sd = stop_sd ; sd < total_sd ; sd ++)
		{
			if(totalload_ptr[sd] < temp_total_load)
			{
				temp_total_load = totalload_ptr[sd];
				tempory_lowest_sd = sd;
			}
		}
		if(temp_total_load > appointed_lowest_total_load)
		totalload_ptr[tempory_lowest_sd] = appointed_lowest_total_load;
	}


	if(strcmp(peak_load_date,""))
	{
		for(int sd = 0 ; sd < total_sd ; sd ++)
		{
			if(load_forecasting_ptr[sd] > temp_load_forecasting)
			{
				temp_load_forecasting = load_forecasting_ptr[sd];
				tempory_peak_sd = sd;
			}
		}
		if(peak_load > temp_load_forecasting)
		{
			float totalload = peak_load * (1 + wsxs / 100) + interconnect_line_load[tempory_peak_sd] - basic_load[tempory_peak_sd];
			totalload_ptr[tempory_peak_sd] = totalload;
		}
	}
*/		
	//get_gen_safe_index(year,month,day);
}

/********************************************************************************************
**
**           this function finds the lowest load sd of the whole day
**
********************************************************************************************/
//找到凌晨低谷时的最低竞价空间（负荷）时段，用于计算第45--48时段的备用水平。
int find_lowest_totalload_sd()
{
        int temp_lowest_sd = 0;
        float temp_lowest_load = 1000000.0;
        for(int i = 0 ; i < 20 ; i ++)//maybe some problem;
        {
                if(totalload_ptr[i] < temp_lowest_load)
                {
                        temp_lowest_load = totalload_ptr[i];
                        temp_lowest_sd = i;
                }
        }
        return temp_lowest_sd;
}

/********************************************************************************************
**
**           this function finds the lowest load sd of the whole day
**
********************************************************************************************/
//找到全天负荷预测值最低的时段
int find_lowest_load_sd()
{
        int temp_lowest_sd = 0;
        float temp_lowest_load = 1000000.0;
        for(int i = 0 ; i < total_sd ; i ++)
        {
                if(load_forecasting_ptr[i] < temp_lowest_load)
                {
                        temp_lowest_load = load_forecasting_ptr[i];
                        temp_lowest_sd = i;
                }
        }
        return temp_lowest_sd;
}
                          
/********************************************************************************************
**
**           this function finds the peak load sd of the whole day
**
********************************************************************************************/
//找到全天负荷预测值最高的时段
int find_peak_load_sd()
{
        int temp_peak_sd = 0;
        float temp_peak_load = 0.0;
        for(int i = oper_sd ; i < total_sd ; i ++)
        {
                if(load_forecasting_ptr[i] > temp_peak_load)
                {
                        temp_peak_load = load_forecasting_ptr[i];
                        temp_peak_sd = i;
                }
        }
        return temp_peak_sd;
}


/********************************************************************************************************
**
**           this function compute the number of sd 
**           that the generator must be on or off
** 
********************************************************************************************************/
//计算机组的必开、必停时段
int compute_must_on_off_sd_num(char * lastTime)
{
        //形参char * lastTime 将机组当前状态的发生时间传给函数。
        tm * my_tm,* my_tm2;//声名两个c语言的时间结构变量
	my_tm = new tm;
	my_tm2 = new tm;
        my_tm->tm_isdst = 0;
        my_tm->tm_sec = 0;
        my_tm2->tm_isdst = 0;
        my_tm2->tm_sec = 0;


        //cout<<'x'<< present_date<<endl;
        char* p ;
        char* p1 ;
        char* p2 ;

	p = new char[20];
        //计划制定的机组启停启始时间为第二天00：00，所以strcat(p,"00:00");	
	strcpy(p,present_date);
	strcat(p," 00:00");
	p2 = p ;
	p1 = p ;
        //将mm/dd/yyyy 00:00形式的时间存储到tm结构中
        p = strpbrk(p2, "/");
        *p = '\0';
        my_tm->tm_mon = atoi(p2) - 1;
        *(p) = '/';// in case of memery leak
        p++;
  
        p2 = strpbrk(p, "/");
        *p2 = '\0';
        my_tm->tm_mday = atoi(p);
        *(p2) = '/';// in case of memery leak
        p2++;

        p = strpbrk(p2, " ");//may change to '-' later
        *p = '\0';
        my_tm->tm_year = atoi(p2) - 1900;
        *(p) = ' ';// in case of memery leak
        p++;

        p2 = strpbrk(p, ":");
        *p2 = '\0';
        my_tm->tm_hour = atoi(p) ;
        *(p2) = ':';// in case of memery leak
        p2++;

        my_tm->tm_min = atoi(p2);


        //lastTime形式为mm/dd/yyyy 00:00
        strcpy(p,  lastTime);//"p = lastTime" is wrong, "new"  can only use strcpy;
        strcpy(p2,  lastTime);

        p = strpbrk(lastTime, "/");
        *p = '\0';
        my_tm2->tm_mon = atoi(lastTime) - 1;
        *(p) = '/';// in case of memery leak
        p++;

        p2 = strpbrk(p, "/");
        *p2 = '\0';
        my_tm2->tm_mday = atoi(p);
        *(p2) = '/';// in case of memery leak
        p2++;

        p = strpbrk(p2, " ");//may change to '-' later
        *p = '\0';
        my_tm2->tm_year = atoi(p2) - 1900;
        *(p) = ' ';// in case of memery leak
        p++;

        p2 = strpbrk(p, ":");
        *p2 = '\0';
        my_tm2->tm_hour = atoi(p);
        *(p2) = ':';// in case of memery leak
        p2++;

        my_tm2->tm_min = atoi(p2);


        time_t my_t,my_t2;//声名c语言时间结构time_t的两个变量。
        my_t = mktime(my_tm);
        my_t2 = mktime(my_tm2);

        if((int)(my_t - my_t2) <= 0 )//如果状态发生时间晚于计划制定时间，则数据库记录有错误。
        {
                cout <<"time compute error!"<<endl;
                sprintf(result_info,"错误4");
		sprintf(result_info_explain,"机组开停时间记录有误");
                write_execute_info();
                exit(0);
        }

	delete[] p1;
	delete my_tm,my_tm2;

	int sd_num = 0;
	sd_num = (int)(my_t - my_t2)/3600;//将以秒表示的时间差折算为小时。
	//printf("sd_num is %d\n",sd_num);

        return  sd_num ;
}

	
/************************************************************************************************************************
**
**                 this function will set generator'states to be must_on or must_off  
**
*************************************************************************************************************************/

void find_must_on_off_sd()
{
	printf(" now I am in find_must_on_off_sd\n");
	int sd_number; 

	for(int i = 0 ; i < total_gen ; i ++)
	{
		///////////maybe some problem here///////////////
		if(gen_ptr[i].sfky == 1)
		{
			if(gen_ptr[i].last_state == 1 || gen_ptr[i].last_state == 2) //如果机组上次的状态为开机
			{
				sd_number = compute_must_on_off_sd_num(gen_ptr[i].last_on_off_time);
				sd_number = gen_ptr[i].min_on_time - sd_number;
				//printf("gen is %s,last_state is %d,must on sd is %d\n",gen_ptr[i].gen_id,gen_ptr[i].last_state,sd_number);
				if(sd_number > 0)//必开时段大于0
				{
					if(2 * sd_number <= total_sd)
					{
						for(int j = 0 ; j < 2 * sd_number ; j ++)
						gen_ptr[i].gen_state[j] = 2;//must on ;
					}
					else
					{
						for(int l = 0 ; l < total_sd ; l ++)
						gen_ptr[i].gen_state[l] = 2;//must on;
					}
				}
			}
			else if(gen_ptr[i].last_state == 0 )//机组上一个状态为停机。
			{
				sd_number = compute_must_on_off_sd_num(gen_ptr[i].last_on_off_time);
				sd_number = gen_ptr[i].min_off_time - sd_number;
                                //printf("gen is %s,last_state is %d,must off sd is %d\n",gen_ptr[i].gen_id,gen_ptr[i].last_state,sd_number);
				if(sd_number > 0)
				{
					if(2 * sd_number <= 36)//less than the sd that has the highest load;
					{
						//printf("I am here for 34\n");
						for(int k = 0 ; k < 2 * sd_number ; k ++)
						gen_ptr[i].gen_state[k] = 3;//must off
						//printf(" I am in find_must_on_off_sd() gen is %s\n",gen_ptr[i].gen_id);
					}
					else
					{
						for(int m = 0 ; m < total_sd ; m ++)
						gen_ptr[i].gen_state[m] = 3;//must off;
					}
				}
			}
			//其他的机组状态表明机组不能开机，设置为必停3。
			else//indicate that this generator can't operate ;
			{
				//printf("gen is %s,last_state is %d\n",gen_ptr[i].gen_id,gen_ptr[i].last_state);
				for(int sd = 0 ; sd < total_sd ; sd ++)
				gen_ptr[i].gen_state[sd] = 3;
			}
		}
		else
		{
			//printf("gen is %s,last_state is %d\n",gen_ptr[i].gen_id,gen_ptr[i].last_state);
                        for(int sd = 0 ; sd < total_sd ; sd ++)
                        gen_ptr[i].gen_state[sd] = 3;
                }

	}
}

/********************************************************************************************************
**
**           the funstion set generators to be must_on 
**           according to the minimal mode of the factory
**           appointed by the dispatcher
**
*********************************************************************************************************/
//将电厂的最小运行方式中指定的必开机组设置为必开
void set_mini_mode_must_on_off()
{
	int flag1 = 0,flag2 = 0,flag3 = 0;
	printf("now I am in set_mini_mode_must_on()\n");
	mini_mode_fac_num = get_mini_mode_fac_num(year);//取得有最小方式的电厂个数。
	printf("mini_mode_fac_num is %d\n",mini_mode_fac_num);
	if(mini_mode_fac_num == 0)
	return ;
	else//如果电厂个数不是0，分配内存并将电厂最小方式的数据取出。
	{
		fac_mini_mode_ptr = new FAC_MINI_MODE[mini_mode_fac_num];
		get_mini_mode_data(year);
	}
	
	for(int i = 0 ; i < mini_mode_fac_num ; i ++)
	{
		//strtok(fac_mini_mode_ptr[i].must_on_gen1,"");
		//strtok(fac_mini_mode_ptr[i].must_on_gen2,"");
		//strtok(fac_mini_mode_ptr[i].must_on_gen3,"");
		//printf(" fac is %d\n",i);
		for(int j = 0 ; j < total_gen ; j ++)
		{
			//printf("mini_gen is %s,gen is %s\n",fac_mini_mode_ptr[i].must_on_gen1,gen_ptr[j].gen_id);
			//如果电厂指定了必开机组1，并且在竞价机组里找到了该机组，将机组设置为必开。			
			if(strcmp(fac_mini_mode_ptr[i].must_on_gen1,"")  && !strncmp(fac_mini_mode_ptr[i].must_on_gen1,gen_ptr[j].gen_id,strlen(fac_mini_mode_ptr[i].must_on_gen1)))
			{
				if(gen_ptr[j].last_state == 0)
				for(int sd = oper_sd ; sd < total_sd ; sd ++)
				{
					if(gen_ptr[j].gen_state[sd] != 3)
					gen_ptr[j].gen_state[sd] = 2;
				}
				if(gen_ptr[j].last_state == 1)	
				{
					for(int sd = 0 ; sd < total_sd ; sd ++)
					gen_ptr[j].gen_state[sd] = 2;
				}
				flag1 = 1;
				//printf("flag1 is %d,gen is %s\n",flag1,gen_ptr[j].gen_id);
			}
			//int len = strlen(fac_mini_mode_ptr[i].must_on_gen2);
			//printf("len is %d,gen is sdffd%s43342\n",len,fac_mini_mode_ptr[i].must_on_gen2);
                        if(strcmp(fac_mini_mode_ptr[i].must_on_gen2,"") && !strncmp(fac_mini_mode_ptr[i].must_on_gen2,gen_ptr[j].gen_id,strlen(fac_mini_mode_ptr[i].must_on_gen2)))
                        {
                                if(gen_ptr[j].last_state == 0)
                                for(int sd = oper_sd ; sd < total_sd ; sd ++)
                                {
                                        if(gen_ptr[j].gen_state[sd] != 3)
                                        gen_ptr[j].gen_state[sd] = 2;
                                }
                               	if(gen_ptr[j].last_state == 1) 
                                {
                                        for(int sd = 0 ; sd < total_sd ; sd ++)
                                        gen_ptr[j].gen_state[sd] = 2;
                                }
				flag2 = 1;
                        }
				
                        if(strcmp(fac_mini_mode_ptr[i].must_on_gen3,"")  && !strncmp(fac_mini_mode_ptr[i].must_on_gen3,gen_ptr[j].gen_id,strlen(fac_mini_mode_ptr[i].must_on_gen3)))
                        {
                                if(gen_ptr[j].last_state == 0)
                                for(int sd = oper_sd ; sd < total_sd ; sd ++)
                                {
                                        if(gen_ptr[j].gen_state[sd] != 3)
                                        gen_ptr[j].gen_state[sd] = 2;
                                }
                                if(gen_ptr[j].last_state == 1)
                                {
                                        for(int sd = 0 ; sd < total_sd ; sd ++)
                                        gen_ptr[j].gen_state[sd] = 2;
                                }
				flag3 = 1;
                        }

			if(strcmp(fac_mini_mode_ptr[i].must_off_gen1,"") && !strncmp(fac_mini_mode_ptr[i].must_off_gen1,gen_ptr[j].gen_id,strlen(fac_mini_mode_ptr[i].must_off_gen1)))
			{
				if(gen_ptr[j].last_state == 0)
				{
					for(int sd = 0 ; sd < total_sd ; sd ++)
					gen_ptr[j].gen_state[sd] = 3;
				}
				if(gen_ptr[j].last_state == 1)
                                for(int sd = stop_sd ; sd < total_sd ; sd ++)
				{
					if(gen_ptr[j].gen_state[sd] != 2)
					gen_ptr[j].gen_state[sd] = 3;
				}
			}
			if(strcmp(fac_mini_mode_ptr[i].must_off_gen2,"") && !strncmp(fac_mini_mode_ptr[i].must_off_gen2,gen_ptr[j].gen_id,strlen(fac_mini_mode_ptr[i].must_off_gen2)))
			{
                                if(gen_ptr[j].last_state == 0)
                                {
                                        for(int sd = 0 ; sd < total_sd ; sd ++)
                                        gen_ptr[j].gen_state[sd] = 3;
                                }
                                if(gen_ptr[j].last_state == 1)
                                for(int sd = stop_sd ; sd < total_sd ; sd ++)
                                {
                                        if(gen_ptr[j].gen_state[sd] != 2)
                                        gen_ptr[j].gen_state[sd] = 3;
                                }
                        }
			if(strcmp(fac_mini_mode_ptr[i].must_off_gen3,"") && !strncmp(fac_mini_mode_ptr[i].must_off_gen3,gen_ptr[j].gen_id,strlen(fac_mini_mode_ptr[i].must_off_gen3)))		
			{
                                if(gen_ptr[j].last_state == 0)
                                {
                                        for(int sd = 0 ; sd < total_sd ; sd ++)
                                        gen_ptr[j].gen_state[sd] = 3;
                                }
                                if(gen_ptr[j].last_state == 1)
                                for(int sd = stop_sd ; sd < total_sd ; sd ++)
                                {
                                        if(gen_ptr[j].gen_state[sd] != 2)
                                        gen_ptr[j].gen_state[sd] = 3;
                                }
                        }
		}
/*
		if(strcmp(fac_mini_mode_ptr[i].must_on_gen1,"") && flag1 == 0)
		{
			sprintf(result_info,"错误");
			sprintf(result_info_explain,"找不到最小方式指定必开机组%s",fac_mini_mode_ptr[i].must_on_gen1);
			printf("result_info_explain is %s\n",result_info_explain);
			//write_execute_info();
			exit(-1);
		}
		if(strcmp(fac_mini_mode_ptr[i].must_on_gen2,"") && flag2 == 0)
		{
			sprintf(result_info,"错误");
			sprintf(result_info_explain,"找不到最小方式指定必开机组%s",fac_mini_mode_ptr[i].must_on_gen2);
                        printf("result_info_explain is %s\n",result_info_explain);
			//write_execute_info();
			exit(-1);
                }
		if(strcmp(fac_mini_mode_ptr[i].must_on_gen3,"") && flag3 == 0)
                {
                        sprintf(result_info,"错误");
                        sprintf(result_info_explain,"找不到最小方式指定必开机组%s",fac_mini_mode_ptr[i].must_on_gen3);
                        printf("result_info_explain is %s\n",result_info_explain);
                        //write_execute_info();
                        exit(-1);
                }
		flag1 = 0;
		flag2 = 0;
		flag3 = 0;
*/
	}  
	
}

/*********************************************************************************************************
**
**           find if the factory that the generator belonged
**           satisfies the minimal mode
**
*********************************************************************************************************/
//检测机组所在电厂最小方式是否满足
int check_gen_mini_num(int gen_ord)
{
	printf("now I am in check_gen_mini_num(int)\n");
	printf("gen_ord is %d\n",gen_ord);
	//int flag = 0;
	int gen_num = 0;
	for(int i = 0 ; i < total_gen ; i ++)//找到该机组所在电厂的运行机组数。
	{
		if((gen_ptr[i].gen_state[total_sd - 1] == 1 || gen_ptr[i].gen_state[total_gen - 1] == 2) && !strcmp(gen_ptr[i].fac_id,gen_ptr[gen_ord].fac_id))//最后一个时段开机，则该机组为运行。
		{
			printf("gen is %s,gen_ptr[gen_ord] is %s\n",gen_ptr[i].fac_id,gen_ptr[gen_ord].fac_id);
			gen_num ++;
		}
	}
	printf("gen_num is %d\n",gen_num);

	for(int j = 0 ; j < mini_mode_fac_num ; j ++)
	{	
		if(!strncmp(gen_ptr[gen_ord].fac_id,fac_mini_mode_ptr[j].fac_id,strlen(gen_ptr[gen_ord].fac_id)))
		{
			printf("gen_ptr[gen_ord].fac_id is %s,fac_mini_mode_ptr[j].fac_id is %s\n",gen_ptr[gen_ord].fac_id,fac_mini_mode_ptr[j].fac_id); 
                        //如果运行机组数大于电厂最小方式指定的台数
			if(gen_num > fac_mini_mode_ptr[j].gen_mini_num)
			return 1;//满足最小方式
			else
			return 0;//不满足最小方式
		}
	} 
	return 1;//this factory has no minimal mode;
}

/*********************************************************************************************************
**
**           set some genertors to be off to
**           satisfy the down_spinning_reserve_3
**
*********************************************************************************************************/
//在第45--48时段停机，以满足下备用。
int set_gen_off_for_down_reserve_3()
{
	printf("now I am in set_gen_off_for_down_reserve_3()\n");
	int ret,ret1,ret2,count = 0;

	for(int i = total_gen - 1 ; i >= 0 ; i --)//从报价高的机组开始停
	{
		printf("i is %d, gen is %s\n",i,gen_ptr[i].gen_id);
		for(int sd = 0 ; sd < total_sd ; sd ++)
		printf("%d",gen_ptr[i].gen_state[sd]);
		printf("\n");
		for(int sd = 0 ; sd < total_sd ; sd ++)//找到该机组的必开时段数，若该机组有必开时段，则不让它停。
		{
			if(gen_ptr[i].gen_state[sd] == 2)
			count ++;
		}
		ret = check_gen_mini_num(i);//检测该机组所在电厂是否满足最小运行方式，如果停机后不满足最小方式，该机组不能停。
		printf("ret of check_gen_mini_num(i) is %d\n",ret);
		if(ret == 1)//满足最小方式	
		{
			printf("gen is %s,gen_ptr[i].last_state is %d,gen_ptr[i].gen_state[total_sd - 1] is %d,count is %d\n",gen_ptr[i].gen_id,gen_ptr[i].last_state,gen_ptr[i].gen_state[total_sd - 1],count);
			if(gen_ptr[i].last_state == 1 && gen_ptr[i].gen_state[total_sd - 1] == 1 && count == 0)//该机组上一个状态为开，并且不是必开机组，如果它在48时段为开，则停掉该机组。
			{
				printf("gen can be set off is %s\n",gen_ptr[i].gen_id);
				printf("ret of check_gen_mini_num(i) is %d\n",ret);
				for(int sd1 = stop_sd ; sd1 < total_sd ; sd1 ++)
				gen_ptr[i].gen_state[sd1] = 0;
		
				ret1 = check_down_spinning_reserve_3();//如果停掉一台机组后下备用满足，则返回。
				if(ret1 == 1)
				return 1;
				else//检测下一台机组前将必开时段数置0
				{
					count = 0;
				}
			}
			if(gen_ptr[i].last_state == 0  && gen_ptr[i].gen_state[total_sd - 1] == 1 && count == 0)//如果该机组为晚开机组，并且不是必开机组，可以停掉;
			{
				for(int sd2 = oper_sd ; sd2 < total_sd ; sd2 ++)
				gen_ptr[i].gen_state[sd2] = 0;
				ret1 = check_down_spinning_reserve_3();
				ret2 = check_up_spinning_reserve_2();//由于停掉晚开机组影响尖峰上备用，所以要检测第15--44时段的上备用。
				if(ret2 == 0)//如果停掉该机组后上备用不满足，将该机组的状态恢复。
				{
					for(int sd2 = oper_sd ; sd2 < total_sd ; sd2 ++)
                               		gen_ptr[i].gen_state[sd2] = 1;
					count = 0;
				}
				if(ret1 == 1 && ret2 == 1)
				return 1;
			}
		}	
		count = 0;
	}
	//所有机组检测完后，函数仍没有返回，说明找不到合适的机组停机来满足45--48时段的下备用。
	return 0;//if till now havn't find suitable combination, return 0;
}

/*********************************************************************************************************
**
**           the function takes some measures to
**           satisy the minimal mode of the factory
**
*********************************************************************************************************/
//采取措施，满足电厂的最小方式，最小方式的优先级高于开机阈值限制，即如果开2台机组才能满足最小方式，而开机阈值为1，则仍开两台机组以满足最小方式。
int deal_with_mini_mode1(int n,int gen_num)
{
        //形参n将电厂最小方式结构数组的序号传入函数，gen_num表示该序号代表的电厂当前有多少运行的机组。
        printf("now I am in deal_with_mini_mode(int n,int gen_num)\n");
        int ret ;
        int * temp_state;//data struct to record the states of generator;
        temp_state = new int[total_sd];//分配内存，记录机组的初始状态。

        for(int i = 0 ; i < total_gen ; i ++)
        {
                printf("i is %d,gen is %s\n",i,gen_ptr[i].gen_id);
                if(!strncmp(gen_ptr[i].fac_id,fac_mini_mode_ptr[n].fac_id,strlen(gen_ptr[i].fac_id)))//如果这台机组属于该电厂
                {
                        printf("gen is %s\n",gen_ptr[i].gen_id);
                        if(gen_ptr[i].gen_state[total_sd - 1] == 0)//如果该机组计划为停机
                        {
                                printf("I am here to record state\n");
                                //记录机组的初始状态。
                                for(int sd1 = 0 ;sd1 < total_sd ; sd1 ++)//record the state of the generator;
                                temp_state[sd1] = gen_ptr[i].gen_state[sd1];

                                if(gen_ptr[i].last_state == 1)//上一个状态为开，该机组全天设置为开。
                                {
                                        printf("gen_ptr[i].last_state == 1 is %s\n",gen_ptr[i].gen_id);
                                        for(int sd = 0 ; sd < total_sd ; sd ++)
                                        gen_ptr[i].gen_state[sd] = 1;
                                        print_state();
                                }
/*
                                if(gen_ptr[i].last_state == 0)//上一个状态为停，该机组从15时段设置为开。
                                {
                                        printf("gen_ptr[i].last_state == 0 is %s\n",gen_ptr[i].gen_id);
                                        for(int sd3 = oper_sd ; sd3 < total_sd ; sd3 ++)
                                        gen_ptr[i].gen_state[sd3] = 1;
                                        print_state();
                                        decrease_later_on_generator(1);

                                }
                                //检测下备用（此处检测下备用也许不必要，因为机组组合程序最后还要检测下备用）
*/
                                ret = check_down_spinning_reserve_3();
                                if(ret == 1)
                                {
                                        gen_num ++;
                                        printf("gen on is %d\n",gen_num);
                                        if(gen_num >= fac_mini_mode_ptr[n].gen_mini_num)//satisy the minimal mode;
                                        return 1;
                                        else
                                        continue;
                                }
                                else//take some measures to satisfy dowm_spinning_reserve_3
                                {
                                        ret = set_gen_off_for_down_reserve_3();
                                        if(ret == 1)
                                        {
                                                gen_num ++;
                                                if(gen_num >= fac_mini_mode_ptr[n].gen_mini_num)//satisy the minimal mode;
                                                return 1;
                                                else
                                                continue;
                                        }
                                        else//can't find suitable combination,restore the states of the   generator;
                                        {
                                                for(int sd2 = 0 ; sd2 < total_sd ; sd2 ++)
                                                gen_ptr[i].gen_state[sd2] = temp_state[sd2] ;
                                        }
                                }
                        }
                }
                //delete[] temp_state;
                //return 0;
        }
        delete[] temp_state;
        printf("something I can't resolve\n");
        return 0;
}
/*********************************************************************************************************
**
**           the function takes some measures to
**           satisy the minimal mode of the factory
**
*********************************************************************************************************/
//采取措施，满足电厂的最小方式，最小方式的优先级高于开机阈值限制，即如果开2台机组才能满足最小方式，而开机阈值为1，则仍开两台机组以满足最小方式。
int deal_with_mini_mode2(int n,int gen_num)
{
        //形参n将电厂最小方式结构数组的序号传入函数，gen_num表示该序号代表的电厂当前有多少运行的机组。
        printf("now I am in deal_with_mini_mode(int n,int gen_num)\n");
        int ret ;
        int * temp_state;//data struct to record the states of generator;
        temp_state = new int[total_sd];//分配内存，记录机组的初始状态。

        for(int i = 0 ; i < total_gen ; i ++)
        {
                printf("i is %d,gen is %s\n",i,gen_ptr[i].gen_id);
                if(!strncmp(gen_ptr[i].fac_id,fac_mini_mode_ptr[n].fac_id,strlen(gen_ptr[i].fac_id)))//如果这台机组属于该电厂
                {
                        printf("gen is %s\n",gen_ptr[i].gen_id);
                        if(gen_ptr[i].gen_state[total_sd - 1] == 0)//如果该机组计划为停机
                        {
                                printf("I am here to record state\n");
                                //记录机组的初始状态。
                                for(int sd1 = 0 ;sd1 < total_sd ; sd1 ++)//record the state of the generator;
                                temp_state[sd1] = gen_ptr[i].gen_state[sd1];
/*
                                if(gen_ptr[i].last_state == 1)//上一个状态为开，该机组全天设置为开。
                                {
                                        printf("gen_ptr[i].last_state == 1 is %s\n",gen_ptr[i].gen_id);
                                        for(int sd = 0 ; sd < total_sd ; sd ++)
                                        gen_ptr[i].gen_state[sd] = 1;
                                        print_state();
                                }
*/
                                if(gen_ptr[i].last_state == 0)//上一个状态为停，该机组从15时段设置为开。
                                {
                                        printf("gen_ptr[i].last_state == 0 is %s\n",gen_ptr[i].gen_id);
                                        for(int sd3 = oper_sd ; sd3 < total_sd ; sd3 ++)
                                        gen_ptr[i].gen_state[sd3] = 1;
                                        print_state();
                                        decrease_later_on_generator(1);

                                }
                                //检测下备用（此处检测下备用也许不必要，因为机组组合程序最后还要检测下备用）
                                ret = check_down_spinning_reserve_3();
                                if(ret == 1)
                                {
                                        gen_num ++;
                                        printf("gen on is %d\n",gen_num);
                                        if(gen_num >= fac_mini_mode_ptr[n].gen_mini_num)//satisy the minimal mode;
                                        return 1;
                                        else
                                        continue;
                                }
                                else//take some measures to satisfy dowm_spinning_reserve_3
                                {
                                        ret = set_gen_off_for_down_reserve_3();
                                        if(ret == 1)
                                        {
                                                gen_num ++;
                                                if(gen_num >= fac_mini_mode_ptr[n].gen_mini_num)//satisy the minimal mode;
                                                return 1;
                                                else
                                                continue;
                                        }
                                        else//can't find suitable combination,restore the states of the   generator;
                                        {
                                                for(int sd2 = 0 ; sd2 < total_sd ; sd2 ++)
                                                gen_ptr[i].gen_state[sd2] = temp_state[sd2] ;
                                        }
                                }
                        }
                }
                //delete[] temp_state;
                //return 0;
        }
        delete[] temp_state;
        printf("something I can't resolve\n");
        return 0;
}
/*********************************************************************************************************
**
**           the function takes some measures to 
**           satisy the minimal mode of the factory
**
*********************************************************************************************************/
//采取措施，满足电厂的最小方式，最小方式的优先级高于开机阈值限制，即如果开2台机组才能满足最小方式，而开机阈值为1，则仍开两台机组以满足最小方式。
int deal_with_mini_mode(int n,int gen_num)
{
	//形参n将电厂最小方式结构数组的序号传入函数，gen_num表示该序号代表的电厂当前有多少运行的机组。
	printf("now I am in deal_with_mini_mode(int n,int gen_num)\n");
	int ret ;
	int * temp_state;//data struct to record the states of generator;
	temp_state = new int[total_sd];//分配内存，记录机组的初始状态。
	
	for(int i = 0 ; i < total_gen ; i ++)
	{
		printf("i is %d,gen is %s\n",i,gen_ptr[i].gen_id);
		if(!strncmp(gen_ptr[i].fac_id,fac_mini_mode_ptr[n].fac_id,strlen(gen_ptr[i].fac_id)))//如果这台机组属于该电厂
		{
			printf("gen is %s\n",gen_ptr[i].gen_id);
			if(gen_ptr[i].gen_state[total_sd - 1] == 0)//如果该机组计划为停机
			{
				printf("I am here to record state\n");
				//记录机组的初始状态。
				for(int sd1 = 0 ;sd1 < total_sd ; sd1 ++)//record the state of the generator;
				temp_state[sd1] = gen_ptr[i].gen_state[sd1];

				if(gen_ptr[i].last_state == 1)//上一个状态为开，该机组全天设置为开。
				{
					printf("gen_ptr[i].last_state == 1 is %s\n",gen_ptr[i].gen_id);
					for(int sd = 0 ; sd < total_sd ; sd ++)
					gen_ptr[i].gen_state[sd] = 1;
					print_state();
				}
				if(gen_ptr[i].last_state == 0)//上一个状态为停，该机组从15时段设置为开。
				{
					printf("gen_ptr[i].last_state == 0 is %s\n",gen_ptr[i].gen_id);
					for(int sd3 = oper_sd ; sd3 < total_sd ; sd3 ++)
					gen_ptr[i].gen_state[sd3] = 1;
					print_state();
					decrease_later_on_generator(1);
					
				}
 				//检测下备用（此处检测下备用也许不必要，因为机组组合程序最后还要检测下备用）
				ret = check_down_spinning_reserve_3();
				if(ret == 1)
				{
					gen_num ++;
					printf("gen on is %d\n",gen_num);
					if(gen_num >= fac_mini_mode_ptr[n].gen_mini_num)//satisy the minimal mode;
					return 1;
					else
					continue;
				}
				else//take some measures to satisfy dowm_spinning_reserve_3
				{
					ret = set_gen_off_for_down_reserve_3();
					if(ret == 1)
					{
						gen_num ++;
						if(gen_num >= fac_mini_mode_ptr[n].gen_mini_num)//satisy the minimal mode;
						return 1;
						else
						continue;
					}
					else//can't find suitable combination,restore the states of the   generator;
					{
						for(int sd2 = 0 ; sd2 < total_sd ; sd2 ++)
						gen_ptr[i].gen_state[sd2] = temp_state[sd2] ;
					}
				}
			}
		}
		//delete[] temp_state;
		//return 0;
	}
        delete[] temp_state;
	printf("something I can't resolve\n");
	return 0;
}

/*********************************************************************************************************
**
**           the function check the schedule of generators
**           to ensure that the minimal mode was satisfied
**
***********************************************************************************************************/
//检测各个电厂的最小方式是否满足
void check_mini_mode()
{
	printf("now I am in check_mini_mode()\n");
	int ret,count = 0;
	//有最小方式的电厂为0个，返回。
	if(mini_mode_fac_num == 0)
	return;

	for(int n = 0 ; n < mini_mode_fac_num ; n ++)
	{ 
		for(int i = 0 ; i < total_gen ; i ++)
		{
			if(!strncmp(gen_ptr[i].fac_id , fac_mini_mode_ptr[n].fac_id,strlen(gen_ptr[i].fac_id)) && (gen_ptr[i].gen_state[total_sd - 1] == 1 || gen_ptr[i].gen_state[total_sd - 1] == 2))//机组属于该电厂，并且开机。
			count ++; 	
		}
		printf("fac is %s,count is %d,mini_gen is %d\n",fac_mini_mode_ptr[n].fac_id,count,fac_mini_mode_ptr[n].gen_mini_num);
		if(count >= fac_mini_mode_ptr[n].gen_mini_num)//运行机组数大于最小方式指定台数。
		{
			count = 0;
			continue;
		}
		else
		{
			printf("n is %d,count is %d\n",n,count);
			//ret = deal_with_mini_mode(n,count);//采取措施以满足最小方式
                        ret = deal_with_mini_mode1(n,count);
                        for(int i = 0 ; i < total_gen ; i ++)
                        {
                                if(!strncmp(gen_ptr[i].fac_id , fac_mini_mode_ptr[n].fac_id,strlen(gen_ptr[i].fac_id)) && (gen_ptr[i].gen_state[total_sd - 1] == 1 || gen_ptr[i].gen_state[total_sd - 1] == 2))//机组属于该电厂，并且开机。
                                count ++;
                        }
                        printf("fac is %s,count is %d,mini_gen is %d\n",fac_mini_mode_ptr[n].fac_id,count,fac_mini_mode_ptr[n].gen_mini_num);
                        if(count >= fac_mini_mode_ptr[n].gen_mini_num)//运行机组数大于最小方式指定台数。
                        {
                                count = 0;
                                continue;
                        }
                        else
                        ret = deal_with_mini_mode2(n,count);
			if(ret == 1)
			{
				count = 0;
				continue;
			}
			else
			{
				printf("can't resolve mini_mode \n");
/*
				sprintf(result_info,"错误");
				sprintf(result_info_explain,"程序无法找到满足最小方式的解，请检测指定最小方式是否合理");
				write_execute_info();
				//exit(-1);
*/
				//程序解决不了该厂最小方式，继续检测其他电厂。
				continue;
			}
		}
	}
}

/*******************************************************************************************************
**
**		this function check the information of mini_mode
**
*******************************************************************************************************/
//将电厂最小方式的满足情况写入程序执行信息。
void mini_mode_info()
{
        printf("now I am in mini_mode_info()\n");
        int ret,count = 0;
	char temp[100];

        if(mini_mode_fac_num == 0)
        return;

        for(int n = 0 ; n < mini_mode_fac_num ; n ++)
        {
                for(int i = 0 ; i < total_gen ; i ++)
                {
                        if(!strncmp(gen_ptr[i].fac_id , fac_mini_mode_ptr[n].fac_id,strlen(gen_ptr[i].fac_id)) && (gen_ptr[i].gen_state[total_sd - 1] == 1 || gen_ptr[i].gen_state[total_sd - 1] == 2))
                        count ++;
                }
                printf("fac is %s,count is %d,mini_gen is %d\n",fac_mini_mode_ptr[n].fac_id,count,fac_mini_mode_ptr[n].gen_mini_num);
                if(count >= fac_mini_mode_ptr[n].gen_mini_num)
                {
                        count = 0;
                        continue;
                }
                else
                {
			if(!strcmp(result_info_explain,""))
			{
				sprintf(result_info_explain,"%s",fac_mini_mode_ptr[n].fac_name);
				//sprintf(temp,"最小方式无法满足，该厂可用机组或有报价机组数小于最小方式指定台数");
				sprintf(temp,"最小方式无法满足，该厂可用机组或有报价机组数为%d,最小方式指定台数为%d",count,fac_mini_mode_ptr[n].gen_mini_num);
				strcat(result_info_explain,temp);
			}
			else
			{
				sprintf(temp,",%s",fac_mini_mode_ptr[n].fac_name);
				strcat(result_info_explain,temp);
				sprintf(temp,"最小方式无法满足，该厂可用机组或有报价机组数为%d,最小方式指定台数为%d",count,fac_mini_mode_ptr[n].gen_mini_num);
				strcat(result_info_explain,temp);
                        }
                        continue;
                }
        }
}

/*****************************************************************************************
**
**        this function set mannul state of the generators 
**
******************************************************************************************/
//将调度员手工设置的机组状态指定为必开或必停。
void set_mannul_state()
{
	//printf("I am in set_mannul_state()!\n");
	 
	for(int sd = 0 ; sd < total_sd ; sd ++)
	{
		get_mannul_state(year,month,day,sd + 1);//从数据库中取得机组手工状态
		for(int i = 0 ; i < total_gen ; i ++)
		{
			for(int j = 0 ; j < total_gen ; j ++)
			{
				if(!strcmp(gen_ptr[i].gen_id , mannul_state_ptr[j].gen_id))
				gen_ptr[i].gen_state[sd] = mannul_state_ptr[j].state;
			}
		}
	}
	for(int i = 0 ; i < total_gen ; i ++)
	{
		for(int sd = 0 ; sd < total_sd ; sd ++)
		{
			if(gen_ptr[i].gen_state[sd] == 2)//sd时段设为必开，则该机组从sd时段起全天为必开。
			{
				for(int sd1 = sd ; sd1 < total_sd ; sd1 ++)
				gen_ptr[i].gen_state[sd1] = 2;
			}
			if(gen_ptr[i].gen_state[sd] == 3)//sd时段设为必停，则从sd时段起全天为必停。
			{
				for(int  sd2 = sd ; sd2 < total_sd ; sd2 ++)
				gen_ptr[i].gen_state[sd2] = 3;
			}
		}
	}
/*
	for(int i = 0 ;  i < total_gen ; i ++)
	{
		//printf("gen is %s\t",gen_ptr[i].gen_id);
		for(int j = 0 ; j < total_sd ; j ++)
		{
			//printf("%d",gen_ptr[i].gen_state[j]);
		}
		//printf("\n");
	}
	//print_state();
*/
}

/*****************************************************************************************
**
**        this function set  state of the generators for mannul states 
**
******************************************************************************************/
//从有约束机组开停机表中取得人工修正的机组必开、必停状态。
void get_gen_state_for_mannul()
{
        printf("I am in ()!\n");
        for(int sd = 0 ; sd < total_sd ; sd ++)
        {
                get_gen_state(year,month,day,sd + 1);//按时段将机组的状态从数据库中取出。
                for(int i = 0 ; i < total_gen ; i ++)
                {
                        for(int j = 0 ; j < total_un_num ; j ++)
                        {
				//只将调度员指定的必开、必停状态取出。
                                if(!strcmp(gen_ptr[i].gen_id , qt_gen_struct[j].un_id) && (qt_gen_struct[j].run_status == 2  || qt_gen_struct[j].run_status == 3))
                                {
                                        gen_ptr[i].gen_state[sd] = qt_gen_struct[j].run_status;
                                        gen_ptr[i].pxh = qt_gen_struct[j].pxh;
                                        //printf("gen %s pxh is %d\n",gen_ptr[i].gen_id,gen_ptr[i].pxh);
                                }
                        }
                }
        }
/*
        for(int i = 0 ;  i < total_gen ; i ++)
        {
                //printf("gen is %s\t",gen_ptr[i].gen_id);
                for(int j = 0 ; j < total_sd ; j ++)
                {
                        //printf("%d",gen_ptr[i].gen_state[j]);
                }
                //printf("\n");
        }
        //print_state();
*/
}

/*****************************************************************************************
**
**        this function set  state of the generators for safe constraint
**
******************************************************************************************/
//取得经过安全校验的机组组合计划。
void get_gen_state_for_safe()
{
        //printf("I am in ()!\n");
        for(int sd = 0 ; sd < total_sd ; sd ++)
        {
                get_gen_state(year,month,day,sd + 1);//按时段将机组的状态从数据库中取出。
                for(int i = 0 ; i < total_gen ; i ++)
                {
                        for(int j = 0 ; j < total_un_num ; j ++)
                        {
                                if(!strcmp(gen_ptr[i].gen_id , qt_gen_struct[j].un_id))
				{
                                	gen_ptr[i].gen_state[sd] = qt_gen_struct[j].run_status;
					gen_ptr[i].pxh = qt_gen_struct[j].pxh;
					//printf("gen %s pxh is %d\n",gen_ptr[i].gen_id,gen_ptr[i].pxh);
				}
                        }
                }
        }
/*
        for(int i = 0 ;  i < total_gen ; i ++)
        {
                //printf("gen is %s\t",gen_ptr[i].gen_id);
                for(int j = 0 ; j < total_sd ; j ++)
                {
                        //printf("%d",gen_ptr[i].gen_state[j]);
                }
                //printf("\n");
        }
        //print_state();
*/
}
/************************************************************************************************
**
**       this function check the spinning reserve of all the sd 
**
*************************************************************************************************/

int check_spinning_reserve()
{
        int up_spinning_reserve = 0;
        int down_spinning_reserve = 0;
        int up_ok = 0;
        int down_ok = 0;

	printf("now I am checking all sd spinning reserve !!\n");
        for(int sd  = 0 ; sd  < total_sd ; sd ++)
        {
                //printf("sd is %d\n",sd);
                for(int i = 0 ; i  < total_gen ; i ++)
                {
                        if(gen_ptr[i].gen_state[sd] == 1 || gen_ptr[i].gen_state[sd] == 2)
                        {
                                up_spinning_reserve = gen_ptr[i].mw_max[sd] + up_spinning_reserve;
                                down_spinning_reserve = gen_ptr[i].mw_min[sd] + down_spinning_reserve;

				//printf("genid is %s ,mw_max is %d,mw_min is %d\n",gen_ptr[i].gen_id,gen_ptr[i].mw_max[sd],gen_ptr[i].mw_min[sd]);
                        }
                }
                //printf("up is %d down is %d\n",up_spinning_reserve,down_spinning_reserve);
                if(up_spinning_reserve < totalload_ptr[sd] + load_forecasting_ptr[sd] * UP_ROTARY_RATE/100)
                {
                        up_ok += 1;
                        printf("sd%d up_spinning_reserve is unenough,load is %f, spinning reserve is %d\n",sd,totalload_ptr[sd],up_spinning_reserve);
                }
                if(down_spinning_reserve > totalload_ptr[sd] - load_forecasting_ptr[sd] * DOWN_ROTARY_RATE/100)
                {
                        down_ok += 1;
                        printf("sd%d down_spinning_reserve is unenough,load is %f, spinning reserve is %d\n",sd,totalload_ptr[sd],down_spinning_reserve);
                }
                up_spinning_reserve = 0;
                down_spinning_reserve = 0;
        }

        if(up_ok == 0 && down_ok == 0)
        return 0;  //0 means up and down spinning reserve are both enough;
        if(up_ok != 0 && down_ok == 0)
        return 1;  // 1 means up spinning reserve is unenough ,but down spinning reserve is enough;
        if(up_ok == 0 && down_ok != 0)
        return 2;  //2 means up spinning reserve is enough,but down spinning reserve is unenough;
        if(up_ok != 0 && down_ok != 0)
        return 3;  //3 means up and spinning down reserve are both unenough;
	return 4;  //something is wrong;           
}

/*******************************************************************************************
**
**         find the number of running generators yesterday
**
********************************************************************************************/

int find_on_gen_num()
{
	int num = 0;
	for(int i = 0 ; i < total_gen ; i ++)
	{
		if(gen_ptr[i].last_state == 1)
		num ++;
	}
	return num;
}

/*******************************************************************************************
**
**         check down spinning reserve of 1 -- oper_sd sd  
**
********************************************************************************************/
//检测1--14时段的下备用
int check_down_spinning_reserve_1()
{
	int lowest_load_sd = find_lowest_totalload_sd();
	int down_ok = 0;
	int down_spinning_reserve = 0 ;
	int running_gen_num = find_on_gen_num();
	float temp = 0.0;
	int rec_sd = 0;
	int rec_down_spinning_reserve = 0;
	float rec_totalload = 0.0;
	float diff = 0;
	for(int sd = 0 ; sd < oper_sd ; sd ++)
	{
		for(int i = 0 ; i < total_gen ; i ++)
		{
			if(gen_ptr[i].gen_state[sd] == 1 || gen_ptr[i].gen_state[sd] == 2)
			{
				down_spinning_reserve = gen_ptr[i].mw_min[sd] + down_spinning_reserve;
				//printf("down_spinning_reserve is %d\n",down_spinning_reserve);
			}
		}
/*
                if(down_spinning_reserve > totalload_ptr[sd] - load_forecasting_ptr[sd] *  DOWN_ROTARY_RATE/100)
                {
                        down_ok += 1;
                        //printf("sd%d down_spinning_reserve is unenough,load  %f,spinning reserve is %d\n",sd,totalload_ptr[sd],down_spinning_reserve);
                }
                down_spinning_reserve = 0;
*/
                //if(down_spinning_reserve > totalload_ptr[sd])//  + 2 * running_gen_num)//- load_forecasting_ptr[sd] *  DOWN_ROTARY_RATE/100)
		if(down_spinning_reserve > totalload_ptr[sd] - load_forecasting_ptr[lowest_load_sd] * FIRST_SEG_DOWN_ROTARY_RATE/100) 
                {
                        down_ok += 1;
			diff = down_spinning_reserve - totalload_ptr[sd] + load_forecasting_ptr[lowest_load_sd] * FIRST_SEG_DOWN_ROTARY_RATE/100;
			if(diff > temp)
			{
				rec_sd = sd;
				rec_down_spinning_reserve = down_spinning_reserve;
				rec_totalload = totalload_ptr[sd];
				temp = diff;
			}		
                        printf("sd%d down_spinning_reserve is unenough,load  %f,spinning reserve is %d,diff is %f\n",sd,totalload_ptr[sd],down_spinning_reserve,diff);
                }
                down_spinning_reserve = 0;
		
        }
	if(down_ok == 0)
	return 1;
	else
	{
       		sprintf(check_reserve_info,"受昨天机组状态影响，第%d时段下备用不够，总负荷为%f,机组出力下限为%d",rec_sd,rec_totalload,rec_down_spinning_reserve);
		printf("check_reserve_info is %s\n",check_reserve_info);
		return 0;
	}
}

/*************************************************************************************
**
**              check up spinning reserve of 1 -- oper_sd sd 
**
**************************************************************************************/

int check_up_spinning_reserve_1()
{
        int up_ok = 0;
        int rec_sd = 0;
        int rec_up_spinning_reserve = 0;
        float rec_totalload = 0.0;
        float diff = 0;
        float temp = 0.0;
	float up_spinning_reserve;
        for(int sd = 0 ; sd < oper_sd ; sd ++)
        {
                for(int i = 0 ; i < total_gen ; i ++)
                {
                        if(gen_ptr[i].gen_state[sd] == 1 || gen_ptr[i].gen_state[sd] == 2)
                        up_spinning_reserve = gen_ptr[i].mw_max[sd] + up_spinning_reserve;
                }
                if(up_spinning_reserve < totalload_ptr[sd] + load_forecasting_ptr[sd] * UP_ROTARY_RATE/100)
                {
                        up_ok += 1;
			diff = totalload_ptr[sd] + load_forecasting_ptr[sd] * UP_ROTARY_RATE/100 - up_spinning_reserve;
			if(diff > temp)
			{
				rec_sd = sd;
				rec_up_spinning_reserve = up_spinning_reserve;
				rec_totalload = totalload_ptr[sd];
				temp = diff;
			}
                        printf("sd%d up_spinning_reserve is unenough,load is %f,spinning reserve is %d\n",sd,totalload_ptr[sd], up_spinning_reserve);
                }
                up_spinning_reserve = 0;
        }
        if(up_ok == 0)
        return 1;
        else
	{
		sprintf(check_reserve_info,"受昨天机组状态影响，第%d时段上备用不够，机组出力上限为%d,差额为%f",rec_sd,rec_up_spinning_reserve,temp);
		printf("check_reserve_info is %s\n",check_reserve_info);
		return 0;
	}
}


/****************************************************************************************
**            
**                check up spinning reserve of oper_sd - 1 -- stop_sd sd 
**
****************************************************************************************/

int check_up_spinning_reserve_2()
{
	int up_ok = 0;
	int peak_sd = find_peak_load_sd();
	int up_spinning_reserve = 0;
        for(int sd = oper_sd ; sd < stop_sd ; sd ++)
        {
                for(int i = 0 ; i < total_gen ; i ++)
                {
                        if(gen_ptr[i].gen_state[sd] == 1 || gen_ptr[i].gen_state[sd] == 2)
                        up_spinning_reserve = gen_ptr[i].mw_max[sd] + up_spinning_reserve;
                }
                if(up_spinning_reserve < totalload_ptr[sd] + load_forecasting_ptr[sd] * PEAK_LOAD_ROTARY_RATE/100)
                //if(up_spinning_reserve < totalload_ptr[sd] + load_forecasting_ptr[sd] * UP_ROTARY_RATE/100)
                {
                        up_ok += 1;
                        printf("sd%d up_spinning_reserve is unenough,load is %f,and spinning reserve is %d\n",sd,totalload_ptr[sd],up_spinning_reserve);
                }
/*
		if(peak_load == 0)
		{
                	if(up_spinning_reserve < totalload_ptr[sd] + load_forecasting_ptr[sd] * UP_ROTARY_RATE/100)
                	{
                        	up_ok += 1;
                        	printf("sd%d up_spinning_reserve is unenough,load is %f,and spinning reserve is %d\n",sd,totalload_ptr[sd],up_spinning_reserve);

 	                }
		}
		else
		{
                        if(up_spinning_reserve < peak_load && peak_sd == sd)
                        {
                                up_ok += 1;
                                printf("sd%d up_spinning_reserve is unenough,load is %f,and spinning reserve is %d\n",sd,totalload_ptr[sd],up_spinning_reserve);

                        }
                }
*/
                up_spinning_reserve = 0;
        }
	if(up_ok == 0)
	return 1;
	else
	return 0;
}

/*********************************************************************************************
**
**             check down spinning reserve of oper_sd - 1 -- stop_sd sd 
**
*********************************************************************************************/

int check_down_spinning_reserve_2()
{
        int down_ok = 0;
	int rec_sd = 0;
	int rec_down_spinning_reserve = 0;
	float rec_totalload = 0.0;
	int temp = 0;
	int diff = 0;
        int down_spinning_reserve = 0;
	int running_gen_num = find_on_gen_num();
        for(int sd = oper_sd ; sd < stop_sd ; sd ++)
        {
                for(int i = 0 ; i < total_gen ; i ++)
                {
                        if(gen_ptr[i].gen_state[sd] == 1 || gen_ptr[i].gen_state[sd] == 2)
                        down_spinning_reserve = gen_ptr[i].mw_min[sd] + down_spinning_reserve;
                }
                if(down_spinning_reserve > totalload_ptr[sd] - load_forecasting_ptr[sd] * DOWN_ROTARY_RATE/100)
                {
                        down_ok += 1;
                        diff = down_spinning_reserve - totalload_ptr[sd];
                        if(diff > temp)
                        {
                                rec_sd = sd;
                                rec_down_spinning_reserve = down_spinning_reserve;
                                rec_totalload = totalload_ptr[sd];
                                temp = diff;
                        }
                        printf("sd%d down_spinning_reserve is unenough,load is %f,spinning reserve is %d\n",sd,totalload_ptr[sd],down_spinning_reserve);

                }
                down_spinning_reserve = 0;
        }
        if(down_ok == 0)
        return 1;
        else
	{
        	sprintf(check_reserve_info,"受昨天机组状态影响，第%d时段下备用不够，总负荷为%f,机组出力下限为%d",rec_sd,rec_totalload,rec_down_spinning_reserve);
                return 0;
        }
}		


/*********************************************************************************
**
**                  check down spinning reserve of 45,46,47,48  sd 
**
**********************************************************************************/

int check_down_spinning_reserve_3()
{
        int down_ok = 0;
	int lowest_load_sd = find_lowest_totalload_sd();
	printf("lowest_load_sd is %d\n",lowest_load_sd);
        int down_spinning_reserve = 0 ;
	//int running_gen_num = find_on_gen_num();
	//printf("running_gennum is  %d\n",running_gen_num);
	//print_state();
        for(int sd = stop_sd  ; sd < 48 ; sd ++)
        {
                for(int i = 0 ; i < total_gen ; i ++)
                {
                        if(gen_ptr[i].gen_state[sd] == 1 || gen_ptr[i].gen_state[sd] == 2)
			{
                        	down_spinning_reserve = gen_ptr[i].mw_min[sd] + down_spinning_reserve;
				//printf("gen is %s,min is %d,max is %d,down_reserve is %d\n",gen_ptr[i].gen_id,gen_ptr[i].mw_min[47],gen_ptr[i].mw_max[47],down_spinning_reserve);
			}
                }
		printf("sd%d ,load is %f,spinning reserve is %d\n",sd, totalload_ptr[sd],down_spinning_reserve);
                if(down_spinning_reserve > totalload_ptr[total_sd - 1] - load_forecasting_ptr[lowest_load_sd] * LOWEST_LOAD_ROTARY_RATE/100)
                //if(down_spinning_reserve > totalload_ptr[lowest_load_sd] - load_forecasting_ptr[sd] * DOWN_ROTARY_RATE/100)
                {
                        down_ok += 1;
                        printf("sd%d down_spinning_reserve is unenough,load is %f,spinning reserve is %d\n",sd,totalload_ptr[sd],down_spinning_reserve);
                }
/*
		if(lowest_load == 0)
		{
                	if(down_spinning_reserve > totalload_ptr[sd] - load_forecasting_ptr[sd] * DOWN_ROTARY_RATE/100)
                	{
                        	down_ok += 1;
                        	printf("sd%d down_spinning_reserve is unenough,load is %f,spinning reserve is %d\n",sd,totalload_ptr[sd],down_spinning_reserve);

 	                }
		}
		else
		{
			printf(" I am in check_down_spinning_reserve_3 else\n");
                        if(down_spinning_reserve > lowest_load && lowest_load_sd == sd)
                        {
                                down_ok += 1;
                                printf("sd%d down_spinning_reserve is unenough,load is %f,spinning reserve is %d\n",sd,totalload_ptr[sd],down_spinning_reserve);

                        }
                }
*/
                down_spinning_reserve = 0;
        }
        if(down_ok == 0)
        return 1;
        else
        return 0;
}

/*************************************************************************************** 
**
**                 check up spinning reserve of 45,46,47,48  sd 
**
*****************************************************************************************/

int check_up_spinning_reserve_3()
{
        int up_ok = 0;
        int up_spinning_reserve = 0 ;
        for(int sd = stop_sd  ; sd < 48 ; sd ++)
        {
                for(int i = 0 ; i < total_gen ; i ++)
                {
                        if(gen_ptr[i].gen_state[sd] == 1 || gen_ptr[i].gen_state[sd] == 2)
                        up_spinning_reserve = gen_ptr[i].mw_max[sd] + up_spinning_reserve;
                }
                if(up_spinning_reserve < totalload_ptr[sd] + load_forecasting_ptr[sd] * UP_ROTARY_RATE/100)
                {
                        up_ok += 1;
                        printf("sd%d up_spinning_reserve is unenough,load is %f,and spinning reserve is %d\n",sd,totalload_ptr[sd],up_spinning_reserve);

                }
                up_spinning_reserve = 0;
        }
        if(up_ok == 0)
        return 1;
        else
        return 0;
}

/**************************************************************************************
**
**             this function sets generators on later or off later
**             set generator later in order to satisfy the down spinning 
**             resesrve at sd 1--14
**             set generator off in order to satisy the up reserve at 
**             time segments when the load is at peak,at the same time 
**             doing so can decease the number of generators that were 
**             going to operate
**
**************************************************************************************/
//将计划中有启停变化的机组设置为晚开、晚停
void set_generator_on_off_later()
{
	int count = 0;
	printf("now I am in set_generator_on_off_later()!\n");
	for(int i = 0 ; i <= edge_generator ; i ++)    //边际机组以下的机组在计划中开机，如果机组上一个状态为停，则该机组设置为晚开。
	{
		for(int sd = 0 ; sd < oper_sd ; sd ++)//机组在1--14时段如果有必开、必停状态，则不做处理。
		{
			if(gen_ptr[i].gen_state[sd] == 2 || gen_ptr[i].gen_state[sd] == 3)
			count ++;
		}
		if(gen_ptr[i].last_state == 0   && count == 0 ) //
		for(int sd = 0 ; sd < oper_sd ; sd ++)
		{
			if(gen_ptr[i].biding_flag[sd] == 1)
			gen_ptr[i].gen_state[sd] = 0;
		}
		count = 0;
	}

	for(int i = edge_generator ; i < total_gen ; i ++)   //边际机组以上的机组设置为停，如果机组上一个状态为开，则该机组设置为晚停。
	{
		for(int sd = 0 ; sd < total_sd ; sd ++)
		{
			if(gen_ptr[i].gen_state[sd] == 3 || gen_ptr[i].gen_state[sd] == 2)//如果是必开、必停机组，不做处理。
			count ++;
		}
		if((gen_ptr[i].last_state == 1 || gen_ptr[i].last_state == 2) && count == 0)    //||gen_ptr[i].last_state == 2
		for(int sd = 0 ; sd < stop_sd ; sd ++)
		{
			if(gen_ptr[i].biding_flag[sd] == 1)
			gen_ptr[i].gen_state[sd] = 1;
		}
		count = 0;
	}

/*

	int ret = check_down_spinning_reserve_3();
	while(ret == 1 && edge_generator < total_gen)
	{
		if(gen_ptr[edge_generator].last_state == 1)    // || gen_ptr[i].last_state == 2)
		for(int sd = 0 ; sd < total_sd ; sd ++)
		{
                        if(gen_ptr[edge_generator].biding_flag[sd] == 1)
                        gen_ptr[edge_generator].gen_state[sd] = 1;
                }
		ret = check_down_spinning_reserve_3();
		edge_generator ++;
	}
*/

	//print_state();
}


/********************************************************************************************
**
**           the function set mannul generator states
**
********************************************************************************************/
//将调度员手工设置的机组状态指定为必开或必停。
void set_mannul_gen_state()
{
	int peak_sd = find_peak_load_sd();//找到负荷最高时段 。
	//printf("I am here for find_peak_load_sd()\n");
	//printf("peak_sd is %d\n",peak_sd);
	for(int sd = 0 ; sd < total_sd ; sd ++)
	{
		get_mannul_state(year,month,day,sd + 1);
		for(int i = 0 ; i < total_gen ; i ++)
		{
			for(int j = 0 ; j < total_gen ; j ++)
			{
				if(!strcmp(gen_ptr[i].gen_id , mannul_state_ptr[j].gen_id) && mannul_state_ptr[j].state == 2)
				gen_ptr[i].gen_state[sd] = 4;//4表示人工设置的必开，以区别于根据机组最小启停时间计算出的必开2;
				if(!strcmp(gen_ptr[i].gen_id , mannul_state_ptr[j].gen_id) && mannul_state_ptr[j].state == 3)
				gen_ptr[i].gen_state[sd] = 5;//5示人工设置的必停，以区别于根据机组最小启停时间计算出的必停3；
			}
		}
	}
	//printf("I am in the middle of the set_mannul_gen_state()\n");
	for(int i = 0 ; i < total_gen ; i ++)
	{
		//printf(" gen is %s\n",gen_ptr[i].gen_id);
		for(int sd = 0 ; sd < total_sd ; sd ++)
		{
			//printf("sd is %d\n",sd);
			if(gen_ptr[i].gen_state[sd] == 4)
			{
				//printf("gen is %s,zt is %d\n",gen_ptr[i].gen_id,gen_ptr[i].gen_state[sd]);
				for(int sd3 = 0 ; sd3 < sd ; sd3 ++)//将人工指定的必开时段以前的时段设置为必停，人工指定的必开时段以后的时段指定为必开。
				{
					//printf(" sd3 is %d\n",sd3);
					gen_ptr[i].gen_state[sd3] = 3;
				}
				for(int sd1 = sd ; sd1 < total_sd ; sd1 ++)
				gen_ptr[i].gen_state[sd1] = 2;
			}
			if(gen_ptr[i].gen_state[sd] == 5 && sd < peak_sd)//如果人工指定的必停时段小于负荷最高时段，该机组全天设置为停。
			{
				for(int sd2 = 0 ; sd2 < total_sd ; sd2 ++ )
				gen_ptr[i].gen_state[sd2] = 3;
			}
			if(gen_ptr[i].gen_state[sd] == 5 && sd > peak_sd)//如果人工指定的必停时段大于负荷最高时段，该机组从指定时段起设置为必停。
                        {
                                for(int sd2 = sd ; sd2 < total_sd ; sd2 ++ )
                                gen_ptr[i].gen_state[sd2] = 3;
                        }

		}
	}
}

/******************************************************************************************** 
**               
**           the function check the up spinning reserve of the present combination 
**
********************************************************************************************/

int check_up_spinning_reserve()
{
	printf("I am in check_up_spinning_reserve() \n");
	print_state();
        int up_ok = 0;
        int up_spinning_reserve = 0;
        for(int sd = 0 ; sd < stop_sd ; sd ++)
        {
                for(int i = 0 ; i < total_gen ; i ++)
                {
                        if(gen_ptr[i].gen_state[sd] == 1 || gen_ptr[i].gen_state[sd] == 2)
			{
                        	up_spinning_reserve = gen_ptr[i].mw_max[sd] + up_spinning_reserve;
			}
                }
                if(up_spinning_reserve < totalload_ptr[sd] + load_forecasting_ptr[sd] * UP_ROTARY_RATE/100)
                {
			//printf("up_spinning_reserve is %d,load is %d\n",up_spinning_reserve,totalload_ptr[sd] + load_forecasting_ptr[sd] * UP_ROTARY_RATE/100);	
                        up_ok += 1;
                }
                up_spinning_reserve = 0;
        }
        if(up_ok == 0)
	{
	printf("up_ok is %d\n",up_ok);
        return 1;
	}
        else
        return 0;
}

/********************************************************************************************
**
**           the function check the up spinning reserve of the present combination
**
********************************************************************************************/

void check_up_spinning_reserve_for_wys()
{
        printf("I am in check_up_spinning_reserve_for_wys() \n");
        print_state();
        int up_spinning_reserve = 0;
	//fixed_energy = 0;
	ZKTRL = 0;
	float need = 0.0;
        for(int sd = 0 ; sd < stop_sd ; sd ++)
        {
                for(int i = 0 ; i < total_gen ; i ++)
                {
                        if(gen_ptr[i].gen_state[sd] == 1 || gen_ptr[i].gen_state[sd] == 2)
                        {
                                up_spinning_reserve = gen_ptr[i].mw_max[sd] + up_spinning_reserve;
                        }
                }
                if(up_spinning_reserve < totalload_ptr[sd] + load_forecasting_ptr[sd] * UP_ROTARY_RATE/100)
                {
                        //printf("up_spinning_reserve is %d,load is %d\n",up_spinning_reserve,totalload_ptr[sd] + load_forecasting_ptr[sd] * UP_ROTARY_RATE/100);
                        //up_ok += 1;
			//get_fixed_energy(year,month,day,sd+1);
			//printf("fixed_energy is %d\n",fixed_energy);
			get_ZKTRL(year,month,day,sd+1);
			printf("ZKTRLis %d\n",ZKTRL);
			need = totalload_ptr[sd] + load_forecasting_ptr[sd] * UP_ROTARY_RATE/100 - up_spinning_reserve;
			printf("need  is %\f",need);
			sprintf(result_info,"错误");
			sprintf(result_info_explain,"第%d时段上备用不足，缺少%f,基荷电厂正可调容量是%d",sd+1,need,ZKTRL);
			write_execute_info();
                	exit(-1);
                }
                up_spinning_reserve = 0;
        }
}

/**************************************************************************************** 
**
**          the function set generator on  to satify the up sinning reserve 
**
*****************************************************************************************/
//增开机组，以满足负荷上备用的要求
void increase_edge_generator_for_up_spinning_reserve()
{
	int peak_sd = find_peak_load_sd();
	printf("now I am in increase_edge_generator_for_up_spinning_reserve()!\n");
	int ret = check_up_spinning_reserve_2();
	//printf("ret in increase_edge_generator_for_up_spinning_reserve() is %d\n",ret);
	while(ret != 1 && edge_generator < total_gen - 1)
	{
		edge_generator += 1; //set one more generator on;
		//if(gen_ptr[edge_generator].gen_state[oper_sd - 1] == 0 && gen_ptr[edge_generator].gen_state[oper_sd] == 1 && gen_ptr[edge_generator].last_state == 0 || gen_ptr[edge_generator].gen_state[peak_sd] == 3) //later on generator or must off generator , don't deal with it;
		//continue;
		//else
		{
			for(int sd = 0 ; sd < total_sd ; sd ++)
			{
				if(gen_ptr[edge_generator].biding_flag[sd] == 1 && gen_ptr[edge_generator].gen_state[sd] != 3 && gen_ptr[edge_generator].gen_state[sd] != 2)//must on or must off,don't deal with it;
				gen_ptr[edge_generator].gen_state[sd] = 1;
			}
		}
			
		ret = check_up_spinning_reserve_2();
	}

	if(edge_generator >= total_gen && ret == 0 )
	{
		printf("total generators can't satisfy up spinning_reserve");
                sprintf(result_info,"错误7");
		//sprintf(result_info_explain,"负荷过大，所有机组都无法满足上旋转备用");
		sprintf(result_info_explain,"所有可运行机组都无法满足上旋转备用，请检查联络线，基荷，机组报价是否正确，机组最小启停时间是否过大致使没有机组可以启停");
                write_execute_info();
		exit(-1);
	}
	printf("edge generator is %d\t %s\n",edge_generator,gen_ptr[edge_generator].gen_id);
}

/*****************************************************************************************************************
**
**         this function set must on or must off generator on or off later 
**
*****************************************************************************************************************/
//将由于受机组最小启停时间约束而造成的必开必停机组进行晚开、晚停处理。
void set_must_on_off_later()
{ 
	//int peak_sd = find_peak_load_sd();
	for(int i = 0 ; i < total_gen ; i ++)
        {
        	for(int sd = 0 ; sd < total_sd - 1 ; sd ++)
                {
                	if(gen_ptr[i].gen_state[sd] == 2 && gen_ptr[i].gen_state[sd + 1] == 0 && sd < stop_sd)
                        {
                       		for(int j = sd + 1 ; j < stop_sd ; j ++)
                                gen_ptr[i].gen_state[j] = 1;
                        }
                }
        }

        for(int i = 0 ; i < total_gen ; i ++)
        {
	        for(int sd = 0 ; sd < total_sd - 1 ; sd ++)
                {
       		        if(gen_ptr[i].gen_state[sd] == 3 && gen_ptr[i].gen_state[sd + 1] == 1 && sd < oper_sd )
                        {
                	        for(int j = 0 ; j < oper_sd ; j ++)
                                gen_ptr[i].gen_state[j] = 0;
                        }
                }
        }
}

/************************************************************************************************************** 
** 
**     set generator off in order to satisfy down spinning reserve of 45,46,47,48 sd 
**
**************************************************************************************************************/
//没有调用
int set_generator_off_for_down_reserve_3()
{
	printf("now I am in set_generator_off_for_down_reserve_3()!\n");
	int ret = 0,gen_num = total_gen;
	while(gen_num >= edge_generator && ret != 1)
	{
		if(gen_ptr[edge_generator].last_state == 1 && gen_ptr[edge_generator].gen_state[total_sd - 1] == 1 && (22 + compute_must_on_off_sd_num(gen_ptr[edge_generator].last_on_off_time)) > gen_ptr[edge_generator].min_on_time)// 22 means that this gen is on for 22 hours today
                {
			for(int sd = stop_sd ; sd < total_sd ; sd ++)
			gen_ptr[gen_num].gen_state[sd] = 0;
                }
		
		gen_num -= 1;
		ret = check_down_spinning_reserve_3();
	}
	if(ret == 1)
	return 1;
	else
	return 0;
}
/**********************************************************************************************
**
**      check later_on generators to see if it is
**      necessary to set later_on generator on
**
***********************************************************************************************/
//检测计划中的晚开机组，如果不将它开机仍能满足上备用，则可以不启动该机组。
//这样处理的原因是不使系统中的启停机组数过多。
// 没有调用！
int deal_with_later_on_generator()
{
        printf("now I am in deal_with_later_on_generator()!\n");
        int count = 0,ret, ret2,ret1 = check_down_spinning_reserve_3();
        //int peak_sd = find_peak_load_sd();
        int temp = total_gen;
        while(temp >= 0 && ret1 != 1)
        {
                if(gen_ptr[temp].last_state == 0 && gen_ptr[temp].gen_state[total_sd - 1] == 1)//later on generator that can be set off .
                {
			ret = check_gen_mini_num(temp);//to see if setting generator off will violate minimal mode; 
			if(ret == 1)//the generator can be set off;
			{		
                        	for(int sd = oper_sd ; sd < total_sd ; sd ++)
                        	gen_ptr[temp].gen_state[sd] = 0;
			}
			else
			{
				temp -= 1;
				continue;
			}
                }
                ret1 = check_down_spinning_reserve_3();
                ret2 = check_up_spinning_reserve_2();
                if(ret2 == 0) //this later on generator can't be off;
                {
                        ret1 = 0;
                        for(int sd = oper_sd ; sd < total_sd ; sd ++)
                        gen_ptr[temp].gen_state[sd] = 1;
                }
                temp -= 1;
        }
        if( ret1 == 1 && edge_generator >= 0)
        {
                return 1;
        }
        else
        {
                return 0;
        }
}

/********************************************************************************************** 
**
**      check later_on generators to see if it is 
**      necessary to set later_on generator on 
**
***********************************************************************************************/
/*
int deal_with_later_on_generator()
{
	printf("now I am in deal_with_later_on_generator()!\n");
	int ret2,ret1 = check_down_spinning_reserve_3();
	int peak_sd = find_peak_load_sd();
	int temp = edge_generator;
	while(edge_generator >= 0 && ret1 != 1)
	{
		//if(gen_ptr[edge_generator].last_state == 0 && gen_ptr[edge_generator].gen_state[oper_sd] == 1 && gen_ptr[edge_generator].gen_state[oper_sd - 1] == 0 )
		if(gen_ptr[edge_generator].last_state == 0 && gen_ptr[edge_generator].gen_state[peak_sd] == 1)//later on generator.
		{
			for(int sd = oper_sd ; sd < total_sd ; sd ++)
			gen_ptr[edge_generator].gen_state[sd] = 0;
		}
		ret1 = check_down_spinning_reserve_3();
		ret2 = check_up_spinning_reserve_2();
		if(ret2 == 0) //this later on generator can't be off;
		{
			ret1 = 0;
			for(int sd = oper_sd ; sd < total_sd ; sd ++)
                        gen_ptr[edge_generator].gen_state[sd] = 1; 
                }
		edge_generator -= 1;
	}
	if( ret1 == 1 && edge_generator >= 0)
	{
		edge_generator = temp;
		return 1;
	}
	else
	{
                edge_generator = temp;
		return 0;
	}
}
*/
/*************************************************************************************************
** 
**               this function deal with the situation 
**               that the up spinning reserve of sd 1--oper_sd is unenough , 
**               this situation must be caused by too many later on generators 
**
**************************************************************************************************/
//由于所有的新开机组都必须晚开，所以由于实时机组状态造成的1--14时段上备用不够，程序无法解决，提示调度员并退出。
void measure_for_up_spinning_reserve_1()
{
/*
	int i = total_gen - 1 ;
	int ret = check_up_spinning_reserve_1();
	while(i >= 0 && ret != 1)
	{
		if(gen_ptr[i].last_state == 0 && gen_ptr[i].gen_state[oper_sd - 1] == 0 && gen_ptr[i].gen_state[oper_sd] == 1)//later on generator;
		{
			for(int j = 0 ; j < oper_sd ; j ++) //here may be amend in the future,may be just set generator later on more earlier;
			gen_ptr[i].gen_state[j] = 1;
		}
		i -=  1;
		ret = check_up_spinning_reserve_1();
	}
*/
	
	int ret = check_up_spinning_reserve_1();
	if(ret != 1)
	{
		printf("something is wrong of up spinning reserve of sd 1 -- oper_sd\n");
		sprintf(result_info,"错误8");
                sprintf(result_info_explain,"程序无法解决第1到第14时段的上旋转备用问题，产生原因可能是没有报价的机组过多或当天停机过多");
		write_execute_info();
		exit(-1);
	}
}

/**************************************************************************************************** 
**
**            this function deal with the situation 
**            that the up spinning reserve of sd 45--48 is unenough , 
**            this situation must be caused by too many later off generators 
**
*****************************************************************************************************/
//如果45--48时段停机过多，造成上备用不够，这时减少停机，以满足上备用。
void measure_for_up_spinning_reserve_3()
{
	int i = edge_generator - 1;
	int ret = check_up_spinning_reserve_3();
	while( i < total_gen && ret != 1)
	{
		if(gen_ptr[i].last_state == 1 && gen_ptr[i].gen_state[stop_sd - 1] == 1 && gen_ptr[i].gen_state[stop_sd] == 0)//later off generator;
		{
			for(int j = stop_sd ; j < total_sd ; j ++)
			gen_ptr[i].gen_state[j] = 1;
		}
		i += 1;
		ret = check_up_spinning_reserve_3();
	}
	if(i == total_gen && ret != 1)
	{
		printf("somthing is wrong of up spinning reserve of sd 45--48\n");
		sprintf(result_info,"错误9");
		sprintf(result_info_explain,"程序无法解决第45到第48时段的上旋转备用问题，产生原因可能是没有报价的机组太多");
		write_execute_info();
		exit(-1);
	}
}

/********************************************************************************************
**
**            check if the factory's minimal mode is satisfied
**
*********************************************************************************************/

int check_fac_mini_num(int gen_ord,int mini_flag)
{
	int peak_sd = find_peak_load_sd();
	int gen_num = 0;
	if(mini_flag == 0)//不考虑最小方式，直接返回1，表示满足；
	return 1;
/*
	for(int i = 0   ; i < total_gen ; i ++)
	{
		if(!strncmp(gen_ptr[i].fac_id , gen_ptr[gen_ord].fac_id,strlen(gen_ptr[gen_ord].fac_id))  && (gen_ptr[i].gen_state[peak_sd] == 1 || gen_ptr[i].gen_state[peak_sd] == 2))
		gen_num ++;
	}
*/ 
	else//考虑最小方式，统计所有属于该电厂并且在最后一个时段为开机的机组个数，如果大于该电厂最小方式指定台数，返回1。
        for(int i = 0   ; i < total_gen ; i ++)
        {
                if(!strncmp(gen_ptr[i].fac_id , gen_ptr[gen_ord].fac_id,strlen(gen_ptr[gen_ord].fac_id))  && (gen_ptr[i].gen_state[total_sd - 1] == 1 || gen_ptr[i].gen_state[total_sd - 1] == 2))
                gen_num ++;
        }
	for(int j = 0 ; j < mini_mode_fac_num ; j ++)
	{
		if(!strncmp(fac_mini_mode_ptr[j].fac_id,gen_ptr[gen_ord].fac_id,strlen(gen_ptr[gen_ord].fac_id)))
		{
			printf(" I am here for !strncmp(fac_mini_mode_ptr[j].fac_id,gen_ptr[gen_ord].fac_id,strlen(gen_ptr[gen_ord].fac_id))\n");
			if(fac_mini_mode_ptr[j].gen_mini_num <= gen_num)
			{
				printf("on_gen_num is %d\n",gen_num);
				return 1;
			}
			else
			return 0; 
		}
	}
	
	//如果所有电厂检测后函数仍没有返回，说明该机组所属电厂没有指定最小方式，返回1。
	return 1;//indicate that the factory has no minimal mode;
}

/********************************************************************************************
**
**            this function check if the substitution between
**            later on generator and later off generator
**            satisfies the constraints;
**
********************************************************************************************/
//检测能否停掉新开机组，形参 i 是机组序号，mini_flag为0时不考虑电厂最小方式，为1考虑电厂最小方式。
int check_substitute(int i,int mini_flag)
{
	printf("later on gen is %s\n",gen_ptr[i].gen_id);
	int ret1,ret2 ;
	for(int sd = oper_sd ; sd < total_sd ; sd ++)
	{
		gen_ptr[i].gen_state[sd] = 0 ;
	}
	ret1 = check_up_spinning_reserve_2();
	ret2 = check_fac_mini_num(i,mini_flag); 
	printf("ret1 is %d,ret2 is %d\n",ret1,ret2);

	if(ret1 == 1 && ret2 == 1)
	return 1;//the substitution is acceptable;
	else // restore the initial states; 
	{
	        for(int sd = oper_sd ; sd < total_sd ; sd ++)
        	{
                	gen_ptr[i].gen_state[sd] = 1 ;
        	}
		
		return 0;
	}
}

/********************************************************************************************
**
**            this function check minimal mode of the factory, 
**            if yesterday's mode doesn't satisy today's mini_mode, 
**            substitude one later_on generator with other later_on generator
**	      to satisfy the minimal mode of the factoory
**
********************************************************************************************/
//该函数减少一台新开机组，mini_flag将传给它调用的函数check_substitute(i,mini_flag)以决定是否考虑电厂最小方式限制。
int substitute_later_on_off_gen(int mini_flag)
{
	int ret ;
	
	for(int i = total_gen - 1  ; i >= 0 ; i --)
	{
		//if((gen_ptr[i].gen_state[oper_sd - 1] == 0 || gen_ptr[i].gen_state[oper_sd - 1] == 3) && gen_ptr[i].gen_state[oper_sd] == 1 && gen_ptr[i].last_state == 0)//indicates that this is a later on generator;
		if(gen_ptr[i].last_state == 0 && gen_ptr[i].gen_state[total_sd - 1] == 1)//说明这是一台新开机组
		{
 			ret = check_substitute(i,mini_flag);//检测能否将这台机组停掉。
			if (ret == 1)
			return 1;
			else
			continue;
		}
	}	
	return 0;
}
/********************************************************************************************
**
**            this function make the number of later on generators 
**            less than two by substitute later_off generators
**            for later_on generators
**
********************************************************************************************/
//由这个函数完成开机阈值的约束，统计计划中新开机组的个数，如果大于开机阈值，则减少新开机组直到满足开机阈值；
//如果由于受上备用约束，新开机组数必须大于开机阈值，则新开机组数不受开机阈值限制；
//该函数有个形参mini_flag，该参数为0时表示减少新开机组时不考虑电厂最小方式的限制，为1时表示减少新开机组时考虑电厂最小方式的限制；
// yys_order()第一次调用该函数时不考虑电厂最小方式限制，mini_flag == 0。
void decrease_later_on_generator(int mini_flag )
{
	printf("now i am in decrease_later_on_generator()\n");
	int ret, gen_num = 0 ;
	//int num = total_gen - edge_generator;
	int count = 0;
	for(int i = 0 ; i < total_gen ; i ++)
	{
		//如果机组的上一个状态为0，而计划中最后一个时段状态为1或2，该机组为晚开机组。
		if(gen_ptr[i].last_state == 0 && (gen_ptr[i].gen_state[total_sd - 1] == 1 || gen_ptr[i].gen_state[total_sd - 1] == 2))
		gen_num ++;
	}
	printf("gen_num is %d\n",gen_num);
	if(gen_num <= THRESHOLD_ON_GEN)
	return;
	else
	{
		//count记录循环的次数，当一定次数的循环后开机阈值仍没有满足，说明受上备用影响新开机组数不能小于开机阈值，退出循环。
		while(gen_num > THRESHOLD_ON_GEN && count < 100)       // * num)
		{
			ret = substitute_later_on_off_gen(mini_flag);//减少一台新开机组
			printf("ret in de is %d\n",ret);
			if(ret == 1)//该机组可以不开机
			{
				printf("sucess decrease laterongen\n");
				print_state();
				gen_num --;
				count ++;
				printf("later_on gen num is %d\n",gen_num);
			}
			else 
			{
				return;
				//count ++;
				//continue;
			}
		}	
	}
}

//********************************************************************************************/
//由这个函数完成开机阈值的约束，统计计划中新开机组的个数，如果大于开机阈值，则减少新开机组直到满足开机阈值；
//  add by Dingpeng    02/13/2001
void decrease_later_on_unit()
{
        printf("now i am in decrease_later_on_unit()\n");
        int balance = 0, gen_num = 0 ;
        int up_spinning_reserve = 0;
        int peak_sd = find_peak_load_sd();
        for(int i = 0 ; i < total_gen ; i ++)
        {
                if(gen_ptr[i].last_state == 0 && (gen_ptr[i].gen_state[total_sd - 1] == 1 || gen_ptr[i].gen_state[total_sd - 1] == 2)) 
                gen_num ++;
        }
        printf("gen_num is %d\n",gen_num);
        if(gen_num <= THRESHOLD_ON_GEN)
        return;
        else
        {
                for(int i = total_gen - 1  ; i >= 0 ; i --)
                {
                        if(gen_ptr[i].last_state == 0 && gen_ptr[i].gen_state[total_sd - 1] == 1 && gen_num > THRESHOLD_ON_GEN)//说明这是一台新开机组
                        {
                                  for(int sd = oper_sd ; sd < total_sd ; sd ++)
                                  {
                                         gen_ptr[i].gen_state[sd] = 0 ;
                                  }
                                  gen_num --;
                         }
                }
                for(int i = 0 ; i < total_gen ; i ++)
                {
                        if(gen_ptr[i].gen_state[peak_sd] == 1 || gen_ptr[i].gen_state[peak_sd] == 2)
                        up_spinning_reserve = gen_ptr[i].mw_max[peak_sd] + up_spinning_reserve;
                }
                if(up_spinning_reserve >= totalload_ptr[peak_sd] + load_forecasting_ptr[peak_sd] * PEAK_LOAD_ROTARY_RATE/100)
                return;
                else
                {
                       balance = totalload_ptr[peak_sd] + load_forecasting_ptr[peak_sd] * PEAK_LOAD_ROTARY_RATE/100 - up_spinning_reserve;
                       sprintf(result_info,"错误");
                       sprintf(result_info_explain,"由于受开机阈值的限制，尖锋时段(第%d时段)上备用不够，机组出力上限为%d,差额为%d,",peak_sd,up_spinning_reserve,balance);
                       write_execute_info();
                        exit(-1);
                }
         }
}

/********************************************************************************************
**
**            this function decrease generator number to 
**            satisfy the down reserve of sd 1--oper_sd
**
********************************************************************************************/
//没有调用
void decrease_gen_for_down1()
{
	printf("I am in decrease_gen_for_down1()\n");
	int ret1,ret2 ;
	ret1 = check_up_spinning_reserve_2();
	ret2 = check_up_spinning_reserve_3();
	while(ret1 == 1 && ret2 == 1)
	{
		if(gen_ptr[edge_generator].last_state == 0)
		{
			for(int sd = 0 ; sd < total_sd ; sd ++)
			{
				gen_ptr[edge_generator].gen_state[sd] = 0;
			}
		}
		if(gen_ptr[edge_generator].last_state == 1)
		{
			for(int sd1 = stop_sd ; sd1 < total_sd ; sd1 ++)
			{
				gen_ptr[edge_generator].gen_state[sd1] = 0;
			}
		}
		edge_generator --;
		//printf("edge_generator is %d\n",edge_generator);
		//print_state();
		ret1 = check_up_spinning_reserve_2();
		ret2 = check_up_spinning_reserve_3();
	}
	edge_generator ++;
}

/******************************************************************************************
**
**              set one later off generator on
**
******************************************************************************************/
//减少一台晚停机组
int decrease_one_gen()
{
	int count = 0;
	for(int i = 0 ; i < total_gen ; i ++)                 //(i > edge_generator ; i --)
	{
/*
		for(int sd = 0 ; sd < total_sd ; sd ++)
		{
			if(gen_ptr[i].gen_state[sd] == 2)
			count ++;
		}
*/
		
		//printf("I am here\n");
		if(gen_ptr[i].last_state == 1 && gen_ptr[i].gen_state[stop_sd - 1] == 1 && gen_ptr[i].gen_state[stop_sd] == 0)//说明这是一台晚停机组
		{
			//printf("I am in decrease_one_gen\n");
			printf("later_off_gen is %s,min_mw is %d\n",gen_ptr[i].gen_id,gen_ptr[i].mw_min[47]);
			for(int sd = stop_sd ; sd < total_sd ; sd ++)
			{
				gen_ptr[i].gen_state[sd] = 1;
			}
			int ret = check_down_spinning_reserve_3();
			if(ret == 1)//如果不停掉该机组45--48时段下备用仍能满足，则该机组不予停机。
			return 1;
			else//否则恢复该机组的状态。
			for(int sd = stop_sd ; sd < total_sd ; sd ++)
                        {
                                gen_ptr[i].gen_state[sd] = 0;
                        }
			return 0;
		}
	}
	return 0;
}

/********************************************************************************************
**
**             decrease later off gen for down_reserve_1
**
********************************************************************************************/

////////////////////////////////////////////////////////////////////////////
//
//         this function may be some problem
//          it may cause up reserve 2 is unenough
//
////////////////////////////////////////////////////////////////////////////
//检测计划中的晚停机组，如果不将它停机仍能满足45--48时段下备用，则可以不停掉该机组。
//这样处理的原因是不使系统中的启停机组数过多。
void decrease_later_off_gen()
{
	int ret ;
	int later_off_gen = 0;
	ret = check_down_spinning_reserve_3();
	//printf("i am in decrease_later_off_gen\n");
	//printf( "ret is %d\n",ret);
	for(int i = 0 ; i < total_gen ; i ++)//统计晚停机组个数
	{
		if(gen_ptr[i].gen_state[stop_sd - 1] == 1 && gen_ptr[i].gen_state[stop_sd] == 0)
		later_off_gen ++;
	}
	while(ret == 1 && later_off_gen >= 0)
	{
		ret =  decrease_one_gen();
		later_off_gen --;
		//printf( "ret in decrease_later_off_gen is %d\n",ret);
	}
}

/********************************************************************************************
**
**            this function resolve down_reserve_2
**
********************************************************************************************/
//没有调用
void measure_for_down_spinning_reserve_2()
{
	int later_on_gen = 0;
	int peak_sd = find_peak_load_sd();
	printf("I am in measure_for_down_spinning_reserve_2()\n");
	printf("peak_sd is %d\n",peak_sd);
	for(int i = 0 ; i < total_gen ; i ++)
	{
		//if((gen_ptr[i].gen_state[oper_sd - 1] == 0 || gen_ptr[i].gen_state[oper_sd - 1] == 3) && (gen_ptr[i].gen_state[oper_sd] == 1|| gen_ptr[i].gen_state[oper_sd] == 2))
		if(gen_ptr[i].last_state == 0 && gen_ptr[i].gen_state[peak_sd] == 1)//it is a later on generator
		later_on_gen ++;
	}
	//printf("later_on_gen is %d\n",later_on_gen);
	int ret = check_down_spinning_reserve_2();
	while(later_on_gen > 0 && ret != 1) 
	{
		for(int i = 0 ; i < total_gen ; i ++)
		{
			//if((gen_ptr[i].gen_state[oper_sd - 1] == 0 || gen_ptr[i].gen_state[oper_sd - 1] == 3) && gen_ptr[i].gen_state[oper_sd] == 1)
	                if(gen_ptr[i].last_state == 0 && gen_ptr[i].gen_state[peak_sd] == 1)
			{
				//printf("I am in if\n");
				for(int sd = oper_sd ; sd < total_sd ; sd ++)
				gen_ptr[i].gen_state[sd] = 0;
			}
		}
		ret = check_down_spinning_reserve_2();
		later_on_gen --;
	}
	if(later_on_gen < 0 && ret != 1)
	{
		printf(" the operation mode of yesterday doesn't satisfy today's situation!\n");
		set_gen_state(year,month,day);
        	sprintf(result_info,"错误5/6");
        	sprintf(result_info_explain,check_reserve_info);
		exit(-1);
	}
}

/********************************************************************************************
**
**            compute generator's average price of the whole day
**            in order to sort generators by them 
**
********************************************************************************************/
//计算机组的日平均价格，计算方法为将机组各个时段的平均报价累加，除以总时段数。
void compute_aver_price()
{
	int count = 0 ;
	float temp = 0 ;
	for(int i = 0 ; i < total_gen ; i ++)
	{
		if(gen_ptr[i].last_state == 1)//对于前一天状态为1的机组，计算48个时段的平均报价。
		{
			for(int sd = 0 ; sd < total_sd ; sd ++)
			{
				if(gen_ptr[i].biding_flag[sd] == 1)//统计机组报价的时段数
				{
					temp = temp + gen_ptr[i].price_for_order[sd];
					count ++;
				}
			}
			if(count != 0)
			gen_ptr[i].average_price = temp/count;
			else//没有报价，设置价格为100000，使该机组处于机组排队表的最底端。
			{
			printf("gen has no biding is %s\n",gen_ptr[i].gen_id);
			gen_ptr[i].average_price = 100000;
			}
		}
		//如果机组前一天的状态为0，受晚开原则的影响，这台机组只有从15--48时段的报价起作用，所以，只计算15--48 时段的平均报价。
		else if(gen_ptr[i].last_state == 0)
		{
			for(int sd = oper_sd ; sd < total_sd ; sd ++)
			{
		                if(gen_ptr[i].biding_flag[sd] == 1)
                                {
                                        temp = temp + gen_ptr[i].price_for_order[sd];
                                        count ++;
                                }
                        }
                        if(count != 0)
                        gen_ptr[i].average_price = temp/count;
                        else
                        {
                        printf("gen has no biding is %s\n",gen_ptr[i].gen_id);
                        gen_ptr[i].average_price = 100000;
			}
                } 
		else//对于前一天状态为1和0以外的机组，不能启停，价格设置为100000。
		gen_ptr[i].average_price = 100000;

		count = 0 ;
		temp = 0 ;
	} 
	for(int i = 0 ; i < total_gen ; i ++)
	{
		printf("gen is %s,average_price is %f\n",gen_ptr[i].gen_id, gen_ptr[i].average_price);
	}
}

/**********************************************************************************************
**
**                sort the generators by average_price of the whole day
**
**********************************************************************************************/
//根据各个机组的日平均报价将机组排队，所用排队算法为冒泡法。
int sort_by_aver_price()
{

        GENERATOR t;
        //printf("now I am in sort_by_aver_price!\n");

        for(int i = 0 ; i < total_gen ; i ++)
        {
                for(int j = 0 ; j < total_gen - i - 1; j ++)
                {
                        if(gen_ptr[j].average_price > gen_ptr[j + 1].average_price)
                        {
                                t = gen_ptr[j];
                                gen_ptr[j] = gen_ptr[j + 1];
                                gen_ptr[j + 1] = t;
                        }
			//机组平均报价相同，则根据其权重来决定顺序。
			//这里有问题，权重如何维护和修改？
                        //if((gen_ptr[j].average_price == gen_ptr[j + 1].average_price)&&(gen_ptr[j].weight < gen_ptr[j + 1].weight))
			if(gen_ptr[j].average_price == gen_ptr[j + 1].average_price)
                        {
				if(gen_ptr[j].last_state != 1)
				{
                                	t = gen_ptr[j];
                                	gen_ptr[j] = gen_ptr[j + 1];
                                	gen_ptr[j + 1] = t;
				}
                        }
                }
        }

        for(int i = 0 ; i < total_gen ; i ++)//find the edge generator;
        {
                printf(" gen_ptr[%d] is %s,average_price is %f\n",i,gen_ptr[i].gen_id,gen_ptr[i].average_price);
        }

	//根据价格阈值来设置机组的初始状态，平均报价小于价格阈值的机组设置为开，平均报价大于价格阈值的机组设置为停。
        for(int i = 0 ; i < total_gen ; i ++)//find the edge generator;
        {
                if(gen_ptr[i].average_price > PRICE_THRESHOLD )
                {
                        //printf("edge_generator is %d,%s\n",i-1,gen_ptr[i-1].gen_id);
                        //print_state();
			//初始边际机组为根据价格阈值来设置机组的初始状态后，最后开机的一台机组，边际机组用来设置机组的晚开、晚停。
                        return i - 1  ;//the edge generator is the generator that is operating last;
                }
        }
	//如果所有机组的平均报价小于价格阈值，返回一个近似边际机组值。
        return 20;//maybe some problem
}

/********************************************************************************************
**
**            set generators' states according to their average prices
**
*********************************************************************************************/
//根据价格阈值来设置机组的初始状态，平均报价小于价格阈值的机组设置为开，平均报价大于价格阈值的机组设置为停。
void mordify_gen_state_with_aver_price()
{
	for(int i = 0 ; i < total_gen ; i ++)
	{
		if(gen_ptr[i].average_price <= PRICE_THRESHOLD)
		{
			for(int sd = 0 ; sd < total_sd ; sd ++)
			{
				//只处理非必开、必停机组。
				if(gen_ptr[i].gen_state[sd] == 0)//this generator isn't must on or must off
				gen_ptr[i].gen_state[sd] = 1;
			}
		}
		else
		{
			for(int sd = 0 ; sd < total_sd ; sd ++)
			{
				//this generator isn't must on or must off
				if(gen_ptr[i].gen_state[sd] != 2 && gen_ptr[i].gen_state[sd] != 3)
                        	gen_ptr[i].gen_state[sd] = 0;
			}
		}
	}
}

/********************************************************************************************
**
**            deal with factory that doesn't satisfy minimal mode
**            by setting generator later on	
**
********************************************************************************************/
//开机以满足电厂的最小方式。
int increase_later_on_gen_for_mini_mode(int fac_ord,int gen_num)
{
	printf("now I am in increase_later_on_gen_for_mini_mode()\n");

	int ret,count = 0;
	for(int i = 0 ; i < total_gen ; i ++)
	{
		for(int sd1 = oper_sd ; sd1 < total_sd ; sd1 ++)
		{
			if(gen_ptr[i].gen_state[sd1] == 3)
			count ++;
		}
		if(!strncmp(gen_ptr[i].fac_id,fac_mini_mode_ptr[fac_ord].fac_id,strlen(gen_ptr[i].fac_id)) && gen_ptr[i].last_state == 0 && gen_ptr[i].gen_state[total_sd - 1] != 2 && count == 0)//indicate that  this generator can be operatored;
		{
			printf(" gen is %s\n",gen_ptr[i].gen_id);
			for(int sd = oper_sd ; sd < total_sd ; sd ++)
			gen_ptr[i].gen_state[sd] = 1;
			gen_num ++;
			if(gen_num >= fac_mini_mode_ptr[fac_ord].gen_mini_num)
			return 1;
			else
			continue;
		}
	}
	return 0;
}

/********************************************************************************************
**
**            the function check the minimal mode of all the factory
**            if yesterday's mode doesn't satisfy today'mini_mode
**            set generator on to satisfy today'mini_mode
**               
*********************************************************************************************/
//开机以满足电厂的最小方式，调用函数increase_later_on_gen_for_mini_mode(int fac_ord,int gen_num).
void set_gen_later_on_for_mini_mode()
{
	printf("now I am in  set_gen_later_on_for_mini_mode()\n");
	int ret,count = 0;
	//printf("mini_mode_fac_num is %d\n",mini_mode_fac_num);
	if(mini_mode_fac_num == 0)
	return;
	else
	for(int i = 0 ; i < mini_mode_fac_num ; i ++)//统计每个电厂的运行机组数
	{
		for(int j = 0 ; j < total_gen ; j ++)
		{
			printf(" gen_fac is %s,mini_fac is %s\n",gen_ptr[j].fac_id,fac_mini_mode_ptr[i].fac_id);
			if(!strncmp(fac_mini_mode_ptr[i].fac_id,gen_ptr[j].fac_id,strlen(gen_ptr[j].fac_id)) && (gen_ptr[j].last_state == 1 || gen_ptr[j].gen_state[total_sd - 1] == 2 ))
			{
				printf(" gen is %s\n",gen_ptr[j].gen_id);
				count ++;
			}
		}
		printf("fac is %s,running gen num is %d,mini_mode_num is %d\n",fac_mini_mode_ptr[i].fac_id,count,fac_mini_mode_ptr[i].gen_mini_num);
		if(count >= fac_mini_mode_ptr[i].gen_mini_num)
		{
			count = 0; 
			continue;
		}
		else
		{
			//如果该电厂的最小方式没有满足，新开机组以满足最小方式
			ret = increase_later_on_gen_for_mini_mode(i,count);	
			if(ret == 1)
			{
				count = 0;
				continue;
			}
			else//如果该电厂的最小方式无法通过开机来得到满足，程序继续处理其他机组。
			{
				printf("can't resove mini_mode of factory %s\n",fac_mini_mode_ptr[i].fac_name);
				count = 0 ;
				continue;
			}
		} 
	}
}

/******************************************************************************************** 
**
**            this function decide generators'state 
**             with considering  time constraint 
**
*********************************************************************************************/
//该函数为进行有约束机组开停的主要调用函数。
void yys_order()
{
	int ret,ret1 , ret2 , ret3 = 5 , ret4 , ret5 ;

	set_mini_mode_must_on_off();//设置电厂最小方式中指定的必开机组。
	print_state();
	compute_aver_price();//计算日平均价格
	mordify_gen_state_with_aver_price();//根据各个机组的日平均价格来初步设置机组的开停状态
	print_state();
	edge_generator = sort_by_aver_price();//根据价格阈值找到边际机组
        increase_edge_generator_for_up_spinning_reserve();//增开机组以满足上备用
	print_state();
        //int rr = check_spinning_reserve();
        set_generator_on_off_later();//设置有状态变化的机组为晚开、晚停机组。
	print_state();

	check_spinning_reserve();
        set_must_on_off_later();//设置有必开、必停状态的机组的晚开、晚停状态。

	set_gen_later_on_for_mini_mode();//开机以满足电厂的最小方式
	ret1 = check_down_spinning_reserve_1();//检测1--14时段的下备用，如果不满足，说明受前一天的机组状态影响，
						//程序无法找到满足备用约束条件的机组组合，程序退出。
	if(ret1 == 0)
	{
		printf("yesterday's combination doesn't satisfy today's spinning_reserve!\n");
		set_gen_state(year,month,day);
		sprintf(result_info,"错误5/6");
		sprintf(result_info_explain,check_reserve_info);
		//sprintf(result_info_explain,"由于昨天机组状态影响，程序无法找到满足备用约束条件的机组组合，请进行人工调整");
		write_execute_info();
		exit(-1);
	}
	//printf("ret1 is %d\n",ret1); 
	print_state();
	//检测1--14时段的上备用，如果不满足，说明受前一天的机组状态影响，
	//程序无法找到满足备用约束条件的机组组合，程序退出。
	ret = check_up_spinning_reserve_1();
	//printf("chech up 1 is %d\n",ret);
	if(ret != 1)
	measure_for_up_spinning_reserve_1();  //must be caused by later on generator and must off generator;
	printf("after measure_for_up_spinning_reserve_1()\n");
	print_state();
	decrease_later_on_unit();//减少晚开机组数，满足开机阈值，参数0表示减少晚开机组时不考虑电厂的最小方式。
	printf("after decrease_later_on_generator()\n");
	print_state();
	//15--44时段的下备用一般情况下会满足，如过出现不满足的情况，说明开机过多。
        ret = check_down_spinning_reserve_2();
	//printf("ret in check_down_spinning_reserve_2 is %d\n",ret);
        if(ret != 1)
	{
                sprintf(result_info,"错误5/6");
                sprintf(result_info_explain,check_reserve_info);
                exit(-1);
	}
        //measure_for_down_spinning_reserve_2();
	print_state();

	decrease_later_off_gen();//减少晚停机组数
        print_state();

	ret2 = check_down_spinning_reserve_3();//检测45--48时段的下备用
	printf("ret2 of check_down_spinning_reserve_3() is %d\n",ret2);
	if(ret2 == 1)//如果满足下备用
	{
		//printf("all is done!!\n");
		//print_state();
		check_mini_mode();//校验并采取措施使电厂的最小方式满足
		int ret6 = check_spinning_reserve();
		return;
	}
	else
	{
		//ret4 = set_generator_off_for_down_reserve_3();
		ret4 = set_gen_off_for_down_reserve_3();//在45--48时段停机，使下备用满足
		printf(" ret of set_gen_off_for_down_reserve_3() is %d\n",ret4);
		if(ret4 == 1)
		{
			//printf("the reserve is satisfied!!\n");
			//print_state();
			check_mini_mode();
			int r = check_spinning_reserve();
			return ;
		}
		else//如果受最小方式和机组最小启停时间约束，可停机组过少，下备用仍无法满足，程序提示信息并退出。
		{
			printf("the program can't resolve down_reserve_3!!\n");
			sprintf(result_info,"错误10");
			sprintf(result_info_explain,"程序无法解决第45到第48时段的下旋转备用问题，请进行人工调整");
			write_execute_info();
			exit(-1);
		}
/*
			ret5 = deal_with_later_on_generator();
			if(ret5 == 1)
			{
				ret1 = check_up_spinning_reserve_3();
				if(ret1 == 1)
				{
					//printf(" OK!!\n");
					return;
				}
			}
			else
			{
				printf("the program can't resolve down_reserve_3!!\n");
				sprintf(result_info,"错误");
				sprintf(result_info_explain,"程序无法解决第45到第48时段的下旋转备用问题，请进行人工调整");
				write_execute_info();
				exit(-1);
			}
		}
*/
	}
/*
	int up = 0,down = 0;
	for(int sd = 0 ; sd < total_sd ; sd ++)
	{
		for(int i = 0 ; i < total_gen ; i ++)
		{/
			if(gen_ptr[i].gen_state[sd] == 1 )
			{
				up = up + gen_ptr[i].mw_max[sd];
				down = down + gen_ptr[i].mw_min[sd];
			}
		}
		printf(" sd is %d,up is %d,down is %d\n",sd+1,up,down);
		up = 0;
		down = 0;
	}
*/
}


/******************************************************************************************** 
**
**            this function decide generators'state 
**             with considering  time constraint 
**            and mannul adjustment
**
*********************************************************************************************/
//没有调用
void mannul_yys_order()
{
	int ret,ret1 , ret2 , ret3 = 5 , ret4 , ret5 ;

        //mordify_state_with_on_off_rate();
        //edge_generator = sort_on_off_rate();
	compute_aver_price();
	edge_generator = sort_by_aver_price();
        increase_edge_generator_for_up_spinning_reserve();
	print_state();
        int rr = check_spinning_reserve();

        set_generator_on_off_later();
	print_state();
        set_must_on_off_later();
	printf("now i am in mannul_yys_order()\n");
	print_state();

	ret1 = check_down_spinning_reserve_1();
        if(ret1 == 0)// here may be some different from yys_order because of mannul_regulation
        {
                printf("yesterday's combination doesn't satisfy today's spinning_reserve!\n");
                set_gen_state(year,month,day);
                sprintf(result_info,"错误");
                sprintf(result_info_explain,check_reserve_info);
                //sprintf(result_info_explain,"由于昨天机组状态影响，程序无法找到满足备用约束条件的机组组合，请进行人工调整");
                write_execute_info();
                exit(-1);
        }
	printf("ret1 is %d\n",ret1); 

        ret = check_up_spinning_reserve_1();
        //printf("chech up 1 is %d\n",ret);
        if(ret != 1)
        measure_for_up_spinning_reserve_1();  //must be caused by later on generator and must off generator;
        printf("after measure_for_up_spinning_reserve_1()\n");
        print_state();
        decrease_later_on_generator(0);
        printf("after decrease_later_on_generator()\n");
        print_state();

        ret = check_down_spinning_reserve_2();
        //printf("ret in check_down_spinning_reserve_2 is %d\n",ret);
        if(ret != 1)
        measure_for_down_spinning_reserve_2();

        decrease_later_off_gen();
        print_state();

        ret2 = check_down_spinning_reserve_3();
        //printf("ret2 is %d\n",ret2);
        if(ret2 == 1)
        {
                //printf("all is done!!\n");
                //print_state();
                int ret6 = check_spinning_reserve();
                return;
        }
        else
        {
                ret4 = set_generator_off_for_down_reserve_3();
                if(ret4 == 1)
                {
                        //printf("the reserve is satisfied!!\n");
                        //print_state();
                        int r = check_spinning_reserve();
                        return ;
                }
                else
                {
                        ret5 = deal_with_later_on_generator();
                        if(ret5 == 1)
                        {
                                ret1 = check_up_spinning_reserve_3();
                                if(ret1 == 1)
                                {
                                        //printf(" OK!!\n");
                                        return;
                                }
                        }
                        else
			{
				printf("the program can't resolve down_reserve_3!!\n");
				sprintf(result_info,"错误");
				sprintf(result_info_explain,"程序无法解决第45到第48时段的下旋转备用问题，请进行人工调整");
				write_execute_info();
				exit(-1);
			}
		}
	}
}

/*************************************************************************************************************
**
**              this function sorts generators in order to decide 
**              which generator chnge state first acorrding to their comprehensive index
**              come from safe constraint program  
** 
**************************************************************************************************************/
//根据机组的安全约束开停机指标将机组进行排队，算法为冒泡法。
void sort_by_safe_index()
{
        GENERATOR t1;
	//计划中停掉的机组按开停机指标从小到大排队，开机指标最大的最先开机。
        for(int i = edge_generator + 1 ; i < total_gen ; i ++)
        {         
       		for(int j = edge_generator + 1 ; j < total_gen -(i - edge_generator - 1) - 1; j ++)//attention!!!!
                {
                        if(gen_ptr[j].ktzb > gen_ptr[j + 1].ktzb)
                        {
                                t1 = gen_ptr[j];
                                gen_ptr[j] = gen_ptr[j + 1];
                                gen_ptr[j + 1] = t1;
                        }
			if(gen_ptr[j].ktzb == gen_ptr[j + 1].ktzb)
			{
				//开停机指标相同的机组，根据机组的排序号来排队，排序号小的机组平均价格低，应该排在后面以便先开机。
				if(gen_ptr[j].pxh < gen_ptr[j + 1].pxh)// small pxh indicates that this generator is economic,so it is firstly going to run;
				{
					t1 = gen_ptr[j];
                                	gen_ptr[j] = gen_ptr[j + 1];
                                	gen_ptr[j + 1] = t1;
				}
			}
/*
			else
			printf("gen%d is %s,ktzb is %f,gen%d is %s,ktzb is %f\n",j,gen_ptr[j].gen_id,gen_ptr[j].ktzb,j+1,gen_ptr[j + 1].gen_id,gen_ptr[j + 1].ktzb);
*/
                }
        }

        GENERATOR t;
	//计划中运行的机组按开停机指标从小到大排队，停机指标最大的最先停机。
        for(int i = 0 ; i <= edge_generator ; i ++)
        {
                for(int j = 0 ; j < edge_generator - i ; j ++)
                {
                        if(gen_ptr[j].ktzb > gen_ptr[j + 1].ktzb)
                        {
                                t = gen_ptr[j];
                                gen_ptr[j] = gen_ptr[j + 1];
                                gen_ptr[j + 1] = t;
                        }
			//开停机指标相同的机组，根据机组的排序号来排队，排序号大的机组平均价格高，应该排在后面以便先停机。
                        if(gen_ptr[j].ktzb == gen_ptr[j + 1].ktzb)
                        {
                                if(gen_ptr[j].pxh > gen_ptr[j + 1].pxh)//big pxh indicates this generator isn't economic,so it is going to be off firstly;
                                {
                                        t1 = gen_ptr[j];
                                        gen_ptr[j] = gen_ptr[j + 1];
                                        gen_ptr[j + 1] = t1;
                                }
                        }
		}
	}
/*
	GENERATOR t1;
        for(int i = edge_generator + 1 ; i < total_gen ; i ++)
        {
                for(int j = edge_generator + 1  ; j < total_gen  - i - 1; j ++)//it is wrong!!
                {
                        if(gen_ptr[j].ktzb > gen_ptr[j + 1].ktzb)
                        {
                                t1 = gen_ptr[j];
                                gen_ptr[j] = gen_ptr[j + 1];
                                gen_ptr[j + 1] = t1;
                        }
                }
        }
	for(int i = 0 ; i < total_gen ; i ++)
                printf("gen is %s,ktzb is %f,pxh is %d,min is %d,max is %d\n",gen_ptr[i].gen_id,gen_ptr[i].ktzb,gen_ptr[i].pxh,gen_ptr[i].mw_min[0],gen_ptr[i].mw_max[0]);
*/
}

/**************************************************************************************************************
**
**               this function find the edge generator for safe order   
**
**************************************************************************************************************/
//找到计划中最后一台开机的机组，设置为安全边际机组
void find_safe_edge_generator()
{
	GENERATOR t;

	/*************** bubble order by pxh ************************/
/*
	for(int i = 0 ; i < total_gen ; i ++)
	{
		for(int j = 0 ; j < total_gen - i - 1 ; j ++)
		{
			if(gen_ptr[j].pxh > gen_ptr[j + 1].pxh)
			{
				t = gen_ptr[j];
				gen_ptr[j] = gen_ptr[j + 1];
				gen_ptr[j + 1] = t;
			}
		}
	}
	print_state();
*/
	int count = 0;
	for(int i = total_gen - 1  ; i >= 0  ; i --)
	{
		for(int j = 0 ; j < total_sd ; j ++)
		{ 
			if(gen_ptr[i].gen_state[j] == 1 || gen_ptr[i].gen_state[j] == 2)
			{
				count ++;
			}
		}
		if(count != 0)
		{
			edge_generator = i;
        		//printf("edge_generator is %s %d\n",gen_ptr[edge_generator].gen_id,edge_generator);
			return;
		}
	}
}

/**************************************************************************************************************
**
**               this function sort generators according to their states
**               to preparing for finding edge generator
**
***************************************************************************************************************/

void order_for_find_edge_gen()
{
	int count = 0;
	for(int i = 0 ; i < total_gen ; i ++)
	{
		for(int sd = 0 ; sd < total_sd ; sd ++)
		{
			if(gen_ptr[i].gen_state[sd] == 1 || gen_ptr[i].gen_state[sd] == 2)
			{
				count ++;
			}
		}
		gen_ptr[i].on_num = count ;
		count = 0 ;
	}

/////////////////////////// bubble order /////////////////////////////

        GENERATOR t;

        for(int i = 0 ; i < total_gen ; i ++)
        {
                for(int j = 0 ; j < total_gen - i - 1; j ++)
                {
                        if(gen_ptr[j].on_num < gen_ptr[j + 1].on_num)
                        {
                                t = gen_ptr[j];
                                gen_ptr[j] = gen_ptr[j + 1];
                                gen_ptr[j + 1] = t;
                        }
		}	
	}
}

/**************************************************************************************************************
**
**               the function was involved by change_gen_state() to 
**               change generators' state to satisfy the safe constraint
**               and return a value indicating the changing result
**
**************************************************************************************************************/
//根据安全约束指标进行开停机调整，以满足安全约束，形参int * temp_state1，int * temp_state2记录机组的初始状态。
int substitute_for_safe(int * temp_state1,int * temp_state2)
{
	int gen_num_change = 0;
	int must_on_num = 0;
	int must_off_num = 0;
	int ret1,ret2,ret3,ret4,ret5,ret6;
	int peak_sd = find_peak_load_sd();
	for(int i = 0 ; i < total_gen ; i ++)
	{
		//////indicates that there are generators the safe program wants to change their states//////
		if(gen_ptr[i].ktzb != 0)
		{
			gen_num_change ++;
		}
	}
	//没有机组要进行安全约束开停机调整，退出。
	if(gen_num_change == 0)
	{
		printf("there is no generator te be changed state!!\n");
		return 1;
	}
	else
	{
	//先在具有安全约束开停机指标的机组中间进行调整，调整方法为先开一台开机指标最大的机组，检测各个时段的上下备用，如果满足，函数返回，
	//如果不满足，停掉一台停机指标最大的机组，检测各个时段的上下备用，如果满足，函数返回，如果不满足，继续检测其他的机组。
	//////////firstly deal with generators which have ktzb////////////
		for(int i = total_gen - 1  ; i > edge_generator  ; i --)//开机
		{
			if(gen_ptr[i].gen_state[peak_sd] == 3)//如果该机组的必停时段超过了尖峰负荷时段，则不让该机组启动;
			continue;
			else if(gen_ptr[i].ktzb != 0)//先在有开停机指标的机组中进行调整
                        {
				for(int sd = 0 ; sd < total_sd ; sd ++)//记录机组的初始状态
				temp_state1[sd] = gen_ptr[i].gen_state[sd];

				if(gen_ptr[i].last_state == 1)//开机
				{
                                	for(int sd2 = 0 ; sd2 < total_sd ; sd2 ++)
                                        gen_ptr[i].gen_state[sd2] = 1;
                                }
				else if(gen_ptr[i].last_state == 0)  //如果机组前一天的状态为0，机组开机时间必须满足最小停机时间约束
				{
					for(int sd2 = 0 ; sd2 < total_sd ; sd2++)//统计必停时段数
					{
						if(gen_ptr[i].gen_state[sd2] == 3)
						must_off_num ++;
					}
					if(must_off_num != 0)//有必停时段，则从必停时段以后开始开机
					{
						for(int sd2 = 0 ; sd2 < total_sd - 1 ; sd2 ++)
						{
							if(gen_ptr[i].gen_state[sd2] == 3 && gen_ptr[i].gen_state[sd2 + 1] == 0)
							{
								for(int sd = sd2 + 1 ; sd < total_sd ; sd ++)
								gen_ptr[i].gen_state[sd] = 1;
					  		}
						}
						must_off_num = 0;
					}
					else//没有必停时段
					{
						for(int sd2 = oper_sd ; sd2 < total_sd ; sd2 ++)//later on
						gen_ptr[i].gen_state[sd2] = 1; 
					}
				}
                                ret1 = check_up_spinning_reserve_1();
                                ret2 = check_up_spinning_reserve_2();
                                ret3 = check_up_spinning_reserve_3();
                                ret4 = check_down_spinning_reserve_1();
                                ret5 = check_down_spinning_reserve_2();
                                ret6 = check_down_spinning_reserve_3();
                                if(ret1 == 1 && ret2 == 1 && ret3 == 1 && ret4 == 1 && ret5 == 1 && ret6 == 1)
                                return 1;
                        }
					
			for(int j = edge_generator  ; j > 0 ; j --)//停机
			{
				for(int sd = 0 ; sd < total_sd ; sd ++)
				temp_state2[sd] = gen_ptr[j].gen_state[sd];
				if(gen_ptr[j].ktzb != 0)//先在有开停机指标的机组中进行调整
				{
					if(gen_ptr[j].last_state == 0)
					{
						for(int sd3 = 0 ; sd3 < total_sd ; sd3 ++)
						gen_ptr[j].gen_state[sd3] = 0;
					}
					else if(gen_ptr[j].last_state == 1)//如果机组前一天的状态为1，停机时段必须满足机组最小开机时间约束
					{
						for(int sd3 = 0 ; sd3 < total_sd ; sd3 ++)//统计必开时段数
						{
							if(gen_ptr[j].gen_state[sd3] == 2)
							must_on_num ++;
						}
						if(must_on_num != 0)//如果有必开时段，从必开时段后开始停机
						{
							for(int sd3 = 0 ; sd3 < total_sd ; sd3 ++)
							{
								if(gen_ptr[j].gen_state[sd3] == 2 && gen_ptr[j].gen_state[sd3 + 1] == 1 && sd3 < gen_ptr[j].check_safe_sd)
								{
									for(int sd3 = gen_ptr[j].check_safe_sd ; sd3 < total_sd ; sd3 ++)
									gen_ptr[j].gen_state[sd3] = 0;
								}
							}
							must_on_num = 0;
						}
						else
						{
							for(int sd4 = gen_ptr[j].check_safe_sd-1 ; sd4 < total_sd ; sd4 ++)
							gen_ptr[j].gen_state[sd4] = 0;
						}
					}
					ret1 = check_up_spinning_reserve_1();
					ret2 = check_up_spinning_reserve_2();
					ret3 = check_up_spinning_reserve_3();
					ret4 = check_down_spinning_reserve_1();
					ret5 = check_down_spinning_reserve_2();
					ret6 = check_down_spinning_reserve_3();
					if(ret1 == 1 && ret2 == 1 && ret3 == 1 && ret4 == 1 && ret5 == 1 && ret6 == 1)
					return 1;
					else//if the reserve isn't satisfied, restore the initial state;
					{
						for(int sd = 0 ; sd < total_sd ; sd ++)
						gen_ptr[j].gen_state[sd] = temp_state2[sd];
					}
				}
			}
			for(int sd = 0 ; sd < total_sd ; sd ++) //if the reserve isn't satisfied , restore the initial state;
			{
				gen_ptr[i].gen_state[sd] = temp_state1[sd];
			}
		}
		printf("if only dealing  with generators having ktzb can't resolve problem then deal with all generaors\n");

		//如果函数处理完有开停机指标的机组后仍没有返回，则必须在所有的机组中进行开停机调整，调整方法与上相同。
                /////////if only dealing  with generators having ktzb can't resolve problem then deal with all generaors///////// 
                for(int i = total_gen - 1  ; i > edge_generator  ; i --)
                {
                        if(gen_ptr[i].gen_state[peak_sd] == 3)//this generator is must off till peak_sd , don't deal with it;
                        continue;
                        else 
                        {
                                for(int sd = 0 ; sd < total_sd ; sd ++)
                                temp_state1[sd] = gen_ptr[i].gen_state[sd];

                                if(gen_ptr[i].last_state == 1)
                                {
                                        for(int sd2 = 0 ; sd2 < total_sd ; sd2 ++)
                                        gen_ptr[i].gen_state[sd2] = 1;
                                }
                                else if(gen_ptr[i].last_state == 0) //deal with time constraint
                                {
                                        for(int sd2 = 0 ; sd2 < total_sd ; sd2++)
                                        {
                                                if(gen_ptr[i].gen_state[sd2] == 3)
                                                must_off_num ++;
                                        }
                                        if(must_off_num != 0)
                                        {
                                                for(int sd2 = 0 ; sd2 < total_sd - 1 ; sd2 ++)
                                                {
                                                        if(gen_ptr[i].gen_state[sd2] == 3 && gen_ptr[i].gen_state[sd2 + 1] == 0)
                                                        {
                                                                for(int sd = sd2 + 1 ; sd < total_sd ; sd ++)
                                                                gen_ptr[i].gen_state[sd] = 1;
                                                        }
                                                }
                                                must_off_num = 0;
                                        }
                                        else
                                        {
                                                for(int sd2 = oper_sd ; sd2 < total_sd ; sd2 ++)//later on
                                                gen_ptr[i].gen_state[sd2] = 1;
                                        }
                                }
                                ret1 = check_up_spinning_reserve_1();
                                ret2 = check_up_spinning_reserve_2();
                                ret3 = check_up_spinning_reserve_3();
                                ret4 = check_down_spinning_reserve_1();
                                ret5 = check_down_spinning_reserve_2();
                                ret6 = check_down_spinning_reserve_3();
                                if(ret1 == 1 && ret2 == 1 && ret3 == 1 && ret4 == 1 && ret5 == 1 && ret6 == 1)
                                return 1;
                        }
                        for(int j = edge_generator  ; j > 0 ; j --)
                        {
                                for(int sd = 0 ; sd < total_sd ; sd ++)
                                temp_state2[sd] = gen_ptr[j].gen_state[sd];
                                if(gen_ptr[j].last_state == 0)
                                {
                                        for(int sd3 = 0 ; sd3 < total_sd ; sd3 ++)
                                        gen_ptr[j].gen_state[sd3] = 0;
                                }
                                else if(gen_ptr[j].last_state == 1)//deal with time constraint
                                {
                                       for(int sd3 = 0 ; sd3 < total_sd ; sd3 ++)
                                       {
                                               if(gen_ptr[j].gen_state[sd3] == 2)
                                               must_on_num ++;
                                       }
                                       if(must_on_num != 0)
                                       {
                                               for(int sd3 = 0 ; sd3 < total_sd ; sd3 ++)
                                               {
                               	                       if(gen_ptr[j].gen_state[sd3] == 2 && gen_ptr[j].gen_state[sd3 + 1] == 1 && sd3 < gen_ptr[j].check_safe_sd)
                                                       {
                                                               for(int sd3 = gen_ptr[j].check_safe_sd ; sd3 < total_sd ; sd3 ++)
                                                               gen_ptr[j].gen_state[sd3] = 0;
                                                       }
                                               }
                                               must_on_num = 0;
                                       }
                                       else
                                       {
                                               for(int sd4 = gen_ptr[j].check_safe_sd-1 ; sd4 < total_sd ; sd4 ++)
                                               gen_ptr[j].gen_state[sd4] = 0;
                                       }
                                }
                                ret1 = check_up_spinning_reserve_1();
                                ret2 = check_up_spinning_reserve_2();
                                ret3 = check_up_spinning_reserve_3();
                                ret4 = check_down_spinning_reserve_1();
                                ret5 = check_down_spinning_reserve_2();
                                ret6 = check_down_spinning_reserve_3();
                                if(ret1 == 1 && ret2 == 1 && ret3 == 1 && ret4 == 1 && ret5 == 1 && ret6 == 1)
                                return 1;
                                else//if the reserve isn't satisfied, restore the initial state;
                                {
                                       for(int sd = 0 ; sd < total_sd ; sd ++)
                                       gen_ptr[j].gen_state[sd] = temp_state2[sd];
                                }
			}
                        for(int sd = 0 ; sd < total_sd ; sd ++) //if the reserve isn't satisfied , restore the initial state;
                        {
                                gen_ptr[i].gen_state[sd] = temp_state1[sd];
                        }
		}
	}
	//处理完所有机组后没有返回，说明找不到合适的机组组合满足各种约束，返回2表明处理失败。
	return 2;//can't find suitable combination to satisfy both safe constraint and time,reserve constraint;
}

/**************************************************************************************************************     
**
**               this funstion changes generators'states as the safe constraint 
**               program wants to satisfy the safe constraint
**
**************************************************************************************************************/
//该函数调用substitute_for_safe(temp_state1,temp_state2)进行机组状态调整。
void change_gen_state()
{
	int ret ;
        int * temp_state1;//data struct to record the states of generator;
        int * temp_state2;//data struct to record the states of generator;
        temp_state1 = new int[total_sd];
        temp_state2 = new int[total_sd];
	ret = substitute_for_safe(temp_state1,temp_state2);
	//delete[] temp_state1;
	//delete[] temp_state2;
/*
	printf("ret in change_gen is %d\n",ret);
	if(ret == 1)
	return;
	while((ret != 1) && (ret != 2))
	{
		ret = substitute_for_safe(temp_state1,temp_state2);
		//printf("ret in change_gen is %d\n",ret);
	}
*/
	if(ret == 2)
	{
		printf("the program can't resolve safe constraint problem!!");
		sprintf(result_info,"错误11");
		sprintf(result_info_explain,"由于无法同时满足尖峰上备用和低谷下备用，程序找不到满足安全约束的机组组合，请人工调整");
        	delete[] temp_state1;
        	delete[] temp_state2;
		write_execute_info();
		exit(-1);
	}
	else
	{
	        delete[] temp_state1;
        	delete[] temp_state2;
		return;
	}
		
}

/**************************************************************************************************************
**
**               this function recombinate generators in order to
**               satisfy the safe constraint
**
**************************************************************************************************************/
//该函数为进行安全约束开停机的主函数。
void recombinate_for_safe()
{
	printf("I am in recombinate_for_safe()\n");
	for(int i = 0 ; i < total_gen ; i ++)
	{
		gen_ptr[i].ktzb = 0.0;
	}
	for(int i = 0 ; i < total_gen ; i ++)
	{
		for(int j = 0 ; j < safe_index_gen_num ; j ++)
		{
			if(!strncmp(gen_ptr[i].gen_id,safe_index_ptr[j].gen_id,strlen(safe_index_ptr[j].gen_id)))
			{
				gen_ptr[i].ktzb = safe_index_ptr[j].ktzb;
				gen_ptr[i].check_safe_sd = safe_index_ptr[j].sd;
				//printf("gen is %s,ktzb is %f,sd is %d\n",gen_ptr[i].gen_id,gen_ptr[i].ktzb,gen_ptr[i].check_safe_sd);
			}
		}
	}
	
	//order_for_find_edge_gen(); 	
	find_safe_edge_generator();
	sort_by_safe_index();
	//print_state();
	change_gen_state();
}

/**************************************************************************************************************
**
**               this function sort the generators in order to
**               get the economic order for dynamic program;
**
**************************************************************************************************************/

void order_for_dym()
{
        int count = 0;
        for(int i = 0 ; i < total_gen ; i ++)
        {
                for(int sd = 0 ; sd < total_sd ; sd ++)
                {
                        if(gen_ptr[i].gen_state[sd] == 1 || gen_ptr[i].gen_state[sd] == 2)
                        {
                                count ++;
                        }
                }
                gen_ptr[i].on_num = count ;
                count = 0 ;
		if((gen_ptr[i].gen_state[stop_sd - 1] == 1 || gen_ptr[i].gen_state[stop_sd - 1] ==2 ) && gen_ptr[i].gen_state[stop_sd] == 0)
		{
			gen_ptr[i].on_num = 1;//may be some problem;
		}
        }

/////////////////////////// bubble order /////////////////////////////

        GENERATOR t;

        for(int i = 0 ; i < total_gen ; i ++)
        {
                for(int j = 0 ; j < total_gen - i - 1; j ++)
                {
                        if(gen_ptr[j].on_num < gen_ptr[j + 1].on_num)
                        {
                                t = gen_ptr[j];
                                gen_ptr[j] = gen_ptr[j + 1];
                                gen_ptr[j + 1] = t;
                        }
                }
        }
		
}

/***********************************************************************************************************
**
**               this  function changes generators' states for save
**
************************************************************************************************************/
//将机组的状态变成0/1表示。
void change_state_for_save()
{
	for(int i = 0 ; i < total_gen ; i ++)
	{
		for(int sd = 0 ; sd  < total_sd ; sd ++)
		{
			if(gen_ptr[i].gen_state[sd] == 2)
			gen_ptr[i].gen_state[sd] = 1;
			if(gen_ptr[i].gen_state[sd] == 3)
			gen_ptr[i].gen_state[sd] = 0;
		}
	}
}

/**************************************************************************************************************
**
**               find which generators are on or off
**               and write these information into datebase
**
***************************************************************************************************************/
//统计机组的开停机情况，将竞价空间，第48时段的所有机组的出力下限写入数据库。
void gen_on_off_info()
{
	char temp[1000];
	//bzero(result_info_explain,252);
	for(int i = 0 ; i < total_gen ; i ++)
	{
		for(int j = 0 ; j < total_sd - 1; j ++)
		{
			if(gen_ptr[i].gen_state[j] == 0 && gen_ptr[i].gen_state[j + 1] == 1)
			{
				if(!strcmp(result_info_explain,""))
                                {
                                        sprintf(result_info_explain,"%s",gen_ptr[i].gen_name);
                                        sprintf(temp,"在第%d时段开机",j+2);
                                        strcat(result_info_explain,temp);
                                }
				else
				{
                                        sprintf(temp,",%s",gen_ptr[i].gen_name);
                                        strcat(result_info_explain,temp);
                                        sprintf(temp,"在第%d时段开机",j+2);
                                        strcat(result_info_explain,temp);
				}
				printf("on gen is %s",gen_ptr[i].gen_id);
			}
		}
	}
        for(int i = 0 ; i < total_gen  ; i ++)
        {
                for(int j = 0 ; j < total_sd - 1; j ++)
                {
                        if(gen_ptr[i].gen_state[j] == 1 && gen_ptr[i].gen_state[j + 1] == 0)
                        {
                                if(!strcmp(result_info_explain,""))
				{
                                        sprintf(result_info_explain,"%s",gen_ptr[i].gen_name);
                                        sprintf(temp,"在第%d时段停机",j+2);
                                        strcat(result_info_explain,temp);
				}
				else
				{
                                        sprintf(temp,",%s",gen_ptr[i].gen_name);
                                        strcat(result_info_explain,temp);
                                        sprintf(temp,"在第%d时段停机",j+2);
                                        strcat(result_info_explain,temp);
				}
				printf("off gen is %s",gen_ptr[i].gen_id);
                        }
                }
        }
	if(!strcmp(result_info_explain,""))
	{
	//sprintf(result_info_explain,"竞价空间是%f,机组出力下限是%d",totalload_ptr[total_sd - 1],sum_low_limit);
		sprintf(result_info_explain,"竞价空间是%f,",totalload_ptr[total_sd - 1]);
		//sprintf(temp,"机组出力下限是%d",sum_low_limit
		//sprintf(temp,"机组出力下限是%d",sum_low_limit);

		//strcat(result_info_explain,temp);

	}
	else
	{
		//sprintf(temp,",竞价空间是%f,机组出力下限是%d",totalload_ptr[total_sd - 1],sum_low_limit);
		sprintf(temp,",竞价空间是%f,",totalload_ptr[total_sd - 1]);
		strcat(result_info_explain,temp);
		//sprintf(temp,"机组出力下限是%d",sum_low_limit);strcat(result_info_explain,temp);
	}	
	int len = strlen(result_info_explain);
	//printf("len is %d\n",len);
}

/*********************************************************************************************************
              计算所有机组的出力下限和
*********************************************************************************************************/

int compute_gen_min_mw_sum()
{
	int sum = 0;
	for(int i = 0; i < total_gen ; i ++)
	{
		if(gen_ptr[i].gen_state[total_sd - 1] == 1 || gen_ptr[i].gen_state[total_sd - 1] == 2)
		sum = sum + gen_ptr[i].mw_min[total_sd - 1];
	}
	printf("sum is %d\n",sum);
	return sum;

}

/**************************************************************************************************************
**
**               this is the main function that can excute for
**               generator combination without constraint or with 
**               load,reserve,time,safe constraint.
**
****************************************************************************************************************/

void main(int argc,char ** argv)
{
	int ret;
        result_correct = 1;
        get_date_and_time();
	sprintf(program_id,"pdjzktj");
	sprintf(program_name,"排队机组开停机");
        long cputime;
        clock();
	//printf("The program is beginning !!\n");

/************ from now to caculate generator combination without constraint *****************/

        if(argc == 3)
        {
                set_opp_rela();
		clear_execute_info();
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

                strcpy(constrain_flag,*(argv+2));
                if(strcmp(constrain_flag,"wys") )
                {
                        printf("please input the correct parameter.");
              	        sprintf(result_info,"错误");
			sprintf(result_info_explain,"无约束排队程序有两个参数，第一个为日期参数mm/dd/yyyy,第二个为wys,表示无约束排队，参数间用空格隔开");
			write_execute_info();
                        exit(-1);
                }
		
		strcpy(user_id,"");
		strcpy(user_name,"");

                //fp = fopen("my.out","w+");
                if(!strcmp(constrain_flag,"wys"))
                {
                        preparing_data();
			set_real_time_state();
			set_gen_mw_scape();

                        for(int i = 0 ; i < total_sd ; i ++)
                        {
                                bubble_order(i);
                                wys_order(i);
				modify_weight(i);
                        }
                        check_up_spinning_reserve_for_wys();
			/*
                        printf("ret = check_up_spinning_reserve() i s %d\n",ret);
			int fixed_energy = 0;
                        if(ret == 0)
                        {
                                sprintf(result_info,"错误");
				fixed_energy = get_fixed_energy(year,month,day,);
                        }
			*/
                        //print_state();
			printf("is %f\n",WYS_DOWN_ROTARY_RATE);
			printf("is %f\n",WYS_UP_ROTARY_RATE);   
			printf("upr is %f\n",UP_ROTARY_RATE);

                        free_new_memory();
                }
	}

/**************** from now to caculate generator combination with constraint ******************/

        else if(argc == 4)
        {
	        set_opp_rela();
		clear_execute_info();
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

        	strcpy(constrain_flag,*(argv+2));
        	if( strcmp(constrain_flag,"yys"))
        	{
        	        printf("please input the correct parameter.");
			sprintf(result_info,"错误");
			sprintf(result_info_explain,"约束类型必须为yys");
			//write_execute_info();
        	        exit(-1);
        	}
		
		mannul_flag = atoi(*(argv + 3));
		strcpy(user_id,"");
		strcpy(user_name,"");

 	        //fp = fopen("my.out","w+");
		
		if(mannul_flag == 1)//deal with mannul modified situation;
		{
		        preparing_data();
/*
                        for(int i = 0 ; i < total_sd ; i ++)
                        {
                                bubble_order(i);
                                wys_order(i);
                        }
*/
                        set_real_time_state();//get generators'state at present;
			set_gen_mw_scape();
                        find_must_on_off_sd();
			print_state();
			get_gen_state_for_mannul();//get yys generators'states form T_YYSJZKTJ;
			print_state();

			yys_order();

			//printf("after yys_order\n");
			//print_state();
                        change_state_for_save();
			order_for_dym();
                        //set_gen_state(year,month,day);//write to the database;
			print_state();


                        free_new_memory();
			free_mannul_state_ptr();
		}

		else if(mannul_flag == 2)       //order for safe constraint
		{
                        preparing_data();
			get_gen_safe_index(year,month,day);
/*
                        for(int i = 0 ; i < total_sd ; i ++)
                        {
                                bubble_order(i);
                                wys_order(i);
                        }
*/
                        set_real_time_state();//get generators'state at present;
                        set_gen_mw_scape();
			//print_mw();
                        find_must_on_off_sd();
			//print_mw();
                        get_gen_state_for_safe();//get yys generators'states form T_YYSJZKTJ;
			//print_mw();
		        order_for_dym();	
			//print_mw();
			//print_state();
			recombinate_for_safe();
			//print_mw();
                        //set_gen_state(year,month,day);//write to the database;
			print_state();
			gen_on_off_info();
			free_gen_state_struct();
			free_gen_safe_index_ptr();
			free_new_memory();
		}

		else if(mannul_flag == 0)
		{
                        preparing_data();
/*
                        for(int i = 0 ; i < total_sd ; i ++)
                        {
                                bubble_order(i);
                                wys_order(i);
                        }
*/
                        set_real_time_state();//get generators'state at present;
                        set_gen_mw_scape();
			for(int i = 0 ; i < total_gen ; i ++)
			{
				printf("gen is %s,mw_max is %d\n",gen_ptr[i].gen_id,gen_ptr[i].mw_max[13]);
			}

                        find_must_on_off_sd();
			print_state();
                        yys_order();
			change_state_for_save();
			order_for_dym();
                        //set_gen_state(year,month,day);//write to the database;
			print_state();

        int up = 0,down = 0;
	for(int i = 0 ; i < total_gen ; i ++)
	{
		if (gen_ptr[i].gen_state[total_sd - 1] == 1)
		{
			printf("gen is %s,min is %d,max is %d\n",gen_ptr[i].gen_id,gen_ptr[i].mw_min[total_sd - 1],gen_ptr[i].mw_max[total_sd - 1]);
			down = down + gen_ptr[i].mw_min[total_sd - 1];
		}
	}
	printf("down is %d\n",down);
	sum_low_limit = down;
	printf("sum_low_limit is %d\n",down);

/*
	down = 0;
        for(int sd = 0 ; sd < total_sd ; sd ++)
        {
                for(int i = 0 ; i < total_gen ; i ++)
                {
                        if(gen_ptr[i].gen_state[sd] == 1 )
                        {
                                up = up + gen_ptr[i].mw_max[sd];
                                down = down + gen_ptr[i].mw_min[sd];
                        }
                }
                printf(" sd is %d,up is %d,down is %d\n",sd+1,up,down);
                up = 0;
                down = 0;
        }
*/

                        sprintf(result_info,"正确");
                        sprintf(result_info_explain,"");
                        print_state();
                        gen_on_off_info();
                        printf("info is %s\n",result_info_explain);
			printf("before mini_mode_info();\n");
			mini_mode_info();
			printf("info is %s\n",result_info_explain);
                        //write_execute_info();
                                                                    
                        free_new_memory();

		}
		else
		{
			printf("please input correct parameter!!\n");
			sprintf(result_info,"错误");
                        sprintf(result_info_explain,"有约束程序中，0代表不考虑手工调整，1代表考虑手工调整，2代表考虑安全约束");
			write_execute_info();
			exit(-1);
		}

	        //fclose(fp);
        }

        else if(argc == 5)
        {
                set_opp_rela();//读数据库配置文件，建立与数据库的连接。
		clear_execute_info();//将数据库中的程序执行信息清空
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

                strcpy(constrain_flag,*(argv+2));
                if(strcmp(constrain_flag,"wys") )
                {
                        printf("please input the correct parameter.");
              	        sprintf(result_info,"错误13");
			sprintf(result_info_explain,"无约束排队程序有4个参数，第一个为日期参数mm/dd/yyyy,第二个为wys,表示无约束排队，参数间用空格隔开，第三个参数为用户ID,第四个参数为用户名称");
			write_execute_info();
                        exit(-1);
                }
		
		strcpy(user_id,*(argv + 3));
		strcpy(user_name,*(argv + 4));

                //fp = fopen("my.out","w+");
                if(!strcmp(constrain_flag,"wys"))
                {
                        preparing_data();

			set_real_time_state();
			set_gen_mw_scape();

                        for(int i = 0 ; i < total_sd ; i ++)
                        {
                                bubble_order(i);
                                wys_order(i);
                                modify_weight(i);
                        }
                        check_up_spinning_reserve_for_wys();
/*
                        ret = check_up_spinning_reserve();
                        if(ret == 0)
                        {
                                sprintf(result_info,"错误12");
				//sprintf(result_info_explain,"程序无法解决上旋转备用问题,或所有机组出力上限和小于负荷，产生原因可能是没有报价的机组过多");
				sprintf(result_info_explain,"所有机组都无法满足上旋转备用，请检查联络线，基荷，机组报价是否正确");
				write_execute_info();
                                exit(-1);
                        }
*/
                        set_gen_state(year,month,day);//write to the database;
                        printf("afterset_gen_state\n");
                        print_state();
                        set_gen_weight();//this function is only used in wys_situation;
                        sprintf(result_info,"正确");
                        sprintf(result_info_explain,"");
                        write_execute_info();
                        free_new_memory();
                }
                //fclose(fp);
	}

        else if(argc == 6)
        {
	        set_opp_rela();
		clear_execute_info();
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

        	strcpy(constrain_flag,*(argv+2));
        	if( strcmp(constrain_flag,"yys"))
        	{
        	        printf("please input the correct parameter.");
			sprintf(result_info,"错误13");
			sprintf(result_info_explain,"约束类型必须为yys");
			write_execute_info();
        	        exit(-1);
        	}
		
		mannul_flag = atoi(*(argv + 3));
		strcpy(user_id,*(argv + 4));
		strcpy(user_name,*(argv + 5));

 	        //fp = fopen("my.out","w+");
		
		if(mannul_flag == 1)//deal with mannul modified situation;
		{
		        preparing_data();
/*
                        for(int i = 0 ; i < total_sd ; i ++)
                        {
                                bubble_order(i);
                                wys_order(i);
                        }
*/
                        set_real_time_state();//get generators'state at present;
                        set_gen_mw_scape();
                        find_must_on_off_sd();
			print_state();
			get_gen_state_for_mannul();//get yys generators'states form T_YYSJZKTJ;

			yys_order();

			//printf("after yys_order\n");
			//print_state();
                        change_state_for_save();
			order_for_dym();
                        set_gen_state(year,month,day);//write to the database;

                        sprintf(result_info,"正确");
                        sprintf(result_info_explain,"");
                        //print_state();
                        gen_on_off_info();
                        printf("info is %s\n",result_info_explain);
                        sum_low_limit = compute_gen_min_mw_sum();
                        char temp[100];
                        sprintf(temp,"机组出力下限是%d",sum_low_limit);strcat(result_info_explain,temp);
			mini_mode_info();
			printf("info is %s\n",result_info_explain);
                        write_execute_info();

                        free_new_memory();
			free_mannul_state_ptr();
		}

		else if(mannul_flag == 2)       //order for safe constraint
		{
                        preparing_data();
			get_gen_safe_index(year,month,day);
                        set_real_time_state();//get generators'state at present;
			set_gen_mw_scape();
                        find_must_on_off_sd();
                        get_gen_state_for_safe();//get yys generators'states form T_YYSJZKTJ;

			order_for_dym();
			print_state();
			recombinate_for_safe();
			//print_state();
                        change_state_for_save();
			//print_state();
                        order_for_dym();
                        set_gen_state(year,month,day);//write to the database;
			print_state();

                        sprintf(result_info,"正确");
                        sprintf(result_info_explain,"");
                        //print_state();
                        gen_on_off_info();
                        printf("info is %s\n",result_info_explain);
                        write_execute_info();

			free_gen_state_struct();
			free_gen_safe_index_ptr();
			free_new_memory();
		}

		else if(mannul_flag == 0)
		{
                        preparing_data();
			get_gen_safe_index(year,month,day);
/*
                        for(int i = 0 ; i < total_sd ; i ++)
                        {
                                bubble_order(i);
                                wys_order(i);
                        }
*/
                        set_real_time_state();//get generators'state at present;
			set_gen_mw_scape();
                        find_must_on_off_sd();
                        yys_order();
			//set_aver_price(year);
			change_state_for_save();
			order_for_dym();
                        set_gen_state(year,month,day);//write to the database;
			print_state();
        		sprintf(result_info,"正确");
        		sprintf(result_info_explain,"");
        		print_state();
                	gen_on_off_info();
                	printf("info is %s\n",result_info_explain);
			sum_low_limit = compute_gen_min_mw_sum();
			char temp[100];
			sprintf(temp,"机组出力下限是%d",sum_low_limit);strcat(result_info_explain,temp);		
			mini_mode_info();
			printf("info is %s\n",result_info_explain);
                	write_execute_info();

                        free_new_memory();

		}
		else
		{
			printf("please input correct parameter!!\n");
			sprintf(result_info,"错误13");
                        sprintf(result_info_explain,"有约束程序中，0代表不考虑手工调整，1代表考虑手工调整，2代表考虑安全约束");
			write_execute_info();
			exit(-1);
		}

	        //fclose(fp);
        }
/************************ somthing is wrong ***********************/

        else
        {
                set_opp_rela();
		clear_execute_info();
                printf("the number of parameter is wrong! please input correct parameter!!\n");
		sprintf(result_info,"错误13");
		sprintf(result_info_explain,"参数个数必须为5或6");
		write_execute_info();
                exit(-1);
        }
        cputime = clock();
/*
	sprintf(result_info,"正确");
	sprintf(result_info_explain,"");
	printf("ksjhdfkash\n");	
	print_state();
	if(!strcmp(constrain_flag,"yys"))
	{
		print_state();
		gen_on_off_info();
		printf("info is %s\n",result_info_explain);
		write_execute_info();
	}
	else
	write_execute_info();
*/
        CS_RETCODE  rcode=0;
        rcode = close_connect_with_server();
        if(rcode!=CS_SUCCEED)
        {
                printf("database close failed!!\n");
                sprintf(result_info,"错误1");
                sprintf(result_info_explain,"数据库关闭失败");
                write_execute_info();
                exit(-1);
         }

	printf("The program ended successfully !!\n");
        printf("it took %.2fsecs.\n",cputime/1.0e+6);
}
