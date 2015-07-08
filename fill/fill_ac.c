static char sz__FILE__[]="@(#)fill_ac.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  fill_ac.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:37:17
 * @(#)  Last delta: 12/2/94 at 11:16:36
 * @(#)  SCCS File: /taxi/sccs/s.fill_ac.c
 *                                                                           *
 *****************************************************************************/

#define KEY_SRC

#include "Account_db.h"

fill_ac_key( char *from, char *to )
{
	ACCOUNT_REC  *ac;
	register int  offset = 0;

	ac = (ACCOUNT_REC *)from;

	stchar(&ac->fleet,&to[offset],sizeof(ac->fleet));      /* fleet indicator */
        offset = sizeof(ac->fleet);
	stchar(ac->nbr,&to[offset],sizeof(ac->nbr)-1);
}


fill_ac( char *from, char *to, int action)
{
	ACCOUNT_REC  *ac = (ACCOUNT_REC *)from;
	register int  offset = 0;

	stchar(&ac->fleet,&to[offset],sizeof(ac->fleet));
	offset += sizeof(ac->fleet);

	stchar(ac->nbr,&to[offset],sizeof(ac->nbr)-1);
	offset += sizeof(ac->nbr)-1;

	stchar(ac->name,&to[offset],sizeof(ac->name)-1);
	offset += sizeof(ac->name) -1;

	stlong(ac->str_nbr,&to[offset]);
	offset += LONGSIZE;

	stchar(&ac->str_suf,&to[offset],sizeof(ac->str_suf));
	offset += sizeof(ac->str_suf);

	stchar(ac->pre_dir,&to[offset],sizeof(ac->pre_dir)-1);
	offset += sizeof(ac->pre_dir) - 1;

	stchar(ac->str_name,&to[offset],sizeof(ac->str_name)-1);
	offset += sizeof(ac->str_name) -1;

	stchar(ac->str_type,&to[offset],sizeof(ac->str_type)-1);
	offset += sizeof(ac->str_type) -1;

	stchar(ac->post_dir,&to[offset],sizeof(ac->post_dir)-1);
	offset += sizeof(ac->post_dir) -1;

	stchar(ac->apt_room,&to[offset],sizeof(ac->apt_room)-1);
	offset += sizeof(ac->apt_room) -1;

	stchar(ac->city,&to[offset],sizeof(ac->city)-1);
	offset += sizeof(ac->city) -1;

	stchar(ac->state,&to[offset],sizeof(ac->state)-1);
	offset += sizeof(ac->state) -1;

	stchar(ac->zip,&to[offset],sizeof(ac->zip)-1);
	offset += sizeof(ac->zip) -1;

	stchar(ac->zip_suf,&to[offset],sizeof(ac->zip_suf)-1);
	offset += sizeof(ac->zip_suf) -1;

	stchar(ac->phone_area,&to[offset],sizeof(ac->phone_area)-1);
	offset += sizeof(ac->phone_area) -1;

	stchar(ac->phone,&to[offset],sizeof(ac->phone)-1);
	offset += sizeof(ac->phone) -1;

	stchar(ac->phone_ext,&to[offset],sizeof(ac->phone_ext)-1);
	offset += sizeof(ac->phone_ext) -1;

	stchar(ac->phone_contact,&to[offset],sizeof(ac->phone_contact)-1);
	offset += sizeof(ac->phone_contact) -1;

	stchar(ac->attention,&to[offset],sizeof(ac->attention)-1);
	offset += sizeof(ac->attention) -1;

	stchar(ac->comment,&to[offset],sizeof(ac->comment)-1);
	offset += sizeof(ac->comment) -1;

	stchar(ac->header_1,&to[offset],sizeof(ac->header_1)-1);
	offset += sizeof(ac->header_1) -1;

	stchar(ac->header_2,&to[offset],sizeof(ac->header_2)-1);
	offset += sizeof(ac->header_2) -1;

	stchar(ac->header_3,&to[offset],sizeof(ac->header_3)-1);
	offset += sizeof(ac->header_3) -1;

	stchar(ac->header_4,&to[offset],sizeof(ac->header_4)-1);
	offset += sizeof(ac->header_4) -1;

	stchar(ac->driver_msg,&to[offset],sizeof(ac->driver_msg)-1);
	offset += sizeof(ac->driver_msg) -1;

	stint(ac->priority,&to[offset]);
	offset += INTSIZE;

	stchar(&ac->show_pckup_adr,&to[offset],sizeof(ac->show_pckup_adr));
	offset += sizeof(ac->show_pckup_adr);

	stchar(ac->phone_number,&to[offset],sizeof(ac->phone_number)-1);
	offset += sizeof(ac->phone_number) -1;

	stchar(ac->drv_attr,&to[offset],sizeof(ac->drv_attr)-1);
	offset += sizeof(ac->drv_attr) -1;

	stchar(ac->veh_attr,&to[offset],sizeof(ac->veh_attr)-1);
	offset += sizeof(ac->veh_attr) -1;

	stchar(&ac->autobook_enabled,&to[offset],sizeof(ac->autobook_enabled));
	offset += sizeof(ac->autobook_enabled);

	stchar(&ac->account_charge_enabled,&to[offset],sizeof(ac->account_charge_enabled));
	offset += sizeof(ac->account_charge_enabled);

	stfloat(ac->factor_value, &to[offset]);
	offset += FLOATSIZE;

	stint(ac->extra_value, &to[offset]);
	offset += INTSIZE;

        stchar(&ac->cash_account, &to[offset],sizeof(ac->cash_account));
        offset += sizeof(ac->cash_account);

	stchar(ac->password, &to[offset], sizeof(ac->password)-1);
	offset += sizeof(ac->password) -1;

}

/* Load program variables from C-ISAM data record */

unfill_ac( char *to, char *from )
{
	ACCOUNT_REC  *ac = (ACCOUNT_REC *)to;
	register int  offset = 0;
	char temp_attr[65]; /* temporary attribute string */

	ac->fleet = from[offset];
	offset += sizeof(ac->fleet);

	ldchar(&from[offset],sizeof(ac->nbr)-1,ac->nbr);
	offset += sizeof(ac->nbr)-1;

	ldchar(&from[offset],sizeof(ac->name)-1,ac->name);
	offset += sizeof(ac->name) -1;

	ac->str_nbr = ldlong(&from[offset]);
	offset += sizeof(ac->str_nbr);

	ac->str_suf = from[offset];
	offset += sizeof(ac->str_suf);

	ldchar(&from[offset],sizeof(ac->pre_dir)-1,ac->pre_dir);
	offset += sizeof(ac->pre_dir) - 1;

	ldchar(&from[offset],sizeof(ac->str_name)-1,ac->str_name);
	offset += sizeof(ac->str_name) -1;

	ldchar(&from[offset],sizeof(ac->str_type)-1,ac->str_type);
	offset += sizeof(ac->str_type) -1;

	ldchar(&from[offset],sizeof(ac->post_dir)-1,ac->post_dir);
	offset += sizeof(ac->post_dir) -1;

	ldchar(&from[offset],sizeof(ac->apt_room)-1,ac->apt_room);
	offset += sizeof(ac->apt_room) -1;

	ldchar(&from[offset],sizeof(ac->city)-1,ac->city);
	offset += sizeof(ac->city) -1;

	ldchar(&from[offset],sizeof(ac->state)-1,ac->state);
	offset += sizeof(ac->state) -1;

	ldchar(&from[offset],sizeof(ac->zip)-1,ac->zip);
	offset += sizeof(ac->zip) -1;

	ldchar(&from[offset],sizeof(ac->zip_suf)-1,ac->zip_suf);
	offset += sizeof(ac->zip_suf) -1;

	ldchar(&from[offset],sizeof(ac->phone_area)-1,ac->phone_area);
	offset += sizeof(ac->phone_area) -1;

	ldchar(&from[offset],sizeof(ac->phone)-1,ac->phone);
	offset += sizeof(ac->phone) -1;

	ldchar(&from[offset],sizeof(ac->phone_ext)-1,ac->phone_ext);
	offset += sizeof(ac->phone_ext) -1;

	ldchar(&from[offset],sizeof(ac->phone_contact)-1,ac->phone_contact);
	offset += sizeof(ac->phone_contact) -1;

	ldchar(&from[offset],sizeof(ac->attention)-1,ac->attention);
	offset += sizeof(ac->attention) -1;

	ldchar(&from[offset],sizeof(ac->comment)-1,ac->comment);
	offset += sizeof(ac->comment) -1;

	ldchar(&from[offset],sizeof(ac->header_1)-1,ac->header_1);
	offset += sizeof(ac->header_1) -1;

	ldchar(&from[offset],sizeof(ac->header_2)-1,ac->header_2);
	offset += sizeof(ac->header_2) -1;

	ldchar(&from[offset],sizeof(ac->header_3)-1,ac->header_3);
	offset += sizeof(ac->header_3) -1;

	ldchar(&from[offset],sizeof(ac->header_4)-1,ac->header_4);
	offset += sizeof(ac->header_4) -1;

	ldchar(&from[offset],sizeof(ac->driver_msg)-1,ac->driver_msg);
	offset += sizeof(ac->driver_msg) -1;

	ac->priority = ldint(&from[offset]);
	offset += INTSIZE;

	ac->show_pckup_adr = from[offset];
	offset += sizeof(ac->show_pckup_adr);

	ldchar(&from[offset],sizeof(ac->phone_number)-1,ac->phone_number);
	offset += sizeof(ac->phone_number) -1;

	ldchar(&from[offset],sizeof(ac->drv_attr)-1,temp_attr); /* Fill attribute string with spaces */
	sprintf(ac->drv_attr,"%-64.64s",temp_attr);             /* as all 64 characters are needed in all TaxiPak routines */
	offset += sizeof(ac->drv_attr) -1;

	ldchar(&from[offset],sizeof(ac->veh_attr)-1,temp_attr); /* Fill attribute string with spaces */
	sprintf(ac->veh_attr,"%-64.64s",temp_attr);             /* as all 64 characters are needed in all TaxiPak routines */
	offset += sizeof(ac->veh_attr) -1;

	ac->autobook_enabled = from[offset];
	offset += sizeof(ac->autobook_enabled);

	ac->account_charge_enabled = from[offset];
	offset += sizeof(ac->account_charge_enabled);

	ac->factor_value = ldfloat(&from[offset]);
	offset += FLOATSIZE;

	ac->extra_value = ldint(&from[offset]);
	offset += INTSIZE;

        ac->cash_account = from[offset];
        offset += sizeof(ac->cash_account);

	ldchar(&from[offset],sizeof(ac->password)-1,ac->password);
	offset += sizeof(ac->password) -1;	
}

