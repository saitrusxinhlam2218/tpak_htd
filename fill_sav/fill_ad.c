static char sz__FILE__[]="@(#)fill_ad.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  fill_ad.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:37:19
 * @(#)  Last delta: 12/2/94 at 11:17:58
 * @(#)  SCCS File: /taxi/sccs/s.fill_ad.c
 *                                                                           *
 *****************************************************************************/
#ident "@(#) head:fill_ad.c	10.1.0.0"

#define KEY_SRC

#include <isam.h>
#include "Address_db.h"
#include "taxi_db.h"

#define   CONST   33
#define   CONST1  39

void
fill_ad_key( char *from, char *buf )
{
	ADDRESS_REC  *ad = (ADDRESS_REC *)from;
	register int  offset = 0;

	stlong(ad->record_nbr, &buf[offset]);
        offset += LONGSIZE;

	stlong(ad->str_index,&buf[offset]);
        offset += LONGSIZE;

	stchar(ad->str_pre_dir,&buf[offset],sizeof(ad->str_pre_dir)-1);
        offset = CONST;

	stchar(ad->str_post_dir,&buf[offset],sizeof(ad->str_post_dir)-1);
        offset += sizeof(ad->str_post_dir)-1;

	stchar(ad->str_city,&buf[offset],sizeof(ad->str_city)-1);
        offset = CONST1;

	stlong(ad->high_nbr,&buf[offset]);
}

void
fill_ad( char *from, char *buf, int action)
{
	ADDRESS_REC *ad = (ADDRESS_REC *)from;
	register int  offset = 0;
	long serial_nbr = 0;

	if (action == ADD_REC)
	  {
	    isuniqueid(Db_files[ADDRESS_FILE_ID].fd, &serial_nbr);
	    ad->record_nbr = (long)serial_nbr;
	  }

	stlong(ad->record_nbr, &buf[offset]);
        offset += LONGSIZE;

	stlong(ad->str_index,&buf[offset]);
        offset += LONGSIZE;

	stchar(ad->str_pre_dir,&buf[offset],sizeof(ad->str_pre_dir)-1);
	offset += sizeof(ad->str_pre_dir) -1;

	stchar(ad->str_name,&buf[offset],sizeof(ad->str_name)-1);
	offset += sizeof(ad->str_name)-1;

	stchar(ad->str_type,&buf[offset],sizeof(ad->str_type)-1);
	offset += sizeof(ad->str_type) -1;

	stchar(ad->str_post_dir,&buf[offset],sizeof(ad->str_post_dir)-1);
	offset += sizeof(ad->str_post_dir) -1;

	stchar(ad->str_city,&buf[offset],sizeof(ad->str_city)-1);
	offset += sizeof(ad->str_city) -1;

	stchar(&ad->odd_even,&buf[offset],sizeof(ad->odd_even));
	offset += sizeof(ad->odd_even);

	stlong(ad->high_nbr,&buf[offset]);
        offset += LONGSIZE;

	stlong(ad->low_nbr,&buf[offset]);
        offset += LONGSIZE;

	stlong(ad->str_zip,&buf[offset]);
        offset += LONGSIZE;

	stchar(ad->str_subzone,&buf[offset],sizeof(ad->str_subzone)-1);
	offset += sizeof(ad->str_subzone) -1;

	stlong(ad->str_x_coord,&buf[offset]);
        offset += LONGSIZE;

	stlong(ad->str_y_coord,&buf[offset]);
        offset += LONGSIZE;

	stint(ad->map_ref_page,&buf[offset]);
        offset += INTSIZE;

	stchar(&ad->place_names,&buf[offset],sizeof(ad->place_names));
	offset += sizeof(ad->place_names);

	stchar(ad->map_ref_block,&buf[offset],sizeof(ad->map_ref_block)-1);
	offset += sizeof(ad->map_ref_block) -1;

	stchar(ad->str_comment,&buf[offset],sizeof(ad->str_comment)-1);
	offset += sizeof(ad->str_comment) -1;

	stchar(ad->ct_comment,&buf[offset],sizeof(ad->ct_comment)-1);
	offset += sizeof(ad->ct_comment) -1;

	stchar(ad->district, &buf[offset], sizeof( ad->district) -1 );
	offset += sizeof(ad->district) -1;
}

/* Load program variables from C-ISAM data record */
void
unfill_ad( char *to, char *buf )
{
	ADDRESS_REC  *ad = (ADDRESS_REC *)to;
	register int  offset = 0;

	ad->record_nbr = ldlong(&buf[offset]);
        offset += sizeof(ad->record_nbr);

	ad->str_index = ldlong(&buf[offset]);
        offset += sizeof(ad->str_index);

	ldchar(&buf[offset],sizeof(ad->str_pre_dir)-1,ad->str_pre_dir);
	offset += sizeof(ad->str_pre_dir) -1;

	ldchar(&buf[offset],sizeof(ad->str_name)-1,ad->str_name);
	offset += sizeof(ad->str_name)-1;

	ldchar(&buf[offset],sizeof(ad->str_type)-1,ad->str_type);
	offset += sizeof(ad->str_type) -1;

	ldchar(&buf[offset],sizeof(ad->str_post_dir)-1,ad->str_post_dir);
	offset += sizeof(ad->str_post_dir) -1;

	ldchar(&buf[offset],sizeof(ad->str_city)-1,ad->str_city);
	offset += sizeof(ad->str_city) -1;

	ad->odd_even = buf[offset];
	offset += sizeof(ad->odd_even);

	ad->high_nbr = ldlong(&buf[offset]);
        offset += sizeof(ad->high_nbr);

	ad->low_nbr = ldlong(&buf[offset]);
        offset += sizeof(ad->low_nbr);

	ad->str_zip = ldlong(&buf[offset]);
        offset += sizeof(ad->str_zip);

	ldchar(&buf[offset],sizeof(ad->str_subzone)-1,ad->str_subzone);
	offset += sizeof(ad->str_subzone) -1;

	ad->str_x_coord = ldlong(&buf[offset]);
        offset += sizeof(ad->str_x_coord);

	ad->str_y_coord = ldlong(&buf[offset]);
        offset += sizeof(ad->str_y_coord);

	ad->map_ref_page = ldint(&buf[offset]);
        offset += sizeof(ad->map_ref_page);

	ad->place_names = buf[offset];
	offset += sizeof(ad->place_names);

	ldchar(&buf[offset],sizeof(ad->map_ref_block)-1,ad->map_ref_block);
	offset += sizeof(ad->map_ref_block) -1;

	ldchar(&buf[offset],sizeof(ad->str_comment)-1,ad->str_comment);
	offset += sizeof(ad->str_comment) -1;

	ldchar(&buf[offset],sizeof(ad->ct_comment)-1,ad->ct_comment);
	offset += sizeof(ad->ct_comment) -1;

	ldchar(&buf[offset], sizeof(ad->district) -1,ad->district);
	offset += sizeof(ad->district) -1;
}

