 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  async.h; Rel: 10.1.2.1; Get date: 4/20/95 at 11:44:19
 * @(#)  Last delta: 12/2/94 at 18:10:50
 * @(#)  SCCS File: /taxi/sccs/s.async.h
 *                                                                           *
 * Copyright (c) 1990 - MDI Seattle                                          *
 *                                                                           *
 *****************************************************************************/
#ident "@(#) head:async.h	10.1.2.1"

/* HEADER */
/******************************************************************************
**
** NAME:
**		async.h
**	
** DESCRIPTION:
**		Contains message format used in passing messages to ACC.
**	
** UPDATE HISTORY:
**	
**	J.Hoy Nov 6/89	Original.
**	
*******************************************************************************/

#ifndef async_h
#define async_h	"@(#)async.h	1.0		6Nov89	JH	RNG"

#include "mmp.h"
#include "hash.h"

#define NUL	0x00
#define SOH 0x01
#define ETX 0x03

#define FCH_LEN 1
#define SEQ_NO_SIZE 2		/* seq no field length */
#define PKT_LEN_SIZE 3		/* pkt field length */
#define ROUTE_ID_SIZE 	MID_SIZE	/* id field length */
#define PKT_ID_SIZE 2		/* tag field length */
#define P_SIZE 		P_FLD_SIZE	/* p field length */

typedef struct {
	int num_rx_msgs[NUM_BLKS];	/* break down of rx msgs by size */
        int num_rx_outseq;                        /* number of seuquence errors */
	int num_rx_msg_outseq;			/* number of messages rx out of sequence */
	int num_rx_bad;				/* number messages rx bad, ie missing etx */
	int num_hl1;
	int num_hl2;
	int num_hl3;
	int num_hl4;
	int num_hl5;
	int num_hl6;
	int num_hl7;
	int num_hl8;  
} RX_STATS;

typedef struct {
  int num_tx_timeouts;              /* number of write timeouts */
  int num_tx_msgs[NUM_BLKS];	/* break down of tx msgs by size */
  int num_hl[8];
} TX_STATS;

typedef struct {
  int  mid;
  char VehIP[32];
  time_t LastAccess;
} VehIPEntry;

typedef struct {
  time_t sentTime;
  char type;
  char VehIP[32];
  int  mid;
  char MMPText[5120];
  int  retry;
} PktIDEntry;

typedef struct {
	RX_STATS rx;
	TX_STATS tx;
	char	 linkup;
	char     trace;
	char     trace_mid[5];
  VehIPEntry     veh_table[500];
  PktIDEntry     pktid_table[257];
} ALC_STATS;



/* This the packet that will go/come from the ACC */

typedef struct {
	char soh;
	char seq_no[SEQ_NO_SIZE];		/* link sequence #, 0-255, ascii encoded */
	char pkt_length[PKT_LEN_SIZE];	/*packet body length,0-2096,ascii encoded*/ 
	char route_id[ROUTE_ID_SIZE];	/* == mid of mdt(24bits) */
	char pkt_id[PKT_ID_SIZE];		/* ==tag, (8 bits) */
	char delvry_prio;				/* delivery priority, normal or expedited*/
	char delvry_disp;				/* delivery disp, indicates whether an */
									/* "ack" is to be returned to the Host. */
	char multicast;					/* indicates selective or group tx */
	char persist;					/* indicates indirectly the retries */
	char type;					/* indicates MMP message type */
	char p_fields[P_SIZE];		/* indicates P fields for MMP */
	char text[MMP_TEXT_SIZE];
	char etx;
} LINK_PKT; 


#define MAX_BUF sizeof(LINK_PKT)


#endif
