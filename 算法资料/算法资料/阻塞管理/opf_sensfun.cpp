//opf_sensfun.cpp by Wang GT
//////////////////////////////////

#include"common.h"
#include"opf.h"
#include"opf_global.h"
///////////////////////////////////////////////////////////////////////////

void sens_GRPG(double **GRPG,double **PBPG,long groupNum,long genrNum)
{
	register long i,j,k;

	long ii,imst,ibs,zbs;
	double sum_sens;

	struct groupstr *grp;
	struct linestr *lp;
	///////////////////////////////////////////////

	initMatrix(GRPG,groupNum,genrNum,0.0);
	grp=groupData;

	for(i=1;i<=groupNum;i++,grp=grp->next)
	{
		for(j=1;j<=genrNum;j++)
		{
			sum_sens=0.0;//init.
			for(k=1;k<=grp->lineNum;k++)
			{
				ibs=grp->line[k].i;
				zbs=grp->line[k].j;

				lp=findData(ibs,zbs,lineData);
				if(lp==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }

				ii=posiChainNode(lineData,lp);
				imst=grp->line[k].imst;
				
				if(imst==abs(lp->i)) sum_sens+= PBPG[ii][j];
				if(imst==abs(lp->j)) sum_sens-= PBPG[ii][j];
			}//end for
			
			GRPG[i][j]=sum_sens;
		}//end for
	}//end for
	//////////////////////////////////////

	return;
}
	
double aPij_KT(struct xfmrstr *xp,double *vv,double *v0,long swch)
{
	long ii,jj;
	double K,K2,K3,brG,brB,theta,ret;

	struct linestr *lp;
	//////////////////////////////////////

	ii=lookup(codeList,xp->i,nodeNum);
	jj=lookup(codeList,xp->j,nodeNum);
	if(ii==NO || jj==NO){ printf("\n\nError! --- %ld",__LINE__); exit(0); }
	
	lp=findData(xp->i,xp->j,lineData);
	if(lp==NULL){ printf("\n\nError! --- %ld",__LINE__); exit(0); }
	//////////////////////////////////////////////////

	getBranchGB(lp,brG,brB);
	K=lp->kb;
	
	if(lp->j<0)//i_side <== K* at j_side
	{
		brG/= K*K;
		brB/= K*K;
		
		K =1.0/K;
	}//end if
	//////////////////////

	K2=K*K;
	K3=K2*K;
	theta=v0[ii]-v0[jj];
	////////////////////////////////////////////

	if(swch==POSI)// --- aPij/aK
	{
		ret=-2.0*vv[ii]*vv[ii]*brG/K3;
		ret+= vv[ii]*vv[jj]*(brG*cos(theta)+brB*sin(theta))/K2;
	}//end if

	if(swch==NEGA)// --- aPji/aK
	{
		ret=vv[ii]*vv[jj]*(brG*cos(theta)-brB*sin(theta))/K2;
	}//end if
	///////////////////////////////////////////

	return ret;
}
			   
double aQij_KT(struct xfmrstr *xp,double *vv,double *v0,long swch)
{
	long ii,jj;
	double K,K2,K3,brG,brB,theta,ret;

	struct linestr *lp;
	//////////////////////////////////////

	ii=lookup(codeList,xp->i,nodeNum);
	jj=lookup(codeList,xp->j,nodeNum);
	if(ii==NO || jj==NO){ printf("\n\nError! --- %ld",__LINE__); exit(0); }
	
	lp=findData(xp->i,xp->j,lineData);
	if(lp==NULL){ printf("\n\nError! --- %ld",__LINE__); exit(0); }
	//////////////////////////////////////////////////

	getBranchGB(lp,brG,brB);
	K=lp->kb;

	if(lp->j<0)//i_side <== K* at j_side
	{
		brG/= K*K;
		brB/= K*K;
		
		K =1.0/K;
	}//end if
	//////////////////////

	K2=K*K;
	K3=K2*K;
	theta=v0[ii]-v0[jj];
	////////////////////////////////////////////
				
	if(swch==POSI)// --- aQij/aK
	{
		ret=2.0*vv[ii]*vv[ii]*brB/K3;
		ret+= vv[ii]*vv[jj]*(brG*sin(theta)-brB*cos(theta))/K2;
	}//end if

	if(swch==NEGA)// --- aQji/aK
	{
		ret=-vv[ii]*vv[jj]*(brG*sin(theta)+brB*cos(theta))/K2;
	}//end if
	////////////////////////////////////////////

	return ret;
}

void sens_QGKT(double **QGKT,double *vv,double *v0)
{
	register long i,j;
	
	struct genrstr *gp;
	struct xfmrstr *xp;
	//////////////////////////////

	initMatrix(QGKT,genrNum,xfmrNum,0.0);
	gp=genrData;

	for(i=1;i<=genrNum;i++,gp=gp->next)
	{
		xp=xfmrData;
		for(j=1;j<=xfmrNum;j++,xp=xp->next)
		{
			if(xp->i==gp->i) QGKT[i][j]=aQij_KT(xp,vv,v0,POSI);
			if(xp->j==gp->i) QGKT[i][j]=aQij_KT(xp,vv,v0,NEGA);
		}//end for  
	}//end for
	///////////////////////////
	
	return;
}

void sens_QGVG(double **QGVG,double **G,double **B,double *vv,double *v0)
{
	register long i,j;
	long ii,jj;

	double Qi,theta;
	struct genrstr *gp,*gp2;
	//////////////////////////////////

	initMatrix(QGVG,genrNum,genrNum,0.0);
	gp=genrData;

	for(i=1;i<=genrNum;i++,gp=gp->next)
	{
		gp2=genrData;//for genr.voltage
		for(j=1;j<=genrNum;j++,gp2=gp2->next)
		{
			ii=lookup(codeList,gp->i,nodeNum);
			jj=lookup(codeList,gp2->i,nodeNum);

            if(ii!=jj)
            {
				theta=v0[ii]-v0[jj];
				QGVG[i][j]=vv[ii]*(G[ii][jj]*sin(theta)-B[ii][jj]*cos(theta));
            }
            else
            {
				Qi=calQi(G,B,vv,v0,ii);
				QGVG[i][j]=-1.0*(vv[ii]*vv[ii]*B[ii][ii]-Qi)/vv[ii];
            }
         }//end for
	}//end for
	/////////////////////

	return;
}

void sens_QGQC(double **QGQC,double **QBQC)
{
	register long i,j,k;
	
	struct genrstr *gp;
	struct linestr *lp;
	///////////////////////////

	initMatrix(QGQC,genrNum,capaNum,0.0);
	gp=genrData;

	for(i=1;i<=genrNum;i++,gp=gp->next)
	{
		lp=lineData;
		for(k=1;k<=lineNum;k++,lp=lp->next)
		{
			if(abs(lp->i)==gp->i)
			{
				for(j=1;j<=capaNum;j++)
					QGQC[i][j]+= QBQC[k][j];
			}//end if

			if(abs(lp->j)==gp->i)
			{
				for(j=1;j<=capaNum;j++)
					QGQC[i][j]-= QBQC[k][j];
			}//end if
		}//end for
	}//end for
	////////////////////////

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

void HQfun(double **HQ,long *tempList,long n)
{
        long i,ii,jj,i2,j2;
        double Bij,Bi0,theta,K,K2,aux2;

        struct linestr *lp;
        ///////////////////////////////

        initMatrix(HQ,lineNum,n,0.0);
        lp=lineData;
        
	for(i=1;i<=lineNum;i++,lp=lp->next)
        {
                ii=lookup(tempList,lp->i,n);
                jj=lookup(tempList,lp->j,n);

                i2=lookup(codeList,lp->i,nodeNum);
                j2=lookup(codeList,lp->j,nodeNum);
                if(i2==NO || j2==NO){ printf("\nError! --- %ld",__LINE__); exit(0); }
                //////////////////////////////////////////////

                Bij=B[i2][j2];
                theta=v0[i2]-v0[j2];

                if(lp->i>0 && lp->j>0) Bi0=lp->kb;
                else//for xfmr.branch
                {
                        aux2=lp->r*lp->r + lp->x*lp->x;
                        K=lp->kb;
			K2=K*K;
			
                        if(lp->i<0) 
				Bi0=-lp->x*(K-1.0)/K/aux2;
                        else 
				Bi0=-lp->x*(1.0-K)/K2/aux2;
                }//end if-else

                if(ii!=NO) HQ[i][ii]=2.0*vv[i2]*(Bij-Bi0)-vv[j2]*Bij*cos(theta);
                if(jj!=NO) HQ[i][jj]=-vv[i2]*Bij*cos(theta);
        }//end for
        ////////////////////////////

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

void sens_QBVG(double **QBVG,double *vv,double *v0,long *codeList)
{
        register long i,j;
		long ii,jj;
        double theta,B0,aux2,aux4;

        struct genrstr *gp;
        struct linestr *lp;
        //////////////////////////////////////

		initMatrix(QBVG,lineNum,genrNum,0.0);
        lp=lineData;

        for(i=1;i<=lineNum;i++,lp=lp->next)
        {
                ii=lookup(codeList,lp->i,nodeNum);
                jj=lookup(codeList,lp->j,nodeNum);

                if(ii==NO || jj==NO){ printf("\nError! --- %ld",__LINE__); exit(0); }
                /////////////////////////////////////////////

                theta=v0[ii]-v0[jj];

				if(lp->i>0 && lp->j>0) B0=lp->kb;
				else B0=0.0;//xfmr.
				////////////////////////////

                gp=genrData;
                for(j=1;j<=genrNum;j++,gp=gp->next)
                {
					if(gp->i!=abs(lp->i) && gp->i!=abs(lp->j)) continue;
					
					if(gp->i==abs(lp->i))
					{
						aux2=vv[jj]*(G[ii][jj]*sin(theta)-B[ii][jj]*cos(theta));
						aux4=2.0*vv[ii]*(B[ii][jj]-B0);
					}//end if

					if(gp->i==abs(lp->j))
					{
						aux2=vv[ii]*(G[ii][jj]*sin(theta)-B[ii][jj]*cos(theta));
						aux4=0.0;
					}//end if
					
					QBVG[i][j]=aux2+aux4;                                
                }//end for
        }//end for
		/////////////////////////

        return;
}
                                       
void sens_VDXX(double **VDKT,double **VDVG,double **VDQC,double **QGVG,double *vv,double *v0,long prnt)
{
//NOTATION://///////////////////////////////////////////////////////
//	VDKT[][]	--- volt.sens vs xfmr.ratio, voltNum * xfmrNum
//	VDVG[][]	--- volt.sens vs genr.volt, voltNum * genrNum
//	VDQC[][]	--- volt.sens vs capa, voltNum * capaNum dimension
//	QGVG[][]	--- input sens, genr.reactive vs genr.voltage
/////////////////////////////////////////////////////////////////////

	if(prnt==YES) printf("\n\nsens_VDXX()...");

	register long i,j,k;
    long ii,jj,it,jt,n=nodeNum-1;//No Change!
	double sum,aux2,aux4;

    long *tempList;
    double *_B2s,**B2s;
	
	struct capastr *cp;
	struct genrstr *gp;
	struct voltstr *vp;
	struct xfmrstr *xp;
	/////////////////////////////////
    
	makeMatrix(_B2s,B2s,n+1,n+1);
    tempList=(long *)calloc(n+1,sizeof(long));
	
    if(tempList==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }
	///////////////////////////////////////////////

	//1: form B2s[][] and inversing
    
	formTempList(tempList,codeList,codeAttr,SKIP_SLCK);
	if(prnt==YES) printf("\n formTempList() ok");

    formB2X(B2s,tempList,n);
    invMatrixGJ(B2s,n);//dV=B2'dQ
	if(prnt==YES) printf("\n inversed B2s[][] formed");
    ///////////////////////////////////////////////

	//2: cal. VDKT[][], VDVG[][] and VDQC[][]
	
	initMatrix(VDKT,voltNum,xfmrNum,0.0);
	initMatrix(VDVG,voltNum,genrNum,0.0);
    initMatrix(VDQC,voltNum,capaNum,0.0);

    vp=voltData;//init.
	for(i=1;i<=voltNum;i++,vp=vp->next)
    {
		ii=lookup(tempList,vp->i,n);
		if(ii==NO) continue;
		///////////////////////////////////

		//2.1 VDKT[][]

		xp=xfmrData;
		for(j=1;j<=xfmrNum;j++,xp=xp->next)
		{
			it=lookup(tempList,xp->i,n);
			jt=lookup(tempList,xp->j,n);
			//////////////////////////////////

			aux2=0.0;//init.
			aux4=0.0;

			if(it!=NO) aux2=-B2s[ii][it]*aQij_KT(xp,vv,v0,POSI);
			if(jt!=NO) aux4=-B2s[ii][jt]*aQij_KT(xp,vv,v0,NEGA);

			VDKT[i][j]=aux2+aux4;
		}//end for
		/////////////////////////////////
		
		//2.2 VDVG[][]

		gp=genrData;
		for(j=1;j<=genrNum;j++,gp=gp->next)
		{
			jj=lookup(tempList,gp->i,n);
			if(jj==NO) continue;

			sum=0.0;
			for(k=1;k<=genrNum;k++)
				sum+= B2s[ii][k]*QGVG[k][j];//No Change!

			VDVG[i][j]=sum/10.0;//thumb rule
		}//end for
		/////////////////////////////

		//2.3 VDQC[][]

        cp=capaData;
        for(j=1;j<=capaNum;j++,cp=cp->next)
        {
			jj=lookup(tempList,cp->i,n);
			if(jj==NO) continue;

			VDQC[i][j]=B2s[ii][jj]/10.0;//thumb rule
		}//end for
    }//end for
    //////////////////////////////////

    freeMatrix(_B2s,B2s);
    free(tempList);

    return;
}

void sens_QBKT(double **QBKT,double *vv,double *v0,long *codeList)
{
        register long i,j,ii,jj;
        double theta,brG,brB,K,K2,K3,aux2,aux4;

        struct linestr *lp;
        struct xfmrstr *xp;
		/////////////////////////////////

		initMatrix(QBKT,lineNum,xfmrNum,0.0);
		lp=lineData;

        for(i=1;i<=lineNum;i++,lp=lp->next)
        {
		xp=findData(lp->i,lp->j,xfmrData);
		if(xp==NULL) continue;
		/////////////////////////////////////////

                ii=lookup(codeList,lp->i,nodeNum);
                jj=lookup(codeList,lp->j,nodeNum);

                if(ii==NO || jj==NO){ printf("\nError! --- %ld",__LINE__); exit(0); }
		/////////////////////////////////////////////////////

		getBranchGB(lp,brG,brB);
		K=lp->kb;

		if(lp->j<0)//i_side <== K* at j_side
		{
			brG/= K*K;
			brB/= K*K;

			K=1.0/K;
		}//end if
		/////////////////////////
		
		K2=K*K;
		K3=K2*K;

		theta=v0[ii]-v0[jj];
		////////////////////////////////

		j=posiChainNode(xfmrData,xp);
		if(j==NO){ printf("\nError! --- %ld",__LINE__); exit(0); }
				
		aux2=2.0*vv[ii]*vv[ii]*brB/K3;
		aux4=vv[ii]*vv[jj]*(brG*sin(theta)-brB*cos(theta))/K2;

		QBKT[i][j]=aux2+aux4;				
        }//end for
        ////////////////////////////

        return;
}

void sens_QBQC(double **QBQC,double **DQ,long *tempList,long n)
//QBQC[][] <== DQ[][]
{
        register long i,j,jj;
        struct capastr *cp;

        for(i=1;i<=lineNum;i++)
        {
                cp=capaData;
                for(j=1;j<=capaNum;j++,cp=cp->next)
                {
                        jj=lookup(tempList,cp->i,n);
                        if(jj==NO){ printf("\nError! --- %ld",__LINE__); exit(0); }

                        QBQC[i][j]=DQ[i][jj];
                }//end for
        }//end for
        ///////////////////////

        return;
}

void sens_PLVG(double *PLVG,double **J2,long *codeList)
{
//NOTATION:///////////////////////////////////////////
//      PLVG[]          --- loss sens. vs generator voltage
//      J2[][]          --- transposed Jacobi matrix
//      codeList[]      --- full nodelist
//////////////////////////////////////////////////////

        register long i,j,ii;
        double sum;

        struct genrstr *gp=genrData;
        for(i=1;i<=genrNum;i++,gp=gp->next)
        {
                ii=lookup(codeList,gp->i,nodeNum);
                if(ii==NO){ printf("\nError! --- %ld",__LINE__); exit(0); }
                //////////////////////////////////////////

                sum=0.0;
                for(j=1;j<=nodeNum;j++)
                        sum+= J2[nodeNum+ii][j];

                PLVG[i]=sum;
        }//end for
        ////////////////////////

        return;
}

void sens_PLPQ(double *PLPQ,double **J2,long slack,long prnt)
{
//NOTATION: This is a very important function! //////////////////
//      PLPQ[]  --- loss sens. vs nodal active/reactive injection,
//                              2*nodeNum dimension with slack included
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

	copyMatrix(J2,J2s,2*nodeNum,2*nodeNum);//restored
	if(prnt==YES) printf("\n J2[][] restored");

	freeMatrix(_J2s,J2s);
        free(PL0V);
	///////////////////

        return;
}

void sens_PLQC(double *PLQC,double *PLPQ,long *codeList)
{
//NOTATION://////////////////////////////////////////////
//      PLQC[]  --- loss sens. vs capacitor reactive
//      PLPQ[]  --- loss sens. vs nodal w/r injections,
//                              2*nodeNum dimension with slack inlcuded
//      codeList--- full nodelist
/////////////////////////////////////////////////////////

        register long i,ii;
        struct capastr *cp;
        ///////////////////////////////////

        cp=capaData;
        for(i=1;i<=capaNum;i++,cp=cp->next)
        {
                ii=lookup(codeList,cp->i,nodeNum);
                if(ii==NO){ printf("\nError! --- %ld",__LINE__); exit(0); }
                ////////////////////////////////////////////////

                PLQC[i]=PLPQ[nodeNum+ii];
        }//end for

        return;
}

void sens_PLKT(double *PLKT,double *PLPQ,double *vv,double *v0,long *codeList)
{
        register long i,ii,jj;
        double PijK,QijK,PjiK,QjiK,aux2,aux4;

        struct xfmrstr *xp=xfmrData;
        for(i=1;i<=xfmrNum;i++,xp=xp->next)
        {
                ii=lookup(codeList,xp->i,nodeNum);
                jj=lookup(codeList,xp->j,nodeNum);

                if(ii==NO || jj==NO){ printf("\nError! --- %ld",__LINE__); exit(0); }
                ////////////////////////////////////////////////////
                
                PijK=aPij_KT(xp,vv,v0,POSI);
				QijK=aQij_KT(xp,vv,v0,POSI);

				PjiK=aPij_KT(xp,vv,v0,NEGA);
				QjiK=aQij_KT(xp,vv,v0,NEGA);
                ////////////////////////////////////////

                aux2=PLPQ[ii]*PijK+PLPQ[nodeNum+ii]*QijK;
                aux4=PLPQ[jj]*PjiK+PLPQ[nodeNum+jj]*QjiK;

                PLKT[i]=-(aux2+aux4);//No Change!
        }//end for
        ///////////////////////////////

        return;
}
                
void DPfun(double **DP,long *tempList,long n,long prnt)
{
//NOTATION:///////////////////////////////////////////////////////////////
//      DP[][]          --- branch power vs nodal injection, lineNum * n dimension
//      tempList[]      --- node list only without slack, n dimension
//      n               --- dim.of tempList[], should = node-1
///////////////////////////////////////////////////////////////////////

	if(prnt==YES) printf("\n\nDPfun()...");

        double *_HP,**HP;
        double *_B1s,**B1s;//local B1[][] for sens.

        makeMatrix(_HP,HP,lineNum+1,n+1);
        makeMatrix(_B1s,B1s,nodeNum,nodeNum);
        ////////////////////////////////////////////

        HPfun(HP,tempList,n);
	if(prnt==YES) printf("\n HP[][] formed");

        formB1(B1s,tempList,n);
	if(prnt==YES) printf("\n B1s[][] formed");
	/////////////////////////////////////////////

        invMatrixGJ(B1s,n);
        multiMatrix(DP,HP,B1s,lineNum,n,n);

		if(prnt==YES) printf("\n DP[][] formed");
       	///////////////////////////////////////////////

        freeMatrix(_HP,HP);
        freeMatrix(_B1s,B1s);
	///////////////////////

        return;
}

void DQfun(double **DQ,long *tempList,long n,long prnt)
{
//NOTATION:///////////////////////////////////////////////////////
//      DQ[][]          --- lineNum*n dimension
//      tempList[]      --- nodelist without generators, n dimension
//      n		--- nodeNum-pvNodeNum-1, all genr.excluded
//////////////////////////////////////////////////////////////////

	if(prnt==YES) printf("\n\nDQfun()...");

        double *_HQ,**HQ;
        double *_B2s,**B2s;//local B2[][] for sens.

        makeMatrix(_HQ,HQ,lineNum+1,n+1);
        makeMatrix(_B2s,B2s,n+1,n+1);
	//////////////////////////////////////////

        HQfun(HQ,tempList,n);   //HQ[][] obtained
	if(prnt==YES) printf("\n HQ[][] obtained");

        formB2X(B2s,tempList,n);
 	if(prnt==YES) printf("\n B2s[][] formed");       
	//////////////////////////////////////////

	invMatrixGJ(B2s,n);
	multiMatrix(DQ,HQ,B2s,lineNum,n,n);//DQ=HQ*B2[][]

	if(prnt==YES) printf("\n DQ[][] formed");
	///////////////////////////////////////////////////

        freeMatrix(_HQ,HQ);
        freeMatrix(_B2s,B2s);
	//////////////////////

        return;
}

void sens_PBPG(double **PBPG,double **DP,long *tempList,long n)
{
        register long i,j,jj;
        struct genrstr *gp;
        //////////////////////////

        for(i=1;i<=lineNum;i++)
        {
                gp=genrData;
                for(j=1;j<=genrNum;j++,gp=gp->next)
                {
                        jj=lookup(tempList,gp->i,n);

                        if(jj==NO)
                                PBPG[i][j]=0.0;//for slack
                        else
                                PBPG[i][j]=DP[i][jj];
                }//end for
        }//end for
        //////////////////////////

        return;
}

//end of file
