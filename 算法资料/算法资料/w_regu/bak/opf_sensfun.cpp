//opf_sensfun.cpp by Wang GT
//////////////////////////////////

#include"common.h"
#include"opf.h"
#include"opf_global.h"
/////////////////////////////////////////////////////////////

void printUsage()
{
        printf("\nUsage: opf_sens date_str sd");
        printf("\n  date_str --- date string in format of mm/dd/yyyy");
        printf("\n  sd       --- period of time: 1, 2, ... 96\n");

        return;
}

void pertsens_XXPG(double **PBPG,long prnt)
{
	if(prnt==YES) printf("\n\npertsens_XXPG()...");

	//1: locals and memory allocation

	register long i,j;
	
	double aux2,inc,*PB0;
	double flow_eps_sv,*vv_sv,*v0_sv;

	struct flowstr *fp;
	struct genrstr *gp;
	///////////////////////////////////////////////////
	
	vv_sv=(double *)calloc(nodeNum+1,sizeof(double));
	v0_sv=(double *)calloc(nodeNum+1,sizeof(double));

	PB0  =(double *)calloc(lineNum+1,sizeof(double));
	if(PB0==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }
	////////////////////////////////////////////////////////

	//2: preparation for iteration

	copyVector(vv_sv,vv,nodeNum);//original saved
	copyVector(v0_sv,v0,nodeNum);

	flow_eps_sv=flow_eps;
	flow_eps*= 5.0;

	initMatrix(PBPG,lineNum,genrNum,0.0);
	
	fp=flowData;
	for(i=1;i<=lineNum;i++,fp=fp->next) PB0[i]=fp->w1;
	//////////////////////////////////////////////////////////
	
	//3: sens. iteration --- genr.voltage

	gp=genrData;
	for(j=1;j<=genrNum;j++,gp=gp->next)
	{
		if(prnt==YES) printf("\n j=%2ld, genr_id=%s",j,gp->id);

		//3.1 regain original state

		initCtrlData(genrData);

		copyVector(vv,vv_sv,nodeNum);
		copyVector(v0,v0_sv,nodeNum);
		///////////////////////////////////////////////

		//3.2 pert. powerflow

		inc=gp->w*PERT_COEFF;
		if(fabs(inc)<SMLL){ printf("\n\nError! --- %ld",__LINE__); exit(0); }

		gp->w+= inc;
		pwrflow(MODPG,NO);//prnt
		////////////////////////////////////////////

		//3.3 sens. calculation --- PBPG[][]

		fp=flowData;
		for(i=1;i<=lineNum;i++,fp=fp->next)
		{
			aux2=fp->w1-PB0[i];
			PBPG[i][j]=aux2/inc;
		}
	}//end for
	/////////////////////////////////////////////////

	//4: restoring and free memory

	copyVector(vv,vv_sv,nodeNum);
	copyVector(v0,v0_sv,nodeNum);
	
	flow_eps=flow_eps_sv;
	////////////////////////////////

	free(vv_sv);
	free(v0_sv);

	free(PB0);
	/////////////////////////////////////

	return;
}

void sens_w_report(char *resfile,char *wrFlg,long prnt)
{
	if(prnt==YES) printf("\n\nsens_w_report()...");

	//1: opening res.file

	FILE *out=fopen(resfile,wrFlg);
	if(out==NULL){ printf("\nCannot open %s",resfile); exit(0); }

	if(prnt==YES) printf("\n resfile opened: %s",resfile);
	//////////////////////////////////////////////////

	//2: reporting PBPG[][]

	report_PBPG(out,PBPG,NO);//prnt
	if(prnt==YES) printf("\n report_PBPG() ok");
	
	//3: reporting GRPG[][]

	report_GRPG(out,GRPG,NO);//prnt
	if(prnt==YES) printf("\n report_GRPG() ok");
	//////////////////////////////////////////////////////

	//4: reporting PBPD[][]

	report_PBPD(out,PBPD,NO);//prnt
	if(prnt==YES) printf("\n report_PBPD() ok");

	//5: reporting GRPD[][]

	report_GRPD(out,GRPD,NO);//prnt
	if(prnt==YES) printf("\n report_GRPD() ok");
	//////////////////////////////////////////////////

	//5: closing res.file

	fclose(out);
	if(prnt==YES) printf("\n resfile closed");
	//////////////////////////////////////////////////

	return;
}

void report_PBPD(FILE *out,double **PBPD,long prnt)
{
	register long i,j;
	long kk=60;

	struct loadstr  *ldp;
	struct linestr  *lp;
	//////////////////////////////////////////////

	//1: painting the title

	fprintf(out,"\n\n******  Branch-ld Active Sens. Report  ******");
	fdrawline(out,kk);

	fprintf(out,"\n load=> ");
	ldp=loadData;

	for(i=1;i<=loadNum;i++,ldp=ldp->next)
		fprintf(out,"  %s(ibs=%ld)",ldp->id,ldp->i);

	fdrawline(out,kk);
	////////////////////////////////////////////////

	//2: reporting sens. of PBPD[][]

	lp=lineData;
	for(i=1;i<=lineNum;i++,lp=lp->next)
	{
		fprintf(out,"\n Line %4ld-%4ld",lp->i,lp->j);

		for(j=1;j<=loadNum;j++)
			fprintf(out,"  %9.5lf",PBPD[i][j]);
	}//end for
	////////////////////////////////////////////////
	
	fdrawline(out,kk);
	return;
}

void report_PBPG(FILE *out,double **PBPG,long prnt)
{
	register long i,j;
	long kk=60;

	struct genrstr  *gp;
	struct linestr  *lp;
	//////////////////////////////////////////////

	//1: painting the title

	fprintf(out,"\n\n******  Branch-un Active Sens. Report  ******");
	fdrawline(out,kk);

	fprintf(out,"\n genr=> ");
	gp=genrData;

	for(i=1;i<=genrNum;i++,gp=gp->next)
		fprintf(out,"  %s(ibs=%ld)",gp->id,gp->i);

	fdrawline(out,kk);
	////////////////////////////////////////////////

	//2: reporting sens. of PBPG[][]

	lp=lineData;
	for(i=1;i<=lineNum;i++,lp=lp->next)
	{
		fprintf(out,"\n Line %4ld-%4ld",lp->i,lp->j);

		for(j=1;j<=genrNum;j++)
			fprintf(out,"  %9.5lf",PBPG[i][j]);
	}//end for
	////////////////////////////////////////////////
	
	fdrawline(out,kk);
	return;
}

void report_GRPD(FILE *out,double **GRPD,long prnt)
{
	register long i,j;
	long kk=60;

	struct loadstr  *ldp;
	struct groupstr *grp;
	//////////////////////////////////////////////

	//1: painting the title

	fprintf(out,"\n\n******  Group-ld Active Sens. Report  ******");
	fdrawline(out,kk);

	fprintf(out,"\n load=> ");
	ldp=loadData;

	for(i=1;i<=loadNum;i++,ldp=ldp->next)
		fprintf(out,"  %s(ibs=%ld)",ldp->id,ldp->i);

	fdrawline(out,kk);
	////////////////////////////////////////////////

	//2: reporting sens. of GRPD[][]

	grp=groupData;
	for(i=1;i<=groupNum;i++,grp=grp->next)
	{
		fprintf(out,"\n Group %s :",grp->id);

		for(j=1;j<=loadNum;j++)
			fprintf(out,"  %9.5lf",GRPD[i][j]);
	}//end for
	////////////////////////////////////////////////
	
	fdrawline(out,kk);
	return;
}

void report_GRPG(FILE *out,double **GRPG,long prnt)
{
	register long i,j;
	long kk=60;

	struct genrstr  *gp;
	struct groupstr *grp;
	//////////////////////////////////////////////

	//1: painting the title

	fprintf(out,"\n\n******  Group-un Active Sens. Report  ******");
	fdrawline(out,kk);

	fprintf(out,"\n genr=> ");
	gp=genrData;

	for(i=1;i<=genrNum;i++,gp=gp->next)
		fprintf(out,"  %s(ibs=%ld)",gp->id,gp->i);

	fdrawline(out,kk);
	////////////////////////////////////////////////

	//2: reporting sens. of GRPG[][]

	grp=groupData;
	for(i=1;i<=groupNum;i++,grp=grp->next)
	{
		fprintf(out,"\n Group %s :",grp->id);

		for(j=1;j<=genrNum;j++)
			fprintf(out,"  %9.5lf",GRPG[i][j]);
	}//end for
	////////////////////////////////////////////////
	
	fdrawline(out,kk);
	return;
}

void sens_r_cal(long prnt)
{
        if(prnt==YES) printf("\n\nsens_r_cal()...");

        double PL0;
        double *QB0,*QG0,*Vm0;
        ///////////////////////////////////////////

        //1: memory allocation

        QB0=(double *)calloc(lineNum+1,sizeof(double));
        QG0=(double *)calloc(genrNum+1,sizeof(double));
        Vm0=(double *)calloc(voltNum+1,sizeof(double));

        if(Vm0==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }
        ///////////////////////////////////////////////////

        //2: saving crr. powerflow

        saveFlowSolution(QB0,lineNum,flowData);
        saveFlowSolution(QG0,genrNum,genrData);
        saveFlowSolution(Vm0,voltNum,voltData);

        PL0=sysInfo.sysLoss;
        if(prnt==YES) printf("\n powerflow saved");
        ///////////////////////////////////////////////////

        //3: reactive sens. calculation

        sens_BGVKT(QB0,QG0,Vm0,PL0,xfmrData,NO);//prnt
        if(prnt==YES) printf("\n sens_BGVKT() ok");

        sens_BGVVG(QB0,QG0,Vm0,PL0,genrData,NO);
        if(prnt==YES) printf("\n sens_BGVVG() ok");

        sens_BGVQC(QB0,QG0,Vm0,PL0,capaData,NO);
        if(prnt==YES) printf("\n sens_BGVQC() ok");
        //////////////////////////////////////////////////////

        //4: free memory

        free(QB0);
        free(QG0);
        free(Vm0);
        //////////////////////

        return;
}

void OPF_report_sens(char *snsfile,long prnt)
{
	if(prnt==YES) printf("\nOPF_sens_report()...");

	//1: opening file

        FILE *out=fopen(snsfile,"w");
        if(out==NULL){ printf("\n\nCannot open %s",snsfile); exit(0); }

	if(prnt==YES) printf("\n %s opened",snsfile);
	////////////////////////////////////////////////////

	//2: active sensitivities

	printVector(out,PLPG,genrNum,"PLPG");
        printSparMatrix(out,GRPG,groupNum,genrNum,"GRPG");
	printSparMatrix(out,PBPG,lineNum,genrNum,"PBPG");
	
	if(prnt==YES) printf("\n active sens. ok");
	////////////////////////////////////////////////////

	//3: reactive sens.

        printVector(out,PLKT,xfmrNum,"PLKT");
        printVector(out,PLVG,genrNum,"PLVG");
        printVector(out,PLQC,capaNum,"PLQC");
        
	printSparMatrix(out,QBKT,lineNum,xfmrNum,"QBKT");
        printSparMatrix(out,QBVG,lineNum,genrNum,"QBVG");
        printSparMatrix(out,QBQC,lineNum,capaNum,"QBQC");
        printSparMatrix(out,QGKT,genrNum,xfmrNum,"QGKT");
        printSparMatrix(out,QGVG,genrNum,genrNum,"QGVG");
        printSparMatrix(out,QGQC,genrNum,capaNum,"QGQC");
        printSparMatrix(out,VmKT,voltNum,xfmrNum,"VmKT");
        printSparMatrix(out,VmVG,voltNum,genrNum,"VmVG");
        printSparMatrix(out,VmQC,voltNum,capaNum,"VmQC");

	if(prnt==YES) printf("\n reactive sens. ok");
	/////////////////////////////////////////////////////
 
	//4: closing file

        fclose(out);
        if(prnt==YES) printf("\n %s closed",snsfile);
	/////////////////////////////////////////////////

	return;
}
 
void sens_w_cal(long prnt)
{
        //1: locals and memory allocation

        if(prnt==YES) printf("\n\nsens_w_cal()...");

        long *tempList;
        double *PLPQ,*_DP,**DP,*_J2,**J2;

        long nw=nodeNum-1;//No Change!
		////////////////////////////////////////////////

        makeMatrix(_DP,DP,lineNum+1,nw+1);
        makeMatrix(_J2,J2,2*nodeNum+1,2*nodeNum+1);

        PLPQ=(double *)calloc(2*nodeNum+1,sizeof(double));
        tempList=(long *)calloc(nw+1,sizeof(long));

        if(tempList==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }
        ////////////////////////////////////////////////////////

		//2: preparation for sens.calculation

        //2.1 trans. Jacobi --- J2[][]

        Jacobi(J2,G,B,vv,v0);
        transMatrix(J2,2*nodeNum);//No Change!

        if(prnt==YES) printf("\n Jacobi() ok");
        //////////////////////////////////////////

        //2.2 forming tempList[]

        formTempList(tempList,codeList,codeAttr,SKIP_SLCK);
        if(prnt==YES) printf("\n formTempList() ok");
        //////////////////////////////////////////////////

        //3: cal. loss sens. --- PLPG[]

        sens_PLPQ(PLPQ,J2,codeList[0],NO);//prnt
        if(prnt==YES) printf("\n sens_PLPQ() ok");

        sens_PLPG(PLPG,PLPQ,codeList);
        if(prnt==YES) printf("\n sens_PLPG() ok");
        ///////////////////////////////////////////////////

        //4:  cal. branch sens. calculation
		
		//4.1 transfer coef. --- DP[][]

        DPfun(DP,tempList,nw,NO);//prnt
        if(prnt==YES) printf("\n DPfun() ok");
        //////////////////////////////////////////////////

        //4.2 branch-un sens. --- PBPG[][]

        sens_PBPG(PBPG,DP,tempList,nw);
        if(prnt==YES) printf("\n sens_PBPG() ok");

		//4.3 branch-ld sens. --- PBPD[][]

		sens_PBPD(PBPD,DP,tempList,nw);
		if(prnt==YES) printf("\n sens_PBPD() ok");
        ///////////////////////////////////////////////////

        //5: group sens. calculation

		//5.1 group-un sens. GRPG[][]

        branch_comp(GRPG,PBPG,groupNum,genrNum);
        if(prnt==YES) printf("\n GRPG[][] obtained");

		//5.2 group-ld sens. GRPD[][]

		branch_comp(GRPD,PBPD,groupNum,loadNum);
		if(prnt==YES) printf("\n GRPD[][] obtained");
        ///////////////////////////////////////////////////

        //6: free memory

        free(tempList);
        free(PLPQ);
        freeMatrix(_DP,DP);
        freeMatrix(_J2,J2);
        /////////////////////////////////

        return;
}

void saveFlowSolution(double *v,long lineNum,struct flowstr *flowData)
{
	register long i;
	struct flowstr *fp;
	////////////////////////////

	fp=flowData;
	for(i=1;i<=lineNum;i++,fp=fp->next) v[i]=fp->r1;

	return;
}

void saveFlowSolution(double *v,long genrNum,struct genrstr *genrData)
{
	register long i;
	struct genrstr *gp;
	////////////////////////////

	gp=genrData;
	for(i=1;i<=genrNum;i++,gp=gp->next) v[i]=gp->r;

	return;
}

void saveFlowSolution(double *v,long voltNum,struct voltstr *voltData)
{
	register long i;
	struct voltstr *vp;
	////////////////////////////

	vp=voltData;
	for(i=1;i<=voltNum;i++,vp=vp->next) v[i]=vp->V;

	return;
}

void sens_BGVKT(double *QB0,double *QG0,double *Vm0,double PL0,struct xfmrstr *xfmrData,long prnt)
{
//Notation:////////////////////////////////////////////////////////////
// QB0[]    --- original branch reactive, lineNum*1
// QG0[]    --- original genr. reactive, genrNum*1
// Vm0[]    --- original nodal voltage, nodeNum*1
// PL0      --- original active loss
// xfmrData --- xfmr.data chain, original datachain
/////////////////////////////////////////////////////////////////////

	if(prnt==YES) printf("\n\nsens_BGVKT()...");

	//1: declaration and memory allocation

	register long i,j;
	
	double aux2,inc,flow_eps_sv;
	double *vv_sv,*v0_sv;

	struct flowstr *fp;
	struct genrstr *gp;
	struct voltstr *vp;
	struct xfmrstr *xp;

	vv_sv=(double *)calloc(nodeNum+1,sizeof(double));
	v0_sv=(double *)calloc(nodeNum+1,sizeof(double));
	
	if(v0_sv==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }
	////////////////////////////////////////////////////

	//2: preparation for iteration

	copyVector(vv_sv,vv,nodeNum);//original saved
	copyVector(v0_sv,v0,nodeNum);

	initMatrix(QBKT,lineNum,xfmrNum,0.0);
	initMatrix(QGKT,genrNum,xfmrNum,0.0);
	initMatrix(VmKT,voltNum,xfmrNum,0.0);
	initVector(PLKT,xfmrNum,0.0);

	flow_eps_sv=flow_eps;//reserved
	flow_eps*= 5.0;
	////////////////////////////////////////////////

	//3: sens. iteration --- xfmr
	
	xp=xfmrData;
	for(j=1;j<=xfmrNum;j++,xp=xp->next)
	{
		if(prnt==YES) printf("\n j=%2ld, xfmr_id=%s",j,xp->id);

		//3.1 regain original state

		initCtrlData(xfmrData);
		initCtrlData(genrData);
		initCtrlData(capaData);

		copyVector(vv,vv_sv,nodeNum);
		copyVector(v0,v0_sv,nodeNum);
		//initVolt(vv,v0,codeList,NO);//prnt
		///////////////////////////////////////////

		//3.2 pert. powerflow
		
		inc=xp->K0*PERT_COEFF;
		if(fabs(inc)<SMLL){ printf("\nError! --- %ld",__LINE__); exit(0); }

		xp->K=xp->K0+inc;
		pwrflow(MODVAR,NO);//prnt
		
		genrReactiv(genrData,nodeQ,codeList,nodeNum);
		monVoltage(voltData,vv,codeList,nodeNum);
		//////////////////////////////////////////////////

		//3.3 sens. calculation

		//3.3.1 QBKT[][]

		fp=flowData;
		for(i=1;i<=lineNum;i++,fp=fp->next)
		{
			aux2=fp->r1-QB0[i];
			QBKT[i][j]=aux2/inc;
		}//end for
		//////////////////////////////////////

		//3.3.2 QGKT[][]

		gp=genrData;
		for(i=1;i<=genrNum;i++,gp=gp->next)
		{
			aux2=gp->r-QG0[i];
			QGKT[i][j]=aux2/inc;
		}//end for
		//////////////////////////////////////

		//3.3.3 VmKT[][]

		vp=voltData;
		for(i=1;i<=voltNum;i++,vp=vp->next)
		{
			aux2=vp->V-Vm0[i];
			VmKT[i][j]=aux2/inc;			
		}//end for
		//////////////////////////////////

		//3.3.4 PLKT[]

		aux2=sysInfo.sysLoss-PL0;
		PLKT[j]=aux2/inc;
	}//end for
	/////////////////////////////////////

	//4: restoring externals

	copyVector(vv,vv_sv,nodeNum);
	copyVector(v0,v0_sv,nodeNum);

	flow_eps=flow_eps_sv;
	////////////////////////////////////

	//5: free momery and return

	free(vv_sv);
	free(v0_sv);
	///////////////////////////////

	return;
}

void sens_BGVVG(double *QB0,double *QG0,double *Vm0,double PL0,struct genrstr *genrData,long prnt)
{
//Notation:////////////////////////////////////////////////////////////
// QB0[]    --- original branch reactive, lineNum*1
// QG0[]    --- original genr. reactive, genrNum*1
// Vm0[]    --- original nodal voltage, nodeNum*1
// PL0      --- original active loss
// genrData --- genr.data chain, original datachain
/////////////////////////////////////////////////////////////////////

	if(prnt==YES) printf("\n\nsens_BGVVG()...");

	//1: declaration and memory allocation

	register long i,j;
	
	double aux2,inc,flow_eps_sv;
	double *vv_sv,*v0_sv;

	struct flowstr *fp;
	struct genrstr *gp,*gp2;
	struct voltstr *vp;
	
	vv_sv=(double *)calloc(nodeNum+1,sizeof(double));
	v0_sv=(double *)calloc(nodeNum+1,sizeof(double));

	if(v0_sv==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }
	////////////////////////////////////////////////////////

	//2: preparation for iteration

	copyVector(vv_sv,vv,nodeNum);//original saved
	copyVector(v0_sv,v0,nodeNum);

	initMatrix(QBVG,lineNum,genrNum,0.0);
	initMatrix(QGVG,genrNum,genrNum,0.0);
	initMatrix(VmVG,voltNum,genrNum,0.0);
	initVector(PLVG,genrNum,0.0);

	flow_eps_sv=flow_eps;
	flow_eps*= 5.0;
	/////////////////////////////////////////////////

	//3: sens. iteration --- genr.voltage

	gp=genrData;
	for(j=1;j<=genrNum;j++,gp=gp->next)
	{
		if(prnt==YES) printf("\n j=%2ld, genr_id=%s",j,gp->id);

		//3.1 regain original state

		initCtrlData(xfmrData);
		initCtrlData(genrData);
		initCtrlData(capaData);

		copyVector(vv,vv_sv,nodeNum);
		copyVector(v0,v0_sv,nodeNum);
		//initVolt(vv,v0,codeList,NO);//prnt
		///////////////////////////////////////////////

		//3.2 pert. powerflow

		inc=gp->v0*PERT_COEFF;
		if(fabs(inc)<SMLL){ printf("\n\nError! --- %ld",__LINE__); exit(0); }

		gp->v=gp->v0+inc;
		pwrflow(MODVAR,NO);//prnt
		
		genrReactiv(genrData,nodeQ,codeList,nodeNum);
		monVoltage(voltData,vv,codeList,nodeNum);
		//////////////////////////////////////////////////

		//3.3 sens. calculation

		//3.3.1 QBVG[][]

		fp=flowData;
		for(i=1;i<=lineNum;i++,fp=fp->next)
		{
			aux2=fp->r1-QB0[i];
			QBVG[i][j]=aux2/inc;
		}//end for
		//////////////////////////////////////

		//3.3.2 QGVG[][]

		gp2=genrData;
		for(i=1;i<=genrNum;i++,gp2=gp2->next)
		{
			aux2=gp2->r-QG0[i];
			QGVG[i][j]=aux2/inc;
		}//end for
		//////////////////////////////////////

		//3.3.3 VmVG[][]

		vp=voltData;
		for(i=1;i<=voltNum;i++,vp=vp->next)
		{
			aux2=vp->V-Vm0[i];
			VmVG[i][j]=aux2/inc;
		}//end for
		//////////////////////////////////

		//3.3.4 PLVG[]

		aux2=sysInfo.sysLoss-PL0;
		PLVG[j]=aux2/inc;
	}//end for
	/////////////////////////////////////

	//4: restoring externals

	copyVector(vv,vv_sv,nodeNum);
	copyVector(v0,v0_sv,nodeNum);
	
	flow_eps=flow_eps_sv;
	///////////////////////////////////////

	//5: free momery and return
	
	free(vv_sv);
	free(v0_sv);
	///////////////////////////////////

	return;
}

void sens_BGVQC(double *QB0,double *QG0,double *Vm0,double PL0,struct capastr *capaData,long prnt)
{
//Notation:////////////////////////////////////////////////////////////
// QB0[]    --- original branch reactive, lineNum*1
// QG0[]    --- original genr. reactive, genrNum*1
// Vm0[]    --- original nodal voltage, nodeNum*1
// PL0      --- original active loss
// capaData --- capa.data chain, original datachain
/////////////////////////////////////////////////////////////////////

	if(prnt==YES) printf("\n\nsens_BGVQC()...");

	//1: declaration and memory allocation

	register long i,j;

	double aux2,inc,flow_eps_sv;
	double *vv_sv,*v0_sv;
	
	struct capastr *cp;
	struct flowstr *fp;
	struct genrstr *gp;
	struct voltstr *vp;
	
	vv_sv=(double *)calloc(nodeNum+1,sizeof(double));
	v0_sv=(double *)calloc(nodeNum+1,sizeof(double));

	if(v0_sv==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }
	//////////////////////////////////////////////

	//2: preparation for iteration

	copyVector(vv_sv,vv,nodeNum);//original saved
	copyVector(v0_sv,v0,nodeNum);

	initMatrix(QBQC,lineNum,capaNum,0.0);
	initMatrix(QGQC,capaNum,capaNum,0.0);
	initMatrix(VmQC,voltNum,capaNum,0.0);
	initVector(PLQC,capaNum,0.0);

	flow_eps_sv=flow_eps;
	flow_eps*= 5.0;
	/////////////////////////////////////////////////

	//3: sens. iteration --- capa 

	cp=capaData;
	for(j=1;j<=capaNum;j++,cp=cp->next)
	{
		if(prnt==YES) printf("\n j=%2ld, capa_id=%s",j,cp->id);

		//3.1 regain original state

		initCtrlData(xfmrData);
		initCtrlData(genrData);
		initCtrlData(capaData);

		copyVector(vv,vv_sv,nodeNum);
		copyVector(v0,v0_sv,nodeNum);
		//initVolt(vv,v0,codeList,NO);//prnt
		/////////////////////////////////////////

		//3.2 pert. powerflow

		inc=cp->Q0*PERT_COEFF;
		if(fabs(inc)<SMLL){ printf("\n\nError! --- %ld",__LINE__); exit(0); }

		cp->Q=cp->Q0+inc;
		pwrflow(MODVAR,NO);//prnt
		
		genrReactiv(genrData,nodeQ,codeList,nodeNum);
		monVoltage(voltData,vv,codeList,nodeNum);
		//////////////////////////////////////////////////

		//3.3 sens. calculation

		//3.3.1 QBQC[][]

		fp=flowData;
		for(i=1;i<=lineNum;i++,fp=fp->next)
		{
			aux2=fp->r1-QB0[i];
			QBQC[i][j]=aux2/inc;
		}//end for
		//////////////////////////////////////

		//3.3.2 QGQC[][]

		gp=genrData;
		for(i=1;i<=capaNum;i++,gp=gp->next)
		{
			aux2=gp->r-QG0[i];
			QGQC[i][j]=aux2/inc;
		}//end for
		//////////////////////////////////////

		//3.3.3 VmQC[][]

		vp=voltData;
		for(i=1;i<=voltNum;i++,vp=vp->next)
		{
			aux2=vp->V-Vm0[i];
			VmQC[i][j]=aux2/inc;
		}//end for
		//////////////////////////////////

		//3.3.4 PLQC[]

		aux2=sysInfo.sysLoss-PL0;
		PLQC[j]=aux2/inc;
	}//end for
	/////////////////////////////////////

	//4: restoring externals

	copyVector(vv,vv_sv,nodeNum);
	copyVector(v0,v0_sv,nodeNum);

	flow_eps=flow_eps_sv;
	///////////////////////////////////

	//5: free momery and return

	free(vv_sv);
	free(v0_sv);
	////////////////////////////
	
	return;
}

void branch_comp(double **group_sens,double **branch_sens,long groupNum,long ctrlNum)
{
//Notes: by Wang GT
// group_sens[][]  --- groupNum * ctrlNum size
// branch_sens[][] --- lineNum * ctrlNum size
// groupNum --- num.of groups
// ctrlNum  --- num.of ctrls, e.g. genr.
/////////////////////////////////////////////////////////////

	//1: locals and memory allocation

	register long i,k;
	long ik,imst,ibs,zbs;

	struct groupstr *grp;
	struct linestr *lp;

	double *row=(double *)calloc(ctrlNum+1,sizeof(double));
	if(row==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }
	////////////////////////////////////////////////////

	//2: init. group_sens as zeros

	initMatrix(group_sens,groupNum,ctrlNum,0.0);

	//3: branch composition

	grp=groupData;
	for(i=1;i<=groupNum;i++,grp=grp->next)
	{
		for(k=1;k<=grp->lineNum;k++)
		{
			//.1 skipping invalid lines

			ibs=grp->line[k].i;
			zbs=grp->line[k].j;

			lp=findData(ibs,zbs,lineData);
			if(lp==NULL) continue;
			/////////////////////////////////////////////

			//.2 getting branch sens.

			ik=posChainNode(lineData,lp);
			if(ik==NO){ printf("\nError! --- %ld",__LINE__); exit(0); }

			copyVectorRow(row,branch_sens,ik,ctrlNum);
			////////////////////////////////////////////

			//.3 sum. --> group sens.
			
			imst=grp->line[k].imst;
			if(imst==abs(lp->j)) mulVector(row,ctrlNum,-1.0);
		
			addMatrixRow(group_sens,row,i,ctrlNum);			
		}//end for
	}//end for
	//////////////////////////////////////////////

	//4: free memory
	
	free(row);
	return;
}

void Jacobi(double **J,double **G,double **B,double *vv,double *v0)
{
        register long i,j;
        double Pi,Qi,theta;
        /////////////////////////////

        //1: J11[][] --- H[][]

        for(i=1;i<=nodeNum;i++)
        {
                Qi=calQi(G,B,vv,v0,i);
                J[i][i]=vv[i]*vv[i]*B[i][i]+Qi;
        }//end for

        for(i=1;i<=nodeNum;i++)
        for(j=1;j<=nodeNum;j++)
        {
                if(j==i) continue;

                theta=v0[i]-v0[j];
                J[i][j]=-vv[i]*vv[j]*(G[i][j]*sin(theta)-B[i][j]*cos(theta));
        }//end for
        /////////////////////////////////////////////////////////////////

        //2: J12[][] --- N[][]

        for(i=1;i<=nodeNum;i++)
        {
                Pi=calPi(G,B,vv,v0,i);
                J[i][nodeNum+i]=-(vv[i]*vv[i]*G[i][i]+Pi)/vv[i];
        }//end for
                                                
        for(i=1;i<=nodeNum;i++)
        for(j=1;j<=nodeNum;j++)
        {
                if(j==i) continue;

                theta=v0[i]-v0[j];
                J[i][nodeNum+j]=-vv[i]*(G[i][j]*cos(theta)+B[i][j]*sin(theta));
        }//end for
        ///////////////////////////////////////////////

        //3: J21[][] --- K[][]

        for(i=1;i<=nodeNum;i++)
        {
                Pi=calPi(G,B,vv,v0,i);
                J[nodeNum+i][i]=vv[i]*vv[i]*G[i][i]-Pi;
        }//end for

        for(i=1;i<=nodeNum;i++)
        for(j=1;j<=nodeNum;j++)
        {
                if(j==i) continue;

                theta=v0[i]-v0[j];
                J[nodeNum+i][j]=vv[i]*vv[j]*(G[i][j]*cos(theta)+B[i][j]*sin(theta));
        }//end for
        ////////////////////////////////////////////////////////

        //4: J22[][] --- L[][]

        for(i=1;i<=nodeNum;i++)
        {
                Qi=calQi(G,B,vv,v0,i);
                J[i+nodeNum][i+nodeNum]=vv[i]*B[i][i]-Qi/vv[i];
        }//end for

        for(i=1;i<=nodeNum;i++)
        for(j=1;j<=nodeNum;j++)
        {
                if(j==i) continue;

                theta=v0[i]-v0[j];
                J[i+nodeNum][j+nodeNum]=-vv[i]*(G[i][j]*sin(theta)-B[i][j]*cos(theta));
        }//end for
        ///////////////////////////////////////////////////////

        //5: Negative Sign
        negaMatrix(J,2*nodeNum,2*nodeNum);//No Change!

        return;
}

void HPfun(double **HP,long *tempList,long n)
{
        register long i,ii,jj,i2,j2;
        struct linestr *lp;
        ////////////////////////////////

        initMatrix(HP,lineNum,n,0.0);
        lp=lineData;

        for(i=1;i<=lineNum;i++,lp=lp->next)
        {
                ii=lookup(tempList,lp->i,n);
                jj=lookup(tempList,lp->j,n);

                i2=lookup(codeList,lp->i,nodeNum);
                j2=lookup(codeList,lp->j,nodeNum);
                if(i2==NO || j2==NO){ printf("\nError! --- %ld",__LINE__); exit(0); }
                ////////////////////////////////////////

                if(ii!=NO) HP[i][ii]=B[i2][j2];
                if(jj!=NO) HP[i][jj]=-B[i2][j2];
        }//end for
        //////////////////////////////

        return;
}

void sens_PLPG(double *PLPG,double *PLPQ,long *codeList)
{
        register long i,ii;
        struct genrstr *gp;
		/////////////////////////////

        gp=genrData;
        for(i=1;i<=genrNum;i++,gp=gp->next)
        {
                ii=lookup(codeList,gp->i,nodeNum);
                if(ii==NO){ printf("\nError! --- %ld",__LINE__); exit(0); }

                PLPG[i]=PLPQ[ii];
        }//end for
        /////////////////////////////

        return;
}

void sens_PLPQ(double *PLPQ,double **J2,long slack,long prnt)
{
//NOTATION: This is a very important function! //////////////////
//      PLPQ[]  --- loss sens. vs nodal active/reactive injection,
//                  2*nodeNum dimension with slack included
//      J2[][]  --- transposed Jacobi matrix, 2*nodeNum square
//      slack   --- slack node
/////////////////////////////////////////////////////////

        if(prnt==YES) printf("\n\nsens_PLPQ()...");

        register long i,j;
        long k,k2,n=nodeNum-1;//init.

        double sum;
        double *PL0V,*_J2s,**J2s;

        makeMatrix(_J2s,J2s,2*nodeNum+1,2*nodeNum+1);
        PL0V=(double *)calloc(2*n+1,sizeof(double));

        if(PL0V==NULL){ printf("\n\nError! --- %ld",__LINE__); exit(0); }
        /////////////////////////////////////////////

        copyMatrix(J2s,J2,2*nodeNum,2*nodeNum);//reserved
        if(prnt==YES) printf("\n J2[][] reserved");

        //1: reducing rows of J2[][]

        k=lookup(codeList,slack,nodeNum);
        k2=k+nodeNum-1;//No Change!
        if(k==NO){ printf("\nError! --- %ld",__LINE__); exit(0); }

        delMatrixRow(J2,k,2*nodeNum,2*nodeNum);
        delMatrixRow(J2,k2,2*nodeNum-1,2*nodeNum);
	
        if(prnt==YES) printf("\n J2[][] reduced by rows");
        ///////////////////////////////////////////////

        //2: form PL0V[]

        for(i=1;i<=2*n;i++)
        {
                sum=0.0;
                for(j=1;j<=nodeNum;j++) //see 5-92 at p.81
                        sum+= J2[i][j];

                PL0V[i]=sum;
        }//end for

        if(prnt==YES) printf("\n PL0V[] formed");
        ///////////////////////////////////////////

        //3: reducing columns of J2[][]

        delMatrixCol(J2,k,2*n,2*nodeNum);
        delMatrixCol(J2,k2,2*n,2*nodeNum-1);
 
        if(prnt==YES) printf("\n J2[][] reduced by columns");
       	////////////////////////////////////////////

        //4: solving and settlement

        gaussPP(PLPQ,J2,PL0V,2*n);//quasi-PLPQ[] obtained
        if(prnt==YES) printf("\n gaussPP() ok");

        insertVectorElm(PLPQ,0.0,k,2*nodeNum);
        insertVectorElm(PLPQ,0.0,k+nodeNum,2*nodeNum);//completed
 
       	if(prnt==YES) printf("\n PLPQ[] completed");
        //////////////////////////////////////////////////

        //5: solution saving and free memory

        copyMatrix(J2,J2s,2*nodeNum,2*nodeNum);//restored
        if(prnt==YES) printf("\n J2[][] restored");

        freeMatrix(_J2s,J2s);
        free(PL0V);
        ////////////////////////////////////////

        return;
}

void DPfun(double **DP,long *tempList,long n,long prnt)
{
//NOTATION:///////////////////////////////////////////////////////////////////
//  DP[][]     --- branch power vs nodal injection, lineNum * n dimension
//  tempList[] --- node list only without slack, n dimension
//  n          --- dim.of tempList[], should = node-1
//  prnt       --- print switch
//////////////////////////////////////////////////////////////////////////

        if(prnt==YES) printf("\n\nDPfun()...");

        //1: declaration and memory allocation 

        double *_HP,**HP;
        double *_B1s,**B1s;//local B1[][] for sens.

        makeMatrix(_HP,HP,lineNum+1,n+1);
        makeMatrix(_B1s,B1s,nodeNum,nodeNum);
        ////////////////////////////////////////////

        //2: HP[][] and B1s[][]

        HPfun(HP,tempList,n);
        if(prnt==YES) printf("\n HP[][] formed");

        formB1(B1s,tempList,n);
        if(prnt==YES) printf("\n B1s[][] formed");
        /////////////////////////////////////////////

        //3: inv-B1s[][] and DP[][]

        invMatrixGJ(B1s,n);
        if(prnt==YES) printf("\n inv-B1s[][] formed");

        mulMatrix(DP,HP,B1s,lineNum,n,n);
        if(prnt==YES) printf("\n DP[][] formed");
       	///////////////////////////////////////////////

        //4: free memory

        freeMatrix(_HP,HP);
        freeMatrix(_B1s,B1s);
        /////////////////////////////

        return;
}

void sens_PBPG(double **PBPG,double **DP,long *tempList,long n)
{
        register long j;
        long k;
		
		struct genrstr *gp;
        ////////////////////////////////////////

		initMatrix(PBPG,lineNum,genrNum,0.0);

		gp=genrData;
		for(j=1;j<=genrNum;j++,gp=gp->next)
		{
			k=lookup(tempList,gp->i,n);
			if(k!=NO) copyMatrixCol(PBPG,DP,j,k,lineNum);
        }//end for
        ///////////////////////////////////

        return;
}

void sens_PBPD(double **PBPD,double **DP,long *tempList,long n)
{
        register long j;
        long k;
		
		struct loadstr *ldp;
        ////////////////////////////////////////

		initMatrix(PBPD,lineNum,loadNum,0.0);

		ldp=loadData;
		for(j=1;j<=loadNum;j++,ldp=ldp->next)
		{
			k=lookup(tempList,ldp->i,n);
			if(k!=NO)
			{
				copyMatrixCol(PBPD,DP,j,k,lineNum);
				mulMatrixCol(PBPD,j,lineNum,-1.0);
			}
        }//end for
        ///////////////////////////////////

        return;
}

///////////////////////////////////
//end of file
