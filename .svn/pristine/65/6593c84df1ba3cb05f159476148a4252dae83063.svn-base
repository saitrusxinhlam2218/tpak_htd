/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: newui.c,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1997/08/09 17:54:17 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/common/newui.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: newui.c,v 1.2 1997/08/09 17:54:17 taxiadm Exp $";

/**************************************************************************
*
*   Newui.c
*   Tom Kee
*   Modification History
*   ====================
*   12.27.89    Created
*
***************************************************************************/
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <grp.h>
#include <pwd.h>
#include "msgq.h"
#include "process.h"
#include "msgcomms.h"
#include "requests.h"

int no_msgq = 0;


ManagerUI()
{
	NewUI("MANAGER",MANAGERUI);
}

UserUI()
{
	NewUI("USER",USERUI);
}

NewUI(name,utype)
char * name;
int utype;
{
	struct passwd * prec;
	struct group * grec;
	PROCESSHDR newid;
	REQUESTDEF sendbuf;
	struct msgbuf msgqrec;
	
	
        if (OpenMsgQ()) 
	{
		no_msgq = 1;
		return;
	}

	prec=getpwuid(getuid());
	grec=getgrgid(getgid());
	strcpy(newid.p_name, name);
	if (prec) strcpy(newid.p_path, prec->pw_name);
	if (grec) strcpy(newid.p_argv, grec->gr_name);
	newid.p_pid=getpid();
	newid.p_ppid=getppid();
	sendbuf.r_rtype=(char) utype;
	memcpy(sendbuf.r_buf, &newid, sizeof(PROCESSHDR));
	sendbuf.r_pid=getpid();
	sendbuf.r_mtype=1L;
	if (msgsnd(msgqid,&sendbuf,sizeof(REQUESTDEF),IPC_NOWAIT)==-1)
		perror("NewUI (msgsend)");
	
}

ExitUI()
{

        REQUESTDEF sendbuf;

	if ( no_msgq ) return;
        sendbuf.r_mtype=1L;
        sendbuf.r_pid=getpid();
        sendbuf.r_rtype=EXITUI;
        if (msgsnd(msgqid,&sendbuf,sizeof(REQUESTDEF),IPC_NOWAIT)==-1)
                perror("ExitUI (msgsend)");
 
}
