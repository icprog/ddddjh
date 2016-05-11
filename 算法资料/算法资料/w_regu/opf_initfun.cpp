//opf_initfun.cpp by Wang GT
/////////////////////////////////

#include"common.h"
#include"opf.h"
#include"opf_global.h"
////////////////////////////////////////////////////

void printUsage()
{
        printf("\nUsage: opf_init date_str sd");
        printf("\n  date_str --- date string in format of mm/dd/yyyy");
        printf("\n  sd       --- period of time: 1, 2, ... 96\n");

        return;
}

void OPF_vfyData()
{
        vfyData(capaData);
        vfyData(genrData);
        vfyData(groupData);
        vfyData(nodeData);
        vfyData(priceData);
        vfyData(xfmrData);
	
	return;
}

long findMarginalNode(struct linestr *lineData,long *list,long *flag,long n)
{
        long ret=NO,ii,jj;
        struct linestr *lp;
        /////////////////////////////////////////

        lp=lineData;
        while(lp!=NULL)
        {
                ii=lookup(list,lp->i,n);
                jj=lookup(list,lp->j,n);

                if(ii==NO || jj==NO){ printf("\nError! --- %ld",__LINE__); exit(0); }

                if(flag[ii]==ONISLND && flag[jj]==OFFISLND){ ret=abs(list[ii]); break; }
                if(flag[ii]==OFFISLND && flag[jj]==ONISLND){ ret=abs(list[jj]); break; }

                lp=lp->next;
        }//end while
        ///////////////////////////////////

        return ret;
}

void formMtrx(long **mtrx,long *list,long n)
{
        register long i,j,k,ii,jj;
        struct linestr *lp;
        ///////////////////////////////////////////

        unitMatrix(mtrx,n);//init.
        k=0;
        
        lp=lineData;
        while(lp!=NULL)
        {
                i=lp->i;
                j=lp->j;

                if(lookup(list,i,k)==NO)//That means a new id.
                {
                        k++;
                        list[k]=abs(i);
                }
		
                if(lookup(list,j,k)==NO)
                {
                        k++;
                        list[k]=abs(j);
                }
                //////////////////////////

                ii=lookup(list,i,k);
                jj=lookup(list,j,k);

                mtrx[ii][jj]=1;//No Change!
                mtrx[jj][ii]=1;

                lp=lp->next;
        }//end while
        ///////////////////////////////

        return;
}

void nodeOptimize(long *codeList,long *list,long **mtrx,long nodeNum)
{
        register long i,j;
	long jj,k,n;

        long *line_bs;
        ////////////////////////////////////////////////

	//1: memory allocation

        line_bs=(long *)calloc(nodeNum+1,sizeof(long));
	if(line_bs==NULL){ printf("\n\nError! --- %ld",__LINE__); exit(0); }
	//////////////////////////////////////////////////

	//2: forming optimal codeList[] 

        n=nodeNum;//init.
	for(i=1;i<=nodeNum;i++,n--)//No Change!
	{
                //2.1 searching for bs 

		countLine(line_bs,mtrx,n);
                k=posmin(line_bs,n);
                
		codeList[i]=list[k];//bs obtained 
		//////////////////////////////////////////

		//2.2 modifying mtrx[][] 

                for(j=1;j<=n;j++)
		{
                        if(mtrx[k][j]==0) continue;

                        for(jj=j+1;jj<=n;jj++)
                        {
                                if(mtrx[k][jj]==0) continue;

                                mtrx[jj][j]=1;//No Change!
                                mtrx[j][jj]=1;
                        }//end for
                }//end for
                ////////////////////////////////////////

		//2.3 size reduction 

		delMatrixRow(mtrx,k,n,n);
		delMatrixCol(mtrx,k,n,n);

		delVectorElm(list,k,n);
		delVectorElm(line_bs,k,n);
        }//end for
	//////////////////////////////////////

        free(line_bs);
        return;
}
                              
long isNewLine(long i,long j,struct linestr *lineData)
{
        long ret=YES;//init.
        struct linestr *lp;
	////////////////////////////////

        lp=lineData;
        while(lp!=NULL)
        {
                if(brMatch(lp,i,j))
		{ 
			ret=NO; 
			break; 
		}//end if

                lp=lp->next;
        }//end while
	//////////////////////

        return ret;
}

void countLine(long *line_bs,long **mtrx,long n)
{
        register long i,j;
	long sum;
	/////////////////////////////

        for(i=1;i<=n;i++)
        {
                sum=0;//init.
                for(j=1;j<=n;j++)
                        sum+= mtrx[i][j];

                line_bs[i]=sum-1;//No Change!
        }//end for
	//////////////////////

        return;
}

void tp_conn(struct linestr *lineData)
{
	register long i;

        long ii,jj,margID;
	///////////////////////////////////////////////////////////////////

	//1: form nodeList[]

        long *nodeList=(long *)calloc(nodeNum+1,sizeof(long));
        long *nodeFlag=(long *)calloc(nodeNum+1,sizeof(long));

        if(nodeFlag==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }

        long n=0;//No Change!
	struct linestr *lp=lineData;

        while(lp!=NULL)
        {
                if(lookup(nodeList,lp->i,n)==NO)
                {
                        n++;
                        nodeList[n]=abs(lp->i);
                }

                if(lookup(nodeList,lp->j,n)==NO)
                {
                        n++;
                        nodeList[n]=abs(lp->j);
                }

                lp=lp->next;
        }//end while

        if(n>nodeNum){ printf("\n\nError! --- %ld",__LINE__); exit(0); }
        ///////////////////////////////////////////////////////////////////

        //2: init. and starting

        initVector(nodeFlag,n,(long)OFFISLND);

        margID=nodeList[1];
        nodeFlag[1]=ONISLND;
        //////////////////////////////////////////////

        //3: loop and marking

        while(margID!=NO)
        {
                lp=lineData;
                while(lp!=NULL)
                {
                        if(abs(lp->i)==margID)
                        {
                                jj=lookup(nodeList,lp->j,n);
                                nodeFlag[jj]=ONISLND;
                        }
                        else if(abs(lp->j)==margID)
                        {
                                ii=lookup(nodeList,lp->i,n);
                                nodeFlag[ii]=ONISLND;
                        }

                        lp=lp->next;
                }//end while

                margID=findMarginalNode(lineData,nodeList,nodeFlag,n);
        }//end while
        ///////////////////////////////////////////////////////////////////

        //4: conn. checking and report

        long flg=YES;//init.
        for(i=1;i<=n;i++)
        {
                if(nodeFlag[i]==OFFISLND)
                {
                        printf("\n\nIsolated Node: %ld",nodeList[i]);
                        flg=NO;
                }
        }//end for

	if(flg==NO)//unexpected
	{
		printf("\n\nError! --- %ld",__LINE__);
		printf("\n  Connectivity Check NOT Passed!");
	
		exit(0);
	}//end if  
	///////////////////////////////////////////////////////////////////

        free(nodeList);
        free(nodeFlag);

        return;
}
                            
void codingNode(long *codeList,long *codeAttr,long nodeNum,long prnt)
{
        if(prnt==YES) printf("\n\ncodingNode()...");

        register long i;
        long bs,attr;

        long *list,*_mtrx,**mtrx;
        /////////////////////////////////////////////////

        makeMatrix(_mtrx,mtrx,nodeNum+1,nodeNum+1);
        list=(long *)calloc(nodeNum+1,sizeof(long));

        if(list==NULL){ printf("\n\nError! --- %ld",__LINE__); exit(0); }
        ////////////////////////////////////////////////////////

        //1: mtrx[][] and codeList[]

        formMtrx(mtrx,list,nodeNum);//mtrx,list[] formed
        if(prnt==YES) printf("\n formMtrx() ok");

        nodeOptimize(codeList,list,mtrx,nodeNum);
        if(prnt==YES) printf("\n nodeOptimize() ok");
        ///////////////////////////////////////////////////////

        //2: forming codeAttr[]
 
        for(i=1;i<=nodeNum;i++)
        {
                bs=codeList[i];
                attr=attribute(bs,nodeData);
                
                codeAttr[i]=attr;
                if(attr==SLACK)
                {
                        codeList[0]=codeList[i]; //slack stored
                        codeAttr[0]=i;           //slack code
                }
        }//end for

        if(prnt==YES) printf("\n codeAttr[] formed");
        ////////////////////////////////////////////////

        free(list);
        freeMatrix(_mtrx,mtrx);

        return;
}

void vfyData(struct nodestr *nodeData)
{
	long flg=YES;//init.
	struct nodestr *np=nodeData;

	while(np!=NULL)
	{
		if(lookup(codeList,np->i,nodeNum)==NO)
		{
			printf("\n\nError! --- %ld",__LINE__); 
			printf("\n  Invalid nodeData: id=%s, ibs=%ld",np->id,np->i);

			flg=NO;//marked
		}//end if

		np=np->next;
	}//end while
	////////////////////////////////

	if(flg==NO){ printf("\n\nVerification NOT passed."); exit(0); }
	return;
}

void vfyData(struct voltstr *voltData)
{
 	long flg=YES;//init.
	double aux2;

        struct voltstr *vp;
	//////////////////////////////////////////

	vp=voltData;
	while(vp!=NULL)
	{
		//1: ibs validity

		if(lookup(codeList,vp->i,nodeNum)==NO)
		{
			printf("\n\nError! --- %ld",__LINE__);
                        printf("\n  Invalid volt: id=%s, ibs=%ld",vp->id,vp->i);

			flg=NO;//marked
		}//end if
		////////////////////////////////////////

		//2: Vmin and Vmax --- constraint

		if(vp->Vmin>vp->Vmax)
		{
			printf("\n\nError! --- %ld",__LINE__);
                        printf("\n  Invalid volt: id=%s, Vmin=%lf, Vmax=%lf",vp->id,vp->Vmin,vp->Vmax);

			flg=NO;//marked
                }//end if

		aux2=vp->Vmax-vp->Vmin;
		if(fabs(aux2)<SMLL) vp->Vmax=vp->Vmin+SMLL;

		vp=vp->next;
	}//end while
	///////////////////////////////////////
	
	if(flg==NO){ printf("\n\nVerification NOT passed."); exit(0); }
	return;
}
					 
void vfyData(struct capastr *capaData)
{
	long flg=YES;//init.

	struct capastr *cp;
	struct nodestr *np;
	/////////////////////////////////////////

	cp=capaData;
	while(cp!=NULL)
	{
		//1: ibs validity

		if(lookup(codeList,cp->i,nodeNum)==NO)
		{
			printf("\n\nError! --- %ld",__LINE__);
                        printf("\n  Invalid capaData: id=%s, ibs=%ld",cp->id,cp->i);

			flg=NO;//marked
	        }//end if
                //////////////////////////////////////////

		//2: reactive limits --- ctrl. bounds 

		if(cp->Qmin>=cp->Qmax-EPS)
		{
                        printf("\n\nError! --- %ld",__LINE__);
                        printf("\n  Invalid capaData: id=%s, ibs=%ld",cp->id,cp->i);
			printf("\n                    Qmin=%lf, Qmax=%lf",cp->Qmin,cp->Qmax);

			flg=NO;//marked
                }//end if
                /////////////////////////////////////////

		//3: node attr.

		np=findData(cp->i,nodeData,PVSLACK);
		if(np!=NULL)
		{
			printf("\n\nError! --- %ld",__LINE__);
			printf("\n  Invalid capa.data found at genr.node:");
		
			printf("\n  capa.id=%s, ibs=%ld",cp->id,cp->i);
			printf("\n  node.id=%s, ibs=%ld",np->id,np->i);

			flg=NO;//marked
		}//end if
		//////////////////////////////////////

		cp=cp->next;
	}//end while
	/////////////////////////////////////

	if(flg==NO){ printf("\n\nVerification NOT passed."); exit(0); }
	return;
}		
	
void vfyData(struct xfmrstr *xfmrData)
{
	long flg=YES;//init.
	double b0k,K0,K0_tap;

	struct linestr  *lp;
	struct taptystr *tp;
	struct xfmrstr  *xp;
	///////////////////////////////////////

	xp=xfmrData;
	while(xp!=NULL)
	{
		//1: ibs,zbs validity
		
		lp=findData(xp->i,xp->j,lineData);
		if(lp==NULL)
		{
                        printf("\n\nError! --- %ld",__LINE__);
			printf("\n  Invalid xfmrData: id=%s, ibs=%ld, zbs=%ld",xp->id,xp->i,xp->j);

			flg=NO;//marked
	        }//end if

		b0k=lp->b0k;//branch ratio
                /////////////////////////////////////////////////
                                                         
		//2: ratio limits --- ctrl. bounds

		if(intrvl(xp->Kmin,0.8,1.06)!=YES)
		{
			printf("\n\nWarning! --- %ld",__LINE__);
			printf("\n  Invalid xfmrData: id=%s, Kmin=%lf",xp->id,xp->Kmin);
		}

		if(intrvl(xp->Kmax,1.0,1.20)!=YES)
		{
			printf("\n\nWarning! --- %ld",__LINE__);
			printf("\n  Invalid xfmrData: id=%s, Kmax=%lf",xp->id,xp->Kmax);
		}

		if(xp->Kmin>=xp->Kmax-EPS)
		{
			printf("\n\nError! --- %ld",__LINE__);
                        printf("\n  Invalid xfmrData: id=%s, Kmin=%lf, Kmax=%lf",xp->id,xp->Kmin,xp->Kmax);

			flg=NO;//marked
                }//end if
                ///////////////////////////////////////////////////////////

		//3: original ratio(s)

		tp=chainNode(taptyData,xp->itapty);
		if(tp==NULL){ printf("\n\nError! --- %ld",__LINE__); exit(0); }

		K0_tap=ratioFun(xp,tp,xp->tap);
		K0    =xp->K0;

		if(fabs(K0_tap-K0)>EPS)
		{
			printf("\n\nWarning! --- %ld",__LINE__);
			printf("\n  id=%s: K0_tap=%lf, K0=%lf",xp->id,K0_tap,K0);
		}//end if

		if(fabs(K0_tap-b0k)>EPS)
		{
			printf("\n\nWarning! --- %ld",__LINE__);
			printf("\n  id=%s: K0_tap=%lf, b0k=%lf",xp->id,K0_tap,b0k);
		}//end if
	
		if(K0<xp->Kmin-EPS || K0>xp->Kmax+EPS)
		{
			printf("\n\nWarning! --- %ld",__LINE__);
			printf("\n  Invalid original ratio: id=%s, K0=%lf",xp->id,K0);
			printf("\n  Kmin=%lf, Kmax=%lf",xp->Kmin,xp->Kmax);
		}//end if
		///////////////////////////////////////////

		xp=xp->next;
	}//end while
	///////////////////////////////	
	
	if(flg==NO){ printf("\n\nVerification NOT passed."); exit(0); }
        return;
}

void vfyData(struct groupstr *groupData)
{
	register long i,j;
	long ibs,zbs,flg=NO;//init.

	struct groupstr *grp;
	struct linestr *lp;
	////////////////////////////////////

	grp=groupData;
	while(grp!=NULL)
	{
		for(i=1;i<=grp->lineNum;i++)
		{	
			ibs=grp->line[i].i;
			zbs=grp->line[i].j;

			//1: validity checkup

			lp=findData(ibs,zbs,lineData);
			if(lp==NULL)
			{
				grp->line[i].i=0;
				grp->line[i].j=0;

				continue;
			}//end if
			///////////////////////////////////////////
			
			//2: redundancy checkup

			if(i==grp->lineNum) continue;//skipped

			for(j=i+1;j<=grp->lineNum;j++)
			{
				if(brMatch(grp->line[j],ibs,zbs)==YES)
				{
					strcpy(idbuf,grp->line[j].id);

					printf("\n\nWarning! --- %ld",__LINE__);
					printf("\n  Redundant Group Line: id=%s",idbuf);
				
					flg=YES;//flg marked
				}//end if	
			}//end for
		}//end for
		/////////////////////////
	
		grp=grp->next;
	}//end while
	////////////////////////

	return;
}

void vfyData(struct genrstr *genrData)
{
	long k,flg=YES;//init.
	double aux2;

	struct genrstr *gp;
	struct nodestr *np;
	///////////////////////////////////////

	gp=genrData;
	while(gp!=NULL)
	{
		//1: ibs validity examination

		np=findData(gp->i,nodeData);
		if(np!=NULL) k=lookup(codeList,gp->i,nodeNum);
	
		if(np==NULL || k==NO)
		{
			printf("\n\nError! --- %ld",__LINE__);
			printf("\n  Invalid genrData: id=%s, ibs=%ld",gp->id,gp->i);

			flg=NO;//marked
		}//end if
		///////////////////////////////////////////

		//2: active power limits --- ctrl. bounds

		if(gp->wmin>gp->wmax)
		{
			printf("\n\nError! --- %ld",__LINE__);
			printf("\n  Invalid genrData: id=%s, ibs=%ld",gp->id,gp->i);
			printf("\n  wmin=%lf, wmax=%lf",gp->wmin,gp->wmax);

			flg=NO;//marked
		}//end if
		///////////////////////////////////////////////////

		//3: reactive limits --- constraint
	
		if(gp->rmin>gp->rmax)
		{
			printf("\n\nError! --- %ld",__LINE__);
			printf("\n  Invalid genrData: id=%s, ibs=%ld",gp->id,gp->i);
			printf("\n  Qmin=%lf, Qmax=%lf",gp->rmin,gp->rmax);

			flg=NO;//marked
		}//end if

		aux2=gp->rmax-gp->rmin;
		if(fabs(aux2)<SMLL) gp->rmax=gp->rmin+SMLL;
		///////////////////////////////////////////////

		//4: voltage limits --- ctrl. bounds

		if(gp->vmin>gp->vmax)
		{
			printf("\n\nError! --- %ld",__LINE__);
			printf("\n  Invalid genrData: id=%s, ibs=%ld",gp->id,gp->i);
			printf("\n  vmin=%lf, vmax=%lf",gp->vmin,gp->vmax);

			flg=NO;//marked
		}//end if
		///////////////////////////////////////////////////
		
		//5: attr. and power consistency 

		np=findData(gp->i,nodeData);//by ibs, not id
		if(np==NULL){ printf("\n\nError! --- %ld",__LINE__); exit(0); }

		if(np->attr!=PV && np->attr!=SLACK)
		{
			printf("\n\nError! --- %ld",__LINE__);
			printf("\n  Invalid genrData: id=%s, attr=%ld",gp->id,np->attr);
			
			flg=NO;//marked
		}//end if

		gp=gp->next;
	}//end while
	////////////////////////////////////////

	if(flg==NO){ printf("\n\nVerification NOT passed."); exit(0); }
	return;
}

void vfyData(struct pricestr *pricData)
{
	long flg=YES;//init.
	long genrDataLen,pricDataLen;

	struct genrstr *gp;
	struct pricestr *pp;
	//////////////////////////////////////////////

	//1: chain length examination

	genrDataLen=chainLen(genrData);
	pricDataLen=chainLen(pricData);

	if(pricDataLen==0) return;

	if(genrDataLen!=genrNum || pricDataLen!=genrNum)
	{
		printf("\n\nError! --- %ld",__LINE__);

		printf("\n  genrNum = %ld",genrNum);
		printf("\n  genrLen = %ld",genrDataLen);
		printf("\n  pricLen = %ld",pricDataLen);
	
		exit(0);
	}//end if
	///////////////////////////////////////////////////

	//2: price data examination

	gp=genrData;
        pp=pricData;

	while(pp!=NULL)
        {
        	//2.1 ibs consistency

	        if(gp->i!=pp->i)
                {
                        printf("\n\nError! --- %ld",__LINE__);
                        printf("\n  genrData: id=%s, ibs=%ld",gp->id,gp->i);
                        printf("\n  pricData: id=%s, ibs=%ld",pp->id,pp->i);

                	flg=NO;//marked
		}//end if
		////////////////////////////////////

		//2.2 power base

                if(pp->Pb<SMLL)
                {
                        printf("\n\nError! --- %ld",__LINE__);
                        printf("\n  Invalid pricData: id=%s, Pb=%lf",pp->id,pp->Pb);

			flg=NO;//marked
                }//end if
       		////////////////////////////////

		gp=gp->next;
		pp=pp->next;
	}//end while 
	///////////////////////////////////////////

        if(flg==NO){ printf("\n\nVerification NOT passed."); exit(0); }
	return;
}

//end of file
