//w_regu.cxx by Wang GT, All Rights Reserved
///////////////////////////////////////////////////

#include"blp.h"
#include"blp_global.h"

#include"common.h"
#include"opf.h"
#include"opf_global.h"

#include"opf_db.h"
#include"oracle.h"
/////////////////////////////////////

#include"blpfun.cpp"
#include"common.cpp"

#include"opffun.cpp"
#include"opf_wfun.cpp"

#include"opf_dbfun.cpp"
#include"oracle.cpp"
/////////////////////////////////////////////////////////

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

        //1: starting and reading data

        OPF_start("Dongbei",NO);//prnt
        printf("\nOPF_start() ok");

        OPF_read_rtdb(NO);
        printf("\nOPF_read_rtdb() ok");

	OPF_read_oracle(date_str,sd,NO);//prnt
	printf("\nOPF_read_oracle() ok");

        printDatafile(basfile,NO);
        printf("\n%s formed",basfile);
        //////////////////////////////////////////////

        //2: pretreatment and org. powerflow 

        OPF_baselineAlt(NO);//prnt
        printf("\nOPF_baselineAlt() ok");

        OPF_prtt(YES);//sys.info obtained
        printf("\nOPF_prtt() ok");

        printDatafile(vfyfile,NO);//prnt
        printf("\n%s formed",vfyfile);
	/////////////////////////////////////////////////

        //2.2 original powerflow

        OPF_org_pwrflow(resfile,YES);//prnt
        printf("\nOPF_org_pwrflow() ok");
        ///////////////////////////////////////////////////

	//3: memory allocation and w_sens.

	OPF_w_alloc();
	printf("\nOPF_w_alloc() ok");

	OPF_w_sens(snsfile,NO);//prnt
	printf("\nOPF_w_sens() ok");
	/////////////////////////////////////////////////////

        //4: preparation for iteration 

        initGenrData(genrData);//genr. power

        pickCurrGenr(crrgen,nodeP,codeList);
        copyVector(optgen,crrgen,genrNum);

        initVector(prevInc,genrNum,0.0);
        initVector(currInc,genrNum,0.0);

        appendSysInfo_w(sysInfo0,flowData0,0.0,0.0);
        printf("\npreparation ok");
        /////////////////////////////////////////////////////////

        //5: w_regu iteration progress

	long OPF_flag=YES;//init.
	long itr_opt,itr=ITR_START;
	
	itrData=NULL;//init.
	itr_start=clock();
	////////////////////////////////////////////////////////

LAB01:	
	//5.1 violation checking 

	if(++itr>ITR_MAX)
	{ 
		printf("\n\nWarning! --- %ld",__LINE__);
		printf("\n  Max iteration reached: itr=%ld",itr);

		OPF_flag=NO;//failed 
		goto LAB99; 
	}//end if
	
	ret=vio_check(itrData,crrgen,genrNum,itr,YES);//prnt
	if(ret==NO){ OPF_flag=YES; goto LAB99; }

	itr_opt=tailNode(itrData)->imin;
	if(itr_opt=itr) copyVector(optgen,crrgen,genrNum);
	////////////////////////////////////////////////////////

	//5.2 blpfun() and step checking

	ret=OPF_w_blpfun(currInc,crrgen,genrNum,YES);//prnt
	if(ret==NO && itr==ITR_START)
	{ 
		printf("\n\nWarning! --- %ld",__LINE__);
		printf("\n  No Solution for Original Problem!");
		
		OPF_flag=NO;//failed 
		goto LAB99; 
	}//end if

	if(ret==NO && itr>ITR_START){ OPF_flag=NO; goto LAB99; }

	ret=step_check(resfile,"a",currInc,prevInc,genw_maxstep,genw_eps,div_coeff,genrNum,YES);
	if(ret==YES) goto LAB99;
	////////////////////////////////////////////////////////
	
	//5.3 power_flow modification

	pwrflow_update(currInc,genrNum,NO);//prnt
	printf("\nOPF_pwrflow_update() ok");
	
	addVector(crrgen,currInc,genrNum);
	copyVector(prevInc,currInc,genrNum);

	goto LAB01;//next itr.
	////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////
LAB99:
	//6: final and optimal powerflow

	//6.1 itrtime and sychoff adjustment

	itrtime=timefun(itr_start);

	synchoff_init(genrData);
	if(OPF_flag==NO)
	{
		synchoff_adj(genrData,GRPG,groupNum,genrNum,NO);//prnt
		printf("\nsynchoff_adj() ok");
	}//end if
	/////////////////////////////////////////////////////////

	//6.2 cal. optimal powerflow

	if(OPF_flag==NO) copyVector(crrgen,optgen,genrNum);

	OPF_opt_pwrflow(resfile,crrgen,genrNum,YES);//prnt
	printf("\nOPF_opt_pwrflow() ok");
	//////////////////////////////////////////////////////////

	//6.3 runtime and writing oracle 

	runtime=timefun(run_start);
	printf("\n\nruntime=%.2lfs",runtime);

	OPF_write_oracle(date_str,sd,YES);//prnt
	printf("\nOPF_write_oracle() ok");
	//////////////////////////////////////////////////////////

	printf("\n\nSuccess!\n");
	return;
}
//end of file
