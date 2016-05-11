//opf_wfun.cpp by Wang GT
///////////////////////////////////

#include"blp.h"
#include"common.h"
#include"opf.h"
#include"opf_global.h"
//////////////////////////////////////////////////////////////////

void modiSlack(struct genrstr *genrData,double *nodeP,long *codeList)
{
	long slack,k;
	struct genrstr *gp;
	///////////////////////////////////
	
	slack=codeList[0];

	k=lookup(codeList,slack,nodeNum);
	if(k==NO){ printf("\n\nError! --- %ld",__LINE__); exit(0); }

	gp=findData(slack,genrData);
	if(gp==NULL){ printf("\n\nError! --- %ld",__LINE__); exit(0); }

	gp->P=nodeP[k];
	//////////////////////////////////////

	return;
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
			
			aux2=max(fabs(fp->s1),fabs(fp->s2));
			aux2-= lp->lmt;

			ret+= 100*aux2/lp->lmt;//%
        }//end for
		///////////////////////////////////

        return ret;
}

double vioPctStat(struct groupstr *groupData)
{
	register long i;
	long imode;

	double aux2,lmt,ret;
	struct groupstr *grp;
	////////////////////////////////////
	
	ret=0.0;//init.
	grp=groupData;

	for(i=1;i<=groupNum;i++,grp=grp->next)
	{
		if(grp->vioFlg!=YES) continue;
		imode=grp->imode;

		lmt=grp->lmt;//default
		if(imode>0) lmt=grp->mode[imode].lmt;
		///////////////////////////////////

		aux2=grp->w-lmt;
		ret+= 100*aux2/grp->w;//%
	}//end for
	////////////////////////////////////

	return ret;
}

void assoFun(double *currGenr,struct genrstr *genrData,struct assostr *assoData,long prnt)
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
		gp->Pa=0.0;//clearing
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

			aux2=currGenr[k]-gp->P0;
			asp->unit[i].w=gp->P0;

			sum_inc+= aux2;
			sum_P0 += gp->P0;
		}//end if		
		///////////////////////////////////////////////////////

		//2: asso.balancing

		if(fabs(sum_P0)<SMLL){ printf("\nError! --- %ld",__LINE__); exit(0); }
		aux0=sum_inc/sum_P0;
		
		for(i=1;i<=asp->unitNum;i++)
			asp->unit[i].inc=asp->unit[i].w*aux0;
		//////////////////////////////////////////////////

		//3: modifying currGenr[] and genrData_Pa

		for(i=1;i<=asp->unitNum;i++)
		{
			gp=findData(asp->unit[i].i,genrData); 
			k=posiChainNode(genrData,gp);

			currGenr[k]=gp->P0+asp->unit[i].inc;
			gp->Pa=asp->unit[i].inc-gp->Pc;
		}//end for
		/////////////////////////////////////////////////////

		asp=asp->next;
	}//end while
	/////////////////////////////////////

	return;
}

void groupExpFlgReset(struct groupstr *groupData)
{
	struct groupstr *grp=groupData;
        while(grp!=NULL)
        {
		grp->expFlg=NO;//reset
		
		grp=grp->next;
	}//end while

	return;
}

void groupExpFlgSet(struct groupstr *groupData)
{
	groupExpFlgReset(groupData);

	struct groupstr *grp=groupData;
	while(grp!=NULL)
	{
		if(grp->vioFlg==YES)
			grp->expFlg=YES;//set

		grp=grp->next;
	}//end while

	return;
}

void pickGroupW(double *grp_w,struct groupstr *groupData,long groupNum)
{
	register long i;
	struct groupstr *grp;

	grp=groupData;
	for(i=1;i<=groupNum;i++,grp=grp->next)
		grp_w[i]=grp->w;
	////////////////////////////////

	return;
}
		
void reportReguPolicy(FILE *out,struct genrstr *genrData,long prnt)
{
	long kk=40;//width
	double inc;

	struct genrstr *gp;
	///////////////////////////////////////////

	//1: table drawing

	if(prnt==YES)
	{
 		printf("\n\nRegulation Policy Report:");
		drawline(kk);

		printf(POLICY_TITL);
		drawline(kk);
	}//end if

	fprintf(out,"\n\nRegulation Policy Report:");
	fdrawline(out,kk);
	fprintf(out,POLICY_TITL);
	fdrawline(out,kk);	
	///////////////////////////////////////////

	//2: policy report

	gp=genrData;
	while(gp!=NULL)
	{
		strcpy(idbuf,gp->id);
		stringEnd(idbuf,(long)8);

		inc=gp->P-gp->P0;

		if(prnt==YES) printf(POLICY_FORM,idbuf,gp->i,gp->P0,gp->P,inc);
		fprintf(out,POLICY_FORM,idbuf,gp->i,gp->P0,gp->P0,inc);  	
	
		gp=gp->next;
	}//end while

	if(prnt==YES) drawline(kk);
	fdrawline(out,kk);
	/////////////////////////////////////
	
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

		lmt=grp->mode[grp->imode].lmt*100.0;
		w=grp->w*100.0;

		if(prnt==YES) printf("\n %s  mode=%s lmt=%7.2lf w=%7.2lf",idbuf,idbuf2,lmt,w);
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

		fprintf(out,"  grpLmt=%6.2lf",grp->mode[imode].lmt);
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

void initCtrlData(struct genrstr *genrData)
{
        double volt;
        struct genrstr *gp;
        struct nodestr *np;
        //////////////////////////////////////

        gp=genrData;
        while(gp!=NULL)
        {
                np=findData(gp->i,nodeData);
                if(np==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }

                if(np->attr==PV)//active power checkup
                {
                        if(fabs(gp->P0-np->p)>SMLL)
                        {
                                printf("\n\nWarning --- %ld",__LINE__);

                                printf("\n  Ambiguous power provided for genr:");
                                printf("\n  id=%s, ibs=%ld",gp->id,gp->i);
                                printf("\n  p =%lf by nodeData",np->p);
                                printf("\n  P0=%lf by genrData",gp->P0);
                        }//end if
                }//end if

                if(np->attr==PV || np->attr==SLCK)//voltage checkup
                {
                        if(np->attr==PV) volt=np->q;
                        else volt=np->p;

                        if(fabs(gp->V0-volt)>SMLL)
                        {
                                printf("\n\nWarning --- %ld",__LINE__);

                                printf("\n  Ambiguous voltage provided for genr:");
                                printf("\n  id=%s, ibs=%ld",gp->id,gp->i);
                                printf("\n  volt=%lf by nodeData",volt);
                                printf("\n  V0  =%lf by genrData",gp->V0);
                        }//end if
                }//end if
                ///////////////////////////

                gp->P=gp->P0;
                gp->V=gp->V0;

                gp=gp->next;
        }//end while
        /////////////////////////

        return;
}

long hangback(double *prevInc,double *currInc,long n)
{
        register long i;
	long ii,k;
	///////////////////////////

        //1: counting

        ii=k=0;
        for(i=1;i<=n;i++)
        {
                if(fabs(currInc[i])<EPS) continue;

                k++;
                if(fabs(prevInc[i]+currInc[i])<EPS) ii++;
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
        solveFactTab(B1,dA,n);

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
//NOTATION: all data of flowstr should be considered

        register long i;
        long kbFlg,vioFlg;
        double aux2;

        struct flowstr *fp;
        struct linestr *lp;
        //////////////////////////

        fp=flowData;
        lp=lineData;

        for(i=1;i<=lineNum;i++,fp=fp->next,lp=lp->next)
        {
                if(brMatch(fp,lp->i,lp->j)!=YES){ printf("\nError! --- %ld",__LINE__); exit(0); }

                fp->w1+= branInc[i];
                fp->w2-= branInc[i];

                fp->s1  =sign(fp->w1)*sqrt(fp->w1*fp->w1+fp->r1*fp->r1);
                fp->s2  =sign(fp->w2)*sqrt(fp->w2*fp->w2+fp->r2*fp->r2);
                ////////////////////////////////////////////

                kbFlg=vioFlg=NO;//init.
                aux2=max(fabs(fp->s1),fabs(fp->s2));
                                                      

                if(lp->lmt>SMLL && aux2>lp->lmt*KB_COEFF) kbFlg=YES;
                if(lp->lmt>SMLL && aux2>lp->lmt) vioFlg=YES;

                fp->kbFlg=kbFlg;
                fp->vioFlg=vioFlg;
        }//end for
        /////////////////////

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

void modiCtrlVctr(double *currGenr,double *currInc)
{
        register long i;

        for(i=1;i<=genrNum;i++)
                currGenr[i]+= currInc[i];

        return;
}

long formLP_w(double *genrInc,					//solution
		  double *d,					//obj.
		  double *beta_w,				//eq.con
		  double **PBPG,double *LBmin,double *LBmax,	//br.con
		  double *Lmin_w,double *Lmax_w,		//bounds
		  struct flowstr *flowData,long prnt)		//flowdata
{
        if(prnt==YES) printf("\n\nformLP_w()...");

        //1: declaration and initialization

        register long i,j;
        long m,n0,kbNum,ret,ret2;

        long *mel,*actvCon,*actvBnd;
        double *_a,**a,*b,*c,*L,*U;
        //////////////////////////////////////////////////

        kbNum=kbNumStat(flowData);	//key branches

        n0=genrNum;		//num. of orig. var.
        m=1+kbNum;		//num. of all constraints

        if(prnt==YES) printf("\n n0=%ld, m=%ld",n0,m);
        ////////////////////////////////////////////////////

        //2: allocation of memory

        actvCon=(long *)calloc(m+1,sizeof(long));
        actvBnd=(long *)calloc(n0+1,sizeof(long));
        
        makeMatrix(_a,a,m+1,n0+1);
        b=(double *)calloc(m+1,sizeof(double));
        c=(double *)calloc(n0+1,sizeof(double));

        mel=(long *)calloc(m+1,sizeof(long));
        L=(double *)calloc(n0+1,sizeof(double));
        U=(double *)calloc(n0+1,sizeof(double));

        if(L==NULL || U==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }
        //////////////////////////////////////////////////////

        //3: constructing BLP

        //3.1 c[]
        for(i=1;i<=genrNum;i++) c[i]=d[i];
        /////////////////////////////////////

        //3.2 b[], mel[] and a[][]

		//3.2.1 EQ --- power flow

        b[1]=0.0;
		mel[1]=EQ;

        for(j=1;j<=genrNum;j++)
                a[1][j]=beta_w[j];
        ///////////////////////////////////

		//3.2.2 key branches

        long ik=1,k=0;//init.
        struct flowstr *fp=flowData;

        for(i=1;i<=lineNum;i++,fp=fp->next)
        {
                if(fp->kbFlg!=YES) continue;
                k++;

                for(j=1;j<=genrNum;j++)
                        a[ik+k][j]=PBPG[i][j];

                if(fp->w1>0.0)//posi.direction
                {
                        b[ik+k]=LBmax[i];
                        mel[ik+k]=LT;
                }
                else
                {
                        b[ik+k]=LBmin[i];
                        mel[ik+k]=MT;
                }
        }//end for

        if(k!=kbNum){ printf("\nError! --- %ld",__LINE__); exit(0); }
        ////////////////////////////////////////////////////////

		//3.3 L[] and U[]

        for(i=1;i<=genrNum;i++)
        {
                L[i]=Lmin_w[i];
                U[i]=Lmax_w[i];
        }//end for
        //////////////////////////////////////////////

		//3.4 printing lp formed

        printLP(tempfile,a,b,c,L,U,mel,m,n0);
        if(prnt==YES) printf("\n\n %s formed",tempfile);
		////////////////////////////////////////////////////

        //4: solve the LP problem

        ret2=blp(genrInc,a,b,c,L,U,mel,m,n0,NO);//prnt
        if(prnt==YES) printf("\n\n blp() returned: %ld",ret2);

        if(ret2!=OPTIML) ret=NO;//unexpected
		else
		{
			actvFun(actvCon,actvBnd,mel,b,L,U,a,genrInc,m,n0);
			ret=YES;
		}//end else
        ////////////////////////////////////////////

        freeMatrix(_a,a);
        free(b); 
		free(c);

		free(actvCon);
		free(actvBnd);
        free(mel);
        free(L); 
		free(U);
        ///////////////////////

        return ret;
}

long formLP_w(double *genrInc,						//solution
			  double *d,					//obj.
			  double *beta_w,				//eq.con
			  double **PBPG,double *LBmin,double *LBmax,	//br.con
			  double **GRPG,double *LGmax,			//grp.con
			  double *Lmin_w,double *Lmax_w,		//bounds
			  struct flowstr *flowData,long prnt)		//power flow
{
        if(prnt==YES) printf("\n\nformLP_w()...");

        //1: declaration and initialization

        register long i,j;
        long m,n0,kbNum,kbNum2,ret,ret2;

        long *mel,*actvCon,*actvBnd;
        double *_a,**a,*b,*c,*L,*U;
        //////////////////////////////////////////////////

        kbNum=kbNumStat(flowData);   //key branch
        kbNum2=kbNumStat(groupData); //key group

        if(prnt==YES) printf("\n kbNum =%ld",kbNum);
        if(prnt==YES) printf("\n kbNum2=%ld",kbNum2);

        n0=genrNum;            //num. of orig. var.
        m=1+kbNum+kbNum2;      //num. of all constraints
        
        if(prnt==YES) printf("\n n0=%ld, m=%ld",n0,m);
	////////////////////////////////////////////////////

        //2: allocation of memory

        actvCon=(long *)calloc(m+1,sizeof(long));
        actvBnd=(long *)calloc(n0+1,sizeof(long));
        
        makeMatrix(_a,a,m+1,n0+1);
        b=(double *)calloc(m+1,sizeof(double));
        c=(double *)calloc(n0+1,sizeof(double));

        mel=(long *)calloc(m+1,sizeof(long));
        L=(double *)calloc(n0+1,sizeof(double));
        U=(double *)calloc(n0+1,sizeof(double));

        if(L==NULL || U==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }
        //////////////////////////////////////////////////////

        //3: constructing BLP

        //3.1 c[]
        for(i=1;i<=genrNum;i++) c[i]=d[i];
        /////////////////////////////////////

        //3.2 b[], mel[] and a[][]

	//3.2.1 EQ --- power flow

        b[1]=0.0;
		mel[1]=EQ;

        for(j=1;j<=genrNum;j++)
                a[1][j]=beta_w[j];
        ///////////////////////////////////////

	//3.2.2 key branches

        long ik=1,k=0;//init.
        struct flowstr *fp=flowData;

        for(i=1;i<=lineNum;i++,fp=fp->next)
        {
                if(fp->kbFlg!=YES) continue;
                k++;

                for(j=1;j<=genrNum;j++)
                        a[ik+k][j]=PBPG[i][j];

                if(fp->w1>0.0)//posi.direction
                {
                        b[ik+k]=LBmax[i];
                        mel[ik+k]=LT;
                }
                else
                {
                        b[ik+k]=LBmin[i];
                        mel[ik+k]=MT;
                }
        }//end for

        if(k!=kbNum) exit(0);
        ///////////////////////////////////////

	//3.2.3 group constraints

	ik+= kbNum;
	k=0;//init.
        
	struct groupstr *grp=groupData;
        for(i=1;i<=groupNum;i++,grp=grp->next)
        {
                if(grp->kbFlg!=YES) continue;
                k++;

                for(j=1;j<=genrNum;j++)
			a[ik+k][j]=GRPG[i][j];

		b[ik+k]=LGmax[i];
		mel[ik+k]=LT;                
        }//end for

        if(k!=kbNum2){ printf("\nError! --- %ld",__LINE__); exit(0); }
	////////////////////////////////////////////

        //3.3 L[] and U[]

        for(i=1;i<=genrNum;i++)
        {
                L[i]=Lmin_w[i];
                U[i]=Lmax_w[i];
        }//end for
        //////////////////////////////////////////////

        //4: solve the LP problem

        printLP(tempfile,a,b,c,L,U,mel,m,n0);
        if(prnt==YES) printf("\n\n %s formed",tempfile);

        ret2=blp(genrInc,a,b,c,L,U,mel,m,n0,NO);//prnt
        if(prnt==YES) printf("\n\n blp() returned: %ld",ret2);

        if(ret2!=OPTIML) ret=NO;//unexpected
		else
		{
			actvFun(actvCon,actvBnd,mel,b,L,U,a,genrInc,m,n0);
			ret=YES;
		}//end else
        ////////////////////////////////////

        freeMatrix(_a,a);
        free(b); 
		free(c);

		free(actvCon);
		free(actvBnd);
        free(mel);
        free(L); 
		free(U);
        ////////////////////

        return ret;
}
                       
void objLnz_w(double *d,double *crrGn,long prnt)
{
	if(prnt==YES) printf("\n\nobjLnz_w()...");

        register long i,k;
        double a,b,pwrpu;

        struct genrstr *gp;
        struct pricstr *pp;
        ///////////////////////////////

        gp=genrData;//init.
        for(i=1;i<=genrNum;i++,gp=gp->next)
        {
		if(prnt==YES) printf("\n\n i=%ld: genr_id=%s",i,gp->id);

                if(crrGn[i]<0.0)//no generation
                {
                        d[i]=0.0;
                        continue;
                }//end if
                ////////////////////////////////////////

                pp=findData(gp->i,priceData);
                if(pp==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }

                pwrpu=crrGn[i]/pp->Pb;
                k=findk_ex(pp,pwrpu);
		
		if(prnt==YES) printf("\n pwrpu=%lf, k=%ld",pwrpu,k);
		/////////////////////////////////////////////////////////

		a=(pp->point[k+1].c-pp->point[k].c)/(pp->point[k+1].p-pp->point[k].p);
		b=pp->point[k].c-a*pp->point[k].p;

		d[i]=(2.0*a*pwrpu+b)*pp->Cb;//No Change!
		if(prnt==YES) printf("\n d[%ld]=%lf",i,d[i]);
        }//end for
        ///////////////////////////////////////////////////////////

        return;
}

void updateData(struct genrstr *genrData,double *currGenr,long genrNum)
{
        register long i;
        struct genrstr *gp;

        gp=genrData;
        for(i=1;i<=genrNum;i++,gp=gp->next)
                gp->P=currGenr[i];

        return;
}

void stepConfin(double *step,long n,double maxstep,double divstep)
{ stepConfin(step,(long)1,n,maxstep,divstep); return; }

void stepConfin(double *step,long n1,long n2,double maxstep,double divstep)
{
	if(maxstep<SMLL)
	{
		printf("\n\nWarning! --- %ld",__LINE__);
		printf("\n  maxstep = %lf",maxstep);
		
		return;
	}//end if
	///////////////////////////////////////////

    double aux2=fabsmax(step,n1,n2);
    while(aux2>maxstep)
    {
		divVector(step,n1,n2,divstep);
		aux2=fabsmax(step,n1,n2);
    }//end while
    ///////////////////////////////////////

    return;
}
                
void limFun_w(double *Lmin_w,double *Lmax_w,double *crrGn)
{
        register long i;
        struct genrstr *gp=genrData;

        for(i=1;i<=genrNum;i++,gp=gp->next)
        {
	        Lmin_w[i]=gp->Pmin-crrGn[i];
		Lmax_w[i]=gp->Pmax-crrGn[i];

		if(fabs(gp->ramp)>SMLL)//considering ramp ratio
		{
			if(Lmin_w[i]<-gp->fall)	Lmin_w[i]=-gp->fall;
        		if(Lmax_w[i]>gp->ramp)	Lmax_w[i]=gp->ramp;
		}//end if
	}//end for
        //////////////////////

        return;
}

void LGfun_w(double *LGmax,struct groupstr *groupData)
{
		register long i;
        double aux2;

        struct groupstr *grp;
        /////////////////////////////////////////

		initVector(LGmax,groupNum,0.0);
        grp=groupData;

        for(i=1;i<=groupNum;i++,grp=grp->next)
        {
                if(grp->lmt<SMLL) continue;

				aux2=grp->lmt*(1.0-ov_coeff);
                LGmax[i]=aux2-grp->w;
        }//end for
        //////////////////////////////////////

		return;
}

void LBfun_w(double *LBmin,double *LBmax,struct flowstr *flowData)
{
        register long i;
        double PBmin,PBmax;

        struct flowstr *fp;
        struct linestr *lp;
        /////////////////////////////////////////

		initVector(LBmin,lineNum,0.0);//init.
		initVector(LBmax,lineNum,0.0);

        fp=flowData;
        lp=lineData;

        for(i=1;i<=lineNum;i++,lp=lp->next,fp=fp->next)
        {
                if(lp->lmt<SMLL) continue;

				PBmax=lp->lmt*(1.0-ov_coeff);
				PBmin=-PBmax;

                LBmin[i]=PBmin-fp->w1;
                LBmax[i]=PBmax-fp->w1;
        }//end for
        ////////////////////////////////////

        return;
}

void beta_wfun(double *beta_w,double *PLPG,long genrNum)
{
        register long i;
        
	for(i=1;i<=genrNum;i++)
                beta_w[i]=1.0-PLPG[i];

        return;
}


//end of file

