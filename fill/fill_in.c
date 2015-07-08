static char sz__FILE__[]="@(#)fill_in.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  fill_in.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:37:52
 * @(#)  Last delta: 12/2/94 at 11:32:06
 * @(#)  SCCS File: /taxi/sccs/s.fill_in.c
 *                                                                           *
 *****************************************************************************/

#define KEY_SRC

#include "Intersect_db.h"

#define CONST  33
#define CONST1 60
#define CONST2 71

void
fill_in_key(req_buf, buf)
     char *req_buf;
     char *buf;
{
	struct  cisam_in  *in;
	register int  offset = 0;

	in = (struct cisam_in*)req_buf;

        stlong(in->index1,&buf[offset]);
        offset += LONGSIZE;

        stlong(in->index2,&buf[offset]);
        offset += LONGSIZE;

	stchar(in->pre_dir1,&buf[offset],sizeof(in->pre_dir1)-1);    
        offset = CONST;

	stchar(in->post_dir1,&buf[offset],sizeof(in->post_dir1)-1);
        offset += sizeof(in->post_dir1)-1;

	stchar(in->pre_dir2,&buf[offset],sizeof(in->pre_dir2)-1);
        offset = CONST1;

	stchar(in->post_dir2,&buf[offset],sizeof(in->post_dir2)-1);
        offset = CONST2;
        
	stchar(in->city,&buf[offset],sizeof(in->city)-1);
}

void
fill_in(req_buf, buf, action)
     char *req_buf;
     char *buf;
     int     action;
{
	struct  cisam_in  *in;
	register int  offset = 0;

	in = (struct cisam_in*)req_buf;

	stlong(in->index1,&buf[offset]);
	offset += LONGSIZE;

	stlong(in->index2,&buf[offset]);
	offset += LONGSIZE;

	stchar(in->pre_dir1,&buf[offset],sizeof(in->pre_dir1)-1);
	offset += sizeof(in->pre_dir1)-1;

	stchar(in->name1,&buf[offset],sizeof(in->name1)-1);
	offset += sizeof(in->name1) -1;

	stchar(in->type1,&buf[offset],sizeof(in->type1)-1);
	offset += sizeof(in->type1) -1;

	stchar(in->post_dir1,&buf[offset],sizeof(in->post_dir1)-1);
	offset += sizeof(in->post_dir1)-1;

	stchar(in->pre_dir2,&buf[offset],sizeof(in->pre_dir2)-1);
	offset += sizeof(in->pre_dir2)-1;

	stchar(in->name2,&buf[offset],sizeof(in->name2)-1);
	offset += sizeof(in->name2) -1;

	stchar(in->type2,&buf[offset],sizeof(in->type2)-1);
	offset += sizeof(in->type2) -1;

	stchar(in->post_dir2,&buf[offset],sizeof(in->post_dir2)-1);
	offset += sizeof(in->post_dir2)-1;


	stlong(in->zip,&buf[offset]);
	offset += LONGSIZE;

	stchar(in->subzone,&buf[offset],sizeof(in->subzone)-1);
	offset += sizeof(in->subzone) - 1;

	stchar(in->city,&buf[offset],sizeof(in->city)-1);
	offset += sizeof(in->city) -1;

	stlong(in->str_x_coord,&buf[offset]);
	offset += LONGSIZE;

	stlong(in->str_y_coord,&buf[offset]);
	offset += LONGSIZE;

	stint(in->map_ref_page,&buf[offset]);
	offset += INTSIZE;

	stchar(in->comment,&buf[offset],sizeof(in->comment)-1);
	offset += sizeof(in->comment) -1;

	stchar(in->map_ref_block,&buf[offset],sizeof(in->map_ref_block)-1);

}

/* Load program variables from C-ISAM data record */
void
unfill_in(req_buf, buf)
     char *req_buf;
     char *buf;
{
	struct  cisam_in  *in;
	register int  offset = 0;

	in = (struct cisam_in*)req_buf;

	in->index1 = ldlong(&buf[offset]);
	offset += LONGSIZE;

	in->index2 = ldlong(&buf[offset]);
	offset += LONGSIZE;

	ldchar(&buf[offset],sizeof(in->pre_dir1)-1,in->pre_dir1);
	offset += sizeof(in->pre_dir1)-1;

	ldchar(&buf[offset],sizeof(in->name1)-1,in->name1);
	offset += sizeof(in->name1) -1;

	ldchar(&buf[offset],sizeof(in->type1)-1,in->type1);
	offset += sizeof(in->type1) -1;

	ldchar(&buf[offset],sizeof(in->post_dir1)-1,in->post_dir1);
	offset += sizeof(in->post_dir1)-1;

	ldchar(&buf[offset],sizeof(in->pre_dir2)-1,in->pre_dir2);
	offset += sizeof(in->pre_dir2)-1;

	ldchar(&buf[offset],sizeof(in->name2)-1,in->name2);
	offset += sizeof(in->name2) -1;

	ldchar(&buf[offset],sizeof(in->type2)-1,in->type2);
	offset += sizeof(in->type2) -1;

	ldchar(&buf[offset],sizeof(in->post_dir2)-1,in->post_dir2);
	offset += sizeof(in->post_dir2)-1;


	in->zip = ldlong(&buf[offset]);
	offset += LONGSIZE;

	ldchar(&buf[offset],sizeof(in->subzone)-1,in->subzone);
	offset += sizeof(in->subzone) - 1;

	ldchar(&buf[offset],sizeof(in->city)-1,in->city);
	offset += sizeof(in->city) -1;

	in->str_x_coord = ldlong(&buf[offset]);
	offset += LONGSIZE;

	in->str_y_coord = ldlong(&buf[offset]);
	offset += LONGSIZE;

	in->map_ref_page = ldint(&buf[offset]);
	offset += INTSIZE;

	ldchar(&buf[offset],sizeof(in->comment)-1,in->comment);
	offset += sizeof(in->comment) -1;

	ldchar(&buf[offset],sizeof(in->map_ref_block)-1,in->map_ref_block);

}
