//oracle.cpp by Wang GT

#include"common.h"

#include"opf.h"
#include"opf_global.h"

#include"oracle.h"
///////////////////////////////////////////////////////

void OPF_write_oracle(char *date_str,long sd,long prnt)
{
	if(prnt==YES) printf("\n\nOPF_write_oracle()...");

        //1: year, month, day, and sd --- globals

        year = date_year(date_str,"mm/dd/yyyy");
        month= date_month(date_str,"mm/dd/yyyy");
        day  = date_day(date_str,"mm/dd/yyyy");

        if(prnt==YES) printf("\n year = %ld",year);
        if(prnt==YES) printf("\n month= %ld",month);
        if(prnt==YES) printf("\n day  = %ld",day);
        if(prnt==YES) printf("\n sd   = %ld",sd);
        ////////////////////////////////////////////////////////

	//2: connecting ora.server

        if(getenv("ORACLE_SID")==NULL)
        {
                printf("\n\nError! --- %ld",__LINE__);
                printf("\n  Env. variable of ORACLE_SID not defined.\n");

                exit(0);
        }//end if

        strcpy(oraServer,getenv("ORACLE_SID"));
        conn_oraServer(oraServer,NO);//prnt

	if(prnt==YES) printf("\n conn_oraServer() ok");
	////////////////////////////////////////////////////////

	//3: corr. policy

	write_corrpolicy(YES);//prnt
	if(prnt==YES) printf("\n write_corrpolicy() ok");

	//3.2 synchoff adjustment
	
	write_synchoff(YES);//prnt
	if(prnt==YES) printf("\n write_synchoff() ok");
	////////////////////////////////////////////////////////

	//4: sensitivity data

	write_sns(YES);//prnt
	if(prnt==YES) printf("\n write_sns() ok");

	//5: flow violation monitor
	
	write_vioflow(YES);//prnt
	if(prnt==YES) printf("\n write_vioflow() ok");
	////////////////////////////////////////////////////////

	//6: itr. process

        write_itr(YES);//prnt
        if(prnt==YES) printf("\n write_itr() ok");

	//7: disconnect ora.server

	disconn_oraServer();
	if(prnt==YES) printf("\n disconn_oraServer() ok");
	////////////////////////////////////////////////////////

	return;
}

void write_synchoff(long prnt)
{
	if(prnt==YES) printf("\n\nwrite_synchoff()...");
	if(prnt==YES) printf("\n  Involved tables: SYNCHOFFADJ");

	register long i;
	
	long synFlg0=YES;//No Change!
	long synFlg1;

	struct genrstr *gp;
	//////////////////////////////////////////////////////////////

	//1: clearing the table
	
        sprintf(cmdbuf,"delete trade%ld.T_SYNCHOFFADJ",year);

        ret=ociTools.ORA_BindNoReturn(cmdbuf);
        if(ret==OCI_FAIL){ printf("\nError! --- %ld",__LINE__); exit(0); }

	//2: inserting records if necessary

	gp=genrData;//init.
	for(i=1;i<=genrNum;i++,gp=gp->next)
	{
		if(gp->synFlg!=YES && gp->offFlg!=YES) continue;

		if(gp->synFlg==YES) synFlg1=YES;
		if(gp->offFlg==YES) synFlg1=NO;

        	sprintf(cmdbuf,"insert into trade%ld.T_SYNCHOFFADJ (UNIT_ID,UNIT_NAME,IBS,SYN_FLG0,SYN_FLG1,USER_ID,USER_NAME) VALUES('%s','%s',%ld,%ld,%ld,'%s','%s')",year,gp->id,gp->descr,gp->i,synFlg0,synFlg1,user_id,user_descr);

        	ret=ociTools.ORA_BindNoReturn(cmdbuf);
        	if(ret==OCI_FAIL){ printf("\nError! --- %ld",__LINE__); exit(0); }
	}//end for
	////////////////////////////////////////////////////////////

	return;
}

void write_vioflow(long prnt)
{
	if(prnt==YES) printf("\n\nwrite_vioflow()...");
	if(prnt==YES) printf("\n Involved tables: TIELINEVIO");

	register long i;
	
	long k;
	long vioFlg0,vioFlg;
        
	double mwmax,mwmin,vioPct;
	double mw0,mvar0,ampMVA0,mw,mvar,ampMVA;
	
	struct flowstr *fp,*fp0;	
	struct linestr *lp;
	
	struct groupstr *grp;
	/////////////////////////////////////////////////////////

	//1: updating mon. records

	sprintf(cmdbuf,"select TIELINE_ID,TIELINE_NAME from trade%ld.T_TIELINEVIO where MONFLG=1",year);
 
        ret=ociTools.ORA_BindWithReturnArray(cmdbuf,(void *)strbuf);
        if(ret==OCI_FAIL){ printf("\nError! --- %ld",__LINE__); exit(0); }

	recNum=ret;
	if(prnt==YES) printf("\n recNum=%ld",recNum);
	/////////////////////////////////////////////////////////

        for(i=0;i<=recNum;i++)
        {
		//.1 validity checking

		strcpy(idbuf,strbuf[i]);
		grp=findData(idbuf,groupData);
		
		if(grp==NULL) lp=findData(idbuf,lineData);
		if(lp==NULL)
		{
			printf("\n\nWarning! --- %ld",__LINE__);
			printf("\n  Invalid mon. record: id=%s, descr=%s",idbuf,strbuf[i+recNum]);

			mwmax = 0.0;//clearing
			mwmin = 0.0;
			
			mw0    = 0.0;
			mvar0  = 0.0;
			ampMVA0= 0.0;
	
			mw     = 0.0;
                        mvar   = 0.0;
                        ampMVA = 0.0;
                
			vioPct = 0.0;
			vioFlg = 0;	
		}//end if
		//////////////////////////////////////////////

		//.2 for groupData

		if(grp!=NULL)
		{
			mwmax = grp->mwmax; 
                        mwmin = grp->mwmin;

                        mw0    = grp->w0;
                        mvar0  = grp->r0;
                        ampMVA0= grp->s0; 

                        mw     = grp->w;
                        mvar   = grp->r;
                        ampMVA = grp->s;

			vioPct = grp->vioPct;
                        vioFlg = grp->vioFlg;
		}//end if
		//////////////////////////////////////////////

		//.3 for lineData

		if(lp!=NULL)
		{	
			mwmax = lp->lmt;//amp/MVA
			mwmin = 0.0;

			fp0=findData(idbuf,flowData0);
			if(fp0==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }  

			fp=findData(idbuf,flowData);
			if(fp==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }	

			mw0    = fp0->w1;
			mvar0  = fp0->r1;
			ampMVA0= fp0->s1;

			mw     = fp->w1;
			mvar   = fp->r1;
			ampMVA = fp->s1;

			vioPct = fp->vioPct;
			vioFlg = fp->vioFlg;
		}//end if
		/////////////////////////////////////////////

		//.4 updating the record

		strcpy(buffer,strbuf[i+recNum]);

		sprintf(cmdbuf,"update trade%ld.T_TIELINEVIO (TIELINE_NAME,MWMAX,MWMIN,MW0,MVAR0,AMPMVA0,MW,MVAR,AMPMVA,VIOPCT,VIOFLG) values ('%s',%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%ld) where TIELINE_ID=%s",year,buffer,mwmax,mwmin,mw0,mvar0,ampMVA0,mw,mvar,ampMVA,vioPct,vioFlg);

		ret=ociTools.ORA_BindNoReturn(cmdbuf);
		if(ret==OCI_FAIL){ printf("\nError! --- %ld",__LINE__); exit(0); }
	}//end for
        
	if(prnt==YES) printf("\n mon.records updated");
	//////////////////////////////////////////////////////////

	//2: deleting non-mon. records

	sprintf(cmdbuf,"delete trade%ld.T_TIELINEVIO where MONFLG=0",year);

	ret=ociTools.ORA_BindNoReturn(cmdbuf);
        if(ret==OCI_FAIL){ printf("\nError! --- %ld",__LINE__); exit(0); }

	if(prnt==YES) printf("\n non-mon. records deleted");
       	///////////////////////////////////////////////////////////
 
	//3: inserting vio. records

	k=0;//init.
	grp=groupData;

	for(i=1;i<=groupNum;i++,grp=grp->next)
	{
		vioFlg =grp->vioFlg;
		vioFlg0=NO;//init.
		
		if(grp->w0>grp->mwmax) vioFlg0=YES;
		if(grp->w0<grp->mwmin) vioFlg0=YES;

		if(vioFlg0!=YES && vioFlg!=YES) continue;

		sprintf(cmdbuf,"insert into trade%ld.T_TIELINEVIO (TIELINE_ID,TIELINE_NAME,MWMAX,MWMIN,MW0,MVAR0,AMPMVA0,MW,MVAR,AMPMVA,VIOPCT,VIOFLG) values ('%s','%s',%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%ld)",year,grp->id,grp->descr,grp->mwmax,grp->mwmin,grp->w0,grp->r0,grp->s0,grp->w,grp->r,grp->s,grp->vioPct,grp->vioFlg);

		ret=ociTools.ORA_BindNoReturn(cmdbuf);
                if(ret==OCI_FAIL){ printf("\nError! --- %ld",__LINE__); exit(0); }	

		k++;
	}//end for

	if(prnt==YES) printf("\n %ld vio.group inserted",k);
	////////////////////////////////////////////////////////////

	//3.2 for vio. branches 

	k=0;//init.
	
	fp0=flowData0;
	fp =flowData;
	lp =lineData; 
	
	for(i=1;i<=lineNum;i++,fp0=fp0->next,fp=fp->next,lp=lp->next)
	{
		if(brMatch(fp0,lp->i,lp->j)!=YES)
		{
			printf("\n\nError! --- %ld",__LINE__);
			printf("\n  Unmatched flowData0: id=%s, ibs=%ld, zbs=%ld",fp0->id,fp0->i,fp0->j);
			printf("\n            lineData : id=%s, ibs=%ld, zbs=%ld",lp->id,lp->i,lp->j);
		
			exit(0);
		}//end if

		if(brMatch(fp,lp->i,lp->j)!=YES)
                {
                        printf("\n\nError! --- %ld",__LINE__);
                        printf("\n  Unmatched flowData: id=%s, ibs=%ld, zbs=%ld",fp->id,fp->i,fp->j);
                        printf("\n            lineData: id=%s, ibs=%ld, zbs=%ld",lp->id,lp->i,lp->j);
                
                        exit(0);
                }//end if
		///////////////////////////////////////////////////

		vioFlg0=fp0->vioFlg;
		vioFlg =fp->vioFlg;

		if(vioFlg0!=YES && vioFlg!=YES) continue;

		sprintf(cmdbuf,"insert into trade%ld.T_TIELINEVIO (TIELINE_ID,TIELINE_NAME,MWMAX,MWMIN,MW0,MVAR0,AMPMVA0,MW,MVAR,AMPMVA,VIOPCT,VIOFLG) values ('%s','%s',%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%ld)",year,lp->id,lp->descr,lp->lmt,0.0,fp0->w1,fp0->r1,fp0->s1,fp->w1,fp->r1,fp->s1,fp->vioPct,fp->vioFlg);

		ret=ociTools.ORA_BindNoReturn(cmdbuf);
                if(ret==OCI_FAIL){ printf("\nError! --- %ld",__LINE__); exit(0); }

                k++;	
	}//end for

	if(prnt==YES) printf("\n %ld vio.flow inserted",k);
	////////////////////////////////////////////////////////////

	return;
}
 
void write_sns(long prnt)
{
	if(prnt==YES) printf("\n\nwrite_sns()...");
	if(prnt==YES) printf("\n Involved tables: LINESNS, GROUPSNS, LOSSSNS");

	register long i,j;
	double sns;

	struct genrstr *gp;
	struct groupstr *grp;

	struct linestr *lp;
	struct loadstr *ldp;
	/////////////////////////////////////////////////////

	//1: clearing linesns 

        sprintf(cmdbuf,"delete trade%ld.T_LINESNS",year);

        ret=ociTools.ORA_BindNoReturn(cmdbuf);
        if(ret==OCI_FAIL){ printf("\nError! --- %ld",__LINE__); exit(0); }

	if(prnt==YES) printf("\n linesns cleared");
	///////////////////////////////////////////////////////

	//1.2 clearing groupsns	

	sprintf(cmdbuf,"delete trade%ld.T_GROUPSNS",year);

	ret=ociTools.ORA_BindNoReturn(cmdbuf);        
        if(ret==OCI_FAIL){ printf("\nError! --- %ld",__LINE__); exit(0); }        
        
        if(prnt==YES) printf("\n groupsns cleared");
	//////////////////////////////////////////////////////

	//1.3 clearing losssns

	sprintf(cmdbuf,"delete trade%ld.T_LOSSSNS",year);

	ret=ociTools.ORA_BindNoReturn(cmdbuf);        
        if(ret==OCI_FAIL){ printf("\nError! --- %ld",__LINE__); exit(0); }        
        
        if(prnt==YES) printf("\n losssns cleared");
	//////////////////////////////////////////////////////////////////

	//2: writing line sns. solution

	lp=lineData;
	for(i=1;i<=lineNum;i++,lp=lp->next)
	{
		//2.1 line-genr. sns.

		gp=genrData;
		for(j=1;j<=genrNum;j++,gp=gp->next)
		{
			sprintf(cmdbuf,"insert into trade%ld.T_LINESNS (LINE_ID,LINE_NAME,UNIT_ID,UNIT_NAME,SNS) values ('%s','%s','%s','%s',%lf)",lp->id,lp->descr,gp->id,gp->descr,PBPG[i][j]);

			ret=ociTools.ORA_BindNoReturn(cmdbuf);
			if(ret==OCI_FAIL){ printf("\nError! --- %ld",__LINE__); exit(0); }
		}//end for
		//////////////////////////////////////////////////////

		//2.2 line-load sns.
	
		ldp=loadData;
		for(j=1;j<=loadNum;j++,ldp=ldp->next)
		{
			sprintf(cmdbuf,"insert into trade%ld.T_LINESNS (LINE_ID,LINE_NAME,LOAD_ID,LOAD_NAME,SNS) values ('%s','%s','%s','%s',%lf)",year,lp->id,lp->descr,ldp->id,ldp->descr,PBPD[i][j]);

			ret=ociTools.ORA_BindNoReturn(cmdbuf);
                        if(ret==OCI_FAIL){ printf("\nError! --- %ld",__LINE__); exit(0); }
                }//end for
	}//end for

	if(prnt==YES) printf("\n line sns. ok");
	//////////////////////////////////////////////////////////////////

	//3: writing group sns. 

	grp=groupData;
	for(i=1;i<groupNum;i++,grp=grp->next)
	{
		//3.1 group-genr. sns.
	
		gp=genrData;
		for(j=1;j<=genrNum;j++,gp=gp->next)
		{
			sprintf(cmdbuf,"insert into trade%ld.T_GROUPSNS (GROUP_ID,GROUP_NAME,UNIT_ID,UNIT_NAME,SNS) values ('%s','%s','%s','%s',%lf)",year,grp->id,grp->descr,gp->id,gp->descr,GRPG[i][j]);

			ret=ociTools.ORA_BindNoReturn(cmdbuf);
                        if(ret==OCI_FAIL){ printf("\nError! --- %ld",__LINE__); exit(0); }
                }//end for
                //////////////////////////////////////////////////////

		//3.2 group-load sns.

		ldp=loadData;
                for(j=1;j<=loadNum;j++,ldp=ldp->next)
                {
                        sprintf(cmdbuf,"insert into trade%ld.T_GROUPSNS (GROUP_ID,GROUP_NAME,LOAD_ID,LOAD_NAME,SNS) values ('%s','%s','%s','%s',%lf)",year,grp->id,grp->descr,ldp->id,ldp->descr,GRPD[i][j]);

			ret=ociTools.ORA_BindNoReturn(cmdbuf);
                        if(ret==OCI_FAIL){ printf("\nError! --- %ld",__LINE__); exit(0); }
                }//end for
	}//end for

	if(prnt==YES) printf("\n group sns. ok");
	//////////////////////////////////////////////////////////////////

	//4: writing loss sns. to generators

	gp=genrData;
	for(i=1;i<=genrNum;i++,gp=gp->next)
	{
		sprintf(cmdbuf,"insert into trade%ld.T_LOSSSNS (UNIT_ID,UNIT_NAME,SNS) values ('%s','%s',%lf)",year,gp->id,gp->descr,PLPG[i]);

		ret=ociTools.ORA_BindNoReturn(cmdbuf);
		if(ret==OCI_FAIL){ printf("\nError! --- %ld",__LINE__); exit(0); }
	}//end for

	if(prnt==YES) printf("\n loss sns. ok");
	/////////////////////////////////////////////////////////////////

	return;
}

void write_corrpolicy(long prnt)
{
	if(prnt==YES) printf("\n\nwrite_corrpolicy()...");
	if(prnt==YES) printf("\n Involved tables: CORRPOLICY");

	register long i;
	struct genrstr *gp;
	/////////////////////////////////////////////////////////////

	//1: clearing the table

	sprintf(cmdbuf,"delete trade%ld.T_CORRPOLICY",year);

        ret=ociTools.ORA_BindNoReturn(cmdbuf);
        if(ret==OCI_FAIL){ printf("\nError! --- %ld",__LINE__); exit(0); }

	if(prnt==YES) printf("\n table cleared");
	/////////////////////////////////////////////////////////////////

	//2: insering new records

	gp=genrData;
	for(i=1;i<=genrNum;i++,gp=gp->next)
	{
		sprintf(cmdbuf,"insert into trade%ld.T_CORRPOLICY (UNIT_ID,UNIT_NAME,MW0,MW,INC,EXPLAIN) values ('%s','%s',%lf,%lf,%lf,'scd')",year,gp->id,gp->descr,gp->w0,gp->w,gp->w-gp->w0);

		ret=ociTools.ORA_BindNoReturn(cmdbuf);
                if(ret==OCI_FAIL){ printf("\nError! --- %ld",__LINE__); exit(0); }
        }//end for

	if(prnt==YES) printf("\n %ld records inserted",genrNum);
	////////////////////////////////////////////////////////////////

	return;
}  

void write_itr(long prnt)
{
	if(prnt==YES) printf("\n\nwrite_itr()...");
	if(prnt==YES) printf("\n Involved tables: SCDITR");

	register long i;
	long n;

	struct itrstr *ip;
	/////////////////////////////////////////////////////////////////

	//1: clearing the table

	sprintf(cmdbuf,"delete trade%ld.T_SCDITR",year);

	ret=ociTools.ORA_BindNoReturn(cmdbuf);
        if(ret==OCI_FAIL){ printf("\nError! --- %ld",__LINE__); exit(0); }

        if(prnt==YES) printf("\n table cleared");
        /////////////////////////////////////////////////////////////////

	//2: inserting records

	n =chainLen(itrData);
	ip=itrData;

	for(i=1;i<=n;i++,ip=ip->next)
	{
		sprintf(idbuf,"scd%02ld",i);
		sprintf(cmdbuf,"insert into trade%ld.T_SCDITR (ITR_ID,ITR_NUM,VIONUM,VIOPCT,PURCOST) values ('%s',%ld,%ld,%lf,%lf)",year,idbuf,ip->i,ip->vioNum,ip->vioPct,ip->purcost);

		ret=ociTools.ORA_BindNoReturn(cmdbuf);
                if(ret==OCI_FAIL){ printf("\nError! --- %ld",__LINE__); exit(0); }
        }//end for
	
	if(prnt==YES) printf("\n %ld records inserted",n);
	/////////////////////////////////////////////////////////////////

	return;
}

void read_loadfor(long prnt)
{
	if(prnt==YES) printf("\n\nread_loadfor()...");
	if(prnt==YES) printf("\n Involved tables: DLOADFOR_%ld_%ld",month,day);

	double wgen,wload;
	//////////////////////////////////////////////////

	//1: reading wgen and wload

	sprintf(cmdbuf,"select VALUE from trade%d.T_DLOADFOR_%d_%d where PERIOD=%d and YES_NO=1",year,month,day,sd);
	ret=ociTools.ORA_BindWithReturnArray(cmdbuf,(void *)numbuf);//No Change!
        if(ret==OCI_FAIL){ printf("\nError! --- %ld",__LINE__); exit(0); }

	wgen=atof(numbuf[0]);
	wload=wgen*(1-loss_ratio);

	if(prnt==YES) printf("\n wload=%.2lf",wload);
	///////////////////////////////////////////////////

	//2: load distribution

	load_distr(nodeData,wload);
	if(prnt==YES) printf("\n load_distr() ok");
	/////////////////////////////////////////////////

	return;
}

void load_distr(struct nodestr *nodeData,double wload)
{
	register long i;
	
	long n=chainLen(nodeData);
	double wt,sum_wt;

	struct nodestr *np;
	//////////////////////////////////////////////

	//1: cal. sum_wt;

	sum_wt=0.0;//init.
	np=nodeData;

	for(i=1;i<=n;i++,np=np->next)
		if(np->attr==PQ) sum_wt+= np->p;
	///////////////////////////////////////////////

	//2: load distribution

	np=nodeData;
	for(i=1;i<=n;i++,np=np->next)
	{
		if(np->attr==PQ)
		{
			wt=np->p/sum_wt;
			np->p=wt*wload;
		}
	}//end for
	/////////////////////////////////////////////

	return;
}

long saveSched(struct genrstr *genrData,char *id,double sched,long prnt)
{
//Notes: by Wang GT, 10/13/2003
//  genrData: chain of generators --- from rtdb
//        id: id of unit or plant --- from oracle
//     sched: sched. related to the id 
//      prnt: YES or NO 
///////////////////////////////////////////////////////

	if(prnt==YES) printf("\n\nsaveSched()...");

	struct genrstr *gp;
        struct genrstr *sub_genrData=NULL;//init.
        //////////////////////////////////////////////////

	//1: for a valid unit
	
	gp=findData(id,genrData);
	if(prnt==YES) printf("\n findData() ok");

	if(gp!=NULL)
	{ 
		gp->w=sched;//sched. saved
		if(prnt==YES) printf("\n sched saved");

		return YES;
	}//end if
	///////////////////////////////////////////////////

	//2: for a valid plant

	makeSubChain(sub_genrData,genrData,id);//id of st
	if(prnt==YES) printf("\n makeSubChain() ok");

	if(sub_genrData!=NULL)
	{
		//.1 load distribution

	 	load_distr(sub_genrData,sched);
		if(prnt==YES) printf("\n load_distr() ok");

		//.2 recover subchain 

		recovSubChain(genrData,sub_genrData);
		if(prnt==YES) printf("\n recovSubChain() ok");

		//.3 free subchain
	
		freeChain(sub_genrData);
		if(prnt==YES) printf("\n freeChain() ok");

		return YES;
	}//end if
	//////////////////////////////////////////////////

	return NO;//sched. not saved
}

void recovSubChain(struct genrstr *genrData,struct genrstr *sub_genrData)
{
	register long i;        
	
	long n=chainLen(sub_genrData);

	struct genrstr *gp,*next;
	struct genrstr *subp;
	///////////////////////////////////////////

	subp=sub_genrData;
	for(i=1;i<=n;i++,subp=subp->next)
	{
		gp=findData(subp->id,genrData);
		if(gp==NULL){ printf("\n\nError! --- %ld",__LINE__); exit(0); }

		next=gp->next;//reserved
		
		memcpy(gp,subp,sizeof(struct genrstr));
		gp->next=next;
	}//end for
	////////////////////////////////////////////
	
	return;
}

void load_distr(struct genrstr *_genrData,double sched)
{
	register long i;

	long n=chainLen(_genrData);
	double wt,sum_wt;

	struct genrstr *gp;
	//////////////////////////////////////

	//1: calculating sum_wt
	
	sum_wt=0.0;//init.
	gp=_genrData;
	
	for(i=1;i<=n;i++,gp=gp->next) 
		sum_wt+= gp->wmax;
	
	if(sum_wt<SMLL)
	{
		printf("\n\nError! --- %ld",__LINE__);
		printf("\n  Invalid sum_wt = %.2lf",sum_wt);

		exit(0);
	}//end if
	/////////////////////////////////////////

	//2: load distribution

	gp=_genrData;
	for(i=1;i<=n;i++,gp=gp->next) 
	{
		wt=gp->wmax/sum_wt;
		gp->w=wt*sched;
	}//end for
	///////////////////////////////////////////

	return;
}

void makeSubChain(struct genrstr *&sub_genrData,struct genrstr *genrData,char *st)
{
	if(sub_genrData!=NULL)//No Change!
        {
                printf("\n\nError! --- %ld",__LINE__);
                printf("\n  Uninitialized variable found: sub_genrData");                       

                exit(0);
        }//end if
	//////////////////////////////////////////////

	register long i;

	long n=chainLen(genrData);
	struct genrstr *gp,*subp;

	gp=genrData;
	for(i=1;i<=n;i++,gp=gp->next)
	{
		if(strcmp(gp->st_id,st)) continue;
			
		subp=(struct genrstr *)malloc(sizeof(struct genrstr));
		if(subp==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }

		memcpy(subp,gp,sizeof(struct genrstr));
		
		subp->next=sub_genrData;
		sub_genrData=subp;
	}//end for 
	////////////////////////////////////////////////////
	
	return;
}

void read_unitparm(long prnt)
{
	if(prnt==YES) printf("\n\nread_unitparm()...");
	if(prnt==YES) printf("\n Involved tables: UNITPARM");

	register long i;
        long n;

        struct genrstr *gp;
        /////////////////////////////////////////////////

	n=chainLen(genrData);
	gp=genrData;

	for(i=1;i<=n;i++,gp=gp->next)
	{
		sprintf(cmdbuf,"select MWMAX,MWMIN,RAMPUP,RAMPDN from trade%ld.T_UNITPARM where UNIT_ID='%s'",year,gp->id);
		ret=ociTools.ORA_BindWithReturnArray(cmdbuf,(void *)numbuf);
		if(ret==OCI_FAIL){ printf("\nError! --- %ld",__LINE__); exit(0); }

		gp->wmax=atof(numbuf[0]);
		gp->wmin=atof(numbuf[1]);
		
		gp->rampUp=atof(numbuf[2]);
		gp->rampDn=atof(numbuf[3]);
	}//end for
	//////////////////////////////////////////////	

	return;
}

void read_sched(long prnt)
{
	if(prnt==YES) printf("\n\nread_sched()...");

	if(prnt==YES) printf("\n Involved tables: HYDROPLAN, DNONBIDPLANT/UNITPLAN_%ld_%ld",month,day);
	if(prnt==YES) printf("\n                  MTHUNITPLAN_%ld_%ld",month,day);

	register long i;
	long n;

	double sched;
	////////////////////////////////////////////////////////////////

	//1: reading hydro schedule

	sprintf(cmdbuf,"select HYDRO_ID from trade%d.T_HYDROPLAN where M_DATE='%s' and PERIOD=%d",year,date_str,sd);

	ret=ociTools.ORA_BindWithReturnArray(cmdbuf,(void *)strbuf);//No Change!
	if(ret==OCI_FAIL){ printf("\nError! --- %ld",__LINE__); exit(0); }

	sprintf(cmdbuf,"select WGEN from trade%d.T_HYDROPLAN where M_DATE='%s' and PERIOD=%d",year,date_str,sd);
        ret=ociTools.ORA_BindWithReturnArray(cmdbuf,(void *)numbuf);//No Change! 
        if(ret==OCI_FAIL){ printf("\nError! --- %ld",__LINE__); exit(0); }

	recNum=ret;
	if(prnt==YES) printf("\n\n hydro: recNum=%ld",recNum);
	////////////////////////////////////////////////////////////////////

	//1.2 saving the schedules

	for(i=0;i<recNum;i++)
	{
		strcpy(idbuf,strbuf[i]);
		sched=atof(numbuf[i]);
	
		ret=saveSched(genrData,idbuf,sched,NO);//prnt
		if(ret==NO)
		{
			printf("\n\nWarning! --- %ld",__LINE__);
			printf("\n  Sched. NOT saved: id=%s, sched=%.2lf",idbuf,sched);
		}//end if
	}//end for

	if(prnt==YES) printf("\n hydro sched. saved");
	/////////////////////////////////////////////////////////////////////////////////

	//2: reading nonbid unit schedules

	sprintf(cmdbuf,"select UNIT_ID from trade%d.T_DNONBIDUNITPLAN_%d_%d where PERIOD=%d",year,month,day,sd);
        ret=ociTools.ORA_BindWithReturnArray(cmdbuf,(void *)strbuf);
        if(ret==OCI_FAIL){ printf("\nError! --- %ld",__LINE__); exit(0); }

	sprintf(cmdbuf,"select WGEN from trade%d.T_DNONBIDUNITPLAN_%d_%d where PERIOD=%d",year,month,day,sd);
        ret=ociTools.ORA_BindWithReturnArray(cmdbuf,(void *)numbuf);
        if(ret==OCI_FAIL){ printf("\nError! --- %ld",__LINE__); exit(0); }

        recNum=ret;
        if(prnt==YES) printf("\n\n nonbid unit: recNum=%ld",recNum);
	////////////////////////////////////////////////////////////////

        //2.2 saving the schedules

        for(i=0;i<recNum;i++)
        {
                strcpy(idbuf,strbuf[i]);
                sched=atof(numbuf[i]);

                ret=saveSched(genrData,idbuf,sched,NO);//prnt
                if(ret==NO)
                {
                        printf("\n\nWarning! --- %ld",__LINE__);
                        printf("\n  Sched. NOT saved: id=%s, sched=%.2lf",idbuf,sched);
                }//end if
	}//end for

        if(prnt==YES) printf("\n nonbid unit sched. saved");
	/////////////////////////////////////////////////////////////////////////////////
	
	//3: reading nonbid plant schedules

	sprintf(cmdbuf,"select PLANT_ID from trade%d.T_DNONBIDPLANTPLAN_%d_%d where PERIOD=%d",year,month,day,sd);
        ret=ociTools.ORA_BindWithReturnArray(cmdbuf,(void *)strbuf);
        if(ret==OCI_FAIL){ printf("\nError! --- %ld",__LINE__); exit(0); }

	sprintf(cmdbuf,"select WGEN from trade%d.T_DNONBIDPLANTPLAN_%d_%d where PERIOD=%d",year,month,day,sd);
        ret=ociTools.ORA_BindWithReturnArray(cmdbuf,(void *)numbuf);
        if(ret==OCI_FAIL){ printf("\nError! --- %ld",__LINE__); exit(0); }

        recNum=ret;
        if(prnt==YES) printf("\n\n nonbid plant: recNum=%ld",recNum);
	//////////////////////////////////////////////////////////////////

	//3.2 saving the schedules

        for(i=0;i<recNum;i++)
        {
                strcpy(idbuf,strbuf[i]);
                sched=atof(numbuf[i]);

                ret=saveSched(genrData,idbuf,sched,NO);//prnt
                if(ret==NO)
                {
                        printf("\n\nWarning! --- %ld",__LINE__);
                        printf("\n  Sched. NOT saved: id=%s, sched=%.2lf",idbuf,sched);
                }//end if
        }//end for

	if(prnt==YES) printf("\n nonbid plant sched. saved");
        /////////////////////////////////////////////////////////////////////

	//4: reading mth-unit schedules

        sprintf(cmdbuf,"select UNIT_ID from trade%d.T_MTHUNITPLAN_%d_%d where PERIOD=%d",year,month,day,sd);
        ret=ociTools.ORA_BindWithReturnArray(cmdbuf,(void *)strbuf);//No Change!
        if(ret==OCI_FAIL){ printf("\nError! --- %ld",__LINE__); exit(0); }

        sprintf(cmdbuf,"select WGEN from trade%d.T_MTHUNITPLAN_%d_%d where PERIOD=%d",year,month,day,sd);
        ret=ociTools.ORA_BindWithReturnArray(cmdbuf,(void *)numbuf);//No Change!
        if(ret==OCI_FAIL){ printf("\nError! --- %ld",__LINE__); exit(0); }

        recNum=ret;
        if(prnt==YES) printf("\n\n mth-unit: recNum=%ld",recNum);
	//////////////////////////////////////////////////////

	//4.2 saving the schedules
	
        for(i=0;i<recNum;i++)
        {
                strcpy(idbuf,strbuf[i]);
                sched=atof(numbuf[i]);

                ret=saveSched(genrData,idbuf,sched,NO);//prnt
                if(ret==NO)
                {
                        printf("\n\nWarning! --- %ld",__LINE__);
                        printf("\n  Sched. NOT saved: id=%s, sched=%.2lf",idbuf,sched);
                }//end if
        }//end for

	if(prnt==YES) printf("\n mth-unit sched. saved");
	////////////////////////////////////////////////////////////////////

	return;
}

void read_group(long prnt)
{
	if(prnt==YES) printf("\n\nread_group()...");
	if(prnt==YES) printf("\n Involved tables: TIELINEDEF, TIELINECODING, TIELINELMT");

	register long i;
	struct groupstr *grp;
	////////////////////////////////////////////////

	//1: creating chain of groupData

	sprintf(cmdbuf,"select TIELINE_NAME,TIELINE_ID from trade%ld.T_TIELINEDEF",year);

	ret=ociTools.ORA_BindWithReturnArray(cmdbuf,(void *)strbuf);
        if(ret==OCI_FAIL){ printf("\nError! --- %ld",__LINE__); exit(0); }

	recNum=ret;
	if(prnt==YES) printf("\n recNum=%ld",recNum);
	/////////////////////////////////////////////////////

	groupData=NULL;//init.
	for(i=0;i<recNum;i++)
	{
		grp=(struct groupstr *)malloc(sizeof(struct groupstr));
		if(grp==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }

		memset(grp,0,sizeof(struct groupstr));

		strcpy(grp->descr,strbuf[i]);
		strcpy(grp->id,strbuf[i+recNum]);

		grp->next=groupData;
		groupData=grp;
	}//end for
	
	if(prnt==YES) printf("\n groupData created");
        /////////////////////////////////////////////////////
	
	//1.2 turnover and groupNum stat.

	turnOver(groupData);
	groupNum=chainLen(groupData);

	if(prnt==YES) printf("\n groupNum=%ld",groupNum);
	//////////////////////////////////////////////////////////////

	//2: reading lines and modes of group

	grp=groupData;
	for(i=1;i<=groupNum;i++,grp=grp->next)
	{
		if(prnt==YES) printf("\n\n group_id=%s, %s",grp->id,grp->descr);

		read_group_line(grp,grp->id,NO);//prnt
		if(prnt==YES) printf("\n read_group_line() ok");

		read_group_mode(grp,grp->id,NO);
		if(prnt==YES) printf("\n read_group_mode() ok");
	}//end for	

	if(prnt==YES) printf("\n groupData completed");
	////////////////////////////////////////////////////////////

	return;
}

void read_group_mode(struct groupstr *grp,char *grp_id,long prnt)
{
	if(prnt==YES) printf("\n\nread_group_mode()...");
	if(prnt==YES) printf("\n  Involved tables: TIELINELMT");

	register long i;
        ///////////////////////////////////////////////////////////////

        //1: group id checking

        if(strcmp(grp->id,grp_id))
        {
                printf("\n\nError! --- %ld",__LINE__);
                printf("\n  Invalid grp_id = %s, %s",grp->id,grp_id);

                exit(0);
        }//end if
        /////////////////////////////////////////////////////////////////

	//2: reading MODE_NAME and MODE_ID of group

	sprintf(cmdbuf,"select MODE_NAME,MODE_ID from trade%ld.T_TIELINELMT where GROUP_ID='%s'",year,grp_id);
	if(prnt==YES) printf("\n cmdbuf=%s",cmdbuf);
 
	ret=ociTools.ORA_BindWithReturnArray(cmdbuf,(void *)strbuf);
        if(ret==OCI_FAIL){ printf("\nError! --- %ld",__LINE__); exit(0); }

        recNum=ret;
        if(prnt==YES) printf("\n recNum=%ld",recNum);

	grp->modeNum=recNum;
	for(i=0;i<recNum;i++)
	{
		strcpy(grp->mode[i+1].descr,strbuf[i]);
		strcpy(grp->mode[i+1].id,strbuf[i+recNum]);
	}//end for

	if(prnt==YES) printf("\n mode_name/id ok");
	/////////////////////////////////////////////////////////////

	//3: reading other attr. of modes

	for(i=1;i<=grp->modeNum;i++)
	{
		//.1 MWMAX and MWMIN of mode

		sprintf(cmdbuf,"select MWMAX,MWMIN from trade%ld.T_TIELINELMT where GROUP_ID='%s' and MODE_ID='%s'",year,grp->id,grp->mode[i].id);

		ret=ociTools.ORA_BindWithReturnArray(cmdbuf,(void *)numbuf);
		if(ret==OCI_FAIL){ printf("\nError! --- %ld",__LINE__); exit(0); }

		grp->mode[i].mwmax=atof(numbuf[0]);
		grp->mode[i].mwmin=atof(numbuf[1]);
		///////////////////////////////////////////////////

		//.2 five conditions of the mode

		read_mode_cond(grp,i,(long)1);
		read_mode_cond(grp,i,(long)2);
		read_mode_cond(grp,i,(long)3);
		read_mode_cond(grp,i,(long)4);
		read_mode_cond(grp,i,(long)5);
	}//end for

	if(prnt==YES) printf("\n other attr. ok");
	//////////////////////////////////////////////////////////////

	return;
}

void read_mode_cond(struct groupstr *grp,long im,long k)
{
	char dev_descr[DESCRLN];
	char dev_id[IDLN];
	
	long mel;
	double val;
	///////////////////////////////////////////////////////

	//1: parm. checking
	
	if(k<1 || k>5)
	{
		printf("\n\nError! --- %ld",__LINE__);
		printf("\n  Invalid condition: k = %ld",k);
	
		exit(0);
	}//end if
	////////////////////////////////////////////////////////

	//2: reading DEV_DESCR, _ID and MEL of cond.

        sprintf(cmdbuf,"select DEV_DESCR%ld,DEV_ID%ld,MEL%ld from trade%ld.T_TIELINELMT where GROUP_ID='%s' and MODE_ID='%s'",k,k,k,year,grp->id,grp->mode[im].id);

        ret=ociTools.ORA_BindWithReturnArray(cmdbuf,(void *)strbuf);
        if(ret==OCI_FAIL){ printf("\nError! --- %ld",__LINE__); exit(0); }

	strcpy(dev_descr,strbuf[0]);
	strcpy(dev_id,strbuf[1]);

	mel=melfun(strbuf[2]);
	//////////////////////////////////////////////////////////////

	//3: reading VAL of the condition

        sprintf(cmdbuf,"select VAL%ld from trade%ld.T_TIELINELMT where GROUP_ID='%s' and MODE_ID='%s'",k,year,grp->id,grp->mode[im].id);

	ret=ociTools.ORA_BindWithReturnArray(cmdbuf,(void *)numbuf);
        if(ret==OCI_FAIL){ printf("\nError! --- %ld",__LINE__); exit(0); }

	val=atof(numbuf[0]);
	//////////////////////////////////////////////////////////////

	//4: saving the condition

	if(k==1) strcpy(grp->mode[im].dev_descr1,dev_descr);
	if(k==2) strcpy(grp->mode[im].dev_descr2,dev_descr);
        if(k==3) strcpy(grp->mode[im].dev_descr3,dev_descr);  
        if(k==4) strcpy(grp->mode[im].dev_descr4,dev_descr);  
        if(k==5) strcpy(grp->mode[im].dev_descr5,dev_descr);  

        if(k==1) strcpy(grp->mode[im].dev_id1,dev_id);
	if(k==2) strcpy(grp->mode[im].dev_id2,dev_id);
        if(k==3) strcpy(grp->mode[im].dev_id3,dev_id);
        if(k==4) strcpy(grp->mode[im].dev_id4,dev_id);
        if(k==5) strcpy(grp->mode[im].dev_id5,dev_id);

        if(k==1) grp->mode[im].mel1 = mel;
        if(k==2) grp->mode[im].mel2 = mel;
        if(k==3) grp->mode[im].mel3 = mel;
        if(k==4) grp->mode[im].mel4 = mel;
        if(k==5) grp->mode[im].mel5 = mel;

	if(k==1) grp->mode[im].val1 = val;
        if(k==2) grp->mode[im].val2 = val;
        if(k==3) grp->mode[im].val3 = val;
        if(k==4) grp->mode[im].val4 = val;
        if(k==5) grp->mode[im].val5 = val;
	//////////////////////////////////////////////////////////////

	return;
}

void read_group_line(struct groupstr *grp,char *grp_id,long prnt)
{
	if(prnt==YES) printf("\n\nread_group_line()...");
	if(prnt==YES) printf("\n  Involved tables: TIELINECODING");

	register long i;

	char st[50],zst[50],mst[50];
	struct linestr *lp;
	/////////////////////////////////////////////////

	//1: group id checking

	if(strcmp(grp->id,grp_id))
	{
		printf("\n\nError! --- %ld",__LINE__);
		printf("\n  Invalid grp_id = %s, %s",grp->id,grp_id);

		exit(0);
	}//end if 
	/////////////////////////////////////////////////

	//2: reading lines of group

	sprintf(cmdbuf,"select LINE_NAME,LINE_ID,MST_NAME,MST from trade%d.T_TIELINECODING where GROUP_ID='%s'",year,grp_id);
	ret=ociTools.ORA_BindWithReturnArray(cmdbuf,(void *)strbuf);
        if(ret==OCI_FAIL){ printf("\nError! --- %ld",__LINE__); exit(0); }

        recNum=ret;
        if(prnt==YES) printf("\n recNum=%ld",recNum);
        /////////////////////////////////////////////////////

	//3: saving the lines

	for(i=0;i<recNum;i++)
	{
		//3.1 fundamental data 

		grp->lineNum=recNum;

		strcpy(grp->line[i+1].descr,strbuf[i]);
		strcpy(grp->line[i+1].id,strbuf[i+recNum]);

		strcpy(grp->line[i+1].mst_descr,strbuf[i+2*recNum]);
		strcpy(grp->line[i+1].mst_id,strbuf[i+3*recNum]);
		//////////////////////////////////////////

		//3.2 ibs and zbs of line 

		lp=findData(grp->line[i+1].id,lineData);
		if(lp==NULL) continue;//invalid line
	
		grp->line[i+1].i=lp->i;
		grp->line[i+1].j=lp->j;
		//////////////////////////////////////////

		//3.3 imst of the line

		strcpy(mst,grp->line[i+1].mst_id);	
		strcpy(st,lp->st);
		strcpy(zst,lp->zst);

		if(!strcmp(mst,st))  grp->line[i+1].imst=lp->i; 
		if(!strcmp(mst,zst)) grp->line[i+1].imst=lp->j;	
	}//end for
	/////////////////////////////////////////////////////

	return;
}

void OPF_read_oracle(char *date_str,long sd,long prnt)
{
	if(prnt==YES) printf("\n\nOPF_read_oracle()...");

	//1: year, month, day, and sd --- globals

	year = date_year(date_str,"mm/dd/yyyy");
	month= date_month(date_str,"mm/dd/yyyy");
	day  = date_day(date_str,"mm/dd/yyyy");

	if(prnt==YES) printf("\n year = %ld",year);
	if(prnt==YES) printf("\n month= %ld",month);
	if(prnt==YES) printf("\n day  = %ld",day);
	if(prnt==YES) printf("\n sd   = %ld",sd);
	/////////////////////////////////////////////////////

        //2: connecting ora.server

        if(getenv("ORACLE_SID")==NULL)
        {
                printf("\n\nError! --- %ld",__LINE__);
                printf("\n  Env. variable of ORACLE_SID not defined.\n");

                exit(0);
        }//end if

        strcpy(oraServer,getenv("ORACLE_SID"));
        conn_oraServer(oraServer,NO);//prnt

        if(prnt==YES) printf("\n conn_oraServer() ok");
        ////////////////////////////////////////////////////////

	//3: reading globals

	read_global(YES);
	if(prnt==YES) printf("\n read_global() ok");
	////////////////////////////////////////////////////////

	//4: reading unit data 
	
	read_unitparm(YES);
	if(prnt==YES) printf("\n read_unitparm() ok");

	read_sched(YES);//prnt
	if(prnt==YES) printf("\n read_sched() ok");
	////////////////////////////////////////////////////////

	//5: reading loadfor

        read_loadfor(YES);//prnt
        if(prnt==YES) printf("\n read_loadfor() ok");

	//6: reading groups

	read_group(YES);//prnt
	if(prnt==YES) printf("\n read_group() ok");
	///////////////////////////////////////////////////////

	//7: disconnect ora.server

	disconn_oraServer();
	if(prnt==YES) printf("\n disconn_oraServer() ok");

	return;
}

void read_global(long prnt)
{
	if(prnt==YES) printf("\n\nread_global()...");
	if(prnt==YES) printf("\n Involved tables: SYSCTRLPARM, SCDPARM");

	//1: av_volt and flow_eps

	av_volt =1.0;//No Change!
	flow_eps=0.001;
	//////////////////////////////////////////////////////////

	//2: loss ratio

	sprintf(cmdbuf,"select PARM_VALUE from trade%ld.T_SYSCTRLPARM where PARM_NAME='loss_ratio'",year);
	if(prnt==YES) printf("\n cmdbuf=%s",cmdbuf);

	ret=ociTools.ORA_BindWithReturnArray(cmdbuf,(void *)strbuf);
       	if(ret==OCI_FAIL)
	{
		printf("\n\nWarning! --- %ld",__LINE__);
		printf("\n  Sys.parm NOT available: loss_ratio");
	}//end if

	if(ret==OCI_FAIL) loss_ratio=LOSS_RATIO_DFLT;
	if(ret!=OCI_FAIL) loss_ratio=atof(strbuf[0]);
	
	if(prnt==YES) printf("\n loss_ratio=%.4lf",loss_ratio);
	////////////////////////////////////////////////////////////

	//todo:

	return;
}

void consoleMsg(char *prog_id,char *msg)
{
        consoleMsg(prog_id,"",msg,"");
        return;
}

void consoleMsg(char *prog_id,char *prog_descr,char *msg,char *msg_xpln)
{
        //1: crr.date/time and conn. to server

        date(crrdate,"mm/dd/yyyy");
        wtime(crrtime);

        year = date_year(crrdate,"mm/dd/yyyy");
        month= date_month(crrdate,"mm/dd/yyyy");
        day  = date_day(crrdate,"mm/dd/yyyy");
        ////////////////////////////////////////////////

        if(getenv("ORACLE_SID")==NULL)
        {
                printf("\n\nError! --- %ld",__LINE__);
                printf("\n  Env. variable of ORACLE_SID not defined.\n");

                exit(0);
        }//end if

        strcpy(oraServer,getenv("ORACLE_SID"));
        conn_oraServer(oraServer,NO);//prnt
        //////////////////////////////////////////////////////

        //2: deleting records

        sprintf(cmdbuf,"delete trade%ld.T_PROGRAMEXECINFO where PROGRAM_ID='%s' and M_DATE='%s' and M_TIME='%s'",year,prog_id,crrdate,crrtime);

        ret=ociTools.ORA_BindNoReturn(cmdbuf);
        if(ret==OCI_FAIL){ printf("\nError! --- %ld",__LINE__); exit(0); }
        ////////////////////////////////////////////////////////////
        
	//3: inserting msg. record

        sprintf(cmdbuf,"insert into trade%ld.T_PROGRAMEXECINFO (PROGRAM_ID,PROGRAM_NAME,M_DATE,M_TIME,RESULT,INFORMATION) VALUES('%s','%s','%s','%s','%s','%s')",year,prog_id,prog_descr,crrdate,crrtime,msg,msg_xpln);

        ret=ociTools.ORA_BindNoReturn(cmdbuf);
        if(ret==OCI_FAIL){ printf("\nError! --- %ld",__LINE__); exit(0); }
        /////////////////////////////////////////////////////////

        //4: dis.connect

        disconn_oraServer();
        return;
}

void conn_oraServer(char *_oraServer,long prnt)
{
        if(prnt==YES) printf("\n\nconn_oraServer()...");

        //1: init. environmental variables

        strcpy(userName,"system");//No Change!
        strcpy(passWord,"manager");
        strcpy(oraServer,_oraServer);

        if(prnt==YES) printf("\n userName = %s",userName);
        if(prnt==YES) printf("\n passWord = %s",passWord);
        if(prnt==YES) printf("\n oraServer= %s",oraServer);
        ///////////////////////////////////////////////////////

        //2: oci.connect

        int ret=ociTools.Connect(userName,passWord,oraServer);
        if(ret==OCI_FAIL)
        {
                printf("\n\nError! --- %ld",__LINE__);
                printf("\n  Cannot connect to ora.Server: %s\n",oraServer);

                exit(0);
        }//end if

        if(prnt==YES) printf("\n oci.Connect() ok");
        ////////////////////////////////////////////////////

        return;
}

void disconn_oraServer()
{
        int ret=ociTools.Disconnect();
        if(ret==OCI_FAIL){ printf("\nError! --- %ld",__LINE__); exit(0); }

        return;
}

////////////////////////////////////////

//end of file
