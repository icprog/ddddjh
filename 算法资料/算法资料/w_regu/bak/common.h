//common.h by Wang GT
//////////////////////////////////////////////

#ifndef __COMMON_H__
#define __COMMON_H__

#include<malloc.h>
#include<math.h>

#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<sys/stat.h>

#include<direct.h>
#include<time.h>
//////////////////////////////////////////////

//project and files

char project[256];

char basfile[256];
char blpfile[256];
char crvfile[256];
char datfile[256];
char inifile[256];
char logfile[256];
char matfile[256];
char mmpfile[256];
char prmfile[256];
char ptnfile[256];
char pwrfile[256];
char resfile[256];
char snsfile[256];
char vfyfile[256];
//////////////////////////////////////////////

//constant numbers

#define MAX_PLOTPNT  100
#define DENS_COEFF   100

#define LARG    1.0E+10
#define EPS     1.0E-4
#define SMLL    1.0E-9

#define BIGNUM  99999
#define PI      3.1415927

#define DESCRLN  100
#define IDLN      50
#define LINELN   500
//////////////////////////////////////////////

//comparison and relation

#define LT    -1  //No Change!
#define EQ     0
#define GT    +1
#define MT    GT

#define UPPER   1	
#define MIDDL   0
#define LOWER  -1

#define YES    +1
#define NO     -1

#define ODD     1
#define EVEN    0
//////////////////////////////////////////////

//macros of max and min

#ifndef max
#define max(a,b) (((a)>(b))?(a):(b))
#endif

#ifndef min
#define min(a,b) (((a)<(b))?(a):(b))
#endif
//////////////////////////////////////////////

//date and time

char strtime[20];//start time
char endtime[20];

char crrdate[20];
char crrtime[20];

char date_str[20];
char time_str[20];

clock_t run_start;
clock_t itr_start;

double runtime;
double itrtime;
///////////////////////////////////////

//seps and token

char *seps="/, ;\t\n";//SBC only
char *token;

char *seps_date="/-";
char *seps_time=":";
char *seps_val ="=";
//////////////////////////////////////

//path and other buffers

char crrpath[256];
char homepath[256];
char path[256];

char buffer[2000];
char buf[1000];
char buf2[1000];

char idbuf[IDLN];
char idbuf2[IDLN];
////////////////////////////////////////////////

//self-defined struct types

typedef struct complex
{
	double re;
	double im;
}COMPLEX;

typedef struct flinestr
{
	struct flinestr *next;
	char line[LINELN];
}FLINE;

typedef struct pointstr
{
        double x;
        double y;
}PNTSTR;
////////////////////////////////////////////////

typedef struct markstr
{
	char descr[DESCRLN];
	struct pointstr point;
}MARK;

typedef struct plotstr
{
	char descr[DESCRLN];
	char id[IDLN];

	long pntNum;
	struct pointstr point[MAX_PLOTPNT];
}PLOT;
///////////////////////////////////////////////////////////////////

//function prototypes

COMPLEX operator- (COMPLEX x,COMPLEX y);
COMPLEX operator* (COMPLEX x,COMPLEX y);
COMPLEX operator/ (COMPLEX x,COMPLEX y);
COMPLEX operator+ (COMPLEX x,COMPLEX y);

struct flinestr *find_flineData(char *leadingchar,struct flinestr *flineData);
struct pointstr crossPoint(struct plotstr plot1,struct plotstr plot2);

double cutPoint(double a,double b,double r);
double fabsmax(double v1,double v2);
double fabsmax(double *v,long n);
double fabsmax(double *v,long n1,long n2);

double frand(double nmin,double nmax);
double frand(double num);

double getparm(char *prmfile,char *parm);
double linearin(struct pointstr *point,long pointNum,double xi);

double LSM_forecast(double *y,double yk,double xk,double xf,long rank,long n,long cctFlg,long prnt);
double LSM_sfun(double *x,long k,long n);
double LSM_vfun(double *x,double *y,long k,long n);

double polynomial(double *a,double x,long n);

double SIForm_a(double x1,double x2,double y1,double y2);
double SIForm_b(double x1,double x2,double y1,double y2);

template<class TYPE>
double stat_mean(TYPE *v,long n);

template<class TYPE>
double stat_MSE(TYPE *v,long n);

template<class TYPE>
double stat_MSE(TYPE *v,TYPE *vb,long n);

template<class TYPE>
double stat_stdev(TYPE *v,long n);

template<class TYPE>
double stat_sum(TYPE *v,long n);

double timefun(clock_t start);
double vsimp(double (*fun)(double),double a,double b,double eps,double h0);

template<class TYPE>
long bitfun(TYPE v,long bit);

template<class TYPE>
long boundary_on(TYPE v,TYPE L,TYPE U);

template<class TYPE>
long boundary_to(TYPE v,TYPE L,TYPE U);

template<class TYPE>
long chainLen(TYPE *chain);

long chars_inString(char ch,char *string);
long conditionMeet(double v,double vb,long mel);

long date_day(char *date,char *format);
long date_month(char *date,char *format);
long date_year(char *date,char *format);
long datediff_day(char *date,char *date0,char *format);

long fileExist(char *filename);

template<class TYPE>
long findk_ex(TYPE *x,long n,TYPE xk);

long fline_insert(char *datfile,char *leadingchar,char *newline);
long fline_insert(char *datfile,char *leadingchar,long k,char *newline);

long fline_read(char *datfile,char *linebuf,char *leadingchar);

long fline_update(char *datfile,char *leadingchar,char *newline);
long fline_update(char *datfile,char *leadingchar,long k,char *newline);

long fline_write(char *datfile,struct flinestr *flineData);

long inArgv(char *str,char **argv,long argc);
long inArgv(char *str1,char *str2,char **argv,long argc);

template<class TYPE>
long intrvl(TYPE v,TYPE a,TYPE b);

long isBlank(char ch);
long isLarge(double v);
long isLeapYear(long year);
long isPrimeNum(long num);

template<class TYPE>
long isUnitCol(TYPE **a,long jk,long m);

template<class TYPE>
long isUnitMatrix(TYPE **a,long n);

template<class TYPE>
long isUnitVector(TYPE *v,long n);

template<class TYPE>
long isZeroMatrix(TYPE **a,long m,long n);

long lookup(char *string,char ch);
long lookup(char *string,char ch,long rptNum);

template<class TYPE>
long lookup(TYPE *v,TYPE k,long n);

long melfun(char *mel_str);
long monthFun(char *month);
long mthdaysFun(long month,long year);

long oddeven(long num);

template<class TYPE>
long oppSign(TYPE num1,TYPE num2);

template<class TYPE>
long partition(TYPE *v,long i,long j,TYPE pivot);

template<class TYPE>
long pivotFun(TYPE *v,long i,long j);

template<class TYPE>
long posAbsmax(TYPE *v,long n);

template<class TYPE>
long posAbsmax(TYPE *v,long n1,long n2);

template<class TYPE>
long posChainNode(TYPE *chain,TYPE *node);

long posDataBlock(FILE *fp,char *caption);

template<class TYPE>
long posmax(TYPE *v,long n);

template<class TYPE>
long posmax(TYPE *v,long n1,long n2);

template<class TYPE>
long posmin(TYPE *v,long n);

template<class TYPE>
long posmin(TYPE *v,long n1,long n2);

template<class TYPE>
long posNonzero(TYPE *v,long n);

template<class TYPE>
long posRecord(TYPE *tb,char *id,long lv_tb);

template<class TYPE>
long posRecord(TYPE *tb,long ibs,long lv_tb);

template<class TYPE>
long posRecord(TYPE *tb,long ibs,long zbs,long lv_tb);

long round(double v);

long scale(double x);
long scale(double *x,long scalmin,long scalmax,long n);

long sdfun(char *hrmin,long sd_num);
long sign(char plunus);
long sign(double v);

long yeardiff_day(long year,long year0);
long yes_no(char *string);

long yrdaysFun(char *date,char *format);
long yrdaysFun(long year);
////////////////////////////////////////////////////////////

template<class TYPE>
TYPE fmax(TYPE *v,long n);

template<class TYPE>
TYPE fmax(TYPE *v,long n1,long n2);

template<class TYPE>
TYPE fmin(TYPE *v,long n);

template<class TYPE>
TYPE stat_maxErr(TYPE *v,TYPE *vb,long n);

template<class TYPE>
TYPE stat_maxErrpct(TYPE *v,TYPE *vb,long n);

template<class TYPE>
TYPE sum(TYPE **a,long i1,long j1,long m,long n);

template<class TYPE>
TYPE sum(TYPE *v,long n);

template<class TYPE>
TYPE sum(TYPE *v,long n1,long n2);

template<class TYPE>
TYPE sum(TYPE *v1,TYPE *v2,long n);

template<class TYPE>
TYPE sumRowVector(TYPE **a,TYPE *v,long ik,long n);

template<class TYPE>
TYPE* chainNode(TYPE *chain,long index);

template<class TYPE>
TYPE* findData(char *id,TYPE* dataChain);

template<class TYPE>
TYPE* findData(long ibs,long zbs,TYPE *dataChain);

template<class TYPE>
TYPE* findData(long ibs,TYPE* dataChain);

template<class TYPE>
TYPE* frontNode(TYPE *chain,TYPE *node);

template<class TYPE>
TYPE* nextNode(TYPE *node,long rptNum);

template<class TYPE>
TYPE* tailNode(TYPE *chain);
///////////////////////////////////////////////////////////

template<class TYPE>
void absMatrix(TYPE **a,long m,long n);

template<class TYPE>
void absVector(TYPE *v,long n);

template<class TYPE>
void add_mulrow(TYPE **a,long ik,long i_add,TYPE mul,long n);

template<class TYPE>
void add_mulrow(TYPE **a,long ik,long i_add,TYPE mul,long n1,long n2);

template<class TYPE>
void addChainNode(TYPE *&chain,TYPE *node);

template<class TYPE>
void addMatrixRow(TYPE **a,TYPE *v,long ik,long n);

template<class TYPE>
void addVector(TYPE *v,TYPE *v_add,long n);

template<class TYPE>
void addVector(TYPE *v,TYPE *v_add,long n1,long n2);

template<class TYPE>
void appendNode(TYPE *&chain,TYPE *node);

void ASCII_string(char *ascii,char *string);

void catArgv(char *arg,char **argv,long argc);
void CF_LSMethod(double *a,double *x,double *y,long m,long n,long prnt);
void CF_splin(double *ys,double *ys1,double *x,double *y,double *xs,long n,long ns);

template<class TYPE>
void combinChainNodes(TYPE *chain);

template<class TYPE>
void copyChain(TYPE *&chain_dest,TYPE *chain_src);

template<class TYPE>
void copyMatrix(TYPE **m_dest,TYPE **m_src,long i1,long j1,long m,long n);

template<class TYPE>
void copyMatrix(TYPE **m_dest,TYPE **m_src,long m,long n);

template<class TYPE>
void copyMatrixCol(TYPE **a,TYPE *v,long j,long m);

template<class TYPE>
void copyMatrixCol(TYPE **a_dest,TYPE **a_src,long j_dest,long j_src,long m);

template<class TYPE>
void copyMatrixRow(TYPE **a,TYPE *v,long ik,long n);

template<class TYPE>
void copyMatrixRow(TYPE **a,TYPE *v,long ik,long n1,long n2);

template<class TYPE>
void copyMatrixRow(TYPE **a_dest,long ik_dest,TYPE **a_src,long ik_src,long n);

template<class TYPE>
void copyVector(TYPE *v_dest,TYPE *v_src,long n);

template<class TYPE>
void copyVector(TYPE *v_dest,TYPE *v_src,long n1,long n2);

template<class TYPE>
void copyVector(TYPE *v_dest,TYPE *v_src,long nd1,long ns1,long ns2);

template<class TYPE>
void copyVectorCol(TYPE *v,TYPE **a,long jk,long m);

template<class TYPE>
void copyVectorRow(TYPE *v,TYPE **a,long ik,long n);

template<class TYPE>
void copyVectorRow(TYPE *v,TYPE **a,long ik,long n1,long n2);

void date(char *date_str);
void date(char *date_str,char *format);
void date_time(char *string);
void DBC_case_num(char *string,long num);

template<class TYPE>
void delChainNode(TYPE *&chain,TYPE *node);

void delLeadingChar(char *string,char ch);
void delLeadingChar(char *string,char ch,long rptNum);

template<class TYPE>
void delMatrixCol(TYPE **a,long jk,long m,long n);

template<class TYPE>
void delMatrixRow(TYPE **a,long ik,long m,long n);

template<class TYPE>
void delVectorElm(TYPE *v,long k,long n);

void deNull(char *string);

template<class TYPE>
void divMatrix(TYPE **a,long m,long n,TYPE div);

template<class TYPE>
void divMatrixRow(TYPE **a,long ik,long n,TYPE div);

template<class TYPE>
void divMatrixRow(TYPE **a,long ik,long n1,long n2,TYPE div);

template<class TYPE>
void divVector(TYPE *v,long n,TYPE div);

template<class TYPE>
void divVector(TYPE *v,long n1,long n2,TYPE div);

void drawline(long n);

void expSmooth(double *s,double *x,double wt,long n);
void expSmooth_forecast(double *F,double *x,double smooth_coeff,long forNum,long n);
void expSmoothCF_a(double *a,double *s1,double *s2,long n);
void expSmoothCF_b(double *b,double *s1,double *s2,double wt,long n);

void factorTab(double **a,long n);
void fdrawline(FILE *fp,long n);

void fileName(char *filename,char *pathfile);

template<class TYPE>
void freeChain(TYPE *&chain);

template<class TYPE>
void freeMatrix(TYPE* &_matrix,TYPE** &matrix);

template<class TYPE>
void ftypeMatrix(FILE *out,TYPE **a,long m,long n,char *caption);

template<class TYPE>
void ftypeVector(FILE *out,TYPE *v,long n,char *caption);

template<class TYPE>
void ftypeVector(FILE *out,TYPE *v,long n1,long n2,char *caption);

void gaussPP(double *x,double **a,double *b,long n);
void getCrrpath(char *crrpath);
void getHomepath(char *homepath,char *inifile);
void getparm(char *value,char *prmfile,char *parm);
void getPathinfo(char *pathinfo,char *path);

template<class TYPE>
void initMatrix(TYPE **a,long i1,long j1,long m,long n,TYPE v);

template<class TYPE>
void initMatrix(TYPE **a,long m,long n,TYPE v);

template<class TYPE>
void initMatrixCol(TYPE **a,long jk,long m,TYPE v);

template<class TYPE>
void initMatrixRow(TYPE **a,long ik,long n,TYPE v);

void initString(char *string);
void initString(char *string,long n);

template<class TYPE>
void initVector(TYPE *a,long n,TYPE v);

template<class TYPE>
void initVector(TYPE *a,long n1,long n2,TYPE v);

template<class TYPE>
void insertVectorElm(TYPE *v,TYPE vElm,long k,long n);

void invMatrixGJ(double **a,long n);

void leadingzero(char *date,char *format);
void lowerCase(char *string);
void LTrim(char *string);

template<class TYPE>
void makeMatrix(TYPE* &_a,TYPE** &a,long m,long n);

template<class TYPE>
void mirrorSym(TYPE *v,long n);

template<class TYPE>
void mirrorSym(TYPE *v,long n1,long n2);

template<class TYPE>
void mulMatrix(TYPE **a,long i1,long j1,long m,long n,TYPE mul);

template<class TYPE>
void mulMatrix(TYPE **a,long m,long n,TYPE mul);

template<class TYPE>
void mulMatrix(TYPE **a3,TYPE **a1,TYPE **a2,long m,long k,long n);

template<class TYPE>
void mulMatrixCol(TYPE **a,long jk,long m,TYPE mul);

template<class TYPE>
void mulMatrixRow(TYPE **a,long ik,long n,TYPE mul);

template<class TYPE>
void mulMatrixVector(TYPE *ret,TYPE **a,TYPE *v,long m,long n);

template<class TYPE>
void mulVector(TYPE *v,long n,TYPE mul);

template<class TYPE>
void mulVector(TYPE *v,long n1,long n2,TYPE mul);

template<class TYPE>
void naturalVector(TYPE *v,long n);

template<class TYPE>
void naturalVector(TYPE *v,long n1,long n2);

template<class TYPE>
void negMatrix(TYPE **a,long m,long n);

void packString(char *string);
void parentPath(char *parent,char *path);

void printSparMatrix(FILE *out,double **matrix,long m,long n,char *caption);
void printSparMatrix(FILE *out,double **matrix,long n,char *caption);

void printVector(FILE *out,double *vector,long n,char *caption);
void printVector(FILE *out,double *vector,long n1,long n2,char *caption);
void printVector(FILE *out,long *vector,long n,char *caption);
void printVector(FILE *out,long *vector,long n1,long n2,char *caption);

template<class TYPE>
void quickSort(TYPE *v,long i,long j);

template<class TYPE>
void quickSort(TYPE *vector,long n);

void read_flineData(char *datfile,struct flinestr *&flineData);

void readMatrix(char *datfile,char *caption,double **a,long m,long n);
void readMatrix(char *datfile,char *caption,double **a,long n);

void readSparMatrix(char *datfile,char *caption,double **a,long m,long n);
void readSparMatrix(char *datfile,char *caption,double **a,long n);

template<class TYPE>
void readVector(char *datfile,char *caption,TYPE *v,long n);

template<class TYPE>
void readVector(char *datfile,char *caption,TYPE *v,long n1,long n2);

void replace_subString(char *string,char *substr,char *substr2);
void RTrim(char *string);

void sdfun(char *hrmin,long sd,long sd_num);
void setupFile(char *inifile);

template<class TYPE>
void shuffVector(TYPE *v,long n);

void smooth_corr(double *v,double corr,double coeff,long k,long n);
void solveFactorTab(double **factorTab,double *b,long n);

template<class TYPE>
void sortVector(TYPE *v,long n);

template<class TYPE>
void sortVector(TYPE *v,long n1,long n2);

void splinfun(double *ys2,double *x,double *y,double ld,double dd,double un,double dn,long n);

void stringEnd(char *string,char ch);
void stringEnd(char *string,long n);
void stringSeg(char *seg,char *string,long k);

template<class TYPE>
void subMatrix(TYPE **sub,TYPE **a,long *icol,long m,long n);

template<class TYPE>
void subtractVector(TYPE *v,TYPE *v_sub,long n);

template<class TYPE>
void subVector(TYPE *sub,TYPE *v,long *index,long n);

template<class TYPE>
void swap(TYPE &v1,TYPE &v2);

template<class TYPE>
void swapMatrixElms(TYPE **a,long i,long j,long i2,long j2);

template<class TYPE>
void swapMatrixRows(TYPE **a,long ik1,long ik2,long n);

template<class TYPE>
void swapVectorElms(TYPE *v,long i,long j);

template<class TYPE>
void transMatrix(TYPE **a,long n);

template<class TYPE>
void transMove(TYPE **a,long it,long jt,long m1,long n1,long m2,long n2);

template<class TYPE>
void transMove(TYPE *v,long t,long n);

template<class TYPE>
void transMove(TYPE *v,long t,long n1,long n2);

void trimString(char *string);

template<class TYPE>
void truncChain(TYPE *&chain,long n);

void tssfun(double *x,double *a,double *b,double *c,double *d,double *q,double *p,long n);

template<class TYPE>
void turnOver(TYPE *&chain);

template<class TYPE>
void typeMatrix(TYPE **a,long m,long n,char *caption);

template<class TYPE>
void typeMatrix(TYPE **a,long mn0,long m,long n,char *caption);

template<class TYPE>
void typeVector(TYPE *v,long n,char *caption);

template<class TYPE>
void typeVector(TYPE *v,long n1,long n2,char *caption);

template<class TYPE>
void unitMatrix(TYPE **a,long n);

template<class TYPE>
void unitVector(TYPE *v,long k,long n);

void upperCase(char *string);

void wait(double sec);
void wpause();
void wtime(char *time_str);

void yesterday(char *date,char *format);
///////////////////////////////////////////////////

#endif
