static char sz__FILE__[]="@(#)geo_sub2.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  geo_sub2.c; Rel: 5.17.0.0; Get date: 8/10/90 at 08:44:11
 * @(#)  Last delta: 8/9/90 at 13:08:03
 * @(#)  SCCS File: /taxi/sccs/s.geo_sub2.c
 *                                                                           *
 *****************************************************************************/

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <isam.h>
#include <cursesX.h>
#include <signal.h>
#include <setjmp.h>

#include "geo_m_strdefs.h"
#include "mad_error.h"
#include "mads_types.h"
#include "mad_ipc.h"
#include "ui_ipc.h"
#include "df_maint.h"
#include "mads_isam.h"
#include "ex_key.h"
#include "path.h"
#include "geo_scr.h"
#include "geo_msg.h"
#include "tokenize.h"
#include "language.h"

extern int do_jmp;              /* flag indicating longjmp processing   */
extern jmp_buf jmp_env;         /* stored environment for longjmp       */

extern int upper_case;
extern int doing_add;			/* prompt in use */
extern int ret_val;		/* return value */
extern int piu;			/* prompt in use */
extern int intr_allowed;	/* interrupts allowed */
extern int key_val;		/* value returned from the check routines */
extern int key_desc();		/* value returned from the check routines */
extern char read_buf[];		/* buffer for reading fields */
int end_read_field;		/* flag, 0/1, set to end read_field function */
int error_flag;
extern int field_entered;	/* field entered or not */
extern int saw_intr;		/* indicates an interrupt signal was entered */
extern int cur_list;		/* indicates a row is in the current list */
extern int (*check_field[])();	/* Array of pointers to routines to validate
				   the data input by the user */
extern int record_nbr;		/* C_ISAM record number */
extern char o_file[];		/* output file as specified by the user */
extern char mode[];		/* user specified Append or Create */	

extern struct scr_flds geo_fld[];
extern struct df_maint df_maint_ipc;	/* structure for passing information 
				   	   to dispatch  */
extern struct cisam_sn *sn_ptr;	/* pointer to street name file structure */
extern struct cisam_sn *temp_sn_ptr;
extern struct cisam_sn sn_buf;	

extern struct cisam_pl *pl_ptr;	/* pointer to place name file structure */
extern struct cisam_pl *temp_pl_ptr;
extern struct cisam_pl pl_buf;	

extern struct cisam_ad *ad_ptr;	/* pointer to address  file structure */
extern struct cisam_ad *temp_ad_ptr;
extern struct cisam_ad ad_buf;	


/******************************************************************************
* prt_labels - print the field labels on the screen.
*******************************************************************************/

void prt_labels()
{
int   i;

	/* print the screen labels  */
mvaddstr(1,0,
catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_116,"[                  ]  "));
attrset(A_BOLD);
mvaddstr(2,0,
catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_71, "Street Name            Type                                            Abbreviation                    Aliases                 "));
attrset(0);
mvaddstr(3,0,
catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_117,"[                    ] [   ]                                           [                    ]          [                    ]  "));
mvaddstr(4,0,
catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_118,"                                                                       [                    ]          [                    ]  "));
mvaddstr(5,0,
"																");
attrset(A_BOLD);
mvaddstr(6,0,
catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_72, " Place Name          Pre Post City O/E Str #  Suf Zip  Subzone   Comment                        X Coord   Y Coord  Map Pg / Blk  Loc"));
attrset(0);
for(i=7;i<10;i++)
mvaddstr(i,0,
catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_119,"[                    ][  ][  ][   ][ ][      ][ ][     ][     ][                              ][        ][        ] [   ] /[    ][ ]"));
attrset(A_BOLD);
mvaddstr(11,0,
catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_73, "Ranges   Pre  Post  City O/E  Low #    High #   Zip    Subzone   Comment                        X Coord   Y Coord   Map Pg / Blk"));
attrset(0);
for(i=12;i<21;i++)
mvaddstr(i,0,
catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_120,"         [  ] [  ] [   ] [ ] [      ] [      ] [     ] [     ] [                              ][        ][        ] [   ] / [    ]"));
		refresh();
}


prt_data()
{
}

/******************************************************************************
* process_input - input data fields.
******************************************************************************/

process_input()
{
}  /*  End of process_input()  */

/******************************************************************************
* prt_scr - read the screen and output characters to a file.
*******************************************************************************/

prt_scr(o_file_fp)
FILE *o_file_fp;
{

	int col, line, c;

#ifdef DEBUG
	sprintf(trace_str,"Entering prt_scr\n");
	TRACE(trace_str);
#endif

	col = 0;
	for ( line = 0; line < LINES ; line++)  {
		for ( col = 0; col < COLS ; col++ )  {
			move(line,col);
			c = inch();
			fputc(c,o_file_fp);	
		}
		if(c != '\n')
			fputc('\n',o_file_fp);
	}
#ifdef DEBUG
	sprintf(trace_str,"Leaving prt_scr\n");
	TRACE(trace_str);
#endif
	return;

}  /*  End of prt_scr() */


/******************************************************************************
* send_ipc - send a message to dispatch that a record in the zone file changed.
*******************************************************************************/

send_ipc()
{
	int qid, key = MSGKEY;

#ifdef DEBUG
	sprintf(trace_str,"Entering send_ipc\n");
	TRACE(trace_str);
#endif

	df_maint_ipc.d_prcs_id = DISP_DEST;
	df_maint_ipc.u_prcs_id = getpid();
	df_maint_ipc.rec_type = DFM_REQ;		/* !!!!!!!!!!!  */
	df_maint_ipc.which_file = STRNAME_FILE_ID;
	df_maint_ipc.key = sn_ptr->index; 
	df_maint_ipc.cisam_num = isrecnum;         
	if((qid = msgget(key,0666 | IPC_CREAT)) == -1)   {
		prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_74, "Zone file maint msgget failed "));
                prt_unix_err(errno,catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_75, "Zone file maint msgget failed"));
		ERROR(' ',ZONE_FILE,"Zone file maint msgget failed");
		sleep(3);
#ifdef DEBUG
	sprintf(trace_str,"Leaving send_ipc - 1\n");
	TRACE(trace_str);
#endif
		cleanup();
	}

	if((msgsnd(qid,&df_maint_ipc,sizeof(df_maint_ipc), IPC_NOWAIT)) == -1)  {
		prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_76, "Zone file maint msgsnd failed "));
                prt_unix_err(errno,catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_77, "Zone file maint msgsnd failed"));
		ERROR(' ',ZONE_FILE,"Zone file maint msgsnd failed");
		sleep(3);
#ifdef DEBUG
	sprintf(trace_str,"Leaving send_ipc - 2\n");
	TRACE(trace_str);
#endif
		cleanup();
	}
#ifdef DEBUG
	sprintf(trace_str,"Leaving send_ipc - 3\n");
	TRACE(trace_str);
#endif

} /*  End of send_ipc()  */
/******************************************************************************
* read_request - read users request for options.
*******************************************************************************/

char read_request()
{
	/*  The main header, the labels and the data from the first
	 *  record in the zone file have been displayed.  At this
	 *  point the user must request one of the eight options,
	 *  they are : Q, q, N, n, P, p, U, u, O, o, E, and e.
	 */

	int c;		/* For reading characters */

#ifdef DEBUG
	sprintf(trace_str,"Entering read_request\n");
	TRACE(trace_str);
#endif

	move(REQ_ROW,REQ_COL);
	refresh();

	while(c = getkey())  {

	        /* Refresh screen on ^L */
	        if (c == '\014')
		  {
		    wrefresh(curscr);
		    continue;
		  }

	        c = Toupper(c);

		if (c == M_QUERY ||
		    c == M_NEXT ||
		    c == M_ADD ||
		    c == M_PREVIOUS ||
		    c == M_UPDATE ||
		    c == M_REMOVE ||
		    c == M_OUTPUT ||
		    c == M_EXIT)
		    break;
		else  {
			move(REQ_ROW,REQ_COL);
			refresh();
		}
	}

#ifdef DEBUG
	sprintf(trace_str,"Leaving read_request\n");
	TRACE(trace_str);
#endif

	return(c);

} /*  End of read_request() */

/******************************************************************************
* prt_query_hdr - print the header line for the query option.
*******************************************************************************/

prt_query_hdr()
{
	int i;		/* loop index */

#ifdef DEBUG
	sprintf(trace_str,"Entering prt_qurey_hdr\n");
	TRACE(trace_str);
#endif
	clear_hdr_line();

	/* Print the Query screen header */

	mvprintw(HDR_ROW_1,HDR_COL_1,catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_78, "QUERY: ESC executes. INTERRUPT aborts. "));
	refresh();


#ifdef DEBUG
	sprintf(trace_str,"Leaving prt_qurey_hdr\n");
	TRACE(trace_str);
#endif
	return;

}  /*  End of prt_query_hdr()  */


/******************************************************************************
* prt_add_hdr - print the header line for the add option.
*******************************************************************************/

prt_add_hdr()
{
#ifdef DEBUG
	sprintf(trace_str,"Entering prt_add_hdr\n");
	TRACE(trace_str);
#endif

	clear_hdr_line();
	mvaddstr(HDR_ROW_1,HDR_COL_1,ADD_HDR);
	refresh();

#ifdef DEBUG
	sprintf(trace_str,"Leaving prt_add_hdr\n");
	TRACE(trace_str);
#endif
	return;

}  /* End of prt_add_hdr() */


/******************************************************************************
* prompt_confirm - confirm that the user wants to remove this record.
*******************************************************************************/

char prompt_confirm()
{
	int c;		/* For reading characters */

#ifdef DEBUG
	sprintf(trace_str,"Entering prompt_confirm\n");
	TRACE(trace_str);
#endif

	clear_hdr_line();
	mvaddstr(DEL_ROW,DEL_COL,DELETE_HDR);
	refresh();

	while(c = getkey())  {
	        c = Toupper(c);
		if(c == YES || c == NO)
			break;
		else  {
			move(DEL_ROW,DEL_COL);
			refresh();
		}
	}

#ifdef DEBUG
	sprintf(trace_str,"Leaving prompt_confirm\n");
	TRACE(trace_str);
#endif

	return(c);

}  /*  End of prompt_confirm()  */

/******************************************************************************
* prt_hdr - print the main screen header.
*******************************************************************************/

void prt_hdr()
{
#ifdef DEBUG
	sprintf(trace_str,"Entering prt_hdr\n");
	TRACE(trace_str);
#endif

	clear_hdr_line();
	mvaddstr(HDR_ROW_1,HDR_COL_1,catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_79, "Query Next Previous Add Update Remove Output Exit"));
	attrset(A_BOLD);
	mvaddstr(HDR_ROW_2,HDR_COL_2,catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_80, "GEO FILE MAINTENANCE"));
	attrset(0);
	refresh();
	
#ifdef DEBUG
	sprintf(trace_str,"Leaving prt_hdr\n");
	TRACE(trace_str);
#endif
	return;

}  /* End of prt_hdr()  */

/******************************************************************************
* prt_upd_hdr - print the header line for the update option.
*******************************************************************************/

prt_upd_hdr()
{
#ifdef DEBUG
	sprintf(trace_str,"Entering prt_upd_hdr\n");
	TRACE(trace_str);
#endif
	clear_hdr_line();
	mvaddstr(HDR_ROW_1,HDR_COL_1,catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_81, "UPDATE: ESC executes. INTERRUPT aborts."));
	refresh();

#ifdef DEBUG
	sprintf(trace_str,"Leaving prt_upd_hdr\n");
	TRACE(trace_str);
#endif
	return;

}  /*  End of prt_upd_hdr() */

/******************************************************************************
* prompt_fname - prompt the user for a file name to output the screen image to.
*******************************************************************************/

prompt_fname()
{

#ifdef DEBUG
	sprintf(trace_str,"Entering prompt_fname\n");
	TRACE(trace_str);
#endif
	
	clear_hdr_line();
	mvaddstr(HDR_ROW_1,HDR_COL_1,catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_82, "Enter output file (default is geo_scr.out): "));
	refresh();
	getstr(o_file);
	if(!strcmp(o_file,""))
		strcpy(o_file,"geo_scr.out");

#ifdef DEBUG
	sprintf(trace_str,"Leaving prompt_fname\n");
	TRACE(trace_str);
#endif
	return;

}  /*  End of prompt_fname()  */

/******************************************************************************
* prompt_mode - prompt the user for file mode Create or Append.
*******************************************************************************/

prompt_mode()
{
	char		answer;
	short		have_valid_response;

#ifdef DEBUG
	sprintf(trace_str,"Entering prompt_mode\n");
	TRACE(trace_str);
#endif

	have_valid_response = 0;
	while(!have_valid_response)
	  {
		clear_hdr_line();
		mvprintw(HDR_ROW_1,HDR_COL_1,"%s %s  %s", 
				catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_83, 
				"Output file"), o_file, 
				catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_84, 
				"Append or Create (default : C) ") );

		getstr(mode);
/*
 *		default to create
 */
		if (!strlen(mode))
			mode[0] = M_CREATE;

		answer = Toupper(mode[0]);

		switch (answer)
	  	  {
			case M_APPEND:
				strcpy(mode,"a");
				have_valid_response = 1;
				break;
			case M_CREATE:
				strcpy(mode,"w");
				have_valid_response = 1;
				break;
			default:
				prt_error(catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, 
					GEO_MAINT_121, 
					"Invalid choice - Please enter (A/C) append or create"));
				break;
	  	  }
	  }


	clear_err_line();

#ifdef DEBUG
	sprintf(trace_str,"Leaving prompt_mode\n");
	TRACE(trace_str);
#endif
	return;

}  /*  End of prompt_mode()  */

/******************************************************************************
* prt_error - print error message.
*******************************************************************************/

prt_error(err_msg)
char *err_msg;			/* error message to be printed */
{
#ifdef DEBUG
	sprintf(trace_str,"Entering prt_error\n");
	TRACE(trace_str);
#endif
	beep();

	clear_err_line();

	/* Set reverse video attributes */
	attrset(A_REVERSE);

	mvprintw(ERROR_ROW,ERROR_COL," %s ",err_msg);
	refresh();

	/* Reset video attributes */

	attrset(0);

#ifdef DEBUG
	sprintf(trace_str,"Leaving prt_error\n");
	TRACE(trace_str);
#endif
	return;

}  /*  End of prt_error()  */

/******************************************************************************
* prt_desc - print description.
*******************************************************************************/
prt_desc(err_msg) 
char  *err_msg;       /* error message to be print */
{
	mvprintw(ERROR_ROW-1,ERROR_COL,"%s",err_msg);
	refresh();
	return;
}



/******************************************************************************
* prt_cisam_err - write the cisam error message to the trace file.
*******************************************************************************/
prt_cisam_err(err_nbr,err_msg)
int err_nbr;			/* C-isam error number associated with this problem */
char *err_msg;			/* Error message associated with this problem */
{
#ifdef DEBUG
	sprintf(trace_str,"Entering prt_cisam_err\n");
	TRACE(trace_str);

	sprintf(trace_str,"%s, ",err_msg);
	TRACE(trace_str);
	sprintf(trace_str,"%s %d\n", catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_85, "C-isam Error Number ="), iserrno);
	TRACE(trace_str);

	sprintf(trace_str,"Leaving prt_cisam_err\n");
	TRACE(trace_str);
#endif
	return;

}  /*  End of prt_cisam_err()  */

/******************************************************************************
* prt_unix_err - write the unix error message to the trace file.
*******************************************************************************/

prt_unix_err(u_err,u_err_msg)
int u_err;			/* unix error number associated with this problem */
char *u_err_msg;		/* Error message associated with this problem */
{
#ifdef DEBUG
	sprintf(trace_str,"Entering prt_unix_err\n");
	TRACE(trace_str);

	sprintf(trace_str,"%s, ",u_err_msg);
	TRACE(trace_str);
	sprintf(trace_str,"%s %d\n", catgets(GEO_MAINT_m_catd, GEO_MAINT_SET, GEO_MAINT_86, "Unix Error Number ="), u_err);
	TRACE(trace_str);

	sprintf(trace_str,"Leaving prt_unix_err\n");
	TRACE(trace_str);
#endif
	return;

}  /*  End of prt_unix_err()  */





/******************************************************************************
* Clear the data fields on the screen. 
*******************************************************************************/

clear_flds(a,b)
int a,b;
{
#ifdef DEBUG
	sprintf(trace_str,"Entering clear_flds\n");
	TRACE(trace_str);
#endif

	while (a <= b)	{
		geo_cl_flds(a); 
		++a;
	}

#ifdef DEBUG
	sprintf(trace_str,"Leaving clear_flds\n");
	TRACE(trace_str);
#endif
}

/******************************************************************************
* clear_err_line - clear the error line on the screen.
*******************************************************************************/

clear_err_line()
{
#ifdef DEBUG
	sprintf(trace_str,"Entering clear_err_line\n");
	TRACE(trace_str);
#endif
	move(ERROR_ROW-1,ERROR_COL);
	clrtoeol();
	refresh();  
	move(ERROR_ROW,ERROR_COL);
	clrtoeol();
	refresh();  
#ifdef DEBUG
	sprintf(trace_str,"Leaving clear_err_line\n");
	TRACE(trace_str);
#endif
	attrset(0);
}

/******************************************************************************
* clear_hdr_line - clear the header line on the screen.
*******************************************************************************/

clear_hdr_line()
{
#ifdef DEBUG
	sprintf(trace_str,"Entering clear_hdr_line\n");
	TRACE(trace_str);
#endif
	move(HDR_ROW_1,HDR_COL_1);
	clrtoeol();
	refresh();
#ifdef DEBUG
	sprintf(trace_str,"Leaving clear_hdr_line\n");
	TRACE(trace_str);
#endif
}

/******************************************************************************
* process_intr - process the interrupt signal.
*******************************************************************************/

process_intr()
{
	/* This routine is called whcn an interrupt signal
	   is received.  A flag is set to indicate that this
	   signal was received.  The flag end_read_field is set
	   so the read_field routine will terminate. */

#ifdef DEBUG
	sprintf(trace_str,"Entering process_intr\n");
	TRACE(trace_str);
#endif
	signal(SIGINT,process_intr);
	if(intr_allowed)  {
		saw_intr = 1;
		end_read_field = 1;
	}
 
        if ( do_jmp ) {
                do_jmp = 0;
                longjmp(jmp_env, 1);
        }

#ifdef DEBUG
	sprintf(trace_str,"Leaving process_intr\n");
	TRACE(trace_str);
#endif
	return;
}
