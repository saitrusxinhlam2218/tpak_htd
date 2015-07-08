/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  ccp_var.h; Rel: 10.1.0.0; Get date: 4/18/95 at 16:36:11		            *			
* @(#)  Last delta: 12/2/94 at 12:16:20						    *	
* @(#)  SCCS File: /taxi/sccs/s.ccp_var.h							    *
*                                                                           *
* Copyright (c) 1994 - TaxiCom ApS					    *
*                                                                           *
*****************************************************************************/
MsgXref	 MinutusMsg[] = 
{
	SESSION_BEGIN,	"OPEN", 3,
	SESSION_END,	"QUIT", 3,
	ERROR_MSG,	"ERRO", 3,
	CHECKING,	"CHEK", 1,
	SEMEL_OK,   "OK__", 1,
	MSG_TO_CAR,	"CARA", 5,
	MSG_FROM_CAR,	"CARQ", 5,
	AS_UP,		"UP__", 1,
	AS_DWN,		"DOWN", 1
};

#define MSG_LENGTH_FIELD	4
#define MSG_TYPE_FIELD		1
#define MSG_TERM_ID_FIELD	3
#define MSG_SUB_TYPE_FIELD	2

#define	NUM_CCP_MSGS ( sizeof(MinutusMsg)/sizeof(MinutusMsg[0]) )
