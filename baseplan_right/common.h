#ifndef COMMON_H
#define COMMON_H
#include<time.h>
#include<stdio.h>
#include<unistd.h>


int time2sd(const char *tmp_time, int total_sd);

void print_curtline(const char *str);

int get_date_and_time(char*in_date,char*in_time);

#endif

