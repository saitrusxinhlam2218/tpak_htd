/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: ui_emerg_bcst.c,v $
*  @(#)  $Revision: 1.5 $
*  @(#)  $Date: 2004/02/02 18:50:12 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/ui/ui_emerg_bcst.c,v $
*
*  Copyright (c) 2000 - MobileSoft Consulting, Inc. Dublin OH
*
***********************************************************************/

static char rcsid[] = "$Id: ui_emerg_bcst.c,v 1.5 2004/02/02 18:50:12 jwelch Exp $";

/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: ui_emerg_bcst.c,v $
*  @(#)  $Revision: 1.5 $
*  @(#)  $Date: 2004/02/02 18:50:12 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/ui/ui_emerg_bcst.c,v $
*
*  Copyright (c) 2001 - MobileSoft Consulting, Inc. Dublin OH
*
***********************************************************************/

#include <cursesX.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include "taxipak.h"
#include "mem_mgmt.h"
#include "taxi_defs.h"
#include "mads_types.h"
#include "Object.h"
#include "Fleet_public.h"
#include "Fleet_private.h"
#include "Call_public.h"
#include "Call_private.h"
#include "Scrn.h"
#include "screens.h"
#include "call_entry.h"
#include "language.h"
#include "func_keys.h"
#include "mads_types.h"
#include "mad_error.h"
#include "ui_ipc.h"
#include "ui_strdefs.h"
#include "ui_error.h"
#include "writer.h"
#include "taxi_db.h"
#include "except.h"
#include "tokenize.h"
#include "ui_def.h"
#include "path.h"
#include "enhance.h"
#include "Exception_private.h"


extern struct fleets *fleet[];
extern int      cur_fl_index;
extern int    scr_name;
extern int pid;
extern int msgkey;

RET_STATUS emerg_scr_background();
RET_STATUS emerg_scr_init();
RET_STATUS emerg_scr_pre_key();
RET_STATUS emerg_scr_post_key();
RET_STATUS emerg_scr_exit();
RET_STATUS function_key_basics();
RET_STATUS emerg_scr_active_verify(SCRN *, char *, int, BOOLEAN);
RET_STATUS emerg_scr_radius_verify(SCRN *, char *, int, BOOLEAN);

EXCEPT_EMERG   EmergBcast;
int        EmergNbrTaxis = 0;

#define EMERG_ACTIVE_COL 12
#define EMERG_ACTIVE_ROW 2
#define EMERG_RADIUS_COL 12
#define EMERG_RADIUS_ROW 3
#define EMERG_NBR_TAXIS_ROW 3
#define EMERG_NBR_TAXIS_COL 44

#define F_EMERG_ACTIVE 1
#define F_EMERG_RADIUS 2
#define F_EMERG_NBR_TAXIS 3
#define F_EMERG_FIELDS_MAX F_EMERG_NBR_TAXIS

#define F_EMERG_ACTIVE_LEN 1
#define F_EMERG_RADIUS_LEN 6
#define F_EMERG_NBR_TAXIS_LEN 4

FIELD Emerg_scr_fields[] =
{
  { F_EMERG_ACTIVE, EMERG_ACTIVE_ROW, EMERG_ACTIVE_COL, F_EMERG_ACTIVE_LEN, UPPER|EXIT_AT_END, "%c", CHAR_TYPE, LEFT, ENTRY, 0, 0, "F_EMERG_ACTIVE", &EmergBcast.active, (RET_STATUS (*)())emerg_scr_active_verify},
  { F_EMERG_RADIUS, EMERG_RADIUS_ROW, EMERG_RADIUS_COL, F_EMERG_RADIUS_LEN, UPPER|EXIT_AT_END, "%d", INT_TYPE, LEFT, ENTRY, 0, 0, "F_EMERG_RADIUS", &EmergBcast.radius, (RET_STATUS (*)())emerg_scr_radius_verify},
  { F_EMERG_NBR_TAXIS, EMERG_NBR_TAXIS_ROW, EMERG_NBR_TAXIS_COL, F_EMERG_NBR_TAXIS_LEN, UPPER|NO_UNDERLINE, "%d", INT_TYPE, LEFT, DISPLAY_ONLY, 0, 0, "F_EMERG_NBR_TAXIS", &EmergNbrTaxis, NULL },
#include "nu_end_field.h"
};

#define EMERG_BCST_SCR  100

SCRN Emerg_entry =
{
  F_EMERG_FIELDS_MAX, 1, 0, EMERG_BCST_SCR, NULL,
  emerg_scr_background, emerg_scr_init, emerg_scr_pre_key,
  emerg_scr_post_key, emerg_scr_exit,
  Emerg_scr_fields, &EmergBcast, NULL
};

#define EMERG_WIN_ROWS  5
#define EMERG_WIN_COLS  COLS
#define EMERG_SCR_ROW   18
#define EMERG_SCR_COL   0

alt_emerg( pEmergExcept )
  EXCEPT_EMERG *pEmergExcept;
{

  Emerg_entry.window = newwin( EMERG_WIN_ROWS, EMERG_WIN_COLS, EMERG_SCR_ROW, EMERG_SCR_COL );
  memcpy( &EmergBcast, pEmergExcept, sizeof( EXCEPT_EMERG ) );
  Scrn_process( &Emerg_entry );

  memcpy( pEmergExcept, &EmergBcast, sizeof( EXCEPT_EMERG ) );
  
  return( SUCCESS );
}

RET_STATUS
emerg_scr_init( SCRN *screen )
{
  box( screen->window, 0, 0 );
  
  Scrn_default_field_use_all( screen );  
  wattrset( screen->window, 0 );
  mvwprintw( screen->window, EMERG_ACTIVE_ROW - 1, 2, "-Modify Broadcast Settings-" );
  mvwprintw( screen->window, EMERG_ACTIVE_ROW, 1, "Active:" );
  mvwprintw( screen->window, EMERG_RADIUS_ROW, 1, "Radius:" );
  mvwprintw( screen->window, EMERG_RADIUS_ROW, 22, "Taxis within radius:" );
  EmergCalcNbrTaxis( &EmergBcast, &EmergNbrTaxis );
  Scrn_init_char_field( screen, F_EMERG_ACTIVE, EmergBcast.active );
  Scrn_init_int_field( screen, F_EMERG_RADIUS, EmergBcast.radius );
  Scrn_init_int_field( screen, F_EMERG_NBR_TAXIS, EmergNbrTaxis );
  Scrn_set_current_field_id( screen, F_EMERG_ACTIVE );
  wrefresh( screen->window );

  return(SUCCESS);
}


RET_STATUS
emerg_scr_background( SCRN *screen )
{
  return(SUCCESS);
}

RET_STATUS
emerg_scr_pre_key( SCRN *screen, int key_entered, BOOLEAN *done_with_screen )
{
  RET_STATUS rs;
  EXCEPT_EMERG *pExceptEmerg;

  pExceptEmerg = (EXCEPT_EMERG *)screen->record;
  
  switch (key_entered)
    {
      //
    default:
      rs = function_key_basics( key_entered, screen );
      if ( rs != SUCCESS )
        return ( FAIL );
      break;
    }
}

RET_STATUS
emerg_scr_post_key( SCRN *screen, int key_entered, BOOLEAN *done_with_screen )
{
  RET_STATUS rs;
  EXCEPT_EMERG *pExceptEmerg;

  pExceptEmerg = (EXCEPT_EMERG *)screen->record;

  switch ( key_entered ) {
  case UPDATE_KEY:
    *done_with_screen = TRUE;
    return(SUCCESS);
  }
}

RET_STATUS
emerg_scr_exit( SCRN *screen, int key_entered, BOOLEAN *exit_screen )
{

  EXCEPT_EMERG *pExceptEmerg;

  pExceptEmerg = (EXCEPT_EMERG *)screen->record;

  werase( screen->window );
  touchwin( stdscr );
  *exit_screen = TRUE;
  return(SUCCESS);
}

RET_STATUS
emerg_scr_active_verify( SCRN *screen,
                 char *read_buf,
                 int key_entered,
                 BOOLEAN field_changed )
{
  EXCEPT_EMERG *pExceptEmerg;

  pExceptEmerg = ( EXCEPT_EMERG * )screen->record;

  switch ( read_buf[0] )
    {
    case 'J':
    case 'j':
    case 'K':
    case 'k':
      pExceptEmerg->active = YES;
      break;
    case 'N':
    case 'n':
    case 'E':
    case 'e':
      pExceptEmerg->active = NO;
      break;
    default:
      break;
    }

  Scrn_update_field_num( screen, key_entered );
  return(SUCCESS);
}

RET_STATUS
emerg_scr_radius_verify( SCRN *screen,
                 char *read_buf,
                 int key_entered,
                 BOOLEAN field_changed )
{
  EXCEPT_EMERG *pExceptEmerg;

  pExceptEmerg = ( EXCEPT_EMERG * )screen->record;

  if ( atoi( read_buf ) > 32767 )
    pExceptEmerg->radius = 32767;
  else
    pExceptEmerg->radius = atoi( read_buf );

  EmergCalcNbrTaxis( pExceptEmerg, &EmergNbrTaxis );
  Scrn_print_field_by_num( screen, F_EMERG_NBR_TAXIS-1 );
  wrefresh( screen->window );  
  Scrn_update_field_num( screen, key_entered );
  
  return(SUCCESS);
}

EmergCalcNbrTaxis( EXCEPT_EMERG *pEmergBcast, short *nbr_taxis )
{
  EMERG_BCAST_RQST   bcast_rqst;
  EMERG_BCAST_RESP   *bcast_resp;
  EMERG_BCAST_HNDL   hEmergBcast;
  int  rc, times_thru;
  char error_str[80];
  
  if ( pEmergBcast->veh_nbr > 0 )
    {
      bcast_rqst.d_prcs_id = DISP_DEST;
      bcast_rqst.u_prcs_id = pid;
      bcast_rqst.rec_type = EMERG_BCAST_REQ;
      bcast_rqst.fleet = pEmergBcast->fleet;
      bcast_rqst.veh_nbr = pEmergBcast->veh_nbr;
      bcast_rqst.radius = pEmergBcast->radius;

      msgsnd( msgkey,
	       (struct msgbuf *)&bcast_rqst,sizeof(EMERG_BCAST_RQST), IPC_NOWAIT);
      hEmergBcast = TMALLOC( EMERG_BCAST_RESP, 1 );
      bcast_resp = ( EMERG_BCAST_RESP * )hEmergBcast;

      times_thru = 0;
      while ( times_thru <= 2 )
        {
          rc = msgrcv( msgkey,
                       (void *)bcast_resp, sizeof(EMERG_BCAST_RESP), (long)pid, 0);

          if ( rc == -1 )
            {
              if (errno != EINTR) 
                {
                  prt_error(UI_FATAL, "%s", 
                            catgets(UI_m_catd, UI_1, UI_UI_SSTAT_74, 
                                    "Error reading from dispatch in msgrcv"));
                  sprintf(error_str,
                          "Error %d reading ipc message from DISPATCH", errno);
                  ERROR(' ',"User-IPC", error_str);
                  TFREE( hEmergBcast );
                  return(0);
                }
              if (++times_thru == 2) 
                {
                  prt_error(UI_SYSTEM_DOWN, "");
                  ERROR(' ',"User-IPC", 
                        "Timeout reading ipc message from DISPATCH");
                  TFREE( hEmergBcast );
                  return(0);
                }
            }
          else 
            if (bcast_resp->rec_type == EMERG_BCAST_REQ)
              break;
            else 
              {
                prt_error(UI_FATAL, "%s", 
                          catgets(UI_m_catd, UI_1, UI_UI_SSTAT_74, 
                                  "Error reading from dispatch in msgrcv"));
                ERROR(' ',"User-IPC", "Invalid msg from dispatch DISPATCH");
                TFREE( hEmergBcast );
                return(0);
              }
        }
      *nbr_taxis = bcast_resp->nbr_taxis;
      TFREE( hEmergBcast );
    }
  else
    *nbr_taxis = -99;
  
}

