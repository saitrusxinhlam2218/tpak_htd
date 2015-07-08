 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  kdt_ary.h; Rel: 10.1.2.1; Get date: 4/20/95 at 11:47:01
 * @(#)  Last delta: 12/2/94 at 18:09:42
 * @(#)  SCCS File: /taxi/sccs/s.kdt_ary.h
 *                                                                           *
 *****************************************************************************/
#ident "@(#) head:kdt_ary.h	10.1.2.1"

/* kdt_ary.h: contains defines for the positions and lengths 
 * of various fields on all the screens.
 * also contains defines for the position of each field type on
 * window; a static structure difining the various fields
 * position, lengths and max allowed char's is also included
 */

/* declare structure for saving field values */
struct scr_flds {
	int prompt_row;	/* row on which prompt appears */
	int prompt_col;	/* col on which prompt appears */
	int max_chars;	/* max chars allowed on a field */
	int tab_field;	/* piu value to move to if tab key pressed */
};

/* definitions for rows and columns for entry of various fields
 * on a KDT_EMULATION_WIN 
 */
#define		D_FLEET_ROW		2
#define		D_DRV_ROW		3
#define		D_FLEET_COL		11
#define		D_VEH_COL		33	
#define		D_ACTION_COL		55	
#define		D_DRV_COL		74	
#define		D_PRIORITY_ROW		2
#define		D_PRIORITY_COL		103
#define		D_ZONE_ROW		2
#define		D_ZONE_COL		74

/* define the lable */
#define		L_FLEET_ROW		2
#define		L_FLEET_COL		1
#define		L_VEH_COL		17	
#define		L_ACTION_COL		42	
#define		L_ZONE_COL		61
#define		L_PRI_COL		82
#define		L_DRV_ROW		3
#define		L_LAB_ROW		4

/* define the max. number of characters that cana be entered for each of the fields */
#define		CHARS_FLEET		1
#define		CHARS_VEH		4
#define		CHARS_ACTION		2
#define		CHARS_ZONE		3
#define		CHARS_DRV		6
#define		CHARS_PRIORITY		2

/* define the positions of the fields on the window */
#define		F_FLEET			1	/* 1st field on the window */
#define		F_VEH			2
#define		F_ACTION		3
#define		F_ZONE			4
#define		F_DRV			5
#define		F_PRIORITY		6




