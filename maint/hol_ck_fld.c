/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: hol_ck_fld.c,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1996/09/06 14:08:16 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/maint/hol_ck_fld.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: hol_ck_fld.c,v 1.2 1996/09/06 14:08:16 jwelch Exp $";

#include <pwd.h>
#include <cursesX.h>
#include <ctype.h>
#include <time.h>
#include <sys/types.h>

#include "comm_strdefs.h"
#include "holiday_per_m_strdefs.h"
#include "mad_error.h"
#include "mads_types.h"
#include "call_entry.h"
#include "mads_isam.h"
#include "df_maint.h"
#include "ex_key.h"
#include "holiday_per_maint.h"
#include "literals.h"
#include "path.h"
#include "tokenize.h"
#include "language.h"

#define	MAIL_COL	105			/* column # to print mail msg on */
#define	ZONE_DESC_CHARS	20			/* # of char's in zone_desc */
#define	MIN_DATE_CHARS	6  			/* min # of chars allowed for date entry */
#define	MIN_TIME_CHARS	4
#define MID_MAX		3839

extern short ipc_required;		/* Set if a variable which resides */
extern struct cisam_hl *pHoliday;		/* pointer to user profile file structure */
extern char read_buf[];			/* buffer for reading fields */
extern FILE *trace_fp;			/* trace file pointer */
extern char trace_str[];		/* trace string */
extern int piu;				/* prompt in use */
extern int key_val;			/* value of key entered */
extern int err_n_fld;			/* set if an error in field condition exists */
extern int doing_add;			/* performing add operation */
extern char *req_buf[];
extern prt_error();
extern clear_err_line();
extern dt_tm();
extern struct scr_flds hol_scr_flds[];
extern char zero_date[];

char *to_upper();
char a_or_p;				/* Last character in the time field indicating am or pm  */
char tmp_date[10], tmp_time[7], g_date_time[13];
long g_tmp_dt_tm;
struct tm *tm_ptr;

int update_piu();
int numeric();
long conversion();

int check_dummy(), check_descript();

int (*check_field[])() =
{ check_dummy, check_dummy,
    check_dummy, check_dummy,
    check_descript };

check_dummy( piu_ptr, read_buf, key_val, field_entered )
     int *piu_ptr;		/* pointer to the number of the field read in */
     char *read_buf; 	/* pointer to the buffer in which the field was read in */
     int key_val;    	/* value of the key pressed or entered */
     int *field_entered; 	/* indicates if data was entered or not */
{
  if ( *field_entered )
    {
      clear_err_line();
      switch( piu )
	{
	case F_YR:
	  pHoliday->year = atoi( read_buf );
	  break;
	case F_MONTH:
	  pHoliday->mon = atoi( read_buf );
	  break;
	case F_DAY:
	  pHoliday->day = atoi( read_buf );
	  break;
	default:
	  break;
	}
    }
  update_piu();
}

check_descript( piu_ptr, read_buf, key_val, field_entered )
     int *piu_ptr;
     char *read_buf;
     int key_val;
     int *field_entered;
{
  if ( *field_entered )
    {
      clear_err_line();
      err_n_fld = 0;
      strcpy( pHoliday->desc, read_buf);
      mvprintw( 5, D_DESCRIPT_COL, "%s", pHoliday->desc );
      refresh();
      update_piu();
      return(VALID);
    }
  else
    {
      if (!err_n_fld)
	update_piu();
      return (VALID);
    }
}

update_piu()
{
 int orig_piu;
 int max_piu = NUM_FLDS;


  orig_piu = piu;

  if(key_val == '\t' ||
	   key_val == '\n' ||
	   key_val == '\r' ||
	   key_val == KEY_DOWN ||
	   key_val == VALID)
    {
      ++piu;
      if ( doing_add && piu > NUM_FLDS )
	piu = 1;
      else if ( piu > NUM_FLDS )
	piu = 1;
    }

  if( key_val == KEY_UP )
    {
      --piu;
      if ( doing_add && piu < 1 )
	  piu = NUM_FLDS;
      else if ( piu < 1 )
	    piu = NUM_FLDS;
    }
  
  return;

} /* End of update_piu()  */
