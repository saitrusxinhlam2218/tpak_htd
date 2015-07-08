 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  ui_stat.h; Rel: 10.1.2.1; Get date: 4/20/95 at 11:50:27
 * @(#)  Last delta: 12/2/94 at 18:28:24
 * @(#)  SCCS File: /taxi/sccs/s.ui_stat.h
 *                                                                           *
 *****************************************************************************/
#define Z_FLEET_ROW	1						/* starting row of fleet field */
#define Z_FLEET_COL	9						/* starting column of fleet field */
#define Z_ZONE_ROW	5						/* starting row of zone field */
#define Z_ZONE_COL	9						/* starting column of zone field */
#define Z_FLEET		1						/* offset into piu structure for fleet field info */
#define Z_ZONE		2						/* offset into piu structure for zone field info */
#define	MAX_PIU_ZONE	2						/* max number of piu's in the structure */
#define Z_FLEET		1						/* offset into piu structure for fleet field info */
#define Z_ZONE		2						/* offset into piu structure for zone field info */
#define	MAX_PIU_ZONE	2						/* max number of piu's in the structure */
#define S_FLEET_ROW	1						/* starting row of fleet field */
#define S_FLEET_COL	9						/* starting column of fleet field */
#define S_VEH_ROW	1						/* starting row of vehicle field */
#define S_VEH_COL	25						/* starting column of vehicle field */
#define S_DRIV_ROW	1						/* starting row of driver field */
#define S_DRIV_COL	43						/* starting column of driver field */
#define S_FLEET		1						/* offset into piu structure for fleet field info */
#define S_VEH		2						/* offset into piu structure for vehicle field info */
#define S_DRIV		3						/* offset into piu structure for driver field info */
#define	MAX_PIU_SYST	3						/* max number of piu's in the structure */
#define CHARS_VEH	4						/* number of characters in vehicle */
#define CHARS_DRIV	6						/* number of characters in driver */
#define KEY_TAB		'\t'						/* tab key */
#define START_SUM_ROW	7						/* row to start zone summary list */
#define START_SUM_COL	2						/* column to start zone summary list */
#define TO_NEXT_COL	14						/* number of characters to next summary column */
#define TO_NEXT_COL_E4  18                                              /* ENH0004 version of TO_NEXT_COL */
#define LAST_DATA_LINE  22                                              /* last line to print zone summary info */
#define MAX_ZONES_ON_PG	144						/* maximum number of zones to display on a page */
#define MAX_ZONES_ON_PG_E4 112                                          /* ENH0004 version of MAX_ZONES_ON_PG */
#define START_BCK_ROW	2						/* row to start zone detail backup zone list */
#define START_BCK_COL	72						/* column to start zone detail backup zone list */
#define START_PST_ROW	3						/* row to start posted vehicle list */
#define START_PST_COL	19						/* column to start posted vehicle list */
#define START_ENR_ROW	4						/* row to start enroute vehicle list */
#define START_ENR_COL	19						/* column to start enroute vehicle list */
#define TO_NEXT_VEH	5						/* number of characters to next column in vehicle list */
#define COLS_IN_ZBCKUP	4						/* columns in backup zone info */
#define SS_SUM_INFO1	13						/* start of summary data for column 1 */
#define SS_SUM_INFO2	20						/* start of summary data for column 2 */
#define SS_SUM_INFO3	44						/* start of summary data for column 3 */
#define SS_SUM_INFO4	51						/* start of summary data for column 4 */
#define SS_SUM_INFO5	59						/* start of summary data for column 5 */
#define SS_SUM_INFO6	82						/* start of summary data for column 6 */
#define SS_SUM_INFO7	98						/* start of summary data for column 7 */
#define SS_SUM_TEXT1	0						/* start of summary text for column 1 */
#define SS_SUM_TEXT2	32						/* start of summary text for column 2 */
#define SS_SUM_TEXT3	59						/* start of summary text for column 3 */
#define SS_SUM_TEXT4	82						/* start of summary text for column 4 */
#define SS_DET_INFO1	56						/* start of detail data for column 1 */
#define SS_DET_INFO2	71						/* start of detail data for column 2 */
#define SS_DET_INFO3	89						/* start of detail data for column 3 */
#define SS_DET_INFO4	109						/* start of detail data for column 4 */
#define SS_DET_INFO5	128						/* start of detail data for column 5 */
#define SS_DET_TEXT1	44						/* start of detail text for column 1 */
#define SS_DET_TEXT2	63						/* start of detail text for column 2 */
#define SS_DET_TEXT3	76						/* start of detail text for column 3 */
#define SS_DET_TEXT4	94						/* start of detail text for column 4 */
#define SS_DET_TEXT5	114						/* start of detail text for column 5 */
