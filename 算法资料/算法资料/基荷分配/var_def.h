//********this struct is defined for the data acquisition of readin_data subroutine************
#ifndef VAR_DEF_H
#define VAR_DEF_H
#define UN_ID_LEN 22
#define UN_NAME_LEN 42
#define FAC_ID_LEN 22
#define NUM_OF_TOTAL_UNIT 100

int total_sd;
char month[3],day[3],year[5];
char present_date[12];

typedef struct{
char un_id[FAC_ID_LEN];
char un_name[UN_NAME_LEN];
int  chain_position; 
float consume_water;
int  pcount;
}hydron_para;
hydron_para *prior_hydron_plant;
hydron_para *lower_hydron_plant;
int num_of_prior_hydron_plant;
int num_of_lower_hydron_plant;

int num_of_hydron_unit;//must think about the attribute of the pcount;
int num_of_thermal_unit;//must think about the attribute of the pcount;
int num_of_generator_unit;

typedef struct{
char  un_id[FAC_ID_LEN];
char  un_name[UN_NAME_LEN];
char  pcount_attr_id[FAC_ID_LEN];
char  pcount_attr[UN_NAME_LEN];
int   pcount;
int   num_of_sd;
int   uplimit[10];
int   downlimit[10];
int   starttime[10];
int   endtime[10];
}fix_amount_struct;
fix_amount_struct *gen_fix_hydron_amount;
fix_amount_struct *gen_fix_thermal_amount;
fix_amount_struct *gen_fix_generator_amount;

int   num_of_block[NUM_OF_TOTAL_UNIT];

float tamount_hydron[NUM_OF_TOTAL_UNIT];
float uplimit_hydron[NUM_OF_TOTAL_UNIT][10];//10 means the num of sd field of each unit;
float downlimit_hydron[NUM_OF_TOTAL_UNIT][10];
int   starttime_hydron[NUM_OF_TOTAL_UNIT][10];
int   endtime_hydron[NUM_OF_TOTAL_UNIT][10];

float tamount_thermal[NUM_OF_TOTAL_UNIT];//10 means the num of sd field of each unit;
float uplimit_thermal[NUM_OF_TOTAL_UNIT][10];
float downlimit_thermal[NUM_OF_TOTAL_UNIT][10];
int   starttime_thermal[NUM_OF_TOTAL_UNIT][10];
int   endtime_thermal[NUM_OF_TOTAL_UNIT][10];


float tamount_generator[NUM_OF_TOTAL_UNIT];
float uplimit_generator[NUM_OF_TOTAL_UNIT][10];//10 means the num of sd field of each unit;
float downlimit_generator[NUM_OF_TOTAL_UNIT][10];
int   starttime_generator[NUM_OF_TOTAL_UNIT][10];
int   endtime_generator[NUM_OF_TOTAL_UNIT][10]; 


float *load_forecast;
float *interconnect_line_load;

char un_id_str_thermal[NUM_OF_TOTAL_UNIT][22];
char un_id_str_hydron[NUM_OF_TOTAL_UNIT][22]; 
char un_id_str_generator[NUM_OF_TOTAL_UNIT][22];
float mw_un_hydron[NUM_OF_TOTAL_UNIT][100];
float mw_un_thermal[NUM_OF_TOTAL_UNIT][100];
float mw_un_generator[NUM_OF_TOTAL_UNIT][100];

char exe_date[12];
char exe_time[10];
char program_id[22];
char program_name[42];
char result_info[10];
char result_info_explain[252];

int correct_result;

char user_id[12];
char user_name[12];
int write_database_flag;

#endif
