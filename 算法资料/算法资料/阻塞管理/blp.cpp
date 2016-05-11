//blp.cpp by Wang GT

#include"common.h"
#include"blp.h"
/////////////////////////////////////////////////////

void consVal(double *cons,double **a,double *x,long m,long n)
{
	register long i,j;
	double aux2,sum;
	//////////////////////////////

	for(i=1;i<=m;i++)
	{
		sum=0.0;//init.
		for(j=1;j<=n;j++)
		{
			aux2=a[i][j];
			if(fabs(aux2)<SMLL) continue;

			sum+= aux2*x[j];
		}//end for

		cons[i]=sum;		
	}//end for
	//////////////////////////////

	return;
}
			
void actvFun(long *actvCon,long *actvBnd,long *mel,double *b,double *L,double *U,
				  double **a,double *x,long m,long n)
{
	register long i;
	double aux2,*cons;
	//////////////////////////////////////////////
	
	//0: allocation and init.

	cons=(double *)calloc(m+1,sizeof(double));
	if(cons==NULL){ printf("\n\nError! --- %ld",__LINE__); exit(0); }

	initVector(actvCon,m,(long)NO);//init.
	initVector(actvBnd,n,(long)NO);
	/////////////////////////////////////////////

	//1: actvCon[] --- active constraints

	consVal(cons,a,x,m,n);
	for(i=1;i<=m;i++)
	{
		aux2=cons[i]-b[i];
		if(fabs(aux2)<EPS) actvCon[i]=YES;
	}//end for
	//////////////////////////////////////////

	//2: actvBnd[] --- active bounds

	for(i=1;i<=n;i++)
	{
		if(fabs(x[i]-L[i])<EPS) actvBnd[i]=YES;
		if(fabs(x[i]-U[i])<EPS) actvBnd[i]=YES;
	}//end for
	/////////////////////////////////////

	free(cons);
	return;
}

void BLP_readData(char *datafile,double **a,double *b,double *c,double *L,double *U,
							long *mel,long m,long n,long nEQ,long prnt)
{
	register long i,j;
	double fa;

	char string[200];
	
	FILE *in=fopen(datafile,"r");
	if(in==NULL){ printf("\nCannot open %s",datafile); exit(0); }
	//////////////////////////////////////////////

	fgets(string,LINELN,in);//module skipped

        //1: a[][]

        for(i=1;i<=m;i++)
        {
                for(j=1;j<=n;j++)
                {
                        if(fscanf(in,"%lf",&fa)==1)
                                a[i][j]=fa;
                }//end for

                fgets(string,LINELN,in);
        }//end for
        ////////////////////////////////

        //2: b[]

        for(i=1;i<=m;i++)
        {
                if(fscanf(in,"%lf",&fa)==1)
                        b[i]=fa;
        }//end for

        fgets(string,LINELN,in);
        ///////////////////////////////////////

	//3: c[]

        for(i=1;i<=n;i++)
        {
                if(fscanf(in,"%lf",&fa)==1)
                        c[i]=fa;
        }//end for

        fgets(string,LINELN,in);
        ////////////////////////////////////

        //4: L[]

        for(i=1;i<=n;i++)
        {
                if(fscanf(in,"%lf",&fa)==1)
                        L[i]=fa;
        }//end for

        fgets(string,LINELN,in);
        ////////////////////////////////////

        //5: U[]

        for(i=1;i<=n;i++)
        {
                if(fscanf(in,"%lf",&fa)==1)
                        U[i]=fa;
        }//end for

        fgets(string,LINELN,in);
        ////////////////////////////////

        //6: mel[]

        for(i=1;i<=nEQ;i++)
                mel[i]=EQ;

        for(i=nEQ+1;i<=m;i++)
                mel[i]=LT;
	/////////////////////////////////////////

	fclose(in);
	return;
}

void printLP(char file[],double **a,double *b,double *c,double *L,double *U,long *mel,long m,long n0)
{
        register long i,j;

        long nEQ,k,kk=10;//init.
        double aux2;
        /////////////////////////////////

        FILE *out=fopen(file,"w");
        if(out==NULL){ printf("\n\nCannot open %s\n",file); exit(0); }

        nEQ=getNumEQ(mel,m);
        fprintf(out,"%ld %ld %ld",m,n0,nEQ);
        ///////////////////////////////////////////////

        //1: a[][]

	fprintf(out,"\n\na[][]");
	for(i=1;i<=m;i++)//EQ constaints
        {
                if(mel[i]!=EQ) continue;

		fprintf(out,"\n");
		k=0;//init.

                for(j=1;j<=n0;j++)
                {
                        fprintf(out,FT84,a[i][j]);
                        if(++k>=kk){ k=0; fprintf(out,"\n"); }
                }//end for
        }//end for
	///////////////////////////////////////

        for(i=1;i<=m;i++)//non_EQ constraints
        {
                if(mel[i]==EQ) continue;

                fprintf(out,"\n");
                k=0;//init.

		for(j=1;j<=n0;j++)
                {
                        if(mel[i]==LT) aux2=a[i][j];
                        if(mel[i]==MT) aux2=-a[i][j];

                        fprintf(out,FT84,aux2);
                        if(++k>=kk){ k=0; fprintf(out,"\n"); }
                }//end for
        }//end for
        ///////////////////////////

        //2: b[]

	fprintf(out,"\n\nb[]\n");
        k=0;//init.
	
	for(i=1;i<=m;i++)//EQ constaints
        {
                if(mel[i]!=EQ) continue;
                
		fprintf(out,FT84,b[i]);
        	if(++k>=kk){ k=0; fprintf(out,"\n"); }
	}//end for

	for(i=1;i<=m;i++)//non_EQ constraints
        {
                if(mel[i]==EQ) continue;

                if(mel[i]==LT) aux2=b[i];
                if(mel[i]==MT) aux2=-b[i];

		fprintf(out,FT84,aux2);
        	if(++k>=kk){ k=0; fprintf(out,"\n"); }
	}//end for
        /////////////////////////////////////

        //3: c[]

	fprintf(out,"\n\nc[]\n");

        k=0;//init.
        for(i=1;i<=n0;i++)
        {
                fprintf(out,FT82,c[i]);
                if(++k>=kk){ k=0; fprintf(out,"\n"); }
        }//end for
        ////////////////////////////

        //4: Lmin[] and Lmax[]

        fprintf(out,"\n\nL[]\n");

	k=0;//init.
        for(i=1;i<=n0;i++)
        {
                fprintf(out,FT84,L[i]);
                if(++k>=kk){ k=0; fprintf(out,"\n"); }
        }//end for
        //////////////////////

	fprintf(out,"\n\nU[]\n");

        k=0;//init.
        for(i=1;i<=n0;i++)
        {
                fprintf(out,FT84,U[i]);
                if(++k>=kk){ k=0; fprintf(out,"\n"); }
        }//end for
        //////////////////////////////

        fclose(out);
        return;
}

void scaling(double **a,double *b,long m,long n0)
{
	register long i;
	long k;
	
	double aux2;
	////////////////////////////////////

	for(i=1;i<=m;i++)
	{
		aux2=fabsmin(a,i,n0);
		if(fabs(aux2)<SMLL) continue;

		k=scale(aux2);
		if(k==0) continue;
		/////////////////////////////

		aux2=pow(10.0,k);

		divMatrixRow(a,i,n0,aux2);
		b[i]/= aux2;
	}//end for
	/////////////////////////////////////

	return;
}
	
long blp(double *x,double **a,double *b,double *c,double *Lmin,double *Lmax,long *mel,long m,long n0,long prnt)
{
////////////////////////////////////////////////////////////////////
//NOTATIONS by Wang GT, EPRI2001
//	x[]	: n0 * 1  --- solution with obj.value stored in x[0];
//	a[][]	: m * n0  --- coefficient matrix;
//	b[]	: m * 1   --- right vector;
//	c[]	: 1 * n0  --- coefficient vector of obj.func.;
//	Lmin[]	: n0 * 1  --- lower limit;
//	Lmax[]	: n0 * 1  --- upper limit;
//	mel[]	: m * 1   --- constraint types, LT EQ or MT;
//	m	: number of constraints;
//	n0	: number of original variables.
//Other Business:
//		1. Min. objective function;
//		2. Return value:
//				NOSOLU --- No Solution
//				OPTIML --- Optimal
//				UNBNDD --- Unbounded
/////////////////////////////////////////////////////////////////

	if(prnt==YES) printf("\n\nblp()...");

	long ret,_m,_n0,_nSR,_n,*_mel;
	double *Lm,*__a,**_a,*_b,*_c,*_x,*_Lmin,*_Lmax;
	//////////////////////////////////////////////////////////
	
	//1: mapping to form Lm[]

	Lm=(double *)calloc(n0+1,sizeof(double));
	if(Lm==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }

	mapping(Lm,a,b,c,Lmin,Lmax,m,n0);
	if(prnt==YES) printf("\n mapping() ok");
	/////////////////////////////////////////////////////////

	//2: constructuring standard LP

	//2.1 init. basic variables

	_m=m;		//num.constraints, not changed later
	_n0=n0;		//num.original variables, modified by standard()

	_nSR=getNumSR(b,mel,m);	//number of surplus
	_n=_n0+_nSR+_m;		//number of all variables

	if(prnt==YES) printf("\n _n0=%ld, _nSR=%ld",_n0,_nSR);
	//////////////////////////////////////////////////////

	//2.2 allocation of memory

	makeMatrix(__a,_a,_m+1,_n+1);//for a[][]
	_b=(double *)calloc(_m+1,sizeof(double));
	_c=(double *)calloc(_n+1,sizeof(double));
	_x=(double *)calloc(_n+1,sizeof(double));
	_mel=(long *)calloc(_m+1,sizeof(long));
	_Lmin=(double *)calloc(_n+1,sizeof(double));
	_Lmax=(double *)calloc(_n+1,sizeof(double));

	if(_Lmax==NULL || _b==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }
	if(prnt==YES) printf("\n allocation ok");
	////////////////////////////////////////////////

	//2.3 initialization

	copyMatrix(_a,a,m,n0);		//a[][]
	copyVector(_b,b,m);		//b[]
	copyVector(_c,c,n0);		//c[]
	copyVector(_mel,mel,m);		//mel[]
	copyVector(_Lmin,Lmin,n0);	//Lmin[]
	copyVector(_Lmax,Lmax,n0);	//Lmax[]

	if(prnt==YES) printf("\n init. ok");
	////////////////////////////////////////

	//2.4 standardization
	
	standard(_a,_b,_c,_Lmin,_Lmax,_mel,_m,_n0,NO);//prnt
	if(prnt==YES) printf("\n standard() ok: _n0=%ld",_n0);
	///////////////////////////////////////////////////////
	
	//3: solving the problem

	ret=blpfun(_x,_a,_b,_c,_Lmin,_Lmax,_mel,_m,_n0,_n,NO,NO);//prnt,debug
	if(prnt==YES) printf("\n blpfun() ok: ret=%ld",ret);

	if(ret==OPTIML)
	{
		copyVector(x,_x,(long)0,n0);//solution stored into x[]
		unmapping(x,Lm,n0);

		if(prnt==YES) printf("\n x[] obtained");
	}//end if
	//////////////////////////////////////

	//4: free memory

	freeMatrix(__a,_a);
	free(_b);
	free(_c);
	free(_x);
	free(_Lmax);
	free(_Lmin);
	free(_mel);

	free(Lm);
	////////////////

	return ret;
}

void mapping(double *Lm,double **a,double *b,double *c,double *L,double *U,long m,long n0)
{
	register long i,j;
    double sum,aux2;
	////////////////////////////
	
	//1. forming Lm[]
	
	for(i=1;i<=n0;i++)
	{
		if(L[i]<0.0)
			Lm[i]=L[i];
		else
			Lm[i]=0.0;
	}//end for
    /////////////////////////////

	//2. c[], a[][] and b[]

	sum=0.0;
	for(j=1;j<=n0;j++)
	{
		aux2=Lm[j];
		sum+= aux2*c[j];
		
		for(i=1;i<=m;i++)
			b[i]-= a[i][j]*aux2;
	}//end for
	
	Lm[0]=sum;//for obj.value
	////////////////////////////

	//3. mapping bounds

    for(i=1;i<=n0;i++)
    {
		L[i]-= Lm[i];
		U[i]-= Lm[i];
	}//end for
	///////////////////////////

	return;
}

void unmapping(double *x,double *Lm,long n0)
{
	register long i;
	
	for(i=0;i<=n0;i++)
		x[i]+= Lm[i];//x[0] --- obj.value
	
	return;
}

long blpfun(double *x,double **a,double *b,double *c,double *Lmin,double *Lmax,long *mel,long m,long n0,long n,long prnt,long debug)
{	
//NOTATIONS:////////////////////////////////////////////////////////
//	x[]	: n * 1 --- the solution with obj.value stored in x[0];
//	a[][]	: m * n --- expanded coefficient matrix;
//	b[]	: m * 1 --- the right vector, non-minus;
//	c[]	: n * 1 --- expanded coefficient vector of obj.func.;
//	Lmin[]	: n * 1 --- lower limit;
//	Lmax[]	: n * 1 --- upper limit;
//	mel[]	: m * 1 --- constraint types, EQ or LT type;
//	m	: number of constraints;
//	n0	: number of original variables;
//	n	: number of all variables, n=n0+m.
//Other Business:
//		1. Min. objective function;
//		2. EQ or LT constraints, NO GT constaint permitted;
//		3. Return value:
//				NOSOLU --- No Solution
//				OPTIML --- Optimal
//				UNBNDD --- Unbounded
/////////////////////////////////////////////////////////////////////

	if(prnt==YES) printf("\n\nblpfun()...");

	register long i,j;

	long nN,nN2,ret=OPTIML,ret2;
	double z;

	long *ixb,*ixn,*ixn2;
	double *_B,**B,*_N,**N,*_N2,**N2;
	double *xb,*xn,*xn2,*cb,*cn,*cn2;
	////////////////////////////////////////////////////

	if(n!=n0+m)
	{
		printf("\n\nError! --- %s: %ld",__FILE__,__LINE__); 
		printf("\n  n0=%ld, m=%ld, n=%ld",n0,m,n);
		exit(0);
	}//end if
	////////////////////////////////

	//Allocation

	makeMatrix(_B,B,m+1,m+1);
	makeMatrix(_N,N,m+1,n0+1);
	makeMatrix(_N2,N2,m+1,n0+1);
	
	xb=(double *)calloc(m+1,sizeof(double));
	xn=(double *)calloc(n0+1,sizeof(double));
	xn2=(double *)calloc(n0+1,sizeof(double));

	ixb=(long *)calloc(m+1,sizeof(long));
	ixn=(long *)calloc(n0+1,sizeof(long));
	ixn2=(long *)calloc(n0+1,sizeof(long));

	cb=(double *)calloc(m+1,sizeof(double));
	cn=(double *)calloc(n0+1,sizeof(double));
	cn2=(double *)calloc(n0+1,sizeof(double));

	if(cn2==NULL || ixn2==NULL || xn2==NULL)
	{ printf("\nError! --- %ld",__LINE__); exit(0); }
	////////////////////////////////////////////////////

	//1: starting up

	//1.1 completing matrices 
	
	for(i=1;i<=m;i++)//completing Lmin[] and Lmax[]
	{	
		Lmin[n0+i]=0.0;
		Lmax[n0+i]=BIGNUM;
	}//end for

	for(i=1;i<=m;i++)//completing a[][] and c[]
	{
		for(j=1;j<=m;j++)
		{
			if(j==i) a[i][n0+j]=1.0;
			else	 a[i][n0+j]=0.0;
		}//end for

		if(mel[i]==EQ)
			c[n0+i]=(double)BIGNUM;// big-M method
		else
			c[n0+i]=0.0;
	}//end for

	if(prnt==YES) printf("\n matrices completed");
	if(debug==YES){ typeMatrix(a,m,n,"a[][]"); wpause(); }
	///////////////////////////////////////////////
	
	//1.2 making classification to form index sets

	for(i=1;i<=m;i++) 
		ixb[i]=n0+i;//for basal varialbles
	
	subMatrix(B,a,ixb,m,m);
	if(isUnitMatrix(B,m)!=YES){ printf("\nError! --- %ld",__LINE__); exit(0); }

	if(prnt==YES) printf("\n base matrix formed");
	if(debug==YES) typeMatrix(B,m,m,"B[][]");
	////////////////////////////////////////////////

	ret2=indexSets(ixn,ixn2,N,N2,xn,xn2,xb,ixb,B,a,b,Lmin,Lmax,m,n0,n,nN,nN2,NO);//prnt
	if(prnt==YES) printf("\n indexSets() returned: %ld",ret2);

	if(ret2==NO)//No Original Basic Feasible Solution
	{
		ret=NOSOLU;
		goto LAB099;
	}
	else//Information Report
	{
		if(prnt==YES) printf("\n nN=%ld, nN2=%ld",nN,nN2);
		
		if(debug==YES)
		{
			typeVector(ixn,nN,"ixn[]");
			typeVector(xn,nN,"xn[]"); 

			typeVector(ixn2,nN2,"ixn2[]");
			typeVector(xn2,nN2,"xn2[]");
			
			typeVector(xb,m,"xb[]"); wpause();
		}//end if
	}//end if-else
	////////////////////////////////////////////////////

	//1.3 preparation for iteration

	for(i=1;i<=m;i++)	cb[i]=c[ixb[i]];
	for(i=1;i<=nN;i++)	cn[i]=c[ixn[i]];
	for(i=1;i<=nN2;i++)	cn2[i]=c[ixn2[i]];

	if(debug==YES)
	{
		typeVector(cb,m,"cb[]"); 
		typeVector(cn,nN,"cn[]");
		typeVector(cn2,nN2,"cn2[]");
	}//end if
	/////////////////////////////////

	z=calZ(cb,xb,cn,xn,cn2,xn2,m,nN,nN2);

	if(prnt==YES) printf("\n\nz=%lf",z); 
	if(debug==YES) wpause(); 
	/////////////////////////////////////////////

	//2: declaration of variables and allocation for iteration 
	
	long *r;
	double *w,*sigma,*yk,*delta,*g;

	long k,l,imin,imax,itrx,il,k_pos,l_pos;
	double sum,max_sigma,delta_k;

	double *_E,**E;
	////////////////////////////////////////////

	makeMatrix(_E,E,m+1,m+1);

	w=(double *)calloc(m+1,sizeof(double));
	yk=(double *)calloc(m+1,sizeof(double));
	sigma=(double *)calloc(n0+1,sizeof(double));

	delta=(double *)calloc(4,sizeof(double));
	r=(long *)calloc(3,sizeof(long));
	g=(double *)calloc(m+1,sizeof(double));

	if(g==NULL || r==NULL || w==NULL)
	{ printf("\nError! --- %ld",__LINE__); exit(0); }
	//////////////////////////////////////////////////////////

	//3: bounded linear programing iteration

	itrx=1;//starting point

LAB001:
	if(prnt==YES) printf("\n\n  ========   ITRX = %ld   ========\n",itrx);
	
	//initialization//////////////////////////////////

	initXN(xn,Lmin,ixn,nN);
	initXN(xn2,Lmax,ixn2,nN2);

	subMatrix(N,a,ixn,m,nN);
	subMatrix(N2,a,ixn2,m,nN2);
	/////////////////////////////////

	for(i=1;i<=m;i++)
		cb[i]=c[ixb[i]];

	for(i=1;i<=nN;i++)
		cn[i]=c[ixn[i]];

	for(i=1;i<=nN2;i++)
		cn2[i]=c[ixn2[i]];
	////////////////////////////////////////////////////

	//(1)	----- calculate w[] and sigma[]

	if(prnt==YES) printf("\n\n(1) Calculate w[] and sigma[]:");

	for(j=1;j<=m;j++)//w[]
	{
		sum=0.0;
		for(i=1;i<=m;i++)
			sum+= cb[i]*B[i][j];

		w[j]=sum;
	}//end for

	if(prnt==YES) typeVector(w,m,"w[]");
	///////////////////////////////////

	for(j=1;j<=nN;j++)//(5-57): R1
	{
		sum=0.0;
		for(i=1;i<=m;i++)
			sum+= w[i]*N[i][j];
	
		sigma[j]=sum-cn[j];
	}//end for

	for(j=1;j<=nN2;j++)//(5-57): R2
	{
		sum=0.0;
		for(i=1;i<=m;i++)
			sum+= w[i]*N2[i][j];

		sigma[nN+j]=cn2[j]-sum;
	}//end for

	if(prnt==YES) typeVector(sigma,nN+nN2,"sigma[]");
	//////////////////////////////////////////////////

	imax=posiMax(sigma,nN+nN2);
	max_sigma=sigma[imax];

	if(imax<=nN)
	{ 
		k=ixn[imax];
		k_pos=LOWER;
	}
	else 
	{
		k=ixn2[imax-nN];
		k_pos=UPPER;
	}//end if-else

	if(prnt==YES) printf("\n\nmax_sigma=%lf, k=%ld",max_sigma,k);
	//////////////////////////////////////////////

	if(max_sigma<=SMLL)
	{ 
		ret=OPTIML; 
		goto LAB099; 
	}
	//////////////////////////////////////////

	if(debug==YES)
	{
		if(k_pos==LOWER) 
			printf("\n\nx[%ld] should increase.",k); 
		else 
			printf("\n\nx[%ld] should decrease.",k); 
	}//end if
	///////////////////////////////////////////////

	//(2)	----- calculate yk[]

	if(prnt==YES) printf("\n\n(2) Calculate yk[]:");

	for(i=1;i<=m;i++)//(5-59): yk[]
	{
		sum=0.0;
		for(j=1;j<=m;j++)
			sum+= B[i][j]*a[j][k];

		yk[i]=sum;
	}//end for

	if(prnt==YES)	typeVector(yk,m,"yk[]");
	if(debug==YES)	wpause();
	//////////////////////////////////////

	//(3)	----- determine delta_k

	if(prnt==YES) printf("\n\n(3) Determine delta_k:");

	if(k_pos==LOWER)
		deltaFunL(delta,r,xb,ixb,yk,Lmin,Lmax,m,k);//(5-62,64,66)
	else
		deltaFunU(delta,r,xb,ixb,yk,Lmin,Lmax,m,k);//(5-70,71,66)

	if(prnt==YES) typeVector(delta,(long)3,"delta[]");
	//////////////////////////////////////////////////////

	imin=posiMin(delta,(long)3);
	delta_k=delta[imin];

	if(imin==1 || imin==2)
	{
		il=r[imin];	//stored as r
		l=ixb[il];	//off_basal obtained
		
		if(prnt==YES)	printf("\nr=il=%ld, l=%ld",il,l);
		if(debug==YES)	wpause();
	}//end if

	if(delta_k>LARG/2.0){ ret=UNBNDD; goto LAB099; }
	//////////////////////////////////////////////////////

	//(4)	----- modify b^[] and z^[]

	if(prnt==YES) printf("\n\n(4) Modify b^[] and z^[]:"); 

	//4.1 obj.func 
	
	z-= max_sigma*delta_k;//No Change!!	//(5-67.1 and 5-69)
	///////////////////////////////////////

	//4.2 basal vector	
	
	for(i=1;i<=m;i++)
    {
		if(k_pos==LOWER)
			xb[i]-= yk[i]*delta_k;  //(5-67.2)
		else
			xb[i]+= yk[i]*delta_k;  //(5-68)
	}//end for
	
	if(prnt==YES) typeVector(xb,m,"xb[]");	
	///////////////////////////////////////////

	//4.3 basal exchange
	
	if(imin==1 || imin==2)
	{
		l_pos=limit(xb,ixb,il,Lmin,Lmax);
		basalEx(ixb,ixn,ixn2,k,k_pos,l,l_pos,m,nN,nN2);

		if(k_pos==LOWER)
			xb[il]=Lmin[k]+delta_k;
		else
			xb[il]=Lmax[k]-delta_k;
		//////////////////////////////////

		if(prnt==YES)
		{
			printf("\n\nz=%lf",z);
			typeVector(xb,m,"xb[]");
		}

		if(debug==YES) wpause();
	}
	else
	{
		if(k_pos==LOWER)
			lower2upper(ixn,ixn2,k,nN,nN2);
		else
			upper2lower(ixn,ixn2,k,nN,nN2);			
	}//end if-else
	//////////////////////////////////

	//(5)	----- modify B`[][]

	if(prnt==YES) printf("\n\n(5) Modify B`[]:");

	if(imin==1 || imin==2)
	{
		//(5.a)

		for(i=1;i<=m;i++)
		{
			if(i==il)
				g[i]=1.0/yk[il];
			else
				g[i]=-yk[i]/yk[il];
		}//end for

		if(debug==YES) typeVector(g,m,"g[]");
		////////////////////////////////////

		//(5.b)

		unitMatrix(E,m);//init.

		for(i=1;i<=m;i++)
			E[i][il]=g[i];

		multiMatrix(B,E,B,m,m,m);//B=E*B[][]

		if(debug==YES){ typeMatrix(B,m,m,"B[][]"); wpause(); }
	}//end if
	//////////////////////////////

	itrx++;
	goto LAB001;
	//////////////////////////////

	//4: conclusion
 
LAB099:
	if(ret==OPTIML)
	{	
		if(feasible(xb,ixb,Lmin,Lmax,m,n)!=YES) ret=NOSOLU;
		if(feasible(xn,ixn,Lmin,Lmax,nN,n)!=YES) ret=NOSOLU;
		if(feasible(xn2,ixn2,Lmin,Lmax,nN2,n)!=YES) ret=NOSOLU;

		solut(x,z,xb,xn,xn2,ixb,ixn,ixn2,m,nN,nN2);
		if(veriSolu(x,a,b,c,mel,Lmin,Lmax,m,n0,NO)!=YES) ret=NOSOLU;
		///////////////////////////////////////////////////

		free(r);	free(w);	free(sigma);
		free(yk);	free(g);	free(delta);

		freeMatrix(_E,E);
	}//end if
	////////////////////////////////////////////////

	freeMatrix(_B,B);
	freeMatrix(_N,N);	
	freeMatrix(_N2,N2);

	free(ixb);	free(ixn);	free(ixn2);
	free(xb);	free(xn);	free(xn2);
	free(cb);	free(cn);	free(cn2);
	///////////////////////////////////////////

	return ret;//NOSOLU, OPTIML, or UNBNDD
}

void lower2upper(long *ixn,long *ixn2,long k,long &nN,long &nN2)
{
	register long i,ik;

	ik=lookup(ixn,k,nN);
	if(ik==NO){ printf("\nError! --- %ld",__LINE__); exit(0); }

	for(i=ik;i<=nN-1;i++)
		ixn[i]=ixn[i+1];

	nN--;
	////////////////////

	nN2++;
	ixn2[nN2]=k;
	///////////////////

	return;
}

void upper2lower(long *ixn,long *ixn2,long k,long &nN,long &nN2)
{
	register long i,ik;

	ik=lookup(ixn2,k,nN2);
	if(ik==NO){ printf("\nError! --- %ld",__LINE__); exit(0); }

	for(i=ik;i<=nN2-1;i++)
		ixn2[i]=ixn2[i+1];

	nN2--;
	///////////////

	nN++;
	ixn[nN]=k;
	/////////////

	return;
}

void basalEx(long *ixb,long *ixn,long *ixn2,long k,long k_pos,long l,long l_pos,long m,long &nN,long &nN2)
{
        register long i,ik,il;

        il=lookup(ixb,l,m);
        if(il==NO){ printf("\nError! --- %ld",__LINE__); exit(0); }

        ixb[il]=k;//No Change!

        if(k_pos==LOWER && l_pos==LOWER)
        {
                ik=lookup(ixn,k,nN);
                ixn[ik]=l;
        }
        else if(k_pos==UPPER && l_pos==UPPER)
        {
                ik=lookup(ixn2,k,nN2);
                ixn2[ik]=l;
        }
        else if(k_pos==LOWER && l_pos==UPPER)
        {
                ik=lookup(ixn,k,nN);
                for(i=ik;i<=nN-1;i++)
                        ixn[i]=ixn[i+1];

                nN--;
                ////////////////////////

                nN2++;
                ixn2[nN2]=l;
        }
        else if(k_pos==UPPER && l_pos==LOWER)
        {
                ik=lookup(ixn2,k,nN2);
                for(i=ik;i<=nN2-1;i++)
                        ixn2[i]=ixn2[i+1];

                nN2--;
                //////////////////////

                nN++;
                ixn[nN]=l;
        }
        else
        {
                printf("\n\nError! --- %ld",__LINE__);
		printf("\n  k_pos=%ld, l_pos=%ld",k_pos,l_pos);
  
                exit(0);
        }//end if-else
        ///////////////////////

	return;
}

long indexSets(long *ixn,long *ixn2,double **N,double **N2,double *xn,double *xn2,double *xb,long *ixb,double **B,
		double **a,double *b,double *Lmin,double *Lmax,long m,long n0,long n,long &nN,long &nN2,long prnt)
{
	if(prnt==YES) printf("\n\nindexSets()...");

	long ret=NO;//init
	register long i;
	//////////////////////////////

	//1: all_lower presumption
	
	nN=n0; nN2=0;

	for(i=1;i<=n0;i++)
	{
		ixn[i]=i;
		xn[i]=Lmin[i];
	}//end for
	///////////////////////////

	subMatrix(N,a,ixn,m,nN);
    subMatrix(N2,a,ixn2,m,nN2);

	calXB(xb,b,B,N,N2,xn,xn2,m,nN,nN2);//(5-53)
	ret=feasible(xb,ixb,Lmin,Lmax,m,n);	

	if(ret==YES) return ret;	
	/////////////////////////////////
	
	//2: all_upper presumption

	nN=0; nN2=n0;

	for(i=1;i<=n0;i++)
	{
		ixn2[i]=i;
		xn2[i]=Lmax[i];
	}//end for
	/////////////////

	subMatrix(N,a,ixn,m,nN);
	subMatrix(N2,a,ixn2,m,nN2);

    calXB(xb,b,B,N,N2,xn,xn2,m,nN,nN2);//(5-53)
    ret=feasible(xb,ixb,Lmin,Lmax,m,n);

	if(ret==YES) return ret;
	//////////////////////////////////


	return ret;
}

void initXN(double *xn,double *Lm,long *ixn,long nN)
{
	register long i;

	quickSort(ixn,nN);	//sorting ixn[]

	for(i=1;i<=nN;i++)
		xn[i]=Lm[ixn[i]];

	return;
}

void calXB(double *xb,double *b,double **B,double **N,double **N2,double *xn,double *xn2,long m,long nN,long nN2)
//B[][]: inversed matrix as shown in (5-53) at p. 190
{
	register long i,j;
	double sum1,sum2,sum3;

	multiMatrix(N,B,N,m,m,nN);//N=B*N[][]
	multiMatrix(N2,B,N2,m,m,nN2);//N2=B*N2[][]

	for(i=1;i<=m;i++)
	{
		sum1=0.0;
		for(j=1;j<=m;j++)
			sum1+= B[i][j]*b[j];

		sum2=0.0;
		for(j=1;j<=nN;j++)
			sum2+= N[i][j]*xn[j];

		sum3=0.0;
		for(j=1;j<=nN2;j++)
			sum3+= N2[i][j]*xn2[j];

		xb[i]=sum1-sum2-sum3;
	}//end for
	
	return;
}

long feasible(double *x,long *ix,double *Lmin,double *Lmax,long m,long n)
//m: size of x[] and ix[]; n: size of Lmin[] and Lmax[].
{
	long ret=YES;
	register long i,k;
	/////////////////////////

	for(i=1;i<=m;i++)
	{
		k=ix[i];
		if(k<1 || k>n || x[i]<Lmin[k] || x[i]>Lmax[k])
		{
			ret=NO; 
			break; 	
		}
	}//end for

	return ret;
}

void deltaFunU(double *delta,long *r,double *xb,long *ixb,double *yk,double *Lmin,double *Lmax,long m,long k)
{
	register long i,imin;
	double *ratio;

	ratio=(double *)calloc(m+1,sizeof(double));
	if(ratio==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }
	/////////////////////////////////////////////////////

	//1: (5-70)

	for(i=1;i<=m;i++)
	{
		if(yk[i]<0.0)
			ratio[i]=(xb[i]-Lmin[ixb[i]])/(-yk[i]);
		else
			ratio[i]=LARG;
	}//end for
	///////////////////

	imin=posiMin(ratio,m);

	delta[1]=ratio[imin];
	r[1]=imin;
	///////////////////////////
		
	//2: (5-71)
	
	for(i=1;i<=m;i++)
	{
		if(yk[i]>0.0)
			ratio[i]=(Lmax[ixb[i]]-xb[i])/yk[i];
		else
			ratio[i]=LARG;
	}//end for
	///////////////////

	imin=posiMin(ratio,m);

	delta[2]=ratio[imin];
	r[2]=imin;
	////////////////////////////

	//3: (5-65)

	delta[3]=Lmax[k]-Lmin[k];
	//////////////////////////////

	free(ratio);
	return;
}

void deltaFunL(double *delta,long *r,double *xb,long *ixb,double *yk,double *Lmin,double *Lmax,long m,long k)
{
	register long i,imin;
	double *ratio;

	ratio=(double *)calloc(m+1,sizeof(double));
	if(ratio==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }
	//////////////////////////////////
	
	//1: (5-62)
	
	for(i=1;i<=m;i++)
	{
		if(yk[i]>0.0)
			ratio[i]=(xb[i]-Lmin[ixb[i]])/yk[i];
		else
			ratio[i]=LARG;
	}//end for

	imin=posiMin(ratio,m);

	delta[1]=ratio[imin];
	r[1]=imin;
	///////////////////////////
		
	//2: (5-64)
		
	for(i=1;i<=m;i++)
	{
		if(yk[i]<0.0)
			ratio[i]=(Lmax[ixb[i]]-xb[i])/(-yk[i]);
		else
			ratio[i]=LARG;
	}//end for
	/////////////////

	imin=posiMin(ratio,m);

	delta[2]=ratio[imin];
	r[2]=imin;
	////////////////////////////

	//3: (5-65)

	delta[3]=Lmax[k]-Lmin[k];
	///////////////////////////////

	free(ratio);
	return;
}
	
void solut(double *x,double z,double *xb,double *xn,double *xn2,
		   long *ixb,long *ixn,long *ixn2,long m,long nN,long nN2)
{
	register long i;

	x[0]=z;//derived from iteration

	for(i=1;i<=m;i++)
		x[ixb[i]]=xb[i];

	for(i=1;i<=nN;i++)
		x[ixn[i]]=xn[i];

	for(i=1;i<=nN2;i++)
		x[ixn2[i]]=xn2[i];
	/////////////////////////
	
	return;
}

long limit(double *x,long *ix,long indx,double *Lmin,double *Lmax)
{
	long ret=MIDDL;

	if(fabs(x[indx]-Lmin[ix[indx]])<SMLL)		ret=LOWER;

	else if(fabs(x[indx]-Lmax[ix[indx]])<SMLL)	ret=UPPER;

	return ret;
}

double calZ(double *cb,double *xb,double *cn,double *xn,double *cn2,double *xn2,long m,long nN,long nN2)
{
	register long i;
	double sum=0.0;

	for(i=1;i<=m;i++)	sum+= cb[i]*xb[i];

	for(i=1;i<=nN;i++)	sum+= cn[i]*xn[i];

	for(i=1;i<=nN2;i++)	sum+= cn2[i]*xn2[i];

	return sum;
}

void baseMatrix(double **B,double **a,long *ixb,long m)
{
	register long i,j,k;

	for(j=1;j<=m;j++)
	{
		k=ixb[j];
		for(i=1;i<=m;i++)
			B[i][j]=a[i][k];
	}//end for

	return;
}

long veriSolu(double *x,double **a,double *b,double *c,long *mel,double *Lmin,double *Lmax,long m,long n0,long prnt)
{
	register long i,j;
	
	long ret=YES;
	double sum;
	//////////////////////////////////////////

	//1: obj.func

	sum=0.0;
	for(i=1;i<=n0;i++) sum+= c[i]*x[i];

	if(fabs(sum-x[0])>EPS)//No Change!
	{
		if(prnt==YES)
		{
			printf("\n\nError! --- %ld",__LINE__);
			printf("\n  obj.valu: z=%lf, while cx=%lf",x[0],sum);
			wpause();
		}//end if

		ret=NO;
	}//end if
	//////////////////////////////////

	//2: constraints

	for(i=1;i<=m;i++)
	{
		sum=0.0;
		for(j=1;j<=n0;j++)
			sum+= a[i][j]*x[j];
		///////////////////////////

		if(mel[i]==EQ && fabs(sum-b[i])>EPS) ret=NO;
		if(mel[i]==LT && sum>=b[i]+EPS) ret=NO;
		if(mel[i]==MT && sum<=b[i]-EPS) ret=NO;
	}//end for
	/////////////////////////////

	//3: bounds of variables

	for(i=1;i<=n0;i++)
	{
		if(x[i]>Lmax[i] || x[i]<Lmin[i])
		{
			if(prnt==YES)
			{
				printf("\n\nError! --- %ld",__LINE__);
				printf("\n  Bounds Violated: # %ld",i);
			}//end if

			ret=NO;
		}//end if
	}//end for
	////////////////////////

	//4: info.report

	if(ret==NO && prnt==YES)
	{
		typeVector(x,(long)0,n0,"x[]");
		wpause();
	}//end if
	////////////////////////////

	return ret;
}

long getNumEQ(long *mel,long m)
{
	register long i,k=0;

	for(i=1;i<=m;i++)
		if(mel[i]==EQ) k++;

	return k;
}

long getNumLT(long *mel,long m)
{
	register long i,k=0;

	for(i=1;i<=m;i++)
		if(mel[i]==LT) k++;

	return k;
}

long getNumSR(double *b,long *mel,long m)
{
	register long i,k=0;

	for(i=1;i<=m;i++)
	{
		if(b[i]>0.0 && mel[i]==MT) k++;
		if(b[i]<0.0 && mel[i]==LT) k++;
	}//end for
	////////////////////

	return k;
}

void standard(double **a,double *b,double *c,double *Lmin,double *Lmax,
			  long *mel,long m,long &n0,long prnt)
{
	if(prnt==YES) printf("\n\nstandard()...");

	register long i,j,nSR=0;

	//1: making b[] >=0

	for(i=1;i<=m;i++)
	{
		if(b[i]<0.0)
		{
			for(j=1;j<=n0;j++)
				a[i][j]*= -1.0;

			mel[i]*= -1;
			b[i]*= -1.0;
		}//end if

		if(mel[i]==MT) nSR++;
	}//end for

	if(prnt==YES) printf("\n nSR=%ld",nSR);
	//if(prnt==YES) typeVector(mel,m,"mel[]");
	/////////////////////////

	//2: considering surplus

	for(j=1;j<=nSR;j++)
		for(i=1;i<=m;i++)
			a[i][n0+j]=0.0;//init.

	for(j=1;j<=nSR;j++)
	{
		c[n0+j]=0.0;
		Lmin[n0+j]=0.0;
		Lmax[n0+j]=(double)BIGNUM;
		///////////////////////////

		i=lookup2(mel,(long)MT,m);
		if(i==NO){ printf("\n\nError! --- %ld",__LINE__); exit(0); }

		a[i][n0+j]=-1.0;
		mel[i]=EQ;
	}//end for

	n0+= nSR;//No Change!
	////////////////////////////

	return;
}

//end of file
