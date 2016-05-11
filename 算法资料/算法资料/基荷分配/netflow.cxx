#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "string.h"
#include "var_def.h"
#include "load_dispatch_db.h"

void gen_fixed_amount(int NumOfHour,int NumOfUnit,float Load[],float TAmount[],int num_of_block[],float UpLimit[][10],float DownLimit[][10],int StartTime[][10],int EndTime[][10],char Type[],char Name[][22],float MW[][100])
{
//to dispatch the units that generation amount is fixed
//Version 1.0 may,1998 Guo wei 
//modified by zzg on Dec,1999 for power market.

//unit data
	float Cost[100][100];
	float EAmount[100],Step[100],Unuplimit[100];
	float p_UpLimit[100][100];

	int i,j,cun,maxhour,unout[100],counter[100];
	int stime[100][10],etime[100][10],ret;
	int time_b[100];//to present the time block that needn't be calculated.
	float maxload;

	for(j=0;j<NumOfUnit;j++)
	{
		EAmount[j]=0;
		Unuplimit[j]=0;
		Step[j] = 1;
		unout[j]=0;
		counter[j]=0;
		time_b[j] = 0;
		for(i=0;i<NumOfHour;i++)
		{
			for(int z_i = 0; z_i < num_of_block[j]; z_i++)
			if(i+1 >= StartTime[j][z_i] && i+1 <= EndTime[j][z_i])
			{
				MW[j][i]=DownLimit[j][z_i];
				p_UpLimit[j][i] = UpLimit[j][z_i];
			}
		}

		if(NumOfHour==48)
		{
			if(TAmount[j]<1)
			{
				//printf("fix amoumt is zero, un=%d,%s, amount=%f, downlimit=%f\n",j,Name[j],TAmount[j],EAmount[j]/4);
				unout[j]=-10;
				for(i=0;i<NumOfHour;i++)
					MW[j][i]=0;
			}
			else if(strcmp(Type,"thermal")==0)//TAmount[j]>1
			{
			for(int z_i = 0; z_i < num_of_block[j]; z_i++)
			if(DownLimit[j][z_i]<1)
				{
				if(StartTime[j][z_i] == 1 && EndTime[j][z_i] != 48)
				{
				stime[j][time_b[j]] = StartTime[j][z_i];
				etime[j][time_b[j]] = EndTime[j][z_i] + 4;
				if(etime[j][time_b[j]] > 48)
					etime[j][time_b[j]] = 48;
				for(int z_j = EndTime[j][z_i] + 1; z_j <= etime[j][time_b[j]]; z_j++)
					{
					MW[j][z_j-1] = DownLimit[j][z_i+1] / 4 * (z_j - EndTime[j][z_i]);
					}
				time_b[j]++;
				}
				if(StartTime[j][z_i] != 1 && EndTime[j][z_i] == 48)
				{
				stime[j][time_b[j]] = StartTime[j][z_i];
				etime[j][time_b[j]] = EndTime[j][z_i];
				time_b[j]++;
				}
				if(StartTime[j][z_i] == 1 && EndTime[j][z_i] == 48)
				{
				stime[j][time_b[j]] = StartTime[j][z_i];
				etime[j][time_b[j]] = EndTime[j][z_i];
				time_b[j]++;
				}
				if(StartTime[j][z_i] != 1 && EndTime[j][z_i] != 48)
				{
				stime[j][time_b[j]] = StartTime[j][z_i];
				etime[j][time_b[j]] = EndTime[j][z_i] + 4;
				if(etime[j][time_b[j]] > 48)
					etime[j][time_b[j]] = 48;
				for(int z_j = EndTime[j][z_i] + 1; z_j <= etime[j][time_b[j]]; z_j++)
					{
					MW[j][z_j-1] = DownLimit[j][z_i+1] / 4 * (z_j - EndTime[j][z_i]);
					}
				time_b[j]++;
				}
				}
			else
				{
				int temp_stime,temp_etime;
				if(EndTime[j][z_i] != 48)
				{
				if(DownLimit[j][z_i]>DownLimit[j][z_i+1] && DownLimit[j][z_i+1] < 1)
					{
					stime[j][time_b[j]] = EndTime[j][z_i] - 3;
					etime[j][time_b[j]] = EndTime[j][z_i];
					if(EndTime[j][z_i] - 3 < 1)
						stime[j][time_b[j]] = 1;
					for(int z_j = stime[j][time_b[j]]; z_j <= EndTime[j][z_i]; z_j++)
						{
						MW[j][z_j-1] = DownLimit[j][z_i] / 4 * (EndTime[j][z_i] - z_j + 1);
						}
					time_b[j]++;
					}
				else if(DownLimit[j][z_i] > DownLimit[j][z_i+1])
					{
					temp_stime = EndTime[j][z_i] - 3;
					temp_etime = EndTime[j][z_i];
					if(EndTime[j][z_i] - 3 < 1)
						temp_stime = 1;
					for(int z_j = temp_stime; z_j <= temp_etime; z_j++)
						{
						MW[j][z_j-1] = DownLimit[j][z_i+1] + (DownLimit[j][z_i] - DownLimit[j][z_i+1]) / 4 * (temp_etime - z_j + 1);
						p_UpLimit[j][z_j-1] = UpLimit[j][z_i+1] + (UpLimit[j][z_i] - UpLimit[j][z_i+1]) / 4 * (temp_etime - z_j + 1);
						}
					}
				else
					{
					temp_stime = EndTime[j][z_i];
					temp_etime = EndTime[j][z_i] + 4;
					if(EndTime[j][z_i] + 4 > 48)
						temp_etime = 48;
					for(int z_j = temp_stime + 1; z_j <= temp_etime; z_j++)
						{
						MW[j][z_j-1] = DownLimit[j][z_i] + (DownLimit[j][z_i+1] - DownLimit[j][z_i]) / 4 * (z_j - temp_stime);
						p_UpLimit[j][z_j-1] = UpLimit[j][z_i] + (UpLimit[j][z_i+1] - UpLimit[j][z_i]) / 4 * (z_j - temp_stime);
						}
					}
				}
				}
			}
		}

		for(i=0;i<NumOfHour;i++)
		{
			Cost[j][i]=9;
			EAmount[j]=EAmount[j]+MW[j][i];
			Unuplimit[j]=Unuplimit[j]+p_UpLimit[j][i];
			Load[i] = Load[i]- MW[j][i];         //add by zhou jingyang 980819
		}

		if(NumOfHour==48)
		{
			if(EAmount[j]/2>TAmount[j])
			{
				if(TAmount[j] > 0)
				{
				printf("fix amoumt lower than down-limit, un=%d,%s, amount=%f, downlimit=%f\n",j,Name[j],TAmount[j],EAmount[j]/2);
				if(correct_result != 1)
				{
				sprintf(result_info,"正确");
			sprintf(result_info_explain,"定电量低于功率下限所发电量之和，机组ID=%s,定电量=%f,下限电量=%f",Name[j],TAmount[j],EAmount[j]/2);
				write_execute_info();
				correct_result = 1;
				}
				}
				unout[j]=-10;
			}
			else if(Unuplimit[j]/2<TAmount[j])
                	{
                        	printf("fix amoumt greater than up-limit, un=%d,%s, amount=%f, uplimit=%f\n",j,Name[j], TAmount[j],Unuplimit[j]/2);
				if(correct_result != 1)
				{
			sprintf(result_info,"正确");
			sprintf(result_info_explain,"定电量高于功率上限所发电量之和，机组ID=%s,定电量=%f,上限电量=%f",Name[j],TAmount[j],Unuplimit[j]/2);
				write_execute_info();
				correct_result = 1;
				}
				EAmount[j]=Unuplimit[j];
				for(i=0;i<NumOfHour;i++)
				{
					MW[j][i]=p_UpLimit[j][i];
					Load[i] = Load[i]- MW[j][i];  
				}
                        	unout[j]=10;
                	}
		}
	}	

	int _alldone,_notdone;
	int nregun[100];

	_alldone = 1; 


	for(j=0;j<NumOfUnit;j++)
	{
		for(i=0;i<NumOfHour;i++)
		{
			for(int z_i = 0; z_i < time_b[j]; z_i++)
			if(i+1 >= stime[j][z_i] && i+1 <= etime[j][z_i])
			{
				Cost[j][i] =  -9;
			}
		}
	}	

	while(_alldone)
	{

tt:		maxload=0;
		maxhour= -1;
		for(i=0;i<NumOfHour;i++)
		{
			nregun[i]=0;
			for(j=0;j<NumOfUnit;j++)
			{
				if ((unout[j]<-1)||(unout[j]>1)) continue;			
				if(Cost[j][i]>0)
				{
					if(NumOfHour==48)
					{	
						if(fabs(TAmount[j]-EAmount[j]/2)>1)
						{
							nregun[i]= nregun[i]+1;
						}
					}
				}
			}
		}


                for(i=0;i<NumOfHour;i++)
                {
                      if((maxload<Load[i])&&(nregun[i]>0))
                      {
                                maxhour=i;
                                maxload=Load[i];
                      }
               }

		if((maxload==0) &&(maxhour==-1))break;
		
		_notdone = 0;
		for(j=0;j<NumOfUnit;j++)
		{
			if ((unout[j]<-1)||(unout[j]>1)) continue;

			if(NumOfHour==48)
			{
				if(fabs(TAmount[j]-EAmount[j]/2)>1)
				{
					cun=j;
					_notdone = 1;
					goto ss;
				}
			}
			
			continue;

ss:			for(int z_i = 0; z_i < time_b[cun]; z_i++)
			if(maxhour+1 >= stime[cun][z_i] && maxhour+1 <= etime[cun][z_i])
		        	continue;
	
			if(Cost[cun][maxhour]>0)
			{
				if((MW[cun][maxhour]+Step[j])>p_UpLimit[cun][maxhour])
				{
					MW[cun][maxhour]=p_UpLimit[cun][maxhour];
					Load[maxhour]=Load[maxhour]-(p_UpLimit[cun][maxhour]-MW[cun][maxhour]);
					Cost[cun][maxhour]=-9;
				}
				else
				{
					MW[cun][maxhour]=MW[cun][maxhour]+Step[j];
					Load[maxhour]=Load[maxhour]-Step[j];
				}

				counter[cun]++;
                        	if(counter[cun]>30000) unout[cun]=10;		

				EAmount[cun]=0;
				for(i=0;i<NumOfHour;i++)
				{
					EAmount[cun]=EAmount[cun]+MW[cun][i];
				}
			}

		}
		if (_notdone == 1)
		{
			goto tt;
		}
		else 
		{
			_alldone = 0;
		}
	}
/*
	for(j=0;j<NumOfUnit;j++)
	{
		for(i=0;i<NumOfHour;i++)
		{
//			if(strcmp(Name[j],"fuxin")==0)
			{
				printf("Name=%s,MW=%f\n",Name[j],MW[j][i]);
			}
		}
	}
*/

	return; 
 }
