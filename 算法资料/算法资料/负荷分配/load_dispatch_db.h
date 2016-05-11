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

void sql_operate(char *sql_str,char *buf_str);
void set_opp_rela();
CS_RETCODE close_connect_with_server();
CS_RETCODE inside_query(char *sq_str,char *buf_str);
CS_RETCODE inside_send_sql(CS_COMMAND *cmd_ptr,CS_CHAR *query_str);
CS_RETCODE inside_handle_returns(CS_COMMAND *cmd_ptr,char *buf_str);
int ret_pos(char *sysid);
void get_price_struct(char *,char *,char *,int);
void get_gen_state(char *,char *,char *,int);
void set_gen_load(char *,char *,char *,int);
void set_unit_load(char *,char *,char *,int);
void set_edge_price(char *,char *,char *,int);
int get_time_num(char *);
int get_bid_price_num(char *,char *,char *,int);
int get_load(char *,char *,char *, int);
int get_gen_num(char *,char *,char *,int);
int get_qt_gen_num(char *,char *,char *,int);
float get_net_loss_factor();
float get_pjcyl();
void free_price_struct();
void free_gen_state_struct();
void free_gen_un_struct();
void get_gen_un_struct();
int  get_gen_un_num();
CS_RETCODE inside_query_for_price(char *sq_str);
CS_RETCODE inside_send_sql_for_price(CS_COMMAND *cmd_ptr,CS_CHAR *query_str);
CS_RETCODE inside_handle_returns_for_price(CS_COMMAND *cmd_ptr);
void sql_operate_for_price(char *sql_str);

CS_RETCODE inside_query_for_run_state(char *sq_str,char *buf_str);
CS_RETCODE inside_send_sql_for_run_state(CS_COMMAND *cmd_ptr,CS_CHAR *query_str);
CS_RETCODE inside_handle_returns_for_run_state(CS_COMMAND *cmd_ptr,char *buf_str);
void sql_operate_for_run_state(char *sql_str,char *buf_str);
//*******************this is for get the loss factor***********************
void get_loss_factor(int, char *);
//*************************************************************************

int get_basic_load(char *,char *,char *,int);
int get_interconnect_line_load(char *,char *,char *,int);

void get_date_and_time();
void write_execute_info();
void clear_execute_info();

//void get_qt_curve_load(int);

void get_start_shut_curve_load();
void get_unit_run_state();
void get_unit_run_state_change_info();

float get_max_price();

void set_gen_gate_power();
void set_unit_gate_power();
int get_num_of_start_curve_type();

//**************************this code is for form the up_down_limit table for the use of ssdd****************
//void set_up_down_limit();
void get_up_down_limit();
int get_num_of_up_down_limit_array(char *,char *,char *,int);
//********************************the end********************************************************************

float powertoratio(price_struct* pointer1,price_struct* pointer2,float peach);
float powertoprice(price_struct* pointer1,price_struct* pointer2,float peach);

float get_former_price();
float get_price_up_limit();
float get_price_down_limit();

void set_edge_unit();
//********add by dingpengdir on May 17,2001******************************
void set_bid_price(char *,char *,char *,int);
//****************************** end ************************************
int get_num_of_defined_fre_adjust_array();
int get_num_of_defined_man_adjust_array();
void get_defined_fre_adjust_array();
void get_defined_man_adjust_array();

int get_num_of_defined_amount_gen();
void get_defined_amount_gen_array();

#endif

