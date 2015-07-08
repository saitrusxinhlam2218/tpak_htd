static char sz__FILE__[]="@(#)writer.c	10.1.2.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  writer.c; Rel: 10.1.2.1; Get date: 5/11/95 at 10:16:39
 * @(#)  Last delta: 12/2/94 at 18:45:46
 * @(#)  SCCS File: /taxi/sccs/s.writer.c
 *                                                                           *
 *****************************************************************************/

/* base on R3 version 5.7 */

#include <stdio.h>
#include <isam.h>
#include <time.h>
#include <sys/types.h>
#include <string.h>
#include "sprintf.h"
#include "path.h"
#include "mads_types.h"
#include "mad_error.h"
#include "mads_isam.h"
#include "mad_ipc.h"
#include "df_maint.h"
#include "ex_key.h"
#include "acc.h"
#include "veh_driv.h"
#include "writer.h"
#include "except.h"
#include "warm_status.h"
#include "language.h"

#define ASSIGNED_PRI 64
extern struct cisam_ch writer_buf;
extern char req_buf[];
extern struct offsets *offset;				/* offsets of structures in shared memory */

extern warm_start;
extern time_t mads_time;					/* system time update in main dispatch loop */
char cur_dt[10];						/* date in ascii format */
char cur_tm[7];							/* time in ascii format */



/***********************************************************************************************************************************
writer_post -
Called by ADD_TAXI when a driver posts on to a mobile terminal.
The VEHICLE record is also updated to show last zone posted and last time posted.
***********************************************************************************************************************************/
writer_post(fleet,veh_nbr,zone,ptime,epost)
char fleet;
short veh_nbr;
int zone;
long ptime;
int epost;
{
        struct cisam_vh *vh_ptr;

	if (warm_start) return;
        vh_ptr = (struct cisam_vh *)req_buf;

        vh_ptr->fleet=fleet;
        vh_ptr->nbr=veh_nbr;
        if (db(VEHICLE_FILE_ID,READ_KEYS,&veh_key2,ISEQUAL+ISLOCK,0) < 0)
	{
                sprintf(error_str,"Error %d reading record: veh_nbr %d",iserrno,  veh_nbr);
         	ERROR(fleet,VEHICLE_FILE,error_str);
                return(-1);                                             /* if error, return */
        }

	if (epost == 1)
		vh_ptr->status = WARM_STATUS_EPOST;
	else
		vh_ptr->status = WARM_STATUS_POST;


        vh_ptr->last_time_posted = ptime;
        vh_ptr->last_zone_posted = zone;

        if (db(VEHICLE_FILE_ID,UPDATE_REC,&veh_key2,0,0) < 0)
        { /* update VEHICLE record */
                sprintf(error_str,"Error %d updating record: veh_nbr %d",iserrno,  veh_nbr);
                ERROR(fleet,VEHICLE_FILE,error_str);
                db(VEHICLE_FILE_ID,RELEASE,&veh_key2,0,0);              /* unlock VEHICLE record */
                return(-1);                                             /* if error, return */
	}

        if (db(VEHICLE_FILE_ID,RELEASE,&veh_key2,0,0) < 0)
        {       /* unlock VEHICLE record */
                sprintf(error_str,"Error %d releasing record: veh_nbr %d",iserrno,  veh_nbr);
                ERROR(fleet,VEHICLE_FILE,error_str);
                return(-1);                                     
	}
	return(0);
}


/***********************************************************************************************************************************
writer_unpost -
Called by FIND_TAXI when a driver posts on to a mobile terminal.
The VEHICLE record is also updated to show last zone posted and last time posted are cleared.
***********************************************************************************************************************************/
writer_unpost(fleet,veh_nbr)
char fleet;
short veh_nbr;
{
        struct cisam_vh *vh_ptr;

        vh_ptr = (struct cisam_vh *)req_buf;

        vh_ptr->fleet=fleet;
        vh_ptr->nbr=veh_nbr;
        if (db(VEHICLE_FILE_ID,READ_KEYS,&veh_key2,ISEQUAL+ISLOCK,0) < 0)
        {     /* read VEHICLE record */
                sprintf(error_str,"Error %d reading record: veh_nbr %d", iserrno, veh_nbr);
                ERROR(fleet,VEHICLE_FILE,error_str);
                return(-1);
	}


        vh_ptr->last_time_posted = 0L;
        vh_ptr->last_zone_posted = 0;



        if (db(VEHICLE_FILE_ID,UPDATE_REC,&veh_key2,0,0) < 0)
        { 
		/* update VEHICLE record */
                sprintf(error_str,"Error %d updating record: veh_nbr %d", iserrno, veh_nbr);
                ERROR(fleet,VEHICLE_FILE,error_str);
                db(VEHICLE_FILE_ID,RELEASE,&veh_key2,0,0);
                return(-1);                                             /* if error, return */
        }

        if (db(VEHICLE_FILE_ID,RELEASE,&veh_key2,0,0) < 0)
        {      
		 /* unlock VEHICLE record */
                sprintf(error_str,"Error %d releasing record: veh_nbr %d", iserrno, veh_nbr);
                ERROR(fleet,VEHICLE_FILE,error_str);
                return(-1);                                             /* if error, return */
        }
}

/***********************************************************************************************************************************
writer_missing_receipts
Called when missing receipt nfo updated
***********************************************************************************************************************************/
int
writer_missing_receipts(veh_ptr)
struct veh_driv *veh_ptr;
{
        return(0);
}



/***********************************************************************************************************************************
writer_sign_on -
Called by DISPATCH when a driver signs on to a mobile terminal.  The DRIVER record is modified to show vehicle number and time,
and the driver statistics are reset.  The VEHICLE record is also updated to show driver and time.
***********************************************************************************************************************************/
writer_sign_on(fleet,veh_nbr,drv_id)
char fleet;
short veh_nbr;
int drv_id;
{
	struct cisam_dr *dr_ptr;
	struct cisam_vh *vh_ptr;


/*
 *	if (warm_start) return;      This line was removed to comply with sms p.1284
 *				     and could case performance problems on a warm start.
 */

	dr_ptr = (struct cisam_dr *)req_buf;
	vh_ptr = (struct cisam_vh *)req_buf;
	get_asc_time(mads_time,cur_dt,cur_tm);			/* get current time */


	dr_ptr->fleet = fleet;					/* fill in fleet for key */
	dr_ptr->id = drv_id;					/* fill in driver id for key */

	if (db(DRIVER_FILE_ID,READ_KEYS,&dr_key1,ISEQUAL+ISLOCK,0) < 0) {	/* read DRIVER record */
		sprintf(error_str,"Error %d reading record: drv_id %d", iserrno, drv_id);
		ERROR(fleet,DRIVER_FILE,error_str);
		db(DRIVER_FILE_ID,RELEASE,&dr_key1,0,0);		/* unlock DRIVER record */
		return(-1);						/* if error, return */
	}

	dr_ptr->last_veh_nbr = veh_nbr;				/* fill in vehicle number */
	dr_ptr->signed_on_dt_tm = mads_time;			/* fill in current date and time */
	dr_ptr->signed_of_dt_tm = 0;				/* clear out signed off time */
	dr_ptr->assigns = 0;					/* reset all driver statistics */
	dr_ptr->calls = 0;
	dr_ptr->flags = 0;
	dr_ptr->bids = 0;
	dr_ptr->posts = 0;
	dr_ptr->cond_posts = 0;
	dr_ptr->enroute_posts = 0;
	dr_ptr->short_meters = 0;
	dr_ptr->late_meters = 0;
	dr_ptr->no_accepts = 0;
	dr_ptr->rejects = 0;
	dr_ptr->breaks = 0;
	dr_ptr->req_to_talk = 0;
	dr_ptr->messages = 0;
	dr_ptr->callbacks = 0;
	dr_ptr->callouts = 0;
	dr_ptr->no_shows = 0;

	if (db(DRIVER_FILE_ID,UPDATE_REC,&dr_key1,0,0) < 0) {	/* update DRIVER record */
		sprintf(error_str,"Error %d updating record: drv_id %d", iserrno, drv_id);
		ERROR(fleet,DRIVER_FILE,error_str);
		db(DRIVER_FILE_ID,RELEASE,&dr_key1,0,0);		/* unlock DRIVER record */
		return(-1);						/* if error, return */
	}

	if (db(DRIVER_FILE_ID,RELEASE,&dr_key1,0,0) < 0) 	{	/* unlock DRIVER record */
		sprintf(error_str,"Error %d releasing record: drv_id %d", iserrno,  drv_id);
		ERROR(fleet,DRIVER_FILE,error_str);
		return(-1);						/* if error, return */
	}

	vh_ptr->fleet = fleet;					/* fill in fleet for key */
	vh_ptr->nbr = veh_nbr;					/* fill in vehicle nubmer for key */

	if (db(VEHICLE_FILE_ID,READ_KEYS,&veh_key2,ISEQUAL+ISLOCK,0) < 0) {	/* read VEHICLE record */
		sprintf(error_str,"Error %d reading record: veh_nbr %d", iserrno, veh_nbr);
		ERROR(fleet,VEHICLE_FILE,error_str);
		db(VEHICLE_FILE_ID,RELEASE,&veh_key2,0,0);		/* unlock VEHICLE record */
		return(-1);						/* if error, return */
	}

	vh_ptr->last_signed_on = mads_time;
	vh_ptr->last_signed_off = 0;
	vh_ptr->last_driver = drv_id;
 	if (!warm_start) 
	{
/*		if this is a warm start do not reset the last zone posted.
		leave it the way it was.  This cuts down on the number of
		writes to the database and aids in determining the Q
		position of booked in drivers.
*/
		vh_ptr->status = WARM_STATUS_SIGNON;
		vh_ptr->last_zone_posted = 0;
		vh_ptr->last_time_posted = 0L;
	}

	if (db(VEHICLE_FILE_ID,UPDATE_REC,&veh_key2,0,0) < 0) {	/* update VEHICLE record */
		sprintf(error_str,"Error %d updating record: veh_nbr %d", iserrno,  veh_nbr);
		ERROR(fleet,VEHICLE_FILE,error_str);
		db(VEHICLE_FILE_ID,RELEASE,&veh_key2,0,0);		/* unlock VEHICLE record */
		return(-1);						/* if error, return */
	}

	if (db(VEHICLE_FILE_ID,RELEASE,&veh_key2,0,0) < 0)	{	/* unlock VEHICLE record */
		sprintf(error_str,"Error %d releasing record: veh_nbr %d", iserrno, veh_nbr);
		ERROR(fleet,VEHICLE_FILE,error_str);
		return(-1);						/* if error, return */
	}

	return(0);
}



/***********************************************************************************************************************************
writer_sign_off -
Called by DISPATCH when a driver signs off of a mobile terminal.  The DRIVER record is modified to show time, and the driver
statistics are set.  The VEHICLE record is also updated to show time and fill in statistics.
***********************************************************************************************************************************/
writer_sign_off(vd)
struct veh_driv *vd;
{
	struct cisam_dr *dr_ptr;
	struct cisam_vh *vh_ptr;

	dr_ptr = (struct cisam_dr *)req_buf;
	vh_ptr = (struct cisam_vh *)req_buf;
	get_asc_time(mads_time,cur_dt,cur_tm);			/* get current time */


	dr_ptr->fleet = vd->fleet;
	dr_ptr->id = vd->driver_id;

	if (db(DRIVER_FILE_ID,READ_KEYS,&dr_key1,ISEQUAL+ISLOCK,0) < 0) {	/* read DRIVER record */
		sprintf(error_str,"Error %d reading record: drv_id %d", iserrno, vd->driver_id);
		ERROR(vd->fleet,DRIVER_FILE,error_str);
		db(DRIVER_FILE_ID,RELEASE,&dr_key1,0,0);		/* unlock DRIVER record */
		return;
	}

	dr_ptr->signed_of_dt_tm = mads_time;
	dr_ptr->assigns = vd->assigns;
	dr_ptr->calls = vd->calls;
	dr_ptr->flags = vd->flags;
	dr_ptr->bids = vd->bids;
	dr_ptr->posts = vd->posts;
	dr_ptr->cond_posts = vd->cond_posts;
	dr_ptr->enroute_posts = vd->enroute_posts;
	dr_ptr->short_meters = vd->short_meters;
	dr_ptr->late_meters = vd->late_meters;
	dr_ptr->no_accepts = vd->no_accepts;
	dr_ptr->rejects = vd->rejects;
	dr_ptr->breaks = vd->breaks;
	dr_ptr->req_to_talk = vd->req_to_talk;
	dr_ptr->messages = vd->messages;
	dr_ptr->callbacks = vd->callbacks;
	dr_ptr->callouts = vd->callouts;
	dr_ptr->no_shows = vd->no_shows;

	if (db(DRIVER_FILE_ID,UPDATE_REC,&dr_key1,0,0) < 0) {	/* update DRIVER record */
		sprintf(error_str,"Error %d updating record: drv_id %d", iserrno, vd->driver_id);
		ERROR(vd->fleet,DRIVER_FILE,error_str);
		db(DRIVER_FILE_ID,RELEASE,&dr_key1,0,0);		/* unlock DRIVER record */
		return;
	}

	if (db(DRIVER_FILE_ID,RELEASE,&dr_key1,0,0) < 0) 	{	/* unlock DRIVER record */
		sprintf(error_str,"Error %d releasing record: drv_id %d", iserrno, vd->driver_id);
		ERROR(vd->fleet,DRIVER_FILE,error_str);
		return;
	}

	vh_ptr->fleet = vd->fleet;
	vh_ptr->nbr = vd->veh_nbr;


	if (db(VEHICLE_FILE_ID,READ_KEYS,&veh_key2,ISEQUAL+ISLOCK,0) < 0) {	/* read VEHICLE record */
		sprintf(error_str,"Error %d reading record: veh_nbr %d", iserrno, vd->veh_nbr);
		ERROR(vd->fleet,VEHICLE_FILE,error_str);
		db(VEHICLE_FILE_ID,RELEASE,&veh_key2,0,0);		/* unlock VEHICLE record */
		return;
	}

	vh_ptr->status = WARM_STATUS_SIGNOFF;
        vh_ptr->last_zone_posted = 0; 
        vh_ptr->last_time_posted = 0L; 
	vh_ptr->last_signed_off = mads_time;
	vh_ptr->assigns += vd->assigns;
	vh_ptr->calls += vd->calls;
	vh_ptr->flags += vd->flags;
	vh_ptr->bids += vd->bids;
	vh_ptr->posts += vd->posts;
	vh_ptr->cond_posts += vd->cond_posts;
	vh_ptr->enroute_posts += vd->enroute_posts;
	vh_ptr->short_meters += vd->short_meters;
	vh_ptr->late_meters += vd->late_meters;
	vh_ptr->no_accepts += vd->no_accepts;
	vh_ptr->reject += vd->rejects;
	vh_ptr->breaks += vd->breaks;
	vh_ptr->req_to_talk += vd->req_to_talk;
	vh_ptr->messages += vd->messages;
	vh_ptr->callbacks += vd->callbacks;
	vh_ptr->callouts += vd->callouts;
	vh_ptr->no_shows += vd->no_shows;

	if (db(VEHICLE_FILE_ID,UPDATE_REC,&veh_key2,0,0) < 0) {	/* update VEHICLE record */
		sprintf(error_str,"Error %d updating record: veh_nbr %d", iserrno, vd->veh_nbr);
		ERROR(vd->fleet,VEHICLE_FILE,error_str);
		db(VEHICLE_FILE_ID,RELEASE,&veh_key2,0,0);		/* unlock VEHICLE record */
		return;
	}

	if (db(VEHICLE_FILE_ID,RELEASE,&veh_key2,0,0) < 0)	{	/* unlock VEHICLE record */
		sprintf(error_str,"Error %d releasing record: veh_nbr %d", iserrno, vd->veh_nbr);
		ERROR(vd->fleet,VEHICLE_FILE,error_str);
		return;
	}

}

/***********************************************************************************************************************************
writer_acpt -
Called by DISPATCH when a driver accepts a call.  The CALL record is modified to show driver and vehicle information of who
accepted the call.
***********************************************************************************************************************************/
writer_acpt(veh_nbr,drv_id,call_nbr,call_cisam_nbr,by,enter_tm)
short veh_nbr;
int drv_id;
int call_nbr;
int call_cisam_nbr;
short by;
time_t enter_tm;
{
	struct cisam_cl *cl_ptr;
	int 	call_save;

#ifdef DEBUG
	if(offset->prattle >= LEVEL3){
		sprintf(trace_str,"writer_acpt : call = %d veh_num = %d cisam # = %d\n",call_nbr,veh_nbr,call_cisam_nbr);
		TRACE(trace_str);
	}
#endif

	cl_ptr = (struct cisam_cl *)req_buf;
	get_asc_time(mads_time,cur_dt,cur_tm);			/* get current time */

	cl_ptr->nbr = call_nbr;
	if (db(CALL_FILE_ID,READ_KEYS,&cl_key1,ISEQUAL+ISLOCK,0) < 0) {	/* read CALL record */
	  sprintf(error_str,"Error %d reading record: call_nbr %d",iserrno, call_nbr);
	  ERROR(' ',CALL_FILE,error_str);
	  db(CALL_FILE_ID,RELEASE,&cl_key1,0,0);			/* release CALL record */
	  return;
	}

	if ( cl_ptr->grouped_with == 1 )
	  cl_ptr->grouped_with = 0;	
	cl_ptr->dispd_date_time = mads_time;
	cl_ptr->dispatch_entry_tm = enter_tm;					/* time call entered dispatch */
	strncpy(cl_ptr->dispd_date,cur_dt,9);
	strncpy(cl_ptr->dispd_time,cur_tm,6);
	cl_ptr->drv_id = drv_id;
	cl_ptr->veh_nbr = veh_nbr;
	cl_ptr->dispatched_by = by;
	cl_ptr->pri_status = ASSIGNED_PRI;
	strcpy(cl_ptr->status,ASSIGNED); 

	if(db(CALL_FILE_ID,UPDATE_REC,&cl_key1,0,0) < 0) {		/* update CALL record */
		sprintf(error_str,"Error %d updating record: call_nbr %d", iserrno, call_nbr);
		ERROR(' ',CALL_FILE,error_str);
		db(CALL_FILE_ID,RELEASE,&cl_key1,0,0);			/* release CALL record */
		return;
	}
	
	if(db(CALL_FILE_ID,RELEASE,&cl_key1,0,0) < 0) 	{	/* unlock CALL record */
		sprintf(error_str,"Error %d releasing record: call_nbr = %d", iserrno, call_nbr);
		ERROR(' ',CALL_FILE,error_str);
		return;
	}

	call_save = cl_ptr->grouped_with;
	
	if ( call_save == 1 )
	  call_save = 0;

	while(call_save != 0) {
		cl_ptr->nbr = call_save;

		if(db(CALL_FILE_ID,READ_KEYS,&cl_key1,ISEQUAL+ISLOCK,0) < 0) {	/* read CALL record */
			sprintf(error_str,"Error %d reading record: call_nbr %d", iserrno, call_nbr);
			ERROR(' ',CALL_FILE,error_str);
			db(CALL_FILE_ID,RELEASE,&cl_key1,0,0);			/* release CALL record */
			return;
		}

		cl_ptr->dispd_date_time = mads_time;
		cl_ptr->dispatch_entry_tm = enter_tm;					/* time call entered dispatch */
		strncpy(cl_ptr->dispd_date,cur_dt,9);
		strncpy(cl_ptr->dispd_time,cur_tm,6);
		cl_ptr->drv_id = drv_id;
		cl_ptr->veh_nbr = veh_nbr;
		cl_ptr->dispatched_by = by;
		cl_ptr->pri_status = ASSIGNED_PRI;
		strcpy(cl_ptr->status,ASSIGNED); 

		if (db(CALL_FILE_ID,UPDATE_REC,&cl_key1,0,0) < 0) {		/* update CALL record */
			sprintf(error_str,"Error %d updating record: call_nbr = %d", iserrno,  call_nbr);
			ERROR(' ',CALL_FILE,error_str);
			db(CALL_FILE_ID,RELEASE,&cl_key1,0,0);			/* release CALL record */
			return;
		}

		if (db(CALL_FILE_ID,RELEASE,&cl_key1,0,0) < 0) 	{	/* unlock CALL record */
			sprintf(error_str,"Error %d releasing record: call_nbr %d", iserrno, call_nbr);
			ERROR(' ',CALL_FILE,error_str);
			return;
		}

		call_save = cl_ptr->grouped_with;
		if(call_save == call_nbr)
			call_save = 0;
	}
}

/***********************************************************************************************************************************
writer_pckup -
Called by DISPATCH when a driver picks up a passenger or the meter is turned on.  The CALL record is updated with the time of
the pickup.
***********************************************************************************************************************************/
writer_pckup(call_nbr,call_cisam_nbr)
int call_nbr;
int call_cisam_nbr;
{
	int 	call_save;
	struct cisam_cl *cl_ptr;

#ifdef DEBUG
	if(offset->prattle >= LEVEL3){
		sprintf(trace_str,"writer_pckup : call = %d cisam # = %d\n",call_nbr,call_cisam_nbr);
		TRACE(trace_str);
	}
#endif

	cl_ptr = (struct cisam_cl *)req_buf;
	get_asc_time(mads_time,cur_dt,cur_tm);			/* get current time */

	cl_ptr->nbr = call_nbr;
	if (db(CALL_FILE_ID,READ_KEYS,&cl_key1,ISEQUAL+ISLOCK,0) < 0) {	/* read CALL record */
	  sprintf(error_str,"Error %d reading record: call_nbr %d", iserrno, call_nbr);
	  ERROR(' ',CALL_FILE,error_str);
	  db(CALL_FILE_ID,RELEASE,&cl_key1,0,0);			/* release CALL record */
	  return;
	}

	if ( cl_ptr->grouped_with == 1 )
	  cl_ptr->grouped_with = 0;	
	cl_ptr->pckup_date_time = mads_time;					/* no, do normal update */
	strncpy(cl_ptr->pckup_date,cur_dt,9);
	strncpy(cl_ptr->pckup_time,cur_tm,6);
	if ( (strcmp( cl_ptr->status, NO_SHOW_ITM ) ) )
	  strcpy(cl_ptr->status,PICKUP);
	cl_ptr->pri_status = ASSIGNED_PRI;

	if(db(CALL_FILE_ID,UPDATE_REC,&cl_key1,0,0) < 0) {		/* update CALL record */
		sprintf(error_str,"Error %d updating record: call_nbr %d", iserrno, call_nbr);
		ERROR(' ',CALL_FILE,error_str);
		db(CALL_FILE_ID,RELEASE,&cl_key1,0,0);			/* release CALL record */
		return;
	}

	if(db(CALL_FILE_ID,RELEASE,&cl_key1,0,0) < 0) 	{	/* unlock CALL record */
		sprintf(error_str,"Error %d releasing record: call_nbr %d", iserrno, call_nbr);
		ERROR(' ',CALL_FILE,error_str);
		return;
	}

	call_save = cl_ptr->grouped_with;

	if ( call_save == 1 )
	  call_save = 0;

	while(call_save != 0) {
		cl_ptr->nbr = call_save;

		if(db(CALL_FILE_ID,READ_KEYS,&cl_key1,ISEQUAL+ISLOCK,0) < 0) {	/* read CALL record */
			sprintf(error_str,"Error %d reading record: call_nbr %d", iserrno, call_nbr);
			ERROR(' ',CALL_FILE,error_str);
			db(CALL_FILE_ID,RELEASE,&cl_key1,0,0);			/* release CALL record */
			return;
		}

		cl_ptr->pckup_date_time = mads_time;				/* no, do normal update */
		strncpy(cl_ptr->pckup_date,cur_dt,9);
		strncpy(cl_ptr->pckup_time,cur_tm,6);
		strcpy(cl_ptr->status,PICKUP);
		cl_ptr->pri_status = ASSIGNED_PRI;

		if(db(CALL_FILE_ID,UPDATE_REC,&cl_key1,0,0) < 0) {	/* update CALL record */
			sprintf(error_str,"Error %d updating record: call_nbr %d", iserrno, call_nbr);
			ERROR(' ',CALL_FILE,error_str);
			db(CALL_FILE_ID,RELEASE,&cl_key1,0,0);		/* release CALL record */
			return;
		}

		if(db(CALL_FILE_ID,RELEASE,&cl_key1,0,0) < 0) 	{		/* unlock CALL record */
			sprintf(error_str,"Error %d releasing record: call_nbr %d", iserrno, call_nbr);
			ERROR(' ',CALL_FILE,error_str);
			return;
		}

		call_save = cl_ptr->grouped_with;
		if(call_save == call_nbr)
			call_save = 0;
	}
}

/***********************************************************************************************************************************
writer_unassgn -
Called by DISPATCH when a driver picks up a passenger or the meter is turned on.  The CALL record is updated with the time of
the pickup.
***********************************************************************************************************************************/
writer_unassgn(call_nbr)
int call_nbr;
{
	int 	call_save;
	struct cisam_cl *cl_ptr;

#ifdef DEBUG
	if(offset->prattle >= LEVEL3){
		sprintf(trace_str,"writer_unassgn : call = %d \n",call_nbr);
		TRACE(trace_str);
	}
#endif

	cl_ptr = (struct cisam_cl *)req_buf;
	get_asc_time(mads_time,cur_dt,cur_tm);			/* get current time */

	cl_ptr->nbr = call_nbr;
	if (db(CALL_FILE_ID,READ_KEYS,&cl_key1,ISEQUAL+ISLOCK,0) < 0) {	/* read CALL record */
		sprintf(error_str,"Error %d reading record: call_nbr %d", iserrno, call_nbr);
		ERROR(' ',CALL_FILE,error_str);
		db(CALL_FILE_ID,RELEASE,&cl_key1,0,0);			/* release CALL record */
		return;
	}

	if ( cl_ptr->grouped_with == 1 )
	  cl_ptr->grouped_with = 0;
	
	cl_ptr->dispd_date_time = (long)0; 			/* Update the call records on disk and in memory */
	strcpy(cl_ptr->dispd_date,"");
	strcpy(cl_ptr->dispd_time,"");
	cl_ptr->pckup_date_time = (long)0;
	strcpy(cl_ptr->pckup_date,"");
	strcpy(cl_ptr->pckup_time,"");
	cl_ptr->close_date_time = (long)0;
	strcpy(cl_ptr->close_date,"");
	strcpy(cl_ptr->close_time,"");
	cl_ptr->veh_nbr = (short)0;
	cl_ptr->dispatched_by = (short)0;
	cl_ptr->drv_id = (short)0;
	strcpy(cl_ptr->status,UNASSGND);
	cl_ptr->pri_status = cl_ptr->priority;

	if(db(CALL_FILE_ID,UPDATE_REC,&cl_key1,0,0) < 0) {		/* update CALL record */
		sprintf(error_str,"Error %d updating record: call_nbr %d", iserrno, call_nbr);
		ERROR(' ',CALL_FILE,error_str);
		db(CALL_FILE_ID,RELEASE,&cl_key1,0,0);			/* release CALL record */
		return;
	}

	if(db(CALL_FILE_ID,RELEASE,&cl_key1,0,0) < 0) 	{	/* unlock CALL record */
		sprintf(error_str,"Error %d releasing record: call_nbr %d", iserrno, call_nbr);
		ERROR(' ',CALL_FILE,error_str);
		return;
	}

	call_save = cl_ptr->grouped_with;

	if ( call_save == 1 )
	  call_save = 0;

	while(call_save != 0) {
		cl_ptr->nbr = call_save;

		if(db(CALL_FILE_ID,READ_KEYS,&cl_key1,ISEQUAL+ISLOCK,0) < 0) {	/* read CALL record */
			sprintf(error_str,"Error %d releasing record: call_nbr %d", iserrno, call_nbr);
			ERROR(' ',CALL_FILE,error_str);
			db(CALL_FILE_ID,RELEASE,&cl_key1,0,0);			/* release CALL record */
			return;
		}

		cl_ptr->dispd_date_time = (long)0; 			/* Update the call records on disk and in memory */
		strcpy(cl_ptr->dispd_date,"");
		strcpy(cl_ptr->dispd_time,"");
		cl_ptr->pckup_date_time = (long)0;
		strcpy(cl_ptr->pckup_date,"");
		strcpy(cl_ptr->pckup_time,"");
		cl_ptr->close_date_time = (long)0;
		strcpy(cl_ptr->close_date,"");
		strcpy(cl_ptr->close_time,"");
		cl_ptr->veh_nbr = (short)0;
		cl_ptr->dispatched_by = (short)0;
		cl_ptr->drv_id = (short)0;
		strcpy(cl_ptr->status,UNASSGND);
		cl_ptr->pri_status = cl_ptr->priority;

		if(db(CALL_FILE_ID,UPDATE_REC,&cl_key1,0,0) < 0) {	/* update CALL record */
			sprintf(error_str,"Error %d updating record: call_nbr %d", iserrno, call_nbr);
			ERROR(' ',CALL_FILE,error_str);
			db(CALL_FILE_ID,RELEASE,&cl_key1,0,0);		/* release CALL record */
			return;
		}

		if (db(CALL_FILE_ID,RELEASE,&cl_key1,0,0) < 0) 	{		/* unlock CALL record */
			sprintf(error_str,"Error %d releasing record: call_nbr %d", iserrno, call_nbr);
			ERROR(' ',CALL_FILE,error_str);
			return;
		}

		call_save = cl_ptr->grouped_with;
		if(call_save == call_nbr)
			call_save = 0;
	}
}

writer_signon_cancel(call_nbr)
     int call_nbr;
{
  struct cisam_cl *cl_ptr;

  cl_ptr = (struct cisam_cl *)req_buf;
  get_asc_time(mads_time, cur_dt, cur_tm);
  
  cl_ptr->nbr = call_nbr;
  if (db(CALL_FILE_ID,READ_KEYS,&cl_key1,ISEQUAL+ISLOCK,0) < 0)
    {	/* read CALL record */
      sprintf(error_str,"Error %d reading record: call_nbr %d", iserrno, call_nbr);
      ERROR(' ',CALL_FILE,error_str);
      db(CALL_FILE_ID,RELEASE,&cl_key1,0,0);			/* release CALL record */
      return;
    }

  if ( cl_ptr->grouped_with == 1 )
    cl_ptr->grouped_with = 0;
  
  cl_ptr->close_date_time = mads_time;
  strncpy(cl_ptr->close_date,cur_dt,9);
  strncpy(cl_ptr->close_time,cur_tm,6);
  cl_ptr->fare_amount = 0;
  strcpy(cl_ptr->status, CALL_CANCEL_SIGNON);

  if (db(CALL_FILE_ID,UPDATE_REC,&cl_key1,0,0) < 0) {		/* update CALL record */
		sprintf(error_str,"Error %d updating record: call_nbr %d", iserrno, call_nbr);
		ERROR(' ',CALL_FILE,error_str);
		db(CALL_FILE_ID,RELEASE,&cl_key1,0,0);			/* release CALL record */
		return;
	}
}

/***********************************************************************************************************************************
writer_cancel -
Called by DISPATCH when a call gets cancelled.  Functionally the same as writer_close except for the string that is
put in the call record and no fare is given.
***********************************************************************************************************************************/
writer_cancel(call_nbr)
int call_nbr;
{
	struct cisam_cl *cl_ptr;

#ifdef DEBUG
	if(offset->prattle >= LEVEL3){
		sprintf(trace_str,"writer_cancel : call = %d \n",call_nbr);
		TRACE(trace_str);
	}
#endif

	cl_ptr = (struct cisam_cl *)req_buf;
	get_asc_time(mads_time,cur_dt,cur_tm);			/* get current time */

									/* don't do a rec_num read for close, ptr is NULL */
	cl_ptr->nbr = call_nbr;						/* stuff the search value */
	if (db(CALL_FILE_ID,READ_KEYS,&cl_key1,ISEQUAL+ISLOCK,0) < 0) {	/* read CALL record */
		sprintf(error_str,"Error %d reading record: call_nbr %d", iserrno, call_nbr);
		ERROR(' ',CALL_FILE,error_str);
		db(CALL_FILE_ID,RELEASE,&cl_key1,0,0);			/* release CALL record */
		return;
	}

	if ( cl_ptr->grouped_with == 1 )
	  cl_ptr->grouped_with = 0;

	cl_ptr->close_date_time = mads_time;
	strncpy(cl_ptr->close_date,cur_dt,9);
	strncpy(cl_ptr->close_time,cur_tm,6);
	cl_ptr->fare_amount = 0;
	strcpy(cl_ptr->status,CANCELD);

	if (db(CALL_FILE_ID,UPDATE_REC,&cl_key1,0,0) < 0) {		/* update CALL record */
		sprintf(error_str,"Error %d updating record: call_nbr %d", iserrno, call_nbr);
		ERROR(' ',CALL_FILE,error_str);
		db(CALL_FILE_ID,RELEASE,&cl_key1,0,0);			/* release CALL record */
		return;
	}

	if (db(CALL_FILE_ID,RELEASE,&cl_key1,0,0) < 0) 	{	/* unlock CALL record */
		sprintf(error_str,"Error %d releasing record: call_nbr %d", iserrno, call_nbr);
		ERROR(' ',CALL_FILE,error_str);
		return;
	}

}

writer_noshow_mdt(call_nbr)
int call_nbr;
{
	struct cisam_cl *cl_ptr;

	cl_ptr = (struct cisam_cl *)req_buf;
	get_asc_time(mads_time,cur_dt,cur_tm);			/* get current time */

									/* don't do a rec_num read for close, ptr is NULL */
	cl_ptr->nbr = call_nbr;						/* stuff the search value */
	if (db(CALL_FILE_ID,READ_KEYS,&cl_key1,ISEQUAL+ISLOCK,0) < 0) {	/* read CALL record */
		sprintf(error_str,"Error %d reading record: call_nbr %d", iserrno, call_nbr);
		ERROR(' ',CALL_FILE,error_str);
		db(CALL_FILE_ID,RELEASE,&cl_key1,0,0);			/* release CALL record */
		return;
	}

	if ( cl_ptr->grouped_with == 1 )
	  cl_ptr->grouped_with = 0;
	
	cl_ptr->close_date_time = mads_time;
	strncpy(cl_ptr->close_date,cur_dt,9);
	strncpy(cl_ptr->close_time,cur_tm,6);
	cl_ptr->fare_amount = 0;
	strcpy(cl_ptr->status,NO_SHOW_MDT);

	if (db(CALL_FILE_ID,UPDATE_REC,&cl_key1,0,0) < 0) {		/* update CALL record */
		sprintf(error_str,"Error %d updating record: call_nbr %d", iserrno, call_nbr);
		ERROR(' ',CALL_FILE,error_str);
		db(CALL_FILE_ID,RELEASE,&cl_key1,0,0);			/* release CALL record */
		return;
	}

	if (db(CALL_FILE_ID,RELEASE,&cl_key1,0,0) < 0) 	{	/* unlock CALL record */
		sprintf(error_str,"Error %d releasing record: call_nbr %d", iserrno, call_nbr);
		ERROR(' ',CALL_FILE,error_str);
		return;
	}

}

writer_noshow_itm(call_nbr)
int call_nbr;
{
	struct cisam_cl *cl_ptr;

	cl_ptr = (struct cisam_cl *)req_buf;
	get_asc_time(mads_time,cur_dt,cur_tm);			/* get current time */

									/* don't do a rec_num read for close, ptr is NULL */
	cl_ptr->nbr = call_nbr;						/* stuff the search value */
	if (db(CALL_FILE_ID,READ_KEYS,&cl_key1,ISEQUAL+ISLOCK,0) < 0) {	/* read CALL record */
		sprintf(error_str,"Error %d reading record: call_nbr %d", iserrno, call_nbr);
		ERROR(' ',CALL_FILE,error_str);
		db(CALL_FILE_ID,RELEASE,&cl_key1,0,0);			/* release CALL record */
		return;
	}

	if ( cl_ptr->grouped_with == 1 )
	  cl_ptr->grouped_with = 0;
	
	strcpy(cl_ptr->status, NO_SHOW_ITM);

	if (db(CALL_FILE_ID,UPDATE_REC,&cl_key1,0,0) < 0) {		/* update CALL record */
		sprintf(error_str,"Error %d updating record: call_nbr %d", iserrno, call_nbr);
		ERROR(' ',CALL_FILE,error_str);
		db(CALL_FILE_ID,RELEASE,&cl_key1,0,0);			/* release CALL record */
		return;
	}

	if (db(CALL_FILE_ID,RELEASE,&cl_key1,0,0) < 0) 	{	/* unlock CALL record */
		sprintf(error_str,"Error %d releasing record: call_nbr %d", iserrno, call_nbr);
		ERROR(' ',CALL_FILE,error_str);
		return;
	}

}

/***********************************************************************************************************************************
writer_close -
Called by DISPATCH when a driver drops a passenger off or the meter is turned off.  The CALL record is modified with the time of
the drop off.
***********************************************************************************************************************************/
writer_close(call_nbr,fare)
int call_nbr;
float fare;
{
	int 	call_save;
	struct cisam_cl *cl_ptr;

#ifdef DEBUG
	if(offset->prattle >= LEVEL3){
		sprintf(trace_str,"writer_close : call = %d \n",call_nbr);
		TRACE(trace_str);
	}
#endif

	cl_ptr = (struct cisam_cl *)req_buf;
	get_asc_time(mads_time,cur_dt,cur_tm);			/* get current time */

									/* don't do a rec_num read for close, ptr is NULL */
	cl_ptr->nbr = call_nbr;						/* stuff the search value */
	if (db(CALL_FILE_ID,READ_KEYS,&cl_key1,ISEQUAL+ISLOCK,0) < 0) {	/* read CALL record */
		sprintf(error_str,"Error %d reading record: call_nbr %d", iserrno, call_nbr);
		ERROR(' ',CALL_FILE,error_str);
		db(CALL_FILE_ID,RELEASE,&cl_key1,0,0);			/* release CALL record */
		return;
	}

	if ( cl_ptr->grouped_with == 1 )
	  cl_ptr->grouped_with = 0;
	
	cl_ptr->close_date_time = mads_time;
	strncpy(cl_ptr->close_date,cur_dt,9);
	strncpy(cl_ptr->close_time,cur_tm,6);
	cl_ptr->fare_amount = fare;
	if ( (strcmp( cl_ptr->status, NO_SHOW_ITM ) ) )
	  strcpy(cl_ptr->status,COMPLETE);
	cl_ptr->pri_status = ASSIGNED_PRI;

	if (db(CALL_FILE_ID,UPDATE_REC,&cl_key1,0,0) < 0) {		/* update CALL record */
		sprintf(error_str,"Error %d updating record: call_nbr %d", iserrno, call_nbr);
		ERROR(' ',CALL_FILE,error_str);
		db(CALL_FILE_ID,RELEASE,&cl_key1,0,0);			/* release CALL record */
		return;
	}

	if (db(CALL_FILE_ID,RELEASE,&cl_key1,0,0) < 0) 	{	/* unlock CALL record */
		sprintf(error_str,"Error %d releasing record: call_nbr %d", iserrno, call_nbr);
		ERROR(' ',CALL_FILE,error_str);
		return;
	}

	call_save = cl_ptr->grouped_with;


	if ( call_save == 1 )
	  call_save = 0;
	
	while(call_save != 0) {
		cl_ptr->nbr = call_save;

		if (db(CALL_FILE_ID,READ_KEYS,&cl_key1,ISEQUAL+ISLOCK,0) < 0) {	/* read CALL record */
			sprintf(error_str,"Error %d reading record: call_nbr %d", iserrno, call_nbr);
			ERROR(' ',CALL_FILE,error_str);
			db(CALL_FILE_ID,RELEASE,&cl_key1,0,0);			/* release CALL record */
			return;
		}

		cl_ptr->close_date_time = mads_time;
		strncpy(cl_ptr->close_date,cur_dt,9);
		strncpy(cl_ptr->close_time,cur_tm,6);
		cl_ptr->fare_amount = fare;
		strcpy(cl_ptr->status,COMPLETE);
		cl_ptr->pri_status = ASSIGNED_PRI;

		if (db(CALL_FILE_ID,UPDATE_REC,&cl_key1,0,0) < 0) {	/* update CALL record */
			sprintf(error_str,"Error %d updating record: call_nbr %d", iserrno, call_nbr);
			ERROR(' ',CALL_FILE,error_str);
			db(CALL_FILE_ID,RELEASE,&cl_key1,0,0);		/* release CALL record */
			return;
		}

		if (db(CALL_FILE_ID,RELEASE,&cl_key1,0,0) < 0) 	{		/* unlock CALL record */
			sprintf(error_str,"Error %d releasing record: call_nbr %d", iserrno, call_nbr);
			ERROR(' ',CALL_FILE,error_str);
			return;
		}

		call_save = cl_ptr->grouped_with;
		if(call_save == call_nbr)
			call_save = 0;
	}
}

/***********************************************************************************************************************************
writer_emer -
Called by DISPATCH when a driver presses the emergency (M) key.  It modifies the DRIVER and VEHICLE record with the time of the
emergency.  It adds a new EMERGENCY record number, and the function returns the Cisam record number that is created.
***********************************************************************************************************************************/
writer_emer(fleet,veh_nbr,drv_id,call_nbr,dt_tm,zone)
char fleet;
short veh_nbr;
int drv_id;
int call_nbr;
int dt_tm;
short zone;
{
	struct cisam_dr *dr_ptr;
	struct cisam_vh *vh_ptr;
	struct cisam_em *em_ptr;
	int x;

	dr_ptr = (struct cisam_dr *)req_buf;
	vh_ptr = (struct cisam_vh *)req_buf;
	em_ptr = (struct cisam_em *)req_buf;
	get_asc_time(dt_tm,cur_dt,cur_tm);			/* get current time */


	dr_ptr->fleet = fleet;
	dr_ptr->id = drv_id;

	//	if (db(DRIVER_FILE_ID,READ_KEYS,&dr_key1,ISEQUAL+ISLOCK,0) < 0) {	/* read DRIVER record */
	if (db(DRIVER_FILE_ID,READ_KEYS,&dr_key1,ISEQUAL,0) < 0) {
		sprintf(error_str,"Error %d reading record: drv_id %d", iserrno, drv_id);
		ERROR(fleet,DRIVER_FILE,error_str);
		//		db(DRIVER_FILE_ID,RELEASE,&dr_key1,0,0);		/* unlock DRIVER record */
	}

	dr_ptr->emerg_on_dt_tm = dt_tm;
	dr_ptr->emerg_of_dt_tm = 0;

	if (db(DRIVER_FILE_ID,UPDATE_REC,&dr_key1,0,0) < 0) {	/* update DRIVER record */
		sprintf(error_str,"Error %d updating record: drv_id %d", iserrno, drv_id);
		ERROR(fleet,DRIVER_FILE,error_str);
		//	db(DRIVER_FILE_ID,RELEASE,&dr_key1,0,0);		/* unlock DRIVER record */
	}

	//	if (db(DRIVER_FILE_ID,RELEASE,&dr_key1,0,0) < 0) 	{	/* unlock DRIVER record */
	//sprintf(error_str,"Error %d releasing record: drv_id %d", iserrno, drv_id);
	//ERROR(fleet,DRIVER_FILE,error_str);
	//}

	vh_ptr->fleet = fleet;
	vh_ptr->nbr = veh_nbr;

	//	if (db(VEHICLE_FILE_ID,READ_KEYS,&veh_key2,ISEQUAL+ISLOCK,0) < 0) {	/* read VEHICLE record */
	if (db(VEHICLE_FILE_ID,READ_KEYS,&veh_key2,ISEQUAL,0) < 0) {	/* read VEHICLE record */	
		sprintf(error_str,"Error %d reading record: veh_nbr %d", iserrno, veh_nbr);
		ERROR(fleet,VEHICLE_FILE,error_str);
		//		db(VEHICLE_FILE_ID,RELEASE,&veh_key2,0,0);		/* unlock VEHICLE record */
	}

	vh_ptr->last_emerg_on = dt_tm;
	vh_ptr->last_emerg_off = 0;

	if (db(VEHICLE_FILE_ID,UPDATE_REC,&veh_key2,0,0) < 0) {	/* update VEHICLE record */
		sprintf(error_str,"Error %d updating record: veh_nbr %d", iserrno, veh_nbr);
		ERROR(fleet,VEHICLE_FILE,error_str);
		//		db(VEHICLE_FILE_ID,RELEASE,&veh_key2,0,0);		/* unlock VEHICLE record */
	}

	//	if (db(VEHICLE_FILE_ID,RELEASE,&veh_key2,0,0) < 0)	{	/* unlock VEHICLE record */
	//		sprintf(error_str,"Error %d releasing record: veh_nbr %d", iserrno, veh_nbr);
	//		ERROR(fleet,VEHICLE_FILE,error_str);
	//	}

	if (db(EMERG_FILE_ID,OPEN,&em_key1,ISINOUT+ISMANULOCK,0) < 0) {	/* open EMERG file */
		sprintf(error_str,"Error %d opening file", iserrno);
		ERROR(' ',EMERG_FILE,error_str);
		db(EMERG_FILE_ID,CLOSE,&em_key1,0,0);			/* close EMERG file */
		return;
	}

	memset(em_ptr, 0, sizeof(struct cisam_em));
	em_ptr->fleet = fleet;
	em_ptr->pad = '\0';
	em_ptr->exc_nbr = EMERGENCY;
	em_ptr->call_nbr = call_nbr;
	em_ptr->date_time = dt_tm;
	strcpy(em_ptr->event_date,cur_dt);
	strcpy(em_ptr->event_time,cur_tm);
	strcpy(em_ptr->resolved_date,"0");
	strcpy(em_ptr->resolved_time,"0");
	em_ptr->veh_nbr = veh_nbr;
	em_ptr->drv_id = drv_id;
	em_ptr->zone_nbr = zone;

	if (db(EMERG_FILE_ID,ADD_REC,&em_key1,0,0) < 0) {		/* write EMERG record to file */
		sprintf(error_str,"Error %d writing record: call_nbr %d", iserrno, call_nbr);
		ERROR(fleet,EMERG_FILE,error_str);
		db(EMERG_FILE_ID,CLOSE,&em_key1,0,0);			/* close EMERG file */
		return;
	}

	if (db(EMERG_FILE_ID,CLOSE,&em_key1,0,0) < 0) {		/* close EMERG file */
		sprintf(error_str,"Error %d closing file", iserrno);
		ERROR(' ',EMERG_FILE,error_str);
		return;
	}

	return;
}

/******************************************************************************************************/
/* read_callh_rec: reads the call history record, using the arguments passed to build the key         */
/*	Returns - 0  if db() failed to read a rec						      */
/*		- 1  if record read								      */
/******************************************************************************************************/
int 
read_callh_rec(call_num,except_type)
int call_num;					/* call # of this exception */
short except_type;				/* type of exception */
{
	struct cisam_ch *ch_ptr;

	ch_ptr = (struct cisam_ch *)req_buf;
	ch_ptr->call_nbr = call_num;
	ch_ptr->exception_nbr = except_type;
	ch_ptr->status_dt_tm = 0;

	if(db(CALLH_FILE_ID,READ_KEYS,&ch_key1,ISGTEQ+ISLOCK, 0) < 0) {
		return(0);
	}

	while(((ch_ptr->call_nbr == call_num)  &&  (ch_ptr->action_dt_tm != 0)) ||
	       (!strncmp(ch_ptr->event_desc,"ACK",3))) /* Don't allow approve/disapprove of ACK events */
	  { 
		if(db(CALLH_FILE_ID,RELEASE,&ch_key1,ISEQUAL,0)<0)  	/* release the write lock */
			ERROR(' ',"", "<cannot release lock on call history rec>");

		if(db(CALLH_FILE_ID,READ_KEYS,&ch_key1,ISNEXT+ISLOCK, 0) < 0) { 	/* try to get the right record */
			ERROR(' ',"", "<call history record>");
			return(0);
		}
	}

	if(ch_ptr->call_nbr != call_num) 		/* got record with wrong call # */
		return(0);
	return(1);					/* desired record is saved in req_buf[]. worked */
}

/********************************************************************************/
/* update_callh_rec: updates the call history record when resolving exceptions  */
/********************************************************************************/
update_callh_rec(action_code,veh_nbr)
char 	*action_code;			/* action code desc for exception resolution code */
int 	veh_nbr;			/* veh # entered .. for manual vehicle assignment */
{
	struct cisam_ch *ch_ptr;
	char date_str[10];
	char time_str[10];

	ch_ptr = (struct cisam_ch *)req_buf;

	strcpy(ch_ptr->action_taken,action_code);
	ch_ptr->action_dt_tm = time(NULL);
	get_pkup_date(date_str);
	strcpy(ch_ptr->action_date,date_str);
	get_pkup_time(time_str);
	strcpy(ch_ptr->action_time,time_str);
	ch_ptr->action_veh_nbr = veh_nbr;
	ch_ptr->action_drv_id = 0;
	ch_ptr->action_user_id = (short)-1;

	if(db(CALLH_FILE_ID,UPDATE_REC,&ch_key1,ISEQUAL,0) < 0)  		/* write this record to disk now */
		ERROR(' ',"", "<call history record>");

	if(db(CALLH_FILE_ID,RELEASE,&ch_key1,ISEQUAL,0) < 0)  		/* release the write lock */
		ERROR(' ',"", "<cannot release lock on call history rec>");

	return(1); 				/* record updated */
}

RET_STATUS
writer_time_call(call_nbr)
   int   call_nbr;
{
   int   call_save;
   CALL_REC *cl_ptr;

#ifdef DEBUG
   if (offset->prattle >= LEVEL3)
   {
      sprintf(trace_str, "writer_time_call : call = %d \n", call_nbr);
      TRACE(trace_str);
   }
#endif

   cl_ptr = (CALL_REC *) req_buf;
   get_asc_time(mads_time, cur_dt, cur_tm);	/* get current time */

   cl_ptr->nbr = call_nbr;
   if (db(CALL_FILE_ID, READ_KEYS, &cl_key1, ISEQUAL + ISLOCK, 0) < 0)
   {				/* read CALL record */
      sprintf(error_str, "Error %d reading record: call_nbr %d", iserrno, call_nbr);
      ERROR(' ', CALL_FILE, error_str);
      db(CALL_FILE_ID, RELEASE, &cl_key1, 0, 0);	/* release CALL record */
      return (FAIL);
   }
   if ( cl_ptr->grouped_with == 1 )
     cl_ptr->grouped_with = 0;   
   cl_ptr->dispd_date_time = (long) 0;	/* Update the call records on disk
					 * and in memory */
   strcpy(cl_ptr->dispd_date, "");
   strcpy(cl_ptr->dispd_time, "");
   cl_ptr->pckup_date_time = (long) 0;
   strcpy(cl_ptr->pckup_date, "");
   strcpy(cl_ptr->pckup_time, "");
   cl_ptr->close_date_time = (long) 0;
   strcpy(cl_ptr->close_date, "");
   strcpy(cl_ptr->close_time, "");
   cl_ptr->veh_nbr = (short) 0;
   cl_ptr->dispatched_by = (short) 0;
   cl_ptr->drv_id = (short) 0;
   cl_ptr->extended_type[CALL_TIME_OFFSET] = TIMEC;
   strcpy(cl_ptr->status, PENDING);
   cl_ptr->pri_status = CALL_PENDING_PRI_STATUS;

   if (db(CALL_FILE_ID, UPDATE_REC, &cl_key1, 0, 0) < 0)
   {				/* update CALL record */
      sprintf(error_str, "Error %d updating record: call_nbr %d", iserrno, call_nbr);
      ERROR(' ', CALL_FILE, error_str);
      db(CALL_FILE_ID, RELEASE, &cl_key1, 0, 0);	/* release CALL record */
      return (FAIL);
   }
   if (db(CALL_FILE_ID, RELEASE, &cl_key1, 0, 0) < 0)
   {				/* unlock CALL record */
      sprintf(error_str, "Error %d releasing record: call_nbr %d", iserrno, call_nbr);
      ERROR(' ', CALL_FILE, error_str);
      return(FAIL);
   }
   call_save = cl_ptr->grouped_with;
   
   if ( call_save == 1 )
     call_save = 0;

   while (call_save != 0)
   {
      cl_ptr->nbr = call_save;

      if (db(CALL_FILE_ID, READ_KEYS, &cl_key1, ISEQUAL + ISLOCK, 0) < 0)
      {				/* read CALL record */
	 sprintf(error_str, "Error %d releasing record: call_nbr %d", iserrno, call_nbr);
	 ERROR(' ', CALL_FILE, error_str);
	 db(CALL_FILE_ID, RELEASE, &cl_key1, 0, 0);	/* release CALL record */
	 return(FAIL);
      }
      cl_ptr->dispd_date_time = (long) 0;	/* Update the call records on
						 * disk and in memory */
      strcpy(cl_ptr->dispd_date, "");
      strcpy(cl_ptr->dispd_time, "");
      cl_ptr->pckup_date_time = (long) 0;
      strcpy(cl_ptr->pckup_date, "");
      strcpy(cl_ptr->pckup_time, "");
      cl_ptr->close_date_time = (long) 0;
      strcpy(cl_ptr->close_date, "");
      strcpy(cl_ptr->close_time, "");
      cl_ptr->veh_nbr = (short) 0;
      cl_ptr->dispatched_by = (short) 0;
      cl_ptr->drv_id = (short) 0;
      strcpy(cl_ptr->status, PENDING);
      cl_ptr->pri_status = CALL_PENDING_PRI_STATUS;

      if (db(CALL_FILE_ID, UPDATE_REC, &cl_key1, 0, 0) < 0)
      {				/* update CALL record */
	 sprintf(error_str, "Error %d updating record: call_nbr %d", iserrno, call_nbr);
	 ERROR(' ', CALL_FILE, error_str);
	 db(CALL_FILE_ID, RELEASE, &cl_key1, 0, 0);	/* release CALL record */
	 return (FAIL);
      }
      if (db(CALL_FILE_ID, RELEASE, &cl_key1, 0, 0) < 0)
      {				/* unlock CALL record */
	 sprintf(error_str, "Error %d releasing record: call_nbr %d", iserrno, call_nbr);
	 ERROR(' ', CALL_FILE, error_str);
	 return (FAIL);
      }
      call_save = cl_ptr->grouped_with;
      if (call_save == call_nbr)
	 call_save = 0;
   }
   return (SUCCESS);
}				/* end writer_time_call */

writer_gps_state( veh_hndl, veh_gps_state )
     VEH_HNDL  veh_hndl;
     int       veh_gps_state;
{
  int       rc = 0;
  VEH       *veh_ptr;
  VEH_REC   veh_db;

  bzero( &veh_db, sizeof( VEH_REC ) );

  veh_ptr = ( VEH * ) veh_hndl;

  veh_db.fleet = (char) Veh_get_value( veh_hndl, VEH_FLEET );
  veh_db.nbr = (short) Veh_get_value( veh_hndl, VEH_NBR );


  if ( ( rc = db_read_key( VEHICLE_FILE_ID, &veh_db, &veh_key2, ISEQUAL ) ) == SUCCESS )
    {
      veh_db.gps_state = veh_gps_state;
      //      if ( veh_gps_state == VEH_GPS_STATE_OFF )
      //        {
      //          veh_db.gps_lat = 0.0;
      //          veh_db.gps_long = 0.0;
      //        }
      db_update( VEHICLE_FILE_ID, &veh_db );
    }
}


  

