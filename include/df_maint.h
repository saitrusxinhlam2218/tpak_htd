 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  df_maint.h; Rel: 10.1.2.1; Get date: 4/20/95 at 11:44:59
 * @(#)  Last delta: 12/2/94 at 18:09:20
 * @(#)  SCCS File: /taxi/sccs/s.df_maint.h
 *                                                                           *
 *****************************************************************************/
#ifndef _DF_MAINT_H
#define _DF_MAINT_H
struct df_maint  {
	long d_prcs_id;		/* destination ID for dispatch */
	long u_prcs_id;		/* message type = users process ID */
	char rec_type;		/* application type code */
	short   which_file;     /* File designator */
	char    fleet;          /* Fleet Indicator */
	int     key;            /* Key for the record which has changed */
	int     cisam_num;     /* C-ISAM number for this record */
}; 
#endif
