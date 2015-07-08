/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: res_per_ck_fld.c,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1996/09/06 14:08:46 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/maint/res_per_ck_fld.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: res_per_ck_fld.c,v 1.2 1996/09/06 14:08:46 jwelch Exp $";

#include <pwd.h>
#include <cursesX.h>
#include <ctype.h>
#include <time.h>
#include <math.h>
#include <sys/types.h>

#include "comm_strdefs.h"
#include "res_per_m_strdefs.h"
#include "mad_error.h"
#include "mads_types.h"
#include "call_entry.h"
#include "mads_isam.h"
#include "df_maint.h"
#include "ex_key.h"
#include "res_per_maint.h"
#include "literals.h"
#include "path.h"
#include "tokenize.h"
#include "language.h"

#define	MAIL_COL	105			/* column # to print mail msg on */

extern short ipc_required;		/* Set if a variable which resides */
extern RP_REC *pResPer;		/* pointer to user profile file structure */
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
extern struct scr_flds res_per_scr_flds[];
extern char zero_date[];

char *to_upper();
char a_or_p;				/* Last character in the time field indicating am or pm  */
char tmp_date[10], tmp_time[7], g_date_time[13];
long g_tmp_dt_tm;
struct tm *tm_ptr;

int update_piu();
int numeric();
long conversion();

int check_dummy(), check_start_end(), check_days(), check_factor();
int (*check_field[])() =
{ check_dummy, check_dummy,
  check_start_end, check_start_end, check_days,
  check_factor };

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
	case F_PERIOD:
	  break;
	default:
	  break;
	}
    }
  update_piu();
}

check_start_end( piu_ptr, read_buf, key_val, field_entered )
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
	case F_START:
	  pResPer->period_definition.start_time = atoi( read_buf );
	  break;
	case F_END:
	  pResPer->period_definition.end_time = atoi( read_buf );
	  break;
	default:
	  break;
	}
    }
  update_piu();
}

check_days( piu_ptr, read_buf, key_val, field_entered )
     int *piu_ptr;		/* pointer to the number of the field read in */
     char *read_buf; 	/* pointer to the buffer in which the field was read in */
     int key_val;    	/* value of the key pressed or entered */
     int *field_entered; 	/* indicates if data was entered or not */
{
  int      i;
  
  if ( *field_entered )
    {
      clear_err_line();
      switch( piu )
	{
	case F_DAYS:
	  for ( i = 0; i < 8; i++ )
	    {
	      if ( read_buf[i] == '0' || read_buf[i] == '1' )
		continue;
	      else
		{
		  prt_error( catgets( RES_PER_MAINT_m_catd, 1, RES_PER_MAINT_15, "Entries must be either '0' or '1'"));
		  return( NOT_VALID );
		}
	    }
	  strcpy( pResPer->days_of_week, read_buf );
	  break;
	default:
	  break;
	}
    }
  update_piu();
}

check_factor( piu_ptr, read_buf, key_val, field_entered )
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
	case F_FACTOR:
	  pResPer->period_definition.lead_time_skew = (float) atof( read_buf );
	  break;
	default:
	  break;
	}
    }
  update_piu();
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
