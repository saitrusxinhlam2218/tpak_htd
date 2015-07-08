 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  ui_error.h; Rel: 10.1.2.1; Get date: 4/20/95 at 11:50:12
 * @(#)  Last delta: 12/2/94 at 18:00:44
 * @(#)  SCCS File: /taxi/sccs/s.ui_error.h
 *                                                                           *
 *****************************************************************************/
/* ui_error.h: contains error message defintions used by the user interface */
#define		UI_NO_MSG	-1	/* Don't print an error message from
					   ui_errlist[] */
#define		UI_OPEN		0 	/* "Unable to open file" */
#define		UI_MSGGET	1	/* "Unable to get msg queue id"	*/
#define		UI_SHMGET	2	/* "Unable to get shared memory id" */
#define		UI_SHMAT	3	/* "Unable to attach shared memory" */
#define		UI_SEMGET	4	/* "Unable to get semaphore set id" */
#define		UI_GETLOGIN	5	/* "Unable to get users login name" */
#define		UI_GETUTENT	6	/* "Unable to get tty number" */
#define		UI_DSKREAD	7	/* "Unable to read cisam record from 
					 * disk" */
#define		UI_NOSCR	8	/* "Unable to display screen" */
#define		UI_NOT_ALLOWED	9	/* "Not allowed to execute"  */
#define		UI_MENU_INDEX	10	/* "Wrong menu index" */
#define		UI_NOT_DEFINED	11	/* "Undefined item" */
#define		UI_INVALID_NUM	12	/* "Invalid number specified" */
#define		UI_WRONG_TTYNUM	13	/* "Wrong tty number specified" */
#define		UI_SEMID	14	/* "Unable to get semaphore id" */
#define		UI_SBRK		15	/* "Sbrk failed"		*/
#define		UI_CLOSE	16	/* "Unable to close file"	*/
#define		UI_NOEXEC	17	/* "Exec failed"		*/
#define		UI_MSGERROR	18	/* "Invalid destination for message */
#define		UI_NOFLEET	19	/* "INvalid fleet id specified" */
#define		UI_NOFIELD	20	/* "Invalid field entered"	*/
#define		UI_REQENTRY	21	/* "Required field not not entered"*/
#define 	UI_NOUPDATE	22	/* "Unable to update record on file" */
#define		UI_FATAL	23	/* "A fatal error hasa occured" */
#define		UI_NOZONE	24	/* "Unable to znoe call 	*/
#define		UI_WRITE	25	/* "Unable to write to file"	*/
#define		UI_NOEXCEPTS	26	/* "Unable to find exceptions" 	*/
#define		UI_NO_RECORD	27	/* "No record found in search" 	*/
#define		UI_NO_DTL	28	/* "No detail line number available" 	*/
#define		UI_WRONG_NUM	29	/* "Wrong number was entered" 	*/
#define		UI_WRONG_TM	30	/* "Wrong date or time was entered" */
#define		UI_ENTERVALUE	31	/* "You must enter value" */
#define		UI_EXPT_RESOLVED 32	/* This exception has already been resolved" */
#define		UI_REC_UPDATED	33	/* "Record updated" */
#define		UI_EXTRA_CABS	34	/* Number of cabs specified is greater than max. allowed */
#define		UI_NOT_ACTIVE	35	/* "Fleet specified is not active" */
#define		UI_NODETAIL	36	/* "This exception does not have a detail" */
#define		UI_NOPREV	37	/* "This exception does not have a detail" */
#define		UI_NO_HP_LN	38	/* "This exception does not have a detail" */
#define		UI_INV_KEY	39	/* "This exception does not have a detail" */
#define		UI_SENTFOR_DISPATCH 	40	/* "call sent for dispatch */
#define		UI_PRT_SCR	41	/* "screen image being printed on printer" */
#define		UI_NOTIMECALL	42	/* "This is not a time call" */
#define		BAD_DATE	43      /* "An invalid date was entered for search criteria"  */
#define		BAD_TIME	44	/* "An invalid time was entered for search criteria"  */
#define		UI_NOHISTORY	45	/* "NO call history available"	*/
#define		UI_TO_DISPATCH	46	/* Call sent for dispatcher approval */
#define		UI_CALL_CANCEL	47	/* Call cancelled */
#define		UI_CALL_BACK	48	/* Call back */
#define		UI_CALL_DISPATCHED	49	/* Call dispatched */
#define		UI_CANCELLED_CALL	50	/* Call cancelled */
#define		UI_SENT		51	/* Call sent */
#define		UI_LINKED	52	/* Call linked */
#define		UI_UNLINKED	53	/* Call Unlinked */
#define 	UI_VEHICLE_SND_ERR	54      /* "Vehicle file maint msgsnd failed" */
#define 	UI_DRIVER_SND_ERR       55	/* "Driver file maint msgsnd failed" */
#define		UI_SYSTEM_DOWN	56
#define         UI_VEH_STATUS   57
#define         UI_BAD_TYPE     58
#define         UI_FLAT_RATE    59
#define		UI_LAST_ERR	59	/* For ui_errlist[] declare */
