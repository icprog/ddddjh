#include"common.h"
#include<time.h>
#include<string.h>

/*
*函数功能:由时间转为时段
*参数:tmp_time为时间值,total_sd为全天的总时段数，比如15:45
*返回值:返回时段号
*/
int time2sd(const char *tmp_time, int total_sd)
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
int get_date_and_time(char*in_date,char*in_time)
{

	time_t timep;
	struct tm *p;
	if(-1 == time(&timep))
		perror("get_date_and_time()错误");
	p = localtime(&timep);
	if(!p)
		perror("get_date_and_time()错误");
	sprintf(in_date,"%04d-%02d-%02d",(1900+p->tm_year), (1+p->tm_mon),p->tm_mday);
	sprintf(in_time,"%02d:%02d:%02d", p->tm_hour, p->tm_min, p->tm_sec);//2000/10/28 Sat 8:15:38
	//printf("get_date_and_time() %s  %s\n",in_date,in_time);
}


