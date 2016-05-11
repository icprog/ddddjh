/*--------------------------------------------------------------
**   FILE NAME: Get_single_attribute.cxx
**--------------------------------------------------------------
**   PURPOSE:
**      This rountine receive sql string from socket class,link to
**       pointed database and query data qualifyed by sql string,
**       write data to a common string.
**--------------------------------------------------------------
**   PARAMETERS:
**      INPUT
**      OUTPUT
**         no returns
**   Author:
**      Zhang zhigang     26/11/99
**
**--------------------------------------------------------------*/

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctpublic.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <time.h>
#include <locale.h>
#include "global_var.h"
#include "load_dispatch_db.h"


float get_pjcyl()
{
        char sql_str[SQL_STRING_LENGTH],res_str[RESULT_STRING_LENGTH];
        float pjcyl;

        //sprintf(sql_str,"xtgl@^$$^@select  CSSZ from xtgl..T_XTKZCS having convert(datetime,xtgl..T_XTKZCS.BZRQ) = max(convert(datetime,xtgl..T_XTKZCS.BZRQ))  and xtgl..T_XTKZCS.CSMC = 'jhdcpjcyl' " );
	sprintf(sql_str,"xtgl@^$$^@select  CSSZ from xtgl..T_XTKZCS where CSMC='jhdcpjcyl' and convert(datetime,BZRQ)=(select max(convert(datetime,BZRQ)) from xtgl..T_XTKZCS where CSMC='jhdcpjcyl' and BZRQ <= '%s')",present_date);
//	printf("sql=%s\n",sql_str);

        sql_operate(sql_str,res_str);
//      printf("res_str=%s\n",res_str);


	pjcyl = atof(res_str);
	return pjcyl;
}

int get_time_num(char *year)
{
	int sd_num;
        char sql_str[SQL_STRING_LENGTH];
        char buf[1*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)];

	sprintf(sql_str,"xtgl@^$$^@select CSSZ from xtgl..T_XTKZCS where CSMC='mrsds' and convert(datetime,BZRQ)=(select max(convert(datetime,BZRQ)) from xtgl..T_XTKZCS where CSMC='mrsds' and BZRQ <= '%s')",present_date);
//	printf("sql=%s\n",sql_str);

	sql_operate(sql_str,buf);

	sd_num=atoi(&buf[0]);

	return sd_num;

}

float get_net_loss_factor()
{
	float loss_factor;
    char sql_str[SQL_STRING_LENGTH];
    char buf[1*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)];


	sprintf(sql_str,"xtgl@^$$^@select CSSZ from xtgl..T_XTKZCS where CSMC='wsxs' and convert(datetime,BZRQ)=(select max(convert(datetime,BZRQ)) from xtgl..T_XTKZCS where CSMC='wsxs' and BZRQ <= '%s')",present_date);
//	printf("sql=%s\n",sql_str);

	sql_operate(sql_str,buf);
	loss_factor=atof(&buf[0]);
	return loss_factor;

}

int get_bid_price_num(char *year_str,char *mon_str,char *day_str,int time_block)
{
	int bid_un;
        char sql_str[SQL_STRING_LENGTH];
        char buf[1*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)];

	sprintf(sql_str,"fdbj@^$$^@select count(*) from fdbj%s..T_XHSCJJSB_%s_%s where SD=%d",year_str,mon_str,day_str,time_block);
//	printf("%s\n",sql_str);

	sql_operate(sql_str,buf);
	bid_un = atoi(&buf[0]);
	return bid_un;
}

int get_load(char *year_str,char *mon_str,char *day_str,int time_block)
{
	int load_flow;
        char sql_str[SQL_STRING_LENGTH];
        char buf[1*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)];
	
	char rq_str[10];
	
//	sprintf(rq_str,"%s/%s/%s",mon_str,day_str,year_str);
	sprintf(sql_str,"jhjygl@^$$^@select FHYCZ from jygl%s..T_RFHYC where RQ='%s' and SD=%d and FHYCLXID=(select CYFHYCID from jygl%s..T_RJHSCYFHYC where RQ='%s')",year_str,present_date,time_block,year_str,present_date);
//	printf("%s\n",sql_str);

	sql_operate(sql_str,buf);

	load_flow=atoi(&buf[0]);
	return load_flow;
}

int get_gen_num(char *year_str,char *mon_str,char *day_str,int time_block)
{
	int gen_num;
        char sql_str[SQL_STRING_LENGTH];
        char buf[1*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)];

	sprintf(sql_str,"fdbj@^$$^@select count(distinct JZID) from fdbj%s..T_XHSCJJSB_%s_%s where SD=%d",year_str,mon_str,day_str,time_block);
//	printf("%s\n",sql_str);

	sql_operate(sql_str,buf);
	gen_num = atoi(&buf[0]);
	return gen_num;
}

int get_qt_gen_num(char *year_str,char *mon_str,char *day_str,int time_block)
{
	int qt_gen_num;
        char sql_str[SQL_STRING_LENGTH];
        char buf[1*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)];
	
	if(!strcmp(constrain_flag,"wys"))
		sprintf(sql_str,"jhjygl@^$$^@select count(*) from jygl%s..T_WYSJZKTJ_%s_%s where SD=%d",year_str,mon_str,day_str,time_block);
	else
		sprintf(sql_str,"jhjygl@^$$^@select count(*) from jygl%s..T_YYSJZKTJ_%s_%s where SD=%d",year_str,mon_str,day_str,time_block);

//	printf("%s\n",sql_str);

	sql_operate(sql_str,buf);
	qt_gen_num = atoi(&buf[0]);
	return qt_gen_num;
}

int get_gen_un_num()
{
	int gen_un_num;
        char sql_str[SQL_STRING_LENGTH];
        char buf[1*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)];


	sprintf(sql_str,"xtgl@^$$^@select count(*) from xtgl..T_JZBM where SSJBDYID not in ('qt') and SFJJ = 1");
//	printf("sql=%s\n",sql_str);

	sql_operate(sql_str,buf);

	gen_un_num=atoi(&buf[0]);
	return gen_un_num;

}

int get_basic_load(char *year_str,char *mon_str,char *day_str,int time_block)
{
	int load_value;
        char sql_str[SQL_STRING_LENGTH];
        char buf[1*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)];


	sprintf(sql_str,"jhjygl@^$$^@select sum(YGGL) from jygl%s..T_DDLDCRFDJH_%s_%s where SD=%d",year_str,mon_str,day_str,time_block);
//	printf("sql=%s\n",sql_str);

	sql_operate(sql_str,buf);

	load_value=atoi(&buf[0]);

	sprintf(sql_str,"jhjygl@^$$^@select sum(YGGL) from jygl%s..T_DDLJZRFDJH_%s_%s where SD=%d",year_str,mon_str,day_str,time_block);
//	printf("sql=%s\n",sql_str);

	sql_operate(sql_str,buf);

	load_value += atoi(&buf[0]);
	return load_value;

}

int get_interconnect_line_load(char *year_str,char *mon_str,char *day_str,int time_block)
{
	int load_value;
    char sql_str[SQL_STRING_LENGTH];
    char buf[1*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)];


	sprintf(sql_str,"jhjygl@^$$^@select sum(YG) from jygl%s..T_RLLXJH where RQ='%s' and SD=%d",year_str,present_date,time_block);
//	printf("sql=%s\n",sql_str);

	sql_operate(sql_str,buf);


	load_value=atoi(&buf[0]);
	return load_value;

}

float get_max_price()
{
	float max_price;
        char sql_str[SQL_STRING_LENGTH];
        char buf[1*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)];

	sprintf(sql_str,"xtgl@^$$^@select CSSZ from xtgl..T_XTKZCS where CSMC='bjsx' and convert(datetime,BZRQ)=(select max(convert(datetime,BZRQ)) from xtgl..T_XTKZCS where CSMC='bjsx' and BZRQ <= '%s')",present_date);
//	printf("sql=%s\n",sql_str);

	sql_operate(sql_str,buf);

	max_price=atof(&buf[0]);

	return max_price;

}

int get_num_of_start_curve_type()
{
	int sd_num;
    char sql_str[SQL_STRING_LENGTH];
    char buf[1*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)];

	sprintf(sql_str,"xtgl@^$$^@select count(*) from xtgl..T_JZKTJQXLX where KTJBSID='kj'");
//	printf("sql=%s\n",sql_str);

	sql_operate(sql_str,buf);

	sd_num=atoi(&buf[0]);

	return sd_num;
}

float get_former_price()
{
        char sql_str[SQL_STRING_LENGTH];
        char buf[1*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)];
        float price;

        if(!strcmp(constrain_flag,"wys"))

              if(!strcmp(month,"1")&&!strcmp(day,"1"))
              {
                 int temp = atoi(year);
                 int year_int = temp-1;
                 sprintf(sql_str,"jhjygl@^$$^@select WYSBJJG from jygl%d..T_RBJJG where RQ = '12/31/%d' and SD=%d",year_int,year_int,sd_id);
                 //printf("sql_str is %s", sql_str);
              }
              else 
              
		 sprintf(sql_str,"jhjygl@^$$^@select WYSBJJG from jygl%s..T_RBJJG where convert(datetime,RQ) in (select max(convert(datetime,RQ)) from jygl%s..T_RBJJG where convert(datetime,RQ) < convert(datetime,'%s')) and SD=%d",year,year,present_date,sd_id);
        else

              if(!strcmp(month,"1")&&!strcmp(day,"1"))
              {
                int temp = atoi(year);
                int year_int = temp-1;
                sprintf(sql_str,"jhjygl@^$$^@select YYSBJJG from jygl%d..T_RBJJG where RQ = '12/31/%d' and SD=%d",year_int,
year_int,sd_id);
                //printf("sql_str is %s", sql_str);
              }
              else
 
                sprintf(sql_str,"jhjygl@^$$^@select YYSBJJG from jygl%s..T_RBJJG where convert(datetime,RQ) in (select max(convert(datetime,RQ)) from jygl%s..T_RBJJG where convert(datetime,RQ) < convert(datetime,'%s')) and SD=%d",year,year,present_date,sd_id);
//      printf("%s\n",sql_str);

        sql_operate(sql_str,buf);
        price = atof(&buf[0]);
        return price;
}

float get_price_up_limit()
{
        char sql_str[SQL_STRING_LENGTH];
        char buf[1*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)];
        float price;

        if(!strcmp(constrain_flag,"wys"))
                sprintf(sql_str,"xtgl@^$$^@select CSSZ from xtgl..T_XTKZCS where CSMC='wysbjjgsxbdfw' and convert(datetime,BZRQ)=(select max(convert(datetime,BZRQ)) from xtgl..T_XTKZCS where CSMC='wysbjjgsxbdfw' and BZRQ <= '%s')",present_date);
        else
                sprintf(sql_str,"xtgl@^$$^@select CSSZ from xtgl..T_XTKZCS where CSMC='yysbjjgsxbdfw' and convert(datetime,BZRQ)=(select max(convert(datetime,BZRQ)) from xtgl..T_XTKZCS where CSMC='yysbjjgsxbdfw' and BZRQ <= '%s')",present_date);
//      printf("%s\n",sql_str);

        sql_operate(sql_str,buf);
        price = atof(&buf[0]);
        return price;
}

float get_price_down_limit()
{
        char sql_str[SQL_STRING_LENGTH];
        char buf[1*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)];
        float price;

        if(!strcmp(constrain_flag,"wys"))
                sprintf(sql_str,"xtgl@^$$^@select CSSZ from xtgl..T_XTKZCS where CSMC='wysbjjgxxbdfw' and convert(datetime,BZRQ)=(select max(convert(datetime,BZRQ)) from xtgl..T_XTKZCS where CSMC='wysbjjgxxbdfw' and BZRQ <= '%s')",present_date);
        else
                sprintf(sql_str,"xtgl@^$$^@select CSSZ from xtgl..T_XTKZCS where CSMC='yysbjjgxxbdfw' and convert(datetime,BZRQ)=(select max(convert(datetime,BZRQ)) from xtgl..T_XTKZCS where CSMC='yysbjjgxxbdfw' and BZRQ <= '%s')",present_date);
//      printf("%s\n",sql_str);

        sql_operate(sql_str,buf);
        price = atof(&buf[0]);
        return price;
}

int get_num_of_defined_fre_adjust_array()
{
	int sd_num;
    char sql_str[SQL_STRING_LENGTH];
    char buf[1*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)];

	sprintf(sql_str,"jhjygl@^$$^@select count(*) from jygl%s..T_RJHTPJZZD where RQ='%s' and SD=%d",year,present_date,sd_id);
//	printf("sql=%s\n",sql_str);

	sql_operate(sql_str,buf);

	sd_num=atoi(&buf[0]);

	return sd_num;
}

int get_num_of_defined_man_adjust_array()
{
	int sd_num;
    char sql_str[SQL_STRING_LENGTH];
    char buf[1*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)];

	sprintf(sql_str,"jhjygl@^$$^@select count(*) from jygl%s..T_RJHSTJZZD where RQ='%s' and SD=%d",year,present_date,sd_id);
//	printf("sql=%s\n",sql_str);

	sql_operate(sql_str,buf);

	sd_num=atoi(&buf[0]);

	return sd_num;
}

int get_num_of_up_down_limit_array(char *year_str,char *month_str,char *day_str,int sd_num)
{
        int up_down_limit_gen_num;
    char sql_str[SQL_STRING_LENGTH];
    char buf[1*MAX_COLUMN_NUM*(MAX_COLUMN_SIZE+1)];

        sprintf(sql_str,"jhjygl@^$$^@select count(*) from jygl%s..T_JZCLSXX_%s_%s where SD=%d",year_str,month_str,day_str,sd_num);
//      printf("sql=%s\n",sql_str);

        sql_operate(sql_str,buf);

        up_down_limit_gen_num=atoi(&buf[0]);

        return up_down_limit_gen_num;

}

