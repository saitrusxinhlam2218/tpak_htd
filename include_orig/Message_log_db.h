/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Message_log_db.h,v $
*  @(#)  $Revision: 1.5 $
*  @(#)  $Date: 2003/01/28 16:38:43 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Message_log_db.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: Message_log_db.h,v $	$Revision: 1.5 $"

#ifndef _MSG_LOG_DB_H
#define _MSG_LOG_DB_H

#include <isam.h>
#include "taxi_defs.h"

typedef struct   cisam_ml  {
       int     nbr;                          /* A number assigned to this message by the system. Could be used for primary key*/
       int    from;                         /* 1-99999.  User ID who sent the message.               */
       int     date_time_sent;               /* Date and time this message was sent.                  */
       short  supervisor_id;                /* 1-9999.  User ID who approved or denied this message.    */
       char   date_sent[10];                /* Date in display format.        */
       char   time_sent[7];                 /* Time in display format.        */
       char   supervisor_saw;                /* Whether the supervisor "A"pproved, "D"enied, "S"aw "I"n progress this message */
       char   repeat;                        /* "Y"/"N".  Whether this message is a repeat message*/
       char   fleet;                         /* Fleet associated with this message. Could be only a specific fleet or "" for all*/
       char   to_type;                       /* Type of user this message was sent to.  Choices: */
                                             /*     * - all users and drivers. */
                                             /*     S - Supervisor position. */
                                             /*     C - Call Taker position. */
                                             /*     M - Manager position. */
                                             /*     T - Specific vehicle. */
                                             /*     D - Specific driver. */
                                             /*     Z - All vehicles in a zone */
                                             /*     U - Specific in-house user. */
       char   to_selection[7];               /* Who the message was sent to (further clarifies to_type).Choices: */
                                             /*    *      - All users within the specified type. */
                                             /*    Number - position # with "S", "C" or "M"; user # with blank type; vehicle # */
                                             /*             with "T"; driver # with "D"; fleet id with "F"; zone # with "Z". */
					     /*             Entry may terminate with a "*" to imply wild carding.    */
       char   msg_line1[101];                /* First line of message.    */
       char   msg_line2[101];                /* Second line of message.   */
       char    veh_attr_set;
       char    veh_attr[ ATTR_MAX + 1];
       char    drv_attr_set;
  char    drv_attr[ ATTR_MAX + 1];
  char msgtext[512 + 1];
    } MSGLOG_REC;


#ifdef KEY_SRC
struct keydesc ml_key1= {
         ISNODUPS,
         1,
         {0,LONGSIZE,LONGTYPE},
};

/* date_time_sent */
struct keydesc ml_key2= {
         ISDUPS,
         1,
         {8,LONGSIZE,LONGTYPE},
};
#else
extern struct keydesc ml_key1;
extern struct keydesc ml_key2;
#endif

#endif
