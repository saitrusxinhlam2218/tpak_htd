static char sz__FILE__[]="@(#)ui_stat.c	10.1.2.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  ui_stat.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:51:46
 * @(#)  Last delta: 12/2/94 at 18:08:54
 * @(#)  SCCS File: /taxi/sccs/s.ui_stat.c
 *                                                                           *
 *****************************************************************************/

#include <cursesX.h>
#include <sys/types.h>
#include <isam.h>

#include "taxipak.h"
#include "Object.h"
#include "mad_ipc.h"
#include "df_maint.h"
#include "ui_def.h"
#include "ui_error.h"
#include "except.h"
#include "mads_types.h"
#include "mads_isam.h"
#include "calls.h"
#include "ui_ipc.h"
#include "ui_strdefs.h"
#include "language.h"

extern struct keydesc su_key1;
extern struct offsets *offset;	/* pointer to offsets structure in shared memory */
/* define structure for controlling updating of user statistics */
struct stat_control {
	short should_update_flag;	/* 0=do not write record to disk; 1= do write */
	struct cisam_su stat_rec;
};
	
struct stat_control statusr[FLEET_MAX];
long from_dt_tm;	/* time from which stats are recorded */
extern int uid;	/* user's unix id */
extern struct fleets *fleet[];
char get_user_type();
extern char req_buf[];
long offset_start_time;	/* time as in offsets struct in shared memory */
extern struct offsets *offset;
extern struct call_ipc_rec call_fifo;
extern struct cisam_cl call_struct;
extern int def_fl_index;	/* user's default fleet index */

/* ============================================================================================================================	*
 * ui_stat.c: contains functions to for recording the user statistics (call takers). This is maintained				*
 * in the statusr database file													*
 * ============================================================================================================================ */


/* ============================================================================================================================ *
 * is_stat_time: returns if its time to add another record to the statusr file. A record is to be added to the 			*
 * statusr file once every hour for each user that is logged in. Time is checked from the offsets structure in			*
 * shared memory, where it is placed by dispatch 										*
 * 	Returns:														*
 *		1 if its time to add a record											*
 *		0 if no need to add record											*
 * ============================================================================================================================	*/
is_stat_time()
{

	if ( offset_start_time != offset->stat_update )  {
	 	offset_start_time = offset->stat_update;
		return(1);
	}
	return(0);

}	/* end is_stat_time()   */

/* ============================================================================================================================	*
 * add_statusr_file: adds   record to the user statistics file
 * ============================================================================================================================ */
add_statusr_rec(statusr_ptr)
	struct stat_control *statusr_ptr;	/* pointer to struct to be written to disk */
{

	/* If statistics file not opened, do nothing */
	if (Db_files[STATUSR_FILE_ID].fd == -1)
	  return;

	memcpy(req_buf,(char *)&statusr_ptr->stat_rec,sizeof(struct cisam_su));
	if ( db(STATUSR_FILE_ID,ADD_REC,&su_key1,ISEQUAL,0) < 0 ) {
		prt_error(UI_WRITE,"<%s: %d>",  catgets(UI_m_catd, UI_1, UI_UI_STAT_1, "statusr file"), iserrno);
	}  /* end if */

}	/* end add_statusr_rec()  */

/*  ===========================================================================================================================	*
 * do_stats: takes care of processing the user statistics									*
 * ============================================================================================================================	*/
do_stats()
{
	
	/* is it time to write statusr records */
	if ( is_stat_time() ) 
		generate_statusr_recs();

}	/* end do_stats()  */

/*  ===========================================================================================================================	*
 * generate_statusr_recs: writes upto FLEET_MAX statusr records on disk for this user.
 * ============================================================================================================================	*/
 generate_statusr_recs()
 {
	int i;

	/* fill statusr rec with info */
	for ( i = 0; i < FLEET_MAX; i++ ) {
		if ( should_add_statusr_rec(&statusr[i], i) ) {
			fill_statusr_rec(&statusr[i]);
			add_statusr_rec(&statusr[i]);
		}  /* end if */
	}  /* end for */

	init_statusr_recs();

}	/* end generate_statusr_recs()  */


/*  ===========================================================================================================================	*
 * should_add_statusr_rec: checks is the argument pointer points to a statusr record which is to be written to disk. A record	*
 * should always be written for the users default fleet. For all other fleets, if the user has worked on them then only should	*
 * be written. 															*
 * 	Returns		1  if record should be written										*
 *			0  if record should not be written									*
 * ============================================================================================================================	*/
should_add_statusr_rec(statusr_ptr,indx)
	struct stat_control *statusr_ptr;	/* pointer to statusr record structure which might be written to disk */
	int indx;
{
	
	/* always write a record for the users default fleet; for other fleets write a record if the user has
	 * done some activity of those fleets 
	 */
	if ( indx == def_fl_index  || statusr_ptr->should_update_flag ) 
		return(1);
	return(0);
	
}	/* end should_add_statusr_rec()   */


/*  ===========================================================================================================================	*
 * fill_statusr_rec: fills info in the structure pointed to by the argument (the counts are already set), so only the time,	*
 * and date, etc fields need to be filled in 											*
 * ============================================================================================================================	*/
 fill_statusr_rec(statusr_ptr)
	struct stat_control *statusr_ptr;	/* pointer to statusr record structure which might be written to disk */
{
	
	long to_time;

	to_time = time(NULL);	/* time till when stat record contains data */
	statusr_ptr->stat_rec.to_dt_tm = (int)to_time;
	get_arg_date(to_time,statusr_ptr->stat_rec.to_date);
	get_arg_time(to_time,statusr_ptr->stat_rec.to_time);
	
}	/* end fill_statusr_rec()   */


/*  ===========================================================================================================================	*
 * init_statusr_rec: initializes the statusr records, so that they could be used for the next 1 hour	*
 * ============================================================================================================================	*/
 init_statusr_recs()
{
	
	int i;
	static int is_first_time = 1;

	/* If no shared mem, nothing we can do here */
	if (!offset)
	    return;

	if ( is_first_time ) {
		is_first_time = 0;
		offset_start_time = offset->stat_update;
	}


	from_dt_tm = time(NULL);	/* from time for stat rec  */
	for ( i = 0; i < FLEET_MAX; i++ )  {
		memset((char *)&statusr[i],'\0',sizeof(struct stat_control));
		statusr[i].stat_rec.fleet = fleet[i]->fleet_id;
		statusr[i].stat_rec.user_type = get_user_type();
		statusr[i].stat_rec.user_id = (short)uid;
		statusr[i].stat_rec.from_dt_tm = (int)from_dt_tm;
		get_arg_date(from_dt_tm,statusr[i].stat_rec.from_date);
		get_arg_time(from_dt_tm,statusr[i].stat_rec.from_time);
	}  /* end for */

}	/* end init_statusr_recs()   */


/*  ===========================================================================================================================	*
 * get_user_type: gets the type that this user belongs to; '1'=call taker; '2' = dispatcher; '3' = manager			*
 * ============================================================================================================================	*/
char get_user_type()
{
	short group;

	group = getgid();
	if(group == 500)
		return('3');
	if(group == 600)
		return('2');
	if(group == 700)
		return('1');

}	/* end get_user_type()  */

/*  ===========================================================================================================================	*
 * bump_stat: increments the count for one of the user statistics fields							*
 * ============================================================================================================================	*/
bump_stat(fleet_id,action)
	char fleet_id;	/* fleet to which this field belong, and needs to be incremented */
	char action;	/* what action is being performed by user; eg: ENTERING_CALL, CANCELLING_CALL, etc */
{
	int findex;	/* index of fleet */


	/* get fleet index */
	if ( (findex = find_findex(fleet_id)) == -1 )  
		/* invalid fleet id passed; do nothing */
		return;

	/* this fleets statrec should be written to disk on next stat update time */
	statusr[findex].should_update_flag = (short)1;

	/* check what action is taken */
	switch ( action )  {

		case ENTERING_CALL:	/* user is on call entry screen, entering a call by SEND, SEND_DUP or MULTIPLE KEY */
			/* check type of call, and bump the count accordingly */
			if ( call_struct.extended_type[0] == PASS )
				statusr[findex].stat_rec.pass_calls++;
			else if ( call_struct.extended_type[0] == DELIV )
				statusr[findex].stat_rec.del_calls++;
			else if ( call_struct.extended_type[0] == TYPEX )
				statusr[findex].stat_rec.typex_calls++;
			else if ( call_struct.extended_type[0] == WAKEUP )
				statusr[findex].stat_rec.wkup_calls++;
			else 
				/* invalid call type */
				return;
			/* if call is unzoned; then bump the unzoned calls count */
			if ( test_bit(UNZONED-1,sizeof(struct except_bits),(unsigned char *)&call_fifo.status) ) 
				statusr[findex].stat_rec.unzoned++;
			break;

		case KILLING_CALL:	/* on call entry screens */
			statusr[findex].stat_rec.killed++;
			break;

		case CANCELLING_CALL:	/* after call has been entered */
			statusr[findex].stat_rec.cancelled++;
			break;

		case CALLING_BACK:
			statusr[findex].stat_rec.callbacks++;
			break;
				
		case SENDING_MSG:
			statusr[findex].stat_rec.messages++;
			break;
			
		case RESOLVING_EXCEPTION:
			statusr[findex].stat_rec.exceptions++;
			break;
			
	}  /* end switch */


}	/* end bump_stat()  */
