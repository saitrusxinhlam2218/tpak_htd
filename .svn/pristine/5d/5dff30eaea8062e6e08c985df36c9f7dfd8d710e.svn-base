/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  ccp_com.h; Rel: 10.1.0.0; Get date: 4/18/95 at 16:36:09		            *			
* @(#)  Last delta: 12/2/94 at 12:16:18						    *	
* @(#)  SCCS File: /taxi/sccs/s.ccp_com.h							    *
*                                                                           *
* Copyright (c) 1994 - TaxiCom ApS					    *
*                                                                           *
*****************************************************************************/

/* Communication pipes */
#define	CCP2TP_PIPE	"/usr/taxi/run/ccp2tp"
#define TP2CCP_PIPE	"/usr/taxi/run/tp2ccp"

/* itm message defines */
#define ITM_SUB_TYPE_LENGTH	2
#define NUM_ITM_CTRL_CHAR	6

/* Semel defines */
#define	MAX_SEMEL_FLDS		5		/* Maximum number of fields in Semel message */
#define INCOMPLETE		-1		/* Validation states of message */
#define SEMEL_INVALID           -2
#define bOK			1

/* Message protocol */
#define	LF	0x0a

/* Buffers */

#define MAX_CCP_DATA    4096

/* System defines */
#define	bTRUE	1
#define bFALSE	0

enum  						/* Message numbering */
{
	SESSION_BEGIN,				/* Message 0 */
	SESSION_END,
	ERROR_MSG,
	CHECKING,
	SEMEL_OK,
	MSG_TO_CAR,
	MSG_FROM_CAR,
	AS_UP,
	AS_DWN
};

typedef struct mrf
{
	int	nMsgNum;			/* Message number */
	UCHAR	szMsgName[5];			/* Message name */
	int	nFieldNum;			/* Number of fields in message */
}MsgXref;

typedef struct mi
{
	int nMsgNum;				/* Message number */
	int state;				/* Validation state of message */
	int nField[MAX_SEMEL_FLDS];		/* Start of field in message */
	int	nFieldSize[MAX_SEMEL_FLDS];	/* Length of field in characters */
	int nSize;				/* Size of entire message in characters */
}MsgInfo;

/* Prototypes */
cBOOL GetSemelMsgInfo(UCHAR *buf, int nBufSize, MsgInfo *RetMsgInfo);
char *field(UCHAR *string, int num);
