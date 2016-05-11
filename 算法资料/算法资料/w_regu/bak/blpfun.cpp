//blp.cpp by Wang GT

#include"common.h"
#include"blp.h"
////////////////////////////////////////////////////////

void blp_global_alloc(long m,long n)
{
	long nmax=n+m+m;//No Change!
	makeMatrix(_a,a,m+1,nmax+1);

	b=(double *)calloc(m+1,sizeof(double));
	c=(double *)calloc(nmax+1,sizeof(double));

	x=(double *)calloc(nmax+1,sizeof(double));
	L=(double *)calloc(nmax+1,sizeof(double));
	U=(double *)calloc(nmax+1,sizeof(double));

	Lm=(double *)calloc(nmax+1,sizeof(double));
	if(Lm==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }

	return;
}

void blp_global_free()
{
		freeMatrix(_a,a);
		free(b);
		free(c);
	
		free(L);
		free(U);
		free(Lm);
		
		return;		
}

long blpfun(double *x,double **a,double *b,double *c,double *L,double *U,long m,long n,long nEQ,long prnt)
{
//Notes: by Wang GT, 2003.07.08 //////////////////////////////
// x[]   --- solution returned, nmax size [nmax = n+2*m]
// a[][] --- coeff. matrix, m * nmax size
// b[]   --- r.h.s vector, m size
// c[]   --- obj.function coeff, nmax size
// L[]   --- lower bound, nmax size
// U[]   --- upper bound, nmax size
// m     --- num.of constraints
// n     --- num.of variables
// nEQ   --- num.of equations
// prnt  --- YES or NO
// ret.= NOSOLU --- no solution
//       OPTIML --- optimal solution
//       UNBNDD --- unbounded
///////////////////////////////////////////////////////////////
	
	//1: local definitions and allocation

	if(prnt==YES) printf("\n\nblpfun()...");
	
	long ret,ret2;
	long n_sv;
	
	double *_a_sv,**a_sv;
	makeMatrix(_a_sv,a_sv,m+1,n+1);

	double *b_sv=(double *)calloc(m+1,sizeof(double));
	if(b_sv==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }

	copyMatrix(a_sv,a,m,n);
	copyVector(b_sv,b,m);

	n_sv=n;//original col.dimension
	//////////////////////////////////////////////

	//2: standardizing the problem

	standard(a,b,c,L,U,m,n,nEQ,NO);//prnt
	if(prnt==YES) printf("\n standard() ok");
	/////////////////////////////////////////////////

	//3: removing lower bounds

	mapping(Lm,a,b,c,L,U,m,n);
	if(prnt==YES) printf("\n mapping() ok");
	///////////////////////////////////////////////

	//4: calling blp_2phase()

	ret=blp_2phase(x,a,b,c,L,U,m,n,PHSTWO,NO);//prnt
	if(prnt==YES) printf("\n blp_2phase() returned: %ld",ret);

	//4.2 verifying the solution

	ret2=vfy_solution(x,a,b,c,L,U,m,n,NO);//prnt
	if(prnt==YES) printf("\n vfy_solution() ok");

	if(ret2!=YES) ret=NOSOLU;
	/////////////////////////////////////////////////

	//5: recovery and unmapping

	n=n_sv;//dim. recovered

	copyMatrix(a,a_sv,m,n);
	copyVector(b,b_sv,m);

	unmapping(x,L,U,Lm,n);	
	if(prnt==YES) printf("\n unmapping() ok");
	/////////////////////////////////////////////////

	//6: free memory

	freeMatrix(_a_sv,a_sv);
	free(b_sv);
	////////////////////////////////////

	return ret;
}

long blp_2phase(double *x,double **a,double *b,double *c,double *L,double *U,long m,long n,long phs,long prnt)
{
//Notes: by Wang GT, 2003.07.08///////////////////////////////
//  x[]   --- solution vector
//  a[][] --- coeff. matrix
//  b[]   --- r.h.s vector, non-negative
//  c[]   --- obj.function
//  L[]   --- lower bound
//  U[]   --- upper boundtions
//  m     --- num.of constraints which are all equations
//  n     --- num.of variables
//  phs   --- phase option: PHSONE or PHSTWO
//  prnt  --- YES or NO
//  ret.= NOSOLU --- no solution
//        OPTIML --- optimal solution
//        UNBNDD --- unbounded
//////////////////////////////////////////////////////////////

	if(prnt==YES) printf("\n\nblp_2phase()...");

	//1: locals and memory allocation

	register long i,j;

	long nonFlg,nN,nN2,uvNum,arNum;
	long ret=NOSOLU;//init.

	double min_g;
	////////////////////////////////////////////////

	long *uvIndex;//index to unit vectors
	long *arIndex;//index to artif.
	long *ixb;    //index to basic variables
	long *ixn;    //index to lower non-basic
	long *ixn2;   //index to upper non-basci 

	double *RHS;  //right-hand-side vector
	double *c2;   //aux. obj.function
	double *_spxTBL,**spxTBL;
	///////////////////////////////////////////////

	arIndex=(long *)calloc(m+1,sizeof(long));
	uvIndex=(long *)calloc(n+1,sizeof(long));

	ixb =(long *)calloc(m+1,sizeof(long));
	ixn =(long *)calloc(n+1,sizeof(long));
	ixn2=(long *)calloc(n+1,sizeof(long));

	RHS=(double *)calloc(m+1,sizeof(double));
	if(RHS==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }
	////////////////////////////////////////////////////

	//2: upper/lower non-basic

	nN =0;//init.
	nN2=0;

	for(j=1;j<=n;j++)
	{
		nonFlg=LOWER;//init.
		if(c[j]<-SMLL && isLarge(U[j])!=YES) nonFlg=UPPER;

		if(nonFlg==LOWER)
		{
			nN++; 
			ixn[nN]=j;
		}//end if

		if(nonFlg==UPPER)
		{
			nN2++;
			ixn2[nN2]=j;
		}//end if
	}//end for

	if(prnt==YES) printf("\n nN=%ld, nN2=%ld",nN,nN2);
	/////////////////////////////////////////////////////////

	//3: cal. RHS vector --- b~[]

	calRHS(RHS,a,b,L,U,ixn,ixn2,nN,nN2,m,n);
	if(prnt==YES) printf("\n calRHS() ok");

	for(i=1;i<=m;i++)
	{
		if(RHS[i]<0.0)
		{
			mulMatrixRow(a,i,n,-1.0);
			b[i]  *= -1.0;
			RHS[i]*= -1.0;
		}
	}//end for

	if(prnt==YES) printf("\n non-nega. ok");

	//4: introducing artif. variables
	
	intrArtif(uvIndex,uvNum,arIndex,arNum,a,m,n,NO);//prnt
	if(prnt==YES) printf("\n intrArtif() ok");
	////////////////////////////////////////////////////

	//5: memory allocation for spxTBL and c2[]

	makeMatrix(_spxTBL,spxTBL,m+1,n+2);
	c2=(double *)calloc(n+1,sizeof(double));

	if(c2==NULL){ printf("\n\nError! --- %ld",__LINE__); exit(0); }
	///////////////////////////////////////////////////////

	//6: creating original simplex table

	//6.1 aux. objective function

	aux_obj(c2,n,arIndex,arNum);
	if(prnt==YES) printf("\n aux_obj() ok");
	////////////////////////////////////////////////

	//6.2 init. ixb[]

	init_ixb(ixb,uvIndex,a,m,n);
	if(prnt==YES) printf("\n init_ixb() ok");
	///////////////////////////////////////////////

	//6.3 init. spx.table

	init_spxTBL(spxTBL,a,RHS,c2,ixb,m,n);
	if(prnt==YES) printf("\n init_spxTBL() ok");
	/////////////////////////////////////////////////////

	//6.4 normalizing spx.table

	spxTBL_norm(spxTBL,ixb,m,n);
	if(prnt==YES) printf("\n spxTBL_norm() ok");

	//6.5 eliminating basic

	delBasic(ixn,nN,ixb,m);
	delBasic(ixn2,nN2,ixb,m);

	if(prnt==YES) printf("\n delBasic() ok");
	//////////////////////////////////////////////////////

	//7: iteration

	ret=spxTBL_itr(spxTBL,ixb,ixn,ixn2,arIndex,arNum,nN,nN2,m,n,NO);//prnt
	if(prnt==YES) printf("\n spxTBL_itr() returned: %ld",ret);

	min_g=spxTBL[0][n+1];
	if(prnt==YES) printf("\n min_g=%.4lf",min_g);
	/////////////////////////////////////////////////////

	//8: making conclusion

	if(min_g<-SMLL){ printf("\nError! --- %ld",__LINE__); exit(0); }
	if(min_g>SMLL){ ret=NOSOLU; goto LAB99; }

	remove_arbasic(spxTBL,ixb,ixn,ixn2,arIndex,nN,nN2,arNum,m,n,NO);//prnt
	if(prnt==YES) printf("\n remove_arbasic() ok");

	get_solution(x,spxTBL,L,U,ixb,ixn,ixn2,nN,nN2,m,n);
	if(prnt==YES) printf("\n get_solution() ok");

	if(phs==PHSONE) goto LAB99;
	///////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////

	if(prnt==YES) printf("\n\nSecond phase...");

	//1: deleting artif. columns

	delArtifcol(spxTBL,ixn,ixn2,nN,nN2,arIndex,arNum,m,n);
	n-= arNum;

	if(prnt==YES) printf("\n delArtfcol() ok");
	////////////////////////////////////////////////

	//2: replacing obj. with original

	spxTBL_obj(spxTBL,c,m,n);
	if(prnt==YES) printf("\n spxTBL_obj() ok");

	//3: normalization

	spxTBL_norm(spxTBL,ixb,m,n);
	if(prnt==YES) printf("\n spxTBL_norm() ok");
	/////////////////////////////////////////////////

	//4: iteration

	ret=spxTBL_itr(spxTBL,ixb,ixn,ixn2,arIndex,arNum,nN,nN2,m,n,NO);//prnt
	if(prnt==YES) printf("\n spxTBL_itr() returned: %ld",ret);

	if(ret==OPTIML)
	{
		get_solution(x,spxTBL,L,U,ixb,ixn,ixn2,nN,nN2,m,n);
		if(prnt==YES) printf("\n get_solution() ok");
	}//end if
	/////////////////////////////////////////////////////
	/////////////////////////////////////////////////////

LAB99:
	//5: free memory

	freeMatrix(_spxTBL,spxTBL);
	free(c2);

	free(arIndex);
	free(uvIndex);

	free(ixb);
	free(ixn);
	free(ixn2);

	free(RHS);
	////////////////////////////////////

	return ret;
}

void get_solution(double *x,double **spxTBL,double *L,double *U,long *ixb,long *ixn,long *ixn2,long nN,long nN2,long m,long n)
{
	register long i;
	long k;
	//////////////////////////////////////

	//1: init. solution vector

	initVector(x,n,0.0);
	x[0]=spxTBL[0][n+1];//obj.value
	///////////////////////////////////////

	//2: basic variables

	for(i=1;i<=m;i++)
		x[ixb[i]]=spxTBL[i][n+1];
	/////////////////////////////////////

	//3: lower non-basics

	for(i=1;i<=nN;i++)
	{
		k=ixn[i];
		x[k]=L[k];
	}//end for
	//////////////////////////////////////////

	//4: upper non-basics
	
	for(i=1;i<=nN2;i++)
	{
		k=ixn2[i];
		x[k]=U[k];//maybe large
	}//end for	
	/////////////////////////////////////
	
	return;
}

void delArtifcol(double **spxTBL,long *ixn,long *ixn2,long &nN,long &nN2,long *arIndex,long arNum,long m,long n)
{
	register long i;
	long k,jk,artif;
	//////////////////////////////////

	for(i=1;i<=arNum;i++)
	{
		//.1 for spx.table

		artif=arIndex[i];
		jk=artif+slide_ccn(arIndex,i,arNum);//No Change!

		delMatrixCol(spxTBL,jk,m,n+1);
		////////////////////////////////////////

		//.2 ixn and nN

		k=lookup(ixn,artif,nN);
		if(k!=NO)
		{
			delVectorElm(ixn,k,nN);
			nN--;
		}//end if
		/////////////////////////////////////////
		
		//.3 ixn2 and nN2

		k=lookup(ixn2,artif,nN2);
		if(k!=NO)
		{
			delVectorElm(ixn2,k,nN2);
			nN2--;
		}//end if
	}//end for
	//////////////////////////////////

	return;
}

long spxTBL_itr(double **spxTBL,long *ixb,long *ixn,long *ixn2,long *arIndex,long arNum,long &nN,long &nN2,long m,long n,long prnt)
{
//Notes: by Wang GT
// ret.= OPTIML or UNBNDD
//////////////////////////////////////////////

	long ik,jk,ikFlg,ret=NOSOLU;//init.
	double delta;
	//////////////////////////////////////////

	while(1)//unconditional
	{
		//.1 pos. jk --- to be basic

		jk=pospivot_jk(spxTBL,ixb,ixn,ixn2,arIndex,nN,nN2,arNum,m,n);
		if(prnt==YES) printf("\n jk=%ld",jk);

		if(jk==NO){ ret=OPTIML; break; }
		///////////////////////////////////////////

		//.2 pos. ik --- to leave base

		ik=pospivot_ik(delta,ikFlg,spxTBL,L,U,ixb,ixn,ixn2,nN,nN2,jk,m,n,NO);
		if(prnt==YES) printf("\n ik=%ld, delta=%.4lf",ik,delta);

		if(isLarge(delta)==YES){ ret=UNBNDD; break; }
		//////////////////////////////////////////////////

		//.3 pivot transform

		pivotfun(spxTBL,ixb,ixn,ixn2,nN,nN2,delta,ik,jk,ikFlg,m,n,NO);//prnt
		if(prnt==YES) printf("\n pivotfun() ok");
	}//end while
	//////////////////////////////////////////////////
	
	return ret;
}

void pivotfun(double **spxTBL,long *ixb,long *ixn,long *ixn2,long &nN,long &nN2,double delta,long ik,long jk,long ikFlg,long m,long n,long prnt)
{
	if(prnt==YES) printf("\n\npivotfun()...");

	register long i;

	long k,jkFlg;
	double aux2,pivot;
	///////////////////////////////////////////////////

	//1: init. jkFlg --- LOWER or UPPER

	jkFlg=MIDDL;//init.
		
	if(lookup(ixn,jk,nN)!=NO)   jkFlg=LOWER;
	if(lookup(ixn2,jk,nN2)!=NO) jkFlg=UPPER;
	
	if(jkFlg==MIDDL){ printf("\nError! --- %ld",__LINE__); exit(0); }
	if(prnt==YES) printf("\n jkFlg=%ld",jkFlg);

	if(ik!=NO) goto LAB02;//No Change!
	//////////////////////////////////////////////////////////

	//2: exchange within non-basics

	//2.1 lower --> upper

	if(jkFlg==LOWER)
	{
		k=lookup(ixn,jk,nN);
		if(k==NO){ printf("\nError! --- %ld",__LINE__); exit(0); }

		delVectorElm(ixn,k,nN);
		nN--;

		nN2++;
		ixn2[nN2]=jk;
		sortVector(ixn2,nN2);

		if(prnt==YES) printf("\n lower --> upper");
	}//end if
	/////////////////////////////////////////

	//2.2 upper --> lower

	if(jkFlg==UPPER)
	{
		k=lookup(ixn2,jk,nN2);
		if(k==NO){ printf("\nError! --- %ld",__LINE__); exit(0); }

		delVectorElm(ixn2,k,nN2);
		nN2--;

		nN++;
		ixn[nN]=jk;
		sortVector(ixn,nN);

		if(prnt==YES) printf("\n upper --> lower");
	}//end if

	goto LAB99;
	///////////////////////////////////////////////////////
	///////////////////////////////////////////////////////

LAB02:
	//3: genuine base-exchange procedure

	//3.1 adding non-basic

	if(ikFlg==LOWER){ nN++;  ixn[nN]  =ixb[ik]; }
	if(ikFlg==UPPER){ nN2++; ixn2[nN2]=ixb[ik]; }

	if(ikFlg!=LOWER && ikFlg!=UPPER)
	{
		printf("\n\nError! --- %ld",__LINE__);
		printf("\n  ikFlg=%ld",ikFlg);

		exit(0);
	}//end if
	/////////////////////////////////////////////////	

	//3.2 deleting lower non-basic

	if(jkFlg==LOWER)
	{
		k=lookup(ixn,jk,nN);
		if(k==NO){ printf("\nError! --- %ld",__LINE__); exit(0); }

		delVectorElm(ixn,k,nN);
		nN--;
	}//end if
	///////////////////////////////////////////////

	//3.3 deleting upper non-basic

	if(jkFlg==UPPER)
	{
		k=lookup(ixn2,jk,nN2);
		if(k==NO){ printf("\nError! --- %ld",__LINE__); exit(0); }

		delVectorElm(ixn2,k,nN2);
		nN2--;
	}//end if
	/////////////////////////////////////////

	//3.4 sorting non-basic indices

	sortVector(ixn,nN);
	sortVector(ixn2,nN2);

	//3.5 updating ixb[]

	ixb[ik]=jk;
	if(prnt==YES) printf("\n ixb[] updated");
	////////////////////////////////////////////////

	//4: pivot transformation
	
	//4.1 ik-row normalization

	pivot=spxTBL[ik][jk];
	if(prnt==YES) printf("\n pivot=%.4lf",pivot);
	
	divMatrixRow(spxTBL,ik,(long)0,n+1,pivot);
	if(prnt==YES) printf("\n divMatrixRow() ok");
	////////////////////////////////////////////////////

	//4.2 row transformation

	for(i=0;i<=m;i++)
	{
		if(i==ik) continue;//No Change!

		aux2=spxTBL[i][jk];
		add_mulrow(spxTBL,i,ik,-aux2,(long)0,n+1);
	}//end for

	if(prnt==YES) printf("\n row trans. ok");
	////////////////////////////////////////////////////
	////////////////////////////////////////////////////

LAB99:
	//5: updating RHS and z0

	//5.1 lower non-basic

	if(jkFlg==LOWER)
	{
		for(i=0;i<=m;i++)
			spxTBL[i][n+1]-= delta*spxTBL[i][jk];

		if(ik!=NO)
			spxTBL[ik][n+1]=L[jk]+delta;
	}//end if
	//////////////////////////////////////////////

	//5.2 upper non-basic

	if(jkFlg==UPPER)
	{
		for(i=0;i<=m;i++)
			spxTBL[i][n+1]+= delta*spxTBL[i][jk];

		if(ik!=NO)
		{
			spxTBL[ik][n+1]=U[jk]-delta;
			if(isLarge(U[jk])==YES) spxTBL[ik][n+1]=LARG;
		}
	}//end if

	if(prnt==YES) printf("\n RHS/z0 updated");
	///////////////////////////////////////////////////

	return;
}

long pospivot_ik(double &delta,long &ikFlg,double **spxTBL,double *L,double *U,long *ixb,long *ixn,long *ixn2,
				 long nN,long nN2,long jk,long m,long n,long prnt)
{
//Notes: by Wang GT
// ret.=1,2,...m --- indicating index to leave-basic
// ret.=NO --- indicating no leave-basic found
// delta   --- returned, the incremental
// ikFlg   --- returned: MIDDL, LOWER or UPPER
//////////////////////////////////////////////////////////

	//1: local definitions and init.

	if(prnt==YES) printf("\n\npospivot_ik()...");
	register long i;
	
	long LNonFlg,UNonFlg,ii,r,s;
	long ret=NO;//init.

	double aux2,aux4,RHS;
	double delta1,delta2,span;
	/////////////////////////////////////////////////////////

	//2: init. LNonFlg and UNonFlg <-- jk

	LNonFlg=NO;//init.
	UNonFlg=NO;

	if(lookup(ixn,jk,nN)!=NO)   LNonFlg=YES;
	if(lookup(ixn2,jk,nN2)!=NO) UNonFlg=YES;

	if(LNonFlg==NO && UNonFlg==NO)
	{
		printf("\n\nError! --- %ld",__LINE__);
		printf("\n  jk=%ld, LNonFlg=%ld, UNonFlg=%ld",jk,LNonFlg,UNonFlg);

		exit(0);
	}//end if

	if(LNonFlg==YES && UNonFlg==YES)
	{
		printf("\n\nError! --- %ld",__LINE__);
		printf("\n  jk=%ld, LNonFlg=%ld, UNonFlg=%ld",jk,LNonFlg,UNonFlg);

		exit(0);
	}//end if

	if(prnt==YES) printf("\n LNonFlg=%ld",LNonFlg);
	if(prnt==YES) printf("\n UNonFlg=%ld",UNonFlg);
	////////////////////////////////////////////////////////////

	//3: cal. delta1 and r

	delta1=LARG;//init.
	r=NO;

	for(i=1;i<=m;i++)
	{
		//.1 cal. locals --- aux2

		aux2=spxTBL[i][jk];
		
		RHS=spxTBL[i][n+1];
		ii=ixb[i];
		///////////////////////////////////////////

		//.2 lower non-basic

		if(LNonFlg==YES)
		{
			if(aux2<SMLL)  continue;
			aux4=(RHS-L[ii])/aux2;
		}//end if
		//////////////////////////////////////////

		//.3 upper non-basic

		if(UNonFlg==YES)
		{
			if(aux2>-SMLL) continue;
			aux4=(RHS-L[ii])/(-aux2);
		}//end if
		////////////////////////////////////////////

		//.4 recording the min. --> r

		if(aux4<delta1)
		{
			delta1=aux4;
			r=i;
		}
	}//end for

	if(prnt==YES) printf("\n delta1=%lf, r=%ld",delta1,r);
	//////////////////////////////////////////////////////////

	//4: cal. delta2 and s

	delta2=LARG;//init.
	s=NO;

	for(i=1;i<=m;i++)
	{
		//.1 cal. locals --- aux2

		aux2=spxTBL[i][jk];
		
		RHS=spxTBL[i][n+1];
		ii=ixb[i];
		///////////////////////////////////////////

		//.2 lower non-basic

		if(LNonFlg==YES)
		{
			if(aux2>-SMLL) continue;

			aux4=(U[ii]-RHS)/(-aux2);
			if(isLarge(U[ii])==YES) aux4=LARG;
		}//end if
		///////////////////////////////////////////

		//.3 upper non-basic

		if(UNonFlg==YES)
		{
			if(aux2<SMLL)  continue;

			aux4=(U[ii]-RHS)/aux2;
			if(isLarge(U[ii])==YES) aux4=LARG;
		}//end if
		//////////////////////////////////////////////

		//.4 recording the min. --> s

		if(aux4<delta2)
		{
			delta2=aux4;
			s=i;
		}
	}//end for

	if(prnt==YES) printf("\n delta2=%lf, s=%ld",delta2,s);
	//////////////////////////////////////////////////////////

	//5: cal. span and delta --> ikFlg, ret
	
	delta=min(delta1,delta2);

	span=U[jk]-L[jk];
	if(isLarge(U[jk])==YES) span=LARG;

	if(span<delta)
	{
		delta=span;
		ikFlg=MIDDL;
		ret  =NO;
	}
	else if(delta1<delta2)
	{
		delta=delta1;
		ikFlg=LOWER;
		ret  =r;
	}		
	else
	{
		delta=delta2;
		ikFlg=UPPER;
		ret  =s;
	}
	////////////////////////////////////////

	return ret;
}

long pospivot_jk(double **spxTBL,long *ixb,long *ixn,long *ixn2,long *arIndex,long nN,long nN2,long arNum,long m,long n)
{
//Notes: by Wang GT
// ret. = 1,2,... n --- indicating non-basic to be basic
// ret. = NO --- indicating optimal solution found
//////////////////////////////////////////////////////////

	//1: local definitions and init.

	register long j;
	
	long k,ret;
	double zeta,maxZeta;

	maxZeta=-LARG;//init.
	ret=NO;
	//////////////////////////////////////////

	//2: positioning maxZeta

	for(j=1;j<=n;j++)
	{
		//.1 skipping basics

		k=lookup(ixb,j,m);
		if(k!=NO) continue;
		//////////////////////////////////

		//.2 skipping artif.

		k=lookup(arIndex,j,arNum);
		if(k!=NO) continue;
		////////////////////////////////////

		//.3 recording maxZeta/ret

		if(lookup(ixn,j,nN)!=NO)   zeta=spxTBL[0][j];
		if(lookup(ixn2,j,nN2)!=NO) zeta=-spxTBL[0][j];
		
		if(zeta>maxZeta)
		{
			maxZeta=zeta;
			ret=j;
		}//end if
	}//end for
	////////////////////////////////////////

	//3: return value

	if(maxZeta<SMLL) ret=NO;//No Change!
	return ret;
}

void delBasic(long *ixn,long &nN,long *ixb,long m)
{
	register long i;
	long k;
	///////////////////////////////////
	
	for(i=1;i<=m;i++)
	{
		k=lookup(ixn,ixb[i],nN);
		if(k!=NO)
		{
			delVectorElm(ixn,k,nN);
			nN--;
		}
	}//end for
	///////////////////////////////////

	return;
}

void calRHS(double *RHS,double **a,double *b,double *L,double *U,long *ixn,long *ixn2,long nN,long nN2,long m,long n)
{
	//1: locals and memory allocation

	double *vector;
	double *xn,*xn2;
	double *_N,**N,*_N2,**N2;
	///////////////////////////////////////

	makeMatrix(_N,N,m+1,nN+1);
	makeMatrix(_N2,N2,m+1,nN2+1);

	xn =(double *)calloc(nN+1,sizeof(double));
	xn2=(double *)calloc(nN2+1,sizeof(double));

	vector=(double *)calloc(m+1,sizeof(double));
	if(vector==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }
	/////////////////////////////////////////////////////

	//2: init. vector and matrix

	subVector(xn,L,ixn,nN);
	subVector(xn2,U,ixn2,nN2);

	subMatrix(N,a,ixn,m,nN);
	subMatrix(N2,a,ixn2,m,nN2);
	//////////////////////////////////////////

	//3: cal. RHS vector

	copyVector(RHS,b,m);//init.

	mulMatrixVector(vector,N,xn,m,nN);
	subtractVector(RHS,vector,m);

	mulMatrixVector(vector,N2,xn2,m,nN2);
	subtractVector(RHS,vector,m);
	///////////////////////////////////////

	//4: free memory

	freeMatrix(_N,N);
	freeMatrix(_N2,N2);

	free(xn);
	free(xn2);
	free(vector);
	///////////////////////////////////

	return;
}
	
void spxTBL_norm(double **spxTBL,long *ixb,long m,long n)
{
	//1: locals and memory allocation

	register long i;

	long ik,jk;
	double aux2,aux4;

	double *col=(double *)calloc(m+1,sizeof(double));
	if(col==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }
	/////////////////////////////////////////////
	
	//2: row transformation --> 0-row

	for(i=1;i<=m;i++)
	{
		//2.1 pos. col --- jk

		jk=ixb[i];//col. index
		aux2=spxTBL[0][jk];

		if(fabs(aux2)<SMLL) continue;
		////////////////////////////////////////

		//2.2 pos. ik

		copyVectorCol(col,spxTBL,jk,m);
		ik=posNonzero(col,m);
		if(ik==NO){ printf("\nError! --- %ld",__LINE__); exit(0); }
		///////////////////////////////////////////

		//2.3 zeta_jk --> 0

		aux4=spxTBL[ik][jk];
		add_mulrow(spxTBL,(long)0,ik,-aux2/aux4,n+1);
	}//end for	
	//////////////////////////////////////////

	free(col);
	return;
}

void spxTBL_obj(double **spxTBL,double *c,long m,long n)
{
	register long j;

	for(j=1;j<=n;j++)
		spxTBL[0][j]=-c[j];

	spxTBL[0][0]  =1;//No Change!
	spxTBL[0][n+1]=0;

	return;
}

long slide_ccn(long *arIndex,long ik,long arNum)
{
	register long i;
	long val,ret=0;//No Change!
	////////////////////////////////////////

	if(ik>arNum){ printf("\nError! --- %ld",__LINE__); exit(0); }

	val=arIndex[ik];
	for(i=1;i<=ik-1;i++)
	{
		if(arIndex[i]==val){ printf("\nError! --- %ld",__LINE__); exit(0); }
		if(arIndex[i]<val) ret--;
	}//end for
	////////////////////////////////////

	return ret;
}

void remove_arbasic(double **spxTBL,long *ixb,long *ixn,long *ixn2,long *arIndex,long nN,long nN2,long arNum,long m,long n,long prnt)
{
	if(prnt==YES) printf("\n\nremove_arbasic()...");

	register long j;
	long k,ik,jk;
	//////////////////////////////////////////////////

LAB01:
	//1: find ar.basic

	ik=posArbasic(arIndex,arNum,ixb,m);
	if(prnt==YES) printf("\n\n ik=%ld",ik);

	if(ik==NO) goto LAB99;
	//////////////////////////////////////

	//2: positioning jk

	jk=NO;//init.
	for(j=1;j<=n;j++)
	{
		//.1 skipping basic variables

		k=lookup(ixb,j,m);
		if(k!=NO) continue;
		//////////////////////////////

		//.2 skipping artif. --- No Change!

		k=lookup(arIndex,j,arNum);
		if(k!=NO) continue;
		///////////////////////////////

		//.3 skipping upper non-basic

		k=lookup(ixn2,j,nN2);
		if(k!=NO) continue;
		///////////////////////////////

		//.4 non-zero elem.

		if(fabs(spxTBL[ik][j])>SMLL)
		{
			jk=j;//found
			break;
		}
	}//end for	

	if(prnt==YES) printf("\n jk=%ld",jk);
	////////////////////////////////////////////////

	//3: pivot transform --- (ik,jk)

	if(jk!=NO)
	{
		pivotfun(spxTBL,ixb,ixn,ixn2,nN,nN2,0.0,ik,jk,LOWER,m,n,NO);//prnt
		if(prnt==YES) printf("\n pivotfun() ok");
	}//end if
	//////////////////////////////////////////////////////

	//4: del. redundant constraint

	if(jk==NO)
	{
		printf("\n\nWarning! --- %ld",__LINE__);
		printf("\n  Deleting redundant constaint...");

		exit(0);//todo:
	}//end if

	goto LAB01;
	///////////////////////////////////////////////

LAB99:
	return;
}

long posArbasic(long *arIndex,long arNum,long *ixb,long m)
{
	register long i;
	long k,ret=NO;//init.
	//////////////////////////////////////////

	for(i=1;i<=m;i++)
	{
		k=lookup(arIndex,ixb[i],arNum);
		if(k!=NO)
		{
			ret=k;
			break;
		}
	}//end for
	/////////////////////////////////////

	return ret;
}

void typeSpxTBL(double **spxTBL,long m,long n,char *caption)
{
	register long i,j;

	printf("\n\n%s:",caption);
	for(i=0;i<=m;i++)
	{
		printf("\n");
		for(j=0;j<=n+1;j++)
			printf(" %7.3lf",spxTBL[i][j]);
	}//end for
	//////////////////////////////////////

	return;
}

void init_spxTBL(double **spxTBL,double **a,double *b,double *c,long *ixb,long m,long n)
{
	register long i,j;

	//1: 0-row

	spxTBL[0][0]=1.0;
	spxTBL[0][n+1]=0;

	for(j=1;j<=n;j++)
		spxTBL[0][j]=-c[j];
	//////////////////////////////////////

	//2: 0-col

	for(i=1;i<=m;i++)
		spxTBL[i][0]=0.0;
	/////////////////////////////////////

	//3: copying a[][]

	copyMatrix(spxTBL,a,m,n);

	//4: copying b[]

	copyMatrixCol(spxTBL,b,n+1,m);
	/////////////////////////////////////

	return;
}

void aux_obj(double *c2,long n,long *arIndex,long arNum)
{
	register long i;
	long jk;
	///////////////////////////////

	initVector(c2,n,0.0);

	for(i=1;i<=arNum;i++)
	{
		jk=arIndex[i];
		c2[jk]=1.0;
	}//end for
	///////////////////////////////////

	return;
}

void init_ixb(long *ixb,long *uvIndex,double **a,long m,long n)
{
	register long i;
	long jk,ik;

	double *col=(double *)calloc(m+1,sizeof(double));
	if(col==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }
	////////////////////////////////////////////

	for(i=1;i<=m;i++)
	{
		jk=uvIndex[i];
		copyVectorCol(col,a,jk,m);

		ik=lookup(col,1.0,m);
		if(ik==NO){ printf("\nError! --- %ld",__LINE__); exit(0); }

		ixb[ik]=jk;
	}//end for
	/////////////////////////////////

	free(col);
	return;
}

void intrArtif(long *uvIndex,long &uvNum,long *arIndex,long &arNum,double **a,long m,long &n,long prnt)
{
	if(prnt==YES) printf("\n\nintrArtif()...");

	register long j;
	long k,ret,arNum_req;
	////////////////////////////////////////////

	//1: init. uvIndex and uvNum

	arNum=0;//init.
	uvNum=0;
	
	for(j=1;j<=n;j++)
	{
		ret=isUnitCol(a,j,m);
		if(ret==YES)
		{
			k=lookup(uvIndex,j,uvNum);
			if(k!=NO) continue;

			uvNum++;
			uvIndex[uvNum]=j;
		}//end if
	}//end for

	if(prnt==YES) printf("\n uvNum=%ld",uvNum);
	if(uvNum==m) return;
	///////////////////////////////////////////////////////

	//2: introducing artif. variable(s)

	arNum_req=m-uvNum;
	if(prnt==YES) printf("\n arNum_req=%ld",arNum_req);

	arNum=0;//init.
	for(j=n+1;j<=n+arNum_req;j++)
	{
		//.1 expanding a[][]

		k=pos_artif(uvIndex,uvNum,a,m,n);
		if(k==NO){ printf("\nError! --- %ld",__LINE__); exit(0); }

		initMatrixCol(a,j,m,0.0);
		a[k][j]=1.0;
		/////////////////////////////////////////////

		//.2 increasing arIndex[]

		arNum++;
		arIndex[arNum]=j;

		//.3 increasing uvIndex[]

		uvNum++;
		uvIndex[uvNum]=j;
		//////////////////////////////////////////

		//.4 expanding L and U[]

		L[j]=0.0;
		U[j]=LARG;//unbounded
	}//end for

	if(prnt==YES) printf("\n artif. introduced");
	//////////////////////////////////////////////////////

	if(arNum!=arNum_req || uvNum!=m)
	{
		printf("\n\nError! --- %ld",__LINE__);
		printf("\n  arNum=%ld,uvNum=%ld",arNum,uvNum);

		exit(0);
	}//end if
	//////////////////////////////////////////////

	//3: increasing the dimension

	n+= arNum;

	if(prnt==YES) printf("\n n=%ld",n);
	if(prnt==YES) printf("\n uvNum=%ld, arNum=%ld",uvNum,arNum);
	
	return;
}

long pos_artif(long *uvIndex,long uvNum,double **a,long m,long n)
{
	register long i;

	long k,ret=NO;//init.
	long *artif;
	
	double *col;
	//////////////////////////////////////////////////
	
	artif=(long *)calloc(m+1,sizeof(long));
	col  =(double *)calloc(m+1,sizeof(double));
	
	if(col==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }
	///////////////////////////////////////////////////////

	//1: marking artif[]

	initVector(artif,m,(long)0);//cleared

	for(i=1;i<=uvNum;i++)
	{
		copyVectorCol(col,a,uvIndex[i],m);

		k=lookup(col,1.0,m);
		if(k==NO){ printf("\nError! --- %ld",__LINE__); exit(0); }

		artif[k]=1;//No Change!
	}//end for
	///////////////////////////////////////////////

	//2: pos. the first zero

	for(i=1;i<=m;i++)
	{
		if(artif[i]==0)
		{
			ret=i;
			break;
		}
	}//end for
	////////////////////////////////////////////

	//3: free memory

	free(col);
	free(artif);
	/////////////////////////////////

	return ret;
}

void consVal(double *cons,double **a,double *x,long m,long n)
{
	register long i;
	double *row;
	//////////////////////////////////////////////

	row=(double *)calloc(n+1,sizeof(double));
	if(row==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }
	
	for(i=1;i<=m;i++)
	{
		copyVectorRow(row,a,i,n);
		cons[i]=sum(row,x,n);
	}//end for
	////////////////////////////////////////

	free(row);
	return;
}
			
void read_coeff(FILE *in,double **a,long m,long n)
{
	long ret=posDataBlock(in,"DE COEFFMATRIX");
	if(ret==NO) exit(0);

	register long i,j;
	double fa;
	//////////////////////////////////////
	
	for(i=1;i<=m;i++)
	{
		for(j=1;j<=n;j++)
		{
			if(fscanf(in,"%lf",&fa)!=1){ printf("\n\nError! --- %ld",__LINE__); exit(0); }
			a[i][j]=fa;
		}
	}//end for
	////////////////////////////////

	return;
}

void read_RHS(FILE *in,double *b,long m)
{
		long ret=posDataBlock(in,"DE RIGHTVECTOR");
		if(ret==NO) exit(0);

        register long i;
		double fa;
		/////////////////////////////////////////
		
        for(i=1;i<=m;i++)
        {
                if(fscanf(in,"%lf",&fa)==1)
					b[i]=fa;
        }//end for

		return;
}

void read_blpsize(FILE *in,long &m,long &n,long &nEQ)
{
	long ret=posDataBlock(in,"DE BLPSIZE");
	if(ret==NO){ printf("\n\nNot found datablock: BLPSIZE"); exit(0); }

	ret=fscanf(in,"%ld%ld%ld",&m,&n,&nEQ);
	if(ret!=3){ printf("\n\nError! --- %ld",__LINE__); exit(0); }

	return;
}

void read_objfunction(FILE *in,double *c,long n)
{
	 	long ret=posDataBlock(in,"DE OBJFUNCTION");
		if(ret==NO){ printf("\n\nNot found datablock: OBJFUNCTION"); exit(0); }

		register long i;
		double fa;
		//////////////////////////////////////////

		for(i=1;i<=n;i++)
        {
                if(fscanf(in,"%lf",&fa)==1)
                        c[i]=fa;
        }//end for
		////////////////////////////////////////

		return;
}

void read_minbound(FILE *in,double *L,long n)
{
		long ret=posDataBlock(in,"DE MINBOUND");
		if(ret==NO){ printf("\n\nNot found datablock: MINBOUND"); exit(0); }

		register long i;
		double fa;
		/////////////////////////////////////

        for(i=1;i<=n;i++)
        {
                if(fscanf(in,"%lf",&fa)==1)
                        L[i]=fa;
        }//end for
		//////////////////////////////////////

		return;
}
 
void read_maxbound(FILE *in,double *U,long n)
{
		long ret=posDataBlock(in,"DE MAXBOUND");
		if(ret==NO){ printf("\n\nNot found datablock: MAXBOUND"); exit(0); }

		register long i;
		double fa;
		/////////////////////////////////////

        for(i=1;i<=n;i++)
        {
                if(fscanf(in,"%lf",&fa)==1)
				{
                        U[i]=fa;
						if(fabs(fa)<SMLL) U[i]=LARG;
				}
        }//end for
		////////////////////////////////////////

		return;
}

void blp_readData(char *datfile,long &m,long &n,long &nEQ,long prnt)
{
	if(prnt==YES) printf("\n\nblp_readData()...");

	//1: opening file and reading size of blp

	FILE *in=fopen(datfile,"r");
	if(in==NULL){ printf("\n\nCannot open %s\n",datfile); exit(0); }

	read_blpsize(in,m,n,nEQ);
	if(prnt==YES) printf("\n read_blpsize() ok");
	
	//2: memory allocation for globals

	blp_global_alloc(m,n);
	if(prnt==YES) printf("\n global_alloc() ok");
	////////////////////////////////////////////////////

	//3: reading a, b, c[]

	//3.1 coeff. matrix --- a[][]

	read_coeff(in,a,m,n);
	if(prnt==YES) printf("\n read_coeff() ok");
	
	//3.2 right vector --- b[]

	read_RHS(in,b,m);
	if(prnt==YES) printf("\n read_RHS() ok");

	//3.3 obj.function --- c[]

	read_objfunction(in,c,n);
	if(prnt==YES) printf("\n read_objfunction() ok");
	/////////////////////////////////////////////////////

	//4: reading bounds
	
	//4.1 minbound --- L[]

	read_minbound(in,L,n);
	if(prnt==YES) printf("\n read_minbound() ok");

	//4.2 maxbound --- U[]

	read_maxbound(in,U,n);
	if(prnt==YES) printf("\n read_maxbound() ok");
	////////////////////////////////////////////////////

	//5: closing file

	fclose(in);
	if(prnt==YES) printf("\n %s closed",datfile);
	///////////////////////////////////////////////////

	return;
}

void printLP(char *blpfile,double **a,double *b,double *c,double *L,double *U,long m,long n)
{
        register long i,j;

        FILE *out=fopen(blpfile,"w");
        if(out==NULL){ printf("\n\nCannot open %s\n",blpfile); exit(0); }
        /////////////////////////////////////////////////

        //1: coeff. matrix --- a[][]

        fprintf(out,"\n\na[][]");
        for(i=1;i<=m;i++)
        {
                fprintf(out,"\n");
                for(j=1;j<=n;j++)
                        fprintf(out,"  %7.3lf",a[i][j]);
        }//end for
        /////////////////////////////////////////////////

        //2: right-hand side --- b[]

        fprintf(out,"\n\nb[]\n");
        for(i=1;i<=m;i++)
                fprintf(out,"  %7.3lf",b[i]);
        //////////////////////////////////////////////

        //3: obj.function --- c[]

        fprintf(out,"\n\nc[]\n");
        for(i=1;i<=n;i++)
                fprintf(out,"  %7.3lf",c[i]);
        /////////////////////////////////////////////

        //4: lower/upper bounds --- L/U[]

        fprintf(out,"\n\nL[]\n");
        for(i=1;i<=n;i++)
                fprintf(out,"  %7.3lf",L[i]);

        fprintf(out,"\n\nU[]\n");
        for(i=1;i<=n;i++)
                fprintf(out,"  %7.3lf",U[i]);
        /////////////////////////////////////////////

        fclose(out);//blpfile closed
        return;
}

void unmapping(double *x,double *L,double *U,double *Lm,long n)
{
	addVector(x,Lm,(long)0,n);
	
	addVector(L,Lm,n);
	addVector(U,Lm,n);
	/////////////////////////////

	return;
}

void mapping(double *Lm,double **a,double *b,double *c,double *L,double *U,long m,long n)
{
	register long i;

	//1: creating Lm[] <-- L[]

	copyVector(Lm,L,n);
	Lm[0]=sum(c,Lm,n);//obj.incremental
	///////////////////////////////////////

	//2: right-hand side

	for(i=1;i<=m;i++)
		b[i]-= sumRowVector(a,Lm,i,n);

	//3: lower/upper bounds

	subtractVector(L,Lm,n);
	subtractVector(U,Lm,n);
	/////////////////////////////////////

	return;
}

long vfy_solution(double *x,double **a,double *b,double *c,double *L,double *U,long m,long n,long prnt)
{
	if(prnt==YES) printf("\n\nvfy_solution()...");

	register long i;
	
	long ret=YES;//init.
	double obj,error;

	double *cons=(double *)calloc(m+1,sizeof(double));
	if(cons==NULL){ printf("\n\nError! --- %ld",__LINE__); exit(0); }
	////////////////////////////////////////////////////

	//1: value of obj.function
	
	obj  =sum(c,x,n);
	error=x[0]-obj;

	if(fabs(error)>EPS)
	{
		printf("\n\nError! --- %ld",__LINE__);
		printf("\n  obj.value=%lf, while x[0]=%lf",obj,x[0]);
		
		ret=NO;
	}//end if

	if(prnt==YES) printf("\n obj.function ok");
	///////////////////////////////////////////////////

	//2: constraint values

	consVal(cons,a,x,m,n);
	for(i=1;i<=m;i++)
	{
		error=cons[i]-b[i];
		if(fabs(error)>EPS)
		{
			printf("\n\nError! --- %ld",__LINE__);
			printf("\n  cons.value=%lf, while b[%ld]=%lf",cons[i],i,b[i]);

			ret=NO;
		}
	}//end for

	if(prnt==YES) printf("\n constraints ok");
	/////////////////////////////////////////////////////

	//3: variable bounds

	for(i=1;i<=n;i++)
	{
		if(x[i]<L[i]-SMLL || x[i]>U[i]+SMLL)
		{
			printf("\n\nError! --- %ld",__LINE__);
			printf("\n  x[%ld]=%lf, while L=%lf, U=%lf",x[i],L[i],U[i]);

			ret=NO;
		}//end if
	}//end for
	////////////////////////////////////////////////

	free(cons);
	return ret;
}

void standard(double **a,double *b,double *c,double *L,double *U,long m,long &n,long nEQ,long prnt)
{
//Notes: by Wang GT, 2003-07-07 ///////////////////////////////
//The standard form of BLP problem is as following:
//  min z=cx    --- a/c[] expanded by slacks
//  st. ax = b  --- b[] non-negative
//////////////////////////////////////////////////////

	if(prnt==YES) printf("\n\nstandard()...");

	register long i,j;
	long nSL;
	///////////////////////////////////////////

	//1: adding slacks

	nSL=m-nEQ;
	if(prnt==YES) printf("\n nSL=%ld",nSL);

	for(j=n+1;j<=n+nSL;j++)
	{
		//.1 expanding vectors

		c[j]=0.0;
		L[j]=0.0;
		U[j]=LARG;//unbounded
		//////////////////////////

		//.2 coeff. matrix

		initMatrixCol(a,j,m,0.0);
		a[nEQ+j-n][j]=1.0;
	}//end for

	n+= nSL;//dim. increased
	if(prnt==YES) printf("\n n=%ld",n);
	///////////////////////////////////////////

	//2: non-negative right vector

	for(i=1;i<=m;i++)
	{
		if(b[i]<0.0)
		{
			mulMatrixRow(a,i,n,-1.0);
			b[i]*= -1.0;
		}//end if
	}//end for

	if(prnt==YES) printf("\n nonneg.b[] ok");
	///////////////////////////////////////////////

	return;
}

///////////////////////////////////////////
//end of file
