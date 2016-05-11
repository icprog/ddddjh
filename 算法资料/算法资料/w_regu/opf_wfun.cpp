//opf_wfun.cpp by Wang GT
///////////////////////////////////

#include"blp.h"
#include"common.h"
#include"opf.h"
#include"opf_global.h"
///////////////////////////////////////////////////

long vio_check(struct itrstr *itrData,double *crrgen,long genrNum,long itr,long prnt)
{
        if(prnt==YES) printf("\n\nvio_check()...");

	long imin,vioNum,minVioNum;
	double vioPct,purcost,minVioPct,minPurcost;

	struct itrstr *ip,*itp;
	///////////////////////////////////////////////////////////

	//1: imin, minVioNum/Pct --- historical

	imin=NO;//init.

        minVioNum=BIGNUM;
        minVioPct=LARG;

        itp=tailNode(itrData);
        if(itp!=NULL)
        { 
                ip=findData(itp->imin,itrData);
                if(ip==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }

                imin=ip->imin;
		minPurcost=ip->purcost;

		minVioNum=ip->vioNum;
                minVioPct=ip->vioPct;
        }//end if
	
	if(prnt==YES) printf("\n minVioNum=%ld",minVioNum);
	if(prnt==YES) printf("\n minVioPct=%.2lf",minVioPct);
	////////////////////////////////////////////////////////////
	
	//2: vioNum, vioPct, purcost, etc. --- this itr.

	purcost = sysPurcost(crrgen,genrNum);

        vioNum  = vioNumStat(flowData)+vioNumStat(groupData);
        vioPct  = vioPctStat(flowData)+vioPctStat(groupData);

        if(prnt==YES) printf("\n  vioNum = %ld",vioNum);
        if(prnt==YES) printf("\n  vioPct = %lf",vioPct);
        if(prnt==YES) printf("\n  purcost= %lf",purcost);
	///////////////////////////////////////////////////////////

	//2.2 cal. imin --- index to optimal itr.

	if(vioNum<minVioNum) imin=itr;
        if(vioNum==minVioNum)
        {
                if(vioPct<minVioPct) imin=itr;
                if(fabs(vioPct-minVioPct)<SMLL && purcost<minPurcost) imin=itr;
        }//end if

	if(prnt==YES) printf("\n imin=%ld",imin);
	///////////////////////////////////////////////////////////

        //3: saving itr. infomation

        ip=(struct itrstr *)malloc(sizeof(struct itrstr));
        if(ip==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }

        ip->i    = itr;
	ip->imin = imin;

        ip->purcost=purcost;
        ip->vioNum =vioNum;
        ip->vioPct =vioPct;

        appendNode(itrData,ip);
 	///////////////////////////////////////////////////////////
	
	if(vioNum==0) return NO;//No violation 
	return YES;
}

void printUsage()
{
	printf("\nUsage: w_regu date_str sd");
	printf("\n  date_str --- date string in format of mm/dd/yyyy");
	printf("\n  sd       --- period of time: 1, 2, ... 96\n");

	return;
}

long step_check(char *resfile,char *waflg,double *currInc,double *prevInc,
				 double maxstep,double eps,double div_coeff,long genrNum,long prnt)
{
	if(prnt==YES) printf("\n\nstep_report()...");

	long istep,hbFlg,ret=NO;//init.
	double step;

	FILE *out=fopen(resfile,waflg);
	if(out==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }
	///////////////////////////////////////////////////

	//1: step confining and reduction

	stepConfin(currInc,genrNum,maxstep,div_coeff);
	if(prnt==YES) printf("\n stepConfin() ok");

	//2: when hang-back

	hbFlg=hangback(prevInc,currInc,genrNum);
	if(hbFlg==YES) stepReduce(currInc,genrNum,div_coeff);

	if(prnt==YES) printf("\n hangback() ok");
	////////////////////////////////////////////////////////

	//3: step-eps check

	istep=posAbsmax(currInc,genrNum);
	step=currInc[istep];

	if(prnt==YES) printf("\n step=%9.5lf",step);
	fprintf(out,"\n step=%9.5lf",step);

	if(fabs(step)<=eps) ret=YES;
	////////////////////////////////////////////////

	fclose(out);//resfile closed
	return ret;
}

long OPF_w_blpfun(double *currInc,double *crrgen,long genrNum,long prnt)
{ 
	if(prnt==YES) printf("\n\nOPF_w_blpfun()...");

	//1: obj.function linearization

	long n=chainLen(priceData);

	if(n==0) objLnz_wcorr(d,crrgen,genrNum,NO);//prnt	
	if(n>0)  objLnz_w(d,crrgen,genrNum,NO);//prnt

	if(prnt==YES) printf("\n objLnz_w() ok");
	/////////////////////////////////////////////////

	//2: cal. constraints

	LBfun_w(LBmin,LBmax,flowData,vv,codeList);
	if(prnt==YES) printf("\n LBfun_w() ok");
	
	LGfun_w(LGmin,LGmax,groupData);
	if(prnt==YES) printf("\n LGfun_w() ok");
	////////////////////////////////////////////////

	//3: cal. bounds

	limFun_w(Lmin_w,Lmax_w,crrgen);
	if(prnt==YES) printf("\n limFun_w() ok");
	//////////////////////////////////////////////

	//4: solve BLP problem

	long ret=formLP_w(currInc,d,beta_w,PBPG,LBmin,LBmax,GRPG,LGmin,LGmax,Lmin_w,Lmax_w,flowData,NO);//prnt
	if(prnt==YES) printf("\n formLP_w() returned %ld",ret);

	return ret;
}

void OPF_opt_pwrflow(char *resfile,double *crrgen,long genrNum,long prnt)
{
	if(prnt==YES) printf("\n\nOPF_opt_pwrflow()...");

	//1: updating genrData

	updateData(genrData,crrgen,genrNum);
	if(prnt==YES) printf("\n updateData() ok");
	//////////////////////////////////////////////////
	
	//2: pwrflow function and related solutions

	pwrflow(MODPG,NO);//modi,prnt
	if(prnt==YES) printf("\npwrflow() ok");

	pickCurrGenr(crrgen,nodeP,codeList);
	updateData(genrData,crrgen,genrNum);

	modiSlack(genrData,nodeP,codeList);
	monVoltage(voltData,vv,codeList,nodeNum);
	
	groupFlow(groupData,flowData,sub_flowData);
	if(prnt==YES) printf("\n groupFlow() ok");
	/////////////////////////////////////////////////////////

	//3: solution reporting

	pwrflow_report(resfile,"a",NO);//prnt
	if(prnt==YES) printf("\npwrflow_report() ok");

	reportReguPolicy(resfile,"a",genrData,NO);
	if(prnt==YES) printf("\nreportReguPolicy() ok");
	/////////////////////////////////////////////////

	return;
}

void OPF_org_pwrflow(char *resfile,long prnt)
{
		if(prnt==YES) printf("\n\nOPF_org_pwrflow()...");

		//1: pwrflow calculation

        pwrflow_init(NO);//prnt
        if(prnt==YES) printf("\npwrflow_init() ok");

        pwrflow(MODNO,YES);//prnt
        if(prnt==YES) printf("\npwrflow() ok");
        ////////////////////////////////////////////

		//2: related solution

        modiSlack(genrData,nodeP,codeList);
        if(prnt==YES) printf("\n modiSlack() ok");

        groupModeAnalysis(groupData,flowData,sub_flowData);
        if(prnt==YES) printf("\ngroupModeAnalysis() ok");

        groupFlow(groupData,flowData,sub_flowData);
        if(prnt==YES) printf("\ngroupFlow() ok");
        ///////////////////////////////////////////////
       
        //3: pwrflow report

		pwrflow_report(resfile,"w",NO);//prnt
		if(prnt==YES) printf("\npwrflow_report() ok");
        ///////////////////////////////////////////////////

        //4: saving original solution

        copyChain(flowData0,flowData);
        copyChain(sub_flowData0,sub_flowData);

        sysInfo0=sysInfo;
        if(prnt==YES) printf("\nsysInfo0 obtained");
		//////////////////////////////////////////////////
		
		return;
}

void OPF_w_sens(char *snsfile,long prnt)
{
	readSparMatrix(snsfile,"GRPG",GRPG,groupNum,genrNum);
	readSparMatrix(snsfile,"PBPG",PBPG,lineNum,genrNum);
	
	readVector(snsfile,"PLPG",PLPG,genrNum);
	beta_wfun(beta_w,PLPG,genrNum);
	/////////////////////////////////////////////
	
	return;
}

void pwrflow_update(double *currInc,long genrNum,long prnt)
{
	if(prnt==YES) printf("\n\npwrflow_update()...");

	//1: getting branch inc.

	getBranInc(branInc,currInc,PBPG);
	if(prnt==YES) printf("\n getBranInc() ok");
	////////////////////////////////////////////////

	//2: modifying flowData

	modiFlowData(flowData,branInc);
	if(prnt==YES) printf("\n modiFlowData() ok");

	//3: modifying voltage angle

	modiBusAngl(v0,currInc);
	if(prnt==YES) printf("\n powerflow modified");
	///////////////////////////////////////////////////

	//4: modifying groupData
	
	groupFlow(groupData,flowData,sub_flowData);
	printf("\n groupFlow() ok");
	//////////////////////////////////////////////////

	return;
}

void synchoff_init(struct genrstr *genrData)
{
	struct genrstr *gp=genrData;
	while(gp!=NULL)
	{
		gp->synFlg=NO;//init.
		gp->offFlg=NO;

		gp=gp->next;
	}//end while
	////////////////////////////////

	return;
}

void synchoff_adj(struct genrstr *genrData,double **GRPG,long groupNum,long genrNum,long prnt)
{
	if(prnt==YES) printf("\n\nsynchoff_adj()...");

	register long i,j;
	
	long imax,bdryFlg;
	double grpsns,vioPct,max_vioPct;
	
	struct genrstr *gp;
	struct groupstr *grp;
	/////////////////////////////////////////////////

	//1: finding vio.serious group

	max_vioPct=-LARG;
	grp=groupData;

	for(i=1;i<=groupNum;i++,grp=grp->next)
	{
		if(grp->vioFlg!=YES) continue;

		vioPct=grp->vioPct;
		if(fabs(vioPct)>fabs(max_vioPct))//No Change!
		{
			max_vioPct=vioPct;
			imax=i;
		}
	}//end for

	if(prnt==YES) printf("\n max_vioPct=lf",max_vioPct);
	/////////////////////////////////////////////////////////

	//2: marking syn/off flags

	synchoff_init(genrData);
	if(prnt==YES) printf("\n synchoff_init() ok");

	gp=genrData;
	for(j=1;j<=genrNum;j++,gp=gp->next)
	{
		//.1 skipping units

		bdryFlg=boundary_on(gp->w,gp->wmin,gp->wmax);
		if(bdryFlg==MIDDL) continue;
		/////////////////////////////////////////////
		
		//.2 getting sns. info.

		grpsns=GRPG[imax][j];
		if(fabs(grpsns)<SMLL) continue;
		/////////////////////////////////////////////
		
		//.3 upper boundary --- setting synFlg

		if(bdryFlg==UPPER)
                if(max_vioPct*grpsns<0) gp->synFlg=YES;

		//.4 lower boundary --- setting offFlg

		if(bdryFlg==LOWER)
			if(max_vioPct*grpsns>0) gp->offFlg=YES;
	}//end for

	if(prnt==YES) printf("\n syn/offFlag marked");
	/////////////////////////////////////////////////

	return;
}

void OPF_w_alloc()
{
	currInc=(double *)calloc(genrNum+1,sizeof(double));
	prevInc=(double *)calloc(genrNum+1,sizeof(double));
	
	crrgen=(double *)calloc(genrNum+1,sizeof(double));
	optgen=(double *)calloc(genrNum+1,sizeof(double));
	currPric=(double *)calloc(genrNum+1,sizeof(double));

	beta_w=(double *)calloc(genrNum+1,sizeof(double));
	d=(double *)calloc(genrNum+1,sizeof(double));

	Lmin_w=(double *)calloc(genrNum+1,sizeof(double));
	Lmax_w=(double *)calloc(genrNum+1,sizeof(double));
	/////////////////////////////////////////////////////////////

	LBmin=(double *)calloc(lineNum+1,sizeof(double));
	LBmax=(double *)calloc(lineNum+1,sizeof(double));
	LGmin=(double *)calloc(groupNum+1,sizeof(double));
	LGmax=(double *)calloc(groupNum+1,sizeof(double));

	branInc=(double *)calloc(lineNum+1,sizeof(double));
	if(branInc==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }

	return;
}

void OPF_w_free()
{
		free(branInc);
		free(currInc);
		free(prevInc);
		
		free(crrgen);
		free(optgen);

		free(beta_w);
		free(d);
		free(currPric);
		free(LBmin);
		free(LBmax);
		free(LGmin);
		free(LGmax);
		free(Lmin_w);
		free(Lmax_w);

		return;
}

void OPF_write_emslog(struct sysstr sysInfo0,struct sysstr sysInfo,struct genrstr *genrData)
{
		strcpy(resfile,"/data/users/mmi/ems_log/opf_scd.log");

		FILE *out=fopen(resfile,"w");
		if(out==NULL){ printf("\nCannot open log file!"); exit(0); }

        reportSys(out,sysInfo0,NO);
        reportSys(out,sysInfo,NO);

        fclose(out);//log closed
        ////////////////////////////////////////

        reportReguPolicy(resfile,"a",genrData,NO);
        return;
}

long kbNumStat(struct groupstr *groupData)
{
	long ret=0;//init.
	struct groupstr *grp;
	///////////////////////////////

	grp=groupData;
	while(grp!=NULL)
	{
		if(grp->kbFlg==YES) ret++;

		grp=grp->next;
	}//end while
	/////////////////////////

	return ret;
}

void appendSysInfo_w(struct sysstr &sysInfo,struct flowstr *flowData,double itrtime,double runtime)
{
        //1: declaration and allocation

        double *crrGn=(double *)calloc(genrNum+1,sizeof(double));
        if(crrGn==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }
        ////////////////////////////////////////////////////////////

        //2: ctrl. numbers

        sysInfo.genrNum   = genrNum;
        sysInfo.genw_regu = reguNum(genrData,GENR_POWR);

        sysInfo.xfmrNum   = xfmrNum;
        sysInfo.capaNum   = capaNum;
        sysInfo.xfmr_regu = 0;//stuffing
        sysInfo.genv_regu = 0;
        sysInfo.capa_regu = 0;
        /////////////////////////////////////////////////////

        //3: time and violation
                                
        sysInfo.itrtime = itrtime;
        sysInfo.runtime = runtime;
        sysInfo.vioNum  = vioNumStat(flowData)+vioNumStat(groupData);
        ////////////////////////////////////////////////

        //4: active purcost

        pickCurrGenr(crrGn,nodeP,codeList);
        sysInfo.purcost=sysPurcost(crrGn,genrNum);
        ///////////////////////////////////////////////

        //5: active offset

        sysInfo.offset=offsetFun(genrData);
        ///////////////////////////////////////

        free(crrGn);
        return;
}

double offsetFun(struct genrstr *genrData)
{
	double ret=0.0;//init.
	struct genrstr *gp;
	//////////////////////////////
	
	gp=genrData;
	while(gp!=NULL)
	{
		ret+= fabs(gp->w-gp->w0);
		gp=gp->next;
	}//end while
	///////////////////////

	return ret;
}

double purcostFun(double *crrgen,long genrNum)
{
	register long i;
	double price,ret=0.0;//init.

	struct pricestr *pp;
	//////////////////////////////////////

	pp=priceData;
	for(i=1;i<=genrNum;i++,pp=pp->next)
	{
		price=priceFun(crrgen[i],pp);
		ret+= crrgen[i]*price;
	}//end for
	//////////////////////////////

	return ret;
}

double priceFun(double wgen,struct pricestr *pp)
{
        long k;
        double x1,x2,y1,y2,p,a,b,ret;
        ///////////////////////////////////////////////

        //1: finding segment

        p=wgen/pp->Pb;
        k=findk_ex(p,pp);

        if(k==NO){ printf("\nError! --- %ld",__LINE__); exit(0); }
        ///////////////////////////////////////////////
        
        //2: cal. price

	x1=pp->point[k].x;
	x2=pp->point[k+1].x;

	y1=pp->point[k].y;
	y2=pp->point[k+1].y;

        a=SIForm_a(x1,x2,y1,y2);
	b=SIForm_b(x1,x2,y1,y2);

        ret=(a*p+b)*pp->Cb;
        //////////////////////////////////////////

        return ret;
}

long findk(double p,struct pricestr *pp)
{
        register long i;
        long n,ret;
        /////////////////////////////////////

		//1: beyond the range

        if(p<pp->point[1].x) return NO;

		n=pp->pntnum;
        if(p>=pp->point[n].x) return NO;
		//////////////////////////////////////

		//2: within the range

        for(i=2;i<=n;i++)
        {
                if(pp->point[i].x>p)
                {
                        ret=i-1;//No Change!
                        break;
                }
        }//end for
        /////////////////////////////

        return ret;
}

long findk_ex(double p,struct pricestr *pp)
{
		if(p<pp->point[1].x) return 1;
		
		long n=pp->pntnum;
        if(p>=pp->point[n].x) return n-1;

        long ret=findk(p,pp);
		return ret;
}

void pickCurrGenr(double *crrGn,double *nodeP,long *codeList)
{
        register long i;
		long ii;

        struct genrstr *gp;
		//////////////////////////////////////////

		gp=genrData;
        for(i=1;i<=genrNum;i++,gp=gp->next)
        {
                ii=lookup(codeList,gp->i,nodeNum);
                if(ii==NO){ printf("\nError! --- %ld",__LINE__); exit(0); }

                crrGn[i]=nodeP[ii];
        }//end for
        ///////////////////////////////

        return;
}

double sysPurcost(double *crrGn,long genrNum)
{
        register long i;
        double genr,price,ret=0.0;//init.

        struct pricestr *pp;
		struct genrstr *gp;
		///////////////////////////////////////////

		gp=genrData;
		pp=priceData;
        
		for(i=1;i<=genrNum;i++,gp=gp->next,pp=pp->next)
        {
                if(gp==NULL || pp==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }
				if(gp->i!=pp->i){ printf("\nError! --- %ld",__LINE__); exit(0); }

				genr=crrGn[i];
				if(genr<=0.0) continue;
				//////////////////////////////////////

				price=priceFun(genr,pp);
                ret+= genr*price;
        }//end for
        //////////////////////////////////////

        return ret;
}

double vioPctStat(struct flowstr *flowData)
{
        register long i;
        double aux2,ret=0.0;//init

        struct flowstr *fp;
        struct linestr *lp;
        ///////////////////////////////////

        fp=flowData;
        lp=lineData;

        for(i=1;i<=lineNum;i++,fp=fp->next,lp=lp->next)
        {
			if(fp->vioFlg!=YES) continue;
			
			aux2=max(fabs(fp->s1),fabs(fp->s2))-lp->lmt;
			aux2*= 100.0/lp->lmt;

			if(aux2>ret) ret=aux2;			
        }//end for
        ///////////////////////////////////

        return ret;
}

double vioPctStat(struct groupstr *groupData)
{
	register long i;
	long imode,k;

	double mwmin,mwmax,aux2,sum,ret;
	struct groupstr *grp;
	////////////////////////////////////////////
		
	k  =0;//init.
	ret=0.0;
	sum=0.0;

	grp=groupData;
	for(i=1;i<=groupNum;i++,grp=grp->next)
	{
		if(grp->vioFlg!=YES) continue;
		k++;

		mwmax=grp->mwmax;//default
		mwmin=grp->mwmin;

		imode=grp->imode;
		if(imode>0) mwmax=grp->mode[imode].mwmax;
		///////////////////////////////////////////////

		if(grp->w>mwmax)
			aux2=(grp->w-mwmax)/mwmax;
		else
			aux2=(grp->w-mwmin)/mwmin;

		sum+= 100.0*fabs(aux2);
	}//end for
	//////////////////////////////////////////

	if(k>0) ret=sum/k;
	return ret;
}
/*
void assoFun(double *crrgen,struct genrstr *genrData,struct assostr *assoData,long prnt)
{
	if(prnt==YES) printf("\n\nassoFun()...");

	register long i,k;
	double aux0,aux2,sum_P0,sum_inc;
	
	struct assostr *asp;
	struct genrstr *gp;
	///////////////////////////////////////////

	gp=genrData;
	while(gp!=NULL)
	{
		gp->wa=0.0;//clearing
		gp=gp->next;
	}//end while
	////////////////////////////////////

	asp=assoData;
	while(asp!=NULL)
	{
		if(prnt==YES) printf("\n\n asso.id = %s",asp->id);

		//1: cal. sum_inc and sum_P0

		sum_inc=0.0;//init.
		sum_P0 =0.0;

		for(i=1;i<=asp->unitNum;i++)
		{
			gp=findData(asp->unit[i].i,genrData);
			if(gp==NULL){ printf("\n\nError! --- %ld",__LINE__); exit(0); }
			
			k=posiChainNode(genrData,gp);
			if(k==NO){ printf("\nError! --- %ld",__LINE__); exit(0); }

			aux2=crrgen[k]-gp->w0;
			asp->unit[i].w=gp->w0;

			sum_inc+= aux2;
			sum_P0 += gp->w0;
		}//end if		
		///////////////////////////////////////////////////////

		//2: asso.balancing

		if(fabs(sum_P0)<SMLL){ printf("\nError! --- %ld",__LINE__); exit(0); }
		aux0=sum_inc/sum_P0;
		
		for(i=1;i<=asp->unitNum;i++)
			asp->unit[i].inc=asp->unit[i].w*aux0;
		//////////////////////////////////////////////////

		//3: modifying crrgen[] and genrData_Pa

		for(i=1;i<=asp->unitNum;i++)
		{
			gp=findData(asp->unit[i].i,genrData); 
			k=posiChainNode(genrData,gp);

			crrgen[k]=gp->w0+asp->unit[i].inc;
			gp->wa=asp->unit[i].inc-gp->wc;
		}//end for
		/////////////////////////////////////////////////////

		asp=asp->next;
	}//end while
	/////////////////////////////////////

	return;
}
*/
void reportReguPolicy(char *resfile,char *waflg,struct genrstr *genrData,long prnt)
{
	long width=66;
	double inc;

	struct genrstr *gp;
	///////////////////////////////////////////

	FILE *out=fopen(resfile,waflg);
	if(out==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }
	
	fprintf(out,"\n\nRegulation Policy Report µ÷Õû²ßÂÔ");
	fdrawline(out,width);
	fprintf(out,POLICY_TITL);
	fdrawline(out,width);	
	///////////////////////////////////////////

	//2: policy report

	gp=genrData;
	while(gp!=NULL)
	{
		strcpy(idbuf,gp->id);
		stringEnd(idbuf,(long)8);

		inc=gp->w-gp->w0;
		fprintf(out,POLICY_FORM,idbuf,gp->i,gp->w0,gp->w,inc,gp->descr);  	
	
		gp=gp->next;
	}//end while

	if(prnt==YES) drawline(width);
	fdrawline(out,width);
	/////////////////////////////////////
	
	fclose(out);
	return;
}

void reportVioGroup(FILE *out,struct groupstr *groupData,long prnt)
{
	register long i;

	double lmt,w;
	struct groupstr *grp;
	///////////////////////////////////////////////

	grp=groupData;
	for(i=1;i<=groupNum;i++,grp=grp->next)
	{
		if(grp->vioFlg!=YES) continue;//No Change!

		strcpy(idbuf,grp->id);
		strcpy(idbuf2,grp->mode[grp->imode].id);

		lmt=grp->mode[grp->imode].mwmax*100.0;
		w=grp->w*100.0;

		fprintf(out,"\n %s  mode=%s lmt=%7.2lf w=%7.2lf",idbuf,idbuf2,lmt,w);
	}//end for 
	///////////////////////////////////////////

	return;
}
	
void reportGroupLmt(FILE *out,struct groupstr *groupData,long prnt)
{
	register long i;
	long imode;

	struct groupstr *grp;
	struct linestr *lp;
	////////////////////////////////////////////////////////

	fprintf(out,"\nDE Group Limit Report:");
	fprintf(out,"\nDE id    |imode    |group_limit |line_limits (powers)");

	grp=groupData;
	while(grp!=NULL)
	{
		imode=grp->imode;
		fprintf(out,"\n   %s %4ld",grp->id,imode);

		if(imode==0){ grp=grp->next; continue; }
		///////////////////////////////////////////////////

		fprintf(out,"  mwmax=%6.2lf",grp->mode[imode].mwmax);
		for(i=1;i<=grp->lineNum;i++)
		{
			lp=findData(grp->line[i].i,grp->line[i].j,lineData);
			if(lp==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }

			fprintf(out,"  %6.2lf (%6.4lf)",lp->lmt,grp->line[i].w);
		}//end for 
		
		grp=grp->next;
	}//end while
	//////////////////////////////////

	return;
}

long hangback(double *prevInc,double *currInc,long n)
{
        register long i;
        long ii,k;

		double crr,prv;
        ////////////////////////////////////

        //1: counting

        ii=0;//init.
		k =0;

        for(i=1;i<=n;i++)
        {
				crr=currInc[i];
				prv=prevInc[i];

                if(fabs(crr)<EPS) continue;
				////////////////////////////////////////

                k++;
				if(oppSign(crr,prv)==YES)
				{
					crr=fabs(crr);
					prv=fabs(prv);
					
					if(crr>=prv) ii++;
				}//end if
        }//end for
        ////////////////////////////

        //2: conclusion

        if(ii>k/2) return YES;
        return NO;
}

void modiBusAngl(double *v0,double *genrInc)
{
        register long i;
        long k,n=nodeNum-1;//slack bus omitted

        long *tempList;
        double *dP,*dA;
        //////////////////////////////////////////

        //1: allocation of memory

        dP=(double *)calloc(n+1,sizeof(double));
        dA=(double *)calloc(n+1,sizeof(double));
        tempList=(long *)calloc(n+1,sizeof(long));

        if(dP==NULL || dA==NULL || tempList==NULL)
        { printf("\nError! --- %ld",__LINE__); exit(0); }
        ///////////////////////////////////////////////////

        //2: form tempList[] and dP[]

        formTempList(tempList,codeList,codeAttr,SKIP_SLCK);
        initVector(dP,genrNum,0.0);//init

        struct genrstr *gp=genrData;
        for(i=1;i<=genrNum;i++,gp=gp->next)
        {
                k=lookup(tempList,gp->i,n);
                if(k==NO) continue;//slack skipped

                dP[k]=genrInc[i];
        }//end for

        for(i=1;i<=n;i++)//making some modification
        {
                k=lookup(codeList,tempList[i],nodeNum);
                if(k==NO){ printf("\nError! --- %ld",__LINE__); exit(0); }

                dP[i]/= vv[k];
        }//end for
        /////////////////////////////////////////

        //3: solve fact.table

        copyVector(dA,dP,n);//dA[] <== dP[]
        solveFactorTab(B1,dA,n);

        divVector(dA,n,av_volt);
        /////////////////////////////////////////

        //4: modifying v0[]

        for(i=1;i<=nodeNum;i++)
        {
                k=lookup(tempList,codeList[i],n);
                if(k==NO) continue;//no modification

                v0[i]+= dA[k];
        }//end for
        //////////////////////////////

        free(tempList);
        free(dP);
        free(dA);
        ////////////////////////
                                                              
        return;
}

void modiFlowData(struct flowstr *flowData,double *branInc)
{
        register long i;
        long ii,jj,kbFlg,vioFlg;
        double s1,s2,aux2;

        struct flowstr *fp;
        struct linestr *lp;
        ////////////////////////////////////////

        fp=flowData;
        lp=lineData;

        for(i=1;i<=lineNum;i++,fp=fp->next,lp=lp->next)
        {
                //.1 looking up codeList[]

		ii=lookup(codeList,fp->i,nodeNum);
                jj=lookup(codeList,fp->j,nodeNum);
                
		if(ii==NO || jj==NO){ printf("\nError! --- %ld",__LINE__); exit(0); }
		/////////////////////////////////////////

		//.2 modifying w1 and w2

                fp->w1+= branInc[i];
                fp->w2-= branInc[i];
		//////////////////////////////////////////

		//.3 updating s1 and s2

		mvaAmpfun(s1,s2,fp->w1,fp->r1,fp->w2,fp->r2,lp);

                fp->s1 = s1;
                fp->s2 = s2;
		////////////////////////////////////////

		//.4 kbFlg and vioFlg

                kbFlg =NO;//init.
		vioFlg=NO;

                aux2=max(fabs(fp->s1),fabs(fp->s2));
                if(lp->lmt>SMLL)
		{
			if(aux2>lp->lmt*KB_COEFF) kbFlg=YES;
                	if(aux2>lp->lmt) vioFlg=YES;
		}//end if

                fp->kbFlg=kbFlg;
                fp->vioFlg=vioFlg;
        }//end for
        ///////////////////////////////////

        return;
}

void getBranInc(double *branInc,double *currInc,double **PBPG)
{
        register long i,j;
        double sum;

        for(i=1;i<=lineNum;i++)
        {
                sum=0.0;

                for(j=1;j<=genrNum;j++)
                        sum+= PBPG[i][j]*currInc[j];

                branInc[i]=sum;
        }//end for

        return;
}

void untread(double *ctrl,double *inc,long n)
{
        register long i;

        for(i=1;i<=n;i++) ctrl[i]-= inc[i];
		return;
}

long formLP_w(double *genrInc,                              //solution
              double *d,                                    //obj.
              double *beta_w,                               //eq.con
              double **PBPG,double *LBmin,double *LBmax,    //br.con
              double **GRPG,double *LGmin,double *LGmax,    //grp.con
              double *Lmin_w,double *Lmax_w,                //bounds
              struct flowstr *flowData,long prnt)           //power flow
{
        if(prnt==YES) printf("\n\nformLP_w()...");

        //1: kb/group stat.

        long kbNum =kbNumStat(flowData);  //key branch
        long kbNum2=kbNumStat(groupData); //key group

        if(prnt==YES) printf("\n kbNum =%ld",kbNum);
        if(prnt==YES) printf("\n kbNum2=%ld",kbNum2);
        /////////////////////////////////////////////////////

        //2: memory allocation for BLP

        long m=1+kbNum+kbNum2; //num. of all constraints
        long n=genrNum;        //num. of ctrl. var.
        
        blp_global_alloc(m,n);
        if(prnt==YES) printf("\n blp_global_alloc() ok");
        //////////////////////////////////////////////////////////

        //3: filling in obj.function --- c[]

        copyVector(c,d,genrNum);
        if(prnt==YES) printf("\n c[] ok");
        /////////////////////////////////////////////

        //4: filling in b[] and a[][]

        //4.1 eq.constraint --- power flow

        b[1]=0.0;
        copyMatrixRow(a,beta_w,(long)1,genrNum);
        /////////////////////////////////////////////

        //4.2 non-equations --- key branches

        register long i;

        long ik,k;
        struct flowstr *fp;
        ///////////////////////////////////
		
        ik=1;//init.--- No Change!
        k =0;

        fp=flowData;
        for(i=1;i<=lineNum;i++,fp=fp->next)
        {
                if(fp->kbFlg!=YES) continue;
                k++;

                copyMatrixRow(a,ik+k,PBPG,i,genrNum);
                b[ik+k]=LBmax[i];

                if(fp->w1<SMLL)//negative power
                {
                        mulMatrixRow(a,ik+k,genrNum,-1.0);
                        b[ik+k]=-LBmin[i];
                }//end if
        }//end for

        if(k!=kbNum){ printf("\nError! --- %ld",__LINE__); exit(0); }
        ////////////////////////////////////////////////

        //4.3 non-equations --- group constraints

        struct groupstr *grp;

        ik+= kbNum;
        k=0;//init.
        	
        grp=groupData;
        for(i=1;i<=groupNum;i++,grp=grp->next)
        {
                if(grp->kbFlg!=YES) continue;
                k++;

                copyMatrixRow(a,ik+k,GRPG,i,genrNum);
                b[ik+k]=LGmax[i];

                if(grp->w<SMLL)//nega. power
                {
                        mulMatrixRow(a,ik+k,genrNum,-1.0);
                        b[ik+k]=-LGmin[i];
                }//end if
        }//end for
		
        if(k!=kbNum2){ printf("\nError! --- %ld",__LINE__); exit(0); }
        if(prnt==YES) printf("\n a/b[] ok");
        ///////////////////////////////////////////////////////

        //5: lower and upper bounds --- L/U[]

        for(i=1;i<=genrNum;i++)
        {
                L[i]=Lmin_w[i];
                U[i]=Lmax_w[i];
        }//end for

        if(prnt==YES) printf("\n L/U[] ok");
        //////////////////////////////////////////////

        //6: solve the LP problem

        printLP(blpfile,a,b,c,L,U,m,n);
        if(prnt==YES) printf("\n\n %s formed",blpfile);

        long ret2=blpfun(x,a,b,c,L,U,m,n,(long)1,NO);//prnt
        if(prnt==YES) printf("\n blpfun() returned: %ld",ret2);

        long ret=NO;//init.
        if(ret2==OPTIML)
        {
                copyVector(genrInc,x,genrNum);
	        ret=YES;
        }//end if
        //////////////////////////////////////////////

        blp_global_free();
        return ret;
}
        
void objLnz_wcorr(double *d,double *wgen,long genrNum,long prnt)
{
	if(prnt==YES) printf("\n\nobjLnz_wcorr()...");

	register long i;
	
	double H,L,D,x0,x;
	struct genrstr *gp;
	////////////////////////////////////////////

	gp=genrData;
	for(i=1;i<=genrNum;i++,gp=gp->next)
	{
		H=gp->wmax;
		L=gp->wmin;
		D=(H-L)*PNSH_COEFF;
		
		x0=gp->w0;	
		x =wgen[i];
		
		d[i]=alphFun(x,x0,H,L,D);
	}//end for
	////////////////////////////////////////////

	return;
}
               
void objLnz_w(double *d,double *wgen,long genrNum,long prnt)
{
        if(prnt==YES) printf("\n\nobjLnz_w()...");

        register long i,k;
        double x1,x2,y1,y2,a,b,pwrpu;

        struct genrstr *gp;
        struct pricestr *pp;
        ///////////////////////////////////////////////

        gp=genrData;//init.
        for(i=1;i<=genrNum;i++,gp=gp->next)
        {
                if(prnt==YES) printf("\n\n i=%ld: genr_id=%s",i,gp->id);

                if(wgen[i]<0.0)//no generation
                {
                        d[i]=0.0;
                        continue;
                }//end if
		//////////////////////////////////////////////////

                pp=findData(gp->i,priceData);
                if(pp==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }

                pwrpu=wgen[i]/pp->Pb;
                k=findk_ex(pwrpu,pp);
		
                if(prnt==YES) printf("\n pwrpu=%lf, k=%ld",pwrpu,k);
                //////////////////////////////////////////////////

		x1=pp->point[k].x;
		x2=pp->point[k+1].x;

		y1=pp->point[k].y;
		y2=pp->point[k+1].y;

                a=SIForm_a(x1,x2,y1,y2);
		b=SIForm_b(x1,x2,y1,y2);

                d[i]=(2.0*a*pwrpu+b)*pp->Cb;//No Change!
                if(prnt==YES) printf("\n d[%ld]=%lf",i,d[i]);
        }//end for
        /////////////////////////////////////////////////////////

        return;
}

void updateData(struct genrstr *genrData,double *crrGn,long genrNum)
{
        register long i;
        long slack=codeList[0];//slack bs

	double aux2,slack_P0,slack_P;
	struct genrstr *gp;
	/////////////////////////////////////////

	double sum_w_regu=0.0;//init.
        gp=genrData;

        for(i=1;i<=genrNum;i++,gp=gp->next)
        {        
		gp->w=crrGn[i];
		
		if(gp->i==slack)
		{
			slack_P0=gp->w0;
			slack_P =gp->w;
		}
		else
		{
			aux2=gp->w - gp->w0;
			sum_w_regu+= aux2;
		}	
	}//end for
	/////////////////////////////////////////

	aux2=slack_P-slack_P0;
	if(fabs(sum_w_regu)>SMLL) sum_w_regu+= aux2; 

	return;
}

void limFun_w(double *Lmin_w,double *Lmax_w,double *crrGn)
{
        register long i;
        struct genrstr *gp;
        ///////////////////////////////////////////
		
        gp=genrData;
        for(i=1;i<=genrNum;i++,gp=gp->next)
        {
                Lmin_w[i]=gp->wmin-crrGn[i];
                Lmax_w[i]=gp->wmax-crrGn[i];

                if(fabs(gp->rampUp)>SMLL)
                {
                        if(Lmin_w[i]<-gp->rampDn) Lmin_w[i]=-gp->rampDn;
                        if(Lmax_w[i]>gp->rampUp)  Lmax_w[i]=gp->rampUp;
                }//end if
        }//end for
        /////////////////////////////////

        return;
}

void LGfun_w(double *LGmin,double *LGmax,struct groupstr *groupData)
{
        register long i;
        double mwmin,mwmax;

        struct groupstr *grp;
        /////////////////////////////////////////

        initVector(LGmin,groupNum,0.0);
        grp=groupData;
        
        for(i=1;i<=groupNum;i++,grp=grp->next)
        {
              	mwmin=grp->mwmin;
                mwmax=grp->mwmax;

                if(fabs(mwmin)<SMLL && mwmax<SMLL) continue;
                //////////////////////////////////////////////

                LGmin[i]=mwmin*(1.0+ov_coeff)-grp->w;
                LGmax[i]=mwmax*(1.0-ov_coeff)-grp->w;
        }//end for
        //////////////////////////////////////

        return;
}

void LBfun_w(double *LBmin,double *LBmax,struct flowstr *flowData,double *vv,long *codeList)
{
        register long i;
       
		double PBmin,PBmax,aux2,aux4;

        struct flowstr *fp;
        struct linestr *lp;
        ///////////////////////////////////////////////////////

        initVector(LBmin,lineNum,0.0);//init.
        initVector(LBmax,lineNum,0.0);

        fp=flowData;
        lp=lineData;

        for(i=1;i<=lineNum;i++,lp=lp->next,fp=fp->next)
        {
			//.1 lmt. check

			if(lp->lmt<SMLL) continue;
			/////////////////////////////////////////

			//.2 PBmax/min <-- lmt

			aux2=lmtPU(lp);//MVA p.u.
			aux4=aux2*aux2-fp->r1*fp->r1;

			if(aux4<SMLL)//serious violation
			{ 
				PBmax=0.0;
				PBmin=0.0; 
			}
			else
			{
				PBmax=sqrt(aux4)*(1.0-ov_coeff);
           		PBmin=-PBmax;
			}//end else
			///////////////////////////////////

			//.3 LBmax/min

			LBmin[i]=PBmin-fp->w1;
			LBmax[i]=PBmax-fp->w1;
        }//end for
        /////////////////////////////////////////

        return;
}

void beta_wfun(double *beta_w,double *PLPG,long genrNum)
{
        register long i;
        for(i=1;i<=genrNum;i++) beta_w[i]=1.0-PLPG[i];

        return;
}

//////////////////////////////////////////
//end of file

