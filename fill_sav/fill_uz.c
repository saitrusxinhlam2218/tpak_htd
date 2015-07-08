static char sz__FILE__[]="@(#)fill_uz.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  fill_uz.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:38:35
 * @(#)  Last delta: 12/2/94 at 11:32:50
 * @(#)  SCCS File: /taxi/sccs/s.fill_uz.c
 *                                                                           *
 *****************************************************************************/
#ident "@(#) head:fill_uz.c	10.1.0.0"

#define KEY_SRC
#include "Unzoned_db.h"

#define  CONST  49
 
void
fill_uz_key(req_buf, buf)
     char *req_buf;
     char *buf;
{
	struct  cisam_uz  *uz;
	register int  offset = 0;

	uz = (struct cisam_uz*)req_buf;

        stlong(uz->call_nbr,&buf[offset]);
        offset += CONST;

	stchar(uz->str_place_name1,&buf[offset],sizeof(uz->str_place_name1)-1); 
        offset += sizeof(uz->str_place_name1)-1;

	stchar(uz->str_type1,&buf[offset],sizeof(uz->str_type1)-1);
        offset += sizeof(uz->str_type1)-1;

	stchar(uz->pre_dir1,&buf[offset],sizeof(uz->pre_dir1)-1);    
        offset += sizeof(uz->pre_dir1)-1;

	stchar(uz->post_dir1,&buf[offset],sizeof(uz->post_dir1)-1);
}

void
fill_uz(req_buf, buf, action)
     char *req_buf;
     char *buf;
     int   action;
{
	struct  cisam_uz  *uz;
	register int  offset = 0;

	uz = (struct cisam_uz*)req_buf;


        stlong(uz->call_nbr,&buf[offset]);
        offset += LONGSIZE;

	stchar(&uz->fleet,&buf[offset],sizeof(uz->fleet)); 
	offset += sizeof(uz->fleet);

        stchar(uz->call_date,&buf[offset],sizeof(uz->call_date)-1);
        offset += sizeof(uz->call_date)-1;

        stchar(uz->call_time,&buf[offset],sizeof(uz->call_time)-1);
        offset += sizeof(uz->call_time)-1;

        stint(uz->entered_by,&buf[offset]);
        offset += INTSIZE;

        stint(uz->zoned_as,&buf[offset]);
        offset += INTSIZE;

        stint(uz->zoned_by,&buf[offset]);
        offset += INTSIZE;

        stlong(uz->street_nbr,&buf[offset]);
        offset += LONGSIZE;

	stchar(&uz->type_of_service,&buf[offset],sizeof(uz->type_of_service)); 
        offset += sizeof(uz->type_of_service);

	stchar(uz->charge_nbr,&buf[offset],sizeof(uz->charge_nbr)-1); 
        offset += sizeof(uz->charge_nbr)-1;

	stchar(uz->str_place_name1,&buf[offset],sizeof(uz->str_place_name1)-1); 
        offset += sizeof(uz->str_place_name1)-1;

	stchar(uz->str_type1,&buf[offset],sizeof(uz->str_type1)-1);
        offset += sizeof(uz->str_type1)-1;

	stchar(uz->pre_dir1,&buf[offset],sizeof(uz->pre_dir1)-1);    
        offset += sizeof(uz->pre_dir1)-1;

	stchar(uz->post_dir1,&buf[offset],sizeof(uz->post_dir1)-1);    
        offset += sizeof(uz->post_dir1)-1;

	stchar(uz->pre_dir2,&buf[offset],sizeof(uz->pre_dir2)-1);
        offset += sizeof(uz->pre_dir2)-1;

	stchar(uz->str_place_name2,&buf[offset],sizeof(uz->str_place_name2)-1);
        offset += sizeof(uz->str_place_name2)-1;

	stchar(uz->str_type2,&buf[offset],sizeof(uz->str_type2)-1);
        offset += sizeof(uz->str_type2)-1;

	stchar(uz->post_dir2,&buf[offset],sizeof(uz->post_dir2)-1);
        offset += sizeof(uz->post_dir2)-1;

	stchar(uz->city,&buf[offset],sizeof(uz->city)-1);

}


/* Load program variables from C-ISAM data record */
void
unfill_uz(req_buf, buf)
     char *req_buf;
     char *buf;
{
	struct  cisam_uz  *uz;
	register int  offset = 0;

	uz = (struct cisam_uz*)req_buf;

        uz->call_nbr = ldlong(&buf[offset]);
        offset += LONGSIZE;

	uz->fleet = buf[offset]; 
	offset += sizeof(uz->fleet);

	ldchar(&buf[offset],sizeof(uz->call_date)-1,uz->call_date); 
        offset += sizeof(uz->call_date)-1;

	ldchar(&buf[offset],sizeof(uz->call_time)-1,uz->call_time); 
        offset += sizeof(uz->call_time)-1;

        uz->entered_by = ldint(&buf[offset]);
        offset += INTSIZE;

        uz->zoned_as = ldint(&buf[offset]);
        offset += INTSIZE;

        uz->zoned_by = ldint(&buf[offset]);
        offset += INTSIZE;

        uz->street_nbr = ldlong(&buf[offset]);
        offset += LONGSIZE;

	uz->type_of_service = buf[offset]; 
        offset += sizeof(uz->type_of_service);

	ldchar(&buf[offset],sizeof(uz->charge_nbr)-1,uz->charge_nbr); 
        offset += sizeof(uz->charge_nbr)-1;

	ldchar(&buf[offset],sizeof(uz->str_place_name1)-1,uz->str_place_name1); 
        offset += sizeof(uz->str_place_name1)-1;

	ldchar(&buf[offset],sizeof(uz->str_type1)-1,uz->str_type1);
        offset += sizeof(uz->str_type1)-1;

	ldchar(&buf[offset],sizeof(uz->pre_dir1)-1,uz->pre_dir1);    
        offset += sizeof(uz->pre_dir1)-1;

	ldchar(&buf[offset],sizeof(uz->post_dir1)-1,uz->post_dir1);    
        offset += sizeof(uz->post_dir1)-1;

	ldchar(&buf[offset],sizeof(uz->pre_dir2)-1,uz->pre_dir2);
        offset += sizeof(uz->pre_dir2)-1;

	ldchar(&buf[offset],sizeof(uz->str_place_name2)-1,uz->str_place_name2);
        offset += sizeof(uz->str_place_name2)-1;

	ldchar(&buf[offset],sizeof(uz->str_type2)-1,uz->str_type2);
        offset += sizeof(uz->str_type2)-1;

	ldchar(&buf[offset],sizeof(uz->post_dir2)-1,uz->post_dir2);
        offset += sizeof(uz->post_dir2)-1;

	ldchar(&buf[offset],sizeof(uz->city)-1,uz->city);

}
