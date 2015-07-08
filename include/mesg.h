 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  mesg.h; Rel: 10.1.2.1; Get date: 4/20/95 at 11:47:59
 * @(#)  Last delta: 12/2/94 at 18:21:11
 * @(#)  SCCS File: /taxi/sccs/s.mesg.h
 *                                                                           *
 *****************************************************************************/
#ifndef MESG_H_
#define MESG_H_
#define NO_SAVE 'N'
#define YES_SAVE 'Y'
#define NO_CURR 'X'
#define IN_PROC 'I'
#define NEW_MSG 'N'
#define OLD_MSG 'O'
#define MSG_LOG 'M'
#define MSG_RPT 'R'
#define MSG_UPD 'U'
#define MSG_DEL 'D'
#define MAX_NO_MSG 5
#define PREFIX_VEH  1000000
#define PREFIX_DRIV 2000000
#define M_RPT           1
#define M_FLT           (M_RPT + 1)
#define M_TO1           (M_FLT + 1)
#define M_TO2           (M_TO1 + 1)
#define M_CANM          (M_TO2 + 1)
#define M_OFTN          (M_CANM + 1)
#define M_UNDT          (M_OFTN + 1)
#define M_UNTM          (M_UNDT + 1)
#define M_TXT1          (M_UNTM + 1)
#define M_TXT2          (M_TXT1 + 1)
#define M_DRVATTR       (M_TXT2 + 1)
#define MAX_PIU_MESG    M_DRVATTR
#define M_RPT_ROW  1
#define M_RPT_COL  14
#define M_RPT_SIZ  1
#define M_FLT_ROW  1
#define M_FLT_COL  26
#define M_FLT_SIZ  1
#define M_TO1_ROW  1
#define M_TO1_COL  34
#define M_TO1_SIZ  1
#define M_TO2_ROW  1
#define M_TO2_COL  36
#define M_TO2_SIZ  6
#define M_CANM_ROW  1
#define M_CANM_COL  54
#define M_CANM_SIZ  3
#define M_OFTN_ROW  1
#define M_OFTN_COL  70
#define M_OFTN_SIZ  5
#define M_UNDT_ROW  1
#define M_UNDT_COL  90
#define M_UNDT_SIZ  8
#define M_UNTM_ROW  1
#define M_UNTM_COL  105
#define M_UNTM_SIZ  6
#define M_VEHATTR_ROW 4
#define M_VEHATTR_COL 34
#define M_VEHATTR_SIZ 2
#define M_DRVATTR_ROW 4
#define M_DRVATTR_COL 14
#define M_DRVATTR_SIZ 2
#define M_TXT1_ROW  2
#define M_TXT1_COL  14
#define M_TXT1_SIZ  100
#define M_TXT2_ROW  3
#define M_TXT2_COL  14
#define M_TXT2_SIZ  100
struct recv_msg {
	int from_id;
	int dt_tm;
	char old;
	char repeat;
	char fleet;
	char to_type;
	char to_select[7];
	short how_often;
	int until;
	char msg_line1[M_TXT1_SIZ + 1];
	char msg_line2[M_TXT2_SIZ + 1];
};
#endif
