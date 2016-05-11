//opf_init.cxx by Wang GT
////////////////////////////////////

#include"common.h"
#include"opf.h"
#include"opf_global.h"

#include"opf_db.h"
#include"sybase.h"
/////////////////////////////

#include"common.cpp"
#include"opffun.cpp"

#include"opf_initfun.cpp"
#include"opf_dbfun.cpp"
#include"sybase.cpp"
/////////////////////////////////////

long main(long argc,char *argv[])
//long main()
{
//Notation://////////////////////////////////////////////////
//	argv[0] = opf_init
//	argv[1] = month/day/year 
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
	/////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////

	//3: memory allocation
	
	makeMatrix(_B1,B1,nodeNum,nodeNum);
    makeMatrix(_B2,B2,nodeNum-pvNodeNum,nodeNum-pvNodeNum);

    codeList=(long *)calloc(nodeNum+1,sizeof(long));
    codeAttr=(long *)calloc(nodeNum+1,sizeof(long));

    if(codeList==NULL || codeAttr==NULL)
	{
		consoleMsg("Error!","Allocation Failed",__FILE__,__LINE__);
		printf("\nError! --- %ld",__LINE__); 

		return OPF_FAIL; 
	}//end if
	////////////////////////////////////////////////

	//4: intercon.chechup and coding nodes

	long ret=interConnex(lineData);
	if(ret!=YES)
	{ 
		consoleMsg("错误","Intercon Checkup NOT Passed",__FILE__,__LINE__);
		printf("\n\nIntercon Checkup NOT Passed"); 
		
		return OPF_FAIL; 
	}//end if
	printf("\ninterConnex() ok");

	codingNode(codeList,codeAttr);
	printf("\ncodingNode() ok");
	/////////////////////////////////////////
      
	//5: factor tables
 
	formB1(B1,codeList,codeAttr);
	factorTable(B1,nodeNum-1);
    	printf("\nB1[][] obtained");

    	formB2X(B2,codeList,codeAttr);
    	factorTable(B2,nodeNum-pvNodeNum-1);
    	printf("\nB2[][] obtained");
	//////////////////////////////////////////////////

	//6: verifying other data chains

	veriData(capaData);	printf("\ncapaData  verified");
	veriData(genrData);	printf("\ngenrData  verified");	
	veriData(groupData);	printf("\ngroupData verified");
	veriData(nodeData);	printf("\nnodeData  verified");
	veriData(priceData);	printf("\npriceData verified");
	/////////////////////////////////////////////////////////////////

	//7: output files

    FILE *out=fopen(initfile,"w");

    printVector(out,codeList,(long)0,nodeNum,"LIST");
    printVector(out,codeAttr,(long)0,nodeNum,"ATTR");

    printSparMatrix(out,B1,nodeNum-1,"B1FT");
    printSparMatrix(out,B2,nodeNum-pvNodeNum-1,"B2FT");

    fclose(out);
    printf("\n%s formed",initfile);

    printDataFile(verifile,NO);
    printf("\n%s formed",verifile);
    /////////////////////////////////////////////////////

	double runtime=(double)(clock()-run_start)/CLOCKS_PER_SEC;
	printf("\nruntime=%7.2lfs",runtime);

	consoleMsg("正确","初始化执行正常！");
	printf("\n\nSuccess!\n");
	
	freeGlobal();
	return OPF_SUCCESS;
}
//end of file
