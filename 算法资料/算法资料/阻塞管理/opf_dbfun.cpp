//opff_dbfun.cpp by Wang GT
///////////////////////////////////////////

#include"common.h"
#include"opf.h"
#include"opf_db.h"
#include"opf_global.h"

#include<dstruct.h>
#include<oodbglobe.h>
#include<oodb.h>
#include<opfdb.h>
#include<psbob.h>

#include"psbobserve.h"
////////////////////////////////////////////////////////////////////

void OPF_readDB(char sysName[],char bobName[],char srvName[],long prnt)
{
 	if(prnt==YES) printf("\n\nOPF_readDB()...");

	register long i,j;
	long iopen,zopen,ret,ret2;
	
	double aux2,vl;//voltage level
	double xfstep,xfmxtap,xfmntap,xfnom;
	///////////////////////////////////////////////////////

	//1: oodbinit

	strcpy(sys_name,sysName); //environmental variables
	strcpy(bob_name,bobName);
        strcpy(real_srv,srvName);
	
	printf("\n sys_name = %s",sys_name);
	printf("\n bob_name = %s",bob_name);
	printf("\n real_srv = %s",real_srv);

        ret=oodbinit();
        if(ret==OO_FAIL){ printf("\n\nError! --- %s: %ld",__FILE__,__LINE__); exit(0); }
	if(prnt==YES) printf("\n oodbinit() ok");
	///////////////////////////////////////////////////////////

	//2: oodbopen
 
        psbob_db_descr=(DB_DESCR *)malloc(sizeof(DB_DESCR));
	if(psbob_db_descr==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }
       	
	ret=oodbopen("psbob",sys_name,bob_name,real_srv,W,psbob_db_descr);
	if(ret==OO_FAIL){ printf("\n\nError! --- %ld",__LINE__); exit(0); }
        //////////////////////////////////////////////////////////////////////////

	//3: oodbuse

        ln_descr=(TB_DESCR *)malloc(sizeof(TB_DESCR));
        ret=oodbuse("ln",psbob_db_descr,ln_descr);
        if(ret==OO_FAIL){ printf("\n\nError! --- %ld",__LINE__); exit(0); }
	
        lv_ln=oodbget_recnum(ln_descr);
        ////////////////////////////////////////////////////

        ld_descr=(TB_DESCR *)malloc(sizeof(TB_DESCR));
        ret=oodbuse("ld",psbob_db_descr,ld_descr);
        if(ret==OO_FAIL){ printf("\n\nError! --- %ld",__LINE__); exit(0); }

        lv_ld=oodbget_recnum(ld_descr);
        //////////////////////////////////////////////////////////

        xf_descr=(TB_DESCR *)malloc(sizeof(TB_DESCR));
        ret=oodbuse("xf",psbob_db_descr,xf_descr);
        if(ret== OO_FAIL){ printf("\n\nError! --- %ld",__LINE__); exit(0); }

        lv_xf=oodbget_recnum(xf_descr);
        ////////////////////////////////////////////////////

        un_descr=(TB_DESCR *)malloc(sizeof(TB_DESCR));
        ret=oodbuse("un",psbob_db_descr,un_descr);
        if(ret==OO_FAIL){ printf("\n\nError! --- %ld",__LINE__); exit(0); }

        lv_un=oodbget_recnum(un_descr);
        /////////////////////////////////////////////////////

        cp_descr=(TB_DESCR *)malloc(sizeof(TB_DESCR));
        ret=oodbuse("cp",psbob_db_descr,cp_descr);
        if(ret==OO_FAIL){ printf("\n\nError! --- %ld",__LINE__); exit(0); }

        lv_cp=oodbget_recnum(cp_descr);
        /////////////////////////////////////////////////////

        bs_descr=(TB_DESCR *)malloc(sizeof(TB_DESCR));
        ret=oodbuse("bs",psbob_db_descr,bs_descr);
        if(ret==OO_FAIL){ printf("\n\nError! --- %ld",__LINE__); exit(0); }

        lv_bs=oodbget_recnum(bs_descr);
        ////////////////////////////////////////////////////////

        tapty_descr=(TB_DESCR *)malloc(sizeof(TB_DESCR));
        ret=oodbuse("tapty",psbob_db_descr,tapty_descr);
        if(ret==OO_FAIL){ printf("\n\nError! --- %ld",__LINE__); exit(0); }

        lv_tapty=oodbget_recnum(tapty_descr);
	/////////////////////////////////////////////////////////////////

        kv_descr=(TB_DESCR *)malloc(sizeof(TB_DESCR));
        ret=oodbuse("kv",psbob_db_descr,kv_descr);
        if(ret==OO_FAIL){ printf("\n\nError! --- %ld",__LINE__); exit(0); }

        lv_kv=oodbget_recnum(kv_descr);
        ////////////////////////////////////////////////////////////////

	//4: oodbread

        kv=NULL;
	kv=(KV *)malloc(lv_kv*sizeof(KV));
	
        ret=oodbread_rp(kv,0,kv_descr,lv_kv,lv_kv*sizeof(KV));
        if(ret==NULL||kv==NULL){ printf("\n\nError! --- %ld",__LINE__); exit(0); }
        ////////////////////////////////////////////////////

        tapty=NULL;
        tapty=(TAPTY *)malloc(lv_tapty*sizeof(TAPTY));

        ret=oodbread_rp(tapty,0,tapty_descr,lv_tapty,lv_tapty*sizeof(TAPTY));
        if(ret==NULL||tapty==NULL){ printf("\n\nError! --- %ld",__LINE__); exit(0); }
	////////////////////////////////////////////////////

        bs=NULL;
        bs=(BS *)malloc(lv_bs*sizeof(BS));

        ret=oodbread_rp(bs,0,bs_descr,lv_bs,lv_bs*sizeof(BS));
        if(bs==NULL||ret==NULL){ printf("\n\nError! --- %ld",__LINE__); exit(0); }
        //////////////////////////////////////////////////////////////////

        cp=NULL;
        cp=(CP *)malloc(lv_cp*sizeof(CP));

        ret=oodbread_rp(cp,0,cp_descr,lv_cp,lv_cp*sizeof(CP));
        if(cp==NULL||ret==OO_FAIL){ printf("\n\nError! --- %ld",__LINE__); exit(0); }
        ////////////////////////////////////////////////////////////////////////

        un=NULL;
        un=(UN *)malloc(lv_un*sizeof(UN));

        ret=oodbread_rp(un,0,un_descr,lv_un,lv_un*sizeof(UN));
        if(un==NULL || ret==OO_FAIL){ printf("\n\nError! --- %ld",__LINE__); exit(0); }
	//////////////////////////////////////////////////////////////////////

        ln=NULL;
        ln=(LN *)malloc(lv_ln*sizeof(LN));

        ret=oodbread_rp(ln,0,ln_descr,lv_ln,lv_ln*sizeof(LN));
        if(ln==NULL || ret==OO_FAIL){ printf("\n\nError! --- %ld",__LINE__); exit(0); }
        //////////////////////////////////////////////////////////////////////

        ld=NULL;
        ld=(LD *)malloc(lv_ld*sizeof(LD));
        if(ld==NULL){ printf("\n\nError! --- %ld",__LINE__); exit(0); }

        ret=oodbread_rp(ld,0,ld_descr,lv_ld,lv_ld*sizeof(LD));
        if(ret==OO_FAIL){ printf("\n\nError! --- %ld",__LINE__); exit(0); }
        ///////////////////////////////////////////////////////////////////

        xf=NULL;
        xf=(XF *)malloc(lv_xf*sizeof(XF));
        if(xf==NULL){ printf("\n\nError! --- %ld",__LINE__); exit(0); }

        ret=oodbread_rp(xf,0,xf_descr,lv_xf,lv_xf*sizeof(XF));
        if(ret==OO_FAIL){ printf("\n\nError! --- %ld",__LINE__); exit(0); }
        ///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////

      	//Reading lineData 
	
	if(prnt==YES) printf("\n Reading lineData...");

	lineData=NULL;
	struct linestr *lp;

	for(i=0;i<lv_ln;i++)//ln
	{
		if((bs+(ln+i)->ibs)->iisland!=opf_island) continue;
                if((bs+(ln+i)->zbs)->iisland!=opf_island) continue;

		iopen=(ln+i)->topflags & LN_IOPEN;
	        zopen=(ln+i)->topflags & LN_ZOPEN;
		
		if(iopen && zopen) continue;//both open
		if((ln+i)->topflags & LN_DEAD) continue;
		///////////////////////////////////////////////////

		lp=(struct linestr *)malloc(sizeof(struct linestr));
		if(lp==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }

		strcpy(lp->id,(ln+i)->id);
		strcpy(lp->st,(ln+i)->st);
		strcpy(lp->zst,(ln+i)->zst);

		lp->i	=(ln+i)->ibs;
		lp->j	=(ln+i)->zbs;

		lp->r	=(ln+i)->r;
		lp->x	=(ln+i)->x;
		lp->kb	=(ln+i)->bch/2.0;
		lp->lmt	=0.0;//no consideration

		lp->next=lineData;
		lineData=lp;
	}//end for

	for(i=0;i<lv_xf;i++)//xf
	{
		if((bs+(xf+i)->ibs)->iisland!=opf_island) continue;
                if((bs+(xf+i)->zbs)->iisland!=opf_island) continue;

		iopen=(xf+i)->topflags & XF_IOPEN;
                zopen=(xf+i)->topflags & XF_ZOPEN;

                if(iopen && zopen) continue;//both open
		if((xf+i)->topflags & XF_DEAD) continue;
		////////////////////////////////////////////////////

		lp=(struct linestr *)malloc(sizeof(struct linestr));
		if(lp==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }

		strcpy(lp->id,(xf+i)->id);
		strcpy(lp->st,(xf+i)->st);	
		strcpy(lp->zst,"");

		lp->i	=-(xf+i)->ibs;
		lp->j 	=(xf+i)->zbs;

		lp->r   =(xf+i)->r;
                lp->x   =(xf+i)->x;

                //xfstep  =(tapty+(xf+i)->itapty)->step;
                //xfnom   =(tapty+(xf+i)->itapty)->nom;
		//lp->kb	=1.0+((xf+i)->tap-xfnom)*xfstep;
		lp->kb	=(xf+i)->t;
		lp->lmt	=0.0;//no consideration
	
		lp->next=lineData;
		lineData=lp;
	}//end for

	if(prnt==YES) printf("ok");
	/////////////////////////////////////////////////////

	//Reading nodeData

	if(prnt==YES) printf("\n Reading nodeData...");

	double genr_volt,genr_powr;
	struct genrstr *gp,*gp2;

	nodeData=NULL;
	struct nodestr *np;

	for(i=0;i<lv_ld;i++)//load
	{
		if((bs+(ld+i)->ibs)->iisland!=opf_island) continue;
		if((ld+i)->q & LD_OPEN) continue; 
		if((ld+i)->topflags & LD_DEAD) continue;	
		/////////////////////////////////////////////////////////////

		np=(struct nodestr *)malloc(sizeof(struct nodestr));
		if(np==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }

		strcpy(np->id,(ld+i)->id);
		np->i	=(ld+i)->ibs;
		
		np->attr=PQ;
		np->p	=0.0;//init.
		np->q	=0.0;

		np->next=nodeData;
		nodeData=np;
	}//end for

	for(i=0;i<lv_un;i++)//generators
	{
		if((bs+(un+i)->ibs)->iisland!=opf_island) continue;
		if((un+i)->q & UN_OPEN) continue;
		if((un+i)->topflags & UN_DEAD) continue;
		//////////////////////////////////////////////////

		np=(struct nodestr *)malloc(sizeof(struct nodestr));
		if(np==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }
		
		strcpy(np->id,(un+i)->id);
		np->i   =(un+i)->ibs;

		genr_powr=0.0;//init. to be from Sybase
		genr_volt=(un+i)->vtarget;

		if(genr_volt<SMLL) genr_volt=1.0;
		////////////////////////////////////////

		if((un+i)->slackun==1)
		{
			np->attr=SLK;
			np->p	=genr_volt;
			np->q	=0.0;//default
		}
		else//other generators
		{
			np->attr=PV;
			np->p   =genr_powr;
			np->q   =genr_volt;
			
		}//end if-else
	
		np->next=nodeData;
		nodeData=np;
	}//end of for

	if(prnt==YES) printf("ok");
	//////////////////////////////////////////////////////

	//Reading capaData

	if(prnt==YES) printf("\n Reading capaData...");
	double mrnom;

	capaData=NULL;
	struct capastr *cpp;

	for(i=0;i<lv_cp;i++)
	{
		if((bs+(cp+i)->ibs)->iisland!=opf_island) continue;
		if((cp+i)->q & CP_OPEN) continue;
		if((cp+i)->topflags & CP_DEAD) continue;
		//////////////////////////////////////////////////////////////

		cpp=(struct capastr *)malloc(sizeof(struct capastr));
		if(cpp==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }

		strcpy(cpp->id,(cp+i)->id);
		cpp->i	=(cp+i)->ibs;
		
		cpp->Q0	=(cp+i)->r;
		mrnom	=(cp+i)->mrnom;
	
		if(mrnom>0.0)//capacitor
		{
			cpp->Qmin=0.0;
			cpp->Qmax=mrnom;
		}
		else//reactor
		{
			cpp->Qmin=mrnom;
			cpp->Qmax=0.0;
		}//end if-else
		///////////////////////////////

		cpp->next=capaData;
		capaData=cpp;
	}//end of for
	
	if(prnt==YES) printf("ok");
	/////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////

	//Completing data chains from Sybase

	long k,flg=NO;//init.
	long ibs,zbs;
	//////////////////////////////////////////////////////

	if(prnt==YES) printf("\n Completing assoData...");

	struct assostr *asp=assoData;
	while(asp!=NULL)
	{
		for(i=1;i<=asp->unitNum;i++)
		{
			ibs=0;//init.

			np=findData(asp->unit[i].id,nodeData,PVSLACK);
			if(np!=NULL) ibs=np->i;
			
			asp->unit[i].i=ibs;
		}//end for

		asp=asp->next;
	}//end while

	if(prnt==YES) printf("ok");
	//////////////////////////////////////////////////////

	if(prnt==YES) printf("\n Completing areaData...");

	struct areastr *ap=areaData;
	while(ap!=NULL)
	{
		for(i=1;i<=ap->lineNum;i++)
		{
			ibs=0;//init.
			zbs=0;

			lp=findData(ap->line[i].id,lineData);
			if(lp!=NULL){ ibs=lp->i; zbs=lp->j; }

			ap->line[i].i=ibs;
			ap->line[i].j=zbs;
		}//end for
		////////////////////////////////////

		for(i=1;i<=ap->unitNum;i++)
		{
			ibs=0;//init.
			np=findData(ap->unit[i].id,nodeData,PVSLACK);
			
			if(np!=NULL) ibs=np->i;
			ap->unit[i].i=ibs;
		}//end for
		///////////////////////////////////

		ap=ap->next;
	}//end while

	if(prnt==YES) printf("ok");
	/////////////////////////////////////////////////////////

	if(prnt==YES) printf("\n Completing loadData...");

	flg=NO;//init.
	struct loadstr *ldp=loadData;

	while(ldp!=NULL)
	{
		k=posiRecord(ld,ldp->id,lv_ld);

		if(k!=NO) ldp->i=(ld+k)->ibs;
		else
		{
                        printf("\n\nError! --- %ld",__LINE__);
                        printf("\n  NOT found ld record: id=%s",ldp->id);

			ldp->i=0;
			flg=YES;//marked
		}

		ldp=ldp->next;
	}//end while

	if(prnt==YES && flg!=YES) printf("ok");
	////////////////////////////////////////////////////
 
	if(prnt==YES) printf("\n Completing genrData...");
	
	flg=NO;//init.
	gp=genrData;

	while(gp!=NULL)
	{
		k=posiRecord(un,gp->id,lv_un);
		
		if(k!=NO) gp->i=(un+k)->ibs;
		else
		{ 
			printf("\n\nError! --- %ld",__LINE__);
			printf("\n  NOT found un record: id=%s",gp->id); 
			
			gp->i=0;
			flg=YES;//marked
		}

		gp=gp->next;
	}//end while
	
	if(prnt==YES && flg!=YES) printf("ok");
	////////////////////////////////////////////////////

	if(prnt==YES) printf("\n Completing genrData2...");

	flg=NO;//init.
	gp2=genrData2;

	while(gp2!=NULL)
	{
		k=posiRecord(un,gp2->id,lv_un);
	
		if(k!=NO) gp2->i=(un+k)->ibs;
		else
		{
			printf("\n\nError! --- %ld",__LINE__);
                        printf("\n  NOT found un record: id=%s",gp2->id);

			gp2->i=0;	
			flg=YES;//marked
		}

		gp2=gp2->next;
	}//end while

	if(prnt==YES && flg!=YES) printf("ok");
	//////////////////////////////////////////////////////

	if(prnt==YES) printf("\n Completing priceData...");

	flg=NO;//init.	
	struct pricstr *pp=priceData;

	while(pp!=NULL)
	{
		k=posiRecord(un,pp->id,lv_un);

		if(k!=NO) pp->i=(un+k)->ibs;
		else
		{ 
			printf("\n\nError! --- %ld",__LINE__);
			printf("\n  NOT found un record: id=%s",pp->id);
 			
			pp->i=0;
			flg=YES;//marked
		}

		pp=pp->next;
	}//end while

	if(prnt==YES && flg!=YES) printf("ok");
	/////////////////////////////////////////////////////////

	if(prnt==YES) printf("\n Completing groupData...");

	flg=NO;//init.	
	struct groupstr *grp=groupData;

	while(grp!=NULL)
	{
		for(i=1;i<=grp->lineNum;i++)
		{
			k=posiRecord(ln,grp->line[i].id,lv_ln);
	
			if(k==NO)//invalid branch
			{ 
				printf("\n\nError! --- %ld",__LINE__);
				printf("\n  NOT found ln: id=%s, group_id=%s",grp->line[i].id,grp->id);

				flg=YES;//marked 
			}
			else
			{
				grp->line[i].i   =(ln+k)->ibs;
				grp->line[i].j   =(ln+k)->zbs;
				grp->line[i].imst=(ln+k)->ibs;//default	
				
				strcpy(buf,grp->line[i].mst);
				strcpy(buffer,(ln+k)->zst);

				if(!strcmp(buf,buffer))
					grp->line[i].imst=(ln+k)->zbs;
			}//end else
		}//end for

		grp=grp->next;
	}//end while

	if(prnt==YES && flg!=YES) printf("ok");
	///////////////////////////////////////////////////////////////

	//close databases and free momery

	oodbclose("psbob",sys_name,bob_name,real_srv,psbob_db_descr);
	free(psbob_db_descr);

	free(bs_descr); 	free(bs);
	free(cp_descr); 	free(cp);
	free(ld_descr); 	free(ld);
        free(ln_descr);		free(ln);
	free(tapty_descr); 	free(tapty);
	free(un_descr); 	free(un);
	free(xf_descr); 	free(xf);
	////////////////////////////////////////////

        turnOver(capaData);
	turnOver(lineData);
	turnOver(nodeData);
	turnOver(xfmrData);
	/////////////////////////

	if(flg==YES) exit(0);
	return;
}

template<class TYPE>
long posiRecord(TYPE *tb,char *id,long lv_tb)
{
	register long i;
	long ret=NO;//init.

	for(i=0;i<lv_tb;i++)
	{
		if(!strcmp((tb+i)->id,id))
		{
			ret=i;
			break;
		}
	}//end for
	////////////////////////

	return ret;
}
	
//end of file
