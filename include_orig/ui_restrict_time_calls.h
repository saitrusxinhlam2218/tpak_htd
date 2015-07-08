/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  ui_restrict_time_calls.h; Rel: 10.1.2.1; Get date: 4/20/95 at 11:50:20
* @(#)  Last delta: 12/2/94 at 18:27:37
* @(#)  SCCS File: /taxi/sccs/s.ui_restrict_time_calls.h
*                                                                           *
* Copyright (c) 1990 - MDI Seattle                                          *
*                                                                           *
*****************************************************************************/
#ident "@(#) head:ui_restrict_time/usr/users/brian/samplan/src/include	10.1.2.1"


#ifndef _UI_RESTRICT_TIME_CALLS_H
#define _UI_RESTRICT_TIME_CALLS_H

/*---------------------------------------------------------------
 *	ui restrict time calls.h
 *---------------------------------------------------------------
 */

#define UI_TCR_MAX_TIME_CALLS_NO_ENTRY 	-1
#define UI_TCR_MAX_TIME_SLOTS 			6
#define UI_TCR_REQUESTED_SLOT 			0
#define UI_TCR_TWENTY_MINUTES 			1200
#define UI_TCR_SLOT_NOT_AVAILABLE 		-1
#define UI_TCR_TIME_SIZE 				6
#define UI_TCR_DATE_SIZE 				10

#define UI_TCR_WIN_ROWS					5
#define UI_TCR_WIN_COLS					80
#define UI_TCR_SCREEN_ROW				18
#define UI_TCR_SCREEN_COLS				0

#define UI_TCR_WIN_COL_1				15
#define UI_TCR_WIN_COL_2				45

#define UI_TCR_FIELD_ROW				0
#define UI_TCR_FIELD_COL				73
#define UI_TCR_FIELD_CHARS				1
#define UI_TCR_FIELD_PIU				1

typedef struct alt_time
				  {
					char		time_string[UI_TCR_TIME_SIZE];
					char		date_string[UI_TCR_DATE_SIZE];
					time_t		date_time;
					short		count;
				  } TIMES;


#endif
