/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Exception_private.h,v $
*  @(#)  $Revision: 1.12 $
*  @(#)  $Date: 2004/02/02 18:45:20 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Exception_private.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ifndef _EXCEPT_PRIVATE_H
#define _EXCEPT_PRIVATE_H
#ifndef __alpha
#include <sys/time.h>
#endif
/* Based on R3 version 5.2 */
#define EXCEPT_MAX 3000		/* maximum number of exceptions */
#define EX_PRI_MAX 65		/* maximum number of priorities for an exception */
#define EXCEPT_DESC_MAX 47	/* maximum number of exception descriptions */
				/* 15 exceptions added from 33 to 47, for SamPlan */
#define EXCEPT_BYTES_MAX 4	/* number of exception bytes */
#define EXCEPT_EMPTY_NODES      EXCEPT_MAX / 5
#define SP_EXCEPT_START 33
#define LOCK_REMOVE 	0		/* lock the exception structure, remove an exception, unlock the exception structure */
#define LOCK_ON 	1		/* lock the exception structure */
#define LOCK_OFF	2		/* unlock the exception structure */
#define LOCK_FREE 	3		/* lock the exception structure, unset the inuse flag, unlock the exception structure */
/* resolution codes */
#define R_MAN_ZONE  	1	/* Zone assigned */
#define R_MAN_VEH   	2	/*  Cab assigned */
#define R_APPROVE 	3	/* approved */
#define R_DISSAPROVE 	4	/* disapproved */
#define R_DISAPPROVE    4
#define R_RESEND	5	/*  info */
#define R_DISPATCH	6	/*  call */
#define R_MULT		7	/* call regrouping */
#define R_NO_ACTION 	8	/* Exception resolved no action required*/
#define R_CANCEL	9	/* cancel the call */
#define R_SUP_HOLD	10	/* supervisor hold */
#define R_CALLBACK	11	/* callback */
#define R_ENTERED	12	/* call entered */
#define	R_UPDATED	13	/* call updated */
#define R_CAN_MULT	14 	/* cancel mult call */
#define R_M_MAN_VEH   	15	/* Cab assigned  on mult call */
#define R_MANUAL_RATE   16      /* For MANUAL_RATE history events */
#define R_NO_TAXIS_AVAIL 17     /* No taxis available history events */
#define R_DISPATCH_ILINK 18     /* Redispatch a trip transferred via Interlink but resulted in a TIMEOUT from remote system */
#define R_EMERG         19      /* generate an emergency exception */
#define EXCEPT_TEXT_MAX		40
/* Exception Types */
#define EMERGENCY 1
#define CALL_CANCEL 2
#define UNZONED 3
#define CALL_W_MSG 4
#define CALL_NO_MSG 5
#define EXTRA_TAXIS 6
#define CALLBACK 7	
#define CALL_TIMEOUT 8	
#define REJECT 9	
#define LATE_METER 10	
#define FLAG_REQ 11	
#define NO_SHOW 12	
#define CALLOUT 13	
#define REQ_TO_TLK 14
#define ETA 15
#define MESSAGE 16
#define BID 17		
#define DRV_MSG 18	
#define WAKE_UP	19	
#define SHORT_METER 20	
#define HOL_TM_CALL 21	
#define SYS_ERR	22	
#define SUP_HOLD 23  	
#define MANUAL 24	
#define PERSONAL 25	
#define MFAULT 26	
#define MULT	27	
#define MISSING_RECEIPT_RETRY_LIMIT 28
#define ITM_INVOICE_NAK  29
#define ATTRIBUTE_HOLD   30
#define NS_SHORT         31
/* SamPlan user defined exceptions */
#define SP_EXCPT_01  33
#define SP_EXCPT_02  34
#define SP_EXCPT_03  35
#define SP_EXCPT_04  36
#define SP_EXCPT_05  37
#define SP_EXCPT_06  38
#define SP_EXCPT_07  39
#define SP_EXCPT_08  40
#define SP_EXCPT_09  41
#define SP_EXCPT_10  42
#define SP_EXCPT_11  43
#define SP_EXCPT_12  44
#define SP_EXCPT_13  45
#define SP_EXCPT_14  46
#define SP_EXCPT_15  47
#define TIME_CALL    48 
#define SPECIAL      49
#define COMMENT_EXC  50
#define NAK_CALL     51
#define ACK_CALL     52
#define ITM_NOSHOW   53
#define HELD_EPOST   54
#define RELEASE_EPOST 55
/* System error exception message type (for mk_except) */
#define BISYNC_ERR     		200    /* Problem in comm. link to NCP */
#define NCP_KISS_ERR   		210    /* Problem NCP module and KISS interface */
#define ASSIGN_ERR     		220    /* Can't send call assignment */
#define REG_DEREG_ERR  		230    /* Can't Reg./Dereg. a vehicle */
#define BID_LST_FULL_ERR   	240    /* the bid list is full */
#define NCP_ALARM      		250    /* NCP ALARM !! */
#define INACT_NCP_ALARM    	260    /* Unable to Receive NCP ALARM */
#define DISK_ALMOST_FULL        270    /* One of the taxi file systems is almost full */
/* ENH0001 */
#define AS_DOWN                 280    /* The credit card application server is down */
/* ENH0002 */
#define NO_ACK_MDT_1            290    /* The MDT did not ACK call message for call 1 */
#define NO_ACK_PRT_1            300    /* The ITM-printer did not ACK call message for call 1*/
#define NO_ACK_ITM_1            310    /* The ITM did not ACK call message for call 1*/
#define NO_ACK_MDT_2            320    /* The MDT did not ACK call message for call 2 */
#define NO_ACK_PRT_2            330    /* The ITM-printer did not ACK call message for call 2*/
#define NO_ACK_ITM_2            340    /* The ITM did not ACK call message for call 2*/
#define NO_ACK_MDT_3            350    /* The MDT did not ACK call message for call 3 */
#define NO_ACK_PRT_3            360    /* The ITM-printer did not ACK call message for call 3*/
#define NO_ACK_ITM_3            370    /* The ITM did not ACK call message for call 3*/
#define API_CHILD_DEATH         380    /* An API client has died */
#define API_NEW_CLIENT          390    /* A new API client has been initiated */
#define CALL_NODE_ERR  		400    /* Call nodes exhausted */
/* The number of system exception types */
#define SYS_ERR_MAX             20
/* Exception priority down-grade for regular calls. */
#define	EX_PRI_DOWNGRADE	20
/* Highest/lowest priority definitions. */
#define	EX_PRI_HIGHEST	1
#define	EX_PRI_LOWEST	(EX_PRI_MAX - 2)
typedef struct _EXCEPT_EMERG_ {
  char  fleet;
  short veh_nbr;
  char  active;
  short radius;
} EXCEPT_EMERG;
typedef struct excepts {
	long nbr; 		/* exception number */
	short priority;		/* priority of exception */
	time_t time_occur; 	/* time the exception ocurred */
	int type;		/* type of exception */
	char status;		/* exception status; WHAT IS THIS ANYWAY???  */
	short zone_nbr;		/* zone number of exception */
	short super_zone;	/* super zone number of exception */
	struct fleets *fl_ptr;  /* pointer to the fleet the exception is in */
	struct excepts *next; 	/* next slot in the exception list */
	struct excepts *prev; 	/* previous slot in the exception list */
	struct calls *call_ptr; /* pointer to the call in the call structure */
	long call_nbr;		/* call number for call */
	struct veh_driv *veh_ptr;/* pointer to the vehicle in the vehicle structure */
	int veh_nbr;		/* vehicle number of taxi with except */
	char inuse;    		/* flag to show if an exception is to be 
				 taken care of */
	long info;		/* miscellaneous information */
#ifdef NAMERICA
	int  radio_channel;     /* Radio channel that terminal is using */
#endif
  EXCEPT_EMERG emerg_bcast;
} EXCEPT; 
typedef struct excpt_index {
	long	nbr;			/* exception number */
	struct excepts *except_ptr; 	/* pointer to an exception record */
} EXCEPT_INDEX;
struct e_intervals {
	struct excepts *first;		/* first exception at an interval */
	struct excepts *last;		/* last exception at an interval */
	int    prev;			/* next used interval */
	int    next;			/* previous used interval */
};
typedef struct excpt_desc {
	short	group_nbr;		/* Exception summary group to which this exception belongs */
	char	short_desc[9];
} EXCEPT_DESC;
/** Used for passing resolution of exception over IPC **/
typedef struct excpt_rec {
       	long    u_prcs_id;      		/*Users process ID*/
       	char    rec_type;       		/*Record type = "E"xception.*/
       	struct excepts   *except_ptr;    	/*Exception pointer.*/
	long 	except_nbr;			/* Exception number */
       	short   resolve_code;   		/* Resolution code */
	long	call_nbr;			/* Call Number */ 
       	long    grouped_with;   		/*Call number of the first call in the group of calls to be dispatched */
						/* together to a single cab*/
       	short   veh_nbr;        		/*Assigned vehicle number.  Only filled in if <MAN_VEH> was pressed*/
       	short   zone_nbr;       		/*Assigned zone number.  Only filled in if <MAN_ZONE> was pressed*/
       	short   old_zone_nbr;       		/*when updating the zone of a call the old value is passed here*/
       	long   old_due_time;       		/*when updating the time and zone number on same update old time goes here*/
} EXCEPT_REC;
typedef struct excpt_rec    EXCEPTION_SHUTTLE;
typedef struct excepts      EXCEPTION;
typedef struct veh_driv     VEHICLE_T;
#endif
