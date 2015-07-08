/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: pvid.c,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1997/08/09 18:08:16 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/sysadmin/pvid.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: pvid.c,v 1.2 1997/08/09 18:08:16 taxiadm Exp $";


#include <stdio.h>
#include <ctype.h>
#include <sys/signal.h>
#include <sys/errno.h>
#include <setjmp.h>
#include "msgcomms.h"

char inbuf[10];
jmp_buf alrm_trap;


static char *blanks="                         ";


static struct   menutype { char *line; } mgmt_menu1[]={
	"\n",
        "  TaxiPak Administration\n",
        "   SYSTEM MANAGEMENT\n",
        "\n",
        "1  Cold Start             \n",
        "2  Warm Start             \n",
        "3  Synchronize Files\n",
        "4  Start Transaction Shadowing\n",
        "5  Stop Transaction Shadowing\n",
	"6  Display Processes\n",
	"7  Stop TAXI System\n",
	"8  Backup Computer Status\n",
	"9  Switch Control to Backup Computer\n",
	"\n",
	"\n",
	"\n",
	"\n",
	"Enter \'e\' to return to previous menu or exit.\n",
	"Type a number followed by a carriage return\n",
	"to execute a selection: ",
        ""};
 
PutMenu()
{
	int i=0;

	clear_scrn();
        while (*mgmt_menu1[i].line)
	{
                printf("%s%s",blanks,mgmt_menu1[i].line);
	 	i++;
	}

} 

timeout()
{
        longjmp(alrm_trap);
}



	
main(argc, argv)
int argc;
char ** argv;
{
    	int i,j;
	int c;
    	FILE * fp, * fopen();

	/* Tell monitor of our presents */
        setuid(0);           /* set id to ROOT */

    	if ((fp=fopen("/usr/taxi/run/monitor.pid","r"))!=NULL)
    	{
       		 fscanf(fp,"%d",&i);
       		 fclose(fp);
    	}
  
	ManagerUI();
	if (kill(i,SIGUSR1)==-1)
	{
		if (errno == ESRCH)
		{
			clear_scrn();
       			printf("Monitor is not running ......\nPress <return> to continue ........\n");
			gets(inbuf);
			exit(1);
 
		}
		perror("kill");
	}
	clear_scrn();
	printf("Please wait while waking up monitor ........\n");
	i=0;
	do {
		i++;
		CheckResponses();
		sleep(3);
		if (i > 10) 
		{
			printf("No communications established ... aborted\n");
			exit(0);
		}
	 } while (!get_monitor_pid());
	alarm(0);
	for (;;)
	{

		PutMenu();
        	if (setjmp(alrm_trap) != 0)
        	{
			CheckResponses();

 		}
	        signal(SIGALRM,timeout);
       	 	alarm(5);
		CheckResponses();
		gets(inbuf);
		c=toupper(inbuf[0]);
		alarm(0);
 
		switch (c)
		{
		 	case '1': SendRequest(COLDSTART);
				clear_scrn();
				printf("\nCold start in progress ....\n");
				while (CheckResponses()!=1);
				break;
			
                        case '2': SendRequest(WARMSTART); 
				clear_scrn();
				printf("\nWarm start in progress ....\n");
				while (CheckResponses()!=1);
                                break;  
 
			case '3':
				  SendRequest(SYNCDB);
				  break;

                        case '4': SendRequest(STARTMIRROR);
				clear_scrn();
				printf("\nMirror start in progress ....\n");
				while (CheckResponses()!=1);
                                break;  
 
                        case '5': 
				SendRequest(STOPMIRROR);
                                break;  
 
                        case '6': 
				SendRequest(VIEWPROCESS);
				set_sw(1);
                                break;  
 
                        case 'P': 
				SendRequest(VIEWPROCESS);
				set_sw(0);
                                break;  
                        case 'Q': 
				SendRequest(QSTATUS);
                                break;  
			case 'Z': signal(get_monitor_pid() ,SIGUSR2);
				  break;
                        case '7': 
				SendRequest(STOPSYSTEM);
				clear_scrn();
				printf("TaxiPak being stopped ....");
				while (CheckResponses()!=1);
                                break;  
                        case '9': 
				
				printf("\n\nAre you sure?(Y/N)");
				gets(inbuf);
				c=toupper(inbuf[0]);
				if (c=='Y')
					SendRequest(CUTOVER);
                                break;  
			case '8':
				SendRequest(BACKUP);
				break;
			case 'E':
				SendRequest(EXITUI);
				clear_scrn();
				exit(0);
				break;
 
			default : 
				  break;
		}
	}
}

