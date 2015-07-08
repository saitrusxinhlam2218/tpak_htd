static char sz__FILE__[]="@(#)fill_up.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  fill_up.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:38:33
 * @(#)  Last delta: 12/2/94 at 11:32:47
 * @(#)  SCCS File: /taxi/sccs/s.fill_up.c
 *                                                                           *
 *****************************************************************************/

#define KEY_SRC
#include "Profile_db.h"

void
fill_up_key(req_buf, buf)
     char *req_buf;
     char *buf;
{
	struct  cisam_up  *up;
	register int  offset = 0;

	up = (struct cisam_up*)req_buf;
	stint(up->user_id,&buf[offset]);

	stchar(up->fullname,&buf[10],sizeof(up->fullname)-1);
}

void
fill_up(req_buf, buf, action)
     char *req_buf;
     char *buf;
     int  action;
{
	struct  cisam_up  *up;
	register int  offset = 0;

	up = (struct cisam_up*)req_buf;


	stint(up->user_id,&buf[offset]);
	offset += INTSIZE;

	stchar(up->user_name,&buf[offset],sizeof(up->user_name)-1);
	offset += sizeof(up->user_name)-1;

	stchar(up->fullname,&buf[offset],sizeof(up->fullname)-1);
	offset += sizeof(up->fullname)-1;

	stchar(up->greeting_name,&buf[offset],sizeof(up->greeting_name)-1);
	offset += sizeof(up->greeting_name)-1;

	stchar(up->menu_perms,&buf[offset],sizeof(up->menu_perms)-1);
	offset += sizeof(up->menu_perms)-1;

	stchar(up->fleet_perms,&buf[offset],sizeof(up->fleet_perms)-1);
	offset += sizeof(up->fleet_perms)-1;

	stchar(&up->default_fleet,&buf[offset],sizeof(up->default_fleet));
	offset += sizeof(up->default_fleet);

	stint(up->default_screen,&buf[offset]);
	offset += INTSIZE;

	stint(up->default_search,&buf[offset]);
	offset += INTSIZE;

	stint(up->default_pos,&buf[offset]);
	offset += INTSIZE;

	stchar(&up->default_call_tp,&buf[offset],sizeof(up->default_call_tp));
	offset += sizeof(up->default_call_tp);

	stchar(up->default_city,&buf[offset],sizeof(up->default_city)-1);
	offset += sizeof(up->default_city)-1;

	stchar(&up->can_zone_calls,&buf[offset],sizeof(up->can_zone_calls));
	offset += sizeof(up->can_zone_calls);

	stint(up->high_call_pri,&buf[offset]);
	offset += INTSIZE;

	stint(up->superzone_1,&buf[offset]);
	offset += INTSIZE;

	stint(up->superzone_2,&buf[offset]);
	offset += INTSIZE;

	stint(up->superzone_3,&buf[offset]);
	offset += INTSIZE;

	stint(up->superzone_4,&buf[offset]);
	offset += INTSIZE;

	stint(up->superzone_5,&buf[offset]);
	offset += INTSIZE;

	stint(up->superzone_6,&buf[offset]);
	offset += INTSIZE;

	stint(up->superzone_7,&buf[offset]);
	offset += INTSIZE;

	stint(up->superzone_8,&buf[offset]);
	offset += INTSIZE;

	stint(up->superzone_9,&buf[offset]);
	offset += INTSIZE;

	stint(up->superzone_10,&buf[offset]);
	offset += INTSIZE;

	stint(up->superzone_11,&buf[offset]);
	offset += INTSIZE;

	stint(up->superzone_12,&buf[offset]);
	offset += INTSIZE;

	stint(up->superzone_13,&buf[offset]);
	offset += INTSIZE;

	stint(up->superzone_14,&buf[offset]);
	offset += INTSIZE;

	stint(up->superzone_15,&buf[offset]);
	offset += INTSIZE;

	stint(up->superzone_16,&buf[offset]);
	offset += INTSIZE;

	stchar(up->logo_file,&buf[offset],sizeof(up->logo_file)-1);
	offset += sizeof(up->logo_file)-1;

	stchar(&up->group_1,&buf[offset],sizeof(up->group_1));
	offset += sizeof(up->group_1);

	stchar(&up->group_2,&buf[offset],sizeof(up->group_2));
	offset += sizeof(up->group_2);

	stchar(&up->group_3,&buf[offset],sizeof(up->group_3));
	offset += sizeof(up->group_3);

	stchar(&up->group_4,&buf[offset],sizeof(up->group_4));
	offset += sizeof(up->group_4);

	stchar(&up->group_5,&buf[offset],sizeof(up->group_5));
	offset += sizeof(up->group_5);

	stchar(&up->group_6,&buf[offset],sizeof(up->group_6));
	offset += sizeof(up->group_6);

	stchar(&up->group_7,&buf[offset],sizeof(up->group_7));
	offset += sizeof(up->group_7);

	stchar(&up->group_8,&buf[offset],sizeof(up->group_8));
	offset += sizeof(up->group_8);

	stchar(&up->group_9,&buf[offset],sizeof(up->group_9));
	offset += sizeof(up->group_9);

	stchar(&up->group_10,&buf[offset],sizeof(up->group_10));
	offset += sizeof(up->group_10);

	stchar(&up->group_11,&buf[offset],sizeof(up->group_11));
	offset += sizeof(up->group_11);

	stchar(&up->group_12,&buf[offset],sizeof(up->group_12));
	offset += sizeof(up->group_12);

	stchar(&up->type_1,&buf[offset],sizeof(up->type_1));
	offset += sizeof(up->type_1);

	stchar(&up->type_2,&buf[offset],sizeof(up->type_2));
	offset += sizeof(up->type_2);

	stchar(&up->type_3,&buf[offset],sizeof(up->type_3));
	offset += sizeof(up->type_3);

	stchar(&up->type_4,&buf[offset],sizeof(up->type_4));
	offset += sizeof(up->type_4);

	stchar(&up->type_5,&buf[offset],sizeof(up->type_5));
	offset += sizeof(up->type_5);

	stchar(&up->type_6,&buf[offset],sizeof(up->type_6));
	offset += sizeof(up->type_6);

	stchar(&up->type_7,&buf[offset],sizeof(up->type_7));
	offset += sizeof(up->type_7);

	stchar(&up->type_8,&buf[offset],sizeof(up->type_8));
	offset += sizeof(up->type_8);

	stchar(&up->type_9,&buf[offset],sizeof(up->type_9));
	offset += sizeof(up->type_9);

	stchar(&up->type_10,&buf[offset],sizeof(up->type_10));
	offset += sizeof(up->type_10);

	stchar(&up->type_11,&buf[offset],sizeof(up->type_11));
	offset += sizeof(up->type_11);

	stchar(&up->type_12,&buf[offset],sizeof(up->type_12));
	offset += sizeof(up->type_12);

	stchar(&up->type_13,&buf[offset],sizeof(up->type_13));
	offset += sizeof(up->type_13);

	stchar(&up->type_14,&buf[offset],sizeof(up->type_14));
	offset += sizeof(up->type_14);

	stchar(&up->type_15,&buf[offset],sizeof(up->type_15));
	offset += sizeof(up->type_15);

	stchar(&up->type_16,&buf[offset],sizeof(up->type_16));
	offset += sizeof(up->type_16);

	stchar(&up->type_17,&buf[offset],sizeof(up->type_17));
	offset += sizeof(up->type_17);

	stchar(&up->type_18,&buf[offset],sizeof(up->type_18));
	offset += sizeof(up->type_18);

	stchar(&up->type_19,&buf[offset],sizeof(up->type_19));
	offset += sizeof(up->type_19);

	stchar(&up->type_20,&buf[offset],sizeof(up->type_20));
	offset += sizeof(up->type_20);

	stchar(&up->type_21,&buf[offset],sizeof(up->type_21));
	offset += sizeof(up->type_21);

	stchar(&up->type_22,&buf[offset],sizeof(up->type_22));
	offset += sizeof(up->type_22);

	stchar(&up->type_23,&buf[offset],sizeof(up->type_23));
	offset += sizeof(up->type_23);

	stchar(&up->type_24,&buf[offset],sizeof(up->type_24));
	offset += sizeof(up->type_24);

	stchar(&up->type_25,&buf[offset],sizeof(up->type_25));
	offset += sizeof(up->type_25);

	stchar(&up->type_26,&buf[offset],sizeof(up->type_26));
	offset += sizeof(up->type_26);

	stchar(&up->type_27,&buf[offset],sizeof(up->type_27));
	offset += sizeof(up->type_27);

	stchar(&up->type_28,&buf[offset],sizeof(up->type_28));
	offset += sizeof(up->type_28);

	stchar(&up->type_29,&buf[offset],sizeof(up->type_29));
	offset += sizeof(up->type_29);

	stchar(&up->type_30,&buf[offset],sizeof(up->type_30));
	offset += sizeof(up->type_30);

	stchar(&up->type_31,&buf[offset],sizeof(up->type_31));
	offset += sizeof(up->type_31);

	stchar(&up->type_32,&buf[offset],sizeof(up->type_32));
	offset += sizeof(up->type_32);

	stchar(&up->auto_free_text, &buf[offset], sizeof( up->auto_free_text ));
	offset += sizeof(up->auto_free_text);

#ifdef NAMERICA
	stchar(&up->hia_1, &buf[offset], sizeof(up->hia_1));
	offset += sizeof(up->hia_1);

	stchar(&up->hia_2, &buf[offset], sizeof(up->hia_2));
	offset += sizeof(up->hia_2);

	stchar(&up->hia_3, &buf[offset], sizeof(up->hia_3));
	offset += sizeof(up->hia_3);

	stchar(&up->hia_4, &buf[offset], sizeof(up->hia_4));
	offset += sizeof(up->hia_4);

	stchar(&up->hia_5, &buf[offset], sizeof(up->hia_5));
	offset += sizeof(up->hia_5);

	stchar(&up->hia_6, &buf[offset], sizeof(up->hia_6));
	offset += sizeof(up->hia_6);

	stchar(&up->hia_7, &buf[offset], sizeof(up->hia_7));
	offset += sizeof(up->hia_7);

	stchar(&up->hia_8, &buf[offset], sizeof(up->hia_8));
	offset += sizeof(up->hia_8);

	stchar(&up->hia_9, &buf[offset], sizeof(up->hia_9));
	offset += sizeof(up->hia_9);

	stchar(&up->hia_10, &buf[offset], sizeof(up->hia_10));
	offset += sizeof(up->hia_10);

	stchar(&up->hia_11, &buf[offset], sizeof(up->hia_11));
	offset += sizeof(up->hia_11);

	stchar(&up->hia_12, &buf[offset], sizeof(up->hia_12));
	offset += sizeof(up->hia_12);

	stchar(&up->hia_13, &buf[offset], sizeof(up->hia_13));
	offset += sizeof(up->hia_13);

	stchar(&up->hia_14, &buf[offset], sizeof(up->hia_14));
	offset += sizeof(up->hia_14);

	stchar(&up->hia_15, &buf[offset], sizeof(up->hia_15));
	offset += sizeof(up->hia_15);

	stchar(&up->hia_16, &buf[offset], sizeof(up->hia_16));
	offset += sizeof(up->hia_16);

	stchar(&up->hia_17, &buf[offset], sizeof(up->hia_17));
	offset += sizeof(up->hia_17);

	stchar(&up->hia_18, &buf[offset], sizeof(up->hia_18));
	offset += sizeof(up->hia_18);
#endif
}

void
unfill_up( req_buf, buf)
     char *req_buf;
     char *buf;
{
	struct  cisam_up  *up;
	register int  offset = 0;

	up = (struct cisam_up*)req_buf;

	up->user_id = ldint(&buf[offset]);
	offset += INTSIZE;

	ldchar(&buf[offset],sizeof(up->user_name)-1,up->user_name);
	offset += sizeof(up->user_name)-1;

	ldchar(&buf[offset],sizeof(up->fullname)-1,up->fullname);
	offset += sizeof(up->fullname)-1;

	ldchar(&buf[offset],sizeof(up->greeting_name)-1,up->greeting_name);
	offset += sizeof(up->greeting_name)-1;

	ldchar(&buf[offset],sizeof(up->menu_perms)-1,up->menu_perms);
	offset += sizeof(up->menu_perms)-1;

	ldchar(&buf[offset],sizeof(up->fleet_perms)-1,up->fleet_perms);
	offset += sizeof(up->fleet_perms)-1;

	up->default_fleet = buf[offset];
	offset += sizeof(up->default_fleet);

	up->default_screen = ldint(&buf[offset]);
	offset += INTSIZE;

	up->default_search = ldint(&buf[offset]);
	offset += INTSIZE;

	up->default_pos = ldint(&buf[offset]);
	offset += INTSIZE;

	up->default_call_tp = buf[offset];
	offset += sizeof(up->default_call_tp);

	ldchar(&buf[offset],sizeof(up->default_city)-1,up->default_city);
	offset += sizeof(up->default_city)-1;

	up->can_zone_calls = buf[offset];
	offset += sizeof(up->can_zone_calls);

	up->high_call_pri = ldint(&buf[offset]);
	offset += INTSIZE;

	up->superzone_1 = ldint(&buf[offset]);
	offset += INTSIZE;

	up->superzone_2 = ldint(&buf[offset]);
	offset += INTSIZE;

	up->superzone_3 = ldint(&buf[offset]);
	offset += INTSIZE;

	up->superzone_4 = ldint(&buf[offset]);
	offset += INTSIZE;

	up->superzone_5 = ldint(&buf[offset]);
	offset += INTSIZE;

	up->superzone_6 = ldint(&buf[offset]);
	offset += INTSIZE;

	up->superzone_7 = ldint(&buf[offset]);
	offset += INTSIZE;

	up->superzone_8  = ldint(&buf[offset]);
	offset += INTSIZE;

	up->superzone_9 = ldint(&buf[offset]);
	offset += INTSIZE;

	up->superzone_10 = ldint(&buf[offset]);
	offset += INTSIZE;

	up->superzone_11 = ldint(&buf[offset]);
	offset += INTSIZE;

	up->superzone_12 = ldint(&buf[offset]);
	offset += INTSIZE;

	up->superzone_13 = ldint(&buf[offset]);
	offset += INTSIZE;

	up->superzone_14 = ldint(&buf[offset]);
	offset += INTSIZE;

	up->superzone_15 = ldint(&buf[offset]);
	offset += INTSIZE;

	up->superzone_16 = ldint(&buf[offset]);
	offset += INTSIZE;

	ldchar(&buf[offset],sizeof(up->logo_file)-1,up->logo_file);
	offset += sizeof(up->logo_file)-1;

	up->group_1 = buf[offset];
	offset += sizeof(up->group_1);

	up->group_2 = buf[offset];
	offset += sizeof(up->group_2);

	up->group_3 = buf[offset];
	offset += sizeof(up->group_3);

	up->group_4 = buf[offset];
	offset += sizeof(up->group_4);

	up->group_5 = buf[offset];
	offset += sizeof(up->group_5);

	up->group_6 = buf[offset];
	offset += sizeof(up->group_6);

	up->group_7 = buf[offset];
	offset += sizeof(up->group_7);

        up->group_8 = buf[offset];
	offset += sizeof(up->group_8);

	up->group_9 = buf[offset];
	offset += sizeof(up->group_9);

	up->group_10 = buf[offset];
	offset += sizeof(up->group_10);

	up->group_11 = buf[offset];
	offset += sizeof(up->group_11);

	up->group_12 = buf[offset];
	offset += sizeof(up->group_12);

	up->type_1 = buf[offset];
	offset += sizeof(up->type_1);

	up->type_2 = buf[offset];
	offset += sizeof(up->type_2);

	up->type_3 = buf[offset];
	offset += sizeof(up->type_3);

	up->type_4 = buf[offset];
	offset += sizeof(up->type_4);

	up->type_5 = buf[offset];
	offset += sizeof(up->type_5);

	up->type_6 = buf[offset];
	offset += sizeof(up->type_6);

	up->type_7 = buf[offset];
	offset += sizeof(up->type_7);

	up->type_8 = buf[offset];
	offset += sizeof(up->type_8);

	up->type_9 = buf[offset];
	offset += sizeof(up->type_9);

	up->type_10 = buf[offset];
	offset += sizeof(up->type_10);

	up->type_11 = buf[offset];
	offset += sizeof(up->type_11);

	up->type_12 = buf[offset];
	offset += sizeof(up->type_12);

	up->type_13 = buf[offset];
	offset += sizeof(up->type_13);

	up->type_14 = buf[offset];
	offset += sizeof(up->type_14);

	up->type_15 = buf[offset];
	offset += sizeof(up->type_15);

	up->type_16 = buf[offset];
	offset += sizeof(up->type_16);

	up->type_17 = buf[offset];
	offset += sizeof(up->type_17);

	up->type_18 = buf[offset];
	offset += sizeof(up->type_18);

	up->type_19 = buf[offset];
	offset += sizeof(up->type_19);

	up->type_20 = buf[offset];
	offset += sizeof(up->type_20);

	up->type_21 = buf[offset];
	offset += sizeof(up->type_21);

	up->type_22 = buf[offset];
	offset += sizeof(up->type_22);

	up->type_23 = buf[offset];
	offset += sizeof(up->type_23);

	up->type_24 = buf[offset];
	offset += sizeof(up->type_24);

	up->type_25 = buf[offset];
	offset += sizeof(up->type_25);

	up->type_26 = buf[offset];
	offset += sizeof(up->type_26);

	up->type_27 = buf[offset];
	offset += sizeof(up->type_27);

	up->type_28 = buf[offset];
	offset += sizeof(up->type_28);

	up->type_29 = buf[offset];
	offset += sizeof(up->type_29);

	up->type_30 = buf[offset];
	offset += sizeof(up->type_30);

	up->type_31 = buf[offset];
	offset += sizeof(up->type_31);

	up->type_32 = buf[offset];
	offset += sizeof(up->type_32);

	up->auto_free_text = buf[offset];
	offset += sizeof(up->auto_free_text);

#ifdef NAMERICA
	up->hia_1 = buf[offset];
	offset += sizeof(up->hia_1);

	up->hia_2 = buf[offset];
	offset += sizeof(up->hia_2);

	up->hia_3 = buf[offset];
	offset += sizeof(up->hia_3);

	up->hia_4 = buf[offset];
	offset += sizeof(up->hia_4);

	up->hia_5 = buf[offset];
	offset += sizeof(up->hia_5);

	up->hia_6 = buf[offset];
	offset += sizeof(up->hia_6);

	up->hia_7 = buf[offset];
	offset += sizeof(up->hia_7);

	up->hia_8 = buf[offset];
	offset += sizeof(up->hia_8);

	up->hia_9 = buf[offset];
	offset += sizeof(up->hia_9);

	up->hia_10 = buf[offset];
	offset += sizeof(up->hia_10);

	up->hia_11 = buf[offset];
	offset += sizeof(up->hia_11);

	up->hia_12 = buf[offset];
	offset += sizeof(up->hia_12);

	up->hia_13 = buf[offset];
	offset += sizeof(up->hia_13);

	up->hia_14 = buf[offset];
	offset += sizeof(up->hia_14);

	up->hia_15 = buf[offset];
	offset += sizeof(up->hia_15);

	up->hia_16 = buf[offset];
	offset += sizeof(up->hia_16);

	up->hia_17 = buf[offset];
	offset += sizeof(up->hia_17);

	up->hia_18 = buf[offset];
	offset += sizeof(up->hia_18);
#endif
}


