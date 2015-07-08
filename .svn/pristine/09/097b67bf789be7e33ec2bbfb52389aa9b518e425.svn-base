 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  call_entry.h; Rel: 10.1.2.1; Get date: 4/20/95 at 11:44:22
 * @(#)  Last delta: 12/2/94 at 18:19:14
 * @(#)  SCCS File: /taxi/sccs/s.call_entry.h
 *                                                                           *
 *****************************************************************************/
/* call_entry.h: contains defines for the positions and lengths 
 * of various fields on all the screens.
 * also contains defines for the position of each field type on
 * these screens; a static structure difining the various fields
 * position, lengths and max allowed char's is also included
 */
/* Make sure location constants are defined. Create a compile error if they
   are not defined or both are defined. */
#ifdef SCANDINAVIA
#ifdef NAMERICA
ERROR: "Both compile time constants SCANDINAVIA and NAMERICA are defined."
#endif
#else
#ifdef NAMERICA
#else
ERROR: Compile time constant SCANDINAVIA or NAMERICA undefined.
#endif
#endif
/* declare structure for saving field values */
struct scr_flds {
	int prompt_row;	/* row on which prompt appears */
	int prompt_col;	/* col on which prompt appears */
	int max_chars;	/* max chars allowed on a field */
	int tab_field;	/* piu value to move to if tab key pressed */
};
/* definitions for rows and columns for entry of various fields
 * on a BASIC_CALL_SCR and SUBS_EXTENDED_SCR
 */
#define		B_FLEET_ROW		3
#define		B_FLEET_COL		14
#define		B_CL_TYPE_ROW		3
#define		B_CL_TYPE_COL		28
#define		B_PKUP_ADDRESS_ROW	6
#define		B_INTERS_ROW		6
#ifdef SCANDINAVIA
#define		B_PKUP_ST_NAME_COL	14
#define		B_PKUP_ST_NO_COL	36
/* These are defined to make compilation easier
   but are not used */
#define		B_PKUP_PRE_DIR_COL	0
#define		B_PKUP_ST_TYPE_COL	0
#define		B_PKUP_POST_DIR_COL	0
#define		B_INTERS_ST_NAME_COL	14
#define		B_INTERS_PRE_DIR_COL	0
#define		B_INTERS_ST_TYPE_COL	0
#define		B_INTERS_POST_DIR_COL	0
#else
#define		B_PKUP_ST_NO_COL	14
#define		B_PKUP_PRE_DIR_COL	22
#define		B_PKUP_ST_NAME_COL	25
#define		B_PKUP_ST_TYPE_COL	46
#define		B_PKUP_POST_DIR_COL	50
#define		B_INTERS_ST_NAME_COL	25
#define		B_INTERS_PRE_DIR_COL	22
#define		B_INTERS_ST_NAME_COL	25
#define		B_INTERS_ST_TYPE_COL	46
#define		B_INTERS_POST_DIR_COL	50
#endif
#define		B_PKUP_APT_ROW		7
#define		B_PKUP_APT_COL		14
#define		B_PKUP_CITY_ROW		8
#define		B_PKUP_CITY_COL		14
#define		B_PKUP_ZONE_ROW		8
#define		B_PKUP_ZONE_COL		27
#define		B_PASSENGER_ROW		9
#define		B_PASSENGER_COL		14
#define		B_REQBY_ROW		10
#define		B_REQBY_COL		14
#define		B_PHONE_ROW		11
#ifdef SCANDINAVIA
#define		B_PH_AREA_CODE_COL	14
#else
#define		B_PH_AREA_CODE_COL	15
#endif
#define		B_PH_NUM1_COL		20
#define		B_PH_NUM2_COL		24
#define		B_PH_EXT_COL		32
#define		B_PKUP_DATE_ROW		12
#define		B_PKUP_DATE_COL		14
#define		B_PKUP_TIME_ROW		12
#define		B_PKUP_TIME_COL		29
#define		B_ENTERED_BY_COL	40
#define		B_DEST_ADDRESS_ROW	6
#ifdef SCANDINAVIA
#define		B_DEST_ST_NAME_COL	92
#define		B_DEST_ST_NO_COL	114
#define		B_DEST_PRE_DIR_COL	0
#define		B_DEST_ST_TYPE_COL	0
#define		B_DEST_POST_DIR_COL	0
#else
#define		B_DEST_ST_NO_COL	92
#define		B_DEST_PRE_DIR_COL	100
#define		B_DEST_ST_NAME_COL	103
#define		B_DEST_ST_TYPE_COL	124
#define		B_DEST_POST_DIR_COL	128
#endif
#define		B_DEST_APT_ROW		7 
#define		B_DEST_APT_COL		92
#define		B_DEST_CITY_ROW		8
#define		B_DEST_CITY_COL		92
#define		B_DEST_ZONE_ROW		8
#define		B_DEST_ZONE_COL		105
#define         B_VIA_ADDRESS_ROW       10
#define         B_VIA_ADDRESS_COL       92
#define		B_PERSONNEL_ROW		11
#define		B_PERSONEL1_COL		92
#define		B_PERSONEL2_COL		97
#define		B_CAR_ROW		12
#define		B_CAR_COL		92
#define		B_CAR_ATTR1_COL		94
#define		B_CAR_ATTR2_COL		97
#define		B_CAR_ATTR3_COL		100
#define		B_CAR_ATTR4_COL		103
#define		B_CAR_ATTR5_COL		106
#define		B_CAR_ATTR6_COL		109
#define		B_CAR_ATTR7_COL		112
#define		B_CAR_ATTR8_COL		115
#define		B_DRIVER_ROW		13
#define		B_DRIVER_COL		92
#define		B_DRIVER_ATTR1_COL	94
#define		B_DRIVER_ATTR2_COL	97
#define		B_DRIVER_ATTR3_COL	100
#define		B_DRIVER_ATTR4_COL	103
#define		B_DRIVER_ATTR5_COL	106
#define		B_DRIVER_ATTR6_COL	109
#define		B_DRIVER_ATTR7_COL	112
#define		B_DRIVER_ATTR8_COL	115
#define		B_CAR_NO_ROW		14
#define		B_CAR_NO_COL		92
#define		B_PRIORITY_ROW		15
#define		B_PRIORITY_COL		92
#define		B_RATE_ROW		16
#define		B_RATE_COL		92
#define         B_VAT_ROW               16
#define         B_VAT_COL               113
#define		B_LOCAL_ROW		17
#define		B_LOCAL_COL		127
#define		B_COMMENT_ROW		17
#define		B_COMMENT_COL		11
#define 	B_ADVISE_ARRIVAL_ROW 17
#define 	B_ADVISE_ARRIVAL_COL 93
#ifdef NAMERICA
#define         B_SEND_DEST_ADDR_ROW 18
#define         B_SEND_DEST_ADDR_COL 93
#define         B_SEND_RATE_ROW      18
#define         B_SEND_RATE_COL      113
#endif
#define         B_HOLDLINE_ROW          17
#define         B_HOLDLINE_COL          113
/* definitions for rows and columns for entry of various fields
 * on a EXTENDED_CALL_SCR and SUBS_EXTENDED_SCR
 */
#define		E_FLEET_ROW		3
#define		E_FLEET_COL		14
#define		E_CL_TYPE_ROW		3
#define		E_CL_TYPE_COL		24
#define         E_CASH_ACCT_ROW         3
#define         E_CASH_ACCT_COL         52
#define         E_INV_ROW               3
#define         E_INV1_COL              92
#define         E_INV2_COL              101
#define         E_INV3_COL              110
#define         E_INV4_COL              119
#define		E_CHARGE_ROW		4
#define		E_CHARGE_COL		14
#define		E_CONAME1_ROW		4
#define		E_CONAME2_ROW		5
#define		E_CONAME3_ROW		6
#define		E_CONAME_COL		52
#define		E_PKUP_ADDRESS_ROW	10
#define   E_INTERS_ADDRESS_ROW  10
/* SamPlan row and column definitions ENH0002 */
#define   E_SP_CL_MSG_ROW   3
#define   E_SP_CL_MSG_COL   56
#ifdef SCANDINAVIA
#define		E_PKUP_ST_NAME_COL	14
#define		E_PKUP_ST_NO_COL	36
#define		E_PKUP_PRE_DIR_COL	0
#define		E_PKUP_ST_TYPE_COL	0
#define		E_PKUP_POST_DIR_COL	0
#define		E_INTERS_PRE_DIR_COL 	0
#define		E_INTERS_ST_NAME_COL	14
#define		E_INTERS_ST_TYPE_COL	0
#define		E_INTERS_POST_DIR_COL	0
#else
#define		E_PKUP_ST_NO_COL	14
#define		E_PKUP_PRE_DIR_COL	22
#define		E_PKUP_ST_NAME_COL	25
#define		E_PKUP_ST_TYPE_COL	46
#define		E_PKUP_POST_DIR_COL	50
#define		E_INTERS_PRE_DIR_COL 	22
#define		E_INTERS_ST_NAME_COL	25
#define		E_INTERS_ST_TYPE_COL	46
#define		E_INTERS_POST_DIR_COL	50
#endif
#define		E_PKUP_APT_ROW		11
#define		E_PKUP_APT_COL		14
#define		E_PKUP_CITY_ROW		12
#define		E_PKUP_CITY_COL		14
#define		E_PKUP_ZONE_ROW		12
#define		E_PKUP_ZONE_COL		25
#define		E_PKUP_Z_DESC_COL	31
#define		E_PASSENGER_ROW		13
#define		E_PASSENGER_COL		14
#define		E_REQBY_ROW		14
#define		E_REQBY_COL		14
#define		E_PHONE_ROW		15
#ifdef SCANDINAVIA
#define		E_PH_AREA_CODE_COL	14
#else
#define		E_PH_AREA_CODE_COL	15
#endif
#define		E_PH_NUM1_COL		19
#define		E_PH_NUM2_COL		23
#define		E_PH_EXT_COL		32
#define		E_PKUP_DATE_ROW		16
#define		E_PKUP_DATE_COL		14
#define		E_PKUP_TIME_ROW		16
#define		E_PKUP_TIME_COL		26
#define		E_COMMENT_ROW		17
#define		E_COMMENT_COL		11
#define		E_DEST_ADDRESS_ROW	10
#ifdef SCANDINAVIA
#define		E_DEST_ST_NAME_COL	92
#define		E_DEST_ST_NO_COL	114
#define		E_DEST_PRE_DIR_COL	0
#define		E_DEST_ST_TYPE_COL	0
#define		E_DEST_POST_DIR_COL	0
#else
#define		E_DEST_ST_NO_COL	92
#define		E_DEST_PRE_DIR_COL	100
#define		E_DEST_ST_NAME_COL	103
#define		E_DEST_ST_TYPE_COL	124
#define		E_DEST_POST_DIR_COL	128
#endif
#define		E_DEST_APT_ROW		11
#define		E_DEST_APT_COL		92
#define		E_DEST_CITY_ROW		12
#define		E_DEST_CITY_COL		92
#define		E_DEST_ZONE_ROW		12
#define		E_DEST_ZONE_COL		104
#define		E_PERSONEL_ROW		4
#define		E_PERSONEL1_COL		92
#define		E_PERSONEL2_COL		97
#define		E_CAR_ROW		5
#define		E_CAR_COL		92
#define		E_CAR_ATTR1_COL		94
#define		E_CAR_ATTR2_COL		97
#define		E_CAR_ATTR3_COL		100
#define		E_CAR_ATTR4_COL		103
#define		E_CAR_ATTR5_COL		106
#define		E_CAR_ATTR6_COL		109
#define		E_CAR_ATTR7_COL		112
#define		E_CAR_ATTR8_COL		115
#define		E_DRIVER_ROW		6
#define		E_DRIVER_COL		92
#define		E_DRIVER_ATTR1_COL	94
#define		E_DRIVER_ATTR2_COL	97
#define		E_DRIVER_ATTR3_COL	100
#define		E_DRIVER_ATTR4_COL	103
#define		E_DRIVER_ATTR5_COL	106
#define		E_DRIVER_ATTR6_COL	109
#define		E_DRIVER_ATTR7_COL	112
#define		E_DRIVER_ATTR8_COL	115
#define		E_CAR_NO_ROW		7
#define		E_CAR_NO_COL		92
#define		E_PRIORITY_ROW		15
#define		E_PRIORITY_COL		92
#define		E_HOLDLINE_ROW		17
#define		E_HOLDLINE_COL		113
#define		E_LOCAL_ROW		17
#define		E_LOCAL_COL		127
#define		E_SEE_ROW		13
#define		E_SEE_COL		92
#define		E_POD_ROW		14
#define		E_POD1_COL		92
#define		E_POD2_COL		94
#define		E_POD_TIME_COL		115
#define		E_RATE_ROW		16
#define		E_RATE_COL		92
#define         E_VAT_ROW               16
#define         E_VAT_COL               113
#define 	E_ADVISE_ARRIVAL_ROW 17
#define  	E_ADVISE_ARRIVAL_COL 93
#define         E_VIA_ADDRESS_ROW       18
#define         E_VIA_ADDRESS_COL       93
#ifdef NAMERICA
#define         E_SEND_DEST_ADDR_ROW 18
#define         E_SEND_DEST_ADDR_COL 113
#define         E_SEND_RATE_COL      127
#define         E_SEND_RATE_ROW      18
#endif
/* defines for the fields on the lines for subscription (lines 20-23). */
#define		SUBS_ROW1		19
#define		SUBS_ROW2		20
#define		SUBS_ROW3		21
#define		SUBS_ROW4		22
#define		SUBS_MON_COL		21
#define		SUBS_TUE_COL		28
#define		SUBS_WED_COL		35
#define		SUBS_THUR_COL		42
#define		SUBS_FRI_COL		49
#define		SUBS_SAT_COL		56
#define		SUBS_SUN_COL		63
#define		SUBS_BEGINDATE_COL	90
#define		SUBS_ENDDATE_COL	90
/* define the fields for a wakeup call entry screen */
#define		W_FLEET_ROW		3
#define		W_NAME_ROW		5
#define		W_PKUP_DATE_ROW		7
#define		W_PHONE_ROW		9
#ifdef SCANDINAVIA
#define		W_PH_AREA_CODE_COL	14
#else
#define		W_PH_AREA_CODE_COL	15
#endif
#define		W_COMMENT_ROW		17
/* define the max. number of characters that cana be entered for each 
 * of the fields on the BASIC_CALL_SCR and the SUBS_BASIC_SCR and
 * the EXTENDED_CALL_SCR and the SUBS_EXTENDED_SCR
 */
#define		CHARS_FLEET		1
#define		CHARS_TYPE		3
#define         CHARS_CASH_ACCT         1
#define         CHARS_INV               8
#define         CHARS_VIA_ADDRESS       1
#ifdef NAMERICA
/* NAMERICA has a suffix, SCANDINAVIA does not */
#define		CHARS_ST_NO		7
#else
#define		CHARS_ST_NO		6
#endif
#define		CHARS_ST_DIR		2
#define		CHARS_ST_NAME		20
#define		CHARS_ST_TYPE		3
#define		CHARS_APT		5
#define		CHARS_CITY		3
#define		CHARS_ZONE		3
#define		CHARS_ZONE_DESC		20
#define		CHARS_PASSENGER		20
#define		CHARS_PH_AREA_CODE	3
#define		CHARS_PH_NUM1		3
#define		CHARS_PH_NUM2		4
#define		CHARS_PH_EXT		4
#define		CHARS_DATE		9	
#define		CHARS_TIME		6
#define		CHARS_COMMENT		64
#define		CHARS_PERSONEL1		4
#define		CHARS_PERSONEL2		1
#define		CHARS_CAR		2
#define		CHARS_CAR_ATTR		1
#define		CHARS_DRIVER		1
#define		CHARS_DRV_ATTR		1
#define		CHARS_CAR_NO		1
#define		CHARS_CL_PRIORITY	2
#define		CHARS_RATE		7   /* VERIFY MODE OF THIS INPUT */
#define         CHARS_VAT               5
#define		CHARS_HOLDLINE		4
#define		CHARS_LOCAL		1
#define		CHARS_PHONE_NUMBER	18
#define		CHARS_ADVISE_ARRIVAL 1
#define         CHARS_SEND_DEST_ADDR 1
#define         CHARS_SEND_RATE      1
/* on the extended screen */
#define		CHARS_CHARGE		21
#define		CHARS_CONAME		25
#define		CHARS_REQBY		20
#define		CHARS_SEE		20
#define		CHARS_POD1		1
#define		CHARS_PODNAME		20
#define		CHARS_POD_TIME		6
/* on the subscription lines (20-23) */
#define		CHARS_SUBS_TIME		6
#define		CHARS_SUBS_BEGINDATE	9
#define		CHARS_SUBS_TMCHG	6
#define		CHARS_SUBS_ENDDATE	9
/* define the positions of the fields on the BASIC_CALL_SCR and the
 * SUBS_BASIC_SCR
 */
#define		B_FLEET			1	/* 1st field on the screen */
#define		B_TYPE			2
#ifdef SCANDINAVIA
#define		B_PKUP_ST_NAME		(B_TYPE + 1)
#define		B_PKUP_ST_NO		(B_PKUP_ST_NAME + 1)
#define		B_DUMMY_1		(B_PKUP_ST_NO + 1)
#define		B_DUMMY_2		(B_DUMMY_1 + 1)
#define		B_DUMMY_3		(B_DUMMY_2 + 1)
#define		B_DUMMY_4		(B_DUMMY_3 + 1)
#define		B_INTERS_ST_NAME	(B_DUMMY_4 + 1)
#define		B_DUMMY_5       	(B_INTERS_ST_NAME + 1)
#define		B_DUMMY_6		(B_DUMMY_5 + 1)
#define		B_PKUP_APT_NO		(B_DUMMY_6 + 1)
#else
#define		B_PKUP_ST_NO		(B_TYPE + 1)
#define		B_PKUP_PRE_DIR		(B_PKUP_ST_NO + 1)
#define		B_PKUP_ST_NAME		(B_PKUP_PRE_DIR + 1)
#define		B_PKUP_ST_TYPE		(B_PKUP_ST_NAME + 1)
#define		B_PKUP_POST_DIR		(B_PKUP_ST_TYPE + 1)
#define		B_INTERS_PRE_DIR	(B_PKUP_POST_DIR + 1)
#define		B_INTERS_ST_NAME	(B_INTERS_PRE_DIR + 1)
#define		B_INTERS_ST_TYPE	(B_INTERS_ST_NAME + 1)
#define		B_INTERS_POST_DIR	(B_INTERS_ST_TYPE + 1)
#define		B_PKUP_APT_NO		(B_INTERS_POST_DIR + 1)
#endif
#define		B_PKUP_CITY		(B_PKUP_APT_NO + 1)
#define		B_PKUP_ZONE		(B_PKUP_CITY + 1)
#define		B_PASSENGER		(B_PKUP_ZONE + 1)
#define         B_REQBY                 (B_PASSENGER + 1)
#define		B_PH_AREA_CODE		(B_REQBY + 1)
#define		B_PH_NUM1		(B_PH_AREA_CODE + 1)
#define		B_PH_NUM2		(B_PH_NUM1 + 1)
#define		B_PH_EXT		(B_PH_NUM2 + 1)
#define		B_DATE			(B_PH_EXT + 1)
#define		B_TIME			(B_DATE + 1)
#define		B_COMMENT		(B_TIME + 1)
#ifdef SCANDINAVIA
#define		B_DEST_ST_NAME		(B_COMMENT + 1)
#define		B_DEST_ST_NO		(B_DEST_ST_NAME + 1)
#define		B_DUMMY_7		(B_DEST_ST_NO + 1)
#define		B_DUMMY_8		(B_DUMMY_7 + 1)
#define		B_DUMMY_9		(B_DUMMY_8 + 1)
#else
#define		B_DEST_ST_NO		(B_COMMENT + 1)
#define		B_DEST_PRE_DIR		(B_DEST_ST_NO + 1)
#define		B_DEST_ST_NAME		(B_DEST_PRE_DIR + 1)
#define		B_DEST_ST_TYPE		(B_DEST_ST_NAME + 1)
#define		B_DEST_POST_DIR		(B_DEST_ST_TYPE + 1)
#endif
#ifdef SCANDINAVIA
#define		B_DEST_APT_NO		(B_DUMMY_9 + 1)
#define		B_DEST_CITY		(B_DEST_APT_NO + 1)
#else
#define		B_DEST_APT_NO		(B_DEST_POST_DIR + 1)
#define		B_DEST_CITY		(B_DEST_APT_NO + 1)
#endif
#define		B_DEST_ZONE		(B_DEST_CITY + 1)
#define         B_VIA_ADDRESS           (B_DEST_ZONE + 1)
#define		B_PERSONEL1		(B_VIA_ADDRESS + 1)
#define		B_PERSONEL2		(B_PERSONEL1 + 1)
#define		B_CAR_NEEDS		(B_PERSONEL2 + 1)
//#define		B_DRV_NEEDS		(B_CAR_NEEDS + 1)
#define		B_CAR_NO		(B_CAR_NEEDS + 1)
#define		B_PRIORITY		(B_CAR_NO + 1)
#define		B_RATE			(B_PRIORITY + 1)
#define         B_VAT                   (B_RATE + 1)
#define		B_ADVISE_ARRIVAL	(B_VAT + 1)
#define		B_HOLDLINE		(B_ADVISE_ARRIVAL + 1)
#define		B_LOCAL                 (B_HOLDLINE + 1)
#ifdef NAMERICA
#define         B_SEND_DEST_ADDR        (B_LOCAL + 1)
#define         B_SEND_RATE             (B_SEND_DEST_ADDR + 1)
#define         B_MON_TIME              (B_SEND_RATE + 1)
#else
/* for the subscription lines on the subscription basic call entry screen */
#define		B_MON_TIME		(B_LOCAL + 1)
#endif
#define		B_TUE_TIME		(B_MON_TIME + 1)
#define		B_WED_TIME		(B_TUE_TIME + 1)
#define		B_THUR_TIME		(B_WED_TIME + 1)
#define		B_FRI_TIME		(B_THUR_TIME + 1)
#define		B_SAT_TIME		(B_FRI_TIME + 1)
#define		B_SUN_TIME		(B_SAT_TIME + 1)
#define		B_BEGIN_DATE		(B_SUN_TIME + 1)
#define		B_MON_CHG		(B_BEGIN_DATE + 1)
#define		B_TUE_CHG		(B_MON_CHG + 1)
#define		B_WED_CHG		(B_TUE_CHG + 1)
#define		B_THUR_CHG		(B_WED_CHG + 1)
#define		B_FRI_CHG		(B_THUR_CHG + 1)
#define		B_SAT_CHG		(B_FRI_CHG + 1)
#define		B_SUN_CHG		(B_SAT_CHG + 1)
#define		B_END_DATE		(B_SUN_CHG + 1)
/* define the positions of the fields on the EXTENDED_CALL_SCR and the
 * SUBS_EXTENDED_SCR
 */
#define		E_FLEET			1	/* 1st field on the screen */
#define		E_TYPE			(E_FLEET + 1)
#ifdef SCANDINAVIA
#define         E_CASH_ACCT             (E_TYPE + 1)
#define         E_CHARGE                (E_CASH_ACCT + 1)
#else
#define         E_CHARGE                (E_TYPE + 1)
#endif
#define		E_CO_NAME1		(E_CHARGE + 1)
#define		E_CO_NAME2		(E_CO_NAME1 + 1)
#define		E_CO_NAME3		(E_CO_NAME2 + 1)
#ifdef SCANDINAVIA
#define		E_PKUP_ST_NAME		(E_CO_NAME3 + 1)
#define		E_PKUP_ST_NO		(E_PKUP_ST_NAME + 1)
#define		E_DUMMY_1		(E_PKUP_ST_NO + 1)
#define		E_DUMMY_2		(E_DUMMY_1 + 1)
#define		E_DUMMY_3		(E_DUMMY_2 + 1)
#define		E_DUMMY_4		(E_DUMMY_3 + 1)
#define		E_INTERS_ST_NAME	(E_DUMMY_4 + 1)
#define		E_DUMMY_5		(E_INTERS_ST_NAME + 1)
#define		E_DUMMY_6		(E_DUMMY_5 + 1)
#define		E_PKUP_APT_NO		(E_DUMMY_6 + 1)
#else
#define         E_PKUP_ST_NO            (E_CO_NAME3 + 1)
#define		E_PKUP_PRE_DIR		(E_PKUP_ST_NO + 1)
#define		E_PKUP_ST_NAME		(E_PKUP_PRE_DIR + 1)
#define		E_PKUP_ST_TYPE		(E_PKUP_ST_NAME + 1)
#define		E_PKUP_POST_DIR		(E_PKUP_ST_TYPE + 1)
#define		E_INTERS_PRE_DIR	(E_PKUP_POST_DIR + 1)
#define		E_INTERS_ST_NAME	(E_INTERS_PRE_DIR + 1)
#define		E_INTERS_ST_TYPE	(E_INTERS_ST_NAME + 1)
#define		E_INTERS_POST_DIR	(E_INTERS_ST_TYPE + 1)
#define		E_PKUP_APT_NO		(E_INTERS_POST_DIR + 1)
#endif
#define		E_PKUP_CITY		(E_PKUP_APT_NO + 1)
#define		E_PKUP_ZONE		(E_PKUP_CITY + 1)
#define		E_PASSENGER		(E_PKUP_ZONE + 1)
#define		E_REQ_BY		(E_PASSENGER + 1)
#define		E_PH_AREA_CODE		(E_REQ_BY + 1)
#define		E_PH_NUM1		(E_PH_AREA_CODE + 1)
#define		E_PH_NUM2		(E_PH_NUM1 + 1)
#define		E_PH_EXT		(E_PH_NUM2 + 1)
#define		E_DATE			(E_PH_EXT + 1)
#define		E_TIME			(E_DATE + 1)
#define		E_COMMENT		(E_TIME + 1)
#define         E_INV1                  (E_COMMENT + 1)
#define         E_INV2                  (E_INV1 + 1)
#define         E_INV3                  (E_INV2 + 1)
#define         E_INV4                  (E_INV3 + 1)
#define		E_PERSONEL1		(E_INV4 + 1)
#define		E_PERSONEL2		(E_PERSONEL1 + 1)
#define		E_CAR_NEEDS		(E_PERSONEL2 + 1)
//#define		E_DRV_NEEDS		(E_CAR_NEEDS + 1)
#define		E_CAR_NO		(E_CAR_NEEDS + 1)
#ifdef SCANDINAVIA
#define		E_DEST_ST_NAME		(E_CAR_NO + 1)
#define		E_DEST_ST_NO		(E_DEST_ST_NAME + 1)
#define		E_DUMMY_7		(E_DEST_ST_NO + 1)
#define		E_DUMMY_8		(E_DUMMY_7 + 1)
#define		E_DUMMY_9		(E_DUMMY_8 + 1)
#define         E_DEST_APT_NO           (E_DUMMY_9 + 1)
#else
#define		E_DEST_ST_NO		(E_CAR_NO + 1)
#define		E_DEST_PRE_DIR		(E_DEST_ST_NO + 1)
#define		E_DEST_ST_NAME		(E_DEST_PRE_DIR + 1)
#define		E_DEST_ST_TYPE		(E_DEST_ST_NAME + 1)
#define		E_DEST_POST_DIR		(E_DEST_ST_TYPE + 1)
#define         E_DEST_APT_NO           (E_DEST_POST_DIR + 1)
#endif
#define		E_DEST_CITY		(E_DEST_APT_NO + 1)
#define		E_DEST_ZONE		(E_DEST_CITY + 1)
#define		E_SEE			(E_DEST_ZONE + 1)
#define		E_POD			(E_SEE + 1)
#define		E_PODNAME		(E_POD + 1)
#define		E_POD_TIME		(E_PODNAME + 1)
#define		E_PRIORITY		(E_POD_TIME + 1)
#define		E_RATE			(E_PRIORITY + 1)
#define         E_VAT                   (E_RATE + 1)
#define		E_ADVISE_ARRIVAL        (E_VAT + 1)
#define		E_HOLDLINE		(E_ADVISE_ARRIVAL + 1)
#define		E_LOCAL			(E_HOLDLINE + 1)
#define         E_VIA_ADDRESS           (E_LOCAL + 1)
#ifdef NAMERICA
#define         E_SEND_DEST_ADDR        (E_VIA_ADDRESS + 1)
#define         E_SEND_RATE             (E_SEND_DEST_ADDR + 1)
#define         E_MON_TIME              (E_SEND_RATE + 1)
#else
#define         E_MON_TIME              (E_VIA_ADDRESS + 1)
#endif
/* for the subscription lines on the subscription extended call entry screen */
#define		E_TUE_TIME		(E_MON_TIME + 1)
#define		E_WED_TIME		(E_TUE_TIME + 1)
#define		E_THUR_TIME		(E_WED_TIME + 1)
#define		E_FRI_TIME		(E_THUR_TIME + 1)
#define		E_SAT_TIME		(E_FRI_TIME + 1)
#define		E_SUN_TIME		(E_SAT_TIME + 1)
#define		E_BEGIN_DATE		(E_SUN_TIME + 1)
#define		E_MON_CHG		(E_BEGIN_DATE + 1)
#define		E_TUE_CHG		(E_MON_CHG + 1)
#define		E_WED_CHG		(E_TUE_CHG + 1)
#define		E_THUR_CHG		(E_WED_CHG + 1)
#define		E_FRI_CHG		(E_THUR_CHG + 1)
#define		E_SAT_CHG		(E_FRI_CHG + 1)
#define		E_SUN_CHG		(E_SAT_CHG + 1)
#define		E_END_DATE		(E_SUN_CHG + 1)
/* for the wakeup call entry screen */
#define		W_FLEET			1
#define		W_NAME			2
#define		W_DATE			3
#define		W_TIME			4
#define		W_PH_AREA_CODE		5
#define		W_PH_NUM1		6
#define		W_PH_NUM2		7
#define		W_PH_EXT		8
#define		W_COMMENT		9
/* on the subscription overlay */
#define		W_MON_TIME		10
#define		W_TUW_TIME		11
#define		W_WED_TIME		12
#define		W_THUR_TIME		13
#define		W_FRI_TIME		14
#define		W_SAT_TIME		15
#define		W_SUN_TIME		16
#define		W_BEGIN_DATE		17
#define		W_MON_CHG		18
#define		W_TUW_CHG		19
#define		W_WED_CHG		20
#define		W_THUR_CHG		21
#define		W_FRI_CHG		22
#define		W_SAT_CHG		23
#define		W_SUN_CHG		24
#define		W_END_DATE		25
/* add defines for printing the pkup and dest address comment on basic
 * and extended call entry screens 
 */
#define		B_PKUP_ADDR_CMNT_ROW	4
#define		B_PKUP_ADDR_CMNT_COL	22
#define		B_DEST_ADDR_CMNT_ROW	4
#define		B_DEST_ADDR_CMNT_COL	100
#define		E_PKUP_ADDR_CMNT_ROW	8
#define		E_PKUP_ADDR_CMNT_COL	22
#define		E_DEST_ADDR_CMNT_ROW	8
#define		E_DEST_ADDR_CMNT_COL	100
#define         B_CT_CMNT_COL           14
#define         B_CT_CMNT_ROW           5
#define         E_CT_CMNT_COL           14
#define         E_CT_CMNT_ROW           9
#define		CHARS_ADDR_CMNT		30
