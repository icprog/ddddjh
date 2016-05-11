#ifndef __OPF_GLOBAL_H__
#define __OPF_GLOBAL_H__
///////////////////////////////////////////////

struct areastr *areaData;   //1 area powers
struct assostr *assoData;   //2 associate
struct capastr *capaData;   //3 capacitor
struct genrstr *genrData;   //4 generator
struct genrstr *genrData2;  //5 non-bidding genr.
struct groupstr*groupData;  //6 line-group
struct linestr *lineData;   //7 line and xfmr
struct loadstr *loadData;   //8 load from Sybase 
struct nodestr *nodeData;   //9 node
struct plantstr*plantData;  //10 non-bidding plant
struct pricstr *priceData;  //11 bidding curve
struct shedstr *shedData;   //12 load shed
struct linestr *subLineData;//13 sub- line and xfmr
struct voltstr *voltData;   //14 load voltage
struct xfmrstr *xfmrData;   //15 xfmr appended

long areaNum;
long assoNum;
long capaNum;
long genrNum;
long genrNum2;
long groupNum;
long lineNum;
long loadNum;
long nodeNum;
long plantNum;
long pvNodeNum;
long shedNum;
long slckNum;
long voltNum;
long xfmrNum;
///////////////////////////////////////////////

double av_volt,flow_eps;//for powerflow
double loss_ratio,sys_wload;

long popu_size,rand_seed;//for ga_var
double div_step,pnsh_ctrl;
double capa_cost,reac_cost;

double genw_eps,genw_maxstep,ov_coeff;
double capa_eps,capa_maxstep;
double genv_eps,genv_maxstep;
double xfmr_eps,xfmr_maxstep;
/////////////////////////////////////////////////////////

char projname[10*IDLN];
char basefile[20],datafile[20],verifile[20],resufile[20];
char initfile[20],mlabfile[20],sensfile[20],tempfile[20];

long opf_island,coslack_num;
long *codeList,*codeAttr;
double *_B1,**B1,*_B2,**B2,*_G,**G,*_B,**B;
double *dPowr,*nodeP,*nodeQ,*vv,*v0;
////////////////////////////////////////////////////////

#endif
