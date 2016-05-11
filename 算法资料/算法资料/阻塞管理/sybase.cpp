//sybase.cpp by Dingpeng, Wang GT, and Zhangyang
////////////////////////////////////////////////////

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

#include "sybase.h"
#include "common.h"
#include "opf.h"
#include "opf_global.h"
//////////////////////////////////////////////

void reguExplain(char *exp_str,long ibs,double inc,struct genrstr *genrData,struct groupstr *groupData)
{
	long slack=codeList[0];
	double aux2;

	struct genrstr *gp;
        struct groupstr *grp;
	/////////////////////////////////////
	
	//1: init. exp_str[]

	strcpy(exp_str,"");//init.
	if(fabs(inc)<EPS) return;//no explanation
	//////////////////////////////////////////////////////

	//2: coslack and asso.regulation
 
	gp=findData(ibs,genrData);
	if(gp==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }

	//2.1 coslack power

	if(fabs(gp->Pc)>SMLL)
	{
		strcat(exp_str,"  ");
		strcat(exp_str,"联合调整");
		
		aux2=gp->Pc*100;//MW
		sprintf(buf,"(%ldMW)",sign(aux2)*(long)(fabs(aux2)+0.5));
		
		strcat(exp_str,buf);
	}//end if
	//////////////////////////////////////

	//2.2 asso.regulation

	if(fabs(gp->Pa)>SMLL)
	{
		strcat(exp_str,"  ");
		strcat(exp_str,"关联调整");

		aux2=gp->Pa*100.0;//p.u --> MW
		sprintf(buf,"(%ldMW)",sign(aux2)*(long)(fabs(aux2)+0.5));
		
		strcat(exp_str,buf);
	}//end if

	//2.3 only for slack
	if(ibs==slack) strcat(exp_str,"  平衡机");
	///////////////////////////////////////////////////

	//3: appending vio.group
 
	grp=groupData;
	while(grp!=NULL)
	{
		if(grp->expFlg==YES)
		{
			strcat(exp_str,"  ");
			strcat(exp_str,grp->descr);
			strcat(exp_str,"越限");
		}//end if

		grp=grp->next;
	}//end while
	/////////////////////////////////////

	return;
}

void form_rq_str(char *rq_str,char *month,char *day,char *year)
{
	char buf_month[3],buf_day[3];

        strcpy(buf_month,month);
        strcpy(buf_day,day);

        if(strlen(buf_month)<2) sprintf(buf_month,"0%s",buf_month);
        if(strlen(buf_day)<2) sprintf(buf_day,"0%s",buf_day);
	
	initString(rq_str,RQ_LEN);
        sprintf(rq_str,"%s/%s/%s",buf_month,buf_day,year);

        return;
}

void form_time_str(char *time_str,long sd)
{
        long hour,minu;
        char hour_ch[10],minu_ch[10];
        //////////////////////////////////

        minu=sd*SD_LEN;//total minutes

        hour=long(minu/60.0);
        minu=minu%60-SD_LEN;

        if(minu<0){ hour--; minu+= 60; }
        ////////////////////////////////////

        sprintf(hour_ch,"%ld",hour);
        sprintf(minu_ch,"%ld",minu);

        addLeadingChar(hour_ch,'0',2);
        addLeadingChar(minu_ch,'0',2);
        /////////////////////////////////

	initString(time_str,TIME_LEN);
        
	strcpy(time_str,hour_ch);
        strcat(time_str,":");
        strcat(time_str,minu_ch);
        //////////////////////////////////

        return;
}

void consoleMsg(char *msg,char *msg_ex)
{
	strcpy(result_info,msg);
	strcpy(result_info_explain,msg_ex);

        set_opp_rela();
        write_execute_info();
        
	CS_RETCODE       rcode=0;
        rcode = close_connect_with_server();
        
	if(rcode!=CS_SUCCEED)
        {
		printf("\n\nError! --- %s: %ld",__FILE__,__LINE__);
                printf("\n  database close failed!!\n");
                exit(-1);
        }
	/////////////////////////////////

        return;
}

void consoleMsg(char *msg,char *msg_ex,char *file,long line)
{
	sprintf(result_info,"%s",msg);
        sprintf(result_info_explain,"%s: %ld --- %s",file,line,msg_ex);
        
	set_opp_rela();
	write_execute_info();

	CS_RETCODE       rcode=0;
        rcode = close_connect_with_server();
        
	if(rcode!=CS_SUCCEED)
        {
		printf("\n\nError! --- %s: %ld",__FILE__,__LINE__);
                printf("\n  database close failed!!\n");
                exit(-1);
        }
	/////////////////////////////

	return;
}

void OPF_writeSybase(char *year,char *month,char *day,long sd,struct genrstr *genrData,
		struct groupstr *groupData,double *grp_w0,double *grp_w,long groupNum,long prnt)
{
	if(prnt==YES) printf("\n\nOPF_writeSybase()...");

	register long i;
	int P0,inc,crr,w0,w,imode,flg,grp_lmt; 
	double aux2;
	
	struct genrstr *gp;
	struct groupstr *grp;
	/////////////////////////////////////////////////

	char exp_str[200],descr_str[DESCRLN];

        char rq_str[RQ_LEN],sql_str[SQL_STRING_LENGTH];
	char res_str[RESULT_STRING_LENGTH];
	CS_RETCODE       rcode=0;
	//////////////////////////////////////

	set_opp_rela();
        if(prnt==YES) printf("\n set_opp_rela() ok");

        form_rq_str(rq_str,month,day,year);
	if(prnt==YES) printf("\n rq_str = %s",rq_str);
	//////////////////////////////////////////////////////

	//1: tie-line flows

	initString(sql_str,SQL_STRING_LENGTH);
        sprintf(sql_str,"jhjygl@^$$^@delete jygl%s..T_ZSGLLLXZCL_%s_%s where SD=%d",year,month,day,sd);
	//printf("sql_str is %s \n",sql_str);
        sql_operate(sql_str,res_str);
	//printf("res_str is %s \n",res_str);

	grp=groupData;
	for(i=0;i<groupNum;i++,grp=grp->next)
	{
		strcpy(idbuf,grp->id);
		strcpy(buf,grp->descr);

		imode=grp->imode;
		if(imode==0)
		{
		 	strcpy(idbuf2,"null");
			grp_lmt=0;
		}
		else 
		{
			strcpy(idbuf2,grp->mode[imode].id);
			grp_lmt=(int)(100*grp->mode[imode].lmt+0.5);
		}

		w0=(int)(100*grp_w0[i+1]+0.5);
		w =(int)(100*grp_w[i+1]+0.5);

		if(grp_lmt>SMLL && w0>grp_lmt) flg=YES;
		else flg=NO;
		/////////////////////////////////////////////////

		initString(sql_str,SQL_STRING_LENGTH);
                sprintf(sql_str,"jhjygl@^$$^@insert jygl%s..T_ZSGLLLXZCL_%s_%s values('%s',%d,'%s','%s','%s',%d,%d,%d,%d,'zht','zht')",year,month,day,rq_str,sd,idbuf,buf,idbuf2,grp_lmt,w0,w,flg);
                //printf("sql_str is %s \n",sql_str);

                sql_operate(sql_str,res_str);
	}//end for 

	if(prnt==YES) printf("\n tie-line flow ok");
	///////////////////////////////////////////////////////

	//2: generator powers

	initString(sql_str,SQL_STRING_LENGTH);
	sprintf(sql_str,"jhjygl@^$$^@delete jygl%s..T_ZSGLTZL_%s_%s where SD=%d",year,month,day,sd);
	//printf("sql_str is %s \n",sql_str);
	sql_operate(sql_str,res_str);

	gp=genrData;
	for(i=0;i<genrNum;i++,gp=gp->next)
	{
		strcpy(idbuf,gp->id);
		strcpy(descr_str,gp->descr);

		P0 =(int)sign(gp->P0)*(100*fabs(gp->P0)+0.5);
		crr=(int)sign(gp->P )*(100*fabs(gp->P )+0.5);

		aux2=gp->P-gp->P0;
		inc=(int)sign(aux2)*(100*fabs(aux2)+0.5);

		reguExplain(exp_str,gp->i,inc,genrData,groupData);
		stringEnd(idbuf,(long)8);
		
		if(prnt==YES) printf("\n id=%s P0=%4d inc=%4d crr=%4d  %s",idbuf,P0,inc,crr,exp_str);
		///////////////////////////////////////////////////////////////////
	
		//modified schedules

		initString(sql_str,SQL_STRING_LENGTH);
		sprintf(sql_str,"jhjygl@^$$^@update jygl%s..T_SSSWYYSRJZFDJH_%s_%s set SSDDYGGL=%d where JZID='%s'and SD=%d",year,month,day,crr,idbuf,sd);
	
		//printf("sql_str is %s \n",sql_str);
    	        sql_operate(sql_str,res_str);
		////////////////////////////////////////////////////////

		//incremental powers and explanation

	        initString(sql_str,SQL_STRING_LENGTH);
		sprintf(sql_str,"jhjygl@^$$^@insert jygl%s..T_ZSGLTZL_%s_%s values('%s',%d,'%s','%s',%d,%d,'%s',0,'','zht','zht')",year,month,day,rq_str,sd,idbuf,descr_str,P0,inc,exp_str);
	
		//printf("sql_str is %s \n",sql_str);
		sql_operate(sql_str,res_str);
	}//end for

	if(prnt==YES) printf("\n generator ok");
	////////////////////////////////////////////////

	//3: close database
	
	rcode = close_connect_with_server();
        if(rcode!=CS_SUCCEED)
	{ 
		printf("database close failed!!\n");
		consoleMsg("错误","database close failed",__FILE__,__LINE__);
		exit(-1);
	}//end if
	/////////////////////////

	return;
}

void clear_execute_info()
{

        char sql_str[SQL_STRING_LENGTH];
        char buf[1*MAX_COLNUM*(MAX_COLSIZE+1)];

        sprintf(sql_str,"xtgl@^$$^@delete xtgl..T_CXZXXX where CXID='%s'",program_id);
        //printf("sql=%s\n",sql_str);

        sql_operate(sql_str,buf);

}

void write_execute_info()
{

        char sql_str[SQL_STRING_LENGTH];
	char buf[1*MAX_COLNUM*(MAX_COLSIZE+1)];

        sprintf(sql_str,"xtgl@^$$^@delete xtgl..T_CXZXXX where CXID='%s'",program_id);
       // printf("sql=%s\n",sql_str);

        sql_operate(sql_str,buf);

        sprintf(sql_str,"xtgl@^$$^@insert xtgl..T_CXZXXX values('%s','%s','%s','%s','%s','%s')",program_id,program_name,result_info,result_info_explain,exe_date,exe_time);
       // printf("sql=%s\n",sql_str);

        sql_operate(sql_str,buf);
}

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
//      char exe_date[12];
//      char exe_time[10];
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

void set_opp_rela()
{

        FILE *fp;
        char read_str[100],*str_h,*str_t;
        int count=0,i;
        char temp_str[5][100];
        if(fp=fopen(getenv("SRVLIST"),"r")) //read config file and set struct db_status_tab
        {
                while(fgets(read_str,95,fp))
                {
                        if(read_str[0]=='#') continue;
                        else
                       {
                                str_h=str_t=read_str;
                                for(i=0;i<5;i++)
                                {
                                        while(strncmp(str_t,"$$",2)) str_t++;
                                        *str_t='\0';
                                        strcpy(temp_str[i],str_h);
                                        str_t+=2;
                                        str_h=str_t;
                                }
                                strcpy(st_tab[count].sys_id,temp_str[0]);
                                strcpy(st_tab[count].serv_nm,temp_str[1]);
                                strcpy(st_tab[count].IP_addr,temp_str[2]);
                                strcpy(st_tab[count].usr_name,temp_str[3]);
                                strcpy(st_tab[count].passwd,temp_str[4]);
                                count++;
                                //printf("reading string =%s\n",read_str);
                        }
                }
                fclose(fp);
        }
        else printf("open file error!\n");

        for(i=0;i<SYSTEM_NUMBER;i++) //create thread mutex for thread to lock
        {
                st_tab[i].cntx_ptr=NULL;
                st_tab[i].conn_ptr=NULL;
                st_tab[i].cmd_ptr=NULL;

                if(pthread_mutex_init(&(st_tab[i].lock_mutex),pthread_mutexattr_default))
                {
                        perror("Init point_mutex error");
                        exit(-1);
                }

        }
}

void sql_operate(char *sql_str,char *res_str)
{
        CS_RETCODE       rcode=0;

        //set_opp_rela();

        rcode=inside_query(sql_str,res_str);
        if(rcode!=CS_SUCCEED)
        {
                printf("database link failed!!\n");
                exit(-1);
        }
        //RETURN_IF(rcode,"inside_query");
}

CS_RETCODE
inside_query(char *sq_str,char *buf_str)
{

//      CS_COMMAND       *cmd_ptr=NULL;          /* handle for this SQL command batch*/
        CS_RETCODE       rcode=0;
        int con_status,pos;
        char *str_h,*str_t,sysid[25],*query_str;
        CS_INT exit_type;

	//printf("inside string is : %s\n",sq_str);
        //analyse string,get sysid and query string
        str_h=sq_str;
        str_t=sq_str;
        while(strncmp(str_h,COL_END_FIX,COL_END_FIX_LEN)) str_h++;
        *str_h='\0';
        strcpy(sysid,str_t);
        str_h+=COL_END_FIX_LEN;
        query_str=str_h;

        pos=ret_pos(sysid);

        /*to promise that each system can only connect once!*/
        if(st_tab[pos].cntx_ptr == NULL || st_tab[pos].conn_ptr == NULL)
        {

        //initial pointed database link contex
        rcode = cs_ctx_alloc(CS_VERSION_100, &(st_tab[pos].cntx_ptr));

        if(rcode!=CS_SUCCEED) printf("server %s can't connect!!\n",st_tab[pos].serv_nm);
        RETURN_IF(rcode,"cs_ctx_alloc");

        rcode = ct_init(st_tab[pos].cntx_ptr, CS_VERSION_100);
        if(rcode!=CS_SUCCEED) printf("server %s can't connect!!\n",st_tab[pos].serv_nm);
        RETURN_IF(rcode,"ct_init");

        //initial every database link connect
        rcode = ct_con_alloc(st_tab[pos].cntx_ptr, &st_tab[pos].conn_ptr);
        RETURN_IF(rcode,"ct_con_alloc");

	//printf("\npos is %d st_tab[pos].cntx_ptr is %d \n",pos,st_tab[pos].cntx_ptr);

        /* set user name and password */

        rcode = ct_con_props(st_tab[pos].conn_ptr, CS_SET, CS_USERNAME,st_tab[pos].usr_name, CS_NULLTERM, NULL);
        RETURN_IF(rcode,"ct_con_props");

        rcode = ct_con_props(st_tab[pos].conn_ptr, CS_SET, CS_PASSWORD,st_tab[pos].passwd, CS_NULLTERM, NULL);
        RETURN_IF(rcode,"ct_con_props");

        /* request connection to SQL server */
        rcode = ct_connect(st_tab[pos].conn_ptr,st_tab[pos].serv_nm,CS_NULLTERM);
        RETURN_IF(rcode,"ct_connect0");

        /* allocate command struct */
        rcode = ct_cmd_alloc(st_tab[pos].conn_ptr,&st_tab[pos].cmd_ptr);
        RETURN_IF(rcode,"ct_cmd_alloc");

        }

        rcode=inside_send_sql(st_tab[pos].cmd_ptr,query_str);
        RETURN_IF(rcode,"send_sql");

        rcode=inside_handle_returns(st_tab[pos].cmd_ptr,buf_str);
        RETURN_IF(rcode,"handle_returns");

        return CS_SUCCEED;
}

CS_RETCODE
inside_send_sql(CS_COMMAND *cmd_ptr,CS_CHAR *query_str)
{
        CS_RETCODE       rcode=0;

        rcode = ct_command(cmd_ptr,CS_LANG_CMD,query_str,CS_NULLTERM,CS_UNUSED);
        RETURN_IF(rcode,"ct_command");

        rcode = ct_send(cmd_ptr);
        RETURN_IF(rcode,"ct_send");

        return CS_SUCCEED;
}

CS_RETCODE
inside_handle_returns(CS_COMMAND *cmd_ptr,char *buf_str)
{
        int count = 0;
        CS_RETCODE       rcode,ret;
        CS_INT   result_type,col_num,row_num,i,j,k,rows_read;
        CS_DATAFMT col_des[MAX_COLNUM];
        CS_CHAR buf[MAX_COLNUM][MAX_COLSIZE+1];
        CS_SMALLINT indicator[MAX_COLNUM];
        CS_INT tem,datalen[MAX_COLNUM];
        CS_CHAR *str_pcol_end_str[10];

        while((rcode = ct_results(cmd_ptr,&result_type))==CS_SUCCEED)
        {
                count = 0;
                switch(result_type)
                {
                        case CS_ROW_RESULT:
                                ret=ct_res_info(cmd_ptr,CS_NUMDATA,&col_num,CS_UNUSED,NULL);
                                RETURN_IF(ret,"ct_res_info");
                        //      printf("col_num=%d\n",col_num);

                                for(i=0;i<col_num;i++)
                                {
                                        ret=ct_describe(cmd_ptr,i+1,&col_des[i]);
                                        RETURN_IF(ret,"ct_describe");
                                        col_des[i].maxlength = MAX_COLSIZE;
                                        col_des[i].datatype = CS_CHAR_TYPE;

                                        col_des[i].format   = CS_FMT_UNUSED;
                                        //datalen[i]= MAX_COLSIZE;
                                        ret=ct_bind(cmd_ptr,i+1,&col_des[i],buf[i], &datalen[i],NULL);
                                        RETURN_IF(ret,"ct_bind");
                                }
                                while (ct_fetch(cmd_ptr,CS_UNUSED, CS_UNUSED, CS_UNUSED,
                                                &rows_read)==CS_SUCCEED)
                                {
                                                for(j=0;j<col_num;j++)
                                                {
                                                        buf[j][datalen[j]]='\0';
                                                        strcpy(&buf_str[count*MAX_COLNUM*(MAX_COLSIZE+1)+j*(MAX_COLSIZE+1)],buf[j]);
                                                }
                                        count++;
                                }

                                break;

                        case CS_CMD_DONE:
                                ret=ct_res_info(cmd_ptr,CS_ROW_COUNT,&row_num,CS_UNUSED,NULL);
                                RETURN_IF(ret,"ct_res_info");
                        //      printf("row_num=%d\n",row_num);
                                ct_cancel(NULL,cmd_ptr,CS_CANCEL_CURRENT);

                                break;
                        case CS_CMD_FAIL:
                                //sprintf(res_str,"%s%d%s%s",res_str,CS_CMD_FAIL,COL_END_FIX,ROW_END_FIX);
                                ret = -1;
                                RETURN_IF(ret,"CS_CMD_FAIL");
                                //return CS_CMD_FAIL;
                                break;
                        default:
                                break;
                }
        }
        switch ((int)rcode)
        {
                  case CS_END_RESULTS:
                       //sprintf(res_str,"%d%s%s",CS_SUCCEED,COL_END_FIX,ROW_END_FIX);
                       break;
                  case CS_FAIL:
                       //yyif(res_str[0]=='\0') strcat(res_str,"fail");
                       //sprintf(res_str,"%s%d%s%s",res_str,CS_FAIL,COL_END_FIX,ROW_END_FIX);
                       //strcat(res_str," fail");
                        ret = -1;
                        RETURN_IF(ret,"CS_FAIL");
                //      return CS_FAIL;
                       break;
                  default:
                       printf("use db: ct_results return unexpected result code\n");
                       return rcode;
        }

        return CS_SUCCEED;
}

CS_RETCODE close_connect_with_server()
{
        CS_RETCODE       rcode=0;
        CS_INT exit_type;

        for(int pos = 0; pos < SYSTEM_NUMBER;pos++)
        {
        	if(st_tab[pos].cntx_ptr == NULL && st_tab[pos].conn_ptr == NULL) continue;
        
		rcode = ct_close(st_tab[pos].conn_ptr,CS_UNUSED);
        	RETURN_IF(rcode,"ct_close");

        	exit_type=(rcode !=CS_SUCCEED ? CS_FORCE_EXIT:CS_UNUSED);

        	rcode=ct_exit(st_tab[pos].cntx_ptr,exit_type);
        	RETURN_IF(rcode,"ct_exit");

        	rcode=cs_ctx_drop(st_tab[pos].cntx_ptr);
        	RETURN_IF(rcode,"ct_ctx_drop");
	
		if(rcode==CS_SUCCEED) return CS_SUCCEED;
        }
        return CS_SUCCEED;
}

void sql_operate_for_price(char *sql_str)
{
        CS_RETCODE       rcode=0;

        //set_opp_rela();

        rcode=inside_query_for_price(sql_str);
        if(rcode!=CS_SUCCEED)
        {
                printf("database link failed!!\n");
                exit(-1);
        }
        //RETURN_IF(rcode,"inside_query");
}

CS_RETCODE
inside_query_for_price(char *sq_str)
{

//      CS_COMMAND       *cmd_ptr=NULL;          /* handle for this SQL command batch*/
        CS_RETCODE       rcode=0;
        int con_status,pos;
        char *str_h,*str_t,sysid[25],*query_str;
        CS_INT exit_type;

        //analyse string,get sysid and query string
        str_h=sq_str;
        str_t=sq_str;
        while(strncmp(str_h,COL_END_FIX,COL_END_FIX_LEN)) str_h++;
        *str_h='\0';
        strcpy(sysid,str_t);
        str_h+=COL_END_FIX_LEN;
        query_str=str_h;

        pos=ret_pos(sysid);

        /*to promise that each system can only connect once!*/
        if(st_tab[pos].cntx_ptr == NULL || st_tab[pos].conn_ptr == NULL)
        {

        //initial pointed database link contex
        rcode = cs_ctx_alloc(CS_VERSION_100, &(st_tab[pos].cntx_ptr));
        if(rcode!=CS_SUCCEED) printf("server %s can't connect!!\n",st_tab[pos].serv_nm);
        RETURN_IF(rcode,"cs_ctx_alloc");

        rcode = ct_init(st_tab[pos].cntx_ptr, CS_VERSION_100);
        if(rcode!=CS_SUCCEED) printf("server %s can't connect!!\n",st_tab[pos].serv_nm);
        RETURN_IF(rcode,"ct_init");


        //initial every database link connect
        rcode = ct_con_alloc(st_tab[pos].cntx_ptr, &st_tab[pos].conn_ptr);
        RETURN_IF(rcode,"ct_con_alloc");

        /* set user name and password */

        rcode = ct_con_props(st_tab[pos].conn_ptr, CS_SET, CS_USERNAME,st_tab[pos].usr_name, CS_NULLTERM, NULL);
        RETURN_IF(rcode,"ct_con_props");

        rcode = ct_con_props(st_tab[pos].conn_ptr, CS_SET, CS_PASSWORD,st_tab[pos].passwd, CS_NULLTERM, NULL);
        RETURN_IF(rcode,"ct_con_props");

        /* request connection to SQL server */
        rcode = ct_connect(st_tab[pos].conn_ptr,st_tab[pos].serv_nm,CS_NULLTERM);
        RETURN_IF(rcode,"ct_connect1");

        /* allocate command struct */
        rcode = ct_cmd_alloc(st_tab[pos].conn_ptr,&st_tab[pos].cmd_ptr);
        RETURN_IF(rcode,"ct_cmd_alloc");

        }

        rcode=inside_send_sql_for_price(st_tab[pos].cmd_ptr,query_str);
        RETURN_IF(rcode,"send_sql");

        rcode=inside_handle_returns_for_price(st_tab[pos].cmd_ptr);
        RETURN_IF(rcode,"handle_returns");

        return CS_SUCCEED;
}

int ret_pos(char *sysid)  //return position of pointed system id in config file
{
        int pos, i;

        for(i=0;i<SYSTEM_NUMBER;i++)
        {
                if(!strcmp(st_tab[i].sys_id,sysid))
                {
                        pos=i;
                        return pos;
                }
        }
        return -1;
}

CS_RETCODE
inside_send_sql_for_price(CS_COMMAND *cmd_ptr,CS_CHAR *query_str)
{
        CS_RETCODE       rcode=0;

        rcode = ct_command(cmd_ptr,CS_LANG_CMD,query_str,CS_NULLTERM,CS_UNUSED);
        RETURN_IF(rcode,"ct_command");


        rcode = ct_send(cmd_ptr);
        RETURN_IF(rcode,"ct_send");

        return CS_SUCCEED;
}

CS_RETCODE
inside_handle_returns_for_price(CS_COMMAND *cmd_ptr)
{
        CS_RETCODE       rcode,ret;
        CS_INT   result_type,col_num,row_num,i,j,k,rows_read;
        CS_DATAFMT col_des[MAX_COLNUM];
        CS_CHAR buf[MAX_COLNUM][MAX_COLSIZE+1];
        CS_SMALLINT indicator[MAX_COLNUM];
        CS_INT tem,datalen[MAX_COLNUM];
        CS_CHAR *str_pcol_end_str[10];

        int count = 0;

        while((rcode = ct_results(cmd_ptr,&result_type))==CS_SUCCEED)
        {
        	//printf("\n\ncount=%d, result_type=%d",count,result_type);

                switch(result_type)
                {
                        case CS_ROW_RESULT:
                                ret=ct_res_info(cmd_ptr,CS_NUMDATA,&col_num,CS_UNUSED,NULL);
                                RETURN_IF(ret,"ct_res_info");
                        //      printf("col_num=%d\n",col_num);

                                for(i=0;i<col_num;i++)
                                {
                                        ret=ct_describe(cmd_ptr,i+1,&col_des[i]);
                                        RETURN_IF(ret,"ct_describe");

                                        col_des[i].maxlength = MAX_COLSIZE;
                                        col_des[i].datatype = CS_CHAR_TYPE;
                                        col_des[i].format   = CS_FMT_UNUSED;
                                        //datalen[i]= MAX_COLSIZE;
                                        ret=ct_bind(cmd_ptr,i+1,&col_des[i],buf[i], &datalen[i],NULL);
                                        RETURN_IF(ret,"ct_bind");
                                }
                                while (ct_fetch(cmd_ptr,CS_UNUSED, CS_UNUSED, CS_UNUSED,&rows_read)==CS_SUCCEED)
                                {
                                                for(j=0;j<col_num;j++)
                                                {
                                                        buf[j][datalen[j]]='\0';
                                                //      printf("%dbuf = %s",j,buf[j]);
                                                        switch(j)
                                                        {
                                                                case 0:
                                                                        strcpy(seq_price[count].uni_id,buf[j]);
                                                                //      printf("uni_id=%s\n",seq_price[count].uni_id);
                                                                        break;
                                                                case 1:
                                                                        strcpy(seq_price[count].un_name,buf[j]);
                                                                //      printf("un_name=%s\n",seq_price[count].un_name);
                                                                        break;
                                                                case 2:
                                                                        seq_price[count].out_power=atoi(buf[j]);
                                                                //      printf("out_power=%d\n",seq_price[count].out_power)
;
                                                                        break;
                                                                case 3:
                                                                        seq_price[count].price=atof(buf[j]);
                                                                //      printf("price=%f\n",seq_price[count].price);
                                                                        break;
                                                                case 4:

                                                                        seq_price[count].fac_used_ratio=atof(buf[j]);
                                                                //      printf("fac_used_ratio=%f\n",seq_price[count].fac_used_ratio);
                                                                        break;
                                                        }
                                                }
                                                count++;
                                        	//printf("count = %d\n",count);
                                }//end while

                                break;

                        case CS_CMD_DONE:
                                ret=ct_res_info(cmd_ptr,CS_ROW_COUNT,&row_num,CS_UNUSED,NULL);
                                RETURN_IF(ret,"ct_res_info");
                        //      printf("row_num=%d\n",row_num);
                                ct_cancel(NULL,cmd_ptr,CS_CANCEL_CURRENT);

                                break;
                        case CS_CMD_FAIL:
                                //sprintf(res_str,"%s%d%s%s",res_str,CS_CMD_FAIL,COL_END_FIX,ROW_END_FIX);
                                ret = -1;
                                RETURN_IF(ret,"CS_CMD_FAIL");
                                //return CS_CMD_FAIL;
                                break;
                        default:
                                break;
                }
        }
        switch ((int)rcode)
        {
                  case CS_END_RESULTS:
                       break;
                  case CS_FAIL:
                        ret = -1;
                        RETURN_IF(ret,"CS_FAIL");
                       break;
                  default:
                       printf("use db: ct_results return unexpected result code\n");
                       return rcode;

        }

        return CS_SUCCEED;
}

int get_seq_price_num(char *year_str,char *mon_str,char *day_str,int time_block)
{
        int num;
        char sql_str[SQL_STRING_LENGTH];
        char buf[1*MAX_COLNUM*(MAX_COLSIZE+1)];

        sprintf(sql_str,"fdbj@^$$^@select count(*) from fdbj%s..T_XHSCJJSB_%s_%s where SD=%d",year_str,mon_str,day_str,time_block);
//      printf("%s\n",sql_str);

        sql_operate(sql_str,buf);
        num = atoi(&buf[0]);
        return num;
}

void get_price_struct(char *year_str,char *mon_str,char *day_str,int time_block)
{
        seq_price_num = get_seq_price_num(year_str,mon_str,day_str,time_block);
        seq_price = new bid_price_struct[seq_price_num];

        char sql_str[SQL_STRING_LENGTH];

        sprintf(sql_str,"fdbj@^$$^@select JZID,JZMC,CL,ZSHDJ,CYL from fdbj%s..T_XHSCJJSB_%s_%s where  SD=%d order by JZID,CL",year_str,mon_str,day_str,time_block);

//      printf("\nsql_str=%s\n",sql_str);
        sql_operate_for_price(sql_str);

}

void form_point_num(int *point_num,int un_num)
{
	register int i;
	int ii,k;
	///////////////////////////

	initVector(point_num,(long)0,un_num-1,0);//clearing
	
	ii=0;	//index for nu
	k=1;	//counter for point_num[]

	for(i = 1; i < seq_price_num; i ++)
        {
		if(!strcmp(seq_price[i].uni_id, seq_price[i-1].uni_id))
		{
 			k++;
		}
		else
		{ 
			point_num[ii]=k;
			
			k=1;
			ii++;
		}
        }//end for

	point_num[ii]=k;//No Change!
	//////////////////////////////////

	return; 
}

int get_un_num()
{
	int un_num=1;//init.
        for(int i = 1; i < seq_price_num; i ++)
        {
                if(strcmp(seq_price[i].uni_id, seq_price[i-1].uni_id))
                        un_num++;
        }//end for

	return un_num;
}

void OPF_readSybase(char *year,char *month,char *day,int sd,long prnt)
{
	if(prnt==YES) printf("\n\nOPF_readSybase()...");

	register long i,j;
	int k,ret2,bid_sd=(sd-1)/3+1;//No Change!
        
	char rq_str[RQ_LEN],time_str[TIME_LEN],sql_str[SQL_STRING_LENGTH];
	char res_str[RESULT_STRING_LENGTH];
        
	char *buf1,*buf10,*buf20,*buf50,*buf60,*buf100,*buf500;
	/////////////////////////////////////////////////////////

        buf1    = new char[BUF1_LEN];
        buf10   = new char[BUF10_LEN];
        buf20   = new char[BUF20_LEN];
        buf50   = new char[BUF50_LEN];
        buf60   = new char[BUF60_LEN];
        buf100  = new char[BUF100_LEN];
	buf500	= new char[BUF500_LEN];
	//////////////////////////////////////////////////

        form_rq_str(rq_str,month,day,year);
        form_time_str(time_str,sd);
		
	set_opp_rela();
	printf("\n set_opp_rela() ok");	
	/////////////////////////////////////////////////////////////////

	//1: Reading groupData from T_AQYSLLXZXZ 

	long group_num,group_line_num,group_mode_num,outage_num;

	//.1 distinct group_num

	initString(sql_str,SQL_STRING_LENGTH);
        initString(buf1,BUF1_LEN);
        sprintf(sql_str,"xtgl@^$$^@select count(distinct LLXZID) from xtgl..T_AQYSLLXZBM"); 
	
	sql_operate(sql_str,buf1);
	group_num=atoi(&buf1[0]);	
       	/////////////////////////////////////////////////

	//.2 group_id and _descr
 
	initString(sql_str,SQL_STRING_LENGTH);
        initString(buf100,BUF100_LEN);

        sprintf(sql_str,"xtgl@^$$^@select distinct LLXZID from xtgl..T_AQYSLLXZBM");
        sql_operate(sql_str,buf100);
	//////////////////////////////////////////////////////////////////////

	//.3 get outage_num 

	initString(sql_str,SQL_STRING_LENGTH);
        initString(buf1,BUF1_LEN);

	sprintf(sql_str,"jhjygl@^$$^@select count(XLID) from jygl%s..T_WLXLZT where QSRQ<='%s' and ZZRQ>='%s' and QSSJ<='%s' and ZZSJ>'%s'",year,rq_str,rq_str,time_str,time_str);
	sql_operate(sql_str,buf1);

	outage_num=atoi(&buf1[0]);
	if(prnt==YES) printf("\n outage_num=%d",outage_num);
	///////////////////////////////////////////////////////

	//.4 outage lines --- stored into buf60[]

	initString(sql_str,SQL_STRING_LENGTH);
	initString(buf60,BUF60_LEN);

	sprintf(sql_str,"jhjygl@^$$^@select XLID from jygl%s..T_WLXLZT where QSRQ<='%s' and ZZRQ>='%s' and QSSJ<='%s' and ZZSJ>'%s'",year,rq_str,rq_str,time_str,time_str);

	//printf("\n sql_str = %s",sql_str);
	sql_operate(sql_str,buf60);
	//////////////////////////////////////////////////////

	if(prnt==YES) printf("\n Reading groupData...");

	groupData=NULL;
	struct groupstr *grp;

	for(i=0;i<group_num;i++)
	{
		strcpy(idbuf,&buf100[i*MAX_COLNUM*(MAX_COLSIZE+1)+0*(MAX_COLSIZE+1)]);
		if(!strncmp(idbuf,"dz",2)) continue;//No Change!
		////////////////////////////////////////////////////////
		
		initString(sql_str,SQL_STRING_LENGTH);
        	initString(buf20,BUF20_LEN);

		sprintf(sql_str,"xtgl@^$$^@select distinct LLXZMC from xtgl..T_AQYSLLXZBM where LLXZID = '%s'",idbuf);
		sql_operate(sql_str,buf20);
		
		strcpy(buf,&buf20[0]);//group_descr saved into buf[]
		//////////////////////////////////////////////////////////////
	
		initString(sql_str,SQL_STRING_LENGTH);
                initString(buf1,BUF1_LEN);
                
                sprintf(sql_str,"jhjygl@^$$^@select count(LLXZID) from jygl%s..T_AQYSLLXZXZ where convert(datetime,RQ)=(select max(convert(datetime,RQ)) from jygl%s..T_AQYSLLXZXZ) and LLXZID = '%s'",year,year,idbuf);
                //printf("\ni = %ld sql_str = %s",i,sql_str);
		sql_operate(sql_str,buf1);
	
		group_mode_num=atoi(&buf1[0]);
		//printf("\n group_mode_num = %ld",group_mode_num);
		//////////////////////////////////////////////////////

		initString(sql_str,SQL_STRING_LENGTH);
	        initString(buf1,BUF1_LEN);
        	
		sprintf(sql_str,"xtgl@^$$^@select count(SHXLID) from xtgl..T_AQYSLLXZBM where LLXZID = '%s'",idbuf);
		//printf("\ni = %ld sql_str = %s",i,sql_str);
		sql_operate(sql_str,buf1);
		
		group_line_num=atoi(&buf1[0]);
		//printf("\ngroup_line_num=%ld",group_line_num);	
		////////////////////////////////////////////////////////

		initString(sql_str,SQL_STRING_LENGTH);
                initString(buf50,BUF50_LEN);

                sprintf(sql_str,"xtgl@^$$^@select LLXZID,SHXLID,JLDCZID from xtgl..T_AQYSLLXZBM where LLXZID = '%s'",idbuf);
                sql_operate(sql_str,buf50);
		////////////////////////////////////////////

		initString(sql_str,SQL_STRING_LENGTH);
                initString(buf20,BUF20_LEN);

                sprintf(sql_str,"jhjygl@^$$^@select YGXZ,LLXZFSID,LLXTDFSID,TJXZXLID1,XZTJ1,XZXZ1,TJXZXLID2,XZTJ2,XZXZ2,TJXZXLID3,XZTJ3,XZXZ3  from jygl%s..T_AQYSLLXZXZ where convert(datetime,RQ)=(select max(convert(datetime,RQ)) from jygl%s..T_AQYSLLXZXZ where convert(datetime,RQ) <= convert(datetime,'%s')) and LLXZID = '%s'",year,year,rq_str,idbuf);
                //printf("\ni = %ld sql_str = %s",i,sql_str);
                sql_operate(sql_str,buf20);
		/////////////////////////////////////////////////////////////////////

		grp=(struct groupstr *)malloc(sizeof(struct groupstr));
		if(grp==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }
		
		stringEnd(idbuf,' ');
		stringEnd(buf,' ');

		strcpy(grp->id,idbuf);	
		strcpy(grp->descr,buf);	
		////////////////////////////////////////////////

		//.1 reading group lines

		k=0;//init.
		for(j=0;j<group_line_num;j++)
		{
			strcpy(idbuf2,&buf50[j*MAX_COLNUM*(MAX_COLSIZE+1)+1*(MAX_COLSIZE+1)]);
			strcpy(buffer,&buf50[j*MAX_COLNUM*(MAX_COLSIZE+1)+2*(MAX_COLSIZE+1)]);
			
			stringEnd(idbuf2,' ');//line_id
			stringEnd(buffer,' ');
			///////////////////////////////////////////////////////

			ret2=lookup(buf60,idbuf2,outage_num,(long)0);

			if(ret2==NO)//not outage
			{
				k++;
				strcpy(grp->line[k].id,idbuf2);
				strcpy(grp->line[k].mst,buffer);
			}//end if
		}//end for
	
		grp->lineNum=k;//No Change!
		/////////////////////////////////////////////

		//.2 reading group modes

		for(j=0;j<group_mode_num;j++)
		{
			strcpy(idbuf2,&buf20[j*MAX_COLNUM*(MAX_COLSIZE+1)+1*(MAX_COLSIZE+1)]);
			stringEnd(idbuf2,(long)4);
	
			strcpy(grp->mode[j+1].id,idbuf2);
			grp->mode[j+1].lmt=atoi(&buf20[j*MAX_COLNUM*(MAX_COLSIZE+1)+0*(MAX_COLSIZE+1)]);
			////////////////////////////////////////////////

			//mode 1

			strcpy(idbuf2,&buf20[j*MAX_COLNUM*(MAX_COLSIZE+1)+3*(MAX_COLSIZE+1)]);
			stringEnd(idbuf2,' ');
 
			if(!strcmp(idbuf2,"null")) stringEnd(idbuf2,(long)0); 
			if(!strcmp(idbuf2,"NULL")) stringEnd(idbuf2,(long)0);
	
			strcpy(grp->mode[j+1].id1,idbuf2);
			grp->mode[j+1].mel1=melFun(&buf20[j*MAX_COLNUM*(MAX_COLSIZE+1)+4*(MAX_COLSIZE+1)]);
			grp->mode[j+1].val1=atoi(&buf20[j*MAX_COLNUM*(MAX_COLSIZE+1)+5*(MAX_COLSIZE+1)]);
			/////////////////////////////////////////

			//mode 2

			strcpy(idbuf2,&buf20[j*MAX_COLNUM*(MAX_COLSIZE+1)+6*(MAX_COLSIZE+1)]);
			stringEnd(idbuf2,' ');

			if(!strcmp(idbuf2,"null")) stringEnd(idbuf2,(long)0);
                        if(!strcmp(idbuf2,"NULL")) stringEnd(idbuf2,(long)0);
	
			strcpy(grp->mode[j+1].id2,idbuf2);
			grp->mode[j+1].mel2=melFun(&buf20[j*MAX_COLNUM*(MAX_COLSIZE+1)+7*(MAX_COLSIZE+1)]);
			grp->mode[j+1].val2=atoi(&buf20[j*MAX_COLNUM*(MAX_COLSIZE+1)+8*(MAX_COLSIZE+1)]);
			///////////////////////////////////////////

			//mode 3

			strcpy(idbuf2,&buf20[j*MAX_COLNUM*(MAX_COLSIZE+1)+9*(MAX_COLSIZE+1)]);
			stringEnd(idbuf2,' '); 

			if(!strcmp(idbuf2,"null")) stringEnd(idbuf2,(long)0);
                        if(!strcmp(idbuf2,"NULL")) stringEnd(idbuf2,(long)0);

			strcpy(grp->mode[j+1].id3,idbuf2);
			grp->mode[j+1].mel3=melFun(&buf20[j*MAX_COLNUM*(MAX_COLSIZE+1)+10*(MAX_COLSIZE+1)]);
			grp->mode[j+1].val3=atoi(&buf20[j*MAX_COLNUM*(MAX_COLSIZE+1)+11*(MAX_COLSIZE+1)]);
		}//end for
		
		grp->modeNum=group_mode_num;
		//////////////////////////////////////

		grp->next=groupData;
		groupData=grp;
	}//end for

	if(prnt==YES) printf("ok");
	////////////////////////////////////////////////////

	//2: Reading loadData 

	long load_num;

        initString(sql_str,SQL_STRING_LENGTH);
        initString(buf1,BUF1_LEN);
	sprintf(sql_str,"jhjygl@^$$^@select count(JDID) from jygl%s..T_SSJYDXRJDFH where SD=1 and DXRLXID='cjxq1'",year);
	sql_operate(sql_str,buf1);
	load_num = atoi(&buf1[0]);

        initString(sql_str,SQL_STRING_LENGTH);
        initString(buf500,BUF500_LEN);
	sprintf(sql_str,"jhjygl@^$$^@select JDID,YGGL,WGGL from jygl%s..T_SSJYDXRJDFH where SD=1 and DXRLXID='cjxq1'",year);
	//printf("\nsql_str = %s",sql_str);
	sql_operate(sql_str,buf500);
	////////////////////////////////////////////////////////

	if(prnt==YES) printf("\n Reading loadData...");
	double wload,rload;
	
	loadData=NULL;
	struct loadstr *lp;

	for(i=0;i<load_num;i++)
	{
		wload=atoi(&buf500[i*MAX_COLNUM*(MAX_COLSIZE+1)+1*(MAX_COLSIZE+1)]);
		rload=atoi(&buf500[i*MAX_COLNUM*(MAX_COLSIZE+1)+2*(MAX_COLSIZE+1)]);

		if(fabs(wload)+fabs(rload)<SMLL) continue;
		///////////////////////////////////////////////////////////////

		lp=(struct loadstr *)malloc(sizeof(struct loadstr));
		if(lp==NULL)
		{ 
			printf("\nError! --- %ld",__LINE__); 
			consoleMsg("错误","Allocation Failed",__FILE__,__LINE__);
			
			exit(0);
		}//end if

		strcpy(idbuf,&buf500[i*MAX_COLNUM*(MAX_COLSIZE+1)+0*(MAX_COLSIZE+1)]);
                stringEnd(idbuf,' ');

		strcpy(lp->id,idbuf);
		lp->w	=wload;
		lp->r	=rload;
		////////////////////////////////////
	
		lp->next=loadData;
		loadData=lp;	
	}//end for

	if(prnt==YES) printf("ok");
	///////////////////////////////////////////////////////

	//3: Reading plantData (non-bidding plant)

	int fix_plant_unitNum,fix_plant_num,fix_plant_power;
	double totlRL,fac_ratio,aux2;
	double LV_fix_power,HV_fix_power;

        initString(res_str,RESULT_STRING_LENGTH);
        initString(sql_str,SQL_STRING_LENGTH);

        sprintf(sql_str,"xtgl@^$$^@select  CSSZ from xtgl..T_XTKZCS where CSMC='jhdcpjcyl' and convert(datetime,BZRQ)=(select max(convert(datetime,BZRQ))from xtgl..T_XTKZCS where CSMC='jhdcpjcyl' and BZRQ <='%s')",rq_str);
        sql_operate(sql_str,res_str);

        fac_ratio=atof(res_str);
	if(prnt==YES) printf("\n fac_ratio = %lf",fac_ratio);
        //////////////////////////////////////////////////////////

        initString(sql_str,SQL_STRING_LENGTH);
	initString(buf1,BUF1_LEN);
        sprintf(sql_str,"xtgl@^$$^@select count(distinct DCID) from xtgl..T_CLJSJHTSJZDC");
	sql_operate(sql_str,buf1);
        
	fix_plant_num = atoi(&buf1[0]);
	//printf("\n fix_plant_num = %d",fix_plant_num);

        initString(sql_str,SQL_STRING_LENGTH);
	initString(buf50,BUF50_LEN);
        sprintf(sql_str,"xtgl@^$$^@select distinct DCID from xtgl..T_CLJSJHTSJZDC");

        sql_operate(sql_str,buf50);
	////////////////////////////////////////////////////////////////

        if(prnt==YES) printf("\n Reading plantData (non-bidding)...");

	HV_fix_power=0;//init.	
	strcpy(buf,"");

	plantData=NULL;
        struct plantstr *plp;
        
	for(i=0;i<fix_plant_num;i++)
        {
        	strcpy(idbuf,&buf50[i*MAX_COLNUM*(MAX_COLSIZE+1)+0*(MAX_COLSIZE+1)]);
		stringEnd(idbuf,' ');

		if(!strcmp(buf,idbuf)) continue;
		strcpy(buf,idbuf);//reserved
		/////////////////////////////////////////////////////////
	
		initString(sql_str,SQL_STRING_LENGTH);
		initString(buf1,BUF1_LEN);
		sprintf(sql_str,"jhjygl@^$$^@select sum(YGGL) from jygl%s..T_SSDDDDLDCRFDJH_%s_%s where SD=%d and DCID='%s'",year,month,day,bid_sd,idbuf);
	
		sql_operate(sql_str,buf1);
		fix_plant_power=atoi(&buf1[0]);

		//if(prnt==YES) printf("\n\n  idbuf=%s,power =%d",idbuf,fix_plant_power);
		if(fix_plant_power<=SMLL) continue;
                ////////////////////////////////////////////////////////////

		initString(sql_str,SQL_STRING_LENGTH);
		initString(buf1,BUF1_LEN);
	
		sprintf(sql_str,"xtgl@^$$^@select count(JZID) from xtgl..T_JZBM where SZDCID='%s' and SFJJ = 0",idbuf);
		//printf("\nsql_str=%s",sql_str);
		sql_operate(sql_str,buf1);

		fix_plant_unitNum=atoi(&buf1[0]);
		//if(prnt==YES) printf("\n   fix_plant_unitNum=%ld",fix_plant_unitNum);
 		////////////////////////////////////////////////////////////

		initString(sql_str,SQL_STRING_LENGTH);
                initString(buf100,BUF100_LEN);
	
		sprintf(sql_str,"xtgl@^$$^@select JZID,SZDCID,RL,SFKY from xtgl..T_JZBM where SZDCID='%s' and SFJJ=0",idbuf);
		sql_operate(sql_str,buf100);
		///////////////////////////////////////////////////////////

		plp=(struct plantstr *)malloc(sizeof(struct plantstr));
		if(plp==NULL)
		{ 
			printf("\nError! --- %ld",__LINE__); 
			consoleMsg("错误","Allocation Failed",__FILE__,__LINE__);

			exit(0); 
		}//end if
		///////////////////////////////////////

                strcpy(plp->id,idbuf);//plant_id
		
		plp->unitNum	=fix_plant_unitNum;
		plp->w		=fix_plant_power*(1-fac_ratio);

		HV_fix_power+= fix_plant_power*(1-fac_ratio); 

		for(j=0;j<fix_plant_unitNum;j++)
		{
                	strcpy(idbuf2,&buf100[j*MAX_COLNUM*(MAX_COLSIZE+1)+0*(MAX_COLSIZE+1)]);
                	stringEnd(idbuf2,' ');
			
			strcpy(plp->unit[j+1].id,idbuf2);
			plp->unit[j+1].wmx=atoi(&buf100[j*MAX_COLNUM*(MAX_COLSIZE+1)+2*(MAX_COLSIZE+1)]);
        	}//end for
		/////////////////////////////////////////

		plp->next=plantData;
		plantData=plp;
	}//end for

	if(prnt==YES) printf("ok");
	/////////////////////////////////////////////////

	//4: Reading genrData2 (non-bidding)

        int fix_un_num,fix_un_power;

        initString(sql_str,SQL_STRING_LENGTH);
	initString(buf1,BUF1_LEN);
        sprintf(sql_str,"jhjygl@^$$^@select count(JZID) from jygl%s..T_SSDDDDLJZRFDJH_%s_%s where SD=%d",year,month,day,bid_sd);
        sql_operate(sql_str,buf1);
        fix_un_num = atoi(&buf1[0]);

        initString(sql_str,SQL_STRING_LENGTH);
	initString(buf10,BUF10_LEN);
        sprintf(sql_str,"jhjygl@^$$^@select JZID,YGGL from jygl%s..T_SSDDDDLJZRFDJH_%s_%s where SD=%d",year,month,day,bid_sd);

        sql_operate(sql_str,buf10);
	//////////////////////////////////////////////////////////////

	if(prnt==YES) printf("\n Reading genrData2 (non-bidding)...");
        
	genrData2=NULL;
	struct genrstr *gp2;

	for(i=0;i<fix_un_num;i++)
        {
                fix_un_power=atoi(&buf10[i*MAX_COLNUM*(MAX_COLSIZE+1)+1*(MAX_COLSIZE+1)]);
                if(fix_un_power<=SMLL) continue;
                ////////////////////////////////////////////////////////////

		gp2=(struct genrstr *)malloc(sizeof(struct genrstr));
                if(gp2==NULL)
		{ 
			printf("\nError! --- %ld",__LINE__); 
			consoleMsg("Error","Allocation Failed",__FILE__,__LINE__);

			exit(0); 
		}//end if
		//////////////////////////////////////////////////////////

                strcpy(idbuf,&buf10[i*MAX_COLNUM*(MAX_COLSIZE+1)+0*(MAX_COLSIZE+1)]);
                stringEnd(idbuf,' ');

                strcpy(gp2->id,idbuf);
                gp2->P0=atoi(&buf10[i*MAX_COLNUM*(MAX_COLSIZE+1)+1*(MAX_COLSIZE+1)]);

                gp2->next=genrData2;
                genrData2=gp2;
        }//end of for

        if(prnt==YES) printf("ok");
        //////////////////////////////////////////////////////////

	//5: Reading parameters --- av_volt, flow_eps, etc.

	if(prnt==YES) printf("\n Reading parameters...");

	//5.1 av_volt
	
	initString(sql_str,SQL_STRING_LENGTH);
	sprintf(sql_str,"xtgl@^$$^@select  CSSZ from xtgl..T_XTKZCS where CSMC='pjdy' and convert(datetime,BZRQ)=(select max(convert(datetime,BZRQ)) from xtgl..T_XTKZCS where CSMC='pjdy' and BZRQ <= '%s')",rq_str);

        sql_operate(sql_str,res_str);
        av_volt=atof(res_str);
	//////////////////////////////////////////

	//5.2 flow_eps

	initString(res_str,RESULT_STRING_LENGTH);
	initString(sql_str,SQL_STRING_LENGTH);
	sprintf(sql_str,"xtgl@^$$^@select  CSSZ from xtgl..T_XTKZCS where CSMC='clsljd' and convert(datetime,BZRQ)=(select max(convert(datetime,BZRQ)) from xtgl..T_XTKZCS where CSMC='clsljd' and BZRQ <= '%s')",rq_str);

	sql_operate(sql_str,res_str);
        flow_eps=atof(res_str);
	/////////////////////////////////////////////

	//5.3 genw_eps
 
	initString(res_str,RESULT_STRING_LENGTH);
        initString(sql_str,SQL_STRING_LENGTH);
        sprintf(sql_str,"xtgl@^$$^@select  CSSZ from xtgl..T_XTKZCS where CSMC='fdjygtzjd' and convert(datetime,BZRQ)=(select max(convert(datetime,BZRQ)) from xtgl..T_XTKZCS where CSMC='fdjygtzjd' and BZRQ <= '%s')",rq_str);

        sql_operate(sql_str,res_str);
        genw_eps=atof(res_str);
	//////////////////////////////////////////

	//5.4 genw_maxstep

        initString(res_str,RESULT_STRING_LENGTH);
        initString(sql_str,SQL_STRING_LENGTH);
        sprintf(sql_str,"xtgl@^$$^@select  CSSZ from xtgl..T_XTKZCS where CSMC='fdjygtzzdyxbc' and convert(datetime,BZRQ)=(select max(convert(datetime,BZRQ)) from xtgl..T_XTKZCS where CSMC='fdjygtzzdyxbc' and BZRQ <= '%s')",rq_str);

        sql_operate(sql_str,res_str);
        genw_maxstep=atof(res_str);
	///////////////////////////////////////////////

	//5.5 opf_island

	initString(res_str,RESULT_STRING_LENGTH);
	initString(sql_str,SQL_STRING_LENGTH);
	sprintf(sql_str,"xtgl@^$$^@select  CSSZ from xtgl..T_XTKZCS where CSMC='zdh'");

	sql_operate(sql_str,res_str);
	opf_island=atoi(res_str);
	////////////////////////////////////////

	//5.6 coslack_num

	initString(res_str,RESULT_STRING_LENGTH);
        initString(sql_str,SQL_STRING_LENGTH);
	sprintf(sql_str,"xtgl@^$$^@select  CSSZ from xtgl..T_XTKZCS where CSMC='lhtzfdjs'");

        sql_operate(sql_str,res_str);
	coslack_num=atoi(res_str);

	if(coslack_num<1) coslack_num=1;//default
	///////////////////////////////////////////

	//5.7 loss_ratio

	initString(res_str,RESULT_STRING_LENGTH);
        initString(sql_str,SQL_STRING_LENGTH);
        sprintf(sql_str,"xtgl@^$$^@select  CSSZ from xtgl..T_XTKZCS where CSMC='wsxs' and convert(datetime,BZRQ)=(select max(convert(datetime,BZRQ)) from xtgl..T_XTKZCS where CSMC='wsxs')");

        sql_operate(sql_str,res_str);

	loss_ratio=atof(res_str)/100.0;
	//////////////////////////////////////////////

	//5.8 sys_wload
 
        initString(sql_str,SQL_STRING_LENGTH);
        initString(buf1,BUF1_LEN);

	sprintf(sql_str,"jhjygl@^$$^@select sum(YGGL) from jygl%s..T_SSDDDDLDCRFDJH_%s_%s where SD=%d",year,month,day,bid_sd); 
	sql_operate(sql_str,buf1);
	LV_fix_power=atoi(&buf1[0])-HV_fix_power;

	initString(sql_str,SQL_STRING_LENGTH);
        initString(buf1,BUF1_LEN);

	sprintf(sql_str,"jhjygl@^$$^@select CDQFHYCZ from jygl%s..T_SSJYRCDQFHYC_%s where RQ='%s' and SD=%d and CDQFHYCLXID=(select CYFHYCID from jygl%s..T_SSJYCYFHYC_%s where RQ='%s' and SD = %d)",year,month,rq_str,sd,year,month,rq_str,sd);

        sql_operate(sql_str,buf1);
	sys_wload=(atoi(&buf1[0])-LV_fix_power)/100.0;//MW --> p.u.
	///////////////////////////////////////////////

	//5.9 ov_coeff
	
	initString(sql_str,SQL_STRING_LENGTH);
        initString(buf1,BUF1_LEN);
	sprintf(sql_str,"xtgl@^$$^@select CSSZ from xtgl..T_XTKZCS where CSMC='ygtzgtxs'");
	
	sql_operate(sql_str,buf1);
	ov_coeff=atof(&buf1[0]);

	if(prnt==YES) printf("ok");//para. saved in *.bas/.vrd
	///////////////////////////////////////////////////////////////

	//6: Reading priceData

	int un_num;

	get_price_struct(year,month,day,bid_sd);
	un_num =  get_un_num();

        point_num = new int[un_num];
	form_point_num(point_num,un_num);
	///////////////////////////////////////////////////

	if(prnt==YES) printf("\n Reading priceData...");

	priceData=NULL;//init.
	struct pricstr *pp;

	long ik=0;//init.
	for(i=0;i<un_num;i++)
	{
		pp=(struct pricstr *)malloc(sizeof(struct pricstr));
		if(pp==NULL)
		{ 
			printf("\nError! --- %ld",__LINE__); 
			consoleMsg("Error","Allocation Failed",__FILE__,__LINE__);

			exit(0); 
		}//end if
		/////////////////////////////////////////////////

		strcpy(idbuf,seq_price[ik].uni_id);
		stringEnd(idbuf,' ');

		strcpy(pp->id,idbuf);
		pp->i		=0;//No ibs data
		
		pp->pntNum	=point_num[i];
		pp->Pb		=1.0;
		pp->Cb		=1.0;

		for(j=1;j<=pp->pntNum;j++)//No Change!
		{
			pp->point[j].p=seq_price[ik+j-1].out_power;
			pp->point[j].c=seq_price[ik+j-1].price;
		}//end for

		ik+= point_num[i];
		////////////////////////////

		pp->next	=priceData;
		priceData	=pp;
	}//end of for
		
	if(prnt==YES) printf("ok");
	//////////////////////////////////////////////////////////
   	
        //7: Reading genrData --- bidding units 

        int bid_gen_num,gen_power;
	
	initString(sql_str,SQL_STRING_LENGTH);
	initString(buf1,BUF1_LEN);
        sprintf(sql_str,"jhjygl@^$$^@select count(distinct JZID) from jygl%s..T_SSSWYYSRJZFDJH_%s_%s",year,month,day);
        sql_operate(sql_str,buf1);
        bid_gen_num = atoi(&buf1[0]);

	initString(sql_str,SQL_STRING_LENGTH);
	initString(buf100,BUF100_LEN);
        sprintf(sql_str,"jhjygl@^$$^@select JZID,SSDDYGGL,SSDDZKDRL,SSDDFKDRL,JZMC from jygl%s..T_SSSWYYSRJZFDJH_%s_%s where SD=%d",year,month,day,sd);
	
        sql_operate(sql_str,buf100);
	//////////////////////////////////////////////////////////////////

        if(prnt==YES) printf("\n Reading genrData...");

        genrData=NULL;
        struct genrstr *gp;

        for(i=0;i<bid_gen_num;i++)
        {
                gen_power=atoi(&buf100[i*MAX_COLNUM*(MAX_COLSIZE+1)+1*(MAX_COLSIZE+1)]);
                if(gen_power<=SMLL) continue;
                ////////////////////////////////////////////////////////////

                gp=(struct genrstr *)malloc(sizeof(struct genrstr));
                if(gp==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }

                strcpy(idbuf,&buf100[i*MAX_COLNUM*(MAX_COLSIZE+1)+0*(MAX_COLSIZE+1)]);
                strcpy(buf,&buf100[i*MAX_COLNUM*(MAX_COLSIZE+1)+4*(MAX_COLSIZE+1)]);
                
		stringEnd(idbuf,' ');
                stringEnd(buf,' ');

                strcpy(gp->id,idbuf);
                strcpy(gp->descr,buf);
 
		gp->i   =0;//init.
		gp->Pc	=0.0;
                gp->P0  =gen_power;
                gp->Pmin=gen_power - atoi(&buf100[i*MAX_COLNUM*(MAX_COLSIZE+1)+3*(MAX_COLSIZE+1)]);
                gp->Pmax=gen_power + atoi(&buf100[i*MAX_COLNUM*(MAX_COLSIZE+1)+2*(MAX_COLSIZE+1)]);
                /////////////////////////////////////////////////

                gp->next=genrData;
                genrData=gp;
        }//end of for

        if(prnt==YES) printf("ok");
        /////////////////////////////////////////////////////////////

	//8: Reading genrData_ramp

	initString(sql_str,SQL_STRING_LENGTH);
	initString(buf100,BUF100_LEN);

	sprintf(sql_str,"fdbj@^$$^@ select JZID,ZDPPSD,ZDJCLSD from fdbj%s..T_XHSCRLSB_%s_%s where SD=%d",year,month,day,bid_sd);
	sql_operate(sql_str,buf100);
	///////////////////////////////////////////

	if(prnt==YES) printf("\n Reading genrData_ramp...");

	gp=genrData;
	while(gp!=NULL)
	{
                gp->ramp=getRamp(gp->id,buf100,bid_gen_num)*SD_LEN;
		gp->fall=getFall(gp->id,buf100,bid_gen_num)*SD_LEN;
                
		gp=gp->next;
        }//end while 

        if(prnt==YES) printf("ok");
	/////////////////////////////////////////////////////////

	//9: Reading areaData --- area powers 

	int line_num; 

	//901: cal. total
       
	initString(sql_str,SQL_STRING_LENGTH);
        initString(buf1,BUF1_LEN);

	sprintf(sql_str,"jhjygl@^$$^@select sum(SSDDYG) from jygl%s..T_RLLXJH where RQ='%s' and SD=%d",year,rq_str,bid_sd);
	//printf("sql=%s\n",sql_str);

        sql_operate(sql_str,buf1);
	int total_sched=-atoi(&buf1[0]);

	if(prnt==YES) printf("\n total_sched=%d",total_sched);
	/////////////////////////////////////////////////

	//902: cal. lm_load
 
        initString(sql_str,SQL_STRING_LENGTH);
        initString(buf1,BUF1_LEN);

        sprintf(sql_str,"jhjygl@^$$^@select YGGL from jygl%s..T_RBJFH where RQ='%s' and SD=%d",year,rq_str,bid_sd);
        //printf("sql=%s\n",sql_str);

        sql_operate(sql_str,buf1);
       
 	int lm_load = atoi(&buf1[0]);
	sys_wload+= lm_load/100.0;//sys_wload obtained

	if(prnt==YES) printf("\n lm_load  =%d",lm_load);
	if(prnt==YES) printf("\n sys_wload=%lf",sys_wload);
	/////////////////////////////////////////////////////

	//903: cal. lm_sched

        initString(sql_str,SQL_STRING_LENGTH);
        initString(buf1,BUF1_LEN);

        sprintf(sql_str,"jhjygl@^$$^@select YGGL from jygl%s..T_BJJZFDJH where RQ='%s' and SD=%d and DCJZID='yuanbaop'",year,rq_str,bid_sd);
      	//printf("sql=%s\n",sql_str);

        sql_operate(sql_str,buf1);
        int lm_sched = atoi(&buf1[0]);

	if(prnt==YES) printf("\n lm_sched=%d",lm_sched);
	///////////////////////////////////////////////////

	//904: all outage lines --> c.f. buf60[] before

	//905: all area modes --> buf500[]

	initString(sql_str,SQL_STRING_LENGTH);
	initString(buf1,BUF1_LEN);

	sprintf(sql_str,"jhjygl@^$$^@select count(FS) from jygl%s..T_BJDZDFH",year);
	sql_operate(sql_str,buf1);
	int mode_num = atoi(&buf1[0]);
	//printf("\n mode_num = %ld",mode_num);

	initString(sql_str,SQL_STRING_LENGTH);
	initString(buf500,BUF500_LEN);

	sprintf(sql_str,"jhjygl@^$$^@select FS,XS,GLYS,XLID,XLZT from jygl%s..T_BJDZDFH",year);
	//printf("\n sql_str = %s",sql_str);
	
	sql_operate(sql_str,buf500);
	//printf("\n buf500[] obtained");
	///////////////////////////////////////////////////////

	//906: get group_id --> buf100[]

	initString(sql_str,SQL_STRING_LENGTH);
        initString(buf100,BUF100_LEN);

        sprintf(sql_str,"xtgl@^$$^@select distinct LLXZID from xtgl..T_AQYSLLXZBM");
        //printf("\nsql_str = %s",sql_str);

	sql_operate(sql_str,buf100);
	//printf("\nbuf100[] obtained");
	////////////////////////////////////////////////////////

	//9.1 creating chain of areaData

	if(prnt==YES) printf("\n Creating areaData...");

	struct areastr *ap;
	areaData=NULL;//init.

	for(i=0;i<group_num;i++)
	{
		strcpy(idbuf,&buf100[i*MAX_COLNUM*(MAX_COLSIZE+1)+0*(MAX_COLSIZE+1)]);
		if(strncmp(idbuf,"dz",2)) continue;//No Change!
		////////////////////////////////////////////////////

		initString(sql_str,SQL_STRING_LENGTH);
                initString(buf20,BUF20_LEN);

                sprintf(sql_str,"xtgl@^$$^@select distinct LLXZMC from xtgl..T_AQYSLLXZBM where LLXZID = '%s'",idbuf);
                sql_operate(sql_str,buf20);
                
		strcpy(buf,&buf20[0]);//group_descr saved into buf[]
             	////////////////////////////////////////////////////////

		ap=(struct areastr *)malloc(sizeof(struct areastr));
		if(ap==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }

		stringEnd(idbuf,' ');
                stringEnd(buf,' ');
	
		strcpy(ap->id,idbuf);
		strcpy(ap->descr,buf);
	
		ap->lineNum=0;//init.
		ap->unitNum=0;
			
		if(!strcmp(ap->id,"dz_tie_0042"))
		{ 
			aux2=(total_sched-(lm_sched-lm_load))/100.0;//p.u.
			ap->wgen=aux2;
		}
		/////////////////////////////////////////////////////

		ap->next=areaData;
		areaData=ap;	
	}//end for
	///////////////////////////////////////////////

	//constructing Liaomeng area
	
	strcpy(idbuf,"dz_tie_0041");//Liaomeng Area
	ap=findData(idbuf,areaData);
	
	if(ap==NULL)//Liaomeng not found
	{
		ap=(struct areastr *)malloc(sizeof(struct areastr));
        	if(ap==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }
	
		strcpy(ap->id,idbuf);
		strcpy(ap->descr,"Chifeng-Yuanbaoshan Area");

		ap->next=areaData;
		areaData=ap;
	}//end if
	///////////////////////////////////////////

	initString(sql_str,SQL_STRING_LENGTH);
        initString(buf1,BUF1_LEN);

        sprintf(sql_str,"jhjygl@^$$^@select YGGL from jygl%s..T_BJJZFDJH where RQ='%s' and SD=%d and DCJZID='%s'",year,rq_str,bid_sd,"yuanbaop");
        //printf("\n sql_str = %s",sql_str);
        sql_operate(sql_str,buf1);
	/////////////////////////////////////////

	ap=findData(idbuf,areaData);
	if(ap==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }

	ap->lineNum=0;
	ap->unitNum=3;//No Change!

	strcpy(ap->unit[1].id,"ybpg1");
	strcpy(ap->unit[2].id,"ybpg2");
	strcpy(ap->unit[3].id,"ybpg3");

	ap->wgen=atoi(&buf1[0])/100.0;//MW --> p.u.

	if(prnt==YES) printf("ok");
	////////////////////////////////////////////////////////

	//9.2 reading lines of an area

	if(prnt==YES) printf("\n Reading lines of area...");
	
	areaNum=chainLen(areaData);//global variable
	ap=areaData;
	
	for(i=0;i<areaNum;i++,ap=ap->next)
	{
		if(ap->unitNum>0) continue;//Liaomeng skipped
		strcpy(idbuf,ap->id);
		////////////////////////////////////////////
	
		initString(sql_str,SQL_STRING_LENGTH);
		initString(buf1,BUF1_LEN);

		sprintf(sql_str,"xtgl@^$$^@select count(SHXLID) from xtgl..T_AQYSLLXZBM where LLXZID='%s'",idbuf);
		//printf("\n sql_str = %s",sql_str);
		sql_operate(sql_str,buf1);
		
		line_num = atoi(&buf1[0]);
		//////////////////////////////////////////////

                initString(sql_str,SQL_STRING_LENGTH);
                initString(buf20,BUF20_LEN);

                sprintf(sql_str,"xtgl@^$$^@select LLXZID,SHXLID from xtgl..T_AQYSLLXZBM where LLXZID = '%s'",idbuf);
                sql_operate(sql_str,buf20);
                //////////////////////////////////////

		ap->lineNum=line_num;
		for(i=0;i<line_num;i++)
		{
			strcpy(idbuf,&buf20[i*MAX_COLNUM*(MAX_COLSIZE+1)+1*(MAX_COLSIZE+1)]);
			stringEnd(idbuf,' ');
			strcpy(ap->line[i+1].id,idbuf);
		}//end for
	}//end for

	if(prnt==YES) printf("ok");
	/////////////////////////////////////////////////////////////

	//9.3 power factors of lines in an area

	if(prnt==YES) printf("\n Reading power factors of area...");

	long imode,mode_size;
	ap=areaData;
	
	for(i=0;i<areaNum;i++,ap=ap->next)
	{
		if(ap->lineNum==0) continue;//Liaomeng skipped
	
		initString(sql_str,SQL_STRING_LENGTH);
                initString(buf1,BUF1_LEN);

 	        sprintf(sql_str,"jhjygl@^$$^@select count(FSID) from jygl%s..T_BJDZDFH where FSID = %d",year,0);
 	      	sql_operate(sql_str,buf1);
		////////////////////////////////////////////////////////////

		mode_size=atoi(&buf1[0]);
		imode=modeID(buf500,buf60,mode_num,outage_num,mode_size);

		if(imode==NO)
		{ 
			printf("\n\nError! --- %ld",__LINE__);
			printf("\n  mode_num=%ld, mode_size=%lf",mode_num,mode_size);

			exit(0);
		}//end if
		/////////////////////////////////////////////

		for(j=1;j<=ap->lineNum;j++)
		{
			strcpy(idbuf,ap->line[j].id);

			ap->line[j].wt=getLineWt(buf500,idbuf,imode,mode_num);
			ap->line[j].pf=getLinePf(buf500,idbuf,imode,mode_num);
		}//end for
	}//end for
		
	if(prnt==YES) printf("ok");
        ////////////////////////////////////////////////////
	
	//10: Reading assoData --- associate

	long bid_plant_num,plant_unit_num;

	initString(sql_str,SQL_STRING_LENGTH);
	initString(buf1,BUF1_LEN);

	sprintf(sql_str,"xtgl@^$$^@select count(distinct SZDCID) from xtgl..T_JZBM where SFJJ=%d",1);
	//printf("\n sql_str = %s",sql_str);
	sql_operate(sql_str,buf1);

	bid_plant_num=atoi(&buf1[0]);	
	//printf("\n bid_plant_num = %d",bid_plant_num);

	initString(sql_str,SQL_STRING_LENGTH);
	initString(buf50,BUF50_LEN);

	sprintf(sql_str,"xtgl@^$$^@select distinct SZDCID from xtgl..T_JZBM where SFJJ=%d",1);
	sql_operate(sql_str,buf50);
	/////////////////////////////////////////////////////////

	if(prnt==YES) printf("\n Reading assoData...");

	struct assostr *asp;
	assoData=NULL;//init.

	for(i=0;i<bid_plant_num;i++)
	{
		strcpy(idbuf,&buf50[i*MAX_COLNUM*(MAX_COLSIZE+1)+0*(MAX_COLSIZE+1)]);
		stringEnd(idbuf,' ');
		//printf("\n idbuf=%s",idbuf);

                initString(sql_str,SQL_STRING_LENGTH);
                initString(buf1,BUF1_LEN);

		sprintf(sql_str,"xtgl@^$$^@select count(JZID) from xtgl..T_JZBM where SFJJ=%d and SZDCID='%s'",1,idbuf);
		sql_operate(sql_str,buf1);
	
		plant_unit_num=atoi(&buf1[0]);
		//printf("\n plant_unit_num = %d",plant_unit_num);

		if(plant_unit_num <= 1) continue;
		//////////////////////////////////////////////////////

		initString(sql_str,SQL_STRING_LENGTH);
		initString(buf10,BUF10_LEN);

		sprintf(sql_str,"xtgl@^$$^@select JZID from xtgl..T_JZBM where SFJJ=%d and SZDCID='%s'",1,idbuf);
		sql_operate(sql_str,buf10);
		////////////////////////////////////////////////////////////////////

		asp=(struct assostr *)malloc(sizeof(struct assostr));
		if(asp==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }

		strcpy(asp->id,idbuf);
		asp->unitNum=plant_unit_num;
		
		for(j=0;j<plant_unit_num;j++)
		{
			strcpy(idbuf2,&buf10[j*MAX_COLNUM*(MAX_COLSIZE+1)+0*(MAX_COLSIZE+1)]);
			stringEnd(idbuf2,' ');

			strcpy(asp->unit[j+1].id,idbuf2);
		}//end for
		//////////////////////////////////////////////

		asp->next=assoData;
		assoData=asp;	
	}//end for
	///////////////////////////////////////////

	//appending tielingp_g3/g4

	asp=(struct assostr *)malloc(sizeof(struct assostr));
	if(asp==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }

	strcpy(asp->id,"tielingp");
	asp->unitNum=2;

	strcpy(asp->unit[1].id,"tipg3");
	strcpy(asp->unit[2].id,"tipg4");

	asp->next=assoData;
	assoData=asp;
	///////////////////////////////
	
	if(prnt==YES) printf("ok");
	//////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////

	turnOver(areaData);
	turnOver(assoData);
        turnOver(genrData);
	turnOver(genrData2);
	turnOver(groupData);
	turnOver(loadData);
	turnOver(plantData);
        turnOver(priceData);

        delete[] seq_price;
	delete[] buf1;
	delete[] buf10;
	delete[] buf20;
	delete[] buf50;
	delete[] buf60;
	delete[] buf100;
	delete[] buf500;
	/////////////////////////////////////////
	
	CS_RETCODE       rcode=0;
        rcode = close_connect_with_server();
        if(rcode!=CS_SUCCEED){ printf("database close failed!!\n"); exit(-1); }

        return;
}

int modeID(char *mode_buf,char *outage_buf,long mode_num,long outage_num,long mode_size)
{
	if(mode_num==0 || mode_size==0) return NO;
        if(mode_num%mode_size!=0) return NO;
	/////////////////////////////////////////////

	register long i,j;
	long ret,ret2,flg,k;
	
	char line_id[40];
	long line_status;
	/////////////////////////////////////////////

	ret=NO;//init.
	for(i=0;i<mode_num;i+= mode_size)
	{
		flg=YES;//init.
		for(j=0;j<mode_size;j++)
		{
			k=i+j;
			strcpy(line_id,&mode_buf[k*MAX_COLNUM*(MAX_COLSIZE+1)+3*(MAX_COLSIZE+1)]);
			stringEnd(line_id,' ');

			line_status=atoi(&mode_buf[k*MAX_COLNUM*(MAX_COLSIZE+1)+4*(MAX_COLSIZE+1)]);	
			ret2=lookup(outage_buf,line_id,outage_num,(long)0);
	
			if(line_status==ON_LINE && ret2==YES){ flg=NO; break; }
			if(line_status==OFFLINE && ret2==NO ){ flg=NO; break; }
		}//end for

		if(flg==YES){ ret=i; break; }
	}//end for
	//////////////////////////////////////////

	return ret;
}

long lookup(char *sybbuf,char *id,long m,long nfield)
{
	register long i;
	long ret=NO;//init.
	
	stringEnd(id,' ');
	////////////////////////////////

	for(i=0;i<m;i++)
	{
		strcpy(idbuf,&sybbuf[i*MAX_COLNUM*(MAX_COLSIZE+1)+nfield*(MAX_COLSIZE+1)]);
		stringEnd(idbuf,' ');
	
		if(!strcmp(idbuf,id))
		{
			ret=YES;
			break;
		}
	}//end for
	////////////////////////////

	return ret;
}

double getLineWt(char *mode_buf,char *ln_id,long imode,long mode_size)
{
	register long i,k;
	double ret=0.0;//init.
	////////////////////////

	for(i=0;i<mode_size;i++)
	{
		k=i+imode;
		if(!strcmp(ln_id,&mode_buf[k*MAX_COLNUM*(MAX_COLSIZE+1)+3*(MAX_COLSIZE+1)]))	
		{
			ret=atof(&mode_buf[k*MAX_COLNUM*(MAX_COLSIZE+1)+1*(MAX_COLSIZE+1)]);
			ret=-ret/100.0;
			break;
		}
	}//end for	
	///////////////////////////////
	
	return ret;
}

double getLinePf(char *mode_buf,char *ln_id,long imode,long mode_size)
{
        register long i,k;
        double ret=0.0;
        ///////////////////////////////

        for(i=0;i<mode_size;i++)
        {
                k=i+imode;
                if(!strcmp(ln_id,&mode_buf[k*MAX_COLNUM*(MAX_COLSIZE+1)+3*(MAX_COLSIZE+1)]))    
       		{
         		ret=atof(&mode_buf[k*MAX_COLNUM*(MAX_COLSIZE+1)+2*(MAX_COLSIZE+1)]);
                	break;
		}
        }//end for
	////////////////////////////////////

	return ret;

}

double getRamp(char *gen_id,char *RLSB_tb,long un_num)
{
	register long i;
	double ret=0.0;//init.

	for(i=0;i<un_num;i++)
	{
		strcpy(idbuf,&RLSB_tb[i*MAX_COLNUM*(MAX_COLSIZE+1)]);
		stringEnd(idbuf,' ');

		if(!strcmp(gen_id,idbuf))
		{
			ret=atoi(&RLSB_tb[i*MAX_COLNUM*(MAX_COLSIZE+1)+1*(MAX_COLSIZE+1)]);	
			break;
		}
	}//end for
	/////////////////////////////

	return ret;
}

double getFall(char *gen_id,char *RLSB_tb,long un_num)
{
        register long i;
        double ret=0.0;//init.

        for(i=0;i<un_num;i++)
        {
		strcpy(idbuf,&RLSB_tb[i*MAX_COLNUM*(MAX_COLSIZE+1)]);
		stringEnd(idbuf,' ');

                if(!strcmp(gen_id,idbuf))
                {
                        ret=atoi(&RLSB_tb[i*MAX_COLNUM*(MAX_COLSIZE+1)+2*(MAX_COLSIZE+1)]);
                        break;
                }
        }//end for
	///////////////////////////////

        return ret;
}

//end of file
