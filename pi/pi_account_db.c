static char sz__FILE__[]="@(#)ab_db.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  ab_db.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:32:04
 * @(#)  Last delta: 12/2/94 at 12:09:13
 * @(#)  SCCS File: /taxi/sccs/s.ab_db.c
 *                                                                           *
 * Copyright (c) 1990 - MDI Seattle                                          *
 *                                                                           *
 *****************************************************************************/

/*---------------------------------------------------------
 *	ab db.c
 *---------------------------------------------------------
 */

#include <sys/types.h>

/*
 *	taxi includes
 */
#include "mads_types.h"
#include "ex_key.h"
#include "df_maint.h"
#include "mads_isam.h"
#include "path.h"
#include "mad_error.h"

/*
 *	ab includes
 */
#include "pi_account_defines.h"

/*
 *	globals
 */

/*
 *	externs
 */
extern char					 req_buf[];
extern short				 debug_level;


/*
 *========> database access
 */

/*--------------------------------------------------------
 *==>	d0000 db access
 *--------------------------------------------------------
 */

d0000_db_access(file, action, key, constraint)
	int				 file,
					 action,
					 constraint;
	struct keydesc	*key;
{
	int				 rc;
	
	switch(action)
	  {
		case OPEN:
			rc = db(file, action, key, ISINOUT+ISMANULOCK, 0);
			break;

		case CLOSE:
			rc = db(file, action, key, ISEQUAL, 0);
			break;

		case READ_KEYS:
			rc = db(file, action, key, constraint, 0);
			break;

		case ADD_REC:
			rc = db(file, action, key, ISEQUAL, 0);
			break;

		case UPDATE_REC:
			rc = db(file, action, key, 0, 0);
			break;

		case RELEASE:
			rc = db(file, action, key, 0, 0);
			break;

		default:
			rc = -1;
	  }

	return(rc);
}


/*
 *========> data base
 */

/*--------------------------------------------------------
 *==>	d1000 open db files
 *--------------------------------------------------------
 */

d1000_open_db_files()
{


	if (d0000_db_access(ACCOUNT_FILE_ID, OPEN, &ac_key1, 0) < 0)
	  {
		ERROR(' ', "", "Error opening account file");
		return(FALSE);
	  }
	if (d0000_db_access(INT_FILE_ID, OPEN, &in_key1, 0) < 0)
	  {
		ERROR(' ', "", "Error opening intersection file");
		return(FALSE);
	  }
	return(TRUE);
}


/*--------------------------------------------------------
 *==>	d1100 close db files
 *--------------------------------------------------------
 */

d1100_close_db_files()
{

	d0000_db_access(ACCOUNT_FILE_ID, CLOSE, &ac_key1, 0);
	d0000_db_access(INT_FILE_ID, CLOSE, &in_key1, 0);

	return(TRUE);
}


/*--------------------------------------------------------
 *==>	d2000 get account
 *--------------------------------------------------------
 */

d2000_get_account(account_number, account, fleet)
	int				 account_number;
	struct cisam_ac	*account;
	char			 fleet;
{
	struct cisam_ac	*db_account;


	db_account = (struct cisam_ac *)req_buf;

	db_account->fleet = fleet;
	sprintf(db_account->nbr, "%04d", account_number);

	if (d0000_db_access(ACCOUNT_FILE_ID, READ_KEYS, &ac_key1, ISEQUAL) < 0)
	  {
		return(FALSE);
	  }

	memcpy(account, db_account, sizeof(struct cisam_ac));

	return(TRUE);
}


/*--------------------------------------------------------
 *==>	d3000 write call to db
 *--------------------------------------------------------
 */

d3000_write_call_to_db(call)
	struct cisam_cl	*call;
{

	if (!call)
		return(FALSE);

	if (DEBUG(AB_DBX_NO_DB_WRITE))
		return(TRUE);

	memcpy(req_buf, (char *)call, sizeof(struct cisam_cl));

	if (d0000_db_access(CALL_FILE_ID, ADD_REC, &cl_key1, ISEQUAL) < 0)
	  {
		ERROR(' ', "", "Error writing out to call file");
		return(FALSE);
	  }

	call->nbr = ((struct cisam_cl *)req_buf)->nbr;

	return((int)call->nbr);
}


/*--------------------------------------------------------
 *==>	d3100 write call history to db
 *--------------------------------------------------------
 */

d3100_write_call_history_to_db(call_history)
	struct cisam_ch	*call_history;
{

	if (!call_history)
		return(FALSE);

	if (DEBUG(AB_DBX_NO_DB_WRITE))
		return(TRUE);

	memcpy(req_buf, (char *)call_history, sizeof(struct cisam_ch));

	if (d0000_db_access(CALLH_FILE_ID, ADD_REC, &ch_key1, ISEQUAL) < 0)
	  {
		ERROR(' ', "", "Error writing out to call history file");
		return(FALSE);
	  }

	return((int)isrecnum);
}
