 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  ui_uud.h; Rel: 10.1.2.1; Get date: 4/20/95 at 11:50:35
 * @(#)  Last delta: 12/2/94 at 18:28:38
 * @(#)  SCCS File: /taxi/sccs/s.ui_uud.h
 *                                                                           *
 *****************************************************************************/
/* ui_uud.h: header file for update_user_defaults screen fields */
#define		MAX_PIU_UUD1	13
#ifdef NAMERICA
#define         MAX_PIU_UUD2    179
#else
#define		MAX_PIU_UUD2	129
#endif
#define		UUD_UD_ROW	3
#define		UUD_FLEET_COL	8
#define		UUD_TYPE_COL	26
#define		UUD_CITY_COL	39
#define		UUD_PHAREA_COL	65
#define		UUD_SCR_COL	82
#define		UUD_SEARCH_COL	99
#define		UUD_EXCEPT_ROW	6
#define		UUD_FLEET1_COL	10
#define		UUD_FLEET2_COL	12
#define		UUD_FLEET3_COL	14
#define		UUD_FLEET4_COL	16
#define		UUD_FLEET5_COL	18
#define		UUD_FLEET6_COL	20
#define		UUD_FLEET7_COL	22
#define		UUD_FLEET8_COL	24
#define		UUD_ZONE1_COL   	22
#define		UUD_ZONE2_COL   	27
#define		UUD_ZONE3_COL   	32
#define		UUD_ZONE4_COL   	37
#define		UUD_ZONE5_COL   	42
#define		UUD_ZONE6_COL   	47
#define		UUD_ZONE7_COL   	52
#define		UUD_ZONE8_COL   	57
#define		UUD_ZONE9_COL   	62
#define		UUD_ZONE10_COL   	67
#define		UUD_ZONE11_COL   	72
#define		UUD_ZONE12_COL   	77
#define		UUD_ZONE13_COL   	82
#define		UUD_ZONE14_COL   	87
#define		UUD_ZONE15_COL   	92
#define		UUD_ZONE16_COL   	97
#define		UUD_GROUPALL_ROW	10
#define		UUD_GROUPALL_COL	11
#define		UUD_GROUP_COL		10
#define		UUD_GROUP1_ROW		11
#define		UUD_GROUP2_ROW		12
#define		UUD_GROUP3_ROW		13
#define		UUD_GROUP4_ROW		14
#define		UUD_GROUP5_ROW		15
#define		UUD_GROUP6_ROW		16
#define		UUD_GROUP7_ROW		17
#define		UUD_GROUP8_ROW		18
#define		UUD_GROUP9_ROW		19
#define		UUD_GROUP10_ROW		20
#define		UUD_GROUP11_ROW		21
#define         UUD_HIA1_ROW            21
#define         UUD_HIA2_ROW            22
#define         UUD_HIA3_ROW            23
#define		UUD_TYPE1_COL		33
#define		UUD_TYPE2_COL		46
#define		UUD_TYPE3_COL		59
#define		UUD_TYPE4_COL		72
#define		UUD_TYPE5_COL		85
#define		UUD_TYPE6_COL		98
#define		UUD_TYPE7_COL		111
#define		UUD_TYPE8_COL		124
/* define max chars allowed on each of the fields */
#define		UUD_FLEET_CHARS		1
#define		UUD_TYPE_CHARS		1
#define		UUD_CITY_CHARS		3
#define		UUD_PHAREA_CHARS	3
#define		UUD_SCREEN_CHARS	3
#define		UUD_SEARCH_CHARS	3
#define		UUD_ZONESUP_CHARS	4
#define		UUD_GROUPTYPE_CHARS	1
/* define constants for formatting screen */
#define		UUD_DISP_TYPE_COL	14
#define		UUD_DISP_CITY_COL	32
#define		UUD_DISP_PHAREA_COL	47
#define		UUD_DISP_SCR_COL	73
#define		UUD_DISP_SRCH_COL	90
#define		UUD_DISP_TYPE1_COL	24
#define		UUD_DISP_TYPE2_COL	37
#define		UUD_DISP_TYPE3_COL	50
#define		UUD_DISP_TYPE4_COL	63
#define		UUD_DISP_TYPE5_COL	76
#define		UUD_DISP_TYPE6_COL	90
#define		UUD_DISP_TYPE7_COL	103
#define		UUD_DISP_TYPE8_COL	116
					/* define the piu structre used for reading in fields on the update_user_defaults screen */
struct scr_flds uud_scr_flds[] = {
		{1,1,1,1},
		{UUD_UD_ROW,UUD_FLEET_COL,UUD_FLEET_CHARS,1},
		{UUD_UD_ROW,UUD_TYPE_COL,UUD_TYPE_CHARS,1},
		{UUD_UD_ROW,UUD_CITY_COL,UUD_CITY_CHARS,1},
		{UUD_UD_ROW,UUD_SCR_COL,UUD_SCREEN_CHARS,1},
		{UUD_UD_ROW,UUD_SEARCH_COL,UUD_SEARCH_CHARS,1},
		{UUD_EXCEPT_ROW-1,UUD_FLEET1_COL,UUD_FLEET_CHARS,1},
		{UUD_EXCEPT_ROW-1,UUD_FLEET2_COL,UUD_FLEET_CHARS,1},
		{UUD_EXCEPT_ROW-1,UUD_FLEET3_COL,UUD_FLEET_CHARS,1},
		{UUD_EXCEPT_ROW-1,UUD_FLEET4_COL,UUD_FLEET_CHARS,1},
		{UUD_EXCEPT_ROW-1,UUD_FLEET5_COL,UUD_FLEET_CHARS,1},
		{UUD_EXCEPT_ROW-1,UUD_FLEET6_COL,UUD_FLEET_CHARS,1},
		{UUD_EXCEPT_ROW-1,UUD_FLEET7_COL,UUD_FLEET_CHARS,1},
		{UUD_EXCEPT_ROW-1,UUD_FLEET8_COL,UUD_FLEET_CHARS,1},
		{UUD_EXCEPT_ROW,UUD_ZONE1_COL,UUD_ZONESUP_CHARS,1},
		{UUD_EXCEPT_ROW,UUD_ZONE2_COL,UUD_ZONESUP_CHARS,1},
		{UUD_EXCEPT_ROW,UUD_ZONE3_COL,UUD_ZONESUP_CHARS,1},
		{UUD_EXCEPT_ROW,UUD_ZONE4_COL,UUD_ZONESUP_CHARS,1},
		{UUD_EXCEPT_ROW,UUD_ZONE5_COL,UUD_ZONESUP_CHARS,1},
		{UUD_EXCEPT_ROW,UUD_ZONE6_COL,UUD_ZONESUP_CHARS,1},
		{UUD_EXCEPT_ROW,UUD_ZONE7_COL,UUD_ZONESUP_CHARS,1},
		{UUD_EXCEPT_ROW,UUD_ZONE8_COL,UUD_ZONESUP_CHARS,1},
		{UUD_EXCEPT_ROW,UUD_ZONE9_COL,UUD_ZONESUP_CHARS,1},
		{UUD_EXCEPT_ROW,UUD_ZONE10_COL,UUD_ZONESUP_CHARS,1},
		{UUD_EXCEPT_ROW,UUD_ZONE11_COL,UUD_ZONESUP_CHARS,1},
		{UUD_EXCEPT_ROW,UUD_ZONE12_COL,UUD_ZONESUP_CHARS,1},
		{UUD_EXCEPT_ROW,UUD_ZONE13_COL,UUD_ZONESUP_CHARS,1},
		{UUD_EXCEPT_ROW,UUD_ZONE14_COL,UUD_ZONESUP_CHARS,1},
		{UUD_EXCEPT_ROW,UUD_ZONE15_COL,UUD_ZONESUP_CHARS,1},
		{UUD_EXCEPT_ROW,UUD_ZONE16_COL,UUD_ZONESUP_CHARS,1},
		{UUD_EXCEPT_ROW+1,UUD_ZONE1_COL,UUD_ZONESUP_CHARS,1},
		{UUD_EXCEPT_ROW+1,UUD_ZONE2_COL,UUD_ZONESUP_CHARS,1},
		{UUD_EXCEPT_ROW+1,UUD_ZONE3_COL,UUD_ZONESUP_CHARS,1},
		{UUD_EXCEPT_ROW+1,UUD_ZONE4_COL,UUD_ZONESUP_CHARS,1},
		{UUD_EXCEPT_ROW+1,UUD_ZONE5_COL,UUD_ZONESUP_CHARS,1},
		{UUD_EXCEPT_ROW+1,UUD_ZONE6_COL,UUD_ZONESUP_CHARS,1},
		{UUD_EXCEPT_ROW+1,UUD_ZONE7_COL,UUD_ZONESUP_CHARS,1},
		{UUD_EXCEPT_ROW+1,UUD_ZONE8_COL,UUD_ZONESUP_CHARS,1},
		{UUD_EXCEPT_ROW+1,UUD_ZONE9_COL,UUD_ZONESUP_CHARS,1},
		{UUD_EXCEPT_ROW+1,UUD_ZONE10_COL,UUD_ZONESUP_CHARS,1},
		{UUD_EXCEPT_ROW+1,UUD_ZONE11_COL,UUD_ZONESUP_CHARS,1},
		{UUD_EXCEPT_ROW+1,UUD_ZONE12_COL,UUD_ZONESUP_CHARS,1},
		{UUD_EXCEPT_ROW+1,UUD_ZONE13_COL,UUD_ZONESUP_CHARS,1},
		{UUD_EXCEPT_ROW+1,UUD_ZONE14_COL,UUD_ZONESUP_CHARS,1},
		{UUD_EXCEPT_ROW+1,UUD_ZONE15_COL,UUD_ZONESUP_CHARS,1},
		{UUD_EXCEPT_ROW+1,UUD_ZONE16_COL,UUD_ZONESUP_CHARS,1},
		{UUD_EXCEPT_ROW+2,UUD_ZONE1_COL,UUD_ZONESUP_CHARS,1},
		{UUD_EXCEPT_ROW+2,UUD_ZONE2_COL,UUD_ZONESUP_CHARS,1},
		{UUD_EXCEPT_ROW+2,UUD_ZONE3_COL,UUD_ZONESUP_CHARS,1},
		{UUD_EXCEPT_ROW+2,UUD_ZONE4_COL,UUD_ZONESUP_CHARS,1},
		{UUD_EXCEPT_ROW+2,UUD_ZONE5_COL,UUD_ZONESUP_CHARS,1},
		{UUD_EXCEPT_ROW+2,UUD_ZONE6_COL,UUD_ZONESUP_CHARS,1},
		{UUD_EXCEPT_ROW+2,UUD_ZONE7_COL,UUD_ZONESUP_CHARS,1},
		{UUD_EXCEPT_ROW+2,UUD_ZONE8_COL,UUD_ZONESUP_CHARS,1},
		{UUD_EXCEPT_ROW+2,UUD_ZONE9_COL,UUD_ZONESUP_CHARS,1},
		{UUD_EXCEPT_ROW+2,UUD_ZONE10_COL,UUD_ZONESUP_CHARS,1},
		{UUD_EXCEPT_ROW+2,UUD_ZONE11_COL,UUD_ZONESUP_CHARS,1},
		{UUD_EXCEPT_ROW+2,UUD_ZONE12_COL,UUD_ZONESUP_CHARS,1},
		{UUD_EXCEPT_ROW+2,UUD_ZONE13_COL,UUD_ZONESUP_CHARS,1},
		{UUD_EXCEPT_ROW+2,UUD_ZONE14_COL,UUD_ZONESUP_CHARS,1},
		{UUD_EXCEPT_ROW+2,UUD_ZONE15_COL,UUD_ZONESUP_CHARS,1},
		{UUD_EXCEPT_ROW+2,UUD_ZONE16_COL,UUD_ZONESUP_CHARS,1},
		{UUD_GROUPALL_ROW,UUD_GROUPALL_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP1_ROW,UUD_GROUP_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP1_ROW,UUD_TYPE1_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP1_ROW,UUD_TYPE2_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP1_ROW,UUD_TYPE3_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP1_ROW,UUD_TYPE4_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP1_ROW,UUD_TYPE5_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP1_ROW,UUD_TYPE6_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP1_ROW,UUD_TYPE7_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP1_ROW,UUD_TYPE8_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP2_ROW,UUD_GROUP_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP2_ROW,UUD_TYPE1_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP2_ROW,UUD_TYPE2_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP2_ROW,UUD_TYPE3_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP2_ROW,UUD_TYPE4_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP2_ROW,UUD_TYPE5_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP2_ROW,UUD_TYPE6_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP2_ROW,UUD_TYPE7_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP2_ROW,UUD_TYPE8_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP3_ROW,UUD_GROUP_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP3_ROW,UUD_TYPE1_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP3_ROW,UUD_TYPE2_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP3_ROW,UUD_TYPE3_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP3_ROW,UUD_TYPE4_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP3_ROW,UUD_TYPE5_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP3_ROW,UUD_TYPE6_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP3_ROW,UUD_TYPE7_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP3_ROW,UUD_TYPE8_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP4_ROW,UUD_GROUP_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP4_ROW,UUD_TYPE1_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP4_ROW,UUD_TYPE2_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP4_ROW,UUD_TYPE3_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP4_ROW,UUD_TYPE4_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP4_ROW,UUD_TYPE5_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP4_ROW,UUD_TYPE6_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP4_ROW,UUD_TYPE7_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP4_ROW,UUD_TYPE8_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP5_ROW,UUD_GROUP_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP5_ROW,UUD_TYPE1_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP5_ROW,UUD_TYPE2_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP5_ROW,UUD_TYPE3_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP5_ROW,UUD_TYPE4_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP5_ROW,UUD_TYPE5_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP5_ROW,UUD_TYPE6_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP5_ROW,UUD_TYPE7_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP5_ROW,UUD_TYPE8_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP6_ROW,UUD_GROUP_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP6_ROW,UUD_TYPE1_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP6_ROW,UUD_TYPE2_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP6_ROW,UUD_TYPE3_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP6_ROW,UUD_TYPE4_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP6_ROW,UUD_TYPE5_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP6_ROW,UUD_TYPE6_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP6_ROW,UUD_TYPE7_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP6_ROW,UUD_TYPE8_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP7_ROW,UUD_GROUP_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP7_ROW,UUD_TYPE1_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP7_ROW,UUD_TYPE2_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP7_ROW,UUD_TYPE3_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP7_ROW,UUD_TYPE4_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP7_ROW,UUD_TYPE5_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP7_ROW,UUD_TYPE6_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP7_ROW,UUD_TYPE7_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP7_ROW,UUD_TYPE8_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP8_ROW,UUD_GROUP_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP8_ROW,UUD_TYPE1_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP8_ROW,UUD_TYPE2_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP8_ROW,UUD_TYPE3_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP8_ROW,UUD_TYPE4_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP8_ROW,UUD_TYPE5_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP8_ROW,UUD_TYPE6_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP8_ROW,UUD_TYPE7_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP8_ROW,UUD_TYPE8_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP9_ROW,UUD_GROUP_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP9_ROW,UUD_TYPE1_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP9_ROW,UUD_TYPE2_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP9_ROW,UUD_TYPE3_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP9_ROW,UUD_TYPE4_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP9_ROW,UUD_TYPE5_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP9_ROW,UUD_TYPE6_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP9_ROW,UUD_TYPE7_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP9_ROW,UUD_TYPE8_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP10_ROW,UUD_GROUP_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP10_ROW,UUD_TYPE1_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP10_ROW,UUD_TYPE2_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP10_ROW,UUD_TYPE3_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP10_ROW,UUD_TYPE4_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP10_ROW,UUD_TYPE5_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP10_ROW,UUD_TYPE6_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP10_ROW,UUD_TYPE7_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP10_ROW,UUD_TYPE8_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP11_ROW,UUD_GROUP_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP11_ROW,UUD_TYPE1_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP11_ROW,UUD_TYPE2_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP11_ROW,UUD_TYPE3_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP11_ROW,UUD_TYPE4_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP11_ROW,UUD_TYPE5_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP11_ROW,UUD_TYPE6_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP11_ROW,UUD_TYPE7_COL,UUD_GROUPTYPE_CHARS,1},
		{UUD_GROUP11_ROW,UUD_TYPE8_COL,UUD_GROUPTYPE_CHARS,1},
#ifdef NAMERICA
		{UUD_HIA1_ROW, UUD_TYPE1_COL, UUD_GROUPTYPE_CHARS, 1},
		{UUD_HIA1_ROW, UUD_TYPE2_COL, UUD_GROUPTYPE_CHARS, 1},
		{UUD_HIA1_ROW, UUD_TYPE3_COL, UUD_GROUPTYPE_CHARS, 1},
		{UUD_HIA1_ROW, UUD_TYPE4_COL, UUD_GROUPTYPE_CHARS, 1},
		{UUD_HIA1_ROW, UUD_TYPE5_COL, UUD_GROUPTYPE_CHARS, 1},
		{UUD_HIA1_ROW, UUD_TYPE6_COL, UUD_GROUPTYPE_CHARS, 1},
		{UUD_HIA1_ROW, UUD_TYPE7_COL, UUD_GROUPTYPE_CHARS, 1},
		{UUD_HIA1_ROW, UUD_TYPE8_COL, UUD_GROUPTYPE_CHARS, 1},
		{UUD_HIA2_ROW, UUD_TYPE1_COL, UUD_GROUPTYPE_CHARS, 1},
		{UUD_HIA2_ROW, UUD_TYPE2_COL, UUD_GROUPTYPE_CHARS, 1},
		{UUD_HIA2_ROW, UUD_TYPE3_COL, UUD_GROUPTYPE_CHARS, 1},
		{UUD_HIA2_ROW, UUD_TYPE4_COL, UUD_GROUPTYPE_CHARS, 1},
		{UUD_HIA2_ROW, UUD_TYPE5_COL, UUD_GROUPTYPE_CHARS, 1},
		{UUD_HIA2_ROW, UUD_TYPE6_COL, UUD_GROUPTYPE_CHARS, 1},
		{UUD_HIA2_ROW, UUD_TYPE7_COL, UUD_GROUPTYPE_CHARS, 1},
		{UUD_HIA2_ROW, UUD_TYPE8_COL, UUD_GROUPTYPE_CHARS, 1},
		{UUD_HIA3_ROW, UUD_TYPE1_COL, UUD_GROUPTYPE_CHARS, 1},
		{UUD_HIA3_ROW, UUD_TYPE2_COL, UUD_GROUPTYPE_CHARS, 1},
#endif
};  /* end uud_scr_flds */
/* declare structure for saving the new default parameters which the user enters on the update_user_defaults_screen */
struct new_def_structs {
	char fleet;			/* new default fleet */
	char call_type;			/* new default call_type */
	char city[4];			/* new default pkup city */
	int screen;			/* new default screen name */
	int search;			/* new default search */
	char excpt_fleet[9];		/* for upto 8 exception fleets */
	int no_of_tty;			/* 1/2 ttys to be used by this user */
	int scr_tty1;			/* screen on tty 1 */
	short zonesup[MAX_SUP_ZONES];	/* for 16 zones/superzones */
	char excpt_groupall;		/* 'Y' = select all exception groups & types */
	char excpt_group[EXC_GROUPS];	/* for upto 12 exception groups */
	char excpt_types[EXC_TYPES];	/* for upto 32 exception types */
	short lead_tm_perc[FLEET_MAX][MAX_SUP_ZONES];	/* % of zone lead time */
	short tm_est[FLEET_MAX][MAX_SUP_ZONES];		/* manual time estimate specified for superzone */
#ifdef NAMERICA
	int channels[18];
#endif
};
/* define field piu values */
#define		UUD_FLEET		1
#define		UUD_CALL_TYPE		2
#define		UUD_CITY		3
#define		UUD_SCREEN		4
#define		UUD_SEARCH		5
#define		UUD_EXCPT_FLEET1	6
#define		UUD_EXCPT_FLEET2	7
#define		UUD_EXCPT_FLEET3	8
#define		UUD_EXCPT_FLEET4	9
#define		UUD_EXCPT_FLEET5	10
#define		UUD_EXCPT_FLEET6	11
#define		UUD_EXCPT_FLEET7	12
#define		UUD_EXCPT_FLEET8	13
#define		UUD_ZONE1		14
#define		UUD_ZONE2		15
#define		UUD_ZONE3		16
#define		UUD_ZONE4		17
#define		UUD_ZONE5		18
#define		UUD_ZONE6		19
#define		UUD_ZONE7		20
#define		UUD_ZONE8		21
#define		UUD_ZONE9		22
#define		UUD_ZONE10		23
#define		UUD_ZONE11		24
#define		UUD_ZONE12		25
#define		UUD_ZONE13		26
#define		UUD_ZONE14		27
#define		UUD_ZONE15		28
#define		UUD_ZONE16		29
#define		UUD_LEAD_TM1		30
#define		UUD_LEAD_TM2		31
#define		UUD_LEAD_TM3		32
#define		UUD_LEAD_TM4		33
#define		UUD_LEAD_TM5		34
#define		UUD_LEAD_TM6		35
#define		UUD_LEAD_TM7		36
#define		UUD_LEAD_TM8		37
#define		UUD_LEAD_TM9		38
#define		UUD_LEAD_TM10		39
#define		UUD_LEAD_TM11		40
#define		UUD_LEAD_TM12		41
#define		UUD_LEAD_TM13		42
#define		UUD_LEAD_TM14		43
#define		UUD_LEAD_TM15		44
#define		UUD_LEAD_TM16		45
#define		UUD_TM_EST1		46
#define		UUD_TM_EST2		47
#define		UUD_TM_EST3		48
#define		UUD_TM_EST4		49
#define		UUD_TM_EST5		50
#define		UUD_TM_EST6		51
#define		UUD_TM_EST7		52
#define		UUD_TM_EST8		53
#define		UUD_TM_EST9		54
#define		UUD_TM_EST10		55
#define		UUD_TM_EST11		56
#define		UUD_TM_EST12		57
#define		UUD_TM_EST13		58
#define		UUD_TM_EST14		59
#define		UUD_TM_EST15		60
#define		UUD_TM_EST16		61
#define		UUD_ALL_GROUPS		62
#define		UUD_GROUP_1		63
#define		UUD_G1_TYPE_1		64
#define		UUD_G1_TYPE_2		65
#define		UUD_G1_TYPE_3		66
#define		UUD_G1_TYPE_4		67
#define		UUD_G1_TYPE_5		68
#define		UUD_G1_TYPE_6		69
#define		UUD_G1_TYPE_7		70
#define		UUD_G1_TYPE_8		71
#define		UUD_GROUP_2		72	
#define		UUD_G2_TYPE_1		73
#define		UUD_G2_TYPE_2		74
#define		UUD_G2_TYPE_3		75
#define		UUD_G2_TYPE_4		76
#define		UUD_G2_TYPE_5		77
#define		UUD_G2_TYPE_6		78
#define		UUD_G2_TYPE_7		79
#define		UUD_G2_TYPE_8		80
#define		UUD_GROUP_3	        81
#define		UUD_G3_TYPE_1		82
#define		UUD_G3_TYPE_2		83
#define		UUD_G3_TYPE_3		84
#define		UUD_G3_TYPE_4		85
#define		UUD_G3_TYPE_5		86
#define		UUD_G3_TYPE_6		87
#define		UUD_G3_TYPE_7		88
#define		UUD_G3_TYPE_8		89
#define		UUD_GROUP_4		90
#define		UUD_G4_TYPE_1		91
#define		UUD_G4_TYPE_2		92
#define		UUD_G4_TYPE_3		93
#define		UUD_G4_TYPE_4		94
#define		UUD_G4_TYPE_5		95
#define		UUD_G4_TYPE_6		96
#define		UUD_G4_TYPE_7		97
#define		UUD_G4_TYPE_8		98
#define		UUD_GROUP_5		99
#define		UUD_G5_TYPE_1		100
#define		UUD_G5_TYPE_2		101
#define		UUD_G5_TYPE_3		102
#define		UUD_G5_TYPE_4		103
#define		UUD_G5_TYPE_5		104
#define		UUD_G5_TYPE_6		105
#define		UUD_G5_TYPE_7		106
#define		UUD_G5_TYPE_8		107
#define		UUD_GROUP_6		108
#define		UUD_G6_TYPE_1		109
#define		UUD_G6_TYPE_2		110
#define		UUD_G6_TYPE_3		111
#define		UUD_G6_TYPE_4		112
#define		UUD_G6_TYPE_5		113
#define		UUD_G6_TYPE_6		114
#define		UUD_G6_TYPE_7		115
#define		UUD_G6_TYPE_8		116
#define		UUD_GROUP_7		117
#define		UUD_G7_TYPE_1		118
#define		UUD_G7_TYPE_2		119
#define		UUD_G7_TYPE_3		120
#define		UUD_G7_TYPE_4		121
#define		UUD_G7_TYPE_5		122
#define		UUD_G7_TYPE_6		123
#define		UUD_G7_TYPE_7		124
#define		UUD_G7_TYPE_8		125
#define		UUD_GROUP_8		126
#define		UUD_G8_TYPE_1		127
#define		UUD_G8_TYPE_2		128
#define		UUD_G8_TYPE_3		129
#define		UUD_G8_TYPE_4		130
#define		UUD_G8_TYPE_5		131
#define		UUD_G8_TYPE_6		132
#define		UUD_G8_TYPE_7		133
#define		UUD_G8_TYPE_8		134
#define		UUD_GROUP_9		135
#define		UUD_G9_TYPE_1		136
#define		UUD_G9_TYPE_2		137
#define		UUD_G9_TYPE_3		138
#define		UUD_G9_TYPE_4		139
#define		UUD_G9_TYPE_5		140
#define		UUD_G9_TYPE_6		141
#define		UUD_G9_TYPE_7		142
#define		UUD_G9_TYPE_8		143
#define		UUD_GROUP_10		144
#define		UUD_G10_TYPE_1		145
#define		UUD_G10_TYPE_2		146
#define		UUD_G10_TYPE_3		147
#define		UUD_G10_TYPE_4		148
#define		UUD_G10_TYPE_5		149
#define		UUD_G10_TYPE_6		150
#define		UUD_G10_TYPE_7		151
#define		UUD_G10_TYPE_8		152
#define		UUD_GROUP_11		153
#define		UUD_G11_TYPE_1		154
#define		UUD_G11_TYPE_2		155
#define		UUD_G11_TYPE_3		156
#define		UUD_G11_TYPE_4		157
#define		UUD_G11_TYPE_5		158
#define		UUD_G11_TYPE_6		159
#define		UUD_G11_TYPE_7		160
#define		UUD_G11_TYPE_8		161
#define         UUD_HIA_TYPE_1          162
#define         UUD_HIA_TYPE_2          163
#define         UUD_HIA_TYPE_3          164
#define         UUD_HIA_TYPE_4          165
#define         UUD_HIA_TYPE_5          166
#define         UUD_HIA_TYPE_6          167
#define         UUD_HIA_TYPE_7          168
#define         UUD_HIA_TYPE_8          169
#define         UUD_HIA_TYPE_9          170
#define         UUD_HIA_TYPE_10         171
#define         UUD_HIA_TYPE_11         172
#define         UUD_HIA_TYPE_12         173
#define         UUD_HIA_TYPE_13         174
#define         UUD_HIA_TYPE_14         175
#define         UUD_HIA_TYPE_15         176
#define         UUD_HIA_TYPE_16         177
#define         UUD_HIA_TYPE_17         178
#define         UUD_HIA_TYPE_18         179
/* define structure for saving info for exception groups & types */
struct excpt_groups {
	char should_process;	/* 'y'/'n' = should/should not process this group on uud screen */
	char group_desc[9];	/* description of the exception group */
	short how_many;		/* # of exception types in this group */
	short excpt_type[12];	/* list of exception types in this group */
	char group_y_n;		/* group selected ('y') or not ('n')  */
	char type_y_n[12];	/* type selected ('y') or not ('n')  */
};
