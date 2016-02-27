/*  @@(#)  $RCSfile: mdt_4.c,v $
 *  @@(#)  $Revision: 2.1 $
 *  @@(#)  $Date: 2015/07/21 09:22:05 $
 *  @@(#)  $Author: jazam $
 *  @@(#)  $Source: /usr/users/jwelch/cvs_g7/taxi-g7/lib/MMP/mdt_4.c,v $ */
static char rcsid[] = "$Id: mdt_4.c,v 2.1 2015/07/21 09:22:05 jazam Exp $";

/* Based on version 6.41 */

/* Make sure location constants are defined.*/
/* Create a compile error if they           */
/*   are not defined or both are defined.   */

#ifdef SCANDINAVIA
#ifdef NAMERICA
ERROR:"Both compile time constants SCANDINAVIA and NAMERICA are defined."
#endif
#else
#ifdef NAMERICA
#else
ERROR:"Compile time constant SCANDINAVIA or NAMERICA undefined."
#endif
#endif

#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <memory.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <ctype.h>
#include <signal.h>
#include <string.h>
#include <langinfo.h>
#include <setjmp.h> 

#include "taxipak.h"
#include "Object.h"
#include "Vehicle_public.h"
#include "dispatch_strdefs.h"
#include "path.h"
#include "sprintf.h"
#include "mad_error.h"
#include "dispatch.h"
#include "language.h"
#include "mdt.h"
#include "veh_driv.h"
#include "df_maint.h"
#include "timers.h"
#include "writer.h"
#include "ex_key.h"
#include "except.h"
#include "mad_ipc.h"
#include "ui_ipc.h"
#include "msg.h"
#include "msg_mmp.h"
#include "Call_public.h"
#include "Call_private.h"
#include "Zone_public.h"
#include "Fleet_public.h"
#include "Fleet_private.h"

#define MSGSIZE		MMP_TEXT_SIZE

#ifndef MSGQ_
#define WRITE_TIME_OUT 2
extern outb_fd;
#ifdef DUAL_PORTS
extern outb_fd_2;
#endif

#else
extern int send_msgqid;
#endif

extern struct cisam_ch writer_buf;
CALL_REC cisam_call;
extern time_t mads_time; /* system time */
extern char buf[];
extern char req_buf[];
extern struct veh_driv *
get_veh_id ();
extern struct fleets *fleet[FLEET_MAX]; /* shared memory fleet structure */
extern char *scratch_ptr;
char *
last_six ();
void
delete_outb_char ();
void
send_pkt_id ();
void
clr_pkt_id ();
void
mk_outb_header ();
void
terminate_outb_text ();
void
mk_msg_priority ();
void
config_outb_msg ();
void
dump_outb_msg ();
void
time_out_hdlr ();

static char str_temp[81];
jmp_buf env;

/* how many time we timeout before the msg is lost */
#define OUTB_RETRY_MAX 2
int retry_cnt;
/* how many message in a row are lost befor we mk an exception */
#define MAX_OUTB_LOST 20
int lost_outb;

MSG_QUEUE_HDR outb;
#ifdef DEBUG
extern int ok_stats_flg;
int out_errors = 0;
int incount = 0;
int outcount = 0;
int timecount = 0;
int max_rate = 0;
int msg_count1 = 0; /* current  minute */
int limit = 1;
//short srch_error = TRUE;        /* search report error reporting */

extern int response_cnts;
extern int max_response_time;
extern int total_time;

void dump_outb(void);
#endif
short srch_error = TRUE; /* search report error reporting */

/*****************************************************************************/
/* */
/* Message send routine                                                     */
/* */
/* This routine performs all of the msgsnd's in mdt_4.c                     */
/* */
/*****************************************************************************/

#ifdef MSGQ_
static int
msgsend(msg_qid, send_struct, nbr_bytes, action, veh_ptr)
int msg_qid;
MSG_QUEUE_HDR *send_struct;
int nbr_bytes;
int action;
struct veh_driv *veh_ptr;
{
	int result = 0;
	struct msqid_ds infoque;

	if (1)
	{ /* send only if acc is up */
		if (veh_ptr != NULL)
		{ /* allow fleetwides without a veh_ptr */
			/* only do send if acc is up, and no emergency */
			if (!veh_ptr->t_status.mdt_working || (veh_ptr->t_status.emer))
			return (0);
			if (!fleet[veh_ptr->fleet_nbr]->criteria.acc_on)
			return (0);
		}
		if (msgctl(msg_qid, IPC_STAT, &infoque) < 0) /* probably BOGUS MRB */
		{
			if (veh_ptr != NULL)
			{ /* allow fleetwides without a veh_ptr */
				sprintf(scratch_ptr,
						"can't write to output queue, full, msg to vehicle %d",
						veh_ptr->veh_nbr);
				ERROR(fleet[veh_ptr->fleet_nbr]->fleet_id, " ", scratch_ptr);
			}
			else
			ERROR(' ', " ", "can't write to output queue, full");

			return (-1);
		}
		if (infoque.msg_qnum > 60)/* 60 is BOGUS MRB */
		{
			return (-1);
		}
		result = msgsnd(msg_qid, (struct msgbuf *) send_struct, nbr_bytes, action);
	}
#ifdef DEBUG
	if (result < 0)
	{
		out_errors++;
		if (ok_stats_flg)
		printf("async send error %d\n", errno);
#ifdef MORE_DEBUG
		dump_msg_q(msg_qid);
#endif
	}
#endif
	return (result);
} /* end msgsend */
#endif

/*****************************************************************************/
/* strcopy (string_pointer)                                                 */
/* */
/* This routine will copy a string (like strcpy), but it will return the    */
/* number of characters copied (like strlen).                               */
/* */
/*****************************************************************************/

strcopy (string_ptr_1, string_ptr_2)
	char *string_ptr_1, *string_ptr_2; {
	int result;

	strcpy (string_ptr_1, string_ptr_2);
	result = strlen (string_ptr_1);

	return (result);
} /* end strcopy */

/****************************************************************************/
/* */
/* Outbound Command Processing Flows                                        */
/* */
/* The outbound KDT routines are called either by the KDT inbound/support   */
/* routines (in those instances where an inbound command is fully processed */
/* by the KDT routines or an error is encountered) and by the DISPATCH      */
/* routines.                                                                */
/* */
/* Functionally, there are several types of outbound messages:              */
/* */
/* emergency                          emerg_ack (term_id)                   */
/* registration                       term_reg (term_id)                    */
/* deregistration                     term_dereg (term_id)                  */
/* Authorize Terminal                 authorize(veh_ptr)    	            */
/* Deauthorize Terminal               deauthorize(veh_ptr)    	            */
/* vehicle info                       send_veh_info(term_id, disp_info)     */
/* zone info                          send_zone_info(term_id,zone_no)       */
/* message                            send_msg_mmp (term_id, type  &        */
/* p_field, veh_ptr,      */
/* priority)              */
/* predefined text                    send_text_predefined (term_id,        */
/* text_id, veh_ptr)     */
/* not predefined text                send_text (term_id, text_str,         */
/* veh_ptr)                */
/* clear terminal screen              clear_terminal_screen (term_id,       */
/* device, veh_ptr        */
/* search display                     send_srch_display (fleet_no)          */
/* bid display                        send_bid_display (fleet_no)           */
/* sign on info                       send_signon_info (term_id)            */
/* send queue position		      send_qp(zone_nbr, veh_ptr)    	    */
/* break time limit                   send_brk_time (term_id)               */
/* text multicast                     send_multicast (fleet_no, mid, text)  */
/****************************************************************************/

/*****************************************************************************/
/* */
/* emrg_ack - Emergency Acknowledge to KDT Routine                         */
/* */
/*****************************************************************************/

emerg_ack (veh_ptr)
	/* emergency ACK */
	struct veh_driv *veh_ptr; {

	return 0; /* THER IS NO ERMEGENCY ACK REQUIRED NOW */
#ifdef EMERGENCY_ACK
	mk_outb_text("");
	if (send_msg_mmp(veh_ptr->mid, EMERG_ACK, veh_ptr) != 0)
	{
		sprintf(str_temp,"unable to send emerg ack to %s",veh_ptr->mid);
		ERROR(fleet[veh_ptr->fleet_nbr]->fleet_id, "", str_temp);
	}

#endif
	return 0;
}

#ifndef	EXT_AUTHORIZE
authorize(veh_ptr)
struct veh_driv *veh_ptr;
{
	char fleet_id[6];

	mk_outb_text("");

	if (send_msg_mmp(veh_ptr->mid, VEH_AUTHORIZE_AND_OPEN_PORT, veh_ptr) != 0)
	{
		sprintf(str_temp,"unable to send authorize/open serial port to %s",veh_ptr->mid);
		ERROR(fleet[veh_ptr->fleet_nbr]->fleet_id, "", str_temp);
	}

	/* Send down fleet mid group (authorize fleetwide messages) */

	memcpy(fleet_id, GROUP_MID, MID_SIZE - 1);
	sprintf(fleet_id + MID_SIZE - 1, "%d", veh_ptr->fleet_nbr);
	mk_outb_text(&fleet_id[2]);

	if (send_msg_mmp(veh_ptr->mid, ASGN_GROUP1, veh_ptr) != 0)
	{
		sprintf(str_temp,"unable to send group id to %s",veh_ptr->mid);
		ERROR(fleet[veh_ptr->fleet_nbr]->fleet_id, "", str_temp);
	}

} /* end authorize */
#endif	// EXT_AUTHORIZE

deauthorize (veh_ptr)
	struct veh_driv *veh_ptr; {
	char fleet_id[6];

	/* if (veh_ptr->term_type != MDT_ITM) MRB */
	{
		mk_outb_text ("");
		if (send_msg_mmp (veh_ptr->mid, DEAUTHORIZE, veh_ptr) != 0) {
			sprintf (str_temp, "unable to send deauthorize to %s", veh_ptr->mid);
			ERROR(fleet[veh_ptr->fleet_nbr]->fleet_id, "", str_temp);
		}

	}
} /* end deauthorize */

/*****************************************************************************/
/* */
/* send_zone_info - <INQRY> Terminal Command Response Routine              */
/* */
/*****************************************************************************/

send_zone_info (term_id, zone_info, veh_ptr)
	/* send zone info */
	char term_id[]; /* rf id */
	char zone_info[]; /* zone info string */
	struct veh_driv *veh_ptr; {

	sprintf (str_temp, "%s", zone_info);

	if (send_text (term_id, str_temp, veh_ptr, PRIORITY_2) != 0)
		ERROR(fleet[veh_ptr->fleet_nbr]->fleet_id, "", "unable to send zone info");

} /* end send_zone_info */

#ifdef DUAL_PORTS
int
send_port_1()
/* choose a port based on mid & multicast */
{
	if (outb.multicast == 'M')
	return (TRUE);
	if (outb.route_id[MID_SIZE -1] > '8')
	return (TRUE);
	else
	return (FALSE);
} /* end send_port_1 */
#endif

/********************************************************************/
/* */
/* transmit_message - preform the message send                      */
/* */
/*******************************************************************/

static int transmit_message (veh_ptr)
	struct veh_driv *veh_ptr; {
	int result;

#ifndef MSGQ_
	/* should move to msgsend ? */
	/* dont send outb.prio */
	retry_cnt = 0;

	if (1) { /* send only if ncp is up */
		signal (SIGALRM, time_out_hdlr);
		errno = 0;
		if (setjmp (env) != 0) {
			lost_outb++;
			/* attend to the user interface */
			if (lost_outb == MAX_OUTB_LOST) {
				/* onle do once */
			}
#ifdef DEBUG_LOTS
			printf("setjmp return\n");
#endif
			return (-1);
		}
		alarm (WRITE_TIME_OUT);

//#ifdef ONE_AT_A_TIME
//      result = write(outb_fd, outb.route_id, MSG_HDR_LEN + get_outb_text_size() + 1);
//#else
//      terminate_outb_msg();
//      outb.size = MSG_HDR_LEN + get_outb_text_size() + 1;
//#ifdef DUAL_PORTS
//      if (send_port_1())
//         result = write(outb_fd, &outb, outb.size + sizeof(outb.size));
//      else
//         result = write(outb_fd_2, &outb, outb.size + sizeof(outb.size));
//#else
//      result = write(outb_fd, &outb, outb.size + sizeof(outb.size));
//#endif
//#endif

		terminate_outb_msg ();

		outb.size = MSG_HDR_LEN + get_outb_text_size () + 1;

// Prise en compte du STAR2015.
#ifdef DUAL_PORTS
		if (veh_ptr == NULL) {
			result = write(outb_fd_2, &outb, outb.size + sizeof(outb.size));
			result = (result < 0) ? result : write(outb_fd, &outb, outb.size + sizeof(outb.size));
		}
		else {
			if ((veh_ptr) && (veh_ptr->term_type == STAR2015_MDT))
				result = write(outb_fd_2, &outb, outb.size + sizeof(outb.size));
			else
				result = write(outb_fd, &outb, outb.size + sizeof(outb.size));
		}
#else
		result = write (outb_fd, &outb, outb.size + sizeof(outb.size));
#endif

		alarm (0);
		if (result < 0) {
			/* if the timeout breaks us out the retrys wont work */
#ifdef DEBUG_LOTS
			fprintf(stderr, "outb pipe access error # %d\n", errno);
#endif
			lost_outb++; /* in case time_out_hdlr really works */
			if (lost_outb == MAX_OUTB_LOST) {
				/* onle do once */
			}
			return (result);
		}
	}
	lost_outb = 0;
	return (0);
#else
	return (msgsend(send_msgqid, &outb, sizeof(MSG_QUEUE_HDR) -
					sizeof(long) - MSGSIZE + get_outb_text_size(),
					(int) IPC_NOWAIT, veh_ptr));
#endif
} /* end transmit_message */

/******************************************************************/
/*
 * send_msg_mmp - User defined mmp specific message Transmission to Terminal
 */
/******************************************************************/

int send_msg_mmp (term_id, mmp_hdr, veh_ptr)
	/* send message */
	char term_id[]; /* rf id */
	char mmp_hdr[]; /* text */
	struct veh_driv *veh_ptr; {
	int status;

	mk_outb_header (term_id, mmp_hdr);
	if (is_text_outb() && ((veh_ptr == NULL) || (veh_ptr->term_type != STAR2015_MDT)))
		terminate_outb_text (); /* if there is any text terminate it */

	status = transmit_message (veh_ptr);

	return (status);
} /* end send_msg_mmp */

/*****************************************************************************/
/* */
/* send_text_predefined - Canned Text Transmission to Terminal                           */
/* */
/*****************************************************************************/

int send_text_predefined (term_id, text_id, veh_ptr)
	/* send predefined text */
	char term_id[]; /* rf id */
	char text_id; /* handle to text string to be displayed */
	struct veh_driv *veh_ptr; {
	int status;
	int ii;
	char *
	get_outb_header ();
	char *
	get_outb_txt ();

	if (is_valid_text_id (text_id)) {
		mk_outb_header (term_id, get_outb_header (text_id));
		mk_outb_text (get_outb_txt (text_id));
		terminate_outb_text ();

#ifdef DEBUG
		for (ii = 0; ii < limit; ii++)
#endif
		status = transmit_message (veh_ptr);
		if (status) {
			sprintf (str_temp, "unable to send msg to %s", veh_ptr->mid);
			ERROR(fleet[veh_ptr->fleet_nbr]->fleet_id, "", str_temp);
		}

	}
	else
		ERROR(fleet[veh_ptr->fleet_nbr]->fleet_id, "", "send_text_predefined: bad text_id ");

	return (status);
} /* end send_text_predefined */

/*****************************************************************************/
/* */
/* send_text - Normal Text Transmission to Terminal                           */
/* */
/*****************************************************************************/

int send_text (term_id, text_str, veh_ptr, priority)
	/* send text */
	char term_id[]; /* rf id */
	char text_str[]; /* text */
	struct veh_driv *veh_ptr;char priority;
/* null string will terminate text already there */
{
	int status;
#ifdef DEBUG
	int ii;
#endif

	mk_outb_header (term_id, TEXT_DISPLAY);
	if (priority >= PRIORITY_0 && priority <= PRIORITY_3)
		mk_msg_priority (priority);
	if (text_str != NULL)
		mk_outb_text (text_str);
	terminate_outb_text ();

#ifdef DEBUG
	for (ii = 0; ii < limit; ii++)
#endif
	status = transmit_message (veh_ptr);

	return (status);
} /* end send_text */

/*****************************************************************************/
/* clear_terminal_screen   clear the terminal device screen */
/*****************************************************************************/

clear_terminal_screen (term_id, device, veh_ptr)
	/* device clear */
	char term_id[]; /* rf id */
	char device; /* which part of screen */
	struct veh_driv *veh_ptr; {

#ifndef CLEAR_NEEDED
	/* obsolete - not supported */
	return 0;
#else
	mk_clear_mmp(term_id, device); /* set the bits */

	if (transmit_message(veh_ptr) != 0)
	ERROR(fleet[veh_ptr->fleet_nbr]->fleet_id, "",
			"clear_terminal_screen: msgsend");
#endif
} /* end clear_terminal_screen */

static void make_search_report_text (mdt_screen, fl_ptr)
	int mdt_screen;struct fleets *fl_ptr; {
	int i;
	char nbr_cabs[3], nbr_calls[3];
	char str[MAX_DISPLAY_LENGTH + 1];
	int nbr_lines = 1;
	int column = 0;

	/* Init outb text */
	mk_outb_text ("");

	for (i = 0; i < fl_ptr->search_report.screen[mdt_screen].nbr_in_search; i++) {
		/* Only 3 zone/cabs/calls info per line of MDT.
		 Put a CR after each 3 */
		if (i && (i % 3 == 0)) {
			nbr_lines++;

			/* Exit if no more room in MDT memory */
			if (nbr_lines > MAX_MSG_LINES)
				break;
			add_outb_text (CR);
			column = 0;
		}

		if (fl_ptr->search_report.screen[mdt_screen].count[i].nbr_cabs > 99)
			strcpy (nbr_cabs, "**");
		else
			sprintf (nbr_cabs, "%02d", fl_ptr->search_report.screen[mdt_screen].count[i].nbr_cabs);

		if (fl_ptr->search_report.screen[mdt_screen].count[i].nbr_calls > 99)
			strcpy (nbr_calls, "**");
		else
			sprintf (nbr_calls, "%02d", fl_ptr->search_report.screen[mdt_screen].count[i].nbr_calls);

		sprintf (str, "%03d:%2s/%2s", fl_ptr->search_report.screen[mdt_screen].count[i].zone_nbr, nbr_calls, nbr_cabs);
		add_outb_text (str);

		/* Only the first two columns of zones need spacing,
		 the last one doesn't */
		if (column != 2)
			add_outb_text ("  "); /* 2 char spacing */
		column++;
	}
} /* end make_search_report_text */

/*****************************************************************************/
/* */
/* send_srch_display - Search Report Transmission Routine                  */
/* */
/*****************************************************************************/

send_srch_display (fleet_nbr)
	/* fleetwide broadcast */
	int fleet_nbr; /* fleet no */
{
	char term_id[MID_SIZE + 1];
	struct fleets *fl_ptr;

#ifdef DEBUG
	char *stime, *ctime();
	long tmtime;
	static long lastime = 0;
	float factor;
	int rate;
	long tdiff;
	int avg;

	/* print the time */
	tmtime = time((long *) 0);
	stime = ctime(&tmtime);
	tdiff = (tmtime - lastime);
	if (tdiff > 1)
	factor = (60.0 / (float) tdiff);
	rate = (int) ((float) (incount + outcount - msg_count1) * factor);
	if (rate > max_rate)
	max_rate = rate;
	lastime = tmtime;

	if (response_cnts)
	{
		avg = total_time / response_cnts;
		if (avg == 0)
		avg = 1;
	}
	else
	avg = 0;

	if (ok_stats_flg)
	{
		printf("%.8s total %6d, inbound %6d, outbound %6d, rate/min %3d, max_rate %d, avg resp %d, max resp %d",
				&(stime[11]), incount + outcount, incount, outcount, rate, max_rate, avg, max_response_time);
		if (out_errors)
		printf(", msg errors %d\n", out_errors);
		else
		printf("\n");
		output_async_stats(FALSE);
	}
	msg_count1 = incount + outcount;
#endif

	fl_ptr = fleet[fleet_nbr];
	memcpy (term_id, GROUP_MID, MID_SIZE - 1);
	sprintf (term_id + MID_SIZE - 1, "%d", fleet_nbr);

	config_outb_msg (10, "000", ' ', ' ', 'M', ' ');

	/******************************************************/
	/* search report includes:                            */
	/* list of zone number - available calls pair    */
	/* for all reported zone   */
	/******************************************************/

	/* Zones for search reports are seperated into 3 seperate screens */

	make_search_report_text (0, fl_ptr);
	if (send_msg_mmp (term_id, SEND_SRCH1, (struct veh_driv *) NULL) != 0)
		if (srch_error) {
			ERROR(fl_ptr->fleet_id, "", "unable to send search report 1");
			srch_error = FALSE;
		}
		else
			srch_error = TRUE;

	make_search_report_text (1, fl_ptr);
	if (send_msg_mmp (term_id, SEND_SRCH2, (struct veh_driv *) NULL) != 0)
		if (srch_error) {
			ERROR(fl_ptr->fleet_id, "", "unable to send search report 2");
			srch_error = FALSE;
		}
		else
			srch_error = TRUE;

	make_search_report_text (2, fl_ptr);
	if (send_msg_mmp (term_id, SEND_SRCH3, (struct veh_driv *) NULL) != 0)
		if (srch_error) {
			ERROR(fl_ptr->fleet_id, "", "unable to send search report 3");
			srch_error = FALSE;
		}
		else
			srch_error = TRUE;

	config_outb_msg (10, "000", ' ', ' ', ' ', ' ');

}

/*****************************************************************************/
/* send_bid_display - Bid Report Transmission Routine */
/*****************************************************************************/

send_bid_display (fleet_nbr)
	/* fleetwide broadcast */
	int fleet_nbr; /* fleet no */
{

	/*    Bid_broadcast(fleet_nbr); */

}

/*****************************************************************************/
/* send_control_msg */
/****************************************************************************/
send_control_msg (veh_ptr)
	struct veh_driv *veh_ptr; {
	if (veh_ptr != NULL) {
		veh_ptr->ext.nak_control_msg_state = NAK_CONTROL_MSG_PENDING;
		mk_outb_text ("CONTROLE TRANSMISSION");
		send_pkt_id (MSG_PKT_TYPE, 0, veh_ptr->veh_nbr);
		send_msg_mmp (veh_ptr->mid, TEXT_DISPLAY, veh_ptr);
	}
}

/*****************************************************************************/
/*   send_qp - Send Queue Position Message to MDT                        */
/*****************************************************************************/
#ifndef EXT_SEND_QP
send_qp(zone_nbr, veh_ptr)
int zone_nbr; /* zone number posted into */
struct veh_driv *veh_ptr;
{
	short q_position;

#ifdef FOO
	short get_queue_position();
#endif

	if (zone_nbr != veh_ptr->zone_nbr)
	{
		sprintf(scratch_ptr,"zn mismatch %d %d", zone_nbr, veh_ptr->zone_nbr);
		ERROR(' ',"",scratch_ptr);
		printf("!!!!!!!!!!!!!!!!!!%s\n",scratch_ptr);
	}
	strcpy(str_temp, QUEUE_POSITION_HDR);
	mk_outb_text(str_temp);
	sprintf(str_temp, " %3d %2d", zone_nbr, q_position);
	add_outb_text(str_temp);

	if (send_msg_mmp(veh_ptr->mid, STATUS_DISPLAY, veh_ptr) != 0)
	ERROR(fleet[veh_ptr->fleet_nbr]->fleet_id, "", "unable to send_msg_mmp");

} /* end send_qp */
#endif	// EXT_SEND_QP

#ifdef OBSOLETE
/*****************************************************************************/
/* */
/* send_brk_time - <BREAK> Command Response Routine                         */
/* */
/*****************************************************************************/

send_brk_time(veh_ptr) /* send break time limit */
struct veh_driv *veh_ptr;
{
	char temp[80];

	sprintf(temp, "%s %d %s", BREAK_TIME,
			fleet[veh_ptr->fleet_nbr]->break_duration / 60,
			MINUTES);

	(void) send_text(veh_ptr->mid, temp, veh_ptr, PRIORITY_2); /* send it out */

} /* end send_brk_time */
#endif

/*****************************************************************************/
/* */
/* send_multicast - Fleetwide Text Multicast Routine                        */
/* */
/*****************************************************************************/

send_multicast (fleet_nbr, mid, text)
	short fleet_nbr;char *mid;char *text; /* text string to be sent */
{ /* variable length message */
	long i;
	char term_id[MID_SIZE + 1];

	config_outb_msg (10, "000", ' ', ' ', 'M', ' ');

	sprintf (term_id, "%s%d", mid, fleet_nbr);

	sprintf (str_temp, "%s", text);

	if (send_text (term_id, str_temp, (struct veh_driv *) NULL, PRIORITY_1) != 0)
		ERROR(fleet[fleet_nbr]->fleet_id, "", "unable to send multicast msg");

	config_outb_msg (10, "000", ' ', ' ', ' ', ' ');

}

void
dump_outb ()
/* print the current outbound message message */
{
	char temp_str[80];
	int ii;

#ifndef MSGQ_
	sprintf (trace_str, "OUTBOUND MSG fid =  %d\n", outb_fd);
#else
	sprintf(trace_str, "OUTBOUND MSG qid =  %d\n", send_msgqid);
#endif
	TRACE(trace_str);

	sprintf (trace_str, " time send = %8.8d\n", ctime (&mads_time) + 11);
	TRACE(trace_str);

	dump_outb_msg (); /* the mmp stuff */
} /* end dump_outb */

void
time_out_hdlr (int dummy) {
	retry_cnt++;
	alarm (0);
#ifdef DEBUG_LOTS
	if (ok_stats_flg)
	printf("write timeout error\n");
#endif
	/*    ERROR(' ',"", "write to async timeout - msg lost"); */
	signal (SIGALRM, time_out_hdlr);
	if (retry_cnt > OUTB_RETRY_MAX) {
#ifdef DEBUG
		out_errors++;
#endif
		longjmp (env, 1); /* sorry to use this but time out doesnt abort blocked write */
	}
	else {
		alarm (WRITE_TIME_OUT); /* retry */
	}
} /* end time_out_hdlr */
