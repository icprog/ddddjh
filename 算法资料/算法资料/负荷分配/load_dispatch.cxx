/*--------------------------------------------------------------
**   FILE NAME: qe.cxx
**--------------------------------------------------------------
**   PURPOSE:
**      This rountine is a independant programme that dispatch the
**      load between the units that has already determined its
**      running status. All the information of the units and data 
**      must be obtained from the database and the results must 
**      write to the corresponding database.
**--------------------------------------------------------------
**   Author:
**      Zhang Zhigang     23/11/99
**
**--------------------------------------------------------------*/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <algorithm>
#include <iostream.h>
#include <fstream.h>
#include <math.h>
#include <stdlib.h>
#include "var_def.h"
#include "load_dispatch_db.h"
#include "global_var.h"
//select distinct JBDYID,count(CL) from table where SD=3 group by JBDYID

int ifgenofshutcurve(char *un_id)
{
	for(int num = 0; num < num_of_start_shut_curve_array; num++)
	{
		if(start_shut_curve_array[num].sd_id == sd_id && !strcmp(start_shut_curve_array[num].un_id,un_id))
		return 1;
	}
	return 0;
}

void readin_data( int sd_num)
{
	sd_id = sd_num;

	un_num = 0;

	if(!strcmp(constrain_flag,"yys")) 
		get_start_shut_curve_load();

	printf("########sd=%d    start_shut_curve_load_sum=%d    ",sd_id,start_shut_curve_load_sum);

	basic_load = get_basic_load(year,month,day,sd_num);
	printf("basic_load=%d    ",basic_load);
	if(basic_load == 0)
	{
		sprintf(result_info,"错误");
        sprintf(result_info_explain,"第%d时段基荷负荷为零",sd_id);
        write_execute_info();
		result_correct = 0;
	}

	interconnect_line_load = get_interconnect_line_load(year,month,day,sd_num);
	printf("interconnect_line_load=%d    ",interconnect_line_load);

	float wsxs = get_net_loss_factor();
	printf("wsxs=%f ",wsxs);

	pload = get_load(year,month,day,sd_id);
	printf("pload=%f    ",pload);
	if(pload == 0)
	{
		sprintf(result_info,"错误");
        sprintf(result_info_explain,"第%d时段负荷预测值为零",sd_id);
        write_execute_info();
		result_correct = 0;
	}
	//printf("ori_pload=%f,sd=%d\n",pload,sd_id);
	pjcyl = get_pjcyl();
	printf("pjcyl is %f     ",pjcyl);
	pload = pload/(1 - wsxs / 100) + interconnect_line_load - basic_load*(1-pjcyl) - start_shut_curve_load_sum;
	printf("*pload=%f\n",pload);

	seq_price_num = get_bid_price_num(year,month,day,sd_num);
	//printf("seq_price_num=%d\n",seq_price_num);

	total_bid_un_num = get_gen_num(year,month,day,sd_num);
	if(total_bid_un_num == 0)
	{
		sprintf(result_info,"错误");
        sprintf(result_info_explain,"第%d时段竞价申报数据为空",sd_id);
        write_execute_info();
		result_correct = 0;
	}
	
	total_un_num = get_qt_gen_num(year,month,day,sd_num);
	if(total_un_num == 0)
	{
		sprintf(result_info,"错误");
        sprintf(result_info_explain,"第%d时段机组开停机结果不正确",sd_id);
        write_execute_info();
		result_correct = 0;
	}

	get_gen_state(year,month,day,sd_num);

	get_price_struct(year,month,day,sd_num);
	
	total_gen_unit_num = get_gen_un_num();

	get_gen_un_struct();
	
	int temp_bid_gen_num = 0;
	bid_gen = new bid_gen_struct[total_bid_un_num];
	for(int i = 0; i < seq_price_num; i ++)
	{
		if(i == 0)
		{
			strcpy(bid_gen[temp_bid_gen_num].rq_date,present_date);
			bid_gen[temp_bid_gen_num].time_block_id = sd_id;
			strcpy(bid_gen[temp_bid_gen_num].un_id,seq_price[i].uni_id);
			strcpy(bid_gen[temp_bid_gen_num].un_name,seq_price[i].un_name);
			bid_gen[temp_bid_gen_num].real_power = 0;
			bid_gen[temp_bid_gen_num].plus_adjust_power = 0;
			bid_gen[temp_bid_gen_num].minus_adjust_power = 0;
			bid_gen[temp_bid_gen_num].gen_power = 0.0;
			bid_gen[temp_bid_gen_num].price_for_gen = 0.0;
			bid_gen[temp_bid_gen_num].fac_used_ratio = 0.0;
			temp_bid_gen_num++;
		}
		else if(i != 0 && strcmp(seq_price[i].uni_id, seq_price[i-1].uni_id))
		{
			strcpy(bid_gen[temp_bid_gen_num].rq_date,present_date);
			bid_gen[temp_bid_gen_num].time_block_id = sd_id;
			strcpy(bid_gen[temp_bid_gen_num].un_id,seq_price[i].uni_id);
			strcpy(bid_gen[temp_bid_gen_num].un_name,seq_price[i].un_name);
			bid_gen[temp_bid_gen_num].real_power = 0;
			bid_gen[temp_bid_gen_num].plus_adjust_power = 0;
			bid_gen[temp_bid_gen_num].minus_adjust_power = 0;
			bid_gen[temp_bid_gen_num].gen_power = 0.0;
			bid_gen[temp_bid_gen_num].price_for_gen = 0.0;
			bid_gen[temp_bid_gen_num].fac_used_ratio = 0.0;
			temp_bid_gen_num++;
		}
	}


	for(int i = 0; i < seq_price_num; i ++)
	{
		if(i != 0 && !strcmp(seq_price[i].uni_id, seq_price[i-1].uni_id))
			continue;
		for(int qt_num = 0; qt_num < total_un_num; qt_num++)
		{
			if(!strcmp(seq_price[i].uni_id, qt_gen_struct[qt_num].un_id))
			{
				if((qt_gen_struct[qt_num].run_status == 1 || qt_gen_struct[qt_num].run_status == 2) && !ifgenofshutcurve(qt_gen_struct[qt_num].un_id))
				{
					un_num++;
					break;
				}
				else 
					break;
			}
		}
	}
	
	point_num = new int[un_num];
	fee_ave = new price_struct[un_num * 10];
	fee_ave_bak = new price_struct[un_num * 10];

	int temp_un_num = 0;
	int temp_point_num = 0;
	for(int i = 0; i < seq_price_num; i ++)
	{
		for(int qt_num = 0; qt_num < total_un_num; qt_num++)
		{
			if(!strcmp(seq_price[i].uni_id, qt_gen_struct[qt_num].un_id))
			{
				if((qt_gen_struct[qt_num].run_status == 1 || qt_gen_struct[qt_num].run_status == 2) && !ifgenofshutcurve(qt_gen_struct[qt_num].un_id))
				{
					if(i == 0 || (temp_un_num == 0 && temp_point_num == 0))
					{
						strcpy(fee_ave[temp_un_num*10+temp_point_num].un_id,seq_price[i].uni_id);
						strcpy(fee_ave[temp_un_num*10+temp_point_num].un_name,seq_price[i].un_name);
						fee_ave[temp_un_num*10+temp_point_num].out_power = seq_price[i].out_power;
						fee_ave[temp_un_num*10+temp_point_num].price = seq_price[i].price;
						fee_ave[temp_un_num*10+temp_point_num].fac_used_ratio = seq_price[i].fac_used_ratio;
						temp_point_num++;
					}
					else if(i != 0 && !strcmp(seq_price[i].uni_id, seq_price[i-1].uni_id))
					{
						strcpy(fee_ave[temp_un_num*10+temp_point_num].un_id,seq_price[i].uni_id);
						strcpy(fee_ave[temp_un_num*10+temp_point_num].un_name,seq_price[i].un_name);
						fee_ave[temp_un_num*10+temp_point_num].out_power = seq_price[i].out_power;
						fee_ave[temp_un_num*10+temp_point_num].price = seq_price[i].price;
						fee_ave[temp_un_num*10+temp_point_num].fac_used_ratio = seq_price[i].fac_used_ratio;
						temp_point_num++;
					}
					else if(i != 0 && strcmp(seq_price[i].uni_id, seq_price[i-1].uni_id))
					{
						point_num[temp_un_num] = temp_point_num;
						temp_un_num++;
						temp_point_num = 0;
						strcpy(fee_ave[temp_un_num*10+temp_point_num].un_id,seq_price[i].uni_id);
						strcpy(fee_ave[temp_un_num*10+temp_point_num].un_name,seq_price[i].un_name);
						fee_ave[temp_un_num*10+temp_point_num].out_power = seq_price[i].out_power;
						fee_ave[temp_un_num*10+temp_point_num].price = seq_price[i].price;
						fee_ave[temp_un_num*10+temp_point_num].fac_used_ratio = seq_price[i].fac_used_ratio;
						temp_point_num++;
					}
				}
				break;
			}
		}
	}
	point_num[temp_un_num] = temp_point_num;
	//for(int i = 0; i < un_num; i++)
	//	printf("jzid=%s\n",fee_ave[i*10].un_id);

//**************************the following code is for up_down_limit of load dispatch.****************
	num_of_up_down_limit_array = get_num_of_up_down_limit_array(year,month,day,sd_num);
	up_down_limit_array = new up_down_limit_struct[num_of_up_down_limit_array];
	limit_not_exist_flag = 0;
	get_up_down_limit();

	if(limit_not_exist_flag == 1) 
		delete[] up_down_limit_array;
	else
	{
		for(int i = 0; i < un_num; i++)//this is for form the down_limit;
			for(int j = 0; j < num_of_up_down_limit_array; j++)
				if(!strcmp(fee_ave[i*10].un_id,up_down_limit_array[j].un_id))
				{
					if(up_down_limit_array[j].down_limit == fee_ave[i*10].out_power)
						break;
					if(up_down_limit_array[j].down_limit < fee_ave[i*10].out_power)
					{
						fee_ave[i*10].price = powertoprice(&fee_ave[i*10],&fee_ave[i*10+1],up_down_limit_array[j].down_limit);
						fee_ave[i*10].out_power = up_down_limit_array[j].down_limit;
						break;
					}
					if(up_down_limit_array[j].down_limit < fee_ave[i*10+1].out_power)
					{
						fee_ave[i*10].price = powertoprice(&fee_ave[i*10],&fee_ave[i*10+1],up_down_limit_array[j].down_limit);
						fee_ave[i*10].fac_used_ratio = powertoratio(&fee_ave[i*10],&fee_ave[i*10+1],up_down_limit_array[j].down_limit);
						fee_ave[i*10].out_power = up_down_limit_array[j].down_limit;
						break;
					}
					for(int k = 2; k < point_num[i]; k++)
					{
						if(up_down_limit_array[j].down_limit < fee_ave[i*10+k].out_power)
						{
							fee_ave[i*10].price = powertoprice(&fee_ave[i*10+k-1],&fee_ave[i*10+k],up_down_limit_array[j].down_limit);
							fee_ave[i*10].fac_used_ratio = powertoratio(&fee_ave[i*10+k-1],&fee_ave[i*10+k],up_down_limit_array[j].down_limit);
							fee_ave[i*10].out_power = up_down_limit_array[j].down_limit;
							fee_ave[i*10].out_power = up_down_limit_array[j].down_limit;
							for(int num = k; num < point_num[i]; num++)
							{
								memcpy(&fee_ave[i*10 + num-k+1],&fee_ave[i*10 + num],sizeof(price_struct));
							}
							point_num[i] = point_num[i] - k +1;
							break;
						}
					}
				}

		for(int i = 0; i < un_num; i++)//this is for form the up_limit;
			for(int j = 0; j < num_of_up_down_limit_array; j++)
				if(!strcmp(fee_ave[i*10].un_id,up_down_limit_array[j].un_id))
				{
					if(up_down_limit_array[j].up_limit == fee_ave[i*10+point_num[i]-1].out_power)
						break;
					if(up_down_limit_array[j].up_limit > fee_ave[i*10+point_num[i]-1].out_power)
					{
						fee_ave[i*10+point_num[i]-1].price = powertoprice(&fee_ave[i*10+point_num[i]-2],&fee_ave[i*10+point_num[i]-1],up_down_limit_array[j].up_limit);
						fee_ave[i*10+point_num[i]-1].out_power = up_down_limit_array[j].up_limit;
						fee_ave[i*10+point_num[i]-1].out_power = up_down_limit_array[j].up_limit;
						break;
					}
					if(up_down_limit_array[j].up_limit > fee_ave[i*10+point_num[i]-2].out_power)
					{
						fee_ave[i*10+point_num[i]-1].price = powertoprice(&fee_ave[i*10+point_num[i]-2],&fee_ave[i*10+point_num[i]-1],up_down_limit_array[j].up_limit);
						fee_ave[i*10+point_num[i]-1].fac_used_ratio = powertoratio(&fee_ave[i*10+point_num[i]-2],&fee_ave[i*10+point_num[i]-1],up_down_limit_array[j].up_limit);
						fee_ave[i*10+point_num[i]-1].out_power = up_down_limit_array[j].up_limit;
						fee_ave[i*10+point_num[i]-1].out_power = up_down_limit_array[j].up_limit;
						break;
					}
					for(int k = point_num[i] - 3; k >= 0; k--)
					{
						if(up_down_limit_array[j].up_limit > fee_ave[i*10+k].out_power)
						{
							fee_ave[i*10+k+1].price = powertoprice(&fee_ave[i*10+k],&fee_ave[i*10+k+1],up_down_limit_array[j].up_limit);
							fee_ave[i*10+k+1].fac_used_ratio = powertoratio(&fee_ave[i*10+k],&fee_ave[i*10+k+1],up_down_limit_array[j].up_limit);
							fee_ave[i*10+k+1].out_power = up_down_limit_array[j].up_limit;
							fee_ave[i*10+k+1].out_power = up_down_limit_array[j].up_limit;
							point_num[i] = k + 2;
							break;
						}
					}
				}
		//for(int i = 0; i < un_num; i++)
		//printf("JZID = %s  up_power = %d  up_price = %f  down_power = %d  down_power %f\n",fee_ave[i*10].un_id,fee_ave[i*10+point_num[i]-1].out_power,fee_ave[i*10+point_num[i]-1].price,fee_ave[i*10].out_power,fee_ave[i*10].price);

		delete[] up_down_limit_array;
	}
//******************************the end by zzg on 13,June,2000*****************************************


	for(int i = 0; i < un_num; i++)
		for(int num = 0; num < point_num[i]; num++) 
			fee_ave[i*10 + num].group_id = 0;
	
	for(int i = 0; i < un_num; i++)
		for(int num = 0; num < point_num[i]; num++)
			memcpy(&fee_ave_bak[i*10 + num],&fee_ave[i*10 + num],sizeof(price_struct));

	for(int i = 0; i < un_num; i++)
	{
		max_p += fee_ave[i*10 + point_num[i]-1].out_power;
		min_p += fee_ave[i*10].out_power;
	}

//********************************the end********************************************************************
}

float lmttoload(price_struct* pointer1, price_struct* pointer2, float lmt_value)
{
 	double point1x,point1y,point2x,point2y;
	point1x = (double)pointer1->out_power;
	point1y = (double)pointer1->price;
	point2x = (double)pointer2->out_power;
	point2y = (double)pointer2->price;
	double slope = (double)(point2y - point1y) / (point2x - point1x);
	double consy = (double)(point2x * point1y - point1x * point2y) / (point2x - point1x);
	float load = (double)(lmt_value - consy) / slope;
	return load;
}

void handle_slope()
{
	float slope;
	float min_slope;
	min_slope = 10000;
	for(int i = 0; i < un_num; i++)
		for(int num =0; num < point_num[i] - 1;num++)
		{
			if(fee_ave[i *10 + num + 1].out_power != fee_ave[i * 10 + num].out_power)
			{
				slope = (fee_ave[i*10 + num+1].price - fee_ave[i*10  + num].price) / (fee_ave[i *10 + num + 1].out_power - fee_ave[i * 10 + num].out_power);
				if(slope != 0)
					min_slope = min(slope, min_slope);
			}
		}
	accuracy = 1.0 / min_slope * 0.001;
}

void sort_qe()
{
	//********now the qe has been transfered into the price_struct data struct************
	//*********that is fee_ave[i].qe_lmt*********
	//now want to find the maximum value and minimum value of qe_lmt
	float temp_max;
	temp_max = 0.0;
	float temp_min;
	for(int i = 0; i < un_num; i ++)
	{
		for(int num =1; num < point_num[i] - 1;num++)
		{
			temp_max = max(temp_max, fee_ave[i*10 +num].price);
		}
		temp_max = max(temp_max, fee_ave[i*10 +0].price);
		temp_max = max(temp_max, fee_ave[i*10 +point_num[i] - 1].price);
	}
	max_lmt = temp_max;
	temp_min = temp_max;
	for(int i = 0; i < un_num; i ++)
	{
		for(int num =1; num < point_num[i] -1;num++)
		{
			temp_min = min(temp_min, fee_ave[i*10 +num].price);
		}
		temp_min = min(temp_min, fee_ave[i*10 +0].price);
		temp_min = min(temp_min, fee_ave[i*10 +point_num[i] - 1].price);
	}
	min_lmt = temp_min;
}

//***************this programme is the key of load dispatch**********************
//*********just think about the situation that price curve ascend continually and can not drop its trend as power increase, so the qe curve ascend continually**********************
void lmttopeach(int un_id,float lmt_value)
{
	int num;
	if(lmt_value > fee_ave[un_id*10 +0].price && lmt_value < fee_ave[un_id*10 +point_num[un_id]-1].price)
	{
	for(num = 0; num < point_num[un_id] - 1; num++)
	{
		if(fee_ave[un_id*10 +num].price == fee_ave[un_id*10 +num+1].price && lmt_value == fee_ave[un_id*10 +num].price)
		{
			if(det_p > 0)
				p_each[un_id] = fee_ave[un_id*10 +num].out_power;
			else if(det_p < 0)
				p_each[un_id] = fee_ave[un_id*10 +num+1].out_power;
			break;
		}
		else if(lmt_value <= fee_ave[un_id*10 +num+1].price)
		{
		p_each[un_id] =  lmttoload(&fee_ave[un_id*10 +num],&fee_ave[un_id*10 +num+1],lmt_value);
		break;
		}
	}
	}
	else if(lmt_value <= fee_ave[un_id*10 +0].price)
		p_each[un_id] = fee_ave[un_id*10 +0].out_power;
	else if(lmt_value >= fee_ave[un_id*10 +point_num[un_id]-1].price)
		p_each[un_id] = fee_ave[un_id*10 +point_num[un_id]-1].out_power;
	//printf("JZID = %s   p_each = %f\n",un_id,p_each[un_id]);
}

void lmttopsum()
{
	float p_value;
	p_value = 0;
	vertical_num = 0;
	for(int i = 0; i < un_num; i ++)
	{
		lmttopeach(i,ave_lmt);//p_value is sum of the dispatched power of each unit
		p_value = p_value + p_each[i];
		//printf("*JZID = %d   p_each = %f\n",i,p_each[i]);
	}
	p_sum = p_value;
	//printf("p_sum=%f\n",p_sum);
}

int lmttootherp(flat_struct* flat_record, int flat_num, float lmt_value)
{
	float p_value;
	float *p_limit;
	p_limit = new float[flat_num];
	float p_limit_sum;
	float p_range_min, p_range_max;
	p_range_min = 0;
	p_range_max = 0;
	p_limit_sum = 0;
        p_value = 0;
	int conti_flag;
	for(int num = 0; num < un_num; num++)
	{
		conti_flag = 0;
		for(int i = 0; i < flat_num; i ++)
		{
		if(num == flat_record[i].un_id)
			conti_flag = 1;
		}
		if(conti_flag == 1)
			continue;
		lmttopeach(num,lmt_value);

		//for(int i = 0; i < un_num; i ++)
		//printf("%d ID = %d  JZID = %s p_each = %f\n",num,i,fee_ave[i*10].un_id,p_each[i]);
		//printf("here is lmttopeach of lmttootherp\n");

		p_value = p_value + p_each[num];
	}
	//printf("det_p_1 = %f\n",det_p);
	det_p = pload - p_value; 
	//printf("***pload = %f p_value = %f det_p_2 = %f\n",pload,p_value,det_p);
//**********here is some code just to deal with the same hori_line with more than two points******
	for(int i = 0; i < flat_num; i++)
	{
		flat_record[i].point_id_end = flat_record[i].point_id;
		flat_record[i].point_num = 1;
		if(i != 0)
		if(flat_record[i].un_id == flat_record[i-1].un_id)
		{
			flat_record[i].point_num = flat_record[i-1].point_num +1;
			for(int j = 1; j < flat_record[i].point_num; j++)
			{
			flat_record[i-j].point_id_end = flat_record[i].point_id_end;
			flat_record[i-j].point_num ++;
			flat_record[i].point_id = flat_record[i-j].point_id;
			}
		}
	}
	//for(int i = 0; i < flat_num; i++)
	//printf("flat_record[%d].un_id = %d flat_record[%d].point_id_end = %d flat_record[%d].point_num = %d flat_record[%d].point_id = %d\n",i,flat_record[i].un_id,i,flat_record[i].point_id_end,i,flat_record[i].point_num,i,flat_record[i].point_id);
//*********************zzg on 5.11.2000********************************
	for(int i = 0; i < flat_num; i ++)
	{
		if(i != 0 && flat_record[i].un_id == flat_record[i-1].un_id)
			continue;
		if(flat_record[i].flat_flag == 0)
		{
			p_range_min = p_range_min + fee_ave[flat_record[i].un_id * 10 + flat_record[i].point_id].out_power;
			p_range_max = p_range_max + fee_ave[flat_record[i].un_id * 10 + flat_record[i].point_id_end + 1].out_power;
			p_limit[i] = fee_ave[flat_record[i].un_id * 10 + flat_record[i].point_id_end + 1].out_power - fee_ave[flat_record[i].un_id * 10 + flat_record[i].point_id].out_power;
			p_limit_sum += p_limit[i];
		}
        }
	//printf("det_p = %f p_range_min = %f p_range_max =%f p_limit_sum = %f\n",det_p,p_range_min,p_range_max,p_limit_sum);
	if(det_p <= p_range_max && det_p >= p_range_min)
	{
			for(int i = 0; i < flat_num; i ++)
			{
			if(i != 0 && flat_record[i].un_id == flat_record[i-1].un_id)
				continue;
			if(flat_record[i].flat_flag == 0)
				{					
				p_each[flat_record[i].un_id] = fee_ave[flat_record[i].un_id * 10 + flat_record[i].point_id].out_power + (det_p - p_range_min) * p_limit[i] / p_limit_sum;
				//printf("fee_ave.out_power = %d JZID = %d  p_each = %f\n",fee_ave[flat_record[i].un_id * 10 + flat_record[i].point_id].out_power,flat_record[i].un_id,p_each[flat_record[i].un_id]);
				}
                        } 
			//for(int i = 0; i < flat_num; i ++)
			//printf("JZID = %d  p_each = %d\n",flat_record[i].un_id,p_each[flat_record[i].un_id]);
		delete[] p_limit;
		return 1;
	}
	delete[] p_limit;
        return 0;
} 

int  check_modify_hori_line()
{
	int ss;
	int flat_line_num;
	float lmt_value;
	for(int i = 0; i < un_num; i ++)
		for(int num = 0; num < point_num[i] - 1; num++)
		{
			flat_line_num = 0;
			if(fee_ave[i*10 + num].price == fee_ave[i*10 + num +1].price)//once there is a original hori_line, we find out all the hori_line that lie in the same hori_line,and calulate each power dispatch, if the power dispatch are right,then return, else we find the next hori_line;
			{
				lmt_value = fee_ave[i*10 + num].price;
				ave_lmt = lmt_value;
				for(int m = 0; m < un_num; m++)
					for(int n = 0; n < point_num[m] -1; n++)
					{
						if(fee_ave[m*10 + n].price == fee_ave[m*10 + n +1].price && fee_ave[m*10 + n].price == lmt_value)
						{
							flat_para[flat_line_num].un_id = m;
							flat_para[flat_line_num].point_id = n;	
							flat_para[flat_line_num].flat_flag = 0;
							flat_line_num ++;
						}
					}
				//printf("flat_line_num = %d  un_num = %d   point_num = %d\n",flat_line_num,i,num);
				//for(int i = 0; i < flat_line_num; i++)
				//printf("flat_para[%d].un_id = %d flat_para[%d].point_id = %d flat_para[%d].flat_flag = %d\n",i,flat_para[i].un_id,i,flat_para[i].point_id,i,flat_para[i].flat_flag);
				ss = lmttootherp(flat_para,flat_line_num,lmt_value);
				if(ss == 1)
					return 1;
			}
	     }
     return 0;  
}

void handle_all_vertical()
{
	//here will be the code that deal with the situation that all the power all in the vertical line,and find out the minimum value of lmt according to the rule.
}

float powertoprice(price_struct* pointer1,price_struct* pointer2,float peach)
{
	double point1x,point1y,point2x,point2y;
	point1x = (double)pointer1->out_power;
	point1y = (double)pointer1->price;
	point2x = (double)pointer2->out_power;
	point2y = (double)pointer2->price;
	double slope = (point2y - point1y) / (point2x - point1x);
	double consy = (point2x * point1y - point1x * point2y) / (point2x - point1x);
	float price = slope * peach + consy;
	return price;
	
}

void getedgeprice()
{
	float max_price;
	max_price = 0;
	//float *price_each;
	price_each = new float[un_num];
	for(int i = 0; i < un_num; i++)
	{
		for(int num = 0; num < point_num[i] - 1; num++)
		{
			if(p_each[i] >= fee_ave_bak[i*10+num].out_power && p_each[i] <= fee_ave_bak[i*10+num+1].out_power)
			{
				price_each[i] = powertoprice(&fee_ave_bak[i*10+num],&fee_ave_bak[i*10+num+1],p_each[i]);
				break;
			}
		}
		max_price = max(max_price, price_each[i]);
		printf("jzid=%s price=%10f power=%10f max_price=%10f\n",fee_ave_bak[i*10].un_id,price_each[i],p_each[i],max_price);

	}
	edge_price = max_price;
//*********************deal with edge_unit****************************
	total_edge_unit_num = 0;
	for(int i = 0; i < un_num; i++)
	{
		if(price_each[i] == edge_price)
		{
			edge_unit_array[total_edge_unit_num] = i;
			total_edge_unit_num++;	
		}
	}
	if(write_database_flag !=0)
		set_edge_unit();
//************************end*****************************************	

	max_edge_price = get_max_price();
	if(max_edge_price < edge_price)
		edge_price = max_edge_price;
	if(!strcmp(constrain_flag,"wys"))
    {
    	float former_price;
        former_price = get_former_price();
        float price_up_limit,price_down_limit;
        price_up_limit = get_price_up_limit();
        price_down_limit = get_price_down_limit();
        if(edge_price > former_price * ( 1 + price_up_limit))
        	edge_price = former_price * ( 1 + price_up_limit);
        if(edge_price < former_price * (1 - price_down_limit))
            edge_price = former_price * (1 - price_down_limit);
    }
	else
    {
    	float former_price;
        former_price = get_former_price();
        float price_up_limit,price_down_limit;
        price_up_limit = get_price_up_limit();
        price_down_limit = get_price_down_limit();
        if(edge_price > former_price * ( 1 + price_up_limit))
        	edge_price = former_price * ( 1 + price_up_limit);
        if(edge_price < former_price * (1 - price_down_limit))
            edge_price = former_price * (1 - price_down_limit);
    }
}

void merge_gen_to_unit()
{
	int temp_num = 0;
	char temp_un_id[UN_ID_LEN];
	strcpy(temp_un_id,"");
	for(int i = 0; i < total_gen_unit_num; i++)
	{
			if(temp_num == 0)
			{
				strcpy(temp_un_id,gen_to_unit[i].unit_id);
				temp_num++;
			}
			else if(temp_num != 0 && strcmp(temp_un_id,gen_to_unit[i].unit_id))
			{
				strcpy(temp_un_id,gen_to_unit[i].unit_id);
				temp_num++;
			}
	}
	basic_un_num = temp_num;
	basic_un = new bid_un_struct[basic_un_num];
//***************************the folowing just do the initiation**************
	temp_num = 0;
	for(int i = 0; i < total_gen_unit_num; i ++)
	{
		if(temp_num == 0)
		{
			strcpy(basic_un[temp_num].rq_date,present_date);
			basic_un[temp_num].time_block_id = sd_id;
			strcpy(basic_un[temp_num].un_id,gen_to_unit[i].unit_id);
			strcpy(basic_un[temp_num].un_name,gen_to_unit[i].unit_name);
			basic_un[temp_num].real_power = 0;
			basic_un[temp_num].plus_adjust_power = 0;
			basic_un[temp_num].minus_adjust_power = 0;
			basic_un[temp_num].gen_power = 0.0;
			basic_un[temp_num].price_for_gen = 0.0;
			strcpy(temp_un_id,gen_to_unit[i].unit_id);
			temp_num++;
		}
		else if(temp_num != 0 && strcmp(temp_un_id,gen_to_unit[i].unit_id))
		{
			strcpy(basic_un[temp_num].rq_date,present_date);
			basic_un[temp_num].time_block_id = sd_id;
			strcpy(basic_un[temp_num].un_id,gen_to_unit[i].unit_id);
			strcpy(basic_un[temp_num].un_name,gen_to_unit[i].unit_name);
			basic_un[temp_num].real_power = 0;
			basic_un[temp_num].plus_adjust_power = 0;
			basic_un[temp_num].minus_adjust_power = 0;
			basic_un[temp_num].gen_power = 0.0;
			basic_un[temp_num].price_for_gen = 0.0;
			strcpy(temp_un_id,gen_to_unit[i].unit_id);
			temp_num++;
		}
	}
//*************************************************************************	
	for(int i = 0; i < total_bid_un_num; i++)
	for(int num = 0; num < total_gen_unit_num; num++)
	{
		if(!strcmp(bid_gen[i].un_id,gen_to_unit[num].gen_id))
		{
			for(int j = 0; j < basic_un_num; j++)
			{
				if(!strcmp(gen_to_unit[num].unit_id,basic_un[j].un_id))
				{
				basic_un[j].real_power += bid_gen[i].real_power;
				basic_un[j].plus_adjust_power += bid_gen[i].plus_adjust_power;
				basic_un[j].minus_adjust_power += bid_gen[i].minus_adjust_power;
				basic_un[j].gen_power += bid_gen[i].gen_power;
				if(bid_gen[i].price_for_gen != 0)
					basic_un[j].price_for_gen = bid_gen[i].price_for_gen;
				break;
				}
			}
			break;
		}
	}
	num_of_defined_amount_gen = get_num_of_defined_amount_gen();
	defined_amount_gen_array = new defined_amount_gen_struct[num_of_defined_amount_gen];
	get_defined_amount_gen_array();
	for(int i = 0; i < num_of_defined_amount_gen; i++)
	for(int num = 0; num < total_gen_unit_num; num++)
	{
		if(!strcmp(defined_amount_gen_array[i].un_id,gen_to_unit[num].gen_id))
		{
			for(int j = 0; j < basic_un_num; j++)
			{
				if(!strcmp(gen_to_unit[num].unit_id,basic_un[j].un_id))
				{
				basic_un[j].real_power += defined_amount_gen_array[i].real_power;
				basic_un[j].plus_adjust_power += defined_amount_gen_array[i].plus_adjust_power;
				basic_un[j].minus_adjust_power += defined_amount_gen_array[i].minus_adjust_power;
				basic_un[j].gen_power += defined_amount_gen_array[i].gen_power;
				break;
				}
			}
			break;
		}
	}
	delete[] defined_amount_gen_array;
}

void calcu_power_of_vibration()
{
	float *p_each_down;
	float *p_each_up;
	p_each_down = new float[un_num];
	p_each_up = new float[un_num];
	float det_p_up;
	float det_p_down;
	
	ave_lmt = lmt_up;
	lmttopsum();
	det_p_up = fabs(p_sum - pload);

	ave_lmt = lmt_down;
	lmttopsum();
	det_p_down = fabs(p_sum - pload);

	ave_lmt = lmt_up;
	for(int i = 0; i < un_num; i ++)
	{
		lmttopeach(i,ave_lmt);
		p_each_up[i] = p_each[i];
	}
				
	ave_lmt = lmt_down;
	for(int i = 0; i < un_num; i ++)
	{
		lmttopeach(i,ave_lmt);
		p_each_down[i] = p_each[i];
	}
	
	for(int i = 0; i < un_num; i ++)
		p_each[i] = p_each_down[i] + (p_each_up[i] - p_each_down[i]) * det_p_down / (det_p_up + det_p_down);
}

float powertoratio(price_struct* pointer1,price_struct* pointer2,float peach)
{
	double point1x,point1y,point2x,point2y;
	point1x = pointer1->out_power;
	point1y = pointer1->fac_used_ratio;
	point2x = pointer2->out_power;
	point2y = pointer2->fac_used_ratio;
	double slope = (point2y - point1y) / (point2x - point1x);
	double consy = (point2x * point1y - point1x * point2y) / (point2x - point1x);
	float ratio = slope * peach + consy;
	return ratio;
	
}

void get_each_fac_ratio()
{
	float min_gate_power,max_gate_power;
	float *ratio_each;
	ratio_each = new float[un_num];
	for(int i = 0; i < un_num; i++)
	{
	for(int num = 0; num < point_num[i] - 1; num++)
	{
		if(p_each[i] >= fee_ave_bak[i*10+num].out_power && p_each[i] <= fee_ave_bak[i*10+num+1].out_power)
		{
			ratio_each[i] = powertoratio(&fee_ave_bak[i*10+num],&fee_ave_bak[i*10+num+1],p_each[i]);
			break;
		}
	}
//	printf("JZID=%s  p_each=%f  ratio_each=%f\n",fee_ave[i*10].un_id,p_each[i],ratio_each[i]);
	}
	for(int num = 0; num < un_num; num++)
	{
		min_gate_power = fee_ave_bak[num*10].out_power / ( 1 - fee_ave_bak[num*10].fac_used_ratio);
		max_gate_power = fee_ave_bak[num*10+point_num[num]-1].out_power / ( 1 - fee_ave_bak[num*10+point_num[num]-1].fac_used_ratio);
		for(int i = 0; i < total_bid_un_num; i++)
		{
			if(!strcmp(fee_ave[num*10].un_id,bid_gen[i].un_id))
			{
				bid_gen[i].fac_used_ratio = ratio_each[num];
				bid_gen[i].real_power = (int)(0.5+bid_gen[i].real_power / ( 1 - bid_gen[i].fac_used_ratio));
				bid_gen[i].gen_power = bid_gen[i].real_power * 0.5;
				bid_gen[i].plus_adjust_power = max_gate_power - bid_gen[i].real_power;
				bid_gen[i].minus_adjust_power = bid_gen[i].real_power - min_gate_power;
				if(bid_gen[i].minus_adjust_power < 0)
					bid_gen[i].minus_adjust_power = 0;
				//printf("JZID=%s  bid_gen[%d].real_power = %d\n",fee_ave[num*10].un_id,num,bid_gen[num].real_power);
				break;
			}
		//printf("JZID=%s  bid_gen[%d].real_power = %f\n",fee_ave[i*10].un_id,i,bid_gen[i].real_power);
		}
	//printf("JZID=%s  bid_gen[%d].real_power = %d\n",fee_ave[num*10].un_id,num,bid_gen[num].real_power);
	}
	//for(int num = 0; num < total_bid_un_num; num++)
	//printf("*JZID=%s  *bid_gen[%d].real_power = %d\n",fee_ave[num*10].un_id,num,bid_gen[num].real_power);
	delete[] ratio_each;
}

void deal_gen_gate_power()
{
	get_each_fac_ratio();
	if(write_database_flag !=0)
		set_gen_gate_power();
	merge_gen_to_unit();
	if(write_database_flag !=0)
		set_unit_gate_power();
}

void deal_with_defined_fre_adjust()
{
	num_of_defined_fre_adjust_array = get_num_of_defined_fre_adjust_array();
	num_of_defined_man_adjust_array = get_num_of_defined_man_adjust_array();
	get_defined_fre_adjust_array();
	get_defined_man_adjust_array();
	num_of_defined_fre_adjust_array = num_of_defined_fre_adjust_array + num_of_defined_man_adjust_array;
	if(num_of_defined_fre_adjust_array == 0)
		return;
	un_num_bak = un_num;
	point_num_bak = new int[un_num_bak];
	for(int i = 0; i < un_num; i++)
		point_num_bak[i] = point_num[i];
	
	
	
	int count = 0;
	for(int i = 0; i < un_num_bak; i++)
	{
		for(int j = 0; j < num_of_defined_fre_adjust_array; j++)
		{
			if(!strcmp(fee_ave_bak[i*10].un_id,defined_fre_adjust_array[j].un_id))
			{
				un_num --;
				for(int num = i; num < un_num_bak - 1; num++)
				{
					point_num[num-count] = point_num_bak[num+1];
					for(int k=0; k < point_num[num-count]; k++)
						memcpy(&fee_ave[(num-count)*10 + k],&fee_ave_bak[(num+1)*10 + k],sizeof(price_struct));			
				}
				defined_fre_adjust_array[j].id_num = i;
				count ++;
			//	printf("defined_fre_adjust_array[%d].id_num = %d  count = %d\n",j,i,count);
			}
		}
	}
/*
	for(int i = 0; i < un_num; i ++)
	printf("***********JZID = %s \n",fee_ave[i*10].un_id);
*/
	float ratio = 0.5;
	float m_pload,m_max_p,m_min_p;
	
	while(1)
	{
	m_pload = pload;
	m_max_p = max_p;
	m_min_p = min_p;
	for(int i = 0; i < un_num_bak; i++)
	{
		for(int j = 0; j < num_of_defined_fre_adjust_array; j++)
		{
			if(!strcmp(fee_ave_bak[i*10].un_id,defined_fre_adjust_array[j].un_id))
			{
				defined_fre_adjust_array[j].out_power = (int)(0.5 + (fee_ave_bak[i*10].out_power + ratio * (fee_ave_bak[i*10+point_num_bak[i]-1].out_power - fee_ave_bak[i*10].out_power)));
				m_pload = m_pload - defined_fre_adjust_array[j].out_power;
				m_max_p = m_max_p - fee_ave_bak[i*10+point_num_bak[i]-1].out_power;
				m_min_p = m_min_p - fee_ave_bak[i*10].out_power;
			}
		}
	}
	if(m_pload <= m_max_p && m_pload >= m_min_p)
	{
		pload = m_pload;
		max_p = m_max_p;
		min_p = m_min_p;
		break;
	}
	else if(m_pload > m_max_p)
		ratio += 0.05;
	else if(m_pload < m_min_p)
		ratio -= 0.05;
	if(ratio > 1 || ratio < 0)
	{
		pload = m_pload;
		max_p = m_max_p;
		min_p = m_min_p;
		break;
	}
	}
}


void return_former_struct()//this function just set p_each,un_num,point_num,fee_ave to the full form and right sequence.
{
	float m_p_each[200];
	if(num_of_defined_fre_adjust_array == 0)
		return;
	int flag;
	int count = 0;
	for(int i = un_num_bak-1; i >= 0; i--)
	{
		flag = 0;
		for(int j = 0; j < num_of_defined_fre_adjust_array; j++)
		{
			if(!strcmp(fee_ave_bak[i*10].un_id,defined_fre_adjust_array[j].un_id))
			{
				flag = 1;
				count ++;
				m_p_each[i] = defined_fre_adjust_array[j].out_power;
			}
		}
		if(flag == 0)
			m_p_each[i] = p_each[i-(un_num_bak-un_num)+count];	
	}
	for(int i = 0; i < un_num_bak; i++)
		p_each[i] = m_p_each[i];
	
	un_num = un_num_bak;
	for(int i = 0; i < un_num; i++)
		point_num[i] = point_num_bak[i];
	for(int i = 0; i < un_num; i++)
	for(int num=0; num < point_num[i]; num++)
		memcpy(&fee_ave[i*10 + num],&fee_ave_bak[i*10 + num],sizeof(price_struct));
}

void deletenew()
{
	delete[] fee_ave;
	delete[] point_num;
	if(num_of_defined_fre_adjust_array != 0)
		delete[] point_num_bak;
	delete[] p_each;
	delete[] fee_ave_bak;
	delete[] bid_gen;
	delete[] basic_un;
}



void calculateflow( int sd_num)
{
	float lmt_0,lmt_1,lmt_2;
	float det_p0,det_p1;
	int ss;
	int equal_flag = 0;
	max_p = 0.0;
	min_p = 0.0;
	
	long cputime;
        //clock();
	readin_data( sd_num);//abtain the parameter of units and other data;
        //clock();
        //cputime = clock();
        //printf("The program ended successfully !!\n");
        //printf(" read data took %.2fsecs.\n",cputime/1.0e+6);
	p_each = new float[un_num];
	if(!strcmp(constrain_flag,"yys"))
		deal_with_defined_fre_adjust();
	
	//printf("max_p=%f,sd=%d\n",max_p,sd_id);
	//printf("min_p=%f,sd=%d\n",min_p,sd_id);
	if(pload > max_p)
	{
		printf("the load is greater than the maximum power that the total units can generated while time block=%d!\n",sd_id);
		if(write_database_flag !=0)
		{
			sprintf(result_info,"错误");
			sprintf(result_info_explain,"第%d时段负荷大于所有运行机组出力上限之和",sd_id);
		    write_execute_info();
			result_correct = 0;
			edge_price = 0.0;
			merge_gen_to_unit();
			set_gen_load(year,month,day,sd_id);
			set_unit_load(year,month,day,sd_id);
			set_edge_price(year,month,day,sd_id);
		}
                                                      
		deletenew();
		free_price_struct();
		free_gen_state_struct();
		free_gen_un_struct();
	//printf("max_p=%f\n",max_p);
		return;
	}
	if(pload < min_p)
	{
		printf("the load is less than the minimum power that the total units must generated while time block=%d!\n",sd_id);
		if(write_database_flag !=0)
		{
			sprintf(result_info,"错误");
			sprintf(result_info_explain,"第%d时段负荷小于所有运行机组出力下限之和",sd_id);
			write_execute_info();
			result_correct = 0;
			edge_price = 0.0;
			merge_gen_to_unit();
			set_gen_load(year,month,day,sd_id);
			set_unit_load(year,month,day,sd_id);
			set_edge_price(year,month,day,sd_id);
		}
		deletenew();
		free_price_struct();
		free_gen_state_struct();
		free_gen_un_struct();
			//printf("min_p=%f\n",min_p);
		return;
	}
	handle_slope();//to deal with the slope line to hori_line if the slope is very little.
	sort_qe();//to find out the maximum and minimum value of ED.
	lmt_plus = max_lmt;
	lmt_minus = min_lmt;
	lmt_up = max_lmt;
	lmt_down = min_lmt;
	circul_num = 0;
	
	//	printf("**pload = %f\n",pload);
	ss = check_modify_hori_line();//if there is a horizontal line located between lmt_plus and lmt_minus, then deal with it to eliminate the inffluence of the horizontal line,which the main reason is that the cuvrve do not ascend continually.
	printf("ave_lmt=%10f\n",ave_lmt); 
	if(ss != 1)
	{
		while(1)
		{
			if(circul_num == 0)
			{
				ave_lmt = lmt_up - (lmt_up - lmt_down) / 5.0;
				lmt_0 = ave_lmt;
				lmttopsum();
				circul_num++;
				det_p = p_sum - pload;
				det_p0 = det_p;
				//if(fabs(det_p) < accuracy)
				if(fabs(det_p) < 0.01)
					break;
				else if(det_p > 0)
				{
					lmt_plus = ave_lmt;
					lmt_up = ave_lmt;
					ave_lmt = ave_lmt - (lmt_up - lmt_down) / 5.0;
					lmt_1 = ave_lmt;
				}
				else 
				{
					lmt_minus = ave_lmt;
					lmt_down = ave_lmt;
					ave_lmt = ave_lmt + (lmt_up - lmt_down) / 5.0;
					lmt_1 = ave_lmt;
				}
			}
			det_p0 = det_p;
			circul_num++;
			lmttopsum();
			det_p = p_sum - pload;
			det_p1 = det_p;
			//if(fabs(det_p) < accuracy || circul_num >= 20)//if the accurate factor is 0.001,the program will not work.
			if(fabs(det_p) < 0.01)//if the accurate factor is 0.001,the program will not work.
				break;
			else if(det_p > 0)
			{
				lmt_up = ave_lmt;
				if(lmt_up == lmt_plus && lmt_down == lmt_minus)
				{
					equal_flag = 1;
					break;
				}
				lmt_plus = ave_lmt;
				if(det_p0 == det_p1)
	                ave_lmt = ave_lmt - (lmt_up - lmt_down)/5.0;
	            else
					ave_lmt = lmt_0 + det_p0 * (lmt_1 - lmt_0) / (det_p0 - det_p1);
				if(ave_lmt <= lmt_down || ave_lmt >= lmt_up)
					ave_lmt = lmt_up - (lmt_up - lmt_down)/5.0;
				lmt_0 = lmt_1;
				lmt_1 = ave_lmt;
			}
			else if(det_p < 0)
			{
				lmt_down = ave_lmt;
				if(lmt_up == lmt_plus && lmt_down == lmt_minus)
				{
					equal_flag = 1;
					break;
				}
				lmt_minus = ave_lmt;
				if(det_p0 == det_p1)
	                ave_lmt = ave_lmt + (lmt_up - lmt_down)/5.0;
	            else
					ave_lmt = lmt_0 + det_p0 * (lmt_1 - lmt_0) / (det_p0 - det_p1);
				if(ave_lmt >= lmt_up || ave_lmt <= lmt_down)
	                ave_lmt = lmt_down + (lmt_up - lmt_down)/5.0;
				lmt_0 = lmt_1;
				lmt_1 = ave_lmt;
			}
			printf("lmt_up=%10f,lmt_down=%10f   ",lmt_up,lmt_down);
		}
		printf("ave_lmt=%10f\n",ave_lmt);
		if(equal_flag == 1)
			calcu_power_of_vibration();
	}
	//**********************************this is for defined fre_adjust code**********************
	if(!strcmp(constrain_flag,"yys"))
		return_former_struct();//deal with fee_ave struct and un_num variable;
	//**********************************code end*************************************************
	getedgeprice();

//*********************to get the loss factor***********************
	gen_loss_factor = new loss_factor_struct[total_bid_un_num];
	for(int i = 0; i < total_bid_un_num; i++)
		get_loss_factor(i,bid_gen[i].un_id);
//******************************************************************	
	for(int num = 0; num < un_num; num++)
	{
		for(int i = 0; i < total_bid_un_num; i++)
		{
			if(!strcmp(fee_ave[num*10].un_id,bid_gen[i].un_id))
			{
				bid_gen[i].real_power = (int)(p_each[num] + 0.5);
				bid_gen[i].plus_adjust_power = fee_ave_bak[num*10+point_num[num]-1].out_power - bid_gen[i].real_power;
				bid_gen[i].minus_adjust_power = bid_gen[i].real_power - fee_ave_bak[num*10].out_power;
				bid_gen[i].gen_power = bid_gen[i].real_power * 0.5;
				bid_gen[i].price_for_power = price_each[num];
				bid_gen[i].price_for_gen = edge_price * ((1 - gen_loss_factor[i].region_loss_factor) * (1 - gen_loss_factor[i].within_region_loss_factor) * (1 - gen_loss_factor[i].distribution_loss_factor));
				break;
			}
		}
	}

//***********************the end by dingpeng on May 17,2001******************************
	//for(int i = 0; i < total_bid_un_num; i++)
	//printf("JZID = %s set_load = %d   plus_adjust = %d  minus_adjust = %d  gen_power = %f price_for_gen = %f\n",bid_gen[i].un_id,bid_gen[i].real_power,bid_gen[i].plus_adjust_power,bid_gen[i].minus_adjust_power,bid_gen[i].gen_power,bid_gen[i].price_for_power);
	//if(write_database_flag !=0)
	set_bid_price(year,month,day,sd_id);
	delete[] price_each;

//*******************just for test************************
                int sum_load = 0;
                for(int i = 0; i < total_bid_un_num; i++)
                {
                        sum_load += bid_gen[i].real_power;
                }
                printf("sum_load=%d    ",sum_load);
//*************************test end***********************
	for(int num = 0; num < num_of_start_shut_curve_array; num++)
	{
		if(start_shut_curve_array[num].sd_id == sd_id)
		for(int i = 0; i < total_bid_un_num; i++)
		{
			if(!strcmp(start_shut_curve_array[num].un_id,bid_gen[i].un_id))
			{
				bid_gen[i].real_power = start_shut_curve_array[num].out_power;
				bid_gen[i].gen_power = bid_gen[i].real_power * 0.5;
				bid_gen[i].price_for_gen = edge_price * ((1 - gen_loss_factor[i].region_loss_factor) * (1 - gen_loss_factor[i].within_region_loss_factor) * (1 - gen_loss_factor[i].distribution_loss_factor));
			}
		}
	}
//*******************just for test************************
                int sum_load_2 = 0;
                for(int i = 0; i < total_bid_un_num; i++)
                {
                        sum_load_2 += bid_gen[i].real_power;
                }
                printf("sum_load_2=%d    ",sum_load_2);
//*************************test end***********************
	merge_gen_to_unit();
	//clock();
	if(write_database_flag !=0)
	{
		set_gen_load(year,month,day,sd_id);
		set_unit_load(year,month,day,sd_id);
		set_edge_price(year,month,day,sd_id);
	}
	//clock();
        //cputime = clock();
        //printf("The program ended successfully !!\n");
        //printf("writedata took %.2fsecs.\n",cputime/1.0e+6);
                                                      

//*************this code just for the chukou power of each gen***********
	if(!strcmp(constrain_flag,"yys"))
	{
		delete[] basic_un;
		deal_gen_gate_power();
	}
//**********************************chukou end***************************

	printf("edge_price=%f    ",edge_price);
	printf("circul_num=%d\n\n",circul_num);
	for(int num = 0; num < un_num; num++)
	{
//		fprintf(fp,"the power dispatch is p_each[num]=%f\n",p_each[num]);
	}
	//fprintf(fp,"the edge price is: %f\n",edge_price);
	//fprintf(fp,"the total number of interation is: %d\n",circul_num);
	delete[] gen_loss_factor;
	deletenew();
	free_price_struct();
	free_gen_state_struct();
	free_gen_un_struct();
        //clock();
        //cputime = clock();
        //printf("The program ended successfully !!\n");
        //printf(" it took %.2fsecs.\n",cputime/1.0e+6);
}

void main(int argc, char** argv)
{
	int readin_sd;
	write_database_flag = 1;
	result_correct = 1;
	get_date_and_time();
	set_opp_rela();
	sprintf(program_id,"dwzfhfp");
    sprintf(program_name,"等微增负荷分配");
	//clear_execute_info();
//******************this part read in the date and time block**************
	if(argc != 4 && argc != 6 && argc != 5)
	{
		printf("please input the correct parameter.");
		sprintf(result_info,"错误");
        sprintf(result_info_explain,"请输入正确的参数，第一个参数为日期参数mm/dd/yyyy；第二个参数为时段，0表示计算所有时段，1-48表示计算某个时段；第三个参数为wys或yys,wys表示计算无约束负荷分配，yys表示计算有约束负荷分配"); 
		write_execute_info();
		return;
	}
	else
	{
		char *p;
		p = *(argv+1);
		strcpy(present_date, p);
		if(p[0] == '0')
		{
			p += 1;
			strncpy(month,p,1);
			p += 2;
			month[1] = '\0';
		}
		else
		{
			strncpy(month,p,2);
			p += 3;
			month[2] = '\0';
		}
		if(p[0] == '0')
		{
			p += 1;
			strncpy(day,p,1);
			p += 2;
			day[1] = '\0';
		}
		else
		{
			strncpy(day,p,2);
			p += 3;
			day[2] = '\0';
		}
		strncpy(year,p,4);
		year[4] = '\0';
	//	sprintf(present_date,"%s/%s/%s",month,day,year);
		readin_sd = atoi(*(argv+2));
		strcpy(constrain_flag,*(argv+3));
		if(strcmp(constrain_flag,"wys") && strcmp(constrain_flag,"yys"))
		{
			printf("please input the correct parameter.");
			return;
		}
	//	printf("month=%s,day=%s,year=%s,readin_sd=%d\n",month,day,year,readin_sd);
		strcpy(user_id,"");
		strcpy(user_name,"");
		if(argc == 6)
		{
			strcpy(user_id,*(argv+4));
			strcpy(user_name,*(argv+5));
		}
		if(argc == 5)
		{
			if(strcmp(*(argv+4),"0") != 0)
			{
				printf("please input the correct parameter.");
				return;
			}
			write_database_flag = atoi(*(argv+4));
		}
	}

	if(write_database_flag !=0)
		clear_execute_info();

//********date is in the form of 11/24/1999. if readin_sd <= 0 then calculate all the time blocks,or calculate the specefied time block***************************************************

	if(readin_sd > 0 )
	{
		calculateflow(readin_sd);
		//printf("sd=%d",readin_sd);
	}
	else if(readin_sd <= 0)
	{
		total_sd = get_time_num(year);
		for(int sd = 1; sd <= total_sd; sd++)
		{
			calculateflow(sd);
		}
	}
	if(write_database_flag !=0)
	{
		if(result_correct != 0)
		{
			sprintf(result_info,"正确");
		    sprintf(result_info_explain,"");
		    write_execute_info();
		}
	}
	CS_RETCODE       rcode=0;
    rcode = close_connect_with_server();
    if(rcode!=CS_SUCCEED)
    {
    	printf("database close failed!!\n");
        exit(-1);
    }
}

