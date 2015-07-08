/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Subcall_hist_db.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 2000/03/02 19:44:11 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Subcall_hist_db.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ifndef _SUBCALL_HIST_DB_H
#define _SUBCALL_HIST_DB_H
#include <isam.h>
typedef struct    cisam_subch 
 {
	int	 record_nbr;		  /* Unique number only for redundancy */
        int      call_nbr;                /* Call number of the call.      */
        short    exception_nbr;           /* Corresponds to the internally used exception numbers. to be
                                           used for quick retrieval prior to resolution.                */
        char     fleet;                   /* Fleet indicator.              */
        char     event_type;              /* This event occured because of "R"eguglar system functions or */
                                          /* this event occured because of dispatcher "E"mulating a kdt terminal. */
        int      status_dt_tm;            /* Status change date and time.   */
        char     status_date[10];         /* Event occurence date.          */
        char     status_time[7];          /* Event occurence time.          */
        char     event_desc[9];           /* Event that took place.         */
        short    veh_nbr;                 /* Vehicle number that serviced this call.           */
        int 	  drv_id;                  /* Driver ID who caused this event  */
        short    user_id;                 /* User ID who caused this event    */
        char     action_taken[9];         /* Action taken by a driver/user to resolve this event.              */
        char     action_date[10];         /* Date of action taken.            */
        char     action_time[7];          /* Timem of action taken.           */
        int    	action_dt_tm;            /* Date and time of the action taken*/
        short    action_veh_nbr;          /* vehicle number of the veh taking this action.                     */
        int    action_drv_id;           /* Driver id of the driver taking this action.                          */
        short    action_user_id;          /* User ID of the user taking action*/
   } SUBCALL_HIST_REC;
#ifdef KEY_SRC
/* Composite of (call_nbr,exception_nbr) */
struct keydesc subch_key1=  {
	 ISDUPS,
	 2,
	 {     {4,LONGSIZE,LONGTYPE},
				 {8,INTSIZE,INTTYPE} }
};
/* record_nbr  system assigned for redundancy  only */
struct keydesc subch_key2=  {
	 ISNODUPS,
	 1,
	 {     {0,LONGSIZE,LONGTYPE} }
};
/* Composite of (call_nbr,status_dt_tm) */
struct keydesc subch_key3=  {
	 ISDUPS,
	 2,
	 {     {4,LONGSIZE,LONGTYPE},
				 {12,LONGSIZE,LONGTYPE} }
};
#else
extern struct keydesc subch_key1;
extern struct keydesc subch_key2;
extern struct keydesc subch_key3;
#endif
#endif
