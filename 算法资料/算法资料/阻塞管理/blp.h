//blp.h by Wang GT

#ifndef __BLP_H__
#define __BLP_H__

#include"common.h"
/////////////////////////////

#define NOSOLU 0
#define OPTIML 1
#define UNBNDD 2

struct estr{ struct estr *next; double **m; };
//////////////////////////////////////////////////////

double calZ(double *cb,double *xb,double *cn,double *xn,double *cn2,double *xn2,long m,long nN,long nN2);

long blp(double *x,double **a,double *b,double *c,double *Lmin,double *Lmax,long *mel,long m,long n0,long prnt);
long blpfun(double *x,double **a,double *b,double *c,double *Lmin,double *Lmax,long *mel,long m,long n0,long n,long prnt,long debug);

long feasible(double *x,long *ix,double *Lmin,double *Lmax,long m,long n);
long getNumEQ(long *mel,long m);
long getNumLT(long *mel,long m);
long getNumSR(double *b,long *mel,long m);

long indexSets(long *ixn,long *ixn2,double **N,double **N2,double *xn,double *xn2,double *xb,long *ixb,double **B,
		 double **a,double *b,double *Lmin,double *Lmax,long m,long n0,long n,long &nN,long &nN2,long prnt);
long limit(double *x,long *ix,long indx,double *Lmin,double *Lmax);
long veriSolu(double *x,double **a,double *b,double *c,long *mel,double *Lmin,double *Lmax,long m,long n0,long prnt);

void actvFun(long *actvCon,long *actvBnd,long *mel,double *b,double *L,double *U,double **a,double *x,long m,long n0);
void basalEx(long *ixb,long *ixn,long *ixn2,long k,long k_pos,long l,long l_pos,long m,long &nN,long &nN2);
void BLP_readData(char *datafile,double **a,double *b,double *c,double *L,double *U,long *mel,
										long m,long n,long nEQ,long prnt);
void calXB(double *xb,double *b,double **B,double **N,double **N2,double *xn,double *xn2,long m,long nN,long nN2);
void consVal(double *cons,double **a,double *x,long m,long n);

void deltaFunL(double *delta,long *r,double *xb,long *ixb,double *yk,double *Lmin,double *Lmax,long m,long k);
void deltaFunU(double *delta,long *r,double *xb,long *ixb,double *yk,double *Lmin,double *Lmax,long m,long k);
void initXN(double *xn,double *Lm,long *ixn,long nN);

void lower2upper(long *ixn,long *ixn2,long k,long &nN,long &nN2);
void mapping(double *Lm,double **a,double *b,double *c,double *L,double *U,long m,long n0);

void printLP(char file[],double **a,double *b,double *c,double *L,double *U,long *mel,long m,long n0);

void scaling(double **a,double *b,long m,long n0);
void solut(double *x,double z,double *xb,double *xn,double *xn2,long *ixb,long *ixn,long *ixn2,long m,long nN,long nN2);
void standard(double **a,double *b,double *c,double *Lmin,double *Lmax,long *mel,long m,long &n0,long prnt);

void unmapping(double *x,double *Lm,long n0);
void upper2lower(long *ixn,long *ixn2,long k,long &nN,long &nN2);
////////////////////////////////////////////////////////////////////////////////////////

#endif
