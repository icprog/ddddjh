//opf_sens.cxx by Wang GT
/////////////////////////////////

#include"common.h"
#include"opf.h"
#include"opf_global.h"

#include"opf_db.h"
#include"oracle.h"
///////////////////////////////

#include"common.cpp"
#include"opffun.cpp"
#include"opf_sensfun.cpp"

#include"opf_dbfun.cpp"
#include"oracle.cpp"
///////////////////////////////////////////////////

void main(long argc,char *argv[])
{
	if(argc!=3){ printUsage(); exit(0); }
	run_start=clock();

        strcpy(date_str,argv[1]);
        sd=atol(argv[2]);

        printf("\ndate_str=%s",date_str);
        printf("\nsd = %ld",sd);

        strcpy(user_id,"scd");
        strcpy(user_descr,"°²È«Ð£ºË");
        //////////////////////////////////////////////

	//1: starting up and reading data

	OPF_start("Dongbei",NO);//prnt 
	printf("\nOPF_start() ok");

	OPF_read_rtdb(NO);
	printf("\nOPF_read_rtdb() ok");

	OPF_read_oracle(date_str,sd,NO);
	printf("\nOPF_read_oracle() ok");

	printDatafile(basfile,NO);
	printf("\n%s formed",basfile);
	///////////////////////////////////////////

	//2: pretreatment of all data

	//OPF_baselineAlt(NO);//prnt
	//printf("\nOPF_baselineAlt() ok");

	OPF_prtt(YES);//sys.info obtained
	printf("\nOPF_prtt() ok");

	printDatafile(vfyfile,NO);//prnt
	printf("\n%s formed",vfyfile);
	//////////////////////////////////////////////
	//////////////////////////////////////////////

    //3: original powerflow

    //3.1 powerflow function

    pwrflow_init(NO);//prnt
    printf("\npwrflow_init() ok");

	pwrflow(MODNO,YES);//prnt
    printf("\npwrflow() ok");
	////////////////////////////////////////////

	//3.2 modi. slack of genrData

	modiSlack(genrData,nodeP,codeList);
    printf("\nmodiSlack() ok");

    //3.3 groupflow calculation

    groupFlow(groupData,flowData,sub_flowData);
    printf("\ngroupFlow() ok");
    ///////////////////////////////////////////////////

	//3.4 generator reactive
        
	genrReactiv(genrData,nodeQ,codeList,nodeNum);
	printf("\ngenrReactiv() ok");

	//3.5 monitored voltage

	monVoltage(voltData,vv,codeList,nodeNum);
	printf("\nmonVoltage() ok");
	////////////////////////////////////////////////// 

	//3.6 resolution report

	pwrflow_report(resfile,"w",NO);//prnt
	printf("\npwrflow_report() ok");
	///////////////////////////////////////////////////////

	//4: sensitivity analysis

	//4.1 active sensitivity 

	if(argc==1 || inArgv("-w",argv,argc)==YES)
	{	
		sens_w_cal(YES);//prnt
		printf("\nsens_w_cal() ok");

		sens_w_report(resfile,"a",NO);//prnt
		printf("\nsens_w_report() ok");
	}//end if
	/////////////////////////////////////////////////

	//4.3 reporting sens. results 

	OPF_report_sens(snsfile,NO);//prnt
	printf("\nOPF_report_sens() ok");
	///////////////////////////////////////
	
LAB99:
	date_time(endtime);
	date_time(crrtime);
	
	global_free();//global freed
	////////////////////////////////////////

	printf("\nSuccess!\n");
	return;
}
//end of file
