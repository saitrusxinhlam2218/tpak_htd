 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  redncy.h; Rel: 10.1.2.1; Get date: 4/20/95 at 11:48:47
 * @(#)  Last delta: 12/2/94 at 18:36:49
 * @(#)  SCCS File: /taxi/sccs/s.redncy.h
 *                                                                           *
 * Copyright (c) 1990 - MDI Seattle                                          *
 *                                                                           *
 *****************************************************************************/
/*#include "net_interf.h"*/
#define	MAX_TCP_DATA	1460
#define  SND_DATA_SZ    MAX_TCP_DATA  /* max sending data size/packet */
#define  RCV_DATA_SZ    MAX_TCP_DATA  /* max rcv'ing data size/packet */
#define  DISCONNECT     -1            /* network disconnected */
#define  BUF_SZ         80            /* buffer size */
struct p_state {            /* state of the program */
   char state;            /* I = initial phase
                             O = after open device (stream)
                             B = after bind to listener transport endpoint
                             L = listening
                             A = after accept a connection request
                             S = sending data
                             P = pause (after finishing sending data)
                             C = after connect req to remote site
                             D = after bind to sender transport endpoint
                             R = receving data
                             F = just finish parsing one packet recv'ed
                           */ 
   char proc;              /* M = mirror proc      R = reflect proc T = rdnmon proc      Q = rdnchk proc */
};
/* define states */
#define INIT_STAT   'I'    /* initial phase */
#define OPEN_STRM1  'O'    /* open the stream (listener, receiver) */
#define OPEN_STRM2  'G'    /* open the stream (sender) */
#define BND_PORT    'B'    /* bound to transport endpoint */
#define LISTENING   'L'    /* listen to the port (server/mirror/rdnmon) */
#define ACPT_CONN   'A'    /* accept a conn request (server/mirror/rdnmon) */
#define SEND_DATA   'S'    /* sending data (server/mirror/rdnmon) */
#define PAUSE_STAT  'P'    /* pause after sending data (server/mirror/rdnmon) */
#define CONN_REQ    'C'    /* connect req to server (client/reflect/rdnchk) */
#define BND_PRT_SR  'D'    /* bound to transport endpoint (by sender)
                             (server/mirror/rdnmon) */
#define RECV_DATA   'R'    /* receving data (client/reflect/rdnchk) */
#define FINISH_PCKT 'F'    /* just finish parsing a packet recv'ed or
                              waiting to receive data (client/reflect/rdnchk) */
/* define bind request type for net_local_mgmt() */
#define MIRR_SRV    1      /* mirror (server) type */
#define REFL_CLT    2      /* reflect (client) type */
#define MONT_SRV    3      /* rdnmon (server) type */
#define CHCK_CLT    4      /* rdnchk (client) type */
#define SNDER_CONN  5      /* sender request (for connect) */
/* error messages for transport interface */
/*  this is for logging error msgs in a file */
#define BAD_ADDR    "Incorrect addr format"
#define BAD_OPT     "Incorrect option format"
#define PERM_WRONG  "Incorrect permissions"
#define ILL_F_ID    "Illegal transport fd"
#define NO_ADDR     "Couldn't allocate addr"
#define OUT_STATE   "Out of state sequence"
#define BAD_SEQ	    "Bad call sequnce number"
#ifndef SYS_ERR
#define SYS_ERR     "System error"
#endif
#define LOOK_DISC   "Disconnected"              /* TLOOK: only DISCONN here */
#define BAD_DATA    "Illegal amount of data"
#define BUF_OVFLW   "Buffer not large enough"
#define FLOW_CNTRL  "Flow control problem"
#define NO_DATA     "No data available"
#define NO_DISCON   "Disconn ind not found on q"
#define NO_UD_ERR   "Unitdata error not found"
#define BAD_FLAG    "Bad flags"
#define NO_ORD_REL  "No orderly release found on queue"
#define NO_SUPPORT  "Primitive not supported"
#define STAT_CHNG   "In process of changing state"    /* should NOT be an error
                in this context.  after t_bind successfully, state=TSTATECHNG */
#define _ERROR      0                  /* failure on return from unsiged func */
#define  MAX_TRANS_SIZE  RCV_DATA_SZ         /* max. transaction size */
#define  TRANS_HDR_SIZE  (4 * sizeof(int))
#define  TRANS_DATA_SIZE MAX_TRANS_SIZE-TRANS_HDR_SIZE
                                             /* size of struct transact header(exclude data field) */
#define  ACT_TYPE_SIZE  (sizeof(long) + sizeof(int))
                                             /* minimum size in order to get the action type from transact struct     */
                                             /* size of struct transact data field */
#define  LOCAL_DATA_SIZE TRANS_DATA_SIZE     /* size of buffer to store struct transact data field */
/* define process type and its name for get_STATE_str() */
#define  PROC_STR     'P'                    /* get proc name/string */
#define  STAT_STR     'S'                    /* get STATE string/name */
#define  MIRR_PROC    'M'                    /* mirror proc */
#define  REFL_PROC    'R'                    /* reflect proc */
#define  MONT_PROC    'T'                    /* rdnmon proc */
#define  CHCK_PROC    'Q'                    /* rdnchk proc */
#define  PROC_NM_SZ   10                     /* process name length */
#define  MIRROR       "MIRROR"               /* mirror process */
#define  REFLECT      "REFLECT"              /* reflect process */
#define  RDNMON       "RDNMON"               /* rdnmon (redundancy/network
                                                monitoring) process */
#define  RDNCHK       "RDNCHK"               /* rdnchk (redundancy/network
                                                checking) process */
/* define actions types (transaction) */
#define  STARTUP      100                    /* start up */
#define  DB_SYNC      200                    /* sync all data base */
#define  TIME_SYNC    300                    /* sync time */
#define  REC_UPDATE   UPDATE_REC             /* record update */
#define  REC_ADD      ADD_REC                /* record creation */
#define  REC_DELETE   DELETE                 /* record deletion */
                                             /* transaction file */
struct transact       /* transaction struct for mirror/reflection proc */
   {
   int size;                                /* size of the transaction */
   int action;                               /* action type */
   int file;                                 /* type of file */
   int rec_no;                               /* record no */
   char data[TRANS_DATA_SIZE];               /* actual data */
   };
extern char proc_name[];                     /* proc name string */
#ifdef	TRUE 
#undef	TRUE
#endif
#ifdef	FALSE
#undef	FALSE
#endif
#define TRUE    1
#define FALSE   0
/* file name string : for error reporting */
#define  MENU_FILE_STR    "Menu"
#define  ERR_FILE_STR     "Error-log"
#define  CALL_FILE_STR    "Call"
#define  CHIS_FILE_STR    "Call-Hist"
#define  DRVR_FILE_STR    "Driver"
#define  VEHL_FILE_STR    "Vehicle"
#define  ZONE_FILE_STR    "Zone"
#define  FLET_FILE_STR    "Fleet"
#define  ADDR_FILE_STR    "Addr"
#define  ACCT_FILE_STR    "Acct"
#define  ATTR_FILE_STR    "Attr"
#define  EMER_FILE_STR    "Emerg"
#define  EXCP_FILE_STR    "Excep"
#define  EXGP_FILE_STR    "Excep-Gp"
#define  INTS_FILE_STR    "Intersec"
#define  LOCL_FILE_STR    "City"
#define  HELP_FILE_STR    "Help"
#define  HLID_FILE_STR    "Holiday"
#define  PLNM_FILE_STR    "Place-Nm"
#define  MALB_FILE_STR    "Mail-Box"
#define  MESG_FILE_STR    "Message"
#define  RATE_FILE_STR    "Rate"
#define  RMSG_FILE_STR    "Rep-Msg"
#define  SEAL_FILE_STR    "Seal"
#define  STNM_FILE_STR    "Street-Nm"
#define  STTP_FILE_STR    "Street-Tp"
#define  SUBC_FILE_STR    "Subs-Call"
#define  SUBZ_FILE_STR    "Sub-Zone"
#define  SYCL_FILE_STR    "Sys-Cntrl"
#define  UNZN_FILE_STR    "Unzoned"
#define  USER_FILE_STR    "User"
#define  CMSG_FILE_STR    "Canned-msg"
#define  SDRV_FILE_STR    "Stat-drvr"
#define  SUSR_FILE_STR    "Stat-user"
#define  STAT_FILE_STR    "Stats"
#define  SHFT_FILE_STR    "Shift"
#define  SHFTH_FILE_STR   "Shift-Hist"
#define  SS_SCHED_FILE_STR   "SS Schedule"
#define  SS_SHIFT_FILE_STR   "SS Shift"
#define  REC_UPDATE_STR   "UPDATE"
#define  REC_ADD_STR      "ADD"
#define  REC_DELETE_STR   "DELETE"
#define  DB_SYNC_STR      "DB-SYNC"
#define  TIME_SYNC_STR    "TIME-SYNC"
#define IDX_INDEX    	1              	/* index for .idx c-isam file */
#define DAT_INDEX    	2              	/* index for .dat c-isam file */
#define FLAT_FILE	3		/* Identifies a flat file to sync */
#define  ONE_MEGA     1000000
#define  BLK_SIZE1    RCV_DATA_SZ * 4       /* ca. 5500 */
                          /* block size for writing to or copying a file */
#define  BLK_SIZE2    RCV_DATA_SZ * 30      /* ca. 50000 */
                          /* block size for writing to or copying a file */
#define CREDIT_FILE_UPDATE           -1
#define MISSING_RECEIPT_FILE_UPDATE  -2
#define RECEIPT_FILE_UPDATE          -3
