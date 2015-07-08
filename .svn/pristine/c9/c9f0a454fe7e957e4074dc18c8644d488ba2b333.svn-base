/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Packet_ID_private.h,v $
*  @(#)  $Revision: 1.4 $
*  @(#)  $Date: 1998/10/23 09:37:48 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Packet_ID_private.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ifndef _PACKET_ID_PRIVATE_H
#define _PACKET_ID_PRIVATE_H
typedef struct pkt_id_struct
{
  char              type;
  int               pkt_nbr;
  unsigned int      pkt_value;
  short             fleet;
  short             veh_nbr;
  time_t            pkt_time;
} PKT_ID_STRUCT;
#define PKT_TIMEOUT   600
typedef struct pkt_id_array_struct
{
   char  type;
   int   nbr;
   int   veh_nbr;
   time_t pkt_time;
}     PKT_ID_ARRAY, *PKT_ID_ARRAY_PTR;
#endif
