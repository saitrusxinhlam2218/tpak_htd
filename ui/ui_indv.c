/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: ui_indv.c,v $
*  @(#)  $Revision: 1.4 $
*  @(#)  $Date: 1998/05/12 19:18:11 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/ui/ui_indv.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: ui_indv.c,v 1.4 1998/05/12 19:18:11 taxiadm Exp $";
#include <stdio.h>
#include <cursesX.h>
#include <memory.h>
#include <sys/types.h>
#include <ctype.h>

#include "mads_types.h"
#include "mads_isam.h"
#include "call_entry.h"
#include "df_maint.h"
#include "func_keys.h"
#include "screens.h"
#include "ui_def.h"
#include "ui_error.h"
#include "user.h"
#include "zgeo.h"
#include "language.h"
#include "ui_strdefs.h"
#include "enhance.h"
#include "switch_ext.h"

#define GB  gbox();
extern int scr_name;
extern WINDOW *srch_win;						/* search window for displaying alternates */
extern WINDOW *alt_win;
extern short silent;
extern short clear_err_flag;
extern struct keydesc	indv_key1;
extern char req_buf[];
extern WINDOW *srch_win;
extern char *Spaces;

struct scr_flds indv_flds[] = {						/* piu fields, 1 for choice selection */
	{0,0,1,1},
	{A_SLCT_ROW, A_SLCT_COL, A_SLCT_SIZ, A_SLCT}
};

INDV_REC indv_choice[8];

GetIndv( pAccount, pIndv )
     struct cisam_ac *pAccount;
     INDV_REC        *pIndv;
{
  int   rc = 0,choices, i,x;
  char indv_buf[READ_BUF_SIZE];
  int select = 0, select_made = 0;
  int rkey = 0;
  int should_clear = 0;
  WINDOW *wAlt;
  INDV_REC  indv_rec;


    wAlt = srch_win;
  
  if ( pAccount == NULL || pIndv == NULL )
    return ( -1 );

  indv_rec.individual_id.acct_id.fleet = pAccount->fleet;
  strcpy( indv_rec.individual_id.acct_id.nbr, pAccount->nbr );

  choices = 0;
  if ( db_read_key( INDV_FILE_ID, &indv_rec, &indv_key1, ISEQUAL ) != SUCCESS )
    {
      return( -1 );
    }
  else
    {
      memcpy( &indv_choice[ choices ], &indv_rec, sizeof( INDV_REC ) );
      ++choices;
      while ( rc == SUCCESS )
	{
	  rc = db_read_key( INDV_FILE_ID, &indv_rec, &indv_key1, ISNEXT );
	  
	  if ( strcmp( indv_rec.individual_id.acct_id.nbr, pAccount->nbr ) )
	    rc = FAIL;
	  
	  if ( rc == SUCCESS )
	    {
	      memcpy( &indv_choice[ choices ], &indv_rec, sizeof( INDV_REC ) );
	      ++choices;
	    }
	}
    }
  alt_win_disp();
  prt_entry_areas( wAlt, indv_flds, A_MAX );
  indv_ch_disp( choices, indv_choice );
  wrefresh( wAlt );

    while ( select == 0 )
    {
      i = read_field( wAlt, indv_flds, A_SLCT, indv_buf, &rkey, &should_clear );
      if ( ( clear_err_flag ) && ( i != -1 ) )
	{
	  clear_error();
	  refresh();
	}
      switch ( i )
	{
	case 0:
	case '\n':
	  rev_to_norm( wAlt, indv_flds, A_SLCT );
	  if ( ( x = atoi( indv_buf ) ) < 1 )
	    {
	      select = 1;
	      break;
	    }
	  if ( ( ( x = atoi( indv_buf ) ) < 1 ) || ( x > choices ) )
	    {
	      prt_error( UI_NO_RECORD, catgets( UI_m_catd, UI_1, UI_ALTRN_2, "Individual record not found" ) );
	    }
	  else
	    {
	      memcpy( pIndv, &indv_choice[ x - 1 ], sizeof( INDV_REC ) );
	      select_made = 1;
	      select = 1;
	    }
	  break;
	default:
	  break;
	}
    }

  werase( wAlt );
  wrefresh( wAlt );
  touchwin( stdscr );
  refresh();
  
  if ( select_made == 1 )
    return( 1 );
  else
    return( -1 );
  
}

indv_ch_disp( count, altr_ch )
     int count;
     INDV_REC  altr_ch[];
{
    int i;									/* temporary variable */
    int row,col;								/* row and column to print data */
    char av_buf[132];								/* temporary buffer for formatting text */

    if (count == 0) {								/* is there at least on alternate choice */
	prt_error(UI_NO_RECORD,catgets(UI_m_catd, UI_1, UI_ALTRN_1, "<for alternates>"));				/* no, print error */
	return;
    }

    for(i=0; i<MAXALT; i++) {							/* print out all choices */
	row = i;								/* get row */
	col = ALT_DATA_COL1;
	if ( !(0) )
	  {
	    if(row > 3){
	      row = row - 4;							/* second set of columns */
	      col = ALT_DATA_COL2;
	    }
	  }
	if (count > i) {							/* is this a valid choice? */
	  sprintf(av_buf,"(%02s) %20s",
		  altr_ch[i].subacct_nbr, altr_ch[i].individual_id.name);
	}
	else								/* out of range of choices */
	  sprintf(av_buf,"");						/* clear segment */
	if (0)
	  mvwprintw(alt_win,row+1,col,"%-64.64s",av_buf);			/* print out choice */
	else
	  mvwprintw(srch_win,row+1,col,"%-62.62s",av_buf);			/* print out choice */
    }
}
  
