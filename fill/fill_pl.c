static char sz__FILE__[]="@(#)fill_pl.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  fill_pl.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:38:03
 * @(#)  Last delta: 12/2/94 at 11:15:33
 * @(#)  SCCS File: /taxi/sccs/s.fill_pl.c
 *                                                                           *
 *****************************************************************************/

#define KEY_SRC
#include "Place_db.h"
#include "taxi_db.h"

#define  CONST  53
#define  CONST1 60 
 
void
fill_pl_key(req_buf, buf)
     char *req_buf;
     char *buf;
{
	struct  cisam_pl  *pl;
	register int  offset = 0;

	pl = (struct cisam_pl*)req_buf;

        stlong(pl->record_nbr, &buf[offset]);
        offset += LONGSIZE;

	stchar(pl->name,&buf[offset],sizeof(pl->name)-1);    
        offset += (sizeof(pl->name)-1 + sizeof(pl->str_name)-1);

        stlong(pl->index,&buf[offset]);
        offset += LONGSIZE;
        
	stchar(pl->str_pre_dir,&buf[offset],sizeof(pl->str_pre_dir)-1);    
        offset = CONST;

	stchar(pl->str_post_dir,&buf[offset],sizeof(pl->str_post_dir)-1);
        offset += sizeof(pl->str_post_dir) -1;

	stchar(pl->str_city,&buf[offset],sizeof(pl->str_city)-1);
        offset = CONST1;

        stlong(pl->street_nbr,&buf[offset]);
}

void
fill_pl(req_buf, buf, action)
     char *req_buf;
     char *buf;
     int    action;
{
	struct  cisam_pl  *pl;
	register int  offset = 0;
	int serial_nbr = 0;

	pl = (struct cisam_pl*)req_buf;

	if (action == ADD_REC)
	  {
	    isuniqueid(Db_files[PLACENM_FILE_ID].fd, &serial_nbr);
	    pl->record_nbr = (int)serial_nbr;
	  }

        stlong(pl->record_nbr, &buf[offset]);
        offset += LONGSIZE;

	stchar(pl->name,&buf[offset],sizeof(pl->name)-1);
	offset += sizeof(pl->name)-1;

	stchar(pl->str_name,&buf[offset],sizeof(pl->str_name)-1);
	offset += sizeof(pl->str_name) -1;

	stlong(pl->index,&buf[offset]);
	offset += LONGSIZE;

	stchar(pl->str_pre_dir,&buf[offset],sizeof(pl->str_pre_dir)-1);
	offset += sizeof(pl->str_pre_dir) -1;


	stchar(pl->str_type,&buf[offset],sizeof(pl->str_type)-1);
	offset += sizeof(pl->str_type)-1;

	stchar(pl->str_post_dir,&buf[offset],sizeof(pl->str_post_dir)-1);
	offset += sizeof(pl->str_post_dir)-1;

	stchar(pl->str_city,&buf[offset],sizeof(pl->str_city)-1);
	offset += sizeof(pl->str_city) -1;

	stchar(&pl->odd_even,&buf[offset],sizeof(pl->odd_even));
	offset += sizeof(pl->odd_even);

	stchar(&pl->str_nbr_suf,&buf[offset],sizeof(pl->str_nbr_suf));
	offset += sizeof(pl->str_nbr_suf);

	stlong(pl->street_nbr,&buf[offset]);
	offset += LONGSIZE;

	stlong(pl->str_zip,&buf[offset]);
	offset += LONGSIZE;

	stchar(pl->str_subzone,&buf[offset],sizeof(pl->str_subzone)-1);
	offset += sizeof(pl->str_subzone) - 1;

	stlong(pl->str_x_coord,&buf[offset]);
	offset += LONGSIZE;

	stlong(pl->str_y_coord,&buf[offset]);
	offset += LONGSIZE;

	stint(pl->map_ref_page,&buf[offset]);
	offset += INTSIZE;

	stchar(pl->map_ref_block,&buf[offset],sizeof(pl->map_ref_block)-1);
	offset += sizeof(pl->map_ref_block)-1;

	stchar(pl->str_comment,&buf[offset],sizeof(pl->str_comment)-1);
	offset += sizeof(pl->str_comment) -1;


	stchar(&pl->local,&buf[offset],sizeof(pl->local));
}

/* Load program variables from C-ISAM data record */
void
unfill_pl(req_buf, buf)
     char *req_buf;
     char *buf;
{
	struct  cisam_pl  *pl;
	register int  offset = 0;

	pl = (struct cisam_pl*)req_buf;

	pl->record_nbr = ldlong(&buf[offset]);
	offset += LONGSIZE;

	ldchar(&buf[offset],sizeof(pl->name)-1,pl->name);
	offset += sizeof(pl->name)-1;

	ldchar(&buf[offset],sizeof(pl->str_name)-1,pl->str_name);
	offset += sizeof(pl->str_name) -1;

	pl->index = ldlong(&buf[offset]);
	offset += LONGSIZE;

	ldchar(&buf[offset],sizeof(pl->str_pre_dir)-1,pl->str_pre_dir);
	offset += sizeof(pl->str_pre_dir) -1;

	ldchar(&buf[offset],sizeof(pl->str_type)-1,pl->str_type);
	offset += sizeof(pl->str_type)-1;

	ldchar(&buf[offset],sizeof(pl->str_post_dir)-1,pl->str_post_dir);
	offset += sizeof(pl->str_post_dir)-1;

	ldchar(&buf[offset],sizeof(pl->str_city)-1,pl->str_city);
	offset += sizeof(pl->str_city) -1;

	pl->odd_even = buf[offset];
	offset += sizeof(pl->odd_even);

	pl->str_nbr_suf = buf[offset];
	offset += sizeof(pl->str_nbr_suf);

	pl->street_nbr = ldlong(&buf[offset]);
	offset += LONGSIZE;

	pl->str_zip = ldlong(&buf[offset]);
	offset += LONGSIZE;

	ldchar(&buf[offset],sizeof(pl->str_subzone)-1,pl->str_subzone);
	offset += sizeof(pl->str_subzone) - 1;

	pl->str_x_coord = ldlong(&buf[offset]);
	offset += LONGSIZE;

	pl->str_y_coord = ldlong(&buf[offset]);
	offset += LONGSIZE;

	pl->map_ref_page = ldint(&buf[offset]);
	offset += INTSIZE;

	ldchar(&buf[offset],sizeof(pl->map_ref_block)-1,pl->map_ref_block);
	offset += sizeof(pl->map_ref_block)-1;

	ldchar(&buf[offset],sizeof(pl->str_comment)-1,pl->str_comment);
	offset += sizeof(pl->str_comment) -1;


	pl->local = buf[offset];
}
