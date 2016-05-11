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
//�ú������������һ��ʱ�ε�ƽ�����ۣ����õķ����Ǽ��㱨�����߸��ǵ���������Ի��������Χ��
float tran_price(int i)
{
	float temprice = 0.0;
	for(int j = 0 ; j < icc_num[i] - 1 ; j ++)//����i�ı��۵�j
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
//����������������ޣ�������Χ�������ݿ���ȡ����
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
//�������ڵڶ���ǰ��״̬���䷢��ʱ���¼������
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
                	sprintf(result_info,"����");
                        sprintf(result_info_explain,"ϵͳ��������޻���%s��ʵʱ״̬��¼",gen_ptr[i].gen_id);
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
//��ʼ���뵥��ʱ����ص����ݡ�price_flag�ڸú�����û�����á�
void initdata(int price_flag, int sd_num)
{
        sd_id = sd_num;
	//printf("sd is %d\n",sd_id);
        un_num = 0;
        basic_load = get_basic_load(year,month,day,sd_num);//����
        interconnect_line_load = get_interconnect_line_load(year,month,day,sd_num);//������
        float load_forecasting ;
	//load_forecasting = get_load(year,month,day,sd_id);
        //��Լ�����鿪ͣ����Լ�����鿪ͣ���õĸ���Ԥ��ֵ����ͬ����Ҫ��������Լ�����õĸ���Ԥ������һ��ʱ�εĸ���Ԥ��Ϊ����Աָ����ȷ��������賿�͹��±������㡣
 	if(!strcmp(constrain_flag,"yys"))
	load_forecasting = get_load(year,month,day,sd_id);
	else
	load_forecasting = get_wys_load(year,month,day,sd_id);	

	load_forecasting_ptr[sd_id - 1] = load_forecasting;
        float totalload = load_forecasting / (1 - wsxs / 100) + interconnect_line_load - basic_load * (1 - pjcyl);//���ۿռ䣬������������������
	//printf("load_forecasting is %f,basic_load is %d, interconnect_line_load is %d,totalload is %f\n",load_forecasting ,basic_load ,interconnect_line_load,totalload);
	totalload_ptr[sd_id - 1] = totalload;
        //���¼�����������������ı������ݴ����ݿ���ȡ����
        seq_price_num = get_bid_price_num(year,month,day,sd_num);//���л���ĸ�ʱ���ڵ����б��۵�����
        total_bid_un_num = get_gen_num(year,month,day,sd_num);//���۵Ļ�������
        total_un_num = get_qt_gen_num(year,month,day,sd_num);//�б��ۺ��ޱ��۵Ļ�������
        get_price_struct(year,month,day,sd_num);

	/******* initiate the struct that all the biding generators *********/

        int temp_bid_gen_num = 0;
        icc_num = new int[total_bid_un_num];//��¼��������ı��۵�����
        UI =new UN_ICC[total_bid_un_num * 10];//��¼�������鱨�۵�����ݡ�����ÿ���������󱨼۵���Ϊ10������ÿ������ɷ���10��UN_ICC�ṹ��Ԫ��

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
        	if(n == 0||(temp_un_num==0&&temp_point_num==0))// ����ǵ�һ������ĵ�һ�����۵� 
                {
              		strcpy(UI[temp_un_num*10+temp_point_num].un_id,seq_price[n].uni_id);
			strcpy(UI[temp_un_num*10+temp_point_num].un_name,seq_price[n].un_name);
                        UI[temp_un_num*10+temp_point_num].mw = seq_price[n].out_power;
                        UI[temp_un_num*10+temp_point_num].price = seq_price[n].price;
                        temp_point_num++;//�û���ı��۵���+1
                }
                else if(!strcmp(seq_price[n].uni_id, seq_price[n-1].uni_id))//�����ͬһ̨����ı��۵�.
                {
                        strcpy(UI[temp_un_num*10+temp_point_num].un_id,seq_price[n].uni_id);
                        strcpy(UI[temp_un_num*10+temp_point_num].un_name,seq_price[n].un_name);
                        UI[temp_un_num*10+temp_point_num].mw = seq_price[n].out_power;
                        UI[temp_un_num*10+temp_point_num].price = seq_price[n].price;
                        temp_point_num++;
                }
                else//from now on to record a different generator.
                {
                        icc_num[temp_un_num] = temp_point_num;//������һ̨��������ݼ�¼��ϣ����û���ı��۵�����¼��
                        temp_un_num++;//������+1
                        temp_point_num = 0;//��ʼ���û���ı��۵�����
                        strcpy(UI[temp_un_num*10+temp_point_num].un_id,seq_price[n].uni_id);
                        strcpy(UI[temp_un_num*10+temp_point_num].un_name,seq_price[n].un_name);
                        UI[temp_un_num*10+temp_point_num].mw = seq_price[n].out_power;
                        UI[temp_un_num*10+temp_point_num].price = seq_price[n].price;
                        temp_point_num++;
                }
        }
        icc_num[temp_un_num] = temp_point_num;//record the last operating generator's biding point number.
        //�����л���ı�������ת�������ṹgen_ptr[].
	for(int i = 0 ; i < total_bid_un_num ; i ++)
	{
		for(int j = 0 ; j < total_gen ; j ++)
		{
			if(!strcmp(gen_ptr[j].gen_id,UI[i*10].un_id))
			{
				//gen_ptr[j].mw_min[sd_id - 1] = UI[i * 10].mw;
				//gen_ptr[j].mw_max[sd_id - 1] = UI[i * 10 + icc_num[i] - 1].mw; 
				//printf("id is %s min is %d max is %d\n",UI[i*10].un_id,UI[i * 10].mw,UI[i * 10 + icc_num[i] - 1].mw);
				gen_ptr[j].price_for_order[sd_id - 1] = tran_price(i);//�����ʱ��ƽ�����ۡ�
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
                sprintf(result_info,"����3");
		sprintf(result_info_explain,"�����޷����������ڴ�");
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
//�ú��������Щ�ṹ�������ݿ�ӿں�������е��ڴ���䣬���������������Ҳ����ڴ������䡣
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
//��ʱ��ƽ�����۽����Ŷӣ����õ��Ŷ��㷨��ð�ݷ���
void bubble_order(int sd )
{
	GENERATOR t;//������ʱ�ṹ������
	
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
			if((gen_ptr[j].price_for_order[sd] == gen_ptr[j + 1].price_for_order[sd])&&(gen_ptr[j].weight > gen_ptr[j + 1].weight))//������ͬ��Ȩ��С���ȿ�����
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
		if(gen_ptr[i].biding_flag[sd] == 1 && gen_ptr[i].sfjj == 1 && gen_ptr[i].sfky == 1)//����û����ڸ�ʱ���б���
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
                        if(tempload_up >= (totalload_ptr[sd] + load_forecasting_ptr[sd] * WYS_UP_ROTARY_RATE/100) && tempload_down >= (totalload_ptr[sd] - load_forecasting_ptr[sd] * WYS_DOWN_ROTARY_RATE/100))//�����ϱ��ã��������±��á�
			{
				continue;//����Ѱ�Һ��ʵĻ���
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
//����̨�����ƽ��������ͬʱ��һ̨��������һ̨ͣ�����򿪻��Ļ���Ȩ��+1
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
//׼����ʱ���޹ص����ݣ�����init_data(int,int)����
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
	if(PEAK_LOAD_ROTARY_RATE == 1000)//���û��ָ����帺�ɲ��ñ����ʣ�����UP_ROTARY_RATE 
	PEAK_LOAD_ROTARY_RATE = UP_ROTARY_RATE;

	LOWEST_LOAD_ROTARY_RATE = get_lowest_load_rotary_rate(year);
	printf("LOWEST_LOAD_ROTARY_RATE is %f\n",LOWEST_LOAD_ROTARY_RATE);
	if(LOWEST_LOAD_ROTARY_RATE == 1000)//���û��ָ����45��48ʱ�εı����ʣ�����DOWN_ROTARY_RATE 
	LOWEST_LOAD_ROTARY_RATE = DOWN_ROTARY_RATE;

	FIRST_SEG_DOWN_ROTARY_RATE = get_hy_down_rotary_rate(year);
	if(FIRST_SEG_DOWN_ROTARY_RATE == 1000)//���û��ָ����1��14ʱ�εı����ʣ�����LOWEST_LOAD_ROTARY_RATE 
	FIRST_SEG_DOWN_ROTARY_RATE = LOWEST_LOAD_ROTARY_RATE;

        THRESHOLD_ON_GEN = get_threshold_on_gen(year);//������ֵ

        total_sd = get_time_num(year);
        totalload_ptr = new float[total_sd];
        load_forecasting_ptr = new float[total_sd];
        get_gen_num();
        gen_ptr = new GENERATOR[total_gen];
        get_gen_id();
        get_weight();

        allocate_memory();

        get_gen_real_time_state(year,month,day);

	oper_sd = get_wksd(year) - 1;//������Ŀ���ʱ��
	stop_sd = get_wtsd(year) - 1;//��ͣ�����ͣ��ʱ��
	//printf("ROTSARY IS %f,%f\n",DOWN_ROTARY_RATE,UP_ROTARY_RATE);	
        wsxs = get_net_loss_factor();//����ϵ��
	printf("wsxs is %d\n",wsxs);
	pjcyl = get_pjcyl();//ƽ��������
	printf("pjcyl is %f\n",pjcyl);

	int must_on_time = get_must_on_time();//ȡ���û�����С����ʱ��
	int must_off_time = get_must_off_time();//ȡ���û�����Сͣ��ʱ��
	printf("must_on_time must_off_time %d,%d\n",must_on_time ,must_off_time);
	if(must_on_time == 0 && must_off_time == 0)//���û��ָ��������С������ͣ��ʱ��
	{
		get_gen_tech_data(year);//ȡ����������С��ͣ��ʱ��
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
	else if(must_on_time == 0 )//���û��ָ��������С����ʱ��
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
	else if(must_off_time == 0)//���û��ָ��������Сͣ��ʱ��
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
//�ҵ��賿�͹�ʱ����;��ۿռ䣨���ɣ�ʱ�Σ����ڼ����45--48ʱ�εı���ˮƽ��
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
//�ҵ�ȫ�츺��Ԥ��ֵ��͵�ʱ��
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
//�ҵ�ȫ�츺��Ԥ��ֵ��ߵ�ʱ��
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
//�������ıؿ�����ͣʱ��
int compute_must_on_off_sd_num(char * lastTime)
{
        //�β�char * lastTime �����鵱ǰ״̬�ķ���ʱ�䴫��������
        tm * my_tm,* my_tm2;//��������c���Ե�ʱ��ṹ����
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
        //�ƻ��ƶ��Ļ�����ͣ��ʼʱ��Ϊ�ڶ���00��00������strcat(p,"00:00");	
	strcpy(p,present_date);
	strcat(p," 00:00");
	p2 = p ;
	p1 = p ;
        //��mm/dd/yyyy 00:00��ʽ��ʱ��洢��tm�ṹ��
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


        //lastTime��ʽΪmm/dd/yyyy 00:00
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


        time_t my_t,my_t2;//����c����ʱ��ṹtime_t������������
        my_t = mktime(my_tm);
        my_t2 = mktime(my_tm2);

        if((int)(my_t - my_t2) <= 0 )//���״̬����ʱ�����ڼƻ��ƶ�ʱ�䣬�����ݿ��¼�д���
        {
                cout <<"time compute error!"<<endl;
                sprintf(result_info,"����4");
		sprintf(result_info_explain,"���鿪ͣʱ���¼����");
                write_execute_info();
                exit(0);
        }

	delete[] p1;
	delete my_tm,my_tm2;

	int sd_num = 0;
	sd_num = (int)(my_t - my_t2)/3600;//�������ʾ��ʱ�������ΪСʱ��
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
			if(gen_ptr[i].last_state == 1 || gen_ptr[i].last_state == 2) //��������ϴε�״̬Ϊ����
			{
				sd_number = compute_must_on_off_sd_num(gen_ptr[i].last_on_off_time);
				sd_number = gen_ptr[i].min_on_time - sd_number;
				//printf("gen is %s,last_state is %d,must on sd is %d\n",gen_ptr[i].gen_id,gen_ptr[i].last_state,sd_number);
				if(sd_number > 0)//�ؿ�ʱ�δ���0
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
			else if(gen_ptr[i].last_state == 0 )//������һ��״̬Ϊͣ����
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
			//�����Ļ���״̬�������鲻�ܿ���������Ϊ��ͣ3��
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
//���糧����С���з�ʽ��ָ���ıؿ���������Ϊ�ؿ�
void set_mini_mode_must_on_off()
{
	int flag1 = 0,flag2 = 0,flag3 = 0;
	printf("now I am in set_mini_mode_must_on()\n");
	mini_mode_fac_num = get_mini_mode_fac_num(year);//ȡ������С��ʽ�ĵ糧������
	printf("mini_mode_fac_num is %d\n",mini_mode_fac_num);
	if(mini_mode_fac_num == 0)
	return ;
	else//����糧��������0�������ڴ沢���糧��С��ʽ������ȡ����
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
			//����糧ָ���˱ؿ�����1�������ھ��ۻ������ҵ��˸û��飬����������Ϊ�ؿ���			
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
			sprintf(result_info,"����");
			sprintf(result_info_explain,"�Ҳ�����С��ʽָ���ؿ�����%s",fac_mini_mode_ptr[i].must_on_gen1);
			printf("result_info_explain is %s\n",result_info_explain);
			//write_execute_info();
			exit(-1);
		}
		if(strcmp(fac_mini_mode_ptr[i].must_on_gen2,"") && flag2 == 0)
		{
			sprintf(result_info,"����");
			sprintf(result_info_explain,"�Ҳ�����С��ʽָ���ؿ�����%s",fac_mini_mode_ptr[i].must_on_gen2);
                        printf("result_info_explain is %s\n",result_info_explain);
			//write_execute_info();
			exit(-1);
                }
		if(strcmp(fac_mini_mode_ptr[i].must_on_gen3,"") && flag3 == 0)
                {
                        sprintf(result_info,"����");
                        sprintf(result_info_explain,"�Ҳ�����С��ʽָ���ؿ�����%s",fac_mini_mode_ptr[i].must_on_gen3);
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
//���������ڵ糧��С��ʽ�Ƿ�����
int check_gen_mini_num(int gen_ord)
{
	printf("now I am in check_gen_mini_num(int)\n");
	printf("gen_ord is %d\n",gen_ord);
	//int flag = 0;
	int gen_num = 0;
	for(int i = 0 ; i < total_gen ; i ++)//�ҵ��û������ڵ糧�����л�������
	{
		if((gen_ptr[i].gen_state[total_sd - 1] == 1 || gen_ptr[i].gen_state[total_gen - 1] == 2) && !strcmp(gen_ptr[i].fac_id,gen_ptr[gen_ord].fac_id))//���һ��ʱ�ο�������û���Ϊ���С�
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
                        //������л��������ڵ糧��С��ʽָ����̨��
			if(gen_num > fac_mini_mode_ptr[j].gen_mini_num)
			return 1;//������С��ʽ
			else
			return 0;//��������С��ʽ
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
//�ڵ�45--48ʱ��ͣ�����������±��á�
int set_gen_off_for_down_reserve_3()
{
	printf("now I am in set_gen_off_for_down_reserve_3()\n");
	int ret,ret1,ret2,count = 0;

	for(int i = total_gen - 1 ; i >= 0 ; i --)//�ӱ��۸ߵĻ��鿪ʼͣ
	{
		printf("i is %d, gen is %s\n",i,gen_ptr[i].gen_id);
		for(int sd = 0 ; sd < total_sd ; sd ++)
		printf("%d",gen_ptr[i].gen_state[sd]);
		printf("\n");
		for(int sd = 0 ; sd < total_sd ; sd ++)//�ҵ��û���ıؿ�ʱ���������û����бؿ�ʱ�Σ�������ͣ��
		{
			if(gen_ptr[i].gen_state[sd] == 2)
			count ++;
		}
		ret = check_gen_mini_num(i);//���û������ڵ糧�Ƿ�������С���з�ʽ�����ͣ����������С��ʽ���û��鲻��ͣ��
		printf("ret of check_gen_mini_num(i) is %d\n",ret);
		if(ret == 1)//������С��ʽ	
		{
			printf("gen is %s,gen_ptr[i].last_state is %d,gen_ptr[i].gen_state[total_sd - 1] is %d,count is %d\n",gen_ptr[i].gen_id,gen_ptr[i].last_state,gen_ptr[i].gen_state[total_sd - 1],count);
			if(gen_ptr[i].last_state == 1 && gen_ptr[i].gen_state[total_sd - 1] == 1 && count == 0)//�û�����һ��״̬Ϊ�������Ҳ��Ǳؿ����飬�������48ʱ��Ϊ������ͣ���û��顣
			{
				printf("gen can be set off is %s\n",gen_ptr[i].gen_id);
				printf("ret of check_gen_mini_num(i) is %d\n",ret);
				for(int sd1 = stop_sd ; sd1 < total_sd ; sd1 ++)
				gen_ptr[i].gen_state[sd1] = 0;
		
				ret1 = check_down_spinning_reserve_3();//���ͣ��һ̨������±������㣬�򷵻ء�
				if(ret1 == 1)
				return 1;
				else//�����һ̨����ǰ���ؿ�ʱ������0
				{
					count = 0;
				}
			}
			if(gen_ptr[i].last_state == 0  && gen_ptr[i].gen_state[total_sd - 1] == 1 && count == 0)//����û���Ϊ�����飬���Ҳ��Ǳؿ����飬����ͣ��;
			{
				for(int sd2 = oper_sd ; sd2 < total_sd ; sd2 ++)
				gen_ptr[i].gen_state[sd2] = 0;
				ret1 = check_down_spinning_reserve_3();
				ret2 = check_up_spinning_reserve_2();//����ͣ��������Ӱ�����ϱ��ã�����Ҫ����15--44ʱ�ε��ϱ��á�
				if(ret2 == 0)//���ͣ���û�����ϱ��ò����㣬���û����״̬�ָ���
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
	//���л�������󣬺�����û�з��أ�˵���Ҳ������ʵĻ���ͣ��������45--48ʱ�ε��±��á�
	return 0;//if till now havn't find suitable combination, return 0;
}

/*********************************************************************************************************
**
**           the function takes some measures to
**           satisy the minimal mode of the factory
**
*********************************************************************************************************/
//��ȡ��ʩ������糧����С��ʽ����С��ʽ�����ȼ����ڿ�����ֵ���ƣ��������2̨�������������С��ʽ����������ֵΪ1�����Կ���̨������������С��ʽ��
int deal_with_mini_mode1(int n,int gen_num)
{
        //�β�n���糧��С��ʽ�ṹ�������Ŵ��뺯����gen_num��ʾ����Ŵ���ĵ糧��ǰ�ж������еĻ��顣
        printf("now I am in deal_with_mini_mode(int n,int gen_num)\n");
        int ret ;
        int * temp_state;//data struct to record the states of generator;
        temp_state = new int[total_sd];//�����ڴ棬��¼����ĳ�ʼ״̬��

        for(int i = 0 ; i < total_gen ; i ++)
        {
                printf("i is %d,gen is %s\n",i,gen_ptr[i].gen_id);
                if(!strncmp(gen_ptr[i].fac_id,fac_mini_mode_ptr[n].fac_id,strlen(gen_ptr[i].fac_id)))//�����̨�������ڸõ糧
                {
                        printf("gen is %s\n",gen_ptr[i].gen_id);
                        if(gen_ptr[i].gen_state[total_sd - 1] == 0)//����û���ƻ�Ϊͣ��
                        {
                                printf("I am here to record state\n");
                                //��¼����ĳ�ʼ״̬��
                                for(int sd1 = 0 ;sd1 < total_sd ; sd1 ++)//record the state of the generator;
                                temp_state[sd1] = gen_ptr[i].gen_state[sd1];

                                if(gen_ptr[i].last_state == 1)//��һ��״̬Ϊ�����û���ȫ������Ϊ����
                                {
                                        printf("gen_ptr[i].last_state == 1 is %s\n",gen_ptr[i].gen_id);
                                        for(int sd = 0 ; sd < total_sd ; sd ++)
                                        gen_ptr[i].gen_state[sd] = 1;
                                        print_state();
                                }
/*
                                if(gen_ptr[i].last_state == 0)//��һ��״̬Ϊͣ���û����15ʱ������Ϊ����
                                {
                                        printf("gen_ptr[i].last_state == 0 is %s\n",gen_ptr[i].gen_id);
                                        for(int sd3 = oper_sd ; sd3 < total_sd ; sd3 ++)
                                        gen_ptr[i].gen_state[sd3] = 1;
                                        print_state();
                                        decrease_later_on_generator(1);

                                }
                                //����±��ã��˴�����±���Ҳ����Ҫ����Ϊ������ϳ������Ҫ����±��ã�
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
//��ȡ��ʩ������糧����С��ʽ����С��ʽ�����ȼ����ڿ�����ֵ���ƣ��������2̨�������������С��ʽ����������ֵΪ1�����Կ���̨������������С��ʽ��
int deal_with_mini_mode2(int n,int gen_num)
{
        //�β�n���糧��С��ʽ�ṹ�������Ŵ��뺯����gen_num��ʾ����Ŵ���ĵ糧��ǰ�ж������еĻ��顣
        printf("now I am in deal_with_mini_mode(int n,int gen_num)\n");
        int ret ;
        int * temp_state;//data struct to record the states of generator;
        temp_state = new int[total_sd];//�����ڴ棬��¼����ĳ�ʼ״̬��

        for(int i = 0 ; i < total_gen ; i ++)
        {
                printf("i is %d,gen is %s\n",i,gen_ptr[i].gen_id);
                if(!strncmp(gen_ptr[i].fac_id,fac_mini_mode_ptr[n].fac_id,strlen(gen_ptr[i].fac_id)))//�����̨�������ڸõ糧
                {
                        printf("gen is %s\n",gen_ptr[i].gen_id);
                        if(gen_ptr[i].gen_state[total_sd - 1] == 0)//����û���ƻ�Ϊͣ��
                        {
                                printf("I am here to record state\n");
                                //��¼����ĳ�ʼ״̬��
                                for(int sd1 = 0 ;sd1 < total_sd ; sd1 ++)//record the state of the generator;
                                temp_state[sd1] = gen_ptr[i].gen_state[sd1];
/*
                                if(gen_ptr[i].last_state == 1)//��һ��״̬Ϊ�����û���ȫ������Ϊ����
                                {
                                        printf("gen_ptr[i].last_state == 1 is %s\n",gen_ptr[i].gen_id);
                                        for(int sd = 0 ; sd < total_sd ; sd ++)
                                        gen_ptr[i].gen_state[sd] = 1;
                                        print_state();
                                }
*/
                                if(gen_ptr[i].last_state == 0)//��һ��״̬Ϊͣ���û����15ʱ������Ϊ����
                                {
                                        printf("gen_ptr[i].last_state == 0 is %s\n",gen_ptr[i].gen_id);
                                        for(int sd3 = oper_sd ; sd3 < total_sd ; sd3 ++)
                                        gen_ptr[i].gen_state[sd3] = 1;
                                        print_state();
                                        decrease_later_on_generator(1);

                                }
                                //����±��ã��˴�����±���Ҳ����Ҫ����Ϊ������ϳ������Ҫ����±��ã�
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
//��ȡ��ʩ������糧����С��ʽ����С��ʽ�����ȼ����ڿ�����ֵ���ƣ��������2̨�������������С��ʽ����������ֵΪ1�����Կ���̨������������С��ʽ��
int deal_with_mini_mode(int n,int gen_num)
{
	//�β�n���糧��С��ʽ�ṹ�������Ŵ��뺯����gen_num��ʾ����Ŵ���ĵ糧��ǰ�ж������еĻ��顣
	printf("now I am in deal_with_mini_mode(int n,int gen_num)\n");
	int ret ;
	int * temp_state;//data struct to record the states of generator;
	temp_state = new int[total_sd];//�����ڴ棬��¼����ĳ�ʼ״̬��
	
	for(int i = 0 ; i < total_gen ; i ++)
	{
		printf("i is %d,gen is %s\n",i,gen_ptr[i].gen_id);
		if(!strncmp(gen_ptr[i].fac_id,fac_mini_mode_ptr[n].fac_id,strlen(gen_ptr[i].fac_id)))//�����̨�������ڸõ糧
		{
			printf("gen is %s\n",gen_ptr[i].gen_id);
			if(gen_ptr[i].gen_state[total_sd - 1] == 0)//����û���ƻ�Ϊͣ��
			{
				printf("I am here to record state\n");
				//��¼����ĳ�ʼ״̬��
				for(int sd1 = 0 ;sd1 < total_sd ; sd1 ++)//record the state of the generator;
				temp_state[sd1] = gen_ptr[i].gen_state[sd1];

				if(gen_ptr[i].last_state == 1)//��һ��״̬Ϊ�����û���ȫ������Ϊ����
				{
					printf("gen_ptr[i].last_state == 1 is %s\n",gen_ptr[i].gen_id);
					for(int sd = 0 ; sd < total_sd ; sd ++)
					gen_ptr[i].gen_state[sd] = 1;
					print_state();
				}
				if(gen_ptr[i].last_state == 0)//��һ��״̬Ϊͣ���û����15ʱ������Ϊ����
				{
					printf("gen_ptr[i].last_state == 0 is %s\n",gen_ptr[i].gen_id);
					for(int sd3 = oper_sd ; sd3 < total_sd ; sd3 ++)
					gen_ptr[i].gen_state[sd3] = 1;
					print_state();
					decrease_later_on_generator(1);
					
				}
 				//����±��ã��˴�����±���Ҳ����Ҫ����Ϊ������ϳ������Ҫ����±��ã�
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
//�������糧����С��ʽ�Ƿ�����
void check_mini_mode()
{
	printf("now I am in check_mini_mode()\n");
	int ret,count = 0;
	//����С��ʽ�ĵ糧Ϊ0�������ء�
	if(mini_mode_fac_num == 0)
	return;

	for(int n = 0 ; n < mini_mode_fac_num ; n ++)
	{ 
		for(int i = 0 ; i < total_gen ; i ++)
		{
			if(!strncmp(gen_ptr[i].fac_id , fac_mini_mode_ptr[n].fac_id,strlen(gen_ptr[i].fac_id)) && (gen_ptr[i].gen_state[total_sd - 1] == 1 || gen_ptr[i].gen_state[total_sd - 1] == 2))//�������ڸõ糧�����ҿ�����
			count ++; 	
		}
		printf("fac is %s,count is %d,mini_gen is %d\n",fac_mini_mode_ptr[n].fac_id,count,fac_mini_mode_ptr[n].gen_mini_num);
		if(count >= fac_mini_mode_ptr[n].gen_mini_num)//���л�����������С��ʽָ��̨����
		{
			count = 0;
			continue;
		}
		else
		{
			printf("n is %d,count is %d\n",n,count);
			//ret = deal_with_mini_mode(n,count);//��ȡ��ʩ��������С��ʽ
                        ret = deal_with_mini_mode1(n,count);
                        for(int i = 0 ; i < total_gen ; i ++)
                        {
                                if(!strncmp(gen_ptr[i].fac_id , fac_mini_mode_ptr[n].fac_id,strlen(gen_ptr[i].fac_id)) && (gen_ptr[i].gen_state[total_sd - 1] == 1 || gen_ptr[i].gen_state[total_sd - 1] == 2))//�������ڸõ糧�����ҿ�����
                                count ++;
                        }
                        printf("fac is %s,count is %d,mini_gen is %d\n",fac_mini_mode_ptr[n].fac_id,count,fac_mini_mode_ptr[n].gen_mini_num);
                        if(count >= fac_mini_mode_ptr[n].gen_mini_num)//���л�����������С��ʽָ��̨����
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
				sprintf(result_info,"����");
				sprintf(result_info_explain,"�����޷��ҵ�������С��ʽ�Ľ⣬����ָ����С��ʽ�Ƿ����");
				write_execute_info();
				//exit(-1);
*/
				//���������˸ó���С��ʽ��������������糧��
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
//���糧��С��ʽ���������д�����ִ����Ϣ��
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
				//sprintf(temp,"��С��ʽ�޷����㣬�ó����û�����б��ۻ�����С����С��ʽָ��̨��");
				sprintf(temp,"��С��ʽ�޷����㣬�ó����û�����б��ۻ�����Ϊ%d,��С��ʽָ��̨��Ϊ%d",count,fac_mini_mode_ptr[n].gen_mini_num);
				strcat(result_info_explain,temp);
			}
			else
			{
				sprintf(temp,",%s",fac_mini_mode_ptr[n].fac_name);
				strcat(result_info_explain,temp);
				sprintf(temp,"��С��ʽ�޷����㣬�ó����û�����б��ۻ�����Ϊ%d,��С��ʽָ��̨��Ϊ%d",count,fac_mini_mode_ptr[n].gen_mini_num);
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
//������Ա�ֹ����õĻ���״ָ̬��Ϊ�ؿ����ͣ��
void set_mannul_state()
{
	//printf("I am in set_mannul_state()!\n");
	 
	for(int sd = 0 ; sd < total_sd ; sd ++)
	{
		get_mannul_state(year,month,day,sd + 1);//�����ݿ���ȡ�û����ֹ�״̬
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
			if(gen_ptr[i].gen_state[sd] == 2)//sdʱ����Ϊ�ؿ�����û����sdʱ����ȫ��Ϊ�ؿ���
			{
				for(int sd1 = sd ; sd1 < total_sd ; sd1 ++)
				gen_ptr[i].gen_state[sd1] = 2;
			}
			if(gen_ptr[i].gen_state[sd] == 3)//sdʱ����Ϊ��ͣ�����sdʱ����ȫ��Ϊ��ͣ��
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
//����Լ�����鿪ͣ������ȡ���˹������Ļ���ؿ�����ͣ״̬��
void get_gen_state_for_mannul()
{
        printf("I am in ()!\n");
        for(int sd = 0 ; sd < total_sd ; sd ++)
        {
                get_gen_state(year,month,day,sd + 1);//��ʱ�ν������״̬�����ݿ���ȡ����
                for(int i = 0 ; i < total_gen ; i ++)
                {
                        for(int j = 0 ; j < total_un_num ; j ++)
                        {
				//ֻ������Աָ���ıؿ�����ͣ״̬ȡ����
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
//ȡ�þ�����ȫУ��Ļ�����ϼƻ���
void get_gen_state_for_safe()
{
        //printf("I am in ()!\n");
        for(int sd = 0 ; sd < total_sd ; sd ++)
        {
                get_gen_state(year,month,day,sd + 1);//��ʱ�ν������״̬�����ݿ���ȡ����
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
//���1--14ʱ�ε��±���
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
       		sprintf(check_reserve_info,"���������״̬Ӱ�죬��%dʱ���±��ò������ܸ���Ϊ%f,�����������Ϊ%d",rec_sd,rec_totalload,rec_down_spinning_reserve);
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
		sprintf(check_reserve_info,"���������״̬Ӱ�죬��%dʱ���ϱ��ò����������������Ϊ%d,���Ϊ%f",rec_sd,rec_up_spinning_reserve,temp);
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
        	sprintf(check_reserve_info,"���������״̬Ӱ�죬��%dʱ���±��ò������ܸ���Ϊ%f,�����������Ϊ%d",rec_sd,rec_totalload,rec_down_spinning_reserve);
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
//���ƻ�������ͣ�仯�Ļ�������Ϊ������ͣ
void set_generator_on_off_later()
{
	int count = 0;
	printf("now I am in set_generator_on_off_later()!\n");
	for(int i = 0 ; i <= edge_generator ; i ++)    //�߼ʻ������µĻ����ڼƻ��п��������������һ��״̬Ϊͣ����û�������Ϊ����
	{
		for(int sd = 0 ; sd < oper_sd ; sd ++)//������1--14ʱ������бؿ�����ͣ״̬����������
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

	for(int i = edge_generator ; i < total_gen ; i ++)   //�߼ʻ������ϵĻ�������Ϊͣ�����������һ��״̬Ϊ������û�������Ϊ��ͣ��
	{
		for(int sd = 0 ; sd < total_sd ; sd ++)
		{
			if(gen_ptr[i].gen_state[sd] == 3 || gen_ptr[i].gen_state[sd] == 2)//����Ǳؿ�����ͣ���飬��������
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
//������Ա�ֹ����õĻ���״ָ̬��Ϊ�ؿ����ͣ��
void set_mannul_gen_state()
{
	int peak_sd = find_peak_load_sd();//�ҵ��������ʱ�� ��
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
				gen_ptr[i].gen_state[sd] = 4;//4��ʾ�˹����õıؿ����������ڸ��ݻ�����С��ͣʱ�������ıؿ�2;
				if(!strcmp(gen_ptr[i].gen_id , mannul_state_ptr[j].gen_id) && mannul_state_ptr[j].state == 3)
				gen_ptr[i].gen_state[sd] = 5;//5ʾ�˹����õı�ͣ���������ڸ��ݻ�����С��ͣʱ�������ı�ͣ3��
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
				for(int sd3 = 0 ; sd3 < sd ; sd3 ++)//���˹�ָ���ıؿ�ʱ����ǰ��ʱ������Ϊ��ͣ���˹�ָ���ıؿ�ʱ���Ժ��ʱ��ָ��Ϊ�ؿ���
				{
					//printf(" sd3 is %d\n",sd3);
					gen_ptr[i].gen_state[sd3] = 3;
				}
				for(int sd1 = sd ; sd1 < total_sd ; sd1 ++)
				gen_ptr[i].gen_state[sd1] = 2;
			}
			if(gen_ptr[i].gen_state[sd] == 5 && sd < peak_sd)//����˹�ָ���ı�ͣʱ��С�ڸ������ʱ�Σ��û���ȫ������Ϊͣ��
			{
				for(int sd2 = 0 ; sd2 < total_sd ; sd2 ++ )
				gen_ptr[i].gen_state[sd2] = 3;
			}
			if(gen_ptr[i].gen_state[sd] == 5 && sd > peak_sd)//����˹�ָ���ı�ͣʱ�δ��ڸ������ʱ�Σ��û����ָ��ʱ��������Ϊ��ͣ��
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
			sprintf(result_info,"����");
			sprintf(result_info_explain,"��%dʱ���ϱ��ò��㣬ȱ��%f,���ɵ糧���ɵ�������%d",sd+1,need,ZKTRL);
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
//�������飬�����㸺���ϱ��õ�Ҫ��
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
                sprintf(result_info,"����7");
		//sprintf(result_info_explain,"���ɹ������л��鶼�޷���������ת����");
		sprintf(result_info_explain,"���п����л��鶼�޷���������ת���ã����������ߣ����ɣ����鱨���Ƿ���ȷ��������С��ͣʱ���Ƿ������ʹû�л��������ͣ");
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
//�������ܻ�����С��ͣʱ��Լ������ɵıؿ���ͣ�������������ͣ����
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
//û�е���
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
//���ƻ��е������飬����������������������ϱ��ã�����Բ������û��顣
//���������ԭ���ǲ�ʹϵͳ�е���ͣ���������ࡣ
// û�е��ã�
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
//�������е��¿����鶼����������������ʵʱ����״̬��ɵ�1--14ʱ���ϱ��ò����������޷��������ʾ����Ա���˳���
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
		sprintf(result_info,"����8");
                sprintf(result_info_explain,"�����޷������1����14ʱ�ε�����ת�������⣬����ԭ�������û�б��۵Ļ���������ͣ������");
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
//���45--48ʱ��ͣ�����࣬����ϱ��ò�������ʱ����ͣ�����������ϱ��á�
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
		sprintf(result_info,"����9");
		sprintf(result_info_explain,"�����޷������45����48ʱ�ε�����ת�������⣬����ԭ�������û�б��۵Ļ���̫��");
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
	if(mini_flag == 0)//��������С��ʽ��ֱ�ӷ���1����ʾ���㣻
	return 1;
/*
	for(int i = 0   ; i < total_gen ; i ++)
	{
		if(!strncmp(gen_ptr[i].fac_id , gen_ptr[gen_ord].fac_id,strlen(gen_ptr[gen_ord].fac_id))  && (gen_ptr[i].gen_state[peak_sd] == 1 || gen_ptr[i].gen_state[peak_sd] == 2))
		gen_num ++;
	}
*/ 
	else//������С��ʽ��ͳ���������ڸõ糧���������һ��ʱ��Ϊ�����Ļ��������������ڸõ糧��С��ʽָ��̨��������1��
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
	
	//������е糧��������û�з��أ�˵���û��������糧û��ָ����С��ʽ������1��
	return 1;//indicate that the factory has no minimal mode;
}

/********************************************************************************************
**
**            this function check if the substitution between
**            later on generator and later off generator
**            satisfies the constraints;
**
********************************************************************************************/
//����ܷ�ͣ���¿����飬�β� i �ǻ�����ţ�mini_flagΪ0ʱ�����ǵ糧��С��ʽ��Ϊ1���ǵ糧��С��ʽ��
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
//�ú�������һ̨�¿����飬mini_flag�����������õĺ���check_substitute(i,mini_flag)�Ծ����Ƿ��ǵ糧��С��ʽ���ơ�
int substitute_later_on_off_gen(int mini_flag)
{
	int ret ;
	
	for(int i = total_gen - 1  ; i >= 0 ; i --)
	{
		//if((gen_ptr[i].gen_state[oper_sd - 1] == 0 || gen_ptr[i].gen_state[oper_sd - 1] == 3) && gen_ptr[i].gen_state[oper_sd] == 1 && gen_ptr[i].last_state == 0)//indicates that this is a later on generator;
		if(gen_ptr[i].last_state == 0 && gen_ptr[i].gen_state[total_sd - 1] == 1)//˵������һ̨�¿�����
		{
 			ret = check_substitute(i,mini_flag);//����ܷ���̨����ͣ����
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
//�����������ɿ�����ֵ��Լ����ͳ�Ƽƻ����¿�����ĸ�����������ڿ�����ֵ��������¿�����ֱ�����㿪����ֵ��
//����������ϱ���Լ�����¿�������������ڿ�����ֵ�����¿����������ܿ�����ֵ���ƣ�
//�ú����и��β�mini_flag���ò���Ϊ0ʱ��ʾ�����¿�����ʱ�����ǵ糧��С��ʽ�����ƣ�Ϊ1ʱ��ʾ�����¿�����ʱ���ǵ糧��С��ʽ�����ƣ�
// yys_order()��һ�ε��øú���ʱ�����ǵ糧��С��ʽ���ƣ�mini_flag == 0��
void decrease_later_on_generator(int mini_flag )
{
	printf("now i am in decrease_later_on_generator()\n");
	int ret, gen_num = 0 ;
	//int num = total_gen - edge_generator;
	int count = 0;
	for(int i = 0 ; i < total_gen ; i ++)
	{
		//����������һ��״̬Ϊ0�����ƻ������һ��ʱ��״̬Ϊ1��2���û���Ϊ�����顣
		if(gen_ptr[i].last_state == 0 && (gen_ptr[i].gen_state[total_sd - 1] == 1 || gen_ptr[i].gen_state[total_sd - 1] == 2))
		gen_num ++;
	}
	printf("gen_num is %d\n",gen_num);
	if(gen_num <= THRESHOLD_ON_GEN)
	return;
	else
	{
		//count��¼ѭ���Ĵ�������һ��������ѭ���󿪻���ֵ��û�����㣬˵�����ϱ���Ӱ���¿�����������С�ڿ�����ֵ���˳�ѭ����
		while(gen_num > THRESHOLD_ON_GEN && count < 100)       // * num)
		{
			ret = substitute_later_on_off_gen(mini_flag);//����һ̨�¿�����
			printf("ret in de is %d\n",ret);
			if(ret == 1)//�û�����Բ�����
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
//�����������ɿ�����ֵ��Լ����ͳ�Ƽƻ����¿�����ĸ�����������ڿ�����ֵ��������¿�����ֱ�����㿪����ֵ��
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
                        if(gen_ptr[i].last_state == 0 && gen_ptr[i].gen_state[total_sd - 1] == 1 && gen_num > THRESHOLD_ON_GEN)//˵������һ̨�¿�����
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
                       sprintf(result_info,"����");
                       sprintf(result_info_explain,"�����ܿ�����ֵ�����ƣ����ʱ��(��%dʱ��)�ϱ��ò����������������Ϊ%d,���Ϊ%d,",peak_sd,up_spinning_reserve,balance);
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
//û�е���
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
//����һ̨��ͣ����
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
		if(gen_ptr[i].last_state == 1 && gen_ptr[i].gen_state[stop_sd - 1] == 1 && gen_ptr[i].gen_state[stop_sd] == 0)//˵������һ̨��ͣ����
		{
			//printf("I am in decrease_one_gen\n");
			printf("later_off_gen is %s,min_mw is %d\n",gen_ptr[i].gen_id,gen_ptr[i].mw_min[47]);
			for(int sd = stop_sd ; sd < total_sd ; sd ++)
			{
				gen_ptr[i].gen_state[sd] = 1;
			}
			int ret = check_down_spinning_reserve_3();
			if(ret == 1)//�����ͣ���û���45--48ʱ���±����������㣬��û��鲻��ͣ����
			return 1;
			else//����ָ��û����״̬��
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
//���ƻ��е���ͣ���飬���������ͣ����������45--48ʱ���±��ã�����Բ�ͣ���û��顣
//���������ԭ���ǲ�ʹϵͳ�е���ͣ���������ࡣ
void decrease_later_off_gen()
{
	int ret ;
	int later_off_gen = 0;
	ret = check_down_spinning_reserve_3();
	//printf("i am in decrease_later_off_gen\n");
	//printf( "ret is %d\n",ret);
	for(int i = 0 ; i < total_gen ; i ++)//ͳ����ͣ�������
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
//û�е���
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
        	sprintf(result_info,"����5/6");
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
//����������ƽ���۸񣬼��㷽��Ϊ���������ʱ�ε�ƽ�������ۼӣ�������ʱ������
void compute_aver_price()
{
	int count = 0 ;
	float temp = 0 ;
	for(int i = 0 ; i < total_gen ; i ++)
	{
		if(gen_ptr[i].last_state == 1)//����ǰһ��״̬Ϊ1�Ļ��飬����48��ʱ�ε�ƽ�����ۡ�
		{
			for(int sd = 0 ; sd < total_sd ; sd ++)
			{
				if(gen_ptr[i].biding_flag[sd] == 1)//ͳ�ƻ��鱨�۵�ʱ����
				{
					temp = temp + gen_ptr[i].price_for_order[sd];
					count ++;
				}
			}
			if(count != 0)
			gen_ptr[i].average_price = temp/count;
			else//û�б��ۣ����ü۸�Ϊ100000��ʹ�û��鴦�ڻ����Ŷӱ����׶ˡ�
			{
			printf("gen has no biding is %s\n",gen_ptr[i].gen_id);
			gen_ptr[i].average_price = 100000;
			}
		}
		//�������ǰһ���״̬Ϊ0������ԭ���Ӱ�죬��̨����ֻ�д�15--48ʱ�εı��������ã����ԣ�ֻ����15--48 ʱ�ε�ƽ�����ۡ�
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
		else//����ǰһ��״̬Ϊ1��0����Ļ��飬������ͣ���۸�����Ϊ100000��
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
//���ݸ����������ƽ�����۽������Ŷӣ������Ŷ��㷨Ϊð�ݷ���
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
			//����ƽ��������ͬ���������Ȩ��������˳��
			//���������⣬Ȩ�����ά�����޸ģ�
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

	//���ݼ۸���ֵ�����û���ĳ�ʼ״̬��ƽ������С�ڼ۸���ֵ�Ļ�������Ϊ����ƽ�����۴��ڼ۸���ֵ�Ļ�������Ϊͣ��
        for(int i = 0 ; i < total_gen ; i ++)//find the edge generator;
        {
                if(gen_ptr[i].average_price > PRICE_THRESHOLD )
                {
                        //printf("edge_generator is %d,%s\n",i-1,gen_ptr[i-1].gen_id);
                        //print_state();
			//��ʼ�߼ʻ���Ϊ���ݼ۸���ֵ�����û���ĳ�ʼ״̬����󿪻���һ̨���飬�߼ʻ����������û����������ͣ��
                        return i - 1  ;//the edge generator is the generator that is operating last;
                }
        }
	//������л����ƽ������С�ڼ۸���ֵ������һ�����Ʊ߼ʻ���ֵ��
        return 20;//maybe some problem
}

/********************************************************************************************
**
**            set generators' states according to their average prices
**
*********************************************************************************************/
//���ݼ۸���ֵ�����û���ĳ�ʼ״̬��ƽ������С�ڼ۸���ֵ�Ļ�������Ϊ����ƽ�����۴��ڼ۸���ֵ�Ļ�������Ϊͣ��
void mordify_gen_state_with_aver_price()
{
	for(int i = 0 ; i < total_gen ; i ++)
	{
		if(gen_ptr[i].average_price <= PRICE_THRESHOLD)
		{
			for(int sd = 0 ; sd < total_sd ; sd ++)
			{
				//ֻ����Ǳؿ�����ͣ���顣
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
//����������糧����С��ʽ��
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
//����������糧����С��ʽ�����ú���increase_later_on_gen_for_mini_mode(int fac_ord,int gen_num).
void set_gen_later_on_for_mini_mode()
{
	printf("now I am in  set_gen_later_on_for_mini_mode()\n");
	int ret,count = 0;
	//printf("mini_mode_fac_num is %d\n",mini_mode_fac_num);
	if(mini_mode_fac_num == 0)
	return;
	else
	for(int i = 0 ; i < mini_mode_fac_num ; i ++)//ͳ��ÿ���糧�����л�����
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
			//����õ糧����С��ʽû�����㣬�¿�������������С��ʽ
			ret = increase_later_on_gen_for_mini_mode(i,count);	
			if(ret == 1)
			{
				count = 0;
				continue;
			}
			else//����õ糧����С��ʽ�޷�ͨ���������õ����㣬������������������顣
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
//�ú���Ϊ������Լ�����鿪ͣ����Ҫ���ú�����
void yys_order()
{
	int ret,ret1 , ret2 , ret3 = 5 , ret4 , ret5 ;

	set_mini_mode_must_on_off();//���õ糧��С��ʽ��ָ���ıؿ����顣
	print_state();
	compute_aver_price();//������ƽ���۸�
	mordify_gen_state_with_aver_price();//���ݸ����������ƽ���۸����������û���Ŀ�ͣ״̬
	print_state();
	edge_generator = sort_by_aver_price();//���ݼ۸���ֵ�ҵ��߼ʻ���
        increase_edge_generator_for_up_spinning_reserve();//���������������ϱ���
	print_state();
        //int rr = check_spinning_reserve();
        set_generator_on_off_later();//������״̬�仯�Ļ���Ϊ������ͣ���顣
	print_state();

	check_spinning_reserve();
        set_must_on_off_later();//�����бؿ�����ͣ״̬�Ļ����������ͣ״̬��

	set_gen_later_on_for_mini_mode();//����������糧����С��ʽ
	ret1 = check_down_spinning_reserve_1();//���1--14ʱ�ε��±��ã���������㣬˵����ǰһ��Ļ���״̬Ӱ�죬
						//�����޷��ҵ����㱸��Լ�������Ļ�����ϣ������˳���
	if(ret1 == 0)
	{
		printf("yesterday's combination doesn't satisfy today's spinning_reserve!\n");
		set_gen_state(year,month,day);
		sprintf(result_info,"����5/6");
		sprintf(result_info_explain,check_reserve_info);
		//sprintf(result_info_explain,"�����������״̬Ӱ�죬�����޷��ҵ����㱸��Լ�������Ļ�����ϣ�������˹�����");
		write_execute_info();
		exit(-1);
	}
	//printf("ret1 is %d\n",ret1); 
	print_state();
	//���1--14ʱ�ε��ϱ��ã���������㣬˵����ǰһ��Ļ���״̬Ӱ�죬
	//�����޷��ҵ����㱸��Լ�������Ļ�����ϣ������˳���
	ret = check_up_spinning_reserve_1();
	//printf("chech up 1 is %d\n",ret);
	if(ret != 1)
	measure_for_up_spinning_reserve_1();  //must be caused by later on generator and must off generator;
	printf("after measure_for_up_spinning_reserve_1()\n");
	print_state();
	decrease_later_on_unit();//�����������������㿪����ֵ������0��ʾ����������ʱ�����ǵ糧����С��ʽ��
	printf("after decrease_later_on_generator()\n");
	print_state();
	//15--44ʱ�ε��±���һ������»����㣬������ֲ�����������˵���������ࡣ
        ret = check_down_spinning_reserve_2();
	//printf("ret in check_down_spinning_reserve_2 is %d\n",ret);
        if(ret != 1)
	{
                sprintf(result_info,"����5/6");
                sprintf(result_info_explain,check_reserve_info);
                exit(-1);
	}
        //measure_for_down_spinning_reserve_2();
	print_state();

	decrease_later_off_gen();//������ͣ������
        print_state();

	ret2 = check_down_spinning_reserve_3();//���45--48ʱ�ε��±���
	printf("ret2 of check_down_spinning_reserve_3() is %d\n",ret2);
	if(ret2 == 1)//��������±���
	{
		//printf("all is done!!\n");
		//print_state();
		check_mini_mode();//У�鲢��ȡ��ʩʹ�糧����С��ʽ����
		int ret6 = check_spinning_reserve();
		return;
	}
	else
	{
		//ret4 = set_generator_off_for_down_reserve_3();
		ret4 = set_gen_off_for_down_reserve_3();//��45--48ʱ��ͣ����ʹ�±�������
		printf(" ret of set_gen_off_for_down_reserve_3() is %d\n",ret4);
		if(ret4 == 1)
		{
			//printf("the reserve is satisfied!!\n");
			//print_state();
			check_mini_mode();
			int r = check_spinning_reserve();
			return ;
		}
		else//�������С��ʽ�ͻ�����С��ͣʱ��Լ������ͣ������٣��±������޷����㣬������ʾ��Ϣ���˳���
		{
			printf("the program can't resolve down_reserve_3!!\n");
			sprintf(result_info,"����10");
			sprintf(result_info_explain,"�����޷������45����48ʱ�ε�����ת�������⣬������˹�����");
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
				sprintf(result_info,"����");
				sprintf(result_info_explain,"�����޷������45����48ʱ�ε�����ת�������⣬������˹�����");
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
//û�е���
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
                sprintf(result_info,"����");
                sprintf(result_info_explain,check_reserve_info);
                //sprintf(result_info_explain,"�����������״̬Ӱ�죬�����޷��ҵ����㱸��Լ�������Ļ�����ϣ�������˹�����");
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
				sprintf(result_info,"����");
				sprintf(result_info_explain,"�����޷������45����48ʱ�ε�����ת�������⣬������˹�����");
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
//���ݻ���İ�ȫԼ����ͣ��ָ�꽫��������Ŷӣ��㷨Ϊð�ݷ���
void sort_by_safe_index()
{
        GENERATOR t1;
	//�ƻ���ͣ���Ļ��鰴��ͣ��ָ���С�����Ŷӣ�����ָ���������ȿ�����
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
				//��ͣ��ָ����ͬ�Ļ��飬���ݻ������������Ŷӣ������С�Ļ���ƽ���۸�ͣ�Ӧ�����ں����Ա��ȿ�����
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
	//�ƻ������еĻ��鰴��ͣ��ָ���С�����Ŷӣ�ͣ��ָ����������ͣ����
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
			//��ͣ��ָ����ͬ�Ļ��飬���ݻ������������Ŷӣ�����Ŵ�Ļ���ƽ���۸�ߣ�Ӧ�����ں����Ա���ͣ����
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
//�ҵ��ƻ������һ̨�����Ļ��飬����Ϊ��ȫ�߼ʻ���
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
//���ݰ�ȫԼ��ָ����п�ͣ�������������㰲ȫԼ�����β�int * temp_state1��int * temp_state2��¼����ĳ�ʼ״̬��
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
	//û�л���Ҫ���а�ȫԼ����ͣ���������˳���
	if(gen_num_change == 0)
	{
		printf("there is no generator te be changed state!!\n");
		return 1;
	}
	else
	{
	//���ھ��а�ȫԼ����ͣ��ָ��Ļ����м���е�������������Ϊ�ȿ�һ̨����ָ�����Ļ��飬������ʱ�ε����±��ã�������㣬�������أ�
	//��������㣬ͣ��һ̨ͣ��ָ�����Ļ��飬������ʱ�ε����±��ã�������㣬�������أ���������㣬������������Ļ��顣
	//////////firstly deal with generators which have ktzb////////////
		for(int i = total_gen - 1  ; i > edge_generator  ; i --)//����
		{
			if(gen_ptr[i].gen_state[peak_sd] == 3)//����û���ı�ͣʱ�γ����˼�帺��ʱ�Σ����øû�������;
			continue;
			else if(gen_ptr[i].ktzb != 0)//�����п�ͣ��ָ��Ļ����н��е���
                        {
				for(int sd = 0 ; sd < total_sd ; sd ++)//��¼����ĳ�ʼ״̬
				temp_state1[sd] = gen_ptr[i].gen_state[sd];

				if(gen_ptr[i].last_state == 1)//����
				{
                                	for(int sd2 = 0 ; sd2 < total_sd ; sd2 ++)
                                        gen_ptr[i].gen_state[sd2] = 1;
                                }
				else if(gen_ptr[i].last_state == 0)  //�������ǰһ���״̬Ϊ0�����鿪��ʱ�����������Сͣ��ʱ��Լ��
				{
					for(int sd2 = 0 ; sd2 < total_sd ; sd2++)//ͳ�Ʊ�ͣʱ����
					{
						if(gen_ptr[i].gen_state[sd2] == 3)
						must_off_num ++;
					}
					if(must_off_num != 0)//�б�ͣʱ�Σ���ӱ�ͣʱ���Ժ�ʼ����
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
					else//û�б�ͣʱ��
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
					
			for(int j = edge_generator  ; j > 0 ; j --)//ͣ��
			{
				for(int sd = 0 ; sd < total_sd ; sd ++)
				temp_state2[sd] = gen_ptr[j].gen_state[sd];
				if(gen_ptr[j].ktzb != 0)//�����п�ͣ��ָ��Ļ����н��е���
				{
					if(gen_ptr[j].last_state == 0)
					{
						for(int sd3 = 0 ; sd3 < total_sd ; sd3 ++)
						gen_ptr[j].gen_state[sd3] = 0;
					}
					else if(gen_ptr[j].last_state == 1)//�������ǰһ���״̬Ϊ1��ͣ��ʱ�α������������С����ʱ��Լ��
					{
						for(int sd3 = 0 ; sd3 < total_sd ; sd3 ++)//ͳ�Ʊؿ�ʱ����
						{
							if(gen_ptr[j].gen_state[sd3] == 2)
							must_on_num ++;
						}
						if(must_on_num != 0)//����бؿ�ʱ�Σ��ӱؿ�ʱ�κ�ʼͣ��
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

		//��������������п�ͣ��ָ��Ļ������û�з��أ�����������еĻ����н��п�ͣ����������������������ͬ��
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
	//���������л����û�з��أ�˵���Ҳ������ʵĻ�������������Լ��������2��������ʧ�ܡ�
	return 2;//can't find suitable combination to satisfy both safe constraint and time,reserve constraint;
}

/**************************************************************************************************************     
**
**               this funstion changes generators'states as the safe constraint 
**               program wants to satisfy the safe constraint
**
**************************************************************************************************************/
//�ú�������substitute_for_safe(temp_state1,temp_state2)���л���״̬������
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
		sprintf(result_info,"����11");
		sprintf(result_info_explain,"�����޷�ͬʱ�������ϱ��ú͵͹��±��ã������Ҳ������㰲ȫԼ���Ļ�����ϣ����˹�����");
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
//�ú���Ϊ���а�ȫԼ����ͣ������������
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
//�������״̬���0/1��ʾ��
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
//ͳ�ƻ���Ŀ�ͣ������������ۿռ䣬��48ʱ�ε����л���ĳ�������д�����ݿ⡣
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
                                        sprintf(temp,"�ڵ�%dʱ�ο���",j+2);
                                        strcat(result_info_explain,temp);
                                }
				else
				{
                                        sprintf(temp,",%s",gen_ptr[i].gen_name);
                                        strcat(result_info_explain,temp);
                                        sprintf(temp,"�ڵ�%dʱ�ο���",j+2);
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
                                        sprintf(temp,"�ڵ�%dʱ��ͣ��",j+2);
                                        strcat(result_info_explain,temp);
				}
				else
				{
                                        sprintf(temp,",%s",gen_ptr[i].gen_name);
                                        strcat(result_info_explain,temp);
                                        sprintf(temp,"�ڵ�%dʱ��ͣ��",j+2);
                                        strcat(result_info_explain,temp);
				}
				printf("off gen is %s",gen_ptr[i].gen_id);
                        }
                }
        }
	if(!strcmp(result_info_explain,""))
	{
	//sprintf(result_info_explain,"���ۿռ���%f,�������������%d",totalload_ptr[total_sd - 1],sum_low_limit);
		sprintf(result_info_explain,"���ۿռ���%f,",totalload_ptr[total_sd - 1]);
		//sprintf(temp,"�������������%d",sum_low_limit
		//sprintf(temp,"�������������%d",sum_low_limit);

		//strcat(result_info_explain,temp);

	}
	else
	{
		//sprintf(temp,",���ۿռ���%f,�������������%d",totalload_ptr[total_sd - 1],sum_low_limit);
		sprintf(temp,",���ۿռ���%f,",totalload_ptr[total_sd - 1]);
		strcat(result_info_explain,temp);
		//sprintf(temp,"�������������%d",sum_low_limit);strcat(result_info_explain,temp);
	}	
	int len = strlen(result_info_explain);
	//printf("len is %d\n",len);
}

/*********************************************************************************************************
              �������л���ĳ������޺�
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
	sprintf(program_name,"�Ŷӻ��鿪ͣ��");
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
              	        sprintf(result_info,"����");
			sprintf(result_info_explain,"��Լ���Ŷӳ�����������������һ��Ϊ���ڲ���mm/dd/yyyy,�ڶ���Ϊwys,��ʾ��Լ���Ŷӣ��������ÿո����");
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
                                sprintf(result_info,"����");
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
			sprintf(result_info,"����");
			sprintf(result_info_explain,"Լ�����ͱ���Ϊyys");
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

                        sprintf(result_info,"��ȷ");
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
			sprintf(result_info,"����");
                        sprintf(result_info_explain,"��Լ�������У�0���������ֹ�������1�������ֹ�������2�����ǰ�ȫԼ��");
			write_execute_info();
			exit(-1);
		}

	        //fclose(fp);
        }

        else if(argc == 5)
        {
                set_opp_rela();//�����ݿ������ļ������������ݿ�����ӡ�
		clear_execute_info();//�����ݿ��еĳ���ִ����Ϣ���
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
              	        sprintf(result_info,"����13");
			sprintf(result_info_explain,"��Լ���Ŷӳ�����4����������һ��Ϊ���ڲ���mm/dd/yyyy,�ڶ���Ϊwys,��ʾ��Լ���Ŷӣ��������ÿո����������������Ϊ�û�ID,���ĸ�����Ϊ�û�����");
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
                                sprintf(result_info,"����12");
				//sprintf(result_info_explain,"�����޷��������ת��������,�����л���������޺�С�ڸ��ɣ�����ԭ�������û�б��۵Ļ������");
				sprintf(result_info_explain,"���л��鶼�޷���������ת���ã����������ߣ����ɣ����鱨���Ƿ���ȷ");
				write_execute_info();
                                exit(-1);
                        }
*/
                        set_gen_state(year,month,day);//write to the database;
                        printf("afterset_gen_state\n");
                        print_state();
                        set_gen_weight();//this function is only used in wys_situation;
                        sprintf(result_info,"��ȷ");
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
			sprintf(result_info,"����13");
			sprintf(result_info_explain,"Լ�����ͱ���Ϊyys");
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

                        sprintf(result_info,"��ȷ");
                        sprintf(result_info_explain,"");
                        //print_state();
                        gen_on_off_info();
                        printf("info is %s\n",result_info_explain);
                        sum_low_limit = compute_gen_min_mw_sum();
                        char temp[100];
                        sprintf(temp,"�������������%d",sum_low_limit);strcat(result_info_explain,temp);
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

                        sprintf(result_info,"��ȷ");
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
        		sprintf(result_info,"��ȷ");
        		sprintf(result_info_explain,"");
        		print_state();
                	gen_on_off_info();
                	printf("info is %s\n",result_info_explain);
			sum_low_limit = compute_gen_min_mw_sum();
			char temp[100];
			sprintf(temp,"�������������%d",sum_low_limit);strcat(result_info_explain,temp);		
			mini_mode_info();
			printf("info is %s\n",result_info_explain);
                	write_execute_info();

                        free_new_memory();

		}
		else
		{
			printf("please input correct parameter!!\n");
			sprintf(result_info,"����13");
                        sprintf(result_info_explain,"��Լ�������У�0���������ֹ�������1�������ֹ�������2�����ǰ�ȫԼ��");
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
		sprintf(result_info,"����13");
		sprintf(result_info_explain,"������������Ϊ5��6");
		write_execute_info();
                exit(-1);
        }
        cputime = clock();
/*
	sprintf(result_info,"��ȷ");
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
                sprintf(result_info,"����1");
                sprintf(result_info_explain,"���ݿ�ر�ʧ��");
                write_execute_info();
                exit(-1);
         }

	printf("The program ended successfully !!\n");
        printf("it took %.2fsecs.\n",cputime/1.0e+6);
}
