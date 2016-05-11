/*--------------------------------------------------------------
**   gen_combination_function.h
**--------------------------------------------------------------
**   PURPOSE:
**      This  head file include global values and functions
**--------------------------------------------------------------
**   Author:
**      Zhang Yang     9/11/99
**
**--------------------------------------------------------------*/

#ifndef APPLICATINSERVER
#define APPLICATINSERVER

#include <ctpublic.h>

int check_down_spinning_reserve_3();
int check_up_spinning_reserve_2();
void decrease_later_on_generator(int);
void decrease_later_on_unit();
void sql_operate(char *sql_str,char *buf_str);
void set_opp_rela();
CS_RETCODE close_connect_with_server();
CS_RETCODE inside_query(char *sq_str,char *buf_str);
CS_RETCODE inside_send_sql(CS_COMMAND *cmd_ptr,CS_CHAR *query_str);
CS_RETCODE inside_handle_returns(CS_COMMAND *cmd_ptr,char *buf_str);
int ret_pos(char *sysid);
void get_gen_mw_scape(char * year_str,char * mon_str,char * day_str,int sd,int un_ord );
void get_price_struct(char *,char *,char *,int);
void get_appointed_peak_load_forecasting(char *,char * ,char *);
void get_appointed_lowest_load_forecasting(char *,char * ,char *);
void get_peak_load(char *year);
void get_lowest_load(char *year);
void get_gen_state(char *,char *,char *,int);
void get_weight();
void get_date_and_time();
void get_gen_tech_data(char * );  
void get_gen_real_time_state(char *,char * ,char *);
void get_mannul_state(char * , char * , char * ,int);
int get_time_num(char *);
int get_must_on_time();
int get_must_off_time();
int get_next_lowest_load_sd(char *,char * ,char *);
int get_appointed_interconnect_line_load(char *,char *,char *,int);
void get_fixed_energy(char *,char *,char *,int);
void get_ZKTRL(char *,char *,char *,int);
float get_pjcyl();
float get_wys_down_rotary_rate(char *);
float get_wys_up_rotary_rate(char *);     
float get_down_rotary_rate(char *);
float get_up_rotary_rate(char *);
float get_lowest_load_rotary_rate(char *);
float get_peak_load_rotary_rate(char *);
float get_hy_down_rotary_rate(char *);
int get_mini_mode_fac_num(char * year);
void get_mini_mode_data(char * year_str);
void mini_mode_save_values(int un_ord,char *row_str);
void free_fac_mini_mode_ptr();
int get_bid_price_num(char *,char *,char *,int);
int get_wksd(char *);
int get_wtsd(char *);
int get_threshold_on_gen(char *);
int get_load(char *,char *,char *, int);
int get_wys_load(char *,char *,char *, int);
int get_gen_num(char *,char *,char *,int);
int get_qt_gen_num(char *,char *,char *,int);
int get_gen_safe_index_num(char *,char *,char *);
int get_gen_tech_num(char *);
int get_real_time_state_num(char *,char *,char *);
float get_net_loss_factor();
void get_gen_safe_index(char *,char *,char *);
void get_gen_num();
void get_gen_id();
void gen_tech_save_values(int , char *);
void qt_save_values(int un_ord,char *row_str);
void weight_save_values(int,char *);
void gen_un_save_values(int un_ord,char *row_str);
void clear_execute_info();
void gen_safe_index_save_values(int,char *);
void gen_id_save_values(int,char *);
void write_execute_info();
void mannul_save_values(int,char *);
void real_time_state_save_values(int,char *);
void set_gen_state(char *,char *,char *);
void set_gen_weight();
void set_aver_price(char *);
void free_price_struct();
void free_gen_state_struct();
void free_gen_safe_index_ptr();
void free_gen_un_struct();
void free_gen_tech_ptr();
void free_gen_real_time_state_ptr();
void free_mannul_state_ptr();
void get_gen_un_struct();
int  get_gen_un_num();
CS_RETCODE inside_query_for_price(char *sq_str);
CS_RETCODE inside_send_sql_for_price(CS_COMMAND *cmd_ptr,CS_CHAR *query_str);
CS_RETCODE inside_handle_returns_for_price(CS_COMMAND *cmd_ptr);
void sql_operate_for_price(char *sql_str);

//*******************this is for get the loss factor***********************
void get_loss_factor(int, char *);
//*************************************************************************

int get_basic_load(char *,char *,char *,int);
int get_interconnect_line_load(char *,char *,char *,int);
#endif

