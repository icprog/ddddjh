//opf_sens.cxx by Wang GT
/////////////////////////////////

#include"common.h"
#include"opf.h"
#include"opf_global.h"
#include"opf_db.h"
#include"sybase.h"
/////////////////////

#include"common.cpp"
#include"opffun.cpp"

#include"opf_sensfun.cpp"
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
	//////////////////////////////////////////

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

	//2: pretreatment of all data

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
	////////////////////////////////////////////////

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

	//2: allocation for sens.matrices and vectors

	double *_PBPG,**PBPG,*_GRPG,**GRPG;
	double *PLPG;

	makeMatrix(_GRPG,GRPG,groupNum+1,genrNum+1);
	makeMatrix(_PBPG,PBPG,lineNum+1,genrNum+1);

	PLPG=(double *)calloc(genrNum+1,sizeof(double));
	if(PLPG==NULL){ printf("\nError! --- %ld",__LINE__); return OPF_FAIL; }
	//////////////////////////////////////////////////////

	//2.2 memory allocation for aux.matrices and vectors

	long nw=nodeNum-1,*tempList_w;
	double *PLPQ,*_J2,**J2,*_DP,**DP;

	makeMatrix(_J2,J2,2*nodeNum+1,2*nodeNum+1);
	makeMatrix(_DP,DP,lineNum+1,nw+1);
	
	PLPQ=(double *)calloc(2*nodeNum+1,sizeof(double));
	tempList_w=(long *)calloc(nw+1,sizeof(long));

	if(tempList_w==NULL || PLPQ==NULL){ printf("\nError! --- %ld",__LINE__); return OPF_FAIL; }
	////////////////////////////////////////////////////////////
	
	//3: Jacobi J2[][]

	Jacobi(J2,G,B,vv,v0);
	transMatrix(J2,2*nodeNum);//No Change!

	printf("\nJacobi() ok");
	////////////////////////////////////

	//3.2 PLPQ[]

	sens_PLPQ(PLPQ,J2,codeList[0],YES);//full PLPQ[] obtained
	printf("\nsens_PLPQ() ok");
	//////////////////////////////////////

	//3.3 tempList_w[] and DP[][]

	formTempList(tempList_w,codeList,codeAttr,SKIP_SLCK);
	printf("\nformTempList() ok");

	DPfun(DP,tempList_w,nw,YES);//prnt
	printf("\nDPfun() ok");
	///////////////////////////////////////////////////////

	//4: PLPG[] --- loss sensitivity

	sens_PLPG(PLPG,PLPQ,codeList);
	printf("\nsens_PLPG() ok");
	////////////////////////////////////////

	//4.2 PBPG[] --- branch sens.

	sens_PBPG(PBPG,DP,tempList_w,nw);
	printf("\nsens_PBPG() ok");
	///////////////////////////////////////

	//4.3 GRPG[] --- group sens.

	sens_GRPG(GRPG,PBPG,groupNum,genrNum);
	printf("\nsens_GRPG() ok");
	//////////////////////////////////////////

	//5: result report

	out=fopen(sensfile,"w");
	if(out==NULL)
	{
		consoleMsg("Error","Cannot open resufile.",__FILE__,__LINE__);
		printf("\n\nCannot open %s",resufile);
	
		return OPF_FAIL;
	}//end if
	///////////////////////////////////////////////////

	printSparMatrix(out,GRPG,groupNum,genrNum,"GRPG");
	printSparMatrix(out,PBPG,lineNum,genrNum,"PBPG");
	printVector(out,PLPG,genrNum,"PLPG");

	fclose(out);
	printf("\n%s formed",sensfile);
	///////////////////////////////////////////////////

	freeGlobal();
	
	free(PLPG);
	free(PLPQ);
	free(tempList_w);

	freeMatrix(_DP,DP);
	freeMatrix(_J2,J2);

	freeMatrix(_GRPG,GRPG);
	freeMatrix(_PBPG,PBPG);
	/////////////////////////////////////
	
	double runtime=(double)(clock()-run_start)/CLOCKS_PER_SEC;
	printf("\nruntime=%7.2lfs",runtime);

	consoleMsg("正确","灵敏度执行正常！");
	printf("\n\nSuccess!\n");

	return OPF_SUCCESS;
}
//end of file
