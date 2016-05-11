//opf_init.cxx by Wang GT
////////////////////////////////////

#include"common.h"
#include"opf.h"
#include"opf_global.h"

#include"opf_db.h"
#include"oracle.h"
/////////////////////////////

#include"common.cpp"

#include"opffun.cpp"
#include"opf_initfun.cpp"

#include"opf_dbfun.cpp"
#include"oracle.cpp"
/////////////////////////////////////////////////////

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

	//1: start and read data

	OPF_start("Dongbei",NO);//prnt 
	printf("\nOPF_start() ok");

	OPF_read_rtdb(YES);//prnt
	printf("\nOPF_read_rtdb() ok");

	printDatafile(basfile,YES);//prnt
        printf("\n%s formed",basfile);
	/////////////////////////////////////////////

	OPF_read_oracle(date_str,sd,YES);
	printf("\nOPF_read_oracle() ok");

	printDatafile(basfile,YES);//prnt
	printf("\n%s formed",basfile);
	//////////////////////////////////////////////

	//2: pretreatment of all data

	OPF_baselineAlt(NO);//prnt
	printf("\nOPF_baselineAlt() ok");

	OPF_prtt(YES);//sys.info obtained
	printf("\nOPF_prtt() ok");

	printDatafile(vfyfile,NO);//prnt
	printf("\n%s formed",vfyfile);
	/////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////

	//3: memory allocation

	makeMatrix(_B1,B1,nodeNum,nodeNum);
	makeMatrix(_B2,B2,nodeNum-pvNodeNum,nodeNum-pvNodeNum);

	codeList=(long *)calloc(nodeNum+1,sizeof(long));
	codeAttr=(long *)calloc(nodeNum+1,sizeof(long));

	if(codeAttr==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }
	//////////////////////////////////////////////////////////

	//4: connectivity chechup and coding nodes

	tp_conn(lineData);
	printf("\ntp_conn() ok");

	codingNode(codeList,codeAttr,nodeNum,NO);//prnt
	printf("\ncodingNode() ok");
	///////////////////////////////////////////////

	itr_start=clock();//for itrtime
      
	//5: factor tables
 
	formB1(B1,codeList,codeAttr);
	factorTable(B1,nodeNum-1);
	printf("\nB1[][] obtained");

	formB2X(B2,codeList,codeAttr);
	factorTable(B2,nodeNum-pvNodeNum-1);
    	
	printf("\nB2[][] obtained");
	itrtime=timefun(itr_start);
	//////////////////////////////////////////////////

	//6: verifying data chains

	//OPF_vfyData();
	printf("\nOPF_vfyData() ok");
	////////////////////////////////////////////////////////

	//7: output files

	FILE *out=fopen(inifile,"w");

	printVector(out,codeList,(long)0,nodeNum,"LIST");
	printVector(out,codeAttr,(long)0,nodeNum,"ATTR");

	printSparMatrix(out,B1,nodeNum-1,"B1FT");
	printSparMatrix(out,B2,nodeNum-pvNodeNum-1,"B2FT");

	fclose(out);
	printf("\n\n%s formed",inifile);

	printDatafile(vfyfile,NO);//prnt
	printf("\n%s formed again",vfyfile);
	///////////////////////////////////////

	//7.2: writing database

	//OPF_write_sysInfo();
	//printf("\nOPF_write_sysInfo() ok\n");
	//////////////////////////////////////////

	printf("\nSuccess!\n");
	return;
}
//end of file
