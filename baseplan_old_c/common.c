#include<time.h>
#include<string.h>
#include<stdlib.h>
#include"common.h"



/**
 * @brief time2bucket 由时间转为时段
 * @param tmp_time 为时间值,比如15:45
 * @param total_sd 为全天的总时段数
 * @return 返回时段号
 */
int time2bucket(const char *tmp_time, int total_sd)
{
	char hour[10], min[10], time[10];
	const char *tmp;
	int int_hour, int_min, i, ret;
	int sd_len = 24*60/total_sd;

	if( NULL == time)return -1;
	
	tmp = tmp_time;
	
	for(i=0; i<5; i++)
	{
		if(2 == i)
		{	if( ':' != tmp[2])
				return -1;
		}

		else
		{
			if( tmp[i] < '0' || tmp[i] > '9')
				return -1;
		}
	}
	
	strcpy( time, tmp_time);
	tmp = strtok( time, ":");
	strcpy( hour, tmp);
	tmp = strtok( NULL, ":");
	strcpy( min, tmp);

	
	int_hour = atoi(hour);
	int_min = atoi(min);

	if(int_hour < 0 || int_hour > 24)
		return -1;
	if(int_min >60  && int_min < 0)
		return -1;	
	if(int_hour == 0 && int_min == 0)
		return -1;


	ret = int_hour*60/sd_len + int_min/sd_len;
	return ret;

}

/**
 * @brief print_curtline 生成漂亮的分隔符
 * @param str 分隔符中间的说明文字
 */
void print_curtline(const char *str)
{
	int i,num_print = 100,front = 0,tail = 0;
	int letter_num = strlen(str);
	if(num_print > letter_num)
	{
		front = (num_print - letter_num)/2;
		tail = num_print - front - letter_num;
		for(i=0;i<front;i++)
			printf("=");
		printf("%s",str);
		for(i=0;i<tail;i++)
			printf("=");
	}else 
		printf("%s",str);
	printf("\n");
}

/**
 * @brief get_date_and_time 获取此时的系统时间，所有参数自己在函数外面申请
 * @param in_date 取得的日期值
 * @param in_time 取得的时间值
 * @return 0执行正确，-1调用时间函数出现错误
 */
int get_date_and_time(char*in_date,char*in_time)
{

	time_t timep;
	struct tm *p;
	if(-1 == time(&timep))
    {
        fprintf(stderr,"%s\n","get_date_and_time()调用time函数错误");
        return -1;
    }
	p = localtime(&timep);
	if(!p)
    {
        fprintf(stderr,"%s\n","get_date_and_time()调用localtime函数错误");
        return -1;
    }
	sprintf(in_date,"%04d-%02d-%02d",(1900+p->tm_year), (1+p->tm_mon),p->tm_mday);
	sprintf(in_time,"%02d:%02d:%02d", p->tm_hour, p->tm_min, p->tm_sec);//2000/10/28 Sat 8:15:38

    return 0;
}

/**
 * @brief init_float_arr 初始化数组为指定值
 * @param arr 需要初始化的数组
 * @param len 数组长度
 * @param value 初始化的值
 * @return 成功返回0
 */
int init_float_arr(float *arr, int len, float value)
{
    int i;
    assert(arr != NULL);
    assert(len >= 0);
    for(i=0; i<len; i++)
    {
        arr[i] = value;
    }
    return 0;
}

/**
 * @brief init_int_arr 初始化数组为指定值
 * @param arr 需要初始化的数组
 * @param len 数组长度
 * @param value 初始化的值
 * @return 成功返回0
 */
int init_int_arr(int *arr, int len, int value)
{
    int i;
    assert(arr != NULL);
    assert(len >= 0);
    for(i=0; i<len; i++)
    {
        arr[i] = value;
    }
    return 0;
}

/**
 * @brief bucket_to_time 由时段向时间转换
 * @param bucket 时段序号
 * @param time_bucket 一天当中的时段数
 * @param time 转换后的时间值（传出参数）
 * @param len time字符数组的长度
 * @return 0转换成功，-1传入的参数有误，转换失败
 */
int bucket_to_time(int bucket, int time_bucket, char *time, int len)
{
    if(bucket < 1 || bucket > time_bucket || len < 6)
    {
        fprintf(stderr, "%s\n", "bucket,time_bucket,len有误");
        return -1;
    }
    int power_to_energy = time_bucket/24;
    int hour, minunite;
    hour = bucket/power_to_energy;
    minunite = bucket%power_to_energy*24*60/time_bucket;
    sprintf(time, "%02d:%02d", hour, minunite);
    return 0;
}

/**
 * @brief bucket2time 由时段向时间转换
 * @param bucket 时段序号
 * @param time_bucket 一天当中的时段数
 * @param time 转换后的时间值（传出参数）
 * @return time转换成功，NULL传入的参数有误，转换失败
 */
char * bucket2time(int bucket, int time_bucket)
{
    static char time[20];
    if(bucket < 1 || bucket > time_bucket)
    {
        fprintf(stderr, "%s\n", "bucket,time_bucket有误");
        return NULL;
    }

    int power_to_energy = time_bucket/24;
    int hour, minunite;
    hour = bucket/power_to_energy;
    minunite = bucket%power_to_energy*24*60/time_bucket;
    sprintf(time, "%02d:%02d", hour, minunite);
    return time;
}
