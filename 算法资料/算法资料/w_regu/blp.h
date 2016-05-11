//blp.h by Wang GT

#ifndef __BLP_H__
#define __BLP_H__

#include"common.h"
/////////////////////////////

#define PHSONE  1
#define PHSTWO  2

#define NOSOLU  0 
#define OPTIML  1 
#define UNBNDD  2
/////////////////////////////////////////////////////////////////

//function prototypes

long blp_2phase(double *x,double **a,double *b,double *c,double *L,double *U,long m,long n,long phs,long prnt);
long blpfun(double *x,double **a,double *b,double *c,double *L,double *U,long m,long n,long nEQ,long prnt);

long pospivot_ik(double &delta,long &ikFlg,double **spxTBL,double *L,double *U,long *ixb,long *ixn,long *ixn2,long nN,long nN2,long jk,long m,long n,long prnt);
long pospivot_jk(double **spxTBL,long *ixb,long *ixn,long *ixn2,long *arIndex,long nN,long nN2,long arNum,long m,long n);

long pos_artif(long *uvIndex,long uvNum,double **a,long m,long n);
long posArbasic(long *arIndex,long arNum,long *ixb,long m);

long slide_ccn(long *arIndex,long ik,long arNum);
long spxTBL_2phase(double *x,double **a,double *b,double *c,long m,long n,long phs,long prnt);
long spxTBL_itr(double **spxTBL,long *ixb,long *ixn,long *ixn2,long *arIndex,long arNum,long &nN,long &nN2,long m,long n,long prnt);

long vfy_solution(double *x,double **a,double *b,double *c,double *L,double *U,long m,long n,long prnt);
//////////////////////////////////////////////////////////////////////////////////

void aux_obj(double *c2,long n,long *arIndex,long arNum);
void blp_global_alloc(long m,long n);
void blp_global_free();
void blp_readData(char *datfile,long &m,long &n,long &nEQ,long prnt);

void calRHS(double *RHS,double **a,double *b,double *L,double *U,long *ixn,long *ixn2,long nN,long nN2,long m,long n);
void consVal(double *cons,double **a,double *x,long m,long n);

void delArtifcol(double **spxTBL,long *ixn,long *ixn2,long &nN,long &nN2,long *arIndex,long arNum,long m,long n);
void delBasic(long *ixn,long &nN,long *ixb,long m);

void get_solution(double *x,double **spxTBL,double *L,double *U,long *ixb,long *ixn,long *ixn2,long nN,long nN2,long m,long n);

void init_ixb(long *ixb,long *uvIndex,double **a,long m,long n);
void init_spxTBL(double **spxTBL,double **a,double *b,double *c,long *ixb,long m,long n);
void intrArtif(long *uvIndex,long &uvNum,long *arIndex,long &arNum,double **a,long m,long &n,long prnt);

void mapping(double *Lm,double **a,double *b,double *c,double *L,double *U,long m,long n);
void unmapping(double *x,double *L,double *U,double *Lm,long n);

void pivotfun(double **spxTBL,long *ixb,long *ixn,long *ixn2,long &nN,long &nN2,double delta,long ik,long jk,long ikFlg,long m,long n,long prnt);
void printLP(char *blpfile,double **a,double *b,double *c,double *L,double *U,long m,long n);

void read_blpsize(FILE *in,long &m,long &n,long &nEQ);
void read_coeff(FILE *in,double **a,long m,long n);
void read_maxbound(FILE *in,double *U,long n);
void read_minbound(FILE *in,double *L,long n);
void read_objfunction(FILE *in,double *c,long n);
void read_RHS(FILE *in,double *b,long m);

void remove_arbasic(double **spxTBL,long *ixb,long *ixn,long *ixn2,long *arIndex,long nN,long nN2,long arNum,long m,long n,long prnt);

void spxTBL_obj(double **spxTBL,double *c,long m,long n);
void spxTBL_norm(double **spxTBL,long *ixb,long m,long n);

void standard(double **a,double *b,double *c,double *L,double *U,long m,long &n,long nEQ,long prnt);
void typeSpxTBL(double **spxTBL,long m,long n,char *caption);
/////////////////////////////////////////////////////////////////////////////////////

#endif
