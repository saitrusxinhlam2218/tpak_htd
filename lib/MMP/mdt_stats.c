/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: mdt_stats.c,v $
*  @(#)  $Revision: 2.0 $
*  @(#)  $Date: 2012/08/28 13:29:52 $
*  @(#)  $Author: taxiprod $
*  @(#)  $Source: /usr/users/jwelch/cvs_g7/taxi-g7/lib/MMP/mdt_stats.c,v $
*
*  Copyright (c) 1992 - Motorola Mobile Data Division, Seattle, WA
*
***********************************************************************/

static char rcsid[] = "$Id: mdt_stats.c,v 2.0 2012/08/28 13:29:52 taxiprod Exp $";

/*****************************************************************************
 *
 *              mdt_trans_stats(short)(void)    - Get MDT stats.
 *		init_count()(void)		- Initialize the stats array and total counter.
 *  		mdt_stats_disp()(void)		- Write out the MDT stats.
 *
 *****************************************************************************/

#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <string.h>
#include "mad_error.h"
#include "taxipak.h"
#include "Object.h"
#include "Fleet_lists_public.h"
#include "dispatch_strdefs.h"
#include "sprintf.h"
#include "timers.h"
#include "mdt.h"
#include "msg.h"
#include "path.h"
#include "itm.h"
#include "Vehicle_public.h"
#include "Vehicle_private.h"
#include "mdt_stats.h"
#include "async.h"

/*
 *	Timer change
 */
#include "Timer_public.h"
extern long mads_time;

void  init_count(void);

#define DISP_DURATION	3600    /* display or report duration (in seconds) */
#define TOTAL_TRANS	"TOTAL TRANSACTIONS"

long  mdt_use_cnt[LAST_STAT + 1];	/* mdt keys (transactions) usage count */
long  total_trans_cnts;		/* mdt keys (transactions) total counts */

extern int out_errors;
extern int incount;
extern int outcount;
extern int max_rate;
extern int nak_cnt;


/* stats display size		 */
/* SIGN ON           */
/* BOOK              */
/* ENROUTE BOOK      */
/* MESSAGE           */
/* CONDITIONAL BOOK  */
/* UNBOOK            */
/* EST. TIME ARRIVAL */
/* ZONE INQUIRY      */
/* NO TRIP           */
/* ACCEPT            */
/* REJECT            */
/* FLAG REQUEST      */
/* TALK              */
/* SIGN OFF          */
/* QP                */
/* BREAK             */
/* LOAD              */
/* UNLOAD            */
/* CALL OUT          */
/* BID               */
/* EMERGENCY         */
/* METER ON          */
/* METER OFF         */
/* CREDIT VERIFY     */
/* TRIP RECEIPT      */
/* REQUESTED RECEIPT */
/* METER FAULT       */
/* SWITCH TO DATA CH */
/* LF MISC.          */
/* EI MISC.          */
/* CM MISC.          */
/* ITM MISC.         */

#define	HDR0	"---------------------------------------------------------------\n"
#define	HDR1	"TRANSACTION TYPE"
#define	HDR2	"\tCOUNT\n"

static FILE *stats_fd = NULL;	/* file desc. for mdt usgage file */

static short inited = 0;	/* initialization flag */

/****************************************************************** */
/*  NOTE:                                                           */
/*     It does NOT kick off the time event initially.               */
/*     This is performed by the dispatch at the initialization phase*/
/*     The subsquent time event is generated by the mdt_stats_disp  */
/*     after it dumps the report to the file every time.            */
/******************************************************************/

#define MAX_TAXI 2000
long start_time[MAX_TAXI + 1];
int max_response_time = 0;
int total_time = 0;
int response_cnts = 0;

static void 
set_response_time(msg)
MSG_QUEUE_HDR *msg;
{
   char term_id[MID_SIZE + 1];
   struct veh_driv *veh_ptr;
   int rf_id;

   strncpy(term_id, msg->route_id, MID_SIZE);		/* Get RFID from input */
   term_id[MID_SIZE] = '\0';
   rf_id = (int) strtol(term_id, (char **) 0, 16);
   if(1)
   {  
      if (veh_ptr->veh_nbr < MAX_TAXI)
      {
         start_time[veh_ptr->veh_nbr] = time((long *) 0);
         response_cnts++;
      }
   }
}              /* end set_response_time */

void 
compute_response_time(veh_ptr)
struct veh_driv *veh_ptr;
{
    long end_time;
    int response_time;


    if (veh_ptr == NULL)
        return;
   
    if (veh_ptr->veh_nbr >= MAX_TAXI)
       return;

    if (start_time[veh_ptr->veh_nbr] == 0)
       return;

    end_time = time((long *) 0);
    response_time = end_time - start_time[veh_ptr->veh_nbr];
    if (!response_time)
       response_time = 1;
    total_time+= response_time;
    if (response_time > max_response_time)
        max_response_time = response_time;
    
    start_time[veh_ptr->veh_nbr] = 0;
}             /* end compute_response_time */

static void
output_response_stats()
{
    int avg;
    int ii;
    
    if (response_cnts)
    {
       avg = total_time / response_cnts;
       if (avg == 0)
           avg = 1;
     }

    fprintf(stats_fd,"      Response stats this period: average %d sec, max %d sec\n",
                    avg, max_response_time);

    response_cnts = 0;
    for (ii = 0; ii < 2000; ii++)
       start_time[ii] = 0; 
    total_time = 0;
    max_response_time = 0;
}            /* end output_response_stats */

void
zero_async_stats()
{
    int ii;
    for(ii = 0; ii < NUM_BLKS; ii++)
    {
    }
}           /* end zero_async_stats */

void
output_async_stats(to_stats)
int to_stats;
{
    int ii;
    int total_msgs;

    total_msgs = 0;
    if (to_stats)
    {
        fprintf(stats_fd,"      Async inbound stats this period:\n");
        fprintf(stats_fd,"       msgs receive by blk cnt:\n");
        fprintf(stats_fd,"         ");
        for (ii = 0; ii < NUM_BLKS; ii++)
        {
        }
        fprintf(stats_fd,"\n       total inbound msgs %d\n",total_msgs);

        total_msgs = 0;
        fprintf(stats_fd,"      Async output stats this period:\n");
        fprintf(stats_fd,"       msgs transmitted by blk cnt:\n");
        fprintf(stats_fd,"        ");
        for (ii = 0; ii < NUM_BLKS; ii++)
        {
        }
        fprintf(stats_fd,"\n       total outbound msgs %d\n",total_msgs);
        zero_async_stats();
    }
    else
    {
        printf(" Async stats: ");
#ifdef WANT_MORE
        printf(" msgs received, by blk cnt:");
        for (ii = 0; ii < NUM_BLKS; ii++)
        {
        }
        printf(" total inbound msgs %d\n",total_msgs);

        total_msgs = 0;
        printf(" msgs transmitted, by blk cnt:");
        printf("        ");
        for (ii = 0; ii < NUM_BLKS; ii++)
        {
        }
        printf(" total outbound msgs %d\n",total_msgs);
#endif
    }
}           /* end output_response_stats */

/*****************************************************************************
 *
 *	FUNCTION:
 *              mdt_trans_stats(short)(void)    - Get MDT stats.
 *
 * 	DESCRIPTION:
 *     		Count the mdt allowed key usage or transactions.
 *
 * 	PARMETERS:
 *              msg(MSG_QUEUE_HDR *)            - Inbound messages.
 *
 *	GLOBALS:
 *		inited(short)			- Flag for first time.
 *		mdt_use_cnt[](long)		- Transaction usage count.
 *		total_trans_cnts(long) 		- Transaction total count.
 *
 *	CALLS:
 *		init_count()(void)		- Initialize the stats array and total counter.
 *
 *	RETURN:
 *		None.
 *
 *	UPDATE:
 *		5.2	- Initial design for MMP. (TR)
 *
 *****************************************************************************/

void
mdt_trans_stats(msg)
   MSG_QUEUE_HDR *msg;
{
   if (inited == 0)
   {
      init_count();
      inited = 1;
   }
   total_trans_cnts++;

   /* FORM message */
   switch (msg->type)
   {				/* Parse message for type */
   case MT_MSG_STATUS:		/* Status Message */
      break;			/* Do next message */

   case MT_FORM_MSG:		/* Form Message */
      switch (msg->p_fields[P1])
      {
      case FT_LOCAL_FORM:
	 switch (msg->p_fields[P2])
	 {
	 case LF_S_IN:
            set_response_time(msg);
	    mdt_use_cnt[S_IN_IDX]++;
	    break;

	 case LF_POST:
            set_response_time(msg);
	    mdt_use_cnt[POST_IDX]++;
	    break;

	 case LF_EPOST:
	    mdt_use_cnt[EPOST_IDX]++;
	    break;

	 case LF_MSG:
	    mdt_use_cnt[MSG_IDX]++;
	    break;

	    case LF_BID:
	       mdt_use_cnt[BID_IDX]++;
	    break;

	 case LF_CPOST:
	    mdt_use_cnt[CPOST_IDX]++;
	    break;

	 case LF_UNPOST:
	    mdt_use_cnt[UNPOST_IDX]++;
	    break;

	 case LF_ETA:
	    mdt_use_cnt[ETA_IDX]++;
	    break;

	 case LF_INQ:
            set_response_time(msg);
	    mdt_use_cnt[INQ_IDX]++;
	    break;

	 case LF_NOSHOW:
	    mdt_use_cnt[NOSHOW_IDX]++;
	    break;

	 case LF_TAXI_ACCT:
	 case LF_ACCT1:
	 case LF_ACCT2:
	 case LF_ACCT3:
	 case LF_ACCT4:
            set_response_time(msg);
            mdt_use_cnt[CV_IDX]++;
	    break;

	 default:
            if(mdt_ext_trans_stats(msg) == SUCCESS)
               break;
	    mdt_use_cnt[LF_MISC_IDX]++;
	    break;
	 }
	 break;

      case FT_AUX_INP:
	 switch ('A')
	 {

	 case RQ_CARD_VALIDNESS_1:
	 case RQ_CARD_VALIDNESS_2:
            set_response_time(msg);
	    mdt_use_cnt[CV_IDX]++;
	    break;

	 case SHIFT_START:
	 case SIGN_ON_INFO:
            set_response_time(msg);
            mdt_use_cnt[S_IN_IDX]++;
	    break;

	 case TRIP_DATA:
	    mdt_use_cnt[TRIP_DATA_IDX]++;
	    break;

	 case TRIP_COMPLETE:
         case TRIP_STOP:
	    mdt_use_cnt[M_OFF_IDX]++;
	    break;

         case RQ_RECEIPT_DATA:
            mdt_use_cnt[RQ_RECPT_IDX]++;
            break;

	 default:
            if(mdt_ext_trans_stats(msg) == SUCCESS)
               break;
            if (strncmp(msg->text, FIELD_END, strlen(FIELD_END)) == 0)
               mdt_use_cnt[M_OFF_IDX]++;
            else
               mdt_use_cnt[ITM_MISC_IDX]++;
	    break;
	 }
	 break;

      default:
         mdt_ext_trans_stats(msg);
	 break;
      }
      break;			/* Do next message */

   case MT_FUNC_MSG:		/* Function Message */
      switch (msg->p_fields[P1])
      {
      case FCT_EMERG:
	 mdt_use_cnt[EMERG_IDX]++;
	 break;

      case FCT_CAN_MSG:
	 switch (msg->p_fields[P2])
	 {
	 case CM_ON_DATA:
	    mdt_use_cnt[ON_DATA_IDX]++;
	    break;

	 case CM_ACPT:
            set_response_time(msg);
	    mdt_use_cnt[ACPT_IDX]++;
	    break;

	 case CM_RJCT:
	    mdt_use_cnt[RJCT_IDX]++;
	    break;

	 case CM_FLAG:
	    mdt_use_cnt[FLAG_IDX]++;
	    break;

	 case CM_VOICE:
	    mdt_use_cnt[VOICE_IDX]++;
	    break;

	 case CM_S_OFF:
	    mdt_use_cnt[S_OFF_IDX]++;
	    break;

	 case CM_INFO:
            set_response_time(msg);
	    mdt_use_cnt[QP_IDX]++;
	    break;

	 case CM_BREAK:
	    mdt_use_cnt[BREAK_IDX]++;
	    break;

	 case CM_PCKUP:
	    mdt_use_cnt[PCKUP_IDX]++;
	    break;

	 case CM_CLOSE:
	    mdt_use_cnt[CLOSE_IDX]++;
	    break;

	 case CM_COUT_V02:
	 case CM_COUT_V03:
	    mdt_use_cnt[COUT_IDX]++;
	    break;

         case CM_MFAULT:
             mdt_use_cnt[M_FAULT_IDX]++;
             break;

	 default:
            if(mdt_ext_trans_stats(msg) == SUCCESS)
               break;
	    mdt_use_cnt[CM_MISC_IDX]++;
	    break;
	 }
	 break;

      case FCT_EXT_INP:
	 switch (msg->p_fields[P2])
	 {
	 case EI_M_ON:
	    mdt_use_cnt[M_ON_IDX]++;
	    break;

	 case EI_M_OFF:
	    mdt_use_cnt[M_OFF_IDX]++;
	    break;

	 default:
            if(mdt_ext_trans_stats(msg) == SUCCESS)
               break;
	    mdt_use_cnt[EI_MISC_IDX]++;
	    break;
	 }
	 break;

      default:
         mdt_ext_trans_stats(msg);
	 break;
      }
      break;			/* Do next message */

   default:
      mdt_ext_trans_stats(msg);
      break;			/* Do next message */
   }
}				/* end mdt_trans_stats */


/**************************************************************/
/*  init_count()                                              */
/*                                                            */
/*     Initialize the count or stats array and total counts.  */
/*                                                            */
/*     5/19/88   Jack Chou  (initial)                         */
/*************************************************************/

void
init_count()
{
   register int i;

   for (i = 0; i < LAST_STAT; i++)
      mdt_use_cnt[i] = 0;
   total_trans_cnts = 0;
}

/**************************************************************/
/*  mdt_stats_disp()                                          */
/*                                                            */
/*     Output the mdt usage or transactions to a file every   */
/*     DISP_DURATION period through time event handling.      */
/*                                                            */
/*     5/19/88   Jack Chou  (initial)                         */
/*************************************************************/

void
mdt_stats_disp()
{
   register int i;
   char  date[10], time[10], key_text[20];
   char error_str[80];

   if ((stats_fd = fopen(MDT_USAGE_FILE, "a")) == NULL)
   {
      sprintf(error_str, "Can't fopen %s - %d\n", MDT_USAGE_FILE, errno);
      ERROR(' ', "", error_str);
      return;
   }
   /* output system time .... */

   get_pkup_date(date);
   get_pkup_time(time);


   /* output the header */

   fprintf(stats_fd, "     Reporting time: %s %s\n", date, time);

   fprintf(stats_fd, "      Message stats: total %6d, inbound %6d, outbound %6d, naks %6d, max_rate %d",
	   incount + outcount, incount, outcount, nak_cnt, max_rate);
   if (out_errors)
      fprintf(stats_fd, ", msg errors %d\n", out_errors);
   else
      fprintf(stats_fd, "\n");

   max_rate = 0;		/* reset it */

   output_response_stats();
   output_async_stats(TRUE);

   fprintf(stats_fd, "     %s", HDR0);
   fprintf(stats_fd, "          %s%s", HDR1, HDR2);
   fprintf(stats_fd, "     %s", HDR0);


   for (i = 0; i <= LAST_STAT; i++)
   {
      switch (i)
      {
      case S_IN_IDX:
	 strcpy(key_text, "SIGN ON           ");
	 break;
      case POST_IDX:
	 strcpy(key_text, "BOOK              ");
	 break;
      case EPOST_IDX:
	 strcpy(key_text, "ENROUTE BOOK      ");
	 break;
      case MSG_IDX:
	 strcpy(key_text, "MESSAGE           ");
	 break;
      case CPOST_IDX:
	 strcpy(key_text, "CONDITIONAL BOOK  ");
	 break;
      case UNPOST_IDX:
	 strcpy(key_text, "UNBOOK            ");
	 break;
      case ETA_IDX:
	 strcpy(key_text, "EST. TIME ARRIVAL ");
	 break;
      case INQ_IDX:
	 strcpy(key_text, "ZONE INQUIRY      ");
	 break;
      case NOSHOW_IDX:
	 strcpy(key_text, "NO TRIP           ");
	 break;
      case ACPT_IDX:
	 strcpy(key_text, "ACCEPT            ");
	 break;
      case RJCT_IDX:
	 strcpy(key_text, "REJECT            ");
	 break;
      case FLAG_IDX:
	 strcpy(key_text, "FLAG REQUEST      ");
	 break;
      case VOICE_IDX:
	 strcpy(key_text, "TALK              ");
	 break;
      case S_OFF_IDX:
	 strcpy(key_text, "SIGN OFF          ");
	 break;
      case QP_IDX:
	 strcpy(key_text, "QP                ");
	 break;
      case BREAK_IDX:
	 strcpy(key_text, "BREAK             ");
	 break;
      case PCKUP_IDX:
	 strcpy(key_text, "LOAD              ");
	 break;
      case CLOSE_IDX:
	 strcpy(key_text, "UNLOAD            ");
	 break;
      case COUT_IDX:
	 strcpy(key_text, "CALL OUT          ");
	 break;
      case BID_IDX:
	 strcpy(key_text, "BID               ");
	 break;
      case EMERG_IDX:
	 strcpy(key_text, "EMERGENCY         ");
	 break;
      case M_ON_IDX:
	 strcpy(key_text, "METER ON          ");
	 break;
      case M_OFF_IDX:
	 strcpy(key_text, "METER OFF         ");
	 break;
      case CV_IDX:
	 strcpy(key_text, "CREDIT VERIFY     ");
	 break;
      case TRIP_DATA_IDX:
	 strcpy(key_text, "TRIP RECEIPT      ");
	 break;
      case RQ_RECPT_IDX:
         strcpy(key_text, "REQUESTED RECEIPT ");
         break;
      case M_FAULT_IDX:
         strcpy(key_text, "METER FAULT       ");
         break;
      case ON_DATA_IDX:
         strcpy(key_text, "SWITCH TO DATA CH ");
         break;
      case LF_MISC_IDX:
	 strcpy(key_text, "LF MISC.          ");
	 break;
      case EI_MISC_IDX:
	 strcpy(key_text, "EI MISC.          ");
	 break;
      case CM_MISC_IDX:
	 strcpy(key_text, "CM MISC.          ");
	 break;
      case ITM_MISC_IDX:
	 strcpy(key_text, "ITM MISC.         ");
	 break;
      default:
         mdt_ext_stats_disp(i, key_text);
         break;
      }

      if (mdt_use_cnt[i])
      {
         fprintf(stats_fd, "          %s    %.7d\n", key_text, mdt_use_cnt[i]);
         fflush(stats_fd);
      }
   }

   /* output the total counts */

   fprintf(stats_fd, "     %s", HDR0);
   fprintf(stats_fd, "          %s    %.7d\n\n", TOTAL_TRANS, total_trans_cnts);
   fclose(stats_fd);

   /* initialize counts and kick off time event */

   init_count();
   inited = 1;

}