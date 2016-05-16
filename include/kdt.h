/****************************************************************************
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  kdt.h; Rel: 6.9.0.0; Get date: 10/29/92 at 13:57:44
* @(#)  Last delta: 12/11/90 at 13:38:36
* @(#)  SCCS File: /taxi/sccs/s.kdt.h
*                                                                           *
* Copyright (c) 1990 - MDI Seattle                                          *
*                                                                           *
*****************************************************************************/
/* based on R3 Version 5.11 */
#include "STD.h"
#include "acc.h"
#include "msg.h"		/* From ACC code */
#include "udp.h"
/* begin mmp new MRB */
#include "msg_defs.h" /* PROMPTS/ERRORS and STATUS */
#include "f_msg_defs.h"  /* formatted message fragments */
#define GROUP_MID	"00F0"
#define PROMPT      '3'
#define DISPLAY_BUF '2'
#define STATUS      '1'
#define DISPLAY     '0'
#define CR           "%R"
#define INVISIBLE    "%I"
#define SCREEN_COORD "%C"
struct drvr_info {                      /* driver info from driver database */
        struct drv_attributes drv_attr; /* driver attribute */
        long susp_fr_time;              /* suspension from time */
        long susp_to_time;              /* suspension to time */
        long susp_d_fr_time;              /* suspension from time */
        long susp_d_to_time;              /* suspension to time */
        long susp_x_fr_time;              /* suspension from time */
        long susp_x_to_time;              /* suspension to time */
        char susp_msg[65];             /* suspension msg */
        struct seals_flags drv_seals;   /* driver seals */
	char driver_class;		/* driver class, rookie , regular */
};
#define  MAXMTYPE               1023            /* same as phase I */
#define  MAX_MSG_READ           150              /* max. number of msgs read from msg queue per kdt_interface() call */
#ifdef	TRUE 
#undef	TRUE
#endif
#ifdef	FALSE
#undef	FALSE
#endif
#define  TRUE                   1
#define  FALSE                  0
#define  DISP_SIZE              256             /* info display size */
#define  DRV_INV_ID             0               /* invalid driver ID, for chk_driver()*/
#define  DRV_SGND_ON            3               /* driver already signed on */
#define SELF_ONLY               '1'
#define MTYPEREPT		256		/* Search report (robby) */
#define MTYPEASGN		MTYPEREPT+1	/* Call assignment */
#define MTYPEEMER		MTYPEASGN+1	/* Emergency acknowledge */
#define MTYPETEXT		MTYPEEMER+1	/* General status text */
#define MTYPESTAL		MTYPETEXT+1	/* Status labels */
#define MTYPETXTL		MTYPESTAL+1	/* Text labels */
#define MTYPEMSGL		MTYPETXTL+1	/* Message labels */
#define MTYPEDATE		MTYPEMSGL+1	/* Download date */
#define MTYPETIME		MTYPEDATE+1	/* Download time */
#define MTYPEINFO		MTYPETIME+1	/* Information report */
#define MTYPEINQU		MTYPEINFO+1	/* Inquiry report */
#define	MTYPESREG		MTYPEINQU+1	/* Supervisor registration */
#define	MTYPEPOST		MTYPESREG+1	/* Post */
/* call_offer_info options */
#define ZONE_ONLY              '0'
#define ZONE_DESC              '1'
#define ZONE_PMAP	       '2'
#define ZONE_BLOCK             '3'
#define ADDRESS                '4'
#define OFFER5                 '5'
#define OFFER6                 '6'
#define OFFER7                 '7'
#define OFFER8                 '8'
#define OFFER9                 '9'
/* call_assignment_info options */
#define H_FORMAT               '0'            /* HELSINKI  */
#define S_FORMAT               '1'            /* Stockholm */
#define DETAIL2                '2'
#define DETAIL3                '3'
#define DETAIL4                '4'
#define DETAIL5                '5'
#define DETAIL6                '6'
#define DETAIL7                '7'
#define DETAIL8                '8'
#define DETAIL9                '9'
#define DETAIL10               'A'
#define KOGE_FORMAT            'B'
#define TH_FORMAT              'C'
#define POST                    0
#define E_POST                  1
#define CPOST                   3
#define NEVER                   '0'
#define ONES_SELF		'1'
#define STANDS                  '2'
#define ALL_ZONES               '3'
/* Define Message Parameters types */
#define	P1		0
#define	P2		1
#define	P3		2
#define	P4		3
#define	P5		4
/* Define TYPES */
#define	MT_FORM_MSG 	'2'
#define	MT_FUNC_MSG	'3'
#define MT_MSG_STATUS	'4'
/* Define FORM Types - P1 */
#define FT_LOCAL_FORM	'1'
#define FT_FILL_FORM	'2'
#define FT_AUX_INP	'3'
#define FT_ZONECHECK_FORM '4'
#define FT_NEW_LOCAL_FORM '5'
/* Define FUNC Types - P1 */
#define FCT_EMERG	'0'
#define FCT_EXT_INP	'1'
#define FCT_HIA         '2' 
#define FCT_CAN_MSG	'3'
/* Define MSG STATUS - P1 */
#define MS_MSG_ACK	'0'
#define MS_SYNTAX	'1'
#define MS_RES_NA	'2'
#define MS_NOT_SUP	'3'
#define MS_IL_MSG	'4'
#define MS_SP1		'5'
#define MS_SP2		'6'
#define	MS_SP3		'7'
#define	MS_DA_SP	'8'
#define	MS_DA_SYNTAX	'9'
#define	MS_DA_RES_NA	'A'
#define MS_DA_NOT_SUP	'B'
#define	MS_DB_SP	'C'
#define	MS_DB_SYNTAX	'D'
#define	MS_DB_RES_NA	'E'
#define MS_DB_NOT_SUP	'F'
#define MS_MSG_NAK	'G'
#define MS_MSG_SENT	'H'
#define MS_MSG_REJECT	'I'
#define MS_IL_MSG_TYPE  'J'
#define MS_IL_TRANS	'K'
/* Define LOCAL FORM MSG - P2 */
#define	LF_S_IN		'0'
#define	LF_POST		'1'
#define	LF_EPOST	'2'
#define	LF_MSG		'3'
#define LF_BID		'4'
#define	LF_CPOST	'5'
#define	LF_UNPOST	'6'
#define	LF_ETA		'7'
#define	LF_INQ		'8'
#define LF_NOSHOW	'9'
#define LF_MON          'E'
#define LF_ACPT         'F'
/* New Local Forms     */
#define LF_STAND_NOTIFY '5'
/* Define Aux Input Form Msg - P2 */
#define AUXF_ITM        '0'
#define AUXF_GPS        '1'
/* V02                     */
#define LF_ACCT1        'A'
#define LF_TAXI_ACCT    'B'
#define LF_ACCT2        'C'
/* V03                     */
#define LF_ACCT_V03     'B'
/* Define CAN MESSAGES - P2 */
#define CM_ON_DATA      '0'
#define CM_ACPT		'1'
#define CM_RJCT		'2'
#define	CM_FLAG		'3'
#define	CM_VOICE	'4'
#define	CM_S_OFF	'5'
#define	CM_INFO		'6'
#define	CM_BREAK	'7'
#define	CM_PCKUP	'8'
#define	CM_CLOSE	'9'
#define	CM_COUT_V02	'B'
#define	CM_COUT_V03	'A'
#define CM_POWERUP      'E' /* Used for MDT powerup-detection */
#define	CM_MFAULT	'F'
#define CM_ROTI         'C'
/* Define External Input Messages - P2 */
#define EI_M_ON		'1'
#define EI_M_OFF	'2'
/* Define ALARM TYPES */
#define	AT_NCP_ALARM	0
#define	NO_RESP		1
#define	NO_ALARM	2
/* terminal types */
#define MDT_ITM           1 /* ITM equiped 7031 V03 */
#define MDT_V03           2 /* 7031 V03 no ITM */
#define MDT_V02           3 /* standard 7031 V02 the default */
#define MDT_420           4
#define MDT_420_TMAT      5 /* MDT420 with TaxiMat taximeter */
#define MDT_420_ITM       6 /* MDT420 with Semel taximeter   */
#define MDT_V03_TMAT      7 /* 7031 V03 with TaxiMat         */
#define MDT_V03_ITM_VERS2 8 /* 7031 V03 with Stockholm MDT S/W Changes installed */
#define MDT_OPUS_2K       9 // Zenitel Opus 2000 MDT
#define COMPLETE_ADDRESS 1 /* for making addresses */
#define STREET_ONLY      2
#define STAR2000_MDT    'S'
