//opf.h by Wang GT
///////////////////////////////

#ifndef __OPF_H__
#define __OPF_H__

#include"common.h"
//#include"patn.h"
//////////////////////////////////////////

#define MAX_LABEL   100
#define MAX_MARK    50
#define MAX_NOTES   100
#define MAX_PLOT    10

#define EQCON_VAR   1
#define EQCON_W     1

#define KB_COEFF    0.98
#define KR_COEFF    0.98
#define KV_COEFF    1.00

#define COSLACK_COEFF 0.10 //10%
#define PERT_COEFF    0.05 //5%

#define OPF_VARAPP  0
#define OPF_WAPP    1 

#define LINE_BR     0
#define LINE_LN     1
#define LINE_XF     2

#define LN_RX       0
#define LN_B0       1

#define GENR_POWR   0
#define GENR_VOLT   1

#define BD_POWR     0
#define BD_COST     1

#define MODLD      0
#define MODNO      1
#define MODPG      2
#define MODSCC     3
#define MODVAR     4

#define OFFISLND    0  //off island
#define ONISLND     1  //on island

#define AAAACASE    0  //initial value
#define CAPACASE    1  //capacitor
#define CURVCASE    2  //curve
#define CTG_CASE    3  //contingency
#define GENRCASE    4  //generator
#define GROUPCAS    5  //group
#define LINECASE    6  //line & transformer
#define NODECASE    7  //node
#define PHASCASE    8  //phase shifter
#define PRICCASE    9  //price curve
#define TAPTYCAS   10  //taptype for xf.
#define VOLTCASE   11  //voltage monitored
#define XFMRCASE   12  //transformer

#define PV       -1    //non_slack generator
#define PQ        0    //loads
#define SLACK     1    //slack generator
#define PVSLACK   2    //PV or SLACK
#define ZPQ       3    //zero PQ

#define ITR_START  0
#define ITR_MAX    200
#define PNSH_COEFF 0.25 

#define MAX_GENR   200  //max.number of generators
#define MAX_POINT  30   //max.number of points for bidding curve 
#define MAX_NODE   1000 //max.number of nodes
#define MAX_LINE   20   //max.num of lines in a group
#define MAX_MODE   20   //max.num of modes in a group
#define MAX_UNIT   30   //max.num of units in a plant

#define P_V0  1	//No change!
#define Q_VV -1

#define SKIP_SLCK P_V0
#define SKIP_GENR Q_VV
////////////////////////////////////////////////////////

#define AREA_TITL "\nDE id       |wgen   |lineNum |unitNum ||w |r |wt |pf"
#define AREA_FORM "\n   %s %6.3lf %4ld %4ld %s"
#define ARLN_FORM "\n   %4ld--%4ld(%s) %6.3lf %6.3lf %6.3lf %6.3lf" 
#define ARUN_FORM "\n   %4ld(%s) %6.3lf"

#define ASSO_TITL "\nDE id       |unitNum | unit_ibs(id)"
#define ASSO_FORM "\n   %s %4ld"
 
#define CAPA_TITL "\nDE ibs   | Q0     | Qmin    | Qmax    | id"
#define CAPA_FORM "\n   %4ld %9.4lf %9.4lf %9.4lf %s"

#define FLOW_GROUP_TITL "\n id   | grp_w   |imode | mwmin | mwmax  |kbFlg |vioFlg"
#define FLOW_GROUP_FORM "\n %s %8.4lf %4ld %7.3lf %7.3lf %3ld %3ld"

#define FLOW_LINE_TITL "\n  ibs  | zbs  | w1   | r1    | w2   | r2    | s1   | s2   | kb | vio | id"
#define FLOW_LINE_FORM "\n %4ld %4ld  % 7.3lf % 7.3lf  % 7.3lf % 7.3lf  % 8.2lf % 8.2lf %3ld %3ld %s" 

#define FLOW_NODE_TITL "\n   # | ibs   | v     | adeg   | wload  | rload   | wgen  | rgen"
#define FLOW_NODE_FORM "\n%4ld %4ld  %8.4lf % 8.3lf  %8.4lf %8.4lf  %8.4lf %8.4lf"

#define FLOW_SUBLINE_TITL "\n   I  |  J  |   PIJ   |   QIJ   |   PJI   |   QJI  |   S1  |   S2  | id"
#define FLOW_SUBLINE_FORM "\n%4ld %4ld  % 8.4lf % 8.4lf  % 8.4lf % 8.4lf  % 8.3lf % 8.3lf %s"

#define FLOW_SYS_TITL "\n WGEN  | WLOAD  | WLOSS  | MAX_V at # | MIN_V at  # | MAX_LOSS at  # #"
#define FLOW_SYS_FORM "\n%8.4lf %8.4lf %7.4lf %7.4lf %4ld %7.4lf %4ld %7.4lf %4ld %4ld"

#define LINE_TITL "\nDE ibs   |zbs   |R     |X     |K(B0/2) |lmt   | vl   | id"
#define LINE_FORM "\n   %4ld %4ld %7.4lf %7.4lf %7.4lf  %7.2lf %7.2lf %s"

#define LOAD_TITL "\nDE ibs    |w       |r      |id"
#define LOAD_FORM "\n   %4ld %9.5lf %9.5lf  %s"

#define NODE_TITL "\nDE ibs  |attr |w    |r/v   |id"
#define NODE_FORM "\n   %4ld %4ld %9.5lf %9.5lf  %s"

#define PLANT_TITL "\nDE id     |w     |unit.num |units"
#define PLANT_FORM "\n   %s %7.4lf %4ld"
  
#define POLICY_TITL "\n id     |ibs   |w0    |w     |inc   |descr"
#define POLICY_FORM "\n %s %4d %6.3lf %6.3lf %6.3lf  %s"

#define PRIC_TITL "\nDE ibs  |pntNum |Pb   |Cb   |bidding curve"
#define PRIC_FORM "\n   %4ld %4ld %6.2lf %6.2lf" 

#define VOLT_TITL "\nDE ibs   |Vmin   |Vmax  |id"
#define VOLT_FORM "\n   %4ld %6.3lf %6.3lf  %s"
///////////////////////////////////////////////////////////////////////

//fundamental structures

struct lnstr//lines of group
{
        char    descr[DESCRLN];
        char    id[IDLN];

        long    i;      //ibs
        long    j;      //zbs

        char    mst_descr[DESCRLN];
        char    mst_id[IDLN];

        long    imst;

        double  w;      //active power
        double  r;      //reactive

        double  wt;     //area weight
        double  pf;     //power factor
};

struct mdstr//modes of group
{
        char    descr[DESCRLN];
        char    id[IDLN];

        double  mwmax;
        double  mwmin;

        char    dev_descr1[DESCRLN];
        char    dev_id1[IDLN];
        long    mel1;
        double  val1;

        char    dev_descr2[DESCRLN];
        char    dev_id2[IDLN];
        long    mel2;
        double  val2;

        char    dev_descr3[DESCRLN];
        char    dev_id3[IDLN];
        long    mel3;
        double  val3;

        char    dev_descr4[DESCRLN];
        char    dev_id4[IDLN];
        long    mel4;
        double  val4;

        char    dev_descr5[DESCRLN];
        char    dev_id5[IDLN];
        long    mel5;
        double  val5;
};

struct unstr//unit structure
{
        char    id[IDLN];
        long    i;      //ibs
        double  w;      //unit sched.

        double	inc;
        double  wmx;
};

////////////////////////////////////////////////////////////////

struct areastr//area ex-powers 
{
	struct 	areastr *next;

	char    descr[DESCRLN];
	char	id[IDLN];

	long	lineNum;
	long    unitNum;

	struct  lnstr line[MAX_LINE];
	struct	unstr unit[MAX_UNIT];

	double	wgen;	//generation
	//double rgen;	//reactive
};

struct assostr//associate
{
	struct assostr *next;

	char    descr[DESCRLN];
	char	id[IDLN];

	long	unitNum;
	struct  unstr unit[MAX_UNIT];
};

struct capastr//capacitor
{		
	struct	capastr *next;	

	char    descr[DESCRLN];
	char 	id[IDLN];
	long	i;    //ibs
	
	double	Q0;   //base_case value
	double	Q;    //current reactive

	double	Qmin;
	double	Qmax; //capacity
	double  pnsh; //rmb/kVar
	
	double	eps;
	double	maxstep;
};

struct curvestr//curve structure
{
	struct curvestr *next;

	char descr[DESCRLN];
	char id[IDLN];

	long ibs;
	long zbs;

	long EOplot;

	long markNum;
	long plotNum;

	struct markstr mark[MAX_MARK];
	struct plotstr plot[MAX_PLOT];

	char xlabel[MAX_LABEL];
	char ylabel[MAX_LABEL];
};

struct flowstr//line flow
{		
	struct	flowstr *next;	

	char    descr[DESCRLN];
	char	id[IDLN];	

	long	i;	//ibs	
	long	j;	//zbs
	
	double	w1;	//active power   (+)
	double  r1;	//reactive power (+)
	double  w2;	//active power   (-)
	double	r2;	//reactive power (-)
	
	double	s1;	//apparent power
	double	s2;

	double  pf1; //power factors
	double  pf2;

	double  vioPct;

	long	kbFlg;
	long	vioFlg;
};

struct genrstr//generator data
{		
	struct	genrstr *next;
	
	char    descr[DESCRLN];
	char	id[IDLN];

	char    st_id[IDLN];
	char    st_descr[DESCRLN];

	long	i;  //ibs

	double  plant;
	double  puf;
	double  xd2;

	double  w0;
	double  r0;
	double  v0;

	double  w;
	double  r;
	double  v;

	double	wmin;
	double	wmax;
	double	rmin;
	double	rmax;

	double	vmin;
	double	vmax;

	double	eps;
	double	maxstep;

	double	rampUp; //ramp ratio
	double	rampDn;	

	long synFlg;
	long offFlg;

	long krFlg;  //key reac.
	long vioFlg; //r.vio.
};

struct groupstr//mon. line-group
{
	struct groupstr *next;
	
	char    descr[DESCRLN];
	char	id[IDLN];

	long	i;    //index
	long    imode;//index to mode
	
	long    lineNum;
	long    modeNum;

	struct  lnstr line[MAX_LINE];
	struct	mdstr mode[MAX_MODE];

	double  mwmax;
	double  mwmin;

	double	w0;//before corr.
	double	r0;
	double	s0;

	double  w;
	double  r;
	double	s;
	double	vioPct;

	long	kbFlg;
	long    vioFlg;
};

struct itrstr//iteration
{
	struct itrstr *next;

	long i;
	long imin;

	long vioNum;

	double purcost;
	double vioPct;
};

struct linestr//line/xfmr data
{	
	struct linestr *next;	
	
	char    descr[DESCRLN];
	char    id[IDLN];

	char    st[IDLN];
	char    zst[IDLN];

	long    i;  //ibs
	long    j;  //zbs

	double  r;
	double  x;	
	double  b0k; //half-chrg./ratio

	double  lmt; //amp/MVA
	double  vl;

	double  pf_min;
	double  pf_max;
	double  pnsh;//rmb/hr
};

struct loadstr//load data
{
	struct loadstr *next;

	char	descr[DESCRLN];	
	char	id[IDLN];
	long	i;	//ibs

	double	w;	//MW
	double	r;	//MVar
};

struct nodestr//node data
{		
	struct nodestr *next;	

	char    descr[DESCRLN];
	char	id[IDLN];

	long	i;	//ibs
	long    attr;

	double  p;	//w, vv
	double  qv;	//r, vv, v0
	
	double  vl;
};

struct phasstr//phase shifter
{
	struct phasstr *next;
	
	char    descr[DESCRLN];
	char    id[IDLN];
	
	long 	i;	
	long 	j;

	double 	r;	
	double 	x;
	double 	kk;
	double 	k0;
};

struct pricestr//price data
{
	struct pricestr *next;
	
	char    descr[DESCRLN];
	char 	id[IDLN];

	long 	i;	//ibs
	long 	pntnum;
	
	double 	Pb;
	double 	Cb;
	struct 	pointstr point[MAX_POINT];
};

struct shortstr//short-circuit data
{
	struct shortstr *next;

	char    descr[DESCRLN];
	char    id[IDLN];

	long    i;	//ibs
	double  amp;

	double  r;
	double  x;

	double  v[MAX_NODE];
	double  bramp[MAX_LINE];
};

struct sysstr//system info.
{
        //system powers

        double rgen;
        double rload;
        double rslack;
        double wgen;   
        double wgen2;
        double wload;
        double wslack;
        double sysLoss;
        ////////////////////////////

        //ctrl. variables

        long capaNum;
        long genrNum;
        long xfmrNum;

        long capa_regu;
        long genw_regu;
        long genv_regu;
        long xfmr_regu;
        ///////////////////////

        //violation and cost 

        double vioPct;
        double offset;
        double purcost;

        long kbNum;
        long krNum;
        long vioNum;
        //////////////////////

        //nodal voltage

        long imaxVolt;
        long iminVolt;

        double maxVolt;
        double minVolt;
        ////////////////////////

        //branch loss

        long ibsMaxLoss;
        long zbsMaxLoss;

        double maxLoss;
        ///////////////////////////

        //status and runtime

        char status[50];

        double itrtime;
        double runtime;
};

struct voltstr//mon.voltage
{		
	struct voltstr *next; 

	char    descr[DESCRLN];
	char 	id[IDLN];

	long 	i;  //ibs
	double  vl; //voltage level
	
	double  V0; //base_case voltage
	double  V;  //current voltage
	
	double 	Vmin;
	double 	Vmax;
	double  pnsh;//rmb/hr

	long	kvFlg;
	long	vioFlg;	
};

struct xfmrstr//transformer
{		
	struct xfmrstr *next;	

	char    descr[DESCRLN];
	char	id[IDLN];

	long 	i;	 //ibs
	long 	j;	 //zbs

	double	K0;	 //base_case value
	double 	K;	 //current ratio  

	double 	Kmin; // <== mntap
	double 	Kmax; // <== mxtap

	double	eps;
	double	maxstep;

	long	tap;
	long	itapty;

	double	kvnom;
	double	zkvnom;

	double	kvvl;
	double	zkvvl;
};

struct taptystr//taptype for xfmr.
{
	struct taptystr *next;	

	char    descr[DESCRLN];
	char	id[IDLN];

	long	mntap;
	long	mxtap; 

	long	nom;
	double 	step;
};
//////////////////////////////////////////////////////////////////

//function prototypes

long attribute(long bs,struct nodestr *nodeData);
long coslack_fun(double *nodeP,struct genrstr *genrData);

template<class TYPE>
long brMatch(TYPE *bp,long i,long j);
long brMatch(struct lnstr ln,long i,long j);

long findk(double p,struct pricestr *pp);
long findk_ex(double p,struct pricestr *pp);
long findMarginalNode(struct linestr *lineData,long *list,long *flag,long n);

long formLP_varDisp(double *currInc,double *PLKT,double *PLVG,double *PLQC,double *beta,double **sens,
		double *Lmin,double *Lmax,double *L2min,double *L2max,struct flowstr *flowData,long prnt);
long formLP_varRegu(double *currInc,double *alph_v,double *beta,double **sens,
		double *Lmin,double *Lmax,double *L2min,double *L2max,struct flowstr *flowData,long prnt);

long formLP_w(double *genrInc,double *d,double *beta_w,double **PBPG,double *LBmin,double *LBmax,
	  double **GRPG,double *LGmin,double *LGmax,double *Lmin_w,double *Lmax_w,struct flowstr *flowData,long prnt);

long getSlide(long code,long *codeAttr,long swtch);
long getNumMT(double *LBmin,double *LBmax,long lineNum);

long hangback(double *prevInc,double *currInc,long n);
long isNewLine(long i,long j,struct linestr *lineData);
long isTerminalNode(long i,struct linestr *lineData);
long vio_check(double *crrgen,long genrNum,long itr,long prnt);

long kbNumStat(struct flowstr *flowData);
long kbNumStat(struct groupstr *groupData);

long krNumStat(struct genrstr *genrData);
long kvNumStat(struct voltstr *voltData);

long OPF_w_blpfun(double *currInc,double *crrgen,long genrNum,long prnt);

long plunus(struct taptystr *tp);
long step_check(char *resfile,char *waflg,double *currInc,double *prevInc,double maxstep,double eps,double div_coeff,long genrNum,long prnt);
long tapFun(struct xfmrstr *xp,struct taptystr *tp,double ratio);

template<class TYPE>
long vioNumStat(TYPE *chain);

long match(struct capastr *np1,struct capastr *cp2);
long match(struct genrstr *gp1,struct genrstr *gp2);
long match(struct linestr *lp1,struct linestr *lp2);
long match(struct nodestr *np1,struct nodestr *np2);
long match(struct pricestr *pp1,struct pricestr *pp2);
long match(struct xfmrstr *xp1,struct xfmrstr *xp2);

long reguNum(struct capastr *capaData);
long reguNum(struct genrstr *genrData,long swtch);
long reguNum(struct xfmrstr *xfmrData);

long veriFlowData(struct flowstr *flowData,double *nodeP,double *nodeQ,long nodeNum);
long veriFlowData(struct flowstr *subFlowData,struct flowstr *flowData);
/////////////////////////////////////////////////////////////////////////

double alphFun(double x,double x0,double H,double L,double D);

double calPi(double **G,double **B,double *vv,double *v0,long i);
double calQi(double **G,double **B,double *vv,double *v0,long i);

double lmtPU(struct linestr *lp);

double offsetFun(double *crrKT,double *crrVG,double *crrQC);
double offsetFun(struct genrstr *genrData);

double pnshFun(double x,double x0,double H,double L,double D);
double priceFun(double wgen,struct pricestr *pp);
double purcostFun(double *crrgen,long genrNum);

double ratioEpsFun(struct xfmrstr *xp,double step);
double ratioFun(struct xfmrstr *xp,struct taptystr *tp,long tap);

double slack_adeg(struct nodestr *nodeData);

double sum_genrData_P0(struct genrstr *genrData);
double sum_genrData_Q0(struct genrstr *genrData,long n);
double sum_groupData_w0(struct groupstr *groupData,long n);
double sum_voltData_V0(struct voltstr *voltData,long n);
double sysPurcost(double *crrGn,long genrNum);

double vioPctStat(struct flowstr *flowData);
double vioPctStat(struct genrstr *genrData);
double vioPctStat(struct groupstr *groupData);
double vioPctStat(struct voltstr *voltData);

double wgenStat(struct nodestr *nodeData);
double wgen2Stat(struct nodestr *nodeData);
double wloadStat(struct nodestr *nodeData);
////////////////////////////////////////////////////////////////////////////////////

void addSubData(struct linestr *&subLineData,struct linestr *lp);
void appendSysInfo_r(struct sysstr &sysInfo,struct flowstr *flowData,double itrtime,double runtime);
void appendSysInfo_w(struct sysstr &sysInfo,struct flowstr *flowData,double itrtime,double runtime);

void areaExPower(struct nodestr *nodeData,struct areastr *areaData,long prnt);
void assoFun(double *crrgen,struct genrstr *genrData,struct assostr *assoData,long prnt);
void autoSlackSet(struct nodestr *nodeData,struct genrstr *genrData,long prnt);

void baselineAlt(struct capastr *capaData,double orig_multpl,double targ_multpl);
void baselineAlt(struct genrstr *genrData,double orig_multpl,double targ_multpl);
void baselineAlt(struct groupstr *groupData,double orig_multpl,double targ_multpl);
void baselineAlt(struct linestr *lineData,double orig_multpl,double targ_multpl,long swch);
void baselineAlt(struct nodestr *nodeData,double orig_multpl,double targ_multpl,long attr);
void baselineAlt(struct pricestr *pricData,double orig_multpl,double targ_multpl,long swch);

void beta_varfun(double *beta,double **QGKT,double **QGVG,double **QGQC,long prnt);
void beta_wfun(double *beta_w,double *PLPG,long genrNum);

void branch_comp(double **group_sens,double **branch_sens,long groupNum,long ctrlNum);

void calGenrPowr(double *nodeP,double *nodeQ,long *codeList,long *codeAttr,struct flowstr *lineFlow);
void calFlowData(struct flowstr* &_flowData,struct linestr *_lineData);
void calSysInfo(struct sysstr &sysInfo,struct flowstr *flowData);

void codingNode(long *codeList,long *codeAttr,long nodeNum,long prnt);
void codingNode2(long *codeList,long *codeAttr);

void combinData(struct capastr *cp,struct capastr *cp2);
void combinData(struct genrstr *gp,struct genrstr *gp2);
void combinData(struct linestr *lp,struct linestr *lp2);
void combinData(struct nodestr *np,struct nodestr *np2);
void combinData(struct pricestr *pp,struct pricestr *pp2);
void combinData(struct xfmrstr *xp,struct xfmrstr *xp2);

void combinLineData(struct linestr *lineData,struct linestr *&subLineData);
void countLine(long *line_bs,long **mtrx,long n);
void DPfun(double **DP,long *tempList,long n,long prnt);

void filterData(struct capastr *&capaData,long planFlg);
void filterData(struct genrstr *&genrData);
void filterData(struct groupstr *&groupData);

void filterData(struct nodestr *&nodeData);
void filterData(struct pricestr *&pricData);
void filterData(struct voltstr *&voltData);
void filterData(struct xfmrstr *&xfmrData);

void formB1(double **B1,long *tempList,long n);
void formB2X(double **B2,long *tempList,long n);
void formB1(double **B1,long *codeList,long *codeAttr);
void formB2X(double **B2,long *codeList,long *codeAttr);
void formMtrx(long **mtrx,long *list,long n);

void formTempList(long *tempList,long *codeList,long *codeAttr,long skip_swch);
void formTempList(long *tempList,long *codeList,long skip_id);

void formY(double **G,double **B,long *codeList,long nodeNum,long modi);
void fprintCondition(FILE *out,char id[],long mel,double value);

void genrDataSlide(struct genrstr *gp,double dP,double dQ,char plunus);
void genrReactiv(struct genrstr *genrData,double *nodeQ,long *codeList,long nodeNum);

void getBranInc(double *branInc,double *currInc,double **PBPG);
void getBranchGB(struct linestr *lp,double &brG,double &brB);
void getdPowr(double *dPower,long swtch);

void getNodePowr(double *nodeP,double *nodeQ,long *codeList,long modi);
void getOverloadBranch(long &ibs,long &zbs,struct flowstr *flowData);
void getValue(double *valu,long **popu,double *step,double *capa,double *genv,double *xfmr,long chrmNum,long ctrlNum,long prnt);

void global_alloc();
void global_init();
void global_free();

void groupFlow(struct groupstr *groupData,struct flowstr *flowData,struct flowstr *subFlowData);
void groupModeAnalysis(struct groupstr *groupData,struct flowstr *flowData,struct flowstr *subFlowData);

void HPfun(double **HP,long *tempList,long n);
void initCtrlData(struct capastr *capaData);
void initCtrlData(struct genrstr *genrData);
void initCtrlData(struct xfmrstr *xfmrData);

void initGenrData(struct genrstr *genrData);//genr.power only
void initVolt(double *vv,double *v0,long *codeList,long prnt);
void Jacobi(double **J,double **G,double **B,double *vv,double *v0);

void LBfun_w(double *LBmin,double *LBmax,struct flowstr *flowData,double *vv,long *codeList);
void LGfun_w(double *LGmin,double *LGmax,struct groupstr *groupData);
void LBfun_var(double *Lmin,double *Lmax,struct flowstr *flowData,double *vv,double *nodeQ,long *codeList);

void limFun_w(double *Lmin_w,double *Lmax_w,double *crrgen);
void limFun_var(double *L2min,double *L2max,double *crrKT,double *crrVG,double *crrQC);

void lineFlow(double &w1,double &r1,double &w2,double &r2,double &s1,double &s2,double &pf1,double &pf2,struct linestr *lp);
void loadDisp(struct nodestr *nodeData,struct loadstr *loadData,double sys_load,long prnt);

void modiBusAngl(double *v0,double *genrInc);
void modiCtrlVctr(double *crrKT,double *crrVG,double *crrQC,double *currInc);

void modiFlowData(struct flowstr *flowData,double *branInc);
void modifyV(double *vector,double *error,long *codeAttr,long swtch);
void modiInitVolt(double *vv,long *codeList,long modi);

void modiSlack(struct genrstr *genrData,double *nodeP,long *codeList);
void monVoltage(struct voltstr *voltData,double *vv,long *codeList,long nodeNum);
void mvaAmpfun(double &s1,double &s2,double w1,double r1,double w2,double r2,struct linestr *lp);

void nodeOptimize(long *codeList,long *list,long **mtrx,long nodeNum);
void nodeNumStat(long &nodeNum,long &pvNodeNum,long &slckNum);

void objLnz_var(double *alph_v,double *crrKT,double *crrVG,double *crrQC);
void objLnz_w(double *d,double *wgen,long genrNum,long prnt);
void objLnz_wcorr(double *d,double *wgen,long genrNum,long prnt);

void OPF_baselineAlt(long prnt);
void OPF_org_pwrflow(char *resfile,long prnt);
void OPF_opt_pwrflow(char *resfile,double *crrgen,long genrNum,long prnt);

void OPF_prtt(long prnt);
void OPF_readData(char datafile[],long prnt);
void OPF_start(char *_homepath,long prnt);
void OPF_vfyData();

void OPF_w_alloc();
void OPF_w_free();
void OPF_w_sens(char *snsfile,long prnt);

void OPF_writeLog(struct sysstr sysInfo0,struct sysstr sysInfo,struct genrstr *genrData);

void pertsens_XXPG(double **PBPG,long prnt);

void pickCurrCtrl(double *crrQC,struct capastr *capaData);
void pickCurrCtrl(double *crrVG,struct genrstr *genrData);
void pickCurrCtrl(double *crrKT,struct xfmrstr *xfmrData);
void pickCurrGenr(double *crrgen,double *nodeP,long *codeList);

void print_capaData(FILE *out,long prnt);
void print_ctrlparm(FILE *out,long prnt);

void print_genrData(FILE *out,long prnt);
void print_groupData(FILE *out,long prnt);

void print_lineData(FILE *out,long prnt);
void print_loadData(FILE *out,long prnt);

void print_nodeData(FILE *out,long prnt);
void print_priceData(FILE *out,long prnt);
void print_shortData(FILE *out,long prnt);

void print_voltData(FILE *out,long prnt);
void print_xfmrData(FILE *out,long prnt);

void printDatafile(char *datafile,long prnt);
void printUsage();

long pwrflow(long modi,long prnt);

long pwrflow_itr(long prnt);
void pwrflow_init(long prnt);

void pwrflow_report(char *resfile,char *waflg,long prnt);
void pwrflow_update(double *currInc,long genrNum,long prnt);

void readSysData(FILE *in,long prnt);

void report_GRPD(FILE *out,double **PBPD,long prnt);
void report_GRPG(FILE *out,double **GRPG,long prnt);

void report_PBPD(FILE *out,double **PBPD,long prnt);
void report_PBPG(FILE *out,double **PBPG,long prnt);

void reportGenrReactiv(FILE *out,struct genrstr *genrData,long prnt);
void reportGroupFlow(FILE *out,struct groupstr *groupData,long prnt);
void reportGroupLmt(FILE *out,struct groupstr *groupData,long prnt);

void reportLine(FILE *out,struct flowstr *flowData,struct flowstr *subFlowData,long prnt);
void reportMonVoltage(FILE *out,struct voltstr *voltData,long prnt);
void reportNode(FILE *out,double *vv,double *v0,double *nodeP,double *nodeQ,long *codeList,long prnt);

void reportReguPolicy(char *resfile,char *waflg,struct genrstr *genrData,long prnt);
void reportSys(FILE *out,struct sysstr sys,long prnt);
void reportVioGroup(FILE *out,struct groupstr *groupData,long prnt);

void saveFlowSolution(double *v,long voltNum,struct voltstr *voltData);
void saveFlowSolution(double *v,long genrNum,struct genrstr *genrData);
void saveFlowSolution(double *v,long lineNum,struct flowstr *flowData);

void sens_r_cal(long prnt);
void sens_w_cal(long prnt);
void sens_w_report(char *resfile,char *wrFlg,long prnt);

void sens_BGVKT(double *QB0,double *QG0,double *Vm0,double PL0,struct xfmrstr *xfmrData,long prnt);
void sens_BGVVG(double *QB0,double *QG0,double *Vm0,double PL0,struct genrstr *genrData,long prnt);
void sens_BGVQC(double *QB0,double *QG0,double *Vm0,double PL0,struct capastr *capaData,long prnt);

void sens_PBPD(double **PBPD,double **DP,long *tempList,long n);
void sens_PBPG(double **PBPG,double **DP,long *tempList,long n);

void sens_PLPG(double *PLPG,double *PLPQ,long *codeList);
void sens_PLPQ(double *PLPQ,double **J2,long slack,long prnt);
void sens_var(double *beta,double **sens,long prnt);

void setGroupLmt(char *group_id,char *mode_id,double mode_lmt,struct groupstr *groupData);
void setNodeData(char *node_id,double w,long attr,struct nodestr *nodeData);

void stepConfin(double *step,long n,double maxstep,double divstep);
void stepConfin(double *step,long n1,long n2,double maxstep,double divstep);

void stepReduce(double *step,long n,double divstep);
void stepReduce(double *step,long n1,long n2,double divstep);

void synchoff_adj(struct genrstr *genrData,double **GRPG,long groupNum,long genrNum,long prnt);
void synchoff_init(struct genrstr *genrData);

void tp_conn(struct linestr *lineData);
void tp_bs();  //SVL-EB ==> bs
void tp_ptr(); //ptr. structure

void updateData(struct capastr *capaData,double *crrQC,long capaNum);
void updateData(struct genrstr *genrData,double *crrGn,long genrNum);
void updateData(struct genrstr *genrData,double *crrVG,long genrNum);
void updateData(struct xfmrstr *xfmrData,double *crrKT,long xfmrNum);

void uncoding(double *ctrl,double *step,long *chrm,long n1,long n2);
void unitSched(struct genrstr *_genrData,struct nodestr *_nodeData,long prnt);

void untread(double *ctrl,double *inc,long n);
void untread(double *crrKT,double *crrVG,double *crrQC,double *inc,long n);

void vfyData(struct capastr *capaData);
void vfyData(struct genrstr *genrData);
void vfyData(struct groupstr *groupData);
void vfyData(struct nodestr *nodeData);
void vfyData(struct pricestr *pricData);
void vfyData(struct xfmrstr *xfmrData);
///////////////////////////////////////////////////////////////////////////////////

struct nodestr* findData(char *id,struct nodestr* nodeData,long attr);
struct nodestr* findData(long ibs,struct nodestr* nodeData,long attr);

template<class TYPE>
TYPE* findData(long ibs,TYPE* dataChain);

template<class TYPE>
TYPE* findData(long ibs,long zbs,TYPE* dataChain);

template<class TYPE>
TYPE* findData2(long ibs,long zbs,TYPE* dataChain);

template<class TYPE>
TYPE* findData(char *id,TYPE* dataChain);
////////////////////////////////////////////////////

#endif
