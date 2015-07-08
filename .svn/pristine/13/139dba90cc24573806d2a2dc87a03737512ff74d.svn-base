 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  mads_isam.h; Rel: 6.17.0.0; Get date: 4/8/92 at 16:02:27
 * @(#)  Last delta: 3/20/92 at 09:45:24
 * @(#)  SCCS File: /taxi/sccs/s.mads_isam.h
 *                                                                           *
 *****************************************************************************/
#ifndef _MADS_ISAM_H
#define _MADS_ISAM_H
#include 	<isam.h>          /* C-ISAM include file  */
#include  	"fleet.h"
#include "acc.h"
#include "ranges.h"
#define FILE_NAME_LEN	40
#define SUCCEED         0
#define WHOLEKEY        0
#define READ_KEYS       0
#define READ_NUM        1
#define UPDATE_UNIQ     2
#define UPDATE_REC      3
#define WRITE           4
#define RE_WRITE        5
#define START           6
#define ADD_REC         7
#define LOCK            8
#define UNLOCK          9
#define RELEASE         10
#define OPEN            11 
#define CLOSE           12
#define DELETE_UNIQ     13
#define DELETE          14
#define BUILD           15
#define ADDINX          16
#define REDN_ADD        17    /* For add on reduntant unit only */
#define UPDATE_NUM      18
#define READ_NUM_SPEC   19
#define FILE_UPDATE     99              /* For flat file renduncy */
struct file_desc {
	char file_name[FILE_NAME_LEN];
	int (*c_to_cisam)();
	int (*cisam_to_c)();
	int (*ctokey)();
	int size;
	int fd;
};
#include "Account_db.h"
#include "Address_db.h"
#include "Attribute_db.h"
#include "Call_msg_db.h"
#include "Call_db.h"
#include "Shift_db.h"
#include "Call_hist_db.h"
#include "Shift_hist_db.h"
#include "Canmsg_db.h"
#include "Driver_db.h"
#include "Emergency_log_db.h"
#include "Error_db.h"
#include "Excpt_type_db.h"
#include "Excpt_group_db.h"
#include "Fleet_private.h"
#include "Holiday_db.h"
#include "Intersect_db.h"
#include "Locality_db.h"
#include "Help_db.h"
#include "Place_db.h"
#include "Menu_db.h"
#include "Mailbox_db.h"
#include "Message_log_db.h"
#include "Rate_db.h"
#include "Rptmsg_log_db.h"
#include "Seal_db.h"
#include "Stats_db.h"
#include "Stats_drv_db.h"
#include "Stats_usr_db.h"
#include "Street_db.h"
#include "Street_type_db.h"
#include "Subcall_db.h"
#include "Subzone_db.h"
#include "System_control_db.h"
#include "Tlg_db.h"
#include "Channel_db.h"
#include "Unzoned_db.h"
#include "Profile_db.h"
#include "Vehicle_db.h"
#include "Zone_db.h"
#include "Customer_db.h"
#include "Hist_cmt_private.h"
#include "Res_periods_db.h"
#include "Time_call_db.h"
#include "Account_Address_db.h"
#include "Indv_db.h"
#include "Receipt_data_db.h"
#include "Bad_credit_db.h"
#include "Card_type_db.h"
#include "GPS_params_db.h"
#include "rate_isam.h"
#include "Sysmenus_db.h"
#include "Veh_canmsg_db.h"
#include "Tracking_Layer_private.h"
#include "Subcall_hist_db.h"
#include "Zonepoly_db.h"
#include "TPakWeb_User.h"
#include "TPakWeb_UserAddress.h"
#include "FTJ_bad_db.h"
#include "Line_Manager_db.h"
#include "GeoAddr_Addr_db.h"
#include "GeoAddr_Line_db.h"
#include "GeoAddr_Point_db.h"
#include "AttrTrans_db.h"
#include "ZoneTrans_db.h"
#include "Interlink_db.h"
#include "IMSI_db.h"
#include "S_Shift_db.h"
#include "S_Sched_db.h"
#include "ZoneDown_db.h"
#include "KelaNode_db.h"
#include "taxi_db.h"
#endif
