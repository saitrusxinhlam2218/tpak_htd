/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: TimeCalls_public.h,v $
*  @(#)  $Revision: 1.4 $
*  @(#)  $Date: 1998/10/23 09:37:50 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/TimeCalls_public.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: TimeCalls_public.h,v $	$Revision: 1.4 $"

#ifndef _TIMECALLS_PUBLIC_H
#define _TIMECALLS_PUBLIC_H

#define TC_PICKUP_LEAD_TIME            0
#define TC_DUE_TIME                    1
#define TC_CALL_CLASS                  2
#define TC_ZONE                        3
#define TC_CALL_NBR                    4
#define TC_FLEET                       5
#define TC_LEAD_TIME                   6

HNDL        TC_get_value(TC_HNDL, int);
RET_STATUS  TC_set_value(TC_HNDL, int, HNDL);
RET_STATUS  TC_send_status();
RET_STATUS  TC_check( TC_HNDL, ZONE_HNDL, FLEET_HNDL );
RET_STATUS  TC_to_Call( TC_HNDL, ZONE_HNDL, FLEET_HNDL, CALL_IPC_REC * );
#endif
