 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  ui_def.h; Rel: 10.1.2.1; Get date: 4/20/95 at 11:50:09
 * @(#)  Last delta: 12/2/94 at 18:33:27
 * @(#)  SCCS File: /taxi/sccs/s.ui_def.h
 *                                                                           *
 *****************************************************************************/
/* ui_def.h: 	definitions to be used in the user interface program */
#define		PADDING_SHMEM	100000		/* allow space b/w end of data segment and shared memory start address */
#define		MAX_MENU_LIST	290		/* max # which a scr_name can have */
#define         MAX_CANM        999
#define		BLOCK_TIME	50		/* 10th of sec to block on reads */
#define		ALARM_TIME	5		/* secs .. parameter for alarm() system call */


#define		CURSOR_ROW_MENU	LINES-3		/* row where cursor is on all menus */
#define		CURSOR_COL_MENU	44		/* col where cursor is on all menus */
#define		PROJ_TITLE_COL	37		/* col to print project title on */
#define		UID_COL		124		/* col to print uid on */
#define		MENU_TITLE_COL	48		/* col to print menu title on */
#define		MENU_TITLE_ROW	1		/* row to print menu/screen title on */
#define		MAX_MENU_ITEMS	9		/* max # of items taht can appear on a menu */
#define		BUF_ITEM_DESC	MAX_MENU_ITEMS*41	/* pointers to item descs */
#define		MENU_1ST_COL	4		/* col to print item desc on menus */
#define		MENU_2ND_COL	42		/* col to print item desc on menus */
#define		ROW_1ST_ITEM	4		/* row to print 1st item desc on */
#define		MAX_ITEMS_ON_1ST_COL	7	/* # of items whose desc appears the 1st col on a menu scr */
#define		DONE		1		/* return value by functions */
#define		NOT_DONE	0		/* return value by functions */
#define		ALLOWED		1		/* return value by functions */
#define		NOT_ALLOWED	0		/* return value by functions */
#define		RET_ERROR	-1		/* error return from function call */	
#define		READ_BUF_SIZE	133		/* size of buffer for reading user input */
#define		SCR_TITLE_COL	50		/* col to print screen title on */
#define		GREETING_COL	38		/* col to print greeting line on */
#define		SRCH_WIN_LINES	6		/* # of lines for srch_win window  */
#define		START_SRCHWIN_ROW	18	/* starting row of srch_win window */
#define         MSG_WIN_LINES   8
#define         START_MSG_WIN_ROW 16
#define         ALT_WIN_LINES   10
#define         ALT_WIN_START_ROW 13
#define		EXTRA_TIME	5*60		/* time before call becomes an old call (in the past) */
/* define max values of "piu" on various screens */
#ifdef SCANDINAVIA
#define		MAX_PIU_BASIC		B_LOCAL	/* max piu for basic scr */
#else
#define         MAX_PIU_BASIC           B_SEND_RATE
#endif
#define		MAX_PIU_BAS_SUBS	B_END_DATE	/* max piu for subs basic */
#ifdef SCANDINAVIA
#define		MAX_PIU_EXTENDED	E_VIA_ADDRESS	/* max piu for extended scr */
#else
#define         MAX_PIU_EXTENDED        E_SEND_RATE
#endif
#define		MAX_PIU_EXT_SUBS	E_END_DATE	/* max piu for subs extended */
#define		MAX_PIU_WAKEUP		9	/* max piu for wakeup scr */
#define		MAX_PIU_WAK_SUBS	25	/* max piu for subs wakeup */
#define		UI_TRACE_FILE		"ui_trace"  /* trace file used by ui */
/* defines for action taken by call taker, for user statistics processing */
#define		ENTERING_CALL		0x01
#define		KILLING_CALL		0x02
#define		CANCELLING_CALL		0x03
#define		CALLING_BACK		0x04
#define		SENDING_MSG		0x05
#define		RESOLVING_EXCEPTION	0x06
#define		OVERRIDING		0x07
#define		MANUAL_ASSIGNING	0x08
/* defines for response time statistics */
#define		CMD_TIME_CHECK	-1
#define		ALL_CALLS_SEARCH_CMD	0
#define		ZONE_CALL_CMD		1
#define		APPROVE_DISAPPROVE_EXCEPT_CMD 2
#define		CALL_ENTRY_CMD		3
#define		START_TIME_CMD		0
#define		END_TIME_CMD		1
/*
 * Print/Clear. These are used as flags for some UI functions.
 */
#define C_PRINT		'P'
#define C_CLEAR		'C'
/*
 * Macro to convert a character to a digit
 */
#define TO_DIGIT(c) ((int) (c - '0'))
