//opf.h by Wang GT
///////////////////////////////

#ifndef __OPF_H__
#define __OPF_H__

#include"common.h"
//////////////////////////////////////////

#define SLACK_PMAX  2.00  //200MW
#define KB_COEFF    0.70  //70% lmt

#define OPF_SUCCESS 0
#define OPF_FAIL    1

#define LINE_BR     0
#define LINE_LN     1
#define LINE_XF     2

#define LN_RX       0
#define LN_B0       1
#define LN_LMT      2

#define GENR_POWR   0
#define GENR_VOLT   1

#define BD_POWR     0
#define BD_COST     1

#define DIV_WREGU   2	
#define DIV_WSCED   2
#define DIV_VARDISP 2
#define DIV_VARREGU 2

#define MODINO		0
#define MODIPG		1
#define MODIVAR		2
#define MODILD		3

#define OFFILND		0	//off island
#define ONISLND		1	//on island

#define AAAACASE	0	//initial value
#define CAPACASE	1	//capacitor
#define CTG_CASE	2	//contingency
#define GENRCASE	3	//generator
#define GROUPCAS	4	//group
#define LINECASE	5	//line & transformer
#define NODECASE	6	//node
#define PHASCASE	7	//phase shifter
#define PRICCASE	8	//price curve
#define VOLTCASE	9	//voltage monitored
#define XFMRCASE	10	//transformer

#define PV 		-1	//non_slack generator
#define PQ  	0	//loads
#define SLACK 	1	//slack generator
#define SPQ 	2	//special/zero loads
#define PVSLACK 3	//PV or SLACK
#define SLCK 	SLACK
#define SLK 	SLACK

#define ITRX_START  0
#define PNSH_POINT	2

#define MAX_GENR	200	//max.number of generators
#define MAX_ITRX	100	//max.iteration
#define MAX_LAYR	5	//max.layer for load shedding
#define MAX_NODE	1000	//max.number of nodes
#define MAX_POINT	30	//max.number of points for bidding curve 
#define MAX_PRI		5	//max.priority for shedding
#define MAX_UNIT        30      //max.num of units in a plant
#define MAX_LINE	20	//max.num of lines in a group
#define MAX_MODE	20	//max.num of modes for a group

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

#define FLOW_GROUP_TITL "\n id   | grp_w   |imode | grp_lmt  |kbFlg |vioFlg"
#define FLOW_GROUP_FORM "\n%s %9.4lf %4ld %9.4lf %4ld %4ld"

#define FLOW_LINE_TITL "\n   I  |  J  |   PIJ   |   QIJ   |   PJI   |   QJI  |   S1  |   S2  | vio | id"
#define FLOW_LINE_FORM "\n%4ld %4ld  % 8.4lf % 8.4lf  % 8.4lf % 8.4lf  % 8.4lf % 8.4lf %4ld %s"

#define FLOW_NODE_TITL "\n   # |  NAME  |   VV   |   JD   |   PH   |   QH   |   PF   |   QF"
#define FLOW_NODE_FORM "\n%4ld %4ld  %8.4lf % 8.3lf  %8.4lf %8.4lf  %8.4lf %8.4lf"

#define FLOW_SUBLINE_TITL "\n   I  |  J  |   PIJ   |   QIJ   |   PJI   |   QJI  |   S1  |   S2  | id"
#define FLOW_SUBLINE_FORM "\n%4ld %4ld  % 8.4lf % 8.4lf  % 8.4lf % 8.4lf  % 8.4lf % 8.4lf %s"

#define FLOW_SYS_TITL "\n WGEN  | WLOAD  | WLOSS  | MAX_V at # | MIN_V at  # | MAX_LOSS at  # #"
#define FLOW_SYS_FORM "\n%8.4lf %8.4lf %7.4lf %7.4lf %4ld %7.4lf %4ld %7.4lf %4ld %4ld"

#define FLOW_VOLT_TITL "\n ibs  |  V   | Vmin  | Vmax  | vio | id"
#define FLOW_VOLT_FORM "\n%4ld % 8.4lf % 8.4lf % 8.4lf %4ld %s"

#define GENR_TITL "\nDE ibs  |Pmin  |Pmax  |ramp  |fall  |id"
#define GENR_FORM "\n   %4ld %6.2lf %6.2lf %6.2lf %6.2lf %s" 

#define GENR_TITL2 "\nDE I#   |P0   |id"
#define GENR_FORM2 "\n   %4ld %6.2lf %s"

#define GROUP_TITL "\nDE id   |lineNum |lmt    ||ln_ ibs zbs |ln_imst"
#define GROUP_FORM "\n   %s %4ld %10.4lf  "
#define GROUP_FORM_LN "\n   %4ld %4ld   imst=%4ld"

#define LINE_TITL "\nDE I#   |J#   |R     |X     |K(B0/2) |LMT   | st    | zst   | id"
#define LINE_FORM "\n   %4ld %4ld %7.4lf %7.4lf %7.4lf %7.2lf %s %s %s"

#define LOAD_TITL "\nDE ibs    |w       |r      |id"
#define LOAD_FORM "\n   %4ld %9.5lf %9.5lf %s"

#define NODE_TITL "\nDE I#   |attr |P   |Q(V)|ID"
#define NODE_FORM "\n   %4ld %4ld %9.5lf %9.5lf %s"

#define PLANT_TITL "\nDE id     |w     |unit.num |units"
#define PLANT_FORM "\n   %s %7.4lf %4ld"
  
#define POLICY_TITL "\n id     |ibs   |w0    |w     |inc"
#define POLICY_FORM "\n %s %4d %6.3lf %6.3lf %6.3lf"

#define PRIC_TITL "\nDE ibs  |pntNum |Pb   |Cb   |bidding curve"
#define PRIC_FORM "\n   %4ld %4ld %6.2lf %6.2lf" 

#define VOLT_TITL "\nDE I#   |Vmin   |Vmax   | pnsh  |ID"
#define VOLT_FORM "\n   %4ld %6.3lf %6.3lf %6.3lf %s"

#define XFMR_TITL "\nDE I#   |J#   |Kmin  |Kmax  |dK1   |f1   |dK2   |f2"
#define XFMR_FORM "\n   %4ld %4ld %6.3lf %6.3lf %6.3lf %6.3lf %6.3lf %6.3lf"
////////////////////////////////////////////////////////////////////////////////

struct areastr//area ex-powers 
{
	struct 	areastr *next;

	char	id[IDLN];
	char	descr[DESCRLN];

	long	lineNum;
	long    unitNum;

	struct  lnstr line[MAX_LINE+1];
	struct	unstr unit[MAX_UNIT+1];

	double	wgen;	//generation
	//double rgen;	//reactive
};

struct assostr//associate
{
	struct assostr *next;

	char	id[IDLN];
	char	descr[DESCRLN];

	long	unitNum;
	struct  unstr unit[MAX_UNIT+1];
};

struct capastr//capacitor
{		
	struct	capastr *next;	

	char 	id[IDLN];
	long	i;		//ibs
	
	double	Q0;		//base_case value
	double	Q;		//current reactive
	double	Qmin;
	double	Qmax;	//i.e. capacity
	
	double	eps;
	double	maxstep;

	double	dQ1;
	double	dQ2;
	double	f1;
	double	f2;
};

struct flowstr//line flow
{		
	struct	flowstr *next;	

	char	id[IDLN];	
	long	i;	//ibs	
	long	j;	//zbs
	
	double	w1;	//active power in posi.direction 
	double  r1;	//reactive power in posi.direction 
	
	double  w2;	//active power in nega.direction
	double	r2;	//reactive power in nega.direction 
	
	double	s1;
	double	s2;

	long	kbFlg;	//kb.flag
	long	vioFlg;	//vio.flag
};

struct genrstr//generator data
{		
	struct	genrstr *next;
	
	char	id[IDLN];
	char 	descr[DESCRLN];

	long	i;  //ibs

	double 	P0; //base_case
	double 	V0;	//target or base_case

	double  Pa; //asso. power
	double  Pc; //coslack power

	double	P;  //current active =P0+Pa+Pc
	double	Q;  //current reactive
	double	V;  //current voltage

	double	Pmin; //ctrl
	double	Pmax;
	double	Qmin;
	double	Qmax;

	double	Vmin; //ctrl
	double	Vmax;
	double	pnsh;

	double	eps;
	double	maxstep;

	double	ramp; //ramp ratio
	double	fall;	

	double	dV1;
	double	dV2;
	double	f1;
	double	f2;

	long krFlg;  //key reac.
	long vioFlg; //r.vio.
};

struct groupstr//monitored line-group
{
	struct groupstr *next;
	
	char	id[IDLN];
	char	descr[DESCRLN];

	long    imode;  //index of group mode
	double  lmt;    //active when imode=0

	long    lineNum;
	long    modeNum;

	struct  lnstr line[MAX_LINE+1];
	struct	mdstr mode[MAX_MODE+1];

	double  w;	//power flow
	double  r;

	long    expFlg;	//explain
	long	kbFlg;	//key br.
	long    vioFlg;	//violated
};
	
struct linestr//line/xfmr data
{	
	struct linestr *next;	
	
	char	id[IDLN];
	char	st[IDLN];
	char	zst[IDLN];

	long	i;	//ibs
	long	j;	//zbs

	double	r;
	double	x;	
	double	kb;  //K or B0/2
	double	lmt; //MW
};

struct loadstr//load data
{
	struct loadstr *next;

	char	id[IDLN];
	long	i;	//ibs

	double	w;	//MW
	double	r;	//MVar
	//double	pf;	//pwr.factor
};

struct nodestr//node data
{		
	struct nodestr *next;	

	char	id[IDLN];
	long	i;	//ibs
	long    attr;

	double  p;	//w
	double  q;	//r
};

struct phasstr//phase shifter
{
	struct phasstr *next;
	
	long i;	
	long j;
	double r;	
	double x;
	double kk;
	double k0;
};

struct plantstr//plant data
{
	struct plantstr *next;

	char 	id[IDLN];
 	long	unitNum;

	double	w;//plant sched.
	struct 	unstr unit[MAX_UNIT+1];
};

struct pricstr//price data
{
	struct pricstr *next;
	
	char 	id[IDLN];
	long 	i;	//ibs
	long 	pntNum;
	
	double 	Pb;
	double 	Cb;
	struct 	pntstr point[MAX_POINT+1];
};

struct shedstr//load shed
{
	struct shedstr *next;
	
	char	id[IDLN];
	long 	i;		//ibs
	long 	pri;	//priority
	
	double 	ratio;	//shedding ratio
	double 	ws;		//active power shed
	double 	rs;		//reactive shed
}; 

struct statstr//statistics
{
	long genrNum;
	long genw_regu;
	long genv_regu;
	
	long capaNum;
	long capa_regu;
	
	long xfmrNum;
	long xfmr_regu;

	double opftime;
	double itrtime;
	
	double offset;
	double purcost;
	
	long vbNum;
	char status[50];
};

struct sysstr//system info.
{
	//1: node info 
	
	long imaxVolt;
	long iminVolt;
	double maxVolt;
	double minVolt;

   	double wgen;
	double wload;
	double rgen;
	double rload;
	
	double wslack;
	double rslack;

	//2: branch info

	long ibsMaxLoss;
	long zbsMaxLoss;
	
	double maxLoss;	//max.bran.loss
	double sysLoss;	//sys.loss

	//3: other info

	double purcost;
	double runtime;
	char status[20];
};

struct voltstr//load voltage
{		
	struct voltstr *next; 

	char 	id[IDLN];
	long 	i;	//ibs

	double 	V;	//current voltage
	double 	Vmin;
	double 	Vmax;
	double	pnsh;

	long	kvFlg;
	long	vioFlg;	
};

struct xfmrstr//transformer
{		
	struct xfmrstr *next;	

	char	id[IDLN];
	long 	i;	//ibs
	long 	j;	//zbs
	
	double	K0;	//base_case value
	double 	K;	//current ratio

	double 	Kmin;	
	double 	Kmax;
	double	pnsh;

	double	eps;
	double	maxstep;
	
	double 	nom;	//K=1.0+(tap-nom)*step
	double 	step;

	double	dK1;
	double	dK2;
	double	f1;
	double	f2;
};
//////////////////////////////////////////////////////
//function prototypes

long attribute(long node);

template<class TYPE>
long brMatch(TYPE *bp,long i,long j);
long brMatch(struct lnstr ln,long i,long j);

long findk(struct pricstr *pp,double p);
long findk_ex(struct pricstr *pp,double p);

long findMarginalNode(struct linestr *lineData,long *list,long *list_flag,long n);
long findMarginalNode(struct linestr *lineData,long *tempList,long n,long line_attr);

long formLP_varDisp(double *currInc,double *PLKT,double *PLVG,double *PLQC,double *beta,double **sens,
		double *Lmin,double *Lmax,double *L2min,double *L2max,struct flowstr *flowData,long prnt);
long formLP_varRegu(double *currInc,double *alph_v,double *beta,double **sens,
		double *Lmin,double *Lmax,double *L2min,double *L2max,struct flowstr *flowData,long prnt);

long formLP_w(double *genrInc,double *d,double *beta_w,double **PBPG,double *LBmin,double *LBmax,
			  double *Lmin_w,double *Lmax_w,struct flowstr *flowData,long prnt);

long formLP_w(double *genrInc,double *d,double *beta_w,double **PBPG,double *LBmin,double *LBmax,
			  double **GRPG,double *LGmax,double *Lmin_w,double *Lmax_w,struct flowstr *flowData,long prnt);

long getSlide(long code,long *codeAttr,long swtch);
long getNumMT(double *LBmin,double *LBmax,long lineNum);

long hangback(double *prevInc,double *currInc,long n);
long interConnex(struct linestr *lineData);

long isNewLine(long i,long j);
long isTerminalNode(long i,struct linestr *lineData);

template<class TYPE>
long kbNumStat(TYPE *chain);
long krNumStat(struct genrstr *genrData);

template<class TYPE>
long vioNumStat(TYPE *chain);

long match(struct capastr *np1,struct capastr *cp2);
long match(struct linestr *np1,struct linestr *np2);
long match(struct nodestr *np1,struct nodestr *np2);
long match(struct xfmrstr *np1,struct xfmrstr *np2);

long overload(long ibs,long zbs,struct flowstr *flowData);
long reguNum(double *inc,double eps,long n);
long reguNum(struct genrstr *genrData,long swtch);
long sawtooth(double *prevInc,double *currInc,long n);
/////////////////////////////////////////////////////////////////////////

double alphFun(double dX,double dX1,double dX2,double f1,double f2);
double aPij_KT(struct xfmrstr *xp,double *vv,double *v0,long swch);
double aQij_KT(struct xfmrstr *xp,double *vv,double *v0,long swch);

double calCC(struct pricstr *pp,double p);
double calPi(double **G,double **B,double *vv,double *v0,long i);
double calQi(double **G,double **B,double *vv,double *v0,long i);
double calVioper(struct flowstr *flowData);
double costFun(double *currGenr,long genrNum);

double offsetFun(double *crrKT,double *crrVG,double *crrQC);
double pnshValu(double dX,double dX1,double dX2,double f1,double f2);
double slackPhAngl(struct nodestr *nodeData);

double vioPctStat(struct flowstr *flowData);
double vioPctStat(struct groupstr *groupData);

double wgenStat(struct nodestr *nodeData);
double wloadStat(struct nodestr *nodeData);
////////////////////////////////////////////////////////////////////////////////////

void addSubData(struct linestr *&subLineData,struct linestr *lp);
void areaExPower(struct nodestr *nodeData,struct areastr *areaData,long prnt);
void assoFun(double *currGenr,struct genrstr *genrData,struct assostr *assoData,long prnt);
void autoSlackSet(struct nodestr *nodeData,struct genrstr *genrData,long prnt);

void baselineAlt(struct capastr *capaData,double orig_multpl,double targ_multpl);
void baselineAlt(struct genrstr *genrData,double orig_multpl,double targ_multpl);
void baselineAlt(struct groupstr *groupData,double orig_multpl,double targ_multpl);
void baselineAlt(struct linestr *lineData,double orig_multpl,double targ_multpl,long swch);
void baselineAlt(struct nodestr *nodeData,double orig_multpl,double targ_multpl,long attr);
void baselineAlt(struct plantstr *plantData,double orig_multpl,double targ_multpl);
void baselineAlt(struct pricstr *pricData,double orig_multpl,double targ_multpl,long swch);

void beta_varfun(double *beta,double *vv,double *v0,long *codeList,long prnt);
void beta_wfun(double *beta_w,double *PLPG,long genrNum);

void calGenrPowr(double *nodeP,double *nodeQ,long *codeList,long *codeAttr,struct flowstr *lineFlow);
void calFlowData(struct flowstr* &flowData,struct linestr *lineData,double *vv,double *v0,long *codeList);
void calSysInfo(struct sysstr &sysInfo,struct flowstr *flowData);

void codingNode(long *newCode,long *codeAttr);
void codingNode2(long *codeList,long *codeAttr);

void combinData(struct capastr *cp,struct capastr *cp2);
void combinData(struct linestr *lp,struct linestr *lp2);
void combinData(struct nodestr *np,struct nodestr *np2);
void combinData(struct xfmrstr *xp,struct xfmrstr *xp2);

void combinLineData(struct linestr *lineData,struct linestr *&subLineData);
void countLine(long **mtrx,long *nodeLine,long size);

void DPfun(double **DP,long *tempList,long n,long prnt);
void DQfun(double **DQ,long *tempList,long n,long prnt);

void filterData(struct assostr *&assoData);
void filterData(struct capastr *&capaData);
void filterData(struct genrstr *&genrData);
void filterData(struct groupstr *&groupData);

void filterData(struct nodestr *&nodeData);
void filterData(struct pricstr *&pricData);

void flowInit(long prnt);
long flowFun(struct flowstr *&flowData,struct flowstr *&subFlowData,struct sysstr &sysInfo,long modi,long prnt);

void formB1(double **B1,long *tempList,long n);
void formB2X(double **B2,long *tempList,long n);
void formB1(double **B1,long *codeList,long *codeAttr);
void formB2X(double **B2,long *codeList,long *codeAttr);

void formMtrx(long **mtrx,long *list);
void formNodeIsland(long *list,long &n,long kernel_bs,struct linestr *lineData,long line_attr);

void formShedList(long *shedList,struct flowstr *flowData,long ibs,long zbs,long layr);
void formTempList(long *tempList,long *codeList,long *codeAttr,long skip_swch);
void formTempList(long *tempList,long *codeList,long skip_id);

void formY(double **G,double **B,long *codeList,long modi);
void fprintCondition(FILE *out,char id[],long mel,double value);
void freeGlobal();

void getBranInc(double *branInc,double *currInc,double **PBPG);
void getBranchGB(struct linestr *lp,double &brG,double &brB);
void getdPowr(double *dPower,long swtch);

void getNodePowr(double *nodeP,double *nodeQ,long *codeList,long modi);
void getOverloadBranch(long &ibs,long &zbs,struct flowstr *flowData);
void getValue(double *valu,long **popu,double *step,double *capa,double *genv,double *xfmr,
								long chrmNum,long ctrlNum,long prnt);
void groupExpFlgReset(struct groupstr *groupData);
void groupExpFlgSet(struct groupstr *groupData);

void groupFlow(struct groupstr *groupData,struct flowstr *flowData,struct flowstr *subFlowData);
void groupModeAnalysis(struct groupstr *groupData,struct flowstr *flowData,struct flowstr *subFlowData);

void HPfun(double **HP,long *tempList,long n);
void HQfun(double **HQ,long *tempList,long n);

void incremental(double *inc,double *crr,struct genrstr *genrData,long swch);
void incremental(double *inc,double *crr,struct xfmrstr *xfmrData);
void incremental(double *inc,double *crr,struct capastr *capaData);

void initCtrlData(struct capastr *capaData);
void initCtrlData(struct genrstr *genrData);
void initCtrlData(struct shedstr *shedData);
void initCtrlData(struct xfmrstr *xfmrData);

void initGlobal();
void initVolt(double *vv,double *v0,long *codeList,long modi);
void Jacobi(double **J,double **G,double **B,double *vv,double *v0);

void LBfun_w(double *LBmin,double *LBmax,struct flowstr *flowData);
void LGfun_w(double *LGmax,struct groupstr *groupData);
void LBfun_var(double *Lmin,double *Lmax,struct flowstr *flowData,double *vv,double *nodeQ,long *codeList);

void limFun_w(double *Lmin_w,double *Lmax_w,double *currGenr);
void limFun_var(double *L2min,double *L2max,double *crrKT,double *crrVG,double *crrQC);

void lineFlow(double &w1,double &r1,double &w2,double &r2,double &s1,double &s2,struct linestr *lp);
void loadDisp(struct nodestr *nodeData,struct loadstr *loadData,double sys_load,long prnt);
void lwrReach(long *shedList,long &n,struct flowstr *flowData,long currNode);

void modiBusAngl(double *v0,double *genrInc);
void modiCtrlVctr(double *currGenr,double *currInc);
void modiCtrlVctr(double *crrKT,double *crrVG,double *crrQC,double *currInc);
void modiFlowData(struct flowstr *flowData,double *branInc);
void modifyV(double *vector,double *error,long *codeAttr,long swtch);

void modiSlack(struct genrstr *genrData,double *nodeP,long *codeList);
void moniVoltage(struct voltstr *voltData,double *vv,long voltNum);

void nodeOptimize(long **mtrx,long *list,long *codeList);
void nodeNumStat(long &nodeNum,long &pvNodeNum,long &slckNum);

void objLnz_var(double *alph_v,double *crrKT,double *crrVG,double *crrQC);
void objLnz_w(double *d,double *crrGn,long prnt);

void OPF_prtt(long prnt);
void OPF_readData(char datafile[],long prnt);
void OPF_start(char *project,long prnt);

void pickCurrGenr(double *currGenr,double *nodeP,long *codeList);
void pickCurrCtrl(double *crrKT,double *crrVG,double *crrQC);
void pickGroupW(double *grp_w,struct groupstr *groupData,long groupNum);

void plantSched(struct plantstr *plantData,struct nodestr *nodeData,long prnt);
void printDataFile(char datafile[],long prnt);

void reportGroupFlow(FILE *out,struct groupstr *groupData,long prnt);
void reportGroupLmt(FILE *out,struct groupstr *groupData,long prnt);

void reportLine(FILE *out,struct flowstr *flowData,struct flowstr *subFlowData,long prnt);
void reportNode(FILE *out,double *vv,double *v0,double *nodeP,double *nodeQ,long *codeList,long prnt);
void reportReguPolicy(FILE *out,struct genrstr *genrData,long prnt);

void reportSys(FILE *out,struct sysstr sys,long prnt);
void reportVioGroup(FILE *out,struct groupstr *groupData,long prnt);
void reportVolt(FILE *out,struct voltstr *voltData,long prnt);

void sens_GRPG(double **GRPG,double **PBPG,long groupNum,long genrNum);
void sens_PBPG(double **PBPG,double **DP,long *tempList,long n);

void sens_PLKT(double *PLKT,double *PLPQ,double *vv,double *v0,long *codeList);
void sens_PLPG(double *PLPG,double *PLPQ,long *codeList);
void sens_PLPQ(double *PLPQ,double **J2,long slack,long prnt);
void sens_PLQC(double *PLQC,double *PLPQ,long *codeList);
void sens_PLVG(double *PLVG,double **J2,long *codeList);

void sens_QBKT(double **QBKT,double *vv,double *v0,long *codeList);
void sens_QBVG(double **QBVG,double *vv,double *v0,long *codeList);
void sens_QBQC(double **QBQC,double **DQ,long *tempList,long n);

void sens_QGKT(double **QGKT,double *vv,double *v0);
void sens_QGVG(double **QGVG,double **G,double **B,double *vv,double *v0);
void sens_QGQC(double **QGQC,double **QBQC);
void sens_VDXX(double **VDKT,double **VDVG,double **VDQC,double **QGVG,double *vv,double *v0,long prnt);

void setGroupLmt(char *group_id,char *mode_id,double mode_lmt,struct groupstr *groupData);
void setNodeData(char *node_id,double w,long attr,struct nodestr *nodeData);

void shedding(double &wshed,double &rshed,long *shedList,long pri);
void showVioData(struct genrstr *genrData);

void stepConfin(double *step,long n,double maxstep,double divstep);
void stepConfin(double *step,long n1,long n2,double maxstep,double divstep);

void stepReduce(double *step,long n,double divstep);
void stepReduce(double *step,long n1,long n2,double divstep);

void upDateData(struct capastr *capaData,double *capa,long capaNum);
void upDateData(struct genrstr *genrData,double *genv,long genrNum);
void upDateData(struct xfmrstr *xfmrData,double *xfmr,long xfmrNum);

void uncoding(double *ctrl,double *step,long *chrm,long n1,long n2);
void unitSched(struct genrstr *_genrData,struct nodestr *_nodeData,long prnt);
void untread(double *prevInc,double *currInc,double *crrKT,double *crrVG,double *crrQC,double divstep);

long variation(long gene,double r);

void veriData(struct capastr *capaData);
void veriData(struct genrstr *genrData);
void veriData(struct groupstr *groupData);

void veriData(struct nodestr *nodeData);
void veriData(struct pricstr *pricData);
void veriData(struct xfmrstr *xfmrData);

void veriFlowData(struct flowstr *flowData,struct flowstr *subFlowData,double *nodeP,double *nodeQ);
//////////////////////////////////////////////////////////

struct nodestr* findData(char *id,struct nodestr* nodeData,long attr);
struct nodestr* findData(long ibs,struct nodestr* nodeData,long attr);

template<class TYPE>
TYPE* findData(long ibs,TYPE* dataChain);

template<class TYPE>
TYPE* findData(long ibs,long zbs,TYPE* dataChain);

template<class TYPE>
TYPE* findData(char id[],TYPE* dataChain);
////////////////////////////////////////////////////

#endif
