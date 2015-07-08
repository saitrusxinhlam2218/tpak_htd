/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: msgcomms.h,v $
*  @(#)  $Revision: 1.5 $
*  @(#)  $Date: 1998/07/28 21:28:36 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/msgcomms.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#define MANAGERRETURN	1
#define WARMSTART	2
#define COLDSTART	3
#define VIEWPROCESS	4
#define STOPSYSTEM	5
#define NUMPROCESSES    6
#define APROCESS        7
#define QSTATUS         8
#define EXITUI		9
#define STOPNAME	10
#define SHUTDOWN	11
#define MANAGERUI	12
#define USERUI		13
#define CUTOVER		14
#define MSGDISPLAY	15
#define STARTMIRROR	16
#define BACKUP		17
#define SYNCDB		18
#define OKSYNC		19
#define STOPMIRROR	20
#define MDIPROCESSES	21
#define STARTFAILED	22
#define REREADCONFIG	23
#define TRACEON		24
#define TRACEOFF	25
#define SUSPENDEDSTART	26
#define START_GEOTAXI   50
#define STOP_GEOTAXI    51
#define START_TAO       52
#define STOP_TAO        53
#define GEO_ALREADY_RUNNING 54
#define GEO_STARTED     55
#define GEO_STOPPED     56
#define TAO_ALREADY_RUNNING 57
#define TAO_STARTED     58
#define TAO_STOPPED     59
#define START_TCB       60
#define STOP_TCB        61
#define TCB_ALREADY_RUNNING 62
#define TCB_STARTED         63
#define TCB_STOPPED         64
#define START_ASYNC         65
#define STOP_ASYNC          66
#define ASYNC_ALREADY_RUNNING       67
#define ASYNC_STARTED               68
#define ASYNC_STOPPED               69
#define START_DISPATCH         70
#define STOP_DISPATCH          71
#define DISPATCH_ALREADY_RUNNING       72
#define DISPATCH_STARTED               73
#define DISPATCH_STOPPED               74
#define START_AB                       75
#define STOP_AB                        76
#define AB_ALREADY_RUNNING             77
#define AB_STARTED                     78
#define AB_STOPPED                     79
#define START_GPS_PROC                 80
#define STOP_GPS_PROC                  81
#define GPS_PROC_ALREADY_RUNNING       82
#define GPS_PROC_STARTED               83
#define GPS_PROC_STOPPED               84
#define START_PI                 85
#define STOP_PI                  86
#define PI_ALREADY_RUNNING            87
#define PI_STARTED               88
#define PI_STOPPED               89
#define START_CCP                 90
#define STOP_CCP                  91
#define CCP_ALREADY_RUNNING       92
#define CCP_STARTED               93
#define CCP_STOPPED               94
#define AUTOCALL_ALREADY_RUNNING        95
#define AUTOCALL_STARTED                96
#define AUTOCALL_STOPPED                97
#define STOP_AUTOCALL             98
#define START_AUTOCALL            99
#define PSTAT_DISPATCH_UP         1000
#define PSTAT_DISPATCH_DOWN       1001
#define PSTAT_ASYNC_UP            1002
#define PSTAT_ASYNC_DOWN          1003
#define PSTAT_AUTOBOOK_UP         1004
#define PSTAT_AUTOBOOK_DOWN       1005
#define PSTAT_GPS_PROC_UP         1006
#define PSTAT_GPS_PROC_DOWN       1007
#define PSTAT_PI_UP               1008
#define PSTAT_PI_DOWN             1009
#define PSTAT_CCP_UP              1010
#define PSTAT_CCP_DOWN            1011
#define PSTAT_AUTOCALL_UP              1012
#define PSTAT_AUTOCALL_DOWN            1013
#define M_QUERYTOKEN	254
