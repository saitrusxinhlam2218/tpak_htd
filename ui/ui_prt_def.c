static char sz__FILE__[]="@(#)ui_prt_def.c	9.1.1.3(ver) Date Release 7/22/94" ;
/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  ui_prt_def.c; Rel: 9.1.1.3; Get date: 7/24/94 at 16:10:44
* @(#)  Last delta: 7/22/94 at 14:03:19
* @(#)  SCCS File: /taxi/sccs/s.ui_prt_def.c
*                                                                           *
* Copyright (c) 1990 - MDI Seattle                                          *
*                                                                           *
*****************************************************************************/

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
#include <sys/types.h>
#include <sys/time.h>
#include <langinfo.h>

#include "user.h"
#include "mads_types.h"
#include "mads_isam.h"
#include "ui_def.h"
#include "ui_ipc.h"
#include "ui_error.h"
#include "call_entry.h"
#include "screens.h"
#include "calls.h"
#include "writer.h"
#include "zgeo.h"
#include "language.h"
#include "ui_strdefs.h"
#include "enhance.h"
#include "switch_ext.h"

#ifndef __alpha
#define		MIN(x,y)	( (x) < (y) ? (x):(y) )
#endif

extern int scr_name;				/* name of screen on display */
extern struct users *uiuser[];			/* array of pointers to users structure in shared memory */
extern int user_index;				/* index into users structure in shared memory */
extern int uid;					/* user's id # */
extern int def_fl_index;			/* index into fleets structure in shared mem */
extern struct cisam_cl call_struct;		/* for disk i/o on call file */
extern struct cisam_sc 	subs_struct;
extern struct call_ipc_rec call_fifo;		/* for writing call rec's to FIFO */
extern struct fleets *fleet[];			/* array of pointers to fleets structure in shared memory */
extern int cur_fl_index;			/* current fleets index which the user is using */
extern struct scr_flds basic_scr_flds[], extended_scr_fld[];
extern int doing_detail;
extern char req_buf[];

/* b_prt_default: prints the default values for the user on a basic screen.
 *		Returns:
 *			1 if input_fleet is valid, and default values are 
 *			printed
 *			-1 if input_fleet is not_valid/non_existent and default
 *			values are not printed
 */

int 
b_prt_default()
{
	int 	i;			/* scratch variable */
	int 	findex;			/* index into fleets structure in shared memory */
	char 	date_str[10];		/* for formatting date */
	char 	time_str[10];		/* for formatting time */
	char	temp_date[10];
	struct tm *pDateTime;
	char  *pDateDescript;

	/* get the index into the fleets structure */
	if((findex = def_fl_index) == -1) {
		/* non existent fleet specified as default fleet */
		prt_error(UI_NOFLEET,catgets(UI_m_catd, UI_1, UI_UI_PRT_DEF_1, "<wrong default fleet>"));
		/* reset attributes */
		attrset(0);
		return(-1);  /* error */
	}  /* end if ... non existent fleet */

	attrset(A_UNDERLINE);
	/* the fleet specified is an existent fleet */
	/* print the default fleet */
	mvaddch(B_FLEET_ROW,B_FLEET_COL,fleet[findex]->fleet_id);
	/* save this fleet in the call_struct */
	call_struct.fleet = fleet[findex]->fleet_id;
	/* save the fleet in the call_fifo struct for writing to FIFO */
	call_fifo.fleet = fleet[findex]->fleet_id;

	/* print the default call type from user struct in shared memory */
	mvaddch(B_FLEET_ROW,B_CL_TYPE_COL, uiuser[user_index]->default_call_type);

	/* save this in call_struct & call_fifo */
	call_struct.extended_type[0] = uiuser[user_index]->default_call_type;
	call_struct.extended_type[1] = ' ';
	call_struct.extended_type[3] = '\0';
	if(scr_name == BASIC_CALL_SCR)
		call_struct.extended_type[2] = '\0';
	else 								/* its a subs screen */
		sprintf(&call_struct.extended_type[2],"%c",SUBS);

	get_call_type();

	/* if the user has not defined the default call type, then print it 
	 * it from the fleets structure in shared memory
	 */

	/* print the default city from the user or the fleet struct in 
	 * shared mem */
	if(uiuser[user_index]->default_city[0] == '\0') {
		/* user has not defined a default city; so print it
		 * from the fleet struct in  shared memory 
		 */
		mvaddstr(B_PKUP_CITY_ROW,B_PKUP_CITY_COL, fleet[findex]->default_city);
		strcpy(call_struct.pckup_city,fleet[findex]->default_city);
	}
	else {
		/* user has defined a default city */
		mvaddstr(B_PKUP_CITY_ROW,B_PKUP_CITY_COL,
			uiuser[user_index]->default_city);
		strcpy(call_struct.pckup_city,uiuser[user_index]->default_city);
	}

#ifdef NAMERICA
	/* print the default area code */
	if ( (fleet[findex]->default_phone_area[0] != '\0') &&
		(strcmp(fleet[findex]->default_phone_area,"000") != 0) ) {
		/* print the default area code */
		mvaddstr(B_PHONE_ROW,B_PH_AREA_CODE_COL,
			fleet[findex]->default_phone_area);
	}
#endif

	/* print the default PU Date or Date Call Entered; depending upon
	 * whether the screen is a BASIC call entry screen or a subscription
	 * call entry screen
	 */
	if ( scr_name == BASIC_CALL_SCR ) {
		/* print the PU Date */
		get_pkup_date(date_str);
		mvaddstr(B_PKUP_DATE_ROW,B_PKUP_DATE_COL, date_str);
		/* get the formatted time */
		get_pkup_time(time_str);
		mvaddstr(B_PKUP_DATE_ROW,B_PKUP_TIME_COL, time_str);
		/* save this in call_struct & call_fifo */
		/* COPY IN CALL_STRUCT, ETC..,, */
		strcpy(call_struct.due_date,date_str);
		strcpy(call_struct.due_time,time_str);
		call_struct.due_date_time = get_internal_time( call_struct.due_date, call_struct.due_time );
#ifdef FOO
		pDateTime = localtime( &call_struct.due_date_time );
		if ( pDateTime != NULL )
		  {
		    pDateDescript = nl_langinfo( DAY_1 + pDateTime->tm_wday );
		    if ( pDateDescript != NULL )
		      {
			attrset( 0);
			mvprintw( B_PKUP_DATE_ROW, B_PKUP_DATE_COL + 24, "(%s)", pDateDescript );
			attrset( A_REVERSE|A_UNDERLINE); 
		      }
		  }
#endif
	      }
	else {
		/* scr_name == SUBS_BASIC */
		/* print the PU Date */
		get_pkup_date(date_str);
		attrset(0);
		mvaddstr(B_PKUP_DATE_ROW,B_PKUP_DATE_COL, date_str);
/*		mvprintw(B_PKUP_DATE_ROW,B_ENTERED_BY_COL,"%d", uid); */
		attrset(A_UNDERLINE);
		/* print todays date as the default for the Beginning date field 
		 * in the subscription column
		 */
		mvprintw(SUBS_ROW2,SUBS_BEGINDATE_COL,date_str);
		/* COPY INFO IN CALL_STRUCT & CALL_FIFO */
		strcpy(call_struct.due_date,date_str);
		strcpy(call_struct.date_in,date_str);
		sprintf(temp_date, "%02d%02d%02d", atoi(&date_str[0]), atoi(&date_str[3]), atoi(&date_str[6]));
		subs_struct.from_date = atoi(temp_date);
	}

	attrset(A_UNDERLINE);
	call_struct.veh_attr_flag = NO;				/* print default car needs/attributes. */
	for ( i = 0; i < ATTR_MAX; i++ )
	  call_struct.veh_attr[i] = NO;
        //	mvaddch(B_CAR_ROW,B_CAR_COL,NO);

        //	mvaddch(B_DRIVER_ROW, B_DRIVER_COL, NO);			/* print the driver attributes */
	call_struct.drv_attr_flag = NO;
	for ( i = 0; i < ATTR_MAX; i++ )
	  call_struct.drv_attr[i] = NO;

	/* print the default priority; since the default call type is
	 * a 1 char field, therefore we should get this default call
	 * priority by checking this byte
	 */
	if((call_struct.priority = get_call_pri()) == -1){	 			/* get the call priority and save it */
		call_struct.priority = fleet[cur_fl_index]->passenger_priority_def;	/* default to passenger */
	}
	call_struct.pri_status = call_struct.priority;
	call_fifo.priority = call_struct.priority;
	mvprintw(B_PRIORITY_ROW,B_PRIORITY_COL,"%d", call_struct.priority);
	
	call_struct.pod_required = NO;
	mvaddch(E_POD_ROW,E_POD1_COL, call_struct.pod_required);

	/* print the default for "local" label */
	call_struct.local = NO;
	mvaddch(B_LOCAL_ROW,B_LOCAL_COL,NO);

	/* print the default for "advise_arrival" label */
	call_struct.advise_arrival = NO;
	mvaddch(B_ADVISE_ARRIVAL_ROW,B_ADVISE_ARRIVAL_COL,NO);

#ifdef NAMERICA
	if (fleet[findex]->send_dest_addr == YES)
	  call_struct.send_destination = YES;
	else
	  call_struct.send_destination = NO;
	mvaddch(B_SEND_DEST_ADDR_ROW, B_SEND_DEST_ADDR_COL, call_struct.send_destination);

	if (fleet[findex]->send_rate == YES)
	  call_struct.send_rate = YES;
	else
	  call_struct.send_rate = NO;
	mvaddch(B_SEND_RATE_ROW, B_SEND_RATE_COL, call_struct.send_rate);	
#endif

	/* print the # of vehicles to be used to service call */
	call_struct.nbr_of_vehs = '1';  /* always 1 */
	mvprintw(B_CAR_NO_ROW,B_CAR_NO_COL,"%c", call_struct.nbr_of_vehs);

	/* set the screen type in call_struct */
/* NOT USED ANYWHERE	call_struct.screen_type = 'B';	/* basic screen */

	/* set the call status to UNASSGND for now calls, and set it to 
	 * "PENDING" for future and subscription calls
	 */
	strcpy(call_struct.status,UNASSGND);

	mvaddch(B_VIA_ADDRESS_ROW, B_VIA_ADDRESS_COL, '0');
	call_struct.via_addresses = 0;

	/* reset attributes */
	attrset(0);

	return(1);	 /* returns done */

}  /* end b_prt_default() */

/************************************************************************************/
/* w_prt_default: prints the default values for the user on a wakeup screen.	    */
/*		Returns: 1 if input_fleet is valid, and default values are printed  */
/*			-1 if input_fleet is not_valid/non_existent and default     */
/*			   values are not printed 				    */
/************************************************************************************/
int w_prt_default()
{

	int i;					/* scratch variable */
	int findex;				/* index into fleets structure in shared memory */
	char date_str[10];			/* for formatting date */
	char time_str[10];			/* for formatting time */
        char temp_date[10];


	attrset(A_UNDERLINE); 			/* set underlined attributes */

	if ( (findex = def_fl_index ) == -1 ) { 			/* get the index into the fleets structure */
		prt_error(UI_NOFLEET,catgets(UI_m_catd, UI_1, UI_UI_PRT_DEF_1, "<wrong default fleet>")); 		/* non existent fleet specified as default fleet */
		attrset(0); 						/* reset attributes */
		return(-1);  						/* error */
	}  								/* end if ... non existent fleet */

	mvaddch(W_FLEET_ROW,B_FLEET_COL,fleet[findex]->fleet_id); 	/* print the default fleet */
	call_struct.fleet = fleet[findex]->fleet_id; 			/* save this fleet in the call_struct */
	call_fifo.fleet = fleet[findex]->fleet_id; 			/* save the fleet in the call_fifo struct */

	call_struct.extended_type[0] = WAKEUP; 				/* let the default call type be wakeup call */
	call_struct.extended_type[1] = '\0';
	call_struct.extended_type[2] = '\0';
	call_struct.extended_type[3] = '\0';
	get_call_type();

#ifdef NAMERICA
	if ( (fleet[findex]->default_phone_area[0] != '\0') && (strcmp(fleet[findex]->default_phone_area,"000") != 0) ) {
		mvaddstr(W_PHONE_ROW,B_PH_AREA_CODE_COL,fleet[findex]->default_phone_area); 	/* print the default area code */
	}
#endif

	if ( scr_name == WAKEUP_CALL_SCR ) {
		get_pkup_date(date_str); 				/* print the PU Date */
		mvaddstr(W_PKUP_DATE_ROW,B_PKUP_DATE_COL, date_str);
		get_pkup_time(time_str); 				/* get the formatted time */
		mvaddstr(W_PKUP_DATE_ROW,B_PKUP_TIME_COL, time_str);
		strcpy(call_struct.due_date,date_str); 			/* save this in call_struct & call_fifo */
		strcpy(call_struct.due_time,time_str);
	}  
	else { 								/* scr_name == SUBS_WAKEUP */
		get_pkup_date(date_str); 				/* print the PU Date */
		attrset(0);
		mvaddstr(W_PKUP_DATE_ROW,B_PKUP_DATE_COL, date_str);
/*		mvprintw(W_PKUP_DATE_ROW,B_ENTERED_BY_COL,"%d", uid); */
		attrset(A_UNDERLINE);
		mvprintw(SUBS_ROW2,SUBS_BEGINDATE_COL,date_str); 	/* Beginning date = todays date (default) */
		strcpy(call_struct.due_date,date_str); 			/* COPY INFO IN CALL_STRUCT & CALL_FIFO */
		strcpy(call_struct.date_in, date_str); 			/* COPY INFO IN CALL_STRUCT & CALL_FIFO */
                sprintf(temp_date, "%02d%02d%02d", atoi(&date_str[0]), atoi(&date_str[3]), atoi(&date_str[6]));
                subs_struct.from_date = atoi(temp_date);
	}

/* NOT USED ANYWHERE	call_struct.screen_type = 'W'; 					/* set the screen type in call_struct */

	if((call_struct.priority = get_call_pri()) == -1){ 				/* get the call priority and save it */
		call_struct.priority = fleet[cur_fl_index]->passenger_priority_def;	/* default to passenger */
	}
	call_struct.pri_status = call_struct.priority;
	call_fifo.priority = call_struct.priority;

	strcpy(call_struct.status,UNASSGND); 				/* set the call status to UNASSGND */

	attrset(0);

	return(1);							/* return for all done */

}  									/* end w_prt_default() */



/* e_prt_default: prints the default values for the user on a extended screen.
 *		Returns:
 *			1 if input_fleet is valid, and default values are 
 *			printed
 *			-1 if input_fleet is not_valid/non_existent and default
 *			values are not printed
 */
int e_prt_default()
{

	int i;	/* scratch variable */
	int findex;	/* index into fleets structure in shared memory */
	char date_str[10];	/* for formatting date */
	char time_str[10];	/* for formatting time */
	char	temp_date[10];
	struct tm *pDateTime;
	char  *pDateDescript;

	/* set underlined attributes */
	attrset(A_UNDERLINE);

	/* get the index into the fleets structure */
	if ( (findex = def_fl_index ) == -1 ) {
		/* non existent fleet specified as default fleet */
		prt_error(UI_NOFLEET,catgets(UI_m_catd, UI_1, UI_UI_PRT_DEF_1, "<wrong default fleet>"));
		/* reset attributes */
		attrset(0);
		return(-1);  /* error */
	}  /* end if ... non existent fleet */

	/* the fleet specified is an existent fleet */
	/* print the default fleet */
	mvaddch(E_FLEET_ROW,E_FLEET_COL,fleet[findex]->fleet_id);
	/* save this fleet in the call_struct */
	call_struct.fleet = fleet[findex]->fleet_id;
	/* save the fleet in the call_fifo struct for writing to FIFO */
	call_fifo.fleet = fleet[findex]->fleet_id;

	/* print the default call type from user struct in shared memory */
	mvaddch(E_FLEET_ROW,E_CL_TYPE_COL, uiuser[user_index]->default_call_type);

	/* save this in call_struct & call_fifo */
	call_struct.extended_type[0] = uiuser[user_index]->default_call_type;
	call_struct.extended_type[1] = ' ';
	call_struct.extended_type[3] = '\0';
	if ( scr_name == EXTENDED_CALL_SCR )
		call_struct.extended_type[2] = '\0';
	else 									/* its a subs screen */
		sprintf(&call_struct.extended_type[2],"%c",SUBS);

	get_call_type();

	/* if the user has not defined the default call type, then print it 
	 * it from the fleets structure in shared memory
	 */

	/* print the default city from the user or the fleet struct in 
	 * shared mem */
	if ( uiuser[user_index]->default_city[0] == '\0' ) {
		/* user has not defined a default city; so print it
		 * from the fleet struct in  shared memory 
		 */
		mvaddstr(E_PKUP_CITY_ROW,E_PKUP_CITY_COL, 
			fleet[findex]->default_city);
		strcpy(call_struct.pckup_city,fleet[findex]->default_city);
	}  /* end if */
	else {
		/* user has defined a default city */
		mvaddstr(E_PKUP_CITY_ROW,E_PKUP_CITY_COL,
			uiuser[user_index]->default_city);
		strcpy(call_struct.pckup_city,uiuser[user_index]->default_city);
	}  /* end else */

#ifdef NAMERICA
	/* print the default area code */
	if ( (fleet[findex]->default_phone_area[0] != '\0') &&
		(strcmp(fleet[findex]->default_phone_area,"000") != 0) ) {
		/* print the default area code */
		mvaddstr(E_PHONE_ROW,E_PH_AREA_CODE_COL,
			fleet[findex]->default_phone_area);
	}  /* end if ... phone_area_code */
#endif

        if (0)
          mvaddch(E_CASH_ACCT_ROW,E_CASH_ACCT_COL,NO);
        call_struct.cash_account = NO;
        
	/* print the default PU Date or Date Call Entered; depending upon
	 * whether the screen is a BASIC call entry screen or a subscription
	 * call entry screen
	 */
	if ( scr_name == EXTENDED_CALL_SCR ) {
		/* print the PU Date */
		get_pkup_date(date_str);
		mvaddstr(E_PKUP_DATE_ROW,E_PKUP_DATE_COL, date_str);
		/* get the formatted time */
		get_pkup_time(time_str);
		mvaddstr(E_PKUP_DATE_ROW,E_PKUP_TIME_COL, time_str);
		/* save this in call_struct & call_fifo */
		/* COPY IN CALL_STRUCT, ETC..,, */
		strcpy(call_struct.due_date,date_str);
		strcpy(call_struct.due_time,time_str);
		call_struct.due_date_time = get_internal_time( call_struct.due_date, call_struct.due_time );
#ifdef FOO
		pDateTime = localtime( &call_struct.due_date_time );
		if ( pDateTime != NULL )
		  {
		    pDateDescript = nl_langinfo( DAY_1 + pDateTime->tm_wday );
		    if ( pDateDescript != NULL )
		      {
			attrset( 0);
			mvprintw( E_PKUP_DATE_ROW, E_PKUP_DATE_COL + 24, "(%s)", pDateDescript );
			attrset( A_REVERSE|A_UNDERLINE); 
		      }
		  }
#endif
	      }  
	else {
		/* scr_name == SUBS_EXTENDED */
		/* print the PU Date */
		get_pkup_date(date_str);
		attrset(0);
		mvaddstr(E_PKUP_DATE_ROW,E_PKUP_DATE_COL, date_str);
/*		mvprintw(E_PKUP_DATE_ROW,B_ENTERED_BY_COL,"%d", uid); */
		attrset(A_UNDERLINE);
		/* print todays date as the default for the Beginning date field 
		 * in the subscription column
		 */
		mvprintw(SUBS_ROW2,SUBS_BEGINDATE_COL,date_str);
		/* COPY INFO IN CALL_STRUCT & CALL_FIFO */
		strcpy(call_struct.due_date,date_str);
		strcpy(call_struct.date_in, date_str);
		sprintf(temp_date, "%02d%02d%02d", atoi(&date_str[0]), atoi(&date_str[3]), atoi(&date_str[6]));
		subs_struct.from_date = atoi(temp_date);
	}

	attrset( A_UNDERLINE );
        //	mvaddch(E_CAR_ROW,E_CAR_COL,NO);				/* print default car needs/attributes. */
	call_struct.veh_attr_flag = NO;

        //	mvaddch(E_DRIVER_ROW, E_DRIVER_COL, NO);			/* print the driver attributes */
	call_struct.drv_attr_flag = NO;


	/* print the default priority; since the default call type is
	 * a 1 char field, therefore we should get this default call
	 * priority by checking this byte
	 */
	if((call_struct.priority = get_call_pri()) == -1){ 				/* get the call priority and save it */
		call_struct.priority = fleet[cur_fl_index]->passenger_priority_def;	/* default to passenger */
	}
	call_struct.pri_status = call_struct.priority;
	call_fifo.priority = call_struct.priority;
	attrset(A_UNDERLINE);
	mvprintw(E_PRIORITY_ROW,E_PRIORITY_COL,"%d", call_struct.priority);

	/* print the default for "local" label */
	call_struct.local = NO;
	mvaddch(E_LOCAL_ROW,E_LOCAL_COL,NO);
	
	/* print the default for "advise arrival" label */
	call_struct.advise_arrival = NO;
	mvaddch(E_ADVISE_ARRIVAL_ROW,E_ADVISE_ARRIVAL_COL,NO);

#ifdef NAMERICA
	call_struct.send_destination = YES;
	mvaddch(E_SEND_DEST_ADDR_ROW, E_SEND_DEST_ADDR_COL, YES);

	if (fleet[findex]->send_rate == YES)
	  call_struct.send_rate = YES;
	else
	  call_struct.send_rate = NO;
	mvaddch(E_SEND_RATE_ROW, E_SEND_RATE_COL, call_struct.send_rate);
#endif

	/* print the # of vehicles to be used to service call */
	call_struct.nbr_of_vehs = '1';
	mvprintw(E_CAR_NO_ROW,E_CAR_NO_COL,"%c", call_struct.nbr_of_vehs);

	/* set the screen type in call_struct */
/* NOT USED ANYWHERE	call_struct.screen_type = 'E';	/* basic screen */

	/* set the call status to UNASSGND */
	strcpy(call_struct.status,UNASSGND);

	call_struct.via_addresses = 0;
	mvaddch( E_VIA_ADDRESS_ROW, E_VIA_ADDRESS_COL, '0');

	/* reset attributes */
	attrset(0);

	return(1);	 /* returns done */

}  /* end e_prt_default() */

/***************************************************************************/
/* get_call_pri: gets the call priority, after looking at the call type    */
/*		in call_struct.extended_type				   */
/*		Returns: call priority					   */
/*  			 -1 on error 					   */
/***************************************************************************/
int get_call_pri()
{
	int temp1_pri;					/* temp priority */
	int temp_priority;				/* temp priority */


	switch (call_struct.extended_type[0] ) {
		case PASS:
		case 'K':
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
	if ( call_struct.priority > 0  )
		/* priority should be taken from the a/c file or the one entered by user */
		temp1_pri = MIN((int)call_struct.priority,temp1_pri);

	switch ( call_struct.extended_type[2] ) { 		/* check the other char's */
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
	}  /* end switch */

	switch ( call_struct.extended_type[3] ) { 		/* check the last char's */
		case '\0':
	        case ' ':
			return(temp_priority);
		case SUBS:
			return(MIN(temp_priority,fleet[cur_fl_index]->subsc_priority_def));
		case TIMEC:
			return(MIN(temp_priority,fleet[cur_fl_index]->time_calls_priority_def));
		case CHARGE:
			return(MIN(temp_priority,fleet[cur_fl_index]->charge_priority_def));
		default:
			return(-1);					/* error */

	}  /* end switch */

}  /* end get_call_pri() */

/* get_call_type: gets the call type, after looking at the call type
 *		in call_struct.extended_type
 *		Seta appropriate bits in call_fifo.call_type
 */
get_call_type()
{

	memset((char *)&call_fifo.call_type,'\0', sizeof(struct call_types));

	switch (call_struct.extended_type[0] ) {
		case PASS:
			call_fifo.call_type.passenger = 1;
			break;
		case DELIV:
			call_fifo.call_type.delivery = 1;
			break;
		case TYPEX:
			call_fifo.call_type.xtype = 1;
			break;
		case WAKEUP:
			call_fifo.call_type.wakeup = 1;
			break;
	}  /* end switch */
			
	if(call_struct.extended_type[4] == MULT_CALL)
		call_fifo.call_type.multiple = 1;

	if ( call_struct.extended_type[1] == '\0' )
		return;

	/* check the other char's */
	switch ( call_struct.extended_type[2] ) {
		case '\0':
			return;
		case SUBS:
			call_fifo.call_type.subscription = 1;
			break;
		case TIMEC:
			call_fifo.call_type.time = 1;
			break;
		case CHARGE:
			call_fifo.call_type.charge = 1;
			break;
	}  /* end switch */

	/* check the last char's */
	switch ( call_struct.extended_type[3] ) {
		case '\0':
			return;
		case SUBS:
			call_fifo.call_type.subscription = 1;
			break;
		case TIMEC:
			call_fifo.call_type.time = 1;
			break;
		case CHARGE:
			call_fifo.call_type.charge = 1;
			break;

	}  /* end switch */

}  /* end get_call_type() */


/* prt_bold_fields: prints the fields on the basic call entry screen,
 *		for previous screen/toggle function keys entered
 */
prt_bold_fields(cp)
	struct cisam_cl *cp;	/* pointer to call_struct */
{
	
	int i;
	char AttributeSet;
	int AttrCol = 0, AttrRow = 0;
	short MaxAttrs;	

	/* print the fields at appropriate positions on a call
	 * entry screen
	 */
	attrset(A_UNDERLINE);
	mvaddch(B_FLEET_ROW,B_FLEET_COL,cp->fleet);
	mvprintw(B_CL_TYPE_ROW,B_CL_TYPE_COL,"%c%c",cp->extended_type[0],
		cp->extended_type[1]);
	if ( cp->pckup_str_nbr ) {
		if ( cp->pckup_str_suf && !(0) )
			mvprintw(B_PKUP_ADDRESS_ROW,B_PKUP_ST_NO_COL,
				"%d%c",cp->pckup_str_nbr,cp->pckup_str_suf);
		else
			mvprintw(B_PKUP_ADDRESS_ROW,B_PKUP_ST_NO_COL,
				"%d",cp->pckup_str_nbr);
	}  /* end if */

#ifdef NAMERICA
	if ( cp->pckup_pre_dir[0] )
		mvaddstr(B_PKUP_ADDRESS_ROW,B_PKUP_PRE_DIR_COL,
			cp->pckup_pre_dir);
#endif

	if ( cp->pckup_str_name[0] )  {
		if ( cp->intr_str_name[0] ) 
			mvprintw(B_PKUP_ADDRESS_ROW,B_PKUP_ST_NAME_COL,"%s/",cp->pckup_str_name);
		else
			mvaddstr(B_PKUP_ADDRESS_ROW,B_PKUP_ST_NAME_COL,cp->pckup_str_name);
	}

#ifdef NAMERICA
	if ( cp->pckup_str_type[0] )
		mvaddstr(B_PKUP_ADDRESS_ROW,B_PKUP_ST_TYPE_COL,
			cp->pckup_str_type);

	if ( cp->pckup_post_dir[0] )
		mvaddstr(B_PKUP_ADDRESS_ROW,B_PKUP_POST_DIR_COL,
			cp->pckup_post_dir);
#endif

	if ( cp->intr_str_name[0] ) {
		inters_flds(C_PRINT, basic_scr_flds);
		attrset(A_UNDERLINE);
		mvaddstr(B_INTERS_ROW,B_INTERS_ST_NAME_COL,cp->intr_str_name);
		attrset(0);
	}

#ifdef NAMERICA
	if ( cp->intr_pre_dir[0] )
		mvaddstr(B_INTERS_ROW,B_INTERS_PRE_DIR_COL,
			cp->intr_pre_dir);

	if ( cp->intr_str_type[0] )
		mvaddstr(B_INTERS_ROW,B_INTERS_ST_TYPE_COL,
			cp->intr_str_type);
	
	if ( cp->intr_post_dir[0] )
		mvaddstr(B_INTERS_ROW,B_INTERS_POST_DIR_COL,
			cp->intr_post_dir);
#endif

	if ( cp->pckup_apt_room[0] )
		mvaddstr(B_PKUP_APT_ROW,B_PKUP_APT_COL,
			cp->pckup_apt_room);

	/* print the pkup address comment */
	attrset(0);
	if ( cp->pckup_adr_cmnt[0] )
		mvaddstr(B_PKUP_ADDR_CMNT_ROW,B_PKUP_ADDR_CMNT_COL,
			cp->pckup_adr_cmnt);
	if ( cp->ct_adr_cmnt[0] )
	  mvaddstr( B_CT_CMNT_ROW, B_CT_CMNT_COL,
		   cp->ct_adr_cmnt );
	
	attrset(A_UNDERLINE);

	if ( cp->pckup_city[0] )
		mvaddstr(B_PKUP_CITY_ROW,B_PKUP_CITY_COL,
			cp->pckup_city);

	if ( cp->pckup_zone )
		mvprintw(B_PKUP_ZONE_ROW,B_PKUP_ZONE_COL, "%d",
			cp->pckup_zone);
	
	/* print the pkup zone comment */
	if ( cp->zone_desc[0] ) {
		attrset(0);
		mvaddstr(B_PKUP_ZONE_ROW,B_PKUP_ZONE_COL+6,cp->zone_desc);
		attrset(A_UNDERLINE);
	}

	/* print the estimated wait time */
	if ( cp->est_wait_time[0] ) {
		attrset(0);
		mvprintw(LINES-2,0,"%s %s",cp->est_wait_time, catgets(UI_m_catd, UI_1, UI_UI_PRT_DEF_3, "MINUTES") );
		attrset(A_UNDERLINE);
	}  /* end if */

	if ( cp->passenger_name[0] )
		mvaddstr(B_PASSENGER_ROW,B_PASSENGER_COL,cp->passenger_name);

        if ( cp->requested_by[0] )
          mvaddstr(B_REQBY_ROW,B_REQBY_COL,cp->requested_by);


#ifdef SCANDINAVIA
	if (cp->phone_number[0] )
		mvaddstr(B_PHONE_ROW,B_PH_AREA_CODE_COL,cp->phone_number);

#else
	if ( cp->phone_area[0] )
		mvaddstr(B_PHONE_ROW,B_PH_AREA_CODE_COL,cp->phone_area);
	if ( cp->phone_nbr[0] ) {
		mvaddstr(B_PHONE_ROW,B_PH_NUM1_COL,cp->phone_nbr);
		mvprintw(B_PHONE_ROW,B_PH_NUM2_COL-1,"-%s",&cp->phone_nbr[3]);
		/* print phone ext if any */
		if ( cp->phone_ext[0] ) 
			mvaddstr(B_PHONE_ROW,B_PH_EXT_COL,cp->phone_ext);
	}
#endif

	if (scr_name == BASIC_CALL_SCR) {
	    mvaddstr(B_PKUP_DATE_ROW,B_PKUP_DATE_COL,cp->due_date);
	    mvaddstr(B_PKUP_TIME_ROW,B_PKUP_TIME_COL,cp->due_time);
	}
	else { /* SUBS_BASIC_SCR */
	    attrset(0);
	    mvaddstr(B_PKUP_DATE_ROW, B_PKUP_DATE_COL,cp->date_in);
	    attrset(A_UNDERLINE);
	}


	if ( cp->general_cmnt[0] )
		mvaddstr(B_COMMENT_ROW,B_COMMENT_COL,cp->general_cmnt);

	/* print the destination address info */
	if ( cp->dest_str_nbr ) {
		if ( cp->dest_str_suf )
			mvprintw(B_DEST_ADDRESS_ROW,B_DEST_ST_NO_COL,
				"%d%c",cp->dest_str_nbr,cp->dest_str_suf);
		else
			mvprintw(B_DEST_ADDRESS_ROW,B_DEST_ST_NO_COL,
				"%d",cp->dest_str_nbr);
	}  /* end if */

#ifdef NAMERICA
	if ( cp->dest_pre_dir[0] )
		mvaddstr(B_DEST_ADDRESS_ROW,B_DEST_PRE_DIR_COL,
			cp->dest_pre_dir);
#endif

	if ( cp->dest_str_name[0] )
		mvaddstr(B_DEST_ADDRESS_ROW,B_DEST_ST_NAME_COL,
			cp->dest_str_name);

#ifdef NAMERICA
	if ( cp->dest_str_type[0] )
		mvaddstr(B_DEST_ADDRESS_ROW,B_DEST_ST_TYPE_COL,
			cp->dest_str_type);

	if ( cp->dest_post_dir[0] )
		mvaddstr(B_DEST_ADDRESS_ROW,B_DEST_POST_DIR_COL,
			cp->dest_post_dir);
#endif

	/* print the dest address comment */
	attrset(0);
	if ( cp->dest_adr_cmnt[0] )
		mvaddstr(B_DEST_ADDR_CMNT_ROW,B_DEST_ADDR_CMNT_COL,
			cp->dest_adr_cmnt);
	attrset(A_UNDERLINE);

	if ( cp->dest_apt_room[0] )
		mvaddstr(B_DEST_APT_ROW,B_DEST_APT_COL,
			cp->dest_apt_room);

	if ( cp->dest_city[0] )
		mvaddstr(B_DEST_CITY_ROW,B_DEST_CITY_COL,
			cp->dest_city);

	if ( cp->dest_zone )  {
		mvprintw(B_DEST_ZONE_ROW,B_DEST_ZONE_COL, "%d",
			cp->dest_zone);
	        attrset(0);
		get_zone_desc(C_PRINT, DEST_ADDR, cp, NULL);
		attrset(A_UNDERLINE);
	}


	mvprintw(B_VIA_ADDRESS_ROW, B_VIA_ADDRESS_COL, "%1d", cp->via_addresses );
	
	if ( cp->personal_veh ) {
		mvprintw(B_PERSONNEL_ROW,B_PERSONEL1_COL,"%d",
			cp->personal_veh);
		mvaddch(B_PERSONNEL_ROW,B_PERSONEL2_COL,cp->personal_rqst);
	}  /* end if */

	attrset(0);
	mvaddstr( B_CAR_ROW, B_CAR_ATTR1_COL, "                               " );
	AttrCol = B_CAR_ATTR1_COL;
	AttrRow = B_CAR_ROW;
	if (1)
	  MaxAttrs = ATTR_MAX;
	else
	  MaxAttrs = 8;
	attrset( A_UNDERLINE );
	
	for ( i = 0; i < MaxAttrs; i++ )
	  {
	    if ( call_struct.veh_attr[i] == YES )
	      {
		if ( fleet[ find_findex(call_struct.fleet) ]->vd_attr_info[i].veh_attr[0] != '\0' )
		  {
		    mvaddstr( AttrRow+1, AttrCol, fleet[ find_findex(call_struct.fleet) ]->vd_attr_info[i].veh_attr );
		    AttrCol += 3;
		    if ( AttrCol >= B_CAR_ATTR8_COL + 2 )
		      break;
		    AttributeSet = YES;
		  }
	      }
	    if ( call_struct.drv_attr[i] == YES )
	      {
		if ( fleet[ find_findex(call_struct.fleet) ]->vd_attr_info[i].drv_attr[0] != '\0' )
		  {
		    mvaddstr( AttrRow+1, AttrCol, fleet[ find_findex(call_struct.fleet) ]->vd_attr_info[i].drv_attr );
		    AttrCol += 3;
		    if ( AttrCol >= B_DRIVER_ATTR8_COL + 2 )
		      break;
		    AttributeSet = YES;
		  }
	      }            
	  }

	
	if ( cp->nbr_of_vehs )
		mvprintw(B_CAR_NO_ROW,B_CAR_NO_COL,"%c", cp->nbr_of_vehs);

	mvprintw(B_PRIORITY_ROW,B_PRIORITY_COL,"%d",cp->priority);

	if ( cp->call_rate )
		mvprintw(B_RATE_ROW,B_RATE_COL,"%7.0f",cp->call_rate);

	if ( cp->vat )
	        mvprintw(B_VAT_ROW,B_VAT_COL,"%5.2f",cp->vat);
	
	if ( cp->hold_line[0] )
		mvaddstr(B_HOLDLINE_ROW,B_HOLDLINE_COL,cp->hold_line);

	if ( cp->local )
		mvaddch(B_LOCAL_ROW,B_LOCAL_COL,cp->local);

	if ( cp->advise_arrival )
		mvaddch(B_ADVISE_ARRIVAL_ROW,B_ADVISE_ARRIVAL_COL,cp->advise_arrival);

#ifdef NAMERICA
	if ( cp->send_destination )
	  mvaddch(E_SEND_DEST_ADDR_ROW, E_SEND_DEST_ADDR_COL, cp->send_destination);

	if ( cp->send_rate )
	  mvaddch(E_SEND_RATE_ROW, E_SEND_RATE_COL, cp->send_rate);		
#endif


	if ( scr_name == SUBS_BASIC_SCR ) 
		/* display the info in the subscription lines also */
		prt_subs_fields();

	attrset(0); /* rest attrset to normal */


}  /* end prt_bold_fields() */


char *
short_to_time(s)
short s;
{
    short hours, minutes;
    static char the_time[7];

 
    if ( s == 999 )
      {
	sprintf( the_time, "99:99" );
	return( the_time );
      }
    
    /* Return empty string if out of range (2359 is 23:59) */
    if (s <= 0 || s > 2359)
	return("");

    hours = s / 100;
    minutes = s - hours * 100;

#ifdef HR24
    sprintf(the_time, "%02d:%02d", hours, minutes);
#else
    if (hours >= 12)
	sprintf(the_time, "%02d:%02dp", hours - 12, minutes);
    else
	sprintf(the_time, "%02d:%02da", hours, minutes);
#endif

    return(the_time);
}

char *
int_to_date(i)
int i;
{
    char in_date[9];
    static char out_date[9];

    /* Simple error check to make sure
       i is has not too many chars. */
    if (i <= 0 || i > 9999999)
	return("");

    if ( i > 1000000 )
      i -= 1000000;

    sprintf(in_date, "%06d", i);

    out_date[0] = in_date[0];
    out_date[1] = in_date[1];
    out_date[2] = DATE_SEPERATOR;
    out_date[3] = in_date[2];
    out_date[4] = in_date[3];
    out_date[5] = DATE_SEPERATOR;
    out_date[6] = in_date[4];
    out_date[7] = in_date[5];
    out_date[8] = '\0';

    return(out_date);
}


/* prt_subs_fields: prints the info in the subscription lines, on
 * previous screen or toggle key entered
 */
prt_subs_fields()
{
    attrset(A_UNDERLINE);

    mvaddstr(SUBS_ROW2, SUBS_MON_COL, short_to_time(subs_struct.mon_time));
    mvaddstr(SUBS_ROW3, SUBS_MON_COL, short_to_time(subs_struct.mon_time_change));
    mvaddstr(SUBS_ROW2, SUBS_TUE_COL, short_to_time(subs_struct.tue_time));
    mvaddstr(SUBS_ROW3, SUBS_TUE_COL, short_to_time(subs_struct.tue_time_change));
    mvaddstr(SUBS_ROW2, SUBS_WED_COL, short_to_time(subs_struct.wed_time));
    mvaddstr(SUBS_ROW3, SUBS_WED_COL, short_to_time(subs_struct.wed_time_change));
    mvaddstr(SUBS_ROW2, SUBS_THUR_COL, short_to_time(subs_struct.thu_time));
    mvaddstr(SUBS_ROW3, SUBS_THUR_COL, short_to_time(subs_struct.thu_time_change));
    mvaddstr(SUBS_ROW2, SUBS_FRI_COL, short_to_time(subs_struct.fri_time));
    mvaddstr(SUBS_ROW3, SUBS_FRI_COL, short_to_time(subs_struct.fri_time_change));
    mvaddstr(SUBS_ROW2, SUBS_SAT_COL, short_to_time(subs_struct.sat_time));
    mvaddstr(SUBS_ROW3, SUBS_SAT_COL, short_to_time(subs_struct.sat_time_change));
    mvaddstr(SUBS_ROW2, SUBS_SUN_COL, short_to_time(subs_struct.sun_time));
    mvaddstr(SUBS_ROW3, SUBS_SUN_COL, short_to_time(subs_struct.sun_time_change));
    mvaddstr(SUBS_ROW2, SUBS_BEGINDATE_COL, int_to_date(subs_struct.from_date));
    mvaddstr(SUBS_ROW3, SUBS_BEGINDATE_COL, int_to_date(subs_struct.to_date));

    attrset(0);

}  /* end prt_subs_fields() */




/* prt_eold_fields: prints the fields on the extended call entry screen,
 *		for previous screen/toggle function keys entered
 */
prt_eold_fields(cp)
	struct cisam_cl *cp;	/* pointer to call_struct */
{
	
	int i;
	int ftj_call; /* BOOL whether a FTJ-call or not */
	int nbr_call_msg; /* Number of call messages associated to a FTJ-call */
	struct cisam_ac *ac_ptr;
	int p_fl_index;
	int AttrCol = 0, AttrRow = 0;	




	/* print the fields at appropriate positions on a call
	 * entry screen
	 */
	attrset(A_UNDERLINE);
	mvaddch(E_FLEET_ROW,E_FLEET_COL,cp->fleet);
        

        if ( ( doing_detail ) && (0) )
          {
            if ( cp->cash_account != YES )
              mvaddch(E_CASH_ACCT_ROW - 1,E_CASH_ACCT_COL,NO);
            else
              mvaddch(E_CASH_ACCT_ROW - 1,E_CASH_ACCT_COL,cp->cash_account);              
          }
        else if (0)
          {
            if ( cp->cash_account != YES )
              mvaddch(E_CASH_ACCT_ROW,E_CASH_ACCT_COL, NO);
            else
              mvaddch(E_CASH_ACCT_ROW,E_CASH_ACCT_COL,cp->cash_account);
          }
        

                
	mvprintw(E_CL_TYPE_ROW,E_CL_TYPE_COL,"%c%c",cp->extended_type[0],
		cp->extended_type[1]);

	mvaddstr(E_INV_ROW, E_INV1_COL, cp->invoice_nbr1);
	mvaddstr(E_INV_ROW, E_INV2_COL, cp->invoice_nbr2);
	mvaddstr(E_INV_ROW, E_INV3_COL, cp->invoice_nbr3);
	mvaddstr(E_INV_ROW, E_INV4_COL, cp->invoice_nbr4);

	if ((0) && (cp->call_msg_nbr > '0'))
		mvprintw(E_SP_CL_MSG_ROW, E_SP_CL_MSG_COL,"%c",cp->call_msg_nbr);

	if ( cp->pckup_str_nbr ) {
		if ( cp->pckup_str_suf && !(0) )
			mvprintw(E_PKUP_ADDRESS_ROW,E_PKUP_ST_NO_COL,
				"%d%c",cp->pckup_str_nbr,cp->pckup_str_suf);
		else
			mvprintw(E_PKUP_ADDRESS_ROW,E_PKUP_ST_NO_COL,
				"%d",cp->pckup_str_nbr);
	}  /* end if */

#ifdef NAMERICA
	if ( cp->pckup_pre_dir[0] )
		mvaddstr(E_PKUP_ADDRESS_ROW,E_PKUP_PRE_DIR_COL,
			cp->pckup_pre_dir);
#endif

	if ( cp->pckup_str_name[0] )	 {
		if ( cp->intr_str_name[0] ) 
			mvprintw(E_PKUP_ADDRESS_ROW,E_PKUP_ST_NAME_COL,"%s/",cp->pckup_str_name);
		else 
			mvaddstr(E_PKUP_ADDRESS_ROW,E_PKUP_ST_NAME_COL,cp->pckup_str_name);
	}
	
	if(cp->charge_nbr[0])
		mvaddstr(E_CHARGE_ROW,E_CHARGE_COL,
			cp->charge_nbr);

	if(cp->company_name[0])
		mvaddstr(E_CONAME1_ROW,E_CONAME_COL,
			cp->company_name);

	if(cp->ac_addt1_info1[0])
		mvaddstr(E_CONAME2_ROW,E_CONAME_COL,
			cp->ac_addt1_info1);

	if(cp->ac_addt1_info2[0])
		mvaddstr(E_CONAME3_ROW,E_CONAME_COL,
			cp->ac_addt1_info2);

#ifdef NAMERICA
	if ( cp->pckup_str_type[0] )
		mvaddstr(E_PKUP_ADDRESS_ROW,E_PKUP_ST_TYPE_COL,
			cp->pckup_str_type);
#endif

	if ( cp->requested_by[0] )
		mvaddstr(E_REQBY_ROW,E_REQBY_COL,
			cp->requested_by);

	if ( cp->person_to_see[0])
		mvaddstr(E_SEE_ROW,E_SEE_COL,
			cp->person_to_see);

	if ( cp->pod_required)
		mvaddch(E_POD_ROW,E_POD1_COL,
			cp->pod_required);

	if ( cp->pod_name[0])
		mvaddstr(E_POD_ROW,E_POD2_COL,
			cp->pod_name);

#ifdef NAMERICA
	if ( cp->pckup_post_dir[0] )
		mvaddstr(E_PKUP_ADDRESS_ROW,E_PKUP_POST_DIR_COL,
			cp->pckup_post_dir);
#endif

	if ( cp->intr_str_name[0] ) {
		inters_flds(C_PRINT,extended_scr_fld);
		attrset(A_UNDERLINE);
		mvaddstr(E_INTERS_ADDRESS_ROW,E_INTERS_ST_NAME_COL,cp->intr_str_name);
		attrset(0);
	}

#ifdef NAMERICA
	if ( cp->intr_pre_dir[0] )
		mvaddstr(E_INTERS_ADDRESS_ROW,E_INTERS_PRE_DIR_COL,
			cp->intr_pre_dir);

	if ( cp->intr_str_type[0] )
		mvaddstr(E_INTERS_ADDRESS_ROW,E_INTERS_ST_TYPE_COL,
			cp->intr_str_type);
	
	if ( cp->intr_post_dir[0] )
		mvaddstr(E_INTERS_ADDRESS_ROW,E_INTERS_POST_DIR_COL,
			cp->intr_post_dir);
#endif

	if ( cp->pckup_apt_room[0] )
		mvaddstr(E_PKUP_APT_ROW,E_PKUP_APT_COL,
			cp->pckup_apt_room);

	/* print the pkup address comment */
	attrset(0);
	if ( cp->pckup_adr_cmnt[0] )
		mvprintw(E_PKUP_ADDR_CMNT_ROW,E_PKUP_ADDR_CMNT_COL,
			 "%-*s", CHARS_ADDR_CMNT, cp->pckup_adr_cmnt);
	if ( cp->ct_adr_cmnt[0] )
	  mvaddstr( E_CT_CMNT_ROW, E_CT_CMNT_COL,
		   cp->ct_adr_cmnt );
	
	attrset(A_UNDERLINE);

	if ( cp->pckup_city[0] )
		mvaddstr(E_PKUP_CITY_ROW,E_PKUP_CITY_COL,
			cp->pckup_city);

	if ( cp->pckup_zone )  
		mvprintw(E_PKUP_ZONE_ROW,E_PKUP_ZONE_COL, "%d",
			cp->pckup_zone);

	/* print the pkup zone comment */
	if ( cp->zone_desc[0] ) {
		attrset(0);
		mvprintw(E_PKUP_ZONE_ROW,E_PKUP_ZONE_COL+6,
			 "%-*s", CHARS_ZONE_DESC, cp->zone_desc);
		attrset(A_UNDERLINE);
	}

	/* Subscription doesn't store zone description */
	else if (scr_name == SUBS_EXTENDED_SCR)
	  {
	    get_zone_desc(C_PRINT, PICKUP_ADDR, cp, NULL);
	    attrset(A_UNDERLINE);
	  }

	/* print the estimated wait time */
	if ( !doing_detail && cp->est_wait_time[0] ) {
		attrset(0);
		mvprintw(LINES-2,0,"%s %s",cp->est_wait_time, catgets(UI_m_catd, UI_1, UI_UI_PRT_DEF_3, "MINUTES") );
		attrset(A_UNDERLINE);
	}  /* end if */

	if ( cp->passenger_name[0] )
		mvaddstr(E_PASSENGER_ROW,E_PASSENGER_COL,cp->passenger_name);

#ifdef SCANDINAVIA
	if ( cp->phone_number[0] )
		mvaddstr(E_PHONE_ROW,E_PH_AREA_CODE_COL,cp->phone_number);
#else	
	if ( cp->phone_area[0] )
		mvaddstr(E_PHONE_ROW,E_PH_AREA_CODE_COL,cp->phone_area);
	
	if ( cp->phone_nbr[0] ) {
		mvaddstr(E_PHONE_ROW,E_PH_NUM1_COL,cp->phone_nbr);
		mvprintw(E_PHONE_ROW,E_PH_NUM2_COL-1,"-%s",&cp->phone_nbr[3]);
		/* print phone ext if any */
		if ( cp->phone_ext[0] ) 
			mvaddstr(E_PHONE_ROW,E_PH_EXT_COL,cp->phone_ext);
	}
#endif

	if (scr_name != SUBS_EXTENDED_SCR) {
	    mvaddstr(E_PKUP_DATE_ROW,E_PKUP_DATE_COL,cp->due_date);
	    mvaddstr(E_PKUP_TIME_ROW,E_PKUP_TIME_COL,cp->due_time);
	}
	else { /* SUBS_EXTENDED_SCR */
	    attrset(0);
	    mvaddstr(E_PKUP_DATE_ROW, E_PKUP_DATE_COL, cp->date_in);
	    mvprintw(E_PKUP_TIME_ROW, B_ENTERED_BY_COL, "%d", cp->entered_by);
	    attrset(A_UNDERLINE);
	}

	if ( cp->general_cmnt[0] )
		mvaddstr(E_COMMENT_ROW,E_COMMENT_COL,cp->general_cmnt);

	/* print the destination address info */
	if ( cp->dest_str_nbr ) {
		if ( cp->dest_str_suf )
			mvprintw(E_DEST_ADDRESS_ROW,E_DEST_ST_NO_COL,
				"%d%c",cp->dest_str_nbr,cp->dest_str_suf);
		else
			mvprintw(E_DEST_ADDRESS_ROW,E_DEST_ST_NO_COL,
				"%d",cp->dest_str_nbr);
	}  /* end if */

#ifdef NAMERICA
	if ( cp->dest_pre_dir[0] )
		mvaddstr(E_DEST_ADDRESS_ROW,E_DEST_PRE_DIR_COL,
			cp->dest_pre_dir);
#endif

	if ( cp->dest_str_name[0] )
		mvaddstr(E_DEST_ADDRESS_ROW,E_DEST_ST_NAME_COL,
			cp->dest_str_name);

#ifdef NAMERICA
	if ( cp->dest_str_type[0] )
		mvaddstr(E_DEST_ADDRESS_ROW,E_DEST_ST_TYPE_COL,
			cp->dest_str_type);

	if ( cp->dest_post_dir[0] )
		mvaddstr(E_DEST_ADDRESS_ROW,E_DEST_POST_DIR_COL,
			cp->dest_post_dir);
#endif

	/* print the dest address comment */
	attrset(0);
	if ( cp->dest_adr_cmnt[0] )
		mvprintw(E_DEST_ADDR_CMNT_ROW,E_DEST_ADDR_CMNT_COL,
			 "%*-s", CHARS_ADDR_CMNT, cp->dest_adr_cmnt);
	attrset(A_UNDERLINE);

	if ( cp->dest_apt_room[0] )
		mvaddstr(E_DEST_APT_ROW,E_DEST_APT_COL,
			cp->dest_apt_room);

	if ( cp->dest_city[0] )
		mvaddstr(E_DEST_CITY_ROW,E_DEST_CITY_COL,
			cp->dest_city);

	if ( cp->dest_zone ) {
		mvprintw(E_DEST_ZONE_ROW,E_DEST_ZONE_COL, "%d",
			cp->dest_zone);
		/* get the dest zone description */
		get_zone_desc(C_PRINT, DEST_ADDR, cp, NULL);
		attrset(A_UNDERLINE);
	}  /* end if */

	if ( cp->personal_veh ) {
		mvprintw(E_PERSONEL_ROW,E_PERSONEL1_COL,"%d",
			cp->personal_veh);
	}  /* end if */

	p_fl_index = find_findex( cp->fleet );		      
	AttrCol = E_CAR_ATTR1_COL;
	AttrRow = E_CAR_ROW;

        attrset(0);
	for ( i = 0; i < ATTR_MAX; i++ )
	  {
	    if ( cp->veh_attr[i] == YES )
	      {
		if ( fleet[ p_fl_index ]->vd_attr_info[i].veh_attr[0] != '\0' )
		{
		  mvaddstr( AttrRow+1, AttrCol, fleet[ p_fl_index ]->vd_attr_info[i].veh_attr );
		  AttrCol += 3;
		  if ( AttrCol >= E_CAR_ATTR8_COL + 2 )
		    break;
		}
	      }
            
	    if ( cp->drv_attr[i] == YES )
	      {
		if ( fleet[ p_fl_index ]->vd_attr_info[i].drv_attr[0] != '\0' )
		  {
		    mvaddstr( AttrRow+1, AttrCol, fleet[ p_fl_index ]->vd_attr_info[i].drv_attr );
		    AttrCol += 3;
		    if ( AttrCol >= E_DRIVER_ATTR8_COL + 2 )
		      break;
		  }
	      }            
            
	  }
	    
	attrset(A_UNDERLINE);	

	if ( cp->nbr_of_vehs )
		mvprintw(E_CAR_NO_ROW,E_CAR_NO_COL,"%c", cp->nbr_of_vehs);

	mvprintw(E_PRIORITY_ROW,E_PRIORITY_COL,"%d",cp->priority);

	if ( cp->call_rate )
		mvprintw(E_RATE_ROW,E_RATE_COL,"%7.0f",cp->call_rate);
	if ( cp->vat )
	        mvprintw(E_VAT_ROW, E_VAT_COL, "%5.2f",cp->vat);

	if ( cp->hold_line[0] )
		mvaddstr(E_HOLDLINE_ROW,E_HOLDLINE_COL,cp->hold_line);

	if ( cp->local )
		mvaddch(E_LOCAL_ROW,E_LOCAL_COL,cp->local);

	if ( cp->advise_arrival )
		mvaddch(E_ADVISE_ARRIVAL_ROW,E_ADVISE_ARRIVAL_COL,cp->advise_arrival);

	mvprintw(E_VIA_ADDRESS_ROW, E_VIA_ADDRESS_COL, "%1d", cp->via_addresses );

#ifdef NAMERICA
	if ( cp->send_destination )
	  mvaddch(E_SEND_DEST_ADDR_ROW, E_SEND_DEST_ADDR_COL, cp->send_destination);
        if ( cp->send_rate )
          mvaddch(E_SEND_RATE_ROW, E_SEND_RATE_COL, cp->send_rate);
#endif

	if (doing_detail || scr_name == INDIVIDUAL_CALLS_SCR)
	  {
	    attrset(0);
	    mvaddstr(E_FLEET_ROW - 1, E_CAR_COL, "          ");
	    if (scr_name == SUBS_EXTENDED_SCR)
	      mvprintw(E_FLEET_ROW - 1, E_CAR_COL, "%d", subs_struct.nbr);
	    else if (cp->nbr)
	      mvprintw(E_FLEET_ROW - 1, E_CAR_COL, "%d",cp->nbr);
	  }

	if ( strlen( cp->charge_nbr ) )
	    {
	      ac_ptr = (struct cisam_ac *)req_buf;
	      if ( valid_charge_nbr(ac_ptr, cp) )
		{
		  mvprintw(E_CHARGE_ROW+1,3,"%-30s  %-15s",
			   ac_ptr->header_1,ac_ptr->header_3);
		  mvprintw(E_CHARGE_ROW+2,3,"%-30s  %-15s",
			   ac_ptr->header_2,ac_ptr->header_4);
		}
	    }
	  

	if ( scr_name == SUBS_EXTENDED_SCR ) 
		/* display the info in the subscription lines also */
		prt_subs_fields();

	attrset(0);	/* set attrsibutes back to normal mode */

}  /* end prt_eold_fields() */


/* prt_wold_fields: prints the dat in the fields on a wakeup call entry 
 * 	screen, whne the  previous screen key is pressed
 */
prt_wold_fields(cp)
	struct cisam_cl *cp;	/* pointer to call_struct */
{
	
	int i;

	/* print the fields at appropriate positions on a call
	 * entry screen
	 */
	attrset(A_UNDERLINE);
	mvaddch(W_FLEET_ROW,B_FLEET_COL,cp->fleet);

	if ( cp->passenger_name[0] )
		mvaddstr(W_NAME_ROW,B_PASSENGER_COL,cp->passenger_name);

#ifdef SCANDINAVIA
	if ( cp->phone_number[0] )
		mvaddstr(W_PHONE_ROW,W_PH_AREA_CODE_COL,cp->phone_number);
#else	
	if ( cp->phone_area[0] )
		mvaddstr(W_PHONE_ROW,W_PH_AREA_CODE_COL,cp->phone_area);
	
	if ( cp->phone_nbr[0] ) {
		mvaddstr(W_PHONE_ROW, B_PH_NUM1_COL, cp->phone_nbr);
		mvprintw(W_PHONE_ROW, B_PH_NUM2_COL-1, "-%s", &cp->phone_nbr[3]);
		/* print phone ext if any */
		if ( cp->phone_ext[0] ) 
			mvaddstr(W_PHONE_ROW,W_PH_AREA_CODE_COL+17,cp->phone_ext);
	}
#endif

	if (scr_name == WAKEUP_CALL_SCR)
	  {
	    mvaddstr(W_PKUP_DATE_ROW, B_PKUP_DATE_COL,cp->due_date);
	    mvaddstr(W_PKUP_DATE_ROW, B_PKUP_TIME_COL,cp->due_time);
	  }
	else /* SUBS_WAKEUP_SCR */
	  {
	    attrset(0);
	    mvaddstr(W_PKUP_DATE_ROW, B_PKUP_DATE_COL,cp->date_in);
	    mvprintw(W_PKUP_DATE_ROW, B_ENTERED_BY_COL, "%d", cp->entered_by);
	    attrset(A_UNDERLINE);
	  }

	if ( cp->general_cmnt[0] )
		mvaddstr(W_COMMENT_ROW, B_COMMENT_COL,cp->general_cmnt);

	if (doing_detail || scr_name == INDIVIDUAL_CALLS_SCR)
	  {
	    attrset(0);
	    mvaddstr(E_FLEET_ROW, E_CAR_COL, "          ");
	    if (scr_name == SUBS_WAKEUP_SCR)
	      mvprintw(E_FLEET_ROW, E_CAR_COL, "%d", subs_struct.nbr);
	    else if (cp->nbr)
	      mvprintw(E_FLEET_ROW, E_CAR_COL, "%d",cp->nbr);
	  }

	if ( scr_name == SUBS_WAKEUP_SCR ) 
		/* display the info in the subscription lines also */
		prt_subs_fields();

}  /* end prt_wold_fields() */
