static char sz__FILE__[]="@(#)da_subcall.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  da_subcall.c; Rel: 10.1.0.0; Get date: 4/18/95 at 17:00:51
 * @(#)  Last delta: 12/2/94 at 11:38:50
 * @(#)  SCCS File: /taxi/sccs/s.da_subcall.c
 *                                                                           *
 * Copyright (c) 1990 - MDI Seattle                                          *
 *                                                                           *
 *****************************************************************************/
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <memory.h>
#include <fcntl.h>
#include <isam.h>
#include <sys/types.h>
#include <sys/ipc.h>

#define DECLARING
#include "comm_strdefs.h"
#include "rep_strdefs.h"
#include "writer_strdefs.h"
#include "mads_types.h"
#include "key.h"
#include "mad_ipc.h"
#include "ui_ipc.h"
#include "mad_error.h"
#include "calls.h"
#include "except.h"
#include "writer.h"
#include "literals.h"
#include "language.h"
#include "path.h"
#include "enhance.h"
#include "switch_ext.h"
#include "ex_key.h"

/* define the structures to attach memory */

struct	offsets *offset;
extern struct	offsets *get_offsets();
extern long conversion();

long glEnhSwitch; /* Enhancement switch for enhancement checking */

extern	char buf[];					/* for db put and get records       */
extern	char req_buf[];					/* buffer to cast as call structure */
extern  char tmp_req_buf[];
extern	int db();

int pipe_fd;						/* File descriptor for pipe */
int pipe_ok;						/* 0=could not open pipe; 1=pipe opened ok */
int pickup_date_time;
long last_isrecnum;					/* c-isam number of last call added to call file */

struct	cisam_cl *callrec;				 /* define a call record pointer */
struct	cisam_hl *hlrec;				 /* define a holiday record pointer */
struct	cisam_sc *subcallrec;				 /* define a subscription call record pointer */
struct cisam_ch *ch_ptr;				/* for call history file */
struct call_ipc_rec cpipe_rec;				/* Record to be sent to dispatch */
char subscript_buff[sizeof(struct cisam_sc)];		 /* room to store the subscription call */

#define secondsperday	86400

char sysbuf[120];
int its_a_holiday=0;			/* 0=not a holiday, 1=call due date falls on a holiday */
int its_a_mult = 0;

/*====================================================================
= da_subcall()
=	Each day subscription calls from the subscription table are put in the call table.
=====================================================================*/

main(argc, argv)
int argc;
char *argv[];
{
	register int ccode;			/* returned from proc calls */
	register int call_index;
	int date_counter;
	int run_date;
	int default_to_date;
	int stack[3];				/* two subscription calls and a zero stack bottom */
	int valid_day;				/* store previously scheduled call */
	long date_time_now = time((time_t *)0);
	long pickup_date;
	long temp_date;
	char from_date[10], tmp_from_date[10], to_date[10];	/* yymmdd formats for account valid date ranges */
        char   c;
	char buff[81];
	struct tm *date_value;
	int  from_date_int, to_date_int;
	char fleet_sav;
/*
        struct tm *current_time;
        long   calendar_time;
        static char *day_of_week[]={"sun","mon","tue","wed","thu","fri","sat"};
        char   tomorrow[4];
*/ 
	int test_date;
        /* init language environment using manager's profile and open catalogs */
        init_mgr_locale();
        COMMON_m_catd = init_catalog("common");
        WRITER_catd = init_catalog("writer");

	offset = get_offsets();			/* attach shared memory and print redundant record transactions */

	callrec = (struct cisam_cl *)req_buf;				/* cast the buffers as the request buffer */
	hlrec = (struct cisam_hl *)req_buf;
	subcallrec = (struct cisam_sc *)subscript_buff;

	if (enh(&glEnhSwitch) < 0)
	  {
	    /* Error logging */
	    exit( -1 );
	  }
	
	pipe_ok = 1;							/* assume pipe opened OK */
	if((pipe_fd = open(USER_PIPE, O_RDWR | O_NDELAY)) < 0) {	/* Open pipe to DISPATCH for read/write */
		ERROR(' ',"","Error opening user pipe");
		pipe_ok = 0;						
	}

	if (db(CALL_FILE_ID, OPEN, &cl_key3, ISINOUT | ISMANULOCK, 0) < 0) {		/* OPEN call file */
		ERROR(' ', CALL_FILE, "Error OPENing call file")
		exit(-1);
	}

	if (db(CALLH_FILE_ID, OPEN, &ch_key1, ISINOUT | ISMANULOCK, 0) < 0) {		/* OPEN call history file */
		ERROR(' ', CALLH_FILE, "Error OPENing call history file")
		exit(-1);
	}

	if (db(SUBCALL_FILE_ID, OPEN, &sc_key1, ISINOUT | ISMANULOCK, 0) < 0) {		/* OPEN subscription call file */
		ERROR(' ', SUBCALL_FILE, "Error OPENing subcall file")
		exit(-1);
	}

	if (db(ZONE_FILE_ID, OPEN, &zn_key1, ISINPUT | ISMANULOCK, 0) < 0) {		/* OPEN zone file */
		ERROR(' ', ZONE_FILE, "Error OPENing zone file")
		exit(-1);
	}

	if (db(HOLIDAY_FILE_ID, OPEN, &hl_key1, ISINPUT | ISMANULOCK, 0) < 0) {		/* OPEN holiday file */
		ERROR(' ', HOLIDAY_FILE, "Error OPENing holiday file")
		exit(-1);
	}

	if (db(STRNAME_FILE_ID, OPEN, &sn_key2, ISINPUT | ISMANULOCK, 0) < 0) {		/* OPEN holiday file */
		ERROR(' ', STRNAME_FILE, "Error OPENing strname file")
		exit(-1);
	}

	if (db(ADDRESS_FILE_ID, OPEN, &ad_key1, ISINPUT | ISMANULOCK, 0) < 0) {		/* OPEN holiday file */
		ERROR(' ', ADDRESS_FILE, "Error OPENing address file")
		exit(-1);
	}

	if (db(CALLMSG_FILE_ID, OPEN, &cmsg_key1, ISINOUT | ISMANULOCK, 0) < 0){
	}

	if (db(M_ACTION_FILE_ID, OPEN, &m_action_key1, ISINPUT | ISMANULOCK, 0) < 0){
	}

/**
        calendar_time = time(NULL);
        current_time = localtime(&calendar_time);
        if (current_time->tm_wday == 6)
           strcpy(tomorrow, day_of_week[0]);
        else
           strcpy(tomorrow, day_of_week[current_time->tm_wday + 1]);

  	setuid(0);	/* at job permision *
	sprintf(sysbuf,"at -r `grep -l da_subcall %s |grep -v sh|grep -v grep| sed 's/.*\///'` 2>/dev/null",ATJOBS);
        system(sysbuf);
	sprintf(sysbuf,"echo \"%s \" | at 23:56 %s 2>/dev/null",SUBCALLS,tomorrow);
        system(sysbuf);
 **/

	date_value = localtime(&date_time_now);						/* initialize date and time variables */
	date_value->tm_hour = date_value->tm_min = date_value->tm_sec = 0;		/* the time structure */

								/* subscription call must be between from_date and to_date */
	run_date = 10000 * date_value->tm_year + 100 * (date_value->tm_mon + 1) + date_value->tm_mday;

	temp_date = date_time_now + (2 * secondsperday);	/* default to_date sould be two days from now */
	date_value = localtime(&temp_date);						/* initialize date and time variables */
	date_value->tm_hour = date_value->tm_min = date_value->tm_sec = 0;		/* the time structure */

								/* set up default to_date */
	default_to_date = 10000 * date_value->tm_year + 100 * (date_value->tm_mon + 1) + date_value->tm_mday;

	subcallrec->nbr = 0; 								/* Look at all subscription calls */
	ccode=db(SUBCALL_FILE_ID, READ_KEYS, &sc_key1, ISGTEQ+ISLOCK, 0);		/* key1 = call number */
	while (ccode >= 0) {								/* for all records in subscription file */
		memcpy(subscript_buff,  req_buf,  sizeof(struct cisam_sc));		/* save the subscription record in memory */
		valid_day = -1;
		stack[0] = 0;						/* manage a two call stack within the subscription record */
		stack[1] = subcallrec->call_nbr1;
		stack[2] = subcallrec->call_nbr2;
		call_index = 3;								/* check call2 then call1 */
		while (--call_index) 							/* scanning the stack */
			if (stack[call_index]) {					/* look for this call record */
				callrec->nbr=stack[call_index];
				ccode = db(CALL_FILE_ID, READ_KEYS, &cl_key1, ISEQUAL, 0);	/*read call from call number stack*/
				if (ccode < 0 || callrec->due_date_time<date_time_now) {	/* not in system or dispatched */
					stack[call_index] = 0;	
					if (call_index == 1) {					/* pull down the stack */
						stack[1] = stack[2];
						stack[2] = 0;
					}
				}
				else {		/* if there are two valid calls scheduled,  then this variable is irrevelant */
							/* if no valid calls are scheduled,  then this variable remains zero */
							/* thus,  only the next unique scheduled call is important to store */
					date_value = localtime( &callrec->due_date_time);
					valid_day = date_value->tm_wday;
				}
			}

		if (DATE_FORMAT == YEAR_FIRST_IN_DATE)
		{
			sprintf(from_date, "%06d",  subcallrec->from_date);
			sprintf(to_date, "%06d",  subcallrec->to_date);
		} else
		{
			sprintf(from_date, "%07d",  subcallrec->from_date);
			sprintf(to_date, "%07d",  subcallrec->to_date);

		/* convert to yymmdd formats for account date ranges comparisons */
			if ( from_date[5] != '9' )
			  tmp_from_date[0] = '1';
			else
			  tmp_from_date[0] = '0';
			
			tmp_from_date[1] = from_date[5];
			tmp_from_date[2] = from_date[6];
			tmp_from_date[3] = from_date[3];
			tmp_from_date[4] = from_date[4];
			tmp_from_date[5] = from_date[1];
			tmp_from_date[6] = from_date[2];
			tmp_from_date[7] = '\0';
			strcpy( from_date, tmp_from_date );

			if ( to_date[5] != '9' )
			  tmp_from_date[0] = '1';
			else
			  tmp_from_date[0] = '0';

			tmp_from_date[1] = to_date[5];
			tmp_from_date[2] = to_date[6];
			tmp_from_date[3] = to_date[3];
			tmp_from_date[4] = to_date[4];
			tmp_from_date[5] = to_date[1];
			tmp_from_date[6] = to_date[2];
			tmp_from_date[7] = '\0';
			strcpy( to_date, tmp_from_date );
			
		}

/* no entry in to_date field means the account is always valid */
		if (!subcallrec->to_date || subcallrec->to_date<0) sprintf(to_date, "%06d",  default_to_date);
		from_date_int = atoi(from_date);
		to_date_int = atoi(to_date);

#ifdef FOO
		if ( from_date[0] != '9' ) /* next century */
		  from_date_int += 1000000;
		if ( to_date[0] != '9' )
		  to_date_int += 1000000;
#endif

		if (from_date_int <= run_date && run_date <= to_date_int ) { 
							/* Has a valid account date.  Schedule subscription calls two days ahead */
			call_index = 1;							/* check call1 then call2 */
			date_counter = 0;	
			while (++date_counter < 3 ) {					/* schedule two days ahead */
				if (stack[1])
					 call_index=2;					/* use the next stack location */
				if (!stack[call_index]) {				/* schedule a subscription call */
					pickup_date = date_time_now + (date_counter*secondsperday);	/* get next day */
					date_value = localtime( &pickup_date);				/* the time structure */
					if (valid_day == date_value->tm_wday) {				/* use the next day */
						pickup_date += secondsperday;
						date_value = localtime( &pickup_date);
					}
					test_date = 10000*date_value->tm_year + 100 * (date_value->tm_mon + 1) + date_value->tm_mday;
					if(test_date > to_date_int)
						continue;
					date_value->tm_hour = date_value->tm_min= date_value->tm_sec=0;
					pickup_date_time = 0;
					switch (date_value->tm_wday) {	
						case 7:							/* sunday */
						case 0:							/* sunday */
							pickup_date_time=(subcallrec->sun_time_change>0) ? subcallrec->sun_time_change: subcallrec->sun_time;
							if ( ( date_counter == 1 ) && ( subcallrec->sun_time_change == 999 ) )
							  subcallrec->sun_time_change=0;
							if ( subcallrec->sun_time_change != 999 )
							  subcallrec->sun_time_change = 0;
							break;

						case 1:							/* monday */
							pickup_date_time=(subcallrec->mon_time_change>0) ? subcallrec->mon_time_change: subcallrec->mon_time;
							if ( ( date_counter == 1 ) && ( subcallrec->mon_time_change == 999 ) )			
							  subcallrec->mon_time_change=0;
							if ( subcallrec->mon_time_change != 999 )
							  subcallrec->mon_time_change = 0;							
							break;

						case 2:							/* tuesday */
							pickup_date_time=(subcallrec->tue_time_change>0) ? subcallrec->tue_time_change: subcallrec->tue_time;
							if ( ( date_counter == 1 ) && ( subcallrec->tue_time_change == 999 ) )
							  subcallrec->tue_time_change=0;
							if ( subcallrec->tue_time_change != 999 )
							  subcallrec->tue_time_change = 0;						   	
							break;

						case 3:							/* wednesday */
							pickup_date_time=(subcallrec->wed_time_change>0) ? subcallrec->wed_time_change: subcallrec->wed_time;
							if ( ( date_counter == 1 ) && ( subcallrec->wed_time_change == 999 ) )
							  subcallrec->wed_time_change=0;
							if ( subcallrec->wed_time_change != 999 )
							  subcallrec->wed_time_change = 0;
							break;

						case 4:							/* thursday */
							pickup_date_time=(subcallrec->thu_time_change>0) ? subcallrec->thu_time_change: subcallrec->thu_time;
							if ( ( date_counter == 1 ) && ( subcallrec->thu_time_change == 999 ) )
							  subcallrec->thu_time_change=0;
							if ( subcallrec->thu_time_change != 999 )
							  subcallrec->thu_time_change = 0;							
							break;

						case 5:							/* friday */
							pickup_date_time=(subcallrec->fri_time_change>0) ? subcallrec->fri_time_change: subcallrec->fri_time;
							if ( ( date_counter == 1 ) && ( subcallrec->fri_time_change == 999 ) )
							  subcallrec->fri_time_change=0;
							if ( subcallrec->fri_time_change != 999 )
							  subcallrec->fri_time_change = 0;
							break;

						case 6:							/* saturday */
							pickup_date_time=(subcallrec->sat_time_change>0) ? subcallrec->sat_time_change: subcallrec->sat_time;
							if ( ( date_counter == 1 ) && ( subcallrec->sat_time_change == 999 ) )
							  subcallrec->sat_time_change=0;
							if ( subcallrec->sat_time_change != 999 )
							  subcallrec->sat_time_change = 0;
							break;

						default:				/* not a week day,  skip this record */
							pickup_date_time=0;
					}

					if (pickup_date_time > 0 && pickup_date_time != 999) {				/* has a subscription call to schedule */
									/* determine if this new call is before a scheduled call */
						if (valid_day > date_value->tm_wday) {		/* then call_index must be 2 */
							stack[2]=stack[1];
							call_index=1;
						}

						its_a_holiday = 0;
                                                its_a_mult = 0;
						hlrec->mon = date_value->tm_mon + 1;		/* month of the year */
						hlrec->day = date_value->tm_mday;		/* day of the month */
						hlrec->year = date_value->tm_year % 100;		/* year - 1900 */
						if (db(HOLIDAY_FILE_ID, READ_KEYS, &hl_key1, ISEQUAL, 0) >= 0) {/* check for hol */
							its_a_holiday = 1;
						}
						memset((char *)callrec,'\0',sizeof(struct cisam_cl));	/* clear call space */	
						format_call(date_value);			/* prepare a call for call table */

						
						if (db(CALL_FILE_ID, ADD_REC, &cl_key1, 0, 0) < 0)	/* add call record */
							ERROR(' ', CALL_FILE, "Error adding to call file")
						else {
						  
							stack[call_index] = callrec->nbr;		/* save call number */
							last_isrecnum = (long)isrecnum;			/* save c-isam number */
							send_to_dispatch();
							if ( glEnhSwitch & ENH0002 )
							  {
							    if ( AddCallMsg( callrec ) == SUCCESS )
							      {
								if ( db_read_key(CALL_FILE_ID, callrec, &cl_key1, ISEQUAL) == SUCCESS )
								  {
								    callrec->call_msg_nbr = '1';
								    db_update( CALL_FILE_ID, callrec );
								  }
							      }
							  }
							fleet_sav = callrec->fleet;
                                                        
                                                        if ( callrec->extended_type[4] == MULT_CALL )
                                                          its_a_mult = 1;

							add_call_ent(callrec->nbr,callrec->fleet);

                                                                                                                
							if ( its_a_holiday )
							  add_callh( stack[call_index], fleet_sav );
                                                        if ( its_a_mult )
                                                          add_callh_multi( stack[call_index], fleet_sav );
							if ( cpipe_rec.call_type.wakeup )
							  add_callh_wakeup( stack[call_index], fleet_sav );
						}

					}
				}
			}
			
			subcallrec->call_nbr2=stack[2];		/* restore call number in call subscription 2 element stack */
			subcallrec->call_nbr1=stack[1];
			memcpy(req_buf,  subscript_buff,  sizeof(struct cisam_sc));

			if (db(SUBCALL_FILE_ID, UPDATE_REC, &sc_key1, 0, 0) < 0) 
				ERROR(' ', SUBCALL_FILE, "Error updating call file")

			if (db(SUBCALL_FILE_ID, RELEASE, &sc_key1, 0, 0) < 0) 
				ERROR(' ', SUBCALL_FILE, "Error releasing subcall file")
		}
		ccode=db(SUBCALL_FILE_ID, READ_KEYS, &sc_key1, ISNEXT+ISLOCK, 0);		/* read the next record */
	}

	if (db(SUBCALL_FILE_ID, CLOSE, &sc_key1, 0, 0) < 0) {					/* CLOSE subscription file */
		ERROR(' ', SUBCALL_FILE, "Error closing subcall file")
		exit(-1);
	}

	if (db(CALL_FILE_ID, CLOSE, &cl_key1, 0, 0) < 0) {					/* CLOSE call file */
		ERROR(' ', CALL_FILE, "Error closing call file")
		exit(-1);
	}

	if (db(CALLH_FILE_ID, CLOSE, &ch_key1, 0, 0) < 0) {					/* CLOSE call history file */
		ERROR(' ', CALLH_FILE, "Error closing call history file")
		exit(-1);
	}

	if (db(ZONE_FILE_ID, CLOSE, &zn_key1, 0, 0) < 0) {					/* CLOSE zone file */
		ERROR(' ', ZONE_FILE, "Error closing zone file")
		exit(-1);
	}

	if (db(HOLIDAY_FILE_ID, CLOSE, &hl_key1, 0, 0) < 0) {					/* CLOSE zone file */
		ERROR(' ', HOLIDAY_FILE, "Error closing holiday file")
		exit(-1);
	}

	shmdt(offset);									/* detach shared memory and end */

        /* close language catalogues */
        cleanup_catalog(COMMON_m_catd);
        cleanup_catalog(WRITER_catd);
}
/************************************************************************/
/* add_call_ent: adds a ENTERED call history record			*/
/***********************************************************************/
add_call_ent(call_nbr,fleet_id)
int call_nbr;
char fleet_id;

{
	struct cisam_ch *ch_ptr;
	char date_str[10];			/* for date in display format */
	char time_str[10];			/* for time in display format */

	ch_ptr = (struct cisam_ch *)req_buf;
	memset((char *)ch_ptr,'\0',sizeof(struct cisam_ch));		/* initialize the buffer for reading call history rec */

	ch_ptr->call_nbr = call_nbr;	 	 			/* call # */
	ch_ptr->exception_nbr = 0;
	ch_ptr->fleet = fleet_id;
	ch_ptr->event_type = REGULAR_EVENT;					/* regular exception */
	ch_ptr->status_dt_tm = (int) time(NULL);
	get_pkup_date(date_str); 					/* get the date in display format */
	strcpy(ch_ptr->status_date,date_str);
	get_pkup_time(time_str); 					/* get the time in display format */
	strcpy(ch_ptr->status_time,time_str);
	strcpy(ch_ptr->event_desc,ENTERED);
	ch_ptr->veh_nbr = 0;
	ch_ptr->drv_id = 0;
	ch_ptr->user_id = -1;
									/* let all action fields be NULL's or 0's */
	if (db(CALLH_FILE_ID,ADD_REC,&ch_key1,0,0) < 0) 	 	/* add the record to disk */
		ERROR(fleet_id,CALLH_FILE,"could not ENTERED record");
}

/****************************************************************************************/
/* format_call - format the fields for the call record 					*/
/****************************************************************************************/
format_call(date_value)
struct tm *date_value;
{
	struct cisam_zn 	*zonerec;
	int	temp_hour, temp_minute, i;
	char 	iobuffer[32];

	zonerec = (struct cisam_zn *)req_buf;  				/* point to the request buffer as a zone buffer */
	zonerec->fleet = subcallrec->fleet;				/* get the zone description from the zone file */
	zonerec->nbr = subcallrec->pckup_zone;

	if (db(ZONE_FILE_ID, READ_KEYS, &zn_key1, ISEQUAL, 0) < 0)	/* key1 = fleet,  zone_number*/
		strcpy(iobuffer, " ");
	else
		strcpy(iobuffer, zonerec->desc);			/* matched the record */

	strcpy(callrec->zone_desc, iobuffer);
	callrec->nbr = 0;

	temp_hour = pickup_date_time / 100;				/* convert hhmm format to seconds */
	temp_minute = pickup_date_time - 100 * temp_hour;
	sprintf(iobuffer,"%02d%02d%02d%02d%02d%02d",date_value->tm_year % 100,date_value->tm_mon+1,date_value->tm_mday,temp_hour,temp_minute,0);
	callrec->due_date_time = conversion(iobuffer);		/* store this call due due and time */
									/* store this call due date in display format */ 
	if (DATE_FORMAT == DAY_FIRST_IN_DATE)
	  sprintf(callrec->due_date,"%02d%c%02d%c%02d",
		  date_value->tm_mday, DATE_SEPERATOR,
		  date_value->tm_mon + 1, DATE_SEPERATOR, date_value->tm_year % 100); 
	else if (DATE_FORMAT == YEAR_FIRST_IN_DATE)
	  sprintf(callrec->due_date,"%02d%c%02d%c%02d",
		  date_value->tm_year % 100, DATE_SEPERATOR,
		  date_value->tm_mon + 1, DATE_SEPERATOR, date_value->tm_mday); 
	else
	  sprintf(callrec->due_date,"%02d%c%02d%c%02d",
		  date_value->tm_mon + 1, DATE_SEPERATOR,
		  date_value->tm_mday, DATE_SEPERATOR, date_value->tm_year % 100); 

	get_time_from_short(pickup_date_time, callrec->due_time);	/* store this call due time in display format */ 

	callrec->grouped_with = 0;					/* read from subcall buffer. write into the call buffer */
	callrec->pod_time = 0;
	callrec->pod_required = NO;
	strcpy(callrec->pod_name, " ");
	strcpy(callrec->status, PENDING);
	callrec->screen_type = UI_EXTERNAL;
	callrec->pri_status = 63;
	callrec->fare_amount = 0;
	callrec->pos_in_group = ' ';
	callrec->nbr_in_group = ' ';

	callrec->subsc_call_nbr = subcallrec->nbr;
	callrec->fleet = subcallrec->fleet;

	strcpy(callrec->extended_type, subcallrec->extended_type);
	strcpy(callrec->pckup_adr_cmnt, subcallrec->pckup_adr_cmnt);
	
	callrec->local = subcallrec->local;
	callrec->pckup_str_nbr = subcallrec->pckup_str_nbr;
	callrec->pckup_str_suf = subcallrec->pckup_str_suf;

	strcpy(callrec->pckup_pre_dir, subcallrec->pckup_pre_dir);
	strcpy(callrec->pckup_str_name, subcallrec->pckup_str_name);
	strncpy(callrec->pckup_str_key, subcallrec->pckup_str_name, 4);
	strcpy(callrec->pckup_str_type, subcallrec->pckup_str_type);
	strcpy(callrec->pckup_post_dir, subcallrec->pckup_post_dir);
	strcpy(callrec->intr_pre_dir, subcallrec->intr_pre_dir);
	strcpy(callrec->intr_str_name, subcallrec->intr_str_name);
	strcpy(callrec->intr_str_type, subcallrec->intr_str_type);
	strcpy(callrec->intr_post_dir, subcallrec->intr_post_dir);
	strcpy(callrec->pckup_apt_room, subcallrec->pckup_apt_room);
	strcpy(callrec->pckup_city, subcallrec->pckup_city);
	strcpy(callrec->passenger_name, subcallrec->passenger_name);
	strncpy(callrec->pass_name_key, subcallrec->passenger_name, 4);

	callrec->pckup_zone = subcallrec->pckup_zone;
#ifdef SCANDINAVIA
	strcpy(callrec->phone_number, subcallrec->phone_number);

	/* Search is keyed off of phone_nbr, so copy it */
	strncpy(callrec->phone_nbr, subcallrec->phone_number, 7);
	callrec->phone_nbr[7] = '\0';
#else
	strcpy(callrec->phone_area, subcallrec->phone_area);
	strcpy(callrec->phone_nbr, subcallrec->phone_nbr);
	strcpy(callrec->phone_ext, subcallrec->phone_ext);
#endif
	strcpy(callrec->general_cmnt, subcallrec->general_cmnt);
	strcpy(callrec->dest_adr_cmnt, subcallrec->dest_adr_cmnt);

	callrec->dest_str_nbr = subcallrec->dest_str_nbr;
	callrec->dest_str_suf = subcallrec->dest_str_suf;

	strcpy(callrec->dest_pre_dir, subcallrec->dest_pre_dir);
	strcpy(callrec->dest_str_name, subcallrec->dest_str_name);
	strcpy(callrec->dest_str_type, subcallrec->dest_str_type);
	strcpy(callrec->dest_post_dir, subcallrec->dest_post_dir);
	strcpy(callrec->dest_apt_room, subcallrec->dest_apt_room);
	strcpy(callrec->dest_city, subcallrec->dest_city);

	callrec->dest_zone = subcallrec->dest_zone;
	callrec->personal_veh = subcallrec->personal_veh;
	callrec->personal_rqst = subcallrec->personal_rqst;
	callrec->veh_attr_flag = subcallrec->veh_attr_flag;

	callrec->drv_attr_flag = subcallrec->drv_attr_flag;

	for ( i = 0; i < ATTR_MAX; i++ )
	  {
	    if ( subcallrec->drv_attr[i] == YES )
	      callrec->drv_attr[i] = YES;
	    else
	      callrec->drv_attr[i] = NO;
	    if ( subcallrec->veh_attr[i] == YES )
	      callrec->veh_attr[i] = YES;
	    else
	      callrec->veh_attr[i] = NO;
	  }

	callrec->nbr_of_vehs = subcallrec->nbr_of_vehs;
	callrec->priority = subcallrec->priority;

	strcpy(callrec->charge_nbr, subcallrec->charge_nbr);
	strcpy(callrec->requested_by, subcallrec->requested_by);
	strcpy(callrec->person_to_see, subcallrec->person_to_see);

	callrec->pod_required = subcallrec->pod_required;
	callrec->call_rate = subcallrec->call_rate;

	callrec->date_time_in = subcallrec->date_time_in;
	callrec->entered_by = subcallrec->entered_by; 
	callrec->last_changed_by = subcallrec->last_changed_by;

	strcpy(callrec->date_in, subcallrec->date_in);
	strcpy(callrec->time_in, subcallrec->time_in);
	strcpy(callrec->ac_addt1_info1, subcallrec->ac_addt1_info1);
	strcpy(callrec->ac_addt1_info2, subcallrec->ac_addt1_info2);

	strcpy(callrec->status_bits, subcallrec->status_bits);
	strcpy(callrec->company_name, subcallrec->company_name);
	strcpy(callrec->driver_msg, subcallrec->driver_msg);
	strcpy(callrec->pckup_pmap, subcallrec->pckup_pmap);
	strcpy(callrec->dest_dmap, subcallrec->dest_dmap);

	callrec->advise_arrival = subcallrec->advise_arrival;
	callrec->est_wait_time[0] = NULL;

	callrec->ext_nbr = -1;
	callrec->gps_lat = 0;
	callrec->gps_long = 0;
        callrec->dest_gpsx = 0.0;
        callrec->dest_gpsy = 0.0;

#ifdef NAMERICA
	if ( subcallrec->send_rate != NO )
	  callrec->send_rate = YES;
#endif
	
#ifdef __alpha
	if ( callrec->extended_type[0] != WAKEUP )
	  read_gps_coords( callrec );
#endif
	
	return(0);
}

/******************************************************************************************************************/
/* send_to_dispatch - this routine is called if the subscription call written to the call file falls on a holiday */
/******************************************************************************************************************/
send_to_dispatch()
{
	int i;									/* temporary variable for loops */
	int blen;								/* length of record */

	memset((char *)&cpipe_rec,'\0',sizeof(struct call_ipc_rec));	/* initialize fifo buffer */

	cpipe_rec.u_prcs_id = 0;					/* Process id */
	cpipe_rec.rec_type = CALL_REQ;					/* Record type */
	cpipe_rec.msg_nbr = 0;						/* Message number */
	cpipe_rec.c_isam_nbr = last_isrecnum;				/* Cisam number */
	cpipe_rec.call_nbr = callrec->nbr;				/* Call number */
	cpipe_rec.grouped_with = callrec->grouped_with;		/* Grouped with */
	cpipe_rec.fleet = callrec->fleet;				/* Fleet */
	get_call_type();						/* Type */
	cpipe_rec.pickup_zone = callrec->pckup_zone;			/* Pickup zone */
	cpipe_rec.dest_zone = callrec->dest_zone;			/* Destination zone */
	cpipe_rec.personal_veh = callrec->personal_veh;			/* Personal vehicle */
	cpipe_rec.personal_rqst = callrec->personal_rqst;		/* Personal request */
	if (Toupper(callrec->veh_attr_flag) == YES){				/* Any attributes turned on ? */
		for(i=0; i<ATTR_MAX; i++) {					/* set status bits */
	   		if (Toupper(callrec->veh_attr[i]) == YES)			/* is attribute set? */
	    			set_bit(i,1,(unsigned char *)&cpipe_rec.veh_attr);		/* yes, set corresponding bit */
    		}
	}
	if (Toupper(callrec->drv_attr_flag) == YES){				/* Any attributes turned on ? */
    		for(i=0; i<ATTR_MAX; i++) {					/* set status bits */
			if (Toupper(callrec->drv_attr[i]) == YES)		/* is attribute set? */
	    			set_bit(i,1,(unsigned char *)&cpipe_rec.drv_attr);		/* yes, set corresponding bit */
    		}
	}
	cpipe_rec.priority = callrec->priority;				/* Priority */
	cpipe_rec.due_time = callrec->due_date_time;			/* Due date/time */
	cpipe_rec.old_due_time = 0;					/* make sute system knows its a new call for F35*/
	cpipe_rec.updated_call = 0;
	cpipe_rec.call_type.time = 1;
	if ( its_a_holiday )
	  cpipe_rec.status.holiday = 1;

	if (write(pipe_fd,&cpipe_rec,sizeof(struct call_ipc_rec)) <= 0) 
		ERROR(' ',"","Time Calls Error writing to named pipe");

	return(0);							/* successfull return */
}

/*********************************************************************************************************************/
/* get_call_type: gets the call type, after looking at the call type in call_struct.extended_type		     */
/*		  Sets appropriate bits in call_fifo.call_type 							     */
/*********************************************************************************************************************/
get_call_type()
{
	memset((char *)&cpipe_rec.call_type,'\0', sizeof(struct call_types));

	switch (callrec->extended_type[0]) {
		case PASS:
			cpipe_rec.call_type.passenger = 1;
			break;
		case DELIV:
			cpipe_rec.call_type.delivery = 1;
			break;
		case TYPEX:
			cpipe_rec.call_type.xtype = 1;
			break;
		case WAKEUP:
			cpipe_rec.call_type.wakeup = 1;
			break;
	}
			
	if (callrec->extended_type[1] == '\0')
		return;

	switch (callrec->extended_type[2]) { 			/* check the other char's */
		case '\0':
			return;
		case SUBS:
			cpipe_rec.call_type.subscription = 1;
			break;
		case TIMEC:
			cpipe_rec.call_type.time = 1;
			break;
		case CHARGE:
			cpipe_rec.call_type.charge = 1;
			break;
	}

	switch (callrec->extended_type[3]) { 			/* check the last char's */
		case '\0':
			return;
		case SUBS:
			cpipe_rec.call_type.subscription = 1;
			break;
		case TIMEC:
			cpipe_rec.call_type.time = 1;
			break;
		case CHARGE:
			cpipe_rec.call_type.charge = 1;
			break;
	}
}

/****************************************************************************************************************/
/* set_bit: sets the bit specified by bit_num to 1. The size of the bit structure and its address 		*/
/*		should be passed as arguments									*/
/****************************************************************************************************************/
set_bit(bit_num,size,struct_ptr)
int bit_num;						/* # of the bit to be tested; leaset significant = 0 */
int size;						/* size of bit struct in bytes */
unsigned char *struct_ptr;				/* pointer to the bit struct */
{
	unsigned char *bit_ptr;	/* poinetr to bit struct */
	unsigned char mask;
	int shift_by;	/* shift the mask by */

	mask = 0x01;
	bit_ptr = (unsigned char *)struct_ptr + bit_num/8;
	shift_by = bit_num % 8;
	mask <<= shift_by;

	*bit_ptr = *bit_ptr | mask;
}

/************************************************************************/
/* add_callh: adds a HOLIDAY call history record			*/
/************************************************************************/
add_callh(call_nbr, fleet_id)
     int call_nbr;
     char fleet_id;
{
	char date_str[10];			/* for date in display format */
	char time_str[10];			/* for time in display format */

	ch_ptr = (struct cisam_ch *)req_buf;
	memset((char *)ch_ptr,'\0',sizeof(struct cisam_ch));		/* initialize the buffer for reading call history rec */

	ch_ptr->call_nbr = call_nbr;	 	 			/* call # */
	ch_ptr->exception_nbr = HOL_TM_CALL;
	ch_ptr->fleet = fleet_id;
	ch_ptr->event_type = REGULAR_EVENT;					/* regular exception */
	ch_ptr->status_dt_tm = (int) time(NULL);
	get_pkup_date(date_str); 					/* get the date in display format */
	strcpy(ch_ptr->status_date,date_str);
	get_pkup_time(time_str); 					/* get the time in display format */
	strcpy(ch_ptr->status_time,time_str);
	strcpy(ch_ptr->event_desc,EVNT_HOLIDAY);
	ch_ptr->veh_nbr = 0;
	ch_ptr->drv_id = 0;
	ch_ptr->user_id = -1;
									/* let all action fields be NULL's or 0's */
	if (db(CALLH_FILE_ID,ADD_REC,&ch_key1,0,0) < 0) 	 	/* add the record to disk */
		ERROR(fleet_id,CALLH_FILE,"could not add HOLIDAY record");
}

add_callh_wakeup(call_nbr, fleet_id)
     int call_nbr;
     char fleet_id;
{
	char date_str[10];			/* for date in display format */
	char time_str[10];			/* for time in display format */

	ch_ptr = (struct cisam_ch *)req_buf;
	memset((char *)ch_ptr,'\0',sizeof(struct cisam_ch));		/* initialize the buffer for reading call history rec */

	ch_ptr->call_nbr = call_nbr;	 	 			/* call # */
	ch_ptr->exception_nbr = WAKE_UP;
	ch_ptr->fleet = fleet_id;
	ch_ptr->event_type = REGULAR_EVENT;					/* regular exception */
	ch_ptr->status_dt_tm = (int) time(NULL);
	get_pkup_date(date_str); 					/* get the date in display format */
	strcpy(ch_ptr->status_date,date_str);
	get_pkup_time(time_str); 					/* get the time in display format */
	strcpy(ch_ptr->status_time,time_str);
	strcpy(ch_ptr->event_desc,EVNT_WAKE_UP);
	ch_ptr->veh_nbr = 0;
	ch_ptr->drv_id = 0;
	ch_ptr->user_id = -1;
									/* let all action fields be NULL's or 0's */
	if (db(CALLH_FILE_ID,ADD_REC,&ch_key1,0,0) < 0) 	 	/* add the record to disk */
		ERROR(fleet_id,CALLH_FILE,"could not add HOLIDAY record");
}

add_callh_multi(call_nbr, fleet_id)
     int call_nbr;
     char fleet_id;
{
	char date_str[10];			/* for date in display format */
	char time_str[10];			/* for time in display format */
	struct excpt_desc *ex_ptr; 		/*for getting except desc from shared mem */
        
	ch_ptr = (struct cisam_ch *)req_buf;
	memset((char *)ch_ptr,'\0',sizeof(struct cisam_ch));		/* initialize the buffer for reading call history rec */

        ex_ptr = offset->except_desc + MULT -1;
	ch_ptr->call_nbr = call_nbr;	 	 			/* call # */
	ch_ptr->exception_nbr = MULT;
	ch_ptr->fleet = fleet_id;
	ch_ptr->event_type = REGULAR_EVENT;					/* regular exception */
	ch_ptr->status_dt_tm = (int) time(NULL);
	get_pkup_date(date_str); 					/* get the date in display format */
	strcpy(ch_ptr->status_date,date_str);
	get_pkup_time(time_str); 					/* get the time in display format */
	strcpy(ch_ptr->status_time,time_str);
	strcpy(ch_ptr->event_desc,ex_ptr->short_desc);
	ch_ptr->veh_nbr = 0;
	ch_ptr->drv_id = 0;
	ch_ptr->user_id = -1;
									/* let all action fields be NULL's or 0's */
	if (db(CALLH_FILE_ID,ADD_REC,&ch_key1,0,0) < 0) 	 	/* add the record to disk */
		ERROR(fleet_id,CALLH_FILE,"could not add HOLIDAY record");
}


read_gps_coords(cp)
struct cisam_cl *cp;						/* pointer to call structure with user supplied data */
{
    	int x;
	short ccode=0;						/* C-ISAM return code */
	short odd_address=0;					/* 0=even address; 1=odd address */
    	char sn_type=' ';
	int  s_idx = 0;
	struct cisam_sn sname;
	struct cisam_ad address;

    	strncpy(sname.name,cp->pckup_str_name,20);
    	strncpy(sname.str_type,cp->pckup_str_type,3);
    	if (db_read_key(STRNAME_FILE_ID,&sname,&sn_key2,ISEQUAL) >= 0) {		/* read street name record */
		sn_type = sname.rec_type;					/* save record type */
		s_idx = sname.index;						/* save index */
		address.str_index = s_idx;					/* fill in address key */
		strncpy(address.str_pre_dir,cp->pckup_pre_dir,2);
		strncpy(address.str_post_dir,cp->pckup_post_dir,2);
		strncpy(address.str_city,cp->pckup_city,3);
		address.high_nbr = cp->pckup_str_nbr;

		if (cp->pckup_str_nbr/2 == (cp->pckup_str_nbr+1)/2)					/* even address ? */
			odd_address = 0;
		else
			odd_address = 1;					/* address is odd */

		ccode = db_read_key(ADDRESS_FILE_ID,&address,&ad_key1,ISGTEQ); 	/* read an address record */
		while (ccode >= 0){						/* more records to read */
			if ((s_idx != address.str_index) || (strncmp(cp->pckup_pre_dir,address.str_pre_dir,2) != 0) ||	/* in range? */
			(strncmp(cp->pckup_post_dir,address.str_post_dir,2) != 0) || (strncmp(cp->pckup_city,address.str_city,3) != 0)) 
	    			return(-1);

			if (odd_address && (address.odd_even == ADDRESS_ODD || address.odd_even == ADDRESS_BOTH))
				break;						/* found appropriate range */

			if (!odd_address && (address.odd_even == ADDRESS_EVEN || address.odd_even == ADDRESS_BOTH))
				break;						/* found appropriate range */

			ccode = db_read_key(ADDRESS_FILE_ID,&address,&ad_key1,ISNEXT); 	/* read next ddress record */
		}

		if (ccode < 0)
			return(-1);						/* no record found */
		
		if ((address.low_nbr <= cp->pckup_str_nbr) && (address.high_nbr >= cp->pckup_str_nbr))
		  { 
		    cp->gps_lat = address.str_y_coord;
		    cp->gps_long = address.str_x_coord;
		  }
		return(-1);								/* address not in range */
    	}
    	return(-1);									/* no street name record */
}

AddCallMsg( cp )
     struct cisam_cl *cp;
{
  M_ACTION_STRUCT  M_Action;
  char             *pHash;
  char             FlatRate, AccountCall;
  char             MeterActionString[ 128 ];
  struct cisam_cmsg cl_msg;

  bzero( &M_Action, sizeof( M_ACTION_STRUCT ) );
  bzero( MeterActionString, sizeof( MeterActionString ) );
  
  M_Action.trans_cat[0] = cp->extended_type[0];
  M_Action.trans_cat[1] = cp->extended_type[1];

  if ( cp->call_rate > 0.2 )
    M_Action.flat_rate = YES;
  else
    M_Action.flat_rate = NO;

  if ( cp->extended_type[3] == CHARGE )
    M_Action.account_call = YES;
  else
    M_Action.account_call = NO;

  if ( db_read_key( M_ACTION_FILE_ID, &M_Action, &m_action_key1, ISEQUAL ) != SUCCESS )
    return( FALSE );

  if ( M_Action.flat_rate == YES )
    {
      pHash = strstr( M_Action.meter_action, "######" );
      if ( pHash != NULL )
	{
	  *pHash = '\0';
	  sprintf( MeterActionString, "%s%.0f", M_Action.meter_action, cp->call_rate );
	}
      else
	strcpy( MeterActionString, M_Action.meter_action );
    }
  else
    strcpy( MeterActionString, M_Action.meter_action );
  

  bzero( &cl_msg, sizeof( struct cisam_cmsg ) );
  cl_msg.call_nbr = cp->nbr;
  cl_msg.msg_nbr = '1';
  cl_msg.device = '1';
  cl_msg.ack = NO;
  strcpy( cl_msg.msg, MeterActionString );

  db_write( CALLMSG_FILE_ID, &cl_msg );
  return( SUCCESS );
}
