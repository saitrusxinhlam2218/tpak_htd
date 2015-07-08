 static char sz__FILE__[]="@(#)ui_syserr.c  10.1.2.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  ui_syserr.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:51:54
 * @(#)  Last delta: 12/2/94 at 18:34:24
 * @(#)  SCCS File: /taxi/sccs/s.ui_syserr.c
 *                                                                           *
 *****************************************************************************/
/* ui_syserr.c: error message for system errors. The #defines corresponding to these messges 
 *		are in except.h. The #defines start from 200 and are incremented  by 10. The max
 *		chars in the string can be 20 char's.
 */
#include <sys/types.h>

#include "except.h"
#include "ui_strdefs.h"

char *ui_syserr[SYS_ERR_MAX] =  { 0 };


void ui_syserr_init()
{
	ui_syserr[0] = 	m_catgets(UI_m_catd, UI_1, UI_SYSERR_1, "COMM LINK TO ACC");	/* BISYNC_ERR = 200 */
	ui_syserr[1] = 	m_catgets(UI_m_catd, UI_1, UI_SYSERR_2, "ACC-KISS INTERFACE");	/* ACC_KISS_ER = 210 */
	ui_syserr[2] = 	m_catgets(UI_m_catd, UI_1, UI_SYSERR_3, "NAK ON CALL ASSIGN");	/* ASSIGN_ERR = 220 */
	ui_syserr[3] = 	m_catgets(UI_m_catd, UI_1, UI_SYSERR_4, "VEHICLE REG/DEREG");	/* REG_DEREG_ERR = 230 */
	ui_syserr[4] = 	m_catgets(UI_m_catd, UI_1, UI_SYSERR_5, "BID LIST IS FULL");	/* BID_LST_FULL_ERR = 240 */
	ui_syserr[5] = 	m_catgets(UI_m_catd, UI_1, UI_SYSERR_6, "ACC ALARM");		/* ACC_ALARM = 250 */
	ui_syserr[6] = 	m_catgets(UI_m_catd, UI_1, UI_SYSERR_7, "INACTIVE ACC ALARM");	/* INACT_ACC_ALARM = 260 */
	ui_syserr[7] = 	m_catgets(UI_m_catd, UI_1, UI_TEXT_34, "DISK ALMOST FULL");	/* DISK_ALMOST_FULL = 270 */
	ui_syserr[8] =  m_catgets(UI_m_catd, UI_1, UI_TEXT_35, "AS IS DOWN");           /* AS_DOWN = 280 */
	ui_syserr[9] =  m_catgets(UI_m_catd, UI_1, UI_TEXT_36, "NO ACK, MDT MSG 1.");   /* NO_ACK_MDT_1 = 290 */
	ui_syserr[10] = m_catgets(UI_m_catd, UI_1, UI_TEXT_37, "NO ACK, PRT MSG 1.");   /* NO_ACK_PRT_1 = 300 */
	ui_syserr[11] = m_catgets(UI_m_catd, UI_1, UI_TEXT_38, "NO ACK, ITM MSG 1.");   /* NO_ACK_ITM_1 = 310 */
	ui_syserr[12] = m_catgets(UI_m_catd, UI_1, UI_TEXT_39, "NO ACK, MDT MSG 2.");   /* NO_ACK_MDT_2 = 320 */
	ui_syserr[13] = m_catgets(UI_m_catd, UI_1, UI_TEXT_40, "NO ACK, PRT MSG 2.");   /* NO_ACK_PRT_2 = 330 */
	ui_syserr[14] = m_catgets(UI_m_catd, UI_1, UI_TEXT_41, "NO ACK, ITM MSG 2.");   /* NO_ACK_ITM_2 = 340 */
	ui_syserr[15] = m_catgets(UI_m_catd, UI_1, UI_TEXT_42, "NO ACK, MDT MSG 3.");   /* NO_ACK_MDT_2 = 350 */
	ui_syserr[16] = m_catgets(UI_m_catd, UI_1, UI_TEXT_43, "NO ACK, PRT MSG 3.");   /* NO_ACK_PRT_2 = 360 */
	ui_syserr[17] = m_catgets(UI_m_catd, UI_1, UI_TEXT_44, "NO ACK, ITM MSG 3.");   /* NO_ACK_ITM_2 = 370 */
	ui_syserr[18] = m_catgets(UI_m_catd, UI_1, UI_TEXT_45, "API CHILD DIED");       /* API_CHILD_DEATH = 380 */
	ui_syserr[19] = m_catgets(UI_m_catd, UI_1, UI_TEXT_46, "NEW API CLIENT");       /* NEW_API_CLIENT = 390 */
}
