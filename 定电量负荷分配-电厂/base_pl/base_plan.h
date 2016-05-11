#ifndef BASE_PLAN_H
#define BASE_PLAN_H


#include<stdio.h>
#include<stdlib.h>
#include"log.h"
#include"dm_operater.h"
#include"inifile.h"
#include"common.h"
#include"api.h"
#define		 UN_ID_LEN		20
#define		 UN_NAME_LEN	40 
#define		 STEP			100
extern act_info_stru act_info;
unsigned int time_count;
unsigned int *load_forcast;
unsigned int plant_num;
char date[20];
float loss_factor;
int time_bucket;
int power_to_energy;//用于在电力和电量间转换
int creep_capacity;
typedef struct
{
	char un_id[UN_ID_LEN];			//电厂ID
	char un_name[UN_NAME_LEN];		//电厂名称
	unsigned int  day_count;		//日定电量
	unsigned int  num_time;			//大的时段的个数
	unsigned int  start_time[10];	//开始时段
	unsigned int  end_time[10];		//结束时段
	unsigned int  up_limit[10];		//时段上限
	unsigned int  down_limit[10];	//时段下限
	unsigned int  have_count_sum;	//已经分配的电力和
	unsigned int  *have_count;		//分配给各个时段的电力,长度动态分配，根据每日时段数
}unit_info;
unit_info *unit_info_p; 

#endif
