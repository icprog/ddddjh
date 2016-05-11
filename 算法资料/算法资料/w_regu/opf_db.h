//opf_db.h by Wang GT
/////////////////////////////////

#ifndef __OPF_DB__
#define __OPF_DB__

#include<dstruct.h>
#include<oodbglobe.h>
#include<oodb.h>
#include<psbob.h>

#include"psbobserve.h"
///////////////////////////////////////////////////////

long index_STun(char *un_id);

void opf_oodbclose();
void opf_oodbinit(long prnt);
void opf_oodbopen();
void opf_oodbuse();
void opf_oodbread();

void opf_insert_capainc();
void opf_insert_flow_vio();
void opf_insert_flow_vio2();
void opf_insert_sens();
void opf_insert_uninc();
void opf_insert_xfmrinc();

void opf_update_volt();
void opf_update_flow_mon();

void opf_update_stat_info();
void opf_update_sys_state();
void opf_update_sys_state(long rec_num);
        
void read_global();
void read_lineData();
void read_nodeData();
void read_voltData();
void read_xfmrData();
void read_genrData();
void read_capaData();
void read_groupData();
void read_taptyData();

void OPF_read_rtdb(long prnt);
////////////////////////////////////////////////

#endif
