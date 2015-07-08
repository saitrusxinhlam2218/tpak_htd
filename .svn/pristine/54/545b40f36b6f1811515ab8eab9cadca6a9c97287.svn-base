 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  zgeo.h; Rel: 10.1.2.1; Get date: 4/20/95 at 11:51:25
 * @(#)  Last delta: 12/2/94 at 18:30:23
 * @(#)  SCCS File: /taxi/sccs/s.zgeo.h
 *                                                                           *
 *****************************************************************************/
#define MAXALT 8			/* max number of alternates to be displayed at one time */
#define PICKUP_ADDR	0		/* symbol to specify pickup address for zoning */
#define DEST_ADDR	1		/* symbol to specify destination address for zoning */
#define ALT_TEXT_COL1	1
#define ALT_TEXT_COL2	67
#define ALT_DATA_COL1	4
#define ALT_DATA_COL2	70
#define A_MAX		1
#define A_SLCT		1
#define A_SLCT_ROW	0
#define A_SLCT_COL	0
#define A_SLCT_SIZ	1
#define FORWRD		1
#define BCKWRD		-1
struct alt_choice {
	int	low;				/* low number */
	int	high;				/* high number */
	char	str_pre[3];			/* street pre-direction */
	char	str_name[21];			/* street name */
	char	str_type[4];			/* street type */
	char	str_post[3];			/* street post-direction */
	char	str_city[4];			/* city */
	char	plc_name[21];			/* place name */
	char	int_pre[3];			/* intersection pre-direction */
	char	int_name[21];			/* intersection name */
	char	int_type[4];			/* intersection type */
	char	int_post[3];			/* intersection post-direction*/
	char	szone[6];			/* sub-zone */
	char	comment[31];			/* comment */
	char    district[21];
	int     gps_x;
	int     gps_y;
};
