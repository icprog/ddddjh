//oracle.h by Wang GT

#ifndef __ORACLE_H__
#define __ORACLE_H__

#include"common.h"
#include"ociserve.h"

#include"opf.h"
#include"opf_global.h"

#define LOSS_RATIO_DFLT 0.03
#define MCP_DFLT         200
////////////////////////////////////////////////////////////

long saveSched(struct genrstr *genrData,char *id,double sched,long prnt);

void consoleMsg(char *prog_id,char *msg);
void consoleMsg(char *prog_id,char *prog_descr,char *msg,char *msg_xpln);

void conn_oraServer(char *_oraServer,long prnt);
void disconn_oraServer();

void load_distr(struct genrstr *_genrData,double sched);
void load_distr(struct nodestr *nodeData,double wload);

void makeSubChain(struct genrstr *&sub_genrData,struct genrstr *genrData,char *st);

void OPF_read_oracle(char *date_str,long sd,long prnt);
void OPF_write_oracle(char *date_str,long sd,long prnt);

void read_global(long prnt);
void read_group(long prnt);

void read_group_line(struct groupstr *grp,char *grp_id,long prnt);
void read_group_mode(struct groupstr *grp,char *grp_id,long prnt);

void read_loadfor(long prnt);
void read_mode_cond(struct groupstr *grp,long im,long k);
 
void read_price(long prnt);
void read_sched(long prnt);

void recovSubChain(struct genrstr *genrData,struct genrstr *sub_genrData);

void write_corrpolicy(long prnt);
void write_itr(long prnt);
void write_sns(long prnt);
void write_synchoff(long prnt);
void write_vioflow(long prnt);
///////////////////////////////////////////////////////////

#endif
