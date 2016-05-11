//opf_initfun.cpp by Wang GT
/////////////////////////////////

#include"common.h"
#include"opf.h"
#include"opf_global.h"
////////////////////////////////////////////////////

long attribute(long node)
{
        long ret=SPQ;           //default: Special_PQ node
        struct nodestr *np;

        np=nodeData;
        while(np!=NULL)
        {
                if(abs(np->i)==abs(node))               //meet the node
                {
                        if(ret==SPQ) ret=np->attr;      //first meet
                        else//meet the node again
                        {
                                if(ret==PV || np->attr==PV)     ret=PV;

                                if(ret==SLK || np->attr==SLK) ret=SLK;
                        }
                }//end if

                np=np->next;
        }//end while

   return(ret);
}

long findMarginalNode(struct linestr *lineData,long *list,long *flag,long n)
{
        long ret=NO,ii,jj;
        struct linestr *lp;

        lp=lineData;
        while(lp!=NULL)
        {
                ii=lookup(list,lp->i,n);
                jj=lookup(list,lp->j,n);

                if(ii==NO || jj==NO){ printf("\nError! --- %ld",__LINE__); exit(0); }

                if(flag[ii]==ONISLND && flag[jj]==OFFILND)
                {
                        ret=abs(list[ii]);
                        break;
                }
                else if(flag[ii]==OFFILND && flag[jj]==ONISLND)
                {
                        ret=abs(list[jj]);
                        break;
                }

                lp=lp->next;
        }//end while
	///////////////////////////////////

        return ret;
}

void formMtrx(long **mtrx,long *list)
{
        register long i,j,k,ii,jj;
        struct linestr *lp;
        ////////////////////////////////////

	unitMatrix(mtrx,nodeNum);//init.

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

                mtrx[ii][jj]=1;
                mtrx[jj][ii]=1;

                lp=lp->next;
        }//end while

        return;
}

void nodeOptimize(long **mtrx,long *list,long *codeList)
{
        register long i,j,jj,k,kk;
        long size,aux1,aux3,aux5;

        long *nodeCod2,*nodeLin2;
        struct linestr *lp,*newLines=NULL;
        ////////////////////////////////////////////

        nodeCod2=(long *)calloc(nodeNum+1,sizeof(long));
        nodeLin2=(long *)calloc(nodeNum+1,sizeof(long));

        if(nodeLin2==NULL || nodeCod2==NULL)
        { printf("\nAllocation Error! --- %ld",__LINE__); exit(0); }
        /////////////////////////////////////////////////////////

        kk=1;
        size=nodeNum;
        for(i=1;i<=nodeNum;i++)
                nodeCod2[i]=list[i];
        //////////////////////////////////

        do
        {
                countLine(mtrx,nodeLin2,size);
                k=posiMin(nodeLin2,size);
                codeList[kk]=nodeCod2[k];

                for(j=1;j<=size;j++)      /* modify mtrxTemp[][] */
		{
                        if(mtrx[k][j]!=0)
                        for(jj=j+1;jj<=size;jj++)
                        {
                                if(mtrx[k][jj]!=0)
                                {
                                        mtrx[jj][j]=1;
                                        mtrx[j][jj]=1;
                                }
                        }
                }//end for
                ///////////////////////

                for(i=k;i<=size-1;i++)
                for(j=1;j<=size;j++)
                        mtrx[i][j]=mtrx[i+1][j];

                for(j=k;j<=size-1;j++)
                for(i=1;i<=size-1;i++)
                        mtrx[i][j]=mtrx[i][j+1];

                for(i=k;i<=size-1;i++)
                {
                        nodeCod2[i]=nodeCod2[i+1];
                        nodeLin2[i]=nodeLin2[i+1];
                }

                size--;
		
                for(i=1;i<=size;i++)
                for(j=1;j<i;j++)
                {
                        aux1=mtrx[i][j];
                        aux3=nodeCod2[i];
                        aux5=nodeCod2[j];

                        if(aux1&&isNewLine(aux3,aux5))
                        {
                                if((lp=(struct linestr *)malloc(sizeof(struct linestr)))==NULL)
                                { printf("\nError! --- %ld",__LINE__); exit(0); }

                                lp->i=aux3;
                                lp->j=aux5;
                                lp->next=newLines;
                                newLines=lp;
                        }
                }//end for

                kk++;
        }while(kk<=nodeNum);
        /////////////////////////////////

        free(nodeCod2);
        free(nodeLin2);

        lp=newLines;
        while(lp!=NULL)
        {
		newLines=lp->next;

                free(lp);
                lp=newLines;
        }//end while

        return;
}
                              
long isNewLine(long i,long j)
{
        long ret=YES;//init.
        struct linestr *lp;

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
	////////////////////

        return ret;
}

void countLine(long **mtrx,long *nodeLine,long size)
{
        register long i,j;

        for(i=1;i<=size;i++)
        {
                nodeLine[i]=0;
                for(j=1;j<=size;j++)
                        nodeLine[i]+= mtrx[i][j];

                nodeLine[i]--;
        }//end for
	//////////////////////

        return;
}

long interConnex(struct linestr *lineData)
{
        long ii,jj,margID;
        register long i,n;

        long *nodeList,*nodeFlag;
        struct linestr *lp;
        ////////////////////////////////////////////////

        nodeList=(long *)calloc(nodeNum+1,sizeof(long));
        nodeFlag=(long *)calloc(nodeNum+1,sizeof(long));

        if(nodeList==NULL || nodeFlag==NULL)
        { printf("\nError! --- %ld",__LINE__); exit(0); }
        ///////////////////////////////////////////////////

        //1: form nodeList[]

        n=0;
	lp=lineData;

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

        initVector(nodeFlag,n,(long)OFFILND);

        margID=nodeList[1];
        nodeFlag[1]=ONISLND;
        /////////////////////////////////////

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
        /////////////////////////////////////

        //4: checking and report

        long ret=YES;//init.
        for(i=1;i<=n;i++)
        {
                if(nodeFlag[i]==OFFILND)
                {
                        printf("\n\nIsolated Node: %ld",nodeList[i]);

                        ret=NO;
                        wpause();
                }
        }//end for
        ///////////////////////////

        free(nodeList);
        free(nodeFlag);
        ////////////////////////////

        return ret;
}
                            
void codingNode(long *codeList,long *codeAttr)
{
        long i,id,attr;
        long *list,*_mtrx,**mtrx;

        makeMatrix(_mtrx,mtrx,nodeNum+1,nodeNum+1);
        list=(long *)calloc(nodeNum+1,sizeof(long));

        if(list==NULL){ printf("\n Error! --- %ld",__LINE__); exit(0); }
        ////////////////////////////////////////////////////////

        formMtrx(mtrx,list);
        nodeOptimize(mtrx,list,codeList);
        //////////////////////////////////////////////

        for(i=1;i<=nodeNum;i++)//to form codeAttr[]
        {
                id=codeList[i];
                attr=attribute(id);
                codeAttr[i]=attr;
                //////////////////////
		
                if(attr==SLK)
                {
                        codeList[0]=codeList[i];        //slackID   stored
                        codeAttr[0]=i;              	//slackCode stored
                }
        }//end for
        ////////////////////////////////

        free(list);
        freeMatrix(_mtrx,mtrx);

        return;
}

void veriData(struct nodestr *nodeData)
{
	long flg=YES;//init.
	struct nodestr *np=nodeData;

	while(np!=NULL)
	{
		if(lookup(codeList,np->i,nodeNum)==NO)
		{
			printf("\n\nError! --- %ld",__LINE__); 
			printf("\n  Invalid node: id=%s, ibs=%ld",np->id,np->i);

			flg=NO;//marked
		}//end if

		np=np->next;
	}//end while
	////////////////////////////////

	if(flg==NO){ printf("\n\nVerification NOT passed."); exit(0); }
	return;
}

void veriData(struct voltstr *voltData)
{
 	long flg=YES;//init.
        struct voltstr *vp=voltData;

	while(vp!=NULL)
	{
		//1: ibs validity
		if(lookup(codeList,vp->i,nodeNum)==NO)
		{
			printf("\n\nError! --- %ld",__LINE__);
                        printf("\n  Invalid volt: id=%s, ibs=%ld",vp->id,vp->i);

			flg=NO;//marked
		}//end if
		/////////////////////////////////

		//2: Vmin and Vmax

		if(vp->Vmin>vp->Vmax)
		{
			printf("\n\nError! --- %ld",__LINE__);
                        printf("\n  Invalid volt: id=%s, Vmin=%lf, Vmax=%lf",vp->id,vp->Vmin,vp->Vmax);

			flg=NO;//marked
                }//end if
                /////////////////////////////////

		vp=vp->next;
	}//end while
	///////////////////////////////////////
	
	if(flg==NO){ printf("\n\nVerification NOT passed."); exit(0); }
	return;
}
					 
void veriData(struct capastr *capaData)
{
	long flg=YES;//init.
	struct capastr *cp=capaData;

	while(cp!=NULL)
	{
		//1: ibs validity

		if(lookup(codeList,cp->i,nodeNum)==NO)
		{
			printf("\n\nError! --- %ld",__LINE__);
                        printf("\n  Invalid capa: id=%s, ibs=%ld",cp->id,cp->i);

			flg=NO;//marked
                }//end if
                /////////////////////////////

		//2: Qmin and Qmax

		if(cp->Qmin>cp->Qmax)
		{
                        printf("\n\nError! --- %ld",__LINE__);
                        printf("\n  Invalid capa: id=%s, Qmin=%lf, Qmax=%lf",cp->id,cp->Qmin,cp->Qmax);

			flg=NO;//marked
                }//end if
                /////////////////////////////

		cp=cp->next;
	}//end while
	/////////////////////////////////////

	if(flg==NO){ printf("\n\nVerification NOT passed."); exit(0); }
	return;
}		
	
void veriData(struct xfmrstr *xfmrData)
{
	long flg=YES;//init.
	struct xfmrstr *xp=xfmrData;
        
	while(xp!=NULL)
	{
		//1: ibs,zbs validity
		
		if(findData(xp->i,xp->j,lineData)==NULL)
		{
                        printf("\n\nError! --- %ld",__LINE__);
			printf("\n  Invalid xfmr: id=%s, ibs=%ld, zbs=%ld",xp->id,xp->i,xp->j);

			flg=NO;//marked
                }//end if
                /////////////////////////////
                                                         
		//2: Kmin and Kmax
	
		if(xp->Kmin>xp->Kmax)
		{
			printf("\n\nError! --- %ld",__LINE__);
                        printf("\n  Invalid xfmr: id=%s, Kmin=%lf, Kmax=%lf",xp->id,xp->Kmin,xp->Kmax);

			flg=NO;//marked
                }//end if
                /////////////////////////////

		xp=xp->next;
	}//end while
	////////////////////////////////////////	
	
	if(flg==NO){ printf("\n\nVerification NOT passed."); exit(0); }
        return;
}

void veriData(struct groupstr *groupData)
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

void veriData(struct genrstr *genrData)
{
	long k,flg=YES;//init.
	double aux2;

	struct genrstr *gp;
	struct nodestr *np;
	///////////////////////////////////////

	gp=genrData;
	while(gp!=NULL)
	{
		//1: id/ibs validity examination

		np=findData(gp->i,nodeData);
		//np=findData(gp->id,nodeData);

		if(np==NULL)
		{
			printf("\n\nError! --- %ld",__LINE__);
			printf("\n  Invalid genrData: id=%s",gp->id);

			flg=NO;//marked
			continue;
		}//end if
		/////////////////////////////////////

		k=lookup(codeList,gp->i,nodeNum);
		if(np->i!=gp->i || k==NO) 
		{
			printf("\n\nError! --- %ld",__LINE__);
			printf("\n  Invalid genrData: id=%s, ibs=%ld %ld",gp->id,gp->i,np->i);

			flg=NO;//marked
		}//end if
		/////////////////////////////////

		//2: Pmin and Pmax validity

		if(gp->Pmin>gp->Pmax)
		{
			printf("\n\nError! --- %ld",__LINE__);
			printf("\n  Invalid genrData: id=%s, Pmin=%lf, Pmax=%lf",gp->id,gp->Pmin,gp->Pmax);

			flg=NO;//marked
		}//end if
		///////////////////////////////////////////

		//3: attr and power consistency 

		np=findData(gp->i,nodeData);
		//np=findData(gp->id,nodeData);

		if(np->attr!=PV && np->attr!=SLCK)
		{
			printf("\n\nError! --- %ld",__LINE__);
			printf("\n  Invalid genrData: id=%s, ibs=%ld, attr=%ld",gp->id,gp->i,np->attr);
			
			flg=NO;//marked
		}//end if
	
		aux2=gp->P0+gp->Pc;
		if(fabs(aux2)>SMLL && np->attr==PV && fabs(np->p-aux2)>SMLL)
		{ 
			printf("\n\nError! --- %ld",__LINE__);
			printf("\n  Wrong genrData: id=%s, P0=%7.4lf Pc=%7.4lf",gp->id,gp->P0,gp->Pc);
		
			flg=NO;//marked
		}//end if
		/////////////////////////////

		gp=gp->next;
	}//end while
	////////////////////////////////////////

	if(flg==NO){ printf("\n\nVerification NOT passed."); exit(0); }
	return;
}

void veriData(struct pricstr *pricData)
{
	//1: chain length examination

	long genrDataLen=chainLen(genrData);
	long pricDataLen=chainLen(pricData);

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

        register long i;

	struct genrstr *gp=genrData;
        struct pricstr *pp=pricData;

        for(i=1;i<=genrNum;i++,gp=gp->next,pp=pp->next)
        {
                if(gp->i!=pp->i)
                {
                        printf("\n\nError! --- %ld",__LINE__);
                        printf("\n  genr: id=%s, ibs=%ld",gp->id,gp->i);
                        printf("\n  pric: id=%s, ibs=%ld",pp->id,pp->i);

                        exit(0);
                }//end if

                if(pp->Pb<SMLL)
                {
                        printf("\n\nError! --- %ld",__LINE__);
                        printf("\n  pric: id=%s, Pb=%lf",pp->id,pp->Pb);

			exit(0);
                }//end if
       	}//end for
	///////////////////////////////////////////

        return;
}

//end of file
