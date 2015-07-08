/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  path.h; Rel: 10.1.2.1; Get date: 4/20/95 at 11:48:36
* @(#)  Last delta: 12/2/94 at 18:04:09
* @(#)  SCCS File: /taxi/sccs/s.path.h
*                                                                           *
* Copyright (c) 1990 - MDI Seattle                                          *
*                                                                           *
*****************************************************************************/
/* system paths */
#define PASSWD_FILE	"/etc/passwd"
#define GROUP_FILE    	"/etc/group"
#define USER_PIPE 	"/usr/taxi/run/user_pipe"	/* name of named pipe file */
#define GPS_PIPE       "/usr/taxi/run/gps_pipe"
#define CALLHND_PIPE   "/usr/taxi/run/callhnd_pipe"
#define TO_ASYNC_PIPE   "/usr/taxi/run/to_async_pipe"
#define FROM_ASYNC_PIPE "/usr/taxi/run/from_async_pipe"
#define TO_UDP_PIPE     "/usr/taxi/run/to_udp_pipe"
#define FROM_UDP_PIPE   "/usr/taxi/run/from_udp_pipe"
#define FROM_AUX_DEVICE_PIPE "/usr/taxi/run/from_aux_device_pipe"
#define TO_AUX_DEVICE_PIPE "/usr/taxi/run/to_aux_device_pipe"
#define	COUNTER_PIPE	"/usr/taxi/run/counter_pipe"	/* named pipe for use by counter process  and ui and dispatch */
#define WARM_TMP_FILE	"/usr/taxi/run/dispws.inp"	/*input file for warm start from user*/
#define KDT_USAGE_FILE 	"/usr/taxi/run/kdt_stats.out"
/* sysadmin files */
#define PASSWD_SAVE     "/usr/taxi/run/passwd.admin"    /* copy of password file */
#define GROUP_SAVE      "/usr/taxi/run/group.admin"     /* copy of group file */
#define SCRATCH_FILE    "/usr/taxi/run/scratch.admin"   /* scratch file */
/* user stuff */
#define	UI_RESPONSE_STAT_FILE	"/usr/taxi/run/ui_resp_stat"	/* file for response time statistics */
#define HOMEDIR		"/usr/taxi/"		/* user home directory */
#define CALLDIR		"ct"			/* call handlers directory */
#define DISPDIR		"disp"			/* call handlers directory */
#define MANDIR		"manager"			/* call handlers directory */
/* user maint stuff */
#define PASSFILE	"/usr/taxi/run/passwd.up"	/* copy of password file */
#define GROUPFILE	"/usr/taxi/run/group.up"	/* copy of group file */
#define SCRATCHFILE	"/usr/taxi/run/scratch.up"	/* scratch file */
#define	TEMPFILE	"/usr/taxi/run/temp.up"	/* temp file */
/* kdt download stuff */
#define V_FILE_NAME  "/usr/taxi/run/v_file"	/* Name of variable-length download function key names */
#define SS_FILE_NAME "/usr/taxi/run/ss_file"	/* Name of short-status download function key names */
#define MS_FILE_NAME "/usr/taxi/run/ms_file"	/* Name of message-status download function key names */
#define VLA_FILE_NAME  "/usr/taxi/run/vla_file"		/* LA Name of variable-length download function key names */
#define SSLA_FILE_NAME "/usr/taxi/run/ssla_file"	/* LA Name of short-status download function key names */
#define MSLA_FILE_NAME "/usr/taxi/run/msla_file"	/* LA Name of message-status download function key names */
/* Communication pipes credit process (Finish enhancement) */
#define	CCP2TP_PIPE	"/usr/taxi/run/ccp2tp"
#define TP2CCP_PIPE	"/usr/taxi/run/tp2ccp"
/* error stuff */
#define TRACE_FILE "/usr/taxi/run/trace.out"		/* trace file name */
#define MAD_ERROR_FILE "/usr/taxi/run/mad_error"	/* madd error file name */
#define	MM_LOG_FILE_PREFIX "/usr/taxi/run/traces/mm_log."		/* Memory management log file prefix (PID is appended later). */
/* redundancy */
#define  TRANS_DIR    "/usr/taxi/run/"   /* transaction dir */
#define  TRANS_FILE   "/usr/taxi/run/trans.data"
#define  TEMP_PASSWD_FILE "/usr/taxi/run/passwd"
#define  TEMP_GROUP_FILE  "/usr/taxi/run/group"
#define  TEMP_BAD_CREDIT_FILE "/usr/taxi/run/bad_credit_file"
/* cisam file names */
#define	ACCOUNT_FILE		"/usr/taxi/mads.dbs/account" 
#define ADDRESS_FILE		"/usr/taxi/mads.dbs/address"
#define ATTR_FILE		"/usr/taxi/mads.dbs/attr"
#define CALL_FILE   "/usr/taxi/mads.dbs/calls"
#define CALLMSG_FILE "/usr/taxi/mads.dbs/call_hnd"
#define CALLH_FILE		"/usr/taxi/mads.dbs/callh"
#define DRIVER_FILE		"/usr/taxi/mads.dbs/driver"
#define EMERG_FILE		"/usr/taxi/mads.dbs/emerg"
#define ERROR_FILE		"/usr/taxi/mads.dbs/error"
#define EXCTYPE_FILE		"/usr/taxi/mads.dbs/exctype"
#define EXCGRP_FILE		"/usr/taxi/mads.dbs/excgrp"
#define FLEET_FILE		"/usr/taxi/mads.dbs/fleet"
#define HELP_FILE		"/usr/taxi/mads.dbs/help"
#define HOLIDAY_FILE		"/usr/taxi/mads.dbs/holiday"
#define INTERST_FILE		"/usr/taxi/mads.dbs/interst"
#define LOCALTY_FILE		"/usr/taxi/mads.dbs/localty"
#define MENU_FILE		"/usr/taxi/mads.dbs/menu"
#define MAILBOX_FILE		"/usr/taxi/mads.dbs/mailbox"
#define MSGLOG_FILE		"/usr/taxi/mads.dbs/msglog"
#define PLACENM_FILE		"/usr/taxi/mads.dbs/placenm"
#define	RATE_FILE		"/usr/taxi/mads.dbs/rate" 
#define RPTMSG_FILE		"/usr/taxi/mads.dbs/rptmsg"
#define SEAL_FILE		"/usr/taxi/mads.dbs/seal"
#define SHIFT_FILE		"/usr/taxi/mads.dbs/shift"
#define SHIFTH_FILE		"/usr/taxi/mads.dbs/shifth"
#define STATS_FILE		"/usr/taxi/mads.dbs/stats"
#define STRNAME_FILE		"/usr/taxi/mads.dbs/strname"
#define STRTYPE_FILE		"/usr/taxi/mads.dbs/strtype"
#define SUBCALL_FILE		"/usr/taxi/mads.dbs/subcall"
#define SUBZONE_FILE		"/usr/taxi/mads.dbs/subzone"
#define SYSCNTL_FILE		"/usr/taxi/mads.dbs/syscntl"
#define UNZONE_FILE		"/usr/taxi/mads.dbs/unzoned"
#define PROFILE_FILE		"/usr/taxi/mads.dbs/profile"
#define VEHICLE_FILE		"/usr/taxi/mads.dbs/vehicle"
#define ZONE_FILE		"/usr/taxi/mads.dbs/zone"
#define CANMSG_FILE		"/usr/taxi/mads.dbs/canmsg"
#define VEH_CMSG_FILE           "/usr/taxi/mads.dbs/veh_cmsg"
#define STATDRV_FILE		"/usr/taxi/mads.dbs/statdrv"
#define STATUSR_FILE		"/usr/taxi/mads.dbs/statusr"
#define S_SCHED_FILE		"/usr/taxi/mads.dbs/s_sched"
#define S_SHIFT_FILE		"/usr/taxi/mads.dbs/s_shift"
#define CUSTOMER_FILE           "/usr/taxi/mads.dbs/customer"
#define HIST_CMT_FILE           "/usr/taxi/mads.dbs/hist_cmt"
#define RP_FILE                 "/usr/taxi/mads.dbs/res_per"
#define TIME_CALL_FILE          "/usr/taxi/mads.dbs/time_calls"
#define TLG_FILE                "/usr/taxi/mads.dbs/tlg"
#define CHANNEL_FILE            "/usr/taxi/mads.dbs/channel"
#define ACAD_FILE               "/usr/taxi/mads.dbs/acad"
#define INDV_FILE               "/usr/taxi/mads.dbs/indv"
#define RECEIPT_DATA_FILE       "/usr/taxi/mads.dbs/receipts"
#define BAD_CRED_FILE           "/usr/taxi/mads.dbs/bad_credit"
#define CARD_TYPE_FILE          "/usr/taxi/mads.dbs/card_type"
#define GPS_PARAMS_FILE         "/usr/taxi/mads.dbs/gps_params"
#define TL_FILE                 "/usr/taxi/mads.dbs/tlayer"
#define SYSMENUS_FILE           "/usr/taxi/mads.dbs/sysme00130"
#define SYSITEMS_FILE           "/usr/taxi/mads.dbs/sysme00131"
#define SUBCALLH_FILE           "/usr/taxi/mads.dbs/subcallh"
#define ZONEPOLY_FILE           "/usr/taxi/mads.dbs/zonepoly"
#define WEBUSER_FILE            "/usr/taxi/mads.dbs/webuser"
#define WEBADDR_FILE            "/usr/taxi/mads.dbs/webaddr"
#define FTJ_BAD_FILE            "/usr/taxi/mads.dbs/ftj_bad"
#define LINE_MGR_FILE           "/usr/taxi/mads.dbs/line_mgr"
#define GEOADDR_ADDR_FILE       "/usr/taxi/mads.dbs/geo_addr"
#define GEOADDR_LINE_FILE       "/usr/taxi/mads.dbs/geo_line"
#define GEOADDR_POINT_FILE      "/usr/taxi/mads.dbs/geo_point"
#define ZONETRANS_FILE          "/usr/taxi/mads.dbs/zonetrans"
#define ATTRANS_FILE            "/usr/taxi/mads.dbs/attrans"
#define ILINK_FILE              "/usr/taxi/mads.dbs/ilink"
#define IMSI_FILE               "/usr/taxi/mads.dbs/imsi"
#define S_VERSION_FILE          "/usr/taxi/mads.dbs/s_version"
#define XFER_FILE               "/usr/taxi/mads.dbs/xfer"
#define ZONEDOWN_FILE           "/usr/taxi/mads.dbs/zonedown"
#define KELANODE_FILE           "/usr/taxi/mads.dbs/kela_node"
/** Begin Rate Database Paths **/
#define TRANS_CAT_FILE          "/usr/taxi/rate.dbs/trans_cat"
#define TPERIODS_FILE           "/usr/taxi/rate.dbs/tperiods"
#define TRIPDATA_FILE           "/usr/taxi/rate.dbs/trip_data"
#define CONDITIONS_FILE         "/usr/taxi/rate.dbs/conditions"
#define SEASONS_FILE            "/usr/taxi/rate.dbs/seasons"
#define RELATIONS_FILE          "/usr/taxi/rate.dbs/relations"
#define REL_DATA_FILE           "/usr/taxi/rate.dbs/rel_data"
#define STD_REL_FILE            "/usr/taxi/rate.dbs/std_rel"
#define HOL_FILE                "/usr/taxi/rate.dbs/holiday"
#define COND_TYPE_FILE          "/usr/taxi/rate.dbs/cond_types"
#define DIST_FILTER_FILE        "/usr/taxi/rate.dbs/dist_filt"
#define M_ACTION_FILE           "/usr/taxi/rate.dbs/m_action"
/* pathnames used by reports */
#ifndef __alpha
#define ATJOBS          "/usr/spool/at/9*"
#else
#define ATJOBS          "/var/spool/cron/atjobs/*.a"
#endif
#define AU_MESSAGE      "/usr/taxi/bin/reports/au_message"
#define AU_LIVE         "/usr/taxi/bin/reports/au_live"
#define AU_TRAIL        "/usr/taxi/bin/reports/au_trail"
#define CRON_LOG        "/usr/lib/cron/log"
#define DA_CLEAN_UP     "/usr/taxi/bin/reports/da_clean_up"
#define DA_REPORT       "/usr/taxi/bin/reports/da_report"
#define DE_ACTVEH       "/usr/taxi/bin/reports/de_actveh"
#define DE_AUTOB        "/usr/taxi/bin/reports/de_autob"
#define DE_DRIVE        "/usr/taxi/bin/reports/de_drive"
#define DE_LMAIN        "/usr/taxi/bin/reports/de_lmain"
#define DE_MANIFST      "/usr/taxi/bin/reports/de_manifst"
#define DE_SUBSC        "/usr/taxi/bin/reports/de_subsc"
#define DE_UMAIN        "/usr/taxi/bin/reports/de_umain"
#define DO_STAT         "/usr/taxi/bin/reports/do_stat"
#define FILENAME_FILE   "/usr/taxi/etc/filenames.data"
#define DB_FILENAME_FILE "/usr/taxi/etc/db_filenames.data"
#define KDT_STATS_OUT   "/usr/taxi/run/kdt_stats.out"
#define KDT_STATS_OLD   "/usr/taxi/run/kdt_stats.old"
#define MADS_DBS_DIR    "/usr/taxi/mads.dbs"
#define MAIL            "/usr/spool/mail/root"
#define MESSAGES        "/usr/taxi/bin/reports/messages"
#define PRINT_CMD       "/usr/ucb/lpr"
#define MGR_PROFILE     "/usr/taxi/manager/.profile"
#define MORE_CMD	"/usr/ucb/more"
#define REP_DEV         "/usr/taxi/reports"  
#define REP_DIR         "/usr/taxi/bin/reports"
#define REPORT_BIN 	"/usr/taxi/bin/reports"
#define REPORT_CHOICES  "/usr/taxi/etc/reports.data"
#define REPORT_DATA	"/usr/taxi/reports"
#define REPTMPFILE 	"/usr/taxi/run/da_report.temp"
#define RUN_DIR         "/usr/taxi/run"
#define SHIFT_IDX       "/usr/taxi/bin/reports/shift.idx"
#define SHIFT_DAT       "/usr/taxi/bin/reports/shift.dat"
#define SHIFTH_IDX      "/usr/taxi/bin/reports/shifth.idx"
#define SHIFTH_DAT      "/usr/taxi/bin/reports/shifth.dat"
#define SUBCALLS        "/usr/taxi/bin/reports/da_subcall"
#define SULOG           "/usr/adm/sulog"
#define WTMP            "/etc/wtmp" 
/* For credit card */
#define CREDIT_BASE_DIR		"/usr/taxi/etc"
#define BAD_CREDIT_FILE		"/usr/taxi/etc/bad_credit_file"
#define LOADED_CREDIT_FILE	"loaded.credit_file"
#define TEMP_CREDIT_SORT_FILE	"/usr/taxi/etc/temp_sort"
#define CREDIT_LOCK_FILE	"/tmp/credit_lock"
#define CREDIT_TRANSACTION_FILE	"/usr/taxi/etc/credit_trans"
/* For ITM receipts */
#define RECEIPT_TRANSACTION_FILE "/usr/taxi/etc/receipt_trans"
#define MISSING_RECEIPT_TRANSACTION_FILE "/usr/taxi/etc/missing_receipt_trans"
#define REDUNDANT_HOST_NAME "redundant"
#define LANG_TEMP		"/usr/taxi/etc/lang.tmp"
/* For unloading database files	*/
#define DB_FILENAMES_SCREEN	"/usr/taxi/etc/db_tables.data"
#define UNLOAD_FILES "/usr/taxi/bin/unload_db_files"
/* File name that records dispatch's PID */
#define DISPATCH_PID_FILE       "/usr/taxi/run/dispatch_pid"
/* SYNCDB files */
#define SYNC_FILES_SCREEN     "/usr/taxi/etc/sync_files.data"
/* File system limits to create system exception */
#define FILE_SYSTEMS_LIST "/usr/taxi/etc/filesys_limits"
/* Emergency vehicle number file */
#define EMERG_VEH_FILE  "/usr/taxi/run/emerg_veh"
