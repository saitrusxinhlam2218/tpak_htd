 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  pi_var.h; Rel: 10.1.2.2; Get date: 4/19/95 at 08:46:23
 * @(#)  Last delta: 12/14/94 at 10:36:27
 * @(#)  SCCS File: /taxi/sccs/s.pi_var.h
 *                                                                           *
 *****************************************************************************/
/* -------------------------------------------------------------------------------
   Copyright 1993 - TaxiCom Copenhagen, Denmark

   System	:	PLANET - LINK
   Filename	:	pi_var.h
   Purpose	:	Variable declared at file scope for pi	

Modification history

Author	Date	Revision		Description
------  ------- --------------- -----------------------------------------------
KAG      931125  0.0			Creation.

------------------------------------------------------------------------------- */


/*Global Variables */
TaxiStats	TaxiCallStats[NUM_STAT_FUNCS];

char    szLogMsg[2500];
int			ListenSock;		
ChildInfo	ServChildInfo[MAX_CHILD];
int		bSuperServer	= bTRUE;
int			nChildren		= bFALSE;
int		bSilent 		= bFALSE;
int			nMsgCount		= 0;

static char *szStatFuncs[] = {	"PI_UNKNOWN_ORDER",	
								"PI_ZONE_ADDRESS", 		"PI_DISPATCH_CALL",	
								"PI_CANCEL_CALL",		"PI_GET_CALL",
								"PI_GET_CAR",			"PI_GET_EXCEPTIONS",
								"PI_REPORT_EXCEPTIONS",	"PI_ACCEPT_ACCEPTIONS",
								"PI_SEND_MESSAGE",		"PI_GET_MESSAGE",
								"PI_ALIVE",       "PI_QUIT",
								"PI_DISP_EXT_CALL", "PI_GENERATE_EXCEPTION"
							};

/* Macros */
#define GETSTAT( func, entry ) TaxiCallStats[func].time[entry]
#define	GET_STATSLOT(func)	TaxiCallStats[func].nNextSlot

/* Function prototypes */
void	MakeStat( int nFunc, clock_t lDuration );
int	DumpStats( int nFunc );
void 	InitStats(void);
void 	DumpFuncStats(int nFunc, FILE *fpStats );
int		HandleStatSig( int code );
void	KillProc( int code );
