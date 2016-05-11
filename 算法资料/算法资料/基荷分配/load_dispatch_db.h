/*--------------------------------------------------------------
**   load_dispatch_db.h
**--------------------------------------------------------------
**   PURPOSE:
**      This  head file include global values and functions
**--------------------------------------------------------------
**   Author:
**      Shao Bin     9/11/99
**
**--------------------------------------------------------------*/

#ifndef APPLICATINSERVER
#define APPLICATINSERVER

#include <ctpublic.h>

void gen_fixed_amount(int NumOfHour,int NumOfUnit,float Load[],float TAmount[],int num_of_block[],float UpLimit[][10],float DownLimit[][10],int StartTime[][10],int EndTime[][10],char Type[],char Name[][22],float MW[][100]);
void get_hydron_plant();

void sql_operate(char *sql_str,char* buf_str);
void set_opp_rela();
CS_RETCODE close_connect_with_server();
CS_RETCODE inside_query(char *sq_str,char* buf_str);
CS_RETCODE inside_send_sql(CS_COMMAND *cmd_ptr,CS_CHAR *query_str);
CS_RETCODE inside_handle_returns(CS_COMMAND *cmd_ptr,char* buf_str,char *sq_str);
int ret_pos(char *sysid);
int get_time_num(char *);
int get_num_of_prior_hydron_plant();
void get_prior_hydron_plant();
void free_prior_hydron_plant();
int get_num_of_lower_hydron_plant(char *);
void get_lower_hydron_plant(char *);
int get_pcount_of_prior_hydron_plant(char *);
int get_num_of_thermal_unit(char *);
void get_gen_fix_thermal_amount(char *);
int get_num_of_generator(char *);
void get_gen_fix_generator_amount(char *); 
int get_num_of_hydron_unit(char *);
void get_gen_fix_hydron_amount(char *);
void get_load_forecast();
void get_interconnect_line_load();
float get_net_loss_factor();

void send_lower_hydron_plant_pcount(char *,int);
void set_prior_hydron_plant_flag();
void set_all_plant_fixed_amount();

void get_date_and_time();
void write_execute_info();
void clear_execute_info();
#endif

