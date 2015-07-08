/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Rptmsg_log_db.h,v $
*  @(#)  $Revision: 1.5 $
*  @(#)  $Date: 2003/01/28 16:38:43 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Rptmsg_log_db.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: Rptmsg_log_db.h,v $	$Revision: 1.5 $"

#ifndef _RPTMSG_LOG_DB_H
#define _RPTMSG_LOG_DB_H

#include <isam.h>
#include "taxi_defs.h"

typedef struct	cisam_rm {
       	int     msg_nbr;		/* A number assigned to this message by the system. Could be used for primary key */
       	short  	created_by;             /* User ID who created this repeat message. */
       	int    	created_dt_tm;          /* Date & time created. */
       	char   	repeat;                 /* "Y"/"N".  Whether this message is a repeat message. */
       	char   	fleet;                  /* Fleet associated with this message. Could be only specific fleet or "*" for all. */
       	char   	to_type;                /* Type of user this message was sent to.  Refer to Message Log File. */
       	char   	to_selection[7];        /* who the message was sent to (further clarifies to_type). Refer to Msg Log File. */
       	short  	how_often;              /* Minutes.  How often this message supposed to be sent. */
       	int    	until_dt_tm;            /* Message expiration date & time. */
       	char   	line_1[101];            /* First line of message. */
       	char   	line_2[101];            /* Second line of message. */
	char	cr_date[9];		/* ASCI create date */
	char	cr_time[7];		/* ASCI create time */
	char	until_date[9];		/* ASCI until date */
	char	until_time[7];		/* ASCI until time */
	char    veh_attr_set;
	char    veh_attr[ ATTR_MAX + 1];
	char    drv_attr_set;
  char    drv_attr[ ATTR_MAX + 1];
  char msgtext[512 + 1];  
}REPEAT_MSG_REC ;

#ifdef KEY_SRC
struct keydesc rm_key1= {
         ISNODUPS,
         1,
         {0,LONGSIZE,LONGTYPE},
};

/* created_by */
struct keydesc rm_key2= {
         ISDUPS,
         1,
         {4,INTSIZE,INTTYPE},
};

#else
extern struct keydesc rm_key1;
extern struct keydesc rm_key2;
#endif

#endif
