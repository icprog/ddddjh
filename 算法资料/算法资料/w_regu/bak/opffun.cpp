//opffun.cpp by Wang GT
///////////////////////////////

#include"common.h"
#include"opf.h"
#include"opf_global.h"
///////////////////////////////////////////////////////////

double alphFun(double x,double x0,double H,double L,double D)
{
	double pct5,span;
	double ret;
	/////////////////////////////////////////////

	//1: init. locals 

	span = H-L;//No Change!
        pct5 = span*0.05;

	if(span<SMLL || D<SMLL)
	{ 
		printf("\n\nError! --- %ld",__LINE__);
		printf("\n  H=%lf, L=%lf, D=%lf",H,L,D);

		exit(0);
	}//end if
	/////////////////////////////////////////////

	//2: three cases --- by x0

	if(x0<L+pct5)
	{
		if(x<L+pct5)   ret=-D/pct5;
		else if(x<H)   ret= D/(span-pct5);
		else           ret= D/pct5;
	}
	else if(x0<H-pct5)
	{
		if(x<L)        ret=-D/pct5;
        	else if(x<x0)  ret=-D/fabs(x0-L);
        	else if(x<H)   ret= D/fabs(x0-H);
        	else           ret= D/pct5;
	}
	else
	{
		if(x<L)           ret=-D/pct5;
		else if(x<H-pct5) ret=-D/(span-pct5);
		else              ret= D/pct5;
	}//end if-else
	/////////////////////////////////////////////

	return ret;
}

double pnshFun(double x,double x0,double H,double L,double D)
{
	double pct5,span;
	double ret;
	/////////////////////////////////////////////

	//1: init. locals 

	span = H-L;//No Change!
	pct5 = span*0.05;

	if(span<SMLL || D<SMLL)
	{ 
		printf("\n\nError! --- %ld",__LINE__);
		printf("\n  H=%lf, L=%lf, D=%lf",H,L,D);
 
		exit(0); 
	}//end if
	/////////////////////////////////////////////

	//2: three cases --- by x0 

	if(x0<L+pct5)
        {
        	if(x<L+pct5)  ret=fabs(L+pct5-x)*D/pct5;
		else if(x<H)  ret=fabs(L+pct5-x)*D/(span-pct5);
                else          ret=fabs(H-pct5-x)*D/pct5;
        }
	else if(x0<H-pct5)
	{
		if(x<L)       ret=fabs(L+pct5-x)*D/pct5;
		else if(x<x0) ret=fabs(x0-x)*D/fabs(x0-L);
		else if(x<H)  ret=fabs(x0-x)*D/fabs(x0-H);
		else          ret=fabs(H-pct5-x)*D/pct5; 
	}
	else
	{
		if(x<L)           ret=fabs(L+pct5-x)*D/pct5;
		else if(x<H-pct5) ret=fabs(H-pct5-x)*D/(span-pct5);
		else              ret=fabs(H-pct5-x)*D/pct5;
	}
	/////////////////////////////////////////////

	return ret;
}

void genrDataSlide(struct genrstr *gp,double dP,double dQ,char plunus)
{
	//1: preparation

	printf("\n\nWarning! --- %ld",__LINE__);
	printf("\n  genrData modified: id=%s, descr=%s",gp->id,gp->descr);

	double aux2 = sign(plunus)*dP;
	double aux4 = sign(plunus)*dQ;
	//////////////////////////////////
 
	//2: modifying genr.data

    gp->w0   += aux2;
    gp->wmin += aux2;
    gp->wmax += aux2;

    gp->r0   += aux4;
    gp->rmin += aux4;
    gp->rmax += aux4;
	/////////////////////////////
	
	return;
}

double ratioEpsFun(struct xfmrstr *xp,double step)
{
	double ratio_bas,ratio_nom;
	double ret=0.0;//init.
	////////////////////////////////////////
	
	ratio_bas = xp->kvvl/xp->zkvvl;
	ratio_nom = xp->kvnom/xp->zkvnom;

	ret=step*ratio_nom/ratio_bas;
	/////////////////////////////////////

	return ret;
}

long plunus(struct taptystr *tp)
{
	long ret=1;//init.

	if(tp->mxtap < tp->mntap) ret=-1;
	return ret;
}

long tapFun(struct xfmrstr *xp,struct taptystr *tp,double ratio)
{
	long nom,ret=0;//init.

	double kvnom,zkvnom,kvvl,zkvvl;
	double r,ratio_nom,step,tap;
	////////////////////////////////////////////////////

	//1: init. variables

	nom = tp->nom;
	step= tp->step;
	
	kvnom = xp->kvnom;
	zkvnom= xp->zkvnom;

	kvvl  = xp->kvvl;
	zkvvl = xp->zkvvl;
	/////////////////////////////////////////

	//2: nom. ratio and tap

	ratio_nom=ratio*kvvl/zkvvl;
	r= ratio_nom/(kvnom/zkvnom);

	tap=plunus(tp)*(r-1.0)/step + nom;
	ret=(long)(tap+0.5);
	///////////////////////////////////////

	return ret;
}

double ratioFun(struct xfmrstr *xp,struct taptystr *tp,long tap)
{
	long nom;//nominal posi.

	double kvnom,zkvnom,kvvl,zkvvl;
	double step,ratio_nom,ret=0.0;//init.
	///////////////////////////////////////////////

	//1: init. variables

	nom = tp->nom;
	step= tp->step;
	
	kvnom = xp->kvnom;
	zkvnom= xp->zkvnom;

	kvvl  = xp->kvvl;
	zkvvl = xp->zkvvl;
	/////////////////////////////////////////

	//2: nom. and std. ratios

	ratio_nom = (1.0 + plunus(tp)*(tap-nom)*step) * kvnom/zkvnom;
	ret = ratio_nom/(kvvl/zkvvl);//std. ratio
	
	return ret;
}

void stepConfin(double *step,long n,double maxstep,double divstep)
{ 
	stepConfin(step,(long)1,n,maxstep,divstep);
	return;
}

void stepConfin(double *step,long n1,long n2,double maxstep,double divstep)
{
	//1: parameter verification

	if(maxstep<SMLL || divstep<=1.0)
	{
		printf("\n\nError! --- %ld",__LINE__);
		printf("\n  maxstep=%lf, divstep=%lf",maxstep,divstep);
		
		exit(0);		
	}//end if
	///////////////////////////////////////////

	//2: step reduction

	double aux2;

	while(1)//unconditional loop
	{
		aux2=fabsmax(step,n1,n2);
		if(aux2<=maxstep) break;

		divVector(step,n1,n2,divstep);
	}//end while
	///////////////////////////////////////

    return;
}

void initGenrData(struct genrstr *genrData)
{
        struct genrstr *gp;
        struct nodestr *np;
        //////////////////////////////////////

        gp=genrData;
        while(gp!=NULL)
        {
		if(gp->w0<SMLL)
		{
       	         	np=findData(gp->i,nodeData,PVSLACK);
                	if(np==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }

	                if(np->attr==PV) gp->w0=np->p;
		}//end if               
                
		gp->w=gp->w0;
		gp=gp->next;
        }//end while
        //////////////////////////////

        return;
}

void initCtrlData(struct capastr *capaData)
{
	struct capastr *cp=capaData;
	while(cp!=NULL)
	{
		cp->Q=cp->Q0;
		cp=cp->next;
	}//end while
	/////////////////////////

	return;
}
	
void initCtrlData(struct genrstr *genrData)
{
	struct genrstr *gp;
	struct nodestr *np;
	////////////////////////////

	gp=genrData;
	while(gp!=NULL)
	{
		if(gp->w0<SMLL)
		{
			np=findData(gp->i,nodeData,PVSLACK);
			if(np==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }

			if(np->attr==PV)
			{
				gp->w0=np->p/(1.0-gp->puf);
				gp->plant=gp->w0-np->p;
			}
		}//end if

		if(gp->v0<SMLL)
		{
			np=findData(gp->i,nodeData,PVSLACK);
			if(np==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }

			if(np->attr==PV)    gp->v0=np->qv;
			if(np->attr==SLACK) gp->v0=np->p;
		}//end if

		gp->w=gp->w0;
		gp->v=gp->v0;

		gp=gp->next;
	}//end while
	/////////////////////////////////

	return;
}

void initCtrlData(struct xfmrstr *xfmrData)
{
	struct linestr  *lp;
	struct taptystr *tp;
	struct xfmrstr  *xp;
	//////////////////////////////////////
	
	xp=xfmrData;
	while(xp!=NULL)
	{
		if(xp->K0<SMLL)
		{
			lp=findData(xp->i,xp->j,lineData);
			if(lp==NULL)
			{
	 			printf("\n\nError! --- %ld",__LINE__); 
				printf("\n  Invalid xfmr: ibs=%ld, zbs=%ld, id=%s",xp->i,xp->j,xp->id);			
				exit(0); 
			}//end if

			xp->K0=lp->b0k;
		}//end if
		/////////////////////////////////////////

		tp=chainNode(taptyData,xp->itapty);
		if(tp==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }

		xp->Kmax=ratioFun(xp,tp,tp->mxtap);
		xp->Kmin=ratioFun(xp,tp,tp->mntap);

		xp->K=xp->K0;
		xp=xp->next;
	}//end while
	//////////////////////////////////

	return;
}

long reguNum(struct xfmrstr *xfmrData)
{
	long ret=0;//init.
	double aux2;

	struct xfmrstr *xp;
	//////////////////////////////////

	xp=xfmrData;
	while(xp!=NULL)
	{
		aux2=fabs(xp->K-xp->K0);
		if(aux2>=xfmr_eps) ret++;
		
		xp=xp->next;
	}//end while
	//////////////////////////////

	return ret;
}

long reguNum(struct capastr *capaData)
{
	long ret=0;//init.
	double aux2;

	struct capastr *cp;
	//////////////////////////////////

	cp=capaData;
	while(cp!=NULL)
	{
		aux2=fabs(cp->Q-cp->Q0);
		if(aux2>=capa_eps) ret++;
		
		cp=cp->next;
	}//end while
	//////////////////////////////

	return ret;
}

long reguNum(struct genrstr *genrData,long swtch)
{
	long ret=0;//init.
	double aux2;

	struct genrstr *gp;
	/////////////////////////////////////

	gp=genrData;
	while(gp!=NULL)
	{
		if(swtch==GENR_POWR)
		{
			aux2=fabs(gp->w-gp->w0);
			if(aux2>=genw_eps) ret++;
		}//end if

		if(swtch==GENR_VOLT)
		{
			aux2=fabs(gp->v-gp->v0);
			if(aux2>=genv_eps) ret++;
		}//end if
		
		gp=gp->next;
	}//end while
	/////////////////////////////////

	return ret;
}

long isTerminalNode(long i,struct linestr *lineData)
{
	long bs,ibs,zbs,ret,k;
	struct linestr *lp;
	/////////////////////////////////////

	bs=abs(i);
	k=ibs=zbs=0;//init.
	
	lp=lineData;
	while(lp!=NULL)
	{
		if(brMatch(lp,ibs,zbs)==YES){ lp=lp->next; continue; }

		ibs=abs(lp->i);
		zbs=abs(lp->j);

		if(bs==ibs || bs==zbs) k++;
		lp=lp->next;
	}//end while 

	if(k==1) ret=YES;
	else ret=NO;
	/////////////////////////////////////

	return ret;
}

void setNodeData(char *node_id,double w,long attr,struct nodestr *nodeData)
{
	struct nodestr *np;

	strcpy(idbuf,node_id);//pretreatment
	stringEnd(idbuf,' ');

        np=findData(idbuf,nodeData,attr);
        if(np==NULL)
	{ 
		printf("\n\nError! --- %ld",__LINE__); 
		printf("\n  NodeData NOT found: id=%s, attr=%ld",node_id,attr);
	
		exit(0); 
	}//end if
	///////////////////////////////////////////////

	if(np->attr!=SLACK && fabs(np->p)>SMLL)
	{
                printf("\n\nError! --- %ld",__LINE__);
		printf("\n  NodeData NOT zero: id=%s, w=%lf",np->id,np->p);
		
		exit(0);
	}//end if

        np->p=w;//PQ or PV
	////////////////////////////////////

	return;
}

void setGroupLmt(char *group_id,char *mode_id,double mode_lmt,struct groupstr *groupData)
{
	register long i;
	struct groupstr *grp;
	////////////////////////////

	//1: find the group

	strcpy(idbuf,group_id);
	stringEnd(idbuf,' ');

	grp=groupData;
	while(grp!=NULL)
	{
		strcpy(buf,grp->id);
		stringEnd(buf,' ');

		if(!strcmp(idbuf,buf)) break;
		grp=grp->next;
	}//end while
	////////////////////////////////

	//2: set mode_lmt

	strcpy(idbuf,mode_id);
	stringEnd(idbuf,' ');

	for(i=1;i<=grp->modeNum;i++)
	{
		strcpy(buf,grp->mode[i].id);
		stringEnd(buf,' ');

		if(!strcmp(idbuf,buf))
		{
			grp->mode[i].mwmax=mode_lmt;
			break;
		}//end if
	}//end for	
	////////////////////////

	return;
}
 
long veriFlowData(struct flowstr *subFlowData,struct flowstr *flowData)
{
		register long i;

		long ret=YES;//init.
		double w1,r1,w2,r2,w_err,r_err;

        struct flowstr *fp,*sp;
		///////////////////////////////////////////////

        fp=flowData;
		for(i=1;i<=lineNum;i++,fp=fp->next)
        {
				//1: parallel-branch verification

                sp=findData(fp->id,subFlowData);
                if(sp==NULL) continue;
				/////////////////////////////////////

				//2: summing up powers

                w1=w2=0.0;//init.
                r1=r2=0.0;

                sp=subFlowData;
                while(sp!=NULL)
                {
                        if(brMatch(fp,sp->i,sp->j)==YES)
                        {
                                w1+= sp->w1;
                                w2+= sp->w2;

                                r1+= sp->r1;
                                r2+= sp->r2;
                        }//end if

                        sp=sp->next;
                }//end while
                /////////////////////////////////////

				//3: 

                w_err=max(fabs(fp->w1-w1),fabs(fp->w2-w2));
                r_err=max(fabs(fp->r1-r1),fabs(fp->r2-r2));

                if(w_err>flow_eps || r_err>flow_eps)
                {
                        printf("\n\nError! --- %ld",__LINE__);

                        printf("\n  flowData: id=%s, ibs=%ld, zbs=%ld",fp->id,fp->i,fp->j);
						printf("\n  branch.w1=%lf, sub.w1=%lf",fp->w1,w1);
												
						ret=NO;
						break;
                }//end if
        }//end for
		//////////////////////////////////////////

		return ret;
}

void calFlowData(struct flowstr* &_flowData,struct linestr *_lineData)
{
	//1: free the chain if necessary

	if(_flowData!=NULL) freeChain(_flowData);
	////////////////////////////////////////////////////

	//2: local variables

    long kbFlg,vioFlg;
    double w1,w2,r1,r2,s1,s2,pf1,pf2,aux2;

    struct flowstr *fp;
    struct linestr *lp;
    //////////////////////////////////////////////////

	//3: creating flow chain by _lineData

    _flowData=NULL;//init.
    lp=_lineData;

    while(lp!=NULL)
    {
        //.1 cal. branch powers

		lineFlow(w1,r1,w2,r2,s1,s2,pf1,pf2,lp);
		////////////////////////////////////////////////////

		//.2 setting kbFlg and vioFlg

        kbFlg=NO;//init.
        vioFlg=NO;

        if(lp->lmt>SMLL)
		{
			aux2=max(fabs(s1),fabs(s2));
	 
			if(aux2>lp->lmt*KB_COEFF) kbFlg=YES;
			if(aux2>lp->lmt) vioFlg=YES;
		}//end if
        ////////////////////////////////////////////////////////

		//.3 memory allocation for chainnode

        fp=(struct flowstr *)malloc(sizeof(struct flowstr));
        if(fp==NULL){ printf("\n\nError! --- %ld",__LINE__); exit(0); }

		memset(fp,0,sizeof(struct flowstr));
		//////////////////////////////////////////////

		//.4 filling in the buf.

        strcpy(fp->descr,lp->descr);
        strcpy(fp->id,lp->id);

        fp->i=lp->i;
        fp->j=lp->j;

        fp->w1=w1;
        fp->r1=r1;

        fp->w2=w2;
        fp->r2=r2;

        fp->s1=s1;
        fp->s2=s2;

        fp->pf1=pf1;
        fp->pf2=pf2;

        fp->kbFlg  = kbFlg;
        fp->vioFlg = vioFlg;
		/////////////////////////////////////

		//.5 chain increases

        fp->next=_flowData;
        _flowData=fp;
        //////////////////////////////

		//.6 next branch
		lp=lp->next;
	}//end while
	//////////////////////////////////////////////////

	//4: turning over the chain
        
	turnOver(_flowData);
	//////////////////////////////////

	return;
}

void addSubData(struct linestr *&subLineData,struct linestr *lp)
{
        struct linestr *sp;

        sp=findData(lp->id,subLineData);
        if(sp!=NULL) return;
        ////////////////////////////////////////////

        sp=(struct linestr *)malloc(sizeof(struct linestr));
        if(sp==NULL){ printf("\n\nError! --- %ld",__LINE__); exit(0); }

        memcpy(sp,lp,sizeof(struct linestr));
        
		sp->next=subLineData;
        subLineData=sp;
        ////////////////////////////////////

        return;
}

void combinLineData(struct linestr *lineData,struct linestr *&subLineData)
{
        if(subLineData!=NULL)
        {
                freeChain(subLineData);
                subLineData=NULL;
        }//end if
        ////////////////////////////////////////

	struct linestr *lp,*lp2,*next;
        lp=lineData;

    	while(lp!=NULL)
    	{
        	lp2=lp->next;
        	while(lp2!=NULL)
 	       	{
            		if(brMatch(lp,lp2->i,lp2->j)==YES)
            		{
                                addSubData(subLineData,lp);
                                addSubData(subLineData,lp2);

                                combinData(lp,lp2);// lp <== lp + lp2
                                next=lp2->next;

                                delChainNode(lineData,lp2);
                                lp2=next;
                        }
                        else
                                lp2=lp2->next;
	        }//end while

 	       lp=lp->next;
    	}//end while
        ///////////////////////////////

        turnOver(subLineData);
    	return;
}

void print_ctrlparm(FILE *out,long prnt)
{
	if(prnt==YES) printf("\n\nprint_ctrlparm()...");

	fprintf(out,"DE datafile for %s",project);
	fprintf(out,"\n   %9.4lf  av_volt",av_volt);
	fprintf(out,"\n   %9.4lf  flow_eps",flow_eps);

	fprintf(out,"\n   %9.4lf  loss_ratio",loss_ratio);
	fprintf(out,"\n   %9.4lf  ov_coeff",ov_coeff);
	fprintf(out,"\n   %9ld  opf_island",opf_island);

	//fprintf(out,"\n   %9.4lf  genw_eps",genw_eps);
	//fprintf(out,"\n   %9.4lf  genw_maxstep",genw_maxstep);
	//fprintf(out,"\n   %9.4lf  genv_eps",genv_eps);
	//fprintf(out,"\n   %9.4lf  genv_maxstep",genv_maxstep);
	//fprintf(out,"\n   %9.4lf  xfmr_eps",xfmr_eps);
	//fprintf(out,"\n   %9.4lf  xfmr_maxstep",xfmr_maxstep);
	//fprintf(out,"\n   %9.4lf  capa_eps",capa_eps);
	//fprintf(out,"\n   %9.4lf  capa_maxstep",capa_maxstep);

	return;
}

void print_lineData(FILE *out,long prnt)
{
	if(prnt==YES) printf("\n\nprint_lineData()...");

	fprintf(out,"\nDE LINEDATA");
	fprintf(out,LINE_TITL);

    struct linestr *lp=lineData;
    while(lp!=NULL)
    {
            fprintf(out,LINE_FORM,lp->i,lp->j,lp->r,lp->x,lp->b0k,lp->lmt,lp->vl,lp->id);

            lp=lp->next;
    }//end while
	////////////////////////////////////////////////

	return;
}

void print_nodeData(FILE *out,long prnt)
{
	if(prnt==YES) printf("\n\nprint_nodeData()...");

    fprintf(out,"\nDE NODEDATA");
    struct nodestr *np=nodeData;
    while(np!=NULL)
    {
            fprintf(out,NODE_FORM,np->i,np->attr,np->p,np->qv,np->id);

            np=np->next;
    }//end while
    /////////////////////////////////////////////

	return;
}

void print_capaData(FILE *out,long prnt)
{
	if(prnt==YES) printf("\n\nprint_capaData()...");

    fprintf(out,"\nDE CAPADATA");
    fprintf(out,CAPA_TITL);//the title

    struct capastr *cp=capaData;
    while(cp!=NULL)
    {
            fprintf(out,CAPA_FORM,cp->i,cp->Q0,cp->Qmin,cp->Qmax,cp->id);
            cp=cp->next;
    }//end while
    //////////////////////////////////////////////

	return;
}

void print_genrData(FILE *out,long prnt)
{
	if(prnt==YES) printf("\n\nprint_genrData()...");

    double Pmin,Pmax,rampUp,rampDn;
    double Qmin,Qmax,Vmin,Vmax;

    struct genrstr *gp;
	/////////////////////////////////////////////////////

    fprintf(out,"\nDE GENRDATA");
    fprintf(out,"\nDE ibs  |xd2   |w0   |w    |wmin  |wmax  |rampUp  |rampDn  |id   | st");
	
	gp=genrData;
    while(gp!=NULL)
    {
		Pmin=gp->wmin;
		Pmax=gp->wmax;
		rampUp=gp->rampUp;
		rampDn=gp->rampDn;

		Qmin=gp->rmin;
		Qmax=gp->rmax;
		Vmin=gp->vmin;
		Vmax=gp->vmax;

		fprintf(out,"\n   %4ld %6.3lf %6.2lf %6.2lf %6.2lf %6.2lf %6.2lf %6.2lf %s %s",
			gp->i,gp->xd2,gp->w0,gp->w,Pmin,Pmax,rampUp,rampDn,gp->id,gp->st_id);
		
		gp=gp->next;
	}//end while
	///////////////////////////////////////////

	return;
}

void print_priceData(FILE *out,long prnt)
{
	if(prnt==YES) printf("\n\nprint_priceData()...");

	register long i;

	struct pricestr *pp;
	////////////////////////////////////////////////

	fprintf(out,"\nDE PRICEDATA");
	fprintf(out,PRIC_TITL);

        pp=priceData;
        while(pp!=NULL)
        {
                fprintf(out,PRIC_FORM,pp->i,pp->pntnum,pp->Pb,pp->Cb,pp->id);
                fprintf(out,"   ");
		
                for(i=1;i<=pp->pntnum;i++)
                        fprintf(out," %5.2lf",pp->point[i].x);//x_axis --- power

                fprintf(out,"   ");
                for(i=1;i<=pp->pntnum;i++)
                        fprintf(out," %5.2lf",pp->point[i].y);//y_axis --- price

                pp=pp->next;
        }//end while
        ///////////////////////////////////////////////////

		return;
}

void print_groupData(FILE *out,long prnt)
{
	if(prnt==YES) printf("\n\nprint_groupData()...");

	register long i;

	long ibs,zbs,imst;

	fprintf(out,"\nDE GROUPDAT");
	fprintf(out,"\nDE id   |lineNum |mwmin  |mwmax  | descr  ||ln_ ibs zbs |ln_imst");

	struct groupstr *grp=groupData;
	while(grp!=NULL)
	{
		fprintf(out,"\n   %s %4ld %7.2lf %7.2lf %s",grp->id,grp->lineNum,grp->mwmin,grp->mwmax,grp->descr);
		for(i=1;i<=grp->lineNum;i++)
		{
			ibs =grp->line[i].i;
			zbs =grp->line[i].j;
			
			imst=grp->line[i].imst;
			strcpy(idbuf,grp->line[i].id);

			fprintf(out,"\n   %4ld %4ld imst=%4ld, id=%s",ibs,zbs,imst,idbuf);
		}//end for

		grp=grp->next;
	}//end while

	if(prnt==YES) printf("\n groupData ok");
	///////////////////////////////////////////////////////////

	fprintf(out,"\nDE GROUPMOD");
	fprintf(out,"\nDE id   |modeNum |descr || mode_id |mode_lmt |conditions #1 --- #5");

	long mel; 
	double val;

	grp=groupData;
	while(grp!=NULL)
	{
		fprintf(out,"\n   %s %4ld %s",grp->id,grp->modeNum,grp->descr);
		for(i=1;i<=grp->modeNum;i++)
		{
                	fprintf(out,"\n     %s %7.2lf",grp->mode[i].id,grp->mode[i].mwmax);
			
                	strcpy(idbuf,grp->mode[i].dev_id1);
                	mel=grp->mode[i].mel1;
                	val=grp->mode[i].val1;
                	fprintCondition(out,idbuf,mel,val);

                	strcpy(idbuf,grp->mode[i].dev_id2);
                	mel=grp->mode[i].mel2;
                	val=grp->mode[i].val2;
                	fprintCondition(out,idbuf,mel,val);

                	strcpy(idbuf,grp->mode[i].dev_id3);
                	mel=grp->mode[i].mel3;
                	val=grp->mode[i].val3;
                	fprintCondition(out,idbuf,mel,val);

			strcpy(idbuf,grp->mode[i].dev_id4);
                        mel=grp->mode[i].mel4;
                        val=grp->mode[i].val4;
                        fprintCondition(out,idbuf,mel,val);

			strcpy(idbuf,grp->mode[i].dev_id5);
                        mel=grp->mode[i].mel5;
                        val=grp->mode[i].val5;
                        fprintCondition(out,idbuf,mel,val);
		}//end for
	
		grp=grp->next;
	}//end while

	if(prnt==YES) printf("\n group mode ok");
	///////////////////////////////////////////////////////

	return;
}

void print_xfmrData(FILE *out,long prnt)
{
	if(prnt==YES) printf("\n\nprint_xfmrData()...");

	fprintf(out,"\nDE XFMRDATA");
	fprintf(out,"\nDE ibs | zbs | tap | tapty | K0  | Kmin | Kmax | kvnom | zkvnom | kvvl | zkvvl | id");

	struct xfmrstr *xp=xfmrData;  
	while(xp!=NULL)
	{
		fprintf(out,"\n   %4ld %4ld %4ld %4ld %7.3lf %7.3lf %7.3lf %7.2lf %7.2lf %7.2lf %7.2lf %s",
			xp->i,xp->j,xp->tap,xp->itapty,xp->K0,xp->Kmin,xp->Kmax,xp->kvnom,xp->zkvnom,xp->kvvl,xp->zkvvl,xp->id);
		xp=xp->next;
	}//end while

	if(prnt==YES) printf("\n xfmrData ok");
	/////////////////////////////////////////////////////////

	fprintf(out,"\nDE TAPTYDAT");
	fprintf(out,"\nDE num |mntap |mxtap |nom |step |id");
	
	struct taptystr *tp=taptyData;
	long k=1;//init.

	while(tp!=NULL)
	{
		fprintf(out,"\n   %4ld %4ld %4ld %4ld %9.4lf %s",
				k,tp->mntap,tp->mxtap,tp->nom,tp->step,tp->id);
		k++;
		tp=tp->next;
	}//end while

	if(prnt==YES) printf("\n taptyData ok");
	//////////////////////////////////////////////////////

	return;
}

void printDatafile(char *datafile,long prnt)
{
	if(prnt==YES) printf("\n\nprintDatafile()...");

	FILE *out=fopen(datafile,"w");
	if(out==NULL){ printf("\nCannot open %s",datafile); exit(0); }
	///////////////////////////////////////////////////////////

	print_ctrlparm(out,NO);//prnt
	if(prnt==YES) printf("\n print_ctrlparm() ok");

	print_lineData(out,NO);//prnt
	if(prnt==YES) printf("\n print_lineData() ok");

	print_nodeData(out,NO);//prnt
	if(prnt==YES) printf("\n print_nodeData() ok");
	///////////////////////////////////////////////////////////

	print_genrData(out,NO);//prnt
	if(prnt==YES) printf("\n print_genrData() ok");

	print_priceData(out,NO);//prnt
	if(prnt==YES) printf("\n print_priceData() ok");
	///////////////////////////////////////////////////////////

	print_capaData(out,NO);//prnt
	if(prnt==YES) printf("\n print_capaData() ok");

	print_xfmrData(out,NO);//prnt
	if(prnt==YES) printf("\n print_xfmrData() ok");

	print_voltData(out,NO);//prnt
	if(prnt==YES) printf("\n print_voltData() ok");
	///////////////////////////////////////////////////////////

	print_groupData(out,NO);//prnt
	if(prnt==YES) printf("\n print_groupData() ok");

	print_loadData(out,NO);//prnt
	if(prnt==YES) printf("\n print_loadData() ok");

	print_shortData(out,NO);//prnt
	if(prnt==YES) printf("\n print_shortData() ok");
	///////////////////////////////////////////////////////////

	fprintf(out,"\nDE end of file");
	fclose(out);
	
	return;
}

void print_shortData(FILE *out,long prnt)
{
	if(prnt==YES) printf("\n\nprint_shortData()...");

	fprintf(out,"\nDE SHORTDATA");
	fprintf(out,"\nDE ibs   |r      |x     |id     |");

	struct shortstr *sp=shortData;
	while(sp!=NULL)
	{
		fprintf(out,"\n   %4ld %7.3lf %7.3lf %s",sp->i,sp->r,sp->x,sp->id);
		sp=sp->next;
	}//end while
	///////////////////////////////////////

	return;
}

void print_loadData(FILE *out,long prnt)
{
	if(prnt==YES) printf("\n\nprint_loadData()...");

	fprintf(out,"\nDE LOADDATA");
	fprintf(out,LOAD_TITL);

	struct loadstr *ldp=loadData;
	while(ldp!=NULL)
	{
		fprintf(out,LOAD_FORM,ldp->i,ldp->w,ldp->r,ldp->id);
		ldp=ldp->next;
	}//end while
	////////////////////////////////////////////

	return;
}

void print_voltData(FILE *out,long prnt)
{
	if(prnt==YES) printf("\n\nprint_voltData()...");
	
	fprintf(out,"\nDE VOLTDATA");
	fprintf(out,VOLT_TITL);

	struct voltstr *vp=voltData;
	while(vp!=NULL)
	{
		fprintf(out,VOLT_FORM,vp->i,vp->Vmin,vp->Vmax,vp->id);
		vp=vp->next;
	}//end while
	////////////////////////////////////////

	return;
}

void fprintCondition(FILE *out,char id[],long mel,double value)
{
	if(id==NULL || strlen(id)==0) return;

	if(mel==EQ) fprintf(out," %s = %6.2lf",id,value);
	if(mel==LT) fprintf(out," %s < %6.2lf",id,value);
	if(mel==MT) fprintf(out," %s > %6.2lf",id,value);

	return;
}

double calPi(double **G,double **B,double *vv,double *v0,long i)
{
        register long j;
        double ret,sum,theta;
	//////////////////////////////////////

        sum=0.0;
        for(j=1;j<=nodeNum;j++)
        {
                theta=v0[i]-v0[j];
                sum+= vv[j]*(G[i][j]*cos(theta)+B[i][j]*sin(theta));
        }//end for

        ret=vv[i]*sum;//Pi obtained
        /////////////////////////////////

        return ret;
}

double calQi(double **G,double **B,double *vv,double *v0,long i)
{
        register long j;
        double ret,sum,theta;
	/////////////////////////////////////

        sum=0.0;
        for(j=1;j<=nodeNum;j++)
        {
                theta=v0[i]-v0[j];
                sum+= vv[j]*(G[i][j]*sin(theta)-B[i][j]*cos(theta));
        }//end for

        ret=vv[i]*sum;//Qi obtained
        //////////////////////////////

        return ret;
}

void reportGenrReactiv(FILE *out,struct genrstr *genrData,long prnt)
{
	long width=80;
	////////////////////////////////////

	fprintf(out,"\n\nGenerator Reactive 发电机无功");
	fdrawline(out,width);
	fprintf(out,"\n ibs | P0  |plant |Qmin | Qmax | Q0   | Q    | krFlg | vioFlg | id    | descr");
	fdrawline(out,width);

	struct genrstr *gp=genrData;
	while(gp!=NULL)
	{
		fprintf(out,"\n %4ld %7.4lf %7.4lf %7.4lf %7.4lf %7.4lf %7.4lf %4ld %4ld  %s  %s",
			gp->i,gp->w0,gp->plant,gp->rmin,gp->rmax,gp->r0,gp->r,gp->krFlg,gp->vioFlg,gp->id,gp->descr);
		
		gp=gp->next;
	}//end while

	fdrawline(out,width);
	/////////////////////////////////////////

	return;
}

void reportMonVoltage(FILE *out,struct voltstr *voltData,long prnt)
{
	long width=65;//init.
	struct voltstr *vp;
	////////////////////////////////////////////////

	fprintf(out,"\n\nVoltage Monitor 监视电压");
	fdrawline(out,width);
	
	fprintf(out,"\n ibs  | Vmin  | Vmax  | V0   | V    | kvFlg | vioFlg | id");
	fdrawline(out,width);
	////////////////////////////////////////

	vp=voltData;
	while(vp!=NULL)
	{
		fprintf(out,"\n%4ld % 8.4lf % 8.4lf % 8.4lf % 8.4lf %4ld %4ld  %s",
			vp->i,vp->Vmin,vp->Vmax,vp->V0,vp->V,vp->kvFlg,vp->vioFlg,vp->id);

		vp=vp->next;
	}//end while

	fdrawline(out,width);
	//////////////////////////////////////
	
	return;
}

void reportGroupFlow(FILE *out,struct groupstr *groupData,long prnt)
{
	long im_grp,kbFlg,vioFlg,width=55;
	double mw_grp,mwmax,mwmin;
	//////////////////////////////////////

	if(prnt==YES)
	{
		printf("\n\nGroup Flow Report 联络线族潮流");
		drawline(width);
		printf(FLOW_GROUP_TITL);
		drawline(width);
	}//end if

	fprintf(out,"\n\nGroup Flow Report 联络线族潮流");
	fdrawline(out,width);
	fprintf(out,FLOW_GROUP_TITL); 
	fdrawline(out,width);
	/////////////////////////////////////////

	struct groupstr *grp=groupData;
	while(grp!=NULL)
	{
		im_grp = grp->imode;
		mw_grp = grp->w;
		
		mwmin  = grp->mwmin;
		mwmax  = grp->mwmax;
		if(im_grp>0) mwmax=grp->mode[im_grp].mwmax;

		kbFlg =grp->kbFlg;
		vioFlg=grp->vioFlg;

		if(prnt==YES) printf(FLOW_GROUP_FORM,grp->id,mw_grp,im_grp,mwmin,mwmax,kbFlg,vioFlg);
		fprintf(out,FLOW_GROUP_FORM,grp->id,mw_grp,im_grp,mwmin,mwmax,kbFlg,vioFlg);
	
		grp=grp->next;
	}//end while

	if(prnt==YES) drawline(width);
	fdrawline(out,width);
	////////////////////////////////////////////

	return;
}
	
void reportLine(FILE *out,struct flowstr *flowData,struct flowstr *subFlowData,long prnt)
{ 
	long ibs,zbs,width=80;
	double w1,w2,r1,r2,s1,s2;
	////////////////////////////////////////////

	//1: flowData report

	fprintf(out,"\n\nLine Flow Report 支路潮流报表");
	fdrawline(out,width);
	fprintf(out,FLOW_LINE_TITL);
	fdrawline(out,width);
	/////////////////////////////////////

	struct flowstr *fp=flowData;
	while(fp!=NULL)
	{ 
		ibs=abs(fp->i);
		zbs=abs(fp->j); 

		fprintf(out,FLOW_LINE_FORM,ibs,zbs,fp->w1,fp->r1,fp->w2,fp->r2,fp->s1,fp->s2,fp->kbFlg,fp->vioFlg,fp->id);
		fp=fp->next;
	}//end while

	if(prnt==YES) drawline(width);
	fdrawline(out,width);
	///////////////////////////////////////////////
	
	//2: subFlowData report

	if(prnt==YES)
	{
		printf("\n\nSubLine Flow Report 并联支路潮流");
		drawline(width);
		printf(FLOW_SUBLINE_TITL);
		drawline(width);
	}//end if
 
        fprintf(out,"\n\nSubLine Flow Report 并联支路潮流");
        fdrawline(out,width);
        fprintf(out,FLOW_SUBLINE_TITL);
        fdrawline(out,width);
	/////////////////////////////////////////////

	fp=subFlowData;
	while(fp!=NULL)
        {
                ibs=abs(fp->i);
                zbs=abs(fp->j);

                w1=fp->w1;
                w2=fp->w2;
                r1=fp->r1;
                r2=fp->r2;
                s1=fp->s1;
                s2=fp->s2;

                if(prnt==YES) printf(FLOW_SUBLINE_FORM,ibs,zbs,w1,r1,w2,r2,s1,s2,fp->id);
                fprintf(out,FLOW_SUBLINE_FORM,ibs,zbs,w1,r1,w2,r2,s1,s2,fp->id);

                fp=fp->next;
        }//end while

        if(prnt==YES) drawline(width);
        fdrawline(out,width);
        //////////////////////////////////////

	return;
}

void reportNode(FILE *out,double *vv,double *v0,double *nodeP,double *nodeQ,long *codeList,long prnt)
{ 
	register long i,width=70;//init.

	double theta,aux2,aux4;
	double zero=0.0;//No Change!
	/////////////////////////////////

	if(prnt==YES)
	{
		printf("\n\nNode Flow Report 节点潮流报表");
		drawline(width);  

		printf(FLOW_NODE_TITL);
		drawline(width);
	}//end if
	/////////////////////////////////////

	fprintf(out,"\n\nNode Flow Report 节点潮流报表");
	fdrawline(out,width);

	fprintf(out,FLOW_NODE_TITL);
	fdrawline(out,width);
	//////////////////////////////////////////

	for(i=1;i<=nodeNum;i++)
	{ 
		aux2= nodeP[i]; 
		aux4= nodeQ[i];

		theta=v0[i]*180.0/PI;// rad ==> deg
   		//////////////////////////////////////////////////////////////
	 
		if(fabs(aux2)<SMLL && fabs(aux4)<SMLL) continue;

		if(aux2>SMLL || (fabs(aux2)<SMLL && aux4>SMLL))//generation
		{
                        if(prnt==YES) printf(FLOW_NODE_FORM,i,codeList[i],vv[i],theta,zero,zero,aux2,aux4);
                        fprintf(out,FLOW_NODE_FORM,i,codeList[i],vv[i],theta,zero,zero,aux2,aux4);
                }
		else//loads
		{ 
			if(prnt==YES) printf(FLOW_NODE_FORM,i,codeList[i],vv[i],theta,-aux2,-aux4,zero,zero);
			fprintf(out,FLOW_NODE_FORM,i,codeList[i],vv[i],theta,-aux2,-aux4,zero,zero);
		}//end if-else
	}//end for
	///////////////////////////////////////////////////

	if(prnt==YES) drawline(width);
    	fdrawline(out,width);

	return;
}

void reportSys(FILE *out,struct sysstr sys,long prnt)
{ 
	long width=45;
  
	fprintf(out,"\n\nSystem Information 系统信息");
	fdrawline(out,width);

	fprintf(out,"\n系统有功注入＝%lf",sys.wgen);
	fprintf(out,"\n    有功负荷＝%lf",sys.wload);
	fprintf(out,"\n    有功网损＝%lf",sys.sysLoss);

	fprintf(out,"\n    最高电压＝%lf, 所在节点＝%ld",sys.maxVolt,sys.imaxVolt);
	fprintf(out,"\n    最低电压＝%lf, 所在节点＝%ld",sys.minVolt,sys.iminVolt);
	fprintf(out,"\n最大支路损耗＝%lf, 所在支路＝%ld--%ld",sys.maxLoss,sys.ibsMaxLoss,sys.zbsMaxLoss);

	fdrawline(out,width);
	//////////////////////////////////////////////

  return;
}

void stepReduce(double *step,long n,double divstep)
{ 
	if(fabs(divstep)<SMLL){ printf("\n\nError! --- %ld",__LINE__); exit(0); }

	divVector(step,(long)1,n,divstep);
	return; 
}

void stepReduce(double *step,long n1,long n2,double divstep)
{ 	
	if(fabs(divstep)<SMLL){ printf("\n\nError! --- %ld",__LINE__); exit(0); }
		
	divVector(step,n1,n2,divstep); 
	return; 
}

template<class TYPE>
long vioNumStat(TYPE *chain)
{
        long ret=0;//init.
        TYPE *cp=chain;

        while(cp!=NULL)
        {
                if(cp->vioFlg==YES) ret++;
                cp=cp->next;
        }

        return ret;
}

long kbNumStat(struct flowstr *flowData)
{
        long ret=0;//init.
        struct flowstr *fp;
	/////////////////////////////////

	fp=flowData;
        while(fp!=NULL)
        {
                if(fp->kbFlg==YES) ret++;
        
	        fp=fp->next;
        }//end while
	///////////////////////

        return ret;
}

void getBranchGB(struct linestr *lp,double &brG,double &brB)
{
        double aux2=lp->r*lp->r + lp->x*lp->x;

        brG=lp->r/aux2;
        brB=-lp->x/aux2;
		
        return;
}

void formY(double **G,double **B,long *codeList,long nodeNum,long mod)
{
//Notes: by Wang GT
//  G,B[][]  --- admittance matrix, Y=G+jB[][]
//  codeList --- list of codes
//  mod = MODNO  --- no modifying
//        MODSCC --- mod. for scc 
//        MODVAR --- mod. for var.
///////////////////////////////////////////////////////

        register long i;
		long k,ii,jj;

        double aux,aux2,aux4,aux22,aux42;
		double K,K2;

		struct genrstr *gp;
        struct linestr *lp;
		struct xfmrstr *xp;
		////////////////////////////////////////////////////

		//1: init. matrix --- G,B[][]

        initMatrix(G,nodeNum,nodeNum,0.0);
        initMatrix(B,nodeNum,nodeNum,0.0);
		////////////////////////////////////////////////////

		//2: considering lineData

        lp=lineData;
        for(i=1;i<=lineNum;i++,lp=lp->next)
        {
                ii=lookup(codeList,lp->i,nodeNum);
                jj=lookup(codeList,lp->j,nodeNum);

                if(ii==NO || jj==NO){ printf("\nError! --- %s: %ld",__FILE__,__LINE__); exit(0); }
                ///////////////////////////////////////////////////////

                if(lp->i>0 && lp->j>0)//ordinary branch
                {
                        aux=lp->r*lp->r+lp->x*lp->x;

                        aux2=lp->r/aux;
                        aux4=lp->x/aux;

                        G[ii][jj]-= aux2;
                        G[jj][ii]-= aux2;
                        B[ii][jj]+= aux4;
                        B[jj][ii]+= aux4;

                        G[ii][ii]+= aux2;
                        G[jj][jj]+= aux2;
                        B[ii][ii]-= aux4;
                        B[jj][jj]-= aux4;

                        B[ii][ii]+= lp->b0k;
                        B[jj][jj]+= lp->b0k;
                }
                else//xfmr branch
                {
                        aux=lp->r*lp->r + lp->x*lp->x;

                        if(mod==MODVAR && (xp=findData(lp->i,lp->j,xfmrData))!=NULL)
                        {
                                K=xp->K;
                                K2=K*K;
                        }
                        else
                        {
                                K=lp->b0k;
                                K2=K*K;
                        }
                        ///////////////////////

                        aux2=lp->r/aux/K;
                        aux4=lp->x/aux/K;

                        G[ii][jj]-= aux2;
                        G[jj][ii]-= aux2;
                        B[ii][jj]+= aux4;
                        B[jj][ii]+= aux4;

                        G[ii][ii]+= aux2;
                        G[jj][jj]+= aux2;
                        B[ii][ii]-= aux4;
                        B[jj][jj]-= aux4;
                        /////////////////////////

                        aux2=(1.0-K)*lp->r/aux/K2;
                        aux4=(1.0-K)*lp->x/aux/K2;

                        aux22=(K-1.0)*lp->r/aux/K;
                        aux42=(K-1.0)*lp->x/aux/K;

                        if(lp->i<0)//K* at i_side
                        {
                                G[ii][ii]+= aux2;
                                B[ii][ii]-= aux4;

                                G[jj][jj]+= aux22;
                                B[jj][jj]-= aux42;
                        }
                        else//K* at j_side
                        {
                                G[jj][jj]+= aux2;
                                B[jj][jj]-= aux4;

                                G[ii][ii]+= aux22;
                                B[ii][ii]-= aux42;
                        }
                }//end if-else
        }//end while
        /////////////////////////////////////////////////////////

		//3: considering genrData --- xd2

		gp=genrData;
		for(i=1;i<=genrNum;i++,gp=gp->next)
		{
			//3.1 index to the genr.

			k=lookup(codeList,gp->i,nodeNum);
			if(k==NO)
			{
				printf("\n\nWarning! --- %ld",__LINE__); 
				printf("\n  Invalid genrData: i=%ld, id=%s",gp->i,gp->id);

				continue;
			}//end if
			/////////////////////////////////////////////

			//3.2 checking genr.parm: xd2

			if(fabs(gp->xd2)<SMLL)
			{
				printf("\n\nError! --- %ld",__LINE__);
				printf("\n  Invalid genrData: i=%ld, xd2=%lf",gp->i,gp->xd2);

				exit(0);
			}//end if
			/////////////////////////////////////////////

			//3.3 adding grnd. branch

			B[k][k]+= -1.0/gp->xd2;
		}//end for
        /////////////////////////////////////////////////////

        return;
}

void getdPowr(double *dPowr,long swch)
{
        register long i,j,ii;

        double sum,aux2,aux4;
		double theta,auxsin,auxcos;
		//////////////////////////////////////////////////////

        initVector(dPowr,nodeNum,0.0);//clearing
		
       	if(swch==P_V0)
 		{
                for(i=1;i<=nodeNum-1;i++)
                {
                        ii=i+getSlide(i,codeAttr,swch);

                        sum=0.0;
                        for(j=1;j<=nodeNum;j++)
                        {
                                theta=v0[ii]-v0[j];//No Change!
                                auxsin=theta-pow(theta,3.0)/6.0+pow(theta,5.0)/120.0;
                                auxcos=1.0-pow(theta,2.0)/2.0+pow(theta,4.0)/24.00;

                                aux2= G[ii][j]*auxcos;
                                aux4= B[ii][j]*auxsin;
                        
								sum += vv[j]*(aux2+aux4);
                        }//end for

                        dPowr[i]=(nodeP[ii]-vv[ii]*sum)/vv[ii];
                }//end for
		}//end if
		///////////////////////////////////////////

		if(swch==Q_VV)
		{
                for(i=1;i<=nodeNum-pvNodeNum-1;i++)
                {
                        ii=i+getSlide(i,codeAttr,swch);

                        sum=0.0;
                        for(j=1;j<=nodeNum;j++)
                        {
                                theta=v0[ii]-v0[j];//No Change!
                                auxsin=theta-pow(theta,3.0)/6.0+pow(theta,5.0)/120.0;
                                auxcos=1.0-pow(theta,2.0)/2.0+pow(theta,4.0)/24.00;

                                aux2= G[ii][j]*auxsin;
                                aux4= B[ii][j]*auxcos;
                                aux2= aux2-aux4;
                                sum+= vv[j]*aux2;
                        }//end for

                        dPowr[i]=(nodeQ[ii]-vv[ii]*sum)/vv[ii];
                }//end for
        }//end if
		///////////////////////////////////// 

		return;
}

long getSlide(long code,long *codeAttr,long swch)
{
        long i,attr,slide,count=0;//init.

        if(swch==P_V0 || swch==SKIP_SLCK)
        {
                if(code>=codeAttr[0]) slide=1;
                else slide=0;
        }
        else// if(swch==Q_VV || swch==SKIP_GENR)
        {
                for(i=1;i<=nodeNum;i++)
                {
                        attr=codeAttr[i];
                        if(attr!=SLACK && attr!=PV) count++;
                        
			if(count==code)
			{ 
				slide=i-count; 
				break; 
			}//end if
                }//end for
        }//end if-else
        ///////////////////////////////

        return slide;
}

void calGenrPowr(double *nodeP,double *nodeQ,long *codeList,long *codeAttr,struct flowstr *flowData)
{
        register long i;

		long ii,ibs,slack;
        double slackP,slackQ,pvQ;

        struct flowstr *fp;
		struct genrstr *gp;
		///////////////////////////////////////

		//1: genr. powers calculation

        slack=codeList[0];
        for(i=1;i<=nodeNum;i++)
        {
				//.1 for slack node --- P and Q

                if(codeAttr[i]==SLACK)
				{
                        slackP=0.0;
                        slackQ=0.0;

                        fp=flowData;
                        while(fp!=NULL)
                        {
                                if(abs(fp->i)==slack)
                                {
                                        slackP+= fp->w1;
                                        slackQ+= fp->r1;
								}
                                
								if(abs(fp->j)==slack)
                                {
                                        slackP+= fp->w2;
                                        slackQ+= fp->r2;
                                }

                                fp=fp->next;
                        }//end while

                        nodeP[i]=slackP;
                        nodeQ[i]=slackQ;
				}//end if
				////////////////////////////////////////

				//.2 for PV nodes --- Q only

				if(codeAttr[i]==PV)
				{
                        pvQ=0.0;
                        ibs=codeList[i];

                        fp=flowData;
                        while(fp!=NULL)
                        {
                                if(abs(fp->i)==ibs) pvQ+= fp->r1;
                                if(abs(fp->j)==ibs) pvQ+= fp->r2;

                                fp=fp->next;
                        }//end while

                        nodeQ[i]=pvQ;
				}//end if
		}//end for
		/////////////////////////////////////////////////

		//2: considering plant-use of slack

		gp=findData(slack,genrData);
		if(gp==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }

		ii=lookup(codeList,slack,nodeNum);
		if(ii==NO){ printf("\nError! --- %ld",__LINE__); exit(0); }

		gp->plant=gp->w0-nodeP[ii];
		///////////////////////////////////////

        return;
}

void formB2X(double **B2,long *codeList,long *codeAttr)
{
	long n,*tempList;
	n=nodeNum-pvNodeNum-1;//all genr. nodes skipped
        
	tempList=(long *)calloc(n+1,sizeof(long));
	if(tempList==NULL){ printf("\nAllocation Error! --- %ld",__LINE__); exit(0); }
	/////////////////////////////////////////////

	formTempList(tempList,codeList,codeAttr,SKIP_GENR);
	formB2X(B2,tempList,n);
	//////////////////////////

	free(tempList);	
	return;
}

void formB2X(double **B2,long *tempList,long n)
//resistance of line omitted
{
	long ii,jj;
	double K,K2;
	struct linestr *lp;
	/////////////////////////////

	initMatrix(B2,n,n,0.0);
	lp=lineData;
	
	while(lp!=NULL)
	{
                ii=lookup(tempList,lp->i,n);
                jj=lookup(tempList,lp->j,n);

                if(lp->i>0 && lp->j>0)//power line
                {
                        if(ii!=NO) B2[ii][ii]+= 1.0/lp->x+lp->b0k;
                        if(jj!=NO) B2[jj][jj]+= 1.0/lp->x+lp->b0k;

                        if(ii!=NO && jj!=NO)
                        {
                                B2[ii][jj]-= 1.0/lp->x;
                                B2[jj][ii]-= 1.0/lp->x;
                        }
                }
                else//xfmr.branch
                {
                        K=lp->b0k;      //ratio
                        K2=K*K;         //ratio^2

                        if(ii!=NO && jj!=NO)
                        {
                                B2[ii][jj]-= 1.0/(lp->x*K);
                                B2[jj][ii]-= 1.0/(lp->x*K);
                        }
                        ////////////////////////////////////

                        if(lp->i<0)//K* at i_side
                        {
                                if(ii!=NO)
                                {
                                        B2[ii][ii]+= 1.0/(lp->x*K);
                                        B2[ii][ii]+= (1.0-K)/(lp->x*K2);
                                }

                                if(jj!=NO)
                                {       B2[jj][jj]+= 1.0/(lp->x*K);
                                        B2[jj][jj]+= (K-1.0)/(lp->x*K);
                                }
                        }
                        else//K* at j_side
                        {
                                if(ii!=NO)
                                {
                                        B2[ii][ii]+= 1.0/(lp->x*K);
                                        B2[ii][ii]+= (K-1.0)/(lp->x*K);
                                }

                                if(jj!=NO)
                                {
                                        B2[jj][jj]+= 1.0/(lp->x*K);
                                        B2[jj][jj]+= (1.0-K)/(lp->x*K2);
                                }
                        }//end if-else
                }//end if-else
	
		lp=lp->next;
        }//end while 
        //////////////////////////

        return;
}

void formB1(double **B1,long *tempList,long n)
//C_branch and K* NOT considered
{
        long ii,jj;
        double aux2,aux4;
        //////////////////////////////////

        initMatrix(B1,n,n,0.0);//init.
        struct linestr *lp=lineData;
        
	while(lp!=NULL)
	{
                ii=lookup(tempList,lp->i,n);
                jj=lookup(tempList,lp->j,n);

                aux2=lp->r*lp->r+lp->x*lp->x;
                aux4=lp->x/aux2;//i.e. Bij
                ///////////////////////////////

                if(ii!=NO) B1[ii][ii]+= aux4;

                if(jj!=NO) B1[jj][jj]+= aux4;

                if(ii!=NO && jj!=NO)
                {
                        B1[ii][jj]-= aux4;
                        B1[jj][ii]-= aux4;
                }//end if
		
		lp=lp->next;
        }//end while 
        ////////////////////////

        return;
}

void formB1(double **B1,long *codeList,long *codeAttr)
{
        long n,*tempList;

        n=nodeNum-1;
        tempList=(long *)calloc(n+1,sizeof(long));

        if(tempList==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }
        ///////////////////////////////////

        formTempList(tempList,codeList,codeAttr,SKIP_SLCK);
        formB1(B1,tempList,n);
        ///////////////////////////

        free(tempList);
        return;
}
                                
void formTempList(long *tempList,long *codeList,long *codeAttr,long skip_swch)
{
        register long i,k=0;

        for(i=1;i<=nodeNum;i++)
        {
                if(skip_swch==SKIP_SLCK && codeAttr[i]==SLACK) continue;

                if(skip_swch==SKIP_GENR && codeAttr[i]==SLACK) continue;
                if(skip_swch==SKIP_GENR && codeAttr[i]==PV)    continue;

                k++;
                tempList[k]=codeList[i];
        }//end for
        /////////////////////////////

        return;
}

void formTempList(long *tempList,long *codeList,long skip_id)
{
        register long i,k;

        k=lookup(codeList,skip_id,nodeNum);
        if(k==NO){ printf("\n\nError! --- %ld",__LINE__); exit(0); }

        for(i=1;i<=k-1;i++)
                tempList[i]=codeList[i];

        for(i=k;i<=nodeNum-1;i++)
                tempList[i]=codeList[i+1];
        /////////////////////////

        return;
}

void readSysData(FILE *in,long prnt)
{
        if(prnt==YES) printf("\n\nreadSysData()...");
		
        //1: skipping comment lines

		do//unconditional loop
		{
			//1.1 end of file?

			if(feof(in))//end reached
			{
				printf("\n\nError! --- %ld",__LINE__);
				printf("\n  End of file reached.");

				exit(0);
			}//end if
			//////////////////////////////////////

			//1.2 comment line?

        	fgets(buf,3,in);
        	if(strcmp(buf,"DE")) break;

			fgets(buffer,LINELN,in);			
        }while(1);

		if(prnt==YES) printf("\n comment line(s) skipped");
		/////////////////////////////////////////////////////////

        //2: reading sys_data

		fscanf(in,"%lf",&av_volt);      //1
		fgets(buffer,LINELN,in);			
		
		fscanf(in,"%lf",&flow_eps);     //2
		fgets(buffer,LINELN,in);			
		
		fscanf(in,"%lf",&pwrbase);      //3
		fgets(buffer,LINELN,in);			
		
		fscanf(in,"%lf",&loss_ratio);   //4
		fgets(buffer,LINELN,in);			
		
		fscanf(in,"%lf",&div_coeff);    //5  
		fgets(buffer,LINELN,in);			
		
		fscanf(in,"%lf",&ov_coeff);     //6
		fgets(buffer,LINELN,in);			
		
		fscanf(in,"%lf",&genw_eps);     //7
		fgets(buffer,LINELN,in);			
		
		fscanf(in,"%lf",&genw_maxstep); //8
		fgets(buffer,LINELN,in);			
		
		fscanf(in,"%lf",&genv_eps);     //9
		fgets(buffer,LINELN,in);			
		
		fscanf(in,"%lf",&genv_maxstep); //10
		fgets(buffer,LINELN,in);			
		
		fscanf(in,"%lf",&capa_eps);     //11
		fgets(buffer,LINELN,in);			
		
		fscanf(in,"%lf",&capa_maxstep); //12
		fgets(buffer,LINELN,in);			
		
		fscanf(in,"%lf",&xfmr_eps);     //13
		fgets(buffer,LINELN,in);			
		
		fscanf(in,"%lf",&xfmr_maxstep); //14
		fgets(buffer,LINELN,in);			
		//////////////////////////////////////////////

		//3: infomation report

		if(prnt==YES) printf("\n av_volt  = %lf",av_volt);
		if(prnt==YES) printf("\n flow_eps = %lf",flow_eps);
		if(prnt==YES) printf("\n xfmr_eps = %lf",xfmr_eps);
		if(prnt==YES) printf("\n xfmr_maxstep = %lf",xfmr_maxstep);
		/////////////////////////////////////////

		return;
}

void OPF_readData(char datfile[],long prnt)
{
        if(prnt==YES) printf("\n\nOPF_readData()...");
        register long i;

        long ia,ib,ic,id,dataFlg=AAAACASE;//init.
        double fa,fb,fc,fd,fe,ff,fg,fh;

        FILE *in=fopen(datfile,"r");
        if(in==NULL){ printf("\nCannot open %s\n",datfile); exit(0); }
        ///////////////////////////////////////////////////////////

        //1: reading system information

        readSysData(in,NO);//prnt
        if(prnt==YES) printf("\n readSysData() ok");
        ////////////////////////////////////////////////

        //2: Reading other data

        struct capastr  *cp;   //for CAPADATA
        struct genrstr  *gp;   //for GENRDATA
        struct groupstr *grp;  //for GROUPDAT
        struct linestr  *lp;   //for LINEDATA
        struct nodestr  *np;   //for NODEDATA
        struct pricestr *pp;   //for PRICDATA
		struct taptystr *tp;
        struct voltstr  *vp;   //for VOLTDATA
        struct xfmrstr  *xp;   //for XFMRDATA

        capaData  = NULL;
        genrData  = NULL;
        groupData = NULL;
        lineData  = NULL;
        priceData = NULL;
        nodeData  = NULL;
		taptyData = NULL;
        voltData  = NULL;
        xfmrData  = NULL;

        do//flexible reading mechanism --- No Change!
        {
                do//data block recognition --> dataFlg
                {
                        fgets(buf,3,in);
                        if(strcmp(buf,"DE") || feof(in)) break;

                        fgets(buf2,10,in);
                        fgets(buffer,LINELN,in);
                        /////////////////////////////////////////////////////

                        if(!strcmp(buf2," CAPADATA")) dataFlg=CAPACASE;
                        if(!strcmp(buf2," CTG_DATA")) dataFlg=CTG_CASE;
                        if(!strcmp(buf2," GENRDATA")) dataFlg=GENRCASE;
                        if(!strcmp(buf2," GROUPDAT")) dataFlg=GROUPCAS;
                        if(!strcmp(buf2," LINEDATA")) dataFlg=LINECASE;
                        if(!strcmp(buf2," NODEDATA")) dataFlg=NODECASE;
                        if(!strcmp(buf2," PHASDATA")) dataFlg=PHASCASE;
                        if(!strcmp(buf2," PRICDATA")) dataFlg=PRICCASE;
						if(!strcmp(buf2," TAPTYDAT")) dataFlg=TAPTYCAS;
                        if(!strcmp(buf2," VOLTDATA")) dataFlg=VOLTCASE;
                        if(!strcmp(buf2," XFMRDATA")) dataFlg=XFMRCASE;
                }while(1);
                ///////////////////////////////////////////////////////////////

                switch(dataFlg)
                {
                case CAPACASE:
                        if(fscanf(in,"%ld%lf%lf%lf",&ia,&fa,&fb,&fc)==4)
                        {
                                cp=(struct capastr*)malloc(sizeof(struct capastr));
                                if(cp==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }
								
                                fgets(buffer,LINELN,in);
                                ///////////////////////////////////////////////////
								
                                delLeadingChar(buffer,' ');
                                stringEnd(buffer,'\n');

                                strcpy(cp->descr,"");
                                strcpy(cp->id,buffer);
                                //////////////////////////////////////

                                cp->i   =ia;
                                cp->Q0  =fa;

                                cp->Qmin=fb;
                                cp->Qmax=fc;


                                cp->next=capaData;
                                capaData=cp;
                        }
                        else
                                fgets(buffer,LINELN,in);

                        break;

                case GENRCASE:
                        if(fscanf(in,"%ld%lf%lf%lf%lf%lf%lf%lf%lf",&ia,&fa,&fb,&fc,&fd,&fe,&ff,&fg,&fh)==9)
                        {
                                gp=(struct genrstr*)malloc(sizeof(struct genrstr));
                                if(gp==NULL){ printf("\nError! --- %ld",__LINE__);  exit(0); }

                                fgets(buffer,LINELN,in);
                                /////////////////////////////////////////////////

                                delLeadingChar(buffer,' ');
                                stringEnd(buffer,'\n');

                                strcpy(gp->descr,"");
                                strcpy(gp->id,buffer);
                                //////////////////////////////////////

                                gp->i   =ia;//ibs

                                gp->wmin=fa;
                                gp->wmax=fb;
                                gp->rampUp=fc;
                                gp->rampDn=fd;

                                gp->rmin=fe;
                                gp->rmax=ff;						
                                gp->vmin=fg;
                                gp->vmax=fh;

                                gp->w0  =0.0;//init.
                                gp->v0  =0.0;

								gp->plant=0.0;
								gp->puf  =0.0;

                                gp->next=genrData;
                                genrData=gp;
                        }
                        else
                                fgets(buffer,LINELN,in);

                        break;

         		case GROUPCAS:
                        if(fscanf(in,"%ld%lf",&ia,&fa)==2)
						{
								grp=(struct groupstr *)malloc(sizeof(struct groupstr));
								if(grp==NULL){ printf("\nError! --- %ld",__LINE__);  exit(0); }
								
								fgets(buffer,LINELN,in);
								/////////////////////////////////////////////////////

								packString(buffer);
								stringEnd(buffer,'\n');

								strcpy(grp->descr,"");
								strcpy(grp->id,buffer);
								//////////////////////////////////

								grp->lineNum=ia;
								grp->mwmax  =fa;
										
								grp->mwmin  =0.0;//init.	
								grp->modeNum=0;//No Change!
								////////////////////////////////////////

								for(i=1;i<=grp->lineNum;i++)
								{
									if(fscanf(in,"%ld%ld",&ia,&ib)==2)
									{
										fgets(buffer,LINELN,in);
										
										packString(buffer);
										stringEnd(buffer,'\n');

										strcpy(grp->line[i].id,buffer);
										/////////////////////////////////////////

										grp->line[i].i=abs(ia);
										grp->line[i].j=abs(ib);

										grp->line[i].imst=abs(ia);//default
										if(ib<0) grp->line[i].imst=abs(ib);
									}//end if
								}//end for
								/////////////////////////////////////////////

								grp->next=groupData;
								groupData=grp;
						}
						else
							fgets(buffer,LINELN,in);

						break;

                case LINECASE:
                        if(fscanf(in,"%ld%ld%lf%lf%lf%lf",&ia,&ib,&fa,&fb,&fc,&fd)==6)
                        {
                                lp=(struct linestr*)malloc(sizeof(struct linestr));
                                if(lp==NULL){ printf("\nError! --- %ld",__LINE__);  exit(0); }

								fgets(buffer,LINELN,in);
                                /////////////////////////////////////////////////////

								delLeadingChar(buffer,' ');
								stringEnd(buffer,'\n');

								strcpy(lp->descr,"");
								strcpy(lp->id,buffer);
								//////////////////////////////////////

								lp->i   =ia;
                                lp->j   =ib;
                            
								lp->r   =fa;
                                lp->x   =fb;
                                lp->b0k =fc;
                                lp->lmt =fd;
								lp->vl  =10.0;//kV
								
                                lp->next=lineData;
                                lineData=lp;
                        }
                        else
                                fgets(buffer,LINELN,in);

                        break;

                case NODECASE:
                        if(fscanf(in,"%ld%ld%lf%lf",&ia,&ib,&fa,&fb)==4)
                        {
                                np=(struct nodestr*)malloc(sizeof(struct nodestr));
								if(np==NULL){ printf("\n\nError! --- %ld",__LINE__); exit(0); }
                                
								fgets(buffer,LINELN,in);
								//////////////////////////////////////////////////

								delLeadingChar(buffer,' ');
								stringEnd(buffer,'\n');

 								strcpy(np->descr,"");
								strcpy(np->id,buffer);
								//////////////////////////////////////

                                np->i=ia;

                                np->attr=ib;
                                np->p=fa;
                                np->qv=fb;

                                np->next=nodeData;
                                nodeData=np;
                        }
                        else
                                fgets(buffer,LINELN,in);

                        break;

                case PRICCASE:
                        if(fscanf(in,"%ld%ld%lf%lf",&ia,&ib,&fa,&fb)==4)
                        {
                                pp=(struct pricestr*)malloc(sizeof(struct pricestr));
                                if(pp==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }
								///////////////////////////////////////////////////////

								strcpy(pp->descr,"");
								strcpy(pp->id,"");

                                pp->i      =ia;
                                pp->pntnum =ib;

                                pp->Pb     =fa;
                                pp->Cb     =fb;

                                for(i=1;i<=pp->pntnum;i++)
                                {
                                        fscanf(in,"%lf",&fa);
                                        pp->point[i].x=fa;
                                }//end for

                                for(i=1;i<=pp->pntnum;i++)
                                {
                                        fscanf(in,"%lf",&fa);
                                        pp->point[i].y=fa;
                                }//end for

                                fgets(buffer,LINELN,in);
								//////////////////////////////////////

                                pp->next=priceData;
                                priceData=pp;
                        }
                        else
                                fgets(buffer,LINELN,in);

                        break;

				case TAPTYCAS:
                        if(fscanf(in,"%ld%ld%ld%lf",&ia,&ib,&ic,&fa)==4)
						{
								tp=(struct taptystr *)malloc(sizeof(struct taptystr));
								if(tp==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }

								fgets(buffer,LINELN,in);
								packString(buffer);
								////////////////////////////////////////

								strcpy(tp->descr,"");
								strcpy(tp->id,buffer);

								tp->mntap = ia;
								tp->mxtap = ib;
								tp->nom   = ic;
								tp->step  = fa;

								tp->next=taptyData;
								taptyData=tp;
						}
						else
							fgets(buffer,LINELN,in);

                        break;

                case VOLTCASE:
                        if(fscanf(in,"%ld%lf%lf",&ia,&fa,&fb)==3)
                        {
                                vp=(struct voltstr *)malloc(sizeof(struct voltstr));
                                if(vp==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }
                                
								fgets(buffer,LINELN,in);
								packString(buffer);
								///////////////////////////////////////

								strcpy(vp->descr,"");
								strcpy(vp->id,buffer);
								
                                vp->i   =ia;
                                vp->Vmin=fa;
                                vp->Vmax=fb;

                                vp->next=voltData;
                                voltData=vp;
                        }
                        else
                                fgets(buffer,LINELN,in);

                        break;

                case XFMRCASE:
                        if(fscanf(in,"%ld%ld%ld%ld%lf%lf%lf%lf",&ia,&ib,&ic,&id,&fa,&fb,&fc,&fd)==8)
                        {
                                xp=(struct xfmrstr *)malloc(sizeof(struct xfmrstr));
                                if(xp==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }

								memset(xp,0,sizeof(struct xfmrstr));
                                ////////////////////////////////////////////////////

								fgets(buffer,LINELN,in);
								packString(buffer);

								strcpy(xp->descr,"");
                                strcpy(xp->id,buffer);
								////////////////////////////////////

                                xp->i      =ia;
                                xp->j      =ib;
								xp->tap    =ic;
								xp->itapty =id;

								xp->kvnom  =fa;
								xp->zkvnom =fb;
								xp->kvvl   =fc;
								xp->zkvvl  =fd;

                                xp->next=xfmrData;
                                xfmrData=xp;
                        }
                        else
                                fgets(buffer,LINELN,in);

                        break;

                default:
                        break;
                }//end switch
        }while(!feof(in));

        fclose(in);//datfile closed
        ////////////////////////////////////////

        turnOver(capaData);
        turnOver(genrData);
        turnOver(groupData);

        turnOver(lineData);
        turnOver(nodeData);
        turnOver(priceData);
        turnOver(voltData);
        turnOver(xfmrData);

        if(prnt==YES) printf("\n Other data ok");
        /////////////////////////////////////////////

        return;
}
                                  
void getNodePowr(double *nodeP,double *nodeQ,long *codeList,long mod)
{
        long ii,n=nodeNum;//init.
        
        struct capastr *cp;
        struct genrstr *gp;
        struct nodestr *np;
        //////////////////////////////////////////

        //1: init. vectors

        initVector(nodeP,n,0.0);
        initVector(nodeQ,n,0.0);
        ////////////////////////////////

        //2: considering nodeData 

        np=nodeData;
        while(np!=NULL)
        {
                ii=lookup(codeList,np->i,n);
                if(ii==NO){ printf("\nError! --- %ld",__LINE__); exit(0); }

                if(np->attr==PV)
                {
                        if(mod==MODPG && (gp=findData(np->i,genrData))!=NULL)
                                nodeP[ii]+= gp->w;
                        else
                                nodeP[ii]+= np->p;
                }//end if

                if(np->attr==PQ)
                {
                        nodeP[ii]-= np->p;
                        nodeQ[ii]-= np->qv;
                }//end if

                np=np->next;
        }//end while
        //////////////////////////////////////

        //3: considering capaData 

        cp=capaData;
        while(cp!=NULL)
        {
                ii=lookup(codeList,cp->i,n);
                if(ii==NO){ printf("\nError! --- %ld",__LINE__); exit(0); }

                if(mod==MODVAR)
                        nodeQ[ii]+= cp->Q;
                else
                        nodeQ[ii]+= cp->Q0;

                cp=cp->next;
        }//end while
        //////////////////////////////////

        return;
}

double slack_adeg(struct nodestr *nodeData)
{
	double ret=0.0;//init.
	struct nodestr *np;
	////////////////////////////////

	np=nodeData;
	while(np!=NULL)
	{
		if(np->attr==SLACK)
		{	
			ret=np->qv;
			break;
		}//end if

		np=np->next;
	}//end while
	/////////////////////////////

	return ret;
}	

void initVolt(double *vv,double *v0,long *codeList,long prnt)
{
	if(prnt==YES) printf("\n\ninitVolt()...");

	long k; double adeg;
	struct nodestr *np;
	///////////////////////////////////////////////

        //1: average start-up

        initVector(vv,nodeNum,av_volt);
        if(prnt==YES) printf("\n vv[] init. ok");

        adeg=slack_adeg(nodeData);
        if(prnt==YES) printf("\n slack_adeg=%lf",adeg);

        initVector(v0,nodeNum,adeg*PI/180.0);//rad <== deg
        if(prnt==YES) printf("\n v0[] init. ok");
        //////////////////////////////////////////////////////

        //2: considering genr. voltages

        np=nodeData;
        while(np!=NULL)
        {
                if(np->attr==SLACK)
                {
                        k=lookup(codeList,np->i,nodeNum);
                        if(k==NO){ printf("\nError! --- %ld",__LINE__); exit(0); }

                        vv[k]=np->p;
                }//end if

                if(np->attr==PV)
                {
                        k=lookup(codeList,np->i,nodeNum);
                        if(k==NO){ printf("\nError! --- %ld",__LINE__); exit(0); }

                        vv[k]=np->qv;
                }//end if

                np=np->next;
        }//end while
       	///////////////////////////////////
 
        return;
}

void modiInitVolt(double *vv,long *codeList,long mod)
{
	if(mod!=MODVAR) return;//No Change!

	long k; 
        struct genrstr *gp;
	///////////////////////////////

        gp=genrData;
        while(gp!=NULL)
        {
        	k=lookup(codeList,gp->i,nodeNum);
		if(k==NO){ printf("\nError! --- %ld",__LINE__); exit(0); }

                vv[k]=gp->v;//crr.value
                gp=gp->next;
        }//end while 
        ///////////////////////////////

        return;
}

void modiSlack(struct genrstr *genrData,double *nodeP,long *codeList)
{
        long slack,k;
        struct genrstr *gp;
        ///////////////////////////////////

        slack=codeList[0];

        k=lookup(codeList,slack,nodeNum);
        if(k==NO){ printf("\n\nError! --- %ld",__LINE__); exit(0); }

        gp=findData(slack,genrData);
		if(gp!=NULL)
		{
			gp->w=nodeP[k];
			if(fabs(gp->w0)<SMLL) gp->w0=gp->w;
		}//end if
        ////////////////////////////////////////

        return;
}

void modifyV(double *vector,double *error,long *codeAttr,long swch)
{
        register long i,ii,n;//n --- dim of error[]

	if(swch==P_V0) n=nodeNum-1;
        if(swch==Q_VV) n=nodeNum-pvNodeNum-1;

        for(i=1;i<=n;i++)
        {
                ii=i+getSlide(i,codeAttr,swch);
                vector[ii]+= error[i];//No Change!
        }//end for
        /////////////////////

        return;
}

long pwrflow(long mod,long prnt)
{
//Notations by Wang GT:///////////////////////////////////////////
//mod = MODLOAD --- to be used for load shedding 
//      MODNO   --- no modification considered
//      MODPG   --- modifying genr. power
//      MODVAR  --- modifying capa., genv., or xfmr. 
//Solution:
//   1) flowData
//   2) sub_flowData
//   3) sysInfo
/////////////////////////////////////////////////////////////////

	if(prnt==YES) printf("\n\npwrflow()...");

	//1: Preparation for Flow Iteration

    formY(G,B,codeList,nodeNum,mod);
    if(prnt==YES) printf("\n formY() ok.");

    getNodePowr(nodeP,nodeQ,codeList,mod);
    if(prnt==YES) printf("\n getNodePowr() ok.");

    modiInitVolt(vv,codeList,mod);
    if(prnt==YES) printf("\n modiInitVolt() ok.");
    //////////////////////////////////////////////////////

	//2: powerflow interation with coslacking
	
	long ret,itr=0;//init.
	while(1)//unconditional loop
	{
		//2.1 pwrflow_itr()

		ret=pwrflow_itr(prnt);//No Change!
		if(prnt==YES) printf("\n pwrflow_itr() ok");

		if(ret!=YES) return NO;//Not convergent
		///////////////////////////////////////////////

		//2.2 flowData and nodal powers

		calFlowData(flowData,lineData);
		if(prnt==YES) printf("\n flowData obtained");

		calGenrPowr(nodeP,nodeQ,codeList,codeAttr,flowData);
		if(prnt==YES) printf("\n nodeP,nodeQ[] completed");
		
		if(slackNum!=0) break;//No Change!
		//////////////////////////////////////////////////

		//2.3 coslack_fun()
		
		ret=coslack_fun(nodeP,genrData);
		if(ret!=YES) break;//No coslacking

		if(++itr>ITR_MAX) return NO;
	}//end while
	///////////////////////////////////////////////////////

	//3: sub_flowData and sysInfo

    calFlowData(sub_flowData,sub_lineData);
    if(prnt==YES) printf("\n sub_flowData obtained");

    calSysInfo(sysInfo,flowData);
    if(prnt==YES) printf("\n sysInfo obtained");
	///////////////////////////////////////////////////

	return YES;
}

long coslack_fun(double *nodeP,struct genrstr *genrData)
{
	double aux2,aux4,sum_P0,wslack;
	long ii,slack;

	struct genrstr *gp;
	//////////////////////////////////////////////

	//1: cal. wslack --- important

	slack=codeList[0];

	gp=findData(slack,genrData);
	if(gp==NULL){ printf("\n\nError! --- %ld",__LINE__); exit(0); }

	ii=lookup(codeList,slack,nodeNum);
	if(ii==NO){ printf("\n\nError! --- %ld",__LINE__); exit(0); }

	wslack=nodeP[ii]-gp->w0*(1.0-gp->puf);//No Change!
	aux2=gp->w0*COSLACK_COEFF;

	if(fabs(wslack)<fabs(aux2)) return NO;
	/////////////////////////////////////////////////

	//2: suspending wslack

	nodeP[ii]-= wslack;
	gp->plant+= wslack;//inaccurate
	/////////////////////////////////////////////

	//3: dispatching wslack

	sum_P0=sum_genrData_P0(genrData);
	if(fabs(sum_P0)<SMLL){ printf("\nError! --- %ld",__LINE__); exit(0); }

	gp=genrData;
	while(gp!=NULL)
	{
		ii=lookup(codeList,gp->i,nodeNum);
		if(ii==NO){ printf("\n\nError! --- %ld",__LINE__); exit(0); }

		aux2=gp->w0/sum_P0;
		aux4=wslack*aux2;

		nodeP[ii]+= aux4;
		gp->plant-= aux4;
		
		gp=gp->next;
	}//end while
	////////////////////////////////////

	return YES;
}
	
long pwrflow_itr(long prnt)
{
	if(prnt==YES) printf("\n\npwrflow_itr()...");

	long wflg,rflg,swch,imax,itr;
	long bs_maxdw,bs_maxdr;
	
	double max,maxdw,maxdr;
	//////////////////////////////////////

	//1: init. locals --- No Change!

	wflg=YES;
	rflg=YES;
		
	itr =0;
	swch=P_V0;
	////////////////////////////////////

	//2: genuine iteration

	while(wflg==YES || rflg==YES)
	{
		//2.1 unbalanced nodal powers

		getdPowr(dPowr,swch);
		//typeVector(dPowr,nodeNum,"dPowr[]");
	
		imax=posAbsmax(dPowr,nodeNum);
		max=fabs(dPowr[imax]);

		if(max>LARG) return NO;//Not Convergent
		/////////////////////////////////////////////

		//2.2 maxdw, maxdr, and bs's

		if(swch==P_V0)
		{
			maxdw=max;
			bs_maxdw=codeList[imax];
		}//end if

		if(swch==Q_VV)
		{
			maxdr=max;
			bs_maxdr=codeList[imax];
		}//end if
		//////////////////////////////////////////

		//2.3 convergency check

		if(max<flow_eps)
        {
                if(swch==P_V0) wflg=NO;
                if(swch==Q_VV) rflg=NO;
        }
        else
        {
                if(swch==P_V0)
                {
                        wflg=YES;//wflg set
                        
						solveFactorTab(B1,dPowr,(nodeNum-1));
                        divVector(dPowr,nodeNum-1,av_volt);//P_V0 only

                        modifyV(v0,dPowr,codeAttr,swch);
                }//end if
	
                if(swch==Q_VV)
                {
                        rflg=YES;//rflg set

                        solveFactorTab(B2,dPowr,(nodeNum-pvNodeNum-1));
                        modifyV(vv,dPowr,codeAttr,swch);
                }//end if
        }//end else
        ////////////////////////////////////////
			
		//2.4 info. report

        if(swch==Q_VV)//only when swch=Q_VV
        {
                if(++itr>ITR_MAX) return NO;//Not Convergent
                if(prnt==YES)
				{
					sprintf(buffer,"\n itr=%2ld: maxdw=%9.5lE maxdr=%9.5lE at %4ld %4ld",
						itr,maxdw,maxdr,bs_maxdw,bs_maxdr);
					printf(buffer);
				}//end if
        }//end if
		////////////////////////////////////

        swch*= -1;//switch turned
	}//end while
	////////////////////////////////////////////

	return YES;
}

long attribute(long bs,struct nodestr *nodeData)
{
        long ibs,ret;
        struct nodestr *np;
        //////////////////////////////////////////

        ibs=abs(bs);
		ret=ZPQ;//init. as Special PQ
        
		np=nodeData;
        while(np!=NULL)
        {
                if(abs(np->i)==ibs)
                {
                        if(ret==ZPQ)//first meet
						{
							ret=np->attr;//PQ, PV, or SLACK
						}
                        else//meet the node again
                        {
                                if(ret==SLACK || np->attr==SLACK) ret=SLACK;
								else if(ret==PV || np->attr==PV)  ret=PV;
                        }
                }//end if

                np=np->next;
        }//end while
        /////////////////////////////////////

        return(ret);
}

void calSysInfo(struct sysstr &sysInfo,struct flowstr *flowData)
{
   	register long i;
    
	long imax,imin;
	long ibs,zbs;
       
	double aux2,aux4; 
	double wload,rload,wgen,wgen2,rgen,wslack,rslack;
	double loss,maxLoss,sysLoss;
	
	struct flowstr *fp;
	struct linestr *lp;
	//////////////////////////////////////////

	//1: nodal information

	imax=posmax(vv,nodeNum);
	imin=posmin(vv,nodeNum);

	wgen    =0.0;//init.
	wgen2   =0.0; 
	wload   =0.0;
	wslack  =0.0;
	rgen	=0.0;
	rload	=0.0;
	rslack	=0.0;

	for(i=1;i<=nodeNum;i++)
	{
 		aux2=nodeP[i];//nodal active power
		aux4=nodeQ[i];//nodal reactive

		if(fabs(aux2)<SMLL && fabs(aux4)<SMLL) continue;
		//////////////////////////////////////////////////////////////

		//stat. by attr.

		if(codeAttr[i]==SLACK)
		{ 
			wslack=aux2; 
			rslack=aux4; 
		}//end if

		if(codeAttr[i]==SLACK || codeAttr[i]==PV) wgen2+= aux2;
		////////////////////////////////////////////////////

		//stat. by injection

                if(aux2>SMLL || (fabs(aux2)<SMLL && aux4>SMLL))//generation
		{
                	wgen+= aux2;
                	rgen+= aux4; 
        	}
        	else//loads
        	{
                	wload-= aux2;
                	rload-= aux4;
        	}
	}//end for
	///////////////////////////////////////

	
	sysInfo.wgen  =wgen;
	sysInfo.wload =wload;

	//sysInfo.wgen    =sum_w_un + sum_w_regu;//Tangshan
	//sysInfo.wload   =sum_w_ld;
	
        sysInfo.wgen2   =wgen2;
    	sysInfo.wslack  =wslack;

    	sysInfo.rgen    =rgen;
    	sysInfo.rload   =rload;
    	sysInfo.rslack  =rslack;

    	sysInfo.maxVolt =vv[imax];
    	sysInfo.minVolt =vv[imin];

    	sysInfo.imaxVolt=codeList[imax];
    	sysInfo.iminVolt=codeList[imin];
    	//////////////////////////////////////////

    	//2: branch information

		ibs=0;//init.
		zbs=0;

    	maxLoss=0.0;
    	sysLoss=0.0;

        fp=flowData;
        lp=lineData;

    	for(i=1;i<=lineNum;i++,fp=fp->next,lp=lp->next)
    	{
            	if(brMatch(fp,lp->i,lp->j)!=YES){ printf("\nError! --- %ld",__LINE__); exit(0); }

            	loss=fabs(fp->w1+fp->w2);
            	sysLoss+= loss;//accumulated

            	if(loss>maxLoss)
            	{
                    	maxLoss=loss;

                    	ibs=abs(fp->i);
                    	zbs=abs(fp->j);
            	}//end if
    	}//end for
    	//////////////////////////////////////

    	sysInfo.ibsMaxLoss = ibs;
    	sysInfo.zbsMaxLoss = zbs;
    	sysInfo.maxLoss    = maxLoss;
    	sysInfo.sysLoss    = sysLoss;
        //////////////////////////////////////

    	return;
}

void genrReactiv(struct genrstr *genrData,double *nodeQ,long *codeList,long nodeNum)
{
	long k;
	double kr_comp=2.0-KR_COEFF;//No Change! 

	struct genrstr *gp;
	/////////////////////////////////////////////////

	gp=genrData;
	while(gp!=NULL)
	{
		//.1 current reactive 

		k=lookup(codeList,gp->i,nodeNum);
		if(k==NO){ printf("\nError! --- %ld",__LINE__); exit(0); }

		gp->r=nodeQ[k];
		///////////////////////////////////////////////////

		//.2 vioFlg and krFlg marking

		gp->vioFlg=NO;//init.
		gp->krFlg =NO;

		if(gp->rmax>SMLL && gp->r>gp->rmax) gp->vioFlg=YES;
		if(gp->r<gp->rmin) gp->vioFlg=YES;

		if(gp->rmax>SMLL && gp->r>gp->rmax*KR_COEFF) gp->krFlg=YES;
		if(gp->r<gp->rmin*kr_comp) gp->krFlg=YES;
		////////////////////////////////////////////////

		gp=gp->next;
	}//end while
	////////////////////////////////////////////

	//only for base_case reactive
	
	if(sum_genrData_Q0(genrData,5)<SMLL)
	{
		gp=genrData;
		while(gp!=NULL)
		{
			gp->r0=gp->r;
			gp=gp->next;
		}//end while
	}//end if
	////////////////////////////////

	return;
}

double sum_voltData_V0(struct voltstr *voltData,long n)
{
	register long i,n2;
        double ret=0.0;//init.

	struct voltstr *vp;
	//////////////////////////////////////

	n2=min(n,voltNum);
	vp=voltData;

	for(i=1;i<=n2;i++,vp=vp->next)
		ret+= vp->V0;
	/////////////////////////////////

	return ret;
}

double sum_genrData_P0(struct genrstr *genrData)
{
	double ret=0.0;//init.
	struct genrstr *gp;
	///////////////////////////////

	gp=genrData;
	while(gp!=NULL)
	{
		ret+= gp->w0;
		gp=gp->next;
	}//end while
	////////////////////////////

	return ret;
}

double sum_genrData_Q0(struct genrstr *genrData,long n)
{
	register long i,n2;
	double ret=0.0;//init.
	
	struct genrstr *gp;
	//////////////////////////

	n2=min(n,genrNum);	
	gp=genrData;

	for(i=1;i<=n2;i++,gp=gp->next)
		ret+= gp->r0;
	////////////////////////////

	return ret;
}

double sum_groupData_w0(struct groupstr *groupData,long n)
{
	register long i,n2;
	double ret=0.0;//init.

	struct groupstr *grp;
	/////////////////////////////////

	n2=min(n,groupNum);
	grp=groupData;

	for(i=1;i<=n2;i++,grp=grp->next)
		ret+= fabs(grp->w0);//No Change!
	//////////////////////////////////

	return ret;
}

void monVoltage(struct voltstr *voltData,double *vv,long *codeList,long nodeNum)
{
	long k;
	double kv_comp=2.0-KV_COEFF;//No Change!

	struct voltstr *vp;
	/////////////////////////////////////////

	//1: 

	vp=voltData;
	while(vp!=NULL)
	{
		//.1 curr. voltage

		k=lookup(codeList,vp->i,nodeNum);
		if(k==NO){ printf("\n\nError! --- %ld",__LINE__); exit(0); } 

		vp->V=vv[k];
		/////////////////////////////////////////

		//.2 vioFlg and kvFlg marking 

		vp->vioFlg=NO;//init.
		vp->kvFlg=NO;
	
		if(vp->Vmax>SMLL && vp->V>vp->Vmax) vp->vioFlg=YES;
		if(vp->Vmin>SMLL && vp->V<vp->Vmin) vp->vioFlg=YES;

		if(vp->Vmax>SMLL && vp->V>vp->Vmax*KV_COEFF) vp->kvFlg=YES;
		if(vp->Vmin>SMLL && vp->V<vp->Vmin*kv_comp) vp->kvFlg=YES;

		vp=vp->next;
	}//end while
	///////////////////////////////////////////

	//only for base_case mon.voltage

	if(sum_voltData_V0(voltData,5)<SMLL)
	{
		vp=voltData;
		while(vp!=NULL)
		{
			vp->V0=vp->V;
			vp=vp->next;
		}
	}//end if
	/////////////////////////////////////////
	
	return;
}

void groupModeAnalysis(struct groupstr *groupData,struct flowstr *flowData,struct flowstr *subFlowData)
{
        register long i,j;
        
        long imode,id_len,mel,flg1,flg2,flg3;
        double val,v;
        
        struct flowstr *fp;
        struct groupstr *grp;
        //////////////////////////////////////////////

        grp=groupData;
		for(i=1;i<=groupNum;i++,grp=grp->next)
        {
				if(grp->modeNum<=0)//No mode definition
				{
					grp->imode=0;
					continue;
				}//end if
				/////////////////////////////////////////////
			
                imode=0;//init.
                for(j=grp->modeNum;j>=1;j--)//reverse order
                {
                        //condition #1:

                        strcpy(idbuf,grp->mode[j].dev_id1);
                        id_len=strlen(idbuf);

                        if(id_len==0) flg1=YES;
                        else
                        {
                                fp=findData(idbuf,subFlowData);
								if(fp==NULL) fp=findData(idbuf,flowData);

                                if(fp==NULL) v=0.0;//no flow
                                else v=fabs(fp->w1);
                                //////////////////////////

                                mel=grp->mode[j].mel1;
                                val=grp->mode[j].val1;

                                flg1=conditionMeet(v,val,mel);
                        }

						if(flg1!=YES) continue;//not meet
                        ////////////////////////////////////////

                        //condition #2:

                        strcpy(idbuf,grp->mode[j].dev_id2);
                        id_len=strlen(idbuf);

                        if(id_len==0) flg2=YES;
                        else
                        {
								fp=findData(idbuf,subFlowData);
                                if(fp==NULL) fp=findData(idbuf,flowData);

                                if(fp==NULL) v=0.0;//no flow
                                else v=fabs(fp->w1);
                                /////////////////////////////

                                mel=grp->mode[j].mel2;
                                val=grp->mode[j].val2;

                                flg2=conditionMeet(v,val,mel);
                        }

						if(flg2!=YES) continue;//not meet
                        /////////////////////////////////////////

                        //condition #3:

                        strcpy(idbuf,grp->mode[j].dev_id3);
                        id_len=strlen(idbuf);

                        if(id_len==0) flg3=YES;
                        else
                        {
								fp=findData(idbuf,subFlowData);
                                if(fp==NULL) fp=findData(idbuf,flowData);

                                if(fp==NULL) v=0.0;//no flow
                                else v=fabs(fp->w1);
                                /////////////////////////////

                                mel=grp->mode[j].mel3;
                                val=grp->mode[j].val3;

                                flg3=conditionMeet(v,val,mel);
                        }

                        if(flg3==YES){ imode=j; break; }//found
                }//end for
        }//end for
        ///////////////////////////////////////////////

        return;
}

void groupFlow(struct groupstr *groupData,struct flowstr *flowData,struct flowstr *subFlowData)
{
        register long i,j;
	
        long ibs,zbs,imode,imst;
        double aux2,mwmax,mwmin,w,r,sum_w,sum_r;
	
        struct groupstr *grp;
        struct flowstr *fp;
        struct linestr *lp;
        ///////////////////////////////////////////////////
	
        grp=groupData;
        for(i=1;i<=groupNum;i++,grp=grp->next)
        {
			//.1 cal. group powers

			sum_w=0.0;//init.
			sum_r=0.0;

			for(j=1;j<=grp->lineNum;j++)
			{
				ibs=grp->line[j].i;
				zbs=grp->line[j].j;

				fp=findData(ibs,zbs,flowData);//No Change!
				lp=findData(ibs,zbs,lineData);
		
				if(fp==NULL || lp==NULL) continue;
				//////////////////////////////////////////
				
				w=0.0;//init.
				r=0.0;

				imst=grp->line[j].imst;
		
				if(imst==abs(lp->i)){ w=fp->w1; r=fp->r1; }
				if(imst==abs(lp->j)){ w=fp->w2; r=fp->r2; }
				//////////////////////////////////////////////
 
				grp->line[j].w=w;
				grp->line[j].r=r;

				sum_w+= w;
				sum_r+= r;
			}//end for

			grp->w=sum_w;
			grp->r=sum_r;
			grp->s=sqrt(sum_w*sum_w+sum_r*sum_r);
			////////////////////////////////////////////

			//.2 init. flgs and mwmax/min

			grp->vioFlg=NO;//init.
			grp->vioPct=.0;
			grp->kbFlg =NO;

			mwmin=grp->mwmin;
			mwmax=grp->mwmax;

			imode=grp->imode;
			if(imode>0) mwmax=grp->mode[imode].mwmax;
			////////////////////////////////////////////////
		
			//.3 setting vioFlg and vioPct
			
			if(mwmax>SMLL && grp->w>mwmax)
			{
				grp->vioFlg=YES;
				grp->vioPct=100.0*(grp->w-mwmax)/mwmax;
			}//end if
			
			if(fabs(mwmin)>SMLL && grp->w<mwmin)
			{
				grp->vioFlg=YES;
				grp->vioPct=100.0*(grp->w-mwmin)/fabs(mwmin);
			}//end if
			//////////////////////////////////////////////////

			//.4 setting kbFlg

			if(mwmax>SMLL && grp->w>KB_COEFF*mwmax) grp->kbFlg=YES;
			
			if(fabs(mwmin)>SMLL)
			{
				aux2=fabs(mwmin)*(1.0-KB_COEFF);
				if(grp->w<mwmin+aux2) grp->kbFlg=YES;
			}//end if
        }//end for
        /////////////////////////////////////////////////////

	//only for base_case group powers
        
	if(sum_groupData_w0(groupData,5)<SMLL)
	{
		grp=groupData;
		while(grp!=NULL)
		{
			grp->w0=grp->w;
			grp->r0=grp->r;
			grp->s0=grp->s;

			grp=grp->next;
		}//end while
	}//end if
	///////////////////////////////

	return;
}

void lineFlow(double &w1,double &r1,double &w2,double &r2,double &s1,double &s2,double &pf1,double &pf2,struct linestr *lp)
{
        long ii,jj;
        double vi,vj,theta,brG,brB;
        ////////////////////////////////////////

        //1: nodal voltages

        ii=lookup(codeList,lp->i,nodeNum);
        jj=lookup(codeList,lp->j,nodeNum);
        if(ii==NO || jj==NO){ printf("\nError! --- %ld",__LINE__); exit(0); }

        vi=vv[ii];
        vj=vv[jj];
        theta=v0[ii]-v0[jj];

        if(lp->i<0) vi/= lp->b0k;//K* at i_side
        if(lp->j<0) vj/= lp->b0k;//K* at j_side
        /////////////////////////////////////////////////////////

        //2: branch powers 

        getBranchGB(lp,brG,brB);//admittance obtained
		
        w1=vi*vi*brG-vi*vj*(brG*cos(theta)+brB*sin(theta));
        r1=vi*vj*(brB*cos(theta)-brG*sin(theta))-vi*vi*brB;

        w2=vj*vj*brG-vi*vj*(brG*cos(theta)-brB*sin(theta));
        r2=vi*vj*(brB*cos(theta)+brG*sin(theta))-vj*vj*brB;

        if(lp->i>0 && lp->j>0)//shunt reactive
        {
                r1-= lp->b0k*vi*vi;
                r2-= lp->b0k*vj*vj;
        }//end if
        //////////////////////////////////////////////////

        //3: other calculation

		//3.1 power factors

        pf1=0.0;//init.
        pf2=0.0;

        if(fabs(w1)>SMLL) pf1=w1/sqrt(w1*w1 + r1*r1);
        if(fabs(w2)>SMLL) pf2=w2/sqrt(w2*w2 + r2*r2);
        /////////////////////////////////////////

		//3.2 s1 and s2 --- MVA or Amp

		mvaAmpfun(s1,s2,w1,r1,w2,r2,lp);
		//////////////////////////////////////

		return;
}

double lmtPU(struct linestr *lp)
{
	long ii;
	double aux2,vi,lmt,ret;
	//////////////////////////////////////

        if(lp->i<0 || lp->j<0)//xf.
        {
                lmt=lp->lmt;    //MVA
                ret=lmt/100.0;  //p.u.
        }
        else//power line
        {
                ii=lookup(codeList,lp->i,nodeNum);
                if(ii==NO){ printf("\n\nError! --- %ld",__LINE__); exit(0); }

                vi=vv[ii];
                vi*= lp->vl; //kV

                lmt=lp->lmt; //Amp.
                lmt/= 1000;  //kA

                aux2=sqrt(3.0)*vi*lmt;//MVA
                ret=aux2/100;         //p.u.    
        }
        ////////////////////////////////////////

	return ret;
}

void mvaAmpfun(double &s1,double &s2,double w1,double r1,double w2,double r2,struct linestr *lp)
{
	long ii,jj;
	double aux2,vi,vj;
	//////////////////////////////////////////
	
	//1: apparent powrs p.u.

	s1 = sign(w1)*sqrt(w1*w1 + r1*r1);
	s2 = sign(w2)*sqrt(w2*w2 + r2*r2);
	/////////////////////////////////////////

	//2: MVA for transformers

	if(lp->i<0 || lp->j<0)
	{
		s1*= pwrbase;
		s2*= pwrbase;
	}//end if
	///////////////////////////////////////

	//3: Amp. for power lines
	
	if(lp->i>0 && lp->j>0)	
	{
		ii=lookup(codeList,lp->i,nodeNum);
		jj=lookup(codeList,lp->j,nodeNum);

		if(ii==NO || jj==NO){ printf("\nError! --- %ld",__LINE__); exit(0); }
		
		vi=vv[ii];
		vj=vv[jj];
		////////////////////////////////////

		aux2=pwrbase/(sqrt(3.0)*lp->vl);

		s1=1000*aux2*s1/vi;//Amp.
		s2=1000*aux2*s2/vj;
	}//end if
	/////////////////////////////////////

	return;
}

long veriFlowData(struct flowstr *flowData,double *nodeP,double *nodeQ,long nodeNum)
{
	register long i;
	
	long ii,jj,imax_dP,imax_dQ,ret=YES;//init.
	double max_dP,max_dQ;
	
	double *dP,*dQ;
	struct flowstr *fp;
	/////////////////////////////////////////////////////////

	dP=(double *)calloc(nodeNum+1,sizeof(double));
	dQ=(double *)calloc(nodeNum+1,sizeof(double));

	if(dP==NULL || dQ==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }
	//////////////////////////////////////////////////

	//1: init. dP, dQ[] with nodal injections

	copyVector(dP,nodeP,nodeNum);
	copyVector(dQ,nodeQ,nodeNum);
	//////////////////////////////////////////////////

	//2: deducting flowData --- branck powers

	fp=flowData;
	for(i=1;i<=lineNum;i++,fp=fp->next)
	{
		ii=lookup(codeList,fp->i,nodeNum);
		jj=lookup(codeList,fp->j,nodeNum);

		if(ii==NO || jj==NO){ printf("\nError! --- %ld",__LINE__); exit(0); }

		dP[ii]-= fp->w1;
		dQ[ii]-= fp->r1;

		dP[jj]-= fp->w2;
		dQ[jj]-= fp->r2;
	}//end for
	/////////////////////////////////////////////

	//3: checking up

	imax_dP=posAbsmax(dP,nodeNum);
	imax_dQ=posAbsmax(dQ,nodeNum);

	max_dP=dP[imax_dP];
	max_dQ=dQ[imax_dQ];
	////////////////////////////////////

	if(fabs(max_dP)>flow_eps || fabs(max_dQ)>flow_eps)
	{
		printf("\n\nError! --- %ld",__LINE__);
		
		printf("\n  max_dP=% lf",max_dP);
		printf("\n  max_dQ=% lf",max_dQ);
	
		ret=NO;
	}//end if
	////////////////////////////////////////////////

	free(dP);
	free(dQ);

	return ret;
}

void pwrflow_report(char *resfile,char *waflg,long prnt)
{
//Notes: by Wang GT
//  resfile --- result file
//  waflg   --- w/a flag
//  prnt    --- YES or NO
///////////////////////////////////////////////////////////

        //1: opening file and painting the title

		if(prnt==YES) printf("\n\npwrflow_report()...");
 		date_time(buffer);

        FILE *out=fopen(resfile,waflg);
        if(out==NULL){ printf("\n\nCannot open %s",resfile); exit(0); }

		fprintf(out,"\n\n******** Powerflow Report for %s ********",project);
		fprintf(out,"\n\nTime Mark: %s",buffer);
        ///////////////////////////////////////////////////

        //2: reporting pwrflow solution

        reportLine(out,flowData,sub_flowData,NO);//prnt
        reportNode(out,vv,v0,nodeP,nodeQ,codeList,NO);
        reportSys(out,sysInfo,YES);

        reportGenrReactiv(out,genrData,NO);//prnt
        reportGroupFlow(out,groupData,NO);
        reportMonVoltage(out,voltData,NO);
	
        if(prnt==YES) printf("\n pwrflow reported");
        /////////////////////////////////////////////////

        //3: closing file

        fclose(out);
		if(prnt==YES) printf("\n resfile closed");
		//////////////////////////////////////////////////

        return;
}

void pwrflow_init(long prnt)
{
	if(prnt==YES) printf("\n\npwrflow_init()...");

	//1: allocation for globals

	global_alloc();
	if(prnt==YES) printf("\n global_alloc() ok");
	////////////////////////////////////////////////////

	//2: codeList[] and codeAttr[]

	readVector(inifile,"LIST",codeList,(long)0,nodeNum);
	if(prnt==YES) printf("\n codeList[] obtained");

	readVector(inifile,"ATTR",codeAttr,(long)0,nodeNum);
	if(prnt==YES) printf("\n codeAttr[] obtained");
	///////////////////////////////////////////////////////////

	//3: factor tables --- B'[][] and B"[][]

	readSparMatrix(inifile,"B1FT",B1,nodeNum-1);
	if(prnt==YES) printf("\n B1[][] obtained");

	readSparMatrix(inifile,"B2FT",B2,nodeNum-1-pvNodeNum);
	if(prnt==YES) printf("\n B2[][] obtained");
	///////////////////////////////////////////////

	//4: init. vectors -- vv[] and v0[]

	initVolt(vv,v0,codeList,NO);//prnt
	if(prnt==YES) printf("\n initVolt() ok");
	///////////////////////////////////////////////////

	return;
}

void OPF_start(char *prj,long prnt)
{
	if(prnt==YES) printf("\n\nOPF_start()...");
	
	//1: project[] and buf[]

	strcpy(project,prj);
	strcat(project," Project");

	strcpy(buf,prj);//public buffer
	if(prnt==YES) printf("\n project=%s",project);
	/////////////////////////////////////////
		
	//2: file names

    strcpy(basfile,buf);
    strcat(basfile,".bas");

    strcpy(blpfile,buf);
    strcat(blpfile,".blp");

    strcpy(datfile,buf);
	strcat(datfile,".dat");
	if(prnt==YES) printf("\n datfile=%s",datfile);

	strcpy(inifile,buf);
    strcat(inifile,".ini");

	strcpy(logfile,buf);
    strcat(logfile,".log");

	strcpy(matfile,buf);
	strcat(matfile,".mat");

	strcpy(ptnfile,buf);
	strcat(ptnfile,".ptn");

    strcpy(resfile,buf);
    strcat(resfile,".res");
	if(prnt==YES) printf("\n resfile=%s",resfile);

    strcpy(snsfile,buf);
    strcat(snsfile,".sns");

    strcpy(vfyfile,buf);
    strcat(vfyfile,".vfy");
    ////////////////////////////////////

	//3: global init.

	global_init();
	if(prnt==YES) printf("\n global_init() ok");
	/////////////////////////////////////////
	
	return;
}

void global_alloc()
{
	//1: admittance matrices

	makeMatrix(_G,G,nodeNum+1,nodeNum+1);
	makeMatrix(_B,B,nodeNum+1,nodeNum+1);
	makeMatrix(_B1,B1,nodeNum,nodeNum);
	makeMatrix(_B2,B2,nodeNum-pvNodeNum,nodeNum-pvNodeNum);
	///////////////////////////////////////////////////////

	//2: code list and attr. 

	codeList=(long *)calloc(nodeNum+1,sizeof(long));
	codeAttr=(long *)calloc(nodeNum+1,sizeof(long));

        //3: nodal voltage and powers
   
        vv=(double *)calloc(nodeNum+1,sizeof(double));
        v0=(double *)calloc(nodeNum+1,sizeof(double));
 
        nodeP=(double *)calloc(nodeNum+1,sizeof(double));
        nodeQ=(double *)calloc(nodeNum+1,sizeof(double));
        dPowr=(double *)calloc(nodeNum+1,sizeof(double));
        /////////////////////////////////////////////////////

        //4: sensitivities

        makeMatrix(_GRPD,GRPD,groupNum+1,loadNum+1);
        makeMatrix(_GRPG,GRPG,groupNum+1,genrNum+1);

        makeMatrix(_PBPD,PBPD,lineNum+1,loadNum+1);
        makeMatrix(_PBPG,PBPG,lineNum+1,genrNum+1);

        makeMatrix(_QBKT,QBKT,lineNum+1,xfmrNum+1);
        makeMatrix(_QBVG,QBVG,lineNum+1,genrNum+1);
        makeMatrix(_QBQC,QBQC,lineNum+1,capaNum+1);

        makeMatrix(_QGKT,QGKT,genrNum+1,xfmrNum+1);
        makeMatrix(_QGVG,QGVG,genrNum+1,genrNum+1);
        makeMatrix(_QGQC,QGQC,genrNum+1,capaNum+1);

        makeMatrix(_VmKT,VmKT,voltNum+1,xfmrNum+1);
        makeMatrix(_VmVG,VmVG,voltNum+1,genrNum+1);
        makeMatrix(_VmQC,VmQC,voltNum+1,capaNum+1);

        PLKT=(double *)calloc(xfmrNum+1,sizeof(double));
        PLVG=(double *)calloc(genrNum+1,sizeof(double));
        PLQC=(double *)calloc(capaNum+1,sizeof(double));
        PLPG=(double *)calloc(genrNum+1,sizeof(double));
	
        if(PLPG==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }
        ////////////////////////////////////////////////////

        return;
}

void global_init()
{
	//original chains

	areaData   = NULL;
	assoData   = NULL;
	capaData   = NULL;
	genrData   = NULL;
	groupData  = NULL;
	lineData   = NULL;
	loadData   = NULL;
	nodeData   = NULL;
	patnData   = NULL;
	priceData  = NULL;
	shortData  = NULL;
	sub_lineData=NULL;
	voltData   = NULL;
	xfmrData   = NULL;
	/////////////////////////////

	//flow and itr. solution 

	flowData0  = NULL;
	flowData   = NULL;
	sub_flowData0=NULL;
	sub_flowData =NULL;

	itrData    = NULL;
	//////////////////////////////

	//global numbers

	areaNum  = 0;
	assoNum  = 0;
	capaNum  = 0;
	genrNum  = 0;
	groupNum = 0;
	lineNum  = 0;
	loadNum  = 0;
	nodeNum  = 0;
	patnNum  = 0;
	plantNum = 0;
	pvNodeNum= 0;
	slackNum = 0;
	shortNum = 0;
	voltNum  = 0;
	xfmrNum  = 0;
	//////////////////////////

	//nodal vectors

	codeList=NULL;
	codeAttr=NULL;

	dPowr = NULL;
	nodeP = NULL;
	nodeQ = NULL;
	vv    = NULL;
	v0    = NULL;
	////////////////////////

	//admittance matrices

	_B1 = NULL;
	_B2 = NULL;
	_G  = NULL;
	_B  = NULL;
	B1  = NULL;
	B2  = NULL;
	G   = NULL;
	B   = NULL;
	////////////////////////

	//sensitivities

	_GRPD = NULL;
	_GRPG = NULL;
	_PBPD = NULL;
	_PBPG = NULL;
	GRPD  = NULL;
	GRPG  = NULL;
	PBPD  = NULL;
	PBPG  = NULL;
	PLPG  = NULL;
	////////////////////////

	return;
}

void global_free()
{
	//original data chains

	freeChain(areaData);
	freeChain(assoData);
	freeChain(capaData);
	freeChain(genrData);
	freeChain(groupData);
	freeChain(lineData);
	freeChain(loadData); 
	freeChain(nodeData);
	freeChain(priceData);
	freeChain(shortData);
	freeChain(sub_lineData);
	freeChain(voltData);
	freeChain(xfmrData);
	///////////////////////////////

	//flow and itr. result

	freeChain(flowData0);
	freeChain(flowData);

	freeChain(sub_flowData0);
	freeChain(sub_flowData);
	
	freeChain(itrData);
	/////////////////////////////////////

	//nodal vectors
 
        free(codeList);
        free(codeAttr);
        free(dPowr);
        free(nodeP);
        free(nodeQ);
        free(vv);
        free(v0);
	/////////////////////////////

	//admittance matrices
	
        freeMatrix(_B1,B1);
        freeMatrix(_B2,B2);
        freeMatrix(_G,G);
        freeMatrix(_B,B);
        ///////////////////////////////

	//sensitivity

	free(PLPG);
	freeMatrix(_GRPD,GRPD);
	freeMatrix(_GRPG,GRPG);
	freeMatrix(_PBPD,PBPD);
	freeMatrix(_PBPG,PBPG);
	/////////////////////////////

        return;
}

void loadDisp(struct nodestr *nodeData,struct loadstr *loadData,double sys_load,long prnt)
{
	if(prnt==YES) printf("\n\nloadDisp()...");

	double sum_w,phai,aux0,aux2;

	struct loadstr *ldp,*next;
	struct nodestr *np;
	////////////////////////////////////////////////

	//1: filtering loadData by nodeData_PQ

        ldp=loadData;
        while(ldp!=NULL)
        {
                np=findData(ldp->id,nodeData,PQ);
                if(np==NULL)
                {
			next=ldp->next;
                        delChainNode(loadData,ldp);

                        ldp=next;
                        continue;
                }//end if

                ldp=ldp->next;
        }//end while

        if(prnt==YES) printf("\n loadData filtered");
        //////////////////////////////////////////////////

	//2: cal. sum from loadData

	sum_w=0.0;//init.
	ldp=loadData;

	while(ldp!=NULL)
	{ 
		sum_w+= ldp->w; 
		ldp=ldp->next; 
	}//end while

	if(sum_w<SMLL)
	{ 
		printf("\n\nError! --- %ld",__LINE__); 
		printf("\n  sum_w = %lf",sum_w);

		exit(0); 
	}//end if

	if(prnt==YES) printf("\n sum_w = %lf",sum_w);
	/////////////////////////////////////////////////////////

	//3: perform distribution

	aux0=sys_load/sum_w;
	ldp=loadData;

	while(ldp!=NULL)
	{
		aux2=sqrt(ldp->w*ldp->w + ldp->r*ldp->r);
		phai=acos(ldp->w/aux2);

		ldp->w=aux0*ldp->w;
		ldp->r=tan(phai)*ldp->w;

		ldp=ldp->next;
	}//end while
	
	if(prnt==YES) printf("\n sys_load distributed"); 
	///////////////////////////////////////////////////

	//4: filling in nodeData_PQ

        ldp=loadData;
        while(ldp!=NULL)
        {
                np=findData(ldp->id,nodeData,PQ);
                if(np==NULL){ printf("\n\nError! --- %ld",__LINE__); exit(0); }

                np->p=ldp->w;
                np->qv=ldp->r;

                ldp=ldp->next;
        }//end while

        if(prnt==YES) printf("\n filling ok");
	/////////////////////////////////////////////

	//5: verification 

	sum_w=0.0;
	np=nodeData;
	
	while(np!=NULL)
	{
		if(np->attr==PQ) sum_w+= np->p;

		np=np->next;
	}//end while

	if(fabs(sum_w-sys_load)>SMLL)
	{ 
		printf("\n\nError! --- %ld",__LINE__); 
		printf("\n  sum_w=%lf, sys_load=%lf",sum_w,sys_load); 
		
		exit(0); 
	}//end if
	//////////////////////////////////////////////////

	return;
}

void unitSched(struct genrstr *_genrData,struct nodestr *_nodeData,long prnt)
{
	if(prnt==YES) printf("\n\nunitSched()...");

	register long i;
	long n;
	
	struct genrstr *gp;
	struct nodestr *np;
	/////////////////////////////

	n=chainLen(_genrData);
	gp=_genrData;
	
	for(i=1;i<=n;i++,gp=gp->next)
	{
		np=findData(gp->id,_nodeData,PVSLACK);
		if(np==NULL)
		{ 	
			printf("\n\nWarning! --- %ld",__LINE__);
			printf("\n  Unit Offline: id=%s P0=%7.4lf",gp->id,gp->w0);

			continue;
		}//end if

		if(np->attr==PV && fabs(np->p)>SMLL){ printf("\nError! --- %ld",__LINE__); exit(0); }
		if(np->attr==PV) np->p=gp->w0;
	}//end for
	//////////////////////////////////////

	return;
}

void areaExPower(struct nodestr *nodeData,struct areastr *areaData,long prnt)
{
	if(prnt==YES) printf("\n\nareaExPower()...");

	register long i;
	long ibs,zbs,imst,k;

	double aux0,aux2,aux4;
	double phai,wt,sum_wt,w,r;

	struct areastr *ap;
	struct nodestr *np;
	///////////////////////////////////////

	ap=areaData;
	while(ap!=NULL)
	{
		if(prnt==YES) printf("\n area_id = %s, wgen=%lf",ap->id,ap->wgen);

		//1: concerning units of area

		if(ap->unitNum>0)
		{
			//1.1 num.of units online

			k=0;//init.
			for(i=1;i<=ap->unitNum;i++)
				if(ap->unit[i].i>0) k++;

			if(k==0)//no unit online
			{
				printf("\n\nWarning! --- %ld",__LINE__);
				printf("\n  No unit online in area %s",ap->id);

				ap=ap->next;
				continue;//next area
			}//end if
			//////////////////////////////////////////////////////////////

			//1.2 unit scheduling averagely
 
			aux0=ap->wgen/k;
			for(i=1;i<=ap->unitNum;i++)
			{	
				ap->unit[i].w=0.0;//init.
				if(ap->unit[i].i>0) ap->unit[i].w=aux0;
			
				if(ap->unit[i].w<SMLL) continue;
				setNodeData(ap->unit[i].id,ap->unit[i].w,PVSLACK,nodeData);
			}//end for
		}//end if
		///////////////////////////////////////////////

		//2: concerning lines of area

		if(ap->lineNum>0)
		{
			//2.1 sum of weights

			sum_wt=0.0;//init.
			for(i=1;i<=ap->lineNum;i++)
			{
				wt=ap->line[i].wt;
				sum_wt+= wt;
			}//end for
		
			if(prnt==YES) printf("\n  sum_wt=%lf",sum_wt);
			///////////////////////////////////////////

			//2.2 branch powers

			aux0=0.0;//init.
			if(fabs(sum_wt)>SMLL) aux0=ap->wgen/sum_wt;
		
			for(i=1;i<=ap->lineNum;i++)
			{
				wt=ap->line[i].wt;
				phai=acos(ap->line[i].pf);
	
				aux2=wt*aux0;
				aux4=aux2*tan(phai);

				ap->line[i].w=aux2;
				ap->line[i].r=aux4;
			}//end for

			if(prnt==YES) printf("\n  branch powers obtained");
			//////////////////////////////////////////////////////

			//2.3 modifying nodal loads
 
			for(i=1;i<=ap->lineNum;i++)
			{	
				ibs = ap->line[i].i;
				zbs = ap->line[i].j;
				imst= ap->line[i].imst;

				if(ibs==0 && zbs==0) continue;//invalid
				///////////////////////////////////////////////

				w=ap->line[i].w;
				r=ap->line[i].r;

				if(fabs(w)<SMLL && fabs(r)<SMLL) continue;
				//////////////////////////////////////////////

				np=NULL;//init.
				
				if(imst==ibs) np=findData(ibs,nodeData,PQ); 
				if(imst==zbs) np=findData(zbs,nodeData,PQ);

				if(np==NULL)
				{ 
					printf("\n\nError! --- %ld",__LINE__); 
					printf("\n  Not found nodeData for area line: id=%s",ap->line[i].id);
					printf("\n  ibs=%ld, zbs=%ld, imst=%ld",ibs,zbs,imst);
					
					exit(0); 
				}//end if
				////////////////////////////////////////////////////

				np->p+= -ap->line[i].w;//No Change!
				np->qv+= -ap->line[i].r;
			}//end for

			if(prnt==YES) printf("\n  nodal loads ok"); 
		}//end if	
		//////////////////////////////////////////////

		ap=ap->next;
	}//end while
	/////////////////////////////////
	
	return;
}

void OPF_baselineAlt(long prnt)
{
	if(prnt==YES) printf("\n\nOPF_baselineAlt()...");

	//1: altering lineData

	baselineAlt(lineData,100,1,LN_RX);
	baselineAlt(lineData,100,1,LN_B0);

	if(prnt==YES) printf("\n lineData altered");
	//////////////////////////////////////////////////

	//2: altering nodeData

	baselineAlt(nodeData,100,1,PQ);
	baselineAlt(nodeData,100,1,PV);

	if(prnt==YES) printf("\n nodeData altered");
	////////////////////////////////////////////////

	//3: altering genrData data

	baselineAlt(genrData,100,1);
	baselineAlt(priceData,100,1,BD_POWR);

	if(prnt==YES) printf("\n genrData altered");
	///////////////////////////////////////////////////

	//4: altering other data

	baselineAlt(capaData,100,1);
	baselineAlt(groupData,100,1);

	if(prnt==YES) printf("\n others altered");
	///////////////////////////////////////

	return;
}

void OPF_prtt(long prnt)//pretreatment of all data --- very important!
{
	if(prnt==YES) printf("\n\nOPF_prtt()...");

   	//1: combine associated data

	combinLineData(lineData,sub_lineData);
	combinChainNodes(nodeData);

	combinChainNodes(xfmrData);
	combinChainNodes(capaData);

	combinChainNodes(genrData);
	combinChainNodes(priceData);

	if(prnt==YES) printf("\n combinData() ok");
	//////////////////////////////////////////////

	//2: filtering redundant data
	
	filterData(capaData,NO);//planFlg
	filterData(groupData);
	filterData(nodeData);
	filterData(voltData);
	filterData(xfmrData);

	//filterData(genrData);
        //filterData(priceData);
	
	if(prnt==YES) printf("\n filterData() ok");
	//////////////////////////////////////////////////

	//3: statistics of global numbers

	lineNum=chainLen(lineData);
	nodeNumStat(nodeNum,pvNodeNum,slackNum);
   
	if(prnt==YES) printf("\n lineNum = %ld",lineNum);
	if(prnt==YES) printf("\n nodeNum = %ld",nodeNum);
	if(prnt==YES) printf("\n pvNodeNum= %ld",pvNodeNum);
	if(prnt==YES) printf("\n slackNum = %ld",slackNum);

	areaNum =chainLen(areaData);
	capaNum =chainLen(capaData);
	genrNum =chainLen(genrData);
	groupNum=chainLen(groupData);
	loadNum =chainLen(loadData);
	voltNum =chainLen(voltData);
	xfmrNum =chainLen(xfmrData);

	if(prnt==YES) printf("\n global num.stat ok");
	//////////////////////////////////////////////////

	//4: initialization of ctrl.data

	initCtrlData(capaData);
	initCtrlData(genrData);
	initCtrlData(xfmrData);

	if(prnt==YES) printf("\n initCtrlData() ok");
	/////////////////////////////////////////////////

	//5: slack auto.selection

    if(slackNum==0)
	{
		autoSlackSet(nodeData,genrData,YES);//prnt
		if(prnt==YES) printf("\n autoSlackSet() ok");
	}//end if

    if(slackNum>1)
    {
		printf("\n\nError! --- %ld",__LINE__);
        	printf("\n  Wrong slackNum = %ld",slackNum);
	
        	exit(0);
    }//end if
	////////////////////////////////////////////

    return;
}

double wloadStat(struct nodestr *nodeData)
{
	double wload=0.0;//init.
	struct nodestr *np=nodeData;
	///////////////////////////////////////

	while(np!=NULL)
	{
		if(np->attr==PQ && np->p>0.0) wload+= np->p;
		if(np->attr==PV && np->p<0.0) wload-= np->p;

		np=np->next;
	}//end while
	//////////////////////////////

	return wload;
}

double wgenStat(struct nodestr *nodeData)
{
	double wgen=0.0;//init.
	struct nodestr *np=nodeData;
	/////////////////////////////////////////

	while(np!=NULL)
	{
		if(np->attr==PV && np->p>0.0) wgen+= np->p;
		if(np->attr==PQ && np->p<0.0) wgen-= np->p;

		np=np->next;
	}//end while
	///////////////////////////////

        return wgen;
}

double wgen2Stat(struct nodestr *nodeData)
//only called after nodeData combined
{
	double wgen2=0.0;//init.
        struct nodestr *np;
	///////////////////////////////

	np=nodeData;
	while(np!=NULL)
	{
		if(np->attr==PV) wgen2+= np->p;

		if(np->attr==SLACK)//No Change!
		{
			printf("\n\nError! --- %ld",__LINE__);
			printf("\n  wgen2Stat() called while slack found:");

			printf("\n  id=%s, ibs=%ld",np->id,np->i);
		}//end if

		np=np->next;
	}//end while
	//////////////////////////////////

	return wgen2;
}

void baselineAlt(struct groupstr *groupData,double orig_multpl,double targ_multpl)
{
	register long i;
	struct groupstr *grp;
	////////////////////////////////////////////

	grp=groupData;
	while(grp!=NULL)
	{
		grp->mwmin/= orig_multpl;
		grp->mwmax/= orig_multpl;
		grp->mwmin*= targ_multpl;
		grp->mwmax*= targ_multpl;

		for(i=1;i<=grp->modeNum;i++)
		{
			grp->mode[i].mwmax/= orig_multpl;
			grp->mode[i].val1 /= orig_multpl;
			grp->mode[i].val2 /= orig_multpl;
			grp->mode[i].val3 /= orig_multpl;

			grp->mode[i].mwmax*= targ_multpl;
			grp->mode[i].val1 *= targ_multpl;
			grp->mode[i].val2 *= targ_multpl;
			grp->mode[i].val3 *= targ_multpl;
		}//end for

		grp=grp->next;
	}//end while
	//////////////////////////////////

	return;
}

void baselineAlt(struct pricestr *priceData,double orig_multpl,double targ_multpl,long swch)
{
        struct pricestr *pp=priceData;
        while(pp!=NULL)
        {
                if(swch==BD_POWR)
                {
                        pp->Pb/= orig_multpl;
                        pp->Pb*= targ_multpl;
                }//end if

                if(swch==BD_COST)
                {
                        pp->Cb/= orig_multpl;
                        pp->Cb*= targ_multpl;
                }//end if

                pp=pp->next;
        }//end while

        return;
}

void baselineAlt(struct capastr *capaData,double orig_multpl,double targ_multpl)
{
        struct capastr *cp=capaData;
        while(cp!=NULL)
        {
                cp->Q0      /= orig_multpl;
                cp->Qmin    /= orig_multpl;
                cp->Qmax    /= orig_multpl;
                cp->eps     /= orig_multpl;
                cp->maxstep /= orig_multpl;

                cp->Q0      *= targ_multpl;
                cp->Qmin    *= targ_multpl;
                cp->Qmax    *= targ_multpl;
                cp->eps     *= targ_multpl;
                cp->maxstep *= targ_multpl;

                cp=cp->next;
        }//end while

        return;
}

void baselineAlt(struct genrstr *genrData,double orig_multpl,double targ_multpl)
{
        struct genrstr *gp=genrData;
        while(gp!=NULL)
        {
                gp->w0     /= orig_multpl;
                gp->wmin   /= orig_multpl;
                gp->wmax   /= orig_multpl;
                gp->rmin   /= orig_multpl;
                gp->rmax   /= orig_multpl;
                gp->rampUp /= orig_multpl;
                gp->rampDn /= orig_multpl;
	
                gp->w0     *= targ_multpl;
                gp->wmin   *= targ_multpl;
                gp->wmax   *= targ_multpl;
                gp->rmin   *= targ_multpl;
                gp->rmax   *= targ_multpl;
                gp->rampUp *= targ_multpl;
                gp->rampDn *= targ_multpl;

                gp=gp->next;
        }//end while

        return;
}

void baselineAlt(struct nodestr *nodeData,double orig_multpl,double targ_multpl,long attr)
{
        struct nodestr *np=nodeData;
        while(np!=NULL)
        {
                if(attr==PQ && np->attr==PQ)//loads
                {
                        np->p/= orig_multpl;
                        np->qv/= orig_multpl;

                        np->p*= targ_multpl;
                        np->qv*= targ_multpl;
                }//end if
				
                if(attr==PV && np->attr==PV)//genr.
                {
                        np->p/= orig_multpl;
                        np->p*= targ_multpl;
                }//end if

                np=np->next;
        }//end while
		/////////////////////////////////////

        return;
}

void baselineAlt(struct linestr *lineData,double orig_multpl,double targ_multpl,long swch)
{
        struct linestr *lp=lineData;
        while(lp!=NULL)
        {
                switch(swch)
		{
		case LN_RX://for r,x of both ln and xf
        	    
			lp->r/= orig_multpl;
               		lp->x/= orig_multpl;

                	lp->r*= targ_multpl;
                	lp->x*= targ_multpl;
                
			break;
	
        	case LN_B0://only for b0 of power lines
                
			if(lp->i<0 || lp->j<0) break;
        
                	lp->b0k/= orig_multpl;
                	lp->b0k*= targ_multpl;
                
			break;

		default:
			printf("\n\nWarning! --- %ld",__LINE__);
			printf("\n  Invalid swch = %ld",swch);
			
			wpause();	
			break;
		}//end switch

                lp=lp->next;
        }//end while
        ///////////////////////////////////////

        return;
}

template<class TYPE>
TYPE* findData2(long ibs,long zbs,TYPE *dataChain)
{
	//1: init. locals

 	long i,j,flg=NO;//init.
	TYPE *dp;
	
	ibs=abs(ibs);
	zbs=abs(zbs);
	///////////////////////////////////////

	//2: looking for second data

	dp=dataChain;
	while(dp!=NULL)
	{
		//2.1 ibs/i and zbs/j

        i=abs(dp->i);
        j=abs(dp->j);
		////////////////////////////////////

		//2.2 the second ?

	    if(ibs==i && zbs==j && flg==YES) break;
		if(ibs==j && zbs==i && flg==YES) break;
        if(zbs==0 && ibs==i && flg==YES) break;
        if(zbs==0 && ibs==j && flg==YES) break;
		///////////////////////////////////////

		//2.3 marking flg

		if(ibs==i && zbs==j) flg=YES;
        if(ibs==j && zbs==i) flg=YES;
        if(zbs==0 && ibs==i) flg=YES;
        if(zbs==0 && ibs==j) flg=YES;
		///////////////////////////////////////

		//2.4 the next

        dp=dp->next;
    }//end while
    ////////////////////////////////

    return dp;
}

struct nodestr* findData(char *id,struct nodestr *nodeData,long attr)
//attr: PQ, PV, or PVSLACK
{
	struct nodestr *np=nodeData;
	while(np!=NULL)
	{
		if(!strcmp(np->id,id))
		{
			if(attr==PQ && np->attr==PQ) break;
			if(attr==PV && np->attr==PV) break;
			if(attr==PVSLACK && (np->attr==PV || np->attr==SLACK)) break;
		}//end if

		np=np->next;
	}//end while
	////////////////////////////////////

	return np;
}

struct nodestr* findData(long ibs,struct nodestr *nodeData,long attr)
//attr: PQ, PV, or PVSLACK
{
	struct nodestr *np=nodeData;
	while(np!=NULL)
	{
		if(np->i==ibs)
		{
			if(attr==PQ && np->attr==PQ) break;
			if(attr==PV && np->attr==PV) break;
			if(attr==PVSLACK && (np->attr==PV || np->attr==SLACK)) break;
		}//end if

		np=np->next;
	}//end while
	/////////////////////////////////

	return np;
}

long brMatch(struct lnstr ln,long i,long j)
{
	long ret=NO;//init.

	long i1=abs(ln.i);
        long j1=abs(ln.j);

        long i2=abs(i);
        long j2=abs(j);

        if(i1==i2 && j1==j2) ret=YES;
        if(i1==j2 && j1==i2) ret=YES;

	return ret;
}

template<class TYPE>
long brMatch(TYPE *bp,long i,long j)
{
        long ret=NO;//init.

        long i1=abs(bp->i);
        long j1=abs(bp->j);

        long i2=abs(i);
        long j2=abs(j);

        if(i1==i2 && j1==j2) ret=YES;
        if(i1==j2 && j1==i2) ret=YES;

        return ret;
}

long match(struct linestr *lp1,struct linestr *lp2)
{ 
	long ret=brMatch(lp1,lp2->i,lp2->j);
	return ret; 
}

long match(struct xfmrstr *xp1,struct xfmrstr *xp2)
{ 
	long ret=brMatch(xp1,xp2->i,xp2->j); 
	return ret;
}

long match(struct nodestr *np1,struct nodestr *np2)
{
        if(np1->i==np2->i) return YES;
        else return NO;
}

long match(struct capastr *cp1,struct capastr *cp2)
{
        if(cp1->i==cp2->i) return YES;
        else return NO;
}

long match(struct genrstr *gp1,struct genrstr *gp2)
{
	if(gp1->i==gp2->i) return YES;
	else return NO;
}

long match(struct pricestr *pp1,struct pricestr *pp2)
{
	if(pp1->i==pp2->i) return YES;
	else return NO;
}

void combinData(struct pricestr *pp,struct pricestr *pp2)
{
	//1: verification of bidding curves to be combined

	if(pp->pntnum!=pp2->pntnum)
	{
		printf("\n\nError! --- %ld",__LINE__);
		printf("\n  Invalid priceData:");

		printf("\n  id=%s, pntnum=%ld",pp->id,pp->pntnum);
		printf("\n  id=%s, pntnum=%ld",pp2->id,pp2->pntnum);
	
		exit(0);
	}//end if
	//////////////////////////////////////////

	//2: combining curve pp2 into pp

	register long i;
	double aux2;

	for(i=1;i<=pp->pntnum;i++)
	{
		aux2=pp2->point[i].x*pp2->Pb;
		aux2/= pp->Pb;

		pp->point[i].x+= aux2; 
	}//end for
	////////////////////////////////////

	return;
}

void combinData(struct xfmrstr *xp,struct xfmrstr *xp2)
{
        xp->Kmin=max(xp->Kmin,xp2->Kmin);
        xp->Kmax=min(xp->Kmax,xp2->Kmax);

        xp->eps=min(xp->eps,xp2->eps);
        xp->maxstep=min(xp->maxstep,xp2->maxstep);

        return;
}

void combinData(struct nodestr *np,struct nodestr *np2)
{
	long attr=np->attr;//init.
	struct genrstr *gp;
	/////////////////////////////////////////

	//1: for PQ node
	
	if(attr==PQ)
	{
      	//1.1 PQ <==> PQ

        if(np2->attr==PQ)
        {
                np->p+= np2->p;
                np->qv+= np2->qv;
        }
       	////////////////////////////////////////

		//1.2 PQ <==> PV or SLACK

		if(np2->attr==PV || np2->attr==SLACK)
		{
            //gp=findData(np2->id,genrData);
            gp=findData(np2->i,genrData);
            if(gp==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }

            genrDataSlide(gp,np->p,np->qv,'-');
			///////////////////////////////////////////

            strcpy(np->descr,np2->descr);
            strcpy(np->id,np2->id);
			
			if(np2->attr==PV)
			{
                        	np->attr=PV;        //PQ --> PV

                        	np->p=np2->p-np->p; //active gen.
                        	np->qv=np2->qv;       //voltage
			}
			else//SLACK
			{
                        	np->attr=SLACK;    //PQ --> SLCK

                        	np->p=np2->p;     //voltage
                        	np->qv=np2->qv;     //theta
			}//end else
		}//end if
	}//end if
	//////////////////////////////////////////////////////////////////

	//2: for PV node
	
	if(attr==PV)
	{
		//2.0 find other data: genr. and price

		//gp=findData(np->id,genrData);
		gp=findData(np->i,genrData);
		if(gp==NULL)
		{
			printf("\n\nError! --- %ld",__LINE__);
			printf("\n  没有发电机数据: ibs=%ld, id=%s",np->i,np->id);
			
			wpause(); exit(0);
		}//end if
		///////////////////////////////////////////////////////////

		//2.1 PV <==> PQ

		if(np2->attr==PQ)
		{
			genrDataSlide(gp,np2->p,np2->qv,'-');
		 	np->p-= np2->p;
		}//end if
		////////////////////////////////////////////

		//2.2 PV <==> PV --- no other data

		if(np2->attr==PV) 
			np->p+= np2->p;
		///////////////////////////////////////////

		//2.3 PV <==> SLACK --- no other data 

		if(np2->attr==SLACK)
		{
			strcpy(np->descr,np2->descr);
			strcpy(np->id,np2->id);

			np->attr=SLACK;  //PV --> SLCK
			
			np->p=np2->p;   //voltage
			np->qv=np2->qv;   //theta
		}//end if
	}//end if 
	//////////////////////////////////////////

	return;
}

void combinData(struct linestr *lp,struct linestr *lp2)
{
        COMPLEX z,z2;
        COMPLEX aux2,aux4,ret;
        //////////////////////////////////////
        
        //1: init. variables of z, z2

        z.re = lp->r;
        z.im = lp->x;

        z2.re = lp2->r;
        z2.im = lp2->x;
        ///////////////////////////////////

        //2: combination of impedance

        aux2 = z * z2;//complex
        aux4 = z + z2;

        ret = aux2/aux4;
       
        lp->r = ret.re;
        lp->x = ret.im;
        ////////////////////////////////////////

        //3: consideration of b0k and lmt
		
        if(lp->i>0 && lp->j>0) lp->b0k+= lp2->b0k;

        lp->lmt+= lp2->lmt;
        ///////////////////////////////////

        return;
}

void combinData(struct capastr *cp,struct capastr *cp2)
{
        cp->Q0+= cp2->Q0;//original reactive

        cp->Qmin+= cp2->Qmin;//No Change!
        cp->Qmax+= cp2->Qmax;

        cp->eps=min(cp->eps,cp2->eps);
        cp->maxstep=min(cp->maxstep,cp2->maxstep);

        return;
}

void combinData(struct genrstr *gp,struct genrstr *gp2)
{
	struct nodestr *np;

	np=findData(gp->i,nodeData,PVSLACK);
	if(np==NULL){ printf("\n\nError! --- %ld",__LINE__); exit(0); }
	///////////////////////////////////////////////

	strcpy(gp->descr,np->descr);
	strcpy(gp->id,np->id);

	gp->w0+= gp->w0;
	gp->r0+= gp->r0;

	gp->wmin+= gp->wmin;
	gp->wmax+= gp->wmax;

	gp->rmin+= gp->rmin;
	gp->rmax+= gp->rmax;

	gp->eps=min(gp->eps,gp2->eps);
	gp->maxstep=min(gp->maxstep,gp2->maxstep);

	return;
}

void filterData(struct pricestr *&priceData)
{
        register long i;

	long genrLen;
	long priceLen;

        struct genrstr *gp;
        struct pricestr *pp;
	////////////////////////////////////////////////////

	//1: chain length examination

        genrLen =chainLen(genrData);
        priceLen=chainLen(priceData);

        if(priceLen<genrLen)
        {
                printf("\n\nError! --- %ld",__LINE__);
                printf("\n  NOT enough priceData: priceLen=%ld, genrLen=%ld",priceLen,genrLen);

		exit(0);
        }//end if
        ///////////////////////////////////////////////////

	//2: priceData sufficiency 

	gp=genrData;//init.
	for(i=1;i<=genrLen;i++,gp=gp->next)
	{
		pp=findData(gp->i,priceData);
		if(pp==NULL)
		{
			printf("\n\nError! --- %ld",__LINE__);
			printf("\n  No price found for genr: id=%s, descr=%s",gp->id,gp->descr);
		
			exit(0);
		}	
	}//end for
	//////////////////////////////////////////////////

        //3: price/genrData consistency  

	gp=genrData;//init.
        pp=priceData;

	for(i=1;i<=genrLen;i++,gp=gp->next,pp=pp->next)
	{
                if(gp->i!=pp->i)//by ibs
		{
			printf("\n\nError! --- %ld",__LINE__);
			printf("\n  Invalid priceData: id=%s, descr=%s",pp->id,pp->descr);
			printf("\n          genrData : id=%s, descr=%s",gp->id,gp->descr);
		
			exit(0);
		}
	}//end for 
        //////////////////////////////////////////////////

        return;
}

void filterData(struct voltstr *&voltData)
{
	struct linestr *lp;
	struct voltstr *vp,*next;
	////////////////////////////////////

	vp=voltData;
	while(vp!=NULL)
	{
		lp=findData(vp->i,(long)0,lineData);
		if(lp==NULL)
		{
			next=vp->next;
			delChainNode(voltData,vp);

			vp=next;
		}
		else
			vp=vp->next;
	}//end while
	//////////////////////////////////

	return;
}

void filterData(struct xfmrstr *&xfmrData)
{
	struct linestr *lp;
	struct xfmrstr *xp,*next;
	///////////////////////////////////

	xp=xfmrData;
	while(xp!=NULL)
	{
		lp=findData(xp->i,xp->j,lineData);
		if(lp==NULL)
		{
			next=xp->next;
			delChainNode(xfmrData,xp);

			xp=next;
		}
		else
			xp=xp->next;
	}//end while
	///////////////////////////
	
	return;
}

void filterData(struct groupstr *&groupData)
{
	register long i,j,k;
	long ibs,zbs,n;

	struct groupstr *grp,*next;
	////////////////////////////////////////////////

	//1: filtering branches in a group

	grp=groupData;
	while(grp!=NULL)
	{
		n=grp->lineNum;//init.
		for(i=1;i<=n-1;i++)
		{
			//.1 current branch

			ibs=grp->line[i].i;
			zbs=grp->line[i].j;

			//.2 del. repeated branch

			for(j=i+1;j<=n;j++)
			{
				if(brMatch(grp->line[j],ibs,zbs)!=YES) continue;

				for(k=j;k<=n-1;k++)
				{
					strcpy(grp->line[k].id,grp->line[k+1].id);
					grp->line[k].i=grp->line[k+1].i;
					grp->line[k].j=grp->line[k+1].j;

					strcpy(grp->line[k].mst_id,grp->line[k+1].mst_id);
					grp->line[k].imst=grp->line[k+1].imst;
				}//end for

				n--;//No Change!
				j--;
			}//end for
		}//end for
		/////////////////////////////////////

		grp->lineNum=n;
		grp=grp->next;
	}//end while
	///////////////////////////////////////////////////////

	//2: filtering empty group.data

	grp=groupData;
	while(grp!=NULL)
	{
		if(grp->lineNum<=0)//empty
		{
			next=grp->next;
			delChainNode(groupData,grp);

			grp=next;
		}
		else	
			grp=grp->next;
	}//end while
	/////////////////////////////////////

	return;
}

void filterData(struct genrstr *&genrData)
{
        struct genrstr *gp,*next;
        struct nodestr *np;
        //////////////////////////////////////

        gp=genrData;
        while(gp!=NULL)
        {
			//.1 ibs validity

			np=findData(gp->i,nodeData);//by ibs, not id
			if(np==NULL)
			{
				next=gp->next;
				delChainNode(genrData,gp);
			
				gp=next;
				continue;
			}//end if
			///////////////////////////////////////////

			//.2 attr. and active power

			if(np->attr==PQ || np->p<SMLL)
			{
				next=gp->next;
				delChainNode(genrData,gp);

				gp=next;
				continue;
			}//end if
			////////////////////////////////////////

			gp=gp->next;
        }//end while
        ///////////////////////////

        return;
}
         
void filterData(struct capastr *&capaData,long planFlg)
//planFlg: YES or NO
{
	double aux2,aux4;
	long flg;

	struct capastr *cp,*next;
	struct genrstr *gp;
	struct linestr *lp;
	struct nodestr *np;
	///////////////////////////////////////////

	cp=capaData;
        while(cp!=NULL)
        {
                flg=NO;//init. --- No Change! 
               	/////////////////////////////////////

		//.1 top. validity 
	
		lp=findData(cp->i,(long)0,lineData);
                if(lp==NULL) flg=YES;
        	//////////////////////////////////////

		//.2 reactive limits
        
		if(planFlg==NO)
                {
                        aux2=fabs(cp->Qmin);
                        aux4=fabs(cp->Qmax);

                        if(aux2<SMLL && aux4<SMLL) flg=YES;
                }//end if
		/////////////////////////////////////////

		//.3 considering node attr.

		np=findData(cp->i,nodeData,PVSLACK);
		if(np!=NULL) flg=YES;
		//////////////////////////////////////

		//.4 considering genr.data

		gp=findData(cp->i,genrData);
		if(gp!=NULL)
		{
			gp->r0   += cp->Q0;
			gp->rmin += cp->Qmin;
			gp->rmax += cp->Qmax;

			flg=YES;
		}//end if	
                ///////////////////////////////////////

                //del. or not

                if(flg==YES)
                {
                        next=cp->next;
                        delChainNode(capaData,cp);

                        cp=next;
                }
                else
                        cp=cp->next;
        }//end while
        /////////////////////////////////////

	return;
}

void filterData(struct nodestr *&nodeData)
{
        long flag;
        struct nodestr *np,*next;
        //////////////////////////////////

        np=nodeData;
        while(np!=NULL)
        {
                flag=NO;//init.
                switch(np->attr)
                {
                case PQ:
                        if(fabs(np->p)<SMLL && fabs(np->qv)<SMLL) flag=YES;
                        break;

                case PV:
                        if(fabs(np->p)<SMLL || fabs(np->qv)<SMLL) flag=YES;
                        break;

                case SLACK:
                        if(fabs(np->p)<SMLL) flag=YES;
                        break;
				
                default:
                        flag=YES;//invalid nodal attr.
                }//end switch
	        //////////////////////////////////////////////

                if(flag==YES)
                {
                        next=np->next;//next reserved
                        delChainNode(nodeData,np);
			
                        np=next;
                }
                else
                        np=np->next;
        }//end while
        //////////////////////////////////

        return;
}

void nodeNumStat(long &nodeNum,long &pvNodeNum,long &slackNum)
{
        register long n;
        long *tempList;

        struct linestr *lp;
        struct nodestr *np;
        ////////////////////////////////////////////////

        //memory allocation

        tempList=(long *)calloc(MAX_NODE+1,sizeof(long));
        if(tempList==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }
        ////////////////////////////////////////////////////////////

        //1: nodeNum stat.

        n=0;//init.--- No Change!
        
	lp=lineData;
	while(lp!=NULL)
        {
		if(n>=MAX_NODE)
		{
			printf("\n\nError! --- %ld",__LINE__);
			printf("\n  Overflow occured: n=%ld, MAX_NODE=%ld",n,MAX_NODE);

			exit(0);
		}//end if

                if(lookup(tempList,lp->i,n)==NO)
                {
                        n++;
                        tempList[n]=abs(lp->i);
                }

                if(lookup(tempList,lp->j,n)==NO)
                {
                        n++;
                        tempList[n]=abs(lp->j);
                }

                lp=lp->next;
        }//end while

        nodeNum=n;//nodeNum obtained
        /////////////////////////////////////////////////

        //2: pvNodeNum and slackNum stat.

        pvNodeNum=0;//init.
        slackNum=0;

        np=nodeData;
        while(np!=NULL)
        {
                if(np->attr==PV)    pvNodeNum++;
                if(np->attr==SLACK) slackNum++;

                np=np->next;
        }//end while
        ///////////////////////////////////////////

        free(tempList);
        return;
}

void autoSlackSet(struct nodestr *nodeData,struct genrstr *genrData,long prnt)
{
	if(prnt==YES) printf("\n\nautoSlackSet()...");

	register long i;
	double Pmax,wload,wgen,wgen2,wslack;

	struct genrstr *gp,*slack_gp;
	struct nodestr *np;
	/////////////////////////////////////////////////////////

	//1: looking for slack --- by Pmax

	Pmax=0.0;//init.
	slack_gp=NULL;

	gp=genrData;
	for(i=1;i<=genrNum;i++,gp=gp->next)
	{
		if(gp->wmax>Pmax)
		{
			Pmax=gp->wmax;
			slack_gp=gp;
		}
	}//end for

	if(slack_gp==NULL){ printf("\n\nError! --- %ld",__LINE__); exit(0); }
	if(prnt==YES) printf("\n slack=%ld, id=%s",slack_gp->i,slack_gp->id);
	///////////////////////////////////////////////////
	
	//2: power balance analysis

	wload =wloadStat(nodeData);
	wgen  =wgenStat(nodeData);
	wgen2 =wgen2Stat(nodeData);
	wslack=wload/(1.0-loss_ratio)-wgen;

	if(prnt==YES) printf("\n wload =%9.4lf",wload);
	if(prnt==YES) printf("\n wgen  =%9.4lf",wgen);
	if(prnt==YES) printf("\n wgen2 =%9.4lf",wgen2);
	if(prnt==YES) printf("\n wslack=%9.4lf, when loss_ratio=%4.2lf%%",wslack,100*loss_ratio);

	if(wslack>min(slack_gp->w0,slack_gp->wmax/2))
	{
		printf("\n\nError! --- %ld",__LINE__);
		printf("\n  Too much wslack = %lf",wslack);

		exit(0);
	}//end if
	/////////////////////////////////////////////

	//3: marking the slack

	np=nodeData;
	while(np!=NULL)
	{
		if(np->i==slack_gp->i)
		{
			np->attr=SLACK;
			np->p=np->qv;
			np->qv=0.0;//in degree

			break;
		}//end if

		np=np->next;
	}//end while
	
	if(prnt==YES) printf("\n nodeData modified");
	/////////////////////////////////////////

	//4: considering pvNodeNum

	pvNodeNum--;//No Change!
	if(prnt==YES) printf("\n pvNodeNum=%ld",pvNodeNum);
	/////////////////////////////////////////

	return;
}

/////////////////////////////////////////
//end of file
