/*--------------------------------------------------------------
**   FILE NAME: Inside_Query.cxx
**--------------------------------------------------------------
**   PURPOSE:
**      This rountine receive sql string and query data
**	qualifyed by sql string,
**--------------------------------------------------------------
**   PARAMETERS:
**      INPUT
**      OUTPUT
**         no returns
**   Author:
**      Zhang Yang     10/11/99
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
#include "gen_combination_function.h"


CS_RETCODE
inside_query(char *sq_str,char *res_str)
{

//	CS_COMMAND       *cmd_ptr=NULL;          /* handle for this SQL command batch*/
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
        RETURN_IF(rcode,"ct_connect0");

	/* allocate command struct */
	rcode = ct_cmd_alloc(st_tab[pos].conn_ptr,&st_tab[pos].cmd_ptr);
	RETURN_IF(rcode,"ct_cmd_alloc");

	}

        rcode=inside_send_sql(st_tab[pos].cmd_ptr,query_str);
	RETURN_IF(rcode,"send_sql");
	
        rcode=inside_handle_returns(st_tab[pos].cmd_ptr,res_str);
	RETURN_IF(rcode,"handle_returns");

//	rcode=ct_cmd_drop(cmd_ptr);
//	RETURN_IF(rcode,"ct_cmd_drop");
	
/*//the following code is transfered to subroutine close_connect_with_server called by the main programme!
	//close shakehand with database

	rcode = ct_close(st_tab[pos].conn_ptr,CS_UNUSED);
	RETURN_IF(rcode,"ct_close");
	
	exit_type=(rcode !=CS_SUCCEED ? CS_FORCE_EXIT:CS_UNUSED);
	rcode=ct_exit(st_tab[pos].cntx_ptr,exit_type);
	RETURN_IF(rcode,"ct_exit");
	
	rcode=cs_ctx_drop(st_tab[pos].cntx_ptr);
	RETURN_IF(rcode,"ct_ctx_drop");
*/

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
inside_handle_returns(CS_COMMAND *cmd_ptr,char *res_str)
{
	CS_RETCODE       rcode,ret;
	CS_INT   result_type,col_num,row_num,i,j,k,rows_read;
	CS_DATAFMT col_des[MAX_COLUMN_NUM];
	CS_CHAR buf[MAX_COLUMN_NUM][MAX_COLUMN_SIZE+1];
	CS_SMALLINT indicator[MAX_COLUMN_NUM];
	CS_INT tem,datalen[MAX_COLUMN_NUM];
	CS_CHAR *str_pcol_end_str[10];
	
	while((rcode = ct_results(cmd_ptr,&result_type))==CS_SUCCEED)
	{
		switch(result_type)
		{
			case CS_ROW_RESULT:
				ret=ct_res_info(cmd_ptr,CS_NUMDATA,&col_num,CS_UNUSED,NULL);
				RETURN_IF(ret,"ct_res_info");
			//	printf("col_num=%d\n",col_num);
				
				sprintf(res_str,"%d%s%s",(int)CS_SUCCEED,COL_END_FIX,ROW_END_FIX);

				for(i=0;i<col_num;i++)
				{
					ret=ct_describe(cmd_ptr,i+1,&col_des[i]);
			                RETURN_IF(ret,"ct_describe");
			                col_des[i].maxlength = MAX_COLUMN_SIZE;
			                col_des[i].datatype = CS_CHAR_TYPE;
			                col_des[i].format   = CS_FMT_UNUSED;
			                //datalen[i]= MAX_COLUMN_SIZE;
			                ret=ct_bind(cmd_ptr,i+1,&col_des[i],buf[i], &datalen[i],NULL);
			                RETURN_IF(ret,"ct_bind");
				}
			        while (ct_fetch(cmd_ptr,CS_UNUSED, CS_UNUSED, CS_UNUSED,
						&rows_read)==CS_SUCCEED)
				{
						for(j=0;j<col_num;j++)
						{
							buf[j][datalen[j]]='\0';
							strcat(res_str,buf[j]);
							strcat(res_str,COL_END_FIX);
						}
						strcat(res_str,ROW_END_FIX);
				}

				break;

			case CS_CMD_DONE:
				ret=ct_res_info(cmd_ptr,CS_ROW_COUNT,&row_num,CS_UNUSED,NULL);
				RETURN_IF(ret,"ct_res_info");
			//	printf("row_num=%d\n",row_num);
				sprintf(res_str,"%s%s%d\n",res_str,COM_AFFACT_LINE_FIX,row_num);
				ct_cancel(NULL,cmd_ptr,CS_CANCEL_CURRENT);
				/*if(res_str[0]=='\0') 
				{
					sprintf(res_str,"%d",CS_CMD_DONE);
					printf("result is:%s\n",res_str);
				}*/
				
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
	          /*case CS_SUCCEED:
		       sprintf(res_str,"%d%s%s",CS_SUCCEED,COL_END_FIX,ROW_END_FIX);
	               break;*/
	          case CS_END_RESULTS:
		       //sprintf(res_str,"%d%s%s",CS_SUCCEED,COL_END_FIX,ROW_END_FIX);
	               break;
	          case CS_FAIL:
		       //yyif(res_str[0]=='\0') strcat(res_str,"fail");
		       //sprintf(res_str,"%s%d%s%s",res_str,CS_FAIL,COL_END_FIX,ROW_END_FIX);
		       //strcat(res_str," fail");
			ret = -1;
			RETURN_IF(ret,"CS_FAIL");
		//	return CS_FAIL;
	               break;
	          default:
	               printf("use db: ct_results return unexpected result code\n");
	               return rcode;
	}
  
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

CS_RETCODE close_connect_with_server()
{
        CS_RETCODE       rcode=0;
        CS_INT exit_type;

        //close shakehand with database

        for(int pos = 0; pos < SYSTEM_NUMBER;pos++)
        {
        if(st_tab[pos].cntx_ptr == NULL && st_tab[pos].conn_ptr == NULL)
                continue;
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
