//opffun.cpp by Wang GT
///////////////////////////////

#include"common.h"
#include"opf.h"
#include"opf_global.h"
///////////////////////////////////////////////////////////

long findMarginalNode(struct linestr *lineData,long *list,long n,long line_attr)
{
	long ibs,zbs,k1,k2,ret=NO;//init.
	struct linestr *lp;
	///////////////////////////////////

	lp=lineData;
	while(lp!=NULL)
	{
		if(line_attr==LINE_XF && lp->i>0 && lp->j>0){ lp=lp->next; continue; }
		if(line_attr==LINE_LN && (lp->i<0||lp->j<0)){ lp=lp->next; continue; }

		ibs=abs(lp->i);
		zbs=abs(lp->j);

		k1=lookup(list,ibs,n);
		k2=lookup(list,zbs,n);

		if(k1!=NO && k2==NO){ ret=ibs; break; }
		if(k1==NO && k2!=NO){ ret=zbs; break; }

		lp=lp->next;
	}//end while
	//////////////////////////////////

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
			grp->mode[i].lmt=mode_lmt;
			break;
		}//end if
	}//end for	
	////////////////////////

	return;
}
 
void veriFlowData(struct flowstr *flowData,struct flowstr *subFlowData,double *nodeP,double *nodeQ)
{
        long ii,jj,imax_dP,imax_dQ;
        double max_dP,max_dQ,w1,r1,w2,r2;

        double *dP,*dQ;//power imbalances
        struct flowstr *fp,*sp;
        ///////////////////////////////////////////////////

        dP=(double *)calloc(nodeNum+1,sizeof(double));
        dQ=(double *)calloc(nodeNum+1,sizeof(double));
        if(dP==NULL || dQ==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }

        copyVector(dP,nodeP,nodeNum);//init with active injection
        copyVector(dQ,nodeQ,nodeNum);//init with reactive inj.
        //////////////////////////////////////////////////

        //1: imbalance powers

        fp=flowData;
        while(fp!=NULL)
        {
                ii=lookup(codeList,fp->i,nodeNum);
                jj=lookup(codeList,fp->j,nodeNum);
                if(ii==NO || jj==NO){ printf("\nError! --- %ld",__LINE__); exit(0); }

                dP[ii]-= fp->w1;
                dQ[ii]-= fp->r1;

                dP[jj]-= fp->w2;
                dQ[jj]-= fp->r2;

                fp=fp->next;
        }//end while
        /////////////////////////////////////

        //2: flowData checkup

        imax_dP=posiAbsmax(dP,nodeNum);
        imax_dQ=posiAbsmax(dQ,nodeNum);

        max_dP=dP[imax_dP];
        max_dQ=dQ[imax_dQ];
        ///////////////////////////

        if(fabs(max_dP)>flow_eps || fabs(max_dQ)>flow_eps)
        {
                printf("\n\nError! --- %ld",__LINE__);
                printf("\n  max_dP=%lf, max_dQ=%lf",max_dP,max_dQ);
				
				exit(0);
        }//end if
        ////////////////////////////////////////////////

        //3: subFlowData checkup

        fp=flowData;
        while(fp!=NULL)
        {
                sp=findData(fp->id,subFlowData);
                if(sp==NULL){ fp=fp->next; continue; }
                //////////////////////////////////////

                sp=subFlowData;
                w1=w2=0.0;
                r1=r2=0.0;

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

                max_dP=max(fabs(fp->w1-w1),fabs(fp->w2-w2));
                max_dQ=max(fabs(fp->r1-r1),fabs(fp->r2-r2));

                if(max_dP>flow_eps||max_dQ>flow_eps)
                {
                        printf("\n\nError! --- %ld",__LINE__);

                        printf("\n  flowData: id=%s, ibs=%ld, zbs=%ld",fp->id,fp->i,fp->j);
						printf("\n  branch w1=%lf, sum.sub=%lf",fp->w1,w1);

						exit(0);
                }//end if
                ////////////////////////////////////

                fp=fp->next;
        }//end while
        //////////////////////////////////////

        return;
}

void calFlowData(struct flowstr* &flowData,struct linestr *lineData,double *vv,double *v0,long *codeList)
{
        if(flowData!=NULL) freeChain(flowData);

        long kbFlg,vioFlg;
        double w1,w2,r1,r2,s1,s2,aux2;

        struct flowstr *fp;
        struct linestr *lp;
        //////////////////////////////////////////////////

	flowData=NULL;//init.
        lp=lineData;

	while(lp!=NULL)
        {
       		lineFlow(w1,r1,w2,r2,s1,s2,lp);
              	aux2=max(fabs(s1),fabs(s2));

                kbFlg=NO;//init.
                vioFlg=NO;

                if(lp->lmt>SMLL && aux2>lp->lmt*KB_COEFF) kbFlg=YES;
                if(lp->lmt>SMLL && aux2>lp->lmt) vioFlg=YES;
                ////////////////////////////////////////////////////////

                fp=(struct flowstr *)malloc(sizeof(struct flowstr));
                if(fp==NULL){ printf("\n\nError! --- %ld",__LINE__); exit(0); }

                strcpy(fp->id,lp->id);

                fp->i=lp->i;
                fp->j=lp->j;

                fp->w1=w1;
                fp->r1=r1;
                fp->w2=w2;
                fp->r2=r2;

                fp->s1=s1;
                fp->s2=s2;

                fp->kbFlg=kbFlg;
                fp->vioFlg=vioFlg;

                fp->next=flowData;
                flowData=fp;
                //////////////////////////////

                lp=lp->next;
        }//end while
        //////////////////////////////////////////////////

        turnOver(flowData);
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

	struct linestr *lp,*lp2,*lp2next;
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
                                lp2next=lp2->next;

                                delChainNode(lineData,lp2);
                                lp2=lp2next;
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

void printDataFile(char datafile[],long prnt)
{
	if(prnt==YES) printf("\n\nPrinting Data File...");
	register long i;
	
	long ibs,zbs,imst,n1,n2;
	double wgen,w,r,wt,pf,aux2;
	
	FILE *out=fopen(datafile,"w");
	////////////////////////////////////////////////////////////////////

	fprintf(out,"DE datafile for %s",projname);
        
	fprintf(out,"\n   %9.4lf  av_volt",av_volt);
	fprintf(out,"\n   %9.4lf  flow_eps",flow_eps);
	fprintf(out,"\n   %9.4lf  genw_eps",genw_eps);
	fprintf(out,"\n   %9.4lf  genw_maxstep",genw_maxstep);
	fprintf(out,"\n   %9.4lf  loss_ratio",loss_ratio);
	fprintf(out,"\n   %9.4lf  ov_coeff",ov_coeff);
	fprintf(out,"\n   %9.4lf  sys_wload",sys_wload);
	
	fprintf(out,"\n   %9ld  coslack_num",coslack_num);
	fprintf(out,"\n   %9ld  opf_island",opf_island);
	////////////////////////////////////////////////////////

	fprintf(out,"\nDE LINEDATA");
	fprintf(out,LINE_TITL);

        struct linestr *lp=lineData;
        while(lp!=NULL)
        {
                fprintf(out,LINE_FORM,lp->i,lp->j,lp->r,lp->x,lp->kb,lp->lmt,lp->st,lp->zst,lp->id);

                lp=lp->next;
        }//end while

        if(prnt==YES) printf("\n LineData OK");
        /////////////////////////////////////////////////

        fprintf(out,"\nDE NODEDATA");
        struct nodestr *np=nodeData;
        while(np!=NULL)
        {
                fprintf(out,NODE_FORM,np->i,np->attr,np->p,np->q,np->id);

                np=np->next;
        }//end while

        if(prnt==YES) printf("\n NodeData OK");
        ///////////////////////////////////////////

        fprintf(out,"\nDE CAPADATA");
        fprintf(out,CAPA_TITL);//the title

        struct capastr *cp=capaData;
        while(cp!=NULL)
        {
                fprintf(out,CAPA_FORM,cp->i,cp->Q0,cp->Qmin,cp->Qmax,cp->id);
                cp=cp->next;
        }//end while

        if(prnt==YES) printf("\n CapaData OK");
        //////////////////////////////////////////////

        fprintf(out,"\nDE GENRDATA");
        fprintf(out,GENR_TITL);

        struct genrstr *gp=genrData;
        while(gp!=NULL)
        {
                fprintf(out,GENR_FORM,gp->i,gp->Pmin,gp->Pmax,gp->ramp,gp->fall,gp->id);
                gp=gp->next;
		}//end while

        if(prnt==YES) printf("\n GenrData OK");
 	/////////////////////////////////////////

	fprintf(out,"\nDE GENRDAT2");      
	fprintf(out,GENR_TITL2);

	gp=genrData2;
	while(gp!=NULL)
	{
		fprintf(out,GENR_FORM2,gp->i,gp->P0,gp->id);

		gp=gp->next;
	}//end while

	if(prnt==YES) printf("\n GenrData2 OK");
	///////////////////////////////////////////

	fprintf(out,"\nDE PLANTDAT");
	fprintf(out,PLANT_TITL);

	struct plantstr *plp=plantData;
	while(plp!=NULL)
	{
		strcpy(idbuf,plp->id);
		stringEnd(idbuf,(long)10);

		fprintf(out,PLANT_FORM,idbuf,plp->w,plp->unitNum);
		for(i=1;i<=plp->unitNum;i++)
			fprintf(out,"\n    unit_id = %s",plp->unit[i].id);

		plp=plp->next;
	}//end while

	if(prnt==YES) printf("\n PlantData OK");
	//////////////////////////////////////////////

	fprintf(out,"\nDE PRICDATA");
	fprintf(out,PRIC_TITL);

        struct pricstr *pp=priceData;
        while(pp!=NULL)
        {
                fprintf(out,PRIC_FORM,pp->i,pp->pntNum,pp->Pb,pp->Cb,pp->id);
                fprintf(out,"   ");

                for(i=1;i<=pp->pntNum;i++)
				{
					aux2=pp->point[i].p;	//p_axis --- power
					fprintf(out," %5.2lf",aux2);
				}//end for

                fprintf(out,"   ");
                for(i=1;i<=pp->pntNum;i++)
				{
					aux2=pp->point[i].c;	//c_axis --- price
					fprintf(out," %5.2lf",aux2);
				}//end for

                pp=pp->next;
        }//end while

        if(prnt==YES) printf("\n PricData OK");
        ///////////////////////////////////////////

	fprintf(out,"\nDE GROUPDAT");
	fprintf(out,GROUP_TITL);

	struct groupstr *grp=groupData;
	while(grp!=NULL)
	{
		fprintf(out,GROUP_FORM,grp->id,grp->lineNum,grp->lmt);
		for(i=1;i<=grp->lineNum;i++)
		{
			ibs=grp->line[i].i;
			zbs=grp->line[i].j;
			imst=grp->line[i].imst;

			fprintf(out,GROUP_FORM_LN,ibs,zbs,imst);
		}//end for

		grp=grp->next;
	}//end while

	if(prnt==YES) printf("\n GroupData OK");
	//////////////////////////////////////////////

	fprintf(out,"\nDE GROUPMOD");
	fprintf(out,"\nDE id   |modeNum |descr || mode_id |mode_lmt |conditions #1 #2 #3");

	long mel; 
	double val;

	grp=groupData;
	while(grp!=NULL)
	{
		fprintf(out,"\n   %s %4ld %s",grp->id,grp->modeNum,grp->descr);
		for(i=1;i<=grp->modeNum;i++)
		{
			fprintf(out,"\n     %s %7.2lf",grp->mode[i].id,grp->mode[i].lmt);
			
			strcpy(idbuf,grp->mode[i].id1);
			mel=grp->mode[i].mel1;
			val=grp->mode[i].val1;
			fprintCondition(out,idbuf,mel,val);

			strcpy(idbuf,grp->mode[i].id2);
                        mel=grp->mode[i].mel2;
                        val=grp->mode[i].val2;
                        fprintCondition(out,idbuf,mel,val);

			strcpy(idbuf,grp->mode[i].id3);
                        mel=grp->mode[i].mel3;
                        val=grp->mode[i].val3;
                        fprintCondition(out,idbuf,mel,val);
		}//end for
	
		grp=grp->next;
        }//end while
	
	if(prnt==YES) printf("\n GroupMode OK");
	/////////////////////////////////////////////////////

	fprintf(out,"\nDE LOADDATA");
	fprintf(out,LOAD_TITL);

	struct loadstr *ldp=loadData;
	while(ldp!=NULL)
	{
		fprintf(out,LOAD_FORM,ldp->i,ldp->w,ldp->r,ldp->id);

		ldp=ldp->next;
	}//end while

	if(prnt==YES) printf("\n LoadData OK");
	////////////////////////////////////////////////

	fprintf(out,"\nDE AREADATA");
	fprintf(out,AREA_TITL);

	struct areastr *ap=areaData;
	while(ap!=NULL)
	{
		//.1 area information

		strcpy(idbuf,ap->id);
		strcpy(buf,ap->descr);

		wgen=ap->wgen;
		n1=ap->lineNum;
		n2=ap->unitNum;

		fprintf(out,AREA_FORM,idbuf,wgen,n1,n2,buf);
		////////////////////////////////////////////////

		//.2 lines of area

		for(i=1;i<=ap->lineNum;i++)
		{
			strcpy(idbuf,ap->line[i].id);
			
			ibs=ap->line[i].i;
			zbs=ap->line[i].j;

			w=ap->line[i].w;
			r=ap->line[i].r;
			wt=ap->line[i].wt;
			pf=ap->line[i].pf;

			fprintf(out,ARLN_FORM,ibs,zbs,idbuf,w,r,wt,pf);
		}//end for
		/////////////////////////////////////////

		//.3 units of area

		for(i=1;i<=ap->unitNum;i++)
		{
			strcpy(idbuf,ap->unit[i].id);
			ibs=ap->unit[i].i;
			w=ap->unit[i].w;

			fprintf(out,ARUN_FORM,ibs,idbuf,w);
		}//end for

		ap=ap->next;
	}//end while

	if(prnt==YES) printf("\n AreaData OK");
	//////////////////////////////////////////////////////

	fprintf(out,"\nDE ASSODATA");
	fprintf(out,ASSO_TITL);

	struct assostr *asp=assoData;
	while(asp!=NULL)
	{
		strcpy(idbuf,asp->id);
		stringEnd(idbuf,(long)12);

		fprintf(out,ASSO_FORM,idbuf,asp->unitNum);
		/////////////////////////////////////////////

		for(i=1;i<=asp->unitNum;i++)
		{
			ibs=asp->unit[i].i;
			strcpy(idbuf,asp->unit[i].id);
			stringEnd(idbuf,(long)6);

			fprintf(out,"  %4ld %s",ibs,idbuf);
		}//end for
	
		asp=asp->next;
	}//end while

	if(prnt==YES) printf("\n AssoData OK");
	//////////////////////////////////////////////////
	
        fprintf(out,"\nDE end of file");
	fclose(out);
       
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

void filterData(struct pricstr *&priceData)
{
	//1: length checkup

	long genrLen=chainLen(genrData);
	long pricLen=chainLen(priceData);

	if(pricLen<genrLen)
	{
		printf("\n\nError! --- %ld",__LINE__);
		printf("\n  genrLen=%ld, pricLen=%ld",genrLen,pricLen);

		exit(0);
	}//end if
	//////////////////////////////////////////

	//2: sorting priceData

	struct genrstr *gp=genrData;
	struct pricstr *pp=priceData,*pp2;
        
	while(gp!=NULL)
        {
		if(gp->i==pp->i)
		{
			gp=gp->next;
			pp=pp->next;

			continue;
		}//end if
		////////////////////////////////

		pp2=findData(gp->i,priceData);

		if(pp2==NULL)
		{
			printf("\n\nError! --- %ld",__LINE__);
			printf("\n  NOT found priceData: id=%s, ibs=%ld",gp->id,gp->i);

			exit(0);
		}//end if
		//////////////////////////////////

		swapChainNodes(pp,pp2,priceData);

                gp=gp->next;
		pp=pp->next;
        }//end while
	///////////////////////////////////

	//3: deleting redundant pric.data
 
	if(pricLen>genrLen)
	{
		trunChain(priceData,genrLen);
		printf("\n priceData truncated");
       	}//end if
	/////////////////////////////////////////////////////
 
	return;
}

void incremental(double *inc,double *crr,struct genrstr *genrData,long swch)
{
        register long i;
        struct genrstr *gp=genrData;

        for(i=1;i<=genrNum;i++,gp=gp->next)
	{                
		if(swch==GENR_POWR) inc[i]=crr[i]-gp->P0;
		if(swch==GENR_VOLT) inc[i]=crr[i]-gp->V0;
	}//end for
	/////////////////////////

        return;
}

void incremental(double *inc,double *crr,struct xfmrstr *xfmrData)
{
	register long i;
        struct xfmrstr *xp=xfmrData;

	for(i=1;i<=xfmrNum;i++,xp=xp->next)
		inc[i]=crr[i]-xp->K0;

	return;
}

void incremental(double *inc,double *crr,struct capastr *capaData)
{
	register long i;
        struct capastr *cp=capaData;

	for(i=1;i<=capaNum;i++,cp=cp->next)
		inc[i]=crr[i]-cp->Q0;

	return;
}
	
long sawtooth(double *prevInc,double *currInc,long n)
{
        register long i,ii,k;
        double aux2,aux4;
        //////////////////////////////

        //1: counting

        ii=k=0;//init.

        for(i=1;i<=n;i++)
        {
                if(fabs(currInc[i])<EPS) continue;

                aux2=prevInc[i];
                aux4=currInc[i];
		
		k++;		
                if(oppSign(aux4,aux2)==YES && fabs(aux4)>fabs(aux2)) ii++;
        }//end for
        //////////////////////////////

        //2: conclusion

        if(ii>k/2) return YES;
        return NO;
}

long reguNum(double *inc,double eps,long n)
{
        register long i,k=0;//init.

        for(i=1;i<=n;i++)
                if(fabs(inc[i])>=eps) k++;

        return k;
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
			aux2=fabs(gp->P-gp->P0);
			if(aux2>=genw_eps) ret++;
		}//end if

		if(swtch==GENR_VOLT)
		{
			aux2=fabs(gp->V-gp->V0);
			if(aux2>=genv_eps) ret++;
		}//end if
		
		gp=gp->next;
	}//end while
	/////////////////////////////////

	return ret;
}

double calPi(double **G,double **B,double *vv,double *v0,long i)
{
        register long j;
        double ret,sum,theta;

        sum=0.0;
        for(j=1;j<=nodeNum;j++)
        {
                theta=v0[i]-v0[j];
                sum+= vv[j]*(G[i][j]*cos(theta)+B[i][j]*sin(theta));
        }//end for

        ret=vv[i]*sum;//Pi obtained
        /////////////////////

        return ret;
}

double calQi(double **G,double **B,double *vv,double *v0,long i)
{
        register long j;
        double ret,sum,theta;

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

void reportVolt(FILE *out,struct voltstr *voltData,long prnt)
{
	long kk=50;
	struct voltstr *vp;
	/////////////////////////////////

	if(prnt==YES)
	{
		printf("\n\nVoltage Monitoring");
		drawline(kk);
		printf(FLOW_VOLT_TITL);
		drawline(kk);
	}//end if

	fprintf(out,"\n\nVoltage Monitoring");
	fdrawline(out,kk);
	fprintf(out,FLOW_VOLT_TITL);
	fdrawline(out,kk);
	////////////////////////////////////////

	vp=voltData;
	while(vp!=NULL)
	{
		if(prnt==YES) printf(FLOW_VOLT_FORM,vp->i,vp->V,vp->Vmin,vp->Vmax,vp->vioFlg,vp->id);
		fprintf(out,FLOW_VOLT_FORM,vp->i,vp->V,vp->Vmin,vp->Vmax,vp->vioFlg,vp->id);

		vp=vp->next;
	}//end while

	if(prnt==YES) drawline(kk);
        fdrawline(out,kk);
	//////////////////////////////////////
	
	return;
}

void reportGroupFlow(FILE *out,struct groupstr *groupData,long prnt)
{
	long kk=50;
	double lmt;
	//////////////////////////////////////

	if(prnt==YES)
	{
		printf("\n\nGroup Flow Report:");
		drawline(kk);
		printf(FLOW_GROUP_TITL);
		drawline(kk);
	}//end if

	fprintf(out,"\n\nGroup Flow Report:");
	fdrawline(out,kk);
	fprintf(out,FLOW_GROUP_TITL); 
	fdrawline(out,kk);
	/////////////////////////////////////////

	struct groupstr *grp=groupData;
	while(grp!=NULL)
	{
		lmt=grp->lmt;//default
		if(grp->imode>0) lmt=grp->mode[grp->imode].lmt;

		if(prnt==YES) printf(FLOW_GROUP_FORM,grp->id,grp->w,grp->imode,lmt,grp->kbFlg,grp->vioFlg);
		fprintf(out,FLOW_GROUP_FORM,grp->id,grp->w,grp->imode,lmt,grp->kbFlg,grp->vioFlg);
	
		grp=grp->next;
	}//end while

	if(prnt==YES) drawline(kk);
	fdrawline(out,kk);
	////////////////////////////////////////////

	return;
}

	
void reportLine(FILE *out,struct flowstr *flowData,struct flowstr *subFlowData,long prnt)
{ 
	long i,j,kk=85;
	double w1,w2,r1,r2,s1,s2;
	////////////////////////////////////////////

	//1: flowData report

	if(prnt==YES)
	{
		printf("\n\nLine Flow Table:");
		drawline(kk);
		printf(FLOW_LINE_TITL);
		drawline(kk);
	}//end if

	fprintf(out,"\n\nLine Flow Table:");
	fdrawline(out,kk);
	fprintf(out,FLOW_LINE_TITL);
	fdrawline(out,kk);
	/////////////////////////////////////

	struct flowstr *fp=flowData;
	while(fp!=NULL)
	{ 
		i=abs(fp->i);
		j=abs(fp->j); 

		w1=fp->w1;
		w2=fp->w2;
		r1=fp->r1;
		r2=fp->r2;
		s1=fp->s1;
		s2=fp->s2;

		if(prnt==YES) printf(FLOW_LINE_FORM,i,j,w1,r1,w2,r2,s1,s2,fp->vioFlg,fp->id);
		fprintf(out,FLOW_LINE_FORM,i,j,w1,r1,w2,r2,s1,s2,fp->vioFlg,fp->id);

		fp=fp->next;
	}//end while

	if(prnt==YES) drawline(kk);
	fdrawline(out,kk);
	///////////////////////////////////////////////
	
	//2: subFlowData report
 
        fprintf(out,"\n\nSubLine Flow Table:");
        fdrawline(out,kk);
        fprintf(out,FLOW_SUBLINE_TITL);
        fdrawline(out,kk);

	fp=subFlowData;
	while(fp!=NULL)
        {
                i=abs(fp->i);
                j=abs(fp->j);

                w1=fp->w1;
                w2=fp->w2;
                r1=fp->r1;
                r2=fp->r2;
                s1=fp->s1;
                s2=fp->s2;

                if(prnt==YES) printf(FLOW_SUBLINE_FORM,i,j,w1,r1,w2,r2,s1,s2,fp->id);
                fprintf(out,FLOW_SUBLINE_FORM,i,j,w1,r1,w2,r2,s1,s2,fp->id);

                fp=fp->next;
        }//end while

        if(prnt==YES) drawline(1,kk);
        fdrawline(out,1,kk);
        //////////////////////////////////

	return;
}

void reportNode(FILE *out,double *vv,double *v0,double *nodeP,double *nodeQ,long *codeList,long prnt)
{ 
	register long i,kk=70;//init.

	double theta,aux2,aux4;
	double zero=0.0;//No Change!
	/////////////////////////////////

	if(prnt==YES)
	{
		printf("\n\nNode Flow Table:");
		drawline(kk);  

		printf(FLOW_NODE_TITL);
		drawline(kk);
	}//end if
	/////////////////////////////////////

	fprintf(out,"\n\nNode Flow Table:");
	fdrawline(out,kk);

	fprintf(out,FLOW_NODE_TITL);
	fdrawline(out,kk);
	//////////////////////////////////////////

	for(i=1;i<=nodeNum;i++)
	{ 
		aux2= nodeP[i]; 
		aux4= nodeQ[i];

		theta=v0[i]*180.0/PAI;// rad ==> deg
   		///////////////////////////////////////////////////////////////////////////////
	 
		if(fabs(aux2)<SMLL && fabs(aux4)<SMLL)//no loads or generation 
		{
			if(prnt==YES) printf(FLOW_NODE_FORM,i,codeList[i],vv[i],theta,zero,zero,zero,zero);
			fprintf(out,FLOW_NODE_FORM,i,codeList[i],vv[i],theta,zero,zero,zero,zero);
		}
		else if(aux2>SMLL)//generation
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

	if(prnt==YES) drawline(kk);
    	fdrawline(out,kk);

	return;
}

void reportSys(FILE *out,struct sysstr sys,long prnt)
{ 
	long kk=75;
  
	if(prnt==YES)
	{
		printf("\n\nSystem Information:");
		drawline(kk);
        
		printf(FLOW_SYS_TITL);	
		drawline(kk);
        	
		printf(FLOW_SYS_FORM,sys.wgen,sys.wload,sys.sysLoss,sys.maxVolt,sys.imaxVolt,
						sys.minVolt,sys.iminVolt,sys.maxLoss,sys.ibsMaxLoss,sys.zbsMaxLoss);
		
		drawline(kk);
	}//end if
	///////////////////////////////////

	fprintf(out,"\n\nSystem Information:");
	fdrawline(out,kk);
	
	fprintf(out,FLOW_SYS_TITL);
	fdrawline(out,kk);
	
	fprintf(out,FLOW_SYS_FORM,sys.wgen,sys.wload,sys.sysLoss,sys.maxVolt,sys.imaxVolt,
					sys.minVolt,sys.iminVolt,sys.maxLoss,sys.ibsMaxLoss,sys.zbsMaxLoss);
	
	fdrawline(out,kk);
	/////////////////////////////////

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

template<class TYPE>
long kbNumStat(TYPE *chain)
{
        long ret=0;//init.
        TYPE *cp=chain;

        while(cp!=NULL)
        {
                if(cp->kbFlg==YES) ret++;
                cp=cp->next;
        }

        return ret;
}

void initGlobal()
{
	//1: global chains

	areaData =NULL;
	assoData =NULL;
	capaData =NULL;
	genrData =NULL;
	genrData2=NULL;
	groupData=NULL;
	lineData =NULL;
	loadData =NULL;
	nodeData =NULL;
	plantData=NULL;
	priceData =NULL;
	shedData =NULL;
	subLineData=NULL;
	voltData =NULL;
	xfmrData =NULL;
	//////////////////////////

	//2: global numbers

	areaNum=  0;
	assoNum=  0;
	capaNum=  0;
	genrNum=  0;
	genrNum2= 0;
	groupNum= 0;
	lineNum=  0;
	loadNum=  0;
	nodeNum=  0;
	plantNum= 0;
	pvNodeNum=0;
	shedNum=  0;
	slckNum=  0;
	voltNum=  0;
	xfmrNum=  0;
	//////////////////////////

	return;
}
void freeGlobal()
{
	//1: data chains //////////////////////////////

	freeChain(areaData);	//1 area powers
	freeChain(assoData);	//2 associate
	freeChain(capaData);	//3 capacitor
	freeChain(genrData);	//4 generator
	freeChain(genrData2);	//5 non-bidding genr.
	freeChain(groupData);	//6 line-group
	freeChain(lineData);	//7 line and xfmr
	freeChain(loadData);	//8 load from Sybase 
	freeChain(nodeData);	//9 node
	freeChain(plantData);	//10 non-bidding plant
	freeChain(priceData);	//11 bidding curve
	freeChain(shedData);	//12 load shed
	freeChain(subLineData);	//13 sub- line and xfmr
	freeChain(voltData);	//14 load voltage
	freeChain(xfmrData);	//15 xfmr appended
	//////////////////////////////////////////////

	//2: flow vectors and matrices
 
    free(codeList);
    free(codeAttr);
    free(dPowr);
	free(nodeP);
	free(nodeQ);
	free(vv);
	free(v0);
	
    freeMatrix(_B1,B1);
    freeMatrix(_B2,B2);
    freeMatrix(_G,G);
    freeMatrix(_B,B);
	//////////////////////////////////////

	return;
}

void getBranchGB(struct linestr *lp,double &brG,double &brB)
{
        double aux2=lp->r*lp->r + lp->x*lp->x;

        brG=lp->r/aux2;
        brB=-lp->x/aux2;
		
        return;
}

void formY(double **G,double **B,long *codeList,long modi)
//modi: MODINO or MODIVAR
{
        register long i,ii,jj;
        double aux,aux2,aux4,aux22,aux42,K,K2;

        struct linestr *lp;
        struct xfmrstr *xp;
        /////////////////////////////////////////

        initMatrix(G,nodeNum,nodeNum,0.0);
        initMatrix(B,nodeNum,nodeNum,0.0);

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

                        B[ii][ii]+= lp->kb;
                        B[jj][jj]+= lp->kb;
                }
                else//xfmr branch
                {
                        aux=lp->r*lp->r + lp->x*lp->x;

                        if(modi==MODIVAR && (xp=findData(lp->i,lp->j,xfmrData))!=NULL)
                        {
                                K=xp->K;
                                K2=K*K;
                        }
                        else
                        {
                                K=lp->kb;
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
        //////////////////////////

        return;
}

void getdPowr(double *dPowr,long swch)
{
        register long i,j,ii;
        double sum,aux2,aux4;
		double theta,auxsin,auxcos;
		//////////////////////////////////////

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
		/////////////////////////// 

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
                        if(attr!=SLK && attr!=PV) count++;
                        
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
		long id,slackID,slackCode;
        double slackP,slackQ,pvQ;

        struct flowstr *fp;

        slackID  =codeList[0];
        slackCode=codeAttr[0];
        ///////////////////////////////////////////

        for(i=1;i<=nodeNum;i++)
        {
                if(codeAttr[i]==SLK)//for slack node
		{
                        slackP=0.0;
                        slackQ=0.0;

                        fp=flowData;
                        while(fp!=NULL)
                        {
                                if(abs(fp->i)==slackID)
                                {
                                        slackP+= fp->w1;
                                        slackQ+= fp->r1;
                                }
                                
				if(abs(fp->j)==slackID)
                                {
                                        slackP+= fp->w2;
                                        slackQ+= fp->r2;
                                }

                                fp=fp->next;
                        }//end while

                        nodeP[i]=slackP;
                        nodeQ[i]=slackQ;
		}//end if
		//////////////////////////////////////

		if(codeAttr[i]==PV)//for PV nodes
		{
                        pvQ=0.0;
                        id=codeList[i];

                        fp=flowData;
                        while(fp!=NULL)
                        {
                                if(abs(fp->i)==id) pvQ+= fp->r1;
                                if(abs(fp->j)==id) pvQ+= fp->r2;

                                fp=fp->next;
                        }//end while

                        nodeQ[i]=pvQ;
        	}//end if
	}//end for
	/////////////////////////////////////////////////

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
                        if(ii!=NO) B2[ii][ii]+= 1.0/lp->x+lp->kb;
                        if(jj!=NO) B2[jj][jj]+= 1.0/lp->x+lp->kb;

                        if(ii!=NO && jj!=NO)
                        {
                                B2[ii][jj]-= 1.0/lp->x;
                                B2[jj][ii]-= 1.0/lp->x;
                        }
                }
                else//xfmr.branch
                {
                        K=lp->kb;       //ratio
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
                if(skip_swch==SKIP_SLCK && codeAttr[i]==SLK) continue;

                if(skip_swch==SKIP_GENR && codeAttr[i]==SLK) continue;
                if(skip_swch==SKIP_GENR && codeAttr[i]==PV)  continue;

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

void OPF_readData(char datafile[],long prnt)
{
        if(prnt==YES) printf("\n\nOPF_readData()...");
        register long i;

		long ia,ib,flag=AAAACASE;//init.
        double fa,fb,fc,fd,fe,ff;

        char strg[3],strgx[10],string[LINELN+1];
        strg[2]=strgx[9]=string[LINELN]='\0';//No Change!

        FILE *in=fopen(datafile,"r");
        if(in==NULL){ printf("\nCannot open %s\n",datafile); exit(0); }
        ///////////////////////////////////////////////////////////

        //1: reading system information

        fgets(strg,3,in);//skipping comments
        while(!strcmp(strg,"DE"))
        {
                fgets(string,LINELN,in);
                fgets(strg,3,in);
        }//end while

        if(fscanf(in,"%lf%lf",&fa,&fb)==2)
        {
                av_volt =fa;
                flow_eps=fb;
        }//end if
		fgets(string,LINELN,in);

		if(fscanf(in,"%lf%lf",&fa,&fb)==2)
        {
                genw_eps    =fa;
                genw_maxstep=fb;
        }//end if
        fgets(string,LINELN,in);

        if(prnt==YES) printf("\n sys.info ok");
        ////////////////////////////////////////////////

        //2: Reading other data

        struct capastr *cp;			//for CAPADATA
        struct genrstr *gp;			//for GENRDATA
		struct groupstr*grp;		//for GROUPDAT

        struct linestr *lp;			//for LINEDATA
        struct nodestr *np;			//for NODEDATA
        struct pricstr *pp;			//for PRICDATA
        struct voltstr *vp;			//for VOLTDATA
        struct xfmrstr *xp;			//for XFMRDATA

        capaData =NULL;
        genrData =NULL;
		groupData=NULL;
        lineData =NULL;
        priceData =NULL;
        nodeData =NULL;
        voltData =NULL;
        xfmrData =NULL;

        do//flexible reading mechanism by Wang GT
        {
                fgets(strg,3,in);
                while(!strcmp(strg,"DE") && !feof(in))
                {
                        fgets(strgx,10,in);
                        fgets(string,LINELN,in);
                        fgets(strg,3,in);
                        /////////////////////////////////////////

                        if(!strcmp(strgx," CAPADATA")) flag=CAPACASE;
                        if(!strcmp(strgx," CTG_DATA")) flag=CTG_CASE;
                        if(!strcmp(strgx," GENRDATA")) flag=GENRCASE;
                        if(!strcmp(strgx," GROUPDAT")) flag=GROUPCAS;

                        if(!strcmp(strgx," LINEDATA")) flag=LINECASE;
                        if(!strcmp(strgx," NODEDATA")) flag=NODECASE;
                        if(!strcmp(strgx," PHASDATA")) flag=PHASCASE;
                        if(!strcmp(strgx," PRICDATA")) flag=PRICCASE;
                        if(!strcmp(strgx," VOLTDATA")) flag=VOLTCASE;
                        if(!strcmp(strgx," XFMRDATA")) flag=XFMRCASE;
                }//end while
                ////////////////////////////////////////////

                switch(flag)
                {
                case CAPACASE:
                        if(fscanf(in,"%ld%lf%lf%lf",&ia,&fa,&fb,&fc)==4)
                        {
                                cp=(struct capastr*)malloc(sizeof(struct capastr));
                                if(cp==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }
                                ///////////////////////////////////////////////////

								fgets(string,LINELN,in);
								packString(string);

                                strcpy(cp->id,string);
                                cp->i   =ia;

                                cp->Q0  =fa;
								cp->Qmin=fb;
								cp->Qmax=fc;

                                cp->next=capaData;
                                capaData=cp;
                        }
                        else
                                fgets(string,LINELN,in);

                        break;

                case GENRCASE:
                        if(fscanf(in,"%ld%lf%lf%lf%lf%lf%lf",&ia,&fa,&fb,&fc,&fd,&fe,&ff)==7)
                        {
                                gp=(struct genrstr*)malloc(sizeof(struct genrstr));
                                if(gp==NULL){ printf("\nError! --- %ld",__LINE__);  exit(0); }
                                ///////////////////////////////////////////////

								fgets(string,LINELN,in);
								packString(string);

								strcpy(gp->id,string);
                                gp->i   =ia;
								
                                gp->Pmin=fa;
                                gp->Pmax=fb;
								
								gp->Vmin=fe;
                                gp->Vmax=ff;

								gp->P0  =0.0;//init.
								gp->Pc  =0.0;
								gp->V0  =0.0;
								gp->ramp=0.0;
								gp->fall=0.0;

                                gp->next=genrData;
                                genrData=gp;
                        }
                        else
                                fgets(string,LINELN,in);

                        break;

				case GROUPCAS:
                        if(fscanf(in,"%s%ld%lf",idbuf,&ia,&fa)==3)
						{
							grp=(struct groupstr *)malloc(sizeof(struct groupstr));
							if(grp==NULL){ printf("\nError! --- %ld",__LINE__);  exit(0); }
							/////////////////////////////////////////////////////

							strcpy(grp->id,idbuf);
							strcpy(grp->descr,"");

							grp->lineNum=ia;
							grp->lmt=fa;
							grp->modeNum=0;//No Change!
							
							for(i=1;i<=grp->lineNum;i++)
							{
								if(fscanf(in,"%ld%ld",&ia,&ib)==2)
								{
									grp->line[i].i=abs(ia);
									grp->line[i].j=abs(ib);

									grp->line[i].imst=abs(ia);//default
									if(ib<0) grp->line[i].imst=abs(ib);
								}//end if
							}//end for

							grp->next=groupData;
							groupData=grp;
						}
						else
							fgets(string,LINELN,in);

						break;

                case LINECASE:
                        if(fscanf(in,"%ld%ld%lf%lf%lf%lf",&ia,&ib,&fa,&fb,&fc,&fd)==6)
                        {
                                lp=(struct linestr*)malloc(sizeof(struct linestr));
                                if(lp==NULL){ printf("\nError! --- %ld",__LINE__);  exit(0); }
                                /////////////////////////////////////////////////////

								fgets(string,LINELN,in);
								packString(string);
                                
								strcpy(lp->id,string);
								strcpy(lp->st,"");
								strcpy(lp->zst,"");

                                lp->i   =ia;
                                lp->j   =ib;
                            
								lp->r   =fa;
                                lp->x   =fb;
                                lp->kb  =fc;
                                //lp->lmt =fd;
								lp->lmt=0.0;//testing

                                lp->next=lineData;
                                lineData=lp;
                        }
                        else
                                fgets(string,LINELN,in);

                        break;

                case NODECASE:
                        if(fscanf(in,"%ld%ld%lf%lf",&ia,&ib,&fa,&fb)==4)
                        {
                                if((np=(struct nodestr*)malloc(sizeof(struct nodestr)))==NULL)
                                { printf("\nAllocation error!");exit(0);}
                                //////////////////////////////////////////////////

 								fgets(string,LINELN,in);
								packString(string);
                                
								strcpy(np->id,string);
                                np->i=ia;

                                np->attr=ib;
                                np->p=fa;
                                np->q=fb;

                                np->next=nodeData;
                                nodeData=np;
                        }
                        else
                                fgets(string,LINELN,in);

                        break;

                case PRICCASE:
                        if(fscanf(in,"%ld%ld%lf%lf",&ia,&ib,&fa,&fb)==4)
                        {
                                pp=(struct pricstr*)malloc(sizeof(struct pricstr));
                                if(pp==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }
                                ///////////////////////////////////////////////////////

                                sprintf(idbuf,"pric%ld",ia);
                                strcpy(pp->id,idbuf);

                                pp->i           =ia;
                                pp->pntNum      =ib;
                                pp->Pb          =fa;
                                pp->Cb          =fb;

                                for(i=1;i<=pp->pntNum;i++)
                                {
                                        fscanf(in,"%lf",&fa);
                                        pp->point[i].p=fa;
                                }//end for

                                for(i=1;i<=pp->pntNum;i++)
                                {
                                        fscanf(in,"%lf",&fa);
                                        pp->point[i].c=fa;
                                }//end for

                                fgets(string,LINELN,in);

                                pp->next=priceData;
                                priceData=pp;
                        }
                        else
                                fgets(string,LINELN,in);

                        break;

                case VOLTCASE:
                        if(fscanf(in,"%ld%lf%lf",&ia,&fa,&fb)==3)
                        {
                                fgets(string,LINELN,in);
                                vp=(struct voltstr*)malloc(sizeof(struct voltstr));

                                if(vp==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }
                                ////////////////////////////////////////////////////

                                sprintf(idbuf,"volt%ld",ia);
                                strcpy(vp->id,idbuf);

                                vp->i   =ia;
                                vp->Vmin=fa;
                                vp->Vmax=fb;

                                vp->next=voltData;
                                voltData=vp;
                        }
                        else
                                fgets(string,LINELN,in);

                        break;

                case XFMRCASE:
                        if(fscanf(in,"%ld%ld%lf%lf",&ia,&ib,&fa,&fb)==4)
                        {
                                fgets(string,LINELN,in);
                                xp=(struct xfmrstr*)malloc(sizeof(struct xfmrstr));

                                if(xp==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }
                                ////////////////////////////////////////////////////

                                sprintf(idbuf,"xfmr%ld-%ld",ia,ib);
                                strcpy(xp->id,idbuf);

                                xp->i	=ia;
                                xp->j	=ib;
								xp->Kmin=fa;
								xp->Kmax=fb;

                                xp->next=xfmrData;
                                xfmrData=xp;
                        }
                        else
                                fgets(string,LINELN,in);

                        break;

                default:
                        break;
                }//end switch
        }while(!feof(in));

        fclose(in);//datafile closed
        ////////////////////////////////

        turnOver(capaData);
        turnOver(genrData);
		turnOver(groupData);

        turnOver(lineData);
        turnOver(nodeData);
        turnOver(priceData);
        turnOver(voltData);
        turnOver(xfmrData);

        if(prnt==YES) printf("\n Other data ok");
        ///////////////////////////////////////////////
        
        return;
}
                                  
void getNodePowr(double *nodeP,double *nodeQ,long *codeList,long modi)
{
	long ii,n=nodeNum;//init.

        struct genrstr *gp;
        struct shedstr *sp;
        //////////////////////////////

        initVector(nodeP,n,0.0);
        initVector(nodeQ,n,0.0);

        struct nodestr *np=nodeData;
        while(np!=NULL)
        {
                ii=lookup(codeList,np->i,n);
                if(ii==NO){ printf("\nError! --- %ld",__LINE__); exit(0); }

                if(np->attr==PV)
                {
                        if(modi==MODIPG && (gp=findData(np->i,genrData))!=NULL)
                                nodeP[ii]+= gp->P;
                        else
                                nodeP[ii]+= np->p;
                }//end if

                if(np->attr==PQ)
                {
                        if(modi==MODILD && (sp=findData(np->i,shedData))!=NULL)
                        {
                                nodeP[ii]-= np->p - sp->ws;
                                nodeQ[ii]-= np->q - sp->rs;
                        }
                        else
                        {
                                nodeP[ii]-= np->p;
                                nodeQ[ii]-= np->q;
                        }
                }//end if

                np=np->next;
        }//end while
        //////////////////////////////////

        struct capastr *cp=capaData;
        while(cp!=NULL)
        {
                ii=lookup(codeList,cp->i,n);
                if(ii==NO){ printf("\nError! --- %ld",__LINE__); exit(0); }

                if(modi==MODIVAR)
                        nodeQ[ii]+= cp->Q;
                else
                        nodeQ[ii]+= cp->Q0;

                cp=cp->next;
        }//end while
        ////////////////////////////////

        return;
}

double slackPhAngl(struct nodestr *nodeData)
{
	double ret=0.0;//init.
	struct nodestr *np=nodeData;
	
	while(np!=NULL)
	{
		if(np->attr==SLK)
		{	
			ret=np->q;
			break;
		}//end if

		np=np->next;
	}//end while
	/////////////////////////

	return ret;
}	

void initVolt(double *vv,double *v0,long *codeList,long modi)
{
	double slack_v0=slackPhAngl(nodeData);
	
	initVector(vv,nodeNum,av_volt);
        initVector(v0,nodeNum,slack_v0*PAI/180.0);//rad <== deg
	///////////////////////////////////////////

	long ii;
	struct nodestr *np=nodeData;

        while(np!=NULL)
        {
                if(np->attr==SLK)
                {
                        ii=lookup(codeList,np->i,nodeNum);
			if(ii==NO){ printf("\nError! --- %ld",__LINE__); exit(0); }

                        vv[ii]=np->p;
                }//end if

                if(np->attr==PV)
                {
                        ii=lookup(codeList,np->i,nodeNum);
                	if(ii==NO){ printf("\nError! --- %ld",__LINE__); exit(0); }

			vv[ii]=np->q;
                }//end if

                np=np->next;
        }//end while
        ////////////////////////////////

        if(modi==MODIVAR)//modification
        {
                struct genrstr *gp=genrData;
                while(gp!=NULL)
                {
                        ii=lookup(codeList,gp->i,nodeNum);
			if(ii==NO){ printf("\nError! --- %ld",__LINE__); exit(0); }

                        vv[ii]=gp->V;
                        gp=gp->next;
                }//end while
        }//end if
        ///////////////////////////////

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

long flowFun(struct flowstr *&flowData,struct flowstr *&subFlowData,struct sysstr &sysInfo,long modi,long prnt)
//modi = MODINO, MODIPG, MODIVAR, or MODILOAD
{
	if(prnt==YES) printf("\n\nflowFun()...");

	long wflg,rflg,swch,imaxdp,itrx;
	long maxdp_wibs,maxdp_ribs;
	double maxdp,maxdp_w,maxdp_r;
	//////////////////////////////////////////

	//1: Preparation for Flow Iteration
 
    formY(G,B,codeList,modi);
    if(prnt==YES) printf("\n formY() ok.");

    getNodePowr(nodeP,nodeQ,codeList,modi);
    if(prnt==YES) printf("\n getNodePowr() ok.");

    initVolt(vv,v0,codeList,modi);
    if(prnt==YES) printf("\n initVolt() ok.");
    //////////////////////////////////////////////////
	//////////////////////////////////////////////////

	//2: PQ_Flow Iteration Section
        
	maxdp_w=maxdp_r=0.0;//init.
	wflg=rflg=1;
		
	itrx=0;
	swch=P_V0;
	//////////////////////////////////////
      
	if(prnt==YES) printf("\n Itrx   maxdp_w      maxdp_r");

	while(wflg==1 || rflg==1)//iteration  begins
	{
		getdPowr(dPowr,swch);//dPowr[] obtained
		imaxdp=posiAbsmax(dPowr,nodeNum);
		maxdp=fabs(dPowr[imaxdp]);

		if(maxdp>LARG) return NO;//Not Convergent
		/////////////////////////////////////////////

		if(swch==P_V0)
		{
			maxdp_w=maxdp;
			maxdp_wibs=codeList[imaxdp];
		}//end if

		if(swch==Q_VV)
		{
			maxdp_r=maxdp;
			maxdp_ribs=codeList[imaxdp];
		}//end if
		/////////////////////////////////////

		if(maxdp<flow_eps)
        {
                if(swch==P_V0) wflg=0;
                if(swch==Q_VV) rflg=0;
        }
        else
        {
                if(swch==P_V0)
                {
                        wflg=1;//wflg set

                        solveFactTab(B1,dPowr,(nodeNum-1));
                        divVector(dPowr,nodeNum-1,av_volt);//only for P_V0

                        modifyV(v0,dPowr,codeAttr,swch);
                }//end if

				if(swch==Q_VV)
                {
                        rflg=1;//rflg set

                        solveFactTab(B2,dPowr,(nodeNum-pvNodeNum-1));
                        modifyV(vv,dPowr,codeAttr,swch);
                }//end if
        }//end else
		////////////////////////////////////////

		if(swch==Q_VV)//report when swch=Q_VV
		{
                if(++itrx>MAX_ITRX) return NO;//Not Convergent
        
				if(prnt==YES) printf("\n %4d:  %9.5lE  %9.5lE",itrx,maxdp_w,maxdp_r);
				if(prnt==YES) printf("  at %4ld %4ld",maxdp_wibs,maxdp_ribs);
        }//end if

		swch*= -1;//switch turned
	}//end while
	//////////////////////////////////////////////////////

	//3: Branch and Nodal Flow Calculation

    calFlowData(flowData,lineData,vv,v0,codeList);
    if(prnt==YES) printf("\n flowData obtained");

    calFlowData(subFlowData,subLineData,vv,v0,codeList);
	if(prnt==YES) printf("\n subFlowData obtained");

    calGenrPowr(nodeP,nodeQ,codeList,codeAttr,flowData);
    if(prnt==YES) printf("\n nodeP,nodeQ[] completed");

    calSysInfo(sysInfo,flowData);
    if(prnt==YES) printf("\n sys.info ok");
    ///////////////////////////////////////////////

    return YES;//normal return
}

void calSysInfo(struct sysstr &sysInfo,struct flowstr *flowData)
{
	//I: node information

    	register long i;
    	long imax,imin,slack;

    	double wload,rload,wgen,rgen;
    	double wslack,rslack;
	////////////////////////////

	slack=codeList[0];//init.
	imax=posiMax(vv,nodeNum);
	imin=posiMin(vv,nodeNum);

	wload=rload=0.0;
	wgen=rgen=0.0;//init.

	for(i=1;i<=nodeNum;i++)
	{
        	if(codeList[i]==slack)
        	{
                	wslack=nodeP[i];
                	rslack=nodeQ[i];
        	}//end if

        	if(nodeP[i]>0.0)//generation
        	{
                	wgen+= nodeP[i];
                	rgen+= nodeQ[i];
        	}
        	else//loads
        	{
                	wload-= nodeP[i];
                	rload-= nodeQ[i];
        	}
	}//end for
	//////////////////////////////

    	sysInfo.wgen    =wgen;
    	sysInfo.wload   =wload;
    	sysInfo.wslack  =wslack;

    	sysInfo.rgen    =rgen;
    	sysInfo.rload   =rload;
    	sysInfo.rslack  =rslack;

    	sysInfo.maxVolt =vv[imax];
    	sysInfo.minVolt =vv[imin];

    	sysInfo.imaxVolt=codeList[imax];
    	sysInfo.iminVolt=codeList[imin];
    	//////////////////////////////////////////

    	//II: branch information

    	long ibs,zbs;
    	double loss,maxLoss,sysLoss;

    	maxLoss=0.0;
    	sysLoss=0.0;//init.
    	////////////////////////////

    	struct flowstr *fp=flowData;
    	struct linestr *lp=lineData;

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
    	/////////////////////////////

    	sysInfo.ibsMaxLoss      =ibs;
    	sysInfo.zbsMaxLoss      =zbs;
    	sysInfo.maxLoss         =maxLoss;
    	sysInfo.sysLoss         =sysLoss;
		//////////////////////////////////////

    	return;
}

double costFun(double *crrGn,long genrNum)
{
        register long i;
        double ret=0.0;//init.

        struct pricstr *pp=priceData;
        for(i=1;i<=genrNum;i++,pp=pp->next)
        {
                if(pp==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }

                ret+= calCC(pp,crrGn[i]);
        }//end for
        ///////////////////

        return ret;
}
                  
double calCC(struct pricstr *pp,double p)
{
        long k;
        double a,b,cc;

        if(p<=0.0) return 0.0;//No generation, no cost.
        /////////////////////////////////////////////////////

        p/= pp->Pb;
        k=findk_ex(pp,p);

		a=(pp->point[k+1].c-pp->point[k].c)/(pp->point[k+1].p-pp->point[k].p);
        b=pp->point[k].c-a*pp->point[k].p;

        cc=(a*p+b)*p*pp->Cb;    //No Change!
        /////////////////////////////////////////

        return cc;
}

long findk(struct pricstr *pp,double p)
{
        register long i,n=pp->pntNum;//init.
	long ret;
	/////////////////////////////////////

        if(p<pp->point[1].p) return NO;
        if(p>=pp->point[n].p) return NO;

        for(i=2;i<=n;i++)
	{
                if(pp->point[i].p>p)
		{
			ret=i-1;//No Change!
			break;
		}
	}//end for
	/////////////////////////////

        return ret;
}

long findk_ex(struct pricstr *pp,double p)
{
	long n=pp->pntNum;

	if(p<pp->point[1].p) return 1;//lower extension
	if(p>=pp->point[n].p) return n-1;//upper extension

	long ret=findk(pp,p);
	return ret;
}

void pickCurrGenr(double *crrGn,double *nodeP,long *codeList)
{
        register long i,ii;

        struct genrstr *gp=genrData;
        for(i=1;i<=genrNum;i++,gp=gp->next)
        {
                ii=lookup(codeList,gp->i,nodeNum);
                if(ii==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }

                crrGn[i]=nodeP[ii];
        }//end for
        ///////////////////////////

        return;
}

void groupModeAnalysis(struct groupstr *groupData,struct flowstr *flowData,struct flowstr *subFlowData)
{
        register long i,j;
        long imode,id_len,mel,flg1,flg2,flg3;

        double val,v;
        struct flowstr *fp;
        /////////////////////////////////////////

        struct groupstr *grp=groupData;
        for(i=1;i<=groupNum;i++,grp=grp->next)
        {
                imode=0;//init.
                for(j=grp->modeNum;j>=1;j--)
                {
                        //condition 1:

                        strcpy(idbuf,grp->mode[j].id1);
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
                        ////////////////////////////////////////

                        //condition 2:

                        strcpy(idbuf,grp->mode[j].id2);
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
                        /////////////////////////////////////////

                        //condition 3:

                        strcpy(idbuf,grp->mode[j].id3);
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

                        if(flg1==YES && flg2==YES && flg3==YES){ imode=j; break; }
                }//end for
                //////////////////////////////////////////////////

                grp->imode=imode;
        }//end for
        ///////////////////////////////

        return;
}

void groupFlow(struct groupstr *groupData,struct flowstr *flowData,struct flowstr *subFlowData)
{
        register long i,j;
	
        long ibs,zbs,imode,imst;
        double aux2,lmt,sum_w;

        struct groupstr *grp;
        struct flowstr *fp;
        struct linestr *lp;
        ///////////////////////////////////////////////////
	
        grp=groupData;
        for(i=1;i<=groupNum;i++,grp=grp->next)
        {
		sum_w=0.0;//init.
		for(j=1;j<=grp->lineNum;j++)
		{
			ibs=grp->line[j].i;
			zbs=grp->line[j].j;

			fp=findData(ibs,zbs,flowData);//No Change!
			lp=findData(ibs,zbs,lineData);
	
			aux2=0.0;//init.
			if(fp!=NULL && lp!=NULL)
			{
				imst=grp->line[j].imst;
	
				if(imst==abs(lp->i)) aux2=fp->w1;
				if(imst==abs(lp->j)) aux2=fp->w2;
			}//end if
			//////////////////////////////////////
 
			grp->line[j].w=aux2;
			sum_w+= aux2;
		}//end for

		grp->w=sum_w;//abs.sum
		///////////////////////////////////

		grp->vioFlg=NO;//init.
		grp->kbFlg=NO;

		lmt=grp->lmt;//default
		imode=grp->imode;
		if(imode>0) lmt=grp->mode[imode].lmt;
			
		if(lmt>SMLL && grp->w>lmt) grp->vioFlg=YES;
		if(lmt>SMLL && grp->w>KB_COEFF*lmt) grp->kbFlg=YES;
        }//end for
        ///////////////////////////////////

        return;
}

void calFlowData(struct flowstr* &flowData,double *vv,double *v0,long *codeList)
{
        register long i;
        long kbFlg,vioFlg;

        double w1,w2,r1,r2,s1,s2,aux2;

        struct flowstr *fp;
        struct linestr *lp;
        ///////////////////////////////////////

        if(flowData!=NULL) freeChain(flowData);//Necessary!

        flowData=NULL;//init.
        lp=lineData;

        for(i=1;i<=lineNum;i++,lp=lp->next)
        {
                fp=(struct flowstr *)malloc(sizeof(struct flowstr));
                if(fp==NULL){ printf("\n\nError! --- %ld",__LINE__); exit(0); }

                kbFlg=vioFlg=NO;//init.

                lineFlow(w1,r1,w2,r2,s1,s2,lp);
                //aux2=max(fabs(s1),fabs(s2));
		aux2=max(fabs(w1),fabs(w2));//for L.N.

                if(lp->lmt>SMLL && aux2>lp->lmt*KB_COEFF) kbFlg=YES;
                if(lp->lmt>SMLL && aux2>lp->lmt) vioFlg=YES;
                ///////////////////////////////////////////////////

                strcpy(fp->id,lp->id);

                fp->i=lp->i;
                fp->j=lp->j;

                fp->w1=w1;
                fp->r1=r1;
                fp->w2=w2;
                fp->r2=r2;

                fp->s1=s1;
                fp->s2=s2;

                fp->kbFlg=kbFlg;
                fp->vioFlg=vioFlg;

                fp->next=flowData;
                flowData=fp;
        }//end while

        turnOver(flowData);//No Change!
        //////////////////////////////////

        return;
}

void lineFlow(double &w1,double &r1,double &w2,double &r2,double &s1,double &s2,struct linestr *lp)
{
        long ii,jj;
        double vi,vj,theta,brG,brB;
        /////////////////////////////////////

        //1: nodal voltages

        ii=lookup(codeList,lp->i,nodeNum);
        jj=lookup(codeList,lp->j,nodeNum);
        if(ii==NO || jj==NO){ printf("\nError! --- %ld",__LINE__); exit(0); }

        vi=vv[ii];
	vj=vv[jj];
	theta=v0[ii]-v0[jj];

	if(lp->i<0) vi/= lp->kb;//K* at i_side
	if(lp->j<0) vj/= lp->kb;//K* at j_side
        /////////////////////////////////////////////////////////

        //2: branch powers 

	getBranchGB(lp,brG,brB);//branch admittance obtained
		
	w1=vi*vi*brG-vi*vj*(brG*cos(theta)+brB*sin(theta));
	r1=vi*vj*(brB*cos(theta)-brG*sin(theta))-vi*vi*brB;

	w2=vj*vj*brG-vi*vj*(brG*cos(theta)-brB*sin(theta));
	r2=vi*vj*(brB*cos(theta)+brG*sin(theta))-vj*vj*brB;

        if(lp->i>0 && lp->j>0)//shunt reactive for power line
        {
		r1-= lp->kb*vi*vi;
		r2-= lp->kb*vj*vj;
        }//end if
        ////////////////////////////

        s1=sign(w1)*sqrt(w1*w1 + r1*r1);
        s2=sign(w2)*sqrt(w2*w2 + r2*r2);
        /////////////////////////////////////////////

        return;
}

void veriFlowData(struct flowstr *flowData,double *nodeP,double *nodeQ,long prnt)
{
	if(prnt==YES) printf("\n\nveriFlowData()...");

	//1: init. dP,dQ[] with nodeP,nodeQ[]

	double *dP,*dQ;//power imbalances
	
	dP=(double *)calloc(nodeNum+1,sizeof(double));
	dQ=(double *)calloc(nodeNum+1,sizeof(double));
	if(dP==NULL || dQ==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }

	copyVector(dP,nodeP,nodeNum);//active injection
	copyVector(dQ,nodeQ,nodeNum);//reactive inj.
	
	if(prnt==YES) printf("\n dP[] and dQ[] init ok");
	//////////////////////////////////////////////////

	//2: cosidering flowData

	long ii,jj;
	struct flowstr *fp=flowData;

	while(fp!=NULL)
	{
		ii=lookup(codeList,fp->i,nodeNum);
		jj=lookup(codeList,fp->j,nodeNum);
		if(ii==NO || jj==NO){ printf("\nError! --- %ld",__LINE__); exit(0); }

		dP[ii]-= fp->w1;
		dQ[ii]-= fp->r1;

		dP[jj]-= fp->w2;
		dQ[jj]-= fp->r2;

		fp=fp->next;
	}//end while

	if(prnt==YES) printf("\n flowData considered");
	//////////////////////////////////////////////////

	//3: checking up

	long imax_dP,imax_dQ;
	double max_dP,max_dQ;

	imax_dP=posiAbsmax(dP,nodeNum);
	imax_dQ=posiAbsmax(dQ,nodeNum);

	max_dP=dP[imax_dP];
	max_dQ=dQ[imax_dQ];
	///////////////////////////

	if(fabs(max_dP)>flow_eps || fabs(max_dQ)>flow_eps)
	{
		printf("\n\nWarning! --- %ld",__LINE__);
		printf("\n  Verification NOT Passed!");
		
		printf("\n  max_dP=% lf",max_dP);
		printf("\n  max_dQ=% lf",max_dQ);

		exit(0);
	}//end if

	if(prnt==YES) printf("\n max_dP=% lf",max_dP);
	if(prnt==YES) printf("\n max_dQ=% lf",max_dQ);
	////////////////////////////////////////////////

	return;
}

void flowInit(long prnt)
{
	if(prnt==YES) printf("\n\nflowInit()...");

	makeMatrix(_G,G,nodeNum+1,nodeNum+1);
	makeMatrix(_B,B,nodeNum+1,nodeNum+1);
	makeMatrix(_B1,B1,nodeNum,nodeNum);
	makeMatrix(_B2,B2,nodeNum-pvNodeNum,nodeNum-pvNodeNum);

	codeList=(long *)calloc(nodeNum+1,sizeof(long));
	codeAttr=(long *)calloc(nodeNum+1,sizeof(long));
	vv=(double *)calloc(nodeNum+1,sizeof(double));
	v0=(double *)calloc(nodeNum+1,sizeof(double));
        
	nodeP=(double *)calloc(nodeNum+1,sizeof(double));
	nodeQ=(double *)calloc(nodeNum+1,sizeof(double));
	dPowr=(double *)calloc(nodeNum+1,sizeof(double));

	if(dPowr==NULL || codeList==NULL){ printf("\n\nError! --- %ld",__LINE__); exit(0); }
	if(prnt==YES) printf("\n memory allocation ok");
	//////////////////////////////////////////////////////

	readVector(initfile,"LIST",codeList,(long)0,nodeNum);
	readVector(initfile,"ATTR",codeAttr,(long)0,nodeNum);
	if(prnt==YES) printf("\n codeList[] and codeAttr[] obtained");

	readSparMatrix(initfile,"B1FT",B1,nodeNum-1);
	readSparMatrix(initfile,"B2FT",B2,nodeNum-1-pvNodeNum);
	if(prnt==YES) printf("\n B1[][] and B2[][] obtained");
	/////////////////////////////////////

	return;
}

void OPF_start(char *project,long prnt)
{
		if(prnt==YES) printf("\n\nOPF_start()...");

		//1: buf[] and projname

		strcpy(buf,project);//public

		strcpy(projname,buf);
		strcat(projname," Project");
		///////////////////////////////////////
		
		//2: file names

        strcpy(basefile,buf);
        strcat(basefile,".bas");

        strcpy(datafile,buf);
		strcat(datafile,".dat");

		strcpy(initfile,buf);
        strcat(initfile,".ini");

		strcpy(mlabfile,buf);
		strcat(mlabfile,".m");

        strcpy(resufile,buf);
        strcat(resufile,".res");

        strcpy(sensfile,buf);
        strcat(sensfile,".sns");

        strcpy(tempfile,buf);
        strcat(tempfile,".tmp");

        strcpy(verifile,buf);
        strcat(verifile,".vrd");
		////////////////////////////////////

		//3: init. others

		initGlobal();
        return;
}

void loadDisp(struct nodestr *nodeData,struct loadstr *loadData,double sys_load,long prnt)
{
	if(prnt==YES) printf("\n\nloadDisp()...");

	double sum_w,phai,aux0,aux2;

	struct loadstr *ldp,*ldpNext;
	struct nodestr *np;
	////////////////////////////////////////////

	//1: filtering loadData by nodeData_PQ

        ldp=loadData;
        while(ldp!=NULL)
        {
                np=findData(ldp->id,nodeData,PQ);
                if(np==NULL)
                {
			ldpNext=ldp->next;
                        delChainNode(loadData,ldp);

                        ldp=ldpNext;
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
                np->q=ldp->r;

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

void plantSched(struct plantstr *plantData,struct nodestr *nodeData,long prnt)
{
	if(prnt==YES) printf("\n\nplantSched()...");

	register long i,j;
	long n;
	double plant_wmx,aux0;

	struct nodestr *np;
	struct plantstr *pp;
	/////////////////////////////////////////

	//1: marking all online units

	n=chainLen(plantData);
	pp=plantData;

	for(i=1;i<=n;i++,pp=pp->next)
	{
		if(pp->w<SMLL) continue;//plant no sched.

		for(j=1;j<=pp->unitNum;j++)
		{
			pp->unit[j].i=0;//init.
			strcpy(idbuf,pp->unit[j].id);
			
			np=findData(idbuf,nodeData,PVSLACK);
			if(np!=NULL) pp->unit[j].i=np->i;
		}//end for
	}//end for

	if(prnt==YES) printf("\n online units marked"); 
	//////////////////////////////////////////////////

	//2: perform sched.dispatch

	pp=plantData;
	for(i=1;i<=n;i++,pp=pp->next)	
	{
		if(pp->w<SMLL) continue;//plant no sched.

		plant_wmx=0.0;//init.
		for(j=1;j<=pp->unitNum;j++)
		{
			if(pp->unit[j].i==0) continue;
			plant_wmx+= pp->unit[j].wmx;
		}//end for
		
		if(plant_wmx<SMLL)
		{
			printf("\n\nWarning! --- %ld",__LINE__);
			printf("\n  Plant Offline: id=%s w=%7.4lf",pp->id,pp->w);
		
			continue;
		}//end if 
		/////////////////////////////////////////////////////

		aux0=pp->w/plant_wmx;
		for(j=1;j<=pp->unitNum;j++)
		{
			if(pp->unit[j].i==0) pp->unit[j].w=0.0;//offline
			else
			{
				pp->unit[j].w=pp->unit[j].wmx*aux0;
			
				strcpy(idbuf,pp->unit[j].id);
				np=findData(idbuf,nodeData,PVSLACK);
				
				if(np==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }
				if(np->attr==SLACK) continue;

				if(fabs(np->p)>SMLL){ printf("\nError! --- %ld",__LINE__); exit(0); }
				np->p=pp->unit[j].w;
			}//end else
		}//end for
	}//end for

	if(prnt==YES) printf("\n scheduling ok");
	////////////////////////////////////////////////

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
			printf("\n  Unit Offline: id=%s P0=%7.4lf",gp->id,gp->P0);

			continue;
		}//end if

		if(np->attr==PV && fabs(np->p)>SMLL){ printf("\nError! --- %ld",__LINE__); exit(0); }
		if(np->attr==PV) np->p=gp->P0;
	}//end for
	//////////////////////////////////////

	return;
}

void areaExPower(struct nodestr *nodeData,struct areastr *areaData,long prnt)
{
	if(prnt==YES) printf("\n\nareaExPower()...");

	register long i;
	long ibs,zbs,k;

	double aux0,aux2,aux4;
	double phai,wt,sum_wt;

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
				ibs=ap->line[i].i;
				zbs=ap->line[i].j;

				if(ibs==0 && zbs==0) continue;//invalid
				/////////////////////////////////////////

				k=isTerminalNode(ibs,lineData);
				if(k==YES) 
					np=findData(ibs,nodeData,PQ);
				else 
					np=findData(zbs,nodeData,PQ);
				////////////////////////////////////////

				if(np==NULL){ printf("\n\nError! --- %ld",__LINE__); exit(0); }
				if(fabs(np->p)>SMLL){ printf("\n\nError! --- %ld",__LINE__); exit(0); }

				np->p= -ap->line[i].w;//No Change!
				np->q= -ap->line[i].r;
			}//end for

			if(prnt==YES) printf("\n  nodal loads ok"); 
		}//end if	
		//////////////////////////////////////////////

		ap=ap->next;
	}//end while
	/////////////////////////////////
	
	return;
}
		
void OPF_prtt(long prnt)//pretreatment of all data --- very important!
{
	if(prnt==YES) printf("\n\nOPF_prtt()...");

	//0.1 load dispatching

	loadDisp(nodeData,loadData,sys_wload,YES);//prnt
	if(prnt==YES) printf("\n loadDisp() ok");
	////////////////////////////////////////////

	//0.2 inter-area exchange powers

	areaExPower(nodeData,areaData,YES);//prnt
	if(prnt==YES) printf("\n areaExPower() ok");
	//////////////////////////////////////////////////

	//0.3 nonbidding units and plants

	unitSched(genrData2,nodeData,YES);
	plantSched(plantData,nodeData,YES);

	if(prnt==YES) printf("\n nonbidding unit/plant ok");
	/////////////////////////////////////////////////////

	//0.4 bidding units

	unitSched(genrData,nodeData,YES);
	if(prnt==YES) printf("\n bidding unitSched() ok");
	///////////////////////////////////////////////////////////////

   	//1: combine associated data

    combinLineData(lineData,subLineData);
    combinChainNodes(xfmrData);

    combinChainNodes(nodeData);
	combinChainNodes(capaData);

    if(prnt==YES) printf("\n combinData() ok");
    //////////////////////////////////////////////

    //2: filtering redundant data

	//2.1 low-level data 

	filterData(capaData);
	filterData(groupData);
	filterData(nodeData);
	filterData(genrData);

	if(prnt==YES) printf("\n low-filterData() ok");

	//2.2 high-level data

	filterData(assoData);
	filterData(priceData);
	
	if(prnt==YES) printf("\n high-filterData() ok");
	//////////////////////////////////////////////////

    //3: statistics of global numbers

    lineNum=chainLen(lineData);
    nodeNumStat(nodeNum,pvNodeNum,slckNum);

    if(prnt==YES)//report
    {
            printf("\n lineNum   = %ld",lineNum);
            printf("\n nodeNum   = %ld",nodeNum);
			printf("\n pvNodeNum = %ld",pvNodeNum);
            printf("\n slckNum   = %ld",slckNum);
    }//end if
    //////////////////////////////////////////

    areaNum =chainLen(areaData);
	capaNum =chainLen(capaData);
    genrNum =chainLen(genrData);
	groupNum=chainLen(groupData);
    voltNum =chainLen(voltData);
    xfmrNum =chainLen(xfmrData);

    if(prnt==YES)//report
    {
        printf("\n areaNum   = %ld",areaNum);
	    printf("\n capaNum   = %ld",capaNum);
		printf("\n genrNum   = %ld",genrNum);
		printf("\n groupNum  = %ld",groupNum);
		//printf("\n voltNum = %ld",voltNum);
		//printf("\n xfmrNum = %ld",xfmrNum);
    }//end if
    //////////////////////////////////////////////////

    //4: slack auto.selection

    if(slckNum==0)
    {
            autoSlackSet(nodeData,genrData,YES);//prnt
            slckNum=1;
    }//end if

    if(slckNum!=1)//slckNum should =1
    {
		printf("\n\nError! --- %ld",__LINE__);
		printf("\n  Wrong slckNum = %ld",slckNum);
		
		sprintf(buffer,"Wrong slckNum = %ld",slckNum);
		consoleMsg("Error!",buffer);

		exit(0);
    }//end if
    //////////////////////////////////////////

    return;
}

double wloadStat(struct nodestr *nodeData)
{
	double wload=0.0;//init.
	struct nodestr *np=nodeData;

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

	while(np!=NULL)
	{
		if(np->attr==PV && np->p>0.0) wgen+= np->p;
		if(np->attr==PQ && np->p<0.0) wgen-= np->p;

		np=np->next;
	}//end while
	///////////////////////////////

        return wgen;
}

void baselineAlt(struct groupstr *groupData,double orig_multpl,double targ_multpl)
{
	register long i;
	struct groupstr *grp;

	grp=groupData;
	while(grp!=NULL)
	{
		for(i=1;i<=grp->modeNum;i++)
		{
			grp->mode[i].lmt /= orig_multpl;
			grp->mode[i].val1/= orig_multpl;
			grp->mode[i].val2/= orig_multpl;
			grp->mode[i].val3/= orig_multpl;

			grp->mode[i].lmt *= targ_multpl;
			grp->mode[i].val1*= targ_multpl;
			grp->mode[i].val2*= targ_multpl;
			grp->mode[i].val3*= targ_multpl;
		}//end for

		grp=grp->next;
	}//end while

	return;
}

void baselineAlt(struct plantstr *plantData,double orig_multpl,double targ_multpl)
{
	struct plantstr *pp=plantData;
	while(pp!=NULL)
	{
		pp->w/= orig_multpl;
		pp->w*= targ_multpl;

		pp=pp->next;
	}//end while
	//////////////////////

	return;
}

void baselineAlt(struct pricstr *priceData,double orig_multpl,double targ_multpl,long swch)
{
        struct pricstr *pp=priceData;
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
                cp->Q0/=	orig_multpl;
                cp->Qmin/=	orig_multpl;
                cp->Qmax/=	orig_multpl;
				cp->eps/=	orig_multpl;
				cp->maxstep/=orig_multpl;

                cp->Q0*=	targ_multpl;
                cp->Qmin*=	targ_multpl;
                cp->Qmax*=	targ_multpl;
				cp->eps*=	targ_multpl;
				cp->maxstep*=targ_multpl;

                cp=cp->next;
        }//end while

        return;
}

void baselineAlt(struct genrstr *genrData,double orig_multpl,double targ_multpl)
{
        struct genrstr *gp=genrData;
        while(gp!=NULL)
        {
                gp->P0  /= orig_multpl;
                gp->Pmax/= orig_multpl;
                gp->Pmin/= orig_multpl;
		gp->ramp/= orig_multpl;
		gp->fall/= orig_multpl;
	
                gp->P0  *= targ_multpl;
                gp->Pmax*= targ_multpl;
                gp->Pmin*= targ_multpl;
		gp->ramp*= targ_multpl;
		gp->fall*= targ_multpl;

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
                        np->q/= orig_multpl;

                        np->p*= targ_multpl;
                        np->q*= targ_multpl;
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
                if(swch==LN_RX)
                {
                        lp->r/= orig_multpl;
                        lp->x/= orig_multpl;

                        lp->r*= targ_multpl;
                        lp->x*= targ_multpl;
                }//end if
                /////////////////////////

                if(swch==LN_B0)
                {
                        if(lp->i>0 && lp->j>0)//power line
                        {
                                lp->kb/= orig_multpl;
                                lp->kb*= targ_multpl;
                        }
                }//end if
                //////////////////////

                if(swch==LN_LMT)
                {
                        lp->lmt/= orig_multpl;
                        lp->lmt*= targ_multpl;
                }//end if
                ////////////////

                lp=lp->next;
        }//end while
        /////////////////////

        return;
}

template<class TYPE>
TYPE* findData(long ibs,long zbs,TYPE *dataChain)
{
 	long i,j;
	TYPE *dp;
	////////////////////////////////	
	
	ibs=abs(ibs);
	zbs=abs(zbs);

	dp=dataChain;
    while(dp!=NULL)
    {
        i=abs(dp->i);
		j=abs(dp->j);

	    if(ibs==i && zbs==j) break;
		if(ibs==j && zbs==i) break;

		dp=dp->next;
    }//end while

	return dp;
}

template<class TYPE>
TYPE* findData(long ibs,TYPE* dataChain)
{
        TYPE *dp=dataChain;
        while(dp!=NULL)
        {
                if(dp->i==ibs) break;
                dp=dp->next;
        }//end while

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
        /////////////////////////////////

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

template<class TYPE>
TYPE* findData(char id[],TYPE* dataChain)
{
        TYPE *dp=dataChain;
        while(dp!=NULL)
        {
                if(!strcmp(id,dp->id)) break;
                dp=dp->next;
        }//end while

        return dp;
}

long brMatch(struct lnstr ln,long i,long j)
{
	long i1,i2,j1,j2,ret=NO;//init.

	i1=abs(ln.i);
        j1=abs(ln.j);

        i2=abs(i);
        j2=abs(j);

        if(i1==i2 && j1==j2) ret=YES;
        if(i1==j2 && j1==i2) ret=YES;

	return ret;
}

template<class TYPE>
long brMatch(TYPE *bp,long i,long j)
{
        long i1,i2,j1,j2,ret=NO;//init.

        i1=abs(bp->i);
        j1=abs(bp->j);

        i2=abs(i);
        j2=abs(j);

        if(i1==i2 && j1==j2) ret=YES;
        if(i1==j2 && j1==i2) ret=YES;

        return ret;
}

long match(struct linestr *np1,struct linestr *np2)
{ return brMatch(np1,np2->i,np2->j); }

long match(struct xfmrstr *np1,struct xfmrstr *np2)
{ return brMatch(np1,np2->i,np2->j); }

long match(struct nodestr *np1,struct nodestr *np2)
{
        if(abs(np1->i)==abs(np2->i)) return YES;
        else return NO;
}

long match(struct capastr *np1,struct capastr *np2)
{
        if(abs(np1->i)==abs(np2->i)) return YES;
        else return NO;
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
	long attr=np->attr;//attr of local node
	
	if(attr==PQ)
	{
			if(np2->attr==PQ)
            {
				np->p+= np2->p;
				np->q+= np2->q;
            }
        
	        if(np2->attr==PV)
			{
				strcpy(np->id,np2->id);
                np->attr=PV;            //attr: PQ --> PV

                np->p=np2->p-np->p;     //active genr.
                np->q=np2->q;           //voltage
			}
                
			if(np2->attr==SLCK)
            {
                    strcpy(np->id,np2->id);
                    np->attr=SLCK;  //attr: PQ --> SLCK

                    np->p=np2->p;   //voltage
                    np->q=np2->q;   //theta
            }
	}//end if
	////////////////////////////////////////////

	if(attr==PV)
	{
	        
		if(np2->attr==PQ) np->p-= np2->p;
		if(np2->attr==PV) np->p+= np2->p;
		
		if(np2->attr==SLCK)
                {
                        strcpy(np->id,np2->id);
                        np->attr=SLCK;  //attr: PV --> SLCK

                        np->p=np2->p;   //voltage
                        np->q=np2->q;   //theta
                }//end if
        
	}//end if 
	//////////////////////////////////////

	return;
}

void combinData(struct linestr *lp,struct linestr *lp2)
{
        double aux2,aux4,aux6,aux8;

        complexMult(lp->r,lp->x,lp2->r,lp2->x,aux2,aux4);
        complexDivi(aux2,aux4,lp->r+lp2->r,lp->x+lp2->x,aux6,aux8);

        lp->r=aux6;
        lp->x=aux8;

        if(lp->i>0 && lp->j>0)
                lp->kb+= lp2->kb;//power lines

        lp->lmt+= lp2->lmt;
        ////////////////////////////////

        return;
}

void combinData(struct capastr *cp,struct capastr *cp2)
{
        cp->Q0+= cp2->Q0;

        cp->Qmin+= cp2->Qmin;//No Change!
        cp->Qmax+= cp2->Qmax;

		cp->eps=min(cp->eps,cp2->eps);
		cp->maxstep=min(cp->maxstep,cp2->maxstep);

        return;
}

void formNodeIsland(long *list,long &n,long k_bs,struct linestr *lineData,long line_attr)
{
	long ibs,zbs,marg;
	struct linestr *lp;
	/////////////////////////////////

	list[1]=k_bs;
	n=1;

	marg=k_bs;//init.
        while(marg!=NO)
        {
		lp=lineData;
                while(lp!=NULL)
                {
                        if(line_attr==LINE_XF && lp->i>0 && lp->j>0){ lp=lp->next; continue; }

                        ibs=abs(lp->i);
                        zbs=abs(lp->j);

                        if(ibs==marg && lookup(list,zbs,n)==NO){ n++; list[n]=zbs; }
                        if(zbs==marg && lookup(list,ibs,n)==NO){ n++; list[n]=ibs; }

                        lp=lp->next;
                }//end while

                marg=findMarginalNode(lineData,list,n,line_attr);
        }//end while
        ///////////////////////////////////////

	return;
}

void filterData(struct assostr *&assoData)
{
	long prnt=YES;

	if(prnt==YES) printf("\n\nfilterData()_assoData...");

	register long i,k;
	
	long ibs,unitNum;
	long *tempList,n;//n --- sizeof tempList[]

	struct assostr *asp,*aspNext;
	struct genrstr *gp;
	/////////////////////////////////////////////////////

	tempList=(long *)calloc(MAX_UNIT+1,sizeof(long));
	if(tempList==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }

	//1: Remove non-bidding units

	if(prnt==YES) printf("\n Removing non-bidding units...");
	
	asp=assoData;
	while(asp!=NULL)
	{
		unitNum=asp->unitNum;//init.
		if(unitNum<2){ asp=asp->next; continue; }
	
		for(i=1;i<=unitNum;i++)
		{
			gp=findData(asp->unit[i].i,genrData);
			if(gp==NULL)
			{
				if(i==unitNum){ unitNum--; break; }

				for(k=i;k<=unitNum-1;k++)
				{
					strcpy(asp->unit[k].id,asp->unit[k+1].id);
					asp->unit[k].i=asp->unit[k+1].i;
				}//end for

				unitNum--;	//size reduced
				i--;		//hesitate
			}//end if
		}//end for

		asp->unitNum=unitNum;	
		asp=asp->next;
	}//end while
	
	if(prnt==YES) printf("ok");
	/////////////////////////////////////////////////

	//2: Remove non-neighboring units
	
	if(prnt==YES) printf("\n Removing non-neighboring units...");

	asp=assoData;
	while(asp!=NULL)
        {
		unitNum=asp->unitNum;
		if(unitNum<2){ asp=asp->next; continue; }

		ibs=asp->unit[1].i;//ibs of the first unit
		formNodeIsland(tempList,n,ibs,lineData,LINE_XF);

		for(i=2;i<=unitNum;i++)
		{
			k=lookup(tempList,asp->unit[i].i,n);
			if(k!=NO) continue;//expected

			for(k=i;k<=n-1;k++)
			{
				strcpy(asp->unit[k].id,asp->unit[k+1].id);
				asp->unit[k].i=asp->unit[k+1].i;
			}//end for

			unitNum--;	//size reduced
			i--;		//hesitate
		}//end for

		asp->unitNum=unitNum;
                asp=asp->next;
        }//end while
	
	if(prnt==YES) printf("ok");
	///////////////////////////////////////////////////////

	//3: Remove invalid asso.data

	if(prnt==YES) printf("\n Removing invalid asso.data...");

	asp=assoData;
	while(asp!=NULL)
	{
		if(asp->unitNum<2)
		{
			aspNext=asp->next;
			delChainNode(assoData,asp);
			
			asp=aspNext;
			continue;
		}//end if

		asp=asp->next;
	}//end while

	if(prnt==YES) printf("ok");
	////////////////////////////////////////////////

	free(tempList);
	return;
}

void filterData(struct groupstr *&groupData)
{
	register long i,j,k;
	long ibs,zbs,n;

	struct groupstr *grp;
	///////////////////////////////////

	grp=groupData;
	while(grp!=NULL)
	{
		n=grp->lineNum;//init.
		for(i=1;i<=n-1;i++)
		{
			ibs=grp->line[i].i;
			zbs=grp->line[i].j;

			for(j=i+1;j<=n;j++)
			{
				if(brMatch(grp->line[j],ibs,zbs)!=YES) continue;

				for(k=j;k<=n-1;k++)
				{
					strcpy(grp->line[k].id,grp->line[k+1].id);	
					grp->line[k].i=grp->line[k+1].i;
					grp->line[k].j=grp->line[k+1].j;
				}//end for

				n--;
				j--;
			}//end for
		}//end for

		grp->lineNum=n;
		grp=grp->next;
	}//end while
	///////////////////////////////////

	return;
}

void filterData(struct genrstr *&genrData)
{
        struct nodestr *np;
        struct genrstr *gp,*next;
        ///////////////////////////////

        gp=genrData;
        while(gp!=NULL)
        {
                np=findData(gp->i,nodeData);//by ibs
				//np=findData(gp->i,nodeData);//by id

                if(np==NULL || np->attr==PQ || np->p<SMLL)
                {
                        next=gp->next;
                        delChainNode(genrData,gp);

                        gp=next;
                }
                else
                        gp=gp->next;
        }//end while
        ///////////////////////////

        return;
}
         
void filterData(struct capastr *&capaData)
{
	long delFlg;
	double aux2,aux4;

	struct capastr *cp,*cpNext;
	////////////////////////////////////

	cp=capaData;
	while(cp!=NULL)
	{
		delFlg=NO;//init.

		aux2=cp->Qmin;
		aux4=cp->Qmax;
 
		if(cp->Qmin>cp->Qmax) delFlg=YES;//marked
		if(fabs(aux2)<SMLL && fabs(aux4)<SMLL) delFlg=YES;

		if(delFlg==YES)
		{
			cpNext=cp->next;
			delChainNode(capaData,cp);

			cp=cpNext;
			continue;
		}//end if

		cp=cp->next;
	}//end while
	/////////////////////////////////////

	return;
}

void filterData(struct nodestr *&nodeData)
{
        long flag;
        struct nodestr *np,*npNext;
        //////////////////////////////////

        np=nodeData;
        while(np!=NULL)
        {
                flag=NO;//init.
                switch(np->attr)
                {
                case PQ:
                        if(fabs(np->p)<SMLL && fabs(np->q)<SMLL) flag=YES;
                        break;

                case PV:
                        if(fabs(np->p)<SMLL || fabs(np->q)<SMLL) flag=YES;
                        break;

                case SLCK:
                        if(fabs(np->p)<SMLL) flag=YES;
                        break;
		
		default:
			flag=YES;//invalid nodal attr.
		}//end switch
                //////////////////////////////////////////////

                if(flag==YES)
                {
			npNext=np->next;//the next node reserved
                        delChainNode(nodeData,np);
			
                        np=npNext;
                }
                else
                        np=np->next;
        }//end while
        //////////////////////////////////

        return;
}

void nodeNumStat(long &nodeNum,long &pvNodeNum,long &slckNum)
{
        register long n;
        long *tempList;

        struct linestr *lp;
        struct nodestr *np;

        tempList=(long *)calloc(MAX_NODE+1,sizeof(long));
        if(tempList==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }
        //////////////////////////////////////////////////////////////

        //1: nodeNum stat.

        n=0;//init.

        lp=lineData;
        while(lp!=NULL)
        {
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
        ////////////////////////////////////

        //2: pvNodeNum and slckNum stat.

        pvNodeNum=0;//init.
        slckNum=0;

        np=nodeData;
        while(np!=NULL)
        {
                if(np->attr==PV) pvNodeNum++;
                if(np->attr==SLCK) slckNum++;

                np=np->next;
        }//end while
        ///////////////////////////

        free(tempList);
        return;
}

void autoSlackSet(struct nodestr *nodeData,struct genrstr *genrData,long prnt)
{
	if(prnt==YES) printf("\n\nautoSlackSet()...");

	register long i;

	long ibs,iminSpace,*coslack;
	double aux0,aux2,minSpace,*space;

	struct genrstr *gp;
        struct nodestr *np;
        ////////////////////////////////////////////////////////////

	if(coslack_num<1) coslack_num=1;//No Change!

	coslack=(long *)calloc(coslack_num+1,sizeof(long));
	space=(double *)calloc(coslack_num+1,sizeof(double));

	if(coslack==NULL || space==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }
	//////////////////////////////////////////////////////////////

	//1: power balance analysis

        double wload,wgen,wslack;

        wload=wloadStat(nodeData);
        wgen=wgenStat(nodeData);
        wslack=wload/(1.0-loss_ratio)-wgen;

        if(prnt==YES) printf("\n wload =%9.4lf",wload);
	if(prnt==YES) printf("\n wgen  =%9.4lf",wgen);
        if(prnt==YES) printf("\n wslack=%9.4lf, when loss_ratio=%4.2lf%%",wslack,100.0*loss_ratio);
	////////////////////////////////////////////////////////////////

	//2: form space[] and coslack[]
       
	initVector(space,coslack_num,0.0);
	initVector(coslack,coslack_num,(long)0);

	iminSpace=1;//init.
	minSpace=.0;

	gp=genrData;
        while(gp!=NULL)
        {
		if(wslack>0.0) 
			aux2=gp->Pmax-gp->P0;
		else 
			aux2=gp->P0-gp->Pmin;
	
                if(aux2>minSpace)
		{
		        coslack[iminSpace]=gp->i;
			space[iminSpace]=aux2;

                        iminSpace=posiMin(space,coslack_num);
                        minSpace=space[iminSpace];
                }//end if

                gp=gp->next;
        }//end while

        if(prnt==YES) printf("\n coslack[] formed");
	/////////////////////////////////////////////////////////////
        
	//3: wslack distribution among coslacks 

	aux2=sum(space,coslack_num);
	if(fabs(aux2)<SMLL){ printf("\nError! --- %ld",__LINE__); exit(0); }

	aux0=wslack/aux2;
	for(i=1;i<=coslack_num;i++)
	{
		ibs=coslack[i];//coslack ibs

		gp=findData(ibs,genrData);
		np=findData(ibs,nodeData,PVSLACK);

		if(gp==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }
		if(np==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }

		if(np->attr==PV)
		{
			np->p+= aux0*space[i];//expected

			if(np->p<gp->Pmin) np->p=gp->Pmin;
			if(np->p>gp->Pmax) np->p=gp->Pmax;

			gp->Pc=np->p-gp->P0;//coslack power
		}//end if 
	}//end for

	if(prnt==YES) printf("\n wslack distribution ok");
	///////////////////////////////////////////////////////

	//4: looking for final slack among coslacks
        
	long slack;
	double low,upp,maxSpace;

	maxSpace=-LARG;//init.
	slack=0;

	for(i=1;i<=coslack_num;i++)
	{
		gp=findData(coslack[i],genrData);
		if(gp==NULL){ printf("\n\nError! --- %ld",__LINE__); exit(0); }

		if(gp->Pmax<SLACK_PMAX) continue;
		//////////////////////////////////////
	
		low=gp->P0-gp->Pmin;
		upp=gp->Pmax-gp->P0;
		
		aux2=low*upp;
		if(aux2>maxSpace)
		{
			maxSpace=aux2;
			slack=gp->i;
		}//end if
	}//end for
	//////////////////////////////////////

	if(slack==0)//Not found
	{ 
		printf("\n\nError! --- %ld",__LINE__); 
		printf("\n  Final slack NOT found");

		consoleMsg("错误","没有找到合适的平衡机");
		exit(0); 
	}//end if
	/////////////////////////////////////////////////

	//5: marking the slack

	np=nodeData;
	while(np!=NULL)
	{
                if(np->i==slack)
                {
                        np->attr=SLCK;
                        np->p=np->q;
                        np->q=0.0;//in degree

			strcpy(idbuf,np->id);
                        break;
                }//end if

		np=np->next;
        }//end while
       
	if(prnt==YES) printf("\n Slack Appointed: id=%s, ibs=%ld",idbuf,slack); 
	//////////////////////////////////////////////////

	free(coslack);
	free(space);

        return;
}



//end of file
