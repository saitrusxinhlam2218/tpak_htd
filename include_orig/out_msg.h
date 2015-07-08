/****************************************************************************
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  out_msg.h; Rel: 10.1.2.1; Get date: 4/20/95 at 11:48:29
* @(#)  Last delta: 12/2/94 at 18:20:14
* @(#)  SCCS File: /taxi/sccs/s.out_msg.h
*                                                                           *
* Copyright (c) 1990 - MDI Seattle                                          *
*                                                                           *
*****************************************************************************/
#ident "@(#) head:out_msg.h	10.1.2.1"

/* file out_msg.h */
/* outbound text message information */
/* started 24 Jan 90 */
/* last update 16 Mar  90 */
/* M. R. Bradley */

#ifndef FALSE
#define FALSE 0
#define TRUE 1
#endif

#include "msg_defs.h"

#define MAX_MSG_LENGTH 17	/* 1 + status length */

#ifndef TEST_ONLY
#include "acc.h"
#else
#define P_FLD_SIZE 5
#endif
#define NEEDED 1

struct m_info
{
   char  type;			/* MMP header type field */
   char  p_fields[P_FLD_SIZE];	/* MMP header control parameters */
   char *txt_ptr;		/* ptr to msg text */
   short is_canned;		/* canned message ? */
};

struct m_info msg_info[MAX_MSG_ID + 1] = {
#include "msg_info.h"
};
