#include"base_plan.h"
#include"assert.h"

int print_unit(char *mess);
int get_max(unsigned int *load_count, int len);
int init_int_arr(int *arr, int len, int value);


int init_prog();
int get_plant_info();
int get_limit();
int check();
int init_to_downlimit_power();
int cal_unit_gen();
int free_memory();
int main(int argc, char *argv[])
{

	
	strcpy(act_info.prog_id,argv[0]);	
    creep_capacity = 10;
   // strcpy(act_info.prog_id,argv[0]);
	strcpy(date, argv[1]);	
	init_prog();
        printf("here\n");
	get_plant_info();
	
	print_unit("获取电厂定电量后打印");
	get_limit();
	check();
	init_to_downlimit_power();
	print_unit("用最小出力初始化电厂出力后打印");
	
	cal_unit_gen();	
	
	printf("\n\n\n");
	
	print_unit("定电量算法运行后打印");
	int max_bucket = get_max(load_forcast, time_bucket);
	printf("最大点在%d, 最大值为%u\n", max_bucket, load_forcast[max_bucket-1] );
	
	
	
	
	
	write_act_info( 1, "执行定电量负荷分配程序成功");
	free_memory();	
	close_database();
	printf("\n\n");
}
int free_memory()
{
	long long int i;
	for( i=0; i<plant_num; i++)
	{
		free(unit_info_p[i].have_count);
	}
	free(unit_info_p);
	free(load_forcast);
}

int get_plant_info()
{
	
//============================================获取电厂日定电量=======================================
	print_curtline("获取电厂日定电量");
	
	char sql_string[256];
	long long i, row_num;
	int flag, j;

	char plant_id[UN_ID_LEN];
	int  plant_id_size;

	char plant_name[UN_NAME_LEN];
	int  plant_name_size;

	char day_count_str[10];
	int  day_count_size;
	unsigned int day_count;



	sprintf(sql_string, "select DCID,DCMC,RFDL from LJJNDB.LJJNDB.T_DCRDDL where RQ='%s'",date);
	if( exec_sql(sql_string) != 0 )
		PERROR_CLOSEDATABASE_EXIT(-1, "获取电厂定电量sql语句执行错误")
	
	if( DM_FALSE == dm_bind_column(hsmt, 1, DM_DATA_CHAR, TYPE_DEFAULT,
				plant_id, sizeof(plant_id), &plant_id_size,
				DM_DESC_PTR_LEN_DEFAULT) )
		PERROR_CLOSEDATABASE_EXIT(-1, "获取电厂定电量时，列1绑定错误")
	
	if( DM_FALSE == dm_bind_column(hsmt, 2, DM_DATA_CHAR, TYPE_DEFAULT,
				plant_name, sizeof(plant_name), &plant_name_size,
				DM_DESC_PTR_LEN_DEFAULT) )
		PERROR_CLOSEDATABASE_EXIT(-1, "获取电厂定电量时，列2绑定错误")
	
	if( DM_FALSE == dm_bind_column(hsmt, 3, DM_DATA_CHAR, TYPE_DEFAULT,
				day_count_str, sizeof(day_count_str), &day_count_size,
				DM_DESC_PTR_LEN_DEFAULT) )
		PERROR_CLOSEDATABASE_EXIT(-1, "获取电厂定电量时，列3绑定错误")
	
	if(DM_FALSE == dm_describe_row_num(hsmt, &row_num) )
		 PERROR_CLOSEDATABASE_EXIT(-1, "获取电厂定电量时，获取返回结果记录数失败")
	if( 0 >=row_num )
		 PERROR_CLOSEDATABASE_EXIT(-1, "获取电厂定电量时，获取返回结果记录数小于0")
	
	plant_num = row_num;
	unit_info_p = malloc( sizeof(unit_info)*row_num );
	printf("unit_info_p用malloc分配了%d*%lld=%lld\n", sizeof(unit_info), row_num, sizeof(unit_info)*row_num);
	
	for(i=0; i<row_num; i++)
	{
		memset(unit_info_p[i].un_id, '\0', UN_ID_LEN);
		memset(unit_info_p[i].un_name, '\0', UN_NAME_LEN);
		unit_info_p[i].day_count = 0;
		unit_info_p[i].num_time = 0;
		for(j=0; j<10; j++)
		{
			unit_info_p[i].up_limit[j] = 0;
			unit_info_p[i].down_limit[j] = 0;
			unit_info_p[i].start_time[j] = 0;
			unit_info_p[i].end_time[j] = 0;
		}
		unit_info_p[i].have_count_sum = 0;
		unit_info_p[i].have_count = NULL;
	}
	for(i=0; i<row_num; i++)
	{
		flag = dm_fetch_scroll(hsmt,DM_FETCH_NEXT,0);
		if(DM_FALSE == flag)
			PERROR_CLOSEDATABASE_EXIT(-1, "获取电厂定电量时,dm_fetch_scroll()出错")
		else if(DM_NO_DATA_FOUND == flag)
			PERROR_CLOSEDATABASE_EXIT(-1,"获取电厂定电量时,数据表中没有数据,"
					"中dm_fetch_scroll()出错")
		strcpy(unit_info_p[i].un_id, plant_id);
		strcpy(unit_info_p[i].un_name,plant_name);
		unit_info_p[i].day_count = atoi(day_count_str);

	}

	for( i=0; i<row_num; i++)
	{
		unit_info_p[i].have_count = malloc(sizeof(unsigned int)*time_bucket);
		for(j=0; j<time_bucket; j++)
		{
			unit_info_p[i].have_count[j] = 0;
		}
		printf("unit_info_p[%lld].have_count用malloc分配了%d*%d=%d\n",
				i, sizeof(unsigned int), time_bucket, sizeof(unsigned int)*time_bucket );
	}
	printf("%-15s	%-20s	%-9s\n","电厂ID","电厂名称","日定电量");
	for( i=0; i<row_num; i++)
	{
		printf("%-15s	%-20s	%-9u\n", unit_info_p[i].un_id, unit_info_p[i].un_name, unit_info_p[i].day_count);
	}
//============================================获取电厂日定电量结束=======================================
	print_curtline("获取电厂日定电量结束");
}
int init_prog()
{
	
	char sql_string[256];
	int flag;
	long long i, j, row_num;
	
	
	

	printf("hello\n");
	if(init_database()!=0)
		return -1;

	
	
	
	print_curtline("获取网损系数和每日时段数");
	//=====================================================获取网损系数和每日时段数======================
	char para_name_str[10];
	int para_name_size;
	
	char para_value_str[10];
	int para_value_size;

	sprintf(sql_string, "%s", "select csmc,cssz from LJJNDB.LJJNDB.T_XTKZCS where csmc in ('wsxs','mrsds')");
	
	
	
	if( exec_sql(sql_string) != 0 )
		PERROR_CLOSEDATABASE_EXIT(-1, "获取系统网损系数和每日时段数sql语句执行错误")
	
	if( DM_FALSE == dm_bind_column(hsmt, 1, DM_DATA_CHAR, TYPE_DEFAULT,
				para_name_str, sizeof(para_name_str), &para_name_size,
				DM_DESC_PTR_LEN_DEFAULT) )
		PERROR_CLOSEDATABASE_EXIT(-1, "获取系统网损系数时和每日时段数时，列1绑定错误")
	
	if( DM_FALSE == dm_bind_column(hsmt, 2, DM_DATA_CHAR, TYPE_DEFAULT,
				para_value_str, sizeof(para_value_str), &para_value_size,
				DM_DESC_PTR_LEN_DEFAULT) )
		PERROR_CLOSEDATABASE_EXIT(-1, "获取系统网损系数时和每日时段数时，列2绑定错误")
	
	if(DM_FALSE == dm_describe_row_num(hsmt, &row_num) )
		 PERROR_CLOSEDATABASE_EXIT(-1, "获取系统网损系数和每日时段数时，获取返回结果记录数失败")
	if( 2 !=row_num )
		 PERROR_CLOSEDATABASE_EXIT(-1, "获取系统网损系数和每日时段数时，获取返回结果记录数不等于2")

	for(i=0; i<row_num; i++)
	{
		flag = dm_fetch_scroll(hsmt,DM_FETCH_NEXT,0);
		if(DM_FALSE == flag)
			PERROR_CLOSEDATABASE_EXIT(-1, "get_unit_info()中dm_fetch_scroll()")
		else if(DM_NO_DATA_FOUND == flag)
			PERROR_CLOSEDATABASE_EXIT(-1,"select distinct sname from "
					"plan.plan.unit_info执行时,数据表中没有数据,"
					"get_unit_info()中dm_fetch_scroll()")
		if( 0 == strcmp(para_name_str,"mrsds") )	
			time_bucket = atoi(para_value_str);
		else if (0 == strcmp(para_name_str,"wsxs") )
			loss_factor = atof(para_value_str)/100;
	}
	power_to_energy = time_bucket/24;	//每日时段数/24=每小时几个时段，用它作为电力到电能的转换系数
	printf("网损系数：%f, 每日时段数: %d\n", loss_factor, time_bucket);
	
	print_curtline("获取网损系数和每日时段数结束");	
	//==============================================="获取网损系数和每日时段数结束"======================	
	
	
	
	
	
	
	//=========================================================从数据库中取负荷预测值=====================
	print_curtline("获取负荷预测值");

	
	char time_count_str[5];
	int time_count_size;

	char load_forcast_str[10];
	int load_forcast_size;
	load_forcast = malloc(sizeof(unsigned int)*time_bucket);
	printf("load_forcast用malloc 分配空间%d*%d=%d\n",sizeof(unsigned int), time_bucket, sizeof(unsigned int)*time_bucket);

	
	
	
	sprintf(sql_string, "select SD,FHYCZ from LJJNDB.LJJNDB.t_rfhyc where FHYCLXID='rsgxzfhyc' and RQ='%s' order by SD;",date);
	if( exec_sql(sql_string) != 0 )
		PERROR_CLOSEDATABASE_EXIT(-1, "获取负荷预测值sql语句执行错误")
	
	if( DM_FALSE == dm_bind_column(hsmt, 1, DM_DATA_CHAR, TYPE_DEFAULT,
				time_count_str, sizeof(time_count_str), &time_count_size,
				DM_DESC_PTR_LEN_DEFAULT) )
		PERROR_CLOSEDATABASE_EXIT(-1, "获取负荷预测值时，列1绑定错误")
	
	if( DM_FALSE == dm_bind_column(hsmt, 2, DM_DATA_CHAR, TYPE_DEFAULT,
				load_forcast_str, sizeof(load_forcast_str), &load_forcast_size,
				DM_DESC_PTR_LEN_DEFAULT) )
		PERROR_CLOSEDATABASE_EXIT(-1, "获获取负荷预测值时，列2绑定错误")
	
	if(DM_FALSE == dm_describe_row_num(hsmt, &row_num) )
		 PERROR_CLOSEDATABASE_EXIT(-1, "获取负荷预测值时，获取返回结果记录数失败")
	if( time_bucket !=row_num )
		 PERROR_CLOSEDATABASE_EXIT(-1, "获取负荷预测值时,获取返回结果记录数不等于时段数")

	for(i=0; i<row_num; i++)
	{
		flag = dm_fetch_scroll(hsmt,DM_FETCH_NEXT,0);
		if(DM_FALSE == flag)
			PERROR_CLOSEDATABASE_EXIT(-1, "处理返回记录时，dm_fetch_scroll失败")
		else if(DM_NO_DATA_FOUND == flag)
			PERROR_CLOSEDATABASE_EXIT(-1,"处理负荷预测值时，dm_fetch_scroll失败"
					"plan.plan.unit_info执行时,数据表中没有数据,"
					"get_unit_info()中dm_fetch_scroll()")
		time_count = atoi(time_count_str);
		if( i+1 != time_count)
		{	
			printf("错误\n");
			PERROR_CLOSEDATABASE_EXIT(-1, "查询的负荷预测值中时段号有错误")
		}
		load_forcast[i] = atoi(load_forcast_str);
	}

	printf("\n");
	printf("所查的日负荷预测值为\n");
	for(i=0; i<10; i++)
		printf("%-6lld",i+1);
	printf("\n");
	for(i=0; i<time_bucket; i++)
		if(0 == (i+1)%10)
			printf("%-5u\n",load_forcast[i]);
		else
			printf("%-5u,",load_forcast[i]);
	printf("\n");
	print_curtline("获取负荷预测值结束");
//============================================获取负荷预测值结束=======================================

	
	
	
	
	
	
	
}

int print_unit(char *mess)
{
	void *tmp;
	long long int i;
	int j;
	printf("++++++++++++++++++++++++++++++++++%s+++++++++++++++++++++++++++++++++++++++++\n\n",mess);
	for(i=0; i<plant_num; i++)
	{
		printf("%-10s %-15s %-10s %-10s %-10s\n", "电厂ID", "电厂名称", "定电量","分段数","已出力");
		printf("%-10s %-15s %-10u %-10u %-10u\n",
				unit_info_p[i].un_id, unit_info_p[i].un_name, unit_info_p[i].day_count,
				unit_info_p[i].num_time, unit_info_p[i].have_count_sum
				);
		printf("----------------------------------------------------\n");
		printf("%-10s %-10s %-10s %-10s\n", "开始时段", "终止时段", "出力上限", "出力下限");
		for(j=0; j<unit_info_p[i].num_time; j++)
		{
			printf("%-10u %-10u %-10u %-10u\n",unit_info_p[i].start_time[j], unit_info_p[i].end_time[j],
					unit_info_p[i].up_limit[j], unit_info_p[i].down_limit[j]);
		}
		printf("----------------------------------------------------\n");
		printf("此电厂已经分配的出力\n");
		printf("%6d",0);
		for(j=0; j<10; j++)
	        printf("%6d",j+1);
	    printf("\n\n");
	    for(j=0; j<time_bucket; j++)
		    if(0 == (j)%10)
			    printf("\n%6d %6u", j/10, unit_info_p[i].have_count[j]);
	        else
	            printf("%6u",unit_info_p[i].have_count[j]);
		printf("\n\n");
		printf("----------------------------------------------------\n");
	}
	printf("++++++++++++++++++++++++++++++++++%s+++++++++++++++++++++++++++++++++++++++++\n\n",mess);
}

int get_limit()
{
	
	printf("\n");
	print_curtline("获取电厂出力上下限");
	
//============================================获取电厂出力上下限=======================================
	char sql_string[256];
	long long int num = 0, i;
	void *db_data;
	void *tmp;
	unsigned int x;
	int column_num = 5, j;
	sprintf(sql_string,"%s","select DCID,QSSD,ZZSD,ZDCL,ZXCL from LJJNDB.LJJNDB.T_DCRDDLCLFW where RQ='2013-01-01'");
	num = exec_select(sql_string, column_num, "取得电厂出力上下限", &db_data);

	for(i=0; i<num; i++)
	{
		tmp = db_data+i*column_num*COLUMN_LEN;
		printf("%-15s %-15s %-15s %-15s %-15s\n", (char*)tmp, (char*)(tmp+COLUMN_LEN), (char*)(tmp+2*COLUMN_LEN), (char*)(tmp+3*COLUMN_LEN), (char*)(tmp+4*COLUMN_LEN));
	}

	for(i=0; i<num; i++)
	{
		tmp = db_data+i*column_num*COLUMN_LEN;
		for(j=0; j<plant_num; j++)
		{					
			if( 0 == strcmp(unit_info_p[j].un_id, (char*)tmp) )
			{
				x = unit_info_p[j].num_time;
				unit_info_p[j].start_time[x] = atoi( (char*)(tmp+COLUMN_LEN) );
				unit_info_p[j].end_time[x] = atoi( (char*)(tmp+2*COLUMN_LEN) );
				unit_info_p[j].up_limit[x] = atoi( (char*)(tmp+3*COLUMN_LEN) );
				unit_info_p[j].down_limit[x] = atoi( (char*)(tmp+4*COLUMN_LEN) );
				unit_info_p[j].num_time++;

			}
		}
	}
	
	print_curtline("获取电厂出力上下限结束");
	printf("\n");
//============================================获取电厂出力上下限结束=======================================
	
}
int init_to_downlimit_power()
{
	
	int i, j, x;
	for(i=0; i<plant_num; i++)
	{
		for(j=1; j<=time_bucket; j++)
		{
			for(x=0; x<unit_info_p[i].num_time; x++)
			{
				if( j>=unit_info_p[i].start_time[x] && j<=unit_info_p[i].end_time[x])
				{
					unit_info_p[i].have_count[j-1] = unit_info_p[i].down_limit[x];
					//load_forcast[j] -= unit_info_p[i].down_limit[x];
					unit_info_p[i].have_count_sum += unit_info_p[i].down_limit[x];
				}
						
			}
		}
	}
}
int check()
{
	int i,j,x;
	unsigned int sum_downlimit, sum_uplimit;
	unsigned int sum_time;
	char mess[256];
	for(i=0; i<plant_num; i++)
	{
		sum_downlimit = 0;
		sum_uplimit   = 0;
		for(j=1; j<=time_bucket; j++)
		{
			for(x=0; x<unit_info_p[i].num_time; x++)
			{
				if( j>=unit_info_p[i].start_time[x] && j<=unit_info_p[i].end_time[x])
				{
					sum_downlimit += unit_info_p[i].down_limit[x];
					sum_uplimit   += unit_info_p[i].up_limit[x];
				}
						
			}
		}//end for j
		sum_time = 0;
		for(x=0; x<unit_info_p[i].num_time; x++)
		{
			sum_time += (unit_info_p[i].end_time[x] - unit_info_p[i].start_time[x] + 1);
		}
			
		memset(mess, '\0', sizeof(mess) );
		sprintf(mess, "机组ID：%s，机组名称：%s，此电厂的出力范围表中时段之和 %u 每日时段数 %u",
				unit_info_p[i].un_id, unit_info_p[i].un_name, sum_time, time_bucket);
		printf("%s\n",mess);
		if(sum_time != time_bucket)
		{
			memset(mess, '\0', sizeof(mess) );
			sprintf(mess, "机组ID：%s，机组名称：%s，此电厂的出力范围表中时段之和 %u 不等于每日时段数 %u",
				unit_info_p[i].un_id, unit_info_p[i].un_name, sum_time, time_bucket);

			PERROR_CLOSEDATABASE_EXIT(-1,mess)
		}
		if( sum_downlimit > unit_info_p[i].day_count*power_to_energy)
		{
			memset(mess, '\0', sizeof(mess) );
			sprintf(mess, "机组ID：%s，机组名称：%s，此电厂的各时段最小出力之和 %u 大于日定电量的出力值 %u",
				unit_info_p[i].un_id, unit_info_p[i].un_name, sum_downlimit, unit_info_p[i].day_count*power_to_energy);

			PERROR_CLOSEDATABASE_EXIT(-1,mess)
		}//end if
		

		if( sum_uplimit < unit_info_p[i].day_count*power_to_energy)
		{
		
			memset(mess, '\0', sizeof(mess) );
			sprintf(mess, "机组ID：%s，机组名称：%s，此电厂的各时段最大出力之和 %u 小于日定电量的出力值 %u",
				unit_info_p[i].un_id, unit_info_p[i].un_name, sum_uplimit, unit_info_p[i].day_count*power_to_energy);

			PERROR_CLOSEDATABASE_EXIT(-1,mess)
		}//end if
	}//end for i

}
int init_int_arr(int *arr, int len, int value)
{
	int i;
	assert(arr != NULL && len >= 0);
	for(i=0; i<len; i++)
	{
		arr[i] = value;
	}
	return 0;
}

int get_max(unsigned int *load_count, int len)
{
    int i, max_bucket;
    unsigned int tmp = 0;
    for(i=0; i<len; i++)
    {
        if( tmp < load_count[i] )
        {
            max_bucket = i;
            tmp = load_count[i];
        }
    }
    return max_bucket;
}

int cal_unit_gen()
{
	int alldone_flag = 0;
	//最大负荷点的时段值
	int max_bucket;
	int differ, differ1, differ2;
	//循环变量
	int i, j, x;
	//如果某一项为0代表此机组还没有升出力结束，如果为1代表升出力已经结束
	int *done = (int*) malloc(sizeof(int)*plant_num);
	int done_sum = 0;
	init_int_arr(done, plant_num, 0);
	
	
	
	max_bucket = get_max(load_forcast, time_bucket);
	while(plant_num != alldone_flag)
	{
		alldone_flag = 0;
		//done_sum与plant_num相等说明在此时段已没有电厂可以升出力了，再找最大负荷的点
		if(plant_num == done_sum)
		{
			//要么就是达到日定电量，要么是达到最大出力
			//在此处修改负荷曲线可以避免重复减去最小出力，因为在一个时段不可能在此处执行两次
			for(j=0; j<plant_num; j++)
                load_forcast[max_bucket] -= unit_info_p[j].have_count[max_bucket];
			max_bucket = get_max(load_forcast, time_bucket);
			done_sum = 0;
			init_int_arr(done, plant_num, 0);
		}
		
		for(i=0; i<plant_num; i++)
		{
			//获得机组在此时段的最大出力
			unsigned int up_limit = 0, down_limit = 0;
			for(x=0; x<unit_info_p[i].num_time; x++)
			{
				if(max_bucket >= unit_info_p[i].start_time[x] && max_bucket <= unit_info_p[i].end_time[x] )
				{
					up_limit   = unit_info_p[i].up_limit[x];
					down_limit = unit_info_p[i].down_limit[x];
				}
			}
			//三个差值求最小值：求得步长，总已经发电量换算为电力和后的值与已出力的差，本时段的已出力与最大出力的差
			int min = 0;
			if(unit_info_p[i].have_count_sum < unit_info_p[i].day_count*power_to_energy && 
                    unit_info_p[i].have_count[max_bucket] < up_limit)
			{
				differ1 = unit_info_p[i].day_count*power_to_energy - unit_info_p[i].have_count_sum;
				differ2 = up_limit - unit_info_p[i].have_count[max_bucket];
				min = differ1<differ2 ? (differ1<STEP?differ1:STEP) : (differ2<STEP?differ2:STEP);
                if(min < creep_capacity)
                {
                    unit_info_p[i].have_count[max_bucket] += min;
                    unit_info_p[i].have_count_sum += min;
                }
                else
                {
                    unit_info_p[i].have_count[max_bucket] += creep_capacity;
                    unit_info_p[i].have_count_sum += creep_capacity;
                    done[i] = 1;
                }
			}
			else//确定该机组在此时段的出力,此时段已经结束升出力
			{
				done[i] = 1;
			}
		}//end for plant_num ,i
        done_sum = 0;
		for(j=0; j<plant_num; j++)
			done_sum += done[j];
		for(j=0; j<plant_num; j++)
		{
			if(unit_info_p[j].have_count_sum == unit_info_p[j].day_count*power_to_energy)
				alldone_flag++;
		}
		//done_sum = 0;//下一个循环开始，清零done_sum数组，即如果有一个for循环没有进行升出力
	}//end for while
	
	free(done);
}//end for cal_unit_gen
