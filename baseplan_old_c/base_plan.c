#include"base_plan.h"
#include"assert.h"




int main(int argc, char *argv[])
{

    if(2 != argc)
    {
        printf("Please input the date as the parameter, \nsuch as %s 2013-08-13\n", argv[0]);
        return -1;
    }

    char *p = strrchr(argv[0],'/');
    strcpy(act_info.prog_id,p);
	strcpy(date, argv[1]);	



    if(init_prog())
    {
        sprintf(mess, "%s\n", "初始化程序执行所需数据失败");
        PERROR_CLOSEDATABASE_EXIT(-1, mess);
    }
    printf("$now=10\n");
    get_unit_info();
    printf("$now=20\n");

    get_limit();
    printf("$now=30\n");
    check();
    printf("$now=40\n");
    init_to_downlimit_power();
    printf("$now=50\n");
    print_unit("用最小出力初始化电厂出力后打印");

    cal_unit_gen();
    printf("$now=70\n");
    printf("\n\n\n");


   // write_loadforcast();
    write_unit_power();
    printf("$now=90\n");
    write_act_info(0,"程序执行成功");
    printf("$now=100\n");
	free_memory();	
    close_database();
	printf("\n\n");
    return 0;
}

int write_unit_power()
{
    char *sql_string;
    char sub_sql_str[MAX_BUFFER];
    char time[10];
    int tmp, i, j;


    //1.delete unit_electric_plan

    sql_string = (char*)malloc(MAX_BUFFER*time_bucket);
    sprintf(sub_sql_str, "delete %sunit_electric_plan "
            "where  sdate='%s'", db_name_str,date);

    tmp = exec_sql(sub_sql_str);
    if(-1 == tmp)
    {
        sprintf(mess, "%s\n", "删除原本(日计划)时句柄分配错误");
        PERROR_CLOSEDATABASE_EXIT(-1, mess);
    }
    else if( -2 == tmp)
    {
        sprintf(mess, "%s\n", "删除原本(日计划)时语句执行错误");
        PERROR_CLOSEDATABASE_EXIT(-1, mess);
    }


    //2.insert generated plan

    for(j=0; j<unit_num; j++)
    {
        for(i=0; i<time_bucket; i++)
        {

            if(bucket_to_time(i+1, time_bucket, time, 10))
            {
                sprintf(mess, "%s\n","bucket_to_time转换错误");
                PERROR_CLOSEDATABASE_EXIT(-1, mess);
            }

            if(0 == i)
            {
                sprintf(sub_sql_str,
                        "insert into %sunit_electric_plan "
                        "(sdate,stime,stype,sname,plan_value)\n "
                        "select '%s','%s','%s','%s','%f'\n ",
                        db_name_str, date, time,"日计划",
                        unit_info_p[j].un_name, unit_info_p[j].have_count[i]);
                sql_string[0] = 0;
            }
            else
            {
                sprintf(sub_sql_str,
                        "union all select '%s','%s','%s','%s','%f'\n ",
                        date, time, "日计划",unit_info_p[j].un_name, unit_info_p[j].have_count[i]);
            }

            strcat(sql_string, sub_sql_str);
        }//end for loop i

        printf("sql_len:%d\n", strlen(sql_string));
        tmp = exec_sql(sql_string);
        if(-1 == tmp)
        {
            sprintf(mess, "%s\n", "插入(日计划)时句柄分配错误");
            PERROR_CLOSEDATABASE_EXIT(-1, mess);
        }
        else if( -2 == tmp)
        {
            sprintf(mess, "%s\n", "插入(日计划)时语句执行错误");
            PERROR_CLOSEDATABASE_EXIT(-1, mess);
        }
    }//end for loop j



    //3.insert modified plan

    for(j=0; j<unit_num; j++)
    {
        for(i=0; i<time_bucket; i++)
        {

            if(bucket_to_time(i+1, time_bucket, time, 10))
            {
                sprintf(mess, "%s\n","bucket_to_time转换错误");
                PERROR_CLOSEDATABASE_EXIT(-1, mess);
            }

            if(0 == i)
            {
                sprintf(sub_sql_str,
                        "insert into %sunit_electric_plan "
                        "(sdate,stime,stype,sname,plan_value)\n "
                        "select '%s','%s','%s','%s','%f'\n ",
                        db_name_str, date, time,"手工修正计划",
                        unit_info_p[j].un_name, unit_info_p[j].have_count[i]);
                sql_string[0] = 0;
            }
            else
            {
                sprintf(sub_sql_str,
                        "union all select '%s','%s','%s','%s','%f'\n ",
                        date, time, "手工修正计划",unit_info_p[j].un_name, unit_info_p[j].have_count[i]);
            }

            strcat(sql_string, sub_sql_str);
        }//end for loop i

        printf("sql_len:%d\n", strlen(sql_string));
        tmp = exec_sql(sql_string);
        if(-1 == tmp)
        {
            sprintf(mess, "%s\n", "插入(手工修正计划)时句柄分配错误");
            PERROR_CLOSEDATABASE_EXIT(-1, mess);
        }
        else if( -2 == tmp)
        {
            sprintf(mess, "%s\n", "插入(手工修正计划)时语句执行错误");
            PERROR_CLOSEDATABASE_EXIT(-1, mess);
        }
    }//end for loop j


    //4.delete plant_electric_plan

    sprintf(sql_string,"delete from %splant_electric_plan "
            "where sdate='%s'",
           db_name_str, date);
    tmp = exec_sql(sql_string);
    if(-1 == tmp)
    {
        sprintf(mess, "%s\n", "删除(日前电厂计划)时句柄分配错误");
        PERROR_CLOSEDATABASE_EXIT(-1, mess);
    }
    else if( -2 == tmp)
    {
        sprintf(mess, "%s\n", "删除(日前电厂计划)时语句执行错误");
        PERROR_CLOSEDATABASE_EXIT(-1, mess);
    }

    //5.insert plant_electric_plan


    sprintf(sql_string, "insert into %splant_electric_plan (sdate,stime,stype,sname,plan_value) "
            "select '%s' as sdate,B.stime,'%s' as stype, "
            "A.sstation as sname,sum(plan_value) as plan_value "
            "from %sunit_info as A, %sunit_electric_plan as B "
            "where A.sname=B.sname and B.stype='%s' "
            "group by A.sstation,B.stime ",
           db_name_str, date,"日计划",db_name_str, db_name_str,"手工修正计划");
    tmp = exec_sql(sql_string);
    if(-1 == tmp)
    {
        sprintf(mess, "%s\n", "插入(日计划)时句柄分配错误");
        PERROR_CLOSEDATABASE_EXIT(-1, mess);
    }
    else if( -2 == tmp)
    {
        sprintf(mess, "%s\n", "插入(日计划)时语句执行错误");
        PERROR_CLOSEDATABASE_EXIT(-1, mess);
    }



    //6.insert modified plant_electric_plan


    sprintf(sql_string, "insert into %splant_electric_plan (sdate,stime,stype,sname,plan_value) "
            "select '%s' as sdate,B.stime,'%s' as stype, "
            "A.sstation as sname,sum(plan_value) as plan_value "
            "from %sunit_info as A, %sunit_electric_plan as B "
            "where A.sname=B.sname and B.stype='%s' "
            "group by A.sstation,B.stime ",
           db_name_str, date,"手工修正计划",db_name_str, db_name_str,"手工修正计划");
    tmp = exec_sql(sql_string);
    if(-1 == tmp)
    {
        sprintf(mess, "%s\n", "插入(手工修正计划)时句柄分配错误");
        PERROR_CLOSEDATABASE_EXIT(-1, mess);
    }
    else if( -2 == tmp)
    {
        sprintf(mess, "%s\n", "插入(手工修正计划)时语句执行错误");
        PERROR_CLOSEDATABASE_EXIT(-1, mess);
    }

    free(sql_string);
    return 0;
}
/**
 * @synopsis  write_loadforcast 
 *
 * @returns   
 */
int write_loadforcast()
{
    char sql_string[MAX_BUFFER];
    char time[10];
    int tmp, i;

    sprintf(sql_string, "delete %sloadfor where stype='系统负荷预测' and sdate='%s'", db_name_str, date);
    tmp = exec_sql(sql_string);
    if(-1 == tmp)
    {
        sprintf(mess, "%s\n", "插入负荷预测值时句柄分配错误");
        PERROR_CLOSEDATABASE_EXIT(-1, mess);
    }
    else if( -2 == tmp)
    {
        sprintf(mess, "%s\n", "插入负荷预测值时语句执行错误");
        PERROR_CLOSEDATABASE_EXIT(-1, mess);
    }
    for(i=0; i<time_bucket; i++)
    {

        if(bucket_to_time(i+1, time_bucket, time, 10))
        {
            sprintf(mess, "%s\n","bucket_to_time转换错误");
            PERROR_CLOSEDATABASE_EXIT(-1, mess);
        }
        sprintf(sql_string,
                "insert into %sloadfor "
                "values('%s','%s','系统负荷预测','%6.3f');",
                db_name_str, date,
                time, load_forcast[i]);
        tmp = exec_sql(sql_string);
        if(-1 == tmp)
        {
            sprintf(mess, "%s\n", "插入负荷预测值时句柄分配错误");
            PERROR_CLOSEDATABASE_EXIT(-1, mess);
        }
        else if( -2 == tmp)
        {
            sprintf(mess, "%s\n", "插入负荷预测值时语句执行错误");
            PERROR_CLOSEDATABASE_EXIT(-1, mess);
        }
    }
    return 0;
}



/**
 * @brief check 校验数据的有效性，机组最小出力之和大于定电量，机组最大出力小于定电量，都为无效数据
 * @return
 */
int check()
{
    int i,j;
    float sum_downlimit = 0, sum_uplimit = 0;
	unsigned int sum_time;
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
            sprintf(mess, "机组名称：%s，此电厂的各时段最小出力之和 (%-10.3f) 大于日定电量的出力值 (%-10.3f*%d)",
                unit_info_p[i].un_name, sum_downlimit, unit_info_p[i].day_count, power_to_energy);

            PERROR_CLOSEDATABASE_EXIT(-1,mess);
        }//end if
		if( sum_uplimit < unit_info_p[i].day_count*power_to_energy)
		{
		
			memset(mess, '\0', sizeof(mess) );
            sprintf(mess, "机组名称：%s，此电厂的各时段最大出力之和(%-10.3f)小于日定电量的出力值(%-10.3f*%d)",
                unit_info_p[i].un_name, sum_uplimit, unit_info_p[i].day_count, power_to_energy);
            PERROR_CLOSEDATABASE_EXIT(-1,mess);
		}//end if
	}//end for i

    sum_downlimit = 0;
    for(i=0; i<time_bucket; i++)
    {
        for(j=0; j<unit_num; j++)
        {
           sum_downlimit += unit_info_p[j].down_limit[i];
        }
        if(sum_downlimit > load_forcast[i])
        {
            sprintf(mess, "在时段(%s),各机组的最小出力之和(%10.3f)大于此时段的负荷预测值(%10.3f)",
                    bucket2time(i+1, time_bucket), sum_downlimit, load_forcast[i]);
            PERROR_CLOSEDATABASE_EXIT(-1,mess);
        }
        sum_downlimit = 0;
    }


    //compare between sum of loadforcast and sum of daycount of all unit
    for(i=0; i<time_bucket; i++)
        sum_loadforcat += load_forcast[i];

    for(i=0; i<unit_num; i++)
        sum_daycount += unit_info_p[i].day_count;
    if( sum_loadforcat < sum_daycount*power_to_energy)
    {
        sprintf(mess, "各机组的日定电量之和（%10.3f*%d)大于负荷预测手工修正值之和（%10.3f）",
                sum_daycount, power_to_energy, sum_loadforcat);
        PERROR_CLOSEDATABASE_EXIT(-1,mess);
    }

    return 0;
}

/**
 * @brief check_creep 检查此点加一后是否满足爬坡约束
 * @param current 该机组在该时段出力值的指针
 * @param front 前一点的出力值
 * @param back 后一点的出力值
 * @return 返回1代表增1后满足爬坡和下坡速率，返回0代表不满足
 */
int check_creep(float *current, float front, float back)
{
    int bing_go = 0;
    float differ1, differ2;


    differ1 = abs(*current+1-front);
    differ2 = abs(*current+1-back);
    if( front > back)
    {
        if(*current+1 > front)
        {
            if( creep_capacity >= differ1 && down_capacity >= differ2)
                bing_go = 1;
        }
        else if( (*current+1 > back && *current+1 < front) || *current+1 == front )
        {
            if( down_capacity >= differ2)
                bing_go = 1;
        }
        else// ( *current+1 <= back)
            bing_go = 1;

    }
    else if( front < back)
    {
        if( *current+1 > back)
        {
            if( creep_capacity >= differ1 && down_capacity >= differ2)
                bing_go = 1;
        }
        else if( (*current+1 < back && *current+1 > front) || *current+1 == back)
        {
            if( creep_capacity >= differ1)
                bing_go = 1;
        }
        else// *current+1<=front
            bing_go = 1;

    }
    else//equals
    {
        if( *current+1 > front)
        {
            if( creep_capacity >= differ1 && down_capacity >= differ2)
                bing_go = 1;
        }
        else
            bing_go = 1;
    }
    return bing_go;
}

int is_all_max( int max_bucket)
{
    int i;
    int flag = 0;
    for(i=0; i< unit_num; i++)
    {
        flag += unit_info_p[i].done[max_bucket];
    }
    if( flag == unit_num)
    {
        power_have_max[max_bucket] = 1;
    }
    return 0;
}

/**
 * @brief cal_unit_gen 计算个机组的出力
 * @return 0找到了分配出力的解，-1没有找到可行解
 */
int cal_unit_gen()
{
    int alldone_sum = 0;
	//最大负荷点的时段值
	int max_bucket;
	//循环变量
    int i, j, k;
    //如果某一项为0代表此机组还没有升出力结束(或者达到最大出力，或者达到定电量)
    //如果为1代表升出力已经结束
    int *done;
    //如果某一机组已经达到日定电量，则其相应的all_done位置为1
    int *all_done;
    //找到的最大负荷时段中每个机组的升出力值
    float *asc;
    float front, back, *current;
	int done_sum = 0;

    loop = 0;//寻找max_buck的次数，大于一个上限就认为找不到解
    int bing_go;//是否升出力了


	
    done = (int*) malloc(sizeof(int)*unit_num);
    all_done = (int*)malloc( sizeof(int)*unit_num);
    asc = (float*)malloc(sizeof(float)*unit_num);
    if( NULL == done || NULL == all_done || NULL == asc)
    {
        sprintf(mess, "%s\n", "申请程序运行所需空间安done,all_done,asc时出现错误");
        PERROR_CLOSEDATABASE_EXIT(-1, mess);
    }


    init_int_arr(done, unit_num, 0);
    init_int_arr(all_done, unit_num, 0);
    init_float_arr(asc,unit_num,0.0);
    float up_limit = 0;
    max_bucket = get_limit_max(load_forcast, time_bucket);
    while(unit_num != alldone_sum)
	{
        loop++;

        //要么就是达到日定电量，要么是达到最大出力
        //在此处修改负荷曲线可以避免重复减去最小出力，因为在一个时段不可能在此处执行两次
        for(j=0; j<unit_num; j++)
            load_forcast[max_bucket] -= asc[j];
        init_float_arr(asc,unit_num,0.0);
        max_bucket = get_limit_max(load_forcast, time_bucket);
        if(-1 == max_bucket)
        {
            char tmp_mess[MAX_BUFFER];
            char all_mess[10*MAX_BUFFER];
            memset(mess, MAX_BUFFER, 1);
            for(k=0; k<unit_num; k++)
            {
                sprintf(tmp_mess, "(%s的日定电量%f)", unit_info_p->un_name, unit_info_p[k].day_count);
                strcat(all_mess, tmp_mess);
            }
            sprintf(tmp_mess, "\n 日定电量之和（%10.3f），手工修正负荷预测之和（%10.3f）", sum_daycount, sum_loadforcat);
            strcat(all_mess,tmp_mess);
            sprintf(tmp_mess,"\n 爬坡能力（%10.3f），下坡能力(%10.3f)", creep_capacity, down_capacity);
            strcat(all_mess, tmp_mess);
            fprintf(stderr,"%s\n", all_mess);
            sprintf(mess, "%s\n", "获取负荷预测值最大值出错，在给定的约束条件（各机组的定电量，负荷预测值，爬坡约束）下无解");
            PERROR_CLOSEDATABASE_EXIT(-1, mess);
        }
        done_sum = 0;
        init_int_arr(done, unit_num, 0);



//        printf("max_bucket=%d\n",max_bucket);
        for(i=0; i<unit_num; i++)
        {
            up_limit   = unit_info_p[i].up_limit[max_bucket];


//            printf("unit_info_p[%d].have_count_sum=%f,unit_info_p[%d].day_count=%f,\n"
//                   "unit_info_p[%d].have_count[%d]=%f,up_limit=%f\n"
//                   "unit_info_p[%d].have_count[%d]=%f,unit_info_p[%d].have_count[%d]=%f\n",
//                   i, unit_info_p[i].have_count_sum, i, unit_info_p[i].day_count,
//                   i, max_bucket, unit_info_p[i].have_count[max_bucket],up_limit,
//                   i,max_bucket-1,unit_info_p[i].have_count[max_bucket-1],
//                    i,max_bucket+1, unit_info_p[i].have_count[max_bucket+1]);

            //如果为关机状态则不考虑此机组的升出力
            if(0 == unit_info_p[i].on_off[max_bucket])
            {
                done[i] = 1;
                unit_info_p[i].done[max_bucket] = 1;
                //continue;
            }
            if(unit_info_p[i].have_count_sum < unit_info_p[i].day_count*power_to_energy &&
                                unit_info_p[i].have_count[max_bucket] < up_limit)
            {
                bing_go = 0;
                if( 0 == max_bucket)
                {
                    front = unit_info_p[i].have_count[max_bucket];
                    back = unit_info_p[i].have_count[max_bucket+1];
                    current = &(unit_info_p[i].have_count[max_bucket]);
                }
                else if( time_bucket-1 == max_bucket)
                {
                    front = unit_info_p[i].have_count[max_bucket-1];
                    back = unit_info_p[i].have_count[max_bucket];
                    current = &(unit_info_p[i].have_count[max_bucket]);
                }
                else{
                    front = unit_info_p[i].have_count[max_bucket-1];
                    back = unit_info_p[i].have_count[max_bucket+1];
                    current = &(unit_info_p[i].have_count[max_bucket]);
                }
                bing_go = check_creep(current, front, back);
                if(1 == bing_go)
                {
                    asc[i] += 1;
                    unit_info_p[i].have_count[max_bucket] += 1;
                    unit_info_p[i].have_count_sum += 1;
                }
                else
                {
                    done[i] = 1;
                    //break;//jump out of while
                }

            }//end for if
            if( unit_info_p[i].have_count_sum >= unit_info_p[i].day_count*power_to_energy)
            {
                all_done[i] = 1;
                done[i] = 1;
                init_int_arr(unit_info_p[i].done, time_bucket, 1);
            }
            if( unit_info_p[i].have_count[max_bucket] >= up_limit)
            {
                done[i] = 1;
                unit_info_p[i].done[max_bucket] = 1;
            }
        }//end for loop i

        done_sum = 0;
        for(j=0; j<unit_num; j++)
            done_sum += done[j];
        alldone_sum = 0;
        for(j=0; j<unit_num; j++)
        {
            alldone_sum += all_done[j];
        }

	}//end for while
    print_unit("定电量算法运行后打印");
//    printf("\n\n\n\n\n\n\n\nloop:%d\n\n\n\n\n\n\n\n",loop);
	free(done);
    free(all_done);
    free(asc);
    return 0;
}//end for cal_unit_gen


/**
 * @brief init_to_downlimit_power 把各机组的出力初始化为其最小出力
 * @return
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

/**
 * @brief get_limit 获取电厂出力的上下限，电厂一天可能有几个不同的处理上下限
 * @return
 */
int get_limit()
{


    sprintf(mess, "%s", "获取机组的出力上下限");
	printf("\n");
    print_curtline(mess);
	
    char sql_string[MAX_BUFFER];
	long long int row_num = 0, i;
	void *db_data;
	void *tmp;
	int column_num = 5, j, k;

	sprintf(sql_string,"select stime, sname, top_limit, low_limit "
            "from %sunit_electric_limit where sdate='%s'", db_name_str, date);
    row_num = exec_select(sql_string, column_num, mess, &db_data);
	if(row_num <= 0)
    {
        sprintf(mess, "%s", "获取机组的出力上下限,返回的结果记录条目数小于或者等于0");
        PERROR_CLOSEDATABASE_EXIT(-1, mess);
    }
    for(i=0; i<row_num; i++)
	{
		tmp = db_data+i*column_num*COLUMN_LEN;
		for(j=0; j<unit_num; j++)
		{
			if( !strcmp( unit_info_p[j].un_name, tmp+COLUMN_LEN) )
			{

                k = time2bucket( tmp, time_bucket );
                float up = atof(tmp+2*COLUMN_LEN), down = atof(tmp+3*COLUMN_LEN);
                unit_info_p[j].count++;
                if(up < down || down < 0)
                {
                    sprintf(mess, "获取机组的出力上下限时机组(%s),(%s)时段的出力上下限不正常(下限大于上限"
                            "或者下限小于零)", unit_info_p[j].un_name, bucket2time(k,time_bucket));
                    PERROR_CLOSEDATABASE_EXIT(-1, mess);
                }
                //off
                if( 0 == unit_info_p[j].on_off[k-1])
                {
                    unit_info_p[j].up_limit[k-1] = 0.0;
                    unit_info_p[j].down_limit[k-1] = 0.0;
                }
                else
                {
                    unit_info_p[j].up_limit[k-1] = up;
                    unit_info_p[j].down_limit[k-1] = down;
                }
			}
		}
	}

    for(i=0; i<unit_num; i++)
    {
        if( unit_info_p[i].count != 96 )
        {
            sprintf(mess, "获取机组的出力上下限时机组(%s)的出力上下限数目(%d)不等于每日时段数(%d)\n",
                    unit_info_p[i].un_name, unit_info_p[i].count, time_bucket);
            PERROR_CLOSEDATABASE_EXIT(-1, mess);
        }
        unit_info_p[i].count = 0;
    }

	free(db_data);	
    print_curtline(mess);
	printf("\n");
	return 0;	
}

/**
 * @brief get_unit_info 获得机组的机组名称，开停机信息,构造结构unit_info数组
 * @return 成功返回0，失败直接退出程序，并且向数据库中写程序执行信息
 */
int get_unit_info()
{
	

    sprintf(mess, "%s", "获取开机机组名称信息");
    print_curtline(mess);
	
    char sql_string[MAX_BUFFER];
	long long i, row_num;
    int  j, column_num, k;
	void *db_data, *tmp;

    sprintf(sql_string, "select distinct sname from %sunit_start_shut"
            " where sdate='%s' and status='开机'", db_name_str, date);
	
	column_num = 1;
    row_num = exec_select( sql_string, column_num, mess, &db_data);
	
    sprintf(mess, "%s", "获取开机机组名称信息返回记录条数小于0");
	if(row_num <= 0)
        PERROR_CLOSEDATABASE_EXIT(-1,mess);
	unit_num = row_num;	
    unit_info_p = (unit_info*)malloc( row_num*sizeof(unit_info) );
    if( NULL == unit_info_p)
    {
        sprintf(mess, "%s", "分配机组信息数据结构所需空间为空");
        PERROR_CLOSEDATABASE_EXIT(-1,mess);
    }
    for(i=0; i<unit_num; i++)
	{
        unit_info_p[i].up_limit = (float*)malloc( time_bucket*sizeof(float) );
        unit_info_p[i].down_limit = (float*)malloc( time_bucket*sizeof(float) );
        unit_info_p[i].have_count = (float*)malloc( time_bucket*sizeof(float) );
        unit_info_p[i].on_off = (int*)malloc( time_bucket*sizeof(int) );
        unit_info_p[i].done = (int*)malloc( time_bucket*sizeof(int));

        if( NULL == unit_info_p[i].up_limit ||
                NULL == unit_info_p[i].down_limit||
                NULL == unit_info_p[i].have_count||
                NULL == unit_info_p[i].on_off||
                NULL == unit_info_p[i].done)
        {
            sprintf(mess, "%s", "分配机组信息内部数据结构所需空间为空");
            PERROR_CLOSEDATABASE_EXIT(-1,mess);
        }
		unit_info_p[i].day_count = 0.0;
		unit_info_p[i].have_count_sum = 0.0;
		init_float_arr( unit_info_p[i].up_limit, time_bucket, 0.0);
		init_float_arr( unit_info_p[i].down_limit, time_bucket, 0.0);
		init_float_arr( unit_info_p[i].have_count, time_bucket, 0.0);
        init_int_arr( unit_info_p[i].on_off, time_bucket, 0);
        init_int_arr( unit_info_p[i].done, time_bucket, 0);
        unit_info_p[i].count = 0;

		tmp = db_data+i*column_num*COLUMN_LEN;
	
		strcpy( unit_info_p[i].un_name, tmp );
	
	}
    printf("%s\n", "一天当中存在开机时段的所有机组：");
	for(i=0; i<row_num; i++)
	{
		if(0 == i%4)
			printf("\n");
		printf("%-20s", unit_info_p[i].un_name);
	}
	printf("\n");
	free(db_data);
    print_curtline(mess);
//============================================获取机组的名称信息结束=======================================


//============================================获取机组开停机状态===========================================
    sprintf(mess,"%s","获取机组开停机状态");
    print_curtline(mess);

    sprintf(sql_string, "select stime, sname, status from %sunit_start_shut "
            "where sdate='%s'", db_name_str, date);
	
    column_num = 3;
    row_num = exec_select( sql_string, column_num, mess, &db_data);
	
	if(row_num <= 0)
    {
        sprintf(mess,"%s","获取机组开停机状态,返回的记录条目数小于或者等于0");
        PERROR_CLOSEDATABASE_EXIT(-1, mess);
    }
	
	for(i=0; i<row_num; i++)
	{
		tmp = db_data+i*column_num*COLUMN_LEN;
		for(j=0; j<unit_num; j++)
		{
			if( !strcmp( unit_info_p[j].un_name, tmp+COLUMN_LEN) )
			{
                unit_info_p[j].count++;
                if( !strcmp("开机", tmp+2*COLUMN_LEN))
                {
                    k = time2bucket( tmp, time_bucket );
                    unit_info_p[j].on_off[k-1] = 1;
                }
                else
                {
                    k = time2bucket( tmp, time_bucket );
                    unit_info_p[j].on_off[k-1] = 0;
                }
			}
        }//end for loop j
	
    }//end for loop i

    for(i=0; i<unit_num; i++)
    {
        if( unit_info_p[i].count != 96)
        {
            sprintf(mess,"获取机组开停机状态,机组(%s)的开停机记录条目数(%d)不等于每日时段数(%d)\n",
                    unit_info_p[i].un_name, unit_info_p[i].count, time_bucket);
            PERROR_CLOSEDATABASE_EXIT(-1, mess);
        }
        unit_info_p[i].count = 0;
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
    print_curtline(mess);
//===========================================获取机组开停机状态结束========================================	

//===========================================获取机组的发电能力信息========================================
    sprintf(mess, "%s", "获取机组的发电能力信息");
    print_curtline(mess);

    sprintf(sql_string, "select sname, electric from %sunit_electric_ability "
            " where sdate='%s'", db_name_str, date);

	column_num = 2;
    row_num = exec_select( sql_string, column_num, mess, &db_data);
	
	if(row_num <= 0)
    {
        sprintf(mess, "%s", "获取机组的发电能力信息,返回的记录条目数小于或者等于0");
        PERROR_CLOSEDATABASE_EXIT(-1, mess);
    }
	
	for(i=0; i<row_num; i++)
	{
		tmp = db_data+i*column_num*COLUMN_LEN;
		for(j=0; j<unit_num; j++)
		{
			if( !strcmp( unit_info_p[j].un_name, tmp) )
			{
				unit_info_p[j].day_count = atof( tmp+COLUMN_LEN );
                unit_info_p[j].count++;
                if( 0 >= unit_info_p[j].day_count)
                {
                    sprintf(mess, "获取机组的发电能力信息,数据库中机组(%s)的发电能力(%f)小于或者等于0\n",
                            unit_info_p[j].un_name, unit_info_p[j].day_count);
                    PERROR_CLOSEDATABASE_EXIT(-1, mess);
                }
			}
		}
	
	}

    for(i=0; i< unit_num; i++)
    {
        if(0 >= unit_info_p[i].count)
        {
            sprintf(mess, "获取机组的发电能力信息,数据库中机组(%s)的发电能力信息不存在\n", unit_info_p[i].un_name);
            PERROR_CLOSEDATABASE_EXIT(-1, mess);
        }
        unit_info_p[i].count = 0;
    }

	printf("%-20s %-6s\n","机组名称", " 发电能力");
	for(j=0; j<unit_num; j++)
	{
		printf("%-20s %-10.3f\n", unit_info_p[j].un_name, unit_info_p[j].day_count);
	}
	printf("\n");
    print_curtline(mess);
	free(db_data);
//===========================================获取机组的发电能力信息结束====================================


	return 0;
}


/**
 * @brief free_memory 释放分配的内存空间
 */
void free_memory()
{
	long long int i;
	for( i=0; i<unit_num; i++)
	{
		free(unit_info_p[i].up_limit);
		free(unit_info_p[i].down_limit);
		free(unit_info_p[i].have_count);
		free(unit_info_p[i].on_off);
        free(unit_info_p[i].done);
	}
	free(unit_info_p);
	free(load_forcast);
}

/**
 * @brief init_prog 初始化程序执行环境：
 *初始化数据库链接，获取机组爬坡速率、机组下坡速率
 *每日时段数(time_bucket)， 获取负荷预测值（load_forcast*)
 * @return 初始化数据库链接失败返回-1，其余失败直接退出写程序执行信息，
 *		   成功返回0
 */
int init_prog()
{
	
    char sql_string[MAX_BUFFER];
    long long i,  row_num;
	int column_num;	
    char creep_speed_str[50], down_speed_str[50], time_bucket_str[50];
    int bucket_current = 0;

    void *db_data;
    void *tmp;
	
	
	if(init_database()!=0)
    {
//        PERROR_CLOSEDATABASE_EXIT(-1, "初始化数据库失败");
        fprintf(stderr,"初始化数据库失败,in file:%s,line:%d\n", __FILE__, __LINE__);
        exit(-1);
    }

    print_curtline("获取每日时段数");
    //=====================================================获取每日时段数============================
    sprintf(time_bucket_str, "%s", "每日时段数");
    sprintf(sql_string, "select sdate, sname, svalue from %ssys_parameter_config "
            "where sname='%s'", db_name_str, time_bucket_str);
    column_num = 3;
    row_num = exec_select( sql_string, column_num, "获取每日时段数", &db_data);

    if(row_num <= 0)
        PERROR_CLOSEDATABASE_EXIT(-1, "取得每日时段数，返回记录条数小于或者等于零");

    if( 0 > strcmp(date, db_data))
        PERROR_CLOSEDATABASE_EXIT(-1, "每日时段数生效日期晚于今天的日期");
                printf("shiduan\n");
   // printf("%s,%s,%s\n",db_data,db_data+COLUMN_LEN,db_data+COLUMN_LEN);
    time_bucket = atoi( (char*)(db_data+2*COLUMN_LEN) );

    free(db_data);
    printf("每日时段数：%d\n",time_bucket);
    print_curtline("获取每日时段数");
    //=====================================================获取每日时段数============================



    print_curtline("获取机组爬坡能力");
    //=====================================================获取机组爬坡能力===========================

    sprintf(creep_speed_str, "%s", "机组爬坡速率");
    sprintf(down_speed_str, "%s", "机组下坡速率");


    sprintf(sql_string, "select sdate, sname, svalue from %ssys_parameter_config "
            "where sname in ('%s', '%s')", db_name_str, creep_speed_str, down_speed_str);
    column_num = 3;
	row_num = exec_select( sql_string, column_num, "获取每日时段数和机组爬坡能力数据", &db_data);	
	
    if(row_num <= 0 || 2!=row_num)
        PERROR_CLOSEDATABASE_EXIT(-1, "取得每日时段数和机组爬坡能力错误，返回记录条数小于或者等于零或者部位2");
	for(i=0; i<row_num; i++)
	{
		tmp = db_data+i*column_num*COLUMN_LEN;
        printf("%s,%s\n",(char*)(tmp+COLUMN_LEN),(char*)(tmp+2*COLUMN_LEN));
        if(!strncmp(creep_speed_str, tmp+COLUMN_LEN, strlen(creep_speed_str)))
		{
			if( 0 > strcmp(date, tmp))
                PERROR_CLOSEDATABASE_EXIT(-1, "机组爬坡速率生效日期晚于今天的日期");
            creep_capacity = atof( (char*)(tmp+2*COLUMN_LEN) ) * ( 60*24/time_bucket );
		}
        if(!strncmp(down_speed_str, tmp+COLUMN_LEN, strlen(down_speed_str)))
        {
            if( 0 > strcmp(date, tmp))
                PERROR_CLOSEDATABASE_EXIT(-1, "机组下坡速率生效日期晚于今天的日期");
                        printf("xiapo\n");
            down_capacity = atof( (char*)(tmp+2*COLUMN_LEN) ) * ( 60*24/time_bucket );
        }
	}
    printf("每日时段数:%d，机组爬坡速率:%7.3f MW/bucket，机组下坡速率：%7.3f MW/bucket\n",
           time_bucket, creep_capacity, down_capacity);

    power_to_energy = time_bucket/24;
	
	free(db_data);
	print_curtline("获取机组爬坡能力和每日时段数结束");	
    //===============================================获取机组爬坡能力====================================
	
	
	
	
	//=========================================================从数据库中取负荷预测值=====================
    print_curtline("获取负荷预测值");


	load_forcast = (float*)malloc(sizeof(float)*time_bucket);
    if( NULL == load_forcast)
    {
        PERROR_CLOSEDATABASE_EXIT(-1,"申请load_forcast错误");
    }
	printf("load_forcast用malloc 分配空间%d*%d=%d\n",sizeof(unsigned int), time_bucket, sizeof(unsigned int)*time_bucket);
	
    sprintf(sql_string, "select stime, svalue from %sloadfor "
            "where sdate='%s' and stype='手工修正负荷预测' order by stime", db_name_str, date);
	
	column_num = 2;
	row_num = exec_select( sql_string, column_num, "获取负荷预测值", &db_data);	
	
	if(time_bucket != row_num)
    {
        sprintf(mess,"取得负荷预测值错误，返回的指定日期的负荷预测值数目%lld不等于每天时段数%d\n", row_num, time_bucket);
        PERROR_CLOSEDATABASE_EXIT(-1, mess);
    }
	for(i=0; i<row_num; i++)
	{

		tmp = db_data+i*column_num*COLUMN_LEN;
        if( (bucket_current + 1) != time2bucket( tmp, time_bucket) )
            PERROR_CLOSEDATABASE_EXIT(-1, "取得负荷预测值错误，返回的指定日期的时段数据不正确");
		load_forcast[i] = atof( tmp+COLUMN_LEN);
        if( load_forcast[i] < 0)
        {
            sprintf(mess,"取得负荷预测值错误，时段(%s)的负荷预测值小于0\n", bucket2time(i+1, time_bucket));
            PERROR_CLOSEDATABASE_EXIT(-1, mess);
        }
        bucket_current++;
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
    return 0;

}


/**
 * @brief print_unit 打印unit_info所知向的结构数组
 * @param mess 打印unit_info结构前打印的说明信息
 * @return
 */
void print_unit(char *mess)
{
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
}
int print_forcast()
{
    long long i;
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
    return 0;
}

/**
* @brief get_limit_max 把所有的约束考虑进去进行最大值的查找
* @param load_count 负荷预测数组
* @param len 负荷预测数组长度
* @return 返回找到满足条件的负荷预测的最大值点的位置,都不满足条件，返回-1
*/
int get_limit_max(float *load_count, int len)
{
    int i, j, max_bucket = -1;
    float tmp = FLOAT_MIN ;
    float *current, front, back;
    for(i=0; i<len; i++)
    {
        //search the max
        if( tmp < load_count[i] && load_count[i] > 0)
        {
            for(j=0; j<unit_num; j++)
            {
                if(unit_info_p[j].have_count_sum < unit_info_p[j].day_count*power_to_energy
                        && unit_info_p[j].have_count[i] < unit_info_p[j].up_limit[i])
                {
                    if(0 != i && (len-1) != i)
                    {
                        current = &(unit_info_p[j].have_count[i]);
                        front   = unit_info_p[j].have_count[i-1];
                        back    = unit_info_p[j].have_count[i+1];
                        if( check_creep(current, front, back) )
                        {
                            max_bucket = i;
                            tmp = load_count[i];
                            break;
                        }
                    }
                    else if(0 == i)
                    {
                        current = &(unit_info_p[j].have_count[i]);
                        front   = unit_info_p[j].have_count[i];
                        back    = unit_info_p[j].have_count[i+1];
                        if( check_creep(current, front, back) )
                        {
                            max_bucket = i;
                            tmp = load_count[i];
                            break;
                        }
                    }
                    else if(len-1 == i)
                    {
                        current = &(unit_info_p[j].have_count[i]);
                        front   = unit_info_p[j].have_count[i-1];
                        back    = unit_info_p[j].have_count[i];
                        if( check_creep(current, front, back) )
                        {
                            max_bucket = i;
                            tmp = load_count[i];
                            break;
                        }
                    }
                }
            }//end for loop j
        }//end if
    }//end for loop i


    return max_bucket;
}

/**
 * @brief get_max 获取数组最大元素
 * @param load_count
 * @param mask
 * @param len
 * @return
 */
int get_max(float *load_count, int* mask, int len)
{
    int i, max_bucket;
    float tmp = 0;
    for(i=0; i<len; i++)
    {
        if( tmp < load_count[i] )
        {
            if(0 == power_have_max[i] && 0 == mask[i])
            {
                max_bucket = i;
                tmp = load_count[i];
            }
        }
    }
    return max_bucket;
}

/**
 * @brief get_k_max 获取前k大的数在数组中的位置
 * @param load_count 数组
 * @param len 数组长度
 * @param k 第k大的数
 * @return 返回查到的数在数组中的位置
 */
int get_k_max(float *load_count, int len, int k)
{
    int *mask = (int*)malloc( sizeof(int)*time_bucket );
    init_int_arr(mask, time_bucket, 0);
    int i, max_sd;
    for(i=0; i<k; i++)
    {
        max_sd = get_max(load_count, mask, len);
        mask[max_sd] = 1;
    }
    free(mask);
    return max_sd;
}
/**
 * @brief get_ran_max 获取头random_top中的随机值
 * @param load_count
 * @param len
 * @return
 */
int get_ran_max(float *load_count, int len)
{
    int num = 0;
    int ret;

    num = rand()%random_top+1;//range from 1 to random_top
    ret = get_k_max(load_count, len, num);
    printf("k=%d,max_k=%f,location is:%d\n",num,load_count[ret],ret);
    return ret;
}

/**
 * @brief just_get 循环获取
 * @param load_count
 * @param len
 * @return
 */
int just_get(float *load_count, int len)
{
    static int point = 0, max = 0;
    int ret;
    if( 0 == point)
    {
        max = get_ran_max(load_count, len);
        point++;
        ret = max-2;
    }
    else if( 1 == point)
    {
        ret = max+2;
        point++;
    }
    else if( 2 == point)
    {
        ret = max-1;
        point++;
    }
    else if( 3 == point)
    {
        ret = max+1;
        point++;
    }
    else if( 4 == point)
    {
        ret = max;
        point = 0;
    }
    printf("ret=%d,max=%d,point=%d\n",ret,max,point);
    return ret;
}
