/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: TES_send_call.c,v $
*  @(#)  $Revision: 1.10 $
*  @(#)  $Date: 2004/05/12 12:21:32 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/lib/TES/TES_send_call.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: TES_send_call.c,v 1.10 2004/05/12 12:21:32 jwelch Exp $";

#include <cursesX.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>

#include "taxipak.h"
#include "taxi_defs.h"
#include "mads_types.h"
#include "Object.h"
#include "Address.h"
#include "Call_public.h"
#include "Call_private.h"
#include "Call_db.h"
#include "language.h"
#include "ui_ipc.h"
#include "ui_strdefs.h"
#include "writer.h"
#include "taxi_db.h"
#include "except.h"
#include "TES.h"
#include "path.h"
#include "Fleet_public.h"
#include "Fleet_private.h"
#include "Fleet_db.h"
#include "Call_hist_db.h"
#include "Account_db.h"
#include "Unzoned_db.h"
#include "Zone_db.h"


void call_entry_to_call_record();
RET_STATUS complete_call_record();
BOOLEAN is_dispatch_up();
RET_STATUS send_call_to_dispatch();
void set_error();
void write_unzoned_call();
void setup_time_call();
void TES_get_call_class_and_client_type(int, char *, char *);
void TES_get_phone_line_from_account_type(int, char *);
int add_call_history_record(CALL_REC *, char *, short );

char TES_call_error[132];
extern char *sys_errlist[];


RET_STATUS
TES_send_call(CALL_INFO *call_info)
{
  CALL_REC call;
  struct call_ipc_rec call_ipc;
  RET_STATUS rs;
  char c;
  int call_isrecnum;
  char field_error[80];
  RET_STATUS validate_call();
  RET_STATUS write_call_record();
  RET_STATUS update_call_record();
  void call_record_to_call_ipc();
  short points;
  int zone;
  ACCOUNT_REC account;
  MT_ZONE mt_zone;


  if (!is_dispatch_up())
  {
    return(err_encode_special(TES_OBJ, APP_ERR_GP, TES_ERR_NO_DISPATCH));
  }

  TES_call_info_to_rec(call_info, &call);

  strncpy(call.phone_nbr, call_info->pickup.cust.phone_nbr, PHONE_NBR_LEN);
  call.phone_nbr[7] = EOS;
  strncpy(call.phone_number, call_info->pickup.cust.phone_nbr, PHONE_NBR_LEN);
  call.phone_number[PHONE_NBR_LEN] = EOS;
  
  rs = validate_call(&call);

  if (rs != SUCCESS)
  {
    return(rs);
  }

  setup_time_call(&call);

  rs = complete_call_record(&call);
  if (rs != SUCCESS)
  {
    return(rs);
  }


  if (call.nbr == 0)
  {
    if (call.charge_nbr[0] != EOS)
    {
      rs = open_table(ACCOUNT_FILE_ID, DB_READ_WRITE | DB_MANUAL_LOCK);
      if (rs != SUCCESS)
	return(rs);
      
      account.fleet = call.fleet;
      strcpy(account.nbr, call.charge_nbr );
      
      rs = db_read_key(ACCOUNT_FILE_ID, &account, &ac_key1, DB_EQUAL);
      if (rs != SUCCESS)
      {
	return(rs);
      }
      
      strcpy(call.company_name, account.name);
      strcpy(call.general_cmnt, account.driver_msg);
      strcpy(call.requested_by, account.name);
      call.extended_type[2] = AUTOBOOK_CALL;
      call.extended_type[3] = CHARGE;
      
      
    } /* end if (call.charge_nbr[0] != EOS) */
    else
      {
        call.extended_type[1] = 'W';
        call.extended_type[2] = 'A';
        call.extended_type[3] = 'P';
      }
  }

  if (call.nbr == 0)
  {
    rs = write_call_record(&call, &call_isrecnum);
    add_call_history_record(&call, ENTERED, 0);
  }
  else
  {
    rs = update_call_record(&call, &call_isrecnum);
    add_call_history_record(&call, UPDATED, 0);
  }

  if (rs != SUCCESS)
    return(rs);

  if (call.pckup_zone == 0)
  {
    write_unzoned_call(&call);
    /* Need to make unzoned a catgets call */
    add_call_history_record(&call, "UNZONED", UNZONED);
  }
  
  /* Do not send the call to dispatch if it is a time call.
     Unless it's a unzone call, which is always sent to dispatch */
  if (call.extended_type[CALL_TIME_OFFSET] != TIMEC || call.pckup_zone == 0)
  {
  
    call_record_to_call_ipc(&call, &call_ipc, call_isrecnum);
    
    rs = send_call_to_dispatch(&call_ipc);
    if (rs != SUCCESS)
    {
      return(err_encode_special(TES_OBJ, APP_ERR_GP, TES_ERR_SEND_TO_DISPATCH_FAILED));
    }
  }


  call_info->call_nbr = call.nbr;

  return(SUCCESS);
}

RET_STATUS
TES_Ilink_send_call(CALL_INFO *call_info, int timeout)
{
  CALL_REC call;
  struct call_ipc_rec call_ipc;
  RET_STATUS rs;
  char c;
  int call_isrecnum;
  char field_error[80];
  RET_STATUS validate_call();
  RET_STATUS write_call_record();
  RET_STATUS update_call_record();
  void call_record_to_call_ipc();
  short points;
  int zone;
  ACCOUNT_REC account;
  MT_ZONE mt_zone;


  if (!is_dispatch_up())
  {
    return(err_encode_special(TES_OBJ, APP_ERR_GP, TES_ERR_NO_DISPATCH));
  }

  TES_call_info_to_rec(call_info, &call);

  rs = validate_call(&call);

  if (rs != SUCCESS)
  {
    return(rs);
  }

  setup_time_call(&call);

  rs = complete_call_record(&call);
  if (rs != SUCCESS)
  {
    return(rs);
  }


  if (call.nbr == 0)
  {
    if (call.charge_nbr[0] != EOS)
    {
      rs = open_table(ACCOUNT_FILE_ID, DB_READ_WRITE | DB_MANUAL_LOCK);
      if (rs != SUCCESS)
	return(rs);
      
      account.fleet = call.fleet;
      strcpy(account.nbr, call.charge_nbr );
      
      rs = db_read_key(ACCOUNT_FILE_ID, &account, &ac_key1, DB_EQUAL);
      if (rs != SUCCESS)
      {
	return(rs);
      }
      
      strcpy(call.company_name, account.name);
      strcpy(call.general_cmnt, account.driver_msg);
      strcpy(call.requested_by, account.name);
      call.extended_type[2] = AUTOBOOK_CALL;
      call.extended_type[3] = CHARGE;
      
      
    } /* end if (call.charge_nbr[0] != EOS) */
    else
      {
        call.extended_type[1] = '8';
	//        call.extended_type[2] = '0';
	//        call.extended_type[3] = '0';
      }
  }

  if (call.nbr == 0)
  {
    rs = write_call_record(&call, &call_isrecnum);
    add_call_history_record(&call, ENTERED, 0);
  }
  else
  {
    rs = update_call_record(&call, &call_isrecnum);
    add_call_history_record(&call, UPDATED, 0);
  }

  if (rs != SUCCESS)
    return(rs);

  if (call.pckup_zone == 0)
  {
    write_unzoned_call(&call);
    /* Need to make unzoned a catgets call */
    add_call_history_record(&call, "UNZONED", UNZONED);
  }
  
  call_record_to_call_ipc(&call, &call_ipc, call_isrecnum);

  call_ipc.ilink_timeout = timeout;

  // Set Ilink call type
  call_ipc.call_type.ilink = 1;
  
  rs = send_call_to_dispatch(&call_ipc);
  if (rs != SUCCESS)
    {
      return(err_encode_special(TES_OBJ, APP_ERR_GP, TES_ERR_SEND_TO_DISPATCH_FAILED));
    }

  call_info->call_nbr = call.nbr;

  return(SUCCESS);
}

RET_STATUS
send_call_to_dispatch(struct call_ipc_rec *call_ipc)
{
  static int fifo_fd = -1;

  if (!is_dispatch_up())
  {
    return(FAIL);
  }

  if (fifo_fd == -1)
  {
    /* open FIFO to write requests to dispatch process */
    if((fifo_fd = open(USER_PIPE, O_RDWR | O_NDELAY)) == -1)
    {
      return(FAIL);
    }
  }

  if (write(fifo_fd, call_ipc, sizeof(struct call_ipc_rec)) == -1)
  {
    return(FAIL);
  }
  return(SUCCESS);
}

/*
  Fill in the parts of the call record not set up on the
  call entry screens.
*/
RET_STATUS
complete_call_record(CALL_REC *call)
{
  RET_STATUS rs;
  int i;
  time_t tmp_time;

  call->date_time_in = time(NULL);

  call->entered_by = MINITEL_USER_ID;
  get_text_from_date_time(call->date_time_in, call->date_in, call->time_in);

  get_date_time_from_text(&tmp_time, call->due_date,
			  call->due_time);
  call->due_date_time = tmp_time;

  /* This field may be removed in the future */
  strncpy(call->pass_name_key, call->passenger_name, CALL_PASSENGER_KEY_LEN);
  call->pass_name_key[CALL_PASSENGER_KEY_LEN] = EOS;

  /* This field may be removed in the future */
  strncpy(call->pckup_str_key, call->pckup_str_name, CALL_STREET_NAME_KEY_LEN);
  call->pckup_str_key[CALL_STREET_NAME_KEY_LEN] = EOS;

  call->extended_type[0] = PASS;
  call->extended_type[1] = ' ';

  /* While doing an update of call, cannot change priority/status */
  /* PENDING time call */
  if (call->extended_type[CALL_TIME_OFFSET] == TIMEC)
  {
    call->pri_status = CALL_PENDING_PRI_STATUS;
    strcpy(call->status, PENDING);
  }
    
  /* Not a time call, regular priority */    
  else
  {
    call->pri_status = call->priority;
    strcpy(call->status, UNASSGND);
  }

  /* This is really an unused field. Each call record only
     refers to one call per vehicle. call->nbr_in_group stores
     the number of calls created from the one call entry */
  call->nbr_of_vehs = '1';


  /* Any NULLs in the extended type must be set to spaces
     so the fill routine works properly. */
  for (i = 0; i < CALL_EXT_TYPE_LEN; i ++)
    if (call->extended_type[i] == EOS)
      call->extended_type[i] = ' ';

  /* Don't let something other that 'Y' or 'N' sneak in */
  for (i = 0; i < ATTR_MAX; i ++)
  {
    if (call->drv_attr[i] != YES && call->drv_attr[i] != NO)
      call->drv_attr[i] = NO;

    if (call->veh_attr[i] != YES && call->veh_attr[i] != NO)
      call->veh_attr[i] = NO;
  }

//  if (call->extended_type[CALL_TIME_OFFSET] == TIMEC)
//  {
  // default to PREMIUM for all calls
    call->drv_attr_flag = YES;
    call->drv_attr[25] = YES;
    call->veh_attr_flag = YES;
    call->veh_attr[25] = YES;
//  }  

  /* Get street index so searches can be made from UI */
#ifdef FOO
  strcpy(street.name, call->pckup_address.street_name);
  strcpy(street.type, call->pckup_address.street_type);

  Geo_get_street_index(&street, &call->ext.pckup_str_index);
#endif

  return(SUCCESS);
}


RET_STATUS
validate_call(CALL_REC *call)
{
  
  /* Must have something in pickup street name */
  if (call->pckup_str_name[0] == EOS)
  {
    return(err_encode_special(TES_OBJ, APP_ERR_GP, TES_ERR_MISSING_STREET_NAME));
  }
  
  if (call->pckup_city[0] == EOS)
  {
    return(err_encode_special(TES_OBJ, APP_ERR_GP, TES_ERR_MISSING_CITY_NAME));
  }


  return(SUCCESS);
}

RET_STATUS
write_call_record(CALL_REC *call, int *call_isrecnum)
{
  RET_STATUS rs;

  rs = open_table(CALL_FILE_ID, DB_READ_WRITE | DB_MANUAL_LOCK);
  if (rs != SUCCESS)
    return(rs);

  rs = db_write(CALL_FILE_ID, call);
  if (rs != SUCCESS)
    return(rs);

  /* Call number is automatically filled in */

  *call_isrecnum = isrecnum;
  return(SUCCESS);
}


RET_STATUS
update_call_record(CALL_REC *call, int *call_isrecnum)
{
  RET_STATUS rs;
  CALL_REC tmp_call;

  rs = open_table(CALL_FILE_ID, DB_READ_WRITE | DB_MANUAL_LOCK);
  if (rs != SUCCESS)
    return(rs);

  /* Find the record to update */
  tmp_call.nbr = call->nbr;
  rs = db_read_key(CALL_FILE_ID, &tmp_call, &cl_key1, DB_EQUAL | DB_LOCK);
  if (rs != SUCCESS)
    return(rs);

  rs = db_update(CALL_FILE_ID, call);
  if (rs != SUCCESS)
  {
    db_release(CALL_FILE_ID);
    return(rs);
  }

  db_release(CALL_FILE_ID);

  *call_isrecnum = isrecnum;
  return(SUCCESS);
}


void
call_record_to_call_ipc(CALL_REC *call_ptr, struct call_ipc_rec *call_ipc_ptr, int call_isrecnum)
{
  int i;

  bzero(call_ipc_ptr, sizeof(struct call_ipc_rec));

  call_ipc_ptr->u_prcs_id = getpid();
  call_ipc_ptr->rec_type = CALL_REQ;

/*****
  Only one cab for minitel calls now

  if (call_ptr->nbr_in_group > max_cabs_per_call)
    call_ipc_ptr->status.extra_taxis = 1;
***/

  if (call_ptr->extended_type[CALL_TYPE_OFFSET] == WAKEUP)
    call_ipc_ptr->status.wakeup = 1;
  else
    if (call_ptr->pckup_zone == 0)
      call_ipc_ptr->status.unzoned = 1;

  call_ipc_ptr->due_time = call_ptr->due_date_time;
  call_ipc_ptr->c_isam_nbr = call_isrecnum;
  call_ipc_ptr->call_nbr = call_ptr->nbr;
  call_ipc_ptr->grouped_with = call_ptr->grouped_with;
  call_ipc_ptr->fleet = call_ptr->fleet;
  call_ipc_ptr->pickup_zone = call_ptr->pckup_zone;
  for (i = 0; i < ATTR_MAX; i ++)
  {
    if (call_ptr->drv_attr[i] == YES)
      set_bit(i, sizeof(struct drv_attributes), &call_ipc_ptr->drv_attr);

    if (call_ptr->veh_attr[i] == YES)
      set_bit(i, sizeof(struct veh_attributes), &call_ipc_ptr->veh_attr);
  }

  call_ipc_ptr->priority = 10; // HTD wants fixed to priority 10 call_ptr->priority;

  set_call_type_for_ipc(call_ptr, call_ipc_ptr);
}

set_call_type_for_ipc(CALL_REC *call, struct call_ipc_rec *call_ipc)
{
  
  bzero((char *)&call_ipc->call_type, sizeof(struct call_types));
  
  switch (call->extended_type[CALL_TYPE_OFFSET])
  {
    case PASS:
    call_ipc->call_type.passenger = 1;
    break;

    case DELIV:
    call_ipc->call_type.delivery = 1;
    break;

    case TYPEX:
    call_ipc->call_type.xtype = 1;
    break;

    case WAKEUP:
    call_ipc->call_type.wakeup = 1;
    break;
  }
  
  if (call->extended_type[CALL_MULT_OFFSET] == MULT_CALL)
    call_ipc->call_type.multiple = 1;
  
  switch (call->extended_type[CALL_TIME_OFFSET])
  {
    case SUBS:
    call_ipc->call_type.subscription = 1;
    break;
  
    case TIMEC:
    call_ipc->call_type.time = 1;
    break;
  }
  
  /* check the last char's */
  switch (call->extended_type[CALL_CHARGE_OFFSET])
  {
    case CHARGE:
    call_ipc->call_type.charge = 1;
    break;
  }

  call_ipc->FTJ_nbr = -1;
  
}

BOOLEAN
is_dispatch_up()
{
  FILE *fd;
  int  rc;
  static BOOLEAN first_time = TRUE;
  static int dispatch_pid;
  
  if (first_time)
  {
    if ((fd = fopen(DISPATCH_PID_FILE, "r")) == NULL)
    {
      return(FALSE);
    }
  
    fscanf(fd, "%d", &dispatch_pid);
    fclose(fd);
    first_time = FALSE;
  }
  
  /* Check if dispatch PID is still up. kill(pid, 0) returns
     a -1 and an errno of ESRCH if the PID does not exist */
  rc = kill(dispatch_pid, 0);
  if (rc == -1 && errno == ESRCH)
    return(FALSE);
  return(TRUE);
}

int
add_call_history_record(CALL_REC *call,
			char *event,
			short action)
{

  CALL_HIST_REC callh;
  char date_str[DATE_LEN + 1];
  char time_str[TIME_LEN + 1];
  RET_STATUS rs;
  
  
  rs = open_table(CALLH_FILE_ID, DB_READ_WRITE | DB_MANUAL_LOCK);
  if (rs != SUCCESS)
    return(rs);

  bzero((char *)&callh, sizeof(callh));
  
  callh.call_nbr = call->nbr;

  callh.fleet = call->fleet;
  callh.event_type = REGULAR_EVENT;					/* regular exception */
  callh.exception_nbr = action;
  callh.status_dt_tm = (int) time(NULL);
  get_pkup_date(date_str); 					/* get the date in display format */
  strcpy(callh.status_date, date_str);
  get_pkup_time(time_str); 					/* get the time in display format */
  strcpy(callh.status_time, time_str);
  strcpy(callh.event_desc, event);
  callh.veh_nbr = 0;
  callh.drv_id = 0;
  callh.user_id = (short) MINITEL_USER_ID;

  rs = db_write(CALLH_FILE_ID, &callh);

  return(rs);

}

void
setup_time_call(CALL_REC *call)
{
  RET_STATUS rs;
  ZONE_REC zone_rec;
  time_t pickup_lead_time;


  /* Since unzoned calls are immediately sent to dispatch, they
     are setup as time calls */
  if (call->pckup_zone == 0)
    return;

  if (open_table(ZONE_FILE_ID, DB_READ_ONLY | DB_MANUAL_LOCK) != SUCCESS)
    return;

  zone_rec.fleet = call->fleet;
  zone_rec.nbr = call->pckup_zone;
  rs = db_read_key(ZONE_FILE_ID, &zone_rec, &zn_key1, DB_EQUAL);
  if (rs == SUCCESS)
  {
    strcpy(call->zone_desc, zone_rec.desc);
    
    pickup_lead_time = zone_rec.pckup_lead_time * 60; /* convert minutes to seconds */

    /* WARNING!!: get_super_lead() accesses shared fleet memory */
    /* pickup_lead_time = get_super_lead(pickup_lead_time, zone_rec.superzn_nbr, call->fleet); */
  }    

  /* Check for time call; everything less than min_time is a NOW call */
  if (call->due_date_time > time ((time_t) NULL) + pickup_lead_time)
  {
    call->extended_type[CALL_TIME_OFFSET] = TIMEC;

    /* Because extended_type may be filled with NULLS,
       have to put in a space so the TIMEC comes through */
    call->extended_type[1] = ' ';
    return;
  }

  call->extended_type[CALL_TIME_OFFSET] = ' ';
}

void
set_error(char *format,
	  long p1, long p2, long p3, long p4,
	  long p5, long p6, long p7, long p8)
{
  int start_col;

  sprintf(TES_call_error, format, p1, p2, p3, p4, p5, p6, p7, p8);
}

/* set_bit: sets the bit specified by bit_num to 1. The size of the bit structure and its address 
 *		should be passed as arguments
 */
set_bit(bit_num,size,struct_ptr)
	int bit_num;	/* # of the bit to be tested; leaset significant = 0 */
	int size;	/* size of bit struct in bytes */
	unsigned char *struct_ptr;	/* pointer to the bit struct */
{
	unsigned char *bit_ptr;	/* poinetr to bit struct */
	unsigned char mask;
	int shift_by;	/* shift the mask by */

	mask = 0x80;
	bit_ptr = (unsigned char *)struct_ptr + bit_num/8;
	shift_by = bit_num % 8;
	mask >>= shift_by;

	*bit_ptr = *bit_ptr | mask;

}  /* end set_bit() */


void
write_unzoned_call(CALL_REC *call_ptr)
{
  UNZONE_REC uz;
  RET_STATUS rs;

  rs = open_table(UNZONE_FILE_ID, DB_READ_WRITE + DB_MANUAL_LOCK);
  if (rs != SUCCESS)
  {
    return;
  }
  
  bzero((char *) &uz, sizeof(uz));

  uz.call_nbr = call_ptr->nbr;
  uz.fleet = call_ptr->fleet;
  strcpy(uz.call_date, call_ptr->date_in);
  strcpy(uz.call_time, call_ptr->time_in);
  uz.entered_by = call_ptr->entered_by;
  uz.street_nbr = call_ptr->pckup_str_nbr;
  uz.type_of_service = call_ptr->extended_type[0];
  strcpy(uz.charge_nbr, call_ptr->charge_nbr);
  strcpy(uz.str_place_name1, call_ptr->pckup_str_name);
  strcpy(uz.str_type1, call_ptr->pckup_str_type);
  strcpy(uz.pre_dir1, call_ptr->pckup_pre_dir);
  strcpy(uz.post_dir1, call_ptr->pckup_post_dir);
  strcpy(uz.pre_dir2, call_ptr->intr_pre_dir);
  strcpy(uz.str_place_name2, call_ptr->intr_str_name);
  strcpy(uz.str_type2, call_ptr->intr_str_type);
  strcpy(uz.post_dir1, call_ptr->intr_post_dir);
  strcpy(uz.city, call_ptr->pckup_city);
  
  rs = db_write(UNZONE_FILE_ID, &uz);
  
  db_close(UNZONE_FILE_ID);
}


