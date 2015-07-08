/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: monitor.c,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1997/08/09 18:08:14 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/sysadmin/monitor.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: monitor.c,v 1.2 1997/08/09 18:08:14 taxiadm Exp $";



#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include "process.h"
#include "msgq.h"
#include "requests.h"


int terminated=0;
int once=0;
int delay = 2;
int SecondaryConnected = 0;
int Primary = 0;
int Secondary = 0;
int r;
int sd;
int sleep_time = 60;
int Check = 0;
int AutoLoad = 0;
int OverRide = 0;
int NoNetWork = 0;
int min_duration;
int max_duration =1;
long wait_beg_time;
int trace=0;
int ReLoaded = 0;
int NoTerminate=0;
int QueryPrim = 1;
int WaitAfterSpawn = 7;


set_options(argc,argv)
int argc;
char **argv;
{
       int c;
       extern int optind, opterr;
       extern char *optarg;

       while ((c = getopt (argc, argv, "ahl:nots:w:")) != EOF)
           switch (c) 
	   {
                case 'a':
			AutoLoad = 1;		/* Automatically load executables */
                        break;
                case 't':
			trace = 1;		/* write log to ../run/monitor.trace  */
                        break;
                case 'o':
			OverRide = 1;
                        break;
		case  'n':
			NoNetWork = 1;
                        break;
                case 'l':
    			WaitAfterSpawn=atoi(optarg);
			if (!WaitAfterSpawn) WaitAfterSpawn = 10;
                        break;
                case 'w':
    			delay=atoi(optarg);
			if (!delay) delay = 2;
                        break;
		case 's':
			sleep_time = atoi(optarg);
			if (!sleep_time) sleep_time = 60;
			else QueryPrim = 0;
			
		 	break;	
		case 'h':
		default : 
               		fprintf(stderr, "\nusage: monitor -ahlostw [options] [files] . . . \n");
                	fprintf(stderr,"-a \tautomatically start executables\n");
                	fprintf(stderr,"-h \tthis help message\n");
                	fprintf(stderr,"-l \tthe time to wait for a process to load on a system start in seconds (default 10)\n");
                	fprintf(stderr,"-o \toverride checking for secondary machine and start as primary\n");
                	fprintf(stderr,"-n \tsingle machine configuration with no network comms to backup\n");
                	fprintf(stderr,"-s \tsleep threshold for checking for responses from processes in seconds (default 60)\n");
                        fprintf(stderr,"-t \tgenerate trace file /usr/taxi/run/monitor.trace\n");
                	fprintf(stderr,"-w \ta weighted sleep while waiting for a connect to primary on startup in seconds (default 2)\n");
                	exit (2);
			break;
           }
}


int get_slices(duration)
int duration;
{
	return(	sleep_time/duration);
}

int set_sleep_time(interval)
int interval;
{
	if (interval)
		sleep_time = interval;
}


int get_sleep_time(intervals)
int intervals;
{
	if (intervals)
		return(sleep_time/intervals);
	else 
		return(sleep_time);
}


setCheck()
{
	Check=1;
}

resetCheck()
{
	Check=0;
}
int isSecondaryConnected()
{
	return(SecondaryConnected);
}

terminate()
{

    monitor_trace(__FILE__,__LINE__,"Terminate");
    set_system_stopped();
    terminated=1;
    if (!NoNetWork)
    {
    	CloseSocket();
    	SignalSecondary();
    }
    DestroyProcesses();
    CutOver();
    RemoveMsgQ();
    MVCore();
    exit(0);
}
 

get_signal(sig)
int sig;
{
    int death();
    int timeout();
    int CheckRequests();
    int CheckReqSignal();	
    int UnInitialize();
    extern int ProcessStatusCalls;


	if (sig!=SIGALRM && !(sig==SIGCHLD && ProcessStatusCalls))
	    monitor_trace(__FILE__,__LINE__,"Get signal has caught signal  = %d",sig);

	switch (sig)
	{
	    case SIGCHLD: death(); break;
	    case SIGBUS: terminated=1; break;
	    case SIGSTOP: terminated=1; break;
	    case SIGTERM: terminate(); break;
	    case SIGINT: terminated=1; break;
	    case SIGSEGV: terminated=1; break;
	    case SIGALRM: timeout(); break;
	    /* case SIGUSR1: CheckReqSignal(); */
	    case SIGQUIT: UnInitialize();break;
            default : break;
	}

}

set_signal_init(sig_handler)
int (*sig_handler)();
{
 
        signal(SIGHUP, SIG_IGN);                /* 01  Hang up */
        signal(SIGINT, sig_handler);                /* 02  Interrupt */
        signal(SIGQUIT, sig_handler);                /* 02  Interrupt */
        signal(SIGILL, sig_handler);            /* 04  Illegal instruction */
        signal(SIGTRAP, SIG_DFL);               /* 05  Trace Trap */
        signal(SIGIOT, SIG_DFL);                /* 06  IOT instruction */
        signal(SIGEMT, SIG_DFL);                /* 07  EMT instruction */
        signal(SIGFPE, SIG_DFL);                /* 08  Floating Point Exception */
        signal(SIGKILL, SIG_DFL);               /* 09  Kill, can't be caught */
        signal(SIGBUS, sig_handler);            /* 10  Bus error */
        signal(SIGSEGV, sig_handler);           /* 11  Segmentation Violation */
        signal(SIGSYS, SIG_DFL);                /* 12  bad arg System call */
        signal(SIGPIPE, sig_handler);           /* 13  Pipe error */
        signal(SIGALRM, sig_handler);               /* 14  Alarm */
        signal(SIGTERM, sig_handler);           /* 15  software Terminate */
        signal(SIGUSR1, sig_handler);           /* 16  User defined signal 1 */
        signal(SIGUSR2, sig_handler);           /* 16  User defined signal 1 */
        signal(SIGCLD, sig_handler);           /* 16  User defined signal 1 */
}

/*
set_sigs()
{
    int death();
    int timeout();
    int CheckRequests();	
    int CheckReqSignal();	
    int UnInitialize();

    signal(SIGCHLD, death);
    signal(SIGBUS, terminate);
    signal(SIGSTOP, terminate);
    signal(SIGTERM, terminate);
    signal(SIGINT, terminate);
    signal(SIGSEGV, terminate);
    signal(SIGALRM, timeout);
    signal(SIGUSR1, CheckReqSignal);
    signal(SIGUSR2, UnInitialize);
}
*/ 
reset_sigs()
{
    signal(SIGBUS,  SIG_DFL);
    signal(SIGSEGV,  SIG_DFL);
    signal(SIGALRM,  SIG_DFL);
    signal(SIGSTOP, SIG_DFL);
    signal(SIGTERM, SIG_DFL);
    signal(SIGINT, SIG_DFL);
    signal(SIGUSR1,  SIG_DFL);

}
  			 
DetermineMachineGen()
{

    Primary= CanIBePrimary(&sd,delay);

    if (!Primary)
    {
    	lat_rate(75);
        Secondary=1;
        system("/usr/taxi/etc/umountbear"); 	
        SyncTime(sd);
	if (QueryPrim) GetPrimeSleepTime(sd);
        StartServer("secondary");
	StartRedundantProcesses();
    	monitor_trace(__FILE__,__LINE__,"Monitor started as secondary backup machine");
    }
    while (!Primary)
    {
        if (!(Primary=TestConnectRequest()))
 	       Primary=PulsePrimary(sd);

    }
    if (Secondary)
    {
        system("/usr/taxi/etc/mountbear"); 	
        AutoLoad = 1;
	Secondary=0;
	KillRedundantProcesses();
        CloseConnection(sd);
        CloseSocket();
    }

}


Initialize()
{

    FILE * fp, * fopen();

    setuid(0);           /* set id to ROOT */
    setpgrp();
    MVCore();
    if (kvm_process_count("monitor") > 1) 
    {
    	monitor_trace(__FILE__,__LINE__,"Monitor already running as pid %d.\n",kvm_process_by_name("monitor"));
    	printf("Monitor already running as pid %d.\n",kvm_process_by_name("monitor"));
	exit(0);
    }
    if ((fp=fopen("/usr/taxi/run/monitor.pid","w"))!=NULL)
    {
	fprintf(fp,"%d",getpid());
	fclose(fp);
    }
    system("/usr/taxi/etc/startlat"); 	
    set_signal_init(get_signal);
    CreateMsgQ();
    CreateProcessList();
    GetProcessData("/usr/taxi/etc/monitor.dat",0);
/* 
 * set minimium duration life for a processes to stay loaded 
 */

    min_duration = get_sleep_time(1) + 10;	

    if (!OverRide) 
	DetermineMachineGen();

    if (!NoNetWork)
    {
	StartServer("primary");
    	lat_rate(255);
    }
    monitor_trace(__FILE__,__LINE__,"Monitor falling through as primary machine");
} 

int get_primary()
{
	return(Primary);
}


UnInitialize()
{
    monitor_trace(__FILE__,__LINE__,"UnInitialize");
    set_system_stopped();	
    if (!NoNetWork)
    {
    	CloseSocket();
    	SignalSecondary();
    	CloseSocket(sd);
    }
    DestroyProcesses();
    RemoveMsgQ();
    MVCore();
    exit(0);
}

 
/**************************************************************************** 
        death() 
        Prevent zombie childern 
*****************************************************************************/
 
death() 
{
	int pid;
        union wait status; 
	struct rusage usage;
	extern int ProcessStatusCalls;

	if(ProcessStatusCalls) return;   /* ignore the killing if provoked by system("ps") in ps.c*/

        while ((pid=wait3(&status,WNOHANG,&usage)) > 0) 
	{
		monitor_trace(__FILE__,__LINE__,"Death has caught pid %d",pid);
		MarkProcessDown(pid);
		if (system_is_running())
		{
			if (terminated) return;
			monitor_trace(__FILE__,__LINE__,"Ready to restart dead pid %d",pid);
			MVCore();
			BlockSignals();
			RestartProcess(pid);
			ResetSignals();
		}
	}
}
 
WaitRest()
{

        PROCESSHDR *p=pheader;
	int slice;
	int i;

	wait_beg_time = time(0);
	if (NoNetWork)
	{
		
    		monitor_trace(__FILE__,__LINE__,"Wait Rest no network sleep.");
		i = 0;
		slice = get_slices(max_duration);
                do
                { 
                        sleep(get_sleep_time(slice));
                        CheckRequests();
                        i++;
                } while (i < slice);
		return;

	}
        if (SecondaryConnected)
        {
		i = 0;
    		monitor_trace(__FILE__,__LINE__,"Wait Rest says Secondary Connected");
		slice = get_slices(max_duration);
		do 
		{
                	sleep(get_sleep_time(slice));
			CheckRequests();
                	if (CheckHandle (r)) {
				SecondaryConnected = 0;
	           		while (p->p_next) {    
	                		p=p->p_next;
       	                		if (!strcmp(p->p_name,"pulse"))
             	  			{ 
						ProcessDisplay(p);
						kill(p->p_pid, SIGKILL);
						
      						if (kill(p->p_pid,0)!=-1) {
							monitor_trace(__FILE__,__LINE__,
							"Kill Process after SIGCONT and SIGTERM, check for pid returns %d", errno);
      							kill(p->p_pid,SIGKILL); 	/* Ok thats it */
						}
						RemoveProcess(p->p_pid);
                			}
           	    		}
			}
			i++;
		} while (i < slice);
        }
        else if (r = MakeConnection())
             {
                SpawnService(r);
                SecondaryConnected = 1;
    		monitor_trace(__FILE__,__LINE__,"Wait Rest connecting Secondary to service");
		i = 0;
		slice = get_slices(max_duration);
		do 
		{
                	sleep(get_sleep_time(slice));
			CheckRequests();
			i++;
		} while (i < slice);
             }


}


SetUservRate()
{
  if (Primary || NoNetWork) lat_rate(255); else lat_rate(75);
}

 
main(argc, argv)
int argc;
char **argv;
{
    long l;

    time(&l);
    set_options(argc, argv);		 /* Get options and configuration information */
    monitor_trace(__FILE__,__LINE__,"Monitor started at %s",ctime(&l));
    set_system_stopped();		 /* Assume that TaxiPak is in the "STOPPED" state */
    Initialize();			 /* Setup and Initialization */
    if (AutoLoad) 
    {
        if (LoadProcesses())	 /* Load TaxiPak on startup or wait for user to */
	{
		set_system_started();
		ReLoaded=0;
	}
	
    }
    while (!terminated)
    {
		SetUservRate();         /* set USERV ratio (unfortunally something is changing while TaxiPak is running) */
		CleanUpMsgQ();          /* Remove unknown messages from the handshake message queue */
		CheckDisapearedUIs();   /* Check for any missing UIs */
		QueryProcesses();	/* Send tokens to valid processes */
					/* Check to see if user wants to communicate with us */
		do {
			WaitRest();	/* Give processes reasonable amount of time to respond */
			if (ReLoaded)			/*In case we reloaded in between query cycles we need to requery*/
			{

    				monitor_trace(__FILE__,__LINE__,"ReLoad after a Query");
				QueryProcesses();	/* Send tokens to valid processes */
				WaitRest();		/* Wait another cycle */
				ReLoaded = 0;		/* Clear reload flag */
			}
		} while ((time(0) - wait_beg_time) < get_sleep_time(1));

		CollectResponses();		/* Get response tokens from processes */

		if (CheckProcesses())		/* if any processes did not responded take action */
		{ 
			terminated=1;
    			monitor_trace(__FILE__,__LINE__,"Check processes returned to terminate ");
			terminate();		/* Terminate and cut over */
		
		}
    }
    UnInitialize(); 			/* Clean Up	*/
}
