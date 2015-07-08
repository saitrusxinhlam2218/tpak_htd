/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: pi_log.c,v $
*  @(#)  $Revision: 1.3 $
*  @(#)  $Date: 2003/01/24 14:26:54 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/pi/pi_log.c,v $
*
*  Copyright (c) 2000 - MobileSoft Consulting, Inc. Dublin OH
*
***********************************************************************/

static char rcsid[] = "$Id: pi_log.c,v 1.3 2003/01/24 14:26:54 jwelch Exp $";

#include <sys/types.h>
#include <sys/ipc.h>
#include <time.h>
#include <pwd.h>
#include <errno.h>
#include "pi.h"
#include "pi_struc.h"
#include "pi_account_defines.h"

extern char szLogMsg[];

LogMsg( pi_msg_type, direction, data_pkg )
  int pi_msg_type;
char *direction;
char *data_pkg;
{
  switch (pi_msg_type) {
  case PI_DISPATCH_ACCOUNT_CALL:
    LogMsg_Dispatch_Account_Call( direction, data_pkg );
    MakeLogEntry( szLogMsg );
    break;

  case PI_DISPATCH_CALL:
    LogMsg_Dispatch_Call( direction, data_pkg );
    MakeLogEntry( szLogMsg );
    break;

  case PI_DISP_EXT_CALL:
    LogMsg_Dispatch_Ext_Call( direction, data_pkg );
    MakeLogEntry( szLogMsg );
    break;

  case PI_CALL_NBR:
    LogMsg_Call_Nbr( direction, data_pkg );
    MakeLogEntry( szLogMsg );
    break;    

  case PI_GET_CALL:
    LogMsg_Get_Call( direction, (char *)(data_pkg) );
    MakeLogEntry( szLogMsg );
    break;
  
  default:
    LogMsg_Unknown_Type( direction, data_pkg );
    break;
  }
}

LogMsg_Get_Call( direction, data_pkg )
  char *direction;
char *data_pkg;
{
  PI_CALL_REC *call_rec;
  short       *status;

  if ( !strcmp(direction, PI_TO_EXTP) )
    {
      status = (short *)data_pkg;
      call_rec = (PI_CALL_REC *)(data_pkg+2);
      sprintf(szLogMsg, "%s PI_GET_CALL <%d><%c><%d><%hd><%d><%d><%d><%d><%-5s><%-21s><%d><%c><%-6s><%-4s><%d><%-31s><%-21s><%-11s><%-21s><%d><%c><%-6s><%-4s><%d><%-31s><%d><%-32s><%-6s><%-32s><%d><%-7s><%-5s><%-7s><%-5s><%-5s><%-5s><%-65s><%d>\n",
              direction, *status, call_rec->fleet, call_rec->call_number,
              call_rec->priority, call_rec->call_group, call_rec->call_position,
              call_rec->number_of_calls, call_rec->number_of_vehicles,
              call_rec->call_type, call_rec->from_addr_street, call_rec->from_addr_number,
              call_rec->from_addr_suffix, call_rec->from_addr_apart, call_rec->from_addr_city,
              call_rec->from_addr_zone, call_rec->from_addr_cmnt, call_rec->passenger, call_rec->phone,
              call_rec->to_addr_street, call_rec->to_addr_number, call_rec->to_addr_suffix,
              call_rec->to_addr_apart, call_rec->to_addr_city, call_rec->to_addr_zone, call_rec->to_addr_cmnt,
              call_rec->car_number, call_rec->car_attrib, call_rec->driver_id, call_rec->driver_attrib,
              call_rec->creator, call_rec->creation_date, call_rec->creation_time, call_rec->due_date,
              call_rec->due_time, call_rec->pickup_time, call_rec->close_time, call_rec->call_comment,
              call_rec->call_status);
    }
  else
    {
      call_rec = (PI_CALL_REC *)data_pkg;
      sprintf(szLogMsg, "%s PI_GET_CALL <%c><%d><%hd><%d><%d><%d><%d><%-5s><%-21s><%d><%c><%-6s><%-4s><%d><%-31s><%-21s><%-11s><%-21s><%d><%c><%-6s><%-4s><%d><%-31s><%d><%-32s><%-6s><%-32s><%d><%-7s><%-5s><%-7s><%-5s><%-5s><%-5s><%-65s><%d>\n",
              direction, call_rec->fleet, call_rec->call_number,
              call_rec->priority, call_rec->call_group, call_rec->call_position,
              call_rec->number_of_calls, call_rec->number_of_vehicles,
              call_rec->call_type, call_rec->from_addr_street, call_rec->from_addr_number,
              call_rec->from_addr_suffix, call_rec->from_addr_apart, call_rec->from_addr_city,
              call_rec->from_addr_zone, call_rec->from_addr_cmnt, call_rec->passenger, call_rec->phone,
              call_rec->to_addr_street, call_rec->to_addr_number, call_rec->to_addr_suffix,
              call_rec->to_addr_apart, call_rec->to_addr_city, call_rec->to_addr_zone, call_rec->to_addr_cmnt,
              call_rec->car_number, call_rec->car_attrib, call_rec->driver_id, call_rec->driver_attrib,
              call_rec->creator, call_rec->creation_date, call_rec->creation_time, call_rec->due_date,
              call_rec->due_time, call_rec->pickup_time, call_rec->close_time, call_rec->call_comment,
              call_rec->call_status);      
    }
}

LogMsg_Call_Nbr( direction, data_pkg )
  char *direction;
char *data_pkg;
{
  int cl_nbr;
  short status;
  char *position;
  int len;


  position = (char *) data_pkg;
  len = 12;
  get_from_data_blk( PI_SHORT_TYPE, &position, (char *)&status, &len );
  position = (char *)(data_pkg + 2);;
  get_from_data_blk( PI_INT_TYPE, &position, (char *) &cl_nbr, &len );  
  
  sprintf(szLogMsg, "%s PI_CALL_NBR <%d><%d>\n", direction, status, cl_nbr);
}

LogMsg_Dispatch_Ext_Call( direction, data_pkg )
  char *direction;
char *data_pkg;
{
  CALL_EXT_REC *call_rec;
  int cl_nbr;
  short status;
  int len;
  char *position;
  
  if ( !strcmp(direction, PI_TO_EXTP) )
    {
      position = (char *) data_pkg;
      len = 12;
      get_from_data_blk( PI_SHORT_TYPE, &position, (char *)&status, &len );
      position = (char *)(data_pkg + 2);
      get_from_data_blk( PI_INT_TYPE, &position, (char *) &cl_nbr, &len );

      sprintf(szLogMsg, "%s PI_DISPATCH_EXT_CALL <%d><%d>\n",
              direction, status, cl_nbr);
    }
  else
    {
      call_rec = (CALL_EXT_REC *)data_pkg;
      if ( call_rec->from_addr_suffix == '\0' )
        call_rec->from_addr_suffix = ' ';
      if ( call_rec->to_addr_suffix == '\0' )
        call_rec->to_addr_suffix = '\0';
      
      sprintf(szLogMsg, "%s PI_DISPATCH_EXT_CALL <%c><%d><%d><%d><%d><%d><%d><%-5.5s><%-21.21s><%d><%c><%-6.6s><%-4.4s><%d><%-31.31s><%-21.21s><%-11.11s><%-21.21s><%d><%c><%-6.6s><%-4.4s><%d><%-31.31s><%d><%-8.8s><%-6.6s><%-8.8s><%d><%-7.7s><%-5.5s><%-7.7s><%-5.5s><%-5.5s><%-5.5s><%-65.65s><%d><%d><%c><%c><%c>\n",
              direction, call_rec->fleet, call_rec->call_number,
              call_rec->priority, call_rec->call_group, call_rec->call_position,
              call_rec->number_of_calls, call_rec->number_of_vehicles,
              call_rec->call_type, call_rec->from_addr_street, call_rec->from_addr_number,
              call_rec->from_addr_suffix, call_rec->from_addr_apart, call_rec->from_addr_city,
              call_rec->from_addr_zone, call_rec->from_addr_cmnt, call_rec->passenger, call_rec->phone,
              call_rec->to_addr_street, call_rec->to_addr_number, call_rec->to_addr_suffix,
              call_rec->to_addr_apart, call_rec->to_addr_city, call_rec->to_addr_zone, call_rec->to_addr_cmnt,
              call_rec->car_number, call_rec->car_attrib, call_rec->driver_id, call_rec->driver_attrib,
              call_rec->creator, call_rec->creation_date, call_rec->creation_time, call_rec->due_date,
              call_rec->due_time, call_rec->pickup_time, call_rec->close_time, call_rec->call_comment,
              call_rec->call_status, call_rec->ftj_number, call_rec->offer_type,
              call_rec->detail_type, call_rec->msg_nbr);
      
    }
}

LogMsg_Dispatch_Call( direction, data_pkg )
  char *direction;
char *data_pkg;
{
  PI_CALL_REC *call_rec;
  int    cl_nbr;
  short  status;
  int len;
  char *position;
  

  if ( !strcmp(direction, PI_TO_EXTP) )
    {
      position = (char *) data_pkg;
      len = 12;
      get_from_data_blk( PI_SHORT_TYPE, &position, (char *)&status, &len );
      position = (char *)(data_pkg + 2);;
      get_from_data_blk( PI_INT_TYPE, &position, (char *) &cl_nbr, &len );        

      sprintf(szLogMsg, "%s PI_DISPATCH_CALL <%d><%d>\n",
              direction, status, cl_nbr);
    }
  else
    {
      call_rec = (PI_CALL_REC *)data_pkg;
      sprintf(szLogMsg, "%s PI_DISPATCH_CALL <%c><%d><%d><%d><%d><%d><%d><%-5.5s><%-21.21s><%d><%c><%-6.6s><%-4.4s><%d><%-31.31s><%-21.21s><%-11.11s><%-21.21s><%d><%c><%-6.6s><%-4.4s><%d><%-31.31s><%d><%-8.8s><%-6.6s><%-8.8s><%d><%-7.7s><%-5.5s><%-7.7s><%-5.5s><%-5.5s><%-5.5s><%-65.65s><%d>\n",
              direction, call_rec->fleet, call_rec->call_number,
              call_rec->priority, call_rec->call_group, call_rec->call_position,
              call_rec->number_of_calls, call_rec->number_of_vehicles,
              call_rec->call_type, call_rec->from_addr_street, call_rec->from_addr_number,
              call_rec->from_addr_suffix, call_rec->from_addr_apart, call_rec->from_addr_city,
              call_rec->from_addr_zone, call_rec->from_addr_cmnt, call_rec->passenger, call_rec->phone,
              call_rec->to_addr_street, call_rec->to_addr_number, call_rec->to_addr_suffix,
              call_rec->to_addr_apart, call_rec->to_addr_city, call_rec->to_addr_zone, call_rec->to_addr_cmnt,
              call_rec->car_number, call_rec->car_attrib, call_rec->driver_id, call_rec->driver_attrib,
              call_rec->creator, call_rec->creation_date, call_rec->creation_time, call_rec->due_date,
              call_rec->due_time, call_rec->pickup_time, call_rec->close_time, call_rec->call_comment,
              call_rec->call_status);      
    }  
}

LogMsg_Dispatch_Account_Call( direction, data_pkg )
  char *direction;
  char *data_pkg;
{
  short status;
  int cl_nbr;
  int len;
  char *position;
  ACCT_DATA_REC *account_order;

  if ( !strcmp(direction, PI_TO_EXTP) )
    {
      position = (char *) data_pkg;
      len = 12;
      get_from_data_blk( PI_SHORT_TYPE, &position, (char *)&status, &len );
      position = (char *)(data_pkg + 2);;
      get_from_data_blk( PI_INT_TYPE, &position, (char *) &cl_nbr, &len );        

      sprintf(szLogMsg, "%s PI_DISPATCH_ACCOUNT_CALL <%d><%d>\n",
              direction, status, cl_nbr);
    }
  else
    {
      account_order = (ACCT_DATA_REC *)(data_pkg);
      sprintf(szLogMsg, "%s PI_DISPATCH_ACCOUNT_CALL <%-16.16s><%c><%-8.8s><%-2.2s><%c><%-5.5s><%-21.21s><%-6.6s><%c><%-6.6s><%-4.4s><%-3.3s><%-31.31s><%-21.21s><%-11.11s><%-21.21s><%-6.6s><%c><%-6.6s><%-4.4s><%-3.3s><%-31.31s><%-6.6s><%-32.32s><%-6.6s><%-32.32s><%-4.4s><%-7.7s><%-5.5s><%-7.7s><%-5.5s><%-5.5s><%-5.5s><%-65.65s><%-8.8s><%-8.8s>\n",
              direction,
              account_order->acct_nbr,account_order->fleet,account_order->call_number,account_order->priority,
              account_order->number_of_calls,account_order->call_type,account_order->from_addr_street,
              account_order->from_addr_number,account_order->from_addr_suffix,account_order->from_addr_apart,
              account_order->from_addr_city,account_order->from_addr_zone,account_order->from_addr_cmnt,
              account_order->passenger,account_order->phone,account_order->to_addr_street,
              account_order->to_addr_number,account_order->to_addr_suffix,account_order->to_addr_apart,
              account_order->to_addr_city,account_order->to_addr_zone,account_order->to_addr_cmnt,
              account_order->car_number,account_order->car_attrib,account_order->driver_id,
              account_order->driver_attrib,account_order->creator,account_order->creation_date,
              account_order->creation_time,account_order->due_date,account_order->due_time,account_order->pickup_time,
              account_order->close_time,account_order->call_comment,account_order->gpsx,account_order->gpsy);
    }
}

LogMsg_Unknown_Type( direction, data_pkg )
  char *direction;
void *data_pkg;
{

}
