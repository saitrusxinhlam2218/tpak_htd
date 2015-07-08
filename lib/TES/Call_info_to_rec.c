/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Call_info_to_rec.c,v $
*  @(#)  $Revision: 1.8 $
*  @(#)  $Date: 2003/01/28 18:33:48 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/lib/TES/Call_info_to_rec.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: Call_info_to_rec.c,v 1.8 2003/01/28 18:33:48 jwelch Exp $";

#include <string.h>

#include "TES.h"
#include "Address.h"
#include "Call_public.h"
#include "Call_private.h"
#include "language.h"

void
TES_call_info_to_rec(CALL_INFO *call_info,
		     void *cr)
{
  CALL_REC *call_rec;
  
  call_rec = (CALL_REC *) cr;
  bzero(call_rec, sizeof(CALL_REC));

  call_rec->fleet = call_info->fleet;
  call_rec->nbr = call_info->call_nbr;
  call_rec->due_date_time = call_info->pickup.due_date_time;
  get_text_from_date_time(call_rec->due_date_time, call_rec->due_date, call_rec->due_time);

  strncpy(call_rec->passenger_name, call_info->pickup.cust.name, NAME_LEN);
  call_rec->passenger_name[NAME_LEN] = EOS;

  call_rec->pckup_str_nbr = call_info->pickup.cust.address.street_nbr;
  call_rec->pckup_str_suf = call_info->pickup.cust.address.street_nbr_suffix;
  strncpy(call_rec->pckup_str_type, call_info->pickup.cust.address.street.type, STREET_TYPE_LEN);
  call_rec->pckup_str_type[STREET_TYPE_LEN] = EOS;

  strncpy(call_rec->pckup_str_name, call_info->pickup.cust.address.street.name, STREET_NAME_LEN);
  call_rec->pckup_str_name[STREET_NAME_LEN] = EOS;

  strncpy(call_rec->pckup_apt_room, call_info->pickup.cust.address.apt, APARTMENT_LEN);
  call_rec->pckup_apt_room[5] = EOS;
  
  strncpy(call_rec->pckup_city, call_info->pickup.cust.address.city_name, CITY_NAME_LEN);
  call_rec->pckup_city[CITY_NAME_LEN] = EOS;

  strncpy(call_rec->pckup_pmap, call_info->pickup.cust.address.map_ref, MAP_COORD_LEN + MAP_REF_BLOCK_LEN);
  call_rec->pckup_pmap[MAP_COORD_LEN+MAP_REF_BLOCK_LEN] = EOS;

  call_rec->dest_str_nbr = call_info->dest.street_nbr;

  strncpy(call_rec->dest_str_name, call_info->dest.street.name, STREET_NAME_LEN);
  call_rec->dest_str_name[STREET_NAME_LEN] = EOS;

  strncpy(call_rec->dest_apt_room, call_info->dest.apt, APARTMENT_LEN);
  call_rec->dest_apt_room[6] = EOS;
  
  strncpy(call_rec->dest_city, call_info->dest.city_name, CITY_NAME_LEN);
  call_rec->dest_city[CITY_NAME_LEN] = EOS;
  
  call_rec->gps_lat = call_info->pickup.cust.address.gps_y;
  call_rec->gps_long = call_info->pickup.cust.address.gps_x;

  call_rec->dest_gpsx = 0.0;
  call_rec->dest_gpsy = 0.0;
  
  strncpy(call_rec->pckup_adr_cmnt, call_info->pickup.cust.address.comment, ADR_COMMENT_LEN);
  call_rec->pckup_adr_cmnt[ADR_COMMENT_LEN] = EOS;

#ifdef FOO
  strncpy(call_rec->phone_nbr, call_info->pickup.cust.phone_nbr, PHONE_NBR_LEN);
  call_rec->phone_nbr[PHONE_NBR_LEN] = EOS;
#endif

  strncpy(call_rec->phone_number, call_info->pickup.cust.phone_nbr, PHONE_NBR_LEN);
  call_rec->phone_number[PHONE_NBR_LEN] = EOS;
  
  strncpy(call_rec->phone_ext, call_info->pickup.cust.phone_ext, PHONE_EXT_LEN);
  call_rec->phone_ext[PHONE_EXT_LEN-1] = EOS;

  call_rec->pckup_zone = call_info->pickup.zone;

  strncpy(call_rec->veh_attr, call_info->pickup.requested_attributes.vehicle, ATTR_MAX);
  call_rec->veh_attr[ATTR_MAX] = EOS;

  strncpy(call_rec->drv_attr, call_info->pickup.requested_attributes.driver, ATTR_MAX);
  call_rec->drv_attr[ATTR_MAX] = EOS;

  if (strchr(call_rec->veh_attr, YES) != NULL)
    call_rec->veh_attr_flag = YES;
  else
    call_rec->veh_attr_flag = NO;

  if (strchr(call_rec->drv_attr, YES) != NULL)
    call_rec->drv_attr_flag = YES;
  else
    call_rec->drv_attr_flag = NO;

  strncpy(call_rec->driver_msg, call_info->pickup.pickup_comments, AC_DRIVER_MSG_LEN);
  call_rec->driver_msg[AC_DRIVER_MSG_LEN] = EOS;

  strncpy(call_rec->general_cmnt, call_info->pickup.general_comments, CALL_GEN_COMMENT_LEN);
  call_rec->general_cmnt[CALL_GEN_COMMENT_LEN] = EOS;

  strncpy(call_rec->charge_nbr, call_info->account_nbr, AC_NUMBER_LEN);
  call_rec->charge_nbr[AC_NUMBER_LEN] = EOS;

  strncpy(call_rec->requested_by, call_info->requested_by, NAME_LEN);
  call_rec->requested_by[NAME_LEN] = EOS;

  strncpy(call_rec->passenger_name, call_info->requested_by, NAME_LEN);
  call_rec->passenger_name[NAME_LEN] = EOS;

  strncpy(call_rec->status, call_info->status, CALL_STATUS_LEN);
  call_rec->status[CALL_STATUS_LEN] = EOS;

  call_rec->veh_nbr = call_info->vehicle_nbr;
  call_rec->personal_veh = call_info->personal_veh;

  call_rec->date_time_in = call_info->originate_date_time;
  get_text_from_date_time(call_rec->date_time_in, call_rec->date_in, call_rec->time_in);

  call_rec->nbr_in_group = (char)( call_info->nbr_in_group + 0x30 );
  call_rec->pos_in_group = (char)( call_info->pos_in_group + 0x30 );

  call_rec->priority = 10; // HTD hard code priority call_info->priority;

}
