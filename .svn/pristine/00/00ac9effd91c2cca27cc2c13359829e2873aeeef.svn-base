/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: language.h.fin,v $
*  @(#)  $Revision: 1.6 $
*  @(#)  $Date: 2002/03/25 18:35:22 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/language.h.fin,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: language.h.fin,v $	$Revision: 1.6 $"

/**
 ** This include file defines the language specific character constants.
 ** There are groups of related constants. In each group the characters
 ** defined must be unique. Across different groups, there can repeated
 ** characters.
 **
 ** This is the SWEDISH language.h (stored as language.h
 ** into language.h)
 */

#ifndef _LANGUAGE_H
#define _LANGUAGE_H
/* 
 * Language in use
 */
#define LANGUAGE        "Finnish"
#ifdef __alpha
#define TPAK_LOCALE     "fi_FI.ISO8859-1"
#else
#define TPAK_LOCALE	"fi_FI.8859"
#endif
#define TPAK_NLS        "/usr/taxi/catalogs/finnish/%N.cat"
#define TPAK_NLS_PATH   "/usr/taxi/catalogs/finnish/"

/*
 * YES/NO group
 */
#ifdef YES
#undef YES
#define YES		'K'
#else 
#define YES     	'K'
#endif

#ifdef SUSP_ON
#undef SUSP_ON
#define SUSP_ON		'J'
#else 
#define SUSP_ON		'J'
#endif

#ifdef NO
#undef NO
#define NO		'E'
#else 
#define NO		'E'
#endif

#ifdef COMP
#undef COMP
#define COMP            'Y'
#else
#define COMP            'Y'
#endif


/*
 * Zone/Vehicle/Driver group
 */
#define C_ZONE		'R'
#define C_VEHICLE	'T'
#define C_DRIVER	'A'

/*
 * Call type group
 */
#define PASS		'M'
#define PASS2	        'K'
#define DELIV		'T'
#define TYPEX		'X'
#define WAKEUP		'H'
#define TIMEC		'E'
#define SUBS		'S'
#define CHARGE		'L'
#define MULT_CALL	'J'
#define AUTOBOOK_CALL	'A'
#define INVOICE_CALL    ' '
#define NON_INVOICE_CALL '3'

/*
 * Personal request type ('O'nly if vehicle is available, 'I'f is available)
 */
#define C_PERSONAL_ONLY	'V'
#define C_PERSONAL_IF	'J'

/*
 * Menu type group as defined for struct cisam_mf.type. Values are for
 * 'M'enu, 'I'nternal executable (part of UI), 'E'xternal excutable (unix
 * command).
 *
 * Must be matched to Informix screen.
 */
#define UI_MENU		'V'
#define UI_SCREEN	'S'
#define UI_EXTERNAL	'U'

/*
 * Call history event type. 'R'egular system event, or 'E'mulating a MDT event.
 *
 * See struct cisam_ch.event_type
 *
 * Must be matched to Informix screen.
 */
#define REGULAR_EVENT	'T'
#define EMULATE_EVENT	'E'

/*
 * Message log to type. '*' All users, 'F' one or more fleets, 'S' Supervisor
 * position, 'C' Call taker position, 'M' Manager position, 'C' specific
 * vehicle, 'D' specific Driver, 'Z' all vehicles in a Zone.
 *
 * See struct cisam_ml.to_type
 *
 * Must be matched to Informix screen.
 */
#define MSG_FLEET	'Y'
#define MSG_SUPERVISOR	'V'
#define MSG_CALL_TAKER	'O'
#define MSG_MANAGER	'J'
#define MSG_VEHICLE	'T'
#define MSG_DRIVER	'A'
#define MSG_ZONE	'R'
#define MSG_USER	'K'

/*
 * Message log supervisor_saw. 'A'pproved, 'D'enied, 'I'n progress, 'S'aw
 *
 * See struct cisam_ml.supervisor_saw
 *
 * Must be matched to Informix screen.
 */
#define SUPER_APPROVED		'H'
#define SUPER_DENIED 		'E'
#define SUPER_IN_PROGRESS	'K'
#define SUPER_SAW		'N'

/*
 * Odd/even/both for addresses.
 *
 * See struct cisam_ad.odd_even (Note ' ' is not used for both odd & even)
 *
 * Must be matched to Informix screen.
 */
#define ADDRESS_ODD	'R'
#define ADDRESS_EVEN	'P'
#define ADDRESS_BOTH	'M'

/*
 * The last upper case letter of the alphabet is used for searching.
 */
#define LAST_UPCASE_LETTER_OF_ALPHABET		'\\'

/*
 * These are used in the menus for the fake Informix screens
 * and the exit in the UI menus.
 */
#define M_QUERY		'Q'
#define M_NEXT		'N'
#define M_PREVIOUS	'P'
#define M_ADD		'A'
#define M_UPDATE	'U'
#define M_REMOVE	'R'
#define M_OUTPUT	'O'
#define M_SCREEN	'S'
#define M_EXIT		'E'

#define M_APPEND	'L'
#define M_CREATE	'U'

/*
 * The character used to seperate day, month, year in dates.
 */
#define DATE_SEPERATOR	'.'

/*
 * set the DATE_FORMAT to the appropriate value for the country
 */
#define DAY_FIRST_IN_DATE	1
#define MONTH_FIRST_IN_DATE	2
#define YEAR_FIRST_IN_DATE	3

#define DATE_FORMAT	DAY_FIRST_IN_DATE

/*
 *
 * the following defines were taken from dispatch.h
 *
 */
#define ONLY_PERS_REQ	'V' 		/* only */
#define IF_PERS_REQ	'J' 		/* if */
#define APPR_ONLY	'H'		/* approved */

/* in dispatch - unknown account type indicator in transaction file */
#define UNKNOWN_ACCT_TYPE '?'

/*
 * Exception Group and Type designators used in Exceptions Screen
 */
#define EXC_GRP_SEL     'V'             /* Exception Group selection */
#define EXC_TYPE_SEL    'J'             /* Exception Type selection */

#ifndef NOW
#define NOW             "NYT"
#endif

#ifndef _ERR_CAT_OPEN
#define _ERR_CAT_OPEN   "VIRHE: ei voi avata hakemistoa( %s )\n"
#endif

#define BIT_CHAR_LEFT_BRACKET  'Ä'
#define BIT_CHAR_RIGHT_BRACKET 'Å' 
#define BIT_CHAR_BACKSLASH     'Ö'
#define BIT_CHAR_CARET         'Ü'


#endif
