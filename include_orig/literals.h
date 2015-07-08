 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  literals.h; Rel: 10.1.2.1; Get date: 4/20/95 at 11:47:20
 * @(#)  Last delta: 12/2/94 at 18:17:14
 * @(#)  SCCS File: /taxi/sccs/s.literals.h
 *                                                                           *
 *****************************************************************************/
#ident "@(#) head:literals.h	10.1.2.1"
#ident "@(#) head:literals.h	4.1.0.0"

#ifdef INTERNATIONAL

#ifndef INTER_INCLUDES
#include <nl_types.h>
#include <locale.h>
#define	INTER_INCLUDES
#endif

#ifdef DECLARING
nl_catd LITERAL_m_catd;
#else
extern nl_catd LITERAL_m_catd;
#endif

nl_catd catopen();
char    *catgets();

#else
/*
 * no international text	
 */

#define catgets( catd, set_num, msg_num, s )	s
#define	catclose( a )
#define setlocale( a, b )
#define open_catalog( a, b, c, d )

#endif

#define NO_ESC			catgets(LITERAL_m_catd, 1, 1, "You must terminate entry with ESC")
#define	ERROR_N_FLD		catgets(LITERAL_m_catd, 1, 2, "Error in field")
#define	NO_ROWS			catgets(LITERAL_m_catd, 1, 3, "There are no rows satisfying the conditions")
#define	READ_PROFILE_ERR_1 	catgets(LITERAL_m_catd, 1, 4, "Unable to read first record in user profile file")
#define READ_PROFILE_ERR	catgets(LITERAL_m_catd, 1, 5, "Unable to read user profile file")
#define	NO_CUR_LIST		catgets(LITERAL_m_catd, 1, 6, "There are no rows in the current list")
#define NO_ROWS_DIR		catgets(LITERAL_m_catd, 1, 7, "There are no more rows in the direction you are going")
#define RD_NXT_PROFILE_ERR	catgets(LITERAL_m_catd, 1, 8, "Unable to read next user profile file record")
#define RD_PREV_PROFILE_ERR	catgets(LITERAL_m_catd, 1, 9, "Unable to read previous user profile file record")
#define ADD_PROFILE_ERR		catgets(LITERAL_m_catd, 1, 10, "Unable to add new record to user profile file")
#define	LOCK_PROFILE_ERR	catgets(LITERAL_m_catd, 1, 11, "Unable to lock current record")
#define	UPD_PROFILE_ERR		catgets(LITERAL_m_catd, 1, 12, "Unable to update record in user profile file")
#define	REC_UPDATED		catgets(LITERAL_m_catd, 1, 13, "Record updated")
#define	RLS_PROFILE_ERR		catgets(LITERAL_m_catd, 1, 14, "Unable to release current record")
#define	DEL_PROFILE_ERR		catgets(LITERAL_m_catd, 1, 15, "Unable to delete record from user profile file")
#define	OPEN_OUT_ERR		catgets(LITERAL_m_catd, 1, 16, "Unable to open output file")
#define	PROFILE_GET_ERR		catgets(LITERAL_m_catd, 1, 17, "User profile file maint msgget failed")
#define	PROFILE_SND_ERR		catgets(LITERAL_m_catd, 1, 18, "User profile file maint msgsnd failed")
#define	QUERY_HDR		catgets(LITERAL_m_catd, 1, 19, "QUERY: ESC executes. INTERRUPT aborts. ")
#define	ADD_HDR			catgets(LITERAL_m_catd, 1, 20, "ADD: ESC executes. INTERRUPT aborts. ")
#define	MAIN_HDR		catgets(LITERAL_m_catd, 1, 21, "Query Next Previous Add Update Remove Output Exit ")
#define	PROFILE_TITLE		catgets(LITERAL_m_catd, 1, 22, "PROFILE FILE MAINTENANCE")
#define	UPDATE_HDR		catgets(LITERAL_m_catd, 1, 23, "UPDATE: ESC executes. INTERRUPT aborts. ")
#define	DELETE_HDR		catgets(LITERAL_m_catd, 1, 24, "Delete (Y or N)? ")
#define	PROFILE_FNAME_PROMPT	catgets(LITERAL_m_catd, 1, 25, "Enter output file(default is up_scr.out): ")
#define	FMODE_PROMPT		catgets(LITERAL_m_catd, 1, 26, "Output file %s Append or Create(default : C)? ")
#define	TRACE_OPEN_ERR		catgets(LITERAL_m_catd, 1, 27, "Unable to open trace file")
#define	PROFILE_OPEN_ERR	catgets(LITERAL_m_catd, 1, 28, "Unable to open user profile file")
#define	PROFILE_CLOSE_ERR	catgets(LITERAL_m_catd, 1, 29, "Unable to close user profile file")
#define	TRACE_CLOSE_ERR		catgets(LITERAL_m_catd, 1, 30, "Unable to close trace file")
#define	NO_U_NAME		catgets(LITERAL_m_catd, 1, 31, "User name does not exist")
#define	READ_VEHICLE_ERR_1 	catgets(LITERAL_m_catd, 1, 32, "Unable to read first record in vehicle file")
#define	VEHICLE_OPEN_ERR	catgets(LITERAL_m_catd, 1, 33, "Unable to open vehicle file")
#define	VEHICLE_CLOSE_ERR	catgets(LITERAL_m_catd, 1, 34, "Unable to close vehicle file")
#define ADD_VEHICLE_ERR		catgets(LITERAL_m_catd, 1, 35, "Unable to add new record to vehicle file")
#define	LOCK_VEHICLE_ERR	catgets(LITERAL_m_catd, 1, 11, "Unable to lock current record")
#define	UPD_VEHICLE_ERR		catgets(LITERAL_m_catd, 1, 36, "Unable to update record in vehicle file")
#define	RLS_VEHICLE_ERR		catgets(LITERAL_m_catd, 1, 14, "Unable to release current record")
#define	DEL_VEHICLE_ERR		catgets(LITERAL_m_catd, 1, 37, "Unable to delete record from vehicle file")
#define READ_VEHICLE_ERR	catgets(LITERAL_m_catd, 1, 38, "Unable to read vehicle file")
#define RD_NXT_VEHICLE_ERR	catgets(LITERAL_m_catd, 1, 39, "Unable to read next vehicle file record")
#define RD_PREV_VEHICLE_ERR	catgets(LITERAL_m_catd, 1, 40, "Unable to read previous vehicle file record")
#define	VEHICLE_GET_ERR		catgets(LITERAL_m_catd, 1, 41, "Vehicle file maint msgget failed")
#define	VEHICLE_SND_ERR		catgets(LITERAL_m_catd, 1, 42, "Vehicle file maint msgsnd failed")
#define	VEHICLE_FNAME_PROMPT	catgets(LITERAL_m_catd, 1, 43, "Enter output file(default is vh_scr.out): ")
#define	INVALID_DATE		catgets(LITERAL_m_catd, 1, 44, "Invalid date, format is MM/DD/YY")
#define	INVALID_TIME		catgets(LITERAL_m_catd, 1, 45, "Invalid time, format is HH:MM[AP]")
#define	VEHICLE_TITLE		catgets(LITERAL_m_catd, 1, 46, "VEHICLE FILE MAINTENANCE")
#define INVALID_FORMAT		catgets(LITERAL_m_catd, 1, 47, "Date/Time array must be 12 characters")
#define	INVALID_FORMAT_D	catgets(LITERAL_m_catd, 1, 48, "Invalid format, format is MM/DD/YY")
#define	INVALID_FORMAT_T    	catgets(LITERAL_m_catd, 1, 49, "Invalid format, format is hh:mm[ap]")
#define	NON_NUMERIC		catgets(LITERAL_m_catd, 1, 50, "Date/Time contains non-numeric characters")
#define	NON_NUMERIC_D		catgets(LITERAL_m_catd, 1, 51, "Date contains non-numeric character format is MM/DD/YY")
#define	NON_NUMERIC_T		catgets(LITERAL_m_catd, 1, 52, "Time contains non-numeric character format is hh:mm[ap]")
#define	INVALID_MONTH		catgets(LITERAL_m_catd, 1, 53, "Invalid Month, the range is (1 - 12)")
#define	INVALID_DAY		catgets(LITERAL_m_catd, 1, 54, "Invalid Day, the range is (1 - 31)")
#define	INVALID_HOUR		catgets(LITERAL_m_catd, 1, 55, "Invalid Hour, the range is (0 - 23)")
#define	INVALID_MIN		catgets(LITERAL_m_catd, 1, 56, "Invalid minute, the range is (0 - 59)")
#define	INVALID_SEC		catgets(LITERAL_m_catd, 1, 57, "Invalid second, the range is (0 - 59)")
#define	NOT_A_P			catgets(LITERAL_m_catd, 1, 58, "Time indicator must be an a or p")

#define	READ_DRIVER_ERR_1 	catgets(LITERAL_m_catd, 1, 59, "Unable to read first record in driver file")
#define	DRIVER_OPEN_ERR		catgets(LITERAL_m_catd, 1, 60, "Unable to open driver file")
#define	DRIVER_CLOSE_ERR	catgets(LITERAL_m_catd, 1, 61, "Unable to close driver file")
#define READ_DRIVER_ERR		catgets(LITERAL_m_catd, 1, 62, "Unable to read driver file")
#define RD_NXT_DRIVER_ERR	catgets(LITERAL_m_catd, 1, 63, "Unable to read next driver file record")
#define RD_PREV_DRIVER_ERR	catgets(LITERAL_m_catd, 1, 64, "Unable to read previous driver file record")
#define ADD_DRIVER_ERR		catgets(LITERAL_m_catd, 1, 65, "Unable to add new record to driver file")
#define	LOCK_DRIVER_ERR		catgets(LITERAL_m_catd, 1, 11, "Unable to lock current record")
#define	UPD_DRIVER_ERR		catgets(LITERAL_m_catd, 1, 66, "Unable to update record in driver file")
#define	RLS_DRIVER_ERR		catgets(LITERAL_m_catd, 1, 14, "Unable to release current record")
#define	DEL_DRIVER_ERR		catgets(LITERAL_m_catd, 1, 67, "Unable to delete record from driver file")
#define	DRIVER_GET_ERR		catgets(LITERAL_m_catd, 1, 68, "Driver file maint msgget failed")
#define	DRIVER_SND_ERR		catgets(LITERAL_m_catd, 1, 69, "Driver file maint msgsnd failed")
#define	DRIVER_TITLE		catgets(LITERAL_m_catd, 1, 70, "DRIVER FILE MAINTENANCE")
#define	DRIVER_FNAME_PROMPT	catgets(LITERAL_m_catd, 1, 71, "Enter output file(default is dr_scr.out): ")
#define	CALL_OPEN_ERR		catgets(LITERAL_m_catd, 1, 72, "Unable to open user call file")
#define	CALL_CLOSE_ERR		catgets(LITERAL_m_catd, 1, 73, "Unable to close user call file")
#define READ_CALL_ERR		catgets(LITERAL_m_catd, 1, 74, "Unable to read user call file")
#define	DRIVER_QUERY_HDR	catgets(LITERAL_m_catd, 1, 75, "QUERY: Enter Fleet and Driver ID to execute or INTERRUPT to abort. ")

#define	READ_FLEET_ERR_1 	catgets(LITERAL_m_catd, 1, 76, "Unable to read first record in fleet file")
#define READ_FLEET_ERR		catgets(LITERAL_m_catd, 1, 77, "Unable to read fleet file")
#define RD_NXT_FLEET_ERR	catgets(LITERAL_m_catd, 1, 78, "Unable to read next fleet file record")
#define RD_PREV_FLEET_ERR	catgets(LITERAL_m_catd, 1, 79, "Unable to read previous fleet file record")
#define ADD_FLEET_ERR		catgets(LITERAL_m_catd, 1, 80, "Unable to add new record to fleet file")
#define	LOCK_FLEET_ERR		catgets(LITERAL_m_catd, 1, 11, "Unable to lock current record")
#define	UPD_FLEET_ERR		catgets(LITERAL_m_catd, 1, 81, "Unable to update record in fleet file")
#define	RLS_FLEET_ERR		catgets(LITERAL_m_catd, 1, 14, "Unable to release current record")
#define	DEL_FLEET_ERR		catgets(LITERAL_m_catd, 1, 82, "Unable to delete record from fleet file")
#define	FLEET_GET_ERR		catgets(LITERAL_m_catd, 1, 83, "Fleet file maint msgget failed")
#define	FLEET_SND_ERR		catgets(LITERAL_m_catd, 1, 84, "Fleet file maint msgsnd failed")
#define	FLEET_MAIN_HDR		catgets(LITERAL_m_catd, 1, 85, "Query Next Previous Update Screen Output Exit ")
#define	FLEET_TITLE		catgets(LITERAL_m_catd, 1, 86, "FLEET FILE MAINTENANCE")
#define	FLEET_FNAME_PROMPT	catgets(LITERAL_m_catd, 1, 87, "Enter output file(default is fl_scr.out): ")
#define	FLEET_QUERY_HDR		catgets(LITERAL_m_catd, 1, 88, "QUERY: Enter Fleet ID to execute or INTERRUPT to abort.  ")
#define	FLEET_OPEN_ERR		catgets(LITERAL_m_catd, 1, 89, "Unable to open fleet file")
#define	FLEET_CLOSE_ERR		catgets(LITERAL_m_catd, 1, 90, "Unable to close fleet file")
#define	FLEET_ADD_HDR		catgets(LITERAL_m_catd, 1, 91, "ADD: Fleet ID and Fleet number are required fields. ESC executes. INTERRUPT aborts. ")
#define DATE_ZERO		catgets(LITERAL_m_catd, 1, 92, "The date must be set before the time can be entered")
#define	FIX_ERROR		catgets(LITERAL_m_catd, 1, 93, "The error must be corrected before exiting")
#define	NO_ATTACH		catgets(LITERAL_m_catd, 1, 94, "Unable to attach shared memory")
#define LOCALITY_OPEN_ERR 	catgets(LITERAL_m_catd, 1, 95, "Unable to open locality file")
#define READ_LOCALITY_ERR	catgets(LITERAL_m_catd, 1, 96, "Unable to read locality file")
#define LOCALITY_CLOSE_ERR 	catgets(LITERAL_m_catd, 1, 97, "Unable to close locality file")
#define ZONE_OPEN_ERR		catgets(LITERAL_m_catd, 1, 98, "Unable to open zone file")
#define READ_ZONE_ERR		catgets(LITERAL_m_catd, 1, 99, "Unable to read zone file")

#define	READ_HOL_ERR_1 	catgets(LITERAL_m_catd, 1, 100, "Unable to read first record in holiday file")
#define	HOL_OPEN_ERR	catgets(LITERAL_m_catd, 1,  101, "Unable to open holiday file")
#define	HOL_CLOSE_ERR	catgets(LITERAL_m_catd, 1, 102, "Unable to close holiday file")
#define ADD_HOL_ERR		catgets(LITERAL_m_catd, 1, 103, "Unable to add new record to holiday file")
#define	LOCK_HOL_ERR	catgets(LITERAL_m_catd, 1, 104, "Unable to lock current record")
#define	UPD_HOL_ERR		catgets(LITERAL_m_catd, 1, 105, "Unable to update record in holiday file")
#define	RLS_HOL_ERR		catgets(LITERAL_m_catd, 1, 106, "Unable to release current record")
#define	DEL_HOL_ERR		catgets(LITERAL_m_catd, 1, 107, "Unable to delete record from holiday file")
#define READ_HOL_ERR	catgets(LITERAL_m_catd, 1, 108, "Unable to read holiday file")
#define RD_NXT_HOL_ERR	catgets(LITERAL_m_catd, 1, 109, "Unable to read next holiday file record")
#define RD_PREV_HOL_ERR	catgets(LITERAL_m_catd, 1, 110, "Unable to read previous holiday file record")
#define	HOL_GET_ERR		catgets(LITERAL_m_catd, 1, 111, "holiday file maint msgget failed")
#define	HOL_SND_ERR		catgets(LITERAL_m_catd, 1, 112, "holiday file maint msgsnd failed")
#define	HOL_FNAME_PROMPT	catgets(LITERAL_m_catd, 1, 113, "Enter output file(default is hol_scr.out): ")
#define	HOL_TITLE		catgets(LITERAL_m_catd, 1, 114, "HOLIDAY FILE MAINTENANCE (FASTPRIS)")

#define	READ_COND_TYPE_ERR_1 	catgets(LITERAL_m_catd, 1, 115, "Unable to read first record in condition type file")
#define	COND_TYPE_OPEN_ERR	catgets(LITERAL_m_catd, 1, 116, "Unable to open condition type file")
#define	COND_TYPE_CLOSE_ERR	catgets(LITERAL_m_catd, 1, 117, "Unable to close condition type file")
#define ADD_COND_TYPE_ERR		catgets(LITERAL_m_catd, 1, 118, "Unable to add new record to condition type file")
#define	LOCK_COND_TYPE_ERR	catgets(LITERAL_m_catd, 1, 119, "Unable to lock current record")
#define	UPD_COND_TYPE_ERR		catgets(LITERAL_m_catd, 1, 120, "Unable to update record in condition type file")
#define	RLS_COND_TYPE_ERR		catgets(LITERAL_m_catd, 1, 121, "Unable to release current record")
#define	DEL_COND_TYPE_ERR		catgets(LITERAL_m_catd, 1, 122, "Unable to delete record from condition type file")
#define READ_COND_TYPE_ERR	catgets(LITERAL_m_catd, 1, 123, "Unable to read condition type file")
#define RD_NXT_COND_TYPE_ERR	catgets(LITERAL_m_catd, 1, 124, "Unable to read next condition type file record")
#define RD_PREV_COND_TYPE_ERR	catgets(LITERAL_m_catd, 1, 125, "Unable to read previous condition type file record")
#define	COND_TYPE_GET_ERR		catgets(LITERAL_m_catd, 1, 126, "condition type file maint msgget failed")
#define	COND_TYPE_SND_ERR		catgets(LITERAL_m_catd, 1, 127, "condition type file maint msgsnd failed")
#define	COND_TYPE_FNAME_PROMPT	catgets(LITERAL_m_catd, 1, 128, "Enter output file(default is ctype_scr.out): ")
#define	COND_TYPE_TITLE		catgets(LITERAL_m_catd, 1, 129, "CONDITION TYPE FILE MAINTENANCE (FASTPRIS)")

#define	READ_REL_ERR_1 	catgets(LITERAL_m_catd, 1, 130, "Unable to read first record in Relations file")
#define	REL_OPEN_ERR	catgets(LITERAL_m_catd, 1, 131, "Unable to open Relations file")
#define	REL_CLOSE_ERR	catgets(LITERAL_m_catd, 1, 132, "Unable to close Relations file")
#define ADD_REL_ERR		catgets(LITERAL_m_catd, 1, 133, "Unable to add new record to Relations file")
#define	LOCK_REL_ERR	catgets(LITERAL_m_catd, 1, 134, "Unable to lock current record")
#define	UPD_REL_ERR		catgets(LITERAL_m_catd, 1, 135, "Unable to update record in Relations file")
#define	RLS_REL_ERR		catgets(LITERAL_m_catd, 1, 136, "Unable to release current record")
#define	DEL_REL_ERR		catgets(LITERAL_m_catd, 1, 137, "Unable to delete record from Relations file")
#define READ_REL_ERR	catgets(LITERAL_m_catd, 1, 138, "Unable to read Relations file")
#define RD_NXT_REL_ERR	catgets(LITERAL_m_catd, 1, 139, "Unable to read next Relations file record")
#define RD_PREV_REL_ERR	catgets(LITERAL_m_catd, 1, 140, "Unable to read previous Relations file record")
#define	REL_GET_ERR		catgets(LITERAL_m_catd, 1, 141, "Relations file maint msgget failed")
#define	REL_SND_ERR		catgets(LITERAL_m_catd, 1, 142, "Relations file maint msgsnd failed")
#define	REL_FNAME_PROMPT	catgets(LITERAL_m_catd, 1, 143, "Enter output file(default is relations_scr.out): ")
#define	REL_TITLE		catgets(LITERAL_m_catd, 1, 144, "RELATIONS FILE MAINTENANCE (FASTPRIS)")
#define STD_REL_TITLE           catgets(LITERAL_m_catd, 1, 145, "STANDARD RELATIONS FILE MAINTENANCE (FASTPRIS)")

#define	READ_REL_TIME_ERR_1 	catgets(LITERAL_m_catd, 1, 146, "Unable to read first record in Relations Time file")
#define	REL_TIME_OPEN_ERR	catgets(LITERAL_m_catd, 1, 147, "Unable to open Relations Time file")
#define	REL_TIME_CLOSE_ERR	catgets(LITERAL_m_catd, 1, 148, "Unable to close Relations Time file")
#define ADD_REL_TIME_ERR		catgets(LITERAL_m_catd, 1, 149, "Unable to add new record to Relations Time file")
#define	LOCK_REL_TIME_ERR	catgets(LITERAL_m_catd, 1, 150, "Unable to lock current record")
#define	UPD_REL_TIME_ERR		catgets(LITERAL_m_catd, 1, 151, "Unable to update record in Relations Time file")
#define	RLS_REL_TIME_ERR		catgets(LITERAL_m_catd, 1, 152, "Unable to release current record")
#define	DEL_REL_TIME_ERR		catgets(LITERAL_m_catd, 1, 153, "Unable to delete record from Relations Time file")
#define READ_REL_TIME_ERR	catgets(LITERAL_m_catd, 1, 154, "Unable to read Relations Time file")
#define RD_NXT_REL_TIME_ERR	catgets(LITERAL_m_catd, 1, 155, "Unable to read next Relations Time file record")
#define RD_PREV_REL_TIME_ERR	catgets(LITERAL_m_catd, 1, 156, "Unable to read previous Relations Time file record")
#define	REL_TIME_GET_ERR		catgets(LITERAL_m_catd, 1, 157, "Relations Time file maint msgget failed")
#define	REL_TIME_SND_ERR		catgets(LITERAL_m_catd, 1, 158, "Relations Time file maint msgsnd failed")
#define	REL_TIME_FNAME_PROMPT	catgets(LITERAL_m_catd, 1, 159, "Enter output file(default is rel_time_scr.out): ")
#define	REL_TIME_TITLE		catgets(LITERAL_m_catd, 1, 160, "RELATION TIME FILE MAINTENANCE (FASTPRIS)")


#define	READ_TRIP_DATA_ERR_1 	catgets(LITERAL_m_catd, 1, 161, "Unable to read first record in Base Rate file")
#define	TRIP_DATA_OPEN_ERR	catgets(LITERAL_m_catd, 1, 162, "Unable to open Base Rate file")
#define	TRIP_DATA_CLOSE_ERR	catgets(LITERAL_m_catd, 1, 163, "Unable to close Base Rate file")
#define ADD_TRIP_DATA_ERR		catgets(LITERAL_m_catd, 1, 164, "Unable to add new record to Base Rate file")
#define	LOCK_TRIP_DATA_ERR	catgets(LITERAL_m_catd, 1, 165, "Unable to lock current record")
#define	UPD_TRIP_DATA_ERR		catgets(LITERAL_m_catd, 1, 166, "Unable to update record in Base Rate file")
#define	RLS_TRIP_DATA_ERR		catgets(LITERAL_m_catd, 1, 167, "Unable to release current record")
#define	DEL_TRIP_DATA_ERR		catgets(LITERAL_m_catd, 1, 168, "Unable to delete record from Base Rate file")
#define READ_TRIP_DATA_ERR	catgets(LITERAL_m_catd, 1, 169, "Unable to read Base Rate file")
#define RD_NXT_TRIP_DATA_ERR	catgets(LITERAL_m_catd, 1, 170, "Unable to read next Base Rate file record")
#define RD_PREV_TRIP_DATA_ERR	catgets(LITERAL_m_catd, 1, 171, "Unable to read previous Base Rate file record")
#define	TRIP_DATA_GET_ERR		catgets(LITERAL_m_catd, 1, 172, "Base Rate file maint msgget failed")
#define	TRIP_DATA_SND_ERR		catgets(LITERAL_m_catd, 1, 173, "Base Rate file maint msgsnd failed")
#define	TRIP_DATA_FNAME_PROMPT	catgets(LITERAL_m_catd, 1, 174, "Enter output file(default is baserate_scr.out): ")
#define	TRIP_DATA_TITLE		catgets(LITERAL_m_catd, 1, 175, "BASE RATE FILE MAINTENANCE (FASTPRIS)")
#define HOLIDAY_TITLE           catgets(LITERAL_m_catd, 1, 176, "HOLIDAY FILE MAINTENANCE (default) " )

#define	READ_RES_PER_ERR_1 	catgets(LITERAL_m_catd, 1, 177, "Unable to read first record in Period file")
#define	RES_PER_OPEN_ERR	catgets(LITERAL_m_catd, 1,  178, "Unable to open Period file")
#define	RES_PER_CLOSE_ERR	catgets(LITERAL_m_catd, 1, 179, "Unable to close Period file")
#define ADD_RES_PER_ERR		catgets(LITERAL_m_catd, 1, 180, "Unable to add new record to Period file")
#define	LOCK_RES_PER_ERR	catgets(LITERAL_m_catd, 1, 181, "Unable to lock current record")
#define	UPD_RES_PER_ERR		catgets(LITERAL_m_catd, 1, 182, "Unable to update record in Period file")
#define	RLS_RES_PER_ERR		catgets(LITERAL_m_catd, 1, 183, "Unable to release current record")
#define	DEL_RES_PER_ERR		catgets(LITERAL_m_catd, 1, 184, "Unable to delete record from Period file")
#define READ_RES_PER_ERR	catgets(LITERAL_m_catd, 1, 185, "Unable to read Period file")
#define RD_NXT_RES_PER_ERR	catgets(LITERAL_m_catd, 1, 186, "Unable to read next Period file record")
#define RD_PREV_RES_PER_ERR	catgets(LITERAL_m_catd, 1, 187, "Unable to read previous Period file record")
#define	RES_PER_GET_ERR		catgets(LITERAL_m_catd, 1, 188, "Period file maint msgget failed")
#define	RES_PER_SND_ERR		catgets(LITERAL_m_catd, 1, 189, "Period file maint msgsnd failed")
#define	RES_PER_FNAME_PROMPT	catgets(LITERAL_m_catd, 1, 190, "Enter output file(default is res_per_scr.out): ")
#define	RES_PER_TITLE		catgets(LITERAL_m_catd, 1, 191, "TIME PERIOD FILE MAINTENANCE ")

#define	READ_TC_ERR_1 	catgets(LITERAL_m_catd, 1, 192, "Unable to read first record in Transport Category file")
#define	TC_OPEN_ERR	catgets(LITERAL_m_catd, 1,  193, "Unable to open Transport Category file")
#define	TC_CLOSE_ERR	catgets(LITERAL_m_catd, 1, 194, "Unable to close Transport Category file")
#define ADD_TC_ERR		catgets(LITERAL_m_catd, 1, 195, "Unable to add new record to Transport Category file")
#define	LOCK_TC_ERR	catgets(LITERAL_m_catd, 1, 196, "Unable to lock current record")
#define	UPD_TC_ERR		catgets(LITERAL_m_catd, 1, 197, "Unable to update record in Transport Category file")
#define	RLS_TC_ERR		catgets(LITERAL_m_catd, 1, 198, "Unable to release current record")
#define	DEL_TC_ERR		catgets(LITERAL_m_catd, 1, 199, "Unable to delete record from Transport Category file")
#define READ_TC_ERR	catgets(LITERAL_m_catd, 1, 200, "Unable to read Transport Category file")
#define RD_NXT_TC_ERR	catgets(LITERAL_m_catd, 1, 201, "Unable to read next Transport Category file record")
#define RD_PREV_TC_ERR	catgets(LITERAL_m_catd, 1, 202, "Unable to read previous Transport Category file record")
#define	TC_GET_ERR		catgets(LITERAL_m_catd, 1, 203, "Transport Category file maint msgget failed")
#define	TC_SND_ERR		catgets(LITERAL_m_catd, 1, 204, "Transport Category file maint msgsnd failed")
#define	TC_FNAME_PROMPT	catgets(LITERAL_m_catd, 1, 205, "Enter output file(default is tc_scr.out): ")
#define	TC_TITLE		catgets(LITERAL_m_catd, 1, 206, "TRANSPORT CATEGORY FILE MAINTENANCE (FASTPRIS)")

#define	READ_TP_ERR_1 	catgets(LITERAL_m_catd, 1, 207, "Unable to read first record in Time Periods file")
#define	TP_OPEN_ERR	catgets(LITERAL_m_catd, 1,  208, "Unable to open Time Periods file")
#define	TP_CLOSE_ERR	catgets(LITERAL_m_catd, 1, 209, "Unable to close Time Periods file")
#define ADD_TP_ERR		catgets(LITERAL_m_catd, 1, 210, "Unable to add new record to Time Periods file")
#define	LOCK_TP_ERR	catgets(LITERAL_m_catd, 1, 211, "Unable to lock current record")
#define	UPD_TP_ERR		catgets(LITERAL_m_catd, 1, 212, "Unable to update record in Time Periods file")
#define	RLS_TP_ERR		catgets(LITERAL_m_catd, 1, 213, "Unable to release current record")
#define	DEL_TP_ERR		catgets(LITERAL_m_catd, 1, 214, "Unable to delete record from Time Periods file")
#define READ_TP_ERR	catgets(LITERAL_m_catd, 1, 215, "Unable to read Time Periods file")
#define RD_NXT_TP_ERR	catgets(LITERAL_m_catd, 1, 216, "Unable to read next Time Periods file record")
#define RD_PREV_TP_ERR	catgets(LITERAL_m_catd, 1, 217, "Unable to read previous Time Periods file record")
#define	TP_GET_ERR		catgets(LITERAL_m_catd, 1, 218, "Time Periods file maint msgget failed")
#define	TP_SND_ERR		catgets(LITERAL_m_catd, 1, 219, "Time Periods file maint msgsnd failed")
#define	TP_FNAME_PROMPT	catgets(LITERAL_m_catd, 1, 220, "Enter output file(default is tc_scr.out): ")
#define	TP_TITLE		catgets(LITERAL_m_catd, 1, 221, "TIME PERIODS FILE MAINTENANCE (FASTPRIS)")

#define	READ_COND_ERR_1 	catgets(LITERAL_m_catd, 1, 222, "Unable to read first record in condition file")
#define	COND_OPEN_ERR	catgets(LITERAL_m_catd, 1, 223, "Unable to open condition file")
#define	COND_CLOSE_ERR	catgets(LITERAL_m_catd, 1, 224, "Unable to close condition file")
#define ADD_COND_ERR		catgets(LITERAL_m_catd, 1, 225, "Unable to add new record to condition file")
#define	LOCK_COND_ERR	catgets(LITERAL_m_catd, 1, 226, "Unable to lock current record")
#define	UPD_COND_ERR		catgets(LITERAL_m_catd, 1, 227, "Unable to update record in condition file")
#define	RLS_COND_ERR		catgets(LITERAL_m_catd, 1, 228, "Unable to release current record")
#define	DEL_COND_ERR		catgets(LITERAL_m_catd, 1, 229, "Unable to delete record from condition file")
#define READ_COND_ERR	catgets(LITERAL_m_catd, 1, 230, "Unable to read condition file")
#define RD_NXT_COND_ERR	catgets(LITERAL_m_catd, 1, 231, "Unable to read next condition file record")
#define RD_PREV_COND_ERR	catgets(LITERAL_m_catd, 1, 232, "Unable to read previous condition file record")
#define	COND_GET_ERR		catgets(LITERAL_m_catd, 1, 233, "condition file maint msgget failed")
#define	COND_SND_ERR		catgets(LITERAL_m_catd, 1, 234, "condition file maint msgsnd failed")
#define	COND_FNAME_PROMPT	catgets(LITERAL_m_catd, 1, 235, "Enter output file(default is cond_scr.out): ")
#define	COND_TITLE		catgets(LITERAL_m_catd, 1, 236, "CONDITION FILE MAINTENANCE (FASTPRIS)")

#define	READ_SEASONS_ERR_1 	catgets(LITERAL_m_catd, 1, 237, "Unable to read first record in seasons file")
#define	SEASONS_OPEN_ERR	catgets(LITERAL_m_catd, 1,  238, "Unable to open seasons file")
#define	SEASONS_CLOSE_ERR	catgets(LITERAL_m_catd, 1, 239, "Unable to close seasons file")
#define ADD_SEASONS_ERR		catgets(LITERAL_m_catd, 1, 240, "Unable to add new record to seasons file")
#define	LOCK_SEASONS_ERR	catgets(LITERAL_m_catd, 1, 241, "Unable to lock current record")
#define	UPD_SEASONS_ERR		catgets(LITERAL_m_catd, 1, 242, "Unable to update record in seasons file")
#define	RLS_SEASONS_ERR		catgets(LITERAL_m_catd, 1, 243, "Unable to release current record")
#define	DEL_SEASONS_ERR		catgets(LITERAL_m_catd, 1, 244, "Unable to delete record from seasons file")
#define READ_SEASONS_ERR	catgets(LITERAL_m_catd, 1, 245, "Unable to read seasons file")
#define RD_NXT_SEASONS_ERR	catgets(LITERAL_m_catd, 1, 246, "Unable to read next seasons file record")
#define RD_PREV_SEASONS_ERR	catgets(LITERAL_m_catd, 1, 247, "Unable to read previous seasons file record")
#define	SEASONS_GET_ERR		catgets(LITERAL_m_catd, 1, 248, "seasons file maint msgget failed")
#define	SEASONS_SND_ERR		catgets(LITERAL_m_catd, 1, 249, "seasons file maint msgsnd failed")
#define	SEASONS_FNAME_PROMPT	catgets(LITERAL_m_catd, 1, 250, "Enter output file(default is seasons_scr.out): ")
#define	SEASONS_TITLE		catgets(LITERAL_m_catd, 1, 251, "SEASONS FILE MAINTENANCE (FASTPRIS)")

#define	READ_DF_ERR_1 	catgets(LITERAL_m_catd, 1, 192, "Unable to read first record in Distance Filter file")
#define	DF_OPEN_ERR	catgets(LITERAL_m_catd, 1,  193, "Unable to open Distance Filter file")
#define	DF_CLOSE_ERR	catgets(LITERAL_m_catd, 1, 194, "Unable to close Distance Filter file")
#define ADD_DF_ERR		catgets(LITERAL_m_catd, 1, 195, "Unable to add new record to Distance Filter file")
#define	LOCK_DF_ERR	catgets(LITERAL_m_catd, 1, 196, "Unable to lock current record")
#define	UPD_DF_ERR		catgets(LITERAL_m_catd, 1, 197, "Unable to update record in Distance Filter file")
#define	RLS_DF_ERR		catgets(LITERAL_m_catd, 1, 198, "Unable to release current record")
#define	DEL_DF_ERR		catgets(LITERAL_m_catd, 1, 199, "Unable to delete record from Distance Filter file")
#define READ_DF_ERR	catgets(LITERAL_m_catd, 1, 200, "Unable to read Distance Filter Category file")
#define RD_NXT_DF_ERR	catgets(LITERAL_m_catd, 1, 201, "Unable to read next Distance Filter  file record")
#define RD_PREV_DF_ERR	catgets(LITERAL_m_catd, 1, 202, "Unable to read previous Distance Filter file record")
#define	DF_GET_ERR		catgets(LITERAL_m_catd, 1, 203, "Distance Filter file maint msgget failed")
#define	DF_SND_ERR		catgets(LITERAL_m_catd, 1, 204, "Distance Filter file maint msgsnd failed")
#define	DF_FNAME_PROMPT	catgets(LITERAL_m_catd, 1, 205, "Enter output file(default is df_scr.out): ")
#define	DF_TITLE		catgets(LITERAL_m_catd, 1, 206, "DISTANCE FILTER FILE MAINTENANCE (FASTPRIS)")