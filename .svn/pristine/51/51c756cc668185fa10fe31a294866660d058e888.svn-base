static char sz__FILE__[]="@(#)ui_error.c	10.1.2.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  ui_error.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:50:25
 * @(#)  Last delta: 12/2/94 at 18:21:16
 * @(#)  SCCS File: /taxi/sccs/s.ui_error.c
 *                                                                           *
 *****************************************************************************/
/* ui_error.c: conatins error messages to be used by the user interface program */

#include "ui_error.h"
#include "ui_strdefs.h"

char *ui_errlist[UI_LAST_ERR + 1];


/*
 * ui_errlist array initialization this may be awkward but it works for
 * internationalizing the strings
 */

void ui_errlist_init()
{

 	ui_errlist[UI_OPEN] 		= m_catgets(UI_m_catd, UI_1, UI_ERROR_1, "Unable to open file");
	ui_errlist[UI_MSGGET] 		= m_catgets(UI_m_catd, UI_1, UI_ERROR_2, "Unable to get msg queue id");
	ui_errlist[UI_SHMGET] 		= m_catgets(UI_m_catd, UI_1, UI_ERROR_3, "Unable to get shared memory id"); 
	ui_errlist[UI_SHMAT] 		= m_catgets(UI_m_catd, UI_1, UI_ERROR_4, "Unable to attach shared memory"); 
	ui_errlist[UI_SEMGET] 		= m_catgets(UI_m_catd, UI_1, UI_ERROR_5, "Unable to get semaphore set id"); 
	ui_errlist[UI_GETLOGIN] 	= m_catgets(UI_m_catd, UI_1, UI_ERROR_6, "Unable to get users login name"); 
	ui_errlist[UI_GETUTENT] 	= m_catgets(UI_m_catd, UI_1, UI_ERROR_7, "Unable to get tty number"); 
	ui_errlist[UI_DSKREAD] 		= m_catgets(UI_m_catd, UI_1, UI_ERROR_8, "Unable to read cisam record from disk"); 
	ui_errlist[UI_NOSCR] 		= m_catgets(UI_m_catd, UI_1, UI_ERROR_9, "Unable to display screen");
	ui_errlist[UI_NOT_ALLOWED]  = m_catgets(UI_m_catd, UI_1, UI_ERROR_10, "Not allowed to execute");
	ui_errlist[UI_MENU_INDEX]   = m_catgets(UI_m_catd, UI_1, UI_ERROR_11, "Wrong menu index");
	ui_errlist[UI_NOT_DEFINED]  = m_catgets(UI_m_catd, UI_1, UI_ERROR_12, "Undefined item");
	ui_errlist[UI_INVALID_NUM]  = m_catgets(UI_m_catd, UI_1, UI_ERROR_13, "Invalid number specified");
	ui_errlist[UI_WRONG_TTYNUM] = m_catgets(UI_m_catd, UI_1, UI_ERROR_14, "Wrong tty number specified");
	ui_errlist[UI_SEMID] 		= m_catgets(UI_m_catd, UI_1, UI_ERROR_15, "Unable to get semaphore id");
	ui_errlist[UI_SBRK] 		= m_catgets(UI_m_catd, UI_1, UI_ERROR_16, "Sbrk failed");
	ui_errlist[UI_CLOSE] 		= m_catgets(UI_m_catd, UI_1, UI_ERROR_17, "Unable to close file");
	ui_errlist[UI_NOEXEC] 		= m_catgets(UI_m_catd, UI_1, UI_ERROR_18, "Unable to exec");
	ui_errlist[UI_MSGERROR] 	= m_catgets(UI_m_catd, UI_1, UI_ERROR_19, "Invalid destination for message");
	ui_errlist[UI_NOFLEET] 		= m_catgets(UI_m_catd, UI_1, UI_ERROR_20, "Invalid fleet id specified");
	ui_errlist[UI_NOFIELD] 		= m_catgets(UI_m_catd, UI_1, UI_ERROR_21, "Invalid field entry");
	ui_errlist[UI_REQENTRY] 	= m_catgets(UI_m_catd, UI_1, UI_ERROR_22, "Required field not entered");
	ui_errlist[UI_NOUPDATE] 	= m_catgets(UI_m_catd, UI_1, UI_ERROR_23, "Unable to update record on disk");
	ui_errlist[UI_FATAL] 		= m_catgets(UI_m_catd, UI_1, UI_ERROR_24, "A fatal error has occured");
	ui_errlist[UI_NOZONE] 		= m_catgets(UI_m_catd, UI_1, UI_ERROR_25, "Unable to zone this call");
	ui_errlist[UI_WRITE] 		= m_catgets(UI_m_catd, UI_1, UI_ERROR_26, "Unable to write to a file");
	ui_errlist[UI_NOEXCEPTS] 	= m_catgets(UI_m_catd, UI_1, UI_ERROR_27, "Unable to find exceptions");
	ui_errlist[UI_NO_RECORD] 	= m_catgets(UI_m_catd, UI_1, UI_ERROR_28, "No more records found");
	ui_errlist[UI_NO_DTL] 		= m_catgets(UI_m_catd, UI_1, UI_ERROR_29, "No detail line number available");
 	ui_errlist[UI_WRONG_NUM] 	= m_catgets(UI_m_catd, UI_1, UI_ERROR_30, "Wrong number was entered");
 	ui_errlist[UI_WRONG_TM] 	= m_catgets(UI_m_catd, UI_1, UI_ERROR_31, "Wrong date or time was entered");
	ui_errlist[UI_ENTERVALUE] 	= m_catgets(UI_m_catd, UI_1, UI_ERROR_32, "You must enter a value");
	ui_errlist[UI_EXPT_RESOLVED] = m_catgets(UI_m_catd, UI_1, UI_ERROR_33, "This exception has already been resolved");
	ui_errlist[UI_REC_UPDATED] 	= m_catgets(UI_m_catd, UI_1, UI_ERROR_34, "Record updated");
	ui_errlist[UI_EXTRA_CABS] 	= m_catgets(UI_m_catd, UI_1, UI_ERROR_35, "Number of cabs specified is greater than max allowed");
	ui_errlist[UI_NOT_ACTIVE] 	= m_catgets(UI_m_catd, UI_1, UI_ERROR_36, "Fleet specified is not active");
	ui_errlist[UI_NODETAIL] 	= m_catgets(UI_m_catd, UI_1, UI_ERROR_37, "This exception does not have detail");
	ui_errlist[UI_NOPREV] 		= m_catgets(UI_m_catd, UI_1, UI_ERROR_38, "No previous line available");
	ui_errlist[UI_NO_HP_LN] 	= m_catgets(UI_m_catd, UI_1, UI_ERROR_39, "No help line available");
	ui_errlist[UI_INV_KEY] 		= m_catgets(UI_m_catd, UI_1, UI_ERROR_40, "Invalid function key");
	ui_errlist[UI_SENTFOR_DISPATCH] = m_catgets(UI_m_catd, UI_1, UI_ERROR_41, "Call sent for dispatch");
	ui_errlist[UI_PRT_SCR] 		= m_catgets(UI_m_catd, UI_1, UI_ERROR_42, "Screen submitted for printing, proceed");
	ui_errlist[UI_NOTIMECALL] 	= m_catgets(UI_m_catd, UI_1, UI_ERROR_43, "This is not a time call");
	ui_errlist[BAD_DATE] 		= m_catgets(UI_m_catd, UI_1, UI_ERROR_44, "Invalid date entered");
	ui_errlist[BAD_TIME] 		= m_catgets(UI_m_catd, UI_1, UI_ERROR_45, "Invalid time entered");
	ui_errlist[UI_NOHISTORY] 	= m_catgets(UI_m_catd, UI_1, UI_ERROR_46, "No call history available");
	ui_errlist[UI_TO_DISPATCH] 	= m_catgets(UI_m_catd, UI_1, UI_ERROR_47, "Call sent for dispatcher approval");
	ui_errlist[UI_CALL_CANCEL] 	= m_catgets(UI_m_catd, UI_1, UI_ERROR_48, "CALL KILLED");
	ui_errlist[UI_CALL_BACK] 	= m_catgets(UI_m_catd, UI_1, UI_ERROR_49, "Call back action taken");
	ui_errlist[UI_CALL_DISPATCHED] = m_catgets(UI_m_catd, UI_1, UI_ERROR_50, "Call has been dispatched");
	ui_errlist[UI_CANCELLED_CALL] = m_catgets(UI_m_catd, UI_1, UI_ERROR_51, "Call cancelled");
	ui_errlist[UI_SENT] 		= m_catgets(UI_m_catd, UI_1, UI_ERROR_52, "Call sent");
	ui_errlist[UI_LINKED] 		= m_catgets(UI_m_catd, UI_1, UI_ERROR_53, "Call linked");
	ui_errlist[UI_UNLINKED] 	= m_catgets(UI_m_catd, UI_1, UI_ERROR_54, "Call Unlinked");
	ui_errlist[UI_VEHICLE_SND_ERR] = m_catgets(UI_m_catd, UI_1, UI_ERROR_55, "Vehicle file maint msgsnd failed");
	ui_errlist[UI_DRIVER_SND_ERR] = m_catgets(UI_m_catd, UI_1, UI_ERROR_56, "Driver file maint msgsnd failed");
	ui_errlist[UI_SYSTEM_DOWN] = m_catgets(UI_m_catd, UI_1, UI_ERROR_57, "SYSTEM NOT RESPONDING");
	ui_errlist[UI_VEH_STATUS] = m_catgets(UI_m_catd, UI_1, UI_ERROR_58, "Vehicle not logged in or not idle");
	ui_errlist[UI_BAD_TYPE] = m_catgets(UI_m_catd, UI_1, UI_ERROR_59, "Wrong call type entered");
	ui_errlist[UI_FLAT_RATE] = m_catgets(UI_m_catd, UI_1, UI_ERROR_60, "Flat Rate Calculation");
}
