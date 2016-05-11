//common.cpp by Wang GT. All rights reserved.

#include"common.h"
/////////////////////////////////////////////////

void replace_subString(char *string,char *substr,char *substr2)
{
	char *cp=NULL;//init.
	long k,n;

	long n2=strlen(substr2);
	long t =strlen(substr2)-strlen(substr);
	/////////////////////////////////////////////

	while(1)
	{
		cp=strstr(string,substr);
		if(cp==NULL) break;

		n=strlen(string);
		k=n-strlen(cp);

		transMove(string,t,k,n);
		copyVector(string,substr2,k,(long)0,n2-1);
	}//end while
	/////////////////////////////////////////////

	return;
}

long round(double v)
{
	long ret=sign(v)*(long)(fabs(v)+0.5);
	return ret;
}

long isLarge(double v)
{
	if(fabs(v)>0.8*LARG) return YES;
	return NO;
}

long oddeven(long num)
{
	if(num%2!=0) return ODD;
	return EVEN;
}

long sdfun(char *hrmin,long sd_num)
{
	char linebuf[LINELN];

	long hr =0;//init.
	long min=0;
	//////////////////////////////////////////////

	//1: getting hr and min

	strcpy(linebuf,hrmin);
	
	token=strtok(linebuf,seps_time);
	if(token!=NULL) hr=atol(token);

	if(token!=NULL) token=strtok(NULL,seps_time);
	if(token!=NULL) min=atol(token);
	//////////////////////////////////////////////

	//2: sd <== hr + min

	long min_sd=1440/sd_num;
	min+= 60*hr;

	long ret=min/min_sd+1;//No Change!
	return ret;
}

void sdfun(char *hrmin,long sd,long sd_num)
{
	long min_sd=1440/sd_num;
	long min=(sd-1)*min_sd;//No Change!

	long hr =min/60;
	min=min%60;

	sprintf(hrmin,"%2ld:%02ld",hr,min);
	return;
}

void stringSeg(char *seg,char *string,long k)
{
	//1: init. linebuf[] and strtok()

	register long i;
	char linebuf[LINELN];

	strcpy(linebuf,string);
	token=strtok(linebuf,seps);
	///////////////////////////////////////////

	//2: getting k-th seg. in string

	for(i=2;i<=k;i++)
	{
		if(token==NULL) break;
		token=strtok(NULL,seps);
	}//end for

	initString(seg);
	if(token!=NULL) strcpy(seg,token);
	///////////////////////////////////////////

	return;
}

long fileExist(char *filename)
{
	long ret=NO;//init.

	FILE *fp=fopen(filename,"r");
	if(fp!=NULL){ fclose(fp); ret=YES; }
	
	return ret;
}

void fileName(char *filename,char *pathfile)
{
	initString(filename);//init.

	char *cp=strrchr(pathfile,'\\');
	if(cp!=NULL) strcpy(filename,cp+1);
	
	stringEnd(filename,'.');
	return;
}

long chars_inString(char ch,char *string)
{
	long n=strlen(string);
	long ret=0;//init.

	register long i;
	for(i=0;i<n;i++) if(string[i]==ch) ret++;

	return ret;
}

struct pointstr crossPoint(struct plotstr plot1,struct plotstr plot2)
{
	double x,dx,xmin,xmax,dy0,dy,y1,y2;
	double aux2;

	struct pointstr ret;
	////////////////////////////////////////////////////

	//1: init. locals

	ret.x =0.0;//init.
	ret.y =0.0;

	long n1=plot1.pntNum;
	long n2=plot2.pntNum;

	xmin=max(plot1.point[1].x,plot2.point[1].x);
	xmax=min(plot1.point[n1].x,plot2.point[n2].x);
	////////////////////////////////////////////////////

	//2: cal. dx --- the step

	aux2=max(n1,n2)*DENS_COEFF;
	if(aux2<SMLL){ printf("\nError! --- %ld",__LINE__); exit(0); }

	dx=(xmax-xmin)/aux2;
	if(fabs(dx)<SMLL)
	{
		printf("\n\nError! --- %ld",__LINE__); 
		printf("\n  dx=%lf",dx);

		wpause(); exit(0); 
	}//end if
	////////////////////////////////////////////////////

	//3: finding cross-points

	dy0=0.0;//init.
	for(x=xmin;x<=xmax;x+= dx)		
	{
		y1=linearin(plot1.point,n1,x);
		y2=linearin(plot2.point,n2,x);

		dy=y1-y2;//No Change!
		if(fabs(dy)<SMLL || dy*dy0<-SMLL)
		{
			ret.x=x;
			ret.y=y1;

			break;
		}//end if

		dy0=dy;//saved
	}//end for
	////////////////////////////////////////////////////

	return ret;
}

void getparm(char *value,char *prmfile,char *parm)
{
	register long i;
	char linebuf[LINELN];

	initString(value);//init.
	/////////////////////////////////////////////

	//1: getting linebuf[]

	long ret=fline_read(prmfile,linebuf,parm);
	if(ret==NO) return;

	trimString(linebuf);
	/////////////////////////////////////////////

	//2: positioning cp by seps_val[]

	long n=strlen(seps_val);//No Change!
	char *cp=NULL;//init.

	for(i=0;i<n;i++)
	{
		cp=strchr(linebuf,seps_val[i]);
		if(cp!=NULL) break;
	}//end for	

	if(cp==NULL) return;
	/////////////////////////////////////////////

	//4: getting value[]

	strcpy(value,cp+1);
	trimString(value);
	/////////////////////////////////////////////

	return;
}

double getparm(char *prmfile,char *parm)
{
	char linebuf[LINELN];
	getparm(linebuf,prmfile,parm);

	double ret=atof(linebuf);
	return ret;
}

void smooth_corr(double *v,double corr,double coeff,long k,long n)
{
	register long i,inc;
	double aux2;
	///////////////////////////////

	//1: preparation

	if(fabs(corr)<SMLL) return;

	if(k<=n) inc=1;
	if(k>n) inc=-1;

	aux2=corr*coeff;
	i=k;//No Change!
	///////////////////////////////

	//2: smooth correction

	while(intrvl(i,k,n)==YES)
	{
		v[i]+= aux2;
		aux2*= coeff;

		k+= inc;
	}//end while
	////////////////////////////////

	return;
}

template<class TYPE>
void transMatrix(TYPE **a,long n)
{
        register long i,j;

        for(i=1;i<=n;i++)
			for(j=i+1;j<=n;j++) swapMatrixElms(a,i,j,j,i);

        return;
}

template<class TYPE>
void transMove(TYPE **a,long it,long jt,long m1,long n1,long m2,long n2)
{
	register long i;
	long i1,inc;
	////////////////////////////////////////////////

	//1: memory allocation for row buf.

	TYPE *v=(TYPE*)calloc(max(n2,n2+jt)+1,sizeof(double));
	if(v==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }

	if(it>0){ i1=max(m1,m2); inc=-1; }//desc. order
	if(it<=0){ i1=min(m1,m2); inc=1; }//asc. order
	////////////////////////////////////////////////

	//2: transMove() by rows

	i=i1;//init.
	while(intrvl(i,m1,m2)==YES)
	{
		copyVectorRow(v,a,i,n1,n2);
		transMove(v,jt,n1,n2);

		copyMatrixRow(a,v,i+it,n1+jt,n2+jt);
		i+= inc;
	}//end while
	////////////////////////////////////////////////

	free(v);
	return;
}

template<class TYPE>
void transMove(TYPE *v,long t,long n)
{
	transMove(v,t,(long)1,n);
	return;
}

template<class TYPE>
void transMove(TYPE *v,long t,long n1,long n2)
{
	register long i;
	long i1,inc;
	////////////////////////////////////////////////

	//1: local parm. checking

	if(t>0){ i1=max(n1,n2); inc=-1; }//desc. order
	if(t<0){ i1=min(n1,n2); inc= 1; }//asc. order

	if(t==0) return;//no moving
	////////////////////////////////////////////////

	//2: trans. moving
	
	i=i1;//init.
	while(intrvl(i,n1,n2)==YES)
	{
		v[i+t]=v[i];
		i+= inc;
	}//end while
	////////////////////////////////////////////////

	return;
}

long date_year(char *date,char *format)
{
//Notes: by Wang GT
// date   --- date string (formatted)
// format --- format of date string, e.g:
//            1> yyyy/mm/dd
//            2> mm/dd/yyyy 
/////////////////////////////////////////////////////

	char datebuf[100];//No Change!
	long ret=NO;//init.

	strcpy(datebuf,date);
	///////////////////////////////////////////////

	//1: year --- yyyy/mm/dd

	if(!strcmp(format,"yyyy/mm/dd"))
	{
		token=strtok(datebuf,seps_date);//year
		if(token==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }

		ret=atol(token);
	}//end if
	///////////////////////////////////////////////

	//2: year --- mm/dd/yyyy
	
	if(!strcmp(format,"mm/dd/yyyy"))
	{
		token=strtok(datebuf,seps_date);//month
		if(token==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }

		token=strtok(NULL,seps_date);//day
		if(token==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }

		token=strtok(NULL,seps_date);//year
		if(token==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }

		ret=atol(token);
	}//end if
	///////////////////////////////////////////

	return ret;
}

long date_month(char *date,char *format)
{
	char datebuf[100];//No Change!
	long ret=NO;//init.

	strcpy(datebuf,date);
	///////////////////////////////////////////

	//1: month --- yyyy/mm/dd 

	if(!strcmp(format,"yyyy/mm/dd"))
	{
		token=strtok(datebuf,seps_date);//year
		if(token==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }

		token=strtok(NULL,seps_date);//month
		if(token==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }
	
		ret=atol(token);
	}//end if
	//////////////////////////////////////////
	
	//2: month --- mm/dd/yyyy
	
	if(!strcmp(format,"mm/dd/yyyy"))
	{
		token=strtok(datebuf,seps_date);//month
		if(token==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }	
	
		ret=atol(token);
	}//end if
	//////////////////////////////////////////

	return ret;
}

long date_day(char *date,char *format)
{
	char datebuf[100];//No Change!
	strcpy(datebuf,date);

	long ret=NO;//init.
	//////////////////////////////////////////////

	//1: day --- yyyy/mm/dd

	if(!strcmp(format,"yyyy/mm/dd"))
	{ 
		token=strtok(datebuf,seps_date);//year
		if(token==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }

		token=strtok(NULL,seps_date);//month
		if(token==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }
	
		token=strtok(NULL,seps_date);//day
		if(token==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }
	
		ret=atol(token);
	}//end if
	////////////////////////////////////////////

	//2: day --- mm/dd/yyyy

	if(!strcmp(format,"mm/dd/yyyy"))
	{
		token=strtok(datebuf,seps_date);//month
		if(token==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }

		token=strtok(NULL,seps_date);//day
		if(token==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }
       
		ret=atol(token);
	}//end if
	//////////////////////////////////////////// 
	
	return ret;
}

long yrdaysFun(long year)
{
	long ret=365;//init.
	if(isLeapYear(year)==YES) ret++;

	return ret;
}

long yrdaysFun(char *date,char *format)
{
	long year=date_year(date,format);
	long ret=yrdaysFun(year);

	return ret;
}

long yeardiff_day(long year,long year0)
{
	if(year<year0){ printf("\nError! --- %ld",__LINE__); exit(0); }

	register long i;
	long ret=0;//init.

	for(i=year0;i<=year-1;i++)
	{
		ret+= 365;
		if(isLeapYear(i)==YES) ret++;
	}//end for
	////////////////////////////////////////////

	return ret;
}

long datediff_day(char *date,char *date0,char *format)
{
	//1: local definitions

	long year0,days0,year,days;
	long ret=NO;//init.
	////////////////////////////////////////////

	//2: year and days

	year0=date_year(date0,format);
	days0=yrdaysFun(date0,format);
	
	year=date_year(date,format);
	days=yrdaysFun(date,format);
	////////////////////////////////////////////

	//3: cal. diff. days

	if(year>=year0) ret=yeardiff_day(year,year0)+days-days0;
	else
	{
		ret=yeardiff_day(year0,year)+days0-days;
		ret*= -1;
	}//end else
	////////////////////////////////////////////

	return ret;
}

void expSmoothCF_a(double *a,double *s1,double *s2,long n)
{
	register long i;
	for(i=1;i<=n;i++) a[i]=2.0*s1[i]-s2[i];//(2-48)

	return;
}

void expSmoothCF_b(double *b,double *s1,double *s2,double wt,long n)
{
	register long i;
	for(i=1;i<=n;i++) b[i]=(s1[i]-s2[i])*wt/(1.0-wt);//(2-49)

	return;
}

void expSmooth_forecast(double *F,double *x,double smooth_coeff,long forNum,long n)
{
//Notes: by Wang GT ///////////////////////////////
// F[]     --- forecast solution, n+forNum size
// x[]     --- sampled data, n size
// smooth_coeff --- exp.smooth coeff: 0-->1
// forNum  --- forecast number
// n       --- size of sampled x[]: 1,2,...n
///////////////////////////////////////////////////

	register long i;
	double *a,*b,*s1,*s2;
	////////////////////////////////////////////

	//1: memory allocation

	a =(double *)calloc(n+1,sizeof(double));
	b =(double *)calloc(n+1,sizeof(double));
	s1=(double *)calloc(n+1,sizeof(double));
	s2=(double *)calloc(n+1,sizeof(double));

	if(s2==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }
	////////////////////////////////////////////

	//2: exp.smooth

	expSmooth(s1,x,smooth_coeff,n);
	expSmooth(s2,s1,smooth_coeff,n);
	////////////////////////////////////////////

	//3: curve-fitting coeff

	expSmoothCF_a(a,s1,s2,n);
	expSmoothCF_b(b,s1,s2,smooth_coeff,n);
	////////////////////////////////////////////

	//4: forecast vector

	copyVector(F,x,n);//init.
	for(i=1;i<=n;i++)
		F[i+forNum]=a[i]+b[i]*forNum;//(2-47)
	////////////////////////////////////////////

	free(a);
	free(b);
	free(s1);
	free(s2);
	////////////////////////////////////////////

	return;
}

void expSmooth(double *s,double *x,double wt,long n)
{
//Notes: by Wang GT ////////////////////////////
// s[] --- exp.smooth solution, n size
// x[] --- sampled data, n size
// wt  --- weight of NNTerm
// n   --- size of x,s[]: 1,2,...n
///////////////////////////////////////////////////

	register long i;
	
	s[1]=x[1];//init.
	for(i=2;i<=n;i++)//No Change!
		s[i]=wt*x[i]+(1.0-wt)*s[i-1];//(2-42)
	////////////////////////////////////////////
	
	return;
}

void getPathinfo(char *pathinfo,char *path)
{
	//1: full info of the path

	struct stat info;
	long ret;
	
	ret=stat(path,&info);
	if(ret!=0){ printf("\n\nError! --- %ld",__LINE__); exit(0); }
	//////////////////////////////////////////////
	
	//2: reducing info.

	strcpy(pathinfo,ctime(&info.st_ctime));
	token=strtok(pathinfo," ");
	
	token=strtok(NULL," ");//No Change!
	token=strtok(NULL," ");
	token=strtok(NULL," ");

	strcpy(pathinfo,token);
	packString(pathinfo);
	//////////////////////////////////////////////

	return;
}

void getCrrpath(char *crrpath)
{
	getcwd(crrpath,256);
	return;
}

void setupFile(char *inifile)
{
	//1: current path
	
	getCrrpath(crrpath);
	sprintf(inifile,"%s\\setup.ini",crrpath);

	if(fileExist(inifile)==YES) return;
	////////////////////////////////////////////

	//2: parent path

	parentPath(path,crrpath);
	sprintf(inifile,"%s\\setup.ini",path);

	if(fileExist(inifile)==YES) return;
	///////////////////////////////////////////

	//3: error report

	printf("\n\nError! --- %ld",__LINE__);
	printf("\n  Cannot find the file of setup.ini");

	wpause();
	exit(0);
	//////////////////////////////////////////

	return;
}

void parentPath(char *parent,char *path)
{
	char *sp=strrchr(path,'\\');
	long n=strlen(path)-strlen(sp);

	strcpy(parent,path);//init.
	stringEnd(parent,n);
	//////////////////////////////////////////
	
	return;
}

void getHomepath(char *homepath,char *inifile)
{
	if(fileExist(inifile)!=YES)
	{
		printf("\n\nError! --- %ld",__LINE__);
		printf("\n  The ini. file NOT exist: %s",inifile);

		exit(0);
	}//end if

	getparm(homepath,inifile,"homepath");
	return;
}

char ANcode(unsigned long x,long nden)
{
	long k,i,r,mod;
	char code;
	///////////////////////////////////
	
	//1: cal. remainder --- r

	nden=abs(nden)%10;
	mod =26+10*nden;
	r   =x%mod;
	///////////////////////////////////

	//2: getting the code

	if(r<26) code='A'+(int)r;
	else
	{
		for(i=1;i<=nden;i++)
		{
			k=26+i*10;//No Change!
			if(r<k){ code='0'+r-(k-10); break; }
		}
	}//end else
	////////////////////////////////
	
	return code;
}

void ASCII_string(char *ascii,char *string)
{
	register long i;
	long n;

	char ascbuf[10];
	//////////////////////////////////////

	initString(ascii,0);
	n=strlen(string);
	
	for(i=0;i<n;i++)
	{
		sprintf(ascbuf,"%ld",string[i]);
		strcat(ascii,ascbuf);
	}//end for
	//////////////////////////////////////

	return;
}

long fline_read(char *datfile,char *linebuf,char *leadingchar)
{
	FILE *fp=fopen(datfile,"r");
	if(fp==NULL){ printf("\nCannot open %s",datfile); exit(0); }

	long n=strlen(leadingchar);
	long ret=NO;//init.

	while(!feof(fp))
	{
		if(fgets(linebuf,LINELN,fp)==NULL) break;
		
		trimString(linebuf);
		if(!strncmp(linebuf,leadingchar,n)){ ret=YES; break; }
	}//end while

	fclose(fp);//datfile closed
	////////////////////////////////////////

	if(ret==NO) initString(linebuf);
	return ret;
}

void read_flineData(char *datfile,struct flinestr *&flineData)
{
	char linebuf[LINELN];
	struct flinestr *flp;
	/////////////////////////////////////////

	//1: opening file

	FILE *fp=fopen(datfile,"r");
	if(fp==NULL){ printf("\n\nCannot open %s",datfile); exit(0); }
	////////////////////////////////////////////////
	
	//2: creating flineData

	flineData=NULL;//init.
	while(!feof(fp))
	{
		if(fgets(linebuf,LINELN,fp)==NULL) break;

		flp=(struct flinestr *)malloc(sizeof(struct flinestr));
		if(flp==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }

		strcpy(flp->line,linebuf);
		flp->next=flineData;

		flineData=flp;
	}//end while
	//////////////////////////////////////

	//3: closing file

	fclose(fp);
	turnOver(flineData);
	////////////////////////////////////

	return;
}

template<class TYPE>
TYPE* findData(char *id,TYPE* dataChain)
{
        TYPE *dp=dataChain;
        while(dp!=NULL)
        {
                if(!strcmp(id,dp->id)) break;
                dp=dp->next;
        }//end while
        //////////////////////////////////////

        return dp;
}

template<class TYPE>
TYPE* findData(long ibs,long zbs,TYPE *dataChain)
{
        long i,j;

        ibs=abs(ibs);
        zbs=abs(zbs);
        //////////////////////////////////////

        TYPE *dp=dataChain;
        while(dp!=NULL)
        {
                i=abs(dp->i);
                j=abs(dp->j);

                if(ibs==i && zbs==j) break;
                if(ibs==j && zbs==i) break;

                if(zbs==0 && ibs==i) break;
                if(zbs==0 && ibs==j) break;

                dp=dp->next;
        }//end while
        //////////////////////////////////////

        return dp;
}

template<class TYPE>
TYPE* findData(long ibs,TYPE* dataChain)
{
	TYPE *dp=dataChain;
	ibs=abs(ibs);

	while(dp!=NULL)
	{
		if(abs(dp->i)==ibs) break;
		dp=dp->next;
	}//end while
	//////////////////////////////////////

	return dp;
}

struct flinestr *find_flineData(char *leadingchar,struct flinestr *flineData)
{
	register long i;

	long flineNum=chainLen(flineData);//init.
	long n,n2;

	char linebuf[LINELN];
	struct flinestr *flp;
	///////////////////////////////////////////////

	flp=flineData;
	for(i=1;i<=flineNum;i++,flp=flp->next)
	{
		strcpy(linebuf,flp->line);
		stringEnd(linebuf,'\n');

		n=strlen(linebuf);
		if(n==0) continue;//No Change!
		////////////////////////////////////////
		
		n2=strlen(leadingchar);
		if(n2<n) n=n2;

		if(!strncmp(linebuf,leadingchar,n)) break;
	}//end for
	///////////////////////////////////////////////

	return flp;
}

long fline_insert(char *datfile,char *leadingchar,char *newline)
{
	long ret=fline_insert(datfile,leadingchar,(long)0,newline);
	return ret;
}

long fline_insert(char *datfile,char *leadingchar,long k,char *newline)
{
	struct flinestr	*flineData;
	struct flinestr *flp;
	struct flinestr *flp2;
	////////////////////////////////////////////////////

	//1: flineData and flp

	read_flineData(datfile,flineData);
	long n=strlen(newline)+2;//for "\n\0"

	flp=find_flineData(leadingchar,flineData);
	if(flp==NULL)
	{
		flp=(struct flinestr *)malloc(sizeof(struct flinestr));
		if(flp==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }

		if(k!=0) strcpy(flp->line,leadingchar);
		if(k==0) strcpy(flp->line,newline);

		strcat(flp->line,"\n");
		
		flp->next=flineData;
		flineData=flp;
	}//end if
	////////////////////////////////////////////////////

	//2: inserting another fline when k!=0

	if(k!=0)
	{
		flp=nextNode(flp,k-1);//No Change!
		if(flp==NULL) flp=tailNode(flineData);

		flp2=(struct flinestr *)malloc(sizeof(struct flinestr));
		if(flp2==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }

		strcpy(flp2->line,newline);
		flp2->next=flp->next;

		flp->next=flp2;
	}//end if
	////////////////////////////////////////////////////

	//3: fline_write and free

	fline_write(datfile,flineData);
	freeChain(flineData);
	////////////////////////////////////////////////////

	return YES;
}

long fline_write(char *datfile,struct flinestr *flineData)
{
	FILE *fp=fopen(datfile,"w");
	if(fp==NULL) return NO;

	struct flinestr *flp=flineData;
	while(flp!=NULL)
	{
		fprintf(fp,"%s",flp->line);
		flp=flp->next;
	}//end while
	//////////////////////////////////////////
	
	fclose(fp);//datfile closed
	return YES;
}

long fline_update(char *datfile,char *leadingchar,char *newline)
{
	long ret=fline_update(datfile,leadingchar,(long)0,newline);
	return ret;
}

long fline_update(char *datfile,char *leadingchar,long k,char *newline)
{
	struct flinestr *flineData;
	struct flinestr *flp;
	///////////////////////////////////////////////////

	//1: getting flineData and flp

	read_flineData(datfile,flineData);
	flp=find_flineData(leadingchar,flineData);
	
	if(flp!=NULL) flp=nextNode(flp,k);//No Change!
	if(flp==NULL)
	{
		printf("\n\nError! --- %ld",__LINE__);
		printf("\n  Cannot found fline: %s, k=%ld",leadingchar,k);
		printf("\n  datfile=%s",datfile);

		return NO;
	}//end if
	///////////////////////////////////////////////////
	
	//2: updating flp->line

	strcpy(flp->line,newline);
	strcat(flp->line,"\n");

	//3: fline_write/free()

	fline_write(datfile,flineData);
	freeChain(flineData);
	///////////////////////////////////////////////////

	return YES;
}

void deNull(char *string)
{
        if(!strcmp(string,"NULL")) strcpy(string,"¡¡");
        if(!strcmp(string,"Null")) strcpy(string,"¡¡");
        if(!strcmp(string,"null")) strcpy(string,"¡¡");

	return;
}

long yes_no(char *string)
{
	long ret=NO;//init.

	if(!strcmp(string,"YES")) ret=YES;
	if(!strcmp(string,"Yes")) ret=YES;
	if(!strcmp(string,"yes")) ret=YES;

	return ret;
}

template<class TYPE>
long intrvl(TYPE v,TYPE a,TYPE b)
{
	if(a>b) swap(a,b);//==> a<=b
	if(v>=a && v<=b) return YES;//No Change!
	
	return NO;
}

template<class TYPE>
long bitfun(TYPE v,long bit)
{
//Notes: by Wang GT
//bit: 0, 1, 2, ... (from right to left)
//v  : variable
////////////////////////////////////////////////

	//1: locals and bit verification

	long ret,maxbit;
	TYPE expB2;

	maxbit=8*sizeof(TYPE)-1;//No Change!
	if(bit>maxbit)
	{
		printf("\n\nError! --- %ld",__LINE__);
		printf("\n  Invalid bit=%ld, maxbit=%ld",bit,maxbit);

		exit(0);
	}//end if
	////////////////////////////////////////////

	//2: cal. value of the bit

	expB2=(TYPE)pow(2.0,bit);
	ret=v&expB2;

	if(ret!=0) ret=1;//0 or 1
	////////////////////////////////////////////

	return ret;
}

template<class TYPE>
long posRecord(TYPE *tb,long ibs,long zbs,long lv_tb)
{
        register long i;
        long ibs2,zbs2,flg,ret;
        ////////////////////////////////////

        ret=NO;//init.
        flg=NO;

        for(i=0;i<lv_tb;i++)
        {
                ibs2 = (tb+i)->ibs;
                zbs2 = (tb+i)->zbs;

                if(ibs2==ibs && zbs2==zbs) flg=YES;
                if(ibs2==zbs && zbs2==ibs) flg=YES;
			
                if(flg==YES)
                {
                        ret=i;
                        break;
                }//end if
        }//end for
        ////////////////////////////////////

        return ret;
}

template<class TYPE>
long posRecord(TYPE *tb,long ibs,long lv_tb)
{
        register long i;
        long ret=NO;//init.
        ////////////////////////////////////

        for(i=0;i<lv_tb;i++)
        {
                if((tb+i)->ibs==ibs)
                {
                        ret=i;
                        break;
                }
        }//end for
        ////////////////////////////////////

        return ret;
}

template<class TYPE>
long posRecord(TYPE *tb,char *id,long lv_tb)
{
        register long i;
        long ret=NO;//init.
        ////////////////////////////////////

        for(i=0;i<lv_tb;i++)
        {
                if(!strcmp((tb+i)->id,id))
                {
                        ret=i;
                        break;
                }
        }//end for
        ////////////////////////////////////

        return ret;
}

double cutPoint(double a,double b,double r)
{
	double ret=a*(1.0-r)+r*b;
	return ret;
}

long isPrimeNum(long num)
{
	//1: init. locals

	register long i;
	long ret=YES;//init.

	if(num<=1) return NO;
	////////////////////////////////////

	//2: judging of num

	for(i=2;i<=sqrt(num);i++)
	{
		if(num%i==0)//No Change!
		{
			ret=NO;
			break;
		}
	}//end for
	/////////////////////////////////

	return ret;
}

long isLeapYear(long year)
{
	long ret=NO;//init.

	if(year%100!=0 && year%4==0)   ret=YES;
	if(year%100==0 && year%400==0) ret=YES;
	
	return ret;
}

COMPLEX operator+ (COMPLEX x,COMPLEX y)
{
	COMPLEX ret;

	ret.re = x.re + y.re;
	ret.im = x.im + y.im;

	return ret;
}

COMPLEX operator- (COMPLEX x,COMPLEX y)
{
	COMPLEX ret;

	ret.re = x.re - y.re;
	ret.im = x.im - y.im;

	return ret;
}

COMPLEX operator* (COMPLEX x,COMPLEX y)
{
	COMPLEX ret;

	ret.re = x.re*y.re - x.im*y.im;
	ret.im = x.re*y.im + x.im*y.re;

	return ret;
}

COMPLEX operator/ (COMPLEX x,COMPLEX y)
{
	COMPLEX xy2,y2,ret;
	double aux2;
	/////////////////////////////////////

	//1: conjugate complex

	y2.re= y.re;
	y2.im=-y.im;
	//////////////////////////////////////

	//2: numerator and denominator

	xy2 = x * y2;//complex
	aux2= y.re*y.re + y.im*y.im;
	//////////////////////////////////////

	//3: final solution

	if(fabs(aux2)<SMLL)
	{
		printf("\n\nError! --- %ld",__LINE__);
		printf("\n  Divided by zero: aux2 = %lf",aux2);

		exit(0);
	}//end if

	ret.re = xy2.re/aux2;
	ret.im = xy2.im/aux2;
	//////////////////////////////////////

	return ret;
}

void catArgv(char *arg,char **argv,long argc)
{
        if(argc==1){ strcpy(arg,"None"); return; }
        register long i;

        strcpy(arg,argv[1]);//init.
        for(i=2;i<argc;i++)
        {
                strcat(arg," ");
                strcat(arg,argv[i]);
        }//end for
        //////////////////////////////////////

        return;
}

long inArgv(char *str1,char *str2,char **argv,long argc)
{
	long ret=inArgv(str1,argv,argc);

	if(ret!=YES) ret=inArgv(str2,argv,argc);
	return ret;
}

long inArgv(char *str,char **argv,long argc)
{
	register long i;
	long ret=NO;//init

	char strbuf1[256];
	char strbuf2[256];
	////////////////////////////////////////

	//1: getting strbuf1[]

	strcpy(strbuf1,str);
	lowerCase(strbuf1);
	////////////////////////////////////////

	//2: comparing strings

	for(i=1;i<argc;i++)//No Change!
	{
		strcpy(strbuf2,argv[i]);
		lowerCase(strbuf2);

		if(!strcmp(strbuf1,strbuf2))
		{
			ret=YES;
			break;
		}	
	}//end for
	////////////////////////////////////////
	
	return ret;
}

void printSparMatrix(FILE *out,double **matrix,long n,char *caption)
{ 
	printSparMatrix(out,matrix,n,n,caption); 
	return; 
}

void printSparMatrix(FILE *out,double **matrix,long m,long n,char *caption)
{
	register long i,j;
	fprintf(out,"\n%s",caption);
	/////////////////////////////////////////

	for(i=1;i<=m;i++)
	for(j=1;j<=n;j++)
	{
		if(fabs(matrix[i][j])<=EPS) continue;
		fprintf(out,"\n%6ld %6ld % 16.8lf",i,j,matrix[i][j]);
	}//end for
	/////////////////////////////////////////

	return;
}

void readMatrix(char *datfile,char *caption,double **a,long n)
{ 
	readMatrix(datfile,caption,a,n,n); 
	return; 
}

void readMatrix(char *datfile,char *caption,double **a,long m,long n)
{
	//1: positioning datablock

	FILE *in=fopen(datfile,"r");
	if(in==NULL){ printf("\n\nCannot open %s",datfile); exit(0); }
	
	long ret=posDataBlock(in,caption);
	if(ret==NO){ printf("\nCannot find %s in %s",caption,datfile); exit(0); }
	///////////////////////////////////////////////////////

	//2: reading data

	register long i,j;
	double fa;

	for(i=1;i<=m;i++)
	for(j=1;j<=n;j++)
	{
		if(fscanf(in,"%lf",&fa)==1) a[i][j]=fa;
		else
		{
			printf("\n\nError! --- %ld",__LINE__);
			exit(0);
		}		
	}//end for
	///////////////////////////////////////////////////////

	fclose(in);//datfile closed
	return;
}

void readSparMatrix(char *datfile,char *caption,double **a,long n)
{ 
	readSparMatrix(datfile,caption,a,n,n); 
	return; 
}

void readSparMatrix(char *datfile,char *caption,double **a,long m,long n)
{
	//1: positioning datablock

	FILE *in=fopen(datfile,"r");
	if(in==NULL){ printf("\n\nCannot open %s\n",datfile); exit(0); }
	
	long ret=posDataBlock(in,caption);
	if(ret==NO){ printf("\nCannot find %s in %s\n",caption,datfile); exit(0); }
	////////////////////////////////////////////////////////////

	//2: reading data --> a[][]

	initMatrix(a,m,n,0.0);//No Change!

	long ia,ib;
 	double fa;

	while(1)//unconditional loop
	{
		if(fscanf(in,"%ld%ld%lf",&ia,&ib,&fa)!=3) break;
		
		if(ia>=1 && ia<=m && ib>=1 && ib<=n) a[ia][ib]=fa;
		else
		{
			printf("\n\nError! --- %ld",__LINE__);

			printf("\n  datfile = %s",datfile);
			printf("\n  caption = %s",caption);
			
			printf("\n  ia,m = %ld,%ld",ia,m);
			printf("\n  ib,n = %ld,%ld",ib,n);

			exit(0);
		}		
	}//end for
	////////////////////////////////////////////////////////////

	fclose(in);
	return;
}

template<class TYPE>
void readVector(char *datfile,char *caption,TYPE *v,long n)
{ 
	readVector(datfile,caption,v,(long)1,n); 
	return; 
}

template<class TYPE>
void readVector(char *datfile,char *caption,TYPE *v,long n1,long n2)
{
	//1: positioning datablock

	FILE *in=fopen(datfile,"r");
	if(in==NULL){ printf("\n\nCannot open %s\n",datfile); exit(0); }

	long ret=posDataBlock(in,caption);
	if(ret==NO){ printf("\nCannot find %s in %s",caption,datfile); exit(0); }
	/////////////////////////////////////////////////

	//2: reading data --> v[]

	register long i;
	double fa;

	for(i=n1;i<=n2;i++)
	{
		if(fscanf(in,"%lf",&fa)==1) v[i]=(TYPE)fa;
		else
		{
			printf("\n\nError! --- %ld",__LINE__);
			
			printf("\n  datfile = %s",datfile);
			printf("\n  caption = %s",caption);

			printf("\n  n1=%ld, n2=%ld",n1,n2);
			printf("\n  i =%ld",i);

			exit(0);
		}//end else
	}//end for
	/////////////////////////////////////////////////

	fclose(in);//datfile closed
	return;
}

void printVector(FILE *out,long *vector,long n,char *caption)
{ 
	printVector(out,vector,(long)1,n,caption); 
	return; 
}

void printVector(FILE *out,long *vector,long n1,long n2,char *caption)
{
	register long i;
	fprintf(out,"\n%s",caption);

	for(i=n1;i<=n2;i++)
		fprintf(out,"\n% 6ld",vector[i]);
	
	return;
}

void printVector(FILE *out,double *vector,long n,char *caption)
{ 
	printVector(out,vector,(long)1,n,caption); 
	return; 
}

void printVector(FILE *out,double *vector,long n1,long n2,char *caption)
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
//Notes: by Wang GT
//  x[] --- data vector, n size
//  n   --- size of x[]: 1,2,...n
//  xk  --- given
/////////////////////////////////////////

        if(xk<=x[1]) return 1;//first segment
        if(xk>=x[n]) return n-1;//No Change!

        register long i;
        long ret=NO;//init.
		/////////////////////////////////////

        for(i=1;i<=n-1;i++)
        {
                if(xk>=x[i] && xk<x[i+1]){ ret=i; break; }
                if(xk<=x[i] && xk>x[i+1]){ ret=i; break; }
        }//end for
		/////////////////////////////////////

        return ret;
}

void leadingzero(char *date,char *format)
{
	long year =date_year(date,format);
	long month=date_month(date,format);
	long day  =date_day(date,format);
	/////////////////////////////////////////////////

	initString(date);

	if(!strcmp(format,"yyyy/mm/dd"))
		sprintf(date,"%ld/%02ld/%02ld",year,month,day);

	if(!strcmp(format,"mm/dd/yyyy"))
		sprintf(date,"%02ld/%02ld/%ld",month,day,year);
	/////////////////////////////////////////////////

	return;
}

void LTrim(char *string)
{
	register long i;

	long n=strlen(string);
	long k=n;//No Change!
	/////////////////////////////////////////

	for(i=0;i<n;i++)
	{
		if(isBlank(string[i])==NO)
		{
			k=i;
			break;
		}
	}//end for
	/////////////////////////////////////////

	transMove(string,-k,k,n);
	return;
}

void RTrim(char *string)
{
	register long i;
	long n=strlen(string);
	///////////////////////////////////

	for(i=n-1;i>=0;i--)
	{
		if(isBlank(string[i])==NO) break;
		string[i]='\0';
	}//end for
	///////////////////////////////////

	return;
}

void trimString(char *string)
{
	LTrim(string);
	RTrim(string);

	return;
}

void packString(char *string)
{
        register long i;
        
		long k,n=strlen(string);//init.
        char ch;
        ////////////////////////////////////////////////////////

		//1: getting string_sv[]
		
        char *string_sv=(char *)calloc(n+1,sizeof(char));
        if(string_sv==NULL){ printf("\n\nError! --- %ld",__LINE__); exit(0); }

        strcpy(string_sv,string);
        ////////////////////////////////////////////////////////

        //2: reforming string <-- string_sv[]

        initString(string,n);
        k=0;//init.

        for(i=0;i<n;i++)//No Change!
        {
                ch=string_sv[i];//crr. char. 

                if(ch=='-'){ string[k]=ch; k++; }
                if(ch=='_'){ string[k]=ch; k++; }
                if(ch=='#'){ string[k]=ch; k++; }

                if(ch>='A' && ch<='Z'){ string[k]=ch; k++; }
                if(ch>='a' && ch<='z'){ string[k]=ch; k++; }
                if(ch>='0' && ch<='9'){ string[k]=ch; k++; }
        }//end for
        ////////////////////////////////////////

        free(string_sv);
        return;
}

template<class TYPE>
double stat_mean(TYPE *v,long n)
{
	if(n<=0){ printf("\nError! --- %ld",__LINE__); exit(0); }

	double ret=stat_sum(v,n)/(double)n;
	return ret;
}

template<class TYPE>
double stat_sum(TYPE *v,long n)
{
	double ret=sum(v,n);
	return ret;
}

template<class TYPE>
double stat_stdev(TYPE *v,long n)
{
	//1: cal. sum and sum2

	double sum=stat_sum(v,n);
	double sum2=0.0;//init.

	register long i;
	for(i=1;i<=n;i++) sum2+= v[i]*v[i];
	/////////////////////////////////////////////

	//2: std. deviation

	double ret=0.0;//init.
	if(n>1)
	{
		aux2=(n*sum2-sum*sum)/(double)(n*(n-1));
		if(aux2<0.0){ printf("\nError! --- %ld",__LINE__); exit(0); }

		ret=sqrt(aux2);
	}//end if
	/////////////////////////////////////////////

	return ret;
}

template<class TYPE>
double stat_MSE(TYPE *v,long n)
{
	register long i;

	double err,mean;
	double sum=0.0;//init.
	/////////////////////////////////////

	mean=stat_mean(v,n);

	for(i=1;i<=n;i++)
	{
		err=v[i]-mean;
		sum+= err*err;
	}//end for

	double ret=0.0;//init.
	if(n>0) ret=sum/(double)n;
	/////////////////////////////////////

	return ret;
}

template<class TYPE>
double stat_MSE(TYPE *v,TYPE *vb,long n)
{
	register long i;
	
	double err;
	double sum=0.0;//init.
	/////////////////////////////////////

	for(i=1;i<=n;i++)
	{
		err=v[i]-vb[i];
		sum+= err*err;
	}//end for

	double ret=0.0;//init.
	if(n>0) ret=sum/(double)n;
	/////////////////////////////////////

	return ret;
}

template<class TYPE>
void shuffVector(TYPE *v,long n)
{
	register long i;
	long r1,r2;
	///////////////////////////////////////////

	srand(unsigned(time(NULL)));

	for(i=1;i<=2*n;i++)
	{
		r1=round(frand(1.0,n));
		r2=round(frand(r1,n));

		swapVectorElms(v,r1,r2);
	}//end for
	///////////////////////////////////////////

	return;
}

template<class TYPE>
void naturalVector(TYPE *v,long n)
{
	naturalVector(v,(long)1,n);
	return;
}

template<class TYPE>
void naturalVector(TYPE *v,long n1,long n2)
{
	register long i;
	for(i=n1;i<=n2;i++) v[i]=(TYPE)i;
	
	return;
}
	
long conditionMeet(double v,double vb,long mel)
{
	long ret=NO;//init.

	if(mel==EQ && fabs(v-vb)<SMLL) ret=YES;
	if(mel==LT && v<vb) ret=YES;
	if(mel==MT && v>vb) ret=YES;
		
	return ret;
}

long melfun(char *mel_str)
{
	long ret=EQ;//init.--- No Change!
	packString(mel_str);

	if(!strcmp(mel_str,"<")) ret=LT;
	if(!strcmp(mel_str,"=")) ret=EQ;
	if(!strcmp(mel_str,">")) ret=MT;

	if(!strcmp(mel_str,"<=")) ret=LT;
	if(!strcmp(mel_str,">=")) ret=MT;

	return ret;
}

double linearin(struct pointstr *point,long pointNum,double xi)
{
	register long i;

	double *x=(double *)calloc(pointNum+1,sizeof(double));
	double *y=(double *)calloc(pointNum+1,sizeof(double));

	if(y==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }
	//////////////////////////////////////////////////////

	//1: init. x[] and y[]

	for(i=1;i<=pointNum;i++)
	{
		x[i]=point[i].x;
		y[i]=point[i].y;
	}//end for
	//////////////////////////////////////////////////////

	//2: linear interpolation  

	long k=findk_ex(x,pointNum,xi);
	if(k==NO){ printf("\nError! --- %ld",__LINE__); exit(0); }

	double aux2=x[k+1]-x[k];
	if(fabs(aux2)<SMLL){ printf("\nError! --- %ld",__LINE__); exit(0); }

	double ret=y[k]+(xi-x[k])*(y[k+1]-y[k])/aux2;
	//////////////////////////////////////////////////////

	free(x);
	free(y);

	return ret;
}

template<class TYPE>
void mirrorSym(TYPE *v,long n)
{ 
	mirrorSym(v,(long)1,n); 
	return; 
}

template<class TYPE>
void mirrorSym(TYPE *v,long n1,long n2)
{
        register long i;
        
		long k;
        long ns=(n1+n2)/2;//No Change!

        for(i=n1;i<=ns;i++)
        {
                k=n2-i+n1;//No Change!
                swapVectorElms(v,i,k);
        }//end for
        //////////////////////////////////////   

        return;
}

long isBlank(char ch)
{
	if(ch==' ')  return YES;
	if(ch=='\n') return YES;
	if(ch=='\t') return YES;

	return NO;
}

void delLeadingChar(char *string,char ch)
{
	long n;
	while(1)//unconditional
	{
		if(string[0]!=ch) break;
		
		n=strlen(string);
		if(n==0) break;

		transMove(string,(long)-1,(long)1,n);
	}//end while 
	/////////////////////////////////

	return;
}

void delLeadingChar(char *string,char ch,long rptNum)
{
	register long i;
	long t,k,n;
	/////////////////////////////////

	for(i=1;i<=rptNum;i++)
	{
		k=lookup(string,ch);
		if(k==NO) break;

		n=strlen(string);
		if(n==0) break;

		t=-(k+1);
		transMove(string,t,k+1,n);
	}//end while 
	//////////////////////////////////

	return;
}

template<class TYPE>
TYPE* nextNode(TYPE *node,long rptNum)
{
	TYPE *np=node;
	while(np!=NULL)
	{
		np=np->next;

		rptNum--;
		if(rptNum==0) break;
	}//end while
	///////////////////////////////
	
	return np;
}

template<class TYPE>
TYPE* chainNode(TYPE *chain,long index)
{
	if(index>chainLen(chain)) return NULL;
	if(index<=0) return NULL;
	///////////////////////////////////////

	long k=0;//init.
	TYPE *cp=chain;

	while(cp!=NULL)
	{
		k++;
		if(k==index) break;

		cp=cp->next;
	}//end while
	///////////////////////////////////////

	return cp;
}

template<class TYPE>
void truncChain(TYPE *&chain,long n)
{
	if(n<=0){ freeChain(chain); return; }
	if(n>=chainLen(chain)) return;

	TYPE *cp=chainNode(chain,n);
	if(cp==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }
	
	freeChain(cp->next);
	cp->next=NULL;
	////////////////////////////////////

	return;
}

template<class TYPE>
TYPE sumRowVector(TYPE **a,TYPE *v,long ik,long n)
{
	register long j;
	TYPE ret=(TYPE)0;//init.

	for(j=1;j<=n;j++) ret+= a[ik][j]*v[j];
	return ret;
}

template<class TYPE>
TYPE sum(TYPE *v,long n)
{ 
	TYPE ret=sum(v,(long)1,n);
	return ret;
}

template<class TYPE>
TYPE sum(TYPE *v,long n1,long n2)
{
	register long i;
	TYPE ret=(TYPE)0.0;//No Change!

	for(i=n1;i<=n2;i++) ret+= v[i];
	return ret;
}

template<class TYPE>
TYPE sum(TYPE *v1,TYPE *v2,long n)
{
	register long i;
	TYPE ret=(TYPE)0;//init.

	for(i=1;i<=n;i++) ret+= v1[i]*v2[i];
	return ret;
}

template<class TYPE>
TYPE sum(TYPE **a,long i1,long j1,long m,long n)
{
	register long i,j;
	TYPE ret=(TYPE)0;//init.

	for(i=i1;i<=m;i++)
		for(j=j1;j<=n;j++) ret+= a[i][j];
	
	return ret;
}

template<class TYPE>
long posChainNode(TYPE *chain,TYPE *node)
{
	register long i;
	
	long n=chainLen(chain);
	long ret=NO;//init.
	//////////////////////////////////////
	
	TYPE *cp=chain;
	for(i=1;i<=n;i++,cp=cp->next)
	{
		if(cp==node)
		{ 
			ret=i;
			break;
		}
	}//end for
	//////////////////////////////////////

	return ret;
}	

double timefun(clock_t start)
{
	double ret=(double)(clock()-start)/CLOCKS_PER_SEC;
	return ret;
}

long mthdaysFun(long month,long year)
{
	if(month<1 || month>12)
	{
		printf("\n\nError! --- %ld",__LINE__);
		printf("\n  month = %ld",month);

		exit(0);
	}//end if
	////////////////////////////////////////////////

	long ret=31;//init.

	if(month==4 || month==6 || month==9 || month==11) ret=30;
	if(month==2)
	{
		ret=28;//init.
		if(isLeapYear(year)==YES) ret=29;
	}//end if
	///////////////////////////////////////////////

	return ret;
}

void yesterday(char *date,char *format)
{
	long year =date_year(date,format);
	long month=date_month(date,format);
	long day  =date_day(date,format);
	//////////////////////////////////////////

	//1: cal. yesterday

	day--;
	if(day==0)//month-end
	{ 
		month--;
		if(month==0)//year-end
		{
			year--;
			month=12;
		}//end if

		day=mthdaysFun(month,year);
	}//end if
	//////////////////////////////////////////

	//2: updating date[] 

	if(!strcmp(format,"yyyy/mm/dd"))
		sprintf(date,"%ld/%02ld/%02ld",year,month,day);

	if(!strcmp(format,"mm/dd/yyyy"))
		sprintf(date,"%02ld/%02ld/%ld",month,day,year);
	//////////////////////////////////////////

	return;
}

long monthFun(char *month)
{
	if(!strncmp(month,"Jan",3)) return 1;
	if(!strncmp(month,"Feb",3)) return 2;
	if(!strncmp(month,"Mar",3)) return 3;
	if(!strncmp(month,"Apr",3)) return 4;
	if(!strncmp(month,"May",3)) return 5;
	if(!strncmp(month,"Jun",3)) return 6;
	if(!strncmp(month,"Jul",3)) return 7;
	if(!strncmp(month,"Aug",3)) return 8;
	if(!strncmp(month,"Sep",3)) return 9;
	if(!strncmp(month,"Oct",3)) return 10;
	if(!strncmp(month,"Nov",3)) return 11;
	if(!strncmp(month,"Dec",3)) return 12;

	return NO;//failed
}

void wtime(char *time_str)
{
	char timebuf[100];

	time_t t=time(NULL);
	strcpy(timebuf,ctime(&t));//Wed Mar  5 09:43:43 2003
	/////////////////////////////////////////////

	//1: removing useless parts 

	token=strtok(timebuf," ");//"Wed" extracted 
	if(token==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }

	token=strtok(NULL," ");//Mar
	if(token==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }

	token=strtok(NULL," ");//5
	if(token==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }
	/////////////////////////////////////////////

	//2: forming time_str[]

	token=strtok(NULL," ");//09:43:43
	if(token==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }

	strcpy(time_str,token);
	/////////////////////////////////////////////

	return;
}

void date(char *date_str)
{
	date(date_str,"mm/dd/yyyy");//default format
	return;
}

void date(char *date_str,char *format)
{
//Notes: by Wang GT
//  date_str --- buffer for date string
//  format   --- date format: 
//               "mm/dd/yyyy" or "yyyy/mm/dd"
//////////////////////////////////////////////////

        char datebuf[100];
        
        long year;
        long month;
        long day;
        ////////////////////////////////////////////

        //1: init. datebuf[]

        time_t t=time(NULL);
        strcpy(datebuf,ctime(&t));//Wed Mar  5 09:43:43 2003

        token=strtok(datebuf," ");//"Wed " removed
        if(token==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }
        ////////////////////////////////////////////

        //2: month, day, and year

        token=strtok(NULL," ");//Mar
        if(token==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }
        
        month=monthFun(token);
        ////////////////////////////////////////////

        //2.2 day

        token=strtok(NULL," ");//5
        if(token==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }

        day=atol(token);
        ////////////////////////////////////////////

        //2.3 year

        token=strtok(NULL," ");//09:43:43
        if(token==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }
		
        token=strtok(NULL," ");//2003
        if(token==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }

        year=atol(token);
        ////////////////////////////////////////////

        //3: forming date_str 

        initString(date_str);

        if(!strcmp(format,"mm/dd/yyyy"))
		sprintf(date_str,"%02ld/%02ld/%ld",month,day,year);
 
        if(!strcmp(format,"yyyy/mm/dd"))
		sprintf(date_str,"%ld/%02ld/%02ld",year,month,day);      
        ////////////////////////////////////////////

        return;
}

void date_time(char *string)
{
	char datebuf[100];
	char timebuf[100];

	date(datebuf);
	wtime(timebuf);
        
	sprintf(string,"%s %s",datebuf,timebuf);
	return;
}

void wait(double sec)
{
	clock_t start=clock();//init.
	double elaps;
		
	while(1)
	{
		elaps=timefun(start);
		if(elaps>=sec) break;
	}//end while
	//////////////////////////////////////
	
	return;
}

double LSM_vfun(double *x,double *y,long k,long n)
{
	register long i;
	
	double ret=0.0;//init.
	for(i=1;i<=n;i++) ret+= y[i]*pow(x[i],k);

	return ret;
}

double LSM_sfun(double *x,long k,long n)
{
	register long i;
	
	double ret=0.0;//init.
	for(i=1;i<=n;i++) ret+= pow(x[i],k);

	return ret;
}

double LSM_forecast(double *y,double yk,double xk,double xf,long rank,long n,long cctFlg,long prnt)
{
//Notes: by WangGT/////////////////////////////////////////////////////
//  y[]  --- sampled data, n size: 1,2,...n
//  yk   --- y-coord. of given point
//  xk   --- x-coord. of given point
//  xf   --- x-coord. of forecast point
//  rank --- rank of LSM fitting
//  n    --- size of y[]
//  cctFlg --- corr. const. term, YES or NO
//  ret.   --- forecast y-coor. for xf
////////////////////////////////////////////////////////////////////////
	
	if(prnt==YES) printf("\n\nLSM_forecast()...");

	double *LSM_coeff=(double *)calloc(rank+1,sizeof(double));
	double *x=(double *)calloc(n+1,sizeof(double));

	if(x==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }
	/////////////////////////////////////////////////////

	//1: cal. LSM coeff

	naturalVector(x,n);
	if(prnt==YES) printf("\n x[] formed");
	
	CF_LSMethod(LSM_coeff,x,y,rank,n,NO);//prnt
	if(prnt==YES) printf("\n CF_LSMethod() ok");
	/////////////////////////////////////////////////////

	//2: cal. the forecast-value

	if(cctFlg==YES) 
	{
		LSM_coeff[0]+= yk-polynomial(LSM_coeff,xk,rank);
		if(prnt==YES) printf("\n Const. term corrected");
	}//end if
	
	double ret=polynomial(LSM_coeff,xf,rank);
	/////////////////////////////////////////////////////

	//3: free memory

	free(LSM_coeff);
	free(x);
	//////////////////////////////////

	return ret;
}

double polynomial(double *a,double x,long n)
{
	register long i;
	
	double ret=a[0];//init.
	for(i=1;i<=n;i++) ret+= a[i]*pow(x,i);

	return ret;
}

double SIForm_a(double x1,double x2,double y1,double y2)
{
	if(fabs(x1-x2)<SMLL){ printf("\nError! --- %ld",__LINE__); exit(0); }

	double ret=(y2-y1)/(x2-x1);
	return ret;
}

double SIForm_b(double x1,double x2,double y1,double y2)
{
	if(fabs(x1-x2)<SMLL){ printf("\nError! --- %ld",__LINE__); exit(0); }

	double ret=y1-x1*(y2-y1)/(x2-x1);
	return ret;
}

void CF_LSMethod(double *a,double *x,double *y,long m,long n,long prnt)
{
//Notes: by Wang GT//////////////////////////////////
//a[]   --- solution of polynomial-coeff, m size
//x,y[] --- sampled data, n size
//m     --- rank/size of a[]: 0,1,...m
//n     --- size of x[], y[]: 1,2,...n
////////////////////////////////////////////////////////

	if(prnt==YES) printf("\n\nCF_LSMethod()...");

	register long i,j;

	double *_s,**s;
	makeMatrix(_s,s,m+2,m+2);//No Change!

	double *a2=(double *)calloc(m+2,sizeof(double));
	double *v =(double *)calloc(m+2,sizeof(double));

	if(v==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }
	////////////////////////////////////////////////////

	//1: cal. s[][] and v[]

	for(i=0;i<=m;i++)
	{
		v[i]=LSM_vfun(x,y,i,n);//(2-20)
		for(j=0;j<=m;j++) s[i][j]=LSM_sfun(x,i+j,n);//(2-19)
	}//end for

	if(prnt==YES) typeMatrix(s,(long)0,m,m,"s[][]");
	if(prnt==YES) typeVector(v,(long)0,m,"v[]");
	////////////////////////////////////////////////////

	//2: cal a[] --- coeff vector

	//.1 low-right translation

	transMove(s,(long)1,(long)1,(long)0,(long)0,m,m);
	transMove(v,(long)1,(long)0,m);

	if(prnt==YES) typeMatrix(s,m+1,m+1,"s[][]");
	if(prnt==YES) typeVector(v,m+1,"v[]");

	//.2 gaussPP function

	gaussPP(a2,s,v,m+1);//No Change!
	if(prnt==YES) typeVector(a2,m+1,"a2[]");

	//.3 back translation

	transMove(a2,-1,1,m+1);
	copyVector(a,a2,(long)0,m);
	////////////////////////////////////////////////////

	//3: free memory

	freeMatrix(_s,s);
	free(a2);
	free(v);
	//////////////////////////////////////////

	return;
}


void CF_splin(double *ys,double *ys1,double *x,double *y,double *xs,long n,long ns)
{
//NOTATION:////////////////////////////////////////////////
// ys[]     --- solution, spline func.value, ns dimension
// ys1[]    --- solution, first derivative, ns dimension
// x[], y[] --- sampled data, n dimension
// xs[]     --- given data, ns dimension
// n        --- dimension of x[], y[]: 0,1,...n
// ns       --- dimension of xs[],ys[],ys1[]: 0,1,...ns
///////////////////////////////////////////////////////////

	register long i,j;
	long k;
  
	double aux2,aux4,haux,xb,xj;
	double ld,dd,un,dn;

	double *ys2=(double *)calloc(n+2,sizeof(double));
	if(ys2==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }
	////////////////////////////////////////////////////

	//1: cal. ys2[] --- second deriv.

	ld=0.0;//init.
	dd=0.0;
	un=0.0;
	dn=0.0;

	splinfun(ys2,x,y,ld,dd,un,dn,n);
	////////////////////////////////////////////////////

	//2: cal. ys[] and ys1[]

	for(j=1;j<=ns;j++)
	{
		//2.1 find k

		k=1;
		for(i=1;i<=n-1;i++)
		{ 
			if(xs[j]<=x[i]){ k=i; break;}
			else k=i+1;
		}//end for
		//////////////////////////////////

		//2.2 preparation

		haux=x[k]-x[k-1];

		xb=x[k]-xs[j];	//xs[j] <===> x
		xj=xs[j]-x[k-1];
		//////////////////////////////////

		//2.3 cal. ys[]

		aux2=y[k-1]-ys2[k-1]*haux*haux/6.0;
		aux4=y[k]-ys2[k]*haux*haux/6.0;
		aux2=aux2*xb+aux4*xj;

		ys[j]=(ys2[k-1]*pow(xb,3.0)/6.0+ys2[k]*pow(xj,3.0)/6.0+aux2)/haux;
		//////////////////////////////////
	
		//2.4 cal. ys1[]

		aux2=-ys2[k-1]*xb*xb/(2.0*haux);
		aux4=ys2[k]*xj*xj/(2.0*haux);
		aux2=aux2+aux4;

		ys1[j]=aux2+(y[k]-y[k-1])/haux-(ys2[k]-ys2[k-1])*haux/6.0;
	}//end for
	////////////////////////////////////////////////////

	free(ys2);
	return;
}

void splinfun(double *ys2,double *x,double *y,double ld,double dd,double un,double dn,long n)
{ 
//NOTATION:////////////////////////////////////////////////////
// ys2[]	--- solution, the second derivative
// x[], y[]	--- data
// n		--- data index: 0,1,2, ... n
// ld,dd,un,dn	--- boundary condition: usu. 0.0
//////////////////////////////////////////////////////////

	//0: local definition and allocation

	register long i;
	long k,n2;

	double *a=(double *)calloc(n+2,sizeof(double));
	double *b=(double *)calloc(n+2,sizeof(double));
	double *c=(double *)calloc(n+2,sizeof(double));
	double *h=(double *)calloc(n+2,sizeof(double));
	double *p=(double *)calloc(n+2,sizeof(double));
	double *q=(double *)calloc(n+2,sizeof(double));

	if(q==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }
	/////////////////////////////////////////////////////////////

	//1: initialization

	for(i=0;i<=n;i++) ys2[i]=y[i];

	for(k=1;k<=2;k++)
		for(i=n;i>=k;i--) ys2[i]=(ys2[i]-ys2[i-1])/(x[i]-x[i-k]);

	h[1]=x[1]-x[0];
	for(i=1;i<=n-1;i++)
	{
		h[i+1]=x[i+1]-x[i];
		c[i]=h[i+1]/(h[i]+h[i+1]);
		b[i]=2.0;
		a[i]=1.0-c[i];
		ys2[i]=6.0*ys2[i+1];
	}//end for
	////////////////////////////////////

	ys2[0]= dd;//init.
	ys2[n]= dn;
	a[n]  = un;
	b[0]  = 2.;
	b[n]  = 2.;
	c[0]  = ld;
	////////////////////////////////////

	//2: tss solution

	for(i=n+1;i>=1;i--)
	{
		a[i]=a[i-1];
		b[i]=b[i-1];
		c[i]=c[i-1];
		ys2[i]=ys2[i-1];
	}//end for
	
	n2=n+1;
	tssfun(ys2,a,b,c,ys2,q,p,n2);

	for(i=0;i<=n;i++)
		ys2[i]=ys2[i+1];
	////////////////////////////////////
	
	//3: free memory

	free(a);
	free(b);
	free(c);
	free(h);
	free(p);
	free(q);
	////////////////////////////////////
  
	return;
}

void tssfun(double *x,double *a,double *b,double *c,double *d,double *q,double *p,long n)
{
	register long i;
	
	double *y=(double *)calloc(n+1,sizeof(double));
	if(y==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }
	///////////////////////////////////////////

	//1: forward iteration

	q[1]=b[1];//init.
	y[1]=d[1];

	for(i=2;i<=n;i++)
	{		
		if(fabs(q[i-1])<SMLL){ printf("\nError! --- %ld",__LINE__); exit(0); }
		
		p[i]=a[i]/q[i-1];
		q[i]=b[i]-p[i]*c[i-1];
		y[i]=d[i]-p[i]*y[i-1];
	}//end for
	///////////////////////////////////////////

	//2: backward iteration

	if(fabs(q[n])<SMLL){ printf("\nError! --- %ld",__LINE__); exit(0); }
	x[n]=y[n]/q[n];

	for(i=n-1;i>=1;i--)
	{
		if(fabs(q[i])<SMLL){ printf("\nError! --- %ld",__LINE__); exit(0); }
		x[i]=(y[i]-c[i]*x[i+1])/q[i];
	}//end for
	///////////////////////////////////////////

	free(y);//memory freed
	return;
}

double frand(double num)
{
//Notes: by Wang GT
//  srand() should be called before as follows:
//  srand(unsigned(time(NULL)));
/////////////////////////////////////////////////

	double ret=frand(0.0,num);
	return ret;
}

double frand(double nmin,double nmax)
{
//Notes: by Wang GT
//  srand() should be called before as follows:
//  srand(unsigned(time(NULL)));
/////////////////////////////////////////////////

	double r=(double)rand()/RAND_MAX;
	double ret=cutPoint(nmin,nmax,r);

	return ret;
}

template<class TYPE>
void appendNode(TYPE *&chain,TYPE *node)
{
	if(node!=NULL) node->next=NULL;//No Change!
	
	TYPE *tp=tailNode(chain);

	if(tp==NULL) chain=node;
	if(tp!=NULL) tp->next=node;

	return;
}

template<class TYPE>
TYPE* tailNode(TYPE *chain)
{
	TYPE *cp=chain;
	while(cp!=NULL)
	{
		if(cp->next==NULL) break;
		cp=cp->next;
	}//end while
	/////////////////////////////
	
	return cp;
}

template<class TYPE>
TYPE* frontNode(TYPE *chain,TYPE *node)
{
	TYPE *cp=chain;
	while(cp!=NULL)
	{
		if(cp->next==node) break;
		cp=cp->next;
	}//end while
	//////////////////////////////////

	return cp;
}

double vsimp(double (*fun)(double),double a,double b,double eps,double h0)
{
//Notes: by Wang GT
//  fun()   --- the given function: double fun(double x)
//	[a,b]   --- integral interval
//	esp     --- convergent precision
//	h0      --- step threshold: convergent judgement not made
//              until steps less than h0
//	ret.    --- integral value by vsimp. method
////////////////////////////////////////////////////////////////

	register long i,j;
	long n=1;//init.--- No Change!

	double sc,sump;
	
	double h=(b-a)/2.0;
	double x=a+h;

	double sp=(*fun)(a)+(*fun)(b)+2.0*(*fun)(x);//No Change!
	double sum=(sp+2.0*(*fun)(x))*h/3.0;
	//////////////////////////////////////////////

	for(i=2;;i++)//unconditional
	{
		sump=sum;
		n+= n;//No Change!

		sc=0.0;//init.
		x=a-0.5*h;

		for(j=1;j<=n;j++)
		{
			x+= h;
			sc+= (*fun)(x);
		}//end for

		h*= 0.5;
		sum=(4.0*sc+sp)*h/3.0;
		/////////////////////////////////////

		if(fabs(h)<h0 && fabs(sum-sump)<eps) break;
		else sp+= 2.0*sc;

		if(i>BIGNUM)
		{
			printf("\n\nWarning! --- %ld",__LINE__);
			printf("\n  NOT convergent vsimp(): i = %ld",i);
			
			break;
		}//end if
	}//end for
	//////////////////////////////////////////////

	return sum;
}

void invMatrixGJ(double **a,long n)
//Gauss-Jordan method
{
	//1: local defintion and init.

	register long i,k;
	
	long imax;
	double max,aux2,*_E,**E;

	makeMatrix(_E,E,n+1,n+1);
	unitMatrix(E,n);//init.
	/////////////////////////////////////////////

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
		///////////////////////////////

		//2.2 rows swapping

		if(imax!=k)
		{ 
			swapMatrixRows(a,imax,k,n);
			swapMatrixRows(E,imax,k,n);
		}//end if
		///////////////////////////////
	
		//2.3 unitization

		aux2=a[k][k];
		if(fabs(aux2)<SMLL){ printf("\n\nError! --- %ld",__LINE__); exit(0); }

		divMatrixRow(a,k,n,aux2);
		divMatrixRow(E,k,n,aux2);
		///////////////////////////////

		//2.4 expunction

		for(i=1;i<=n;i++)
		{
			if(i==k) continue;//No Change!
			aux2=a[i][k]/a[k][k];

			add_mulrow(a,i,k,-aux2,n);
			add_mulrow(E,i,k,-aux2,n);
		}//end for
	}//end for 
	/////////////////////////////////////////////

	//3: settlement

	copyMatrix(a,E,n,n);
	freeMatrix(_E,E);

	return;
}

template<class TYPE>
TYPE stat_maxErr(TYPE *v,TYPE *vb,long n)
{
	register long i;
	TYPE err,ret;
	////////////////////////////////////

	ret=(TYPE)0;//init.
	for(i=1;i<=n;i++)
	{
		err=v[i]-vb[i];
		if(fabs(err)>ret) ret=err;
	}//end for
	////////////////////////////////////
 
	return ret;
}

template<class TYPE>
TYPE stat_maxErrpct(TYPE *v,TYPE *vb,long n)
{
	register long i;
	double err,ret=0.0;//init.
	////////////////////////////////////

	for(i=1;i<=n;i++)
	{
		if(fabs(vb[i])<SMLL) continue;

		err=(v[i]-vb[i])/(double)vb[i];
		if(fabs(err)>ret) ret=err;
	}//end for
	////////////////////////////////////
 
	ret=(TYPE)(ret*100);//pct
	return ret;
}

template<class TYPE>
long oppSign(TYPE num1,TYPE num2)
{
	long ret=NO;//init.

	if(num1>SMLL && num2<-SMLL) ret=YES;
	if(num1<-SMLL && num2>SMLL) ret=YES;

	return ret;
}

double fabsmax(double v1,double v2)
{
	v1=fabs(v1);
	v2=fabs(v2);

	if(v1>v2) return v1;
	return v2;
}	

double fabsmax(double *v,long n1,long n2)
{
	long ret=posAbsmax(v,n1,n2);
	return ret;
}

double fabsmax(double *v,long n)
{
	double ret=fabsmax(v,(long)1,n);
	return ret;
}

template<class TYPE>
void delMatrixRow(TYPE **a,long ik,long m,long n)
{
	register long i,j;

	for(i=ik;i<=m-1;i++)
		for(j=1;j<=n;j++) a[i][j]=a[i+1][j];

	return;
}

template<class TYPE>
void delMatrixCol(TYPE **a,long jk,long m,long n)
{
	register long i,j;

	for(i=1;i<=m;i++)
		for(j=jk;j<=n-1;j++) a[i][j]=a[i][j+1];

	return;
}

template<class TYPE>
void insertVectorElm(TYPE *v,TYPE elm,long k,long n)
{
//Notes: by Wang GT
//  v[] --- n-sized vector
//  elm --- elm. to be inserted
//  k   --- insertion pos.
//  n   --- full dimension of v[]
////////////////////////////////////////////

	register long i;
	for(i=n;i>=k+1;i--)	v[i]=v[i-1];

	v[k]=elm;//elm. inserted
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
void initMatrixCol(TYPE **a,long jk,long m,TYPE v)
{
	register long i;
	for(i=1;i<=m;i++) a[i][jk]=v;

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
{
	initVector(a,(long)1,n,v);
	return;
}

template<class TYPE>
void initVector(TYPE *a,long n1,long n2,TYPE v)
{
	register long i;
	for(i=n1;i<=n2;i++)	a[i]=v;
	
	return;
}

template<class TYPE>
void divMatrixRow(TYPE **a,long ik,long n,TYPE div)
{
	divMatrixRow(a,ik,(long)1,n,div);
	return;
}

template<class TYPE>
void divMatrixRow(TYPE **a,long ik,long n1,long n2,TYPE div)
{
	if(fabs(div)<SMLL){ printf("\nError! --- %ld",__LINE__); exit(0); }

	register long j;
	for(j=n1;j<=n2;j++) a[ik][j]/= div;
	
	return;
}

template<class TYPE>
void divMatrix(TYPE **a,long m,long n,TYPE div)
{
	if(fabs(div)<SMLL){ printf("\nError! --- %ld",__LINE__); exit(0); }

	register long i,j;

	for(i=1;i<=m;i++)
		for(j=1;j<=n;j++) a[i][j]/= div;

	return;
}

template<class TYPE>
void initMatrix(TYPE **a,long m,long n,TYPE v)
{
	initMatrix(a,(long)1,(long)1,m,n,v);
	return;
}

template<class TYPE>
void initMatrix(TYPE **a,long i1,long j1,long m,long n,TYPE v)
{
	register long i,j;

	for(i=i1;i<=m;i++)
		for(j=j1;j<=n;j++) a[i][j]=v;

	return;
}

template<class TYPE>
void negMatrix(TYPE **a,long m,long n)
{
	register long i,j;

	for(i=1;i<=m;i++)
		for(j=1;j<=n;j++) a[i][j]*= (TYPE)-1.0;

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
long isUnitCol(TYPE **a,long jk,long m)
{
	TYPE *col=(TYPE *)calloc(m+1,sizeof(TYPE));
	if(col==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }

	copyVectorCol(col,a,jk,m);
	long ret=isUnitVector(col,m);

	free(col);//memory freed
	return ret;
}

template<class TYPE>
long isUnitVector(TYPE *v,long n)
{
	register long i;

	long ret =YES;//init.
	long flg1=NO;

	TYPE elm;
	///////////////////////////////

	for(i=1;i<=n;i++)
	{
		//.1 elm must be 0 or 1

		elm=v[i];
		if(fabs(elm)>SMLL && fabs(elm-1)>SMLL)
		{
			ret=NO;
			break;
		}//end if
		//////////////////////////////////

		//.2 only one would be 1

		if(fabs(elm-1)<=SMLL)
		{
			if(flg1==NO) flg1=YES;//setting flg1
			else
			{
				ret=NO;
				break;
			}
		}//end if
	}//end for
	//////////////////////////////////////

	return ret;
}

template<class TYPE>
long isUnitMatrix(TYPE **a,long n)
{
	register long i,j;

	for(i=1;i<=n;i++)//diagonal elements
		if(fabs((double)a[i][i]-1.0)>SMLL) return NO;			

	for(i=1;i<=n;i++)//non-diagonal
	{
		for(j=1;j<=i-1;j++)
			if(fabs((double)a[i][j])>SMLL) return NO;

		for(j=i+1;j<=n;j++)
			if(fabs((double)a[i][j])>SMLL) return NO;
	}//end for
	////////////////////////////////////////

	return YES;
}

template<class TYPE>
void subVector(TYPE *sub,TYPE *v,long *index,long n)
{
	register long i;
	for(i=1;i<=n;i++) sub[i]=v[index[i]];

	return;
}

template<class TYPE>
void subMatrix(TYPE **sub,TYPE **a,long *icol,long m,long n)
{
//Notes: by Wang GT
// sub[][]  --- returned submatrix, m * n size
// a[][]    --- given matrix, m * ? size
// icol[]   --- index of column, n size
// m, n     --- the dimensions
///////////////////////////////////////////////////

	register long i,j;

	for(i=1;i<=m;i++)
		for(j=1;j<=n;j++) sub[i][j]=a[i][icol[j]];
	
	return;
}

template<class TYPE>
void combinChainNodes(TYPE *chain)
{
        TYPE *cp=chain;//init.
        TYPE *cp2;
		TYPE *cp2_next;
        ///////////////////////////////////////

        while(cp!=NULL)
        {
                cp2=cp->next;
                while(cp2!=NULL)
                {
					if(match(cp,cp2)!=YES)
					{
						cp2=cp2->next;
						continue;
					}//end if
					////////////////////////////////

					combinData(cp,cp2);//cp <-- cp2
					cp2_next=cp2->next;

					delChainNode(chain,cp2);
					cp2=cp2_next;
                }//end while

                cp=cp->next;
        }//end while
        ///////////////////////////////////////

        return;
}

template<class TYPE>
long boundary_on(TYPE v,TYPE L,TYPE U)
{
	if(fabs(U)>SMLL && L>U)
	{
		printf("\n\nError! --- %s: %ld",__FILE__,__LINE__);
		printf("\n  L=%lf, U=%lf",L,U);

		exit(0);
	}//end if
	//////////////////////////////////////

	double pct5=fabs(U-L)*0.05;

	if(fabs(U)<SMLL) return LOWER;
	if(v<L+pct5)     return LOWER;
	if(v>U-pct5)     return UPPER;
	///////////////////////////////////////

	return MIDDL;
}

template<class TYPE>
long boundary_to(TYPE v,TYPE L,TYPE U)
{
	if(fabs(U)>SMLL && L>U)
	{
		printf("\n\nError! --- %s: %ld",__FILE__,__LINE__);
		printf("\n  L=%lf, U=%lf",L,U);

		exit(0);
	}//end if
	///////////////////////////////////////

	double mid=0.5*(L+U);//init.

	if(fabs(U)<SMLL) return LOWER;
	if(v<mid-SMLL)   return LOWER;
	if(v>mid+SMLL)   return UPPER;
	////////////////////////////////////////

	return MIDDL;
}

template<class TYPE>
void copyChain(TYPE *&chain_dest,TYPE *chain_src)
{
	if(chain_src==NULL){ freeChain(chain_dest); return; }
	if(chain_dest!=NULL) freeChain(chain_dest);

	chain_dest= NULL;//init.
	TYPE *cp2 = NULL;
	TYPE *cp  = chain_src;
	////////////////////////////////////////

	while(cp!=NULL)
	{
		cp2=(TYPE *)malloc(sizeof(TYPE));
		if(cp2==NULL){ printf("\nError! --- %ld",__LINE__); exit(0); }

		memcpy(cp2,cp,sizeof(TYPE));

		cp2->next=chain_dest;
		chain_dest=cp2;
			
		cp=cp->next;
	}//end while
	////////////////////////////////////////
	
	turnOver(chain_dest);
	return;
}

template<class TYPE>
void copyMatrixRow(TYPE **a,TYPE *v,long ik,long n)
{
	copyMatrixRow(a,v,ik,(long)1,n);
	return;
}

template<class TYPE>
void copyMatrixRow(TYPE **a,TYPE *v,long ik,long n1,long n2)
{
	register long j;
	for(j=n1;j<=n2;j++) a[ik][j]=v[j];

	return;
}

template<class TYPE>
void copyMatrixRow(TYPE **a_dest,long ik_dest,TYPE **a_src,long ik_src,long n)
{
	register long j;
	for(j=1;j<=n;j++) a_dest[ik_dest][j]=a_src[ik_src][j];

	return;
}

template<class TYPE>
void copyMatrixCol(TYPE **a_dest,TYPE **a_src,long j_dest,long j_src,long m)
{
	register long i;
	for(i=1;i<=m;i++) a_dest[i][j_dest]=a_src[i][j_src];

	return;
}

template<class TYPE>
void copyMatrixCol(TYPE **a,TYPE *v,long j,long m)
{
	register long i;
	for(i=1;i<=m;i++) a[i][j]=v[i];

	return;
}

template<class TYPE>
void copyMatrix(TYPE **m_dest,TYPE **m_src,long m,long n)
{
	copyMatrix(m_dest,m_src,(long)1,(long)1,m,n);
	return;
}

template<class TYPE>
void copyMatrix(TYPE **m_dest,TYPE **m_src,long i1,long j1,long m,long n)
{
	register long i,j;

	for(i=1;i<=m;i++)
		for(j=1;j<=n;j++) m_dest[i1+i-1][j1+j-1]=m_src[i][j];
	
	return;
}

template<class TYPE>
void copyVectorRow(TYPE *v,TYPE **a,long ik,long n)
{
	copyVectorRow(v,a,ik,(long)1,n);
	return;
}

template<class TYPE>
void copyVectorRow(TYPE *v,TYPE **a,long ik,long n1,long n2)
{
	register long j;
	for(j=n1;j<=n2;j++) v[j]=a[ik][j];

	return;
}

template<class TYPE>
void copyVectorCol(TYPE *v,TYPE **a,long jk,long m)
{
	register long i;
	for(i=1;i<=m;i++) v[i]=a[i][jk];

	return;
}

template<class TYPE>
void copyVector(TYPE *v_dest,TYPE *v_src,long n)
{ 
	copyVector(v_dest,v_src,(long)1,(long)1,n);
	return;
}

template<class TYPE>
void copyVector(TYPE *v_dest,TYPE *v_src,long nd1,long ns1,long ns2)
{
	register long i;
	for(i=ns1;i<=ns2;i++) v_dest[nd1+i-ns1]=v_src[i];

	return;
}

template<class TYPE>
void copyVector(TYPE *v_dest,TYPE *v_src,long n1,long n2)
{
	copyVector(v_dest,v_src,n1,n1,n2);
	return;
}

template<class TYPE>
void addChainNode(TYPE *&chain,TYPE *node)
{
	TYPE *cp=(TYPE *)malloc(sizeof(TYPE));
	if(cp==NULL){ printf("\n\nError! --- %ld",__LINE__); exit(0); }

	memcpy(cp,node,sizeof(TYPE));
	
	cp->next=chain;
	chain=cp;
	////////////////////////////////////

	return;
}

template<class TYPE>
void delChainNode(TYPE *&chain,TYPE *node)
{
        //1: deleting the head node

        TYPE *cp;
        if(node==chain)
        {
                cp=chain->next;

                free(chain);
                chain=cp;

				return;
        }//end if
        ///////////////////////////////////////////

        //2: deleting non-head node

		cp=chain;
		while(cp!=NULL)
		{
				if(cp->next==node)
                {
                        cp->next=node->next;

                        free(node);
                        break;
                }//end if 

				cp=cp->next;
        }//end while
        ///////////////////////////////////////

        return;
}

template<class TYPE>
long chainLen(TYPE *chain)
{
        long ret=0;//init.
        TYPE *cp=chain;
        
		while(cp!=NULL){ ret++; cp=cp->next; }
        return ret;
}

template<class TYPE>
TYPE fmax(TYPE *v,long n)
{
	TYPE ret=fmax(v,(long)1,n); 
	return ret;
}
 
template<class TYPE>
TYPE fmax(TYPE *v,long n1,long n2)
{
	TYPE ret=v[posmax(v,n1,n2)];
	return ret;
}

template<class TYPE>
TYPE fmin(TYPE *v,long n)
{
	TYPE ret=v[posmin(v,n)];
	return ret;
}

template<class TYPE>
void quickSort(TYPE *vector,long n)
{
	quickSort(vector,(long)1,n);
	return;
}

template<class TYPE>
void quickSort(TYPE *v,long i,long j)
{
        long ipivot=pivotFun(v,i,j);
        if(ipivot!=NO)
        {
                TYPE pivot=v[ipivot];
                long k=partition(v,i,j,pivot);

                quickSort(v,i,k-1);
                quickSort(v,k,j);
        }//end if
        //////////////////////////////////

        return;
}

template<class TYPE>
long pivotFun(TYPE *v,long i,long j)
{
        register long k;

		long ret=NO;//init.
        TYPE vkey=v[i];
		///////////////////////////////////

        for(k=i+1;k<=j;k++)
        {
                if(v[k]>vkey)
				{
					ret=k;
					break;
				}

				if(v[k]<vkey)
				{
					ret=i;
					break;
				}
        }//end for
		///////////////////////////////////
        
        return ret;
}

template<class TYPE>
long partition(TYPE *v,long i,long j,TYPE pivot)
{
        while(i<=j)
        {
                swapVectorElms(v,i,j);

                while(v[i]<pivot)  i++;
                while(v[j]>=pivot) j--;
        }//end while

        return i;
}

long posDataBlock(FILE *fp,char *caption)
{
	char linebuf[LINELN];

	long n=strlen(caption);
	long ret=NO;//init.

	rewind(fp);//No Change!
	///////////////////////////////////////

	while(!feof(fp))
	{
		if(fgets(linebuf,LINELN,fp)==NULL) break;
		if(!strncmp(linebuf,caption,n))
		{
			ret=YES;
			break;
		}
	}//end while
	///////////////////////////////////////

	return ret;
}
		
template<class TYPE>
long posNonzero(TYPE *v,long n)
{
	register long i;
	long ret=NO;//init.
	/////////////////////////////////

	for(i=1;i<=n;i++)
	{
		if(fabs(v[i])>SMLL)
		{
			ret=i;
			break;
		}
	}//end for
	////////////////////////////////

	return ret;
}

template<class TYPE>
long posAbsmax(TYPE *v,long n)
{ 
	long ret=posAbsmax(v,(long)1,n);
	return ret;
}

template<class TYPE>
long posAbsmax(TYPE *v,long n1,long n2)
{
	register long i;
	long imax=n1;//init.

	TYPE max =fabs(v[n1]);
	TYPE aux2;
	///////////////////////////////////

	for(i=n1+1;i<=n2;i++)
	{
		aux2=fabs(v[i]);
		if(aux2>max)
		{
			max =aux2;
			imax=i;
		}
	}//end for
	///////////////////////////////////

	return imax;
}

template<class TYPE>
long posmax(TYPE *v,long n)
{
	long imax=posmax(v,(long)1,n);
	return imax;
}

template<class TYPE>
long posmax(TYPE *v,long n1,long n2)
{
        register long i;

        long imax=n1;//init.
        TYPE max =v[n1];
		///////////////////////////////////

        for(i=n1+1;i<=n2;i++)
        {
                if(v[i]>max)
                {
					max=v[i];
                    imax=i;
                }
        }//end for
		///////////////////////////////////

        return imax;
}

template<class TYPE>
long posmin(TYPE *v,long n)
{
	long ret=posmin(v,(long)1,n);
	return ret;
}

template<class TYPE>
long posmin(TYPE *v,long n1,long n2)
{
        register long i;
		
        long imin=n1;//init.
        TYPE min =v[n1];
		//////////////////////////////////////

        for(i=n1+1;i<=n2;i++)
        {
			if(v[i]<min)
			{
				min =v[i]; 
				imin=i;
			}
		}//end for
		/////////////////////////////////

        return imin;
}

template<class TYPE>
void sortVector(TYPE *v,long n)
{
	sortVector(v,(long)1,n);
	return;
}

template<class TYPE>
void sortVector(TYPE *v,long n1,long n2)
{
        register long i;
        long imin;
        //////////////////////////////

        for(i=n1;i<=n2-1;i++)
        {
                imin=posmin(v,i,n2);
                if(imin!=i) swapVectorElms(v,i,imin);
        }//end for
        /////////////////////////////

        return;
}

template<class TYPE>
void swapMatrixElms(TYPE **a,long i,long j,long i2,long j2)
{
	TYPE elm  = a[i][j];
	a[i][j]   = a[i2][j2];
	a[i2][j2] = elm;

	return;
}

template<class TYPE>
void swapVectorElms(TYPE *v,long i,long j)
{
		TYPE elm = v[i];
        v[i]     = v[j];
        v[j]     = elm;

        return;
}

template<class TYPE>
void swapMatrixRows(TYPE **a,long ik1,long ik2,long n)
{
		register long j;
        for(j=1;j<=n;j++) swapMatrixElms(a,ik1,j,ik2,j);

        return;
}

template<class TYPE>
void addMatrixRow(TYPE **a,TYPE *v,long ik,long n)
{
	register long j;
	for(j=1;j<=n;j++) a[ik][j]+= v[j];

	return;
}

template<class TYPE>
void add_mulrow(TYPE **a,long ik,long i_add,TYPE mul,long n)
{
	add_mulrow(a,ik,i_add,mul,(long)1,n);
	return;
}

template<class TYPE>
void add_mulrow(TYPE **a,long ik,long i_add,TYPE mul,long n1,long n2)
{
//Notes: by Wang GT
// a[ik][] <== a[ik][] + a[i_add][]*mul
// a[ik][] changed, but a[i_add][] not.
//////////////////////////////////////////////////

        register long j;
        for(j=n1;j<=n2;j++) a[ik][j]+= a[i_add][j]*mul;

        return;
}

template<class TYPE>
void subtractVector(TYPE *v,TYPE *v_sub,long n)
{
	register long i;
	for(i=1;i<=n;i++) v[i]-= v_sub[i];

	return;
}

template<class TYPE>
void addVector(TYPE *v,TYPE *v_add,long n)
{
	addVector(v,v_add,(long)1,n);
	return;
}

template<class TYPE>
void addVector(TYPE *v,TYPE *v_add,long n1,long n2)
{
        register long i;
        for(i=n1;i<=n2;i++) v[i]+= v_add[i];

        return;
}
       
template<class TYPE>
void mulVector(TYPE *v,long n,TYPE mul)
{
	mulVector(v,(long)1,n,mul);
	return;
}

template<class TYPE>
void mulVector(TYPE *v,long n1,long n2,TYPE mul)
{
	register long i;
	for(i=n1;i<=n2;i++) v[i]*= mul;

	return;
}

template<class TYPE>
void absMatrix(TYPE **a,long m,long n)
{
	register long i,j;

	for(i=1;i<=m;i++)
		for(j=1;j<=n;j++) a[i][j]=(TYPE)fabs(a[i][j]);

	return;
}

template<class TYPE>
void absVector(TYPE *v,long n)
{
	register long i;
	for(i=1;i<=n;i++) v[i]=(TYPE)fabs(v[i]);

	return;
}

template<class TYPE>
void divVector(TYPE *v,long n,TYPE div)
{
	divVector(v,(long)1,n,div);
	return;
}

template<class TYPE>
void divVector(TYPE *v,long n1,long n2,TYPE div)
{
	if(fabs(div)<SMLL)
	{
		printf("\n\nError! --- %ld",__LINE__);
		printf("\n  Vector divided by zero: div = %lf",div);
	
		exit(0);
	}//end if
	////////////////////////////////////////

	register long i;
	for(i=n1;i<=n2;i++) v[i]/= div;

	return;
}

template<class TYPE>
void mulMatrixVector(TYPE *ret,TYPE **a,TYPE *v,long m,long n)
{
	register long i,k;
	TYPE sum;
	////////////////////////////////////////

	for(i=1;i<=m;i++)
	{
		sum=(TYPE)0;//init.
		for(k=1;k<=n;k++) sum+= a[i][k]*v[k];

		ret[i]=sum;
	}//end for
	///////////////////////////////////////
	
	return;
}

template<class TYPE>
void mulMatrixRow(TYPE **a,long ik,long n,TYPE mul)
{
	register long j;
	for(j=1;j<=n;j++) a[ik][j]*= mul;

	return;
}

template<class TYPE>
void mulMatrixCol(TYPE **a,long jk,long m,TYPE mul)
{
	register long i;
	for(i=1;i<=m;i++) a[i][jk]*= mul;

	return;
}

template<class TYPE>
void mulMatrix(TYPE **a,long m,long n,TYPE mul)
{
	mulMatrix(a,(long)1,(long)1,m,n,mul);
	return;
}

template<class TYPE>
void mulMatrix(TYPE **a,long i1,long j1,long m,long n,TYPE mul)
{
	register long i,j;

	for(i=i1;i<=m;i++)
		for(j=j1;j<=n;j++) a[i][j]*= mul;

	return;
}

template<class TYPE>
void mulMatrix(TYPE **a3,TYPE **a1,TYPE **a2,long m,long k,long n)
{
        register long i,j;
		register long kk;

        TYPE sum;
        TYPE *_atmp,**atmp;

		makeMatrix(_atmp,atmp,m+1,n+1);
        /////////////////////////////////////////////

        for(i=1;i<=m;i++)
    	{
			for(j=1;j<=n;j++)
			{
				sum=(TYPE)0;//init.
				for(kk=1;kk<=k;kk++) sum+= a1[i][kk]*a2[kk][j];
			
				atmp[i][j]=sum;
			}
		}//end for
        /////////////////////////////////////////////

        copyMatrix(a3,atmp,m,n);
        freeMatrix(_atmp,atmp);
		
        return;
}

template<class TYPE>
void unitVector(TYPE *v,long k,long n)
{
	initVector(v,n,(TYPE)0);
	v[k]=(TYPE)1;

	return;
}

template<class TYPE>
void unitMatrix(TYPE **a,long n)
{
        register long i,j;

        for(i=1;i<=n;i++)
        for(j=1;j<=n;j++)
        {
                if(j==i) a[i][j]=(TYPE)1;
                else     a[i][j]=(TYPE)0;

        }//end for
		//////////////////////////

        return;
}

template<class TYPE>
void makeMatrix(TYPE* &_a,TYPE** &a,long m,long n)
{
    _a=(TYPE *)calloc(m*n,sizeof(TYPE));
 	if(_a==NULL){ printf("\n\nError! --- %ld",__LINE__);  exit(0); }

    a=(TYPE **)calloc(m,sizeof(TYPE *));
 	if(a==NULL){ printf("\n\nError! --- %ld",__LINE__);  exit(0); }

    register long i;
    for(i=0;i<m;i++) a[i]=&_a[n*i];

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
void typeVector(TYPE *v,long n,char *caption)
{
	typeVector(v,(long)1,n,caption);
	return;
}

template<class TYPE>
void typeVector(TYPE *v,long n1,long n2,char *caption)
{
        register long i;

        printf("\n\n%s:\n",caption);
        for(i=n1;i<=n2;i++) printf("  %7.3lf",(double)v[i]);

        return;
}

template<class TYPE>
void ftypeVector(FILE *out,TYPE *v,long n,char *caption)
{
	ftypeVector(out,v,(long)1,n,caption);
	return;
}

template<class TYPE>
void ftypeVector(FILE *out,TYPE *v,long n1,long n2,char *caption)
{
        register long i;

        fprintf(out,"\n\n%s:\n",caption);
        for(i=n1;i<=n2;i++) fprintf(out,"  %7.3lf",(double)v[i]);

        return;
}

template<class TYPE>
void typeMatrix(TYPE **a,long m,long n,char *caption)
{
	typeMatrix(a,(long)1,m,n,caption);
	return;
}

template<class TYPE>
void typeMatrix(TYPE **a,long mn0,long m,long n,char *caption)
{
        register long i,j;

        printf("\n\n%s:",caption);
        for(i=mn0;i<=m;i++)
        {
                printf("\n");
                for(j=mn0;j<=n;j++) printf("  %7.3lf",(double)a[i][j]);
        }//end for
		///////////////////////////////////////

        return;
}

template<class TYPE>
void ftypeMatrix(FILE *out,TYPE **a,long m,long n,char *caption)
{
        register long i,j;

        fprintf(out,"\n\n%s:",caption);
        for(i=1;i<=m;i++)
        {
                fprintf(out,"\n");
                for(j=1;j<=n;j++) fprintf(out,"  %7.3lf",(double)a[i][j]);
        }//end for
		/////////////////////////////////////////////

        return;
}

void factorTab(double **a,long n)
{
        register long i,j;
        long k;
        ///////////////////////////////////////////

        for(k=1;k<=n-1;k++)
        for(j=k+1;j<=n;j++)
        { 
                if(fabs(a[k][k])<SMLL){ printf("\nError! --- %ld",__LINE__); exit(0); }
                
				a[k][j]/= a[k][k];
                for(i=k+1;i<=n;i++) a[i][j]-= a[k][j]*a[i][k];
        }//end for
        ///////////////////////////////////////////
	
        return;
}

void solveFactorTab(double **factorTab,double *b,long n)
{
        register long i,j;
        register long k;

        double sum;
        double *_tab_sv,**tab_sv;
        /////////////////////////////////////////////

        //1: saving factor table

        makeMatrix(_tab_sv,tab_sv,n+1,n+1);
        copyMatrix(tab_sv,factorTab,n,n);
        /////////////////////////////////////////////

        //2: partial expunction

        for(k=1;k<=n-1;k++)
        {
                b[k]/= factorTab[k][k];
                for(i=k+1;i<=n;i++)
                        b[i]-= factorTab[i][k]*b[k];
        }//end for
        /////////////////////////////////////////////

        //3: back substitution

        b[n]/= factorTab[n][n];
        for(i=n-1;i>=1;i--)
        {
                sum=0.0;//init.
                for(j=n;j>=i+1;j--)
                        sum+= factorTab[i][j]*b[j];

                b[i]-= sum;
        }//end for
        /////////////////////////////////////////////

        //4: restoring factor table

        copyMatrix(factorTab,tab_sv,n,n);
        freeMatrix(_tab_sv,tab_sv);

        return;
}

void gaussPP(double *x,double **a,double *b,long n)
{
//Notes: by Wang GT
// x[]	 --- returned solution: 1,2,3,...n
// a[][] --- coeff.matrix, not changed
// b[]	 --- right vector, not changed
/////////////////////////////////////////////////////
 
	register long i,j,k;
	long imax;
	
	double *_a_sv,**a_sv,*b_sv;
	double sum,max,aux2;
	////////////////////////////////////////////////

	//1: init. locals and memory allocation
	
	makeMatrix(_a_sv,a_sv,n+1,n+1);
	
	b_sv=(double *)calloc(n+1,sizeof(double));
	if(b_sv==NULL){ printf("\n\nError! --- %ld",__LINE__); exit(0); }

	copyMatrix(a_sv,a,n,n);
	copyVector(b_sv,b,n);
	////////////////////////////////////////////////

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

		if(max<SMLL){ printf("\n\nError! --- %ld",__LINE__); exit(0); }
		//////////////////////////////////

		//2.2 Rows Swapping

		if(imax!=k)
		{
			swapMatrixRows(a,imax,k,n);
			swapVectorElms(b,imax,k);
		}//end if
		//////////////////////////////////

		//2.3 Genuine Expunction

		for(i=k+1;i<=n;i++)
		{
			aux2=a[i][k]/a[k][k];
		
			for(j=k+1;j<=n;j++)
				a[i][j]-= aux2*a[k][j];

			b[i]-= aux2*b[k];
		}//end for
	}//end for
	////////////////////////////////////////////////

	//3: back substitution

        b[n]/= a[n][n];
        for(i=n-1;i>=1;i--)//No Change!
        {
                sum=0.0;
                for(j=i+1;j<=n;j++)
                        sum+= a[i][j]*b[j];

                b[i]=(b[i]-sum)/a[i][i];
        }//end for

	copyVector(x,b,n);//x[] obtained
	////////////////////////////////////////////////

	//4: restore and free memory
	
	copyMatrix(a,a_sv,n,n);
	copyVector(b,b_sv,n);
		
	freeMatrix(_a_sv,a_sv);
	free(b_sv);
	////////////////////////////////////////////////

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
        //////////////////////////////

        return;
}

void drawline(long n)
{
        register long i;

        printf("\n");
        for(i=1;i<=n;i++) printf("-");

        return;
}

void fdrawline(FILE *fp,long n)
{
        register long i;

        fprintf(fp,"\n");
        for(i=1;i<=n;i++)  fprintf(fp,"-");

        return;
}

long sign(double v)
{
	if(v>SMLL)  return  1;
	if(v<-SMLL) return -1;

	return 0; 
}

long sign(char plunus)
{
	if(plunus=='+') return  1;
	if(plunus=='-') return -1;

	return 0;
}

void DBC_case_num(char *string,long num)
{
//Notes: by Wang GT
// string --- returned, DBC number
// num    --- given number
////////////////////////////////////////////////

        //1: validity checking
 
        if(num<0 || num>12)
        {
			printf("\n\nError! --- %ld",__LINE__);
			printf("\n  Invalid num = %ld",num);
			exit(0); 
		}//end if
        ///////////////////////////////////////////////

        //2: DBC number

        if(num==0) strcpy(string,"£°");
        if(num==1) strcpy(string,"£±");
        if(num==2) strcpy(string,"£²");
        if(num==3) strcpy(string,"£³");
        if(num==4) strcpy(string,"£´");
        if(num==5) strcpy(string,"£µ");
        if(num==6) strcpy(string,"£¶");
        if(num==7) strcpy(string,"£·");
        if(num==8) strcpy(string,"£¸");
        if(num==9) strcpy(string,"£¹");
        if(num==10) strcpy(string,"£±£°");
        if(num==11) strcpy(string,"£±£±");
        if(num==12) strcpy(string,"£±£²");
        ///////////////////////////////////////////////

        return;
}
 
void lowerCase(char *string)
{
	register long i;

	long n=strlen(string);//init.
	char ch;
	//////////////////////////////////////
	
	for(i=0;i<n;i++)
	{
		ch=string[i];
		if(ch>='A' && ch<='Z')
		{
			ch-= 'A'-'a';
			string[i]=ch;
		}
	}//end for
	//////////////////////////////////////
	
	return;
}

void upperCase(char *string)
{
	register long i;

	long n=strlen(string);//init.
	char ch;
	//////////////////////////////////////

	for(i=0;i<n;i++)
	{
		ch=string[i];
		if(ch>='a' && ch<='z')
		{
			ch+= 'A'-'a';
			string[i]=ch;
		}
	}//end for
	//////////////////////////////////////
	
	return;
}

void stringEnd(char *string,char ch)
{
	long k=lookup(string,ch);
	if(k!=NO) string[k]='\0';

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

void initString(char *string)
{
	string[0]='\0';
	return;
}

void initString(char *string,long n)
{
    register long i;
    for(i=0;i<n;i++) string[i]='\0';

    return;
}	
	
template<class TYPE>
void turnOver(TYPE *&chain)
{
	TYPE *front=NULL;//init.
	TYPE *next;
	
	while(chain!=NULL)
	{
		next=chain->next; //next saved
		chain->next=front;

		front=chain;//moving forward
		chain=next;
	}//end while

	chain=front;
	/////////////////////////////////////

	return;
}

long lookup(char *string,char ch,long rptNum)
{
	register long i;
	char *cp=string;//init.

	for(i=1;i<=rptNum;i++,cp++)
	{
		cp=strchr(cp,ch);
		if(cp==NULL) return NO;
	}//end for
	//////////////////////////////////////

	long ret=cp-string-1;
	return ret;
}

long lookup(char *string,char ch)
{
	char *cp=strchr(string,ch);
	if(cp==NULL) return NO;

	long ret=cp-string;
	return ret;
}

template<class TYPE>
long lookup(TYPE *v,TYPE k,long n)
{ 
	register long i;
	
	long ret=NO;//init. --- No Change!
	double f1,f2=fabs(k);
	/////////////////////////////////////

	for(i=1;i<=n;i++)
	{
		f1=fabs(v[i]);
		if(fabs(f1-f2)<SMLL)
		{
			ret=i;
			break;
		}
	}//end for
	/////////////////////////////////////

	return ret;
}

template<class TYPE>
void swap(TYPE &v1,TYPE &v2)
{
	TYPE v=v1;
	
	v1 = v2;
	v2 = v;
	///////////////////////////
	
	return;
}

void wpause()
{	
	printf("\nPress ENTER to continue...");
	fflush(stdin); 

	char ch;	
	scanf("%c",&ch);
	///////////////////////////////////

	return;
}

long scale(double x)
{
	if(fabs(x)<SMLL) return -BIGNUM;//No Change!
	
	double logx=log10(fabs(x));
	long ret=sign(logx)*round(fabs(logx));

	return ret;
}

long scale(double *x,long scalmin,long scalmax,long n)
{
	//1: init. locals

	double Xmax=pow(10.0,scalmax);
	double Xmin=pow(10.0,scalmin);

	double max=fmax(x,n);
	double min=fmin(x,n);

	long ret=0;//init.
	///////////////////////////////////////////////

	//2: dataspan check

	if(min<Xmin && max>Xmax)
	{
		printf("\n\nWarning! --- %ld",__LINE__);
		printf("\n  Too large data-span: min=%lf, max=%lf",min,max);

		return ret;
	}//end if
	///////////////////////////////////////////////

	//3: downward scaling

	while(max>Xmax)
	{
		divVector(x,n,10.0);//No Change!
		ret++;

		max/= 10.0;
	}//end while
	///////////////////////////////////////////////

	//4: upward scaling

	while(min<Xmin)
	{
		mulVector(x,n,10.0);//No Change!
		ret--;

		min*= 10.0;
	}//end while
	///////////////////////////////////////////////
		
	return ret;
}

//end of file
