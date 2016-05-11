#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "var_def.h"
void get_date_and_time()
{
	time_t nowtime;
	tm *tm_t;
	time(&nowtime);
	//printf("time=%d\n",nowtime);
	tm_t = new tm;
	tm_t = localtime(&nowtime);
	int date_year,date_mon,date_day,date_hour,date_min,date_sec;
	date_year = 1900 + tm_t->tm_year;
	date_mon = tm_t->tm_mon + 1;
	date_day = tm_t->tm_mday;
	date_hour = tm_t->tm_hour;
	date_min = tm_t->tm_min;
	date_sec = tm_t->tm_sec;
	delete tm_t;
//	char exe_date[12];
//	char exe_time[10];
	if(date_mon < 10 && date_day < 10)
		sprintf(exe_date,"0%d/0%d/%d",date_mon,date_day,date_year);
	else if(date_mon < 10)
		sprintf(exe_date,"0%d/%d/%d",date_mon,date_day,date_year);
	else if(date_day < 10)
		sprintf(exe_date,"%d/0%d/%d",date_mon,date_day,date_year);
	else
		sprintf(exe_date,"%d/%d/%d",date_mon,date_day,date_year);
	if(date_hour < 10 && date_min < 10 && date_sec < 10) 
		sprintf(exe_time,"0%d:0%d:0%d",date_hour,date_min,date_sec);
	else if(date_hour < 10 && date_min < 10) 
		sprintf(exe_time,"0%d:0%d:%d",date_hour,date_min,date_sec);
	else if(date_hour < 10 && date_sec < 10) 
		sprintf(exe_time,"0%d:%d:0%d",date_hour,date_min,date_sec);
	else if(date_min < 10 && date_sec < 10) 
		sprintf(exe_time,"%d:0%d:0%d",date_hour,date_min,date_sec);
	else if(date_hour < 10) 
		sprintf(exe_time,"0%d:%d:%d",date_hour,date_min,date_sec);
	else if(date_min < 10) 
		sprintf(exe_time,"%d:0%d:%d",date_hour,date_min,date_sec);
	else if(date_sec < 10) 
		sprintf(exe_time,"%d:%d:0%d",date_hour,date_min,date_sec);
	else
		sprintf(exe_time,"%d:%d:%d",date_hour,date_min,date_sec);
	//printf("exe_date=%s\n",exe_date);
	//printf("exe_time=%s\n",exe_time);
}
