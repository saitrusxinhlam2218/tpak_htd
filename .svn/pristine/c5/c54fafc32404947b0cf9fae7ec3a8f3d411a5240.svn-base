 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  msg.h; Rel: 10.1.2.1; Get date: 4/20/95 at 11:48:06
 * @(#)  Last delta: 12/2/94 at 18:07:39
 * @(#)  SCCS File: /taxi/sccs/s.msg.h
 *                                                                           *
 *****************************************************************************/
#ident "@(#) head:msg.h	10.1.2.1"

/* HEADER */
/******************************************************************************
**
** NAME:
**		msg.h
**	
** DESCRIPTION:
**		Contains message format used in putting/getting messages from
**		"message queues".
**	
** UPDATE HISTORY:
**	
**	J.Hoy Nov 6/89	Original.
**	
**		Copyright (c), 1989
**		All rights reserved
**
**	MOBILE DATA INTERNATIONAL, INC.
**		RICHMOND, BRITISH COLUMBIA
**		CANADA
**
*******************************************************************************/

#ifndef msg_h
#define msg_h	"@(#)msg.h	1.0		6Nov89	JH	RNG"

#include "async.h"

typedef struct {
#ifdef PIPES
        int  size;
#else
	LONGINT prio;			/* ipc message queue priority */
#endif
	CHAR route_id[ROUTE_ID_SIZE];	/* == mid of mdt(24bits) */
	CHAR pkt_id[PKT_ID_SIZE];		/* ==tag, (8 bits) */
	CHAR delvry_prio;				/* delivery priority, normal or expedited*/
	CHAR delvry_disp;				/* delivery disp, indicates whether an */
									/* "ack" is to be returned to the Host. */
	CHAR multicast;					/* indicates selective or group tx */
	CHAR persist;					/* indicates indirectly the retries */
	CHAR type;					/* indicates MMP message type */
	CHAR p_fields[P_SIZE];		/* indicates P fields for MMP */
	CHAR text[MMP_TEXT_SIZE];
} MSG_QUEUE_HDR; 

#define MSG_HDR_LEN (ROUTE_ID_SIZE + PKT_ID_SIZE + P_SIZE + 5)
#define MAX_MSG_SIZE (MSG_HDR_LEN + MMP_TEXT_SIZE)
#define CHECK_CHAR     '|'
#define CHECK_CHAR_STR "|"
#endif
