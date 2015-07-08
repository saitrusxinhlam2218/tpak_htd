static char sz__FILE__[]="@(#)pi_unzoned.c	10.1.2.1(ver) Date Release 12/2/94";
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  pi_unzoned.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:46:19
 * @(#)  Last delta: 12/2/94 at 18:52:56
 * @(#)  SCCS File: /taxi/sccs/s.pi_unzoned.c
 *                                                                           *
 *****************************************************************************/
#ident "@(#) head:ui_unzoned.c	6.1.0.0"

#include <sys/types.h>
#include "mads_types.h"
#include "mads_isam.h"
#include "df_maint.h"
#include "ui_strdefs.h"
#include "pi.h"

extern struct cisam_cl call;
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
		ERROR("unzoned address file");
		return;
	}

	uz_ptr = (struct cisam_uz *)req_buf; 				/* build the unzoned record to be added to disk file */
	memset((char *)uz_ptr,'\0',sizeof(struct cisam_uz));
	uz_ptr->call_nbr = call.nbr;
	uz_ptr->fleet = call.fleet;
 	strcpy(uz_ptr->call_date,call.date_in);
	strcpy(uz_ptr->call_time,call.time_in);
	uz_ptr->entered_by = call.entered_by;
	uz_ptr->zoned_as = 0;
	uz_ptr->zoned_by = 0;
	uz_ptr->street_nbr = call.pckup_str_nbr;
	uz_ptr->type_of_service = call.extended_type[0];
	strcpy(uz_ptr->charge_nbr,call.charge_nbr);
	strcpy(uz_ptr->str_place_name1,call.pckup_str_name);
	strcpy(uz_ptr->str_type1,call.pckup_str_type);
	strcpy(uz_ptr->pre_dir1,call.pckup_pre_dir);
	strcpy(uz_ptr->post_dir1,call.pckup_post_dir);
	strcpy(uz_ptr->pre_dir2,call.intr_pre_dir);
	strcpy(uz_ptr->str_place_name2,call.intr_str_name);
	strcpy(uz_ptr->str_type2,call.intr_str_type);
	strcpy(uz_ptr->post_dir1,call.intr_post_dir);
	strcpy(uz_ptr->city,call.pckup_city);

	if(db(UNZONE_FILE_ID,ADD_REC,&uz_key1,ISEQUAL,0) < 0) {		 		/* write the record to disk */
		ERROR("unzoned address file");
		return;
	}

	if(db(UNZONE_FILE_ID,CLOSE,&uz_key1,ISEQUAL,0) < 0) {	 			/* close the unzoned addresses file */
		ERROR("unzoned address file");
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
		ERROR("unzoned address file");
		return;
	}

	uz_ptr = (struct cisam_uz *)req_buf; 				/* build the unzoned record to be added to disk file */
	uz_ptr->fleet = fleet;
	uz_ptr->call_nbr = nbr;

	if(db(UNZONE_FILE_ID, READ_KEYS, &uz_key1, ISEQUAL, 0) < 0)  { 	/* can't open unzoned file */
		ERROR("unzoned address file");
		return;
	}

	uz_ptr->zoned_as = zone;
	uz_ptr->zoned_by = by;

	if(db(UNZONE_FILE_ID, UPDATE_REC, &uz_key1, 0, 0) < 0) {		 		/* write the record to disk */
		ERROR("unzoned address file");
		return;
	}

	if(db(UNZONE_FILE_ID,CLOSE,&uz_key1,ISEQUAL,0) < 0) {	 			/* close the unzoned addresses file */
		ERROR("unzoned address file");
		return;
	}
}
