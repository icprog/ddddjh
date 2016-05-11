//opf_global.h by Wang GT
//////////////////////////////////////

#ifndef __OPF_GLOBAL_H__
#define __OPF_GLOBAL_H__

#include"opf.h"
///////////////////////////////////////

//OPF globals

double *branInc;
double *currInc;
double *prevInc;

double *crrgen;
double *optgen;

double *beta_w;
double *d;
double *currPric;
double *LBmin;
double *LBmax;
double *LGmin;
double *LGmax;
double *Lmin_w;
double *Lmax_w;
///////////////////////////////////////

//general globals

long opf_island;
long opf_objty;

double div_coeff;
double ov_coeff;

char user_id[50];
char user_descr[100];
//////////////////////////////////////

//sens. solution

double *PLPG;
double *PLKT;
double *PLVG;
double *PLQC;

double *_GRPD,**GRPD;
double *_GRPG,**GRPG;
double *_PBPD,**PBPD;
double *_PBPG,**PBPG;
double *_QBKT,**QBKT;
double *_QBVG,**QBVG;
double *_QBQC,**QBQC;
double *_QGKT,**QGKT;
double *_QGVG,**QGVG;
double *_QGQC,**QGQC;
double *_VmKT,**VmKT;
double *_VmVG,**VmVG;
double *_VmQC,**VmQC;
/////////////////////////////////////

//original data

struct areastr  *areaData; 
struct curvestr *curveData;
struct assostr  *assoData;
struct capastr  *capaData;
struct genrstr  *genrData;
struct groupstr *groupData;
struct itrstr   *itrData;
struct linestr  *lineData;
struct loadstr  *loadData; 
struct nodestr  *nodeData;
struct patnstr  *patnData;
struct pricestr *priceData;
struct shortstr *shortData;
struct linestr  *sub_lineData;
struct taptystr *taptyData;
struct voltstr  *voltData;
struct xfmrstr  *xfmrData; 
////////////////////////////////////////

//power flow solution

struct flowstr *flowData0;
struct flowstr *flowData;
struct flowstr *sub_flowData0;
struct flowstr *sub_flowData;

struct sysstr sysInfo0;
struct sysstr sysInfo;
////////////////////////////////////////

//global numbers

long areaNum;
long assoNum;
long capaNum;
long curveNum;
long genrNum;
long groupNum;
long lineNum;
long loadNum;
long nodeNum;
long patnNum;
long plantNum;
long pvNodeNum;
long slackNum;
long shortNum;
long taptyNum;
long voltNum;
long xfmrNum;
/////////////////////////////////////////

//gavar program

long popu_size;
long rand_seed;

double loss_price;
double yr_payback;
/////////////////////////////////////////

//ctrl. eps and maxstep

double genw_eps;
double genw_maxstep;

double genv_eps;
double genv_maxstep;

double capa_eps;
double capa_maxstep;

double xfmr_eps;
double xfmr_maxstep;
////////////////////////////////////////

//power flow

long *codeAttr;
long *codeList;

double av_volt;
double flow_eps;
double loss_ratio;
double pwrbase;

double *_B1,**B1;
double *_B2,**B2;
double *_G,**G;
double *_B,**B;

double *dPowr;
double *nodeP;
double *nodeQ;
double *vv;
double *v0;
double *v_adeg;
/////////////////////////////////////

#endif
