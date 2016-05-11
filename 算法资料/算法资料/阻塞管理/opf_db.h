//opf_db.h by Wang GT
/////////////////////////////////

#ifndef __OPF_DB__
#define __OPF_DB__

#include"opf.h"
#include<opfdb.h>
///////////////////////////////////////////////////////

template<class TYPE>
long posiRecord(TYPE *tb,char *id,long lv_tb);

double getPowr(PRIC *pric,int j);
double getPric(PRIC *pric,int j);

double setPowr(double lower,double upper,long pntNum,long i);
double setPric(double powr,double flt);

void OPF_initDB(char sysName[],char bobName[],char srvName[],long prnt);
void OPF_readDB(char sysName[],char bobName[],char srvName[],long prnt);
//////////////////////////////////////////////////////////////////////////////////////////

#endif
