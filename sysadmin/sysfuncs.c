/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: sysfuncs.c,v $
*  @(#)  $Revision: 1.4 $
*  @(#)  $Date: 1998/07/28 21:28:47 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/sysadmin/sysfuncs.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: sysfuncs.c,v 1.4 1998/07/28 21:28:47 taxiadm Exp $";

/**************************************************************************
*
*   sysfuncs.c
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
#include <sys/signal.h>
#include <curses.h>
#include <errno.h>
#include "comm_strdefs.h"
#include "smgmt_strdefs.h"
#include "machdep.h"
#include "language.h"
#include "process.h"
#include "msgq.h"
#include "msgcomms.h"
#include "requests.h"
#include "msgstrings.h"
#include "path.h"

char *blanks="                         ";
#define max_filenames 38
int ps_sw = 1;
char inbuf[10];
extern char getret();
extern WINDOW *wStatus;
extern WINDOW *wMain;
extern int bStatusText;
WINDOW *wProcess;
int    proc_scr_line = 3;

char * ctime();
int monitor_pid=0;
int pstat_dispatch = 0;
int pstat_async = 0;
int pstat_autobook = 0;
int pstat_pi = 0;
int pstat_gps_proc = 0;
int pstat_ccp = 0;
int pstat_autocall = 0;

int get_monitor_pid()
{
	return(monitor_pid);
}
int set_monitor_pid(pid)
int pid;
{
	monitor_pid=pid;
}



PrintMsg(m)
REQUESTDEF * m;
{
      printf(" r_mtype %ld\n" ,m->r_mtype );
      printf(" r_rtype %d\n",  m->r_rtype);
      printf(" r_pid %d\n", m->r_pid);
      printf(" r_seq %d\n", m->r_seq);
      printf(" r_size %d\n", m->r_size);
      printf(" r_buf %s\n", m->r_buf);
      getchar();
 
}

ClearMsgQ()
{
	REQUESTDEF msgqx;

       while (RecvMsgQ(0, &msgqx) == 1)
       {
               PrintMsg(&msgqx);

      } 
}


PrintQStats(msgqstat)
struct msqid_ds * msgqstat;
{
	struct msqid_ds m;

	memcpy(&m,msgqstat,sizeof(struct msqid_ds));

	clear_scrn();
        printf("\n\t msgqstat->msg_perm.cuid \t%d\n", m.msg_perm.cuid );
        printf("\t msgqstat->msg_perm.cgid \t%d\n", m.msg_perm.cgid );
        printf("\t msgqstat->msg_perm.uid \t%d\n", m.msg_perm.uid );
        printf("\t msgqstat->msg_perm.gid \t%d\n", m.msg_perm.gid );
        printf("\t msgqstat->msg_perm.mode \t%X\n", m.msg_perm.mode );
         
        printf("\t msgqstat->msg_cbytes \t%d\n", m.msg_cbytes );     /* current # bytes on q */
        printf("\t msgqstat->msg_qnum \t%d\n", m.msg_qnum );       /* # of messages on q */
        printf("\t msgqstat->msg_qbytes \t%d\n", m.msg_qbytes );     /* max # of bytes on q */
        printf("\t msgqstat->msg_lspid \t%d\n", m.msg_lspid );      /* pid of last msgsnd */
        printf("\t msgqstat->msg_lrpid \t%d\n", m.msg_lrpid);      /* pid of last msgrcv */
        printf("\t msgqstat->msg_stime \t%s", ctime(&m.msg_stime) );      /* last msgsnd time */
        printf("\t msgqstat->msg_rtime \t%s", ctime(&m.msg_rtime) );      /* last msgrcv time */
        printf("\t msgqstat->msg_ctime \t%s", ctime(&m.msg_ctime) );      /* last change time */
	printf(catgets(SMGMT_m_catd, SMGMT_SET, SMGMT_24, "Press <return> key ..."));
	getret();

}

ShowMDIProcess(p)
PROCESSHDR * p;
{

	PROCESSHDR x;
	char msg[40];
	char *strchr();
	char buffer[30];

	memcpy(&x,p,sizeof(PROCESSHDR));


        printf("%-14s ",x.p_name);
        printf("%-20s ",x.p_path);
        printf("%-20s ",x.p_argv);
        printf("%-10ld ",x.p_pid);
        printf("%-10ld ",x.p_ppid);
        sprintf(msg,"%d (%d) ",x.p_reloadcount,x.p_supp_reloads); 
        printf("%-10s ",msg);
	get_status_str(&x,msg);
	
        printf("%-10s ",msg);
        strcpy(buffer,ctime(&x.p_lasttime));
	*strchr(buffer , '\n')=NULL;
	printf("%-28s \n",buffer);
}



get_status_str(x,msgx)
PROCESSHDR  *x;
char * msgx;
{
	char * msg;

	switch (x->p_status)
	{
	case     PROCESS_UP          :
			msg=catgets(SMGMT_m_catd, SMGMT_SET, SMGMT_25, "Up");
			break;
	case     PROCESS_DOWN        :
			msg=catgets(SMGMT_m_catd, SMGMT_SET, SMGMT_26, "Down");
			break;
	case     PROCESS_SUSPENDED   :
			msg=catgets(SMGMT_m_catd, SMGMT_SET, SMGMT_27, "Suspended");
			break;
	case     PROCESS_IGNORE      :
			msg=catgets(SMGMT_m_catd, SMGMT_SET, SMGMT_28, "Ignore");
			break;
	case     PROCESS_STOPPED     :
			msg=catgets(SMGMT_m_catd, SMGMT_SET, SMGMT_29, "Stopped");
			break;
	case     PROCESS_MANAGER     :
			msg=catgets(SMGMT_m_catd, SMGMT_SET, SMGMT_30, "Manager");
			break;
	case     PROCESS_REDUNDANT   :
			msg=catgets(SMGMT_m_catd, SMGMT_SET, SMGMT_31, "Secondary");
			break;
	case     PROCESS_USER_LOAD   :
			msg=catgets(SMGMT_m_catd, SMGMT_SET, SMGMT_32, "User Load");
			break;
	case     PROCESS_NOLOOK	     :
			msg=catgets(SMGMT_m_catd, SMGMT_SET, SMGMT_33, "No Look");
			break;
	default:	
		msg=catgets(SMGMT_m_catd, SMGMT_SET, SMGMT_34, "Unkown");
		break;

	}
	strcpy(msgx,msg);
}




ShowUserProcess(p)
PROCESSHDR * p;
{

	PROCESSHDR x;
	char msg[40];
	char *strchr();
	char buffer[30];

	memcpy(&x,p,sizeof(PROCESSHDR));


	get_status_str(&x,msg);
        printf("%-10s ",msg);
	strcpy(buffer,ctime(&x.p_lasttime));
	buffer[19]='\0';

        mvwprintw( wProcess, proc_scr_line, 10, "%-14s %-10ld %-10s %-28s",x.p_name, x.p_pid, msg, buffer);
	++proc_scr_line;
}

GetProcesses(count,sw)
int count;
int sw;
{
	int i=0;
	int len;
	REQUESTDEF *recvbuf;
	char       *hRecv;
	char c;


	hRecv = (char *)malloc( sizeof(REQUESTDEF) );
	bzero( hRecv, sizeof(REQUESTDEF) );
	
	wProcess = newwin(  12, 88, 4, 25 );
	box( wProcess, 0, 0 );
	
	if (sw)
	{
	  mvwprintw( wProcess, 1, 10, "%-14s %-10s %-10s %-28s",catgets(SMGMT_m_catd, SMGMT_SET, SMGMT_35, "PROCESS"),
		    catgets(SMGMT_m_catd, SMGMT_SET, SMGMT_36, "PID"),
		    catgets(SMGMT_m_catd, SMGMT_SET, SMGMT_37, "STATUS"),
		    catgets(SMGMT_m_catd, SMGMT_SET, SMGMT_38, "LAST CONTACT TIME"));
	  mvwprintw( wProcess, 2, 10, "%-14s %-10s %-10s %-28s","-------","---","------",  "-----------------");
	}
	else 
	{
		printf("\n%-14s %-20s %-10s %-10s %-10s %-10s %-10s %-28s\n",
				catgets(SMGMT_m_catd, SMGMT_SET, SMGMT_35, "PROCESS"),
				catgets(SMGMT_m_catd, SMGMT_SET, SMGMT_36, "WHO"),
				catgets(SMGMT_m_catd, SMGMT_SET, SMGMT_39, "GROUP"),
				catgets(SMGMT_m_catd, SMGMT_SET, SMGMT_40, "PID"),
				catgets(SMGMT_m_catd, SMGMT_SET, SMGMT_41, "PPID"),
				catgets(SMGMT_m_catd, SMGMT_SET, SMGMT_42, "LOADED"),
				catgets(SMGMT_m_catd, SMGMT_SET, SMGMT_37, "STATUS"),
				catgets(SMGMT_m_catd, SMGMT_SET, SMGMT_38, "LAST CONTACT TIME"));
		printf("%-14s %-20s %-20s %-10s %-10s %-10s %-10s %-28s\n",
				"-------","---","-----","---","----","------","------","-----------------");
	}
	do 
	{

   	      if ((len = msgrcv(msgqid, hRecv, sizeof(REQUESTDEF), getourpid(), IPC_NOWAIT)) == -1)
    	      {  
    		    if (errno==ENOMSG) continue;
	            else return(-1);
    	      }
   	      else
   	      {
         	if (len)
         	{
		  recvbuf = (REQUESTDEF *)hRecv;
           		 if (recvbuf->r_rtype==APROCESS)
			 {
				if (sw)
					ShowUserProcess((PROCESSHDR * )recvbuf->r_buf);
				else
					ShowMDIProcess((PROCESSHDR * )recvbuf->r_buf);
				i++;
			 }
			 else ProcessResponds(recvbuf);
         	}
 
              }	
	} while (i < count);
	proc_scr_line = 3;
        mvwprintw( wProcess, 10, 40, "%s", catgets(SMGMT_m_catd, SMGMT_SET, SMGMT_43, "Press <return> to continue ....") );
	wrefresh( wProcess );
	c = wgetkey( wProcess );

	return(0);
 
}
 

DisplayMsg(msgno)
int msgno;
{
	char * msg;
	char msg_store[134];

	msg_store[0] = '\0';

	switch (msgno)
	{
		case WARMSTARTED : msg = catgets(SMGMT_m_catd, SMGMT_SET, SMGMT_44, "TaxiPak is warm started ....");break;
		case COLDSTARTED : msg = catgets(SMGMT_m_catd, SMGMT_SET, SMGMT_45, "TaxiPak is cold started ....");break;
		case ALREADYRUNNING : msg = catgets(SMGMT_m_catd, SMGMT_SET, SMGMT_46, "TaxiPak is already running, stop the system to reload");break;
		case STOPPED : msg = catgets(SMGMT_m_catd, SMGMT_SET, SMGMT_47, "TaxiPak is stopped ... ");break;
		case MIRRORSTARTED : msg = catgets(SMGMT_m_catd, SMGMT_SET, SMGMT_48, "Mirror is started ......");break;
		case MIRRORNOSTART : msg = catgets(SMGMT_m_catd, SMGMT_SET, SMGMT_49, "Mirror is NOT started ......");break;
		case MIRRORSTOPPED : msg = catgets(SMGMT_m_catd, SMGMT_SET, SMGMT_50, "Mirror has been stopped");break;
		case NOBACKUP : msg = catgets(SMGMT_m_catd, SMGMT_SET, SMGMT_51, "There is NO back up machine present or connected .....");break;
		case YESBACKUP : msg = catgets(SMGMT_m_catd, SMGMT_SET, SMGMT_52, "A back up machine IS present and connected .....");break;
		case STARTFAILED : msg = catgets(SMGMT_m_catd, SMGMT_SET, SMGMT_59, "TaxiPak startup failed .....");break;
		case CANTRUNONBACKUP : msg = catgets(SMGMT_m_catd, SMGMT_SET, SMGMT_60, "Can NOT start TaxiPak on backup machine .....");
		case GEO_STARTED: msg = "GEOTAXI STARTED"; break;
		case GEO_ALREADY_RUNNING: msg = "GEOTAXI ALREADY RUNNING"; break;
		case GEO_STOPPED: msg = "GEOTAXI STOPPED"; break;
		case TAO_ALREADY_RUNNING: msg = "TAO ALREADY RUNNING"; break;
		case TAO_STARTED: msg = "TAO STARTED"; break;
		case TAO_STOPPED: msg = "TAO STOPPED"; break;
		case TCB_STARTED: msg = "TCB STARTED"; break;
		case TCB_ALREADY_RUNNING: msg = "TCB ALREADY RUNNING"; break;
		case TCB_STOPPED: msg = "TCB STOPPED"; break;
		case ASYNC_ALREADY_RUNNING: msg = "ASYNC ALREADY RUNNING"; break;
		case ASYNC_STARTED: msg = "ASYNC STARTED"; break;
		case ASYNC_STOPPED: msg = "ASYNC STOPPED"; break;
		case DISPATCH_ALREADY_RUNNING: msg = "DISPATCH ALREADY RUNNING";break;
		case DISPATCH_STARTED: msg = "DISPATCH STARTED";break;
		case DISPATCH_STOPPED: msg = "DISPATCH STOPPED";break;
		case AB_ALREADY_RUNNING: msg = "AUTOBOOK ALREADY RUNNING";break;
		case AB_STARTED: msg = "AUTOBOOK STARTED"; break;
		case AB_STOPPED: msg = "AUTOBOOK STOPPED"; break;
		case AUTOCALL_ALREADY_RUNNING: msg = "AUTOCALL ALREADY RUNNING";break;
		case AUTOCALL_STARTED: msg = "AUTOCALL STARTED"; break;
		case AUTOCALL_STOPPED: msg = "AUTOCALL STOPPED"; break;		  
		case GPS_PROC_ALREADY_RUNNING: msg = "GPS PROC ALREADY RUNNING";break;
		case GPS_PROC_STARTED: msg = "GPS PROC STARTED";break;
		case GPS_PROC_STOPPED: msg = "GPS_PROC_STOPPED";break;
		case PI_STARTED: msg = "PI STARTED";break;
		case PI_STOPPED: msg = "PI STOPPED"; break;
		case PI_ALREADY_RUNNING: msg = "PI ALREADY RUNNING"; break;
		case CCP_STARTED: msg = "CCP STARTED"; break;
		case CCP_STOPPED: msg = "CCP STOPPED"; break;
		case CCP_ALREADY_RUNNING: msg = "CCP ALREADY RUNNING"; break;
		case PSTAT_DISPATCH_UP: msg = ""; pstat_dispatch = 1; break;
		case PSTAT_DISPATCH_DOWN: msg =""; pstat_dispatch = 0; break;
		case PSTAT_ASYNC_UP: msg = ""; pstat_async = 1; break;
		case PSTAT_ASYNC_DOWN: msg = ""; pstat_async = 0; break;
		case PSTAT_AUTOBOOK_UP: msg = ""; pstat_autobook = 1; break;
		case PSTAT_AUTOBOOK_DOWN: msg = ""; pstat_autobook = 0; break;
		case PSTAT_AUTOCALL_UP: msg = ""; pstat_autocall = 1; break;
		case PSTAT_AUTOCALL_DOWN: msg = ""; pstat_autocall = 0; break;
		case PSTAT_GPS_PROC_UP: msg = ""; pstat_gps_proc = 1; break;
		case PSTAT_GPS_PROC_DOWN: msg = ""; pstat_gps_proc = 0; break;
		case PSTAT_PI_UP: msg = ""; pstat_pi = 1; break;
		case PSTAT_PI_DOWN: msg = ""; pstat_pi = 0; break;
		case PSTAT_CCP_UP: msg = ""; pstat_ccp = 1; break;
		case PSTAT_CCP_DOWN: msg = ""; pstat_ccp = 0; break;
		  
		default : break;

	}
	if ( strlen( msg ) )
	    {
	      wclear( wStatus );
	      mvwprintw( wStatus, 1, 5, "%s", msg );
	      wrefresh( wStatus );
	      bStatusText = 1;
	    }
	PutMenu();
}

set_sw(x)
int x;
{
 
	ps_sw = x;

}	

int get_sw()
{
	return(ps_sw);
}

ProcessResponds(respptr)
REQUESTDEF * respptr;
{

        PROCESSHDR * p;
	int answer;
        int reply_ok;
        char reply[60];         /* reply buffer */
	int done = 0;


 
        switch (respptr->r_rtype)
        {
                case QSTATUS:
                             PrintQStats((struct msqid_ds * )respptr->r_buf);
			     break;
		case NUMPROCESSES:
			     GetProcesses(respptr->r_seq,get_sw());
			     break;
		case MANAGERRETURN:
			     monitor_pid=respptr->r_pid;
			     break;
		case MSGDISPLAY:
			     DisplayMsg(respptr->r_seq);
			     break;
		case OKSYNC:
			      system("/usr/taxi/bin/syncdb");
			      break;


		default : break;
	}
}
               
int CheckResponses()
{
 
    int len;
    REQUESTDEF recvbuf;
 
 
    if ((len = msgrcv(msgqid, &recvbuf, sizeof(REQUESTDEF), getpid(), IPC_NOWAIT)) == -1)
    {
        if (errno==ENOMSG) return(0);
        else 
	{
                if (errno == EINVAL)
                {
                        if (OpenMsgQ()) 
			{
				printf("\n\n\n%s\n", catgets(SMGMT_m_catd, SMGMT_SET, SMGMT_58, "Monitor has gone away...."));
				exit(1);
			}
			else return(0);
                }
                else
		{
			perror("CheckResponse");
			return(-1);
		}
	}
    }
    else
    {   
         if (len)
         {
            ProcessResponds(&recvbuf);
	    if ( (recvbuf.r_rtype == MANAGERRETURN) ||
		 (recvbuf.r_seq == PSTAT_DISPATCH_UP) ||
		 (recvbuf.r_seq == PSTAT_DISPATCH_DOWN) ||
		 (recvbuf.r_seq == PSTAT_ASYNC_UP) ||
		 (recvbuf.r_seq == PSTAT_ASYNC_DOWN) ||
		 (recvbuf.r_seq == PSTAT_AUTOBOOK_UP) ||
		 (recvbuf.r_seq == PSTAT_AUTOBOOK_DOWN) ||
		 (recvbuf.r_seq == PSTAT_GPS_PROC_UP) ||
		 (recvbuf.r_seq == PSTAT_GPS_PROC_DOWN) ||
		 (recvbuf.r_seq == PSTAT_PI_UP) ||
		 (recvbuf.r_seq == PSTAT_PI_DOWN) ||
		 (recvbuf.r_seq == PSTAT_CCP_UP) ||
		 (recvbuf.r_seq == PSTAT_AUTOCALL_UP) ||
		 (recvbuf.r_seq == PSTAT_AUTOCALL_DOWN) ||
		 (recvbuf.r_seq == PSTAT_CCP_DOWN) )
	      return (-1);
         }
 
         return(1);
    }
 
}


SendRequest(rtype)
char rtype;
{

	REQUESTDEF sendbuf;
	int ss_size = sizeof(REQUESTDEF) - MAXREQ;

        sendbuf.r_mtype=1L;
	sendbuf.r_pid=getourpid();
        sendbuf.r_rtype=rtype;
        sendbuf.r_size=0;
        sendbuf.r_seq=0;
	
        if (msgsnd(msgqid,&sendbuf,ss_size,IPC_NOWAIT)==-1) perror("SendProcesses (msgsend)");
/*
	if (kill(monitor_pid,SIGUSR1) == -1) perror("SendRequest (kill)");
 */
}


