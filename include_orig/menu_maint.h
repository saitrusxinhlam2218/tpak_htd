 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  menu_maint.h; Rel: 10.1.2.1; Get date: 4/20/95 at 11:47:56
 * @(#)  Last delta: 12/2/94 at 18:18:38
 * @(#)  SCCS File: /taxi/sccs/s.menu_maint.h
 *                                                                           *
 *****************************************************************************/
#ident "@(#) head:menu_maint.h	10.1.2.1"

/*  This file defines the cursor positions for the labels and data fields
 *  as well as the maximum characters allowed in a particular field and the
 *  field number on the menu file maintenance screen.
 */

/*  Define row and columns for misc fields for the menu file 
 *  maintenance screen.
 */

#define		HDR_ROW_1		0
#define		HDR_COL_1		0
#define		HDR_ROW_2		1
#define		HDR_COL_2		56
#define		REQ_ROW			0
#define		REQ_COL			54
#define		DESC_ROW		22
#define		DESC_COL		0
#define		ERROR_ROW		23
#define		ERROR_COL		0
#define		DEL_ROW			0
#define		DEL_COL			0

/*  Define row and columns for the labels for the menu file maintenance 
 *  screen.
 */

#define		L_M_NBR_ROW		3
#define		L_M_NBR_COL		1
#define		L_CMD_COL		10
#define		L_LST_MENU_COL		20
#define		L_TYPE_COL		38
#define		L_DESC_COL		46
#define		L_UNIX_CMD_ROW		6
#define		L_UNIX_CMD_COL		1
#define		L_PRE_SCR_COL		45

/*
 *  Define row and columns for the data fields of the menu file maintenance
 *  screen.
 */

#define		D_M_NBR_ROW		4
#define		D_M_NBR_COL		2
#define		D_CMD_COL		11
#define		D_LST_MENU_COL		21
#define		D_TYPE_COL		39
#define		D_DESC_COL		47
#define		D_UNIX_CMD_ROW 		7
#define		D_UNIX_CMD_COL		2
#define		D_PRE_SCR_COL		46

/*  Define maximum characters that are allowed for each field on
 *  the menu file maintenance screen.
 */

#define		MC_NBR			3
#define		MC_CMD			3
#define		MC_LST_MENU		1
#define		MC_TYPE			1
#define		MC_DESC			33
#define		MC_UNIX_CMD		40
#define		MC_PRE_SCR		1

/*  Define the field number of each field on the menu file maintenance screen.
 */

#define		F_M_NBR			1
#define		F_CMD			2	
#define		F_LST_MENU		3
#define		F_TYPE			4
#define		F_DESC			5
#define		F_UNIX_CMD		6
#define		F_PRE_SCR		7
#define		NUM_FLDS		7

/*  Define misc constants for the menu file maintenance screen program.  */

#define		BUFFER_SIZE	133	/* size of buffer for reading input fields */
#define		MENU_OF_OPEN 	1	/* menu output file open error */
#define		BAD_MSG_GET	2	/* unable to do a message queue get */
#define		BAD_MSG_SND	3	/* unable to do a message send */
#define 	BAD_FLD_ENT	4	/* bad field was entered by user */
#define		BAD_MENU_OPEN	5	/* unable to open menu file */
#define		BAD_MENU_CLOSE  6	/* unable to close menu file */
#define		BAD_MENU_READ	7	/* unable to read menu file */
#define		BAD_MENU_NEXT	8	/* There are no more rows in the direction you are going */
#define		BAD_MENU_PREV	9	/* There are no more rows in the direction you are going */
#define		BAD_MENU_ADD	10	/* unable to add a menu file record */
#define		BAD_MENU_UPD	11	/* unable to update a menu file record */
#define		BAD_MENU_DEL	12	/* unable to delete a menu file record */
#define		BAD_READ_FIRST	13	/* unable to read first record in the menu file */
#define		BAD_TRACE_OPEN  14	/* unable to open trace file */
#define 	BAD_TRACE_CLOSE 15	/* unable to close trace file */
#define		BAD_REQUEST	16	/* Invalid user request */
#define		MENU_NEXT_ERR	17	/* Unable to read next menu file record */
#define		MENU_PREV_ERR	18	/* Unable to read previous menu file record */

#define		VALID		1	/* input field was valid */
#define		NOT_VALID  	-1	/* input field was not valid */
#define 	CR		'\n'

#define		F_NAME_SIZE	15	/* maximum file name size */
