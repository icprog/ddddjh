//psbobserve.h by Wang GT
////////////////////////////////////////////////

#ifndef __PSBOBSERVE_H__
#define __PSBOBSERVE_H__

#include<dstruct.h>
#include<oodbglobe.h>
#include<oodb.h>
#include<psbob.h>
//////////////////////////////////////

#define _MXELE  27
#define STID 13 
#define TP_FAIL 0
#define TP_SUCCEED 1

/* database descr DB_DESCR */
DB_DESCR     *psbob_db_descr;



/* envirament variable */
char    bob_name[MAX_CHAR];
char    sys_name[MAX_CHAR];
char    real_srv[MAX_NAME];
char    env_buf_bob[PATH_LENGTH];
char    env_buf_sys[PATH_LENGTH];


/* normal table descr  TB_DESCR */

TB_DESCR     *point_descr,*analog_descr,*pcount_descr;
TB_DESCR     *digctrl_descr;
TB_DESCR     *calcitem_descr;
TB_DESCR     *calcpoint_descr;
TB_DESCR     *calcpoint_calcitem_descr;
TB_DESCR     *functype_descr;
TB_DESCR     *global_descr;
TB_DESCR     *co_descr,*dv_descr,*st_descr,*kv_descr;
TB_DESCR     *un_descr,*ld_descr,*cp_descr,*nd_descr,*bus_descr;
TB_DESCR     *ln2_descr,*xf2_descr,*zbr2_descr,*eqln2_descr,*cb2_descr;
TB_DESCR     *ln_descr,*xf_descr,*zbr_descr,*eqln_descr,*cb_descr,*xfmr_descr;
TB_DESCR     *bs_descr,*bsl_descr,*chnd_descr,*top_descr,*tpl_descr,*island_descr;
TB_DESCR     *note_descr,*tagnote_descr,*mannote_descr,*nisnote_descr,*almnote_descr;
TB_DESCR     *tptrace_descr,*intrfc_descr,*lnseg_descr;
TB_DESCR     *mongrp_descr,*vlim_descr;
TB_DESCR     *ldarea_descr,*ldty_descr,*skedty_descr,*tymset_descr,*wkseg_descr;
TB_DESCR     *dayseg_descr,*tympt_descr,*abdate_descr,*sked_descr,*value_descr;
TB_DESCR     *ldal_descr,*tapty_descr;
TB_DESCR     *oo_r_intrfc_lnseg_descr;

/* relation table descr */
TB_DESCR	*point_digctrl_descr,*st_point_descr;
TB_DESCR	*co_dv_descr,*dv_st_descr,*dv_zbr_descr,*st_kv_descr,*st_xfmr_descr,*xfmr_xf_descr;
TB_DESCR	*dv_ln_descr,*kv_nd_descr,*kv_un_descr,*kv_ld_descr,*kv_cp_descr,*kv_bus_descr;
TB_DESCR	*kv_cb_descr,*nd_ln2_descr,*nd_xf2_descr,*nd_zbr2_descr,*nd_eqln2_descr;
TB_DESCR  *nd_cb2_descr;
TB_DESCR  *skedty_tymset_descr,*tymset_wkseg_descr,*tymset_dayseg_descr;
TB_DESCR  *dayseg_tympt_descr,*skedty_abdate_descr,*skedty_sked_descr;
TB_DESCR  *sked_value_descr;

POINT		*point;
DIGCTRL		*digctrl;
DIGLOCK		*diglock;
DIGPLS		*digpls;
DPLSLOCK	*dplslock;
ANALOG		*analog;
SETPNT		*setpnt;
SETLOCK		*setlock;
PULSE		*pulse;
PLSLOCK		*plslock;
PLSDIG		*plsdig;
PLSDIGLOCK	*plsdiglock;
PCOUNT		*pcount;
CO		*co;
DV		*dv;
ST		*st;
KV		*kv;
CB		*cb;
UN		*un;
XFMR		*xfmr;
XF		*xf;
LD		*ld;
CP		*cp;
BUS		*bus;
LN		*ln;
ZBR		*zbr;
NOTE		*note;
CTRLNOTE	*ctrlnote;
CTRLLOG		*ctrllog;
TAGNOTE		*tagnote;
MANNOTE		*mannote;
NISNOTE		*nisnote;
ALMNOTE		*almnote;
ND		*nd;
LN2		*ln2;
CB2		*cb2;
XF2		*xf2;
ZBR2		*zbr2;
EQLN		*eqln;
EQLN2		*eqln2;
BS		*bs;
BSL		*bsl;
TPL		*tpl;
CHND		*chnd;
ISLAND		*island;
TOP		*top;
GLOBAL		*global;
TAPTY		*tapty;
TPTRACE		*tptrace;
INTRFC		*intrfc;
LNSEG		*lnseg;
VLIM		*vlim;
MONGRP		*mongrp;
OO_R_POINT_DIGCTRL	*oo_r_point_digctrl;
OO_R_POINT_DIGPLS	*oo_r_point_digpls;
OO_R_DIGCTRL_DIGLOCK	*oo_r_digctrl_diglock;
OO_R_DIGPLS_DPLSLOCK	*oo_r_digpls_dplslock;
OO_R_ANALOG_SETPNT	*oo_r_analog_setpnt;
OO_R_ANALOG_PULSE	*oo_r_analog_pulse;
OO_R_SETPNT_SETLOCK	*oo_r_setpnt_setlock;
OO_R_PULSE_PLSLOCK 	*oo_r_pulse_plslock;
OO_R_CO_DV		*oo_r_co_dv;
OO_R_DV_ST		*oo_r_dv_st;
OO_R_DV_LN		*oo_r_dv_ln;
OO_R_DV_ZBR		*oo_r_dv_zbr;
OO_R_ST_POINT		*oo_r_st_point;
OO_R_ST_ANALOG		*oo_r_st_analog;
OO_R_ST_PCOUNT		*oo_r_st_pcount;
OO_R_ST_KV		*oo_r_st_kv;
OO_R_ST_XFMR		*oo_r_st_xfmr;
OO_R_KV_UN		*oo_r_kv_un;
OO_R_KV_CB		*oo_r_kv_cb;
OO_R_KV_LD		*oo_r_kv_ld;
OO_R_KV_CP		*oo_r_kv_cp;
OO_R_KV_BUS		*oo_r_kv_bus;
OO_R_KV_ND		*oo_r_kv_nd;
OO_R_XFMR_XF		*oo_r_xfmr_xf;
OO_R_ND_CB2		*oo_r_nd_cb2;
OO_R_ND_LN2		*oo_r_nd_ln2;
OO_R_ND_XF2		*oo_r_nd_xf2;
OO_R_ND_ZBR2		*oo_r_nd_zbr2;
OO_R_ND_EQLN2		*oo_r_nd_eqln2;
OO_R_ANALOG_PLSDIG	*oo_r_analog_plsdig;
OO_R_PLSDIG_PLSDIGLOCK	*oo_r_plsdig_plsdiglock;

SKEDTY       *skedty;
TYMSET       *tymset;
WKSEG        *wkseg;
DAYSEG       *dayseg;
TYMPT        *tympt;
ABDATE       *abdate;
SKED         *sked;
VALUE        *value;
LDTY         *ldty;
LDAREA       *ldarea;
LDAL         *ldal;
OO_R_SKEDTY_TYMSET   *oo_r_skedty_tymset;
OO_R_TYMSET_WKSEG    *oo_r_tymset_wkseg;
OO_R_TYMSET_DAYSEG   *oo_r_tymset_dayseg;
OO_R_DAYSEG_TYMPT    *oo_r_dayseg_tympt;
OO_R_SKEDTY_ABDATE   *oo_r_skedty_abdate;
OO_R_SKEDTY_SKED     *oo_r_skedty_sked;
OO_R_SKED_VALUE      *oo_r_sked_value;
OO_R_INTRFC_LNSEG    *oo_r_intrfc_lnseg;

#define _ST 1 
#define _KV 2
#define _BS 3
#define _UN 4
#define _LD 5
#define _ND 6
#define _CHND 7
#define _CP 8
#define _LN 9
#define _XF 10
#define _ZBR 11
#define _EQLN 12
#define _LN2 13
#define _XF2 14
#define _ZBR2 15
#define _EQLN2 16
#define _TOP 17
#define _TPL 18
#define _ISLAND 19
#define _BSL 20
#define _INTRFC 21

#define _OPEN_CB 21
#define _REMOVE_UN 22
#define _REMOVE_LD 23
#define _REMOVE_CP 24
#define _REMOVE_LN 25
#define _REMOVE_XFMR 26
#define _REMOVE_ZBR 27
#define _REMOVE_XF 28		//for CA ; to remove just xf not xfmr 98.2.16

#define _LDAREA 29

#define UNR 28
#define LDR 29
#define CPR 30
#define LNR 31
#define ZBRR 32
#define XFMRR 33
#define XFR 34

#define PWRFLOW_FAIL 0
#define PWRFLOW_SUCCEED 1

int lv_co,mx_co;
int lv_dv,mx_dv;
int lv_st,mx_st;
int lv_kv,mx_kv;
int lv_nd,mx_nd;
int lv_cb,mx_cb;
int lv_un,mx_un;
int lv_ld,mx_ld;
int lv_cp,mx_cp;
int lv_bus,mx_bus;
int lv_cb2,mx_cb2;
int lv_ln2,mx_ln2;
int lv_xf2,mx_xf2;
int lv_zbr2,mx_zbr2;
int lv_eqln2,mx_eqln2;
int lv_xf,mx_xf;
int lv_xfmr,mx_xfmr;
int lv_ln,mx_ln;
int lv_eqln,mx_eqln;
int lv_zbr,mx_zbr;
int lv_bs,mx_bs;
int lv_bsl,mx_bsl;
int lv_chnd,mx_chnd;
int lv_tptrace,mx_tptrace;
int lv_island,mx_island;
int lv_tpl,mx_tpl;
int lv_intrfc,mx_intrfc;
int lv_lnseg,mx_lnseg;
int lv_mongrp,mx_mongrp;
int lv_vlim,mx_vlim;
int lv_analog,mx_analog;
int lv_point,mx_point;
int lv_ldarea,mx_ldarea;
int lv_ldty,mx_ldty;
int lv_skedty,mx_skedty;
int lv_tymset,mx_tymset;
int lv_wkseg,mx_wkseg;
int lv_dayseg,mx_dayseg;
int lv_tympt,mx_tympt;
int lv_abdate,mx_abdate;
int lv_sked,mx_sked;
int lv_value,mx_value;
int lv_ldal,mx_ldal;
int lv_tapty,mx_tapty;
int lv_oo_r_intrfc_lnseg;

int ret;
int lv_aux;
//////////////////////////////////

class CHILD_POS
{
public:
        int pos;
        CHILD_POS *next;
	CHILD_POS *last;
        CHILD_POS()
        {
                pos=-1;
                next=NULL;
		last=NULL;
        };
};

int *_nn;
CHILD_POS *_pp,*_pp1,*_pp2,*_head;

CHILD_POS *dv_p,*st_p,*kv_p,*un_p,*ld_p,*cp_p,*cb_p,*bus_p,*nd_p,*ln2_p,*xf2_p,*cb2_p,*eqln2_p,*xfmr_p,*xf_p,*ln_p,*sh_p;
CHILD_POS *dv_h,*st_h,*kv_h,*un_h,*ld_h,*cp_h,*cb_h,*bus_h,*nd_h,*ln2_h,*xf2_h,*cb2_h,*eqln2_h,*xfmr_h,*xf_h,*ln_h,*sh_h;
int	  *dv_n,*st_n,*kv_n,*un_n,*ld_n,*cp_n,*cb_n,*bus_n,*nd_n,*ln2_n,*xf2_n,*cb2_n,*eqln2_n,*xfmr_n,*xf_n,*ln_n,*sh_n;

class PHTOLOG
{
public:
        int     logic_pos;
        PHTOLOG()
        {
                logic_pos = -1;
        }
};

PHTOLOG *dv_phlog,*st_phlog,*kv_phlog,*nd_phlog,*cb_phlog,*un_phlog,*ld_phlog,*cp_phlog,*cb2_phlog,*ln2_phlog,*xf2_phlog,*zbr2_phlog,*xf_phlog,*eqln2_phlog,*xfmr_phlog,*bus_phlog,*tymset_phlog,*wkseg_phlog,*dayseg_phlog,*tympt_phlog,*abdate_phlog,*sked_phlog,*value_phlog,*analog_phlog;


#define ID_LENTH 11
///////////////////////////////////

#endif
