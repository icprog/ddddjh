#ifndef _BASE_PLAN_H
#define _BASE_PLAN_H


#include<stdio.h>
#include<stdlib.h>
#include"log.h"
#include"dm_operater.h"
#include"inifile.h"
#include"common.h"
#include"api.h"
#define		 UN_ID_LEN		20
#define		 UN_NAME_LEN	100
#define      MAX_LOOP       99000
#define     FLOAT_MIN       (-1*__FLT_MAX__)
extern act_info_stru act_info;
int time_count;
float *load_forcast;
//sum of loadforcast and sum of daycount of all unit
float sum_loadforcat = 0, sum_daycount = 0;
//标记一种时段，所有机组在这种时段处的出力已经达到最大出力
int *power_have_max;
int unit_num;
char date[20];

int time_bucket;
//每日时段数/24=每小时几个时段，用它作为电力到电能的转换系数
//用于在电力和电量间转换
int power_to_energy;
float creep_capacity;
float down_capacity;
char mess[MAX_BUFFER];


//以下变量暂时不再使用
long loop;
int random_top = 1;


typedef struct
{
	char   un_name[UN_NAME_LEN];		//机组名称
	float  day_count;		//日定电量
	float  *up_limit;		//时段上限
	float  *down_limit;	    //时段下限
	float  have_count_sum;	//已经分配的电力和
	float  *have_count;		//分配给各个时段的电力
	int    *on_off;			//指示是否开机，0表示停机，1表示开机
    int    *done;           //指示此时段是否结束升出力，达到最大出力设为1，达到定电量，全部96点全置1
    int    count;
}unit_info;
unit_info *unit_info_p; 

/**
 * @brief init_prog 初始化程序执行环境：
 *初始化数据库链接，获取机组爬坡速率、机组下坡速率
 *每日时段数(time_bucket)， 获取负荷预测值（load_forcast*)
 * @return 初始化数据库链接失败返回-1，其余失败直接退出写程序执行信息，
 *		   成功返回0
 */
int init_prog();

/**
 * @brief get_unit_info 获得机组的机组名称，开停机信息,构造结构unit_info数组
 * @return 成功返回0，失败直接退出程序，并且向数据库中写程序执行信息
 */
int get_unit_info();

void print_unit(char *mess);

/**
 * @brief get_k_max 获取前k大的数在数组中的位置
 * @param load_count 数组
 * @param len 数组长度
 * @param k 第k大的数
 * @return 返回查到的数在数组中的位置
 */
int get_k_max(float *load_count, int len, int k);

int get_max(float *load_count, int* mask, int len);
int get_ran_max(float *load_count, int len);
int just_get(float *load_count, int len);

int print_forcast();
int write_loadforcast();


/**
 * @brief get_limit 获取电厂出力的上下限，电厂一天可能有几个不同的处理上下限
 * @return
 */
int get_limit();


/**
* @brief get_limit_max 把所有的约束考虑进去进行最大值的查找
* @param load_count 负荷预测数组
* @param len 负荷预测数组长度
* @return 返回找到满足条件的负荷预测的最大值点的位置
*/
int get_limit_max(float *load_count, int len);

/**
 * @brief check 校验数据的有效性，机组最小出力之和大于定电量，机组最大出力小于定电量，都为无效数据
 * @return
 */
int check();

/**
 * @brief init_to_downlimit_power 把各机组的出力初始化为其最小出力
 *注意：要确保time_bucket和unit_num为大于零的数
 * @return
 */
int init_to_downlimit_power();


/**
 * @brief cal_unit_gen 计算个机组的出力
 * @return
 */
int cal_unit_gen();

/**
 * @brief check_creep 检查此点加一后是否满足爬坡约束
 * @param current 该机组在该时段出力值的指针
 * @param front 前一点的出力值
 * @param back 后一点的出力值
 * @return 返回1代表增1后满足爬坡和下坡速率，返回0代表不满足
 */
int check_creep(float *current, float front, float back);
/**
 * @brief free_memory 释放程序申请的空间
 */

int write_unit_power();
void free_memory();
#endif
