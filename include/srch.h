 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  srch.h; Rel: 10.1.2.1; Get date: 4/20/95 at 11:49:31
 * @(#)  Last delta: 12/2/94 at 18:23:07
 * @(#)  SCCS File: /taxi/sccs/s.srch.h
 *                                                                           *
 *****************************************************************************/
#define		FLEET_MAX		8	
#define		CHARS_STARTDATE		9	
#define		CHARS_STARTTIME		6	
#define		CHARS_STREETNAME	20	
#define		CHARS_PASSENGERNAME	20	
#define		CHARS_CHARGENUMBER	18	
#define		CHARS_PHONENUMBER1	3	
#define		CHARS_PHONENUMBER2	4	
#define		CHARS_PHONENUMBER	7	
#define		CHARS_CALLNUMBER	20	
#define		CHARS_VHNUMBER		4	
#define		CHARS_DRIVERID		6	
#define		CHARS_USERID		4	
#define		CHARS_DRIVERNAME		26
#define         CHARS_ZONE_NBR          3
struct select_criterias {
	char fleet[FLEET_MAX];
	char startdate[CHARS_STARTDATE+1];
	char starttime[CHARS_STARTTIME+1];
	char streetname[CHARS_STREETNAME];
	char passengername[CHARS_PASSENGERNAME];
	char chargenumber[CHARS_CHARGENUMBER];
	char phonenumber1[CHARS_PHONENUMBER1];
	char phonenumber2[CHARS_PHONENUMBER2];
	char phonenumber[CHARS_PHONENUMBER];
	char callnumber[CHARS_CALLNUMBER];
	char vhnumber[CHARS_VHNUMBER];
	char driverid[CHARS_DRIVERID];
	char drivername[CHARS_DRIVERNAME];
	char userid[CHARS_USERID];
	char zone_nbr[CHARS_ZONE_NBR];
};
/* From al_cl.c */
#define		SRCH_ROW0 		0 
#define		SRCH_ROW1 		1 
#define		SRCH_ROW2 		2 
#define		SRCH_ROW3 		3 
#define		SRCH_FLEET1_COL		26
#define		SRCH_FLEET2_COL		28
#define		SRCH_FLEET3_COL		30
#define		SRCH_FLEET4_COL		32
#define		SRCH_FLEET5_COL		34
#define		SRCH_FLEET6_COL		36
#define		SRCH_FLEET7_COL		38
#define		SRCH_FLEET8_COL		40
#define		SRCH_DATE_COL		60
#define		SRCH_TIME_COL		85
#define		SRCH_STREET_COL		40
#define		SRCH_PASSENGER_COL	77
#define		SRCH_PHONE_COL1		76
#define		SRCH_PHONE_COL2		80
#define		SRCH_CHARGE_COL		35
#define		SRCH_CALLNUM_COL	33
#define         SRCH_ZONE_NBR_COL       40
#define		CHARS_PHONE_NUM1	3
#define		CHARS_PHONE_NUM2	4
/* From dr_cl.c */
#define		SRCH_DR_DRIVERID_COL	30
#define		SRCH_DRVNAME_COL	54
/* From unassign.c */
#define		SRCH_UN_VHNUMBER_COL	31
/* From vh_cl.c */
#define		SRCH_VH_VHNUMBER_COL	59
/* From vhdr_cl.c */
#define		SRCH_VHNUMBER_COL	31
#define		SRCH_DRIVERID_COL	56
#ifdef SRCH_DCLS
struct select_criterias select_criteria;
#endif
#ifdef SRCH_EXTERNS
extern struct select_criterias select_criteria;
#endif
#define	NOT_AVAILABLE	99
#define INVALID -1
#define	DATE_SIZE	10
#define TIME_SIZE	7
