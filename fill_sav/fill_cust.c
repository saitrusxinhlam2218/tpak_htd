/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: fill_cust.c,v $
*  @(#)  $Revision: 1.11 $
*  @(#)  $Date: 2003/01/28 16:30:04 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/fill/fill_cust.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: fill_cust.c,v $	$Revision: 1.11 $"

static char rcsid[] = "$Id: fill_cust.c,v 1.11 2003/01/28 16:30:04 jwelch Exp $";

#define KEY_SRC

#include "Customer_db.h"
#include "taxi_db.h"

void
fill_cust_key( req_buf, buf )
     char *req_buf;
     char *buf;
{

  struct cisam_cust  *cust;

  cust = (struct cisam_cust *)req_buf;
  stchar(cust->phone_nbr, &buf[4], sizeof(cust->phone_nbr)-1);
  
}

void
fill_cust( req_buf, buf, action)
     char *req_buf;
     char *buf;
     int action;

{
  struct cisam_cust    *cust;
  register int offset = 0;
  long serial_nbr = 0;

  cust = (struct cisam_cust *)req_buf;

  if (action == ADD_REC)
    {
      isuniqueid(Db_files[CUSTOMER_FILE_ID].fd, &serial_nbr);
      cust->nbr = (int)serial_nbr;
    }

  stlong(cust->nbr, &buf[offset]);
  offset += LONGSIZE;
  
  stchar(cust->phone_nbr, &buf[offset], sizeof(cust->phone_nbr)-1);
  offset += sizeof(cust->phone_nbr)-1;

  stchar(cust->phone_ext, &buf[offset], sizeof(cust->phone_ext)-1);
  offset += sizeof(cust->phone_ext)-1;

  stchar(&cust->fleet, &buf[offset], sizeof(cust->fleet));
  offset += sizeof(cust->fleet);

  stlong(cust->pckup_str_nbr, &buf[offset]);
  offset += LONGSIZE;

  stchar(&cust->pckup_str_suf, &buf[offset], sizeof(cust->pckup_str_suf));
  offset += sizeof(cust->pckup_str_suf);

  stchar(cust->pckup_pre_dir, &buf[offset], sizeof(cust->pckup_pre_dir)-1);
  offset += sizeof(cust->pckup_pre_dir)-1;

  stchar(cust->pckup_str_name, &buf[offset], sizeof(cust->pckup_str_name)-1);
  offset += sizeof(cust->pckup_str_name)-1;

  stchar(cust->pckup_str_key, &buf[offset], sizeof(cust->pckup_str_key)-1);
  offset += sizeof(cust->pckup_str_key)-1;

  stchar(cust->pckup_str_type, &buf[offset], sizeof(cust->pckup_str_type)-1);
  offset += sizeof(cust->pckup_str_type)-1;

  stchar(cust->pckup_post_dir, &buf[offset], sizeof(cust->pckup_post_dir)-1);
  offset += sizeof(cust->pckup_post_dir)-1;

  stchar(cust->pckup_apt_room, &buf[offset], sizeof(cust->pckup_apt_room)-1);
  offset += sizeof(cust->pckup_apt_room)-1;

  stchar(cust->pckup_city, &buf[offset], sizeof(cust->pckup_city)-1);
  offset += sizeof(cust->pckup_city)-1;

  stint(cust->pckup_zone, &buf[offset]);
  offset += INTSIZE;

  stchar(cust->passenger_name, &buf[offset], sizeof(cust->passenger_name)-1);
  offset += sizeof(cust->passenger_name)-1;

  stlong(cust->access_date_time, &buf[offset]);
  offset += LONGSIZE;

  stlong(cust->made_calls, &buf[offset]);
  offset += LONGSIZE;

  stchar(cust->veh_attr,&buf[offset],sizeof(cust->veh_attr)-1);
  offset += sizeof(cust->veh_attr) -1;
  
  stchar(cust->drv_attr,&buf[offset],sizeof(cust->drv_attr)-1);
  offset += sizeof(cust->drv_attr) -1;
  
  stchar(cust->customer_comment,&buf[offset], sizeof(cust->customer_comment)-1);
  offset += sizeof(cust->customer_comment) -1;
}

void
unfill_cust( req_buf, buf )
     char *req_buf;
     char *buf;
{
  struct cisam_cust  *cust;
  register int offset = 0;
  char temp_attr[65];  

  cust = (struct cisam_cust *)req_buf;

  cust->nbr = ldlong(&buf[offset]);
  offset += LONGSIZE;

  ldchar(&buf[offset], sizeof(cust->phone_nbr)-1, cust->phone_nbr);
  offset += sizeof(cust->phone_nbr)-1;

  ldchar(&buf[offset], sizeof(cust->phone_ext)-1, cust->phone_ext);
  offset += sizeof(cust->phone_ext)-1;

  cust->fleet = buf[offset];
  offset += sizeof(cust->fleet);

  cust->pckup_str_nbr = ldlong(&buf[offset]);
  offset += LONGSIZE;

  cust->pckup_str_suf = buf[offset];
  offset += sizeof(cust->pckup_str_suf);

  ldchar(&buf[offset], sizeof(cust->pckup_pre_dir)-1, cust->pckup_pre_dir);
  offset += sizeof(cust->pckup_pre_dir)-1;

  ldchar(&buf[offset], sizeof(cust->pckup_str_name)-1, cust->pckup_str_name);
  offset += sizeof(cust->pckup_str_name)-1;

  ldchar(&buf[offset], sizeof(cust->pckup_str_key)-1, cust->pckup_str_key);
  offset += sizeof(cust->pckup_str_key)-1;

  ldchar(&buf[offset], sizeof(cust->pckup_str_type)-1, cust->pckup_str_type);
  offset += sizeof(cust->pckup_str_type)-1;

  ldchar(&buf[offset], sizeof(cust->pckup_post_dir)-1, cust->pckup_post_dir);
  offset += sizeof(cust->pckup_post_dir)-1;

  ldchar(&buf[offset], sizeof(cust->pckup_apt_room)-1, cust->pckup_apt_room);
  offset += sizeof(cust->pckup_apt_room)-1;

  ldchar(&buf[offset], sizeof(cust->pckup_city)-1, cust->pckup_city);
  offset += sizeof(cust->pckup_city)-1;

  cust->pckup_zone = ldint(&buf[offset]);
  offset += INTSIZE;

  ldchar(&buf[offset], sizeof(cust->passenger_name)-1, cust->passenger_name);
  offset += sizeof(cust->passenger_name)-1;

  cust->access_date_time = ldlong(&buf[offset]);
  offset += LONGSIZE;

  cust->made_calls = ldlong(&buf[offset]);
  offset += LONGSIZE;

  ldchar(&buf[offset],sizeof(cust->veh_attr)-1,temp_attr); /* Fill attribute string with spaces */
  sprintf(cust->veh_attr,"%-64.64s",temp_attr);             /* as all 64 characters are needed in all TaxiPak routines */
  offset += sizeof(cust->veh_attr) -1;
  
  ldchar(&buf[offset],sizeof(cust->drv_attr)-1,temp_attr); /* Fill attribute string with spaces */
  sprintf(cust->drv_attr,"%-64.64s",temp_attr);             /* as all 64 characters are needed in all TaxiPak routines */
  offset += sizeof(cust->drv_attr) -1;
  
  ldchar(&buf[offset], sizeof(cust->customer_comment)-1, cust->customer_comment);
  offset += sizeof(cust->customer_comment)-1;
}
