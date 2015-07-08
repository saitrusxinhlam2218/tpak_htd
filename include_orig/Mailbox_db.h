/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Mailbox_db.h,v $
*  @(#)  $Revision: 1.3 $
*  @(#)  $Date: 1998/07/09 17:01:46 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Mailbox_db.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: Mailbox_db.h,v $	$Revision: 1.3 $"

#ifndef _MAILBOX_DB_H
#define _MAILBOX_DB_H

#include <isam.h>

typedef struct    cisam_mb  {
        int  address;	                 /* 1-99999. Number of the address (can go up to 5 digits for drivers */
        int    msg_nbr;                  /* A number assigned to this message by the system.        */
        short  from;                     /* 1-9999.  User ID who sent the message.                  */
        int    date_time_sent;           /* Date and time this message was sent.                     */
        char   msg_line1[101];           /* First line of message.           */
        char   msg_line2[101];           /* Second line of message.          */
        char   status;                   /* Whether this message is "N"ew or is it "O"ld.               */
	char   fleet;			 /* Fleet ID							*/
        char   to_type;                  /* Type of user this message was sent to.  Choices: */
                                             /*     * - all users and drivers. */
                                             /*     S - Supervisor position. */
                                             /*     C - Call Taker position. */
                                             /*     M - Manager position. */
                                             /*     T - Specific vehicle. */
                                             /*     D - Specific driver. */
                                             /*     Z - All vehicles in a zone */
                                             /*     U - Specific in-house user. */
        char   to_selection[7];          /* Who the message was sent to (further clarifies to_type).Choices: */
                                             /*    *      - All users within the specified type. */
                                             /*    Number - position # with "S", "C" or "M"; user # with blank type; vehicle # */
                                             /*             with "T"; driver # with "D"; fleet id with "F"; zone # with "Z". */
					     /*             Entry may terminate with a "*" to imply wild carding.    */
        char   date_sent[10];                /* Date in display format.        */
        char   time_sent[7];                 /* Time in display format.        */
	long   msg_type;		 /* Type of the original command sent to NCP	*/
  } MAILBOX_REC;

#ifdef KEY_SRC
struct keydesc mb_key1= {
         ISDUPS,
         1,
         {0,LONGSIZE,LONGTYPE},
};

/* Composite of (address,status,date_time_sent) */
struct keydesc mb_key2= {
         ISDUPS,
         3,
				 { {0,LONGSIZE,LONGTYPE},
					 {214,CHARSIZE,CHARTYPE},
					 {10,LONGSIZE,LONGTYPE} }
};
/* msg_nbr system assigned for redundancy only */
struct keydesc mb_key3= {
         ISNODUPS,
         1,
         {4,LONGSIZE,LONGTYPE},
};
#else
extern struct keydesc mb_key1;
extern struct keydesc mb_key2;
extern struct keydesc mb_key3;
#endif

#endif
