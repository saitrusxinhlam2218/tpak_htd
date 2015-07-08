/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: ui_acad.c,v $
*  @(#)  $Revision: 1.7 $
*  @(#)  $Date: 2000/04/11 14:33:24 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/ui/ui_acad.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: ui_acad.c,v 1.7 2000/04/11 14:33:24 taxiadm Exp $";
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
extern struct keydesc	ACAD_key;
extern char req_buf[];
extern WINDOW *srch_win;
extern char *Spaces;

struct scr_flds acad_flds[] = {						/* piu fields, 1 for choice selection */
	{0,0,1,1},
	{A_SLCT_ROW, A_SLCT_COL, A_SLCT_SIZ, A_SLCT}
};

ACAD_REC  acad_choice[8];

GetAccountAddress( pAccount, pAcad )
     struct cisam_ac *pAccount;
     ACAD_REC *pAcad;
{
  int   rc = 0,choices, i,x;
  char acad_buf[READ_BUF_SIZE];
  int select = 0, select_made = 0;
  int rkey = 0;
  int should_clear = 0;
  WINDOW *wAlt;
  ACAD_REC  acad_rec;


    wAlt = srch_win;
  
  if ( pAccount == NULL || pAcad == NULL )
    return ( -1 );

  acad_rec.id.account_id.fleet = pAccount->fleet;
  strcpy( acad_rec.id.account_id.nbr, pAccount->nbr );
  acad_rec.id.index = 1;
  choices = 0;
  if ( db_read_key( ACAD_FILE_ID, &acad_rec, &ACAD_key, ISEQUAL ) != SUCCESS )
    {
      return( -1 );
    }
  else
    {
      memcpy( &acad_choice[ choices ], &acad_rec, sizeof( ACAD_REC ) );
      ++choices;
      while ( rc == SUCCESS )
	{
	  rc = db_read_key( ACAD_FILE_ID, &acad_rec, &ACAD_key, ISNEXT );
	  if ( strcmp( acad_rec.id.account_id.nbr, pAccount->nbr ) )
	    rc = FAIL;
	  
	  if ( rc == SUCCESS )
	    {
	      memcpy( &acad_choice[ choices ], &acad_rec, sizeof( ACAD_REC ) );
	      ++choices;
	    }
	}
    }
  /** Display ACAD choices **/
  alt_win_disp();
  prt_entry_areas( wAlt, acad_flds, A_MAX );
  acad_ch_disp( choices, acad_choice );
  wrefresh( wAlt );

  while ( select == 0 )
    {
      i = read_field( wAlt, acad_flds, A_SLCT, acad_buf, &rkey, &should_clear );
      if ( ( clear_err_flag ) && ( i != -1 ) )
	{
	  clear_error();
	  refresh();
	}
      switch ( i )
	{
	case 0:
	case '\n':
	  rev_to_norm( wAlt, acad_flds, A_SLCT );
	  if ( ( x = atoi( acad_buf ) ) < 1 )
	    {
	      select = 1;
	      break;
	    }
	  if ( ( ( x = atoi( acad_buf ) ) < 1 ) || ( x > choices ) )
	    {
	      prt_error( UI_NO_RECORD, catgets( UI_m_catd, UI_1, UI_ALTRN_2, "Account Address record not found" ) );
	    }
	  else
	    {
	      memcpy( pAcad, &acad_choice[ x - 1 ], sizeof( ACAD_REC ) );
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
  

  
AccountAddress_to_Account( pAccount, pAcad )
     struct cisam_ac *pAccount;
     ACAD_REC *pAcad;
{
  if ( pAccount == NULL || pAcad == NULL )
    return ( -1 );

  pAccount->str_nbr = pAcad->address.street_nbr;
  pAccount->str_suf = pAcad->address.street_nbr_suffix;
  strcpy( pAccount->pre_dir, pAcad->address.street_pre_dir );
  strcpy( pAccount->str_name, pAcad->address.street_name );
  strcpy( pAccount->str_type, pAcad->address.street_type );
  strcpy( pAccount->post_dir, pAcad->address.street_post_dir );
  strcpy( pAccount->apt_room, pAcad->address.apt );
  strcpy( pAccount->city, pAcad->address.city_name );
  strncpy( pAccount->zip, pAcad->address.zip, 5 );
  strncpy( pAccount->phone_area, pAcad->phone.phone_nbr, 3 );
  strncpy( pAccount->phone, &pAcad->phone.phone_nbr[3], 7 );
  strcpy( pAccount->phone_ext, pAcad->phone.phone_ext );
  
}


acad_ch_disp( count, altr_ch )
     int count;
     ACAD_REC  altr_ch[];
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
	if ( !( 0 ) )
	  {
	    if(row > 3){
	      row = row - 4;							/* second set of columns */
	      col = ALT_DATA_COL2;
	    }
	  }
	if (count > i) {							/* is this a valid choice? */
#ifdef NAMERICA
	  sprintf(av_buf,"%6d %2s %-20s %3s %2s   %3s",altr_ch[i].address.street_nbr,
		  altr_ch[i].address.street_pre_dir,altr_ch[i].address.street_name,altr_ch[i].address.street_type,
		  altr_ch[i].address.street_post_dir,altr_ch[i].address.city_name);
#else
	  if ( 0 )
	    sprintf(av_buf,"(%02d) %20s %6d    %3s",
		    altr_ch[i].id.index, altr_ch[i].address.street_name, altr_ch[i].address.street_nbr,
		    altr_ch[i].address.city_name);
	  else
	    sprintf(av_buf,"(%02d) %20s %6d   %3s",
		    altr_ch[i].id.index, altr_ch[i].address.street_name, altr_ch[i].address.street_nbr,
		    altr_ch[i].address.city_name);
#endif
	  
	}
	else								/* out of range of choices */
	  sprintf(av_buf,"");						/* clear segment */
	if (0)
	  mvwprintw(alt_win,row+1,col,"%-64.64s",av_buf);			/* print out choice */
	else
	  mvwprintw(srch_win,row+1,col,"%-62.62s",av_buf);			/* print out choice */
    }
}
