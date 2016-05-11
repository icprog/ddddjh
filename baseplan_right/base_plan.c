#include"base_plan.h"
#include"assert.h"

int print_unit(char *mess);
int get_max(float *load_count, int len);
int init_float_arr(float *arr, int len, float value);
int init_int_arr(int *arr, int len, int value);


int init_prog();
int get_unit_info();
int get_limit();
int check();
int init_to_downlimit_power();
int cal_unit_gen();
int free_memory();
int get_unit_ability();
int main(int argc, char *argv[])
{

	
	strcpy(act_info.prog_id,argv[0]);	
        //creep_capacity = 10;
   // strcpy(act_info.prog_id,argv[0]);
	strcpy(date, argv[1]);	
	init_prog();
        printf("here\n");
	get_unit_info();

	
	get_limit();
	check();
	init_to_downlimit_power();
	print_unit("用最小出力初始化电厂出力后打印");
	
	cal_unit_gen();	
	
	printf("\n\n\n");
	

	
    //write_act_info( 1, "执行定电量负荷分配程序成功");
	free_memory();	
    close_database();



	printf("\n\n");
}



int check()
{
    int i,j;
	float sum_downlimit, sum_uplimit;
	unsigned int sum_time;
	char mess[256];
	for(i=0; i<unit_num; i++)
	{
		sum_downlimit = 0;
		sum_uplimit   = 0;
		for(j=0; j<time_bucket; j++)
		{
			if( 1 == unit_info_p[i].on_off[j] )
			{
				sum_downlimit += unit_info_p[i].down_limit[j];
				sum_uplimit   += unit_info_p[i].up_limit[j];
			}
		}//end for j
		sum_time = 0;
		if( sum_downlimit > unit_info_p[i].day_count*power_to_energy)
		{
			memset(mess, '\0', sizeof(mess) );
			sprintf(mess, "机组名称：%s，此电厂的各时段最小出力之和 %-10.3f 大于日定电量的出力值 %-10.3f",
				unit_info_p[i].un_name, sum_downlimit, unit_info_p[i].day_count*power_to_energy);

			PERROR_CLOSEDATABASE_EXIT(-1,mess)
		}//end if
		

		if( sum_uplimit < unit_info_p[i].day_count*power_to_energy)
		{
		
			memset(mess, '\0', sizeof(mess) );
			sprintf(mess, "机组名称：%s，此电厂的各时段最大出力之和 %-10.3f 小于日定电量的出力值 %-10.3f",
				unit_info_p[i].un_name, sum_uplimit, unit_info_p[i].day_count*power_to_energy);

			PERROR_CLOSEDATABASE_EXIT(-1,mess)
		}//end if
	}//end for i

}


int cal_unit_gen()
{
	int alldone_flag = 0;
	//最大负荷点的时段值
	int max_bucket;
	float differ, differ1, differ2;
	//循环变量
    int i, j;
    //如果某一项为0代表此机组还没有升出力结束(或者达到最大出力，或者达到定电量)
    //如果为1代表升出力已经结束
	int *done = (int*) malloc(sizeof(int)*unit_num);
    //找到的最大负荷时段中每个机组的升出力值
	float *asc = (float*)malloc(sizeof(float)*unit_num);
    int have_max = 0;
	int done_sum = 0;
	int loop = 0;//寻找max_buck的次数，大于一个上限就认为找不到解
	char mess[500];
	init_int_arr(done, unit_num, 0);
	init_float_arr(asc,unit_num,0.0);
	
	
	
	max_bucket = get_max(load_forcast, time_bucket);
	while(unit_num != alldone_flag)
	{
		loop++;
        if( MAX_LOOP < loop )
            break;
		alldone_flag = 0;

		//done_sum与plant_num相等说明在此时段已没有电厂可以升出力了，再找最大负荷的点
		if(unit_num == done_sum)
		{
			//要么就是达到日定电量，要么是达到最大出力
			//在此处修改负荷曲线可以避免重复减去最小出力，因为在一个时段不可能在此处执行两次
			for(j=0; j<unit_num; j++)
                load_forcast[max_bucket] -= asc[j];
			init_float_arr(asc,unit_num,0.0);
			max_bucket = get_max(load_forcast, time_bucket);
			done_sum = 0;
			init_int_arr(done, unit_num, 0);
    //        alldone_flag = 0;
		}
        //==============================================查找是否此时段处是否都达到最大出力或者=========================
        have_max = 0;
        for(i=0;i<unit_num;i++)
        {
            if(unit_info_p[i].have_count[max_bucket] ==unit_info_p[i].up_limit[max_bucket]
                   ||unit_info_p[i].have_count_sum == unit_info_p[i].day_count*power_to_energy )
            {
                have_max++;
            }
        }
        if(have_max == unit_num)
        {
            power_have_max[max_bucket] = 1;
           // continue;
        }
       // init_int_arr(power_have_max, time_bucket, 0);
        //=================================================================================================
		for(i=0; i<unit_num; i++)
		{
			//获得机组在此时段的最大出力
			float up_limit = 0, down_limit = 0;
			up_limit   = unit_info_p[i].up_limit[max_bucket];
			down_limit = unit_info_p[i].down_limit[max_bucket];
			//三个差值求最小值：求得步长，总已经发电量换算为电力和后的值与已出力的差，
			//本时段的已出力与最大出力的差
            float min = 0;
			if( 0 == unit_info_p[i].on_off[max_bucket] )
            {
                done[i] = 1;
                continue;
            }
//            printf("max_bucket=%d\n",max_bucket);
//            printf("unit_info_p[%d].have_count_sum=%f,unit_info_p[%d].day_count=%f,\n"
//                   "unit_info_p[%d].have_count_sum[%d]=%f,up_limit=%f\n",
//                   i, unit_info_p[i].have_count_sum, i, unit_info_p[i].day_count,
//                   i, max_bucket, unit_info_p[i].have_count[max_bucket],up_limit );
            if(unit_info_p[i].have_count_sum < unit_info_p[i].day_count*power_to_energy &&
                    unit_info_p[i].have_count[max_bucket] < up_limit)
			{
				differ1 = unit_info_p[i].day_count*power_to_energy - unit_info_p[i].have_count_sum;
				differ2 = up_limit - unit_info_p[i].have_count[max_bucket];
                min = differ1<differ2 ? differ1 : differ2;
                if(differ1 < differ2)
                {
					if( 1 == unit_info_p[i].on_off[max_bucket] )
					{
						unit_info_p[i].have_count[max_bucket] += min;
						unit_info_p[i].have_count_sum += min;
						asc[i]+=min;
                        done[i] = 0;
					}
                }
                else
                {
					if( 1 == unit_info_p[i].on_off[max_bucket] )
					{
                        unit_info_p[i].have_count[max_bucket] += min;
                        unit_info_p[i].have_count_sum += min;
                        asc[i]+= min;
						done[i] = 1;
					}
                }
			}
			else//确定该机组在此时段的出力,此时段已经结束升出力
			{
				done[i] = 1;
			}
		}//end for unit_num ,i
        done_sum = 0;
		for(j=0; j<unit_num; j++)
			done_sum += done[j];
        alldone_flag = 0;
		for(j=0; j<unit_num; j++)
		{
			if(unit_info_p[j].have_count_sum == unit_info_p[j].day_count*power_to_energy)
				alldone_flag++;
		}
		//done_sum = 0;//下一个循环开始，清零done_sum数组，即如果有一个for循环没有进行升出力
	}//end for while
    print_unit("定电量算法运行后打印");
    if( MAX_LOOP < loop)
	{
		sprintf(mess, "%s", "在所给的定电量和开停机、爬坡速率、降速率条件下，找不到解");
		PERROR_CLOSEDATABASE_EXIT(-1,mess)
	}
	free(done);
}//end for cal_unit_gen


/*
*函数功能：把电厂的出力初始化为其最小出力
*
*/
int init_to_downlimit_power()
{
	
    int i, j;
	assert(unit_num > 0);
	assert(time_bucket > 0);
	for(i=0; i<unit_num; i++)
	{
		for(j=0; j<time_bucket; j++)
		{
			if( 1 == unit_info_p[i].on_off[j] )
			{
				unit_info_p[i].have_count[j] = unit_info_p[i].down_limit[j];
				//load_forcast[j] -= unit_info_p[i].down_limit[x];
				unit_info_p[i].have_count_sum += unit_info_p[i].down_limit[j];
				load_forcast[j] -= unit_info_p[i].down_limit[j];
			}
						
		}
	}
	return 0;
}
/*
*函数功能：获取电厂出力的上下限，电厂一天可能有几个不同的处理上下限
*
*/
int get_limit()
{

	char desc[256];
	sprintf(desc, "%s", "获取机组的出力上下限");
	printf("\n");
	print_curtline(desc);
	
//============================================获取机组出力上下限=======================================
	char sql_string[256];
	long long int row_num = 0, i;
	void *db_data;
	void *tmp;
//	unsigned int x;
	int column_num = 5, j, k;
//	select stime, sname, top_limit, low_limit 
//		from plan.plan.unit_electric_limit
//		where sdate='2013-08-13'
	sprintf(sql_string,"select stime, sname, top_limit, low_limit "
			"from plan.plan.unit_electric_limit where sdate='%s'", date);
	row_num = exec_select(sql_string, column_num, desc, &db_data);
	if(row_num <= 0)
		PERROR_CLOSEDATABASE_EXIT(-1, desc)
	for(i=0; i<row_num; i++)
	{
		tmp = db_data+i*column_num*COLUMN_LEN;
		for(j=0; j<unit_num; j++)
		{
			if( !strcmp( unit_info_p[j].un_name, tmp+COLUMN_LEN) )
			{
				k = time2sd( tmp, time_bucket );
				unit_info_p[j].up_limit[k-1] = atof(tmp+2*COLUMN_LEN);
				unit_info_p[j].down_limit[k-1] = atof(tmp+3*COLUMN_LEN);
			}
		}
	}

	free(db_data);	
	//print_unit("获取机组上下限后");
	print_curtline(desc);
	printf("\n");
//============================================获取机组出力上下限结束=======================================
	return 0;	
}

/*
*函数功能：获得机组的机组名称，开停机信息
*		   构造结构unit_info数组
*返回值：  成功返回0，失败直接退出程序，并且向数据库中写程序执行信息
*/
int get_unit_info()
{
	
	char desc[256];
	sprintf(desc, "%s", "获取开机机组名称信息");
//============================================获取机组的名称信息=======================================
	print_curtline(desc);
	
	char sql_string[256];
	long long i, row_num;
	int flag, j, column_num, k;
	void *db_data, *tmp;

//select distinct sname from plan.plan.unit_start_shut 
//	where sdate='2013-10-01' and status='开机'
	sprintf(sql_string, "select distinct sname from plan.plan.unit_start_shut"
			" where sdate='%s' and status='开机'",date);
	
	column_num = 1;
	row_num = exec_select( sql_string, column_num, desc, &db_data);	
	
	if(row_num <= 0)
		PERROR_CLOSEDATABASE_EXIT(-1,desc)
	unit_num = row_num;	
	unit_info_p = malloc( row_num*sizeof(unit_info) );
	for(i=0; i<row_num; i++)
	{
		unit_info_p[i].up_limit = malloc( time_bucket*sizeof(float) );
		unit_info_p[i].down_limit = malloc( time_bucket*sizeof(float) );
		unit_info_p[i].have_count = malloc( time_bucket*sizeof(float) );
		unit_info_p[i].on_off = malloc( time_bucket*sizeof(int) );

		unit_info_p[i].day_count = 0.0;
		unit_info_p[i].have_count_sum = 0.0;
		init_float_arr( unit_info_p[i].up_limit, time_bucket, 0.0);
		init_float_arr( unit_info_p[i].down_limit, time_bucket, 0.0);
		init_float_arr( unit_info_p[i].have_count, time_bucket, 0.0);
		init_int_arr( unit_info_p[i].on_off, time_bucket, 0.0);


		tmp = db_data+i*column_num*COLUMN_LEN;
	
		strcpy( unit_info_p[i].un_name, tmp );
	
	}

	for(i=0; i<row_num; i++)
	{
		if(0 == i%4)
			printf("\n");
		printf("%-20s", unit_info_p[i].un_name);
	}
	printf("\n");
	free(db_data);
	print_curtline(desc);
//============================================获取机组的名称信息结束=======================================


//============================================获取机组开停机状态===========================================
	sprintf(desc,"%s","获取机组开停机状态");
	print_curtline(desc);
//select stime, sname from plan.plan.unit_start_shut 
//	where sdate='2013-08-13' and status='开机';
	sprintf(sql_string, "select stime, sname from plan.plan.unit_start_shut "
			"where sdate='%s' and status='开机'",date);
	
	column_num = 2;
	row_num = exec_select( sql_string, column_num, desc, &db_data);	
	
	if(row_num <= 0)
		PERROR_CLOSEDATABASE_EXIT(-1, desc)
	
	for(i=0; i<row_num; i++)
	{
		tmp = db_data+i*column_num*COLUMN_LEN;
		for(j=0; j<unit_num; j++)
		{
			if( !strcmp( unit_info_p[j].un_name, tmp+COLUMN_LEN) )
			{
				k = time2sd( tmp, time_bucket );
				unit_info_p[j].on_off[k-1] = 1;
			}
		}
	
	}
	for(j=0; j<unit_num; j++)
	{
		printf("机组名称：%s\n",unit_info_p[j].un_name);
		for(i=0; i<10; i++)
			printf("%-3lld ", i);
		printf("\n");
		for(i=0; i<time_bucket; i++)
		{
			if( 0 == (i%10))
				printf("\n%-3d ", unit_info_p[j].on_off[i]);
			else
				printf("%-3d ", unit_info_p[j].on_off[i]);
		}
		printf("\n");
	}
	free(db_data);
	printf("\n");
	print_curtline(desc);
//===========================================获取机组开停机状态结束========================================	

//===========================================获取机组的发电能力信息========================================
	sprintf(desc, "%s", "获取机组的发电能力信息");
	print_curtline(desc);

	sprintf(sql_string, "select sname, electric from plan.plan.unit_electric_ability "
			" where sdate='%s'", date);

	column_num = 2;
	row_num = exec_select( sql_string, column_num, desc, &db_data);	
	
	if(row_num <= 0)
		PERROR_CLOSEDATABASE_EXIT(-1, desc)
	
	for(i=0; i<row_num; i++)
	{
		tmp = db_data+i*column_num*COLUMN_LEN;
		for(j=0; j<unit_num; j++)
		{
			if( !strcmp( unit_info_p[j].un_name, tmp) )
			{
				unit_info_p[j].day_count = atof( tmp+COLUMN_LEN );
			}
		}
	
	}

	printf("%-20s %-6s\n","机组名称", " 发电能力");
	for(j=0; j<unit_num; j++)
	{
		printf("%-20s %-10.3f\n", unit_info_p[j].un_name, unit_info_p[j].day_count);
	}
	printf("\n");
	print_curtline(desc);
	free(db_data);
//===========================================获取机组的发电能力信息结束====================================


	return 0;
}



/*
*函数功能:释放分配的内存空间
*
*
*/
int free_memory()
{
	long long int i;
	for( i=0; i<unit_num; i++)
	{
		free(unit_info_p[i].up_limit);
		free(unit_info_p[i].down_limit);
		free(unit_info_p[i].have_count);
		free(unit_info_p[i].on_off);
	}
	free(unit_info_p);
	free(load_forcast);
}
/*
*函数功能：初始化程序执行环境：初始化数据库链接，获取网损系数(loss_factor)和
*		   每日时段数(time_bucket)， 获取负荷预测值（load_forcast*)
*返回值：  初始化数据库链接失败返回-1，其余失败直接退出写程序执行信息，
*		   成功返回0
*
*/
int init_prog()
{
	
	char sql_string[256];
	int flag;
	long long i, j, row_num;
	int column_num;	
	
	

	printf("hello\n");
	if(init_database()!=0)
		return -1;
	
	
	print_curtline("获取机组爬坡能力和每日时段数");
	//=====================================================获取机组爬坡能力和每日时段数======================

	void *db_data;
	void *tmp;
	char tmp_date[20];
	


	sprintf(sql_string, "%s", "select sdate, sname, svalue from plan.plan.sys_parameter_config "
			"where sname in ('机组爬坡能力', '每日时段数')");
	column_num = 3;
	row_num = exec_select( sql_string, column_num, "获取每日时段数和机组爬坡能力数据", &db_data);	
	
	if(row_num <= 0)
		PERROR_CLOSEDATABASE_EXIT(-1, "取得每日时段数和机组爬坡能力错误，返回记录条数小于或者等于零")
	for(i=0; i<row_num; i++)
	{
		tmp = db_data+i*column_num*COLUMN_LEN;
		if(!strcmp("机组爬坡能力", tmp+COLUMN_LEN))
		{
			if( 0 > strcmp(date, tmp))
				PERROR_CLOSEDATABASE_EXIT(-1, "机组爬坡能力生效日期晚于今天的日期")
			creep_capacity = atof( (char*)(tmp+2*COLUMN_LEN) ) * ( 60*24/time_bucket );
		}
		if(!strcmp("每日时段数", tmp+COLUMN_LEN))
		{
			if( 0 > strcmp(date, tmp))
				PERROR_CLOSEDATABASE_EXIT(-1, "每日时段数生效日期晚于今天的日期")
			time_bucket = atoi( (char*)(tmp+2*COLUMN_LEN) );
		}
	}
	printf("每日时段数:%d，机组爬坡能力:%7.3f MW/min\n", time_bucket, creep_capacity);

	power_to_energy = time_bucket/24;	//每日时段数/24=每小时几个时段，用它作为电力到电能的转换系数
	
	free(db_data);

    power_have_max = (int*)malloc(sizeof(int)*time_bucket);
    init_int_arr(power_have_max, time_bucket, 0);
	print_curtline("获取机组爬坡能力和每日时段数结束");	
    //===============================================获取机组爬坡能力和每日时段数======================
	
	
	
	
	//=========================================================从数据库中取负荷预测值=====================
	print_curtline("获取负荷预测值");
	
	int sd_current = 0;
	

	load_forcast = (float*)malloc(sizeof(float)*time_bucket);
	printf("load_forcast用malloc 分配空间%d*%d=%d\n",sizeof(unsigned int), time_bucket, sizeof(unsigned int)*time_bucket);
	
	sprintf(sql_string, "select stime, svalue from plan.plan.loadfor "
			"where sdate='%s' and stype='系统负荷预测' order by stime",date);
	
	column_num = 2;
	row_num = exec_select( sql_string, column_num, "获取负荷预测值", &db_data);	
	
	if(time_bucket != row_num)
		PERROR_CLOSEDATABASE_EXIT(-1, "取得负荷预测值错误，返回的指定日期的负荷预测值数目不等于每天时段数")
	
	for(i=0; i<row_num; i++)
	{

		tmp = db_data+i*column_num*COLUMN_LEN;
		if( (sd_current + 1) != time2sd( tmp, time_bucket) )
			PERROR_CLOSEDATABASE_EXIT(-1, "取得负荷预测值错误，返回的指定日期的时段数据不正确")
		load_forcast[i] = atof( tmp+COLUMN_LEN);
		sd_current++;
	}
	free(db_data);
	printf("\n");
	printf("所查的日负荷预测值为\n");
	for(i=0; i<10; i++)
		printf("%-12lld ",i);
	printf("\n");
	for(i=0; i<time_bucket; i++)
		if(0 == (i+1)%10)
			printf("%-12.3f\n", load_forcast[i]);
		else
			printf("%-12.3f ",load_forcast[i]);
	printf("\n");
	print_curtline("获取负荷预测值结束");
//============================================获取负荷预测值结束=======================================

}
/*
*函数功能：打印unit_info所知向的结构数组
*		   
*
*/
int print_unit(char *mess)
{
	void *tmp;
	long long int i;
	int j;
	assert(unit_num>0);
	printf("++++++++++++++++++++++++++++++++++%s+++++++++++++++++++++++++++++++++++++++++\n\n",mess);
	for(i=0; i<unit_num; i++)
	{
		printf("%-25s %-10s %-10s\n", "机组名称", "定电量","已出力");
		printf("%-25s %-10.3f %-10.3f\n",
				unit_info_p[i].un_name, unit_info_p[i].day_count,
				unit_info_p[i].have_count_sum
				);
		printf("----------------------------------------------------\n");
		printf("%-10s %-10s %-10s\n", "时段号", "出力上限", "出力下限");
		for(j=0; j<time_bucket/2; j++)
		{
			printf("%-10u %-10.3f %-10.3f %-10u %-10.3f %-10.3f \n",
					j+1,unit_info_p[i].up_limit[j], unit_info_p[i].down_limit[j],
					j+time_bucket/2+1, unit_info_p[i].up_limit[j+time_bucket/2],
					unit_info_p[i].down_limit[j+time_bucket/2]);
		}
		printf("----------------------------------------------------\n");
		printf("此电厂已经分配的出力\n");
		printf("%9d",0);
		for(j=0; j<10; j++)
	        printf("%9d",j+1);
	    printf("\n\n");
	    for(j=0; j<time_bucket; j++)
		    if(0 == (j)%10)
			    printf("\n%9d %9.3f", j/10, unit_info_p[i].have_count[j]);
	        else
	            printf("%9.3f",unit_info_p[i].have_count[j]);
		printf("\n\n");
		printf("----------------------------------------------------\n");
	}
	printf("++++++++++++++++++++++++++++++++++%s+++++++++++++++++++++++++++++++++++++++++\n\n",mess);
	return 0;
}
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

int get_max(float *load_count, int len)
{
    int i, max_bucket;
    float tmp = 0;
    for(i=0; i<len; i++)
    {
        if( tmp < load_count[i] )
        {
            if(0 == power_have_max[i])
            {
                max_bucket = i;
                tmp = load_count[i];
            }
        }
    }
    return max_bucket;
}
