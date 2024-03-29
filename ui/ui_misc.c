static char sz__FILE__[]="@(#)ui_misc.c	10.1.2.2(ver) Date Release 1/30/95" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  ui_misc.c; Rel: 6.14.0.0; Get date: 10/8/93 at 04:04:37
 * @(#)  Last delta: 2/7/91 at 16:58:08
 * @(#)  SCCS File: /taxi/sccs/s.ui_misc.c
 *                                                                           *
 *****************************************************************************/

/****************************************************************************/
/* ui_misc.c: This file contains some miscellaneous functions used by	    */
/*	      the user interface program.  				    */
/****************************************************************************/


/* Make sure location constants are defined. Create a compile error if they
   are not defined or both are defined. */
#ifdef SCANDINAVIA

#ifdef NAMERICA
ERROR: "Both compile time constants SCANDINAVIA and NAMERICA are defined."
#endif
#else
#ifdef NAMERICA
#else
ERROR: "Compile time constant SCANDINAVIA or NAMERICA undefined."
#endif
#endif


#include <cursesX.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>

#include "taxipak.h"
#include "mem_mgmt.h"
#include "Object.h"
#include "user.h"
#include "ui_error.h"
#include "screens.h"
#include "call_entry.h"
#include "mads_types.h"
#include "mads_isam.h"
#include "df_maint.h"
#include "zgeo.h"
#include "except.h"
#include "ui_ipc.h"
#include "calls.h"
#include "writer.h"
#include "language.h"
#include "ui_strdefs.h"
#include "ui_def.h"
#include "enhance.h"
#include "switch_ext.h"

#ifndef __alpha
#define		MIN(x,y)	( (x) < (y) ? (x):(y) )
#define		MAX(x,y)	( (x) > (y) ? (x):(y) )
#endif

#define		MAIL_COL	105			/* column # to print mail msg on */
#define		ZONE_DESC_CHARS	20			/* # of char's in zone_desc */

extern int msgkey;
extern int user_index; 					/* index into array of users structure in shared memory*/
extern struct users *uiuser[];				/* array of pointer to users structure in shared memory */
extern char req_buf[];					/* for cisam disk i/o */
extern struct cisam_cl call_struct;			/* struct to be written to disk */
extern struct keydesc zn_key1,rt_key1,ac_key1,lo_key1;	/* key for reading zone/rate file */
extern int scr_name;					/* screen on display */
extern struct fleets *fleet[];				/* array of pointers to fleets struct in shared memory */
extern int cur_fl_index;				/* index of the fleet being used */
extern struct call_ipc_rec call_fifo;
extern short call_zoned;				/* whether call is zoned (1) or unzoned(0)  */
extern struct scr_flds extended_scr_fld[];
extern short write_dispatch;				/* flag whether a call structure should be sent to dispatch */
short zone_pkup_lead_time;				/* pickup lead time for pickup zone - for time calls */
extern short user_knows;				/* does user know that call is zoned or not */
extern int doing_update;				/* is the user on the update screen */
int zip1,zip2;						/* pickup zip and destintation zip */
int super_zone_nbr;					/* super zone used for manual time estimates */
extern short been_touched;
extern int pid;					/* process id of this process */
extern BOOLEAN bExplicitPremium;

/****************************************************************************************/
/* check_mail: this function checks how many mail messages the user has.		*/
/*		The info is read from the users structure in shared memory		*/
/*		(it is placed there by the counter process). The function		*/
/*		prints the number of messages on the first line of the 			*/
/*		screen. If the # of messages is 0, then a blank string is		*/
/*		printed (so that it would wipe out the # of messages printed		*/
/*		earlier on (which may no longer be correct).				*/
/****************************************************************************************/
check_mail()
{
	
	static char blank_string[] = "                   "; 
	char mail_buf[40];	/* buffer for saving the string to be printed */
	int  attrib;

	if ( uiuser[user_index]->cnt_msg ) 
		/* there are some unread messages */
		sprintf(mail_buf,"%s %-3d", catgets(UI_m_catd, UI_1, UI_UI_MISC_1, "MESSAGES ="), uiuser[user_index]->cnt_msg);
	else 
		/* msgs = 0 */
		sprintf(mail_buf,"%s", blank_string);

	/* print the # of messages */

	/* Get the attributes for the top line */
	attrib = mvinch(0, COLS - 1) & A_ATTRIBUTES;

	attrset(attrib);
		
	mvprintw(0,MAIL_COL,"%s", mail_buf);
	/* do not refresh screen */
	attrset(0);

}	/* end check_mail() */


/* prt_date: prints todays date on the top left corner of the screen */
prt_date()
{
    char datestr[10];
	
	get_pkup_date(datestr);
	mvaddstr(0,0, datestr);

}  /* end prt_date() */



/* term_to_index - This function is called to convert a terminal id (TTYxx) 
 *		to an index into the user profile area of shared memory.  
 *		Since the ports are not numbered in a regular ascending order, 
 *		this is necessary to avoid wasted space of unused array 
 *		elements.  Ports are (0,1,11,12,13,14,15,16,17,18,21,...)
 */
term_to_index(t_id)
int t_id;	/* Terminal id to convert */
{
    int x;	/* Temporary storage */

    if ( t_id > 1 )
    	x = (t_id - (9 + (2 * (t_id / 10)))) + 2;  /* Convert terminal id */
    else 
	x = t_id;

    if ((x < 0) || (x >= USER_MAX)) {	/* Is it in the valid range? */
	printf("%s %d", catgets(UI_m_catd, UI_1, UI_UI_MISC_2, "Wrong TTY number:"), x);
	return(-1);	/* No, Return error */
    }
    else
	return(x);	/* Yes, Return index */

}

/**********************************************************************************************/
/* get_zone_desc: gets the zone description for a basic/extended call entry screen, for the   */
/* 		  pkup zone. If call is not zoned or if the zone # entered is invalid, then   */
/* 		  the zone_desc area is cleared, else the zone desc is printed. Also 	      */
/*		  call_struct.zone_desc field is updated properly 			      */
/**********************************************************************************************/
get_zone_desc(clear_or_print, pkup_or_dest, cp, zone_file_ptr)
char clear_or_print;
char pkup_or_dest;					/* which address cbeing zoned */
struct cisam_cl *cp;
struct cisam_zn *zone_file_ptr;				/* pointer to req_buf[] */
{
	int row;					/* row to print desc on */
	int col;					/* col to print desc on */
	struct cisam_zn *zn_ptr;
	int i;
	int zone;

	if (clear_or_print != C_CLEAR && clear_or_print != C_PRINT)
	  return;

	if ( scr_name == BASIC_CALL_SCR || scr_name == SUBS_BASIC_SCR ) { 	/* get the row & col for printing the desc on */
		if ( pkup_or_dest == PICKUP_ADDR ) {
			zone_pkup_lead_time = 0;				/* zero out previous lead time */
			super_zone_nbr =0 ;					/* super zone used for manual time estimates */
			row =  B_PKUP_ZONE_ROW;
			col =  B_PKUP_ZONE_COL + 6;
			zone = cp->pckup_zone;
		}
		else { 								/* dest address */
			row =  B_DEST_ZONE_ROW;
			col =  B_DEST_ZONE_COL + 6;
			zone = cp->dest_zone;
		}
	}
	else if (scr_name == EXTENDED_CALL_SCR || scr_name == SUBS_EXTENDED_SCR || scr_name == INDIVIDUAL_CALLS_SCR) {
		if ( pkup_or_dest == PICKUP_ADDR ) {
			row =  E_PKUP_ZONE_ROW;
			col =  E_PKUP_ZONE_COL + 6;
		}
		else {
			row =  E_DEST_ZONE_ROW;
			col =  E_DEST_ZONE_COL + 6;
		}
	}  							/* end else if */
	else {
		return; 					/* this function shoul not be called from any other screen */
	} 
	
	attrset(0);
	move(row,col);

	/* clear the zone_desc area */
	for ( i = 0; i < ZONE_DESC_CHARS; i++ )  
		addch(' ');

#ifdef  FOO
	/* clear the map area **/
	move (row - 2, col + 11);
	for ( i = 0; i < 9; i++ )
	  addch( ' ' );
#endif

	if (cp == NULL)
	  return;

	if ( pkup_or_dest == PICKUP_ADDR )
	  {
	    zone = cp->pckup_zone;
	    cp->zone_desc[0] = '\0';		/* clear the old desc */
	  }
	else
	  zone = cp->dest_zone;

	if ( (zone == 0 || clear_or_print == C_CLEAR) && ( pkup_or_dest == PICKUP_ADDR ) )
	  {
	    call_struct.gps_lat = 0.0;
	    call_struct.gps_long = 0.0;
	    return;
	  }


	if ( zone_file_ptr == NULL )
	  {
	    zn_ptr = (struct cisam_zn *)req_buf;
	    zn_ptr->fleet = cp->fleet;
	    zn_ptr->nbr = zone;
	    if ( db(ZONE_FILE_ID,READ_KEYS,&zn_key1,ISEQUAL,0) < 0 )  	/* read it from disk */
	      return; 						/* unable to read record from zone file */
	  }  									/* end if */
	else 
	  zn_ptr = (struct cisam_zn *)zone_file_ptr;

	if( pkup_or_dest == PICKUP_ADDR)
		super_zone_nbr = zn_ptr->superzn_nbr;

	if ( zn_ptr->desc[0] != '\0' )  {
		mvprintw(row, col, "%-*s", CHARS_ZONE_DESC, zn_ptr->desc);
		if (0)
		  {
		    if ( pkup_or_dest == PICKUP_ADDR )
		      mvprintw(row - 2, col + 11, "%-*s", 9, cp->pckup_pmap );
		    else
		      mvprintw(row - 2, col + 11, "%-*s", 9, cp->dest_dmap );
		  }
		if ( pkup_or_dest == PICKUP_ADDR ) {
			strcpy(cp->zone_desc, zn_ptr->desc);
			if (zn_ptr->pckup_lead_time > 0
			    && scr_name != SUBS_BASIC_SCR && scr_name != SUBS_EXTENDED_SCR && scr_name != INDIVIDUAL_CALLS_SCR) {
			        if (cp->extended_type[0] == 'X')
				  zone_pkup_lead_time = zn_ptr->typex_lead_time;
				else
				  zone_pkup_lead_time = zn_ptr->pckup_lead_time;		/* calculate pickup lead time */
				zone_pkup_lead_time = get_super_lead(zone_pkup_lead_time,zn_ptr->superzn_nbr,cur_fl_index);
			}
		}
	}  									/* end if */

	return;

}  								/* end get_zone_desc() */
		
		
/************************************************************************************************************************/
/* get_est_wait_time: gets the estimated wait time for a call, from the fleet structure in shared memory		*/
/************************************************************************************************************************/
get_est_wait_time(int_flag)
int int_flag;					/* if 0 clear old entry only,1 = get new est time */
{
	int i, indx;
	short avg_time;
	struct zone_rqst zone_request;
	ZONE_RESP *zone_response;
	ZONE_RESP_HNDL  hZoneResp;
	int rc, times_thru, veh_in_bckup;
	int calls_in_bckup = 0;
	int veh_enroute_to_bckup = 0;	
	
	call_struct.est_wait_time[0] = '\0';				/* clear the old entry */
        
        if (scr_name != BASIC_CALL_SCR && scr_name != EXTENDED_CALL_SCR)
		return;

	mvprintw(LINES-2,0,"                              ");
	mvprintw(LINES-5, 30, "                          ");
	mvprintw(LINES-4,15,"                            ");
	mvprintw(LINES-3,15,"                            ");
	mvprintw(LINES-2,15,"                            ");
	mvprintw(LINES-4,33,"                            ");
	mvprintw(LINES-3,33,"                            ");
	mvprintw(LINES-2,33,"                            ");

	if (int_flag == 0)						/* just called to clear old entry */
		return;

	if (call_struct.pckup_zone <= 0 || call_struct.pckup_zone >= ZONE_MAX)
		return;							/* illegal zone # */

	/* No estimated time if a time call (ui seems to use both 1 or 2 in extended type for TIMEC) */
	if (call_struct.extended_type[2] == TIMEC)
	  {
	    /* set premium attributes for TIMEC */
	    call_struct.drv_attr_flag = YES;
	    call_struct.veh_attr_flag = YES;
	    call_struct.veh_attr[25] = YES;
	    call_struct.drv_attr[25] = YES;
	    return;
	  }
	else
	  {
	    /* unset premium attributes for TIMEC */
	    if (!bExplicitPremium)
	      {
		call_struct.veh_attr[25] = NO;
		call_struct.drv_attr[25] = NO;
	      }
	  }

	avg_time = 0;							/* default to zero */
	for(i=0;i<MAX_SUP_ZONES;i++){
		if(super_zone_nbr == fleet[cur_fl_index]->super_zone[i].nbr){
			if(fleet[cur_fl_index]->super_zone[i].man_time_est >0 && 
			   fleet[cur_fl_index]->super_zone[i].man_time_est < 999){
				avg_time = fleet[cur_fl_index]->super_zone[i].man_time_est;
				break;
			}
		}
	}
	if( avg_time == 0){
		for (i = 0; i < fleet[cur_fl_index]->zone_ct_ptr; i++) {	/* check up to specified number of zones */
			if (fleet[cur_fl_index]->zone_sumry[i].zone_num == call_struct.pckup_zone) {
    				avg_time = fleet[cur_fl_index]->zone_sumry[i].avg_time;
				break;						/* found the zone */
			}
		}
	}

	indx = -1;
	for (i = 0; i < RANGE_MAX; i++) {				/* get the index of the range to be used */
		if (avg_time <= fleet[cur_fl_index]->range_limit[i]) {
			indx = i;
			break;
		}
	}

	if (indx == -1 ||  fleet[cur_fl_index]->estimate_text[indx][0] == '\0')		/* use default value */
		strcpy(call_struct.est_wait_time,fleet[cur_fl_index]->def_est_text);
	else
		strcpy(call_struct.est_wait_time,&fleet[cur_fl_index]->estimate_text[indx][0]);
	


	zone_request.d_prcs_id = DISP_DEST;
	zone_request.u_prcs_id = pid;
	zone_request.rec_type = ZONE_REQ;
	zone_request.fleet = fleet[cur_fl_index]->fleet_id;
	zone_request.zone_nbr = call_struct.pckup_zone;

	bzero( &zone_request.veh_attr, sizeof(zone_request.veh_attr) );
	bzero( &zone_request.drv_attr, sizeof(zone_request.drv_attr) );  
	if (1)
	  {
	    for ( i = 0; i < ATTR_MAX; i++ )
	      {
		if ( call_struct.veh_attr[i] == YES )
		  set_bit( i, sizeof(zone_request.veh_attr), &zone_request.veh_attr );
		if ( call_struct.drv_attr[i] == YES )
		  set_bit( i, sizeof(zone_request.drv_attr), &zone_request.drv_attr );
	      }
	  }
	
	msgsnd( msgkey, ( struct msgbuf * )&zone_request,
	       sizeof( struct zone_rqst ), IPC_NOWAIT );
	
	times_thru = 0;
	hZoneResp = TMALLOC( ZONE_RESP, 1 );
	zone_response = ( ZONE_RESP * )hZoneResp;
	while (times_thru <=2)
	  {
	    rc = msgrcv(msgkey,(struct msgbuf *)zone_response,
			sizeof(struct zone_resp), pid, 0);
	    if (rc == -1)
	      {
		if (errno != EINTR)
		  {
		    TFREE( hZoneResp );
		    return(-1);
		  }
		if (++times_thru == 2)
		  {
		    TFREE( hZoneResp );
		    return(-1);
		  }
	      }
	    else if (zone_response->rec_type == ZONE_REQ)
	      break;
	    else
	      {
		TFREE( hZoneResp );
		return(-1);
	      }
	  }
	veh_in_bckup = 0;
	calls_in_bckup = 0;
	veh_enroute_to_bckup = 0;
	for ( i= 0; i < BCK_IN_ZN_RESP; i++ )
	  {
	    if ( zone_response->bkup_zones[i][0] )
	      {
		calls_in_bckup += zone_response->bkup_zones[i][1];
		veh_in_bckup += zone_response->bkup_zones[i][2];
		veh_enroute_to_bckup += zone_response->bkup_zones[i][3];
	      }
	  }
	mvprintw(LINES-5,30,          "%s       %s", catgets(UI_m_catd, UI_1, UI_TEXT_61, "PRIMARY"),
		                                     catgets(UI_m_catd, UI_1, UI_TEXT_62, "BACKUPS" ));
	mvprintw(LINES-4,17,"%s", catgets(UI_m_catd, UI_1, UI_UI_ZSTAT_3, "CALLS  "));
	mvprintw(LINES-3,17,"%s", catgets(UI_m_catd, UI_1, UI_UI_ZSTAT_5, "POSTED "));
	mvprintw(LINES-2,17,"%s", catgets(UI_m_catd, UI_1, UI_UI_ZSTAT_6, "ENROUTE"));
	mvprintw(LINES-4,33,"%2d           %2d",
		 zone_response->unassn_calls,
		 calls_in_bckup );
	mvprintw(LINES-3,33,"%2d           %2d",
		 zone_response->veh_posted,
		 veh_in_bckup );
	mvprintw(LINES-2,33,"%2d           %2d", 
		 zone_response->veh_enroute,
		 veh_enroute_to_bckup );
	mvprintw(LINES-2,0,"%s %s",
		 call_struct.est_wait_time,
		 catgets(UI_m_catd, UI_1, UI_UI_MISC_3, "MINUTES"));
}


/* ==================================================================================================================== *
 * print_addr_comment: prints/clears the address comment on the call entry screen.					*
 * ==================================================================================================================== */
print_addr_comment(clear_or_print,pkup_or_dest)
	char clear_or_print; /* C_CLEAR =lear old comment; C_PRINT=print new comment*/
	int pkup_or_dest;	/* whether PKUP_ADDR or DEST_ADDR */
{
	static char blanks[] = "                              ";
	int row, col;
	char *aptr;	/* pointer to field of call_struct */

	/* get the row & col to print address */
	if ( pkup_or_dest == PICKUP_ADDR )  {
		aptr = call_struct.pckup_adr_cmnt;
		if ( scr_name == BASIC_CALL_SCR || scr_name == SUBS_BASIC_SCR) {
			row = B_PKUP_ADDR_CMNT_ROW;
			col = B_PKUP_ADDR_CMNT_COL;
		}
		else if (scr_name == EXTENDED_CALL_SCR || 
			 scr_name == SUBS_EXTENDED_SCR)  {
			row = E_PKUP_ADDR_CMNT_ROW;
			col = E_PKUP_ADDR_CMNT_COL;
		}
		else 
			/* no need to do anything; for wakeup call screen */
			return;
	} /* end pkup_addr */
	else {
		/* dest address */
		aptr = call_struct.dest_adr_cmnt;
		if ( scr_name == BASIC_CALL_SCR || scr_name == SUBS_BASIC_SCR) {
			row = B_DEST_ADDR_CMNT_ROW;
			col = B_DEST_ADDR_CMNT_COL;
		}
		else if (scr_name == EXTENDED_CALL_SCR ||
			 scr_name == SUBS_EXTENDED_SCR) {
			row = E_DEST_ADDR_CMNT_ROW;
			col = E_DEST_ADDR_CMNT_COL;
		}
		else 
			/* no need to do anything; for wakeup call screen */
			return;
	} /* end pkup_addr */
	
	if ( clear_or_print == C_CLEAR ) {
		mvaddstr(row,col,blanks);
		*aptr = '\0';	/*  no comment for this address */
	}
	else 
		/* if address has commet then print it */
		if ( *aptr )
			mvaddstr(row,col,aptr);

}  /* end print_addr_comment() */
			
/**************************************************************************************/
/* get_internal_time: gets the time in internal C format; from the display formats    */
/*	of date & time								      */
/*	Returns 	0  if invalid date/time arg is passed to function	      */
/*			time in unix format 					      */
/**************************************************************************************/
time_t get_internal_time(date_copy,time_copy)
	char *date_copy;			/* assumed to be in mm/dd/yy format */
	char *time_copy;			/* assumed to be in 24hr or 12hr format; hh:mm{a/p} */
{
	char date_time[15];			/* date & time formatted in YYMMDDhhmmss format */
	char scratch_ptr[3];			/* scratch area */
	int hours=0;

	if (DATE_FORMAT == YEAR_FIRST_IN_DATE)
	{
		date_time[0] = date_copy[0];
		date_time[1] = date_copy[1];
		date_time[2] = date_copy[3];
		date_time[3] = date_copy[4];
		date_time[4] = date_copy[6];
		date_time[5] = date_copy[7];	  
	} else
	{
		date_time[0] = date_copy[6]; 			/* copy year from date */
		date_time[1] = date_copy[7];

		if (DATE_FORMAT == DAY_FIRST_IN_DATE) {
	    	date_time[4] = date_copy[0]; 			/* copy the day */
	    	date_time[5] = date_copy[1];

	    	date_time[2] = date_copy[3]; 			/* copy month from date */
	    	date_time[3] = date_copy[4];
		}
		else {
	    	date_time[2] = date_copy[0]; 			/* copy month from date */
	    	date_time[3] = date_copy[1];
	    
	    	date_time[4] = date_copy[3]; 			/* copy the day */
	    	date_time[5] = date_copy[4];
		}
	}

	/* copy the hour */
	date_time[6] = time_copy[0];
	date_time[7] = time_copy[1];

#ifdef HR12
	strncpy(scratch_ptr,time_copy,2);			/* extract just the hours */
	scratch_ptr[2] = '\0';					/* null terminate it */
	hours = atoi(scratch_ptr);				/* convert to integer hours */
	if (time_copy[5] == 'p' || time_copy[5] == 'P'){	/* PM time; take care of 1:00p - 11:59p */
		if(hours < 12)
			hours += 12;
	}
	else							/* AM time; take care of 12:01a - 12:59a */
		if(hours == 12)
			hours -= 12;
	sprintf(scratch_ptr, "%02d", hours);			/* convert back to ascii format */
	date_time[6] = scratch_ptr[0];
	date_time[7] = scratch_ptr[1];
#endif

	date_time[8] = time_copy[3]; 			/* copy the min from time */
	date_time[9] = time_copy[4];
	
	date_time[10] = '0'; 				/* copy sec's to be 0 secs */
	date_time[11] = '0';
	date_time[12] = '\0';
	
	return(conversion(date_time)); 			/* now get the internal time */

}  							/* end get_internal_time() */

/********************************************************************************************************/
/* time_subs_call: checks if the call being entered is a time call or a subscription call. sets the     */
/*		   call_struct.extended_type[2] to SUBS (for subscription calls) or TIMEC (for time calls) */
/********************************************************************************************************/
time_subs_call()
{
	time_t max_time;					/* max secs that a future call can be entered */
	time_t min_time;					/* min secs that a future call can be eneterd */
	time_t cur_time;					/* currnet time in secs from system clock */
	char date_str[10];				/* for formatting date */
	char time_str[10];				/* for formatting time */

	if ( scr_name == SUBS_BASIC_SCR || scr_name == SUBS_EXTENDED_SCR || scr_name == SUBS_WAKEUP_SCR ) {
		call_struct.extended_type[2] = SUBS;
		if (call_struct.extended_type[1] == '\0')
			call_struct.extended_type[1] = ' ';
		if (call_struct.extended_type[3] != CHARGE) {
			if(call_struct.extended_type[4] != MULT_CALL) 
				call_struct.extended_type[3] = '\0';
		}
		return;
	}
							/* its not a subscription call */
	cur_time = time(NULL);
	max_time = cur_time + (time_t) (fleet[cur_fl_index]->future_call_maximum); 		/* secs */
	min_time = cur_time + (time_t) (fleet[cur_fl_index]->future_call_minimum);		/* secs */
	call_struct.due_date_time = get_internal_time(call_struct.due_date, call_struct.due_time);

	if (call_struct.due_date_time < cur_time){				/* user left his screen unattended for a while ? */
		get_pkup_date(date_str);					/* get current date */
		get_pkup_time(time_str);					/* get current time */
		strcpy(call_struct.due_date,date_str);				/* save date in call record */
		strcpy(call_struct.due_time,time_str);				/* save time in call record */
		call_struct.due_date_time = get_internal_time(call_struct.due_date, call_struct.due_time);	/* new date/time */
		if ( call_struct.extended_type[2] != 'K' )
		  call_struct.extended_type[2] = ' ';
	}

	if ( call_struct.due_date_time > cur_time ) { 			/* it can be a time call */
		if ( call_struct.due_date_time < min_time )  { 		/* not far enough into the future to become a time call */
			prt_error(UI_NOTIMECALL,catgets(UI_m_catd, UI_1, UI_UI_MISC_4, "<time not far enough into the future for a time call>"));
			if ( ( call_struct.extended_type[2] != SUBS ) && ( call_struct.extended_type[2] != 'K' ) )
			  call_struct.extended_type[2] = ' ';
			if (!bExplicitPremium)
			  {
			    call_struct.veh_attr[25] = NO;
			    call_struct.drv_attr[25] = NO;
			  }
			return;
		}
		if ( call_struct.due_date_time > max_time ) { 		/* time entred > min time for time calls */
			prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_MISC_5, "<time entered too far into the future>"));
			set_call_bit(CALL_NO_MSG-1);
			set_bit(CALL_NO_MSG-1,sizeof(struct except_bits), (unsigned char *)&call_fifo.status);
			if ( call_struct.extended_type[2] != 'K' )			
			  call_struct.extended_type[2] = ' ';
			return;
		}
		if ((call_struct.extended_type[2] != SUBS)&&
		    (call_struct.extended_type[2] != 'K')) /* make a T only if not a subscription */
			call_struct.extended_type[2] = TIMEC;		/* time entered > min time  & time entered < max time; */
									/* its a time call */
		if (call_struct.extended_type[1] == '\0')
			call_struct.extended_type[1] = ' ';
		if (call_struct.extended_type[3] != CHARGE) {
			if(call_struct.extended_type[4] != MULT_CALL) 
				call_struct.extended_type[3] = '\0';
		}

		if (!doing_update)
			strcpy(call_struct.status,PENDING); 
	}  								/* end if  time entered > cur_time */
}   									/* end  time_subs_call() */



/* has_date_passed: checks whether the formatted date (in mm/dd/yy format) has passed or not
 *	Returns 	1	if date has passed 
 *	Returns		0	if date has not passed
 */
has_date_passed(the_date)
char *the_date;	/* in xx?xx?YY format */
{
	struct tm *tm_ptr;	/* pointer to formatted time structure */
	time_t nowtime;/* to store systime */
	int day, month, year;

	nowtime = time(NULL);
	tm_ptr = localtime(&nowtime);

	/* get the month day and year from date */
	if (DATE_FORMAT == YEAR_FIRST_IN_DATE) 
	{
	  if (TO_DIGIT(the_date[0]) * 10 + TO_DIGIT(the_date[1]) < 80 )
	    year = (TO_DIGIT(the_date[0]) * 10 + TO_DIGIT(the_date[1]) + 100 );
	  else
	    year = (TO_DIGIT(the_date[0]) * 10 + TO_DIGIT(the_date[1]));
	  
	    month = TO_DIGIT(the_date[3]) * 10 + TO_DIGIT(the_date[4]);
	    day   = TO_DIGIT(the_date[6]) * 10 + TO_DIGIT(the_date[7]);
	} else
	{
		if (DATE_FORMAT == DAY_FIRST_IN_DATE) {
	    	day = TO_DIGIT(the_date[0]) * 10 + TO_DIGIT(the_date[1]);
	    	month =  TO_DIGIT(the_date[3]) * 10 + TO_DIGIT(the_date[4]);
		}
		else {
	    	month= TO_DIGIT(the_date[0]) * 10 + TO_DIGIT(the_date[1]);
	    	day = TO_DIGIT(the_date[3]) * 10 + TO_DIGIT(the_date[4]);
		}
		if (TO_DIGIT(the_date[6]) * 10 + TO_DIGIT(the_date[7]) < 80 )
		  year = (TO_DIGIT(the_date[6]) * 10 + TO_DIGIT(the_date[7]) + 100 );
		else
		  year = (TO_DIGIT(the_date[6]) * 10 + TO_DIGIT(the_date[7]));		
	}

	/* check if year has passed */
	if ( year > tm_ptr->tm_year )
		return(0);	/* time entered is in future */
	else if ( year < tm_ptr->tm_year )
		return(1);	/* time has passed */
	/* year entered is current year */
	/* check month */
	if ( month > tm_ptr->tm_mon + 1 )
		return(0);	
	else if ( month < tm_ptr->tm_mon + 1 )
		return(1);	/* time has passed */
	/* month entered is current month */
	/* check day */
	if ( day > tm_ptr->tm_mday )
		return(0);
	else if ( day < tm_ptr->tm_mday )
		return(1);	/* past */

	/* current day entered */
	return(0);

}  /* end has_date_passed() */

/* get_rate: checks if rate should be taken from the rate file on disk 
 *	Returns 	0	if not allowed to enter rate manually
 *	Returns		1	if allowed to enter rate manually
 */
int get_rate(clear_or_print,need_rate,tzip1,tzip2)
	char clear_or_print;				/* clear the field or print the rate */
	char need_rate;					/* YES get rate from file, NO = check if user can enter rate */
	int *tzip1;
	int *tzip2;
{

#ifndef NAMERICA
  if ( ( clear_or_print == C_CLEAR ) && (0) )
    {
      switch( scr_name )
	{
	case BASIC_CALL_SCR:
	  call_struct.call_rate = 0.0;
	  attrset( A_UNDERLINE );
	  mvprintw( B_RATE_ROW, B_RATE_COL, "%7.2f", 0.0 );
	  mvprintw( B_VAT_ROW, B_VAT_COL, "%5.2f", 0.0 );
	  attrset( 0 );
	  break;
	case EXTENDED_CALL_SCR:
	  call_struct.call_rate = 0.0;
	  attrset( A_UNDERLINE );
	  mvprintw( E_RATE_ROW, E_RATE_COL, "%7.2f", 0.0 );
	  mvprintw( E_VAT_ROW, E_VAT_COL, "%5.2f", 0.0 );
	  attrset( 0 );
	  break;
	}
    }
#else
	char tuner;
	float rate;

	/* should entry area be cleared ? */
	if ( clear_or_print == C_CLEAR ) { 				/* clear it */
		call_struct.call_rate = 0.0;
		attrset(A_UNDERLINE);
		if ( scr_name == BASIC_CALL_SCR || scr_name == SUBS_BASIC_SCR ) 
			mvaddstr(B_RATE_ROW,B_RATE_COL,"       ");
		else if ( scr_name == EXTENDED_CALL_SCR || scr_name == SUBS_EXTENDED_SCR )
			mvaddstr(E_RATE_ROW,E_RATE_COL,"       ");
		attrset(0);
	}  /* end if */

	/* check type of call; rate calculations, apply only for PASS, DELIV calls */
	if ( call_struct.extended_type[0] == PASS ) 
		tuner = fleet[cur_fl_index]->pass_flat_rate_calc;
	else if ( call_struct.extended_type[0] == DELIV ) 
		tuner = fleet[cur_fl_index]->del_flat_rate_calc;
	else 
		/* its either TYPEX or WAKEUP type call */
		return(0);

	/* check how rate calculation is to be done */
	switch ( tuner )  {
		case '1':
			/* cannot enter rate */
			return(0);
		case '2':
			/* can enter rate manually */
			return(1);
		case '3':
			/* can enter rate manually; plus automatic rate calculation is zone to zone */
			if ( need_rate == YES )
				read_rate(call_struct.pckup_zone, call_struct.dest_zone);
			return(1);
		case '4':
			/* can enter rate manually; plus automatic rate calculation is zip to zip */
			if ( need_rate == YES )
				read_rate(*tzip1,*tzip2);
			return(1);
		default:
			/* error */
			prt_error(UI_FATAL,"<%s: %c>", catgets(UI_m_catd, UI_1, UI_UI_MISC_6, "invalid entry in shared memory for rate calculation"), tuner);
			return(0);
	}  /* switch */
#endif
	return( 1 );

}  /* end get_rate()  */

/* read_rate:  reads the rate for a call from the rate file */
read_rate(pkup_zone,dest_zone)
	int pkup_zone;				/* zone or zip */
	int dest_zone;				/* zone or zip */
{
	
	struct cisam_rt *rate_ptr;

	/* if any of the zone # does not exist, then do not try to get rate */
	if ( pkup_zone <= 0  ||  dest_zone <= 0 )   {
		call_struct.call_rate = 0.0;
		attrset(A_UNDERLINE);
		if ( scr_name == BASIC_CALL_SCR || scr_name == SUBS_BASIC_SCR ) 
			mvaddstr(B_RATE_ROW,B_RATE_COL,"       ");
		else if ( scr_name == EXTENDED_CALL_SCR || scr_name == SUBS_EXTENDED_SCR )
			mvaddstr(E_RATE_ROW,E_RATE_COL,"       ");
		attrset(0);
		return;
	}

	/* read the record */
	rate_ptr = (struct cisam_rt *)req_buf;
	rate_ptr->zone_zip_nbr1 = (int) MIN(pkup_zone,dest_zone);
	rate_ptr->zone_zip_nbr2 = (int) MAX(pkup_zone,dest_zone);
	rate_ptr->fleet = call_struct.fleet;


	/* First try with min zone first, maximum zone second */
	if (db(RATE_FILE_ID, READ_KEYS, &rt_key1, ISEQUAL, 0) < 0)
	  {
	    /* Try to find by reversing zones (max first, min second) */
	    
	    rate_ptr->zone_zip_nbr1 = (int) MAX(pkup_zone,dest_zone);
	    rate_ptr->zone_zip_nbr2 = (int) MIN(pkup_zone,dest_zone);
	    rate_ptr->fleet = call_struct.fleet;
	    if (db(RATE_FILE_ID, READ_KEYS, &rt_key1, ISEQUAL,0) < 0)
	      {
		prt_error(UI_DSKREAD,catgets(UI_m_catd, UI_1, UI_UI_MISC_7, "<rate file>"));
		/* clear the rate field */
		call_struct.call_rate = 0.0;
		attrset(A_UNDERLINE);
		if ( scr_name == BASIC_CALL_SCR || scr_name == SUBS_BASIC_SCR ) 
			mvaddstr(B_RATE_ROW,B_RATE_COL,"       ");
		else if ( scr_name == EXTENDED_CALL_SCR || scr_name == SUBS_EXTENDED_SCR )
			mvaddstr(E_RATE_ROW,E_RATE_COL,"       ");
		attrset(0);
		return;
	      }
	  }

	/* save the rate in call_struct */
	call_struct.call_rate = rate_ptr->flat_rate;
	/* print it on the screen */
	attrset(A_UNDERLINE);
	if ( scr_name == BASIC_CALL_SCR || scr_name == SUBS_BASIC_SCR ) 
		mvprintw(B_RATE_ROW,B_RATE_COL,"%7.2f",call_struct.call_rate);
	else if ( scr_name == EXTENDED_CALL_SCR || scr_name == SUBS_EXTENDED_SCR )
		mvprintw(E_RATE_ROW,E_RATE_COL,"%7.2f",call_struct.call_rate);
	attrset(0);

}  /* end read_rate()  */

/* get_charge_nbr: strips the argument strip passed to it free of spaces , slashes, and dashes, 
 * saves the stripped version of the string in call_struct.charge_nbr[]
 *	Returns		0	if charge # entered has not changed
 *			1	if charge # entered has changed
 */
int get_charge_nbr(read_buf)
	char *read_buf;	/* pointer to buffer with input */
{
	char *sptr;	/* pointer to walk thru read_buf[] */
	char *dptr;	/* pointer to walk thru charge_num */
	char charge_num[21];	/* to save charge # as strored on disk */
	int i;

	sptr = read_buf;
	dptr = charge_num;
	i = 0;
	while ( *sptr != '\0'  && i < 18 ) {
		if ( *sptr == ' '  ||  *sptr == '/'  ||  *sptr == '-' )
			/* do not copy this char */
			sptr++;
		else  { 
			/* copy this char */
			*dptr++ = *sptr++; 
			i++;
		}  /* end else */
	}  /* end while */
	*dptr = '\0';	 /* null terminate the string */
	/* save the charge # in call_struct */
	if ( strcmp(call_struct.charge_nbr,charge_num) == 0 )
		/* charge # entered is the same as the old entry */
		return(0);
	else  {
		/* charge # entered has changed */
		strcpy(call_struct.charge_nbr,charge_num);
		return(1);
	}  /* end else */

}  /* end get_charge_nbr()  */

/*****************************************************************************/
/* valid_charge_nbr: checks if the charge # entered is valid or not.          */
/*	Returns 	1   	if charge # is valid			      */
/*			0	if charge # is not valid		      */
/******************************************************************************/
int valid_charge_nbr(ac_ptr,cp, verify_flag)
struct cisam_ac *ac_ptr;			/* pointer to req_buf[] */
struct cisam_cl *cp;				/* pointer to call structure with info */
int             verify_flag;
{

	if (db(ACCOUNT_FILE_ID,OPEN,&ac_key1,ISINOUT+ISMANULOCK, 0) < 0) {	
		prt_error(UI_OPEN,catgets(UI_m_catd, UI_1, UI_UI_MISC_8, "<ACCOUNT_FILE>"));
		return(0);
	}

	memset(ac_ptr, '\0', sizeof(struct cisam_ac));

	ac_ptr->fleet = cp->fleet;	/* build key for reading account file */
	strcpy(ac_ptr->nbr,cp->charge_nbr);

	/* try to read the record from the account file */
	if (db_read_key(ACCOUNT_FILE_ID,ac_ptr,&ac_key1,ISEQUAL) != SUCCESS) {	
		prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_MISC_9, "<charge #>"));	/* invalid charge # entered */
		if (db(ACCOUNT_FILE_ID,CLOSE,&ac_key1,ISEQUAL,0) < 0) {		/* close the account file */
			prt_error(UI_CLOSE,catgets(UI_m_catd, UI_1, UI_UI_MISC_10, "<account file>"));
		}
		return(0);
	}

	if (db(ACCOUNT_FILE_ID,CLOSE,&ac_key1,ISEQUAL,0) < 0) {			/* close the account file */
		prt_error(UI_CLOSE,catgets(UI_m_catd, UI_1, UI_UI_MISC_10, "<account file>"));
	}

	if ( ( verify_flag == TRUE ) && ( ac_ptr->account_charge_enabled == NO ) )
	  return(0);
	
	return(1);								/* valid charge # entered */
}

/****************************************************************************
 *charge related fields
 *	if argument passed is C_CLEAR, then the function clears the fields from the 
 *	charge # to the phone extendsion fields; else if the argument is C_PRINT, 
 *	then the function tries to print info in these fields, after clearing 
 *	the old data
 ****************************************************************************/

charge_related_fields(c_or_p, ac_ptr, cptr)
	char 			 c_or_p;		
	struct cisam_ac *ac_ptr;		
	struct cisam_cl *cptr;
{
	int 			 i;				
	static char 	 blank_string[] = "                                                    "; 
	short 			 zone_num;	
	int 			 inters_exists = 0;
	int zip;
	int j, len;
	char AttributeSet;
	int AttrCol = 0, AttrRow = 0;
	short MaxAttrs;


	if (c_or_p == C_CLEAR) {	/* clear the variables from the call_struct */
		cptr->pckup_adr_cmnt[0] = '\0';
		cptr->pckup_str_nbr = 0;
		cptr->pckup_str_suf = '\0';
		cptr->pckup_pre_dir[0] = '\0';
		cptr->pckup_str_name[0] = '\0';
		cptr->pckup_str_key[0] = '\0';
		cptr->pckup_str_type[0] = '\0';
		cptr->pckup_post_dir[0] = '\0';
		cptr->intr_pre_dir[0] = '\0';
		cptr->intr_str_name[0] = '\0';
		cptr->intr_str_type[0] = '\0';
		cptr->intr_post_dir[0] = '\0';
		cptr->pckup_apt_room[0] = '\0';
		cptr->pckup_city[0] = '\0';
		cptr->pckup_zone = (short)0;
		call_zoned = 0;			/* call is not zoned */
		cptr->company_name[0] = '\0';
		cptr->ac_addt1_info1[0] = '\0';
		cptr->ac_addt1_info2[0] = '\0';
		cptr->driver_msg[0] = '\0';
		cptr->expire_date = 0;
		cptr->approve_date = 0;
		cptr->approve_by = (short)0;
		cptr->priority = 0;
		cptr->priority = get_pri(cptr);		
/*
 *		added for autobooker
 */
		cptr->veh_attr_flag = NO;
		cptr->drv_attr_flag = NO;
		memset(cptr->veh_attr, NO, sizeof(cptr->veh_attr));
		memset(cptr->drv_attr, NO, sizeof(cptr->drv_attr));

		get_zone_desc(C_PRINT, PICKUP_ADDR, cptr, NULL);
		get_est_wait_time(0);

		attrset(A_UNDERLINE);			
		for (i = E_CO_NAME1; i < E_PH_EXT; i++) {
			if (i == E_REQ_BY)
				continue;			/* do not clear reqby field */
			clear_field(stdscr,extended_scr_fld,i);
		}

/*
 *		added for autobooker
 */
		//mvaddch(E_CAR_ROW,E_CAR_COL,NO);
		attrset(A_NORMAL);
		mvaddstr( E_CAR_ROW + 1, E_CAR_ATTR1_COL, "                               " );
		//mvaddch(E_DRIVER_ROW, E_DRIVER_COL, NO);
                //		attrset(A_NORMAL);
                //		mvaddstr( E_DRIVER_ROW, E_DRIVER_ATTR1_COL, "                               " );

		attrset(0);

		/* clear the area below the charge field */
		mvaddstr(E_CHARGE_ROW+1,0,blank_string);
		mvaddstr(E_CHARGE_ROW+2,0,blank_string);
		mvaddstr(E_CHARGE_ROW+3,13,blank_string);
		mvaddstr(E_CHARGE_ROW+3,22,blank_string);
		return(0);
	}

	if (c_or_p == C_PRINT  &&  ac_ptr != NULL) {
		/* print & save additional info */
		strcpy(cptr->company_name,ac_ptr->name);	
		attrset(A_UNDERLINE);
		mvaddstr(E_CONAME1_ROW,E_CONAME_COL,"                         ");
		attrset(0);
		strcpy(cptr->driver_msg,ac_ptr->driver_msg);
		add_drv_msg_to_general_cmnt(cptr);				/* To fix bug TC0034 */
		cptr->priority = 0;
		/* check if normal defaults are less */
		cptr->priority = get_pri(cptr);	
		cptr->priority= MIN(ac_ptr->priority,cptr->priority);
		if(!doing_update)
			cptr->pri_status = cptr->priority;
		attrset(A_UNDERLINE);
		mvaddstr(E_CONAME1_ROW,E_CONAME_COL,cptr->company_name);
		mvprintw(E_PRIORITY_ROW,E_PRIORITY_COL,"%-2d",cptr->priority);
		attrset(0);
		mvprintw(E_CHARGE_ROW+1,3,"%-30s  %-15s",
					ac_ptr->header_1,ac_ptr->header_3);
		mvprintw(E_CHARGE_ROW+2,3,"%-30s  %-15s",
					ac_ptr->header_2,ac_ptr->header_4);
		mvaddstr(E_CHARGE_ROW+3,22,blank_string);
		mvaddstr(E_CHARGE_ROW+3,22,cptr->driver_msg);

                if (0)
                  {
                    attrset(A_UNDERLINE);
                    if ( ac_ptr->cash_account == YES )
                      {
                        mvaddch(E_CASH_ACCT_ROW,E_CASH_ACCT_COL,YES);
                        call_struct.cash_account = YES;
                      }
                    else
                      {
                        mvaddch(E_CASH_ACCT_ROW,E_CASH_ACCT_COL,NO);
                        call_struct.cash_account = NO;
                      }
                    attrset(0);
                  }


		for (i = 0; i < ATTR_MAX; i++)
		  {
		    if (ac_ptr->veh_attr[i] == YES)
		      {
			cptr->veh_attr[i] = YES;
			cptr->veh_attr_flag = YES;
		      }
		    else if ( cptr->veh_attr[i] != YES )
                      cptr->veh_attr[i] = NO;
		  }
		for (i = 0; i < ATTR_MAX; i++)
		  {
		    if (ac_ptr->drv_attr[i] == YES)
		      {
			cptr->drv_attr[i] = YES;
			cptr->drv_attr_flag = YES;
		      }
		    else if ( cptr->drv_attr[i] != YES )
		      cptr->drv_attr[i] = NO;
		  }
		//mvaddch(E_CAR_ROW,E_CAR_COL,cptr->veh_attr_flag);
		attrset(A_NORMAL);
		AttrCol = E_CAR_ATTR1_COL;
		AttrRow = E_CAR_ROW;
		if (1)
		  MaxAttrs = ATTR_MAX;
		else
		  MaxAttrs = 8;
		for (i = 0; i < MaxAttrs; i ++)
		  {
		    if ( cptr->veh_attr[i] == YES )
		      {
			if ( fleet[ find_findex(cptr->fleet) ]->vd_attr_info[i].veh_attr[0] != '\0' )
			  {
			    mvaddstr( E_CAR_ROW + 1, AttrCol, fleet[ find_findex(cptr->fleet) ]->vd_attr_info[i].veh_attr );
			    AttrCol += 3;
			    if ( AttrCol >= E_CAR_ATTR8_COL + 2 )
			      break;
			    AttributeSet = YES;
			  }
		      }
		  }

		attrset(A_NORMAL);
		AttrRow = E_DRIVER_ROW;
		if (1)
		  MaxAttrs = ATTR_MAX;
		else
		  MaxAttrs = 8;
		for (i = 0; i < MaxAttrs; i ++)
		  {
		    if ( cptr->drv_attr[i] == YES )
		      {
			if ( fleet[ find_findex(cptr->fleet) ]->vd_attr_info[i].drv_attr[0] != '\0' )
			  {
			    mvaddstr( E_DRIVER_ROW, AttrCol, fleet[ find_findex(cptr->fleet) ]->vd_attr_info[i].drv_attr );
			    AttrCol += 3;
			    if ( AttrCol >= E_DRIVER_ATTR8_COL + 2 )
			      break;
			    AttributeSet = YES;
			  }
		      }
		  }
		
		
		
		attrset(0);			/* set attributes back to normal mode */
		
		
	      }		
	/* check if pickup address is to be printed & sent */
		if (ac_ptr->show_pckup_adr == YES)
		  {
                    if ( !strlen(cptr->pckup_adr_cmnt) )
                      cptr->pckup_adr_cmnt[0] = '\0';
                    
			if ( ac_ptr->str_nbr < 0 )
			  ac_ptr->str_nbr = 0;

                        if ( ( cptr->pckup_str_nbr == 0 ) )
                          cptr->pckup_str_nbr = ac_ptr->str_nbr;

                        if ( cptr->pckup_str_suf == '\0' )
                          cptr->pckup_str_suf = ac_ptr->str_suf;

                        if ( !strlen(cptr->pckup_pre_dir) )
                          strcpy(cptr->pckup_pre_dir,ac_ptr->pre_dir);

                        if ( !strlen(cptr->pckup_str_name) )
                          {
                            strcpy(cptr->pckup_str_name,ac_ptr->str_name);
                            strncpy(cptr->pckup_str_key,ac_ptr->str_name,4);
                            cptr->pckup_str_key[4] = '\0';
                          }

                        if ( !strlen(cptr->pckup_str_type) )
                          strcpy(cptr->pckup_str_type,ac_ptr->str_type);

                        if ( !strlen(cptr->pckup_post_dir) )
                          strcpy(cptr->pckup_post_dir,ac_ptr->post_dir);

                        if ( !strlen(cptr->pckup_apt_room) )
                          strcpy(cptr->pckup_apt_room,ac_ptr->apt_room);

                        if ( !strlen(cptr->pckup_city) )
                          strcpy(cptr->pckup_city,ac_ptr->city);
#ifdef NAMERICA
			strcpy(cptr->phone_area,ac_ptr->phone_area);
			strcpy(cptr->phone_nbr,ac_ptr->phone);
			strcpy(cptr->phone_ext,ac_ptr->phone_ext);
#else
			/* Strip out any non alphanumeric characters and copy
			 into cptr->phone_number */
                        if ( !strlen(cptr->phone_number) )
                          {
                            len = strlen(ac_ptr->phone_number);
                            for (i = 0, j = 0; i < len; i ++)
                              if (isalnum(ac_ptr->phone_number[i]))
                                cptr->phone_number[j ++] = ac_ptr->phone_number[i];
                            cptr->phone_number[j] = '\0';
                            
			/* Search is keyed off of phone_nbr, so copy it */
                            strncpy(cptr->phone_nbr, cptr->phone_number, 7);
                            cptr->phone_nbr[7] = '\0';
                          }
#endif

			if ((zone_num = zone_addr(&call_struct,PICKUP_ADDR,&zip)) > 0) {
				call_zoned = 1;
				cptr->pckup_zone = zone_num;
				get_zone_desc(C_PRINT, PICKUP_ADDR, cptr, NULL);
				print_addr_comment(C_PRINT,PICKUP_ADDR);
				get_est_wait_time(1);
			}

/*
 *			added for autobooker
 */


			/* show the fields on a call entry screen */
			mvprintw(E_PKUP_ADDR_CMNT_ROW, E_PKUP_ADDR_CMNT_COL, "%-30s", cptr->pckup_adr_cmnt);

			attrset(A_UNDERLINE);
			if (cptr->pckup_str_nbr) {
				if (cptr->pckup_str_suf && !(0) )
					mvprintw(E_PKUP_ADDRESS_ROW,E_PKUP_ST_NO_COL,"%d%c",
							cptr->pckup_str_nbr,cptr->pckup_str_suf);
				else
					mvprintw(E_PKUP_ADDRESS_ROW,E_PKUP_ST_NO_COL,"%d",
							cptr->pckup_str_nbr);
			}

#ifdef NAMERICA
			if (cptr->pckup_pre_dir[0])
				mvaddstr(E_PKUP_ADDRESS_ROW,E_PKUP_PRE_DIR_COL,
						cptr->pckup_pre_dir);
#endif

			if (cptr->pckup_str_name[0])
				mvaddstr(E_PKUP_ADDRESS_ROW,E_PKUP_ST_NAME_COL,
						cptr->pckup_str_name);

#ifdef NAMERICA
			if (cptr->pckup_str_type[0])
				mvaddstr(E_PKUP_ADDRESS_ROW,E_PKUP_ST_TYPE_COL,
						cptr->pckup_str_type);

			if (cptr->pckup_post_dir[0])
				mvaddstr(E_PKUP_ADDRESS_ROW,E_PKUP_POST_DIR_COL,
						cptr->pckup_post_dir);

			if (cptr->intr_pre_dir[0]) {
				inters_exists = 1;
				mvaddstr(E_INTERS_ADDRESS_ROW,E_INTERS_PRE_DIR_COL,
						cptr->intr_pre_dir);
			}
#endif

			if (cptr->intr_str_name[0]) {
				inters_exists = 1;
				mvaddstr(E_INTERS_ADDRESS_ROW,E_INTERS_ST_NAME_COL,
						cptr->intr_str_name);
			}

#ifdef NAMERICA
			if (cptr->intr_str_type[0]) {
				inters_exists = 1;
				mvaddstr(E_INTERS_ADDRESS_ROW,E_INTERS_ST_TYPE_COL,
						cptr->intr_str_type);
			}
			if (cptr->intr_post_dir[0]) {
				inters_exists = 1;
				mvaddstr(E_INTERS_ADDRESS_ROW,E_INTERS_POST_DIR_COL,
						cptr->intr_post_dir);
			}
#endif

			if (cptr->pckup_apt_room[0])
				mvaddstr(E_PKUP_APT_ROW,E_PKUP_APT_COL,
						cptr->pckup_apt_room);

			if (cptr->pckup_city[0])
				mvaddstr(E_PKUP_CITY_ROW,E_PKUP_CITY_COL,
						cptr->pckup_city);

			if (cptr->pckup_zone)  
				mvprintw(E_PKUP_ZONE_ROW,E_PKUP_ZONE_COL,"%3d",
						cptr->pckup_zone);

			if (cptr->passenger_name[0])
				mvaddstr(E_PASSENGER_ROW,E_PASSENGER_COL,
						cptr->passenger_name);

#ifdef SCANDINAVIA
			if ( cptr->phone_number[0] )
			  mvaddstr(E_PHONE_ROW,E_PH_AREA_CODE_COL,cptr->phone_number);
#else	
			if (cptr->phone_area[0])
				mvaddstr(E_PHONE_ROW,E_PH_AREA_CODE_COL,cptr->phone_area);
			
			if (cptr->phone_nbr[0]) {
				mvaddstr(E_PHONE_ROW,E_PH_NUM1_COL,cptr->phone_nbr);
				mvprintw(E_PHONE_ROW,E_PH_NUM2_COL-1,"-%s",&cptr->phone_nbr[3]);
				if (cptr->phone_ext[0])				
					mvaddstr(E_PHONE_ROW,E_PH_EXT_COL,cptr->phone_ext);
			}
#endif
			
			if(cptr->call_rate)
				mvprintw(E_RATE_ROW,E_RATE_COL,"%7.2f",cptr->call_rate);

/*
 *			added for autobooker
 */



		      }

	if (!inters_exists)					/* if not an intersection */
		/* remove underlines from intersection fields */
		inters_flds(C_CLEAR,extended_scr_fld);	
}

/***********************************************************************************************************************************
add_drv_msg_to_general_cmnt -
This routine is used to add the driver msg to the general comment so that it will be sent to the mdt's
This is to fix bug TC0034
***********************************************************************************************************************************/
add_drv_msg_to_general_cmnt(cp)
struct cisam_cl *cp;						/* pointer to call structure with user supplied data */
{
    int  i, j;
    int  len, old_len;
    char driver_msg_cmnt[65];
    char *start;
    char *end_comment;
    char old_str[65];
    char new_str[130];
    static char blank_string[] = "                                                                ";
    char *spacing;

    if (*cp->driver_msg)
        sprintf(driver_msg_cmnt, "%s", cp->driver_msg);
    else
        driver_msg_cmnt[0] = '\0';

    if ((start = strchr(cp->general_cmnt, '/')) != NULL)
    {
	/* Find close paren (end of driver msg comment) */
	if ((end_comment = strchr(start + 1, '/')) == NULL)
  	    /* If not found go to complete end of string */
	    end_comment = start + strlen(start);
	else
	    /* Go past '/' */
	    end_comment ++;
	    
	/* Get part before driver msg comment */
	old_len = (int) (start - cp->general_cmnt);
	strncpy(old_str, cp->general_cmnt, old_len);
	old_str[old_len] = '\0';
	
	/* Remove trailing spaces */
	for (i = old_len - 1; i >= 0; i --)
	{
	    if (old_str[i] != ' ')
	        break;
	    else
	    {
		old_str[i] = '\0';
		old_len --;
	    }
	}

	/* Only put space at start of new comment if it's not at start of comment */
	if (old_len != 0)
	    spacing = " ";
	else
	    spacing = "";
	
	/* New general comment is part before old driver msg comment, new driver msg comment, and then
	   stuff after old driver msg comment (this might get longer than the room we have for the
	   general comment) */
	sprintf(new_str, "%s%s%s%s", old_str, spacing, driver_msg_cmnt, end_comment);
	
	/* Max length is 64 characters, so cut it off it got too long */
	new_str[65] = '\0';
	
	strcpy(cp->general_cmnt, new_str);
    }

    /* If couldn't find old driver msg comment then only add driver msg comment if there's room for it (+1 is for space) */
    else if (strlen(cp->general_cmnt) + strlen(driver_msg_cmnt)  <= 64)
    {
	if (cp->general_cmnt[0] == '\0' || cp->general_cmnt[0] == ' ')
	    strcpy(cp->general_cmnt, driver_msg_cmnt);
	else
	{
	    /* Add a space between old comment and new placename comment */
	    strcat(cp->general_cmnt, " ");
	    strcat(cp->general_cmnt, driver_msg_cmnt);
	}
    }

    /* Remove leading spaces */
    len = strlen(cp->general_cmnt);
    for (i = 0; i < len && cp->general_cmnt[i] == ' '; i ++)
    {
	for (j = 0; j < len; j ++)
	    cp->general_cmnt[i + j] = cp->general_cmnt[i + j + 1];
	len --;
    }
    
    /* Show the general comment on the screen after first blanking it out */
    attrset(A_UNDERLINE);
    mvaddstr(E_COMMENT_ROW,E_COMMENT_COL,blank_string);
    mvaddstr(E_COMMENT_ROW,E_COMMENT_COL,cp->general_cmnt);

}  /* add_drv_msg_to_general_cmnt */

int 
get_pri(cp)
struct cisam_cl *cp;
{
	int temp1_pri;					/* temp priority */
	int temp_priority;				/* temp priority */

	switch(cp->extended_type[0]) {
		case PASS:
			temp1_pri = fleet[cur_fl_index]->passenger_priority_def;
			break;
		case DELIV:
			temp1_pri = fleet[cur_fl_index]->delivery_priority_def;
			break;
		case TYPEX:
			temp1_pri = fleet[cur_fl_index]->typex_priority_def;
			break;
		case WAKEUP:
			temp1_pri = fleet[cur_fl_index]->wake_up_priority_def;
			break;
		default:
			return(-1);			/* error */
	}  /* end switch */
			
	/* check if priority is to be taken from the a/c file */
	if (cp->priority > 0  )
		/* priority should be taken from the a/c file or the one entered by user */
		temp1_pri = MIN((int)cp->priority,temp1_pri);

	switch ( cp->extended_type[2] ) { 		/* check the other char's */
		case '\0':
			return(temp1_pri);
		case SUBS:
			temp_priority = MIN(temp1_pri,fleet[cur_fl_index]->subsc_priority_def);
			break;
		case TIMEC:
			temp_priority = MIN(temp1_pri,fleet[cur_fl_index]->time_calls_priority_def);
			break;
		case CHARGE:
			temp_priority = MIN(temp1_pri,fleet[cur_fl_index]->charge_priority_def);
			break;
		case ' ':
			temp_priority = temp1_pri;  /* its probably a charge call */
			break;
		default:
			return(-1);				/* error */
	}

	switch ( cp->extended_type[3] ) { 		/* check the last char's */
		case '\0':
			return(temp_priority);
		case SUBS:
			return(MIN(temp_priority,fleet[cur_fl_index]->subsc_priority_def));
		case TIMEC:
			return(MIN(temp_priority,fleet[cur_fl_index]->time_calls_priority_def));
		case CHARGE:
			return(MIN(temp_priority,fleet[cur_fl_index]->charge_priority_def));
		default:
			return(-1);					/* error */
	}
}

/*************************************************************************/
/* inters_flds: clears the interscetion fields if argument is 'c.        */
/* prints underlines if argument is C_PRINT. \]				 */
/*************************************************************************/
inters_flds(c_or_p,ptr_scr_fld)
char c_or_p;						/* C_CLEAR = clear fields, C_PRINT = print field areas */
struct scr_flds *ptr_scr_fld;
{
	int start_piu, end_piu, i;

	if ( scr_name == BASIC_CALL_SCR || scr_name == SUBS_BASIC_SCR ) {
#ifdef SCANDINAVIA
	        start_piu = B_INTERS_ST_NAME;
	        end_piu = B_INTERS_ST_NAME;
#else
		start_piu = B_INTERS_PRE_DIR;
		end_piu = B_INTERS_POST_DIR;
#endif
	}
	else {
#ifdef SCANDINAVIA
	        start_piu = E_INTERS_ST_NAME;
	        end_piu = E_INTERS_ST_NAME;
#else
		start_piu = E_INTERS_PRE_DIR;
		end_piu = E_INTERS_POST_DIR;
#endif
	}

	if ( c_or_p == C_CLEAR )  {
		/* clear the fields in call_struct */
		call_struct.intr_pre_dir[0] = '\0';
		call_struct.intr_str_name[0] = '\0';
		call_struct.intr_str_type[0] = '\0';
		call_struct.intr_post_dir[0] = '\0';
		attrset(0);
	}  /* end if .. C_CLEAR */
	else 
		/* C_PRINT = print underlines in intersection fields */
		attrset(A_UNDERLINE);
#ifdef FOO
	for (i = start_piu; i <= end_piu; i++ )  
		clear_field(stdscr,ptr_scr_fld,i);
#endif
	attrset(0);

}  /* end inters_flds()  */
		

/* print_priority: prints the call priority in the priority field on the screen */
print_priority()
{
	if ( scr_name == BASIC_CALL_SCR || scr_name == SUBS_BASIC_SCR ) 
		mvprintw(B_PRIORITY_ROW,B_PRIORITY_COL,"%-2d",call_struct.priority);
	else if ( scr_name == EXTENDED_CALL_SCR || scr_name == SUBS_EXTENDED_SCR ) 
		mvprintw(E_PRIORITY_ROW,E_PRIORITY_COL,"%-2d",call_struct.priority);

}  /* end print_priority() */

/* ----------------------------------------------------------------------------------------------------------------------------	*
 * zone_call: tries to zone a call, this fuction is invoked whenever any of the address fields is entered on a basic or an	*
 * entended call entry screen													*
 * 	Returns -1 or zone # 													*
 * ---------------------------------------------------------------------------------------------------------------------------- */
int zone_call(ptr_scr_fld,piu) 
struct scr_flds *ptr_scr_fld;				/* pointer to array of piu structures */
int piu;  						/* piu of the field from which the call is made */
{
	struct scr_flds *ptr_to_fld;
	struct scr_flds *city_field;
	char *city_name;
	char *temp_ptr;
	short *zone_temp;				/* pointer to call_struct zone # field */
	short zone_num = 0;
	struct cisam_lo *city_ptr;			/* for disk i/o */
	int pkup_or_dest;				/* which address should be zoned */
	int i;						/* scratch variable */
	int error_zone = 0;				/* error in zoning call */

	if ( scr_name == BASIC_CALL_SCR || scr_name == SUBS_BASIC_SCR )  {
		if ( piu > B_FLEET   &&   piu <= B_PKUP_CITY /* && piu != B_PKUP_APT_NO */) {
			/* pkup zone needs to be processed */
			temp_ptr = call_struct.pckup_city;
			zone_temp = (short *)&call_struct.pckup_zone;
			pkup_or_dest =  PICKUP_ADDR;
			ptr_to_fld = ptr_scr_fld + B_PKUP_ZONE;
			city_field = ptr_scr_fld + B_PKUP_CITY;
			city_name = call_struct.pckup_city;
		}  /* end if */
		else {
			/* destinatioin zone needs to be processed */
			temp_ptr = call_struct.dest_city;
			zone_temp = (short *)&call_struct.dest_zone;
			pkup_or_dest = DEST_ADDR;
			ptr_to_fld = ptr_scr_fld + B_DEST_ZONE;
			city_field = ptr_scr_fld + B_DEST_CITY;
			city_name = call_struct.dest_city;
		}  /* end if */
	}  	/* end basic scr */
	else if(scr_name==EXTENDED_CALL_SCR || scr_name==SUBS_EXTENDED_SCR) {
		if ( piu > E_TYPE   &&   piu <= E_PKUP_CITY /* && piu != E_PKUP_APT_NO */) {
			/* pkup zone needs to be processed */
			temp_ptr = call_struct.pckup_city;
			zone_temp = (short *)&call_struct.pckup_zone;
			pkup_or_dest =  PICKUP_ADDR;
			ptr_to_fld = ptr_scr_fld + E_PKUP_ZONE;
			city_field = ptr_scr_fld + E_PKUP_CITY;
			city_name = call_struct.pckup_city;
		}  /* end if */
		else {
			/* destinatioin zone needs to be processed */
			temp_ptr = call_struct.dest_city;
			zone_temp = (short *)&call_struct.dest_zone;
			pkup_or_dest = DEST_ADDR;
			ptr_to_fld = ptr_scr_fld + E_DEST_ZONE;
			city_field = ptr_scr_fld + E_DEST_CITY;
			city_name = call_struct.dest_city;
		}  /* end else */
	}  	/* end extended screen */

	if ( *temp_ptr != '\0' )  {
		/* city field conatins an entry, try to zone call */
		city_ptr = (struct cisam_lo *)req_buf;
		strcpy(city_ptr->code,temp_ptr);
		if ( db(LOCALTY_FILE_ID,READ_KEYS,&lo_key1,ISEQUAL,0) < 0 ) {
			prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_MISC_11, "<city>")); 		/* invalid city entered */
			error_zone = 1;
		}
	}  /* end if */
	else 
		/* no entry in city field, cannot zone call */
		error_zone = 1;

	if ( ! error_zone )  {
		if ((pkup_or_dest == PICKUP_ADDR && strlen(call_struct.pckup_str_name) > 0) ||
		    (pkup_or_dest == DEST_ADDR && strlen(call_struct.dest_str_name) > 0)) {	/* do we have an address to zone */
			if ( city_ptr->subzone[0] != '\0'){			/* no addresses for this city? */
				zone_num = sz_to_zone(call_struct.fleet, city_ptr->subzone);	/* get zone for city */
				if(pkup_or_dest == PICKUP_ADDR)
					zip1 = city_ptr->zip;
				else if(pkup_or_dest == DEST_ADDR)
					zip2 = city_ptr->zip;
			}
			else{
				if(pkup_or_dest == PICKUP_ADDR)
				  {
				    zone_num = zone_addr(&call_struct,pkup_or_dest,&zip1);	/* regular address, zone it */
				    if (scr_name == EXTENDED_CALL_SCR)
				      {
					print_addr_comment(C_PRINT, PICKUP_ADDR);
				      }
				  }
				else if(pkup_or_dest == DEST_ADDR)
				  {
				    zone_num = zone_addr(&call_struct,pkup_or_dest,&zip2);	/* regular address, zone it */
				    if (scr_name == EXTENDED_CALL_SCR)
				      {
					print_addr_comment(C_PRINT, DEST_ADDR);
				      }
				  }
			}
			if ( zone_num < 0 )
				error_zone = 1;
		} /* end if */
		else
			/* no street name exists, so its an error */
			error_zone = 1;
	}  /* end if !error_zone */

	if ( error_zone )  {
		/* this call is not zoned */

	        /* On updates allow unzoned calls because the original call
		   might have been manually zoned */
	        if (doing_update && uiuser[user_index]->can_zone_calls == YES)
		  return(-1);

		*zone_temp = (short)0;						/* reset zone # to 0 = unzoned */
		attrset(A_UNDERLINE); 		/* clear any old entry which might be there in the zone field */
		move(ptr_to_fld->prompt_row,ptr_to_fld->prompt_col);
		for ( i = 0; i < ptr_to_fld->max_chars; i++ )
			addch(' ');
		attrset(0);
		get_zone_desc(C_CLEAR, pkup_or_dest, &call_struct, NULL); 	/* clear old entry */
		get_rate(C_CLEAR,NO,&zip1,&zip2);				/* sets rate = 0.0 */
		if ( pkup_or_dest == PICKUP_ADDR ) {
			get_est_wait_time(0);
			call_zoned = 0;						/* call not zoned */
			user_knows = 0;					/* user knows that call is not zoned */

			/* Clear pickup place name */
			if (scr_name == BASIC_CALL_SCR || scr_name == SUBS_BASIC_SCR )
			    mvaddstr(B_PKUP_ADDR_CMNT_ROW, B_PKUP_ADDR_CMNT_COL, "                     ");
			else if (scr_name == EXTENDED_CALL_SCR || scr_name == SUBS_EXTENDED_SCR )
			    mvaddstr(E_PKUP_ADDR_CMNT_ROW, E_PKUP_ADDR_CMNT_COL, "                     ");
			call_struct.pckup_adr_cmnt[0] = '\0';

		}  /* end if pickup address */
		else {
		    /* Clear Dest place name */
		    if (scr_name == BASIC_CALL_SCR || scr_name == SUBS_BASIC_SCR )
			mvaddstr(B_DEST_ADDR_CMNT_ROW, B_DEST_ADDR_CMNT_COL, "                     ");
		    else if (scr_name == EXTENDED_CALL_SCR || scr_name == SUBS_EXTENDED_SCR )
			mvaddstr(E_DEST_ADDR_CMNT_ROW, E_DEST_ADDR_CMNT_COL, "                     ");
		    call_struct.dest_adr_cmnt[0] = '\0';
		}
		return(-1);	/* cannot zone this call */
	}  /* end if ... call not zoned */
	else { 							/* call zoned; print zone number, etc on screen */
		*zone_temp = (short)zone_num;
		if ( zone_num )  {
			attrset(A_UNDERLINE);
			mvprintw(ptr_to_fld->prompt_row,ptr_to_fld->prompt_col, "%3d", zone_num);
			mvaddstr(city_field->prompt_row, city_field->prompt_col, city_name);
			attrset(0);
		}  /* end if */
		get_zone_desc(C_PRINT, pkup_or_dest, &call_struct, NULL);	/* get the zone description from the zone file */
		get_rate(C_PRINT,YES,&zip1,&zip2); 				/* print the rate from the rate file, if applicable */
		if ( pkup_or_dest == PICKUP_ADDR )  {
			get_est_wait_time(1); 			/* print estimated ewait time on scr, and save it */
			call_zoned = 1;				/* set flag that call is zoned */
		}  /* end if .. pickup address */
		return((int)zone_num);		/* returns zone # */
	}	/* end else */
}

/*****************************************************************************
* any_excepts - see if any exceptions on this call
*****************************************************************************/
any_excepts()
{
	unsigned char *excpt_bit_ptr;
	unsigned char mask;
	register i,j;

	excpt_bit_ptr = (unsigned char*)call_struct.status_bits;	/* must use unsigned variables */
	for(i=0;i<EXCEPT_BYTES_MAX;i++){						
		mask = 0x80;						/* set the bit mask */
		for(j=1;j<=8;j++){					/* read high bit to low */
			if(*excpt_bit_ptr & mask )			/* check if bit is set */
				return(j+(i*8));
			mask >>= 1;				/* shift the mask for next attribute */
		}
		++excpt_bit_ptr;					/* point to the next 8 bits */
	}

	return(0);							/* no excepts */
}
