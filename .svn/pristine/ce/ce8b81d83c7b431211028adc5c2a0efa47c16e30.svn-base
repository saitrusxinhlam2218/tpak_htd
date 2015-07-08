/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: System_control_db.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1998/02/23 21:44:35 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/System_control_db.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ifndef _SYS_CONTROL_DB_H
#define _SYS_CONTROL_DB_H
#include <isam.h>
typedef struct  cisam_syc  {
       short day_start;                       /* Starting time for a complete work day.       */
       short day_end;                         /* Ending time for a complete work day.                */
       short prod_start_time;                 /* Time of day to start the daily production run.  If none entered, has to be */
                                              /* done manually by the system manager.          */ 
       char  fleet0;
       short index0;
       char  gen_rpt0_n[33];                  /* A flag for each report to designate whether each */
                                              /* report is to be generated or not during the daily production run.           */
       char  print_rpt0_n[33];                /* A flag for each report to designate whether each report is to be printed or */
       char  fleet1;
       short index1;
       char  gen_rpt1_n[33];                  /* A flag for each report to designate whether each */
                                              /* report is to be generated or not during the daily production run.           */
       char  print_rpt1_n[33];                /* A flag for each report to designate whether each report is to be printed or */
       char  fleet2;
       short index2;
       char  gen_rpt2_n[33];                  /* A flag for each report to designate whether each */
                                              /* report is to be generated or not during the daily production run.           */
       char  print_rpt2_n[33];                /* A flag for each report to designate whether each report is to be printed or */
       char  fleet3;
       short index3;
       char  gen_rpt3_n[33];                  /* A flag for each report to designate whether each */
                                              /* report is to be generated or not during the daily production run.           */
       char  print_rpt3_n[33];                /* A flag for each report to designate whether each report is to be printed or */
       char  fleet4;
       short index4;
       char  gen_rpt4_n[33];                  /* A flag for each report to designate whether each */
                                              /* report is to be generated or not during the daily production run.           */
       char  print_rpt4_n[33];                /* A flag for each report to designate whether each report is to be printed or */
       char  fleet5;
       short index5;
       char  gen_rpt5_n[33];                  /* A flag for each report to designate whether each */
                                              /* report is to be generated or not during the daily production run.           */
       char  print_rpt5_n[33];                /* A flag for each report to designate whether each report is to be printed or */
       char  fleet6;
       short index6;
       char  gen_rpt6_n[33];                  /* A flag for each report to designate whether each */
                                              /* report is to be generated or not during the daily production run.           */
       char  print_rpt6_n[33];                /* A flag for each report to designate whether each report is to be printed or */
       char  fleet7;
       short index7;
       char  gen_rpt7_n[33];                  /* A flag for each report to designate whether each */
                                              /* report is to be generated or not during the daily production run.           */
       char  print_rpt7_n[33];                /* A flag for each report to designate whether each report is to be printed or */
                                              /* just saved on disk during the daily production run. */
       short keep_data_for_n_days;            /* How many days to keep data */
       char  send_mail;
       char  system_mgr[16];
       short keep_cust_days;
} SYSCONTROL_REC;
#ifdef KEY_SRC
#endif
#endif
