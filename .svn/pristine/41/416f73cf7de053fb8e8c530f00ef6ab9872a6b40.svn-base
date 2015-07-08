/***********************************************************************
*                 RCS INFO
*
*  @@(#)  $RCSfile: async_util.c,v $
*  @@(#)  $Revision: 1.1 $
*  @@(#)  $Date: 2001/02/02 16:21:59 $
*  @@(#)  $Author: jwelch $
*  @@(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-g7/bin/async_util/async_util.c,v $
*
*  Copyright (c) 1992 - Motorola Mobile Data Division, Seattle, WA
*
***********************************************************************/
#ident "@@(#) head:$RCSfile: async_util.c,v $    $Revision: 1.1 $"

static char rcsid[] = "$Id: async_util.c,v 1.1 2001/02/02 16:21:59 jwelch Exp $";
#include <stdio.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/ipc.h>
#include <signal.h>
#include <isam.h>

#include "acc.h"
#include "async.h"

#include "mads_types.h"
#include "key.h"
#include "mad_error.h"

#define TRACE_LOGFILE "/usr/taxi/run/trace_logfile"


void display_menu();
void do_vehicle_search(char *, char, char *, char, char *);
void veh_get_mid(char, char *, char *);
static void sig_int(int);

extern char	req_buf[];

ALC_STATS   *alc_ptr;
int         true = 1;
char	    global_veh_nbr[10];
char	    global_fleet_id;

main(int argc, char **argv)
{
  char  trace_mid[ROUTE_ID_SIZE];
  char  veh_nbr[10];
  char  fleet;
  char  hour_time[4];
  char  out_option;
  char  out_file[132];
  int   alc_shmid;
  int   alc_id;
  int   segment_size;
  long  key = 22801;
  char  response;
  char  inbuf[10];
  int   i;
  CHAR *ctime(), *stime;
  long tmtime;
   char           DateBuff[16];    

  signal(SIGINT, sig_int);

  segment_size = sizeof(ALC_STATS);

  alc_id = shmget(key, segment_size, 0660);
  if (alc_id < 0) {
    perror("Shmget error");
    exit (-1);
  }

  alc_ptr = (ALC_STATS *) shmat(alc_id, 0, 0);
  if (alc_ptr == NULL) {
    perror("Shmat error");
    exit (-1);
  }

  strcpy(global_veh_nbr, alc_ptr->trace_mid);
  global_fleet_id = 'A';
  while ((toupper(response) != 'Q')) {
  if (response != '\n')
  	display_menu();
  fflush(stdin);
  response = getc(stdin);
  switch (response)
    {
    case '1':
	  if (alc_ptr->trace == 'Y') {
	    alc_ptr->trace = 'N';
	  }
	  else {
	    alc_ptr->trace = 'Y';
	  }
      break;
      case '2':
	fprintf(stderr,"\t\tVehicle Number to trace --> ");
	fflush(stdin);
	do 
	  fgets(veh_nbr, 8, stdin);
	while (veh_nbr[0] == '\n');
	if (strncmp("ALL", veh_nbr, 3))
	{
		fflush(stdin);
		fprintf(stderr, "\t\tFleet Designator \t--> ");
		fleet = getc(stdin);
		global_fleet_id = fleet;
		veh_get_mid(fleet, veh_nbr, trace_mid);
	}
	if (!strncmp("ALL", veh_nbr, 3))
		strcpy(alc_ptr->trace_mid, "ALL");
	else
		strcpy(alc_ptr->trace_mid, trace_mid);
	strcpy(global_veh_nbr, veh_nbr);
	for (i = 0; i < 10 ; i++)
		if (global_veh_nbr[i] == '\n')
			global_veh_nbr[i] = '\0';
	break;
      case '3':
	for ( i = 0; i < 500; i++ )
	  {
	    if ( strlen(alc_ptr->veh_table[i].VehIP) > 0 )
	      {
		//tmtime = time((time_t *) &alc_ptr->veh_table[i].LastAccess);
		stime = ctime(&alc_ptr->veh_table[i].LastAccess);
		//fprintf(stderr, "slot %d\t%05x\t%s access %d\n", i,alc_ptr->veh_table[i].mid, alc_ptr->veh_table[i].VehIP, alc_ptr->veh_table[i].LastAccess);		
				fprintf(stderr, "slot %d\t%05x\t%s access %.15s\n", i,alc_ptr->veh_table[i].mid, alc_ptr->veh_table[i].VehIP, &stime[4]);
	      }
	  }
	fflush(stdin);fflush(stdout);fflush(stderr);
	fprintf(stderr,"Press any <ENTER> to continue...");
	fgetc(stdin);
        break;

       case '4':
	 fprintf(stderr,"Received: %d Transmitted: %d\n", alc_ptr->rx.num_rx_msgs[1], alc_ptr->tx.num_tx_msgs[0]);
	fflush(stdin);fflush(stdout);fflush(stderr);
	fprintf(stderr,"Press any <ENTER> to continue...");
	fgetc(stdin);	 
	  break;
       case '5':
	break;
     }
  }
}

void
display_menu()
{
  system("clear");
  fprintf(stderr, "\n\n\n");
  fprintf(stderr, "\t\tT a x i P a k  A s y n c  U t i l i t i e s\n");
  fprintf(stderr, "\t       ---------------------------------------------\n");
  fprintf(stderr, "\t\t(1)   Toggle trace output  [%c]\n", alc_ptr->trace);
  fprintf(stderr, "\t\t(2)   Select Vehicle to trace  [%s][%c]\n", global_veh_nbr, global_fleet_id);
  fprintf(stderr, "\t\t(3)   Dump GPRS IP table\n");
#ifdef VEHICLE_TRACE
  fprintf(stderr, "\t\t(3)   Retrieve Vehicle trace \n");
#endif
  fprintf(stderr, "\t\t(4)   Counters \n");
  fprintf(stderr, "\t\t(Q)   Quit\n");
  fprintf(stderr, "\n      Choice ? ");
}

static void
sig_int(int signo)
{
  true = 0;
}

void
do_vehicle_search(char *vh_nbr, char fleet, char *hour_time, char out_option, char *out_file)
{

}

void
veh_get_mid(char fleet, char *vh_nbr, char *rf_id)
{

}	
