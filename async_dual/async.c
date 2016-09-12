
/*
 *                 RCS INFO
 *
 *  @(#)  $RCSfile: async.c,v $
 *  @(#)  $Revision: 1.3 $
 *  @(#)  $Date: 2002/01/07 16:31:33 $
 *  @(#)  $Author: taxiadm $
 *  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/async_dual/async.c,v $
 *
 *  Copyright (c) 1991 - Motorola Mobile Data Division, Seattle, WA
 *
 */

/****************************************************************************/


/* HEADER */
/****************************************************************************
**
** NAME:
**	async.c
**
**
** INPUT DATA:
**
**
** OUTPUT DATA:
**
**
** NOTES:
**
**
** UPDATE HISTORY:
**
**	JHoy          	Nov 16/89   		Original.
**      MB & TR         Q1/Q2 90                Update
**
**
*******************************************************************************/
/* END HEADER */

/* INCLUDE FILES */
typedef char BOOLEAN;
/* INCLUDE FILES */
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdarg.h>

#include <sys/types.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <sys/ipc.h>
#include <time.h>
#include <malloc.h>
#include <sys/param.h>
#include <errno.h>
#include "std.h"
#include "STDIO.h"
#include "string.h"
#include "fcntl.h"
#include "signal.h"
#include "ipcpriority.h"
#include <sys/msg.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "parman.h"
#include "acc.h"
#include "async.h"
#include "msg.h"
#include "mmp.h"
#include "language.h"
#define DEBUG_R4
#include "itm.h"
#include "path.h"
#define DECLARING
#include "comm_strdefs.h"

#define	ERROR	-1

/* LOCAL DEFINES */


/* LOCAL TYPEDEFS */

/* IMPORTED VARIABLES AND FUNCTIONS */
IMPORT ALC_STATS *alc_ptr;

/* LOCAL VARIABLES AND FUNCTIONS */
INT   Par_errno = 0;
#define MAX_LINKS    8

INT   Fd_tx[MAX_LINKS];
INT   Fd_rx[MAX_LINKS];

INT   Opentimeout;		/* timeout value for port open etc.. */
INT   Openretry;		/* retry count for port errror */
INT   Openretryinterval;	/* time interval to wait before retry */
INT   Writetimeout;		/* timeout value for port write etc.. */
INT   Writeretry;		/* retry count for port errror */
INT   Writeretryinterval;	/* time interval to wait before retry */
INT   Readretry;		/* retry count for port errror */
INT   Readretryinterval;	/* time interval to wait before retry */

CHAR  parse_file1[MAXLEN_P_VALUE + 1];
CHAR  parse_file2[MAXLEN_P_VALUE + 1];

CHAR    TCP_service[MAX_LINKS][MAXLEN_P_VALUE + 1];
CHAR    TCP_server[MAX_LINKS][MAXLEN_P_VALUE + 1];
CHAR    Link_type[MAX_LINKS][MAXLEN_P_VALUE + 1];

long  TCP_Buff;
time_t midnight = 0L;
char    trace_file[65];

#define TRACE_MSG_FILE "/usr/taxi/run/traces/trace_log"
#define ASYNC_PID_FILE "/usr/taxi/run/async_pid_file"
void write_file_with_async_pid();
void kill_async();

CHAR  Err_msg[MAXLEN_P_VALUE + 1];	/* contains error msg to output on */
/* alarm. */
CHAR  Err_toggle;
MSG_QUEUE_HDR Txmsg;		/* holds message from ROUT */
MSG_QUEUE_HDR Rxmsg;		/* hold message for RIN */


LONGINT rpid, tpid, term_pid;
LOCAL VOIDT sig_int();
LOCAL VOIDT sig_term();
LOCAL VOIDT parent_sig_term();

int   inb_cnt = 0;
int   outb_cnt = 0;

int recording = 0;




int     outb_fd;   /* named pipe to async */
int     inb_fd;    /* named pipe from async */

#define MMP_LINK_TYPE  'M'
#define STAR2000_LINK_TYPE 'S'
#define GPRS_LINK_TYPE     'G'


   ENUM
     {
       T1, T2, T3
	 } tstate;

/* CODE */


/********************************************************************
	main
********************************************************************/

main(argc, argv)
     INT   argc;
     CHAR **argv;
{
  INT   ps_global;		/* system-wide parameter-set ID */
  INT   ps_local;		/* process-specific parameter-set ID */
  CHAR  par_val[MAXLEN_P_VALUE + 1];
  INT   count;
  LONGINT key;
  int   i;
  extern struct tm *localtime();
  extern time_t tm_to_time();
  struct tm *tm_ptr;
  time_t nowtime;
  
  sigsetmask(0);
  
  /* Open queue to monitor */
  OpenMsgQ();
  
  /* Init shared memory */
  pminit();	
  
  /* initialize the DLC */
  
  write_file_with_async_pid();
  signal(SIGUSR2, kill_async);
  
  if (argc != 3)
    {
      strcpy (parse_file2,"alc_1.ps");
      strcpy (parse_file1,"rng.ps");
    }
  else
    {
      
      strcpy (parse_file1,argv[1]);
      strcpy (parse_file2,argv[2]);
    }
  
  /* open the parset(s) */
  GetAsyncParams( parse_file2 );
  
  if ((outb_fd = open(TO_ASYNC_PIPE, O_RDWR)) < 0)
    fprintf(stderr,"pipe open failure - to async errno %d\n",errno);

  if ((inb_fd = open(FROM_ASYNC_PIPE, O_RDWR)) < 0)
    fprintf(stderr,"pipe open failure - from async errno %d\n",errno);
  
  while (1)
    {
      
      for ( i = 0; i < MAX_LINKS; i++ )
	Fd_rx[i] = Fd_tx[i] = -1;
      

      for ( i = 0; i < MAX_LINKS; i++ )
	{
	  if ( atoi(&TCP_service[i][1]) > 0 )
	    {
	      if ( !strcmp(Link_type[i], "GPRS") )
		{
		  if ((Fd_tx[i] = udp_open(TCP_service[i])) == -1)
		    fprintf(stderr, "**UDP %s:%s** can't open socket\n", TCP_server[i], TCP_service[i]);
		}
	      else
		{
		  if ((Fd_tx[i] = tcp_open(TCP_server[i], TCP_service[i], TCP_Buff,
					   Openretryinterval, Openretry )) == -1)
		    {
		      fprintf(stderr, "**TCP %s:%s** can't open socket\n", TCP_server[i],TCP_service[i]);
		    }
		}
	    }
	}
      
      for ( i = 0; i < MAX_LINKS; i++ )
	{
	  Fd_rx[i] = Fd_tx[i];
	}
      
	
      
      
  
  /*
   * * alc_start will ONLY return if its child process dies!!
   */
      alc_ptr->trace = 'Y';
      strcpy(alc_ptr->trace_mid, "ALL");
      nowtime = (long)time((long *)0);
      
      /** Set global for determination of midnight **/
      
      tm_ptr = localtime(&nowtime);
      sprintf(trace_file, "%s.%02d%02d%02d", TRACE_MSG_FILE,
	      tm_ptr->tm_year % 100, tm_ptr->tm_mon + 1, tm_ptr->tm_mday);
      tm_ptr->tm_sec = 59;
      tm_ptr->tm_min = 59;
      tm_ptr->tm_hour = 23;
      midnight = tm_to_time( tm_ptr );
      midnight += 1;

      alc_start();

      //alc_rx();
      
      for ( i = 0; i < MAX_LINKS; i++ )
        {
          if (  Fd_tx[i] != -1 )
            close( Fd_tx[i] );
        }
      sleep(5);
      GetAsyncParams( parse_file2 );      
    }
}



/********************************************************************
	alc_start -forks off tx/rx process then monitors the children
	           forever.
********************************************************************/
alc_start()
{
int i;
 
FILE *trace_logfile;
 CHAR *ctime(), *stime;
 time_t tmtime;
 struct tm *tm_ptr;
 time_t nowtime;
 extern  struct tm *localtime();
 char   DateBuff[16];
 int                 res, sockfd; 
 char                udp_buf[132];
 struct sockaddr_in  serv_addr, cli_addr;
 struct hostent      *hp;
   
/* Fork Transmit process */

alc_ptr->linkup = 'F';


   tpid = fork();
   if (tpid == -1)
   {
      fprintf(stderr,
	 "**ALC** Error in forking transmit process (errno = %d)\n", errno);
      exit(1);
   } else if (tpid == 0)
      alc_tx();			/* child process */

   /*
    * * Fork Receive process
    */
   rpid = fork();


   if (rpid == -1)
   {
      /*
       * * kill off the tx process
       */
      kill(tpid, SIGTERM);

      fprintf(stderr,
	  "**ALC** Error in forking receive process (errno = %d)\n", errno);
      exit(1);
   } else if (rpid == 0)
      alc_rx();			/* child process */

    signal(SIGTERM, parent_sig_term); 

   /* at this point the link is considered UP */

   alc_ptr->linkup = 'T';

   /* parent process doesn't need the ports so close them */

   /* wait for either one of the kids */
   /* to terminate or for a signal    */
   do
   {
      term_pid = wait(0);
   } while ((term_pid != rpid) && (term_pid != tpid) && (errno != ECHILD));

   if (term_pid == rpid)
   {
     if ((trace_logfile = fopen(trace_file, "a")) == NULL)
       fprintf(stderr, "Error opening trace file \n");

     tmtime = time((long *) 0);
     get_arg_date(tmtime, DateBuff);
     stime = ctime(&tmtime);
     fprintf(trace_logfile, "%.2s", DateBuff);
     fprintf(trace_logfile, "%.2s", &DateBuff[3]);
     fprintf(trace_logfile, "%.2s ", &DateBuff[6]);
     fprintf(trace_logfile, "%.2s", &stime[11]);
     fprintf(trace_logfile, "%.2s", &stime[14]);
     fprintf(trace_logfile, "%.2s", &stime[17]);
     fprintf(trace_logfile, " %s", "**ALC** [alc_start]:alc read process died\n");
     fclose(trace_logfile);
     
     fprintf(stderr, "**ALC** [alc_start]:alc read process died\n");

       /* kill the tx process */

      if (tpid > 0)
      {
	 kill(tpid, SIGTERM);
	 kill(rpid, SIGTERM);
	 do
	 {
	    term_pid = wait(0);
	 } while (term_pid != tpid);
      }
   } else if (term_pid == tpid)
   {
     if ((trace_logfile = fopen(trace_file, "a")) == NULL)
       fprintf(stderr, "Error opening trace file \n");
     
     tmtime = time((long *) 0);
     get_arg_date(tmtime, DateBuff);
     stime = ctime(&tmtime);
     fprintf(trace_logfile, "%.2s", DateBuff);
     fprintf(trace_logfile, "%.2s", &DateBuff[3]);
     fprintf(trace_logfile, "%.2s ", &DateBuff[6]);
     fprintf(trace_logfile, "%.2s", &stime[11]);
     fprintf(trace_logfile, "%.2s", &stime[14]);
     fprintf(trace_logfile, "%.2s", &stime[17]);
     fprintf(trace_logfile, " %s", "**ALC** [alc_start]:alc tx process died\n");
     fclose(trace_logfile);
     
      fprintf(stderr, "**ALC** [alc_start]:alc write process died\n");
      if (rpid > 0)
      {
	 kill(rpid, SIGTERM);
	 do
	 {
	    term_pid = wait(0);
	 } while ((term_pid != rpid) && (errno != ECHILD));
      }
   }

     if ((trace_logfile = fopen(trace_file, "a")) == NULL)
       fprintf(stderr, "Error opening trace file \n");
     
     tmtime = time((long *) 0);
     get_arg_date(tmtime, DateBuff);
     stime = ctime(&tmtime);
     fprintf(trace_logfile, "%.2s", DateBuff);
     fprintf(trace_logfile, "%.2s", &DateBuff[3]);
     fprintf(trace_logfile, "%.2s ", &DateBuff[6]);
     fprintf(trace_logfile, "%.2s", &stime[11]);
     fprintf(trace_logfile, "%.2s", &stime[14]);
     fprintf(trace_logfile, "%.2s", &stime[17]);
     fprintf(trace_logfile, " %s", "**ALC** [alc_start]:close port(s)\n");
     fclose(trace_logfile);
     
     fprintf(stderr, "**ALC** [alc_start]:close port(s)\n");




   for (i = 0; i < MAX_LINKS; i++ )
     {
       if ( Fd_rx[i] != -1 )
	 close( Fd_rx[i] );
     }


   // Send UDP message to HTD server to alert of the restart
   bzero(udp_buf, sizeof(udp_buf));
   sprintf(udp_buf, "%s!%s", "async", "restart");   
   bzero((char *)&serv_addr, sizeof(serv_addr));
   serv_addr.sin_family = AF_INET;
   hp = gethostbyname("ws226.taksihelsinki.fi");
   if ( hp != NULL )
     {
       bcopy(hp->h_addr, (char *)&serv_addr.sin_addr, hp->h_length);
       serv_addr.sin_port = htons(5252);
       if ( ( sockfd = socket(AF_INET, SOCK_DGRAM, 0 ) ) != 0 )
	 {
	   bzero((char *)&cli_addr, sizeof(cli_addr));
	   cli_addr.sin_family = AF_INET;
	   cli_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	   cli_addr.sin_port = htons(0);
	   if ( bind(sockfd, (struct sockaddr *) &cli_addr, sizeof(cli_addr) ) >= 0 )
	     res = sendto(sockfd, udp_buf, strlen(udp_buf), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
	   
	   close(sockfd);
	 }
     }
		 
   
   exit(-1); // terminate parent process. Monitor to restart
   fprintf(stderr, "**ALC** [alc_start]:back to main\n");
   /* * return to main, to re-start */
}


/********************************************************************
	alc_tx -module to take messages from "Router out" process and
	        transmit to ACC. On write to "tty" port this module
	        must: 1) sense when "carrier detect" is present, this
	                 tells us whether the cabling is ok.
	              2) sense when we are "xoff'd" by the ACC. The time
	                 should not be great else there are problems.
********************************************************************/
alc_tx()
{
   INT   len, blks;
   INT   sq1, sq2, max_sq;
   INT   sq[MAX_LINKS];
   INT   status;
   CHAR  length[PKT_LEN_SIZE + 1];
   CHAR  seq_no_hl1[SEQ_NO_SIZE + 1];
   CHAR  seq_no_hl2[SEQ_NO_SIZE + 1];
   CHAR  seq_no_hl[MAX_LINKS][SEQ_NO_SIZE + 1];
   LINK_PKT packet;
   CHAR *pkt;
   CHAR  nul[2], soh[2], etx[2];
   FILE *trace_logfile;
   CHAR *ctime(), *stime;
   long tmtime;

 
  INT   Fleet_bcast, host_link;
   INT   i;
   MSG_QUEUE_HDR  *Tmsg;
   char           Tbuf[MMP_TEXT_SIZE + 1];
   int            kk, xx;
   int            itm_type;
   int            text_len = 0;   
   char           DateBuff[16];
   char           DescriptBuff[16];
   extern    struct tm *localtime();
   extern    time_t tm_to_time();
   long    nowtime;
   struct tm *tm_ptr;
   char    cLink;
   int     link_index = 0;   
#define MAX_SPACES  56

   signal(SIGTERM, sig_term);  


   nul[0] = NUL;
   nul[1] = 0;
   soh[0] = SOH;
   soh[1] = 0;
   etx[0] = ETX;
   etx[1] = 0;

   /* calculate max SQ # (as a function of SEQ_LEN) */
   for (i = 0; i < MAX_LINKS; i++ )
     for ( sq[i] = 0, max_sq = 1; sq[i] < SEQ_NO_SIZE; sq[i]++, max_sq *= 16 );
   
   for (sq1 = 0, max_sq = 1; sq1 < SEQ_NO_SIZE; sq1++, max_sq *= 16);
   for (sq2 = 0, max_sq = 1; sq2 < SEQ_NO_SIZE; sq2++, max_sq *= 16);
   max_sq--;

   for (sq[0] = 0, sq[1] = 0;;)
   {
     
     bzero( &Txmsg, sizeof(MSG_QUEUE_HDR) );
     itm_type = 0;
      status = get_outb_msg( &Txmsg);

      //     if ( status == 0 ) // GPRS outbound message that was redirected
      //	continue;
      
      if (status == ERROR)
	fprintf(stderr, "**ALC** [alc_tx]:read error from queue\n");

      else
      {

	
         /* Process msg read from QUEUE, text part is NULL terminated */

	 /* update no packets tx'd */

	 len = strlen(Txmsg.text);
	 blks = len / BLK_SZ;	/* get no of blks */
	 if ((len % BLK_SZ) > 0)
	    ++blks;
         if (blks >=  NUM_BLKS)
            blks = NUM_BLKS -1;  
	  alc_ptr->tx.num_tx_msgs[blks]++;

	 /* calc length */
	 len = strlen(Txmsg.route_id);
	 sprintf(length, "%03x", len);

	 sprintf(Err_msg, "%s%c%s%c", "writing port ", '[', "tcp", ']');

	 Fleet_bcast = 0;
	 host_link = 0;

         /** Fleet broadcast messages are sent out on host link 1.  ACC will 
	     handle delivery to all in group 				**/
	 if (strncmp(Txmsg.route_id, "00F0", 4) == 0)
	   {
	     cLink = Txmsg.delvry_prio;
             Fleet_bcast = 1;
	     for ( i = 0; i < MAX_LINKS; i++ )
	       {
		 if ( Fd_tx[i] != -1 )
		   {
                     // don't send the EMERG_BCAST message on MMP links
                     if ( Txmsg.p_fields[2] == 'F' && !strcmp(Link_type[i], "MMP") )
                       continue;
                     
		     bzero( &packet, sizeof( LINK_PKT ));		     
		     sprintf(seq_no_hl[i], "%02x", sq[i]);
		     sq[i] = (sq[i] < max_sq) ? sq[i] + 1 : 0;
		     /** Send broadcast on link **/
		     pkt = (CHAR *) & packet;
		     strcpy(pkt, soh);
		     strcat(pkt, seq_no_hl[i]);
		     strcat(pkt, length);
		     strcat(pkt, Txmsg.route_id);
		     strcat(pkt, etx);
		     len += 2;

		     if ( strcmp(Link_type[i], "GPRS") )
		       alc_write(Fd_tx[i], pkt, len + PKT_LEN_SIZE + SEQ_NO_SIZE);
		     else
		       udp_gprs_broadcast_write(Fd_tx[i], pkt, len + PKT_LEN_SIZE + SEQ_NO_SIZE);

		     alc_ptr->tx.num_hl[i]++;
		   }
	       }
	   }

	 
	 if (!Fleet_bcast)
	   {
	     if ( Txmsg.delvry_prio == '1' )
	       {
		 host_link = Fd_tx[0];
		 alc_ptr->tx.num_hl[0]++;
                 link_index = 0;                 		 
	       }
	     else if ( Txmsg.delvry_prio == '2' )
	       {
		 host_link = Fd_tx[1];
		 alc_ptr->tx.num_hl[1]++;
                 link_index = 1;                 		 
	       }		 
	     else if ( Txmsg.delvry_prio == '3' )
	       {
		 host_link = Fd_tx[2];
		 alc_ptr->tx.num_hl[2]++;
                 link_index = 2;                 		 
	       }		 
	     else if ( Txmsg.delvry_prio == '4' )
	       {
		 host_link = Fd_tx[3];
		 alc_ptr->tx.num_hl[3]++;
                 link_index = 3;                 		 
	       }		 
	     else if ( Txmsg.delvry_prio == '5' )
	       {
		 host_link = Fd_tx[4];
		 alc_ptr->tx.num_hl[4]++;
                 link_index = 4;                 		 
	       }		 
	     else if ( Txmsg.delvry_prio == '6' )
	       {
		 host_link = Fd_tx[5];
		 alc_ptr->tx.num_hl[5]++;
                 link_index = 5;                 		 
	       }		 
	     else if ( Txmsg.delvry_prio == '7' )
	       {
		 host_link = Fd_tx[6];
		 alc_ptr->tx.num_hl[6]++;
                 link_index = 6;                 		 
	       }		 
	     else if ( Txmsg.delvry_prio == '8' )
	       {
		 host_link = Fd_tx[7];
		 alc_ptr->tx.num_hl[7]++;
                 link_index = 7;                 		 
	       }		 
	     
	     cLink = Txmsg.delvry_prio;
	     Txmsg.delvry_prio = ' ';
	     
	     len = strlen(Txmsg.route_id);
	     sprintf(length, "%03x", len);
	     pkt = (CHAR *) & packet;
	     strcpy(pkt, soh);

	     if (host_link == Fd_tx[0]) 
	       {
		 sprintf(seq_no_hl[0], "%02x", sq[0]);
		 strcat(pkt, seq_no_hl[0]);
		 sq[0] = (sq[0] < max_sq) ? sq[0] + 1 : 0;
	       }
	     else if ( host_link == Fd_tx[1] )
	       {
		 sprintf(seq_no_hl[1], "%02x", sq[1]);
		 strcat(pkt, seq_no_hl[1]);
		 sq[1] = (sq[1] < max_sq) ? sq[1] + 1 : 0;
	       }
	     else if ( host_link == Fd_tx[2] )
	       {
		 sprintf(seq_no_hl[2], "%02x", sq[2]);
		 strcat(pkt, seq_no_hl[2]);
		 sq[2] = (sq[2] < max_sq) ? sq[2] + 1 : 0;
	       }
	     else if ( host_link == Fd_tx[3] )
	       {
		 sprintf(seq_no_hl[3], "%02x", sq[3]);
		 strcat(pkt, seq_no_hl[3]);
		 sq[3] = (sq[3] < max_sq) ? sq[3] + 1 : 0;
	       }
	     else if ( host_link == Fd_tx[4] )
	       {
		 sprintf(seq_no_hl[4], "%02x", sq[4]);
		 strcat(pkt, seq_no_hl[4]);
		 sq[4] = (sq[4] < max_sq) ? sq[4] + 1 : 0;
	       }
	     else if ( host_link == Fd_tx[5] )
	       {
		 sprintf(seq_no_hl[5], "%02x", sq[5]);
		 strcat(pkt, seq_no_hl[5]);
		 sq[5] = (sq[5] < max_sq) ? sq[5] + 1 : 0;
	       }
	     else if ( host_link == Fd_tx[6] )
	       {
		 sprintf(seq_no_hl[6], "%02x", sq[6]);
		 strcat(pkt, seq_no_hl[6]);
		 sq[6] = (sq[6] < max_sq) ? sq[6] + 1 : 0;
	       }
	     else if ( host_link == Fd_tx[7] )
	       {
		 sprintf(seq_no_hl[7], "%02x", sq[7]);
		 strcat(pkt, seq_no_hl[7]);
		 sq[7] = (sq[7] < max_sq) ? sq[7] + 1 : 0;
	       }
	     Tmsg = (MSG_QUEUE_HDR *)&Txmsg;
	     itm_type = hex2int(Tmsg->text, 2);
	     if ( itm_type == CCP_MSG_FROM_ITM )
	       {
		 text_len = 0;
		 for ( tstate = T1;; )
		   {
		     if ( tstate == T1 )
		       {
			 if ( Tmsg->text[text_len] == '%' )
			   tstate = T2;
			 ++text_len;
		       }
		     else
		       {
			 if ( (Tmsg->text[text_len] == 'E') && (Tmsg->text[text_len+1] == '\0') && (Tmsg->text[text_len+2] == '\0') )
			   {
			     ++text_len;
			     break;
			   }
			 else
			   {
			     ++text_len;
			     tstate = T1;
			   }
		       }
		   }
		 sprintf(length, "%03x", text_len + ROUTE_ID_SIZE + PKT_ID_SIZE + 5 + P_SIZE);
		 strcat(pkt, length);
		 strncat(pkt, Txmsg.route_id, ROUTE_ID_SIZE + PKT_ID_SIZE + 5 + P_SIZE);
		 memcpy(pkt + 1 + 2 + PKT_LEN_SIZE + ROUTE_ID_SIZE + PKT_ID_SIZE + 5 + P_SIZE, Txmsg.text, text_len);
		 *(pkt + 1 + SEQ_NO_SIZE + PKT_LEN_SIZE + ROUTE_ID_SIZE + PKT_ID_SIZE + 5 + P_SIZE + text_len) = ETX;
		 len = 2 + SEQ_NO_SIZE + PKT_LEN_SIZE + ROUTE_ID_SIZE + PKT_ID_SIZE + 5 + P_SIZE + text_len;


		 
		 if ( host_link != -1 )
		   alc_write(host_link, pkt, len);
	       }
	     else
	       {
		 strcat(pkt, length);
		 strcat(pkt, Txmsg.route_id);
		 strcat(pkt, etx);
	     
		 len += 2;
		 if ( host_link != -1 )
		   {
		     if ( strcmp(Link_type[link_index], "GPRS") )
		       alc_write(host_link, pkt, len + PKT_LEN_SIZE + SEQ_NO_SIZE);
		     else
		       udp_gprs_write(host_link, pkt, len + PKT_LEN_SIZE + SEQ_NO_SIZE);
		   }
	       }
	     
	     if ( time((long *) 0) > midnight )
	       {
		 nowtime = (long)time((long *)0);
		 tm_ptr = localtime(&nowtime);
		 sprintf(trace_file,"%s.%02d%02d%02d", TRACE_MSG_FILE,
			 tm_ptr->tm_year % 100, tm_ptr->tm_mon + 1, tm_ptr->tm_mday);
		 
		 tm_ptr->tm_sec = 59;
		 tm_ptr->tm_min = 59;
		 tm_ptr->tm_hour = 23;
		 midnight = tm_to_time( tm_ptr );
		 midnight += 1;
	       }
	   }
	 
	 
	 if ((alc_ptr->trace == 'Y') &&
	     ((strncmp(Txmsg.route_id, alc_ptr->trace_mid, 5) == 0) ||
	      (strncmp("ALL", alc_ptr->trace_mid, 3) == 0)))
	   {
	     if ((trace_logfile = fopen(trace_file, "a")) == NULL)
	       fprintf(stderr, "Error opening trace file \n");
	     Tmsg = &Txmsg;
	     tmtime = time((long *) 0);
	     get_arg_date(tmtime, DateBuff);
	     stime = ctime(&tmtime);
	     fprintf(trace_logfile, "%.2s", DateBuff);
	     fprintf(trace_logfile, "%.2s", &DateBuff[3]);
	     fprintf(trace_logfile, "%.2s ", &DateBuff[6]);
	     fprintf(trace_logfile, "%.2s", &stime[11]);
	     fprintf(trace_logfile, "%.2s", &stime[14]);
	     fprintf(trace_logfile, "%.2s", &stime[17]);
	     
	     fprintf(trace_logfile, " U %.03d %c ", outb_cnt++, cLink);
	     if (outb_cnt > 999)
	       outb_cnt = 1;
	     
	     strncpy(Tbuf, Tmsg->route_id, ROUTE_ID_SIZE);
	     Tbuf[ROUTE_ID_SIZE] = '\0';
	     fprintf(trace_logfile, "%s ", Tbuf);	/* == mid of mdt(24bits) */

	     strncpy(Tbuf, Tmsg->pkt_id, PKT_ID_SIZE);
	     Tbuf[PKT_ID_SIZE] = '\0';
	     fprintf(trace_logfile, "%s ", Tbuf);

	     fprintf(trace_logfile, "%c", Tmsg->type);
	     strncpy(Tbuf, Tmsg->p_fields, P_SIZE);
	     Tbuf[P_SIZE] = '\0';
	     fprintf(trace_logfile, "%s ", Tbuf);
	     strncpy(Tbuf, Tmsg->text, MMP_TEXT_SIZE);
	     Tbuf[MMP_TEXT_SIZE] = '\0';
	     strncpy(Tbuf, Tmsg->text, MMP_TEXT_SIZE);

	     DescriptBuff[0] = '\0';
	     switch (Tmsg->type)
	       {
	       case '0':
		 switch(Tmsg->p_fields[0])
		   {
		   case '0': 
		     sprintf(DescriptBuff," EMERG_ACK");
		     break;
		   case '1':
		     sprintf(DescriptBuff," DEAUTH");
		     break;
		   case '2':
		     if (Tmsg->p_fields[1] == '1')
		       sprintf(DescriptBuff," AUTH");
		     break;
		   case 'A':
		     sprintf(DescriptBuff, "GROUP_ASSGN");
		     break;
		   }
		 break;
	       case '1':
		 switch(Tmsg->p_fields[0])
		   {
		   case '1':
		     sprintf(DescriptBuff," DEAUTH");
		     break;
		   
		   case '3':
                     switch(Tmsg->p_fields[1])
                       {
		       case '2':
			 sprintf(DescriptBuff," TPAK_DEFOBJ_NVRAM");
			 break;			 
                       case '4':
                         sprintf(DescriptBuff," TIMER_DWNLD");
                         break;
                       default:
                         switch(Tmsg->p_fields[2])
                           {
                           case '0':
                             sprintf(DescriptBuff," ZONE_BCST_1");
                             break;
                           case '1':
                             sprintf(DescriptBuff," ZONE_BCST_2");
                             break;
                           case '2':
                             sprintf(DescriptBuff," ZONE_BCST_3");
                             break;
                           case '3':
                             sprintf(DescriptBuff," ZONE_BCST_4");
                             break;
                           case 'F':
                             sprintf(DescriptBuff," EMERG_BCST");
                             break;
                           }
                       }
		     break;
		   }
		 break;
	       case '2':
		 switch (Tmsg->p_fields[0])
		     {
		     case '0':
		       sprintf(DescriptBuff," ITM");
		       itm_type = hex2int(Tmsg->text, 2);
		       switch (itm_type)
			 {
			 case CCP_MSG_TO_ITM:
			   strcat(DescriptBuff, "-CCP_OUT");
			   break;			   
			 case RECEIPT_FEEDBACK:
			   strcat(DescriptBuff, "-RCPT_FDBCK");
			   break;
			 case RQ_RECEIPT_DATA:
			   strcat(DescriptBuff, "-RQ_RCPT_DATA");
			   break;
			 case CARD_VALIDNESS_YES:
			   strcat(DescriptBuff, "-CARD_VALID_YES");
			   break;
			 case CARD_VALIDNESS_NO:
			   strcat(DescriptBuff, "-CARD_VALID_NO");
			   break;
			 case DRIVER_VALIDNESS_YES:
			   strcat(DescriptBuff, "-DRV_VALID_YES");
			   break;
			 case DRIVER_VALIDNESS_NO:
			   strcat(DescriptBuff, "-DRV_VALID_NO");
			   break;
			 case RQ_DRIVER_ID:
			   strcat(DescriptBuff, "-RQ_DRIVER_ID");
			   break;
			 case ACK_FIFO:
			   strcat(DescriptBuff, "-ACK_FIFO");
			   break;
			 case CARD_VALIDITY:
			   strcat(DescriptBuff, "-CARD_VALIDITY");
			   break;
			 case ACK_KEYS:
			   strcat(DescriptBuff, "-ACK_KEYS");
			   break;
			 case PRINT_TEXT:
			   strcat(DescriptBuff, "-PRINT_TEXT");
			   break;
			 case METER_ACTION:
			   strcat(DescriptBuff, "-METER_ACTION");
			   break;
			 case RQ_STATUS:
			   strcat(DescriptBuff, "-RQ_STATUS");
			   break;
			 case RQ_PARAMETERS:
			   strcat(DescriptBuff, "-RQ_PARAMS");
			   break;
			 case RQ_CONTROL_REGS:
			   strcat(DescriptBuff, "-RQ_CTRL_REGS");
			   break;
			 case RELAY_OFF:
			   strcat(DescriptBuff, "-RELAY_OFF");
			   break;			   
			 default:
			   if ( Tmsg->p_fields[2] == '1' )
			     strcpy(DescriptBuff, "GPS_MSG");
			   else
			     strcat(DescriptBuff, "-UNKNOWN");
			   break;
			 }
		       break;
		     case '1':
		       strcat(DescriptBuff, " STATUS_DISP");
		       break;
		     case '6':
		       sprintf(DescriptBuff," PRINT_TEXT");
		       Tbuf[32] = '\0';
		       strncpy(Tbuf, Tmsg->text, 32);
		       break;		       
		     case '2':
		       switch(Tmsg->p_fields[1])
			 {
			 case '0':
			   sprintf(DescriptBuff," MSG_DISP");
			   Tbuf[32] = '\0';
			   strncpy(Tbuf, Tmsg->text, 32);
			   break;
			 case '1':
			   sprintf(DescriptBuff," OFFER_DISP");
			   break;
			 case '2':
			   sprintf(DescriptBuff," ASSIGN_DISP");
			   break;
			 case '9':
			   sprintf(DescriptBuff," KELA_RESP");
			   break;
			 }
		       break;
		     case '3':
		       if (Tmsg->p_fields[1] == '0')
			 sprintf(DescriptBuff," PROMPT_DISP");
		       break;
		     case '4':
		       sprintf(DescriptBuff, " POLYGON_DWNLD");
		       break;
		     }
		   break;
	       case '3':
		 sprintf(DescriptBuff, " ");
		 switch(Tmsg->p_fields[1])
		   {
		   case '1':
		     sprintf(DescriptBuff, "INV_ZONE");
		     break;
		   case '2':
		     sprintf(DescriptBuff, "SIGNED_ON");
		     break;
		   case '3':
		     sprintf(DescriptBuff, "BOOKED_OFF");
		     break;
		   case '4':
		     sprintf(DescriptBuff, "SIGNED_OFF");
		     break;
		   case '5':
		     sprintf(DescriptBuff, "ALRDY_ON");
		     break;
		   case '6':
		     sprintf(DescriptBuff, "TRIP_CANX");
		     break;
		   case '7':
		     sprintf(DescriptBuff, "BID_UNSUCC");
		     break;
		   case '8':
		     sprintf(DescriptBuff, "SW_VOX");
		     break;
		   case '9':
		     sprintf(DescriptBuff, "INV_REQ");
		     break;
		   case 'a':
		   case 'A':
		     sprintf(DescriptBuff, "NOT_BOOKED_IN");
		     break;
		   case 'b':
		   case 'B':
		     sprintf(DescriptBuff, "ZONE_FULL");
		     break;
		   case 'c':
		   case 'C':
		     sprintf(DescriptBuff, "FLAG_OK");
		     break;
		   case 'd':
		   case 'D':
		     sprintf(DescriptBuff, "FLAG_DENIED");
		     break;
		   case 'e':
		   case 'E':
		     sprintf(DescriptBuff, "GOOD_ACCT");
		     break;
		   case 'f':
		   case 'F':
		     sprintf(DescriptBuff, "BAD_ACCT");
		     break; 
		   }
		 break;
	       }
	     fprintf(trace_logfile, "%20s ", DescriptBuff);

	     if ( itm_type == CCP_MSG_FROM_ITM )
	       {
		 text_len = 0;
		 for ( tstate = T1;; )
		   {
		     if ( tstate == T1 )
		       {
			 if ( isprint(Tmsg->text[text_len]) )
			   fprintf(trace_logfile, "%c", Tmsg->text[text_len]);
			 else
			   fprintf(trace_logfile, "[%2.2x]", Tmsg->text[text_len] & 0x000000ff);
			 if ( Tmsg->text[text_len] == '%' )
			   tstate = T2;
			 ++text_len;
		       }
		     else
		       {
			 if ( isprint(Tmsg->text[text_len]) )
			   fprintf(trace_logfile, "%c", Tmsg->text[text_len]);
			 else
			   fprintf(trace_logfile, "[%2.2x]", Tmsg->text[text_len] & 0x000000ff);
			 if ( (Tmsg->text[text_len] == 'E') && (Tmsg->text[text_len+1] == '\0') && (Tmsg->text[text_len+2] == '\0') )
			   break;
			 else
			   {
			     ++text_len;
			     tstate = T1;
			   }
		       }
		   }
		 fprintf(trace_logfile, "\n");
	       }
	     else
	       {
		 Tbuf[MMP_TEXT_SIZE] = '\0';
		 strncpy(Tbuf, Tmsg->text, MMP_TEXT_SIZE);
		 fprintf(trace_logfile, "%s\n", Tbuf);
	       }
	     fclose(trace_logfile);
	   }
      }

   }

}


/********************************************************************
	alc_rx -module to receive messages from ACC and put them into
	        a message queue for "Router in" process.
********************************************************************/
alc_rx()
{
   INT   len, blks, text_len, i;
   INT   sq, missed_sq, old_sq, prev_sq1, prev_sq2, max_sq;
   INT   prev_sq[MAX_LINKS];
   INT   k, status;
   CHAR  fch_buffer[FCH_LEN + 1], *fch_ptr;
   CHAR  len_buffer[PKT_LEN_SIZE + 1], *len_ptr;
   CHAR  seq_buffer[SEQ_NO_SIZE + 1], *seq_ptr;
   CHAR  msg_buffer[MAX_BUF + 1], *msg_ptr;
   BOOL  past_first_rxd_msg_hl1 = FALSE;
   BOOL  past_first_rxd_msg_hl2 = FALSE;
   BOOL  past_first_rxd_msg_hl3 = FALSE;
   BOOL  past_first_rxd_msg_hl4 = FALSE;
   BOOL  past_first_rxd_msg_hl5 = FALSE;
   BOOL  past_first_rxd_msg_hl6 = FALSE;
   BOOL  past_first_rxd_msg_hl7 = FALSE;
   BOOL  past_first_rxd_msg_hl8 = FALSE;   
   ENUM
   {
      R1, R2, R3, R4, R5, R6
   } rstate;
   CHAR *ctime(), *stime;
   time_t tmtime;
   struct tm *tm_ptr;
   time_t nowtime;
   extern  struct tm *localtime();
   int     MsgCounter = 0;
   float     avail_blocks, total_blocks, percent_free;
   struct  fs_data  *buf;
   CHAR  link_type = 'M';   
   MSG_QUEUE_HDR  *Tmsg;
   FILE   *trace_logfile;
   int                 res, sockfd;
   char                udp_buf[132];
   struct sockaddr_in  serv_addr, cli_addr;
   struct hostent      *hp;
   fd_set  readmask;
   INT     maxfdpl;
   INT     nfound;
   INT     Fd_read;
   INT     Fd_last_read;
   int     udp_len = 0;


  char  Tbuf[MMP_TEXT_SIZE + 1];
  int itm_type;
   CHAR   DescriptBuff[16];
   char   DateBuff[16];
   signal(SIGTERM, sig_term);
   /* clear buffers */
   for (k = 0; k < FCH_LEN + 1; fch_buffer[k] = 0, k++);
   for (k = 0; k < PKT_LEN_SIZE + 1; len_buffer[k] = 0, k++);
   for (k = 0; k < SEQ_NO_SIZE + 1; seq_buffer[k] = 0, k++);
   for (k = 0; k < MAX_BUF + 1; msg_buffer[k] = 0, k++);


   /* calculate max SQ # (as a function of SEQ_LEN) */
   for (sq = 0, max_sq = 1; sq < SEQ_NO_SIZE; sq++, max_sq *= 16);
   max_sq--;

   /* Start with read priority on Host Link 1 in the case where  */
   /* both host links have read interrupts pending               */
   Fd_last_read = Fd_rx[1];


   for (rstate = R1;;)
   {
      if (rstate == R1)
      {
	FD_ZERO(&readmask);	
	for ( i = 0; i < MAX_LINKS; i++ )
	  {
	    if ( Fd_rx[i] != -1 )
	      {
		FD_SET(Fd_rx[i], &readmask);
		maxfdpl = Fd_rx[i] + 1;
	      }
	  }

	//        FD_SET( outb_fd, &readmask );
	
	Fd_read = 0;


	
	nfound = select(maxfdpl, &readmask, (fd_set *) 0, (fd_set *) 0,
			(struct timeval *) 0);

#ifdef FOO	
        if ( FD_ISSET( outb_fd, &readmask ) )
	  {
	    alc_tx();
	    rstate = R1;
	    continue;
	  }
#endif
	if (nfound == 1)
	  {
	    for ( i = 0; i< MAX_LINKS; i++ )
	      {
		if ((Fd_rx[i] != -1) && FD_ISSET(Fd_rx[i], &readmask))
		  Fd_read = Fd_rx[i];
	      }
	  }
	      

	if (nfound > 1)
	  {
	      if ( Fd_last_read == Fd_rx[0] )
		  {
		      if ( ( Fd_rx[1] != -1 ) && ( FD_ISSET(Fd_rx[1], &readmask ) ) )
			  Fd_read = Fd_rx[1];
		      else if ( ( Fd_rx[2] != -1 ) && ( FD_ISSET(Fd_rx[2], &readmask ) ) )
			  Fd_read = Fd_rx[2];		      		      
		      else if ( ( Fd_rx[3] != -1 ) && ( FD_ISSET(Fd_rx[3], &readmask ) ) )
			  Fd_read = Fd_rx[3];
		      else if ( ( Fd_rx[4] != -1 ) && ( FD_ISSET(Fd_rx[4], &readmask ) ) )
			  Fd_read = Fd_rx[4];
		      else if ( ( Fd_rx[5] != -1 ) && ( FD_ISSET(Fd_rx[5], &readmask ) ) )
			  Fd_read = Fd_rx[5];
		      else if ( ( Fd_rx[6] != -1 ) && ( FD_ISSET(Fd_rx[6], &readmask ) ) )
			  Fd_read = Fd_rx[6];
		      else if ( ( Fd_rx[7] != -1 ) && ( FD_ISSET(Fd_rx[7], &readmask ) ) )
			  Fd_read = Fd_rx[7];		      
		      else if ( ( Fd_rx[0] != -1 ) && ( FD_ISSET(Fd_rx[0], &readmask ) ) )
			  Fd_read = Fd_rx[0];		      
		  }
	      else if ( Fd_last_read == Fd_rx[1] )
		  {
		      if ( ( Fd_rx[2] != -1 ) && ( FD_ISSET(Fd_rx[2], &readmask ) ) )
			  Fd_read = Fd_rx[2];
		      else if ( ( Fd_rx[3] != -1 ) && ( FD_ISSET(Fd_rx[3], &readmask ) ) )
			  Fd_read = Fd_rx[3];
		      else if ( ( Fd_rx[4] != -1 ) && ( FD_ISSET(Fd_rx[4], &readmask ) ) )
			  Fd_read = Fd_rx[4];
		      else if ( ( Fd_rx[5] != -1 ) && ( FD_ISSET(Fd_rx[5], &readmask ) ) )
			  Fd_read = Fd_rx[5];
		      else if ( ( Fd_rx[6] != -1 ) && ( FD_ISSET(Fd_rx[6], &readmask ) ) )
			  Fd_read = Fd_rx[6];
		      else if ( ( Fd_rx[7] != -1 ) && ( FD_ISSET(Fd_rx[7], &readmask ) ) )
			  Fd_read = Fd_rx[7];		      		      
		      else if ( ( Fd_rx[0] != -1 ) && ( FD_ISSET(Fd_rx[0], &readmask ) ) )
			  Fd_read = Fd_rx[0];
		      else if ( ( Fd_rx[1] != -1 ) && ( FD_ISSET(Fd_rx[1], &readmask ) ) )
			  Fd_read = Fd_rx[1];		      
		  }
		  else if ( Fd_last_read == Fd_rx[2] )
		    {
		      if ( ( Fd_rx[3] != -1 ) && ( FD_ISSET(Fd_rx[3], &readmask ) ) )
			Fd_read = Fd_rx[3];
		      else if ( ( Fd_rx[4] != -1 ) && ( FD_ISSET(Fd_rx[4], &readmask ) ) )
			Fd_read = Fd_rx[4];
		      else if ( ( Fd_rx[5] != -1 ) && ( FD_ISSET(Fd_rx[5], &readmask ) ) )
			  Fd_read = Fd_rx[5];
		      else if ( ( Fd_rx[6] != -1 ) && ( FD_ISSET(Fd_rx[6], &readmask ) ) )
			  Fd_read = Fd_rx[7];		      		      		      
		      else if ( ( Fd_rx[7] != -1 ) && ( FD_ISSET(Fd_rx[7], &readmask ) ) )
			Fd_read = Fd_rx[7];
		      else if ( ( Fd_rx[0] != -1 ) && ( FD_ISSET(Fd_rx[0], &readmask ) ) )
			Fd_read = Fd_rx[0];
		      else if ( ( Fd_rx[1] != -1 ) && ( FD_ISSET(Fd_rx[1], &readmask ) ) )
			Fd_read = Fd_rx[1];
		      else if ( ( Fd_rx[2] != -1 ) && ( FD_ISSET(Fd_rx[2], &readmask ) ) )
			Fd_read = Fd_rx[2];
		    }              
		  else if ( Fd_last_read == Fd_rx[3] )
		    {
		      if ( ( Fd_rx[4] != -1 ) && ( FD_ISSET(Fd_rx[4], &readmask ) ) )
			Fd_read = Fd_rx[4];
		      else if ( ( Fd_rx[5] != -1 ) && ( FD_ISSET(Fd_rx[5], &readmask ) ) )
			Fd_read = Fd_rx[5];
		      else if ( ( Fd_rx[6] != -1 ) && ( FD_ISSET(Fd_rx[6], &readmask ) ) )
			  Fd_read = Fd_rx[6];
		      else if ( ( Fd_rx[7] != -1 ) && ( FD_ISSET(Fd_rx[7], &readmask ) ) )
			  Fd_read = Fd_rx[7];		      		      		      
		      else if ( ( Fd_rx[0] != -1 ) && ( FD_ISSET(Fd_rx[0], &readmask ) ) )
			Fd_read = Fd_rx[0];
		      else if ( ( Fd_rx[1] != -1 ) && ( FD_ISSET(Fd_rx[1], &readmask ) ) )
			Fd_read = Fd_rx[1];
		      else if ( ( Fd_rx[2] != -1 ) && ( FD_ISSET(Fd_rx[2], &readmask ) ) )
			Fd_read = Fd_rx[2];
		      else if ( ( Fd_rx[3] != -1 ) && ( FD_ISSET(Fd_rx[3], &readmask ) ) )
			Fd_read = Fd_rx[3];
		    }
		  else if ( Fd_last_read == Fd_rx[4] )
		    {
		      if ( ( Fd_rx[5] != -1 ) && ( FD_ISSET(Fd_rx[5], &readmask ) ) )
			Fd_read = Fd_rx[5];
		      else if ( ( Fd_rx[6] != -1 ) && ( FD_ISSET(Fd_rx[6], &readmask ) ) )
			  Fd_read = Fd_rx[6];
		      else if ( ( Fd_rx[7] != -1 ) && ( FD_ISSET(Fd_rx[7], &readmask ) ) )
			  Fd_read = Fd_rx[7];		      		      		      
		      else if ( ( Fd_rx[0] != -1 ) && ( FD_ISSET(Fd_rx[0], &readmask ) ) )
			Fd_read = Fd_rx[0];
		      else if ( ( Fd_rx[1] != -1 ) && ( FD_ISSET(Fd_rx[1], &readmask ) ) )
			Fd_read = Fd_rx[1];
		      else if ( ( Fd_rx[2] != -1 ) && ( FD_ISSET(Fd_rx[2], &readmask ) ) )
			Fd_read = Fd_rx[2];
		      else if ( ( Fd_rx[3] != -1 ) && ( FD_ISSET(Fd_rx[3], &readmask ) ) )
			Fd_read = Fd_rx[3];
		      else if ( ( Fd_rx[4] != -1 ) && ( FD_ISSET(Fd_rx[4], &readmask ) ) )
			Fd_read = Fd_rx[4];
		    }
		  else if ( Fd_last_read == Fd_rx[5] )
		    {
		      if ( ( Fd_rx[6] != -1 ) && ( FD_ISSET(Fd_rx[6], &readmask ) ) )
			Fd_read = Fd_rx[6];
		      else if ( ( Fd_rx[7] != -1 ) && ( FD_ISSET(Fd_rx[7], &readmask ) ) )
			  Fd_read = Fd_rx[7];
		      else if ( ( Fd_rx[0] != -1 ) && ( FD_ISSET(Fd_rx[0], &readmask ) ) )
			  Fd_read = Fd_rx[0];		      		      		      
		      else if ( ( Fd_rx[1] != -1 ) && ( FD_ISSET(Fd_rx[1], &readmask ) ) )
			Fd_read = Fd_rx[1];
		      else if ( ( Fd_rx[2] != -1 ) && ( FD_ISSET(Fd_rx[2], &readmask ) ) )
			Fd_read = Fd_rx[2];
		      else if ( ( Fd_rx[3] != -1 ) && ( FD_ISSET(Fd_rx[3], &readmask ) ) )
			Fd_read = Fd_rx[3];
		      else if ( ( Fd_rx[4] != -1 ) && ( FD_ISSET(Fd_rx[4], &readmask ) ) )
			Fd_read = Fd_rx[4];
		      else if ( ( Fd_rx[5] != -1 ) && ( FD_ISSET(Fd_rx[5], &readmask ) ) )
			Fd_read = Fd_rx[5];
		    }
		  else if ( Fd_last_read == Fd_rx[6] )
		    {
		      if ( ( Fd_rx[7] != -1 ) && ( FD_ISSET(Fd_rx[7], &readmask ) ) )
			Fd_read = Fd_rx[7];
		      else if ( ( Fd_rx[0] != -1 ) && ( FD_ISSET(Fd_rx[0], &readmask ) ) )
			  Fd_read = Fd_rx[0];
		      else if ( ( Fd_rx[1] != -1 ) && ( FD_ISSET(Fd_rx[1], &readmask ) ) )
			  Fd_read = Fd_rx[1];		      		      		      
		      else if ( ( Fd_rx[2] != -1 ) && ( FD_ISSET(Fd_rx[2], &readmask ) ) )
			Fd_read = Fd_rx[2];
		      else if ( ( Fd_rx[3] != -1 ) && ( FD_ISSET(Fd_rx[3], &readmask ) ) )
			Fd_read = Fd_rx[3];
		      else if ( ( Fd_rx[4] != -1 ) && ( FD_ISSET(Fd_rx[4], &readmask ) ) )
			Fd_read = Fd_rx[4];
		      else if ( ( Fd_rx[5] != -1 ) && ( FD_ISSET(Fd_rx[5], &readmask ) ) )
			Fd_read = Fd_rx[5];
		      else if ( ( Fd_rx[6] != -1 ) && ( FD_ISSET(Fd_rx[6], &readmask ) ) )
			Fd_read = Fd_rx[6];
		    }
		  else if ( Fd_last_read == Fd_rx[7] )
		    {
		      if ( ( Fd_rx[0] != -1 ) && ( FD_ISSET(Fd_rx[0], &readmask ) ) )
			Fd_read = Fd_rx[7];
		      else if ( ( Fd_rx[1] != -1 ) && ( FD_ISSET(Fd_rx[1], &readmask ) ) )
			  Fd_read = Fd_rx[1];
		      else if ( ( Fd_rx[2] != -1 ) && ( FD_ISSET(Fd_rx[2], &readmask ) ) )
			  Fd_read = Fd_rx[2];		      		      		      
		      else if ( ( Fd_rx[3] != -1 ) && ( FD_ISSET(Fd_rx[3], &readmask ) ) )
			Fd_read = Fd_rx[3];
		      else if ( ( Fd_rx[4] != -1 ) && ( FD_ISSET(Fd_rx[4], &readmask ) ) )
			Fd_read = Fd_rx[4];
		      else if ( ( Fd_rx[5] != -1 ) && ( FD_ISSET(Fd_rx[5], &readmask ) ) )
			Fd_read = Fd_rx[5];
		      else if ( ( Fd_rx[6] != -1 ) && ( FD_ISSET(Fd_rx[6], &readmask ) ) )
			Fd_read = Fd_rx[6];
		      else if ( ( Fd_rx[7] != -1 ) && ( FD_ISSET(Fd_rx[7], &readmask ) ) )
			Fd_read = Fd_rx[7];
		    }		      			      	      	      	      


	  }

        if ( Fd_read == Fd_rx[0] || Fd_read == Fd_rx[1] || Fd_read == Fd_rx[2] || Fd_read == Fd_rx[3]
             || Fd_read == Fd_rx[4] || Fd_read == Fd_rx[5]
             || Fd_read == Fd_rx[6] || Fd_read == Fd_rx[7] ) 
          Fd_last_read = Fd_read;
        
      }

      // set link type

      
      for ( i = 0; i < MAX_LINKS; i++ )
        {
          if (Fd_read == Fd_rx[i])
            break;
        }
      
      if ( !strcmp(Link_type[i], "MMP") )
        link_type = MMP_LINK_TYPE;
      else if ( !strcmp(Link_type[i], "GPRS") )
	link_type = GPRS_LINK_TYPE;
      else
        link_type = STAR2000_LINK_TYPE;
      
      itm_type = 0;
      if ( link_type == GPRS_LINK_TYPE )
	{
	  bzero( msg_buffer, MAX_BUF );
	  bzero( &Rxmsg, sizeof(MSG_QUEUE_HDR) );	  
	  udp_len = 0;
	  udp_gprs_receive( Fd_read, msg_buffer, &udp_len );
	  len = udp_len;
	  Rxmsg.size = len;
	  rstate = R6;
	}
      
      switch (rstate)
      {
      case R1:			/* (blocking) read for NUL */
      case R2:			/* read for SOH after read NUL */

	bzero( msg_buffer, MAX_BUF );
	bzero( &Rxmsg, sizeof(MSG_QUEUE_HDR) );
	 fch_ptr = fch_buffer;

	 if (Fd_read > 0)
	   if ((status = alc_readn(Fd_read, fch_ptr, FCH_LEN)) == FCH_LEN)
	    rstate = (*fch_ptr == SOH) ? R3 : R1;
	 else
	 {
	    fprintf(stderr,
		    "**ALC** R2: Error reading SOH from port (%d)\n",
		    errno);
	    rstate = R1;
	 }

	 break;

      case R3:			/* read for line packet after read SOH */

	 /* read sequence number */
	 seq_ptr = seq_buffer;

 	 if ((status = alc_readn(Fd_read, seq_ptr, SEQ_NO_SIZE)) > 0)
	    sq = ahtoid(seq_buffer);
	 else
	 {
	    fprintf(stderr,
		    "**ALC** R3: Error reading SQ # from port (%d)\n",
		    errno);
	    rstate = R1;
	    break;
	 }

	 /* read length */
	 len_ptr = len_buffer;

	 if ((status = alc_readn(Fd_read, len_ptr, PKT_LEN_SIZE)) == PKT_LEN_SIZE)
	    len = ahtoid(len_buffer);
	 else
	 {
	    fprintf(stderr,
		    "**ALC** R3: Error reading length from port (%d)\n",
		    errno);
	    rstate = R1;
	    break;
	 }

         if (len > MAX_BUF) 
         {
            fprintf(stderr, "Packet length read error. Length = %d \n", len);
            rstate = R1;
            break;
         }

	 /* read message */
	 msg_ptr = msg_buffer;

	 if ((status = alc_readn(Fd_read, msg_ptr, len)) == len)
	    rstate = R4;
	 else
	 {
	    fprintf(stderr,
		    "**ALC** R3: Error reading message from port (%d)\n",
		    errno);
	    rstate = R1;
	 }

	 msg_buffer[len] = 0;	/* NUL-terminate msg */
	 break;

      case R4:			/* read for ETX after read line packet */

	 fch_ptr = fch_buffer;

	 if ((status = alc_readn(Fd_read, fch_ptr, FCH_LEN)) == FCH_LEN)
	 {
	    if (*fch_ptr == ETX)
	       rstate = R5;
	    else
	    {
	       /*
	        * * missing etx
	        */
	       ++alc_ptr->rx.num_rx_bad;
	       fprintf(stderr, "**ALC** R4: Bad packet (%02x)\n", sq); 
	       rstate = R1;
	    }
	 } else
	 {
	    fprintf(stderr,
		    "**ALC** R4: Error reading ETX from port (%d)\n",
		    errno);
	    rstate = R1;
	 }
	 break;

      case R5:			
      case R6:			/* send msg to inbound queue; check packet
				 * sequence */
	 strcpy(Rxmsg.route_id, msg_buffer);

	  if ( Rxmsg.type == '2' && !(strncmp(Rxmsg.p_fields, "30000", 5) ) )
	    {
	      memcpy( Rxmsg.text, (msg_buffer + MSG_HDR_LEN), len - MSG_HDR_LEN );
	      text_len = len - MSG_HDR_LEN;
#ifdef FOO
	      if ( Rxmsg.text[text_len-1] != 'E' )
		{
		  Rxmsg.text[text_len] = '%';
		  Rxmsg.text[text_len + 1] = 'E';
		  text_len += 2;
		}
#endif
	    }
	  else
	    {
	      /* get stats on message size */
	      text_len = strlen(Rxmsg.text);
	    }
	  
	 blks = text_len / BLK_SZ;	/* get no of blks */
	 if ((text_len % BLK_SZ) > 0)
	    ++blks;
         if (blks >= NUM_BLKS)
            blks = NUM_BLKS - 1;
	 alc_ptr->rx.num_rx_msgs[blks]++;
	 
	 Tmsg = &Rxmsg;
	 
         len = MSG_HDR_LEN + text_len + 1;
	 if ( time((long *) 0) > midnight )
	     {
	       rename( TRACE_MSG_FILE, trace_file );
	       nowtime = (long)time((long *)0);
	       tm_ptr = localtime(&nowtime);
	       sprintf(trace_file,"%s.%02d%02d%02d", TRACE_MSG_FILE,
		       tm_ptr->tm_year % 100, tm_ptr->tm_mon + 1, tm_ptr->tm_mday);

	       tm_ptr->tm_sec = 59;
	       tm_ptr->tm_min = 59;
	       tm_ptr->tm_hour = 23;
	       midnight = tm_to_time( tm_ptr );
	       midnight += 1;
	     }

	itm_type = 0;	
	 if ((alc_ptr->trace == 'Y') && ((strncmp(Rxmsg.route_id, alc_ptr->trace_mid, 5) == 0) ||
					 (strncmp("ALL", alc_ptr->trace_mid, 3) == 0))
	     && (Rxmsg.route_id[0] != '\0') && (strncmp(Rxmsg.route_id, "00F0", 4)))
	   {
	     Tmsg = &Rxmsg;
	     
	      /** This section will insure adequate disk space for the trace
	         functions.  Will toggle trace if disk space available is
		 less than the DISK_THRESHOLD                             **/

	     
	     if ((trace_logfile = fopen(trace_file, "a")) == NULL)
	       fprintf(stderr, "Error opening trace file \n");

	     tmtime = time((long *) 0);
	     get_arg_date(tmtime, DateBuff);
	     stime = ctime(&tmtime);
	     fprintf(trace_logfile, "%.2s", DateBuff);
	     fprintf(trace_logfile, "%.2s", &DateBuff[3]);
	     fprintf(trace_logfile, "%.2s ", &DateBuff[6]);
	     fprintf(trace_logfile, "%.2s", &stime[11]);
	     fprintf(trace_logfile, "%.2s", &stime[14]);
	     fprintf(trace_logfile, "%.2s", &stime[17]);
	     
	     fprintf(trace_logfile, " I %.03d ", inb_cnt++);
	     if (inb_cnt > 999)
	       inb_cnt = 1;

	     for ( i = 0; i < MAX_LINKS; i++ )
	       {
		 if ( Fd_read == Fd_rx[i] )
		   {
		     fprintf(trace_logfile, "%d ", i + 1);
		     break;
		   }
	       }


	     strncpy(Tbuf, Tmsg->route_id, ROUTE_ID_SIZE);
	     Tbuf[ROUTE_ID_SIZE] = '\0';
	     fprintf(trace_logfile, "%s ", Tbuf);

	     strncpy(Tbuf, Tmsg->pkt_id, PKT_ID_SIZE);
	     Tbuf[PKT_ID_SIZE] = '\0';
	     fprintf(trace_logfile, "%s ", Tbuf);	/* ==tag, (8 bits) */
	     
	     fprintf(trace_logfile, "%c", Tmsg->type);	/* indicates MMP message
							 * type */
	     strncpy(Tbuf, Tmsg->p_fields, P_SIZE);
	     Tbuf[P_SIZE] = '\0';
	     fprintf(trace_logfile, "%s ", Tbuf);	/* indicates P fields for MMP */
	     Tbuf[MMP_TEXT_SIZE] = '\0';
	     DescriptBuff[0] = '\0';
	     switch (Tmsg->type)
	       {
	       case '4':
		 sprintf(DescriptBuff, " ");
		 if (Tmsg->p_fields[0] == 'H')
		   sprintf(DescriptBuff,"HACK");
		 if (Tmsg->p_fields[0] == 'G')
		   sprintf(DescriptBuff, "NOACK");
		 if (Tmsg->p_fields[0] == '0')
		   sprintf(DescriptBuff, "ACK");
		 if (Tmsg->p_fields[0] == '2')
		   sprintf(DescriptBuff, "TERM_BUFF_FUL");
		 break;
	       case '2':
		 sprintf(DescriptBuff, " ");
		 switch(Tmsg->p_fields[0])
		   {
		   case '2':
		     sprintf(DescriptBuff, "KELA_RQST");
		     break;
		   case '1':
		     switch (Tmsg->p_fields[1])
		       {
		       case '0':
			 sprintf(DescriptBuff, "SIGN_IN");
			 break;
		       case '1':
			 sprintf(DescriptBuff, "BOOK_IN");
			 break;
		       case '2':
			 sprintf(DescriptBuff, "SOON_TO_CLR");
			 break;
		       case '3':
			 sprintf(DescriptBuff, "MSG_FROM_DRV");
			 break;
		       case '4':
			 sprintf(DescriptBuff, "BID");
			 break;
		       case '5':
			 sprintf(DescriptBuff, "COND_BOOK_IN");
			 break;
		       case '6':
			 sprintf(DescriptBuff, "BOOK_OFF");
			 break;
		       case '7':
			 sprintf(DescriptBuff, "ETA");
			 break;
		       case '8':
			 sprintf(DescriptBuff, "ZONE_INQ");
			 break;
		       case '9':
			 sprintf(DescriptBuff, "NO_SHOW");
			 break;
		       default:
			 break;
		       }
		     break;
		   case '4':
		     sprintf(DescriptBuff, "POLYGON_CHECKSUM");
		     break;
		   case '5':
			 sprintf(DescriptBuff, "STAND_NOTIFY");
			 break;
		   case '3':
                     sprintf(DescriptBuff,"ITM");
		     itm_type = hex2int(Tmsg->text, 2);
		     switch (itm_type)
		       {
		       case CCP_MSG_FROM_ITM:
			 strcat(DescriptBuff, "-CCP_IN");
			 break;			 
		       case RQ_CARD_VALIDNESS_1: 
			 strcat(DescriptBuff, "-RQ_CARD_VALID_1");
			 break;
		       case RQ_CARD_VALIDNESS_2:
			 strcat(DescriptBuff, "-RQ_CARD_VALID_2");
			 break;
		       case SIGN_ON_INFO_VERS1:
			 strcat(DescriptBuff, "-OLD_SIGN_ON_INFO");
			 break;
		       case NON_SUPPORTED_1:
			 strcat(DescriptBuff, "-PASS_ON_BOARD");
			 break;
		       case BREAK_START:
			 strcat(DescriptBuff, "-BREAK_START");
			 break;
		       case NOSHOW:
			 strcat(DescriptBuff, "-BOMTRIP");
			 break;
		       case BREAK_STOP:
			 strcat(DescriptBuff, "-BREAK_STOP");
			 break;
		       case SIGN_ON_INFO_VERS2:
			 strcat(DescriptBuff, "-NEW_SIGN_ON_INFO");
			 break;
		       case SHIFT_START:
			 strcat(DescriptBuff, "-SHIFT_START");
			 break;
		       case SHIFT_STOP:
			 strcat(DescriptBuff, "-SHIFT_STOP");
			 break;
		       case TRIP_DATA:
			 strcat(DescriptBuff, "-TRIP_DATA");
			 break;
		       case TRIPFIFO_DATA:
			 strcat(DescriptBuff, "-TRIP_FIFO_DATA");
			 break;
		       case MULT_TRIPFIFO_DATA:
			 strcat(DescriptBuff, "-TRIP_FIFO_DATA");
			 break;
			 
		       case TRIP_COMPLETE:
			 strcat(DescriptBuff, "-TRIP_COMP");
			 break;
		       case TRIP_STOP:
			 strcat(DescriptBuff, "-TRIP_STOP");
			 break;
		       case METER_FAULT:
			 strcat(DescriptBuff, "-METER_FAULT");
			 break;
		       case PRINT_TEXT_ACK:
			 strcat(DescriptBuff, "-PRNT_TXT_ACK");
			 break;
		       case METER_ACTION_ACK:
			 strcat(DescriptBuff, "-METER_ACT_ACK");
			 break;
		       case RQ_CARD:
			 strcat(DescriptBuff, "-RQ_CARD");
			 break;
		       case RQ_CARDVAL:
			 strcat(DescriptBuff, "-RQ_CARD_VAL");
			 break;
		       case FTJ_VALID:
			   strcat(DescriptBuff, "-FTJ_VALID");
			   break;
		       case NEW_KEYS:
			 strcat(DescriptBuff, "-NEW_KEYS");
			 break;
		       case TX_PARAMETERS:
			 strcat(DescriptBuff, "-TX_PARAMS");
			 break;
		       case METER_STATUS:
			 strcat(DescriptBuff, "-METER_STATUS");
			 break;
		       case CONTROL_REGS:
			 strcat(DescriptBuff, "-CTRL_REGS");
			 break;
		       default:
			 if ( Tmsg->p_fields[1] == '1' )
			   strcpy(DescriptBuff, "GPS_UPDATE");
			 else
			   strcat(DescriptBuff, "-UNKNOWN");
			 break;
		       } 
		     break;
		   }
		 break;
	       case '3':
		 sprintf(DescriptBuff, " ");
		 switch (Tmsg->p_fields[0])
		   {
		   case '3':
		     switch (Tmsg->p_fields[1])
		       {
		       case '1':
			 sprintf(DescriptBuff, "TRIP_ACCEPT");
			 break;
		       case '2':
			 sprintf(DescriptBuff, "TRIP_REJECT");
			 break;
		       case '3':
			 sprintf(DescriptBuff, "FLAG_REQ");
			 break;
		       case '4':
			 sprintf(DescriptBuff, "REQ_TALK");
			 break;
		       case '5':
			 sprintf(DescriptBuff, "SIGN_OFF");
			 break;
		       case '6':
			 sprintf(DescriptBuff, "QP");
			 break;
		       case '7':
			 sprintf(DescriptBuff, "BREAK");
			 break;
		       case '8':
			 sprintf(DescriptBuff, "LOAD");
			 break;
		       case '9':
			 sprintf(DescriptBuff, "UNLOAD");
			 break;
		       case 'A':
			 sprintf(DescriptBuff, "CALL_OUT");
			 break;
		       case 'b':
		       case 'B':
			 sprintf(DescriptBuff, "ADV_ARRIVE");
			 break;
		       case 'E':
			 sprintf(DescriptBuff, "MDT_PWR_UP");
			 break;
		       case 'f':
		       case 'F':
			 sprintf(DescriptBuff, "METER_FLT");
			 break;
		       }
		     break;
		   case '1':
		     sprintf(DescriptBuff, "METER_");
		     if (Tmsg->p_fields[1] == '1')
		       sprintf(DescriptBuff, "ON");
		     else
		       sprintf(DescriptBuff, "OFF");
		     break;
		   case '0':
		     sprintf(DescriptBuff, "EMERG");
		     break;
		   }			/* end case p_field[0] */
		 break;
	       }
	     fprintf(trace_logfile, "%20s ", DescriptBuff);
	     strncpy(Tbuf, Tmsg->text, MMP_TEXT_SIZE);
	     Tbuf[MMP_TEXT_SIZE] = '\0';

	     if ( itm_type != CCP_MSG_FROM_ITM )
	       fprintf(trace_logfile, "%s\n", Tbuf);
	     else
	       {
		 text_len = 0;
		 for ( tstate = T1;; )
		   {
		     if ( tstate == T1 )
		       {
			 if ( isprint(Tmsg->text[text_len]) )
			   fprintf(trace_logfile, "%c", Tmsg->text[text_len]);
			 else
			   fprintf(trace_logfile, "[%2.2x]", Tmsg->text[text_len] & 0x000000ff);
			 if ( Tmsg->text[text_len] == '%' )
			   tstate = T2;
			 ++text_len;
		       }
		     else
		       {
			 if ( isprint(Tmsg->text[text_len]) )
			   fprintf(trace_logfile, "%c", Tmsg->text[text_len]);
			 else
			   fprintf(trace_logfile, "[%2.2x]", Tmsg->text[text_len]& 0x000000ff);
			 if ( ( Tmsg->text[text_len] == 'E' ) && ( Tmsg->text[text_len+1] == '\0' ) && ( Tmsg->text[text_len+2] == '\0') )
			   break;
			 else
			   {
			     tstate = T1;
			     ++text_len;
			   }
		       }
		   }
		 ++text_len;
		 fprintf(trace_logfile, "\n");
	       }
	     
	     fclose(trace_logfile);
	   }	     

	 
         /* discard normal acks */
         if ((Rxmsg.type == '4') && (!strncmp(Rxmsg.pkt_id, "00", 2)))
	   goto no_xmit;

	 Rxmsg.text[text_len] = CHECK_CHAR;
	 Rxmsg.text[text_len + 1] = '\0';
	 len++;
	 Rxmsg.size = len;

	 for ( i = 0; i < MAX_LINKS; i++ )
	   {
	     if ( Fd_read == Fd_rx[i] )
	       {
		 Rxmsg.delvry_prio = 0x30 + i + 1;
		 break;
	       }
	   }

         // Set the link type in MMP message -- reusing the 'persist' field on inbound message
         Rxmsg.persist = link_type;

	 // Trap POLYGON_CHECKSUM messages and direct to ZonePolygonManager
	 //	 if ( Rxmsg.type == '2' && Rxmsg.p_fields[0] == '4' )
	 //  {


	 // }
         // Trap MSG_FROM_DRV messages 97/98 and redirect to MPK Bridge
	if ( Rxmsg.type == '2' && Rxmsg.p_fields[0] == '1' && Rxmsg.p_fields[1] == '3' &&
	      ((!strncmp(Rxmsg.text, "97", 2)) || ((!strncmp(Rxmsg.text, "99", 2))) || (!strncmp(Rxmsg.text, "98", 2))) )
	   {
	     bzero(udp_buf, sizeof(udp_buf));
	     sprintf(udp_buf, "%.5s!%.2s", Rxmsg.route_id, Rxmsg.text);
	     bzero((char *)&serv_addr, sizeof(serv_addr));
	     serv_addr.sin_family = AF_INET;
	     hp = gethostbyname("ws226.taksihelsinki.fi");
	     
	     
	     if ( hp != NULL )
	       {
		 bcopy(hp->h_addr, (char *)&serv_addr.sin_addr, hp->h_length);
		 serv_addr.sin_port = htons(6234);
		 if ( ( sockfd = socket(AF_INET, SOCK_DGRAM, 0 ) ) != 0 )
		   {
		     bzero((char *)&cli_addr, sizeof(cli_addr));
		     cli_addr.sin_family = AF_INET;
		     cli_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		     cli_addr.sin_port = htons(0);
		     if ( bind(sockfd, (struct sockaddr *) &cli_addr, sizeof(cli_addr) ) >= 0 )
		       res = sendto(sockfd, udp_buf, strlen(udp_buf), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
	   
		     close(sockfd);
		   }

		 serv_addr.sin_port = htons(6233);
		 if ( ( sockfd = socket(AF_INET, SOCK_DGRAM, 0 ) ) != 0 )
		   {
		     bzero((char *)&cli_addr, sizeof(cli_addr));
		     cli_addr.sin_family = AF_INET;
		     cli_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		     cli_addr.sin_port = htons(0);
		     if ( bind(sockfd, (struct sockaddr *) &cli_addr, sizeof(cli_addr) ) >= 0 )
		       res = sendto(sockfd, udp_buf, strlen(udp_buf), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
	   
		     close(sockfd);
		   }		 
	       }
	     if ( (strncmp(Rxmsg.text, "99", 2) ) ) //Forward all except for the 99 messages...workaround to DDS MDT problem
	       status = write(inb_fd, &Rxmsg, len + sizeof(Rxmsg.size));
	   }
	 else
	   status = write(inb_fd, &Rxmsg, len + sizeof(Rxmsg.size));
	 
	 if (Fd_last_read == Fd_rx[0])
	   alc_ptr->rx.num_hl1++;
	 else if ( Fd_last_read == Fd_rx[1] )
	   alc_ptr->rx.num_hl2++;
	 else if ( Fd_last_read == Fd_rx[2] )
	   alc_ptr->rx.num_hl3++;
	 else if ( Fd_last_read == Fd_rx[3] )
	   alc_ptr->rx.num_hl4++;
	 else if ( Fd_last_read == Fd_rx[4] )
	   alc_ptr->rx.num_hl5++;
	 else if ( Fd_last_read == Fd_rx[5] )
	   alc_ptr->rx.num_hl6++;
	 else if ( Fd_last_read == Fd_rx[6] )
	   alc_ptr->rx.num_hl7++;
	 else if ( Fd_last_read == Fd_rx[7] )
	   alc_ptr->rx.num_hl8++;	 
	 
no_xmit:
	 /* check packet sequence */
	 if ((!past_first_rxd_msg_hl1) ||
	     (!past_first_rxd_msg_hl2) ||
	     (!past_first_rxd_msg_hl3) ||
	     (!past_first_rxd_msg_hl4) ||
	     (!past_first_rxd_msg_hl5) ||
	     (!past_first_rxd_msg_hl6) ||
	     (!past_first_rxd_msg_hl7) ||
	     (!past_first_rxd_msg_hl8) )
	 {
	    if (Fd_read == Fd_rx[0])
	    {
	    	past_first_rxd_msg_hl1 = TRUE;
		prev_sq[0] = sq;
	    }
	    else if (Fd_read == Fd_rx[1])
	    {
	        past_first_rxd_msg_hl2 = TRUE;
	        prev_sq[1] = sq;
	    }
	    else if (Fd_read == Fd_rx[2])
	    {
	        past_first_rxd_msg_hl3 = TRUE;
	        prev_sq[2] = sq;
	    }
	    else if (Fd_read == Fd_rx[3])
	    {
	        past_first_rxd_msg_hl4 = TRUE;
	        prev_sq[3] = sq;
	    }
	    else if (Fd_read == Fd_rx[4])
	    {
	        past_first_rxd_msg_hl5 = TRUE;
	        prev_sq[4] = sq;
	    }
	    else if (Fd_read == Fd_rx[5])
	    {
	        past_first_rxd_msg_hl6 = TRUE;
	        prev_sq[5] = sq;
	    }
	    else if (Fd_read == Fd_rx[6])
	    {
	        past_first_rxd_msg_hl7 = TRUE;
	        prev_sq[6] = sq;
	    }
	    else if (Fd_read == Fd_rx[7])
	    {
	        past_first_rxd_msg_hl8 = TRUE;
	        prev_sq[7] = sq;
	    }	    
	 }
	 else 
	 {
	    if (Fd_read == Fd_rx[0])
	    {
            	if (sq != (prev_sq[0] + 1) % (max_sq + 1))
	    	{
		    ++alc_ptr->rx.num_rx_outseq;
		    old_sq = (prev_sq[0] + 1) % (max_sq +1);
		    if (sq > old_sq)
		       missed_sq = sq - old_sq;
		    else
		       missed_sq = sq + (max_sq - old_sq);
		    alc_ptr->rx.num_rx_msg_outseq += missed_sq;
		}
		prev_sq[0] = sq;
	     }
	    else if ( Fd_read == Fd_rx[1] )
	     {
		if (sq != (prev_sq[1] + 1) % (max_sq + 1))
		{
		     ++alc_ptr->rx.num_rx_outseq;
		     old_sq = (prev_sq[1] + 1) % (max_sq + 1);
		     if (sq > old_sq)
			missed_sq = sq - old_sq;
		     else
			missed_sq = sq + (max_sq - old_sq);
		     alc_ptr->rx.num_rx_msg_outseq += missed_sq;
	 	}
		prev_sq[1] = sq;
	      }
	    else if ( Fd_read == Fd_rx[2] )
	     {
		if (sq != (prev_sq[2] + 1) % (max_sq + 1))
		{
		     ++alc_ptr->rx.num_rx_outseq;
		     old_sq = (prev_sq[2] + 1) % (max_sq + 1);
		     if (sq > old_sq)
			missed_sq = sq - old_sq;
		     else
			missed_sq = sq + (max_sq - old_sq);
		     alc_ptr->rx.num_rx_msg_outseq += missed_sq;
	 	}
		prev_sq[2] = sq;
	      }
	    else if ( Fd_read == Fd_rx[3] )
	     {
		if (sq != (prev_sq[3] + 1) % (max_sq + 1))
		{
		     ++alc_ptr->rx.num_rx_outseq;
		     old_sq = (prev_sq[3] + 1) % (max_sq + 1);
		     if (sq > old_sq)
			missed_sq = sq - old_sq;
		     else
			missed_sq = sq + (max_sq - old_sq);
		     alc_ptr->rx.num_rx_msg_outseq += missed_sq;
	 	}
		prev_sq[3] = sq;
	      }
	    else if ( Fd_read == Fd_rx[4] )
	     {
		if (sq != (prev_sq[4] + 1) % (max_sq + 1))
		{
		     ++alc_ptr->rx.num_rx_outseq;
		     old_sq = (prev_sq[4] + 1) % (max_sq + 1);
		     if (sq > old_sq)
			missed_sq = sq - old_sq;
		     else
			missed_sq = sq + (max_sq - old_sq);
		     alc_ptr->rx.num_rx_msg_outseq += missed_sq;
	 	}
		prev_sq[4] = sq;
	      }
	    else if ( Fd_read == Fd_rx[5] )
	     {
		if (sq != (prev_sq[5] + 1) % (max_sq + 1))
		{
		     ++alc_ptr->rx.num_rx_outseq;
		     old_sq = (prev_sq[5] + 1) % (max_sq + 1);
		     if (sq > old_sq)
			missed_sq = sq - old_sq;
		     else
			missed_sq = sq + (max_sq - old_sq);
		     alc_ptr->rx.num_rx_msg_outseq += missed_sq;
	 	}
		prev_sq[5] = sq;
	      }
	    else if ( Fd_read == Fd_rx[6] )
	     {
		if (sq != (prev_sq[6] + 1) % (max_sq + 1))
		{
		     ++alc_ptr->rx.num_rx_outseq;
		     old_sq = (prev_sq[6] + 1) % (max_sq + 1);
		     if (sq > old_sq)
			missed_sq = sq - old_sq;
		     else
			missed_sq = sq + (max_sq - old_sq);
		     alc_ptr->rx.num_rx_msg_outseq += missed_sq;
	 	}
		prev_sq[6] = sq;
	      }
	    else if ( Fd_read == Fd_rx[7] )
	     {
		if (sq != (prev_sq[7] + 1) % (max_sq + 1))
		{
		     ++alc_ptr->rx.num_rx_outseq;
		     old_sq = (prev_sq[7] + 1) % (max_sq + 1);
		     if (sq > old_sq)
			missed_sq = sq - old_sq;
		     else
			missed_sq = sq + (max_sq - old_sq);
		     alc_ptr->rx.num_rx_msg_outseq += missed_sq;
	 	}
		prev_sq[7] = sq;
	      }	    

	 }
	 rstate = R1;
	 break;

      default:			/* invalid state */
	 fprintf(stderr, "**ALC** [alc_rx]: Reached an invalid state\n");
	 break;
      }
   }
}


/********************************************************************
** ahtoid() analogous to atoi(), this function will convert a
**          string of ascii hex characters to an integer decimal
********************************************************************/
#include <ctype.h>

ahtoid(ah)
   CHAR  ah[];
{
   INT   base, id, digit, length;
   CHAR *hex;

   /* get hex string length */
   for (length = 0; isxdigit(ah[length]); length++);

   /* calc highest base */
   for (base = 1; length > 1; length--, base *= 16);

   for (hex = ah, id = 0; base > 0; hex++, id += digit * base, base /= 16)
   {
      /* convert one hex char to a dec int (digit) */
      if ('0' <= *hex && *hex <= '9')
	 digit = *hex - '0';
      else if ('A' <= *hex && *hex <= 'F')
	 digit = *hex - ('A' - 10);
      else if ('a' <= *hex && *hex <= 'f')
	 digit = *hex - ('a' - 10);
      else
	 digit = 0;
   }

   return (id);
}

/********************************************************************
	alc_readn() -analogous to read() but will read n characters or
				 return an error. Needs to sense for presence of
				 "carrier detect".
********************************************************************/

alc_readn(fd, buf, ch_req)
   INT   fd;
   CHAR *buf;
   INT   ch_req;
{
   INT   status, ch_rxd;
   CHAR *buf_ptr;
   INT   count;
   CHAR  line_prob;

   for (buf_ptr = buf, ch_rxd = 0; ch_req > 0;)
   {
      Err_toggle = 1;		/* only output error message ONCE */
      count = 0;
      line_prob = TRUE;
      while (line_prob == TRUE)
      {
	 status = read(fd, buf_ptr, ch_req);
	 if (status > 0)
	 {
	    line_prob = FALSE;
	    ch_req -= status;
	    ch_rxd += status;

	    buf_ptr += status;	/* advance buf_ptr past last char read */
	 } else
	 {
	    if (status <= -1)
	       return (status);
	    else
	    {
	       /*
	        * * CARRIER DETECT gone
	        */
	       line_prob = TRUE;
	       if (Err_toggle)
	       {
		  Err_toggle = 0;
		  fprintf(stderr, "**ALC** read port , carrier gone\n");
	       }
	       if (++count == Readretry)
	       {
		  fprintf(stderr,
			  "**ALC** read port exit after [%d] retries\n",
			  Readretry);

		  exit(1);
	       }
	       sleep(Readretryinterval);
	    }
	 }
      }
   }

   return (ch_rxd);
}


/********************************************************************
	alc_write -write a message to a tty port, this module only returns if
	           SUCCESSFUL else the process will exit.
********************************************************************/
alc_write(fd, msg, len)
   INT   fd;
   CHAR *msg;
   INT   len;
{
   INT   count;
   INT   status;

   Err_toggle = 1;		/* only output error message ONCE */
   count = 0;

   /*
    * * set an alarm to get us outof "xoff" position, ie ACC has sent * RNG a
    * xoff and RNG has not received Xon for x secs.
    */
   alarm(Writetimeout);
   while ((status = write(fd, msg, len)) <= 0)
   {
      /*
       * * write failed for some reason
       */
      if (Err_toggle)
      {
	 if (status == -1)
	 {
	    fprintf(stderr,
		    "**ALC** Port write Error: (errno = %d)\n",
		    errno);
	 } else
	 {
	    /*
	     * * CARRIER DETECT not present
	     */
	    fprintf(stderr,
		    "**ALC** Write Port , check cabling\n");
	 }
	 Err_toggle = 0;
      }
      /*
       * * wait awhile before trying to write again
       */
      sleep(Writeretryinterval);

      /*
       * * should we give up?
       */
      if (++count == Writeretry)
      {
	 fprintf(stderr,
		 "**ALC** Can't write to port after [%d] retries\n",
		  Writeretry);

	 exit(1);
      }
      /*
       * * if reason for exit outof "write" is ALARM interrupt * then arm
       * alarm again.
       */
      if (errno == EINTR)
	 alarm(Writetimeout);
   }
   alarm(0);

}

/********************************************************************
	alc_open -open a tty port, this module only returns if
	          SUCCESSFUL else the process will exit.
	          Alarm timeout will ONLY occur if "carrier detect" is
	          not present!!!
********************************************************************/
alc_open(port_name, fd)
   CHAR *port_name;
   INT  *fd;
{
   INT   count;

   Err_toggle = 1;		/* only output error message ONCE */
   count = 0;
   alarm(Opentimeout);
   while ((*fd = open(port_name, O_RDWR | O_NDELAY)) == -1)
   {
      if (Err_toggle)
      {
	 fprintf(stderr,
		 "**ALC** Port Open Error: [%s] (errno = %d)\n",
		 port_name, errno);
	 Err_toggle = 0;
      }
      /*
       * * wait awhile before trying to open port
       */
      sleep(Openretryinterval);

      /*
       * * should we give up?
       */
      if (++count == Openretry)
      {
	 fprintf(stderr,
		 "**ALC** Can't open port [%s] after [%d] retries\n",
		 port_name, Openretry);
	 exit(1);
      }
      /*
       * * if reason for exit outof "open" is interrupt * then reset alarm.
       */
      if (errno == EINTR)
	 alarm(Opentimeout);
   }
   alarm(0);
}

/********************************************************************
	sig_int -exit/close routine
********************************************************************/

LOCAL VOIDT 
sig_int()
{
  int i;
  
  for ( i = 0; i < MAX_LINKS; i++ )
    {
      if ( ( Fd_tx[i] != -1 ) && ( Fd_tx[i] == Fd_rx[i] ) )
	close( Fd_tx[i] );
      else if ( Fd_tx[i] != -1 )
	{
	  close( Fd_tx[i] );
	  close( Fd_rx[i] );
	}
    }  

   exit(1);
}

/********************************************************************
	sig_term -
********************************************************************/
LOCAL VOIDT 
sig_term(sig)
   INT   sig;
{
  int i;
  
  for ( i = 0; i < MAX_LINKS; i++ )
    {
      if ( ( Fd_tx[i] != -1 ) && ( Fd_tx[i] == Fd_rx[i] ) )
	close( Fd_tx[i] );
      else if ( Fd_tx[i] != -1 )
	{
	  close( Fd_tx[i] );
	  close( Fd_rx[i] );
	}
    }
  
   exit(1);
}



void
outb_msg_recover()
/* attempt to get back in sync from the pipe */
{
  char ch;
  int num;

  num = 10;
  ch = ' ';
  while((ch != CHECK_CHAR) && (num > 0))
     num = read(outb_fd, &ch,1);
  read(outb_fd, &ch, 1);
}              /* end outb_msg_recover */

int
get_outb_msg(msg)
   char *msg;
{
   int   num;
   char *ptr;
   char size[sizeof(int) + 1];
   int msg_size;
   MSG_QUEUE_HDR  *Tmsg;   

   /* read in the size n the priority field*/
   num = read(outb_fd, &msg_size, sizeof(int));
   if (num <= 0)
      return(-1);
 
   msg += sizeof(int);   /* because we dont use the size  field */
 
   /* got a message folks */
   /* msg_size = atoi(size); */
   if (msg_size <= 0)
      return(-1);
   if (msg_size > MAX_MSG_SIZE)

   {
     /* printf("error 3 size %s\n", size); */
     outb_msg_recover();
     return(-1);
   }

   num = read(outb_fd, msg, msg_size);

   if (msg[num - 2] != CHECK_CHAR)
   {
      /* printf("eror 1 ch is %c\n",msg[num - 2]);*/ /* DEBUG */
      outb_msg_recover();
      return(-1);
   }

   if (num != msg_size)
   {
      /* printf("eror 2 sizes %d %d \n",num, msg_size); */ /* DEBUG */
      outb_msg_recover();
      return(-1);
   }

   msg[num - 2] = '\0';
   //   Tmsg = (MSG_QUEUE_HDR *)msg;

   //   if ( !strcmp(Link_type[(Tmsg->delvry_prio - 0x30)-1], "GPRS") )
   //     {
   //  fprintf(stderr,"GPRS Message received");
   //  return(0);
   // }
   return(num);
}				/* end get_outb_msg */


LOCAL VOIDT 
parent_sig_term(sig)
   INT   sig;
{
   if (tpid > 0)
   {
       kill(tpid, SIGTERM);
       do
       {
         term_pid = wait(0);
       } while (term_pid != tpid);
   }

   if (rpid > 0)
   {
       kill(rpid, SIGTERM);
       do
       {
	 term_pid = wait(0);
       } while (term_pid != rpid);
   }
      exit(0);
}   /* end parent_sig_term */

int
hex2int(text, digits)
/* convert tow characters of hex to integer */
   char  text[];
   int digits;
{
   int   type = 0;
   if (digits > 2)
       digits = 2;
   if (digits > 1)
   {
      if (text[ITM_TYPE_START] > '9')
         type += text[ITM_TYPE_START] - 'A' + 10;
      else
         type += text[ITM_TYPE_START] - '0';
      type *= 16;
   }
   if (text[ITM_TYPE_START + 1] > '9')
      type += text[ITM_TYPE_START + 1] - 'A' + 10;
   else
      type += text[ITM_TYPE_START + 1] - '0';
   return (type);
}				/* end hex2int*/


void
write_file_with_async_pid()
{
	FILE *fd;
			
	if ((fd = fopen(ASYNC_PID_FILE, "w")) == NULL)
	{
		fprintf(stderr, "%s: %s", "Can't open file", ASYNC_PID_FILE);
		return;
	}
	fchmod(fileno(fd), 0666);
									 
	fprintf(fd, "%d\n", getpid());
	fclose(fd);
}

void kill_async()
{
  signal(SIGTERM, SIG_DFL);
  if ( rpid > 0 )
    {
      kill(rpid, SIGTERM);
      do
	{
	  term_pid = wait(0);
	} while ((term_pid != rpid)  && (errno != ECHILD));
    }

  if ( tpid > 0 )
    {
      kill(tpid, SIGTERM);
      do
	{
	  term_pid = wait(0);
	} while ((term_pid != tpid) && (errno != ECHILD));
    }  

  
}

GetAsyncParams( parse_file )
     char parse_file[];
{
   INT   ps_local;		/* process-specific parameter-set ID */
   CHAR  par_val[MAXLEN_P_VALUE + 1];
   LONGINT key;
   
   if ((ps_local = parset_open(parse_file, PM_FLG_READ)) < 0)
   {
      fprintf(stderr, "**ALC** can't open parset [%s]\n", parse_file2);
      exit(0);
   }

   /*
    * * read in parameters from the local parset, "dlc_xx.ps"
    */
   parval_read(ps_local, "opentimeout", par_val, sizeof(par_val));
   Opentimeout = atoi(par_val);

   parval_read(ps_local, "openretry", par_val, sizeof(par_val));
   Openretry = atoi(par_val);

   parval_read(ps_local, "openretryinterval", par_val, sizeof(par_val));
   Openretryinterval = atoi(par_val);

   parval_read(ps_local, "writetimeout", par_val, sizeof(par_val));
   Writetimeout = atoi(par_val);

   parval_read(ps_local, "writeretry", par_val, sizeof(par_val));
   Writeretry = atoi(par_val);

   parval_read(ps_local, "writeretryinterval", par_val, sizeof(par_val));
   Writeretryinterval = atoi(par_val);

   parval_read(ps_local, "readretry", par_val, sizeof(par_val));
   Readretry = atoi(par_val);

   parval_read(ps_local, "readretryinterval", par_val, sizeof(par_val));
   Readretryinterval = atoi(par_val);
   key = sizeof(ALC_STATS);
   key = read_tolong(ps_local, "ALC_STAT_KEY");

   if ( alc_ptr == NULL )
     alc_initshm(key);

  parval_read(ps_local, "tcp_server1", par_val, sizeof(par_val));
  strcpy(TCP_server[0], par_val);
  
  parval_read(ps_local, "tcp_service1", par_val, sizeof(par_val));
  strcpy(TCP_service[0], par_val);

  parval_read(ps_local, "link_type1", par_val, sizeof(par_val));
  strcpy(Link_type[0], par_val);
  
  parval_read(ps_local, "tcp_server2", par_val, sizeof(par_val));
  strcpy(TCP_server[1], par_val);

  parval_read(ps_local, "tcp_service2", par_val, sizeof(par_val));
  strcpy(TCP_service[1], par_val);
  
  parval_read(ps_local, "link_type2", par_val, sizeof(par_val));
  strcpy(Link_type[1], par_val);  
  
  parval_read(ps_local, "tcp_server3", par_val, sizeof(par_val));
  strcpy(TCP_server[2], par_val);
  
  parval_read(ps_local, "tcp_service3", par_val, sizeof(par_val));
  strcpy(TCP_service[2], par_val);

  parval_read(ps_local, "link_type3", par_val, sizeof(par_val));
  strcpy(Link_type[2], par_val);
  
  parval_read(ps_local, "tcp_server4", par_val, sizeof(par_val));
  strcpy(TCP_server[3], par_val);
  
  parval_read(ps_local, "tcp_service4", par_val, sizeof(par_val));
  strcpy(TCP_service[3], par_val);

  parval_read(ps_local, "link_type4", par_val, sizeof(par_val));
  strcpy(Link_type[3], par_val);
  
  parval_read(ps_local, "tcp_server5", par_val, sizeof(par_val));
  strcpy(TCP_server[4], par_val);
  
  parval_read(ps_local, "tcp_service5", par_val, sizeof(par_val));
  strcpy(TCP_service[4], par_val);

  parval_read(ps_local, "link_type5", par_val, sizeof(par_val));
  strcpy(Link_type[4], par_val);
  
  parval_read(ps_local, "tcp_server6", par_val, sizeof(par_val));
  strcpy(TCP_server[5], par_val);
  
  parval_read(ps_local, "tcp_service6", par_val, sizeof(par_val));
  strcpy(TCP_service[5], par_val);

  parval_read(ps_local, "link_type6", par_val, sizeof(par_val));
  strcpy(Link_type[5], par_val);
  
  parval_read(ps_local, "tcp_server7", par_val, sizeof(par_val));
  strcpy(TCP_server[6], par_val);
  
  parval_read(ps_local, "tcp_service7", par_val, sizeof(par_val));
  strcpy(TCP_service[6], par_val);

  parval_read(ps_local, "link_type7", par_val, sizeof(par_val));
  strcpy(Link_type[6], par_val);
  
  parval_read(ps_local, "tcp_server8", par_val, sizeof(par_val));
  strcpy(TCP_server[7], par_val);
  
  parval_read(ps_local, "tcp_service8", par_val, sizeof(par_val));
  strcpy(TCP_service[7], par_val);          

  parval_read(ps_local, "link_type8", par_val, sizeof(par_val));
  strcpy(Link_type[7], par_val);
  
  if (parset_close(ps_local) == -1)
    {
	//      fprintf(stderr, "Parset Close Error:  (par_errno = %d)\n", 
	//     Par_errno);

    }
}
