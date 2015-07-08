static char sz__FILE__[]="@(#)ui_resp.c	10.1.2.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  ui_resp.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:51:29
 * @(#)  Last delta: 12/2/94 at 18:13:28
 * @(#)  SCCS File: /taxi/sccs/s.ui_resp.c
 *                                                                           *
 *****************************************************************************/

#include <cursesX.h>
#include "ui_error.h"
#include "ui_def.h"
#include "path.h"
#include "ui_strdefs.h"

#define		MAX_SPACE		120	/* max count whose response time can be saved for each type of command */
#define		MAX_CMDS		5	/* max # of comands for which response time stats is to be recorded */

char *response_time_msgs[MAX_CMDS] = { NULL };

/* ============================================================================================================================	*
 * ui_resp: contains routines for response time statistics for the user interface program 					*
 * ============================================================================================================================	*/

short response_stat_flag;	/* if set ui generates response time stats */
extern char *getenv();
FILE *response_stat_fp;		/* file pointer for response_stat file */
extern int errno;
extern char *sys_errlist[];
struct response_stats {
	short no_of_cmds;
	short resp_time[MAX_SPACE];
} response_stat[MAX_CMDS];
long resp_from_dt_tm;
extern int uid;
struct formatted_rts {
	int uid;
	char start_time[10];
	char start_date[10];
	char  end_time[10];
	char  end_date[10];
	short no_of_cmds;
	float avg_rsp_time;
	float sigma;
	short longest_rsp_time;
	long time_of_longest_rsp;
};

/*
 * initialize the response messages array
 */

ui_init_response_time_msgs()
{
	response_time_msgs[0] = m_catgets(UI_m_catd, UI_1, UI_RESP_1, "ALL CALLS SEARCH");
	response_time_msgs[1] =	m_catgets(UI_m_catd, UI_1, UI_RESP_2, "ZONING CALL COMMAND");
	response_time_msgs[2] =	m_catgets(UI_m_catd, UI_1, UI_RESP_3, "APPROVE/DISAPPROVE EXCEPTION");
	response_time_msgs[3] =	m_catgets(UI_m_catd, UI_1, UI_RESP_4, "CALL ENTRY");
	response_time_msgs[4] =	m_catgets(UI_m_catd, UI_1, UI_RESP_5, " NOTHING HERE");	/* for future use */
}



/* ============================================================================================================================	*
 * set_response_stat_flag: checks whether the user has defined an environment variable UI_RESPONSE_STATS. If yes then the	*
 * ui generates response time stats, else it does not generate response time stats. This function sets the global flag 		*
 * response_stat_flag to 1 if ui should generate response time stats, else it sets the flag to 0				*
 * ============================================================================================================================	*/
set_response_stat_flag()
{
	
	if ( getenv("UI_RESPONSE_STATS") == NULL )
		response_stat_flag = 0;
	else {
		response_stat_flag = 1;
		if ( ( response_stat_fp = fopen(UI_RESPONSE_STAT_FILE,"a")) == NULL )  {
			/* fopen failed */
			prt_error(UI_OPEN,"%s <%s ... %s>", UI_RESPONSE_STAT_FILE,
					sys_errlist[errno], catgets(UI_m_catd, UI_1, UI_RESP_6, "response time stat will not be reported") );
			response_stat_flag = 0;
		}  /* end if ... fopen */
		resp_from_dt_tm = time(NULL);
	}  /* end else */

}  /* end set_response_stat_flag()  */


/* ============================================================================================================================	*
 * close_resp_stat_file: closes UI_RESPONSE_STAT_FILE 										*
 * ============================================================================================================================	*/
close_resp_stat_file()
{
	if ( response_stat_flag ) {
		response_stat_flag = 0;
		fclose(response_stat_fp);
	}  /* end if */

}	/* end close_resp_stat_file()  */


/* ============================================================================================================================	*
 * bump_resp: increments the count for the response time stat & saves the response time 
 * ============================================================================================================================	*/
bump_resp(cmd_num,start_or_end)
	int cmd_num;
	int start_or_end;	/* START_TIME or END_TIME */
{
	static struct response_stats *rs_ptr;
	static long cmd_start_time;
	static int doing_stat=0;	/* no */

	if ( response_stat_flag  &&  cmd_num >= 0 )
		rs_ptr = &response_stat[cmd_num];
	if ( rs_ptr == (struct response_stats *)0 )
		return;
	if ( response_stat_flag  &&  rs_ptr->no_of_cmds < MAX_SPACE )  {
		if ( start_or_end == START_TIME_CMD )  {
			rs_ptr->no_of_cmds++;
			cmd_start_time =  time(NULL);
			doing_stat = 1;
		}
		else if ( doing_stat )  {
			/* command END_TIME */
			rs_ptr->resp_time[rs_ptr->no_of_cmds-1] = time(NULL) - cmd_start_time;
			doing_stat = 0;
		}
	}	/* end if */

}	/* end bump_resp()  */


/* ============================================================================================================================	*
 * do_resp_time_stats: does processing for response time stats
 * ============================================================================================================================	*/
do_resp_time_stats()
{
	if ( response_stat_flag )  {
		format_resp_time_stats();
		init_resp_time_stats();
	}  /* end if */

}	/* end do_resp_time_stats()  */


/* ============================================================================================================================	*
 * init_resp_time_stat: initializes structures used to store response time stats
 * ============================================================================================================================	*/
init_resp_time_stats()
{
	resp_from_dt_tm = time(NULL);
	memset((char *)&response_stat[0],'\0', sizeof(struct response_stats)*MAX_CMDS);
}	/* end init_resp_time_stat()  */


/* ============================================================================================================================	*
 * format_resp_time_stats: formats the requst to be written to an ASCII file
 * ============================================================================================================================	*/
format_resp_time_stats()
{
	int i, j, longest_index;
	long end_time;

	struct formatted_rts  formatted_output;	

	formatted_output.uid = uid;
	get_arg_date(resp_from_dt_tm,formatted_output.start_date);
	get_arg_time(resp_from_dt_tm,formatted_output.start_time);
	end_time = time(NULL);
	get_arg_date(end_time,formatted_output.end_date);
	get_arg_time(end_time,formatted_output.end_time);
	for ( i = 0; i < MAX_CMDS; i++ ) {
		formatted_output.no_of_cmds = response_stat[i].no_of_cmds;
		formatted_output.longest_rsp_time = response_stat[i].resp_time[0];
		formatted_output.avg_rsp_time = 0.0;
		for ( j = 0; j < response_stat[i].no_of_cmds; j++ )  {
			formatted_output.avg_rsp_time += response_stat[i].resp_time[j];
			/* get logest response time */
			if ( formatted_output.longest_rsp_time  < response_stat[i].resp_time[j] )   {
				formatted_output.longest_rsp_time = response_stat[i].resp_time[j];
				longest_index = j;
			}  /* end if */			
		}  /* end for */
		if ( response_stat[i].no_of_cmds )
			formatted_output.avg_rsp_time = formatted_output.avg_rsp_time/response_stat[i].no_of_cmds;
		else 
			formatted_output.avg_rsp_time = 0.0;
		formatted_output.time_of_longest_rsp = response_stat[i].resp_time[longest_index];

		/* write to file */
		write_resp_time_stats(&formatted_output,i);
	} /* end for */

}	/* end format_resp_time_stats()  */



/* ============================================================================================================================	*
 * write_resp_time_stats: formats the requst to be written to an ASCII file
 * ============================================================================================================================	*/
write_resp_time_stats(formatted_ptr,i)
	struct formatted_rts *formatted_ptr;
	int i;
{
	char print_buf[400];


	sprintf(print_buf, "%s: %d  %s: %s   %s: %s  %s: %s  %s: %s\n\t%s: %s\n\t%s : %d\n\t%s: %f\n\t%s: %d\n\t%s : %d\n\n",
	catgets(UI_m_catd, UI_1, UI_RESP_7, "USER ID"), formatted_ptr->uid, catgets(UI_m_catd, UI_1, UI_RESP_8, "START DATE"), formatted_ptr->start_date, catgets(UI_m_catd, UI_1, UI_RESP_9, "START TIME"), formatted_ptr->start_time,
		catgets(UI_m_catd, UI_1, UI_RESP_10, "END DATE"), formatted_ptr->end_date, catgets(UI_m_catd, UI_1, UI_RESP_11, "END TIME"), formatted_ptr->end_time, catgets(UI_m_catd, UI_1, UI_RESP_12, "NAME OF COMMAND"), 
		response_time_msgs[i], catgets(UI_m_catd, UI_1, UI_RESP_13, "NO OF COMMANDS"), formatted_ptr->no_of_cmds, catgets(UI_m_catd, UI_1, UI_RESP_14, "AVERAGE RESPONSE TIME"), 
		formatted_ptr->avg_rsp_time, catgets(UI_m_catd, UI_1, UI_RESP_15, "LONGEST RESPONSE TIME"), formatted_ptr->longest_rsp_time,
		catgets(UI_m_catd, UI_1, UI_RESP_16, "TIME OF LONGEST RESPONSE"), formatted_ptr->time_of_longest_rsp);

	fputs(print_buf,response_stat_fp);

}	/* end write_resp_time_stats()  */

