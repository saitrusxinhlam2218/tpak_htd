 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  net_interf.h; Rel: 10.1.2.1; Get date: 4/20/95 at 11:48:25
 * @(#)  Last delta: 12/2/94 at 18:20:04
 * @(#)  SCCS File: /taxi/sccs/s.net_interf.h
 *                                                                           *
 *****************************************************************************/
#define	AF_INET		2		/* sys/socket.h */
#define	MAX_TCP_DATA	1460
#define  PUT_ADDR_FAMLY_TYPE(buf) {*(short *)buf = AF_INET;}
                                      /* put socket/internet address family type
                                         in the address buffer (buf).
                                         address family type is the
                                         first 2 bytes of 8-byte transport
                                         interface address.
                                         assuming "short" is 2-byte long.
                                         buf is the address buffer */
#define  PUT_PORT_ADDR(buf, port_id)  {*(ushort *)buf = port_id;}
                                      /* put port address (port_id) in the
                                         address bffer (buf).
                                         port address is the second 2 bytes
                                         of 8-byte transport interface address.
                                         assuming "short" is 2-byte long */
#define  TRANSP_ADDR_LEN_INT   SCK_ADDR_SIZE/sizeof(unsigned int)
                                      /* transport layer interf addr len (int)*/
#define  CONN_INDICATIONS  1          /* connection indications allowed by
                                         server (mirror) */
#define  MIRR_PORT_ADDR   0x0410      /* server (mirror) known port ID */
#define  MONT_PORT_ADDR   0x0412      /* monitor (rdnmon) known port ID */
extern unsigned int inet_addr;        /* internet addr (hex)
                                         init'ed in the net_local_mgmt */
extern short addr_size;               /* transport layer interface address size
                                         (in bytes)
                                         init'ed in the net_local_mgmt */
extern int t_errno;                   /* TI system routines' error no. */
                                      /* this is defined in the network
                                         library */
#define  PROTO_STRM   "/dev/tcpip"    /* protocol stream: TCP/IP */ 
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
#define SYS_ERR     "System error"
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
#define FAILED      -1
#define SUCCESS     0
#define RMT_INET_ADDR_FILE 	"/user/mads2/redncy/remote"		 /* remote host internet address */
#define INET_ADDR_FILE		"/user/mads2/redncy/host"		 /* host internet address */
