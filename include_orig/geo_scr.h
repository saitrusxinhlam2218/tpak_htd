 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  geo_scr.h; Rel: 10.1.2.1; Get date: 4/20/95 at 11:46:24
 * @(#)  Last delta: 12/2/94 at 18:14:38
 * @(#)  SCCS File: /taxi/sccs/s.geo_scr.h
 *                                                                           *
 *****************************************************************************/
#ident "@(#) head:geo_scr.h	10.1.2.1"

struct scr_flds {
	int prompt_row;		/* row on which prompt appears */
	int prompt_col;		/* col on which prompt appears */
	int max_chars;		/* maximum chars allowed on a field */
	int tab_field;		/* for printing chars in upper/lower mode */
};

/*  This file defines ths cursor positions for the labels and data fields
 * as well as the maximum characters allowed in a particular field and the
 * field number on the Geo file maintenance screen.
 */

/* Define row and columns for misc fields for the Geo file
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

/*  Define row and columns for the data fields of the geo files
 *  maintenance screen.
 */

#define		D_ST_NAME_ROW	3
#define		D_ST_NAME_COL	1
#define		D_TYPE_COL	24
#define		D_ABBRV_1_COL	72
#define		D_ALIAS_1_COL	104
#define		D_ABBRV_2_ROW	4
#define		D_ABBRV_2_COL	72
#define		D_ALIAS_2_COL	104
#define		D_PL_NAME_1_ROW	7
#define		D_PL_NAME_COL	1
#define		D_PL_PRE_COL	23
#define		D_PL_POST_COL	27
#define		D_PL_CITY_COL	(D_PL_NAME_COL + 22) // 23
#define		D_PL_OE_COL	(D_PL_CITY_COL + 5)  // 28
#define		D_ST_NBR_COL	(D_PL_OE_COL + 3)    // 31
#define		D_SUF_COL	47
#define		D_PL_ZIP_COL	50
#define		D_PL_SBZ_COL	(D_ST_NBR_COL + 8)  // 39
#define		D_PL_CMNT_COL	(D_PL_SBZ_COL + 7)  // 46
#define		D_PL_X_CD_COL 96
#define		D_PL_Y_CD_COL	106
#define		D_PL_MAP_COL	(D_PL_CMNT_COL + 62) // 108
#define		D_PL_PB_COL	(D_PL_MAP_COL + 7)   // 115
#define		D_LOC_COL	(D_PL_PB_COL + 6)    // 121
#define		D_PL_NAME_2_ROW	8
#define		D_PL_NAME_3_ROW	9
#define		D_ADD_ROW_1	12 
#define		D_AD_PRE_COL	10 
#define		D_AD_POST_COL	15
#define		D_AD_CITY_COL	20
#define		D_AD_OE_COL	26
#define		D_LO_NBR_COL	30
#define		D_HI_NBR_COL	39
#define		D_AD_ZIP_COL	48
#define		D_AD_SBZ_COL	56
#define		D_AD_CMNT_COL	64
#define		D_AD_X_CD_COL 96
#define		D_AD_Y_CD_COL	106
#define		D_AD_MAP_COL	117
#define		D_AD_PB_COL	125
#define		D_ADD_ROW_2	13 
#define		D_ADD_ROW_3	14 
#define		D_ADD_ROW_4	15 
#define		D_ADD_ROW_5	16 
#define		D_ADD_ROW_6	17 
#define		D_ADD_ROW_7	18 
#define		D_ADD_ROW_8	19 
#define		D_ADD_ROW_9	20 

/*  Define maximum characters that are allowed for each field
 *  on the geo files maintenance screen.
 */

#define		MC_ST_NAME		20
#define		MC_TYPE			3
#define		MC_ABBRV_1		20
#define		MC_ALIAS_1		20
#define		MC_ABBRV_2		20
#define		MC_ALIAS_2		20
#define		MC_PL_NAME		20
#define		MC_PL_PRE		2
#define		MC_PL_POST		2
#define		MC_PL_CITY		3
#define		MC_PL_OE		1
#define		MC_ST_NBR		6
#define		MC_SUF		1
#define		MC_PL_ZIP		5
#define		MC_PL_SBZ		5
#define		MC_PL_CMNT		60
#define		MC_PL_X_CD		8
#define		MC_PL_Y_CD		8
#define		MC_PL_MAP		3
#define		MC_PL_PB		4
#define		MC_LOC		1
#define		MC_AD_PRE		2 
#define		MC_AD_POST		2
#define		MC_AD_CITY		3
#define		MC_AD_OE		1
#define		MC_HI_NBR		6
#define		MC_LO_NBR		6
#define		MC_AD_ZIP		5
#define		MC_AD_SBZ		5
#define		MC_AD_CMNT		30
#define		MC_AD_X_CD		8
#define		MC_AD_Y_CD		8
#define		MC_AD_MAP		3
#define		MC_AD_PB		4


/*  Define the field number of each field on the geo files
 *  maintenance screen.
 */
#define         F_INDEX                 1
#define		F_ST_NAME		(F_INDEX + 1)
#define		F_TYPE			(F_ST_NAME + 1)
#define		F_ABBRV_1		(F_TYPE + 1)
#define		F_ALIAS_1		(F_ABBRV_1 + 1)
#define		F_ABBRV_2		(F_ALIAS_1 + 1)
#define		F_ALIAS_2		(F_ABBRV_2 + 1)
#define		F_PL_NAME_1		(F_ALIAS_2 + 1)
#define		F_PL_CITY_1		(F_PL_NAME_1 + 1)
#define		F_PL_OE_1		(F_PL_CITY_1 + 1)
#define		F_ST_NBR_1		(F_PL_OE_1 + 1)
#define		F_PL_SBZ_1		(F_ST_NBR_1 + 1)
#define		F_PL_CMNT_1		(F_PL_SBZ_1 + 1)
#define		F_PL_MAP_1		(F_PL_CMNT_1	+ 1) //13 + 1
#define		F_PL_PB_1		(F_PL_MAP_1		+1) //14 + 1
#define		F_LOC_1			(F_PL_PB_1		+1) //15 + 1
#define		F_PL_NAME_2		(F_LOC_1		+1) //6 + 1
#define		F_PL_CITY_2             (F_PL_NAME_2+1) //		17 + 1
#define		F_PL_OE_2	        (F_PL_CITY_2+1) //	18 + 1
#define		F_ST_NBR_2		(F_PL_OE_2	+1) //19 + 1
#define		F_PL_SBZ_2	        (F_ST_NBR_2	+1) //	20 + 1
#define		F_PL_CMNT_2	        (F_PL_SBZ_2	+1) //	21 + 1
#define		F_PL_MAP_2	        (F_PL_CMNT_2+1) //	22 + 1
#define		F_PL_PB_2	        (F_PL_MAP_2	+1) //	23 + 1
#define		F_LOC_2			(F_PL_PB_2	+1) //24 + 1
#define		F_PL_NAME_3		(F_LOC_2			+1) //5 + 1
#define		F_PL_CITY_3             (F_PL_NAME_3+1) //		26 + 1
#define		F_PL_OE_3	        (F_PL_CITY_3+1) //	27 + 1
#define		F_ST_NBR_3		(F_PL_OE_3	+1) //28 + 1
#define		F_PL_SBZ_3	        (F_ST_NBR_3	+1) //	29 + 1
#define		F_PL_CMNT_3	        (F_PL_SBZ_3	+1) //	30 + 1
#define		F_PL_MAP_3	        (F_PL_CMNT_3+1) //	31 + 1
#define		F_PL_PB_3	        (F_PL_MAP_3	+1) //	32 + 1
#define		F_LOC_3			(F_PL_PB_3	+1) //33 + 1
#define		F_AD_PRE_1		(F_LOC_3			+1) //4  + 1
#define		F_AD_POST_1	        (F_AD_PRE_1	+1) //	35 + 1
#define		F_AD_CITY_1             (F_AD_POST_1+1) //		36 + 1
#define		F_AD_OE_1	        (F_AD_CITY_1+1) //	37 + 1
#define		F_LO_NBR_1		(F_AD_OE_1	 +1) //38 + 1
#define		F_HI_NBR_1	        (F_LO_NBR_1	+1) //	39 + 1
#define		F_AD_ZIP_1	        (F_HI_NBR_1	+1) //	40 + 1
#define		F_AD_SBZ_1	        (F_AD_ZIP_1	+1) //	41 + 1
#define		F_AD_CMNT_1	        (F_AD_SBZ_1	+1) //	42 + 1
#define		F_AD_X_CD_1             (F_AD_CMNT_1+1) //		43 + 1
#define		F_AD_Y_CD_1             (F_AD_X_CD_1+1) //		44 + 1
#define		F_AD_MAP_1	        (F_AD_Y_CD_1+1) //	45 + 1
#define		F_AD_PB_1	        (F_AD_MAP_1	+1) //	46 + 1
#define		F_AD_PRE_2		(F_AD_PB_1	+1) //47  + 1
#define		F_AD_POST_2	        (F_AD_PRE_2	+1) //	48 + 1
#define		F_AD_CITY_2             (F_AD_POST_2+1) //		49 + 1
#define		F_AD_OE_2	        (F_AD_CITY_2+1) //	50 + 1
#define		F_LO_NBR_2		(F_AD_OE_2	+1) //51 + 1
#define		F_HI_NBR_2	        (F_LO_NBR_2	+1) //	52 + 1
#define		F_AD_ZIP_2	        (F_HI_NBR_2	+1) //	53 + 1
#define		F_AD_SBZ_2	        (F_AD_ZIP_2	+1) //	54 + 1
#define		F_AD_CMNT_2	        (F_AD_SBZ_2	+1) //	55 + 1
#define		F_AD_X_CD_2             (F_AD_CMNT_2+1) //		56 + 1
#define		F_AD_Y_CD_2             (F_AD_X_CD_2+1) //		57 + 1
#define		F_AD_MAP_2	        (F_AD_Y_CD_2+1) //	58 + 1
#define		F_AD_PB_2	        (F_AD_MAP_2	+1) //	59 + 1
#define		F_AD_PRE_3		(F_AD_PB_2	+1) //60 + 1
#define		F_AD_POST_3	        (F_AD_PRE_3	+1) //	61 + 1
#define		F_AD_CITY_3             (F_AD_POST_3+1) //		62 + 1
#define		F_AD_OE_3	        (F_AD_CITY_3+1) //	63 + 1
#define		F_LO_NBR_3		(F_AD_OE_3	+1) //64 + 1
#define		F_HI_NBR_3	        (F_LO_NBR_3	+1) //	65 + 1
#define		F_AD_ZIP_3	        (F_HI_NBR_3	+1) //	66 + 1
#define		F_AD_SBZ_3	        (F_AD_ZIP_3	+1) //	67 + 1
#define		F_AD_CMNT_3	        (F_AD_SBZ_3	+1) //	68 + 1
#define		F_AD_X_CD_3             (F_AD_CMNT_3+1) // 		69 + 1
#define		F_AD_Y_CD_3             (F_AD_X_CD_3+1) //		70 + 1
#define		F_AD_MAP_3	        (F_AD_Y_CD_3+1) //	71 + 1
#define		F_AD_PB_3	        (F_AD_MAP_3	+1) //	72 + 1
#define		F_AD_PRE_4		(F_AD_PB_3	+1) //73 + 1
#define		F_AD_POST_4	        (F_AD_PRE_4	+1) //	74 + 1
#define		F_AD_CITY_4             (F_AD_POST_4+1) //		75 + 1
#define		F_AD_OE_4	        (F_AD_CITY_4+1) //	76 + 1
#define		F_LO_NBR_4		(F_AD_OE_4	+1) //77 + 1
#define		F_HI_NBR_4	        (F_LO_NBR_4	+1) //	78 + 1
#define		F_AD_ZIP_4	        (F_HI_NBR_4	+1) //	79 + 1
#define		F_AD_SBZ_4	        (F_AD_ZIP_4	+1) //	80 + 1
#define		F_AD_CMNT_4	        (F_AD_SBZ_4	+1) //	81 + 1
#define		F_AD_X_CD_4             (F_AD_CMNT_4+1) //		82 + 1
#define		F_AD_Y_CD_4             (F_AD_X_CD_4+1) //		83 + 1
#define		F_AD_MAP_4	        (F_AD_Y_CD_4+1) //	84 + 1
#define		F_AD_PB_4	        (F_AD_MAP_4	+1) //	85 + 1
#define		F_AD_PRE_5		(F_AD_PB_4	+1) //86 + 1
#define		F_AD_POST_5	        (F_AD_PRE_5	+1) //	87 + 1
#define		F_AD_CITY_5             (F_AD_POST_5+1) //		88 + 1
#define		F_AD_OE_5	        (F_AD_CITY_5+1) //	89 + 1
#define		F_LO_NBR_5		(F_AD_OE_5	+1) //90 + 1
#define		F_HI_NBR_5	        (F_LO_NBR_5	+1) //	91 + 1
#define		F_AD_ZIP_5	        (F_HI_NBR_5	+1) //	92 + 1
#define		F_AD_SBZ_5	        (F_AD_ZIP_5	+1) //	93 + 1
#define		F_AD_CMNT_5	        (F_AD_SBZ_5	+1) //	94 + 1
#define		F_AD_X_CD_5             (F_AD_CMNT_5+1) //		95 + 1
#define		F_AD_Y_CD_5             (F_AD_X_CD_5+1) //		96 + 1
#define		F_AD_MAP_5	        (F_AD_Y_CD_5+1) //	97 + 1
#define		F_AD_PB_5	        (F_AD_MAP_5	+1) //	98 + 1
#define		F_AD_PRE_6		(F_AD_PB_5	+1) //99 + 1
#define		F_AD_POST_6	        (F_AD_PRE_6	+1) //	100 + 1
#define		F_AD_CITY_6             (F_AD_POST_6+1) //		101 + 1
#define		F_AD_OE_6	        (F_AD_CITY_6+1) //	102 + 1
#define		F_LO_NBR_6		(F_AD_OE_6	+1) //103 + 1
#define		F_HI_NBR_6	        (F_LO_NBR_6	+1) //	104 + 1
#define		F_AD_ZIP_6	        (F_HI_NBR_6	+1) //	105 + 1
#define		F_AD_SBZ_6	        (F_AD_ZIP_6	+1) //	106 + 1
#define		F_AD_CMNT_6	        (F_AD_SBZ_6	+1) //	107 + 1
#define		F_AD_X_CD_6             (F_AD_CMNT_6+1) //		108 + 1
#define		F_AD_Y_CD_6             (F_AD_X_CD_6+1) //		109 + 1
#define		F_AD_MAP_6	        (F_AD_Y_CD_6+1) //	110 + 1
#define		F_AD_PB_6	        (F_AD_MAP_6	+1) //	111 + 1
#define		F_AD_PRE_7		(F_AD_PB_6	+1) //112 + 1
#define		F_AD_POST_7	        (F_AD_PRE_7	+1) //	113 + 1
#define		F_AD_CITY_7             (F_AD_POST_7+1) //		114 + 1
#define		F_AD_OE_7	        (F_AD_CITY_7+1) //	115 + 1
#define		F_LO_NBR_7		(F_AD_OE_7	+1) //116 + 1
#define		F_HI_NBR_7	        (F_LO_NBR_7	+1) //	117 + 1
#define		F_AD_ZIP_7	        (F_HI_NBR_7	+1) //	118 + 1
#define		F_AD_SBZ_7	        (F_AD_ZIP_7	+1) //	119 + 1
#define		F_AD_CMNT_7	        (F_AD_SBZ_7	+1) //	120 + 1
#define		F_AD_X_CD_7             (F_AD_CMNT_7+1) //		121 + 1
#define		F_AD_Y_CD_7             (F_AD_X_CD_7+1) //		122 + 1
#define		F_AD_MAP_7	        (F_AD_Y_CD_7+1) //	123 + 1
#define		F_AD_PB_7	        (F_AD_MAP_7	+1) //	124 + 1
#define		F_AD_PRE_8		(F_AD_PB_7	+1) //125 + 1
#define		F_AD_POST_8	        (F_AD_PRE_8	+1) //	126 + 1
#define		F_AD_CITY_8             (F_AD_POST_8+1) //		127 + 1
#define		F_AD_OE_8	        (F_AD_CITY_8+1) //	128 + 1
#define		F_LO_NBR_8		(F_AD_OE_8	+1) //129 + 1
#define		F_HI_NBR_8	        (F_LO_NBR_8	+1) //	130 + 1
#define		F_AD_ZIP_8	        (F_HI_NBR_8	+1) //	131 + 1
#define		F_AD_SBZ_8	        (F_AD_ZIP_8	+1) //	132 + 1
#define		F_AD_CMNT_8	        (F_AD_SBZ_8	+1) //	133 + 1
#define		F_AD_X_CD_8             (F_AD_CMNT_8+1) //		134 + 1
#define		F_AD_Y_CD_8             (F_AD_X_CD_8+1) //		135 + 1
#define		F_AD_MAP_8	        (F_AD_Y_CD_8+1) //	136 + 1
#define		F_AD_PB_8	        (F_AD_MAP_8	+1) //	137 + 1
#define		F_AD_PRE_9		(F_AD_PB_8	+1) //138 + 1
#define		F_AD_POST_9	        (F_AD_PRE_9	+1) //	139 + 1
#define		F_AD_CITY_9             (F_AD_POST_9+1) //		140 + 1
#define		F_AD_OE_9	        (F_AD_CITY_9+1) //	141 + 1
#define		F_LO_NBR_9		(F_AD_OE_9	+1) //142 + 1
#define		F_HI_NBR_9	        (F_LO_NBR_9	+1) //	143 + 1
#define		F_AD_ZIP_9	        (F_HI_NBR_9	+1) //	144 + 1
#define		F_AD_SBZ_9	        (F_AD_ZIP_9	+1) //	145 + 1
#define		F_AD_CMNT_9	        (F_AD_SBZ_9	+1) //	146 + 1
#define		F_AD_X_CD_9             (F_AD_CMNT_9+1) //		147 + 1
#define		F_AD_Y_CD_9             (F_AD_X_CD_9+1) //		148 + 1
#define		F_AD_MAP_9	        (F_AD_Y_CD_9+1) //	149 + 1
#define		F_AD_PB_9	        (F_AD_MAP_9	+1) //	150 + 1
#define         NUM_FLDS                F_AD_PB_9

#define		BUFFER_SIZE		133	/* size of buffer for reading input fields */
#define		DF_TRACE_FILE		"/user/mads2/run/trace.out"	
#define		VALID			1	/* Input field was valid */
#define		NOT_VALID		-1	/* Input field was not valid */
#define		CR			015	/* Return character */
#define		NEW_LINE		012	/* New line character */
#define		F_NAME_SIZE		15	/* Maximum file name size */
