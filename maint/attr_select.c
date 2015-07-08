/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: attr_select.c,v $
*  @(#)  $Revision: 1.4 $
*  @(#)  $Date: 1998/01/13 13:54:33 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/maint/attr_select.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: attr_select.c,v 1.4 1998/01/13 13:54:33 taxiadm Exp $";

#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <isam.h>
#include <curses.h>
#include <signal.h>
#include <memory.h>

#include "vh_m_strdefs.h"
#include "mad_error.h"
#include "mads_types.h"
#include "mad_ipc.h"
#include "ui_ipc.h"
#include "df_maint.h"
#include "mads_isam.h"
#include "ex_key.h"
#include "vh_maint.h"
#include "literals.h"
#include "tokenize.h"
#include "path.h"
#include "language.h"
#include "fleet.h"
#include "func_keys.h"
#include "scr.h"

extern  struct fleets *fleet[];
extern short skip_return;
extern short silent;
extern int MaxAttrs;
WINDOW   *newwin(), *attr_win;
int     bAttrSelect = 0;

#define ATTR_SET_ROW 1
#define ATTR_SET_COL 2
#define ATTR_SET_CHARS 1
#define F_ATTR_SET1    1
#define F_ATTR_SET2     (F_ATTR_SET1 + 1)
#define F_ATTR_SET3     (F_ATTR_SET2 + 1)
#define F_ATTR_SET4     (F_ATTR_SET3 + 1)
#define F_ATTR_SET5     (F_ATTR_SET4 + 1)
#define F_ATTR_SET6     (F_ATTR_SET5 + 1)
#define F_ATTR_SET7     (F_ATTR_SET6 + 1)
#define F_ATTR_SET8     (F_ATTR_SET7 + 1)
#define F_ATTR_SET9     (F_ATTR_SET8 + 1)
#define F_ATTR_SET10    (F_ATTR_SET9 + 1)
#define F_ATTR_SET11    (F_ATTR_SET10 + 1)
#define F_ATTR_SET12    (F_ATTR_SET11 + 1)
#define F_ATTR_SET13    (F_ATTR_SET12 + 1)
#define F_ATTR_SET14    (F_ATTR_SET13 + 1)
#define F_ATTR_SET15    (F_ATTR_SET14 + 1)
#define F_ATTR_SET16    (F_ATTR_SET15 + 1)
#define MAX_ATTR_PIU    F_ATTR_SET16

#define FIRST_PAGE 0
#define FORWARD_PAGE 1
#define BACK_PAGE  2

short LastAttrNbr;

struct scr_flds attr_flds[] = {
  { 0, 0, 1},
  { ATTR_SET_ROW, ATTR_SET_COL, ATTR_SET_CHARS, 1 },
  { ATTR_SET_ROW + 1, ATTR_SET_COL, ATTR_SET_CHARS, 1 },
  { ATTR_SET_ROW + 2, ATTR_SET_COL, ATTR_SET_CHARS, 1 },
  { ATTR_SET_ROW + 3, ATTR_SET_COL, ATTR_SET_CHARS, 1 },
  { ATTR_SET_ROW + 4, ATTR_SET_COL, ATTR_SET_CHARS, 1 },
  { ATTR_SET_ROW + 5, ATTR_SET_COL, ATTR_SET_CHARS, 1 },
  { ATTR_SET_ROW + 6, ATTR_SET_COL, ATTR_SET_CHARS, 1 },
  { ATTR_SET_ROW + 7, ATTR_SET_COL, ATTR_SET_CHARS, 1 },
  { ATTR_SET_ROW + 8, ATTR_SET_COL, ATTR_SET_CHARS, 1 },
  { ATTR_SET_ROW + 9, ATTR_SET_COL, ATTR_SET_CHARS, 1 },
  { ATTR_SET_ROW + 10, ATTR_SET_COL, ATTR_SET_CHARS, 1 },
  { ATTR_SET_ROW + 11, ATTR_SET_COL, ATTR_SET_CHARS, 1 },
  { ATTR_SET_ROW + 12, ATTR_SET_COL, ATTR_SET_CHARS, 1 },
  { ATTR_SET_ROW + 13, ATTR_SET_COL, ATTR_SET_CHARS, 1 },
  { ATTR_SET_ROW + 14, ATTR_SET_COL, ATTR_SET_CHARS, 1 },
  { ATTR_SET_ROW + 15, ATTR_SET_COL, ATTR_SET_CHARS, 1 },
};

typedef struct _ATTR_SELECT_ARRAY
{
  short     AttrNbr;
  char      SetFlag;
} ATTR_SELECT_ARRAY;

ATTR_SELECT_ARRAY   AttrSel[ SCR_ATTR_MAX ];

int key_stroke, attr_piu;

int
attr_select( VehDrivFlag, FleetIndex, CurrentAttr )
     short VehDrivFlag;
     short FleetIndex;
     char  *CurrentAttr;
{
  char response_buffer[133];
  int      count = 1, i, field_entered = 0, clear_field = 0, choice;
  int      done = 0;




  for ( i = 0; i < SCR_ATTR_MAX; i++ )
    bzero( &AttrSel[i], sizeof( ATTR_SELECT_ARRAY ) );
  
  attr_start_alt_win();

  if ( !( getenv("UI_NO_BOXES" ) ) )
    box( attr_win, 0, 0  );
  wrefresh( attr_win );

  attr_display( FIRST_PAGE , FleetIndex, VehDrivFlag, CurrentAttr );


  /** Now get input from user **/
  done = 0;
  attr_piu = F_ATTR_SET1;
  while ( !done )
    {
      skip_return = 1;
      silent = 1;
      bAttrSelect = 1;
      key_stroke = read_field( attr_win, attr_flds,
			       attr_piu, response_buffer, &field_entered, &clear_field );
      bAttrSelect = 0;
      skip_return = 0;
      silent = 0;

      if ( field_entered )
	{
	  if ( toupper(response_buffer[0]) == YES )
	    {
	      wmove( attr_win, attr_flds[attr_piu].prompt_row, attr_flds[attr_piu].prompt_col );
	      waddch( attr_win, '*');
	      CurrentAttr[ AttrSel[attr_piu].AttrNbr ] = YES;
	    }
	  if ( toupper(response_buffer[0]) == NO )
	    {
	      wmove( attr_win, attr_flds[attr_piu].prompt_row, attr_flds[attr_piu].prompt_col );
	      waddch( attr_win, ' ');
	      CurrentAttr[ AttrSel[attr_piu].AttrNbr ] = NO;
	    }
	}
	  
      switch (key_stroke)
	{
	case '\n':
	case '\t':
	case KEY_DOWN:
	case KEY_UP:
	  attr_update_piu();
	  break;

	case NEXT_PG_KEY:
	  attr_display( FORWARD_PAGE, FleetIndex, VehDrivFlag, CurrentAttr );
	  break;
	case PREV_PG_KEY:
	  attr_display( BACK_PAGE, FleetIndex, VehDrivFlag, CurrentAttr );
	  break;
	case ALTERNATE_KEY:
	  done = 1;
	  break;
	default:
	  break;
	}
    }
  werase( attr_win );
  wrefresh( attr_win );
  touchwin( stdscr );
  refresh( );
  
}

attr_display( Direction, FleetIndex, VehDrivFlag, CurrentAttr )
     short Direction, FleetIndex, VehDrivFlag;
     char *CurrentAttr;
{

  int i;
  short    AttrRow, AttrCol, AttrNbr, NumDisplayed;

  AttrRow = 1;
  AttrCol = 5;

  attr_clear_display();
  
  NumDisplayed = 0;
  if ( Direction == FIRST_PAGE )
    AttrNbr = 0;
  else if ( Direction == FORWARD_PAGE )
    AttrNbr = LastAttrNbr + 1;
  else if ( Direction == BACK_PAGE )
    AttrNbr = 0;

  for ( i = AttrNbr; i < MaxAttrs; i++ )
    {
      switch ( VehDrivFlag )
	{
	case SELECT_DRIVER_ATTR:
	  wattrset( attr_win, A_REVERSE );
	  mvwprintw( attr_win, 0, 0, "        DRIVER ATTRIBUTES       " );
	  wattrset( attr_win, 0 );
	  if ( fleet[ FleetIndex ]->vd_attr_info[i].drv_attr[0] != '\0' )
	    {
	      LastAttrNbr = i;
	      ++NumDisplayed;
	      AttrSel[NumDisplayed].AttrNbr = i;
	      if ( CurrentAttr[i] == YES )
		{
		  AttrSel[NumDisplayed].SetFlag = YES;
		  wmove( attr_win, attr_flds[NumDisplayed].prompt_row, attr_flds[NumDisplayed].prompt_col );
		  waddch( attr_win, '*' );
		}
	      mvwprintw( attr_win, AttrRow, AttrCol, "%s %s",
			 fleet[ FleetIndex ]->vd_attr_info[i].drv_attr,
			 fleet[ FleetIndex ]->vd_attr_info[i].drv_at_desc );
	      wattrset( attr_win , 0 );
	      ++AttrRow;
	    }
	  break;
	case SELECT_VEHICLE_ATTR:
	  wattrset( attr_win, A_REVERSE );
	  mvwprintw( attr_win, 0, 0, "       VEHICLE ATTRIBUTES       " );
	  wattrset( attr_win, 0 );
	  if ( fleet[ FleetIndex ]->vd_attr_info[i].veh_attr[0] != '\0' )
	    {
	      LastAttrNbr = i;
	      ++NumDisplayed;
	      AttrSel[NumDisplayed].AttrNbr = i;
	      if ( CurrentAttr[i] == YES )
		{
		  AttrSel[NumDisplayed].SetFlag = YES;
		  wmove( attr_win, attr_flds[NumDisplayed].prompt_row, attr_flds[NumDisplayed].prompt_col );
		  waddch( attr_win, '*' );
		}

	      mvwprintw( attr_win, AttrRow, AttrCol, "%s %s",
			 fleet[ FleetIndex ]->vd_attr_info[i].veh_attr,
			 fleet[ FleetIndex ]->vd_attr_info[i].veh_at_desc );
	      wattrset( attr_win, 0 );

	      ++AttrRow;
	    }
	  break;
	default:
	  break;
	}
      if ( NumDisplayed >= SCR_ATTR_MAX )
	break;
    }
}

attr_clear_display()
{
  short i;
  
  for ( i = 0; i <= SCR_ATTR_MAX - 1; i++ )
    mvwprintw( attr_win, i + 1, 5, "                         " );

  for ( i = 0; i < SCR_ATTR_MAX; i++ )
    bzero( &AttrSel[i], sizeof( ATTR_SELECT_ARRAY ) );

  for ( i = 0; i <= MAX_ATTR_PIU; i++ )
    {
      wmove( attr_win, attr_flds[ i ].prompt_row, attr_flds[ i ].prompt_col );
      waddch( attr_win, ' ' );
    }
}

attr_start_alt_win()
{
  attr_win = newwin( ATTR_WIN_ROWS, ATTR_WIN_COLS,
		     ATTR_SCR_ROW, ATTR_SCR_COL );

  return ( TRUE );
}
  
attr_update_piu()
{
  if ( key_stroke == '\t' || key_stroke == '\n' || key_stroke == '\r' || key_stroke == KEY_DOWN )
    {
      if ( attr_piu == F_ATTR_SET16 )
	attr_piu = F_ATTR_SET1;
      else
	attr_piu++;
    }
  else if ( key_stroke == KEY_UP )
    {
      if ( attr_piu == F_ATTR_SET1 )
	attr_piu = F_ATTR_SET16;
      else
	attr_piu--;
    }
}
