static char sz__FILE__[]="@(#)da_credit.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  da_credit.c; Rel: 6.4.0.0; Get date: 4/10/92 at 10:06:02
 * @(#)  Last delta: 11/14/90 at 10:03:02
 * @(#)  SCCS File: /taxi/sccs/s.da_credit.c
 *                                                                           *
 * Copyright (c) 1990 - MDI Seattle                                          *
 *                                                                           *
 *****************************************************************************/

/* da_credit.c
   Generate a credit transaction report
   Doug Rudof 6-8-90 */

#include <stdio.h>
#include <sys/types.h>
#include <time.h>

#include "path.h"
#include "language.h"
#include "rep_strdefs.h"
#include "credit.h"
#include "acc.h"
#include "msg_mmp.h"

extern char *get_rp_name();
#define MAX_REPORT_LINE 180
#define SECONDS_DAY 86400

da_credit (fleet_id, start_date, start_time, start_dt_tm,
	   end_date, end_time, ending_dt_tm, fleet_name)

char fleet_id;
char *start_date;
char *start_time;
long start_dt_tm;
char *end_date;
char *end_time;
long ending_dt_tm;
char *fleet_name;

{
    extern struct tm  *localtime();
    extern int sys_day_end;
    long  seconds;
    int trans_start;
    FILE  *rep_fp;
    FILE  *ct_fp;
    long  trans_dt_tm;
#ifdef TEMP
    int   vehid, drivid, callno;
    char  trans_date[20], trans_time[10], response[15];
    char  expiration[10], amount[10], account[20];
    char  sys[100];
#endif
    char  path_name[80];
    char  label[MAX_REPORT_LINE + 1];
    char  ct_buf[MAX_REPORT_LINE + 1];
    char  cr_trans_file_name[80];
    struct tm   *tm_ptr;
    long  nowtime,day_1,day_2;
    int   page_number;
    char  *report_name;
    char  *line_seperator = "===================================================================================================================================";

    page_number = 1;
    report_name = get_rp_name(fleet_id, 20, end_date);
  
    sprintf(path_name, "%s/%s", REPORT_DATA, report_name);

    if ((rep_fp = fopen(path_name, "w")) == NULL) {
       fprintf(stderr, "%s %s%s\n", catgets(REP_catd, 13, 1, "Can't write to file:"), REPORT_DATA, report_name);
       return(-1);
    }

    seconds = time( (long *) 0);			/* current date and time in seconds */

    report_heading(seconds, start_date, start_time, end_date, 
                   end_time, fleet_name, 
                   catgets(REP_catd, 13, 2, "CREDIT CARD TRANSACTION REPORT"), 
                   &page_number, rep_fp);

    strcpy(label, catgets(REP_catd, 13, 3, " DATE    TIME   TYPE      ACCOUNT      EXP   AMOUNT   RESPONSE   VEHID DRIVID       CALL #"));

    fprintf(rep_fp, "%s\n%s\n%s\n", line_seperator, label, line_seperator);

    nowtime = (long)time((long *)0);
    day_1   = nowtime - SECONDS_DAY;
    day_2   = day_1 - SECONDS_DAY;
    tm_ptr = localtime(&day_2);

    sprintf(cr_trans_file_name,"%s.%02d%02d%02d", CREDIT_TRANSACTION_FILE, tm_ptr->tm_year % 100,
	    tm_ptr->tm_mon+1,tm_ptr->tm_mday);

    /****************************************************************/
    /* Check the first  "credit_trans.XXXXXX" file that was broken at mid- */
    /* night.  Need to exclude those transactions that took place   */
    /* prior to the start of the shift.                             */
    /****************************************************************/
    if ((ct_fp = fopen (cr_trans_file_name, "r")) != NULL) 
    {
       trans_start = 0;
       while (fgets(ct_buf, MAX_REPORT_LINE, ct_fp) != NULL)
        {
           sscanf(ct_buf,"%ld",&trans_dt_tm);
           if (trans_start == 0)
           {
              while(ct_buf[trans_start++] != ' ');
              if (trans_start < 9)
                 trans_start = 9;         /* safety check */
           }
	  if ((trans_dt_tm > start_dt_tm) && (trans_dt_tm < ending_dt_tm))
              fprintf(rep_fp, "%s", &ct_buf[trans_start]);
       }  
       fclose(ct_fp);
    }

    /*************************************************************/
    /* Check the second "credit_trans.XXXXXX" file that was      */
    /* broken at midnight. Same criteria for inclusion as above. */
    /*************************************************************/
    tm_ptr = localtime(&day_1);
    sprintf(cr_trans_file_name,"%s.%02d%02d%02d", CREDIT_TRANSACTION_FILE, tm_ptr->tm_year % 100,
            tm_ptr->tm_mon+1, tm_ptr->tm_mday);
    if ((ct_fp = fopen (cr_trans_file_name, "r")) != NULL)
    {
       trans_start = 0;
       while (fgets(ct_buf, MAX_REPORT_LINE, ct_fp) != NULL)
       {
           sscanf(ct_buf,"%ld",&trans_dt_tm);
           if (trans_start == 0)
           {
               while(ct_buf[trans_start++] != ' ');
               if (trans_start < 9)
                  trans_start = 9;
           }
           if ((trans_dt_tm > start_dt_tm) && (trans_dt_tm < ending_dt_tm))
	      fprintf(rep_fp, "%s", &ct_buf[trans_start]);
        }
        fclose(ct_fp);
     }

    /*************************************************************/
    /* Check the "credit_trans" file for transactions that took  */
    /* place from midnight to the end of the shift.              */
    /************************************************************/
    if ((ct_fp = fopen (CREDIT_TRANSACTION_FILE, "r")) != NULL)
    {
       trans_start = 0;
       while (fgets(ct_buf, MAX_REPORT_LINE, ct_fp) != NULL)
       {
	   sscanf(ct_buf,"%ld",&trans_dt_tm);
	   if (trans_start == 0)
	   {
	      while(ct_buf[trans_start++] != ' ');
	      if (trans_start < 9)
		 trans_start = 9;
	   }

	   if (trans_dt_tm < ending_dt_tm) 
	      fprintf(rep_fp, "%s", &ct_buf[trans_start]);
       }
       fclose(ct_fp);
     }	


  
    fprintf(rep_fp, "%s", line_seperator);
    fclose(rep_fp);
}
