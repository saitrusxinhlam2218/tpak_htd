/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: process.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1997/08/09 17:57:51 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/process.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
typedef struct process_header
{
    char    p_name[20];		/* process name */
    char    p_path[64];		/* path */
    char    p_argv[20];		/* arguments */
    int     p_pid;		/* process id */
    int     p_ppid;		/* parent process id */
    long    p_total_seconds_up; /* total time up in seconds */ 
    long    p_loadtime;		/* time of first load  */
    long    p_starttime;	/* time of last start */
    long    p_lasttime;		/* time of last contact */
    short   p_status;		/* current status */
#define	    PROCESS_UP		0
#define	    PROCESS_DOWN	1
#define	    PROCESS_SUSPENDED	2
#define     PROCESS_IGNORE	3
#define     PROCESS_STOPPED	4
#define     PROCESS_MANAGER	5
#define     PROCESS_REDUNDANT	6
#define     PROCESS_USER_LOAD	7
#define     PROCESS_NOLOOK	8
#define     PROCESS_DYNAMIC     9
    short   p_downaction;	/* action to be taken if load retries are exceeded */
#define     DA_FAILOVER		0
#define     DA_SUSPEND		1
    short   p_reloadcount;	/* reload count (number of times reloaded) */
    int     p_supp_reloads;     /* a supplemental count */
    struct process_header * p_next; 
}   PROCESSHDR;
extern PROCESSHDR * pheader;
extern PROCESSHDR * pcurrent;
#define MAXRELOAD 3
PROCESSHDR * CreateProcess();
