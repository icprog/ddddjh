//w_regu.cxx by Wang GT, All Rights Reserved
/////////////////////////////////////////////////

#include"blp.h"
#include"common.h"
#include"opf.h"
#include"opf_global.h"
#include"opf_db.h"
#include"sybase.h"

#include"blp.cpp"
#include"common.cpp"
#include"opffun.cpp"
#include"opf_wfun.cpp"
#include"opf_dbfun.cpp"
#include"sybase.cpp"
/////////////////////////////////////

long main(long argc,char *argv[])
//long main()
{
//Notation://////////////////////////////////////////////////
//      argv[0] = opf_sens
//      argv[1] = month/day/year
//      argv[2] = sd
/////////////////////////////////////////////////////////////

        //0: console & command interpretation

        get_date_and_time();//date,time obtained

        sprintf(program_id,"ssjyzsgl");
        sprintf(program_name,"实时交易阻塞管理");

        if(argc!=3){ printf("\nUsage: opf_init mm/dd/yyyy sd\n"); return OPF_FAIL; }
        char month[5],day[5],year[10]; long sd;

        mdyID(month,day,year,argv[1]);
        sd=atoi(argv[2]);

        delLeadingZero(month);
        delLeadingZero(day);
	///////////////////////////////////////////////////

	clock_t run_start=clock();//for runtime

	//1: starting up and reading data

	OPF_start("Liaoning",NO);//prnt 
	printf("\nOPF_start() ok");

	OPF_readSybase(year,month,day,sd,YES);//prnt
	printf("\nOPF_readSybase() ok");

	OPF_readDB("realtime","opf",getenv("MYSERVER"),YES);
	//OPF_readDB("realtime","rtnet","es2",YES);
	printf("\nOPF_readDB() ok");

	printDataFile(basefile,NO);
	printf("\n%s formed",basefile);
	//////////////////////////////////////////////

	//2: pretreatment and original powerflow 

	baselineAlt(lineData,100,1,LN_RX);
	baselineAlt(lineData,100,1,LN_B0);

	baselineAlt(nodeData,100,1,PQ);
	baselineAlt(capaData,100,1);
	///////////////////////////////////////

	baselineAlt(genrData,100,1);
	baselineAlt(priceData,100,1,BD_POWR);

   	baselineAlt(genrData2,100,1);
	baselineAlt(plantData,100,1);
	//////////////////////////////////

	baselineAlt(groupData,100,1);
	printf("\nbaselineAlt() ok");

	OPF_prtt(YES);//sys.info obtained
	printf("\nOPF_prtt() ok");

	printDataFile(verifile,NO);//prnt
	printf("\n%s formed",verifile);
	//////////////////////////////////////////////
	
	//3: original powerflow

	struct flowstr *flowData=NULL;//No Change!
	struct flowstr *subFlowData=NULL;
	struct sysstr sysInfo;
	
	//3.1 powerflow function 

	flowInit(NO);//prnt
	printf("\nflowInit() ok");

	flowFun(flowData,subFlowData,sysInfo,MODINO,YES);//prnt
	printf("\nflowFun() ok");
	//////////////////////////////////////////////////

	//3.2 groupmode analysis --- once only

	groupModeAnalysis(groupData,flowData,subFlowData);
	printf("\ngroupModeAnalysis() ok");

	//3.3 groupflow calculation

	groupFlow(groupData,flowData,subFlowData);
	printf("\ngroupFlow() ok");
	////////////////////////////////////////////

	//3.4 resolution report

	FILE *out=fopen(resufile,"w");
	if(out==NULL){ printf("\n\nCannot open %s",resufile); return OPF_FAIL; }
	
	fprintf(out,"Powerflow Solution for %s",projname);

	reportNode(out,vv,v0,nodeP,nodeQ,codeList,NO);
	reportLine(out,flowData,subFlowData,NO);
	reportSys(out,sysInfo,YES);
	reportGroupFlow(out,groupData,YES);

	fclose(out);
	printf("\n%s formed",resufile); //wpause(); //exit(0);
	////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////

	//declaration and allocation for w_regu

	long itrx,istep,vioNum,minVioNum,ret,ret2;
	double step,minVioPct,vioPct;

	double *branInc,*currInc,*prevInc;
	double *currGenr,*optiGenr,*grp_w0,*grp_w;

	double *PLPG,*beta_w,*d;
	double *LBmin,*LBmax,*LGmax;
	double *Lmin_w,*Lmax_w;
	double *_GRPG,**GRPG,*_PBPG,**PBPG;
	//////////////////////////////////////////////////////

	makeMatrix(_GRPG,GRPG,groupNum+1,genrNum+1);
	makeMatrix(_PBPG,PBPG,lineNum+1,genrNum+1);

	grp_w0=(double *)calloc(groupNum+1,sizeof(double));
	grp_w =(double *)calloc(groupNum+1,sizeof(double));

	branInc=(double *)calloc(lineNum+1,sizeof(double));
	currInc=(double *)calloc(genrNum+1,sizeof(double));
	prevInc=(double *)calloc(genrNum+1,sizeof(double));
	
	currGenr=(double *)calloc(genrNum+1,sizeof(double));
	optiGenr=(double *)calloc(genrNum+1,sizeof(double));

	PLPG=(double *)calloc(genrNum+1,sizeof(double));
	beta_w=(double *)calloc(genrNum+1,sizeof(double));
	d=(double *)calloc(genrNum+1,sizeof(double));

	LBmin=(double *)calloc(lineNum+1,sizeof(double));
	LBmax=(double *)calloc(lineNum+1,sizeof(double));
	LGmax=(double *)calloc(groupNum+1,sizeof(double));

	Lmin_w=(double *)calloc(genrNum+1,sizeof(double));
	Lmax_w=(double *)calloc(genrNum+1,sizeof(double));

	if(Lmax_w==NULL || LGmax==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }
	//////////////////////////////////////////////////////////////////

	//4: sens.data: PBPG, PLPG, and beta_w[]
 
	readSparMatrix(sensfile,"GRPG",GRPG,groupNum,genrNum);
	readSparMatrix(sensfile,"PBPG",PBPG,lineNum,genrNum);
	readVector(sensfile,"PLPG",PLPG,genrNum);

	beta_wfun(beta_w,PLPG,genrNum);
	printf("\nbeta_wfun() ok");
	/////////////////////////////////////////////////////

	//4.2 preparation for iteration 
 
	pickGroupW(grp_w0,groupData,groupNum);	//orig.grp.flow
	pickCurrGenr(currGenr,nodeP,codeList);	//orig.genr.w

	initVector(currInc,genrNum,0.0);
	initVector(prevInc,genrNum,0.0);
	
	itrx=ITRX_START;//init.
	copyVector(optiGenr,currGenr,genrNum);

	minVioNum=BIGNUM;
	minVioPct=LARG;
	/////////////////////////////////////////////////////

	//5: w_regu iteration

	out=fopen(resufile,"a");
	if(out==NULL){ printf("\n\nError! --- %ld",__LINE__); exit(0); }

LAB01:	
	//.1 violation examination

	vioNum=vioNumStat(groupData);
	vioPct=vioPctStat(groupData);

	if(vioNum<=minVioNum && vioPct<minVioPct)
	{
		copyVector(optiGenr,currGenr,genrNum);
		
		minVioNum=vioNum;
		minVioPct=vioPct;
	}//end if

	printf("\n\nitrx=%2ld: vioNum=%ld, vioPct=%7.4lf",itrx,vioNum,vioPct);
	fprintf(out,"\n\nitrx=%2ld: vioNum=%ld, vioPct=%7.4lf",itrx,vioNum,vioPct);
	/////////////////////////////////////////////////////////

	//.2 process control

	if(itrx>MAX_ITRX)
	{ 
		printf("\n\nWarning! --- %ld",__LINE__);
		printf("\n  Max_itrx reached: itrx=%ld",itrx);

		ret=OPF_SUCCESS; 
		goto LAB99; 
	}//end if
	
	if(vioNum==0)
	{ 
		printf("\nOptimal Solution");

		ret=OPF_SUCCESS;
		goto LAB99; 
	}//end if
	///////////////////////////////////////////////////

	//.3 sensitivity calculation

	objLnz_w(d,currGenr,NO);//prnt
	printf("\n objLnz_w() ok");

	//.4 constraints and bounds

	LBfun_w(LBmin,LBmax,flowData);
	printf("\n LBfun_w() ok");
	
	LGfun_w(LGmax,groupData);
	printf("\n LGfun_w() ok");

	limFun_w(Lmin_w,Lmax_w,currGenr);
	printf("\n limFun_w() ok");
	/////////////////////////////////////////////////////////

	//.5 solve LP problem  --- a critical function

	ret2=formLP_w(currInc,d,beta_w,PBPG,LBmin,LBmax,GRPG,LGmax,Lmin_w,Lmax_w,flowData,YES);//prnt
	printf("\n formLP_w() returned %ld",ret2);
		
	if(ret2==NO && itrx==ITRX_START)
	{
		groupExpFlgReset(groupData);
		printf("\n groupExpFlgReset() ok");

		consoleMsg("错误","初始问题无解，请适当放宽约束！");
		printf("\nNo solution for original problem");

		ret=OPF_FAIL; 
		goto LAB99;
	}//end if

	if(ret2==NO && itrx>ITRX_START){ ret=OPF_FAIL; goto LAB99; }
	/////////////////////////////////////////////////////////////////////

	//.6 step qualification

	stepConfin(currInc,genrNum,genw_maxstep,DIV_WREGU);
	printf("\n stepConfin() ok");

	ret2=sawtooth(prevInc,currInc,genrNum);
	if(ret2==YES) stepReduce(currInc,genrNum,DIV_WREGU);

	ret2=hangback(prevInc,currInc,genrNum);
	if(ret2==YES) stepReduce(currInc,genrNum,DIV_WREGU);
	////////////////////////////////////////////////////////
	
	//.7 step checkup and report

	istep=posiAbsmax(currInc,genrNum);
	step=currInc[istep];

	printf(", step=%7.4lf",step);//No Change!
	if(fabs(step)<genw_eps)
	{ 
		printf("\n\nWarning! --- %ld",__LINE__);
		printf("\n  genw_eps(=%lf) reached",genw_eps); 
	
		ret=OPF_SUCCESS;
		goto LAB99; 
	}//end if
	///////////////////////////////////////////////
	
	//.8 power_flow modification

	getBranInc(branInc,currInc,PBPG);
	printf("\n getBranInc() ok");

	modiFlowData(flowData,branInc);
	modiBusAngl(v0,currInc);
	printf("\n powerflow modified");
	
	groupFlow(groupData,flowData,subFlowData);
	printf("\n groupFlow() ok");

	modiCtrlVctr(currGenr,currInc);
	printf("\n modiCtrlVctr() ok");
		
	copyVector(prevInc,currInc,genrNum);//reserved	
	
	itrx++; //wpause();
	goto LAB01;//vioNum checkup
	///////////////////////////////////
 
LAB99:
	fclose(out);//resufile closed
	//wpause();

	if(ret==OPF_FAIL) copyVector(currGenr,optiGenr,genrNum);
	/////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////

	//6: final powerflow

	//6.1 asso.regulation

	assoFun(currGenr,genrData,assoData,NO);//prnt
	printf("\nassoFun() ok");

	//6.2 updating genrData 

	updateData(genrData,currGenr,genrNum);
	printf("\ngenrData updated");
	//////////////////////////////////////////////

	//6.3 final powerflow

	flowFun(flowData,subFlowData,sysInfo,MODIPG,NO);//modi,prnt
	modiSlack(genrData,nodeP,codeList);
	printf("\nfinal flowFun() ok");

	groupFlow(groupData,flowData,subFlowData);
	printf("\ngroupFlow() ok");
	
	pickGroupW(grp_w,groupData,groupNum);
	printf("\npickGroupW() ok");
	/////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////

	//7: final powerflow report

	out=fopen(resufile,"a");
	if(out==NULL)
	{ 
		printf("\n\nCannot open %s",resufile); 
		return OPF_FAIL; 
	}//end if
	//////////////////////////////////////////

	reportGroupFlow(out,groupData,YES);//prnt
	reportLine(out,flowData,subFlowData,NO);
	reportNode(out,vv,v0,nodeP,nodeQ,codeList,NO);
	reportSys(out,sysInfo,NO);

	fclose(out);
	printf("\nfinal powerflow saved in %s",resufile);
	////////////////////////////////////////////////////

	//7.2 writing into resufile

	out=fopen(resufile,"a");
	if(out==NULL){ printf("\nCannot open %s",resufile); return OPF_FAIL; }

	reportReguPolicy(out,genrData,NO);//prnt
	printf("\nreportReguPolicy() ok");

	fclose(out);
	printf("\nregulation policy saved in %s",resufile);
	///////////////////////////////////////////////////////
 
	//7.3 writing into Sybase

	OPF_writeSybase(year,month,day,sd,genrData,groupData,grp_w0,grp_w,groupNum,YES);//prnt
	printf("\nOPF_writeSybase() ok");
	////////////////////////////////////////////////////////

	freeGlobal();
	freeMatrix(_PBPG,PBPG);

	free(branInc);
	free(currInc);
	free(prevInc);

	free(currGenr);
	free(optiGenr);

	free(PLPG);
	free(beta_w);
	free(d);

	free(LBmin);
	free(LBmax);
	free(Lmin_w);
	free(Lmax_w);
	/////////////////////////////////////////////////////

	double runtime=(clock()-run_start)/CLOCKS_PER_SEC;
	printf("\nruntime=%7.2lfs",runtime);	

	if(ret==OPF_SUCCESS) consoleMsg("正确","程序执行正常！");
	
	printf("\n\nSuccess!\n");//terminated
	return ret;
}
//end of file
