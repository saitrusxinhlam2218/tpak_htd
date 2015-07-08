/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Emergency_log_db.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1998/02/23 21:42:59 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Emergency_log_db.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ifndef _EMERG_LOG_DB_H
#define _EMERG_LOG_DB_H
#include <isam.h>
typedef struct cisam_em   {
       char   fleet;                      /* Fleet indicator              */
       char   pad;
       int    nbr;                        /* A system assigned emergency id*/
       int    exc_nbr;                    /* Exception No. of the correspond exception.                    */
       int    call_nbr;                   /* Call number of the call.      */
       int    date_time;                  /* Emergency date and time.      */
       char   event_date[10];		/* INFORMIX format		*/
       char   event_time[7];	
       char   resolved_date[10];
       char   resolved_time[7];
       short  veh_nbr;                    /* Vehicle number that invoked emergency.            */
       int    drv_id;                     /* Driver ID that invoked emergency.                    */
       short  zone_nbr;                   /* Zone number of the zone the driver was posted into or zone of call pickup.       */
       int    resolved_dt_tm;             /* Date and time this emergency was resolved.       */
       short  resolved_by;                /* User ID of user that resolved this emergency.      */
       char   comment[79];		  /* 79 because with the brackets "[]" and text that's all that fits on screen 80 wide */
} EMERG_REC;
#ifdef KEY_SRC
struct keydesc em_key1= {
         ISDUPS,
         2,
				 { {0,CHARSIZE,CHARTYPE},
					 {14,LONGSIZE,LONGTYPE} }
};
/* nbr system assigned for redundancy only */
struct keydesc em_key2= {
         ISNODUPS,
         1,
				 { {2,LONGSIZE,LONGTYPE} }
};
#else
extern struct keydesc em_key1;
extern struct keydesc em_key2;
#endif
#endif
