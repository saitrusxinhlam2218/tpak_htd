static char sz__FILE__[]="@(#)fill_mf.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  fill_mf.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:37:58
 * @(#)  Last delta: 12/2/94 at 11:32:13
 * @(#)  SCCS File: /taxi/sccs/s.fill_mf.c
 *                                                                           *
 *****************************************************************************/

#define KEY_SRC
#include "Menu_db.h"
#include "taxi_db.h"

void
fill_mf_key(req_buf, buf)
     char *req_buf;
     char *buf;
{
	struct  cisam_mf  *mf;
	register int  offset = 0;

	mf = (struct cisam_mf*)req_buf;

        stlong(mf->nbr,&buf[offset]);
        offset += LONGSIZE;

        stint(mf->command,&buf[offset]);
}

void
fill_mf(req_buf, buf, action)
     char *req_buf;
     char *buf;
     int  action;
{
	struct  cisam_mf  *mf;
	register int  offset = 0;
        int serial_nbr = 0;
        
	mf = (struct cisam_mf*)req_buf;

	if(action == ADD_REC)
          {
            isuniqueid(Db_files[MENU_FILE_ID].fd,&serial_nbr);
            mf->nbr = (int)serial_nbr;
          }

        stlong(mf->nbr,&buf[offset]);
        offset += LONGSIZE;

        stint(mf->command,&buf[offset]);
	offset += INTSIZE;

	stchar(&mf->last_menu_item,&buf[offset],sizeof(mf->last_menu_item));
	offset += sizeof(mf->last_menu_item);

	stchar(&mf->type,&buf[offset],sizeof(mf->type));
	offset += sizeof(mf->type);

	stchar(mf->desc,&buf[offset],sizeof(mf->desc)-1);
	offset += sizeof(mf->desc) -1;

	stchar(mf->unix_string,&buf[offset],sizeof(mf->unix_string)-1);
	offset += sizeof(mf->unix_string) - 1;

	stchar(&mf->prev_scr_flag,&buf[offset],sizeof(mf->prev_scr_flag));
}

/* Load program variables from C-ISAM data record */
void
unfill_mf(req_buf, buf)
     char *req_buf;
     char *buf;
{
	struct  cisam_mf  *mf;
	register int  offset = 0;

	mf = (struct cisam_mf*)req_buf;

        mf->nbr = ldlong(&buf[offset]);
        offset += LONGSIZE;

        mf->command = ldint(&buf[offset]);
	offset += INTSIZE;

	mf->last_menu_item = buf[offset];
	offset += sizeof(mf->last_menu_item);

	mf->type = buf[offset];
	offset += sizeof(mf->type);

	ldchar(&buf[offset],sizeof(mf->desc)-1,mf->desc);
	offset += sizeof(mf->desc) -1;

	ldchar(&buf[offset],sizeof(mf->unix_string)-1,mf->unix_string);
	offset += sizeof(mf->unix_string) - 1;

	mf->prev_scr_flag = buf[offset];
}
