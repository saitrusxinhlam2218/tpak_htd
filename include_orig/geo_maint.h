 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  geo_maint.h; Rel: 10.1.2.1; Get date: 4/20/95 at 11:46:17
 * @(#)  Last delta: 12/2/94 at 18:14:34
 * @(#)  SCCS File: /taxi/sccs/s.geo_maint.h
 *                                                                           *
 *****************************************************************************/
#ident "@(#) head:geo_maint.h	10.1.2.1"

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

/*  Define row and columns for the labels for the geo files 
 *  maintenance screen.
 */

#define		L_ST_NAME_ROW	2
#define		L_ST_NAME_COL	1
#define		L_TYPE_COL	24
#define		L_ABBRV_COL	72
#define		L_ALIAS_COL	104
#define		L_PL_NAME_ROW	6
#define		L_PL_NAME_COL	1
#define		L_PL_PRE_COL	24
#define		L_PL_POST_COL	29
#define		L_PL_CITY_COL	34
#define		L_PL_O_E_COL	40
#define		L_ST_NBR_COL	44
#define		L_SUF_COL	52
#define		L_PL_ZIP_COL	56
#define		L_PL_SBZ_COL	64
#define		L_PL_CMNT_1_COL	72
#define		L_PL_XY_CRD_COL	104
#define		L_PL_MAP_COL	119
#define		L_PL_PB_COL	125
#define		L_LOC_COL	130
#define		L_ADD_RNG_ROW 	11
#define		L_ADD_RNG_COL	1
#define		L_AD_PRE_COL	22
#define		L_AD_POST_COL	27
#define		L_AD_CITY_COL	32
#define		L_AD_O_E_COL	38
#define		L_HI_NBR_COL	42
#define		L_LO_NBR_COL	51
#define		L_AD_ZIP_COL	60
#define		L_AD_SBZ_COL	68
#define		L_AD_CMNT_COL	76
#define		L_AD_XY_CRD_COL	108
#define		L_AD_MAP_COL	123
#define		L_AD_PB_COL	127

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
#define		D_PL_NAME_1_COL	1
#define		D_PL_PRE_1_COL	24
#define		D_PL_POST_1_COL	29
#define		D_PL_CITY_1_COL	34
#define		D_PL_OE_1_COL	40
#define		D_ST_NBR_1_COL	44
#define		D_SUF_1_COL	52
#define		D_PL_ZIP_1_COL	56
#define		D_PL_SBZ_1_COL	64
#define		D_PL_CMNT_1_COL	72
#define		D_PL_X_CD_1_COL 104
#define		D_PL_Y_CD_1_COL	111
#define		D_PL_MAP_1_COL	119
#define		D_PL_PB_1_COL	125
#define		D_LOC_1_COL	130
#define		D_PL_NAME_2_ROW	8
#define		D_PL_NAME_2_COL	1
#define		D_PL_PRE_2_COL	24
#define		D_PL_POST_2_COL	29
#define		D_PL_CITY_2_COL	34
#define		D_PL_OE_2_COL	40
#define		D_ST_NBR_2_COL	44
#define		D_SUF_2_COL	52
#define		D_PL_ZIP_2_COL	56
#define		D_PL_SBZ_2_COL	64
#define		D_PL_CMNT_2_COL	72
#define		D_PL_X_CD_2_COL 104
#define		D_PL_Y_CD_2_COL	111
#define		D_PL_MAP_2_COL	119
#define		D_PL_PB_2_COL	125
#define		D_LOC_2_COL	130
#define		D_PL_NAME_3_ROW	9
#define		D_PL_NAME_3_COL	1
#define		D_PL_PRE_3_COL	24
#define		D_PL_POST_3_COL	29
#define		D_PL_CITY_3_COL	34
#define		D_PL_OE_3_COL	40
#define		D_ST_NBR_3_COL	44
#define		D_SUF_3_COL	52
#define		D_PL_ZIP_3_COL	56
#define		D_PL_SBZ_3_COL	64
#define		D_PL_CMNT_3_COL	72
#define		D_PL_X_CD_3_COL 104
#define		D_PL_Y_CD_3_COL	111
#define		D_PL_MAP_3_COL	119
#define		D_PL_PB_3_COL	125
#define		D_LOC_3_COL	130
#define		D_ADD_ROW_1	12 
#define		D_AD_PRE_1_COL	22 
#define		D_AD_POST_1_COL	27
#define		D_AD_CITY_1_COL	32
#define		D_AD_OE_1_COL	38
#define		D_LO_NBR_1_COL	42
#define		D_HI_NBR_1_COL	51
#define		D_AD_ZIP_1_COL	60
#define		D_AD_SBZ_1_COL	68
#define		D_AD_CMNT_1_COL	76
#define		D_AD_X_CD_1_COL 108
#define		D_AD_Y_CD_1_COL	115
#define		D_AD_MAP_1_COL	123
#define		D_AD_PB_1_COL	129
#define		D_ADD_ROW_2	13 
#define		D_AD_PRE_2_COL	22 
#define		D_AD_POST_2_COL	27
#define		D_AD_CITY_2_COL	32
#define		D_AD_OE_2_COL	38
#define		D_LO_NBR_2_COL	42
#define		D_HI_NBR_2_COL	51
#define		D_AD_ZIP_2_COL	60
#define		D_AD_SBZ_2_COL	68
#define		D_AD_CMNT_2_COL	76
#define		D_AD_X_CD_2_COL 108
#define		D_AD_Y_CD_2_COL	115
#define		D_AD_MAP_2_COL	123
#define		D_AD_PB_2_COL	129
#define		D_ADD_ROW_3	14 
#define		D_AD_PRE_3_COL	22 
#define		D_AD_POST_3_COL	27
#define		D_AD_CITY_3_COL	32
#define		D_AD_OE_3_COL	38
#define		D_LO_NBR_3_COL	42
#define		D_HI_NBR_3_COL	51
#define		D_AD_ZIP_3_COL	60
#define		D_AD_SBZ_3_COL	68
#define		D_AD_CMNT_3_COL	76
#define		D_AD_X_CD_3_COL 108
#define		D_AD_Y_CD_3_COL	115
#define		D_AD_MAP_3_COL	123
#define		D_AD_PB_3_COL	129
#define		D_ADD_ROW_4	15 
#define		D_AD_PRE_4_COL	22 
#define		D_AD_POST_4_COL	27
#define		D_AD_CITY_4_COL	32
#define		D_AD_OE_4_COL	38
#define		D_LO_NBR_4_COL	42
#define		D_HI_NBR_4_COL	51
#define		D_AD_ZIP_4_COL	60
#define		D_AD_SBZ_4_COL	68
#define		D_AD_CMNT_4_COL	76
#define		D_AD_X_CD_4_COL 108
#define		D_AD_Y_CD_4_COL	115
#define		D_AD_MAP_4_COL	123
#define		D_AD_PB_4_COL	129
#define		D_ADD_ROW_5	16 
#define		D_AD_PRE_5_COL	22 
#define		D_AD_POST_5_COL	27
#define		D_AD_CITY_5_COL	32
#define		D_AD_OE_5_COL	38
#define		D_LO_NBR_5_COL	42
#define		D_HI_NBR_5_COL	51
#define		D_AD_ZIP_5_COL	60
#define		D_AD_SBZ_5_COL	68
#define		D_AD_CMNT_5_COL	76
#define		D_AD_X_CD_5_COL 108
#define		D_AD_Y_CD_5_COL	115
#define		D_AD_MAP_5_COL	123
#define		D_AD_PB_5_COL	129
#define		D_ADD_ROW_6	17 
#define		D_AD_PRE_6_COL	22 
#define		D_AD_POST_6_COL	27
#define		D_AD_CITY_6_COL	32
#define		D_AD_OE_6_COL	38
#define		D_LO_NBR_6_COL	42
#define		D_HI_NBR_6_COL	51
#define		D_AD_ZIP_6_COL	60
#define		D_AD_SBZ_6_COL	68
#define		D_AD_CMNT_6_COL	76
#define		D_AD_X_CD_6_COL 108
#define		D_AD_Y_CD_6_COL	115
#define		D_AD_MAP_6_COL	123
#define		D_AD_PB_6_COL	129
#define		D_ADD_ROW_7	18 
#define		D_AD_PRE_7_COL	22 
#define		D_AD_POST_7_COL	27
#define		D_AD_CITY_7_COL	32
#define		D_AD_OE_7_COL	38
#define		D_LO_NBR_7_COL	42
#define		D_HI_NBR_7_COL	51
#define		D_AD_ZIP_7_COL	60
#define		D_AD_SBZ_7_COL	68
#define		D_AD_CMNT_7_COL	76
#define		D_AD_X_CD_7_COL 108
#define		D_AD_Y_CD_7_COL	115
#define		D_AD_MAP_7_COL	123
#define		D_AD_PB_7_COL	129
#define		D_ADD_ROW_8	19 
#define		D_AD_PRE_8_COL	22 
#define		D_AD_POST_8_COL	27
#define		D_AD_CITY_8_COL	32
#define		D_AD_OE_8_COL	38
#define		D_LO_NBR_8_COL	42
#define		D_HI_NBR_8_COL	51
#define		D_AD_ZIP_8_COL	60
#define		D_AD_SBZ_8_COL	68
#define		D_AD_CMNT_8_COL	76
#define		D_AD_X_CD_8_COL 108
#define		D_AD_Y_CD_8_COL	115
#define		D_AD_MAP_8_COL	123
#define		D_AD_PB_8_COL	129
#define		D_ADD_ROW_9	20 
#define		D_AD_PRE_9_COL	22 
#define		D_AD_POST_9_COL	27
#define		D_AD_CITY_9_COL	32
#define		D_AD_OE_9_COL	38
#define		D_LO_NBR_9_COL	42
#define		D_HI_NBR_9_COL	51
#define		D_AD_ZIP_9_COL	60
#define		D_AD_SBZ_9_COL	68
#define		D_AD_CMNT_9_COL	76
#define		D_AD_X_CD_9_COL 108
#define		D_AD_Y_CD_9_COL	115
#define		D_AD_MAP_9_COL	123
#define		D_AD_PB_9_COL	129

/*  Define maximum characters that are allowed for each field
 *  on the geo files maintenance screen.
 */

#define		MC_ST_NAME		20
#define		MC_TYPE			1
#define		MC_ABBRV_1		20
#define		MC_ALIAS_1		20
#define		MC_ABBRV_2		20
#define		MC_ALIAS_2		20
#define		MC_PL_NAME_1		20
#define		MC_PL_PRE_1		2
#define		MC_PL_POST_1		2
#define		MC_PL_CITY_1		3
#define		MC_PL_OE_1		1
#define		MC_ST_NBR_1		6
#define		MC_SUF_1		1
#define		MC_PL_ZIP_1		5
#define		MC_PL_SBZ_1		5
#define		MC_PL_CMNT_1		30
#define		MC_PL_X_CD_1		5
#define		MC_PL_Y_CD_1		5
#define		MC_PL_MAP_1		3
#define		MC_PL_PB_1		3
#define		MC_LOC_1		1
#define		MC_PL_NAME_2		20
#define		MC_PL_PRE_2		2
#define		MC_PL_POST_2		2
#define		MC_PL_CITY_2		3
#define		MC_PL_OE_2		1
#define		MC_ST_NBR_2		6
#define		MC_SUF_2		1
#define		MC_PL_ZIP_2		5
#define		MC_PL_SBZ_2		5
#define		MC_PL_CMNT_2		30
#define		MC_PL_X_CD_2 		5
#define		MC_PL_Y_CD_2		5
#define		MC_PL_MAP_2		3
#define		MC_PL_PB_2		2
#define		MC_LOC_2		1
#define		MC_PL_NAME_3		20
#define		MC_PL_PRE_3		2
#define		MC_PL_POST_3		2
#define		MC_PL_CITY_3		3
#define		MC_PL_OE_3		1
#define		MC_ST_NBR_3		6
#define		MC_SUF_3		1
#define		MC_PL_ZIP_3		5
#define		MC_PL_SBZ_3		5
#define		MC_PL_CMNT_3		30
#define		MC_PL_X_CD_3 		5
#define		MC_PL_Y_CD_3		5
#define		MC_PL_MAP_3		3
#define		MC_PL_PB_3		2
#define		MC_LOC_3		1
#define		MC_AD_PRE_1		2 
#define		MC_AD_POST_1		2
#define		MC_AD_CITY_1		3
#define		MC_AD_OE_1		1
#define		MC_HI_NBR_1		6
#define		MC_LO_NBR_1		6
#define		MC_AD_ZIP_1		5
#define		MC_AD_SBZ_1		5
#define		MC_AD_CMNT_1		30
#define		MC_AD_X_CD_1		5
#define		MC_AD_Y_CD_1		5
#define		MC_AD_MAP_1		3
#define		MC_AD_PB_1		2
#define		MC_AD_PRE_2		2 
#define		MC_AD_POST_2		2
#define		MC_AD_CITY_2		3
#define		MC_AD_OE_2		1
#define		MC_HI_NBR_2		6
#define		MC_LO_NBR_2		6
#define		MC_AD_ZIP_2		5
#define		MC_AD_SBZ_2		5
#define		MC_AD_CMNT_2		30
#define		MC_AD_X_CD_2		5
#define		MC_AD_Y_CD_2		5
#define		MC_AD_MAP_2		3
#define		MC_AD_PB_2		2
#define		MC_AD_PRE_3		2 
#define		MC_AD_POST_3		2
#define		MC_AD_CITY_3		3
#define		MC_AD_OE_3		1
#define		MC_HI_NBR_3		6
#define		MC_LO_NBR_3		6
#define		MC_AD_ZIP_3		5
#define		MC_AD_SBZ_3		5
#define		MC_AD_CMNT_3		30
#define		MC_AD_X_CD_3 		5
#define		MC_AD_Y_CD_3		5
#define		MC_AD_MAP_3		3
#define		MC_AD_PB_3		2
#define		MC_AD_PRE_4		2 
#define		MC_AD_POST_4		2
#define		MC_AD_CITY_4		3
#define		MC_AD_OE_4		1
#define		MC_HI_NBR_4		6
#define		MC_LO_NBR_4		6
#define		MC_AD_ZIP_4		5
#define		MC_AD_SBZ_4		5
#define		MC_AD_CMNT_4		30
#define		MC_AD_X_CD_4		5
#define		MC_AD_Y_CD_4		5
#define		MC_AD_MAP_4		3
#define		MC_AD_PB_4		2
#define		MC_AD_PRE_5		2 
#define		MC_AD_POST_5		2
#define		MC_AD_CITY_5		3
#define		MC_AD_OE_5		1
#define		MC_HI_NBR_5		6
#define		MC_LO_NBR_5		6
#define		MC_AD_ZIP_5		5
#define		MC_AD_SBZ_5		5
#define		MC_AD_CMNT_5		30
#define		MC_AD_X_CD_5		5
#define		MC_AD_Y_CD_5		5
#define		MC_AD_MAP_5		3
#define		MC_AD_PB_5		2
#define		MC_AD_PRE_6		2 
#define		MC_AD_POST_6		2
#define		MC_AD_CITY_6		3
#define		MC_AD_OE_6		1
#define		MC_HI_NBR_6		6
#define		MC_LO_NBR_6		6
#define		MC_AD_ZIP_6		5
#define		MC_AD_SBZ_6		5
#define		MC_AD_CMNT_6		30
#define		MC_AD_X_CD_6		5
#define		MC_AD_Y_CD_6		5
#define		MC_AD_MAP_6		3
#define		MC_AD_PB_6		2
#define		MC_AD_PRE_7		2 
#define		MC_AD_POST_7		2
#define		MC_AD_CITY_7		3
#define		MC_AD_OE_7		1
#define		MC_HI_NBR_7		6
#define		MC_LO_NBR_7		6
#define		MC_AD_ZIP_7		5
#define		MC_AD_SBZ_7		5
#define		MC_AD_CMNT_7		30
#define		MC_AD_X_CD_7		5
#define		MC_AD_Y_CD_7		5
#define		MC_AD_MAP_7		3
#define		MC_AD_PB_7		2
#define		MC_AD_PRE_8		2 
#define		MC_AD_POST_8		2
#define		MC_AD_CITY_8		3
#define		MC_AD_OE_8		1
#define		MC_HI_NBR_8		6
#define		MC_LO_NBR_8		6
#define		MC_AD_ZIP_8		5
#define		MC_AD_SBZ_8		5
#define		MC_AD_CMNT_8		30
#define		MC_AD_X_CD_8		5
#define		MC_AD_Y_CD_8		5
#define		MC_AD_MAP_8		3
#define		MC_AD_PB_8		2
#define		MC_AD_PRE_9		2 
#define		MC_AD_POST_9		2
#define		MC_AD_CITY_9		3
#define		MC_AD_OE_9		1
#define		MC_HI_NBR_9		6
#define		MC_LO_NBR_9		6
#define		MC_AD_ZIP_9		5
#define		MC_AD_SBZ_9		5
#define		MC_AD_CMNT_9		30
#define		MC_AD_X_CD_9		5
#define		MC_AD_Y_CD_9		5
#define		MC_AD_MAP_9		3
#define		MC_AD_PB_9		2

/*  Define the field number of each field on the geo files
 *  maintenance screen.
 */
#define		F_ST_NAME		1
#define		F_TYPE			2
#define		F_ABBRV_1		3
#define		F_ALIAS_1		4
#define		F_ABBRV_2		5
#define		F_ALIAS_2		6
#define		F_PL_NAME_1		7
#define		F_PL_CITY_1		8
#define		F_PL_OE_1		9
#define		F_ST_NBR_1		10
#define		F_PL_SBZ_1		11
#define		F_PL_CMNT_1		12
#define		F_PL_MAP_1		13
#define		F_PL_PB_1		14
#define		F_LOC_1			15
#define		F_PL_NAME_2		16
#define		F_PL_CITY_2		17
#define		F_PL_OE_2		18
#define		F_ST_NBR_2		19
#define		F_PL_SBZ_2		20
#define		F_PL_CMNT_2		21
#define		F_PL_MAP_2		22
#define		F_PL_PB_2		23
#define		F_LOC_2			24
#define		F_PL_NAME_3		25
#define		F_PL_CITY_3		26
#define		F_PL_OE_3		27
#define		F_ST_NBR_3		28
#define		F_PL_SBZ_3		29
#define		F_PL_CMNT_3		30
#define		F_PL_MAP_3		31
#define		F_PL_PB_3		32
#define		F_LOC_3			33
#define		F_AD_PRE_1		34 
#define		F_AD_POST_1		35
#define		F_AD_CITY_1		36
#define		F_AD_OE_1		37
#define		F_LO_NBR_1		38
#define		F_HI_NBR_1		39
#define		F_AD_ZIP_1		40
#define		F_AD_SBZ_1		41
#define		F_AD_CMNT_1		42
#define		F_AD_X_CD_1		43
#define		F_AD_Y_CD_1		44
#define		F_AD_MAP_1		45
#define		F_AD_PB_1		46
#define		F_AD_PRE_2		47 
#define		F_AD_POST_2		48
#define		F_AD_CITY_2		49
#define		F_AD_OE_2		50
#define		F_LO_NBR_2		51
#define		F_HI_NBR_2		52
#define		F_AD_ZIP_2		53
#define		F_AD_SBZ_2		54
#define		F_AD_CMNT_2		55
#define		F_AD_X_CD_2		56
#define		F_AD_Y_CD_2		57
#define		F_AD_MAP_2		58
#define		F_AD_PB_2		59
#define		F_AD_PRE_3		60
#define		F_AD_POST_3		61
#define		F_AD_CITY_3		62
#define		F_AD_OE_3		63
#define		F_LO_NBR_3		64
#define		F_HI_NBR_3		65
#define		F_AD_ZIP_3		66
#define		F_AD_SBZ_3		67
#define		F_AD_CMNT_3		68
#define		F_AD_X_CD_3 		69
#define		F_AD_Y_CD_3		70
#define		F_AD_MAP_3		71
#define		F_AD_PB_3		72
#define		F_AD_PRE_4		73
#define		F_AD_POST_4		74
#define		F_AD_CITY_4		75
#define		F_AD_OE_4		76
#define		F_LO_NBR_4		77
#define		F_HI_NBR_4		78
#define		F_AD_ZIP_4		79
#define		F_AD_SBZ_4		80
#define		F_AD_CMNT_4		81
#define		F_AD_X_CD_4		82
#define		F_AD_Y_CD_4		83
#define		F_AD_MAP_4		84
#define		F_AD_PB_4		85
#define		F_AD_PRE_5		86
#define		F_AD_POST_5		87
#define		F_AD_CITY_5		88
#define		F_AD_OE_5		89
#define		F_LO_NBR_5		90
#define		F_HI_NBR_5		91
#define		F_AD_ZIP_5		92
#define		F_AD_SBZ_5		93
#define		F_AD_CMNT_5		94
#define		F_AD_X_CD_5		95
#define		F_AD_Y_CD_5		96
#define		F_AD_MAP_5		97
#define		F_AD_PB_5		98
#define		F_AD_PRE_6		99
#define		F_AD_POST_6		100
#define		F_AD_CITY_6		101
#define		F_AD_OE_6		102
#define		F_LO_NBR_6		103
#define		F_HI_NBR_6		104
#define		F_AD_ZIP_6		105
#define		F_AD_SBZ_6		106
#define		F_AD_CMNT_6		107
#define		F_AD_X_CD_6		108
#define		F_AD_Y_CD_6		109
#define		F_AD_MAP_6		110
#define		F_AD_PB_6		111
#define		F_AD_PRE_7		112
#define		F_AD_POST_7		113
#define		F_AD_CITY_7		114
#define		F_AD_OE_7		115
#define		F_LO_NBR_7		116
#define		F_HI_NBR_7		117
#define		F_AD_ZIP_7		118
#define		F_AD_SBZ_7		119
#define		F_AD_CMNT_7		120
#define		F_AD_X_CD_7		121
#define		F_AD_Y_CD_7		122
#define		F_AD_MAP_7		123
#define		F_AD_PB_7		124
#define		F_AD_PRE_8		125
#define		F_AD_POST_8		126
#define		F_AD_CITY_8		127
#define		F_AD_OE_8		128
#define		F_LO_NBR_8		129
#define		F_HI_NBR_8		130
#define		F_AD_ZIP_8		131
#define		F_AD_SBZ_8		132
#define		F_AD_CMNT_8		133
#define		F_AD_X_CD_8		134
#define		F_AD_Y_CD_8		135
#define		F_AD_MAP_8		136
#define		F_AD_PB_8		137
#define		F_AD_PRE_9		138
#define		F_AD_POST_9		139
#define		F_AD_CITY_9		140
#define		F_AD_OE_9		141
#define		F_LO_NBR_9		142
#define		F_HI_NBR_9		143
#define		F_AD_ZIP_9		144
#define		F_AD_SBZ_9		145
#define		F_AD_CMNT_9		146
#define		F_AD_X_CD_9		147
#define		F_AD_Y_CD_9		148
#define		F_AD_MAP_9		149
#define		F_AD_PB_9		150

/*  The following global arrays of structures are used by the "read_field()"
 *  routine to place the cursor/prompt to the correct row and column of 
 *  the field being read.  The index to the array is the field number.
 */

static struct geo_st_fld[] = {
	{ 0, 0, 1 },
	{ D_ST_NAME_ROW, D_ST_NAME_COL, MC_ST_NAME },
	{ D_ST_NAME_ROW, D_TYPE_COL, MC_TYPE }
};
static struct geo_abbrv_fld[] = {
	{ 0, 0, 1 },
	{ D_ST_NAME_ROW, D_ABBRV_1_COL, MC_ABBRV_1 },
	{ D_ABBRV_2_ROW, D_ABBRV_2_COL, MC_ABBRV_2 }
};
static struct geo_alias_fld[] = {
	{ D_ST_NAME_ROW, D_ALIAS_1_COL, MC_ALIAS_1 },
	{ D_ABBRV_2_ROW, D_ALIAS_2_COL, MC_ALIAS_2 }
};

static struct geo_pl_fld[] = {
	{ 0, 0, 1 },
	{ D_PL_NAME_1_ROW, D_PL_NAME_1_COL, MC_PL_NAME_1 },
	//	{ D_PL_NAME_1_ROW, D_PL_PRE_1_COL, MC_PL_PRE_1 },
	//	{ D_PL_NAME_1_ROW, D_PL_POST_1_COL, MC_PL_POST_1 },
	{ D_PL_NAME_1_ROW, D_PL_CITY_1_COL, MC_PL_CITY_1 },
	{ D_PL_NAME_1_ROW, D_PL_OE_1_COL, MC_PL_OE_1 },
	{ D_PL_NAME_1_ROW, D_ST_NBR_1_COL, MC_ST_NBR_1 },
	//	{ D_PL_NAME_1_ROW, D_SUF_1_COL, MC_SUF_1 },
	//	{ D_PL_NAME_1_ROW, D_PL_ZIP_1_COL, MC_PL_ZIP_1 },
	{ D_PL_NAME_1_ROW, D_PL_SBZ_1_COL, MC_PL_SBZ_1 },
	{ D_PL_NAME_1_ROW, D_PL_CMNT_1_COL, MC_PL_CMNT_1 },
	//	{ D_PL_NAME_1_ROW, D_PL_X_CD_1_COL, MC_PL_X_CD_1 },
	//	{ D_PL_NAME_1_ROW, D_PL_Y_CD_1_COL, MC_PL_Y_CD_1 },
	{ D_PL_NAME_1_ROW, D_PL_MAP_1_COL, MC_PL_MAP_1 },
	{ D_PL_NAME_1_ROW, D_PL_PB_1_COL, MC_PL_PB_1 },
	{ D_PL_NAME_1_ROW, D_LOC_1_COL, MC_LOC_1 },
	{ D_PL_NAME_2_ROW, D_PL_NAME_2_COL, MC_PL_NAME_2 },
	//	{ D_PL_NAME_2_ROW, D_PL_PRE_2_COL, MC_PL_PRE_2 },
	//	{ D_PL_NAME_2_ROW, D_PL_POST_2_COL, MC_PL_POST_2 },
	{ D_PL_NAME_2_ROW, D_PL_CITY_2_COL, MC_PL_CITY_2 },
	{ D_PL_NAME_2_ROW, D_PL_OE_2_COL, MC_PL_OE_2 },
	{ D_PL_NAME_2_ROW, D_ST_NBR_2_COL, MC_ST_NBR_2 },
	//	{ D_PL_NAME_2_ROW, D_SUF_2_COL, MC_SUF_2 },
	//	{ D_PL_NAME_2_ROW, D_PL_ZIP_2_COL, MC_PL_ZIP_2 },
	{ D_PL_NAME_2_ROW, D_PL_SBZ_2_COL, MC_PL_SBZ_2 },
	{ D_PL_NAME_2_ROW, D_PL_CMNT_2_COL, MC_PL_CMNT_2 },
	//	{ D_PL_NAME_2_ROW, D_PL_X_CD_2_COL, MC_PL_X_CD_2 },
	//	{ D_PL_NAME_2_ROW, D_PL_Y_CD_2_COL, MC_PL_Y_CD_2 },
	{ D_PL_NAME_2_ROW, D_PL_MAP_2_COL, MC_PL_MAP_2 },
	{ D_PL_NAME_2_ROW, D_PL_PB_2_COL, MC_PL_PB_2 },
	{ D_PL_NAME_2_ROW, D_LOC_2_COL, MC_LOC_2 },
	{ D_PL_NAME_3_ROW, D_PL_NAME_3_COL, MC_PL_NAME_3 },
	//	{ D_PL_NAME_3_ROW, D_PL_PRE_3_COL, MC_PL_PRE_3 },
	//	{ D_PL_NAME_3_ROW, D_PL_POST_3_COL, MC_PL_POST_3 },
	{ D_PL_NAME_3_ROW, D_PL_CITY_3_COL, MC_PL_CITY_3 },
	{ D_PL_NAME_3_ROW, D_PL_OE_3_COL, MC_PL_OE_3 },
	{ D_PL_NAME_3_ROW, D_ST_NBR_3_COL, MC_ST_NBR_3 },
	//	{ D_PL_NAME_3_ROW, D_SUF_3_COL, MC_SUF_3 },
	//	{ D_PL_NAME_3_ROW, D_PL_ZIP_3_COL, MC_PL_ZIP_3 },
	{ D_PL_NAME_3_ROW, D_PL_SBZ_3_COL, MC_PL_SBZ_3 },
	{ D_PL_NAME_3_ROW, D_PL_CMNT_3_COL, MC_PL_CMNT_3 },
	//	{ D_PL_NAME_3_ROW, D_PL_X_CD_3_COL, MC_PL_X_CD_3 },
	//	{ D_PL_NAME_3_ROW, D_PL_Y_CD_3_COL, MC_PL_Y_CD_3 },
	{ D_PL_NAME_3_ROW, D_PL_MAP_3_COL, MC_PL_MAP_3 },
	{ D_PL_NAME_3_ROW, D_PL_PB_3_COL, MC_PL_PB_3 },
	{ D_PL_NAME_3_ROW, D_LOC_3_COL, MC_LOC_3 },
};
static struct geo_add_fld[] = {
	{ 0, 0, 1 },
	{ D_ADD_ROW_1, D_AD_PRE_1_COL, MC_AD_PRE_1 },
	{ D_ADD_ROW_1, D_AD_POST_1_COL, MC_AD_POST_1 },
	{ D_ADD_ROW_1, D_AD_CITY_1_COL, MC_AD_CITY_1 },
	{ D_ADD_ROW_1, D_AD_OE_1_COL, MC_AD_OE_1_COL },
	{ D_ADD_ROW_1, D_LO_NBR_1_COL, MC_LO_NBR_1_COL },
	{ D_ADD_ROW_1, D_HI_NBR_1_COL, MC_HI_NBR_1 },
	{ D_ADD_ROW_1, D_AD_ZIP_1_COL, MC_AD_ZIP_1 },
	{ D_ADD_ROW_1, D_AD_SBZ_1_COL, MC_AD_SBZ_1 },
	{ D_ADD_ROW_1, D_AD_CMNT_1_COL, MC_AD_CMNT_1 },
	{ D_ADD_ROW_1, D_AD_X_CD_1_COL, MC_AD_X_CD_1 },
	{ D_ADD_ROW_1, D_AD_Y_CD_1_COL, MC_AD_Y_CD_1 },
	{ D_ADD_ROW_1, D_AD_MAP_1_COL, MC_AD_MAP_1 },
	{ D_ADD_ROW_1, D_AD_PB_1_COL, MC_AD_PB_1 },
	{ D_ADD_ROW_2, D_AD_PRE_2_COL, MC_AD_PRE_2 },
	{ D_ADD_ROW_2, D_AD_PRE_2_COL, MC_AD_PRE_2 },
	{ D_ADD_ROW_2, D_AD_POST_2_COL, MC_AD_POST_2 },
	{ D_ADD_ROW_2, D_AD_CITY_2_COL, MC_AD_CITY_2 },
	{ D_ADD_ROW_2, D_AD_OE_2_COL, MC_AD_OE_2_COL },
	{ D_ADD_ROW_2, D_LO_NBR_2_COL, MC_LO_NBR_2_COL },
	{ D_ADD_ROW_2, D_HI_NBR_2_COL, MC_HI_NBR_2 },
	{ D_ADD_ROW_2, D_AD_ZIP_2_COL, MC_AD_ZIP_2 },
	{ D_ADD_ROW_2, D_AD_SBZ_2_COL, MC_AD_SBZ_2 },
	{ D_ADD_ROW_2, D_AD_CMNT_2_COL, MC_AD_CMNT_2 },
	{ D_ADD_ROW_2, D_AD_X_CD_2_COL, MC_AD_X_CD_2 },
	{ D_ADD_ROW_2, D_AD_Y_CD_2_COL, MC_AD_Y_CD_2 },
	{ D_ADD_ROW_2, D_AD_MAP_2_COL, MC_AD_MAP_2 },
	{ D_ADD_ROW_2, D_AD_PB_2_COL, MC_AD_PB_2 },
	{ D_ADD_ROW_3, D_AD_POST_3_COL, MC_AD_POST_3 },
	{ D_ADD_ROW_3, D_AD_CITY_3_COL, MC_AD_CITY_3 },
	{ D_ADD_ROW_3, D_AD_OE_3_COL, MC_AD_OE_3_COL },
	{ D_ADD_ROW_3, D_LO_NBR_3_COL, MC_LO_NBR_3_COL },
	{ D_ADD_ROW_3, D_HI_NBR_3_COL, MC_HI_NBR_3 },
	{ D_ADD_ROW_3, D_AD_ZIP_3_COL, MC_AD_ZIP_3 },
	{ D_ADD_ROW_3, D_AD_SBZ_3_COL, MC_AD_SBZ_3 },
	{ D_ADD_ROW_3, D_AD_CMNT_3_COL, MC_AD_CMNT_3 },
	{ D_ADD_ROW_3, D_AD_X_CD_3_COL, MC_AD_X_CD_3 },
	{ D_ADD_ROW_3, D_AD_Y_CD_3_COL, MC_AD_Y_CD_3 },
	{ D_ADD_ROW_3, D_AD_MAP_3_COL, MC_AD_MAP_3 },
	{ D_ADD_ROW_3, D_AD_PB_3_COL, MC_AD_PB_3 },
	{ D_ADD_ROW_4, D_AD_PRE_4_COL, MC_AD_PRE_4 },
	{ D_ADD_ROW_4, D_AD_POST_4_COL, MC_AD_POST_4 },
	{ D_ADD_ROW_4, D_AD_CITY_4_COL, MC_AD_CITY_4 },
	{ D_ADD_ROW_4, D_AD_OE_4_COL, MC_AD_OE_4_COL },
	{ D_ADD_ROW_4, D_LO_NBR_4_COL, MC_LO_NBR_4_COL },
	{ D_ADD_ROW_4, D_HI_NBR_4_COL, MC_HI_NBR_4 },
	{ D_ADD_ROW_4, D_AD_ZIP_4_COL, MC_AD_ZIP_4 },
	{ D_ADD_ROW_4, D_AD_SBZ_4_COL, MC_AD_SBZ_4 },
	{ D_ADD_ROW_4, D_AD_CMNT_4_COL, MC_AD_CMNT_4 },
	{ D_ADD_ROW_4, D_AD_X_CD_4_COL, MC_AD_X_CD_4 },
	{ D_ADD_ROW_4, D_AD_Y_CD_4_COL, MC_AD_Y_CD_4 },
	{ D_ADD_ROW_4, D_AD_MAP_4_COL, MC_AD_MAP_4 },
	{ D_ADD_ROW_4, D_AD_PB_4_COL, MC_AD_PB_4 },
	{ D_ADD_ROW_5, D_AD_PRE_5_COL, MC_AD_PRE_5 },
	{ D_ADD_ROW_5, D_AD_POST_5_COL, MC_AD_POST_5 },
	{ D_ADD_ROW_5, D_AD_CITY_5_COL, MC_AD_CITY_5 },
	{ D_ADD_ROW_5, D_AD_OE_5_COL, MC_AD_OE_5_COL },
	{ D_ADD_ROW_5, D_LO_NBR_5_COL, MC_LO_NBR_5_COL },
	{ D_ADD_ROW_5, D_HI_NBR_5_COL, MC_HI_NBR_5 },
	{ D_ADD_ROW_5, D_AD_ZIP_5_COL, MC_AD_ZIP_5 },
	{ D_ADD_ROW_5, D_AD_SBZ_5_COL, MC_AD_SBZ_5 },
	{ D_ADD_ROW_5, D_AD_CMNT_5_COL, MC_AD_CMNT_5 },
	{ D_ADD_ROW_5, D_AD_X_CD_5_COL, MC_AD_X_CD_5 },
	{ D_ADD_ROW_5, D_AD_Y_CD_5_COL, MC_AD_Y_CD_5 },
	{ D_ADD_ROW_5, D_AD_MAP_5_COL, MC_AD_MAP_5 },
	{ D_ADD_ROW_5, D_AD_PB_5_COL, MC_AD_PB_5 },
	{ D_ADD_ROW_6, D_AD_PRE_6_COL, MC_AD_PRE_6 },
	{ D_ADD_ROW_6, D_AD_POST_6_COL, MC_AD_POST_6 },
	{ D_ADD_ROW_6, D_AD_CITY_6_COL, MC_AD_CITY_6 },
	{ D_ADD_ROW_6, D_AD_OE_6_COL, MC_AD_OE_6_COL },
	{ D_ADD_ROW_6, D_LO_NBR_6_COL, MC_LO_NBR_6_COL },
	{ D_ADD_ROW_6, D_HI_NBR_6_COL, MC_HI_NBR_6 },
	{ D_ADD_ROW_6, D_AD_ZIP_6_COL, MC_AD_ZIP_6 },
	{ D_ADD_ROW_6, D_AD_SBZ_6_COL, MC_AD_SBZ_6 },
	{ D_ADD_ROW_6, D_AD_CMNT_6_COL, MC_AD_CMNT_6 },
	{ D_ADD_ROW_6, D_AD_X_CD_6_COL, MC_AD_X_CD_6 },
	{ D_ADD_ROW_6, D_AD_Y_CD_6_COL, MC_AD_Y_CD_6 },
	{ D_ADD_ROW_6, D_AD_MAP_6_COL, MC_AD_MAP_6 },
	{ D_ADD_ROW_6, D_AD_PB_6_COL, MC_AD_PB_6 },
	{ D_ADD_ROW_7, D_AD_PRE_7_COL, MC_AD_PRE_7 },
	{ D_ADD_ROW_7, D_AD_POST_7_COL, MC_AD_POST_7 },
	{ D_ADD_ROW_7, D_AD_CITY_7_COL, MC_AD_CITY_7 },
	{ D_ADD_ROW_7, D_AD_OE_7_COL, MC_AD_OE_7_COL },
	{ D_ADD_ROW_7, D_LO_NBR_7_COL, MC_LO_NBR_7_COL },
	{ D_ADD_ROW_7, D_HI_NBR_7_COL, MC_HI_NBR_7 },
	{ D_ADD_ROW_7, D_AD_ZIP_7_COL, MC_AD_ZIP_7 },
	{ D_ADD_ROW_7, D_AD_SBZ_7_COL, MC_AD_SBZ_7 },
	{ D_ADD_ROW_7, D_AD_CMNT_7_COL, MC_AD_CMNT_7 },
	{ D_ADD_ROW_7, D_AD_X_CD_7_COL, MC_AD_X_CD_7 },
	{ D_ADD_ROW_7, D_AD_Y_CD_7_COL, MC_AD_Y_CD_7 },
	{ D_ADD_ROW_7, D_AD_MAP_7_COL, MC_AD_MAP_7 },
	{ D_ADD_ROW_7, D_AD_PB_7_COL, MC_AD_PB_7 },
	{ D_ADD_ROW_8, D_AD_PRE_8_COL, MC_AD_PRE_8 },
	{ D_ADD_ROW_8, D_AD_POST_8_COL, MC_AD_POST_8 },
	{ D_ADD_ROW_8, D_AD_CITY_8_COL, MC_AD_CITY_8 },
	{ D_ADD_ROW_8, D_AD_OE_8_COL, MC_AD_OE_8_COL },
	{ D_ADD_ROW_8, D_LO_NBR_8_COL, MC_LO_NBR_8_COL },
	{ D_ADD_ROW_8, D_HI_NBR_8_COL, MC_HI_NBR_8 },
	{ D_ADD_ROW_8, D_AD_ZIP_8_COL, MC_AD_ZIP_8 },
	{ D_ADD_ROW_8, D_AD_SBZ_8_COL, MC_AD_SBZ_8 },
	{ D_ADD_ROW_8, D_AD_CMNT_8_COL, MC_AD_CMNT_8 },
	{ D_ADD_ROW_8, D_AD_X_CD_8_COL, MC_AD_X_CD_8 },
	{ D_ADD_ROW_8, D_AD_Y_CD_8_COL, MC_AD_Y_CD_8 },
	{ D_ADD_ROW_8, D_AD_MAP_8_COL, MC_AD_MAP_8 },
	{ D_ADD_ROW_8, D_AD_PB_8_COL, MC_AD_PB_8 },
	{ D_ADD_ROW_9, D_AD_PRE_9_COL, MC_AD_PRE_9 },
	{ D_ADD_ROW_9, D_AD_POST_9_COL, MC_AD_POST_9 },
	{ D_ADD_ROW_9, D_AD_CITY_9_COL, MC_AD_CITY_9 },
	{ D_ADD_ROW_9, D_AD_OE_9_COL, MC_AD_OE_9_COL },
	{ D_ADD_ROW_9, D_LO_NBR_9_COL, MC_LO_NBR_9_COL },
	{ D_ADD_ROW_9, D_HI_NBR_9_COL, MC_HI_NBR_9 },
	{ D_ADD_ROW_9, D_AD_ZIP_9_COL, MC_AD_ZIP_9 },
	{ D_ADD_ROW_9, D_AD_SBZ_9_COL, MC_AD_SBZ_9 },
	{ D_ADD_ROW_9, D_AD_CMNT_9_COL, MC_AD_CMNT_9 },
	{ D_ADD_ROW_9, D_AD_X_CD_9_COL, MC_AD_X_CD_9 },
	{ D_ADD_ROW_9, D_AD_Y_CD_9_COL, MC_AD_Y_CD_9 },
	{ D_ADD_ROW_9, D_AD_MAP_9_COL, MC_AD_MAP_9 },
	{ D_ADD_ROW_9, D_AD_PB_9_COL, MC_AD_PB_9 }
};

/*  Declare an array of pointers to functions which check the field
 *  entered for validity.
 */

int (*check_field[])() = { check_dummy, check_name, check_type, 
			   	check_abbrv, check_alias, check_abbrv,
			   	check_alias, check_pl_name, check_pre,
			   	check_post, check_city, check_oe,
			   	check_st_nbr, check_suf, check_pl_zip,
			   	check_pl_sbz, check_pl_cmnt, check_pl_x_cd,
			   	check_pl_y_cd, check_pl_map, check_pl_pb,
			   	check_loc, check_pre, check_post, check_city,
			   	check_oe, check_st_nbr, check_suf, 
			   	check_pl_zip, check_pl_sbz, check_pl_cmnt,
			   	check_pl_x_cd, check_pl_y_cd, check_pl_map,
			   	check_pl_pb, check_loc, check_pre, check_post,
			   	check_city, check_oe, check_st_nbr, check_suf,
			   	check_pl_zip, check_pl_sbz, check_pl_cmnt,
			   	check_pl_x_cd, check_pl_y_cd, check_pl_map,
			   	check_pl_pb, check_loc,
			   	check_ad_pre, check_ad_post, check_ad_city,
			   	check_ad_oe, check_hi_nbr, check_lo_nbr,
			   	check_ad_zip, check_ad_sbz, check_ad_x_cd,
			   	check_ad_y_cd, check_ad_map, check_ad_pb,
			   	check_ad_pre, check_ad_post, check_ad_city,
			   	check_ad_oe, check_hi_nbr, check_lo_nbr,
			   	check_ad_zip, check_ad_sbz, check_ad_x_cd,
			   	check_ad_y_cd, check_ad_map, check_ad_pb,
			   	check_ad_pre, check_ad_post, check_ad_city,
			   	check_ad_oe, check_hi_nbr, check_lo_nbr,
			   	check_ad_zip, check_ad_sbz, check_ad_x_cd,
			   	check_ad_y_cd, check_ad_map, check_ad_pb,
			   	check_ad_pre, check_ad_post, check_ad_city,
			   	check_ad_oe, check_hi_nbr, check_lo_nbr,
			   	check_ad_zip, check_ad_sbz, check_ad_x_cd,
			   	check_ad_y_cd, check_ad_map, check_ad_pb,
			   	check_ad_pre, check_ad_post, check_ad_city,
			   	check_ad_oe, check_hi_nbr, check_lo_nbr,
			   	check_ad_zip, check_ad_sbz, check_ad_x_cd,
			   	check_ad_y_cd, check_ad_map, check_ad_pb,
			   	check_ad_pre, check_ad_post, check_ad_city,
			   	check_ad_oe, check_hi_nbr, check_lo_nbr,
			   	check_ad_zip, check_ad_sbz, check_ad_x_cd,
			   	check_ad_y_cd, check_ad_map, check_ad_pb,
			   	check_ad_pre, check_ad_post, check_ad_city,
			   	check_ad_oe, check_hi_nbr, check_lo_nbr,
			   	check_ad_zip, check_ad_sbz, check_ad_x_cd,
			   	check_ad_y_cd, check_ad_map, check_ad_pb, 
			   	check_ad_pre, check_ad_post, check_ad_city, 
			   	check_ad_oe, check_hi_nbr, check_lo_nbr, 
			   	check_ad_zip, check_ad_sbz, check_ad_x_cd, 
			   	check_ad_y_cd, check_ad_map, check_ad_pb, 
			   	check_ad_pre, check_ad_post, check_ad_city, 
			   	check_ad_oe, check_hi_nbr, check_lo_nbr, 
			   	check_ad_zip, check_ad_sbz, check_ad_x_cd, 
			   	check_ad_y_cd, check_ad_map, check_ad_pb}; 
			 	 
#define 	BUFFER_SIZE	133	/* size of buffer for reading input fields */ 

#define		VALID		1	/* input field was valid */ 
#define		NOT_VALID	-1	/* input field was not valid */ 

#define		ABBRV_FLD	1	/* abbreviation field */ 
#define		ALIAS_FLD	2	/* alias field	*/ 
#define		PL_NAME_FLD	3	/* place name field */ 
#define		ADDRESS_FLD	4	/* address field */

struct geo {
	struct cisam_sn st_name_file[9];
	struct cisam_pl pl_name_file[6];
	struct cisam_ad address_file[18];
} geo_file;

short which_field;			/* indicates which field the cursor
					   is on */
main()
{
	short ret_val;			/* returned value from check_field()*/
	char query_by;			/* Query by field indicator */
	char input_buffer[BUFFER_SIZE]; 

	Call Trace routine.

	Prepare to catch signals.
	
	1.  Print the header lines on the screen.

	    prt_scr_hdr();	

	2.  Print the labels.  They include: 
	
    	    Line 2  Street Name, Street Type, Abbreviations, Aliases
		    
    	    Line 6  Place Name, Pre-direction, Post-direction, City, 
		    odd/even address indicator, street number, suffix,
		    zip,  subzone, comment, x/y coordinates, Map,
		    page/block, and Loc.

	    Line 11 Address Ranges, Pre-direction, Post-direction, City,
		    odd/even address indicator, address high number,
		    address low number, zip, subzone, comment, x/y coordinates,
		    map, and page/block.

	    prt_labels();

	3.  Input the users request (Query, Next, Previous, Add, Update,
	    Remove, Output, Exit.

	    while(!valid_req(request = read_request())
		;

	    switch(request)  {

		case 'Q':	/* Query */
			process_query();
			break;

		case 'N':	/* Next */
			process_next();
			break;

		case 'P':	/* Previous */
			process_previous();
			break;

		case 'A':	/* Add */
			process_add();
			break;

		case 'U':	/* Update */
			process_update();
			break;

		case 'R':	/* Remove */
			process_remove();
			break;

		case 'O':	/* Output */
			process_output();
			break;
		
		case 'E':	/* Exit */
			exit(0);
			break;

		default:
			exit(-1);
			break;
	}



	4.  Process user request. 

		process_query()
		{

		1.  Display the header line.

		    prt_query_hdr();

		2.  Display the screen labels.

		    prt_labels();

		3.  Process users input of key fields(street name and type).  
		    If the user tries to input data into any other fields, 
		    an error results.

		    ret_val = NOT_VALID;
			while(ret_val == NOT_VALID)  {
		    		field_nbr = F_ST_NAME;
				key_val = read_field(field_nbr,input_buffer,fld_entered);
			    	ret_val = check_field[F_ST_NAME](&field_nbr,input_buffer,key_val,&fld_entered);
			}
			while(ret_val == NOT_VALID)  {
				field_nbr = F_TYPE;
				key_val = read_field(field_nbr,input_buffer,fld_entered);
				ret_val = check_field[F_TYPE](&field_nbr,input_buffer,key_val,&fld_entered);
			}

		5.  Using the keys entered in step 3, call C-Isam routine
		    to retrieve specific record.

				db2(OPEN Street name file);
				db2(READ with street name and type);
				db2(READ first abbreviation);
				db2(READ second abbreviation);
				db2(READ first alias);
				db2(READ second alias);
				db2(CLOSE Street name file);

				db2(OPEN Place name file);
				db2(READ with index);
				for(i = 0;i < 2; i++)
					db2(GETNEXT record);
				db2(CLOSE Place name file);

				db2(OPEN Address file);
				db2(READ with index);
				for(i = 0;i < 8; i++)
					db2(GETNEXT record);
				db2(CLOSE Address file);

		6.  Display data in appropriate fields on the screen.

		    prt_data();
		}

		process_next()
		{

		1.  Call the C-Isam routine getnext.

		switch(which_field)  {
			case 'ABBRV_FLD':  	/* abbreviation field */
				db2(OPEN street name file);
				db2(READ next abbreviation);
				db2(CLOSE street name file);
				break;

			case 'ALIAS_FLD':  	/* alias field */
				db2(OPEN street name file);
				db2(READ next alias);
				db2(CLOSE street name file);
				break;

			case 'PL_NAME_FLD':	/* place name field */
				db2(OPEN place name file);
				db2(READ next place name);
				db2(CLOSE place name file);
				break;

			case 'ADDRESS_FLD':	/* address field */
				db2(OPEN address file);
				db2(READ next address record);
				db2(CLOSE address file);
				break;
		}

		2.  Display data in appropriate fields on the screen.

		    switch(which_field)  {
			case 'ABBRV_FLD': 	/* abbreviation field */
				prt_abbrv();
				break;

			case 'ALIAS_FLD':	/* alias field */
				prt_alias();
				break;

			case 'PL_NAME_FLD':	/* place name field */
				prt_pl_name();
				break;

			case 'ADDRESS_FLD':	/* address field */
				prt_address();
				break;

		    }

		}

		process_previous()
		{

		1.  Call the C-Isam routine getprevious.

		switch(which_field)  {
			case 'ABBRV_FLD':  	/* abbreviation field */
				db2(OPEN street name file);
				db2(READ previous abbreviation);
				db2(CLOSE street name file);
				break;

			case 'ALIAS_FLD':  	/* alias field */
				db2(OPEN street name file);
				db2(READ previous alias);
				db2(CLOSE street name file);
				break;

			case 'PL_NAME_FLD':	/* place name field */
				db2(OPEN place name file);
				db2(READ previous place name);
				db2(CLOSE place name file);
				break;

			case 'ADDRESS_FLD':	/* address field */
				db2(OPEN address file);
				db2(READ previous address record);
				db2(CLOSE address file);
				break;
		}


		2.  Display data in appropriate fields on the screen.

		    switch(which_field)  {
			case 'ABBRV_FLD': 	/* abbreviation field */
				prt_abbrv();
				break;

			case 'ALIAS_FLD':	/* alias field */
				prt_alias();
				break;

			case 'PL_NAME_FLD':	/* place name field */
				prt_pl_name();
				break;

			case 'ADDRESS_FLD':	/* address field */
				prt_address();
				break;

		    }


		}

		process_add()
		{

		1.  Display the header line.

		    1 - Abbreviation, 2 - Alias, 3 - Place Name, 4 - Address ?
		    scanf("%c",&which_field);

		    prt_add_hdr();

		2.  Display the appropriate screen labels.

		    prt_labels();

		3.  Process each field as input by the user.

		    switch(which_field)  {
			case 'ABBRV_FLD': 	/* abbreviation field */
				process_ab_input();
				break;

			case 'ALIAS_FLD': 	/* alias field */
				process_al_input();
				break

			case 'PL_NAME_FLD':	/* place name field */
				process_pn_input();
				break;

			case 'ADDRESS_FLD':	/* address field */
				process_ad_input();
				break;
		    }

		4.  Call the C-Isam add record routine.

		switch(which_field)  {
			case 'ABBRV_FLD':  	/* abbreviation field */
				db2(OPEN street name file);
				db2(ADD abbreviation); 
				db2(CLOSE street name file);
				break;

			case 'ALIAS_FLD':  	/* alias field */
				db2(OPEN street name file);
				db2(ADD alias); 
				db2(CLOSE street name file); 
				break;

			case 'PL_NAME_FLD':	/* place name field */
				db2(OPEN place name file);
				db2(ADD place name);
				db2(CLOSE place name file);
				break;

			case 'ADDRESS_FLD':	/* address field */
				db2(OPEN address file);
				db2(ADD address record);
				db2(CLOSE address file);
				break;
		}

		5.  Display the main header.

		    prt_hdr();

		}

		process_update()
		{

		1.  Display the header line.

		    1 - Abbreviation, 2 - Alias, 3 - Place Name, 4 - Address ?
		    scanf("%c",&which_field);

		    prt_upd_hdr();

		2.  Process each field as input by the user.

		    switch(which_field)  {
			case 'ABBRV_FLD': 	/* abbreviation field */
				process_ab_input();
				break;

			case 'ALIAS_FLD': 	/* alias field */
				process_al_input();
				break

			case 'PL_NAME_FLD':	/* place name field */
				process_pn_input();
				break;

			case 'ADDRESS_FLD':	/* address field */
				process_ad_input();
				break;
		    }


		3.  Call the C-Isam update record 
		    routine.

		switch(which_field)  {
			case 'ABBRV_FLD':  	/* abbreviation field */
				db2(OPEN street name file);
				db2(UPDATE abbreviation); 
				db2(CLOSE street name file);
				break;

			case 'ALIAS_FLD':  	/* alias field */
				db2(OPEN street name file);
				db2(UPDATE alias); 
				db2(CLOSE street name file); 
				break;

			case 'PL_NAME_FLD':	/* place name field */
				db2(OPEN place name file);
				db2(UPDATE place name);
				db2(CLOSE place name file);
				break;

			case 'ADDRESS_FLD':	/* address field */
				db2(OPEN address file);
				db2(UPDATE address record);
				db2(CLOSE address file);
				break;
		}

		3.  Display the main header.

		    prt_hdr();

		}

		process_remove()
		{

		1.  Display the header line.

		    1 - Abbreviation, 2 - Alias, 3 - Place Name, 4 - Address ?
		    scanf("%c",&which_field);

		    prt_rm_hdr();

		1.  Prompt the user to comfirm the desire to remove the 
		    record.

		2.  If the user responded to step 1 negatively, return.

		3.  If the user responded to step 1 affirmatively, call
		    C-Isam routine delete record.

		switch(which_field)  {
			case 'ABBRV_FLD':  	/* abbreviation field */
				db2(REMOVE abbreviation); 
				db2(CLOSE street name file);
				break;

			case 'ALIAS_FLD':  	/* alias field */
				db2(REMOVE alias); 
				db2(CLOSE street name file); 
				break;

			case 'PL_NAME_FLD':	/* place name field */
				db2(REMOVE place name);
				db2(CLOSE place name file);
				break;

			case 'ADDRESS_FLD':	/* address field */
				db2(REMOVE address record);
				db2(CLOSE address file);
				break;
		}

		4.  Clear the data fields on the screen.

			switch(which_field)  {
				case 'ABBRV_FLD': 	/* abbreviation field */
					clear_ab_fld();
					break;

				case 'ALIAS_FLD':	/* alias filed */
					clear_al_fld();
					break;

				case 'PL_NAME_FLD':	/* place name field */
					clear_pn_fld();
					break;

				case 'ADDRESS_FLD':	/* address field */
					clear_ad_fld();
					break;
				
			}

		}

		process_output()
		{

		1.  Prompt the user for the name of an output file.

		2.  Ask the user if they want to Append this data to
		    an existing file or create a new one ?

		3.  Open output file for append or create.

		4.  Write the screen information to the file.

		    prt_scr(ofile);

		}

		process_exit()
		{

		1.  Terminate the program, which should return to SQL.
		 
		}

	Call trace routine.
}

prt_labels()
{

}

prt_data()
{
}

process_ab_input()
{
	key_val = read_field(F_ABBRV_1,input_buffer,field_entered);
	ret_val = check_field(F_ABBRV_1,input_buffer, key_val,&field_entered);
	if(ret_val = NOT_VALID)  {
		prt_error("Bad field was entered while adding/updating a record");
		return(-1);
	}
	key_val = read_field(F_ABBRV_2,input_buffer,field_entered);
	ret_val = check_field(F_ABBRV_2,input_buffer, key_val,&field_entered);
	if(ret_val = NOT_VALID)  {
		prt_error("Bad field was entered while adding/updating a record");
		return(-1);
	}
}
process_al_input()
{
	key_val = read_field(F_ALIAS_1,input_buffer,field_entered);
	ret_val = check_field(F_ALIAS_1,input_buffer, key_val,&field_entered);
	if(ret_val = NOT_VALID)  {
		prt_error("Bad field was entered while adding/updating a record");
		return(-1);
	}
	key_val = read_field(F_ALIAS_2,input_buffer,field_entered);
	ret_val = check_field(F_ALIAS_2,input_buffer, key_val,&field_entered);
	if(ret_val = NOT_VALID)  {
		prt_error("Bad field was entered while adding/updating a record");
		return(-1);
	}
}

process_pn_input()
{
	int i;

	for(i = F_PL_NAME_1, i < F_AD_PRE_1, i++)  {
		key_val = read_field(i,input_buffer,fld_entered);
		ret_val = check_field[i](&field_nbr,input_buffer,key_val,&fld_entered);
		if(ret_val = NOT_VALID)  {
			prt_error("Bad field was entered while adding a record");
			return(-1);

		if(ret_val == ESC)
			break;
	}
	return();
}

process_ad_input()
{
	int i;

	for(i = F_AD_PRE_1, i <= F_AD_PB_9, i++)  {
		key_val = read_field(i,input_buffer,fld_entered);
		ret_val = check_field[i](&field_nbr,input_buffer,key_val,&fld_entered);
		if(ret_val = NOT_VALID)  {
			prt_error("Bad field was entered while adding a record");
			return(-1);

		if(ret_val == ESC)
			break;
	}
	return();
}

prt_scr(o_file)
char *ofile;
{

	FILE *fopen();
	FILE *fptr;
	int col, line, c;

	if ( (fptr = fopen(ofile, "w")) == NULL ) {
		printw("fopen failed");
		refresh();
		endwin();
		exit(1);
	}

	col = 0;
	for ( line = 0; line < LINES ; line++)  {
		for ( col = 0; col < COLS ; col++ )  {
			move(line,col);
			c = inch();
			fputc(c,fptr);	
		}
		if(c != '\n')
			fputc('\n',fptr);
	}
	
	fclose(pptr);
}

int check_dummy (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
}

int check_name (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
}

int check_type (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
}

int check_abbrv (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
}

int check_alias (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
}

int check_pl_name (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
}

int check_pre (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
}

int check_post (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
}

int check_city (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
}

int check_oe (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
}

int check_st_nbr (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
}

int check_suf (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
}

int check_pl_zip (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
}

int check_pl_sbz (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
	The subzone must be validated in the subzone file.
}

int check_pl_cmnt (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
}

int check_pl_x_cd (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
}

int check_pl_y_cd (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
}

int check_pl_map (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
}

int check_pl_pb (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
}

int check_loc (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
}

int check_ad_pre (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
}

int check_ad_post (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
}

int check_ad_city (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
}

int check_ad_oe (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
}

int check_hi_nbr (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
}

int check_lo_nbr (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
}

int check_ad_zip (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
}

int check_ad_sbz (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
	The subzone must be validated in the subzone file.
}

int check_ad_x_cd (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
}

int check_ad_y_cd (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
}

int check_ad_map (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
}

int check_ad_pb (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
}


#define		BUFF_SIZE		133	/* size of buffer for reading input fields */
#define		DF_TRACE_FILE		"/user/mads2/run/trace.out"	
#define		VAD			1	/* Input field was valid */
#define		NOT_VAD		-1	/* Input field was not valid */
#define		ESC			033	/* Escape character */
#define		CR			015	/* Return character */
#define		NEW_LINE		012	/* New line character */
#define		F_NAME_SIZE		15	/* Maximum file name size */





