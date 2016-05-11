//ociserve.h by Wang GT
/////////////////////////////////////////////

#ifndef __OCISERVE_H__
#define __OCISERVE_H__

#include"oodboci.h"
////////////////////////////////////////

long year;
long month;
long day;
long sd;

long recNum;
//////////////////////////////////////////

char cmdbuf[256];

char numbuf[1000][OCI_NUMBER_LEN];
char numbuf2[1000][OCI_NUMBER_LEN];

char strbuf[500][OCI_STRING_LEN];
char strbuf2[500][OCI_STRING_LEN];

char oraServer[20];
char userName[20];
char passWord[20];

class OCI_TOOLS	ociTools;
////////////////////////////////////////

#endif
