//********this struct is defined for the data acquisition of readin_data subroutine************
#ifndef VAR_DEF_H
#define VAR_DEF_H
#define NUM_FLAT 100
#define UN_ID_LEN 22
#define UN_NAME_LEN 42
#define FAC_ID_LEN 22

int un_num;
int basic_load;
int interconnect_line_load;
int *point_num;
float pload;
float edge_price;
int flat_handle_flag;
int price_ed_flag;
int  sd_id;
FILE *fp;
int total_sd;
char month[3],day[3],year[5];
typedef struct generator_struct
{
	char rq_date[12];
	int  time_block_id;
	char un_id[UN_ID_LEN];
	char un_name[UN_NAME_LEN];
	int  real_power;
	int  plus_adjust_power;
	int  minus_adjust_power;
	float gen_power;
	float price_for_gen;
}bid_gen_struct;
bid_gen_struct *bid_gen;
int total_bid_un_num;//the gen_num in the table of jzbj.
char present_date[12];

typedef struct unit_struct
{
	char rq_date[12];
	int  time_block_id;
	char un_id[UN_ID_LEN];
	char un_name[UN_NAME_LEN];
	int  real_power;
	int  plus_adjust_power;
	int  minus_adjust_power;
	float gen_power;
	float price_for_gen;
}bid_un_struct;
bid_un_struct *basic_un;
int basic_un_num;

typedef struct gen_to_un_struct
{
	char gen_id[UN_ID_LEN];
	char fac_id[FAC_ID_LEN];
	char unit_id[UN_ID_LEN];
	char unit_name[UN_NAME_LEN];
}gen_to_unit_struct;
gen_to_unit_struct *gen_to_unit;
int total_gen_unit_num;
char constrain_flag[4];

//********************this struct is for the loss factor struct *******************
typedef struct factor_struct
{
	char gen_id[UN_ID_LEN];
	float region_loss_factor;
	float within_region_loss_factor;
	float distribution_loss_factor;
}loss_factor_struct;
loss_factor_struct *gen_loss_factor;


/******************** this struct is for the generators'technical data ************/

typedef struct gen_tech_struct
{
	char gen_id[UN_ID_LEN];
	char date_for_biding[12];
	int  min_on_time;
	int  min_off_time;
	//int nomal_ascend_speed;
	//int max_ascend_speed;
	//int nomal_descend_speed;
	//int max_descend_speed;
}GEN_TECH_STRUCT;
GEN_TECH_STRUCT * gen_tech_ptr; 
int gen_tech_num;//the generator number of T_XHSCJSSJSB;

/** this struct is used for recording real_time_state of the generators **/

typedef struct real_time_state
{
	char gen_id[UN_ID_LEN];
	int  present_state;
	char state_occuring_time[31];
	int sfjj;
	int sfky;
}REAL_TIME_STATE;
REAL_TIME_STATE * real_time_state_ptr; 
int gen_real_time_state_num;

/** this struct stores generators'state which were modified by dispatcher **/

typedef struct mannul_state
{
	char gen_id[UN_ID_LEN];
	char date[12];
	int  state;
}MANNUL_STATE;
MANNUL_STATE * mannul_state_ptr;

/** these variable are used for the information  of the program'execution **/

char exe_date[12];
char exe_time[10];
char program_id[22];
char program_name[42];
char result_info[10];
char result_info_explain[252];

int result_correct;

#endif
