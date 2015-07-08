/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Call_hist_db.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1998/02/23 21:42:27 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Call_hist_db.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ifndef _CALL_HIST_DB_H
#define _CALL_HIST_DB_H
#include <isam.h>
typedef struct    cisam_ch 
 {
	int	 record_nbr;		  
        int      call_nbr;                
        short    exception_nbr;           
                                          
        char     fleet;                   
        char     event_type;              
                                          
        int      status_dt_tm;            
        char     status_date[10];         
        char     status_time[7];          
        char     event_desc[9];           
        short    veh_nbr;                 
        int 	  drv_id;                 
        short    user_id;                 
        char     action_taken[9];         
        char     action_date[10];         
        char     action_time[7];          
        int    	action_dt_tm;            
        short    action_veh_nbr;         
        int    action_drv_id;           
        short    action_user_id;        
   } CALL_HIST_REC;
#ifdef KEY_SRC
/* Composite of (call_nbr,exception_nbr) */
struct keydesc ch_key1=  {
	 ISDUPS,
	 2,
	 {     {4,LONGSIZE,LONGTYPE},
				 {8,INTSIZE,INTTYPE} }
};
/* record_nbr  system assigned for redundancy  only */
struct keydesc ch_key2=  {
	 ISNODUPS,
	 1,
	 {     {0,LONGSIZE,LONGTYPE} }
};
/* Composite of (call_nbr,status_dt_tm) */
struct keydesc ch_key3=  {
	 ISDUPS,
	 2,
	 {     {4,LONGSIZE,LONGTYPE},
				 {12,LONGSIZE,LONGTYPE} }
};
#else
extern struct keydesc ch_key1;
extern struct keydesc ch_key2;
extern struct keydesc ch_key3;
#endif
#endif
