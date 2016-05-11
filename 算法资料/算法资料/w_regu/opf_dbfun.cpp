//opf_dbfun.cpp by Wang GT
///////////////////////////////////////////

#include"common.h"
#include"opf.h"
#include"opf_db.h"
#include"opf_global.h"

#include<dstruct.h>
#include<oodbglobe.h>
#include<oodb.h>
#include<psbob.h>

#include"psbobserve.h"
/////////////////////////////////////////////////////

void OPF_read_rtdb(long prnt)
{
	if(prnt==YES) printf("\n\nOPF_read_rtdb()...");

	//1: init. and read tables

	opf_oodbinit(YES);//prnt
	printf("\n opf_oodbinit() ok");

        opf_oodbopen();
	printf("\n opf_oodbopen() ok");

        opf_oodbuse();
	printf("\n opf_oodbuse() ok");

        opf_oodbread();
        printf("\n opf_oodbread() ok");
	/////////////////////////////////////////////////////

        //2: reading original data

	read_genrData();
        read_lineData();
        read_nodeData();
        
	printf("\n read_data() ok");
	////////////////////////////////////////

	//3: closing database

        opf_oodbclose();
        printf("\n opf_oodbclose() ok");
	///////////////////////////////////

        return;
}

void opf_oodbinit(long prnt)
{
	if(prnt==YES) printf("\n\nopf_oodbinit()...");

	strcpy(sys_name,"realtime");
        strcpy(bob_name,"study");
        strcpy(real_srv,getenv("MYSERVER"));

	if(prnt==YES) printf("\n sys_name=%s",sys_name);
	if(prnt==YES) printf("\n bob_name=%s",bob_name);
	if(prnt==YES) printf("\n real_srv=%s",real_srv);

        int ret=oodbinit();
	if(ret==OO_FAIL)
	{ 
		printf("\n\nError! --- %s: %ld",__FILE__,__LINE__); 
		exit(0);
	}//end if
	///////////////////////////////////////////

	return;
}

void opf_oodbopen()
{
	psbob_db_descr=(DB_DESCR *)malloc(sizeof(DB_DESCR));
        if(psbob_db_descr==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }

        ret=oodbopen("psbob",sys_name,bob_name,real_srv,W,psbob_db_descr);
        if(ret==OO_FAIL){ printf("\n\nError! --- %ld",__LINE__); exit(0); }
	////////////////////////////////////////////////

	return;
}

void opf_oodbuse()
{
	//1: psbob tables

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

        kv_descr=(TB_DESCR *)malloc(sizeof(TB_DESCR));
        ret=oodbuse("kv",psbob_db_descr,kv_descr);
        if(ret==OO_FAIL){ printf("\n\nError! --- %ld",__LINE__); exit(0); }

        lv_kv=oodbget_recnum(kv_descr);
        //////////////////////////////////////////////////////////

	st_descr=(TB_DESCR *)malloc(sizeof(TB_DESCR));
	ret=oodbuse("st",psbob_db_descr,st_descr);
	if(ret==OO_FAIL){ printf("\n\nError! --- %ld",__LINE__); exit(0); }

	lv_st=oodbget_recnum(st_descr);
        //////////////////////////////////////////////////////////

	return;
}

void opf_oodbread()
{
	//1: reading psbob tables

	st=NULL;
	if(lv_st>0)
	{
		st=(ST *)malloc(lv_st*sizeof(ST));

		ret=oodbread_rp(st,0,st_descr,lv_st,lv_st*sizeof(ST));
		if(ret==OO_FAIL){ printf("\n\nError! --- %ld",__LINE__); exit(0); }
	}
	////////////////////////////////////////////////////

        kv=NULL;
        if(lv_kv>0)
	{
		kv=(KV *)malloc(lv_kv*sizeof(KV));

        	ret=oodbread_rp(kv,0,kv_descr,lv_kv,lv_kv*sizeof(KV));
        	if(ret==OO_FAIL){ printf("\n\nError! --- %ld",__LINE__); exit(0); }
        }
	////////////////////////////////////////////////////

        bs=NULL;
        if(lv_bs>0)
	{
		bs=(BS *)malloc(lv_bs*sizeof(BS));

        	ret=oodbread_rp(bs,0,bs_descr,lv_bs,lv_bs*sizeof(BS));
        	if(ret==OO_FAIL){ printf("\n\nError! --- %ld",__LINE__); exit(0); }
        }
	//////////////////////////////////////////////////////////////////

        cp=NULL;
        if(lv_cp)
	{
		cp=(CP *)malloc(lv_cp*sizeof(CP));

        	ret=oodbread_rp(cp,0,cp_descr,lv_cp,lv_cp*sizeof(CP));
        	if(ret==OO_FAIL){ printf("\n\nError! --- %ld",__LINE__); exit(0); }
        }
	////////////////////////////////////////////////////////////////////////

        un=NULL;
        if(lv_un>0)
	{
		un=(UN *)malloc(lv_un*sizeof(UN));

        	ret=oodbread_rp(un,0,un_descr,lv_un,lv_un*sizeof(UN));
        	if(ret==OO_FAIL){ printf("\n\nError! --- %ld",__LINE__); exit(0); }
        }
	//////////////////////////////////////////////////////////////////////

        ln=NULL;
        if(lv_ln>0)
	{
		ln=(LN *)malloc(lv_ln*sizeof(LN));

        	ret=oodbread_rp(ln,0,ln_descr,lv_ln,lv_ln*sizeof(LN));
        	if(ret==OO_FAIL){ printf("\n\nError! --- %ld",__LINE__); exit(0); }
        }
	//////////////////////////////////////////////////////////////////////

        ld=NULL;
        ld=(LD *)malloc(lv_ld*sizeof(LD));

        ret=oodbread_rp(ld,0,ld_descr,lv_ld,lv_ld*sizeof(LD));
        if(ret==OO_FAIL){ printf("\n\nError! --- %ld",__LINE__); exit(0); }
        ///////////////////////////////////////////////////////////////////

        xf=NULL;
        xf=(XF *)malloc(lv_xf*sizeof(XF));

        ret=oodbread_rp(xf,0,xf_descr,lv_xf,lv_xf*sizeof(XF));
        if(ret==OO_FAIL){ printf("\n\nError! --- %ld",__LINE__); exit(0); }
        ////////////////////////////////////////////////////////////////////////

	return;
}

void read_lineData()
{
	register long i;
	
	long iopen,zopen;
	double aux2,aux4;

	struct linestr *lp;
	/////////////////////////////////

	//1: reading data from ln

        lineData=NULL;
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

                strcpy(lp->descr,(ln+i)->descr);
                strcpy(lp->id,(ln+i)->id);

                strcpy(lp->st,(ln+i)->st);
                strcpy(lp->zst,(ln+i)->zst);

                lp->i   =(ln+i)->ibs;
                lp->j   =(ln+i)->zbs;

                lp->r   =(ln+i)->r;
                lp->x   =(ln+i)->x;
                lp->b0k =(ln+i)->bch/2.0;
                /////////////////////////////////////

                lp->lmt=(ln+i)->lmtlong;
                lp->vl =(kv+(bs+(ln+i)->ibs)->ikv)->vl;

                lp->next=lineData;
                lineData=lp;
        }//end for
        ////////////////////////////////////////////

        //2: reading data from xf

        for(i=0;i<lv_xf;i++)
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

                strcpy(lp->descr,(xf+i)->descr);
                strcpy(lp->id,(xf+i)->id);

                strcpy(lp->st,(xf+i)->st);
                strcpy(lp->zst,"");

                lp->i   =-(xf+i)->ibs;
                lp->j   =(xf+i)->zbs;

                lp->r   =(xf+i)->r;
                lp->x   =(xf+i)->x;
                lp->b0k =(xf+i)->t;//No Change!
                /////////////////////////////////////////////

                aux2=(kv+(bs+(xf+i)->ibs)->ikv)->vl;
                aux4=(kv+(bs+(xf+i)->zbs)->ikv)->vl;

                lp->lmt = (xf+i)->lmtlong;
                lp->vl  = max(aux2,aux4);
               //////////////////////////////////////////

                lp->next=lineData;
                lineData=lp;
        }//end for
        //////////////////////////////////////

	turnOver(lineData);
	return;
}

void read_nodeData()
{
        register long i;

	double genr_volt,genr_powr;
        double puf;//plant-use factor

        nodeData=NULL;
        struct nodestr *np;
        /////////////////////////////////////////

        //5.3.1 generator nodes

        for(i=0;i<lv_un;i++)//scanning un table
        {
                if((bs+(un+i)->ibs)->iisland!=opf_island) continue;
                if((un+i)->q & UN_OPEN) continue;
                if((un+i)->topflags & UN_DEAD) continue;
                //////////////////////////////////////////////////

                np=(struct nodestr *)malloc(sizeof(struct nodestr));
                if(np==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }

                strcpy(np->descr,(un+i)->descr);
                strcpy(np->id,(un+i)->id);
                np->i=(un+i)->ibs;

                puf=(un+i)->percent3/100.0;
                genr_powr=(un+i)->w*(1.0-puf);

                genr_volt=(un+i)->vtarget;
                if(genr_volt<SMLL) genr_volt=1.0;
                ////////////////////////////////////////

                if((un+i)->slackun==1)
                {
                        np->attr=SLACK;
                        np->p   =genr_volt;
                        np->qv  =0.0;//default
                }
                else//other generators
                {
                        np->attr=PV;
                        np->p   =genr_powr;
                        np->qv  =genr_volt;

                }//end if-else

                np->next=nodeData;
                nodeData=np;
        }//end for
        ////////////////////////////////////////////////

        //5.3.2 load nodes

        for(i=0;i<lv_ld;i++)//scanning ld table
        {
                if((bs+(ld+i)->ibs)->iisland!=opf_island) continue;
                if((ld+i)->q & LD_OPEN) continue;
                if((ld+i)->topflags & LD_DEAD) continue;
                //////////////////////////////////////////

                np=(struct nodestr *)malloc(sizeof(struct nodestr));
                if(np==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }

                strcpy(np->descr,(ld+i)->descr);
                strcpy(np->id,(ld+i)->id);

                np->i   =(ld+i)->ibs;
                np->attr=PQ;
                np->p   =(ld+i)->w;
                np->qv   =(ld+i)->r;

                np->next=nodeData;
                nodeData=np;
        }//end for
        ////////////////////////////////////////////////

        //5.3.3 noavr capacitors ---> all capa. www!!!

        for(i=0;i<lv_cp;i++)//scanning cp table
        {
                if((bs+(cp+i)->ibs)->iisland!=opf_island) continue;
                if((cp+i)->topflags & CP_DEAD) continue;

                if((cp+i)->q & CP_OPEN) continue;
                if((cp+i)->q & CP_AVR) continue;
                ////////////////////////////////////////

                np=(struct nodestr *)malloc(sizeof(struct nodestr));
                np=(struct nodestr *)malloc(sizeof(struct nodestr));
                if(np==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }

                strcpy(np->descr,(cp+i)->descr);
                strcpy(np->id,(cp+i)->id);

                np->i   =(cp+i)->ibs;
                np->attr=PQ;
                np->p   =0.0;
                np->qv   =-(cp+i)->r;

                np->next=nodeData;
                nodeData=np;
        }//end for
	/////////////////////////////////////////////

	turnOver(nodeData);
	return;
}

long index_STun(char *un_id)
{
	register long i;
	
	long n,ret=NO;//init.
	char abb[50];
	////////////////////////////////////////

	for(i=0;i<lv_st;i++)
	{
		strcpy(abb,(st+i)->abb);
		n=min(strlen(abb),strlen(un_id));

		if(!strncmp(un_id,abb,n)){ ret=i; break; }
	}//end for
	////////////////////////////////////

	return ret;
}

void read_genrData()
{
	register long i;
	
	long ist;
	double puf,vl;
	
	struct genrstr *gp;
	/////////////////////////////////////////////////

	genrData=NULL;
	for(i=0;i<lv_un;i++)
	{
		if((bs+(un+i)->ibs)->iisland!=opf_island) continue;
                if((un+i)->topflags & UN_DEAD) continue;
	
		if((un+i)->q & UN_OPEN) continue;	
		if((un+i)->q & UN_NOAVR) continue;	
		//////////////////////////////////////////////////

		gp=(struct genrstr *)malloc(sizeof(struct genrstr));
		if(gp==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }

		memset(gp,0,sizeof(struct genrstr));

		strcpy(gp->descr,(un+i)->descr);
		strcpy(gp->id,(un+i)->id);

		gp->i=(un+i)->ibs;
		
		ist=index_STun((un+i)->id);
		if(ist==NO){ printf("\n\nError! --- %ld",__LINE__); exit(0); }
	
		strcpy(gp->st_descr,(st+ist)->descr);
		strcpy(gp->st_id,(st+ist)->id);
		
		gp->next=genrData;
                genrData=gp;
	}//end for
	//////////////////////////////////////////////

	turnOver(genrData);
	return;
}

void opf_oodbclose()
{
	oodbclose("psbob",sys_name,bob_name,real_srv,psbob_db_descr);

	free(psbob_db_descr);

	free(bs_descr); 	free(bs);
	free(ld_descr); 	free(ld);
        free(ln_descr);		free(ln);
	free(un_descr); 	free(un);

	return;
}

//end of file
