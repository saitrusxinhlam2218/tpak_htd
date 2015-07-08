 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  in_maint.h; Rel: 10.1.2.1; Get date: 4/20/95 at 11:46:35
 * @(#)  Last delta: 12/2/94 at 18:15:30
 * @(#)  SCCS File: /taxi/sccs/s.in_maint.h
 *                                                                           *
 *****************************************************************************/
#ident "@(#) head:in_maint.h	10.1.2.1"

/*  This file defines ths cursor positions for the labels and data fields
 * as well as the maximum characters allowed in a particular field and the
 * field number on the Intersection file maintenance screen.
 */

/* Define row and columns for misc fields for the intersection file
 * maintenance screen.
 */
#define		HDR_ROW_1	0
#define		HDR_COL_1	0
#define		HDR_ROW_2	1
#define		HDR_COL_2	56	
#define		REQ_ROW		0
#define		REQ_COL		54
#define		ERROR_ROW	23
#define		ERROR_COL	0
#define		ERROR_COL1	70
#define		DEL_ROW		0
#define		DEL_COL		0
/* Define row and columns for the labels for the intersection file maintenance screen */

#define		L_INDEX1_ROW	3
#define		L_INDEX1_COL	1
#define		L_PRE_DIR1_COL	33
#define		L_NAME1_COL	48
#define		L_TYPE1_COL	71
#define		L_POST_DIR1_COL	77
#define		L_INDEX2_ROW	 6
#define		L_INDEX2_COL	 1
#define		L_PRE_DIR2_COL  33
#define		L_NAME2_COL	48
#define		L_TYPE2_COL	71
#define		L_POST_DIR2_COL	77
#define		L_ZIP_ROW	9
#define		L_ZIP_COL	1
#define		L_SUBZONE_COL	14
#define		L_CITY_COL	24
#define		L_X_COORD_COL	35
#define		L_Y_COORD_COL	51
#define		L_MAP_PAGE_ROW	12
#define		L_MAP_PAGE_COL	1
#define		L_MAP_BLK_COL	17
#define		L_CMT_COL	35

/*  Define row and columns for the data fields of the intersection 
 *  file maintenance screen.
 */

#define		D_INDEX1_ROW	4
#define		D_INDEX1_COL	11
#define		D_PRE_DIR1_COL	34
#define		D_NAME1_COL	49
#define		D_TYPE1_COL	72
#define		D_POST_DIR1_COL	78
#define		D_INDEX2_ROW	 7
#define		D_INDEX2_COL	 11
#define		D_PRE_DIR2_COL  34
#define		D_NAME2_COL	49
#define		D_TYPE2_COL	72
#define		D_POST_DIR2_COL	78
#define		D_ZIP_ROW	10
#define		D_ZIP_COL	2 
#define		D_SUBZONE_COL	15
#define		D_CITY_COL	25
#define		D_X_COORD_COL	36
#define		D_Y_COORD_COL	52
#define		D_MAP_PAGE_ROW	13
#define		D_MAP_PAGE_COL	2
#define		D_MAP_BLK_COL	18
#define		D_CMT_COL	36

/*  Define maximum characters that are allowed for each
 *  field on the intersection file maintenance screen.
 */

#define		CHARS_INDEX1	18
#define		CHARS_PRE_DIR1	2
#define		CHARS_NAME1	20
#define		CHARS_TYPE1	3
#define		CHARS_POST_DIR1	2
#define		CHARS_INDEX2	18
#define		CHARS_PRE_DIR2	2
#define		CHARS_NAME2	20
#define		CHARS_TYPE2	3
#define		CHARS_POST_DIR2	2
#define		CHARS_ZIP		9
#define		CHARS_SUBZONE	5
#define		CHARS_CITY		3
#define		CHARS_X_COORD	12
#define		CHARS_Y_COORD	12
#define		CHARS_MAP_PAGE	3
#define		CHARS_MAP_BLK	4
#define		CHARS_CMT		30

/*  Define the field number of each field on the intersection file 
 *  maintenance screen.
 */

#define		F_INDEX1		0
#define		F_PRE_DIR1		(F_INDEX1 + 1)
#define		F_NAME1			(F_PRE_DIR1 + 1)
#define		F_TYPE1			(F_NAME1 + 1)
#define		F_POST_DIR1		(F_TYPE1 + 1)
#define		F_INDEX2		(F_POST_DIR1 + 1)
#define		F_PRE_DIR2		(F_INDEX2 + 1)
#define		F_NAME2			(F_PRE_DIR2 + 1)
#define		F_TYPE2			(F_NAME2 + 1)
#define		F_POST_DIR2		(F_TYPE2 + 1)
#define		F_ZIP			(F_POST_DIR2 + 1)
#define		F_SUBZONE		(F_ZIP + 1)
#define		F_CITY			(F_SUBZONE + 1)
#define		F_X_COORD		(F_CITY + 1)
#define		F_Y_COORD		(F_X_COORD + 1)
#define		F_MAP_PAGE		(F_Y_COORD + 1)
#define		F_MAP_BLOCK		(F_MAP_PAGE + 1)
#define		F_COMMENT		(F_MAP_BLOCK + 1)

#define		BUFFER_SIZE		133	/* size of buffer for reading input fields */
#define		VALID			0	/* Input field was valid */
#define		NOT_VALID		-1	/* Input field was not valid */
#define		FILE_NAME_SIZE		15	/* Maximum file name size */

/* The ways to query */
#define	INDEX		1
#define STREET_NAME	2


struct scr_flds {
  int prompt_row;		/* row on which prompt appears */
  int prompt_col;		/* col on which prompt appears */
  int max_chars;		/* maximum chars allowed on a field */
  int upper_flag;		/* for printing chars in upper/lower mode */
  char *format;			/* String format for printing (set to NULL on initialization) */
  int type;			/* CHAR, STRING, or INT */
  int justification;		/* LEFT or RIGHT for justification when printing field */
  char *descrip;		/* The field description */
  int *data;			/* Pointer to structure holding data */
  int (*check_func)();		/* Pointer to verification function */
};
#define FORMAT_LEN	8

#define CHAR_TYPE	1
#define STRING_TYPE	2
#define INT_TYPE	3
#define SHORT_TYPE	4

#define RIGHT		1
#define LEFT		2	


#ifdef IN_MAINT_SRC
struct cisam_in In;

extern int check_index(), check_dir(), check_street_name(), check_street_type(),
           check_numeric_or_empty(), check_subzone(), check_city(), check_string();

struct scr_flds In_scr_flds[] = {
	{ D_INDEX1_ROW, D_INDEX1_COL, CHARS_INDEX1, 0, NULL, INT_TYPE, LEFT, NULL, &In.index1, check_index},
	{ D_INDEX1_ROW, D_PRE_DIR1_COL, CHARS_PRE_DIR1,1, NULL, STRING_TYPE, LEFT, NULL, (int *) In.pre_dir1, check_dir},
	{ D_INDEX1_ROW, D_NAME1_COL, CHARS_NAME1,1, NULL, STRING_TYPE, LEFT, NULL, (int *) In.name1, check_street_name},
	{ D_INDEX1_ROW, D_TYPE1_COL, CHARS_TYPE1,1, NULL, STRING_TYPE, LEFT, NULL, (int *) In.type1, check_street_type},
	{ D_INDEX1_ROW, D_POST_DIR1_COL, CHARS_POST_DIR1,1, NULL, STRING_TYPE, LEFT, NULL, (int *) In.post_dir1, check_dir},
	{ D_INDEX2_ROW, D_INDEX2_COL, CHARS_INDEX2,0, NULL, INT_TYPE, LEFT, NULL, &In.index2, check_index},
	{ D_INDEX2_ROW, D_PRE_DIR2_COL, CHARS_PRE_DIR2,1, NULL, STRING_TYPE, LEFT, NULL, (int *) In.pre_dir2, check_dir},
	{ D_INDEX2_ROW, D_NAME2_COL, CHARS_NAME2,1, NULL, STRING_TYPE, LEFT, NULL, (int *) In.name2, check_street_name},
	{ D_INDEX2_ROW, D_TYPE2_COL, CHARS_TYPE2,1, NULL, STRING_TYPE, LEFT, NULL, (int *) In.type2, check_street_type},
	{ D_INDEX2_ROW, D_POST_DIR2_COL, CHARS_POST_DIR2,1, NULL, STRING_TYPE, LEFT, NULL, (int *) In.post_dir2, check_dir},
	{ D_ZIP_ROW, D_ZIP_COL, CHARS_ZIP,0, NULL, INT_TYPE, LEFT, NULL, &In.zip, check_numeric_or_empty},
	{ D_ZIP_ROW, D_SUBZONE_COL, CHARS_SUBZONE,1, NULL, STRING_TYPE, LEFT, NULL, (int *) In.subzone, check_subzone},
	{ D_ZIP_ROW, D_CITY_COL, CHARS_CITY,1, NULL, STRING_TYPE, LEFT, NULL, (int *) In.city, check_city},
	{ D_ZIP_ROW, D_X_COORD_COL, CHARS_X_COORD,0, NULL, INT_TYPE, LEFT, NULL,
	    &In.str_x_coord, check_numeric_or_empty},
	{ D_ZIP_ROW, D_Y_COORD_COL, CHARS_Y_COORD,0, NULL, INT_TYPE, LEFT, NULL,
	    &In.str_y_coord, check_numeric_or_empty},
	{ D_MAP_PAGE_ROW, D_MAP_PAGE_COL, CHARS_MAP_PAGE,0, NULL, SHORT_TYPE, LEFT, NULL,
	    (int *) &In.map_ref_page, check_numeric_or_empty},
	{ D_MAP_PAGE_ROW, D_MAP_BLK_COL, CHARS_MAP_BLK,1, NULL, STRING_TYPE, LEFT, NULL, (int *) In.map_ref_block, check_string},
	{ D_MAP_PAGE_ROW, D_CMT_COL, CHARS_CMT,1, NULL, STRING_TYPE, LEFT, NULL, (int *) In.comment, check_string},
	{-1, -1, -1, -1, NULL, -1, -1, NULL} /* This row must be here to identify end of array */
};

int Num_fields;
int Do_jump;		/* flag indicating longjmp processing	*/
jmp_buf Jump_env;	/* stored environment for longjmp	*/
int Interupts_allowed;	/* interrupts allowed */
int Saw_interupt;	/* indicates an interrupt signal was entered */

#else
extern struct cisam_in In;
extern struct scr_flds In_scr_flds[];
extern int Num_fields;
extern int Do_jump;			/* flag indicating longjmp processing	*/
extern jmp_buf Jump_env;		/* stored environment for longjmp	*/
extern int Interupts_allowed;		/* interrupts allowed */
extern int Saw_interupt;		/* indicates an interrupt signal was entered */
#endif

extern char req_buf[];			/* buffer used for interface to db routine */
