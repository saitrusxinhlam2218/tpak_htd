/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  scr_flds.h; Rel: 10.1.2.1; Get date: 4/20/95 at 11:48:58
* @(#)  Last delta: 12/2/94 at 18:16:18
* @(#)  SCCS File: /taxi/sccs/s.scr_flds.h
*                                                                           *
* Copyright (c) 1990 - MDI Seattle                                          *
*                                                                           *
*****************************************************************************/
#ident "@(#) head:scr_flds.h	10.1.2.1"

/* scr_flds.h: conatins global declarations of structures used by 
 *		read_field() routine for reading fields on various 
 *		screens
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

/* declare structure for reading fields on a basic call entry screen */
struct scr_flds basic_scr_flds[] = {
	{ 0, 0, 1, 1},
	{B_FLEET_ROW, B_FLEET_COL, CHARS_FLEET, B_TYPE},
	{B_CL_TYPE_ROW, B_CL_TYPE_COL, CHARS_TYPE, B_FLEET},
#ifdef SCANDINAVIA
	{B_PKUP_ADDRESS_ROW,B_PKUP_ST_NAME_COL, CHARS_ST_NAME, B_DEST_ST_NAME},
	{B_PKUP_ADDRESS_ROW, B_PKUP_ST_NO_COL, CHARS_ST_NO, B_DEST_ST_NAME},
	{0, 0, 0, 0},
	{0, 0, 0, 0},
	{0, 0, 0, 0},
	{0, 0, 0, 0},
	{B_INTERS_ROW, B_INTERS_ST_NAME_COL, CHARS_ST_NAME, B_DEST_ST_NAME},
	{0, 0, 0, 0},
	{0, 0, 0, 0},
#else
	{B_PKUP_ADDRESS_ROW, B_PKUP_ST_NO_COL, CHARS_ST_NO, B_DEST_ST_NO},
	{B_PKUP_ADDRESS_ROW, B_PKUP_PRE_DIR_COL, CHARS_ST_DIR, B_DEST_ST_NO},
	{B_PKUP_ADDRESS_ROW,B_PKUP_ST_NAME_COL, CHARS_ST_NAME, B_DEST_ST_NO},
	{B_PKUP_ADDRESS_ROW, B_PKUP_ST_TYPE_COL, CHARS_ST_TYPE, B_DEST_ST_NO},
	{B_PKUP_ADDRESS_ROW, B_PKUP_POST_DIR_COL, CHARS_ST_DIR, B_DEST_ST_NO},
	{B_INTERS_ROW, B_INTERS_PRE_DIR_COL, CHARS_ST_DIR, B_DEST_ST_NO},
	{B_INTERS_ROW, B_INTERS_ST_NAME_COL, CHARS_ST_NAME, B_DEST_ST_NO},
	{B_INTERS_ROW, B_INTERS_ST_TYPE_COL, CHARS_ST_TYPE, B_DEST_ST_NO},
	{B_INTERS_ROW, B_INTERS_POST_DIR_COL, CHARS_ST_DIR, B_DEST_ST_NO},
#endif
	{B_PKUP_APT_ROW, B_PKUP_APT_COL, CHARS_APT, B_DEST_APT_NO},
	{B_PKUP_CITY_ROW, B_PKUP_CITY_COL, CHARS_CITY, B_DEST_CITY},
	{B_PKUP_ZONE_ROW, B_PKUP_ZONE_COL, CHARS_ZONE, B_DEST_CITY},
	{B_PASSENGER_ROW, B_PASSENGER_COL, CHARS_PASSENGER,B_CAR_NEEDS},
        
        {B_REQBY_ROW, B_REQBY_COL, CHARS_REQBY,B_PERSONEL1},
        
#ifdef SCANDINAVIA
	{B_PHONE_ROW, B_PH_AREA_CODE_COL, CHARS_PHONE_NUMBER,B_PERSONEL1},  /* Finland has one long phone # field*/
#else
	{B_PHONE_ROW, B_PH_AREA_CODE_COL, CHARS_PH_AREA_CODE,B_PERSONEL1},
#endif
	{B_PHONE_ROW, B_PH_NUM1_COL, CHARS_PH_NUM1,B_PERSONEL1},
	{B_PHONE_ROW, B_PH_NUM2_COL, CHARS_PH_NUM2,B_PERSONEL1},
	{B_PHONE_ROW, B_PH_EXT_COL, CHARS_PH_EXT,B_PERSONEL1},
	{B_PKUP_DATE_ROW, B_PKUP_DATE_COL, CHARS_DATE,B_CAR_NEEDS},
	{B_PKUP_TIME_ROW, B_PKUP_TIME_COL, CHARS_TIME,B_CAR_NEEDS},
	{B_COMMENT_ROW, B_COMMENT_COL, CHARS_COMMENT,B_ADVISE_ARRIVAL},
#ifdef SCANDINAVIA
	{B_DEST_ADDRESS_ROW, B_DEST_ST_NAME_COL, CHARS_ST_NAME,B_PKUP_ST_NAME},
	{B_DEST_ADDRESS_ROW, B_DEST_ST_NO_COL, CHARS_ST_NO,B_PKUP_ST_NAME},
	{0, 0, 0, 0},
	{0, 0, 0, 0},
	{0, 0, 0, 0},
#else
	{B_DEST_ADDRESS_ROW, B_DEST_ST_NO_COL, CHARS_ST_NO,B_PKUP_ST_NO},
	{B_DEST_ADDRESS_ROW, B_DEST_PRE_DIR_COL, CHARS_ST_DIR,B_PKUP_ST_NO},
	{B_DEST_ADDRESS_ROW, B_DEST_ST_NAME_COL, CHARS_ST_NAME,B_PKUP_ST_NO},
	{B_DEST_ADDRESS_ROW, B_DEST_ST_TYPE_COL, CHARS_ST_TYPE,B_PKUP_ST_NO},
	{B_DEST_ADDRESS_ROW, B_DEST_POST_DIR_COL, CHARS_ST_DIR,B_PKUP_ST_NO},
#endif
	{B_DEST_APT_ROW, B_DEST_APT_COL, CHARS_APT, B_PKUP_APT_NO},
	{B_DEST_CITY_ROW, B_DEST_CITY_COL, CHARS_CITY, B_PKUP_CITY},
	{B_DEST_ZONE_ROW, B_DEST_ZONE_COL, CHARS_ZONE, B_PKUP_CITY},
	{B_VIA_ADDRESS_ROW, B_VIA_ADDRESS_COL, CHARS_VIA_ADDRESS, B_PASSENGER},
	{B_PERSONNEL_ROW, B_PERSONEL1_COL, CHARS_PERSONEL1, B_PH_NUM1},
	{B_PERSONNEL_ROW, B_PERSONEL2_COL, CHARS_PERSONEL2, B_PH_NUM1},
	{B_CAR_ROW, B_CAR_COL, CHARS_CAR, B_DATE},
        //	{B_DRIVER_ROW, B_DRIVER_COL, CHARS_DRIVER, B_DATE},
	{B_CAR_NO_ROW, B_CAR_NO_COL, CHARS_CAR_NO, B_DATE},
	{B_PRIORITY_ROW, B_PRIORITY_COL, CHARS_CL_PRIORITY, B_DATE},
	{B_RATE_ROW, B_RATE_COL, CHARS_RATE, B_DATE},
	{B_VAT_ROW, B_VAT_COL, CHARS_VAT, B_DATE},
	{B_ADVISE_ARRIVAL_ROW, B_ADVISE_ARRIVAL_COL, CHARS_ADVISE_ARRIVAL, B_COMMENT},
	{B_HOLDLINE_ROW, B_HOLDLINE_COL, CHARS_HOLDLINE, B_DATE},
	{B_LOCAL_ROW, B_LOCAL_COL, CHARS_LOCAL, B_DATE},
#ifdef NAMERICA
	{B_SEND_DEST_ADDR_ROW, B_SEND_DEST_ADDR_COL, CHARS_SEND_DEST_ADDR, B_COMMENT},
	{B_SEND_RATE_ROW, B_SEND_RATE_COL, CHARS_SEND_RATE, B_COMMENT},
#endif
	{SUBS_ROW2, SUBS_MON_COL, CHARS_SUBS_TIME, B_BEGIN_DATE},
	{SUBS_ROW2, SUBS_TUE_COL, CHARS_SUBS_TIME, B_BEGIN_DATE},
	{SUBS_ROW2, SUBS_WED_COL, CHARS_SUBS_TIME, B_BEGIN_DATE},
	{SUBS_ROW2, SUBS_THUR_COL, CHARS_SUBS_TIME, B_BEGIN_DATE},
	{SUBS_ROW2, SUBS_FRI_COL, CHARS_SUBS_TIME, B_BEGIN_DATE},
	{SUBS_ROW2, SUBS_SAT_COL, CHARS_SUBS_TIME, B_BEGIN_DATE},
	{SUBS_ROW2, SUBS_SUN_COL, CHARS_SUBS_TIME, B_BEGIN_DATE},
	{SUBS_ROW2, SUBS_BEGINDATE_COL, CHARS_SUBS_BEGINDATE, B_MON_TIME},
	{SUBS_ROW3,SUBS_MON_COL, CHARS_SUBS_TMCHG, B_END_DATE},
	{SUBS_ROW3, SUBS_TUE_COL, CHARS_SUBS_TMCHG, B_END_DATE},
	{SUBS_ROW3,SUBS_WED_COL, CHARS_SUBS_TMCHG, B_END_DATE},
	{SUBS_ROW3, SUBS_THUR_COL, CHARS_SUBS_TMCHG, B_END_DATE},
	{SUBS_ROW3, SUBS_FRI_COL, CHARS_SUBS_TMCHG, B_END_DATE},
	{SUBS_ROW3, SUBS_SAT_COL, CHARS_SUBS_TMCHG, B_END_DATE},
	{SUBS_ROW3, SUBS_SUN_COL, CHARS_SUBS_TMCHG, B_END_DATE},
	{SUBS_ROW3, SUBS_ENDDATE_COL, CHARS_SUBS_ENDDATE, B_MON_CHG},
};

/* declare structure for reading fields on a wakeup call entry screen */
struct scr_flds wakeup_scr_fld[] = {
	{ 0, 0, 1, 1},
	{W_FLEET_ROW, B_FLEET_COL, CHARS_FLEET,W_NAME},
	{W_NAME_ROW, B_PASSENGER_COL, CHARS_PASSENGER, W_DATE},
	{W_PKUP_DATE_ROW, B_PKUP_DATE_COL, CHARS_DATE, W_TIME},
	{W_PKUP_DATE_ROW, B_PKUP_TIME_COL, CHARS_TIME, W_DATE},
#ifdef SCANDINAVIA
	{W_PHONE_ROW, B_PH_AREA_CODE_COL, CHARS_PHONE_NUMBER,W_PH_AREA_CODE},
#else
	{W_PHONE_ROW, B_PH_AREA_CODE_COL, CHARS_PH_AREA_CODE,W_PH_NUM1},
#endif
	{W_PHONE_ROW, B_PH_NUM1_COL, CHARS_PH_NUM1,W_PH_NUM2},
	{W_PHONE_ROW, B_PH_NUM2_COL, CHARS_PH_NUM2,W_PH_EXT},
	{W_PHONE_ROW, B_PH_EXT_COL, CHARS_PH_EXT, W_COMMENT},
	{W_COMMENT_ROW, B_COMMENT_COL, CHARS_COMMENT, W_MON_TIME},
	{SUBS_ROW2, SUBS_MON_COL, CHARS_SUBS_TIME, W_BEGIN_DATE},
	{SUBS_ROW2, SUBS_TUE_COL, CHARS_SUBS_TIME, W_BEGIN_DATE},
	{SUBS_ROW2, SUBS_WED_COL, CHARS_SUBS_TIME, W_BEGIN_DATE},
	{SUBS_ROW2, SUBS_THUR_COL, CHARS_SUBS_TIME, W_BEGIN_DATE},
	{SUBS_ROW2, SUBS_FRI_COL, CHARS_SUBS_TIME, W_BEGIN_DATE},
	{SUBS_ROW2, SUBS_SAT_COL, CHARS_SUBS_TIME,W_BEGIN_DATE},
	{SUBS_ROW2, SUBS_SUN_COL, CHARS_SUBS_TIME, W_BEGIN_DATE},
	{SUBS_ROW2, SUBS_BEGINDATE_COL,CHARS_SUBS_BEGINDATE,W_MON_TIME},
	{SUBS_ROW3,SUBS_MON_COL, CHARS_SUBS_TMCHG, W_END_DATE},
	{SUBS_ROW3, SUBS_TUE_COL, CHARS_SUBS_TMCHG, W_END_DATE},
	{SUBS_ROW3,SUBS_WED_COL, CHARS_SUBS_TMCHG, W_END_DATE},
	{SUBS_ROW3, SUBS_THUR_COL, CHARS_SUBS_TMCHG, W_END_DATE},
	{SUBS_ROW3, SUBS_FRI_COL, CHARS_SUBS_TMCHG, W_END_DATE},
	{SUBS_ROW3, SUBS_SAT_COL, CHARS_SUBS_TMCHG, W_END_DATE},
	{SUBS_ROW3, SUBS_SUN_COL, CHARS_SUBS_TMCHG, W_END_DATE},
	{SUBS_ROW3, SUBS_ENDDATE_COL, CHARS_SUBS_ENDDATE,W_MON_CHG},
};



/* declare structure for reading fields on a basic call entry screen */
struct scr_flds extended_scr_fld[] = {
	{ 0, 0, 1, 1},
	{E_FLEET_ROW, E_FLEET_COL, CHARS_FLEET,E_TYPE},
	{E_CL_TYPE_ROW, E_CL_TYPE_COL, CHARS_TYPE, E_FLEET},
#ifdef SCANDINAVIA
        {E_CASH_ACCT_ROW,E_CASH_ACCT_COL,CHARS_CASH_ACCT, E_INV1},
#endif
	{E_CHARGE_ROW,E_CHARGE_COL,CHARS_CHARGE, E_INV1},
	{E_CONAME1_ROW,E_CONAME_COL,CHARS_CONAME, E_INV1},
	{E_CONAME2_ROW,E_CONAME_COL,CHARS_CONAME, E_INV1},
	{E_CONAME3_ROW,E_CONAME_COL,CHARS_CONAME, E_INV1},
#ifdef SCANDINAVIA
	{E_PKUP_ADDRESS_ROW,E_PKUP_ST_NAME_COL, CHARS_ST_NAME, E_DEST_ST_NAME},
	{E_PKUP_ADDRESS_ROW, E_PKUP_ST_NO_COL, CHARS_ST_NO, E_DEST_ST_NAME},
	{0, 0, 0, 0},
	{0, 0, 0, 0},
	{0, 0, 0, 0},
	{0, 0, 0, 0},
	{E_INTERS_ADDRESS_ROW,E_INTERS_ST_NAME_COL,CHARS_ST_NAME,E_DEST_ST_NAME},
	{0, 0, 0, 0},
	{0, 0, 0, 0},
#else
	{E_PKUP_ADDRESS_ROW, E_PKUP_ST_NO_COL, CHARS_ST_NO, E_DEST_ST_NO},
	{E_PKUP_ADDRESS_ROW, E_PKUP_PRE_DIR_COL, CHARS_ST_DIR, E_DEST_ST_NO},
	{E_PKUP_ADDRESS_ROW,E_PKUP_ST_NAME_COL, CHARS_ST_NAME, E_DEST_ST_NO},
	{E_PKUP_ADDRESS_ROW, E_PKUP_ST_TYPE_COL, CHARS_ST_TYPE, E_DEST_ST_NO},
	{E_PKUP_ADDRESS_ROW, E_PKUP_POST_DIR_COL, CHARS_ST_DIR, E_DEST_ST_NO},
	{E_INTERS_ADDRESS_ROW,E_INTERS_PRE_DIR_COL,CHARS_ST_DIR, E_DEST_ST_NO},
	{E_INTERS_ADDRESS_ROW,E_INTERS_ST_NAME_COL,CHARS_ST_NAME,E_DEST_ST_NO},
	{E_INTERS_ADDRESS_ROW,E_INTERS_ST_TYPE_COL,CHARS_ST_TYPE,E_DEST_ST_NO},
	{E_INTERS_ADDRESS_ROW,E_INTERS_POST_DIR_COL,CHARS_ST_DIR,E_DEST_ST_NO},
#endif
	{E_PKUP_APT_ROW, E_PKUP_APT_COL, CHARS_APT, E_DEST_APT_NO},
	{E_PKUP_CITY_ROW, E_PKUP_CITY_COL, CHARS_CITY, E_DEST_CITY},
	{E_PKUP_ZONE_ROW, E_PKUP_ZONE_COL, CHARS_ZONE, E_DEST_CITY},
	{E_PASSENGER_ROW, E_PASSENGER_COL, CHARS_PASSENGER, E_SEE},
	{E_REQBY_ROW,E_REQBY_COL,CHARS_REQBY, E_POD},
#ifdef SCANDINAVIA
	{E_PHONE_ROW, E_PH_AREA_CODE_COL, CHARS_PHONE_NUMBER, E_PRIORITY},
#else
	{E_PHONE_ROW, E_PH_AREA_CODE_COL, CHARS_PH_AREA_CODE, E_PRIORITY},
#endif
	{E_PHONE_ROW, E_PH_NUM1_COL, CHARS_PH_NUM1, E_PRIORITY},
	{E_PHONE_ROW, E_PH_NUM2_COL, CHARS_PH_NUM2, E_PRIORITY},
	{E_PHONE_ROW, E_PH_EXT_COL, CHARS_PH_EXT, E_PRIORITY},
	{E_PKUP_DATE_ROW, E_PKUP_DATE_COL, CHARS_DATE, E_RATE},
	{E_PKUP_TIME_ROW, E_PKUP_TIME_COL, CHARS_TIME, E_RATE},
	{E_COMMENT_ROW, E_COMMENT_COL, CHARS_COMMENT, E_ADVISE_ARRIVAL},
	{E_INV_ROW, E_INV1_COL, CHARS_INV, E_INV2},
	{E_INV_ROW, E_INV2_COL, CHARS_INV, E_INV3},
	{E_INV_ROW, E_INV3_COL, CHARS_INV, E_INV4},
	{E_INV_ROW, E_INV4_COL, CHARS_INV, E_FLEET},
	{E_PERSONEL_ROW, E_PERSONEL1_COL, CHARS_PERSONEL1, E_CHARGE},
	{E_PERSONEL_ROW, E_PERSONEL2_COL, CHARS_PERSONEL2, E_CHARGE},
	{E_CAR_ROW, E_CAR_COL, CHARS_CAR, E_CO_NAME2},
        //	{E_DRIVER_ROW, E_DRIVER_COL, CHARS_DRIVER, E_CO_NAME3},
	{E_CAR_NO_ROW, E_CAR_NO_COL, CHARS_CAR_NO, E_CHARGE},
#ifdef SCANDINAVIA
	{E_DEST_ADDRESS_ROW, E_DEST_ST_NAME_COL, CHARS_ST_NAME, E_PKUP_ST_NAME},
	{E_DEST_ADDRESS_ROW, E_DEST_ST_NO_COL, CHARS_ST_NO, E_PKUP_ST_NAME},
	{0, 0, 0, 0},
	{0, 0, 0, 0},
	{0, 0, 0, 0},
#else
	{E_DEST_ADDRESS_ROW, E_DEST_ST_NO_COL, CHARS_ST_NO, E_PKUP_ST_NO},
	{E_DEST_ADDRESS_ROW, E_DEST_PRE_DIR_COL, CHARS_ST_DIR, E_PKUP_ST_NO},
	{E_DEST_ADDRESS_ROW, E_DEST_ST_NAME_COL, CHARS_ST_NAME, E_PKUP_ST_NO},
	{E_DEST_ADDRESS_ROW, E_DEST_ST_TYPE_COL, CHARS_ST_TYPE, E_PKUP_ST_NO},
	{E_DEST_ADDRESS_ROW, E_DEST_POST_DIR_COL, CHARS_ST_DIR, E_PKUP_ST_NO},
#endif
	{E_DEST_APT_ROW, E_DEST_APT_COL, CHARS_APT, E_PKUP_APT_NO},
	{E_DEST_CITY_ROW, E_DEST_CITY_COL, CHARS_CITY, E_PKUP_CITY},
	{E_DEST_ZONE_ROW, E_DEST_ZONE_COL, CHARS_ZONE, E_PKUP_CITY},
	{E_SEE_ROW, E_SEE_COL, CHARS_SEE, E_PASSENGER},
	{E_POD_ROW, E_POD1_COL,CHARS_POD1, E_REQ_BY},
	{E_POD_ROW, E_POD2_COL,CHARS_PODNAME, E_REQ_BY},
	{E_POD_ROW,E_POD_TIME_COL,CHARS_TIME, E_REQ_BY},
	{E_PRIORITY_ROW, E_PRIORITY_COL, CHARS_CL_PRIORITY, E_PH_NUM1},
	{E_RATE_ROW, E_RATE_COL, CHARS_RATE, E_DATE},
	{E_VAT_ROW, E_VAT_COL, CHARS_VAT, E_DATE},
	{E_ADVISE_ARRIVAL_ROW, E_ADVISE_ARRIVAL_COL, CHARS_ADVISE_ARRIVAL, E_COMMENT},
	{E_HOLDLINE_ROW, E_HOLDLINE_COL, CHARS_HOLDLINE, E_DATE},
	{E_LOCAL_ROW, E_LOCAL_COL, CHARS_LOCAL, E_DATE},
	{E_VIA_ADDRESS_ROW, E_VIA_ADDRESS_COL, CHARS_VIA_ADDRESS, E_COMMENT},
#ifdef NAMERICA
	{E_SEND_DEST_ADDR_ROW, E_SEND_DEST_ADDR_COL, CHARS_SEND_DEST_ADDR, E_COMMENT_ROW},
	{E_SEND_RATE_ROW, E_SEND_RATE_COL, CHARS_SEND_RATE, E_COMMENT_ROW},
#endif
	{SUBS_ROW2, SUBS_MON_COL, CHARS_SUBS_TIME, E_BEGIN_DATE},
	{SUBS_ROW2, SUBS_TUE_COL, CHARS_SUBS_TIME, E_BEGIN_DATE},
	{SUBS_ROW2, SUBS_WED_COL, CHARS_SUBS_TIME, E_BEGIN_DATE},
	{SUBS_ROW2, SUBS_THUR_COL, CHARS_SUBS_TIME, E_BEGIN_DATE},
	{SUBS_ROW2, SUBS_FRI_COL, CHARS_SUBS_TIME, E_BEGIN_DATE},
	{SUBS_ROW2, SUBS_SAT_COL, CHARS_SUBS_TIME, E_BEGIN_DATE},
	{SUBS_ROW2, SUBS_SUN_COL, CHARS_SUBS_TIME, E_BEGIN_DATE},
	{SUBS_ROW2, SUBS_BEGINDATE_COL, CHARS_SUBS_BEGINDATE, E_MON_TIME},
	{SUBS_ROW3,SUBS_MON_COL, CHARS_SUBS_TMCHG, E_END_DATE},
	{SUBS_ROW3, SUBS_TUE_COL, CHARS_SUBS_TMCHG, E_END_DATE},
	{SUBS_ROW3,SUBS_WED_COL, CHARS_SUBS_TMCHG, E_END_DATE},
	{SUBS_ROW3, SUBS_THUR_COL, CHARS_SUBS_TMCHG, E_END_DATE},
	{SUBS_ROW3, SUBS_FRI_COL, CHARS_SUBS_TMCHG, E_END_DATE},
	{SUBS_ROW3, SUBS_SAT_COL, CHARS_SUBS_TMCHG, E_END_DATE},
	{SUBS_ROW3, SUBS_SUN_COL, CHARS_SUBS_TMCHG, E_END_DATE},
	{SUBS_ROW3, SUBS_ENDDATE_COL, CHARS_SUBS_ENDDATE, E_MON_CHG},
};

