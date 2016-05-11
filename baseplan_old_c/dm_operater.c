#include "dm_operater.h"
#include "inifile.h"
#include "des_encode.h"



int PERROR_CLOSEDATABASE_EXIT(int ret,char *str)
{
    sprintf(mess_error,"error in: %s,file:%s,line:%d\n",str,__FILE__,__LINE__);
    fprintf(stderr,"%s", mess_error);
    write_act_info(-1,mess_error);
    close_database();
    exit(ret);
}
/********************************************
* note: init database connection
* return: false: -1, -2,-3,-4; true: 0
********************************************/
int init_database()
{
	// get server ip, usr, pwd
    int len;
    void *fpini = IniOpenFile(INI_PATH, 1);
    if(fpini == NULL)
    {
		memset(buffer, 0, sizeof(buffer));
		strcpy(buffer, "read ini file failure!\n");
		write_log_file(LOGPATH, FILE_MAX_SIZE, buffer, strlen(buffer));
		printf("%s",buffer);
		printf("read ini file failed!\n");
		return -1;
    }        
  
    IniGetString("DB", "ip", ipstr, MAXSIZE, fpini);
    printf("ip:%s\n", ipstr);

	IniGetString("DB", "usr", usrstr, MAXSIZE, fpini);
    printf("usr:%s\n", usrstr);

    get_pass("DB", "pwd",pwdstr,fpini);
    IniGetString("DB", "db_name", db_name_str, MAXSIZE, fpini);
    printf("db_name_str:%s\n", db_name_str);
    //IniGetString("DB", "pwd", pwdstr, MAXSIZE, fpini);
//    printf("pwd:%s\n", pwdstr);

    IniCloseFile(fpini);

    len = strlen(pwdstr);
    while( ' '==pwdstr[len-1])
        len--;
    pwdstr[len] = 0;
	//LOG BEGIN
	memset(buffer, 0 ,sizeof(buffer));
	sprintf(buffer, "%s\n", "init database connection");
	write_log_file(LOGPATH, FILE_MAX_SIZE, buffer, strlen(buffer));
	
	//create API environment
	printf("create API environment\n");
	sret = dm_api_init();
	if(!(sret == DM_OK || sret == DM_SUCCESS_WITH_INFO))
	{
		memset(buffer, 0, sizeof(buffer));
		strcpy(buffer, "create API environment failure!\n");
		write_log_file(LOGPATH, FILE_MAX_SIZE, buffer, strlen(buffer));
		printf("%s",buffer);
		return -1;
	}

	// alloc environment handle
	printf("alloc enironment handle\n");
	if(!dm_alloc_env(&henv))
	{
		memset(buffer, 0, sizeof(buffer));
		strcpy(buffer, "alloc environment handle failure!\n");
		write_log_file(LOGPATH, FILE_MAX_SIZE, buffer, strlen(buffer));
		printf("%s",buffer);
		return -2;
	}

	//alloc connection handle
	printf("alloc connection handle\n");
	if(!dm_alloc_connect(henv, &hdbc))
	{
		memset(buffer, 0, sizeof(buffer));
		strcpy(buffer,  "alloc connection handle failure!\n");
		write_log_file(LOGPATH, FILE_MAX_SIZE, buffer, strlen(buffer));
		printf("%s",buffer);
		dm_free_env(henv);
		return -3;
	}

	//connect localhost
  	printf("connect localhost\n");
	if(!dm_login_port(hdbc, ipstr, usrstr, pwdstr, 12345))
	{
		memset(buffer, 0, sizeof(buffer));
		strcpy(buffer, "connect localhost failure!\n");
		write_log_file(LOGPATH, FILE_MAX_SIZE, buffer, strlen(buffer));
		printf("%s",buffer);
		dm_free_connect(hdbc);
		dm_free_env(henv);
		return -4;
	}
	return 0;
}

 
/**********************************************************************
* note: close database connection
* return: true 0;
**********************************************************************/
int close_database()
{
	//free statement handle
	printf("free statement handle\n");
	if(!dm_free_stmt(hsmt))
	{
		memset(buffer, 0, sizeof(buffer));
		strcpy(buffer, "free statement handle failure!\n");
		write_log_file(LOGPATH, FILE_MAX_SIZE, buffer, strlen(buffer));
	}

	//disconnect DM
	printf("disconnect DM\n");
	if(!dm_logout(hdbc))
	{
		memset(buffer, 0, sizeof(buffer));
		strcpy(buffer, "disconnect DM failure!\n");
		write_log_file(LOGPATH, FILE_MAX_SIZE, buffer, strlen(buffer));
	}

	//free connection handle
 	printf("free connection handle\n");
	if(!dm_free_connect(hdbc))
	{
		memset(buffer, 0, sizeof(buffer));
		strcpy(buffer, "free connection handle failure!\n");
		write_log_file(LOGPATH, FILE_MAX_SIZE, buffer, strlen(buffer));
	}

	//free environment handle
	printf("free environment handle\n");
	if(!dm_free_env(henv))
	{
		memset(buffer, 0, sizeof(buffer));
		strcpy(buffer, "free environment handle failure!\n");
		write_log_file(LOGPATH, FILE_MAX_SIZE, buffer, strlen(buffer));
	}
	return 0;
}


/********************************************************************
* note: exe sql
* @param: sql
* return: false -1, -2; true 0;
********************************************************************/
int exec_sql(char* sql)
{
	//alloc statement handle
	printf("alloc statement handle\n");
	if(!dm_alloc_stmt(hdbc, &hsmt))
	{
		memset(buffer, 0, sizeof(buffer));
		strcpy(buffer, "alloc statement handle failure!\n");
		write_log_file(LOGPATH, FILE_MAX_SIZE, buffer, strlen(buffer));
	//	printf(buffer);
		dm_logout(hdbc);
		dm_free_connect(hdbc);
		dm_free_env(henv);
		return -1;
	}

	//execute procedure
	printf("execute sql\n");
	printf("sql:%s\n", sql);
	if(!dm_direct_exec(hsmt, sql))
	{
		memset(buffer, 0, sizeof(buffer));
		strcpy(buffer, "execute sql failure!\n");
		write_log_file(LOGPATH, FILE_MAX_SIZE, buffer, strlen(buffer));
		return -2;
	}	
	return 0;		
}

/**
 * @brief write_act_info 写程序运行信息
 * @param result 程序执行结果
 * @param note 程序执行描述
 * @return -1执行语句失败，0执行数据库操作成功并插入数据库
 */
int write_act_info(int result,char *note)
{
    char sql_string[MAX_BUFFER];
	
	int seconds = time( (time_t*)NULL);
	sprintf(act_info.id, "%d", seconds);
	sprintf(act_info.prog_name,"%s","定电量符合分配");
	act_info.result = result;
	sprintf(act_info.result_desc,"%s",note);
	get_date_and_time(act_info.date,act_info.time);

    sprintf(sql_string,"insert into %ssys_cmd_act_info "
            "(sdate,stime,stype,sid,sdesc,sresult,note) "
        "values('%s','%s','%s','%s','%s','%d','%s')",
            db_name_str,
        act_info.date, act_info.time,
            "火电机组发电计划编制", act_info.prog_id,
		act_info.prog_name,act_info.result,
        act_info.result_desc
        );
	printf("%s\n",sql_string);
	if (exec_sql(sql_string) != 0)
    {
		fprintf(stderr,"%s","write_act_info()中exec_sql()写程序运行信息错误");
        return -1;
    }
    return 0;
}

/**
 * @brief exec_select 用于执行达梦数据库sql语句,函数内部实现语句执行、列绑定、记录数返回、fetch操作
 * @param sql_string 所要执行的sql语句
 * @param column_num select语句返回的列的数目
 * @param desc 对select语句所取数据的描述
 * @param db_data 输出参数，select语句执行结果存放在此结构中
 * @return 执行exec_sql失败向数据库写出错信息，关闭数据库，
 *退出程序执行成功返回select语句返回记录的数目
 */
int exec_select(char *sql_string, int column_num, char *desc, void **db_data)
{
//============================================获取电厂出力上下限=======================================
	long long i, row_num;
	int flag, j;

	char stru[10][COLUMN_LEN];
	int  str_size[10];

    char mess[MAX_BUFFER];
	sprintf(mess, "%s sql语句执行错误", desc);
	if( exec_sql(sql_string) != 0 )
        PERROR_CLOSEDATABASE_EXIT(-1, mess);
	for(j=0; j<column_num; j++)
	{				
		memset(mess,'\0',sizeof(mess));
		sprintf(mess,"%s，列%d绑定失败", desc, j+1);
		if( DM_FALSE == dm_bind_column(hsmt, j+1, DM_DATA_CHAR, TYPE_DEFAULT,
					stru[j], sizeof(stru[j]), &str_size[j],
					DM_DESC_PTR_LEN_DEFAULT) )
            PERROR_CLOSEDATABASE_EXIT(-1, mess);
	}
	
	sprintf(mess, "%s,获取返回结果记录数失败", desc);
	if(DM_FALSE == dm_describe_row_num(hsmt, &row_num) )
         PERROR_CLOSEDATABASE_EXIT(-1, mess);
	
	sprintf(mess, "%s,获取返回结果记录数小于0", desc);
	if( 0 >=row_num )
         PERROR_CLOSEDATABASE_EXIT(-1, mess);
	
	*db_data = malloc( column_num*row_num*COLUMN_LEN );
	memset(*db_data, '\0', column_num*row_num*COLUMN_LEN);
	printf("db_data用malloc分配了%lld\n", column_num*row_num*COLUMN_LEN);

	for(i=0; i<row_num; i++)
	{
		flag = dm_fetch_scroll(hsmt,DM_FETCH_NEXT,0);
		if(DM_FALSE == flag)
            PERROR_CLOSEDATABASE_EXIT(-1, "获取电厂定电量时,dm_fetch_scroll()出错");
		else if(DM_NO_DATA_FOUND == flag)
			PERROR_CLOSEDATABASE_EXIT(-1,"获取电厂定电量时,数据表中没有数据,"
                    "中dm_fetch_scroll()出错");
		for(j=0; j<column_num; j++)
		{
			strcpy( *db_data + i*column_num*COLUMN_LEN + j*COLUMN_LEN, stru[j]);
		}

	}
	return row_num;
}
