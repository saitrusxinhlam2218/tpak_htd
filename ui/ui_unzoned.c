static char sz__FILE__[]="@(#)ui_unzoned.c	10.1.2.1(ver) Date Release 12/2/94" ;
/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  ui_unzoned.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:51:59
* @(#)  Last delta: 12/2/94 at 18:11:12
* @(#)  SCCS File: /taxi/sccs/s.ui_unzoned.c
*                                                                           *
* Copyright (c) 1990 - MDI Seattle                                          *
*                                                                           *
*****************************************************************************/

#include <cursesX.h>
#include <sys/types.h>
#include "mads_types.h"
#include "mads_isam.h"
#include "df_maint.h"
#include "ui_error.h"
#include "ui_strdefs.h"

extern struct cisam_cl call_struct;
extern struct keydesc uz_key1;
extern char req_buf[];
extern int iserrno;

/**********************************************************************************/
/* write_unzoned: for an unzoned call; add a record to the unzoned addresses file */
/**********************************************************************************/

write_unzoned()
{
	struct cisam_uz *uz_ptr;

	if(db(UNZONE_FILE_ID,OPEN,&uz_key1,ISINOUT+ISMANULOCK,0) < 0)  { 	/* cant open unzoned file */
		prt_error(UI_OPEN,"<%s %d >", catgets(UI_m_catd, UI_1, UI_UI_UNZONED_1, "unzoned address file"), iserrno);
		return;
	}

	uz_ptr = (struct cisam_uz *)req_buf; 				/* build the unzoned record to be added to disk file */
	memset((char *)uz_ptr,'\0',sizeof(struct cisam_uz));
	uz_ptr->call_nbr = call_struct.nbr;
	uz_ptr->fleet = call_struct.fleet;
 	strcpy(uz_ptr->call_date,call_struct.date_in);
	strcpy(uz_ptr->call_time,call_struct.time_in);
	uz_ptr->entered_by = call_struct.entered_by;
	uz_ptr->zoned_as = 0;
	uz_ptr->zoned_by = 0;
	uz_ptr->street_nbr = call_struct.pckup_str_nbr;
	uz_ptr->type_of_service = call_struct.extended_type[0];
	strcpy(uz_ptr->charge_nbr,call_struct.charge_nbr);
	strcpy(uz_ptr->str_place_name1,call_struct.pckup_str_name);
	strcpy(uz_ptr->str_type1,call_struct.pckup_str_type);
	strcpy(uz_ptr->pre_dir1,call_struct.pckup_pre_dir);
	strcpy(uz_ptr->post_dir1,call_struct.pckup_post_dir);
	strcpy(uz_ptr->pre_dir2,call_struct.intr_pre_dir);
	strcpy(uz_ptr->str_place_name2,call_struct.intr_str_name);
	strcpy(uz_ptr->str_type2,call_struct.intr_str_type);
	strcpy(uz_ptr->post_dir1,call_struct.intr_post_dir);
	strcpy(uz_ptr->city,call_struct.pckup_city);

	if(db(UNZONE_FILE_ID,ADD_REC,&uz_key1,ISEQUAL,0) < 0) {		 		/* write the record to disk */
		prt_error(UI_WRITE,"<%s %d >",  catgets(UI_m_catd, UI_1, UI_UI_UNZONED_1, "unzoned address file"), iserrno); 		/* db failed */
		return;
	}

	if(db(UNZONE_FILE_ID,CLOSE,&uz_key1,ISEQUAL,0) < 0) {	 			/* close the unzoned addresses file */
		prt_error(UI_CLOSE,"<%s %d >", catgets(UI_m_catd, UI_1, UI_UI_UNZONED_1, "unzoned address file"), iserrno); 		/* db failed */
		return;
	}
}

update_unzoned(fleet, nbr, zone, by)
char	fleet;
int	nbr;
short	zone;
short	by;
{
	struct cisam_uz *uz_ptr;

	if(db(UNZONE_FILE_ID,OPEN,&uz_key1,ISINOUT+ISMANULOCK,0) < 0)  { 	/* cant open unzoned file */
		prt_error(UI_OPEN,"<%s %d >", catgets(UI_m_catd, UI_1, UI_UI_UNZONED_1, "unzoned address file"), iserrno);
		return;
	}

	uz_ptr = (struct cisam_uz *)req_buf; 				/* build the unzoned record to be added to disk file */
	uz_ptr->fleet = fleet;
	uz_ptr->call_nbr = nbr;

	if(db(UNZONE_FILE_ID, READ_KEYS, &uz_key1, ISEQUAL, 0) < 0)  { 	/* can't open unzoned file */
		prt_error(UI_OPEN,"<%s %d >", catgets(UI_m_catd, UI_1, UI_UI_UNZONED_1, "unzoned address file"), iserrno);
		return;
	}

	uz_ptr->zoned_as = zone;
	uz_ptr->zoned_by = by;

	if(db(UNZONE_FILE_ID, UPDATE_REC, &uz_key1, 0, 0) < 0) {		 		/* write the record to disk */
		prt_error(UI_WRITE,"<%s %d >", catgets(UI_m_catd, UI_1, UI_UI_UNZONED_1, "unzoned address file"), iserrno); 		/* db failed */
		return;
	}

	if(db(UNZONE_FILE_ID,CLOSE,&uz_key1,ISEQUAL,0) < 0) {	 			/* close the unzoned addresses file */
		prt_error(UI_CLOSE,"<%s %d >", catgets(UI_m_catd, UI_1, UI_UI_UNZONED_1, "unzoned address file"), iserrno); 		/* db failed */
		return;
	}
}
