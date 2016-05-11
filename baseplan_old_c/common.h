#ifndef _COMMON_H
#define _COMMON_H
#include<time.h>
#include<stdio.h>
#include<unistd.h>
#include<assert.h>


/**
 * @brief print_curtline 生成漂亮的分隔符
 * @param str 分隔符中间的说明文字
 */
int time2bucket(const char *tmp_time, int total_sd);

/**
 * @brief print_curtline 生成漂亮的分隔符
 * @param str 分隔符中间的说明文字
 */
void print_curtline(const char *str);

/**
 * @brief get_date_and_time 获取此时的系统时间，所有参数自己在函数外面申请
 * @param in_date 取得的日期值
 * @param in_time 取得的时间值
 * @return 0执行正确，-1调用时间函数出现错误
 */
int get_date_and_time(char*in_date,char*in_time);

/**
 * @brief init_float_arr 初始化数组为指定值。注意：确保arr不为空,len>0,否则程序会退出，打印出错消息
 * @param arr 需要初始化的数组
 * @param len 数组长度
 * @param value 初始化的值
 * @return 成功返回0
 */
int init_float_arr(float *arr, int len, float value);

/**
 * @brief init_int_arr 初始化数组为指定值。注意：确保arr不为空,len>0,否则程序会退出，打印出错消息
 * @param arr 需要初始化的数组
 * @param len 数组长度
 * @param value 初始化的值
 * @return 成功返回0
 */
int init_int_arr(int *arr, int len, int value);

/**
 * @brief bucket_to_time 由时段向时间转换
 * @param bucket 时段序号
 * @param time_bucket 一天当中的时段数
 * @param time 转换后的时间值（传出参数）
 * @param len time字符数组的长度
 * @return 0转换成功，-1传入的参数有误，转换失败
 */
int bucket_to_time(int bucket, int time_bucket, char *time, int len);

/**
 * @brief bucket2time 由时段向时间转换
 * @param bucket 时段序号
 * @param time_bucket 一天当中的时段数
 * @param time 转换后的时间值（传出参数）
 * @return time转换成功，NULL传入的参数有误，转换失败
 */
char * bucket2time(int bucket, int time_bucket);
#endif

