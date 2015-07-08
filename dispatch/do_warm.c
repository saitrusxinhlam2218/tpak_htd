static char sz__FILE__[]="@(#)do_warm.c	10.1.2.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  do_warm.c; Rel: 10.1.2.1; Get date: 5/11/95 at 10:14:10
 * @(#)  Last delta: 12/2/94 at 18:43:22
 * @(#)  SCCS File: /taxi/sccs/s.do_warm.c
 *                                                                           *
 *****************************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <string.h>
#include <errno.h>

#include "timers.h"
#include "taxipak.h"
#include "Object.h"
#include "dispatch_strdefs.h"
#include "path.h"
#include "sprintf.h"
#include "mads_types.h"
#include "mad_error.h"
#include "ui_ipc.h"
#include "except.h"
#include "calls.h"
#include "kdt.h"
#include "veh_driv.h"
#include "mads_isam.h"
#include "df_maint.h"
#include "dispatch.h"
#include "writer.h"
#include "language.h"
#include "Call_timers_public.h"
#include "Fleet_db.h"

#define	DATE_FIELD_SZ	9
#define TIME_FIELD_SZ	6
#define WARM_FLEET_DEF	FLEET_MAX					/* warm start fleet default: all fleets */
#define WARM_TIME_DEF	120						/* warm start go-back time default: 2 hrs */

extern struct fleets *fleet [FLEET_MAX];        /* shared memory fleet structure */
extern char req_buf[], buf[];
extern struct keydesc cl_key3, em_key1;
extern char *scratch_ptr;
extern time_t mads_time;

static int go_back_time, no_fleets, act_fleets;
static char act_fleet_id[FLEET_MAX+1], fleet_id[FLEET_MAX+1];
int	grouped[100];
int	cnt = 0;

int get_time();
struct veh_driv *get_veh_ptr();

/************************************************************************/
/*  do_warm()								*/
/*     Rebuild CALL list and emergency exceptions from disk		*/
/*     after MADS goes away for a while.				*/
/*									*/
/*     RETURNS:    0   successful					*/
/*                -1   failed						*/
/*									*/
/*     ROUTINES used:							*/
/*          get_act_fleets      get current active fleets		*/
/*          get_ws_data         get user-specified warm start data	*/
/*          rebuild_calls       rebuild call list			*/
/*          regen_emer          regenerate emergency exceptions		*/
/*          strcpy              copy one string to another		*/
/*									*/
/*     GLOBALS:								*/
/*          act_fleets          number of active fleets (static)	*/
/*          act_fleet_id        active fleet id's			*/
/************************************************************************/
do_warm()
{
	short err = 0;

#ifdef DEBUG
	int i;
	sprintf(trace_str, "entering do_warm\n");
	TRACE(trace_str);
#endif

	if((act_fleets = get_act_fleets(act_fleet_id)) == -1)		/* get active fleets from database */
		return(-1);

#ifdef DEBUG
	sprintf(trace_str, "active fleets:");
	for (i=0; i<act_fleets; i++)
		sprintf(trace_str+strlen(trace_str), " %c", act_fleet_id[i]);
	sprintf(trace_str+strlen(trace_str), "\n");
	TRACE(trace_str);
#endif

	if (!err) {
		get_ws_data();					/*  Read input from warm start file */
		if (no_fleets == FLEET_MAX) {			/* if all fleets, then use the active fleets */
	     		strcpy(fleet_id, act_fleet_id);
	     		no_fleets = act_fleets;
	     	}
	}

#ifdef FOO
	if (rebuild_calls() == -1)				/*  Rebuild CALLS List */
		err = 1;

	if (regen_emer() == -1)					/*  Regenerate Emergency Exceptions */
		err = 1;
#endif

#ifdef DEBUG
	sprintf(trace_str, "leaving do_warm\n");
	TRACE(trace_str);
#endif

	if (err)
		return(-1);
	else
		return(0);
}

/****************************************************************************************/
/*  rebuild_calls()									*/
/*											*/
/*     Rebuild CALL list for selected and active fleet from disk.			*/
/*											*/
/*     RETURNS:    0   successful							*/
/*                -1   failed								*/
/*											*/
/*     ROUTINES used:									*/
/*          is_fleet_act        is a fleet active					*/
/*          db                  MADS database access routine				*/
/*          fill_call_rec       fill struct call_rec with data from disk		*/
/*          add_call            add a call to the call list				*/
/*          call_pre_process    preprocess a call(add call,updates,zone summary)	*/
/*          mk_except           generate an exception					*/
/*          get_time            get current time					*/
/*          get_asc_time        get time is ASCII format				*/
/*											*/
/*     GLOBALS:										*/
/*          fleet_id  (static)								*/
/*          go_back_time  (static)							*/
/*          no_fleets  (static)								*/
/****************************************************************************************/
rebuild_calls()
{
  register int i, retval, cur_dt_tm;
  int start_time;
  short	dummy;
  char cur_date[DATE_FIELD_SZ+1], cur_time[TIME_FIELD_SZ+1];
  struct cisam_cl *callrec;
  struct calls *call_ptr;
  struct call_ipc_rec call_buf;
  char cancelstr[9];
  char killstr[9];
  
  start_time = (int)time((time_t *)0) - go_back_time*60;
  
#ifdef DEBUG
  sprintf(trace_str,"time calls to be built= %d   fleets calls to be built= %d\n", start_time, no_fleets);
  TRACE(trace_str);
#endif
  
  callrec = (struct cisam_cl *)req_buf;
  
  for (i=0; i < no_fleets; i++) {					/* go through one fleet at a time */
    if (!is_fleet_act(fleet_id[i])) {			/* if inactive, forget it */
      fprintf(stderr, "fleet %c is NOT active\n", fleet_id[i]);
#ifdef DEBUG
      sprintf(trace_str, "fleet %c is NOT active\n", fleet_id[i]);
      TRACE(trace_str);
#endif
      continue;
    }
    
    fprintf(stderr, "Rebuilding CALLS for fleet %c\n", fleet_id[i]);
    
#ifdef DEBUG
    sprintf(trace_str, "Rebuilding CALLS for fleet %c\n", fleet_id[i]);
    TRACE(trace_str);
#endif
    callrec->fleet = fleet_id[i];
    callrec->due_date_time = start_time;
    
    if (db(CALL_FILE_ID, READ_KEYS,  &cl_key3, ISGTEQ, 0) < 0) {
      if (iserrno == EENDFILE || iserrno == ENOREC)		/* ignore EOF or no record found error*/
	continue;					/* finished this fleet */
      else {
	sprintf(trace_str,"READ (KEYS) CALL file err - %d",iserrno);
	ERROR (fleet_id[i], "", trace_str)
	  /* db(CALL_FILE_ID, CLOSE, &cl_key3, 0,0); */
	  continue;
      }
    }
    
    while(1) {
      CheckMsgQ();                                                    /* Hey Im not really dead! (monitor)*/
      if ( (callrec->close_date_time == 0) &&
	  (strcmp(callrec->status, CANCELD)) &&
	  (strcmp(callrec->status, KILLED)) )
	{
	  if(callrec->pckup_date_time == 0)
	    {
	      
	      if(callrec->grouped_with)			/* Add MULT call to list */
		set_grp(callrec->grouped_with);
	      
	      if(!grp_call(callrec->nbr)) {			/* Not part of existing MULT call */
		
		Call_ipc_rec_load( &call_buf, (CALL_REC_HNDL) callrec );
		
		if(callrec->dispd_date_time > 0)
		  {
		    
		    call_ptr = (CALL *)Call_activate(&call_buf, &dummy);
		    
		    if(callrec->veh_nbr != 0)
		      {
			if (Veh_get_hndl(BY_NBR, callrec->veh_nbr,
					 call_ptr->fleet, (VEH_HNDL *) & call_ptr->veh_ptr) == FAIL)
			  {							    
			    sprintf(scratch_ptr,"fill_call - invalid vehicle %d",
				    callrec->veh_nbr);
			    ERROR(call_ptr->fleet,"",scratch_ptr);
			  }
			else
			  {
			    call_ptr->veh_ptr->t_status.idle = 0;
			    call_ptr->veh_ptr->t_status.offered = 1;
			    call_ptr->veh_ptr->t_status.accepted = 1;
			    
			    if (call_ptr->veh_ptr->t_status.e_posted==TRUE)
			      {
				switch (fleet[call_ptr->fleet]->when_enroute_post)
				  {
				  case '3':
				  case '2':
				    call_ptr->veh_ptr->t_status.pckup = TRUE;
				    break;
				  case '1':
				  default:
				    break;
				  }
			      }
			    
			    call_ptr->veh_ptr->time_info = callrec->dispd_date_time;
			    call_ptr->veh_ptr->call_ptr = call_ptr;
			    call_ptr->veh_ptr->call_nbr = call_ptr->call_number;
			  }
		      }
		    if (fleet[call_ptr->fleet]->late_meter)
		      {
			if ((fleet[call_ptr->fleet]->late_meter + callrec->dispd_date_time) < mads_time)
			  /*	                      mk_except(call_ptr, (struct veh_driv *) NULL, LATE_METER, 0, -1); */
			  {
			    Call_set_state((CALL_HNDL) call_ptr, CALL_LATE_METER_STATE, HNDL_NULL);
			  }
			else
			  {
			    if (Call_add_timer((CALL_HNDL) call_ptr, T_LATE_METER,
					       fleet[call_ptr->fleet]->late_meter -
					       (mads_time - callrec->dispd_date_time),
					       (int (*)())call_latemeter_notify_func) != SUCCESS)
			      ERROR(' ', "", "add call timer T_LATE_METER");
			  }
		      }		
		  }
		else
		  {				/* not assigned yet: add to calls & zone summary */
		    if((strncmp(callrec->status,CANCELD,strlen(callrec->status))) && 
		       (strncmp(callrec->status,KILLED,strlen(callrec->status))) &&
		       (strncmp(callrec->status,PENDING,strlen(callrec->status))))
		      (void)Call_initialize(&call_buf, FALSE);
		  }
	      }
	    }
	  else
	    {         					/* already picked up, assume it is closed */
	      cur_dt_tm = get_time();
	      callrec->close_date_time = cur_dt_tm;
	      get_asc_time((long)cur_dt_tm, cur_date, cur_time);
	      strncpy(callrec->close_date, cur_date, DATE_FIELD_SZ);
	      strncpy(callrec->close_time, cur_time, TIME_FIELD_SZ);
	      strcpy(callrec->status, COMPLETE);
	      callrec->pri_status = 64;				/* complete */
	      
	      if(db(CALL_FILE_ID, UPDATE_REC, &cl_key3, 0, 0) < 0) {
		sprintf(trace_str,"Can't UPDATE CALL file - %d, CALL NUMBER - %d", iserrno, 
			callrec->nbr);
		ERROR (' ', "", trace_str)
	      }
	    }
	}
      
      if (db(CALL_FILE_ID, READ_KEYS, &cl_key3, ISNEXT, 0) == SUCCESS) {
	if (callrec->fleet != fleet_id[i])
	  break;					/* done with this fleet, next fleet */
      }
      else {
	if (iserrno == EENDFILE || iserrno == ENOREC)		/* ignore EOF or no record found error*/
	  break;						/* finished this fleet */
	else {
	  sprintf(trace_str,"READ (KEYS) CALL file err - %d",iserrno);
	  ERROR (fleet_id[i], "", trace_str)
	    /* db(CALL_FILE_ID, CLOSE, &cl_key3, 0, 0); */
	    continue;
	}
      }
    }
  }
  
  return(0);
}


/************************************************************************/
/*  regen_emer()							*/
/*									*/
/*     Regenerate emergency exceptions from disk.			*/
/*     Go through each record, regenerate an exception for every	*/
/*     unresovled emergency for each active fleet.			*/
/*									*/
/*     RETURNS:    0   successful					*/
/*                -1   failed						*/
/*									*/
/*     ROUTINES used:							*/
/*          is_fleet_act        is a fleet active			*/
/*          db                  MADS database access routine		*/
/*          mk_except           generate an exception			*/
/*          get_veh_ptr         get a vehicle pointer			*/
/*          find_fl_nbr         find a fleet number			*/
/*									*/
/*     GLOBALS:								*/
/*          fleet_id  (static)						*/
/*          no_fleets  (static)						*/
/*									*/
/************************************************************************/
regen_emer()
{
	register int i, flt_nbr;
	struct cisam_em *emer_rec;
	struct veh_driv *veh_ptr;
	FLEET_HNDL   hFleet;
	VEH_HNDL     hVehicle;
	FLEET_VEH_NBR_LIST_HNDL  veh_nbr_list_hndl;

	fprintf(stderr, "Regenerating EMERGENCY exceptions\n");

#ifdef DEBUG
	sprintf(trace_str, "Regenerating EMERGENCY exceptions\n");
	TRACE(trace_str);
#endif

	if (db(EMERG_FILE_ID, OPEN, &em_key1, ISINPUT | ISMANULOCK, 0) < 0) {			/* open for read only */
		sprintf(trace_str, "Can't OPEN EMERG file - %d", iserrno);
		ERROR (' ', "", trace_str)
		return(-1);
	}

	emer_rec = (struct cisam_em *)req_buf;

	for (i=0; i < no_fleets; i++) 
	  {
		if (!is_fleet_act(fleet_id[i]))		
	   		continue;

		Fleet_get_hndl( BY_NBR, i, &hFleet );
		veh_nbr_list_hndl = ( FLEET_VEH_NBR_LIST_HNDL ) Fleet_get_value( hFleet, FLEET_VEH_NBR_LIST );

		emer_rec->fleet = fleet_id[i];
		emer_rec->date_time = 0;		

/*
 *		get first record
 */
		if (db(EMERG_FILE_ID, READ_KEYS, &em_key1, ISGTEQ, 0) >= 0)
		  {
			while (1) 
			  {
	     		if (emer_rec->fleet != fleet_id[i])	
	          		break;
/* 
 *				not resolved yet
 *				make exception
 */
	     		if ((emer_rec->resolved_dt_tm == 0) && (time(0) - emer_rec->date_time < 86400)) 
			  {
/*
 *				added vehicle context
 *				21.12.90 tek				
 *
 */
	          		if ( ( hVehicle = ( VEH_HNDL )Fleet_veh_nbr_list_find( veh_nbr_list_hndl, emer_rec->veh_nbr )) == HNDL_NULL )
				  continue;

				veh_ptr = ( struct veh_driv * ) hVehicle;
				
		                veh_ptr->t_status.emer = 1;                                     /* It's an emergency now */
               			fleet[veh_ptr->fleet_nbr]->emergencies +=1;                     /* increment the fleet number of emergencies */
		                fleet[veh_ptr->fleet_nbr]->now_emergencies +=1;                 /* increment the fleet number of emergencies */
                		veh_ptr->emergency_on_date = emer_rec->date_time;
              			veh_ptr->emergency_off_date = 0; 
	          		mk_except((struct calls *)NULL, veh_ptr, EMERGENCY,0,-1);
	          	  }

/*
 *				get next record
 */
				if (db(EMERG_FILE_ID, READ_KEYS, &em_key1, ISNEXT, 0) < 0) 
				  {
					if (iserrno == EENDFILE || iserrno == ENOREC)
						break;
	          		else 
					  {
						sprintf(trace_str, "READ EMERG file err - %d", iserrno);
						ERROR (fleet_id[i], "", trace_str)
						db(EMERG_FILE_ID, CLOSE, &em_key1, 0,0);
							break;
					  }
				  }
			  }
		  }
	  }

	if (db(EMERG_FILE_ID, CLOSE, &em_key1, 0, 0) < 0) {				/* close */
		sprintf(trace_str, "Can't CLOSE EMERG file - %d", iserrno);
		ERROR (' ', "", trace_str)
		return(-1);
	}

	return(0);
}

/****************************************************************/
/* get_ws_data()						*/
/*								*/
/*    Read warm start parameters from a file and		*/
/*    assign the proper values.					*/
/*								*/
/*    variables init'ed here:					*/
/*       no_fleets -  number of fleets chosen by user		*/
/*       fleet_id  -  fleet id's chosen by user			*/
/*       go_back_time -  how far back for building call list	*/
/*								*/
/*    returns:  NONE						*/
/*								*/
/****************************************************************/

get_ws_data()
{
	FILE *fp;
	register int i, cnt = 0, j, init_err = 0;
	short dup;
	char chr;

	if ((fp = fopen(WARM_TMP_FILE, "r")) != NULL) {
   		if (fscanf(fp, "%d ", &no_fleets) == 1) {
      			if (no_fleets <= FLEET_MAX) {
         			i=0;
         			while (fscanf(fp, "%c", &chr) == 1) {
            				if (chr == ' ' || chr == '\n' || chr == '\0')
               					continue;

	    				chr = Toupper(chr);

            				dup = 0;
            				for (j=0; j<cnt; j++) {				 /* make sure no duplicates */
                				if (fleet_id[j] == chr) {   
                   					dup = 1;
                   					break;
                   				}
					}

            				if (!dup) {
						if(is_fleet_act(chr))
               						fleet_id[cnt++] = chr;
						else
							init_err = 3;			/* Invalid fleet entered */
	    				}

            				if (++i == no_fleets)
               					break;
            			}

         			if (i != no_fleets)
            				init_err = 2;				/* fscanf error */
         			else
            				no_fleets = cnt;
         		}
      			else
         			no_fleets = FLEET_MAX;

      			if (!init_err) {
         			if (fscanf(fp, "%d", &go_back_time) != 1)
            				init_err = 2;
			}
      		}
   		else
      			init_err = 2;						/* fscanf error */
   	}
	else
   		init_err = 1;							/* fopen error */

	if (init_err != 1)
   		fclose(fp);							/* close the file */

/*	removed in response to sms p.1321  TEK
 *	unlink(WARM_TMP_FILE);   remove the temp file 
 */

	if (init_err) {								/* if error, then default values assumed */
   		if (init_err == 1)
                {
                        sprintf(scratch_ptr,"Error %d for warm strt file open; defaults assumed", errno);
      			ERROR(' ', "", scratch_ptr);
                }
   		else if(init_err == 2)
      			ERROR(' ', "", "Can't fscanf warm start file; default values assumed")
   		else
      			ERROR(' ', "", "Invalid fleet entered; default values assumed")

   		no_fleets = WARM_FLEET_DEF;
   		go_back_time = WARM_TIME_DEF;
   	}

#ifdef DEBUG
	else
   		if (no_fleets <= FLEET_MAX) {
      			sprintf(trace_str, "get_ws_data: no. fleets chosen= %d: ", no_fleets);
      			for (i=0; i<no_fleets; i++)
         			sprintf(trace_str+strlen(trace_str), "%c ", fleet_id[i]);
      			sprintf(trace_str+strlen(trace_str), "\n");
      			TRACE(trace_str);
      			sprintf(trace_str, "get_ws_data: go_back_time= %d (min)\n", go_back_time);
      			TRACE(trace_str);
      		}

	if ((init_err) && (no_fleets >= FLEET_MAX)) {
   		sprintf(trace_str, "get_ws_data: All active fleets chosen (default)\n");
   		TRACE(trace_str);
   		sprintf(trace_str, "get_ws_data: go_back_time= %d (min)(default)\n", go_back_time);
   		TRACE(trace_str);
   	}
#endif
}

/********************************************************/
/* is_fleet_act()					*/
/*							*/
/*    Check if a fleet is active.			*/
/*							*/
/*    returns:  0   not active				*/
/*              1   active				*/
/********************************************************/
is_fleet_act(fleet)
char fleet;
{
	register int i;

	for (i=0; i < act_fleets; i++) {
   		if (act_fleet_id[i] == fleet)
      			return (1);
	}
	return(0);
}

set_grp(num)
int	num;
{
	grouped[cnt++] = num;
}

grp_call(num)
int	num;
{
	int	i;

	for(i = 0; i < cnt; i++) 
		if(num == grouped[i])
			return(1);

	return(0);
}

/*====================================================================
= get_act_fleets() -
=	Scans all active fleets.
=	Returns the number of active fleets and what they are.
=	What they are is returned with the argument.
=
=====================================================================*/
int get_act_fleets(fleet_id)
char fleet_id[];
{
	register int ccode, fleetno=0;
	struct	cisam_fl *fleetrec;

	fleetrec=(struct cisam_fl *)req_buf;						/* to cast req_buf */

	if(db(FLEET_FILE_ID,OPEN,&fl_key1,ISINOUT | ISMANULOCK,0) < 0){		/*  open the file */
		ERROR(' ',FLEET_FILE,"getactflts.c - cannot open file\n");
		return(-1);
	}

	fleetrec->nbr = 0;
	ccode=db(FLEET_FILE_ID,READ_KEYS,&fl_key2,ISEQUAL,0);
	while (ccode >=0) {
		if (fleetrec->active == YES) {
			if (fleetno >= FLEET_MAX) {
				ERROR(' ', "", "Fleet no. exceeds FLEET_MAX");
				db(FLEET_FILE_ID,CLOSE,&fl_key2,0,0);
				return(-1);
			}
			fleet_id[fleetno++] = fleetrec->id;
		}	
		ccode=db(FLEET_FILE_ID,READ_KEYS,&fl_key2,ISNEXT,0);
	}	

	db(FLEET_FILE_ID,CLOSE,&fl_key2,0,0);
	fleet_id[fleetno] = NULL;
	return(fleetno);
}
