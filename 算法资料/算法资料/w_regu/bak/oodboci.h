//oodboci.h by Zhang Rui

#ifndef  OODBOCI_H
#define  OODBOCI_H
//////////////////////////////////////////////////////////

#include <iostream.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <oci.h>

#define MAX_COLS	100
#define MAX_COLUMN	100
//#define MAX_NAME        20
#define OCI_FAIL        -1 
#define OCI_SUCCEED     1 
#define OCI_STRING_LEN  50
#define OCI_NUMBER_LEN  22
//////////////////////////////////////////////////////////

class _list_bind
{
	public:
		char          *bindbuf[MAX_COLS];
		_list_bind    *next;

		_list_bind()
		{
			next = NULL;
			for(int i=0;i<MAX_COLS;i++)
			bindbuf[i] = NULL;
		}    
};

class OCI_TOOLS
{
	protected:
		sword		status;
		sword		ret;

		OCIEnv		*envhp;
		OCIError	*errhp;
		OCIServer	*srvhp;
		OCISvcCtx	*svchp;
		OCISession	*authp;
		OCIStmt		*stmthp;
		OCIBind		*bindhp;//
		OCIParam	*colhp;

	public:
		OCI_TOOLS();
		~OCI_TOOLS();
		OCIDefine *defhp[MAX_COLS];
                int ORA_Initial();
		int ORA_AttachServer(char *server_name);
		int ORA_SessionBegin(char *user_name,char *user_passwd);
		int ORA_Logon(char *user_name,char *user_passwd);
		int Connect(char *user_name, char *user_passwd,char *server_name);

		int ORA_Logoff();
		int ORA_SessionEnd();
		int ORA_Disconnect();
		int ORA_HandleFree();
                int Disconnect();

                int ORA_BindWithReturnArray(char *cmdbuf,void *data_addr); 
		int ORA_BindWithReturn(_list_bind **first_bind,char *cmdbuf);
		int ORA_BindNoReturn(char *cmdbuf);
		int ORA_ExecProcedure(char *cmdbuf,char *,char *,char *);

		void ORA_flush();
		void error_proc(dvoid *errhp,sword status);
		int Free_list_bind(_list_bind *first_bind);
};

/////////////////////////////////////////////////////////////////

#endif
