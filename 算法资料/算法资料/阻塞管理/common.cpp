///common.cpp by Wang GT
#include"common.h"
///////////////////////////////////////////////

/*
void consoleMsg(char *msg,char *msg_exp)
{
	//to do:
	return;
}

void consoleMsg(char *msg,char *msg_exp,char *file,long line)
{
	//to do:
	return;
}
*/

void printSparMatrix(FILE *out,double **matrix,long n,char caption[])
{ printSparMatrix(out,matrix,n,n,caption); return; }

void printSparMatrix(FILE *out,double **matrix,long m,long n,char caption[])
{
	register long i,j;
	fprintf(out,"\n%s",caption);
	/////////////////////////////////////////

	for(i=1;i<=m;i++)
	for(j=1;j<=n;j++)
	{
		if(fabs(matrix[i][j])<=SMLL) continue;
		fprintf(out,"\n%6ld %6ld % 16.8lf",i,j,matrix[i][j]);
	}//end for
	//////////////////////////////////

	return;
}

void readMatrix(char filename[],char caption[],double **matrix,long n)
{ readMatrix(filename,caption,matrix,n,n); return; }

void readMatrix(char filename[],char caption[],double **matrix,long m,long n)
{
	register long i,j;
	double fa;
	///////////////////////////////////////////////////

	FILE *in=fopen(filename,"r");
	if(in==NULL){ printf("\n\nCannot open %s",filename); exit(0); }
	
	//1: positioning datablock

	do{
			fgets(buf,5,in);//No Change!
			if(feof(in))
			{
				printf("\n\nError! --- %ld",__LINE__);
				printf("\n  Cannot find %s data in %s\n",caption,filename);

				exit(0); 
			}
	}while(strcmp(buf,caption));
	///////////////////////////////////////

	//2: reading data

	for(i=1;i<=m;i++)
	for(j=1;j<=n;j++)
	{
		if(fscanf(in,"%lf",&fa)==1) matrix[i][j]=fa;
		else
		{
			printf("\n\nError! --- %ld",__LINE__);
			exit(0);
		}		
	}//end for
	////////////////////////////////////

	fclose(in);
	return;
}

void readSparMatrix(char filename[],char caption[],double **matrix,long n)
{ readSparMatrix(filename,caption,matrix,n,n); return; }

void readSparMatrix(char filename[],char caption[],double **matrix,long m,long n)
{
	FILE *in=fopen(filename,"r");
	if(in==NULL){ printf("\n\nCannot open %s\n",filename); exit(0); }

	//1: init. and positioning

	initMatrix(matrix,m,n,0.0);//init.
	do{
			fgets(buf,5,in);//No Change!
			if(feof(in))
			{
					printf("\n\nError! --- %ld",__LINE__);
					printf("\n  Cannot find %s data in %s\n",caption,filename);
					exit(0);
			}
	}while(strcmp(buf,caption));
	////////////////////////////////////////

	//2: reading data

	long ia,ib;
 	double fa;
	
	while(1)//unconditional loop
	{
		if(fscanf(in,"%ld%ld%lf",&ia,&ib,&fa)!=3) break;
		
		if(ia>=1 && ia<=m && ib>=1 && ib<=n) matrix[ia][ib]=fa;
		else
		{
			printf("\n\nError! --- %ld",__LINE__);

			printf("\n  filename=%s",filename);
			printf("\n  caption =%s",caption);
			
			printf("\n  ia,m = %ld,%ld",ia,m);
			printf("\n  ib,n = %ld,%ld",ib,n);

			exit(0);
		}		
	}//end for
	////////////////////////////////////////

	fclose(in);
	return;
}

void readVector(char filename[],char caption[],double *vector,long n)
{ readVector(filename,caption,vector,(long)1,n); return; }

void readVector(char filename[],char caption[],double *vector,long n1,long n2)
{
	register long i;
	double fa;
	///////////////////////////////////////////////////////
	
	//1: positioning datablock

	FILE *in=fopen(filename,"r");
	if(in==NULL){ printf("\n\nCannot open %s\n",filename); exit(0); }

	do{
			fgets(buf,5,in);//No Change!
			if(feof(in))
			{
					printf("\n\nError! --- %ld",__LINE__);
					printf("\n  Cannot find %s data in %s\n",caption,filename);

					exit(0);
			}
	}while(strcmp(buf,caption));
	/////////////////////////////////////////////////

	//2: reading data

	for(i=n1;i<=n2;i++)
	{
		if(fscanf(in,"%lf",&fa)==1) vector[i]=fa;
		else
		{
			printf("\n\nError! --- %ld",__LINE__);
			
			printf("\n  filename=%s",filename);
			printf("\n  caption =%s",caption);

			printf("\n  n1 = %ld",n1);
			printf("\n  n2 = %ld",n2);

			exit(0);
		}//end else
	}//end for
	///////////////////////////////////////

	fclose(in);
	return;
}

void readVector(char filename[],char caption[],long *vector,long n)
{ readVector(filename,caption,vector,(long)1,n); return; }

void readVector(char filename[],char caption[],long *vector,long n1,long n2)
{

	register long i;
	long ia;
	///////////////////////////////////////////////

	FILE *in=fopen(filename,"r");
	if(in==NULL){ printf("\n\nCannot open %s\n",filename); exit(0); }

	//1: positioning

	do{
			fgets(buf,5,in);//No Change!
			if(feof(in))
			{
                        printf("\n\nError! --- %ld",__LINE__);
                        printf("\n  Cannot find %s data in %s\n",caption,filename);
                        exit(0);
			}
	}while(strcmp(buf,caption));
	///////////////////////////////////////////

	//2: reading data
	
	for(i=n1;i<=n2;i++)
	{
		if(fscanf(in,"%ld",&ia)==1) vector[i]=ia;
		else
		{
			printf("\n\nError! --- %ld",__LINE__);
			exit(0);
		}//end else		
	}//end for
	/////////////////////////////////

	fclose(in);
	return;
}

void printVector(FILE *out,long *vector,long n,char caption[])
{ printVector(out,vector,(long)1,n,caption); return; }

void printVector(FILE *out,long *vector,long n1,long n2,char caption[])
{
	register long i;
	fprintf(out,"\n%s",caption);

	for(i=n1;i<=n2;i++)
		fprintf(out,"\n% 6ld",vector[i]);
	
	return;
}

void printVector(FILE *out,double *vector,long n,char caption[])
{ printVector(out,vector,(long)1,n,caption); return; }

void printVector(FILE *out,double *vector,long n1,long n2,char caption[])
{
	register long i;
	fprintf(out,"\n%s",caption);

	for(i=n1;i<=n2;i++)
		fprintf(out,"\n% 16.8lf",vector[i]);

	return;
}

template<class TYPE>
long findk_ex(TYPE *x,long n,TYPE xk)
{
        register long i;
        long ret=NO;//init.

        if(xk<=x[1]) return 1;//first segment
        if(xk>=x[n]) return n-1;//No Change!

        for(i=1;i<=n-1;i++)
        {
                if(xk>=x[i] && xk<x[i+1]){ ret=i; break; }
                if(xk<=x[i] && xk>x[i+1]){ ret=i; break; }
        }//end for
        /////////////////////////////////

        return ret;
}

void addLeadingChar(char *string,char ch,long len)
{
	register long i;
	long k,n;

	n=strlen(string);
	if(n>=len) return;
	////////////////////////////

	k=len-n;
	for(i=len;i>=k;i--) string[i]=string[i-k];
	for(i=0;i<k;i++) string[i]=ch;
	///////////////////////////////

	return;
}

void chopString(char *part1,char *part2,char *string,char ch)
{
	register long i;
	long k,n;
	/////////////////////////////////

	n=strlen(string);
	k=lookup(string,ch,n);

	if(k==NO)
	{
		printf("\n\nError! --- %ld",__LINE__);
		printf("\n  Invalid Character: ch=%c",ch);

		exit(0);
	}//end if
	///////////////////////////////

	for(i=0;i<k;i++) part1[i]=string[i];
	part1[k]='\0';
	
	for(i=k+1;i<n;i++) part2[i-k-1]=string[i];
	part2[n-k-1]='\0';
	/////////////////////////////

	return;
}
	 
void mdyID(char *month,char *day,char *year,char *string)
{
	char buf[20];

	chopString(month,buf,string,'/');
	chopString(day,year,buf,'/');

	return;
}

void packString(char *string)
{
        register long i;
        long k,n=strlen(string);//init.
        
		char ch,*string_tmp;
        ////////////////////////////////////

        string_tmp=(char *)calloc(n+1,sizeof(char));
        if(string_tmp==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }
        ////////////////////////////////////////////////////

        initString(string_tmp,n);
        k=0;//init.

        for(i=0;i<=n-1;i++)
        {
                ch=string[i];
                if(ch=='_'){ string_tmp[k]=ch; k++; }

                if(ch>='A' && ch<='Z'){ string_tmp[k]=ch; k++; }
                if(ch>='a' && ch<='z'){ string_tmp[k]=ch; k++; }
                if(ch>='0' && ch<='9'){ string_tmp[k]=ch; k++; }
        }//end for
        ////////////////////////////////////////

        strcpy(string,string_tmp);
        free(string_tmp);
        /////////////////////////////////

        return;
}

template<class TYPE>
double statMSD(TYPE *v,long n)
{
	register long i;
	double av,aux2,aux4;
	///////////////////////////

	aux2=0.0;//init.
	av=(double)sum(v,n)/(double)n;

	for(i=1;i<=n;i++)
	{
		aux4=v[i]-av;		
		aux2+= aux4*aux4;
	}//end for

	return aux2/n;
}

template<class TYPE>
void shuffVector(TYPE *vector,long n)
{
	register long i;
	long r1,r2;

	for(i=1;i<=2*n;i++)
	{
		r1=(long)(frand(1.0,n)+0.5);
		r2=(long)(frand(r1,n)+0.5);

		swapElems(vector,r1,r2);
	}//end for
	/////////////////////

	return;
}

void natrualVector(long *v,long n)
{
	register long i;
	for(i=1;i<=n;i++) v[i]=i;
	
	return;
}
	
long conditionMeet(double v,double value,long mel)
{
	long ret=NO;//init.

	if(mel==EQ && fabs(v-value)<SMLL) ret=YES;
	if(mel==LT && v<value) ret=YES;
	if(mel==MT && v>value) ret=YES;
		
	return ret;
}

long melFun(char melstr[])
{
	stringEnd(melstr,' ');//init.
	
	if(!strcmp(melstr,"<")) return LT;
	if(!strcmp(melstr,"=")) return EQ;
	if(!strcmp(melstr,">")) return MT;

	if(!strcmp(melstr,"<=")) return LT;
	if(!strcmp(melstr,">=")) return MT;

	return EQ;//useless
}

template<class TYPE>
double interLin(TYPE *v,long n,long m,long k)
{
//Notation:////////////////////////////////////
// v[]	--- n dimension
// m	--- mapped-into dimension
// k	--- mapped v[k] returned
///////////////////////////////////////////////
 
	register long i;
        long j;
        double k0,aux2,aux4,ret;
        ///////////////////////////////////

        k0=(m-1.0)/(n-1.0);//No Change!
        for(i=2;i<=n;i++)
        {
                j=1+(i-1)*k0;
                if(j>=k)
                {
                        aux2=(k-1.0)/k0+1.0;
                        aux4=aux2-(i-1);

                        ret=v[i-1]+(v[i]-v[i-1])*aux4;
                        break;
                }
        }//end for
        //////////////////////

        return ret;
}

template<class TYPE>
void mirrorSym(TYPE *vector,long n1,long n2)
{
        register long i,k;
        TYPE aux2;
        long ns=(long)((n1+n2)/2.0);//No Change!

        for(i=n1;i<=ns;i++)
        {
                k=n2-i+n1;//sym.index 
                aux2=vector[i];
                
                vector[i]=vector[k];    
                vector[k]=aux2;
        }//end for
        ////////////////////////////////   

        return;
}

void delLeadingZero(char *numString)
{
	register long i,len;

	while(1)//unconditional loop
	{
		if(numString[0]!='0') break;
		
		len=strlen(numString);
		for(i=0;i<len;i++) 
			numString[i]=numString[i+1];
	}//end while 
	//////////////////////////

	return;
}

template<class TYPE>
TYPE* chainNode(TYPE *chain,long index)
{
	if(index<=0) return NULL;

	register long k=0;//init.
	TYPE *cp=chain;

	while(cp!=NULL)
	{
		k++;
		if(k==index) break;

		cp=cp->next;
	}//end while
	//////////////////////

	return cp;
}

template<class TYPE>
void trunChain(TYPE *&chain,long len)
//truncating chain into a len-sized subchain
{
	if(len<=0){ freeChain(chain); return; }
	if(len>=chainLen(chain)) return;
	////////////////////////////////////

	TYPE *cp;
	cp=chainNode(chain,len);
	
	freeChain(cp->next);
	cp->next=NULL;
	///////////////////////////////

	return;
}

template<class TYPE>
TYPE sum(TYPE *vector,long n)
{ return sum(vector,(long)1,n); }

template<class TYPE>
TYPE sum(TYPE *vector,long n1,long n2)
{
	register long i;
	TYPE ret=(TYPE)0.0;

	for(i=n1;i<=n2;i++)
		ret+= vector[i];

	return ret;
}

template<class TYPE>
long posiChainNode(TYPE *chain,TYPE *node)
{
	register long i;
	long n,ret=NO;//init.
	/////////////////////////////

	n=chainLen(chain);
	TYPE *cp=chain;
	
	for(i=1;i<=n;i++,cp=cp->next)
	{
		if(cp==node)
		{ 
			ret=i;
			break;
		}
	}//end for
	////////////////////////

	return ret;
}	

long diffMatrix(double **m1,double **m2,long n,char caption[])
{ long ret=diffMatrix(m1,m2,n,n,caption); return ret; }

long diffMatrix(double **m1,double **m2,long m,long n,char caption[])
{
	long ret=NO;
	register long i,j;
	
	for(i=1;i<=m;i++)
	for(j=1;j<=n;j++)
	{
		if(fabs(m1[i][j]-m2[i][j])<SMLL) continue;

		printf("\n\nWarning! --- %ld",__LINE__);
		printf("\n  %s: [%ld][%ld]=%lf, %lf",caption,i,j,m1[i][j],m2[i][j]);
		
		ret=YES;
	}//end for 
	//////////////////////////

	return ret;
}

void wait(double sec)
{
	clock_t start=clock();
	double laps;
	
	while(1)//unconditional loop
	{
		laps=(double)(clock()-start)/CLOCKS_PER_SEC;
		if(laps>=sec) break;
	}//end while
	/////////////////////
	
	return;
}

void splin(double *ys,double *ys1,double *x,double *y,double *xs,long n,long ns)
{
//NOTATION:////////////////////////////////////////////////
// ys[]		--- solution, spline func.value, ns dimension
// ys1[]	--- solution, first derivative, ns dimension
// x[], y[]	--- sampled data, n dimension
// xs[]		--- given data, ns dimension
// n		--- dimension of x[], y[]: 0,1,...n
// ns		--- dimension of xs[],ys[],ys1[]: 0,1,...ns
///////////////////////////////////////////////////////////

	register long i,j,k;
  
	double aux2,aux4,haux,xb,xj;
	double ld,dd,un,dn;
	double *ys2;

	ys2=(double *)calloc(n+2,sizeof(double));
	if(ys2==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }

	ld=dd=un=dn=0.0;//init.
	////////////////////////////////

	//1: ys2[]
	splinfun(ys2,x,y,ld,dd,un,dn,n);
	////////////////////////////////////

	//2: ys[] and ys1[]

	for(j=1;j<=ns;j++)
	{
		//2.1 find k

		k=1;
		for(i=1;i<=n-1;i++)
		{ 
			if(xs[j]<=x[i]){ k=i; break;}
			else k=i+1;
		}//end for
		//////////////////////

		//2.2 preparation

		haux=x[k]-x[k-1];

		xb=x[k]-xs[j];	//xs[j] <===> x
		xj=xs[j]-x[k-1];
		//////////////////////////

		//2.3 cal. ys[]

		aux2=y[k-1]-ys2[k-1]*haux*haux/6.0;
		aux4=y[k]-ys2[k]*haux*haux/6.0;
		aux2=aux2*xb+aux4*xj;

		ys[j]=(ys2[k-1]*pow(xb,3.0)/6.0+ys2[k]*pow(xj,3.0)/6.0+aux2)/haux;
		/////////////////////////////////////////////////////////		

		//2.4 cal. ys1[]

		aux2=-ys2[k-1]*xb*xb/(2.0*haux);
		aux4=ys2[k]*xj*xj/(2.0*haux);
		aux2=aux2+aux4;

		ys1[j]=aux2+(y[k]-y[k-1])/haux-(ys2[k]-ys2[k-1])*haux/6.0;
	}//end for
	/////////////////////////

	free(ys2);

	return;
}

void splinfun(double *ys2,double *x,double *y,double ld,double dd,double un,double dn,long n)
{ 
//NOTATION:////////////////////////////////////////////////////
// ys2[]		--- solution, the second derivative
// x[], y[]		--- data
// n			--- data index: 0,1,2, ... n
// ld,dd,un,dn	--- boundary condition: usu. 0.0
//////////////////////////////////////////////////////////

	long i,k,n2;
	double *a,*b,*c,*h,*q,*p;
	////////////////////////////////////

	a=(double *)calloc(n+2,sizeof(double));
	b=(double *)calloc(n+2,sizeof(double));
	c=(double *)calloc(n+2,sizeof(double));
	h=(double *)calloc(n+2,sizeof(double));
	p=(double *)calloc(n+2,sizeof(double));
	q=(double *)calloc(n+2,sizeof(double));
	if(a==NULL || p==NULL || q==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }
	/////////////////////////////////////////////////////////////

	//1: initialization

	for(i=0;i<=n;i++)
		ys2[i]=y[i];

	for(k=1;k<=2;k++)
		for(i=n;i>=k;i--)
			ys2[i]=(ys2[i]-ys2[i-1])/(x[i]-x[i-k]);

	h[1]=x[1]-x[0];
	for(i=1;i<=n-1;i++)
	{
		h[i+1]=x[i+1]-x[i];
		c[i]=h[i+1]/(h[i]+h[i+1]);
		b[i]=2.0;
		a[i]=1.0-c[i];
		ys2[i]=6.0*ys2[i+1];
	}//end for

	ys2[0]=dd;
	ys2[n]=dn;
	a[n]=un;
	b[0]=2.0;
	b[n]=2.0;
	c[0]=ld;
	////////////////////////////

	//2: tss solution

	for(i=n+1;i>=1;i--)
	{
		a[i]=a[i-1]; b[i]=b[i-1];
		c[i]=c[i-1]; ys2[i]=ys2[i-1];
	}//end for
	
	n2=n+1;
	tssfun(ys2,a,b,c,ys2,q,p,n2);
	for(i=0;i<=n;i++)
		ys2[i]=ys2[i+1];
	///////////////////////////////
	
	free(a);free(b);free(c);
	free(h);free(p);free(q);
  
	return;
}

void tssfun(double *x,double *a,double *b,double *c,double *d,double *q,double *p,long n)
{
	register long k;
	double *y;
  
	y=(double *)calloc(n+1,sizeof(double));
	if(y==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }
	/////////////////////////////////////////

	q[1]=b[1];//init.
	y[1]=d[1];

	for(k=2;k<=n;k++)
	{
		p[k]=a[k]/q[k-1];
		q[k]=b[k]-p[k]*c[k-1];
		y[k]=d[k]-p[k]*y[k-1];
	}//end for
	////////////////////////////

	x[n]=y[n]/q[n];
	for(k=n-1;k>=1;k--)
		x[k]=(y[k]-c[k]*x[k+1])/q[k];
	//////////////////////////////////

	free(y);
	return;
}

double frand(double num)
{ return frand(0.0,num); }//srand(unsigned(time(NULL)));

double frand(double lower,double upper)
{
	double r,ret;
	
	r=(double)rand()/RAND_MAX;
	ret=(1.0-r)*lower+r*upper;

	return ret;
}

template<class TYPE>
void showChain2(TYPE *chain,char caption[])
{
	printf("\n\n%s:",caption);

	TYPE *cp=chain;
	while(cp!=NULL)
	{
		printf("\n i=%ld j=%ld",cp->i,cp->j);
		cp=cp->next;
	}//end while

	return;
}

template<class TYPE>
void showChain(TYPE *chain,char caption[])
{
	printf("\n\n%s:",caption);

	TYPE *cp=chain;
	while(cp!=NULL)
	{
		printf("\n i=%ld",cp->i);
		cp=cp->next;
	}//end while

	return;
}

template<class TYPE>
TYPE* findNode(long i,TYPE *chain)
{
	TYPE *cp=chain;
	while(cp!=NULL)
	{
		if(cp->i==i) break;

		cp=cp->next;
	}//end while

	return cp;
}

template<class TYPE>
TYPE* frontNode(TYPE *node,TYPE *chain)
{
	TYPE *cp=chain;
	while(cp!=NULL)
	{
		if(cp->next==node) break;

		cp=cp->next;
	}//end while

	return cp;
}

template<class TYPE>
void swapChainNodes(TYPE *&node1,TYPE *&node2,TYPE *&chain)
{
	TYPE *cp1,*cp2,*node1Next,*node2Next;
	TYPE *node1_sv=node1,*node2_sv=node2;
	//////////////////////////////////////

	if(node1==node2) return;//No swapping needed

	if(node1==chain || node2==chain)//involving head
	{
		if(node2==chain) swap(node1,node2);

		cp2=frontNode(node2,chain);
		//////////////////////////////

		cp2->next=node1;
		node1Next=node1->next;
		node1->next=node2->next;

		node2->next=node1Next;
		chain=node2;
	}
	else//chainhead not involved
	{
		if(node1->next==node2 || node2->next==node1)
		{
			if(node2->next==node1) swap(node1,node2);

			cp1=frontNode(node1,chain);
			cp1->next=node2;
			node2Next=node2->next;
			node2->next=node1;
			node1->next=node2Next;
		}
		else
		{
			cp1=frontNode(node1,chain);
			cp2=frontNode(node2,chain);

			cp1->next=node2;
			node2Next=node2->next;//reserved
			node2->next=node1->next;

			cp2->next=node1;
			node1->next=node2Next;
		}
	}//end if-else
	//////////////////////////////
		
	node1=node2_sv;
	node2=node1_sv;

	return;
}

double vsimp(double (*fun)(double),double a,double b,double eps,double h0)
{
//NOTATION:///////////////////////////////////////////////////
//	vsimp	--- varying-step Simpson integral value returned
//	[a,b]	---	integral interval
//	esp		--- convergent precision
//	h0		---	step threshold: convergent judgement not made
//				until steps less than h0
//////////////////////////////////////////////////////////////

	register long i,j,n;
	double h,x,sc,sp,sum,sump;
	//////////////////////////////////

	n=1;//init.

	h=(b-a)/2.0;
	x=a+h;

	sp=(*fun)(a)+(*fun)(b)+2.0*(*fun)(x);
	sum=(sp+2.0*(*fun)(x))*h/3.0;
	////////////////////////////////////

	for(i=2;;i++)//unconditional loop
	{
		sump=sum;
		n+= n;//No Change!

		sc=0.0;
		x=a-0.5*h;

		for(j=1;j<=n;j++)
		{
			x+= h;
			sc+= (*fun)(x);
		}//end for

		h*= 0.5;
		sum=(4.0*sc+sp)*h/3.0;
		////////////////////////////

		if(fabs(h)<h0 && fabs(sum-sump)<eps) break;
		else sp+= 2.0*sc;

		if(i>BIGNUM)
		{
			printf("\n\nWarning! --- vsim(): %ld",__LINE__);
			printf("\n  NOT Convergent after %ld Iterations.",i);
			
			wpause(); break;
		}//end if
	}//end for
	///////////////////////////

	return sum;
}

template<class TYPE>
long posiDiff(TYPE *v1,TYPE *v2,long n1,long n2)
{
	register long i;
	long n,ret;

	n=min(n1,n2);
	ret=n+1;//init.

	for(i=1;i<=n;i++)
	{
		if(fabs(v1[i]-v2[i])>SMLL)
		{
			ret=i;
			break;
		}
	}//end for
	///////////////////

	return ret;
}

void invMatrixGJ(double **a,long n)
//invMatrix by Gauss-Jordan method
{
	register long i,j,k;
	long imax;
	double max,aux2;

	double *_E,**E;
	/////////////////////////////

	//1: initialization

	makeMatrix(_E,E,n+1,n+1);
	unitMatrix(E,n);//init.
	///////////////////////////

	//2: Gauss-Jordan expunction

	for(k=1;k<=n;k++)
	{
		//2.1 pivot selection

		imax=k;//init.
		max=fabs(a[k][k]);

		for(i=k+1;i<=n;i++)
		{
			aux2=fabs(a[i][k]);
			if(aux2>max)
			{
				imax=i;
				max=aux2;
			}
		}//end for
		////////////////////

		//2.2 rows swapping

		if(imax!=k)
		{ 
			swapRows(a,imax,k,n);
			swapRows(E,imax,k,n);
		}//end if
		/////////////////////////////
	
		//2.3 unitization

		aux2=a[k][k];
		if(fabs(aux2)<SMLL){ printf("\n\nError! --- %ld",__LINE__); exit(0); }

		for(j=1;j<=n;j++)
		{
			a[k][j]/= aux2;
			E[k][j]/= aux2;
		}//end for
		/////////////////////////

		//2.4 expunction

		for(i=1;i<=n;i++)
		{
			if(i==k) continue;

			aux2=a[i][k]/a[k][k];
			
			for(j=1;j<=n;j++)
			{
				a[i][j]-= aux2*a[k][j];
				E[i][j]-= aux2*E[k][j];
			}
		}//end for
	}//end for 
	////////////////////

	//3: settlement

	copyMatrix(a,E,n,n);
	freeMatrix(_E,E);
	///////////////////////

	return;
}

template<class TYPE>
TYPE maxError(TYPE **m1,TYPE **m2,long m,long n)
{
	TYPE err,maxErr=0;//init
	register long i,j;

	for(i=1;i<=m;i++)
	{
		for(j=1;j<=n;j++)
		{
			err=m1[i][j]-m2[i][j];
			if(fabs(err)>maxErr) maxErr=err;
		}
	}//end for
	/////////////////

	return maxErr;
}

template<class TYPE>
TYPE maxError(TYPE *v1,TYPE *v2,long n)
{
	TYPE err,maxErr=0;//init.
	register long i;

	for(i=1;i<=n;i++)
	{
		err=v1[i]-v2[i];
		if(fabs(err)>maxErr) maxErr=err;
	}//end for
 
	return maxErr;
}

template<class TYPE>
long oppSign(TYPE num1,TYPE num2)
{
	long ret=NO;

	if(num1>SMLL && num2<-SMLL) ret=YES;
	if(num1<-SMLL && num2>SMLL) ret=NO;

	return ret;
}

double fabsmin(double **a,long i,long n)
{
	register long j;
	double aux2,ret;
	//////////////////////////

	ret=fabs(a[i][1]);
	for(j=1;j<=n;j++)
	{
		aux2=fabs(a[i][j]);
		if(aux2<ret) ret=aux2;
	}//end for
	//////////////////////////

	return ret;
}

double fabsmax(double *list,long n1,long n2)
{
	register long i;
	double aux2,ret;
	
	ret=fabs(list[n1]);
	for(i=n1+1;i<=n2;i++)
	{
		aux2=fabs(list[i]);
		if(aux2>ret) ret=aux2;
	}

	return ret;
}


double fabsmax(double *list,long n)
{ return fabsmax(list,1,n); }

template<class TYPE>
void showZeroRows(TYPE **a,long m,long n,char caption[])
{
	register long i,j;
	long k=0,flag;

	for(i=1;i<=m;i++)
	{
		flag=YES;
		for(j=1;j<=n;j++)
		{
			if(fabs(a[i][j])>SMLL)
			{
				flag=NO; 
				break;
			}
		}//end for
		////////////////////////

		if(flag==YES)
		{
			printf("\n\n ZeroRow of %s: %ld",caption,i);
			k++; 
		}//end if
	}//end for

	printf("\n\n %ld zero rows found for %s",k,caption);
	////////////////////////////////////////
	
	return;
}

template<class TYPE>
void delMatrixRow(TYPE **a,long ik,long m,long n)
{
	register long i,j;

	for(i=ik;i<=m-1;i++)
		for(j=1;j<=n;j++)
			a[i][j]=a[i+1][j];

	return;
}

template<class TYPE>
void delMatrixCol(TYPE **a,long jk,long m,long n)
{
	register long i,j;

	for(i=1;i<=m;i++)
		for(j=jk;j<=n-1;j++)
			a[i][j]=a[i][j+1];

	return;
}

template<class TYPE>
void insertVectorElm(TYPE *v,TYPE vElm,long k,long n)
{
//NOTATION://///////////////////////////////////////
//	v[]		---	vector buffer
//	vElm	--- element value to be inserted
//	k		--- index where to insert
//	n		--- full dimension of v[]
////////////////////////////////////////////////////

	register long i;

	for(i=n;i>=k+1;i--)
		v[i]=v[i-1];

	v[k]=vElm;

	return;
}
	
template<class TYPE>
void delVectorElm(TYPE *v,long k,long n)
{
	register long i;
	for(i=k;i<=n-1;i++)	v[i]=v[i+1];

	return;
}

template<class TYPE>
void initMatrixRow(TYPE **a,long ik,long n,TYPE v)
{
	register long j;
	for(j=1;j<=n;j++) a[ik][j]=v;

	return;
}
				   
template<class TYPE>
void initVector(TYPE *a,long n,TYPE v)
{ initVector(a,(long)1,n,v); return; }

template<class TYPE>
void initVector(TYPE *a,long n1,long n2,TYPE v)
{
	register long i;
	for(i=n1;i<=n2;i++)	a[i]=v;
	
	return;
}

template<class TYPE>
void divMatrixRow(TYPE **a,long i,long n,TYPE div)
{
	register long j;
	for(j=1;j<=n;j++) a[i][j]/= div;

	return;
}

template<class TYPE>
void divMatrixRow(TYPE **a,TYPE *v,long i,long n,TYPE div)
{
	register long j;
	for(j=1;j<=n;j++) a[i][j]/= div;

	v[i]/= div;
	return;
}

template<class TYPE>
void divMatrix(TYPE **a,long m,long n,TYPE div)
{
	if(fabs(div)<SMLL){ printf("\nError! --- %ld",__LINE__); exit(0); }

	register long i,j;
	for(i=1;i<=m;i++)
		for(j=1;j<=n;j++)
			a[i][j]/= div;

	return;
}

template<class TYPE>
void initMatrix(TYPE **a,long m,long n,TYPE v)
{
	register long i,j;

	for(i=1;i<=m;i++)
	{
		for(j=1;j<=n;j++)
			a[i][j]=v;
	}//end for

	return;
}

template<class TYPE>
void negaMatrix(TYPE **a,long m,long n)
{
	register long i,j;

	for(i=1;i<=m;i++)
		for(j=1;j<=n;j++)
			a[i][j]*= (TYPE)-1.0;

	return;
}

template<class TYPE>
long isZeroMatrix(TYPE **a,long m,long n)
{
	register long i,j;

	for(i=1;i<=m;i++)
		for(j=1;j<=n;j++)
			if(fabs(a[i][j])>SMLL) return NO;

	return YES;
}

template<class TYPE>
long isUnitMatrix(TYPE **a,long n)
{
	register long i,j;

	for(i=1;i<=n;i++)//diagonal elements
		if(fabs((double)a[i][i]-1.0)>SMLL) return NO;			

	for(i=1;i<=n;i++)//non-diagonal elements
	{
		for(j=1;j<=i-1;j++)
			if(fabs((double)a[i][j])>SMLL) return NO;

		for(j=i+1;j<=n;j++)
			if(fabs((double)a[i][j])>SMLL) return NO;
	}//end for
	//////////////////////

	return YES;
}

template<class TYPE>
void subMatrix(TYPE **sub,TYPE **a,long *icol,long m,long n)
//sub[][]	--- m * n
//a[][]		--- m * ?
//icol[]	--- n	
{
	register long i,j,jj;

	for(i=1;i<=m;i++)
	{
		for(j=1;j<=n;j++)
		{
			jj=icol[j];
			sub[i][j]=a[i][jj];
		}
	}//end for
	////////////////
	
	return;
}

template<class TYPE>
void combinChainNodes(TYPE *chain)
{
        TYPE *cp,*cp2,*cp2_next;

        cp=chain;
        while(cp!=NULL)
        {
                cp2=cp->next;
                while(cp2!=NULL)
                {
                       	if(match(cp,cp2)==YES) 
                        {
				combinData(cp,cp2);// cp <== cp + cp2

                                cp2_next=cp2->next;
                                delChainNode(chain,cp2);

                                cp2=cp2_next;
                        }
                        else
                                cp2=cp2->next;
                }//end while
		/////////////////////////////////

                cp=cp->next;
        }//end while
	///////////////////////////////

        return;
}

template<class TYPE>
long drift(TYPE v,TYPE L,TYPE U)
{
	if(L>U && fabs(U)>SMLL)
	{
		printf("\n\nError! --- %s: %ld",__FILE__,__LINE__);
		printf("\n  L=%lf, U=%lf",L,U);

		exit(0);
	}//end if
	//////////////////////////////////

	double mid=0.5*(L+U);

	if(fabs(U)<SMLL) return LOWER;
	if(v<=mid-SMLL) return LOWER;
	
	if(v>=mid+SMLL) return UPPER;
	//////////////////////////////////

	return MIDDL;
}

template<class TYPE>
void copyMatrix(TYPE **m_destination,TYPE **m_source,long m,long n)
{
	register long i,j;

	for(i=1;i<=m;i++)
	for(j=1;j<=n;j++)
		m_destination[i][j]=m_source[i][j];

	return;
}

template<class TYPE>
void copyVectorRow(TYPE *vector,TYPE **matrix,long i,long n)
{
	register long j;
	for(j=1;j<=n;j++) vector[j]=matrix[i][j];

	return;
}

template<class TYPE>
void copyVector(TYPE *v_destination,TYPE *v_source,long n)
{ 
	copyVector(v_destination,v_source,(long)1,n,(long)1);
	return;
}

template<class TYPE>
void copyVector(TYPE *vd,TYPE *vs,long nd1,long nd2,long ns1)
{
	register long i;

	for(i=nd1;i<=nd2;i++)
		vd[i]=vs[ns1+i-nd1];

	return;
}

template<class TYPE>
void copyVector(TYPE *v_destination,TYPE *v_source,long n1,long n2)
{
	copyVector(v_destination,v_source,n1,n2,n1);
	return;
}

template<class TYPE>
void transMatrix(TYPE **matrix,long n)
{
        register long i,j;
        TYPE aux;

        for(i=1;i<=n;i++)
        for(j=i+1;j<=n;j++)
        {
                aux=matrix[i][j];
                matrix[i][j]=matrix[j][i];
                matrix[j][i]=aux;
        }//end for

        return;
}

template<class TYPE>
void delChainNode(TYPE *&chain,TYPE *node)
{
        TYPE *p;

        if(node==chain)//the head to be deleted
        {
                p=chain->next;

                free(chain);
                chain=p;
        }
        else//other node to be deleted
        {
                p=chain;
                while(p!=NULL)
                {
                        if(p->next==node)
                        {
                                p->next=node->next;

                                free(node);
                                break;
                        }//end if 

			p=p->next;
                }//end while
        }//end of if-else

        return;
}

template<class TYPE>
long chainLen(TYPE *chain)
{
        register long ret=0;
        TYPE *cp=chain;

        while(cp!=NULL)
        {
                ret++;
                cp=cp->next;
        }//end while

        return ret;
}

template<class TYPE>
TYPE fmin(TYPE **matrix,long i,long n)
{
	register long j;
	TYPE ret=matrix[i][1];//init.

	for(j=1;j<=n;j++)
	{
		if(matrix[i][j]<ret)
			ret=matrix[i][j];
	}//end for
	///////////////////////////

	return ret;
}

template<class TYPE>
TYPE fmax(TYPE *list,long n){ return fmax(list,(long)1,n); }
 
template<class TYPE>
TYPE fmax(TYPE *list,long n1,long n2){ return list[posiMax(list,n1,n2)]; }

template<class TYPE>
TYPE fmin(TYPE *list,long n)
{ return list[posiMin(list,n)]; }

template<class TYPE>
void quickSort(TYPE *vector,long n)
{ quickSort(vector,(long)1,n); return; }

template<class TYPE>
void quickSort(TYPE *vector,long i,long j)
{
        TYPE pivot;
        long ipivot,k;

        ipivot=findPivot(vector,i,j);
        if(ipivot!=NO)
        {
                pivot=vector[ipivot];
                k=partition(vector,i,j,pivot);

                quickSort(vector,i,k-1);
                quickSort(vector,k,j);
        }//end if
	/////////////////////////

        return;
}

template<class TYPE>
long findPivot(TYPE *vector,long i,long j)
{
        register long k;
        TYPE firstKey;

        firstKey=vector[i];

        for(k=i+1;k<=j;k++)
        {
                if(vector[k]>firstKey)
                        return(k);
                else if(vector[k]<firstKey)
                        return(i);
        }//end for
	//////////////////////

        return(NO);
}

template<class TYPE>
long partition(TYPE *vector,long i,long j,TYPE pivot)
{
        TYPE aux;
        do
        {
                //swapping elements
                aux=vector[i];
                vector[i]=vector[j];
                vector[j]=aux;

                while(vector[i]<pivot) i++;
                while(vector[j]>=pivot) j--;
        }while(i<=j);

        return(i);
}

long posiAbsmax2(double *vector,long n)
{ return posiAbsmax2(vector,(long)1,n); }

long posiAbsmax2(double *vector,long n1,long n2)
//positioning 2nd absmax
{
	long imax,imax1,imax2;
	double max1,max2;
	
	if(n1==n2) return n1;//only one element
	imax=posiAbsmax(vector,n1,n2);
	
	if(imax==n1) return posiAbsmax(vector,n1+1,n2);
	if(imax==n2) return posiAbsmax(vector,n1,n2-1);

	imax1=posiAbsmax(vector,n1,imax-1);
	imax2=posiAbsmax(vector,imax+1,n2);

	max1=fabs(vector[imax1]);
	max2=fabs(vector[imax2]);

	if(max1>=max2) return imax1;
	else return imax2;
}

template<class TYPE>
long posiAbsmax(TYPE *vector,long n)
{ return posiAbsmax(vector,(long)1,n); }

template<class TYPE>
long posiAbsmax(TYPE *vector,long n1,long n2)
{
	register long i,imax;
	TYPE max,aux2;

	if(n1==n2) return n1;//only one element

	imax=n1;//init.
	max=fabs(vector[n1]);

	for(i=n1+1;i<=n2;i++)
	{
		aux2=fabs(vector[i]);
		if(aux2>max){ max=aux2; imax=i; }
	}//end for
	///////////////////

	return imax;
}

template<class TYPE>
long posiMax(TYPE *vector,long n){ return posiMax(vector,(long)1,n); }

template<class TYPE>
long posiMax(TYPE *vector,long n1,long n2)
{
        register long i,imax;
        TYPE max;

        imax=n1;//init.
        max=vector[n1];

        for(i=n1+1;i<=n2;i++)
        {
                if(vector[i]>max)
                {
                        max=vector[i];
                    imax=i;
                }
        }//end for
		////////////////////////////

        return imax;
}

template<class TYPE>
long posiMin(TYPE *vector,long n)
{ return posiMin(vector,(long)1,n); }

template<class TYPE>
long posiMin(TYPE *vector,long n1,long n2)
{
        register long i,imin;
        TYPE min;

        imin=n1;//init.
        min=vector[n1];

        for(i=n1+1;i<=n2;i++)
        {
                if(vector[i]<min)
                {
                        min=vector[i];
                    imin=i;
                }
        }//end for
	//////////////////////

        return(imin);
}

template<class TYPE>
void sortVector(TYPE *list,long n)
{
        register long i,imin;
        TYPE aux;

        for(i=1;i<=n-1;i++)
        {
                imin=posiMin(list,i,n);

                if(imin!=i)//swapping
                {
                        aux=list[i];
                        list[i]=list[imin];
                        list[imin]=aux;
                }
        }//end for

        return;
}

template<class TYPE>
long rank(TYPE **matrix,long m,long n)
{
        register long i,j,k;
        double aux2,*_a,**a;
        //////////////////////////

		//1: make duplication
 
        makeMatrix(_a,a,m+1,n+1);
		copyMatrix(a,matrix,m,n);
        /////////////////////////////////////////

		//2: upper triangle 

        for(i=1;i<=m;i++)
        {
                if(fabs(a[i][i])<SMLL)
                {
                        for(k=i+1;k<=m;k++)
                                if(fabs(a[k][i])>SMLL) break;

                        if(k<=m)
                                swapRows(a,i,k,n);
                        else
                                continue;
                }//end if
                /////////////////////////

                aux2=a[i][i];
                for(j=i;j<=n;j++)
                        a[i][j]/= aux2;
                ///////////////////////

                for(k=i+1;k<=m;k++)
                        addMultiRow(a,k,i,-a[k][i],n);
        }//end  for
        //////////////////////////////////

		//3: counting the rank

        k=0;
        for(i=1;i<=min(m,n);i++)
                if(fabs(a[i][i])>SMLL) k++;
        //////////////////////////////////

        freeMatrix(_a,a);
        return k;
}

template<class TYPE>
void swapMatrixEle(TYPE **a,long i,long j,long i2,long j2)
{
	TYPE ele =a[i][j];
	a[i][j]  =a[i2][j2];
	a[i2][j2]=ele;

	return;
}

template<class TYPE>
void swapElems(TYPE *v,long i,long j)
{
        if(i==j) return;

        TYPE aux2;

        aux2=v[i];
        v[i]=v[j];
        v[j]=aux2;

        return;
}

template<class TYPE>
void swapRows(TYPE **a,long i,long k,long n)
{
        if(i==k) return;

        register long j;
        TYPE aux2;

        for(j=1;j<=n;j++)
        {
                aux2=a[i][j];
                a[i][j]=a[k][j];
                a[k][j]=aux2;
        }


        return;
}

template<class TYPE>
void addMultiRow(TYPE **a,long k,long i,TYPE multiple,long n)
//adding i_th row multipied by multiple onto k_th line
{
        register long j;

        for(j=1;j<=n;j++)
                a[k][j]+= a[i][j]*multiple;

        return;
}

template<class TYPE>
void addVector(TYPE *v,TYPE *v_add,long n)
//v[] <== v[]+v_add[], with n dimension
{
        register long i;
        for(i=1;i<=n;i++) v[i]+= v_add[i];

        return;
}
          
template<class TYPE>
void divVector(TYPE *v,long n,TYPE div)
{ divVector(v,(long)1,n,div); return; }

template<class TYPE>
void divVector(TYPE *v,long n1,long n2,TYPE div)
{
	if(fabs(div)<SMLL){ printf("\nError! --- %ld",__LINE__); exit(0); }

	register long i;
	for(i=n1;i<=n2;i++) v[i]/= div;

	return;
}

template<class TYPE>
void multiMatrix(TYPE **M3,TYPE **M1,TYPE **M2,long m,long k,long n)
//M3[][] <== M1[][] * M2[][]
{
	register long i,j,kk;
    	TYPE sum;
	TYPE *_tempMatrix,**tempMatrix;
	/////////////////////////////////////////////

    	makeMatrix(_tempMatrix,tempMatrix,m+1,n+1);

    	for(i=1;i<=m;i++)
    	{
		for(j=1;j<=n;j++)
		{
			sum=(TYPE)0.0;
			
			for(kk=1;kk<=k;kk++)
				sum+= M1[i][kk]*M2[kk][j];
			
			tempMatrix[i][j]=sum;
		}
	}//end for

	copyMatrix(M3,tempMatrix,m,n);
	//////////////////////////////////

    	freeMatrix(_tempMatrix,tempMatrix);
	return;
}

template<class TYPE>
void unitMatrix(TYPE **a,long n)
//to initialize a unit matrix
{
        register long i,j;

        for(i=1;i<=n;i++)
                for(j=1;j<=n;j++)
                {
                        if(j==i) a[i][j]=(TYPE)1;
                        else     a[i][j]=(TYPE)0;

                }

        return;
}

template<class TYPE>
void makeMatrix(TYPE* &_matrix,TYPE** &matrix,long row,long col)
{
	register long i;

	_matrix=(TYPE*)calloc(row*col,sizeof(TYPE));
	matrix=(TYPE**)calloc(row,sizeof(TYPE*));

    	if(_matrix==NULL || matrix==NULL)
	{ printf("\nError! --- %ld",__LINE__);  exit(0); }

    	for(i=0;i<row;i++)
		matrix[i]=&_matrix[col*i];
    	///////////////////////////////////

	return;
}

template<class TYPE>
void freeMatrix(TYPE* &_matrix,TYPE** &matrix)
{
	free(_matrix);
	free(matrix); 

	return;
}

template<class TYPE>
void typeVector(TYPE *V,long n,char caption[])
{
	register long i;
	double aux2;

        printf("\n\n%s:\n",caption);
        for(i=1;i<=n;i++)
	{
		aux2=(double)V[i];
                printf(FT73,aux2);
	}//end for
	///////////////////////////////

        return;
}

template<class TYPE>
void typeVector(TYPE *V,long n1,long n2,char caption[])
{
        printf("\n\n%s:\n",caption);
        for(register long i=n1;i<=n2;i++)
                printf(FT73,(double)(TYPE)V[i]);

        return;
}

template<class TYPE>
void ftypeVector(FILE *out,TYPE *V,long n,char caption[])
{
        long i;

        fprintf(out,"\n\n%s:\n",caption);
        for(i=1;i<=n;i++)
                fprintf(out,FT84,(double)(TYPE)V[i]);

        return;
}

template<class TYPE>
void ftypeVector(FILE *out,TYPE *V,long n1,long n2,char caption[])
{
        fprintf(out,"\n\n%s:\n",caption);
        for(register long i=n1;i<=n2;i++)
                fprintf(out,FT84,(double)(TYPE)V[i]);

        return;
}

template<class TYPE>
void typeMatrix(TYPE **M,long row,long col,char caption[])
{
        register long i,j;

        printf("\n\n%s:",caption);
        for(i=1;i<=row;i++)
        {
                printf("\n");
                for(j=1;j<=col;j++)
                        printf(FT73,(double)(TYPE)M[i][j]);
        }//end for

        return;
}

template<class TYPE>
void ftypeMatrix(FILE *out,TYPE **M,long row,long col,char caption[])
{
        register long i,j;

        fprintf(out,"\n\n%s:",caption);
        for(i=1;i<=row;i++)
        {
                fprintf(out,"\n");
                for(j=1;j<=col;j++)
                        fprintf(out,FT84,(double)(TYPE)M[i][j]);
        }//end for

        return;
}

void factorTable(double **a,long n)
{ 
	register long i,j,k;

	for(k=1;k<=n-1;k++)
    for(j=k+1;j<=n;j++)
    { 
		if(fabs(a[k][k])<SMLL){ printf("\nError! --- %s: %ld",__FILE__,__LINE__); exit(0); }

		a[k][j]/= a[k][k];
		for(i=k+1;i<=n;i++)
			a[i][j]-= a[k][j]*a[i][k];
    }//end for
	/////////////////////////
	
	return;
}

void solveFactTab(double **factTab,double *b,long n)
{

        register long i,j,k;
        double sum,*_tab_sv,**tab_sv;
	///////////////////////////////////////

	//1: saving factTab into tab_sv[][]

	makeMatrix(_tab_sv,tab_sv,n+1,n+1);
	copyMatrix(tab_sv,factTab,n,n);
	/////////////////////////////////////

	//2: partial expunction

        for(k=1;k<=n-1;k++)
        {
                b[k]/= factTab[k][k];
                for(i=k+1;i<=n;i++)
                        b[i]-= factTab[i][k]*b[k];
        }//end for
	//////////////////////////////////

	//3: back substitution

        b[n]/= factTab[n][n];
        for(i=n-1;i>=1;i--)
        {
                sum=0.0;
                for(j=n;j>=i+1;j--)
                        sum+= factTab[i][j]*b[j];

                b[i]-= sum;
        }//end for
	/////////////////////////////////

	//4: restoring factTab from tab_sv[][]

	copyMatrix(factTab,tab_sv,n,n);
	freeMatrix(_tab_sv,tab_sv);
	/////////////////////////////////////

        return;
}

void gaussPP(double *x,double **a,double *b,long n)
{
//NOTATION:////////////////////////////////////////
//x[]	--- solution of Ax=b[], returned
//a[][]	--- coeff.matrix, changed
//b[]	--- right vector, not changed
//////////////////////////////////////////////////
        
	register long i,j,k;
		long imax;
        double *bsv,sum,max,aux2;

		bsv=(double *)calloc(n+1,sizeof(double));
		if(bsv==NULL){ printf("\n\nError! --- %ld",__LINE__); exit(0); }
		////////////////////////////////////////////////

		//1: saving b[] into bsv[]
		copyVector(bsv,b,n);

		//2: expunction on a[][] and b[]

		for(k=1;k<=n-1;k++)
		{
			//2.1 Pivot Selection

			imax=k;//init.
			max=fabs(a[k][k]);

			for(i=k+1;i<=n;i++)
			{
				aux2=fabs(a[i][k]);
				if(aux2>max)
				{
					imax=i;
					max=aux2;
				}
			}//end for

			if(max<SMLL){ printf("\n\nError! --- %s: %ld",__LINE__,__LINE__); exit(0); }
			/////////////////////////////

			//2.2 Rows Swap

			if(imax!=k)
			{
				swapRows(a,imax,k,n);
				swapElems(b,imax,k);
			}//end if
			////////////////////////////

			//2.3 Genuine Expunction

			for(i=k+1;i<=n;i++)
			{
				aux2=a[i][k]/a[k][k];
				
				for(j=k+1;j<=n;j++)
					a[i][j]-= aux2*a[k][j];

				b[i]-= aux2*b[k];
			}//end for
		}//end for
		//////////////////////////////////

		//3: back substitution

        b[n]/= a[n][n];
        for(i=n-1;i>=1;i--)//No Change!
        {
                sum=0.0;
                for(j=i+1;j<=n;j++)
                        sum+= a[i][j]*b[j];

                b[i]=(b[i]-sum)/a[i][i];
        }//end for
		/////////////////////////////

		//4: settlement

		copyVector(x,b,n);//x[] obtained
		copyVector(b,bsv,n);//b[] restored
		///////////////////////////////////
		
		free(bsv);		
        return;
}

template<class TYPE>
void freeChain(TYPE *&chain)
{
        TYPE *next;
        while(chain!=NULL)
        {
                next=chain->next;

                free(chain);
                chain=next;
        }//end while

        return;
}

void drawline(long n)
{
	drawline((long)1,n);
	return;
}
 
void drawline(long x1,long x2)
{
        register long i;

        printf("\n");

        for(i=1;i<=x1-1;i++)
                printf(" ");

        for(i=x1;i<=x2;i++)
                printf("-");

        return;
}

void fdrawline(FILE *out,long n)
{
	fdrawline(out,(long)1,n);
	return;
}

void fdrawline(FILE *out,long x1,long x2)
{
        register long i;

        fprintf(out,"\n");

        for(i=1;i<=x1-1;i++)
                fprintf(out," ");

        for(i=x1;i<=x2;i++)
                fprintf(out,"-");

        return;
}

void complexMult(double re1,double im1,double re2,double im2,double &re,double &im)
{
        double _re,_im;

        _re=re1*re2 - im1*im2;
        _im=re1*im2 + re2*im1;

        re=_re; im=_im;

        return;
}
      
void complexDivi(double re1,double im1,double re2,double im2,double &re,double &im)
{
        double aux2=re2*re2 + im2*im2;
        double _re,_im;

        _re=(re1*re2 + im1*im2)/aux2;
        _im=(re2*im1 - re1*im2)/aux2;

        re=_re; im=_im;

        return;
}
             
template<class TYPE>
long sign(TYPE number)
{
	long ret=0;

	if(number>SMLL) ret=POSI;
	if(number<-SMLL) ret=NEGA;

	return ret;
}

void upperCase(char *string)
{
	register long i,n;
	char ch;
	
	n=strlen(string);
	for(i=0;i<n;i++)
	{
		ch=string[i];
		if(ch>='a' && ch<='z')
		{
			ch+= 'A'-'a';
			string[i]=ch;
		}
	}//end for
	
	return;
}

void strcat2(char *strDestination,char *strSource,long size)
{
	register long i;
	long n1,n2;

	n1=strlen(strDestination);
	n2=strlen(strSource);
	
	if(n1+n2<size) size=n1+n2;//ultimate size
	
	for(i=n1;i<=size-1;i++)
		strDestination[i]=strSource[i-n1];

	stringEnd(strDestination,size);
	return;
}

void stringEnd(char *string,char ch)
{
	register long i;
	long n=strlen(string);

	for(i=0;i<n;i++)
	{
		if(string[i]==ch)
		{
			string[i]='\0';
			break;
		}
	}//end for

	return;
}

void stringEnd(char *string,long n)
{
	register long i;
	long n1=strlen(string);
	
	for(i=n1;i<=n-1;i++) string[i]=' ';
	string[n]='\0';
	
	return;
}

long isAlphaNum(char ch)
{
	//1: alpha_num characters

	if(ch>='0' && ch<='9') return YES;
	if(ch>='A' && ch<='Z') return YES;
	if(ch>='a' && ch<='z') return YES;
	//////////////////////////////////////

	//2: expanded characters

	if(ch=='_') return YES;
	if(ch==' ') return YES;
	if(ch=='#') return YES;
	/////////////////////////////

	return NO;
}

void initString(char *string,long n)
{
	register long i;

	for(i=0;i<=n-1;i++)
		string[i]='\0';

	return;
}	
	
template<class TYPE>
void turnOver(TYPE *&chain)
//turning over a chain
{
	TYPE *oldp=NULL,*p;

	while(chain!=NULL)
	{
		p=chain->next;//reserved
		chain->next=oldp;

		oldp=chain;
		chain=p;
	}//end while
	/////////////////////

	chain=oldp;

	return;
}

long lookup(char *string,char ch,long n)
{
	register long i;
	long ret=NO;//init.

	for(i=0;i<n;i++)//No Change!
	{
		if(string[i]==ch)
		{
			ret=i;
			break;
		}
	}//end for
	///////////////////////

	return ret;
}

long lookup(long *list,long id,long n)
{ 
	register long i;
	long ret=NO;//init.

	for(i=1;i<=n;i++)
	{
		if(abs(list[i])==abs(id))
		{
			ret=i;
			break;
		}
	}//end for
	//////////////////////

	return ret;
}

long lookup2(long *list,long id,long n)
{
	register long i;
	long ret=NO;//init.

	for(i=1;i<=n;i++)
	{
		if(list[i]==id)
		{
			ret=i;
			break;
		}
	}//end for
	/////////////////////

	return ret;
}

template<class TYPE>
void swap(TYPE &v1,TYPE &v2)
{ TYPE v=v1; v1=v2; v2=v; return; }

void wpause()
{	
	char ch;	

	printf("\nPress ENTER to continue...");
	
	fflush(stdin); 
	scanf("%c",&ch);

	return;
}

long scale(double number)
{
	register long i;
	long ret;
	
	double aux2;
	/////////////////////////////

	aux2=fabs(number);
	for(i=-10;i<=10;i++)//10E-10 --> 10E10
	{
		if(pow(10.0,i)>number)
		{
			ret=i-1;//the exponential
			break;
		}
	}//end for
	/////////////////////////

	return ret;
}

//end of file
