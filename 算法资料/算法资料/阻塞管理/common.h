#ifndef __COMMON_H__
#define __COMMON_H__

#include<malloc.h>
#include<math.h>

#include<stdlib.h>
#include<stdio.h>
#include<string.h>

#include<time.h>
/////////////////////////////////////////////

#define LT  -1   //NO Change!
#define EQ   0
#define GT  +1
#define MT  GT

#define FE73 " % 7.3le"
#define FE84 " % 8.4le"
#define FT62 " % 6.2lf"
#define FT64 " % 6.4lf"
#define FT73 " % 7.3lf"
#define FT74 " % 7.4lf"
#define FT82 " % 8.2lf"
#define FT83 " % 8.3lf"
#define FT84 " % 8.4lf"

#define min(a,b) (((a)<(b))?(a):(b))
#define max(a,b) (((a)>(b))?(a):(b))

#define ULRG 1.0E100
#define VLRG 1.0E+50
#define LARG 1.0E+10
#define BIGNUM 99999

#define EPS  1.0E-4
#define SMLL 1.0E-9

#define DESCRLN	40
#define IDLEN	20
#define IDLN 	IDLEN
#define LINELN  200

#define PAI 3.1415926
#define PI  PAI

#define YES 	+1
#define NO  	-1

#define POSI 	+1
#define ZERO 	 0
#define NEGA 	-1

#define UPPER	POSI
#define MIDDL	ZERO
#define LOWER	NEGA
/////////////////////////////////////

char buf[100];
char buffer[2000];

char idbuf[IDLN];
char idbuf2[IDLN];
/////////////////////////////////////////

struct ldstr//load
{
	char	id[IDLN];
	long	i;	//ibs
	double	w;	//
	double	r;
};

struct lnstr//lines of group
{
	char	id[IDLN];
	long	i;	//ibs
	long	j;	//zbs

	char	mst[IDLN];
	long	imst;

	double	w;	//active power
	double	r;	//reactive

	double	wt;	//area weight
	double	pf;	//power factor

	long	slg;//sluggish
};

struct mdstr//mode of group
{
	char	id[IDLN];
	double	lmt;

	char	id1[IDLN];
	long	mel1;	//MT, EQ, or LT
	double	val1;	//limit of mode

	char	id2[IDLN];
	long	mel2;
	double	val2;

	char	id3[IDLN];
	long	mel3;
	double	val3;
};

struct pntstr//point structure
{
        double p;       //x_power
        double c;       //y_cost
};

struct unstr//unit structure
{
        char    id[IDLN];
        long    i;      //ibs
        double  w;      //unit sched.

        double	inc;
	double  wmx;
};
///////////////////////////////////////////////////////////////

double fabsmax(double *list,long n);
double fabsmax(double *list,long n1,long n2);
double fabsmin(double **a,long i,long n);

double frand(double num);
double frand(double lower,double upper);

template<class TYPE>
double interLin(TYPE *v,long n,long m,long k);

template<class TYPE>
double statMSD(TYPE *v,long n);
double vsimp(double (*fun)(double),double a,double b,double eps,double h0);
////////////////////////////////////////////////////////////////

template<class TYPE>
long chainLen(TYPE *chain);
long conditionMeet(double v,double value,long mel);

long diffMatrix(double **m1,double **m2,long n,char caption[]);
long diffMatrix(double **m1,double **m2,long m,long n,char caption[]);

template<class TYPE>
long drift(TYPE v,TYPE L,TYPE U);

template<class TYPE>
long findk_ex(TYPE *x,long n,TYPE xk);

template<class TYPE>
long findPivot(TYPE *vector,long i,long j);

long getNoneZeroNum(double **matrix,long m,long n);
long isAlphaNum(char ch);

template<class TYPE>
long isUnitMatrix(TYPE **a,long n);

template<class TYPE>
long isZeroMatrix(TYPE **a,long m,long n);

long lookup(char *string,char ch,long n);
long lookup(long *list,long id,long n);
long lookup2(long *list,long id,long n);
long melFun(char melstr[]);

template<class TYPE>
long oppSign(TYPE num1,TYPE num2);

template<class TYPE>
long partition(TYPE *vector,long i,long j,TYPE pivot);

template<class TYPE>
long posiAbsmax(TYPE *vector,long n);

template<class TYPE>
long posiAbsmax(TYPE *vector,long n1,long n2);

long posiAbsmax2(double *vector,long n);
long posiAbsmax2(double *vector,long n1,long n2);

template<class TYPE>
long posiChainNode(TYPE *chain,TYPE *node);

template<class TYPE>
long posiDiff(TYPE *v1,TYPE *v2,long n1,long n2);

template<class TYPE>
long posiMax(TYPE *vector,long n);

template<class TYPE>
long posiMax(TYPE *vector,long n1,long n2);

template<class TYPE>
long posiMin(TYPE *vector,long n);

template<class TYPE>
long posiMin(TYPE *vector,long n1,long n2);

template<class TYPE>
long rank(TYPE **matrix,long m,long n);

long scale(double number);

template<class TYPE>
long sign(TYPE number);
///////////////////////////////////////////////////////////////////

void addLeadingChar(char *string,char ch,long len);

template<class TYPE>
void addMultiRow(TYPE **a,long k,long i,TYPE multiple,long n);

template<class TYPE>
void addVector(TYPE *v,TYPE *v_add,long n);

void chopString(char *part1,char *part2,char *string,char ch);

template<class TYPE>
void combinChainNodes(TYPE *chain);

void complexDivi(double re1,double im1,double re2,double im2,double &re,double &im);
void complexMult(double re1,double im1,double re2,double im2,double &re,double &im);

void consoleMsg(char *msg,char *msg_exp);
void consoleMsg(char *msg,char *msg_exp,char *file,long line);

template<class TYPE>
void copyMatrix(TYPE **m_destination,TYPE **m_source,long m,long n);

template<class TYPE>
void copyVector(TYPE *v_destination,TYPE *v_source,long n);

template<class TYPE>
void copyVector(TYPE *v_destination,TYPE *v_source,long n1,long n2);

template<class TYPE>
void copyVector(TYPE *vd,TYPE *vs,long nd1,long nd2,long ns1);

template<class TYPE>
void copyVectorRow(TYPE *vector,TYPE **matrix,long i,long n);

template<class TYPE>
void delChainNode(TYPE *&chain,TYPE *node);
void delLeadingZero(char *date);

template<class TYPE>
void delMatrixRow(TYPE **a,long ik,long m,long n);

template<class TYPE>
void delMatrixCol(TYPE **a,long jk,long m,long n);

template<class TYPE>
void delVectorElm(TYPE *v,long k,long n);

template<class TYPE>
void divMatrix(TYPE **a,long m,long n,TYPE div);

template<class TYPE>
void divMatrixRow(TYPE **a,long i,long n,TYPE div);

template<class TYPE>
void divMatrixRow(TYPE **a,TYPE *v,long i,long n,TYPE div);

template<class TYPE>
void divVector(TYPE *v,long n,TYPE div);

template<class TYPE>
void divVector(TYPE *v,long n1,long n2,TYPE div);

void drawline(long n);
void drawline(long x1,long x2);

void factorTable(double **a,long n);
void fdrawline(FILE *out,long n);
void fdrawline(FILE *out,long x1,long x2);

template<class TYPE>
void freeChain(TYPE *&chain);

template<class TYPE>
void freeMatrix(TYPE* &_matrix,TYPE** &matrix);

template<class TYPE>
void ftypeVector(FILE *out,TYPE *V,long n,char caption[]);

template<class TYPE>
void ftypeMatrix(FILE *out,TYPE **M,long row,long col,char caption[]);

void gaussPP(double *x,double **a,double *b,long n);
void initString(char *string,long n);

template<class TYPE>
void initMatrix(TYPE **a,long m,long n,TYPE v);

template<class TYPE>
void initMatrixRow(TYPE **a,long ik,long n,TYPE v);

template<class TYPE>
void initVector(TYPE *a,long n,TYPE v);

template<class TYPE>
void initVector(TYPE *a,long n1,long n2,TYPE v);

template<class TYPE>
void insertVectorElm(TYPE *v,TYPE vElm,long k,long n);
void invMatrixGJ(double **a,long n);

template<class TYPE>
void makeMatrix(TYPE* &_matrix,TYPE** &matrix,long row,long col);

void mdyID(char *month,char *day,char *year,char *string);

template<class TYPE>
void mirrorSym(TYPE *vector,long n1,long n2);

template<class TYPE>
void multiMatrix(TYPE **M3,TYPE **M1,TYPE **M2,long m,long k,long n);

void natrualVector(long *v,long n);

template<class TYPE>
void negaMatrix(TYPE **a,long m,long n);

void packString(char *string);

void printSparMatrix(FILE *out,double **matrix,long n,char caption[]);
void printSparMatrix(FILE *out,double **matrix,long m,long n,char caption[]);

void printVector(FILE *out,long *vector,long n,char caption[]);
void printVector(FILE *out,long *vector,long n1,long n2,char caption[]);
void printVector(FILE *out,double *vector,long n,char caption[]);
void printVector(FILE *out,double *vector,long n1,long n2,char caption[]);

template<class TYPE>
void quickSort(TYPE *vector,long n);

template<class TYPE>
void quickSort(TYPE *vector,long i,long j);

void readMatrix(char filename[],char caption[],double **matrix,long n);
void readMatrix(char filename[],char caption[],double **matrix,long m,long n);
void readSparMatrix(char filename[],char caption[],double **matrix,long n);
void readSparMatrix(char filename[],char caption[],double **matrix,long m,long n);

void readVector(char filename[],char caption[],long *vector,long n);
void readVector(char filename[],char caption[],long *vector,long n1,long n2);
void readVector(char filename[],char caption[],double *vector,long n);
void readVector(char filename[],char caption[],double *vector,long n1,long n2);

template<class TYPE>
void showChain(TYPE *chain,char caption[]);

template<class TYPE>
void showChain2(TYPE *chain,char caption[]);

template<class TYPE>
void showZeroRows(TYPE **a,long m,long n,char caption[]);

template<class TYPE>
void shuffVector(TYPE *vector,long n);

void solveFactTab(double **factTab,double *b,long n);

template<class TYPE>
void sortVector(TYPE *list,long n);

void splin(double *ys,double *ys1,double *x,double *y,double *xs,long n,long ns);
void splinfun(double *ys2,double *x,double *y,double ld,double dd,double un,double dn,long n);

void strcat2(char *strDestination,char *strSource,long size);

void stringEnd(char *string,long n);
void stringEnd(char *string,char ch);

template<class TYPE>
void subMatrix(TYPE **sub,TYPE **a,long *icol,long m,long n);

template<class TYPE>
void swap(TYPE &v1,TYPE &v2);

template<class TYPE>
void swapMatrixEle(TYPE **a,long i,long j,long i2,long j2);

template<class TYPE>
void swapChainNodes(TYPE *&node1,TYPE *&node2,TYPE *&chain);

template<class TYPE>
void swapElems(TYPE *v,long i,long j);

template<class TYPE>
void swapRows(TYPE **a,long i,long k,long n);

template<class TYPE>
void transMatrix(TYPE **matrix,long n);

template<class TYPE>
void trunChain(TYPE *&chain,long len);

void tssfun(double *x,double *a,double *b,double *c,double *d,double *q,double *p,long n);

template<class TYPE>
void typeMatrix(TYPE **M,long row,long col,char caption[]);

template<class TYPE>
void typeVector(TYPE *V,long n,char caption[]);

template<class TYPE>
void typeVector(TYPE *V,long n1,long n2,char caption[]);

template<class TYPE>
void unitMatrix(TYPE **a,long n);
void upperCase(char *string);

void wait(double sec);
void wpause();

template<class TYPE>
void turnOver(TYPE *&chain);
///////////////////////////////////////////////

template<class TYPE>
TYPE fmax(TYPE *list,long n);

template<class TYPE>
TYPE fmax(TYPE *list,long n1,long n2);

template<class TYPE>
TYPE fmin(TYPE *list,long n);

template<class TYPE>
TYPE fmin(TYPE **matrix,long i,long n);

template<class TYPE>
TYPE maxError(TYPE **m1,TYPE **m2,long m,long n);

template<class TYPE>
TYPE maxError(TYPE *v1,TYPE *v2,long n);

template<class TYPE>
TYPE sum(TYPE *vector,long n);

template<class TYPE>
TYPE sum(TYPE *vector,long n1,long n2);

template<class TYPE>
TYPE* findNode(long i,TYPE *chain);

template<class TYPE>
TYPE* frontNode(TYPE *node,TYPE *chain);

template<class TYPE>
TYPE* chainNode(TYPE *chain,long index);
///////////////////////////////////////////////////

#endif
