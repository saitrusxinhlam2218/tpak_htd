static char sz__FILE__[]="@(#)ui_excepts.c	10.1.2.4(ver) Date Release 5/24/95" ;
/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  ui_excepts.c; Rel: 6.17.0.0; Get date: 5/2/91 at 14:46:39
* @(#)  Last delta: 3/28/91 at 10:03:27
* @(#)  SCCS File: /taxi/sccs/s.ui_excepts.c
*                                                                           *
* Copyright (c) 1990 - MDI Seattle                                          *
*                                                                           *
*****************************************************************************/

/* Based on R3 version 5.20 */

/* Make sure location constants are defined. Create a compile error if they
   are not defined or both are defined. */
#ifdef SCANDINAVIA
#ifdef NAMERICA
ERROR: "Both compile time constants SCANDINAVIA and NAMERICA are defined."
#endif
#else
#ifdef NAMERICA
#else
ERROR: "Compile time constant SCANDINAVIA or NAMERICA undefined."
#endif
#endif

#include <cursesX.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <memory.h>
#include <ctype.h>
#include <string.h>
#include <signal.h>

#include "taxipak.h"
#include "mem_mgmt.h"
#include "Object.h"
#include "ui_def.h"
#include "menu.h"
#include "screens.h"
#include "mads_types.h"
#include "mads_isam.h"
#include "calls.h"
#include "except.h"
#include "user.h"
#include "mad_ipc.h"
#include "df_maint.h"
#include "call_entry.h"
#include "ui_error.h"
#include "func_keys.h"
#include "ui_ipc.h"
#include "writer.h"
#include "mesg.h"
#include "language.h"
#include "ui_strdefs.h"
#include "enhance.h"
#include "switch_ext.h"
#include "semops.h"

							/* define macro to map into the array of error messages */
#define		SYS_ERR_MAP_INDEX(array_index)		((array_index - BISYNC_ERR) / 10)

#define		VALUE_TO_WAIT_FOR	0	/* for semaphore lock */
#define		PLACE_LOCK		1	/* to place read lock on exceptions structure in shared mem using semop() */
#define		RELEASE_LOCK		-1	/* to release read lock on exceptions structure in shared mem using semop() */
#define		FL_EXCEPT_COL		4 	/* col for printing the fleet */
#define		STATUS_EXCEPT_COL	8	/* status printing col */
#define		DUE_EXCEPT_COL		18	/* due time col */
#define		TYPE_EXCEPT_COL		24	/* call type col */
#define		VEHNO_EXCEPT_COL	30	/* veh# col */
#define		ZONE_EXCEPT_COL		35	/* zone # col */
#define		CITY_EXCEPT_COL		39	/* pkup city col */
#define		STNO_EXCEPT_CL		43	/* street # col */
#define		PREDIR_EXCEPT_COL	51	/* pre dir col */
#define		STNAME_EXCEPT_COL	54	/* street name col */
#define		STTYPE_EXCEPT_COL	75	/* street type col */
#define		POSTDIR_EXCEPT_COL	79	/* street post dir col */
#define		PASS_EXCEPT_COL		85	/* passenger col */
#define		PLACE_EXCEPT_COL	106	/* place/address comment col */
#define         RC_EXCEPT_COL           35      /* radio channel display */
#define		LINE_NO_ROW		3	/* row of "Line" prompt */
#define		LINE_NO_COL		9	/* col of "Line" prompt */
#define		EXCEPT_LINE_CHARS	9	/* max chars for Line field */
#define		SEARCHES_ON_PG		17	/* # of summary lines on multi_line screens */
#define		FORWARD			1	/* search forwards .. next pg */
#define		BACKWARD		0	/* search BACKWARD ...prev. pg */
#define		NOT_AN_EXCEPTION	0	/* if exception nbr = 0, its not an exception */
#define		LENGTH_SUM_DESC		133	/* length of sum desc */
#define		START_SUM_ROW		6	/* line # of the 1st summary line */
#define		PG_NO_COL		128	/* to print the page # */
#define		CHARS_VEH		4	/* # of char's for the vehicle # */
#define		OLD_DISPLAY		2	/* for displaying the previous screen */
#define		MAX_EXCEPT_ZONES	16	/* # of zones/superzones the user can select for exception selection */

/********************************************************************************/
/* ui_excepts.c: contains routines for exceptions processing 			*/
/********************************************************************************/

extern int scr_name;					/* name of screen on display */
extern int map_array[];					/* to map scr_name into index into menus struct */
extern int uid;						/* users id # */
extern struct menus *cmenu[];				/* array of pointers to menus struct in shared  memory */
extern char req_buf[];
extern int prev_scr_flag;				/* set if PREV_SCR key is pressed */
extern short end_exceptions;				/* flag for ending exceptions.. define in ui_ftnkey() */
extern struct cisam_cl srch_calls[];                    /* for storing calls rec from disk */
extern int scr_name;					/* name of screen on display */
extern short clear_err_flag;				/* set if last line error should be cleared */
struct excepts *saved_except_ptr[SEARCHES_ON_PG];	/* for saving as prev scr */
extern struct keydesc cl_key1,zn_key1,veh_key2,em_key1, dr_key1;
extern struct offsets *offset;				/* pointer to offsets struct in shared memory */
extern int calls_on_page;
char *sum_ptr[SEARCHES_ON_PG];				/* searches on pg */
char sum_desc[SEARCHES_ON_PG*LENGTH_SUM_DESC];		/* buffer for summary desc */

int   mult_refresh = FALSE;
							/* declare pointers to field checking routines */
int 	ck_except_dummy(), ck_except_line(), ck_except_zone_veh();
int 	(*ck_except_fld[])() = {ck_except_dummy, ck_except_line, ck_except_zone_veh};

extern int fifo_fd;					/* file descriptor for FIFO */
extern struct users *uiuser[];				/* array of pointers to users struct in shared memory */
extern int user_index;					/* index/slot used by this user in users struct in shared memory */
extern int pid;						/* process id */
short resolved_except[SEARCHES_ON_PG];			/* 1=exception resolved; 0=not resolved */
short end_man_zone=0;					/* flag for ending man_zone_mode */
short end_man_veh=0;					/* flag for ending man_zone_mode */
short pg_no;						/* exception screens pg # */
extern int semid;					/* semaphore id */
short msg_on_display=0;					/* set if a message window is on display */
extern char canmtxt[MAX_CANM+1][41];			/* array of message strings for canned msg #'s */
extern struct except_bits selected_except;		/* for saving default exception selection criteria */
struct except_bits temp_selected_except;		/* temporary exception selection criteria */
extern struct recv_msg *message_read();
extern struct fleets *fleet[];				/* array of pointers to fleets struct in shared memory */
extern char *ui_syserr[];
extern char excpt_fl[];					/* fleets selected for exception processing */
int mesgs();
extern int mult_call;
extern int msgkey;

extern CALL_IPC_REC 	call_fifo;	/* for call rec to FIFO */
extern CALL_REC	call_struct;
extern	int	own_isrecnum;
extern char *Spaces;
extern int New_screen_from_command;

WINDOW *mesg_wind = NULL;
void create_message_window();
void delete_message_window();

/*********************************************************************************/
/* exceptions: processes the exceptions screen					 */
/*********************************************************************************/
exceptions()
{
	int got_excepts = 0;				/* # of exceptions that appear on the screen */
	int i;						/* scratch variable */
	int piu;					/* field in use */
	static struct scr_flds except_fld[] = { 
		{0,0,1,1},
		{LINE_NO_ROW,LINE_NO_COL,EXCEPT_LINE_CHARS,1}
	};
	int key_val;					/* return code from read_field() */
	char read_buf[READ_BUF_SIZE];			/* for reading input */
	int field_entered;				/* whether field entered or not */
	int ret_ck_fld;					/* return code from check_except_line_no() */
	struct excepts *except_ptr[SEARCHES_ON_PG];	/* pointers to 17 exception slots */
	int except_highlighted = 0;			/* # of the exception line highlighted */
	int should_clear = 0;


	/* initialize so that no exceptions are resolved */
	for(i=0; i < SEARCHES_ON_PG; i++ )
		resolved_except[i] = 0;

	/* display the exceptions screen with labels and no data */
	disp_except_scr();

	/* initialize array of srch_calls[] structures */
	for(i = 0; i < SEARCHES_ON_PG; i++) 
		memset((char *)&srch_calls[i], '\0', sizeof(struct cisam_cl));
	
	/* might want to initialize pointers to scr_flds */

	/* if called due to pressing PREV_SCR key, then try to restore previous state */
	if(prev_scr_flag) {
		/* initilialize exception selection criteria */
		temp_selected_except = selected_except;
		prev_scr_flag = 0;
		pg_no = pg_no;						/* use old value */

		for(i = 0; i < SEARCHES_ON_PG; i++) {
			except_ptr[i] = saved_except_ptr[i];
			saved_except_ptr[i] = (struct excepts *)0;
		}

		got_excepts = get_excepts(NO,OLD_DISPLAY,except_ptr);
		calls_on_page = got_excepts;
		if (got_excepts == 0) {
			prt_error(UI_NOEXCEPTS,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_1, "<meeting selection criteria>")); 	/* did not find any exceptions */
			pg_no = 1;
		}
		else 
			disp_except_sum(got_excepts,except_ptr); 	/* did find some exceptions; so display the summary */
	}
	else {
		/* called directly; need to display the first page of the exceptions screen */
		 /* set old temporary exception criteria */
		temp_selected_except = selected_except;
		pg_no = 1;

		got_excepts = get_excepts(YES,FORWARD,except_ptr);
		calls_on_page = got_excepts;
		if (got_excepts == 0) {
			prt_error(UI_NOEXCEPTS,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_1, "<meeting selection criteria>")); 	/* did not find any exceptions */
			pg_no = 1;
	        }
		else 
			disp_except_sum(got_excepts,except_ptr); 	/* did find some exceptions; so display the summary */
	}

	/* at this point, either some exceptions are on display or no  exceptions are on display; place the cursor on the line 
	 * prompt and wait for input
	 */
	end_exceptions = 0;
	piu = 1;	/* go to line prompt */
	while(!end_exceptions && scr_name == EXCEPTIONS_SCR )  {
		check_mail();
		attrset(A_REVERSE);
		clear_field(stdscr,except_fld,piu);	/* clear old entry on line # field */
		attrset(0);

		key_val = read_field(stdscr,except_fld,piu,read_buf,&field_entered,&should_clear);
		
		
		if(key_val == -1)
		  continue;			 /* read_field() timed out */
		
		/* check if an error msg on the last line should be cleared */
		if(clear_err_flag)
		  clear_error();
		/* check and process the field entered by the user */
		
		ret_ck_fld = check_except_line_no(except_fld,&piu,read_buf,key_val,&field_entered,
						  &got_excepts,&except_highlighted,except_ptr);
		if(ret_ck_fld == NOT_DONE)
		  {
		    /* processing needs to be completed by the function key handlier */
		    ftnkey_handler(&piu,key_val,0);
		  }

		/* Redisplay the message that might have been overwritten */
		if (msg_on_display && mesg_wind)
		  {
		    touchwin(mesg_wind);
		    wrefresh(mesg_wind);
		  }
	}

	/* save the pointers for previous screen access */
	for(i = 0; i < SEARCHES_ON_PG; i++)
		saved_except_ptr[i] = except_ptr[i];


	/* Make sure the message window goes away */
	delete_message_window();
}

/* ============================================================================================================================	*
 * disp_except_scr: 	displays the exceptions screen with labels only and no data *
 * ============================================================================================================================	*/
disp_except_scr()
{
	int indx;	/* index into menus struct in shared memory */
	int len;	/* length of the screen title description */
	int start_col;	/* for printing the centered screen title */


	/* clear the previous screen */
	clear();

	attrset(A_REVERSE);
	mvaddstr(0,0, Spaces);

	prt_date();	/* print the date */

	/* print the screen title */
	indx = map_array[scr_name];
	/* need to center the title on the terminals screen */
	len = strlen(cmenu[indx]->mf_desc);
	start_col = (COLS - len - 1)/2;
	mvprintw(0,start_col,"%s",cmenu[indx]->mf_desc);
	mvprintw(0,start_col+len+3,"(%d)",cmenu[indx]->mf_command);

	/* print the user's id */
	mvprintw(0,UID_COL,"%d",uid);

	/* print the 2nd & the 3rd line in reverse bold */
	attron(A_BOLD);
	mvaddstr(1,0,Spaces);
	mvaddstr(2,0,Spaces);
	mvaddstr(3,0,Spaces);
	mvaddstr(4,0,Spaces);
	attroff(A_BOLD);

	/* print the 6th line */
	mvaddstr(5,0,Spaces);

	/* print the labels */
	mvaddstr(3,1,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_2, " Line:   "));
	mvaddstr(3,DUE_EXCEPT_COL-1,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_3, " Due  Disp'd Veh# Pickup Close  Pri    Date     Attr "));

	/* print labels on the 5th line */
	mvaddstr(5,FL_EXCEPT_COL,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_4, "Fl"));
	mvaddstr(5,STATUS_EXCEPT_COL-1,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_5, "Status"));
	mvaddstr(5,DUE_EXCEPT_COL-2,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_6, "Due"));
	mvaddstr(5,TYPE_EXCEPT_COL,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_7, "Type"));
	mvaddstr(5,VEHNO_EXCEPT_COL,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_8, "Veh#"));
	
	if ( !(0) )
	  {
	    mvaddstr(5,ZONE_EXCEPT_COL,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_9, "Zon"));
	    mvaddstr(5,CITY_EXCEPT_COL,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_10, "Cty"));
	    mvaddstr(5,STNAME_EXCEPT_COL-1,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_11, "Pickup Address"));
	    mvaddstr(5,PASS_EXCEPT_COL,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_12, "Passenger"));
	    mvaddstr(5,PLACE_EXCEPT_COL,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_13, "Place/Address Comment"));
	  }
	else
	  {
	    mvaddstr(5,RC_EXCEPT_COL, "Chn");
	    mvaddstr(5,ZONE_EXCEPT_COL+4,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_9, "Zon"));
	    mvaddstr(5,CITY_EXCEPT_COL+4,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_10, "Cty"));
	    mvaddstr(5,STNAME_EXCEPT_COL-1+4,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_11, "Pickup Address"));
	    mvaddstr(5,PASS_EXCEPT_COL+1,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_12, "Passenger"));
	    mvaddstr(5,PLACE_EXCEPT_COL,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_13, "Place/Address Comment"));
	  }

	attrset(0);

	refresh();	  /* screen will be painted and blank if semaphore lock is present */
}

/* get_except_call_rec: reads a call record from the database using 
 * the cisam rec # or the call #. If record is read successfully, then
 * 1 is returned, else 0 is returned
 */
int 
get_except_call_rec(walk_except_ptr)
struct excepts *walk_except_ptr;	/* pointer to except slot for which call rec is to be read */
{
	struct cisam_cl *temp_cl_ptr;	/* for req_buf[] */
	int cisam_num;	/* cisam rec # of the call rec */

	/* call_ptr is NULL, try to read record by call # */
	temp_cl_ptr = (struct cisam_cl *)req_buf;
	temp_cl_ptr->nbr = walk_except_ptr->call_nbr;

		if(db(CALL_FILE_ID, READ_KEYS, &cl_key1, ISEQUAL, 0) < 0)               /* db failed, no record found */
		return(0);

	/* data is saved in req_buf */
	return(1);
}

/*******************************************************************************************************************************/
/* get_excepts: selects 17 (or less) exceptions from the shared memory; reads appropriate records from the disk.               */
/* Returns: # of exceptions found											       */
/********************************************************************************************************************************/
int get_excepts(first_time,srch_dir,ptr_ptr)
char first_time;		/*YES = display 1st 17 exceptions, else NO */
int srch_dir;			/* whether FORWARD or BACKWARD */
struct excepts **ptr_ptr;	/* pointer to the 1st pointer in the array of pointers to the exceptions slots in shared memory */
{
	int i;					/* scratch variable */
	int got_excepts;			/* # of exceptions found */
	struct excepts *walk_except_ptr;	/* pointer to walk thru exceptions slots */
	int pi;					/* index for except slot pointers */
	int j;					/* scratch variable */



	walk_except_ptr = (struct excepts *)0;			/* initialize pointers, etc */
	
	SetReadLock(semid);        /* place read lock on the exceptions structure in shared memory */

	if (first_time == YES) {				/* the first page of the exceptions screen needs to be displayed, */
		walk_except_ptr = offset->first_except;		/* so start from the first exception */
		for (i = 0; i < SEARCHES_ON_PG; i++ )		/* set pointers to NULL's */
			*(ptr_ptr + i) = (struct excepts *)0;
	}
	else {							/* this is not first_time, get the pointer to walk thru the list */
		if (srch_dir == OLD_DISPLAY) {			/* invoked as a prev screen */
			if (*ptr_ptr == (struct excepts *)0) {	/* get the last excpt slot pointer which still points to good exc*/
				walk_except_ptr = offset->first_except;		/* no exceptions were on display, when previous */
				pg_no = 1;			/* screen was saved; so start from the 1st exception */
			}
			else {					/* get the last excpt pointer which still points to valid excpt */
				pi = SEARCHES_ON_PG-1;
				while (pi >= 0) {
					if (*(ptr_ptr+pi) == (struct excepts *)0) {
						pi--;
						continue;
					}
					else  {				/* pointer points to a slot; check if exception is valid */
						if ((*(ptr_ptr + pi))->nbr != (long)NOT_AN_EXCEPTION) {
							walk_except_ptr = *(ptr_ptr + pi );		/* got a valid exception */
							break;						/* leave while loop */
						}
						else			/* exception slot pointed is no longer a valid exception */
							pi--;
					}
				}
				if (walk_except_ptr == (struct excepts *)0) {
					walk_except_ptr = offset->first_except;			/* start from the fisrt exception */
					pg_no = 1;
				}
			}
			srch_dir = BACKWARD;
		}
		else if(srch_dir == FORWARD) {		/* check pointers which were assigned, last time this function was called;*/
			for (pi = SEARCHES_ON_PG-1; pi > 0; pi--)				/*then initialize walk_except_ptr */
				if(*(ptr_ptr + pi) != (struct excepts *)0) 
					break;

			if(*(ptr_ptr+pi) == (struct excepts *)0) {				/* check if full page is blank */
				ReleaseReadLock(semid);              /* relese semaphore lock */
				return(0);
			}

			for (i = pi; i > 0 ; i--) {	/* start from the last position which still points to a valid exception */
				if ((*(ptr_ptr + i))->nbr != (long)NOT_AN_EXCEPTION) 
					break;							/* got to a valid exception slot */
			}

			if ((*(ptr_ptr+i))->nbr == (long)NOT_AN_EXCEPTION)		/*check if have any valid exception slots */
				walk_except_ptr = offset->first_except;			/* start from the first exception */
			else								/* exception slot got valid exception */
				walk_except_ptr = (*(ptr_ptr+i))->next;
		}
		else {						/* srch_dir == BACKWARD; try to get the previous 17 exceptions */
			if (*ptr_ptr == (struct excepts *)0)	/* no exceptions are on display; start from the first exception */
				walk_except_ptr = offset->first_except;
			else if((*ptr_ptr)->prev == (struct excepts *)0) {		/* 1st exception pg is already on display */
				ReleaseReadLock(semid);      /* release semaphore lock */
				return(0);						/* return # of exceptions found */
			}
			else {			/* get the fisrt exception slot pointer which still points to a valid exception */
				 pi = 0;
				 while (pi < SEARCHES_ON_PG) {
					if(*(ptr_ptr+pi) == (struct excepts *)0) {	/* no more exception slot pointed to */
						walk_except_ptr = offset->first_except;	
						break;					/* leave while loop */
					}
					if((*(ptr_ptr + pi))->nbr != (long)NOT_AN_EXCEPTION) {	/* pointing to a valid exception?*/
						walk_except_ptr = (*(ptr_ptr+pi))->prev;	/* got to a valid exception */
						break;						/* leave while loop */
					}
					pi++;
				}

				if (walk_except_ptr == (struct excepts *)0)		/* make sure walk_except_ptr is assigned */
					walk_except_ptr = offset->first_except;
			}
		}
	}


	if (srch_dir == FORWARD) {				/* now try to get the next or previous 17 (or less) exceptions */
		got_excepts = 0;				/* got none so far */
		pi = 0;                                         /* index for exceptions slot pointers */
		while (got_excepts < SEARCHES_ON_PG && walk_except_ptr != (struct excepts *)0) { /* loop until get 17 recsor end*/
			if (except_matches(walk_except_ptr)) {
				*(ptr_ptr + pi) = walk_except_ptr;				/* assign exception slot pointer */
				got_excepts++;				/* we got an exception, so try to get the next exception */
				pi++;
			}
			walk_except_ptr = walk_except_ptr->next;	/* go to next exception slot */
		}

		if(got_excepts) 					/* if found new exceptions */
			for (i=pi; i < SEARCHES_ON_PG; i++)		/* for the remainder of 17 - # of exceptions found */
				*(ptr_ptr + i) = (struct excepts *)0;	/* set pointers to NULL */

		ReleaseReadLock(semid);      /* release the semaphore lock */
	}
	else {					/* srch_dir == BACKWARD; we have the starting slot pointed to by walk_except_ptr */
		got_excepts = 0;		/* note that the first exception slot's previous pointer is a null pointer */
		pi = SEARCHES_ON_PG - 1;	/* so as to walk backwards */
		while (got_excepts < SEARCHES_ON_PG && walk_except_ptr != (struct excepts *)0) {
			if (except_matches(walk_except_ptr)) {			/* meets all the criteria ? */
				*(ptr_ptr + pi) = walk_except_ptr;		/* assign exception slot pointer */
				got_excepts++;				/* we got an exception, so try to get the next exception */
				pi--;
			}
			walk_except_ptr = walk_except_ptr->prev;	/* go to previous exception slot */
		}
		if (got_excepts ==0)
		  pg_no = 1;
		else if (got_excepts < SEARCHES_ON_PG) {
			pg_no = 2;					/* must be first page NOTE: a -- is done by caller */
			j = 0;						/* try tp get 17 exceptioons, from the fist exception */
			for (i=SEARCHES_ON_PG-got_excepts; i < SEARCHES_ON_PG; i++) {
				*(ptr_ptr + j) = *(ptr_ptr + i);
				j++;
			}
			walk_except_ptr = (*(ptr_ptr + j - 1 ))->next;	/* get the rest of the exceptions to make it 17 */
			pi = got_excepts;
			while (got_excepts < SEARCHES_ON_PG && walk_except_ptr != (struct excepts *)0) {
				if (except_matches(walk_except_ptr)) {
					*(ptr_ptr + pi) = walk_except_ptr;		/* assign exception slot pointer */
					got_excepts++;			/* we got an exception, so try to get the next exception */
					pi++;
				}
				walk_except_ptr = walk_except_ptr->next;		/* go to the next exception slot */
			}
		}
#if 0
A previous page should never return less than a pageful. Problem occurred here when searching for exception of one
group and doing a previous page; here we would set the exceptions to NULL

		if (got_excepts < SEARCHES_ON_PG) {				/* is less then 17 exceptions */
			for (i = got_excepts; i < SEARCHES_ON_PG; i++)		/* NULL out rest of slots */
				*(ptr_ptr + i) = (struct excepts *)0;
		}
#endif
		ReleaseReadLock(semid);              /* release the semaphore lock */
	}


	for (i = 0; i < got_excepts; i++) {					/* read call records from disk */
		walk_except_ptr = *(ptr_ptr + i);				/* set pointer to exception to be displayed */
		if (walk_except_ptr->call_ptr != (struct calls *)0 || walk_except_ptr->call_nbr > (long)0) {
			if (get_except_call_rec(walk_except_ptr)) {
				memcpy((char *)&srch_calls[i],req_buf,sizeof(struct cisam_cl));
				if (walk_except_ptr->type == REQ_TO_TLK || walk_except_ptr->type == SYS_ERR){
					if (walk_except_ptr->veh_nbr != srch_calls[i].veh_nbr){
						if (!((0) && (walk_except_ptr->info >= NO_ACK_MDT_1)
									    && (walk_except_ptr->info <= NO_ACK_ITM_3)))
						{
							SetReadLock(semid);                                     /* place semaphore lock */
							walk_except_ptr->call_ptr = NULL;                       /* not on this call */
							walk_except_ptr->call_nbr = (long) 0;                   /* anymore */
							ReleaseReadLock(semid);

						}
					}
				}
			}
			else {                                                                          /* cant read call rec */
				SetReadLock(semid);                                                     /* place semaphore lock */
				walk_except_ptr->call_ptr = NULL;					/* wrong call */
				walk_except_ptr->call_nbr = (long) 0;		
				ReleaseReadLock(semid);                                                 /* release semaphore lock */
			}
		}
	}
	return(got_excepts);

}

/********************************************************************************************************************************/
/* except_matches: checks if the exception slot pointed to by the argumnet pointer conatins an exception which meets the users  */
/* exception criteria. 														*/
/* Returns 0 if criteria not met, 1 if criteria is met										*/
/********************************************************************************************************************************/
except_matches(walk_except_ptr)
struct excepts *walk_except_ptr;			/* pointer to the exception slot */
{
	int i;
	int select_fleet = 0;				/* fllet is the right one or not */
	int select_zone = 0;				/* zone or superzone is the right one or not */
	int select_type = 0;				/* exception type is the right one or not */
	int select_channel = 0;	

							/* note that excpt_fl[0] should be set to default fleet in fill_user()  */

	for (i = 0; i < FLEET_MAX; i++) {		/* check if this exception belongs to fleet the user wants to work with */
		if (walk_except_ptr->fl_ptr != NULL && walk_except_ptr->fl_ptr->fleet_id != excpt_fl[i] )
			continue;
		else  {					/* user wants to work with this fleet exceptions */
			select_fleet = 1;
			break;
		}
	}

	if (!select_fleet)				/* this exception belongs to a fleet on which user does not wish to work */
		return(0);

	for (i = 0; i < MAX_EXCEPT_ZONES; i++) {
		if ((walk_except_ptr->zone_nbr != 0) && (walk_except_ptr->zone_nbr == uiuser[user_index]->zones_sel[i])) {
			select_zone = 1;		/* user selected this zone */
			break;
		}
		if ((walk_except_ptr->super_zone != 0) && (walk_except_ptr->super_zone == uiuser[user_index]->zones_sel[i])) {
			select_zone = 1;		/* user selected this superzone */
			break;
		}
	}

#ifdef NAMERICA
	if ( glEnhSwitch & ENH0016 )
	  {
	    if ( (walk_except_ptr->radio_channel < 0) || ( walk_except_ptr->radio_channel > 18 ) )
	      select_channel = 1;
	    else
	      {
		if (uiuser[user_index]->channels[walk_except_ptr->radio_channel] == 1)
		  select_channel = 1;
		else
		  select_channel = 0;
	      }
	  }
#endif
	
	if (!select_zone && (walk_except_ptr->super_zone == 0))
		select_zone = 1;			/* this exception does not belong to a superzone, display to dispatcher */

	if (test_bit((walk_except_ptr->type-1),sizeof(struct except_bits),(unsigned char *)&temp_selected_except))  
		select_type = 1;			/* user has selected this exception type */

	if ((0) && (walk_except_ptr->type >= SP_EXCEPT_START))
		return(1);                              /* All users should see SamPlan exceptions */

	if (!select_type)
		return(0);                              /* user did not select this exception type */


	if (0)
	  {
	    if (select_fleet && select_zone && select_type && select_channel)
	      return(1);				/* user gets to see this one */
	    else
	      return(0);
	  }
	else
	  {
	    if (select_fleet && select_zone && select_type && walk_except_ptr->inuse != '1')
	      return(1);				/* user gets to see this one */
	    else
	      return(0);
	  }
}

/* ============================================================================================================================	*
 * disp_except_sum: displays the exception summary on the screen 
 * Who: Naveed Cochinwala	What: Written		When: 1988
 * ============================================================================================================================	*/
disp_except_sum(got_excepts,except_ptr)
int got_excepts;				/* # of excepts found */
struct excepts *except_ptr[];			/* array of exception slot pointers */
{
	int i,j, k, msg_num, jj;
	char 	temp_comment[41];		/* buffer for comment */
	char 	temp_fleet;			/* temporary fleet holder */
	char 	temp_zone_nbr[4];		/* temporary zone holder */
	char 	temp_veh_nbr[5];		/* temporary vehicle number holder */
	char    temp_channel_nbr[3];
	char 	exdesc[9];			/* description of exception from shared memory */
	struct 	excpt_desc *exptr;		/* pointer to exception desc slot */
	char 	call_except_num;		/* '*' = exception has more than 1 exception; ' ' = call has 1 exception */
	char	*last_six();
	CHANNEL_ENTRY *channel_ptr;
	int     done;	

	memset(sum_desc, ' ',SEARCHES_ON_PG*LENGTH_SUM_DESC);
	for(i=0;i < SEARCHES_ON_PG;i++) {
		sum_ptr[i] = &sum_desc[i*LENGTH_SUM_DESC];
		sum_desc[(i*LENGTH_SUM_DESC)+(LENGTH_SUM_DESC-1)] = '\0';
	}

	for(i=0; i < got_excepts; i++)  { 			/* for each of the exception, save the summary desc in sum_desc[] */
		if(except_ptr[i] == (struct excepts *)0) 
			break; 					/* got to the last exception meeting criteria */

		if(except_ptr[i]->call_ptr != (struct calls *)0 || except_ptr[i]->call_nbr > (long)0 ){ 	/* with call ? */
			if(except_ptr[i]->type == ETA) 							/* is it ETA ? */
				sprintf(temp_comment,"%ld %s", except_ptr[i]->info, catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_14, "MINUTES") ); 		
			else if(except_ptr[i]->type == MESSAGE)  { 		/* desc of canned msg in the place/address field */
				if(except_ptr[i]->info < 0 || except_ptr[i]->info > MAX_CANM) 
					strcpy(temp_comment,"  ");  		/* invalid canned msg # in exception slot */
                                else
                                {                                               /* valid canned msg # */
        				if (canmtxt[(int)except_ptr[i]->info][0] == NULL)
                                        {
					        sprintf(temp_comment,"%s %d",
                                                catgets(UI_m_catd,UI_1,UI_UI_MESG_15, "Canned mesg #"),
						(int)except_ptr[i]->info);
                                                for (jj=0; jj < strlen(temp_comment); jj++)
                                                        temp_comment[jj]= Toupper(temp_comment[jj]);
                                        }
					else
                                        {   
                                                strncpy(temp_comment,&canmtxt[(int)except_ptr[i]->info][0],40);
                                                temp_comment[40] = '\0';
                                        }

                                }

			}
			else if(except_ptr[i]->type == SYS_ERR) {		/* system error message should be printed ? */
				msg_num = SYS_ERR_MAP_INDEX((int) except_ptr[i]->info);
				if ((msg_num >= 0) && (msg_num < SYS_ERR_MAX))
					strcpy(temp_comment,ui_syserr[SYS_ERR_MAP_INDEX((int)except_ptr[i]->info)]);
				else 
					temp_comment[0]='\0';
			}
			else
				strncpy(temp_comment,srch_calls[i].pckup_adr_cmnt,20);

			temp_comment[20] = '\0';

			exptr = offset->except_desc + except_ptr[i]->type - 1; 		/* get the description of this exception */
			strcpy(exdesc, exptr->short_desc);

			if(srch_calls[i].pckup_zone)
				sprintf(temp_zone_nbr,"%-03d",srch_calls[i].pckup_zone);
			else
				sprintf(temp_zone_nbr,"   ");

			if(except_ptr[i]->type == MANUAL)		/* MANUAL always print vehicle nbr */
				sprintf(temp_veh_nbr,"%-03d", except_ptr[i]->veh_nbr);
			else if(srch_calls[i].veh_nbr)
				sprintf(temp_veh_nbr,"%-03d",srch_calls[i].veh_nbr);
			else
				sprintf(temp_veh_nbr,"   ");

			if(call_with_more_excepts(except_ptr[i]))
				call_except_num = '*';	/* call has more than 1 exception */
			else 
				call_except_num = ' ';	/* call has 1 exception */

#ifdef NAMERICA
			if ( glEnhSwitch & ENH0016 )
			  {
			    /*** Hooks into shared memory to get radio channel code to display **/
			    k = 0;
			    done = 0;
			    channel_ptr = &(offset->channels[k]);
			    while ((k < 18) && (done == 0))
			      {
				if (channel_ptr->channel_nbr == except_ptr[i]->radio_channel)
				  {
				    sprintf(temp_channel_nbr, "%-03s", channel_ptr->channel_short_descript);
				    done = 1;
				  }
				k++;
				channel_ptr = &(offset->channels[k]);
			      }
			    if (done == 0) 
			      strcpy(temp_channel_nbr, "***");
			    else
			      {
				if (except_ptr[i]->call_ptr != NULL)
				  if (except_ptr[i]->call_ptr->veh_ptr == NULL)
				    strcpy(temp_channel_nbr, "***");
			      }
			  }
#endif

			if ( !(0) )
			  {
#ifdef SCANDINAVIA
				temp_comment[18] = '\0';
				if (0)
					sprintf(sum_ptr[i],
					" %-2d %c  %-8s%c%-6s  %-5s %-4s %-3s %-3s %20s %-6d               %-20s %-18s  %6s",
					i+1, srch_calls[i].fleet, exdesc, call_except_num, srch_calls[i].due_time,
					srch_calls[i].extended_type, temp_veh_nbr, temp_zone_nbr, srch_calls[i].pckup_city,
					srch_calls[i].pckup_str_name, srch_calls[i].pckup_str_nbr,srch_calls[i].passenger_name,
					temp_comment, last_six(srch_calls[i].nbr));
				else
				  {
				    if ( ( except_ptr[i]->type == MULT ) && ( srch_calls[i].grouped_with > 0 ) )
					sprintf(sum_ptr[i],
					" %-2d %c #%-8s%c%-6s  %-5s %-4s %-3s %-3s %20s %-6d%c              %-20s %-18s  %6s",
					i+1, srch_calls[i].fleet, exdesc, call_except_num, srch_calls[i].due_time,
					srch_calls[i].extended_type, temp_veh_nbr, temp_zone_nbr, srch_calls[i].pckup_city,
					srch_calls[i].pckup_str_name, srch_calls[i].pckup_str_nbr, srch_calls[i].pckup_str_suf,
					srch_calls[i].passenger_name, temp_comment, last_six(srch_calls[i].nbr));
				    else
				      sprintf(sum_ptr[i],
					" %-2d %c  %-8s%c%-6s  %-5s %-4s %-3s %-3s %20s %-6d%c              %-20s %-18s  %6s",
					i+1, srch_calls[i].fleet, exdesc, call_except_num, srch_calls[i].due_time,
					srch_calls[i].extended_type, temp_veh_nbr, temp_zone_nbr, srch_calls[i].pckup_city,
					srch_calls[i].pckup_str_name, srch_calls[i].pckup_str_nbr, srch_calls[i].pckup_str_suf,
					srch_calls[i].passenger_name, temp_comment, last_six(srch_calls[i].nbr));
				  }
#else
				temp_comment[18] = '\0';
				sprintf(sum_ptr[i],
					" %-2d %c  %-8s%c%-6s  %-5s %-4s %-3s %-3s %-6d%c %-2s %-20s %-3s %-2s    %-20s %-20s%6s",
					i+1, srch_calls[i].fleet, exdesc, call_except_num, srch_calls[i].due_time, 
					srch_calls[i].extended_type, temp_veh_nbr, temp_zone_nbr, srch_calls[i].pckup_city,
					srch_calls[i].pckup_str_nbr, srch_calls[i].pckup_str_suf, srch_calls[i].pckup_pre_dir,
					srch_calls[i].pckup_str_name, srch_calls[i].pckup_str_type, srch_calls[i].pckup_post_dir, 
					srch_calls[i].passenger_name, temp_comment, last_six(srch_calls[i].nbr));
#endif
			      }
			else
			  {
			    sprintf(sum_ptr[i],
				    " %-2d %c  %-8s%c%-6s  %-5s %-4s %-3s %-3s %-3s %-6d%c %-2s %-20s %-3s %-2s    %-20s %-16s%6s",
				    i+1, srch_calls[i].fleet, exdesc, call_except_num, srch_calls[i].due_time, 
				    srch_calls[i].extended_type, temp_veh_nbr, temp_channel_nbr, temp_zone_nbr, srch_calls[i].pckup_city,
				    srch_calls[i].pckup_str_nbr, srch_calls[i].pckup_str_suf, srch_calls[i].pckup_pre_dir,
				    srch_calls[i].pckup_str_name, srch_calls[i].pckup_str_type, srch_calls[i].pckup_post_dir, 
				    srch_calls[i].passenger_name, temp_comment, last_six(srch_calls[i].nbr));
			  }

			sum_ptr[i][strlen(sum_ptr[i])] = ' ';	/* strip off null char since we want a 132 byte string */
			sum_ptr[i][COLS] = '\0';
		}
		else { 								/* exception not assocaited with call */
			exptr = offset->except_desc + except_ptr[i]->type - 1; 	/* get the description of this exception */
			strcpy(exdesc, exptr->short_desc);
			if(except_ptr[i]->type == SYS_ERR)  {               /* system error message should be printed
? */                                msg_num = SYS_ERR_MAP_INDEX((int) except_ptr[i]->info); 
                                if ((msg_num >= 0) && (msg_num < SYS_ERR_MAX)) 
                                        strcpy(temp_comment,ui_syserr[SYS_ERR_MAP_INDEX((int)except_ptr[i]->info)]);
                                else  
                                        temp_comment[0]='\0';
                        } 
			else if(except_ptr[i]->type == ETA) 							/* is it ETA ? */
				sprintf(temp_comment,"%ld %s", except_ptr[i]->info, catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_14, "MINUTES") ); 		
			else if(except_ptr[i]->type == MESSAGE)  { 		/* desc of canned msg in the place/address field */
				if(except_ptr[i]->info < 0 || except_ptr[i]->info > MAX_CANM) 
					strcpy(temp_comment,"  ");  		/* invalid canned msg # in exception slot */
				else
				{
        				if (canmtxt[(int)except_ptr[i]->info][0] == NULL)
                                        {
					        sprintf(temp_comment,"%s %d",
                                                catgets(UI_m_catd,UI_1,UI_UI_MESG_15, "Canned mesg #"),
						(int)except_ptr[i]->info);
                                                for (jj=0; jj < strlen(temp_comment); jj++)
                                                        temp_comment[jj]= Toupper(temp_comment[jj]);
                                        }
					else
                                        {   
                                                strncpy(temp_comment,&canmtxt[(int)except_ptr[i]->info][0],40);
                                                temp_comment[40] = '\0';
                                        }

                                }

			}
			else 
				temp_comment[0] = '\0';

			if (except_ptr[i]->fl_ptr == NULL)
				temp_fleet = ' ';
			else
				temp_fleet = except_ptr[i]->fl_ptr->fleet_id;

			if(except_ptr[i]->veh_nbr)
				sprintf(temp_veh_nbr,"%-03d",except_ptr[i]->veh_nbr);
			else
				sprintf(temp_veh_nbr,"   ");

#ifdef NAMERICA
			if (0)
			  {
			    /*** Hooks into shared memory to get radio channel code to display **/
			    k = 0;
			    done = 0;
			    channel_ptr = &(offset->channels[k]);
			    while ((k < 18) && (done == 0))
			      {
				if (channel_ptr->channel_nbr == except_ptr[i]->radio_channel)
				  {
				    sprintf(temp_channel_nbr, "%-03s", channel_ptr->channel_short_descript);
				    done = 1;
				  }
				k++;
				channel_ptr = &(offset->channels[k]);
			      }
			    if (done == 0)
			      strcpy(temp_channel_nbr, "***");
			  }
#endif
			if (0)
			  {
			    sprintf(sum_ptr[i],
				    " %-2d %c  %-8s %-6s  %-4s  %-4s %-3s                                               %-40s ",
				    i+1, temp_fleet, exdesc, " ", " ", temp_veh_nbr,temp_channel_nbr, temp_comment);
			  }
			else
			  {
			    sprintf(sum_ptr[i],
				    " %-2d %c  %-8s %-6s  %-4s  %-4s                                                   %-40s ", 
				    i+1, temp_fleet, exdesc, " ", " ", temp_veh_nbr,temp_comment);
			    
			    sum_ptr[i][strlen(sum_ptr[i])] = ' ';	/* strip off null char since we want a 132 byte string */
			    sum_ptr[i][COLS] = '\0';
			  }
		}
	}

	for(i=0; i < got_excepts; i++) 					/* print the summary on the screen */
		mvprintw(START_SUM_ROW+i,0,"%s",sum_ptr[i]);

	attrset(A_REVERSE); 						/* print the page # */
	mvprintw(5,PG_NO_COL,"%-2d",pg_no);
	attrset(0);
}

/* ============================================================================================================================	*
 * change_line_attr: changes the old line the new line # attrbutes as defined by the arguments of the new attributes passed
 * ============================================================================================================================	*/
change_line_attr(old_num,old_line_attr,new_num,new_line_attr)
int 	old_num;		/* line # of the old line */
unsigned int 	old_line_attr;	/* new attribute of the old line */
int 	new_num;		/* new line # */
unsigned int 	new_line_attr;	/* new attr of the new line */
{
	if(old_num != 0) { 				/* reprint the old line */
		if(resolved_except[old_num-1])
			attrset(A_REVERSE); 		/* this exception has been resolved; so print it in dim reverse mode */
		else 
			attrset(old_line_attr); 	/* exception has not been resolved; so print it in normal mode */
		mvaddstr(START_SUM_ROW+old_num-1,0,sum_ptr[old_num-1]);
		refresh();				/* refresh req'd because of a curses bug */
	}

	/* reprint the new line */
	attrset(new_line_attr); 
	mvaddstr(START_SUM_ROW+new_num-1,0,sum_ptr[new_num-1]);

	attrset(0); 					/* reset attrbutes to normal mode */
}

/****************************************************************************************************************/
/* check_except_line_no: processes the input typed on the "Line:" prompt of the exceptions screen 		*/
/*	Returns   -  DONE - if processing is completed								*/
/*		  -  NOT_DONE - if processing is not completed							*/
/****************************************************************************************************************/
int 
check_except_line_no(ptr_scr_fld, piu_ptr, read_buf, key_val, field_entered, got_excepts, except_highlighted, ptr_ptr)
struct scr_flds 	*ptr_scr_fld;				/* pointer to start of piu struct used */
int 	*piu_ptr;						/* pointer to piu value */
char 	*read_buf;						/* points to buffer with input */
int 	key_val;						/* key value entered by user */
int 	*field_entered;						/* 0=field not entered, else 1 */
int 	*got_excepts;						/* # of exception lines on display */
int 	*except_highlighted;					/* # of the exception line highlighted */
struct excepts 	**ptr_ptr;
{
	char 	*ptr;						/* to walk thru read_buf */
	int 	line_num;					/* line # entered by user */
	short 	i;	
	struct scr_flds 	*ptr_to_fld;	
	int 	temp_int;					/* # of exceptions for next pg /prev pg */
	char 	s_or_i;						/* Exception Group ('S') or Exception Type ('I') entered */
	int update_call_history;	
	int	choice;


	ptr_to_fld = ptr_scr_fld + (*piu_ptr);

	if(*field_entered) {					/* field entered */
		ptr = read_buf;
		if(*ptr == EXC_GRP_SEL || *ptr == EXC_TYPE_SEL) {	/* user entered S, S<HELP> or S.... */
			s_or_i = *ptr;
			ptr++;
			if(*ptr == '\0') {			/* user entered an S or S<HELP> */
				if(key_val == HELP_KEY) {
					/* display help window with the listing of exception groups and group numbers */
					/* help(FIELD_NUM.....);  */
				}
				else {	/* user entered 'S'; go back to default exception selection criteria; display exceptions */
					temp_selected_except = selected_except;
					pg_no = 1;				/* clear summary area; and fill it with new info */
					move(START_SUM_ROW,0);
					clrtobot();
					*except_highlighted = 0;		/* since new start */
					for(i=0; i < SEARCHES_ON_PG; i++)	/* set all exceptions to not resolved */
						resolved_except[i] = 0;
					clear_detail_area();			/* clear the detail area */

					if(!(*got_excepts = get_excepts(YES,FORWARD,ptr_ptr))) 
						prt_error(UI_NOEXCEPTS,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_1, "<meeting selection criteria>"));	/* no exceptions found */
					else 
						disp_except_sum(*got_excepts,ptr_ptr);		/* got some exceptions */
				}
			}
			else  {				/* user entered some group #'s for the new exception selection criteria; */
				if(reset_sel_crit(s_or_i,ptr) == 0)  
					return(DONE);					/* error in entering new crit; do nothing */

				pg_no = 1;				/* clear the summary area; and fill it with new info */
				move(START_SUM_ROW,0);
				clrtobot();
				*except_highlighted = 0;		/* since new start */
				for(i=0; i < SEARCHES_ON_PG; i++)	/* set all exceptions to not resloved */
					resolved_except[i] = 0;
				clear_detail_area();			/* clear the detail area */

				if(!(*got_excepts = get_excepts(YES,FORWARD,ptr_ptr))) 
					prt_error(UI_NOEXCEPTS,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_1, "<meeting selection criteria>"));		/* no exceptions found */
				else 
					disp_except_sum(*got_excepts,ptr_ptr);				/* got some exceptions */
			}
			return(DONE);
	 	}

		/* check if user entered a line # */
		i = 0;
		while(i < ptr_to_fld->max_chars && read_buf[i] != '\0')
		  {
		    if(!isdigit(read_buf[i]))
		      {
			prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_15, "<digits only>"));
			return(DONE);
		      }
		    i++;
		  }

		if((line_num = atoi(read_buf)) <= 0 || line_num > *got_excepts)  {
			prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_16, "<invalid line # entered>"));		 /* invalid line # entered */
			return(DONE);
		}

		/* valid line # entered;  if exception number highlighted is other
		 * than the # entered, then display the detail of that exception and move the highlight
		 */
		if(line_num != *except_highlighted) {
			change_line_attr(*except_highlighted,0,line_num,A_REVERSE|A_BOLD);
			*except_highlighted = line_num;
			disp_except_detail(ptr_ptr,*except_highlighted); 		/* display the detail */
		}
		/* check if user had entered a function key or just the entry with <CR> */
		if(key_val == OK || key_val == '\r' || key_val == '\n')
			return(DONE);
		else 
			return(NOT_DONE);	/* user entered a function key */
	}
	else  {


		/* field not entered; user might have entered <CR>, etc, or a function key */
		switch(key_val) {
			case '\n':
			case '\r':
			case KEY_DOWN:
				/* move highlight down by 1 line */
				if(*except_highlighted <= 0 || *except_highlighted > *got_excepts) {
					prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_17,
								     "<no exception is highlighted >"));
					return(DONE);
				}

				/* an exception is highlighted; move highlight */
				line_num = (*except_highlighted %(*got_excepts)) + 1;
				change_line_attr(*except_highlighted,0,line_num,A_BOLD|A_REVERSE);
				*except_highlighted = line_num;

				disp_except_detail(ptr_ptr,*except_highlighted); 	/* display the detail of the new line */
				return(DONE);
			
			case KEY_UP:
				/* move highlight up by 1 line */
				if(*except_highlighted <= 0 || *except_highlighted > *got_excepts) {
					prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_17,
								     "<no exception is highlighted >"));
					return(DONE);
				}

				/* an exception is highlighted; move highlight */
				line_num = *except_highlighted - 1;
				if(line_num == 0)
					line_num = *got_excepts;

				change_line_attr(*except_highlighted,0,line_num,A_BOLD|A_REVERSE);
				*except_highlighted = line_num;

				/* display the detail of the new line */
				disp_except_detail(ptr_ptr,*except_highlighted); 
				return(DONE);
			
			case NEXT_PG_KEY:				/* try to display the previous 17 exceptions */
				if((temp_int = get_excepts(NO,FORWARD,ptr_ptr)) == 0) {
					prt_error(UI_NOEXCEPTS,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_18,
								       "<no exceptions meeting selection criteria>"));
					return(DONE);
				}
				else {						/* got some excepts */
					pg_no++;
					*got_excepts = temp_int;

					/* Erase unused slots in srch_calls */
					for(i = *got_excepts; i < SEARCHES_ON_PG; i++) 
					  memset((char *)&srch_calls[i], '\0', sizeof(struct cisam_cl));

					move(START_SUM_ROW,0);
					clrtobot();				/* clear the summary area */
					*except_highlighted = 0;		/* since prev pg */
					for(i=0; i<SEARCHES_ON_PG; i++)		/* set all exceptions to not resloved */
						resolved_except[i] = 0;
					clear_detail_area();			/* clear the detail area */
					disp_except_sum(*got_excepts,ptr_ptr);
				}
				return(DONE);

			case PREV_PG_KEY:				/* try to display the previous 17 exceptions */
				if((temp_int = get_excepts(NO,BACKWARD,ptr_ptr)) == 0) {
					prt_error(UI_NOEXCEPTS,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_18,
								       "<no exceptions meeting selection criteria>"));
					pg_no = 1;			/* got to 1st pg */
					return(DONE);
				}
				else {						/* got some excepts */
					pg_no--;
					*got_excepts = temp_int;
					move(START_SUM_ROW,0);
					clrtobot();				/* clear the summary area */
					*except_highlighted = 0;		/* since prev pg */
					for (i=0; i<SEARCHES_ON_PG; i++)	/* set all exceptions to not resloved */
						resolved_except[i] = 0;
					clear_detail_area();			/* clear the detail area */
					disp_except_sum(*got_excepts,ptr_ptr);
				}
				return(DONE);

			case REFRESH_KEY:			/* clear the summary area; and fill it with new info */
				if (msg_on_display) {		/* check if message window should be displayed or erased */
					msg_on_display = 0;	/* there is no message window on display */
					delete_message_window();
				}
				pg_no = 1;
				move(START_SUM_ROW,0);
				clrtobot();
				*except_highlighted = 0;			/* since new start */
				for (i=0; i < SEARCHES_ON_PG; i++)		/* set all exceptions to not resloved */
					resolved_except[i] = 0;
				clear_detail_area();				/* clear the detail area */

				/* initialize array of srch_calls[] structures */
				for(i = 0; i < SEARCHES_ON_PG; i++) 
					memset((char *)&srch_calls[i], '\0', sizeof(struct cisam_cl));

				if(!(*got_excepts = get_excepts(YES,FORWARD,ptr_ptr))) 
				  /* no exceptions found */
				  prt_error(UI_NOEXCEPTS,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_1,
								 "<meeting selection criteria>"));
				else 
					disp_except_sum(*got_excepts,ptr_ptr);				/* got some exceptions */
				return(DONE);

			case HISTORY_KEY: 	/* display call history record(s) for the call/exception which is highlighted */
				if(*except_highlighted <= 0)  { 		/* check if any detail is available on display */
					prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_19,
								     "<no exception highlighted>"));
					return(DONE);
				}
					/* this key should only be allowed for exceptions which have a call associated with it */
				if (srch_calls[*except_highlighted - 1].nbr == 0) {
					prt_error(UI_NOFIELD,
						  catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_20,
							  "<call history is allowed only for exceptions associated with a call>"));
					return(DONE);
				}
				call_history(srch_calls[*except_highlighted-1].nbr);	/* exception is with a call */
				return(DONE);

			case MANZONE_KEY:
				if(*except_highlighted <= 0)  { 		/* check if any detail is available on display */
					prt_error(UI_NOFIELD,catgets(UI_m_catd,UI_1,UI_UI_EXCEPTS_19,"<no exception highlighted>"));
					return(DONE);
				}
				if ((*(ptr_ptr + *except_highlighted -1))->type == ITM_INVOICE_NAK)
				  {
				    prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_FTNKEY_1,
								 "<wrong function key pressed>"));
				    return(DONE);
				  }

/*
 *                              fix for multiple calls
 *                                      manual zone assignment not allowed
 */
                                if ((*(ptr_ptr + *except_highlighted -1))->type == MULT)
                                  {
                                        prt_error(UI_NOT_ALLOWED,"<not allowed to zone multiple calls>");
                                        return(DONE);
                                  }

				if(uiuser[user_index]->can_zone_calls != YES) { 			/* can user zone calls */
					prt_error(UI_NOT_ALLOWED,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_21, "<to zone calls>"));
					return(DONE);
				}
				manual_zone_veh(except_highlighted,read_buf,ptr_ptr,got_excepts,C_ZONE); /* remain in this mode */
				return(DONE);
			
			case MANVEH_KEY:
				/* check if any detail is available on display */
				if(*except_highlighted <= 0)  {
					prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_19,
								     "<no exception highlighted>"));
					return(DONE);
				}
				if ((*(ptr_ptr + *except_highlighted -1))->type == ITM_INVOICE_NAK)
				  {
				    prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_FTNKEY_1,
								 "<wrong function key pressed>"));
				    return(DONE);
				  }

				/* remain in this mode until mode ends */
				manual_zone_veh(except_highlighted,read_buf,ptr_ptr,got_excepts,C_VEHICLE);
				return(DONE);

			case APPROVE_KEY:
				/* check if any detail is available on display */
				if(*except_highlighted <= 0)  {
					prt_error(UI_NOFIELD,catgets(UI_m_catd,UI_1,UI_UI_EXCEPTS_19,
								     "<no exception highlighted>"));
					return(DONE);
				}
				/* check type of exception being resolved and process accordingly */
				switch((*(ptr_ptr + *except_highlighted -1))->type)  {
					case ETA:
					case MESSAGE:
					case REQ_TO_TLK:
						if (((*(ptr_ptr + *except_highlighted -1))->call_nbr == 0) &&
						   ((*(ptr_ptr + *except_highlighted -1))->call_ptr == NULL))
							update_call_history = 0;
						else 
							update_call_history = 1;

						resolve_except(ptr_ptr,(*except_highlighted-1),0,0,update_call_history,
								R_APPROVE,ACT_APPROVE,
					 			(*(ptr_ptr + *except_highlighted -1))->type ); 
						break;
                                        case MISSING_RECEIPT_RETRY_LIMIT:
					case CALL_TIMEOUT:
					case LATE_METER:
					case SHORT_METER:
					case CALLBACK:
					case CALLOUT:
					case REJECT:
					case FLAG_REQ:
					case BID:
					case SUP_HOLD:
					case EXTRA_TAXIS:
					case CALL_NO_MSG:
					case HOL_TM_CALL:
					case NO_SHOW:
				case NS_SHORT:
					case CALL_W_MSG:
                                        case MULT:
					case PERSONAL:
					case MANUAL:	/* manual exception, when cab's kdt is not working */
						/* update call history record, and write request to FIFO ;
						 do not update call record */
						resolve_except(ptr_ptr,(*except_highlighted-1),0,0,1,R_APPROVE,ACT_APPROVE,
					 		(*(ptr_ptr + *except_highlighted -1))->type );

						if ( (*(ptr_ptr + *except_highlighted -1))->type == MULT )
						  {
						    mult_refresh = TRUE;
						    sleep(1);
						    if (msg_on_display) {		/* check if message window should be displayed or erased */
						      msg_on_display = 0;	/* there is no message window on display */
						      delete_message_window();
						    }
						    pg_no = 1;
						    move(START_SUM_ROW,0);
						    clrtobot();
						    *except_highlighted = 0;			/* since new start */
						    for (i=0; i < SEARCHES_ON_PG; i++)		/* set all exceptions to not resloved */
						      resolved_except[i] = 0;
						    clear_detail_area();				/* clear the detail area */
						    
						    /* initialize array of srch_calls[] structures */
						    for(i = 0; i < SEARCHES_ON_PG; i++) 
						      memset((char *)&srch_calls[i], '\0', sizeof(struct cisam_cl));
						    
						    if(!(*got_excepts = get_excepts(YES,FORWARD,ptr_ptr))) 
						      /* no exceptions found */
						      prt_error(UI_NOEXCEPTS,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_1,
										     "<meeting selection criteria>"));
						    else 
						      disp_except_sum(*got_excepts,ptr_ptr);				/* got some exceptions */
						  }
						break;

					case SYS_ERR:   /* most system errors; do not update call or callh rec */
						if (((0) && ((*(ptr_ptr + *except_highlighted-1))->info >= NO_ACK_MDT_1)
									    && ((*(ptr_ptr + *except_highlighted-1))->info <= NO_ACK_ITM_3))
						   || ((0) && ((*(ptr_ptr + *except_highlighted-1))->info == ASSIGN_ERR)))

						{
							/* update the call history record for NACKs of call messages
							   or nack of call assign */
							resolve_except(ptr_ptr,(*except_highlighted-1),0,0,1,R_APPROVE,ACT_APPROVE,
								(*(ptr_ptr + *except_highlighted -1))->type );
							break;

						}
					case EMERGENCY:
					case MFAULT:
						resolve_except(ptr_ptr,(*except_highlighted-1),0,0,0,R_APPROVE,ACT_APPROVE,
							(*(ptr_ptr + *except_highlighted -1))->type );
						break;

					case SP_EXCPT_01 :
					case SP_EXCPT_02 :
					case SP_EXCPT_03 :
					case SP_EXCPT_04 :
					case SP_EXCPT_05 :
					case SP_EXCPT_06 :
					case SP_EXCPT_07 :
					case SP_EXCPT_08 :
					case SP_EXCPT_09 :
					case SP_EXCPT_10 :
					case SP_EXCPT_11 :
					case SP_EXCPT_12 :
					case SP_EXCPT_13 :
					case SP_EXCPT_14 :
					case SP_EXCPT_15 :
						if (0)
						{
							resolve_except(ptr_ptr,(*except_highlighted-1),0,0,0,R_APPROVE,ACT_APPROVE,
								(*(ptr_ptr + *except_highlighted -1))->type );
						}
						break;
					case CALL_CANCEL:
					case WAKE_UP:
						/* update call history & call rec */
						if(srch_calls[*except_highlighted - 1].grouped_with)
							resolve_except(ptr_ptr,(*except_highlighted-1),0,1,1,R_CAN_MULT,ACT_APPROVE,
					 			(*(ptr_ptr + *except_highlighted -1))->type ); 
						else
							resolve_except(ptr_ptr,(*except_highlighted-1),0,1,1,R_APPROVE,ACT_APPROVE,
					 			(*(ptr_ptr + *except_highlighted -1))->type ); 
						break;
						
					case UNZONED:
						prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_22,
									     "<APPROVE not allowed for unzoned exceptions>"));
						break;

                                case PF_1_KEY:
                                  prt_error(UI_NOFIELD,"MAKE CALL Function Key pressed");
                                  break;

					case DRV_MSG:
						/* update msg log file, do not update call file or call history file */
						resolve_except(ptr_ptr,(*except_highlighted-1),0,0,0,R_APPROVE,ACT_APPROVE,
					 		(*(ptr_ptr + *except_highlighted -1))->type ); 
						break;

					      case ITM_INVOICE_NAK:
						if (read_callh_rec(srch_calls[*except_highlighted -1].nbr, ITM_INVOICE_NAK))
						  update_callh_rec(ACT_APPROVE, 0);
						resolve_except(ptr_ptr, (*except_highlighted-1), 0, 0, 0, R_APPROVE,
							       ACT_APPROVE,
							       (*(ptr_ptr + *except_highlighted-1))->type );
						break;

					      case ATTRIBUTE_HOLD:
						resolve_except(ptr_ptr, (*except_highlighted-1), 0, 0, 0, R_APPROVE,
							       ACT_APPROVE,
							       (*(ptr_ptr + *except_highlighted-1))->type );
						break;

					default:
						prt_error(UI_FATAL,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_23,
									   "DEBUG MSG: this exception is not coded yet!!!"));
				} 

				if ( mult_refresh != TRUE )
				  disp_except_detail(ptr_ptr,*except_highlighted);
				else
				  mult_refresh = FALSE;
				
				break;	/* leave outer switch */


			case DISAPPROVE_KEY:
				/* check if any detail is available on display */
				if(*except_highlighted <= 0)  {
					prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_19,
								     "<no exception highlighted>"));
					return(DONE);
				}
				/* check type of exception being resolved and process accordingly */
				switch((*(ptr_ptr + *except_highlighted -1))->type)  {
					case ETA:
					case MESSAGE:
					case REQ_TO_TLK:
						if (((*(ptr_ptr + *except_highlighted -1))->call_nbr == 0) &&
						   ((*(ptr_ptr + *except_highlighted -1))->call_ptr == NULL))
							update_call_history = 0;
						else 
							update_call_history = 1;

						resolve_except(ptr_ptr,(*except_highlighted-1),0,0,update_call_history,
								R_DISSAPROVE,ACT_DISAPPROVE,
					 			(*(ptr_ptr + *except_highlighted -1))->type ); 
						break;
                                        case MISSING_RECEIPT_RETRY_LIMIT:
					case CALL_TIMEOUT:
					case LATE_METER:
					case SHORT_METER:
					case CALLBACK:
					case CALLOUT:
					case REJECT:
					case FLAG_REQ:
					case BID:
					case SUP_HOLD:
					case WAKE_UP:
					case NO_SHOW:
				case NS_SHORT:
					case PERSONAL:
					case MANUAL:	/* when cab's kdt is not working */
						/* update call history record, and write request to FIFO ;
						 * do not update call record
						 */
						resolve_except(ptr_ptr,(*except_highlighted-1),0,0,1,R_DISSAPROVE,ACT_DISAPPROVE,
					 		(*(ptr_ptr + *except_highlighted -1))->type ); 
						break;

					case EXTRA_TAXIS:
					case CALL_NO_MSG:
					case CALL_CANCEL:
                                        case MULT:
					case HOL_TM_CALL:
					case CALL_W_MSG:
						/* mark call as cancelled, update call & callh recs */
						resolve_except(ptr_ptr,(*except_highlighted-1),0,1,1,R_DISSAPROVE,ACT_DISAPPROVE,
					 		(*(ptr_ptr + *except_highlighted -1))->type ); 
						break;

					case SYS_ERR:   /* system errors; do not update call or callh rec */
						if ((0) && ((*(ptr_ptr + *except_highlighted-1))->info >= NO_ACK_MDT_1)
									    && ((*(ptr_ptr + *except_highlighted-1))->info <= NO_ACK_ITM_3))
						{
							/* display the call message screen and update the call history record */
							disp_call_msg( get_cl_nbr( (*(ptr_ptr + *except_highlighted-1))->call_nbr,
										   (*(ptr_ptr + *except_highlighted-1))->info),1);
							resolve_except(ptr_ptr,(*except_highlighted-1),0,0,1,R_DISSAPROVE,ACT_DISAPPROVE,
								(*(ptr_ptr + *except_highlighted -1))->type );
							break;

						}

						if ((0) && ((*(ptr_ptr + *except_highlighted-1))->info == ASSIGN_ERR))
						{
							/* update call history for nack on call assign */
							resolve_except(ptr_ptr,(*except_highlighted-1),0,0,1,R_DISSAPROVE,ACT_DISAPPROVE,
								(*(ptr_ptr + *except_highlighted -1))->type );
							break;
						}

						resolve_except(ptr_ptr,(*except_highlighted-1),0,0,0,R_DISSAPROVE,ACT_DISAPPROVE,
							(*(ptr_ptr + *except_highlighted -1))->type );
						break;
					case EMERGENCY:
					case MFAULT:
						resolve_except(ptr_ptr,(*except_highlighted-1),0,0,0,R_DISSAPROVE,ACT_DISAPPROVE,
							(*(ptr_ptr + *except_highlighted -1))->type );

						break;

					case SP_EXCPT_01 :
					case SP_EXCPT_02 :
					case SP_EXCPT_03 :
					case SP_EXCPT_04 :
					case SP_EXCPT_05 :
					case SP_EXCPT_06 :
					case SP_EXCPT_07 :
					case SP_EXCPT_08 :
					case SP_EXCPT_09 :
					case SP_EXCPT_10 :
					case SP_EXCPT_11 :
					case SP_EXCPT_12 :
					case SP_EXCPT_13 :
					case SP_EXCPT_14 :
					case SP_EXCPT_15 :
						if (0)
						{
							resolve_except(ptr_ptr,(*except_highlighted-1),0,0,0,R_DISSAPROVE,ACT_DISAPPROVE,
								(*(ptr_ptr + *except_highlighted -1))->type );
						}
						break;
					case UNZONED:
						prt_error(UI_NOFIELD,
							  catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_24,
								  "<DISAPPROVE not allowed for unzoned exceptions>"));
						break;

					case DRV_MSG:
						/* update msg log file, do not update call file or call history file */
						resolve_except(ptr_ptr,(*except_highlighted-1),0,0,0,R_DISSAPROVE,ACT_DISAPPROVE,
					 		(*(ptr_ptr + *except_highlighted -1))->type ); 
						break;
						
					case ITM_INVOICE_NAK:
						if (read_callh_rec(srch_calls[*except_highlighted - 1].nbr, ITM_INVOICE_NAK))
						  update_callh_rec(ACT_DISAPPROVE, 0);
						resolve_except(ptr_ptr, (*except_highlighted-1), 0, 0, 0, R_DISSAPROVE,
							       ACT_DISAPPROVE,
							       (*(ptr_ptr + *except_highlighted-1))->type );
						break;

					      case ATTRIBUTE_HOLD:
						resolve_except(ptr_ptr, (*except_highlighted-1), 0, 0, 0, R_DISSAPROVE,
							       ACT_DISAPPROVE,
							       (*(ptr_ptr + *except_highlighted-1))->type );
						break;
						
					default:
						prt_error(UI_FATAL,
							  catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_23,
								  "DEBUG MSG: this exception is not coded yet!!!"));
				}
				disp_except_detail(ptr_ptr,*except_highlighted);
				break;   /* leave outer switch */

			case HOLD_KEY:
				/* write request to FIFO; do not generate or update call history record or call record */
				/* check if any detail is available on display */
				if(*except_highlighted <= 0)  {
					prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_19,
								     "<no exception highlighted>"));
					return(DONE);
				}
				if ((*(ptr_ptr + *except_highlighted -1))->type == ITM_INVOICE_NAK)
				  {
				    prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_FTNKEY_1,
								 "<wrong function key pressed>"));
				    return(DONE);
				  }

				/* this key should only be allowed for exceptions which have a call associated with it */
				if (srch_calls[*except_highlighted - 1].nbr == 0) {
					prt_error(UI_NOFIELD,
						  catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_25,
							  "<hold is allowed only for exceptions associated with a call>"));
					return(DONE);
				}
				/* hold is only allowed for unassigned & assigned calls; is not allowed for
				 * completed calls, killed calls, cancelled calls for calls with status of pickup 
				 */
				if((strcmp(srch_calls[*except_highlighted-1].status,PICKUP) == 0) ||
				 	(strcmp(srch_calls[*except_highlighted-1].status,COMPLETE) == 0) ||
				 	(strcmp(srch_calls[*except_highlighted-1].status,KILLED) == 0) ||
				 	(strcmp(srch_calls[*except_highlighted-1].status,CANCELD) == 0) )  {
					prt_error(UI_NOFIELD,"<%s %s %s>",
						  catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_26, "hold not allowed for calls with"), 
						  srch_calls[*except_highlighted-1].status,
						  catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_27, "status") );
					return(DONE);
				}

				if(srch_calls[*except_highlighted - 1].grouped_with) {
					prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_28,
								     "<hold not allowed for mult call>"));
					return(DONE);
				}

				/* No more than one hold per call at a time */
				if (hold_exists_for_call(srch_calls[*except_highlighted - 1].nbr))
				{
				  prt_error(UI_NO_MSG, catgets(UI_m_catd, UI_1, UI_TEXT_33,
							       "There is already a HOLD on the call"));
				  return(-1);
				}

				resolve_except(ptr_ptr,(*except_highlighted-1),0,0,1,R_SUP_HOLD,ACT_HOLD,
					(*(ptr_ptr + *except_highlighted -1))->type); 
				break;
		
			case CALLBACK_KEY:
				/* write request to FIFO; generate a new call history record */
				/* check if an exception is highlighted */
				if(*except_highlighted <= 0)  {
					prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_19,
								     "<no exception highlighted>"));
					return(DONE);
				}
				if ((*(ptr_ptr + *except_highlighted -1))->type == ITM_INVOICE_NAK)
				  {
				    prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_FTNKEY_1,
								 "<wrong function key pressed>"));
				    return(DONE);
				  }

				/* this key should only be allowed for exceptions which have a call associated with it */
				if (srch_calls[*except_highlighted - 1].nbr == 0) {
				  prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_29,
                                            "<callback is allowed only for exceptions associated with a call>"));
					return(DONE);
				}
				/* try to resolve this exception */
				resolve_except(ptr_ptr,(*except_highlighted-1),0,0,1,R_CALLBACK,ACT_CALLBACK,
					(*(ptr_ptr + *except_highlighted -1))->type ); 
				break;

			case RESEND_KEY:
				/* write request to FIFO; generate a new call history record */
				/* check if an exception is highlighted */
				if ( *except_highlighted <= 0 )  {
					prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_19,
                                                                     "<no exception highlighted>"));
					return(DONE);
				}
				/* this key should only be allowed for exceptions which have a 
				 * call associated with it
				 */
/**
Note that this is commented out (not sure why DNR 90-12-06) 
				if ((*(ptr_ptr + *except_highlighted - 1 ))->call_ptr == (struct calls *)0 &&
				    (*(ptr_ptr + *except_highlighted - 1 ))->call_nbr == (long)0 ) { 	/* no call ? *
					/* exception not associated with a call *
					prt_error(UI_NOFIELD,
					          catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_30,
						          "<resend is allowed only for exceptions associated with a call>"));
					return(DONE);
				} 
 **/
				/* try to resolve this exception */
				resolve_except(ptr_ptr,(*except_highlighted-1),0,0,1,R_RESEND,ACT_RESEND,
					(*(ptr_ptr + *except_highlighted -1))->type ); 
				break;	/* leave switch */
			

			case DISPATCH_KEY:		/* write request to FIFO; generate a new call history record */
				if (*except_highlighted <= 0) {			/* check if an exception is highlighted */
					prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_19, "<no exception highlighted>"));
					return(DONE);
				}
				if ((*(ptr_ptr + *except_highlighted -1))->type == ITM_INVOICE_NAK)
				  {
				    prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_FTNKEY_1,
								 "<wrong function key pressed>"));
				    return(DONE);
				  }
				
				if (srch_calls[*except_highlighted - 1].nbr == 0) {
					prt_error(UI_NOFIELD,
						  catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_31,
							  "<dispatch is allowed only for exceptions associated with a call>"));
					return(DONE);
				}

/*
 *                              fix for multiple calls
 *                              multiple calls are not allowed to be dispatched

 */
                                if ((*(ptr_ptr + *except_highlighted -1))->type == MULT) {
					prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_32, "<dispatch not allowed for mult call>"));
					return(DONE);
				}

				resolve_except(ptr_ptr,(*except_highlighted-1),0,1,1,R_DISPATCH,ACT_DISPATCH,
					       (*(ptr_ptr + *except_highlighted -1))->type ); 
				break;
			

							/* write request to FIFO; generate a new call history record; */
			case CANCEL_KEY:		/* update record on disk per fleet option */
				if (*except_highlighted <= 0) {		/* check if an exception is highlighted */
					prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_19, "<no exception highlighted>"));
					return(DONE);
				}
				if (( *(ptr_ptr + *except_highlighted -1))->type == ITM_INVOICE_NAK)
				  {
				    prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_FTNKEY_1,
								 "<wrong function key pressed>"));
				    return(DONE);
				  }
				if((*(ptr_ptr + *except_highlighted -1))->type == LATE_METER)
                                  {
                                        prt_error(UI_NOT_ALLOWED,"<not allowed to cancel latemeter calls>");
                                        return(DONE);
                                  }				  
									/* exception associated with a call ? */
				if (srch_calls[*except_highlighted - 1].nbr == 0) {
					prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_33,
					"<cancel is allowed only for exceptions associated with a call>"));
					return(DONE);
				}

				/* Check if call has already been cancelled. Note that a cancelled call can be
				 revived, so look at the last entry in call history */
				if (get_last_exception_type_from_callh((*(ptr_ptr + *except_highlighted - 1 ))->call_nbr)
				    == CALL_CANCEL)
				  {
				    prt_error(UI_CANCELLED_CALL, "");
				    return(DONE);
				  }

				/* try to resolve this exception */
				resolve_except(ptr_ptr,(*except_highlighted-1),0,1,1,R_CANCEL,ACT_CANCEL,
					       (*(ptr_ptr + *except_highlighted - 1))->type); 
				disp_except_detail(ptr_ptr,*except_highlighted);
				break;
			
			case DETAIL_KEY :
			        New_screen_from_command = 0;
				if( *except_highlighted <= 0)  {
					prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_19, "<no exception highlighted>"));
					return(DONE);
				}

				if (srch_calls[*except_highlighted - 1].nbr != 0)
                                  {
				    /* calls_on_page is a global */
				    calls_on_page = *got_excepts;
				    choice = *except_highlighted - 1;
				    detail_key(&choice);
				    *except_highlighted = choice + 1;
                                  }
				else {
					prt_error(UI_NOFIELD,
						  catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_34,
							  "<detail is allowed only for exceptions associated with a call>"));
					return(DONE);
				}

				/* Only redisplay screen if COMMAND_KEY did not change it */
				if (!New_screen_from_command) {
				    /* returned from the detail screen. display the exception screen */
				    scr_name = EXCEPTIONS_SCR;
				    disp_except_scr();
				    disp_except_sum(*got_excepts,ptr_ptr);
				    change_line_attr(*except_highlighted,0,*except_highlighted,A_BOLD|A_REVERSE);
				    disp_except_detail(ptr_ptr,*except_highlighted);	/* display the current line */
				}
				return(DONE);
				break;

			case MESSAGE_KEY :
				if( *except_highlighted <= 0)  {
				    mesgs(NULL);
				    return(DONE);
				}

				if (mesgs(*except_highlighted, ptr_ptr) >= 0) {
				    // TSAB wants these exceptions to remain until
				    // explicitly approved
				  if ( !(0) )
					resolve_except(ptr_ptr,(*except_highlighted-1),0,0,1,R_APPROVE,ACT_APPROVE,
						       (*(ptr_ptr + *except_highlighted -1))->type ); 
				}
				return(DONE);


                case UPDATE_KEY:
                  if (*except_highlighted <= 0) {		/* check if an exception is highlighted */
                    prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_19, "<no exception highlighted>"));
                    return(DONE);
                  }
                  else if ( (*(ptr_ptr + *except_highlighted -1))->type != EMERGENCY )
                    {
                      prt_error(UI_INV_KEY," ");
                      return(DONE);
                    }                      
                  alt_emerg( &(*(ptr_ptr + *except_highlighted - 1))->emerg_bcast );
                  if ( msg_on_display ) {
                    msg_on_display = 0;
                    delete_message_window();
                  }
                  msg_on_display = 1;
                  create_message_window(*except_highlighted);
                  display_emerg_mesg(&(*(ptr_ptr + *except_highlighted -1))->emerg_bcast);
                  return(DONE);
                  break;
                                
			case MULTIPLE_KEY:
				if (*except_highlighted <= 0) {		/* check if an exception is highlighted */
					prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_19, "<no exception highlighted>"));
					return(DONE);
				}
				if ((*(ptr_ptr + *except_highlighted -1))->type == ITM_INVOICE_NAK)
				  {
				    prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_FTNKEY_1,
								 "<wrong function key pressed>"));
				    return(DONE);
				  }

				if((*(ptr_ptr + *except_highlighted - 1))->type == MULT)
				  {
				    multiple_key(*except_highlighted - 1, ptr_ptr);
				    disp_except_sum(*got_excepts,ptr_ptr);
				    change_line_attr(*except_highlighted,0,*except_highlighted,A_REVERSE|A_BOLD);
				  }
				else
					prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_35, "<MULT only MULTIPLE exceptions>"));
				break;

			default:						/* user probably entered a function key */
				return(NOT_DONE);
		}
		return(DONE);
	}
}

/*-------------------------------------------------------------------------------*/
/* disp_except_detail: displays the detail of a call in the detail area 	 */
/*-------------------------------------------------------------------------------*/
disp_except_detail(except_ptr,detail_line)
struct excepts *except_ptr[];			/* array of pointers to exception slots */
int detail_line;				/* line # whose detail is to be displayed */
{
	clear_detail_area();	/* prepare to display a call */
	if ( except_ptr[detail_line-1]->call_ptr != (struct calls *)0 || except_ptr[detail_line-1]->call_nbr != (long)0 ) 
		dsp_cl_detail(detail_line);	/* this exception concerns a call */
	

	if ( msg_on_display )  { 			/* check if message window should be displayed or erased */
	    msg_on_display = 0;
	    delete_message_window();
	}

	if (except_ptr[detail_line-1]->type == DRV_MSG) {
		msg_on_display = 1; 					/* need to display the message window */
		create_message_window(detail_line);
		display_mesg(except_ptr[detail_line-1]->info);
	}
        else if (except_ptr[detail_line-1]->type == EMERGENCY) {
		msg_on_display = 1; 					/* need to display the message window */
		create_message_window(detail_line);
		display_emerg_mesg(&except_ptr[detail_line-1]->emerg_bcast);
	}

  	else 	if((except_ptr[detail_line-1]->type == CALL_W_MSG || except_ptr[detail_line - 1]->type == MULT) && 
			(except_ptr[detail_line-1]->call_ptr != NULL) && (except_ptr[detail_line-1]->call_ptr->msg_nbr > 0)){
		msg_on_display = 1; 					/* need to display the message window */
		create_message_window(detail_line);
		display_mesg(except_ptr[detail_line-1]->call_ptr->msg_nbr);
	}

}  /* end disp_except_detail() */

/******************************************************************************************/
/* manual_zone_veh: processes the manual zone/vehicle assignment mode on the exceptions screen    */
/*		Returns  	DONE	if processing is complete			  */
/*				NOT_DONE	if processing is not completed 		  */
/******************************************************************************************/
int manual_zone_veh(except_line,read_buf,ptr_ptr,got_excepts,z_or_v)
int *except_line;					/* exception line # highlighted */
char *read_buf;						/* buffer for input */
struct excepts **ptr_ptr;
int *got_excepts;					/* # of exceptions on screen */
char z_or_v;						/* whether manual zone or manual_veh assign */
{
	static struct scr_flds man_zone_fld[] = { {0,0,1,1},
			{LINE_NO_ROW,LINE_NO_COL,EXCEPT_LINE_CHARS,2},
			{START_SUM_ROW,ZONE_EXCEPT_COL,CHARS_ZONE,1},
	};

	static struct scr_flds man_veh_fld[] = { {0,0,1,1},
			{LINE_NO_ROW,LINE_NO_COL,EXCEPT_LINE_CHARS,2},
			{START_SUM_ROW,VEHNO_EXCEPT_COL,CHARS_VEH,1},
	};

	struct scr_flds *ptr_scr_fld;
	struct scr_flds *ptr_to_fld;
	int piu;
	int field_entered;
	short error_flag = 0;
	int key_val;
	int ret_ck_fld;
	short *end_man_zv;			/* pointer to end_man_zone or end_man_veh field */
	int should_clear = 0;

	attrset(A_REVERSE); 			/* on the first line show that we are on manual zone assignment mode */
	if ( z_or_v == C_ZONE ) {
		mvaddstr(0,TYPE_EXCEPT_COL,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_36, "MODE=MAN_ZONE"));
		ptr_scr_fld = man_zone_fld;
	}
	else  { 							/* its manual veh assignment */
		mvaddstr(0,TYPE_EXCEPT_COL,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_37, "MODE=MAN_VEH"));
		ptr_scr_fld = man_veh_fld;
	}

	attrset(0);

	piu = 2;							/* for zoning calls */
	ptr_to_fld = ptr_scr_fld + piu;
	ptr_to_fld->prompt_row = START_SUM_ROW + (*except_line) - 1; 	/* initialize the row of entry */


	/* enter a while loop which executes as long as the screen is the exceptions screen and the user remains in this mode */
	 end_man_zone = 0;	  				/* flag for ending manual_zone mode */
	 end_man_veh = 0;					/* flag for ending manual vehicle mode */
	 if ( z_or_v == C_ZONE )
		 end_man_zv =  &end_man_zone;
	else
		end_man_zv = &end_man_veh;

	while ( ! end_exceptions && scr_name == EXCEPTIONS_SCR  &&  ! *end_man_zv )  {
		check_mail();
		key_val = read_field(stdscr,ptr_scr_fld,piu,read_buf,&field_entered,&should_clear);

		if ( key_val == -1 ) 
			continue;				/* read_field() timed out */
		
		if ( clear_err_flag )
			clear_error();				/* clear error msg from last line */

		/* chack the field entered by the user */
		ret_ck_fld = (*ck_except_fld[piu])(ptr_scr_fld,&piu,read_buf,key_val,&field_entered,except_line,
							ptr_ptr,got_excepts,z_or_v);

		if ( ret_ck_fld == NOT_DONE )  { 		/* function key entered; should end manual zone assignment mode  */
			*end_man_zv = 1;
			ftnkey_handler(&piu,key_val,0);
		}

		/* Redisplay the message that might have been overwritten */
		if (msg_on_display && mesg_wind)
		  {
		    touchwin(mesg_wind);
		    wrefresh(mesg_wind);
		  }
	}

	attrset(A_REVERSE); 				/* on the first line show that we are out of manual zone assignment mode */
	mvaddstr(0,TYPE_EXCEPT_COL,"             ");
	attrset(0);
	return(DONE);

}  /* end manual_zone_veh() */


/*******************************************************************************************************/
/* ck_except_zone_veh: checks if the zone # entered is valid for manual zone assignment 	       */
/*	or vehicle # entered is valid for manual vehicle assignment mode on  the exceptions screen     */
/*******************************************************************************************************/
int ck_except_zone_veh(ptr_scr_fld,piu_ptr,read_buf,key_val,field_entered,except_line,ptr_ptr,got_excepts,z_or_v)
struct scr_flds *ptr_scr_fld;
int *piu_ptr;
char *read_buf;
int key_val;
int *field_entered;
int *except_line;					/* exception line # highlighted */
struct excepts **ptr_ptr;				/* pointers to exception slots */
int *got_excepts;					/* # of exceptions on screen */
char z_or_v;						/* C_ZONE/C_VEHICLE for manual zone or veh assignmnet */

{
	short i;
	struct scr_flds *ptr_to_fld;
	short error_flag=0;
	short zone_veh_num;
	struct cisam_zn *zn_ptr;
	char temp_comment[41];				/* for pickup address comment truncation */
	int old_num;  					/* old line # highlighted */
	int temp_int;	
	char zv_desc[7];				/* for error msgs etc */
	struct cisam_vh *vh_ptr;
	int    times_thru, rc;
	struct veh_rqst vrq;
	VEH_RESP_REC  *vrp;
	VEH_RESP_HNDL hVehResp;

	if (z_or_v == C_ZONE)				/* is it manual zone assignment or manual vehicle assignment */
		strcpy(zv_desc,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_38, "zone"));
	else 
		strcpy(zv_desc,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_39, "veh"));

	ptr_to_fld = ptr_scr_fld + (*piu_ptr);

	if (*field_entered) {					/* user entered the zone #, verify it */
		i = 0;
		while (i< ptr_to_fld->max_chars  && read_buf[i] != '\0') {	
			if (!isdigit(read_buf[i])) {
				prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_15, "<digits only>"));
				error_flag = 1;
				break;				/* leave while loop */
			}
			i++;
		}

		if (!error_flag) {	/* manual zoning and manual vehicle assignment modes are only allowed for exceptions */
					/* associated with a call. Check if this exception is associated with a call or not */
			if ((*(ptr_ptr + *except_line -1))->call_ptr == (struct calls *)0 && (*(ptr_ptr + *except_line -1))->call_ptr == (long)0) {
					prt_error(UI_NOT_ALLOWED,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_40, "<manual zone/vehicle for exceptions not associated with a call>"));
					error_flag = 1;
			}
		}

		if (!error_flag) {
			if ((zone_veh_num = atoi(read_buf)) <= 0) {	/* veh # 0 allowed to unassign a call; zone 0 not allowed */
				if (zone_veh_num == 0  &&  z_or_v == C_VEHICLE) 
					;
				else {					/* invalid zone or vehicle # entered */
					prt_error(UI_NOFIELD,"<%s %s %s>", catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_41, "invalid"), zv_desc, catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_42, "# entered") );
					error_flag = 1;
				}
			}
		}
		
		if (!error_flag) {
			if (z_or_v == C_ZONE) {
			 	if (srch_calls[*except_line-1].pckup_zone != zone_veh_num) {	/* its manual zoning */
					zn_ptr = (struct cisam_zn *)req_buf;
					zn_ptr->fleet = srch_calls[*except_line-1].fleet;
					zn_ptr->nbr = zone_veh_num;
					if (db(ZONE_FILE_ID,READ_KEYS,&zn_key1,ISEQUAL,0)<0){	
						prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_43, "<zone #>"));			/* invalid zone entered */
						error_flag = 1;
					}
					else {				/* check the # entered is a zone #, and not a superzone # */
						if (zn_ptr->type == '3') {
							prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_44, "<superzones not allowed>"));
							error_flag = 1;
						}
						else {				/* check the # entered is a zone #, and not a superzone # */
							if (zn_ptr->type == '4') {
								prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_69, "<special backup zones not allowed>"));
								error_flag = 1;
							}
							else {			/* this exception should be resolved */
			 					srch_calls[*except_line - 1].pckup_zone = zone_veh_num;
								strncpy(srch_calls[*except_line-1].zone_desc,zn_ptr->desc,20);
								resolve_except(ptr_ptr,(*except_line-1),zone_veh_num,1,1,R_MAN_ZONE,
									ACT_MANZONE,UNZONED);
							}
						}
					}
				}
			}
			/* It's manual veh assignment */
			else {
			 	if (srch_calls[*except_line-1].veh_nbr != zone_veh_num) {

				  /* Check if time call */
				  if (srch_calls[*except_line - 1].pri_status == 63 &&
				      strcmp(srch_calls[*except_line - 1].status, PENDING) == 0)
				    {
				      prt_error(UI_NO_MSG, catgets(UI_m_catd, UI_1, UI_TEXT_7,
								   "Can't assign time calls. Change to NOW first."));
				      error_flag = 1;
				    }
				  else
				    {
				      vh_ptr = (struct cisam_vh *)req_buf;
				      vh_ptr->fleet = srch_calls[*except_line-1].fleet;
				      vh_ptr->nbr = (short)zone_veh_num;

				      if (zone_veh_num > 0 && db(VEHICLE_FILE_ID,READ_KEYS,&veh_key2,ISEQUAL, 0) , 0){
					prt_error(UI_NOFIELD, catgets(UI_m_catd, UI_1, UI_CK_LN_NO_15, "<vehicle #>"));
					error_flag = 1;
				      }
				      else
					{

					  vrq.d_prcs_id = DISP_DEST;
					  vrq.u_prcs_id = pid;
					  vrq.rec_type = VEH_REQ;
					  vrq.fleet = vh_ptr->fleet;
					  vrq.veh_nbr = vh_ptr->nbr;
					  msgsnd(msgkey, (struct msgbuf *)&vrq, sizeof(struct veh_rqst), IPC_NOWAIT);

					  hVehResp = TMALLOC( VEH_RESP_REC, 1 );
					  vrp = ( VEH_RESP_REC * )hVehResp;
					  
					  times_thru = 0;
					  while (times_thru <= 2)
					    {
					      rc = msgrcv(msgkey, (void *)vrp,
							  sizeof(VEH_RESP_REC), pid, 0);
					      if (rc == -1)
						{
						  if (errno != EINTR)
						    {
						      prt_error(UI_FATAL, "%s", catgets(UI_m_catd, UI_1, UI_UI_SSTAT_74,
											"Error reading from dispatch in msgrcv"));
						      return(0);
						    }
						  if (++times_thru == 2)
						    {
						      prt_error(UI_SYSTEM_DOWN, "");
						      return(0);
						    }
						}
					      else if (vrp->rec_type == VEH_REQ)
						break;
					      else
						{
						  prt_error(UI_FATAL, "%s", catgets(UI_m_catd, UI_1, UI_UI_SSTAT_74,
										    "Error reading from dispatch in msgrcv"));
						  return(0);
						}
					    }
					  if (vrp->cur_status.idle != 1)
					    {
					      prt_error(UI_VEH_STATUS, " ");
					      error_flag = 1;
					    }
					  else {						/* this exception should be resolved */
					    if(srch_calls[*except_line - 1].grouped_with) 
					      resolve_except(ptr_ptr, (*except_line-1), zone_veh_num, 1, 1, R_M_MAN_VEH,
							     ACT_MANVEH, (*(ptr_ptr + *except_line -1))->type);
					    else
					      resolve_except(ptr_ptr, (*except_line-1), zone_veh_num, 1, 1, R_MAN_VEH,
							     ACT_MANVEH, (*(ptr_ptr + *except_line -1))->type);
					  }
					}
				    }
				}
			}
		}
	}

	old_num = *except_line;						/* update piu position for next read_field call */
	switch (key_val) {
		case OK:						/* field was entered */
		case '\n':
		case '\r':
		case '\t':
			*piu_ptr = 1;					/* go to "Line" prompt */
			return(DONE);

		case KEY_DOWN:						/* move the highlight down by one count */
			if (*except_line == *got_excepts)
				*except_line = 1;
			else 
				*except_line  += 1;
			change_line_attr(old_num,0,*except_line,A_REVERSE|A_BOLD);
			disp_except_detail(ptr_ptr,*except_line);	/* display the detail */

									/*  change the row for reading the zone # appropriately */
			ptr_to_fld->prompt_row = START_SUM_ROW + (*except_line) - 1;
			return(DONE);

		case KEY_UP:						/* move highlight up by 1 line */
			if ( *except_line == 1 )
				*except_line = *got_excepts;
			else
				*except_line -= 1;
			change_line_attr(old_num,0,*except_line,A_REVERSE|A_BOLD);
			disp_except_detail(ptr_ptr,*except_line);			/* display the detail */

									/*  change the row for reading the zone # appropriately */
			ptr_to_fld->prompt_row = START_SUM_ROW + (*except_line) - 1;
			return(DONE);
			
		case MANZONE_KEY:					/* get out of manula zone mode */
			end_man_zone = 1;
			return(DONE);

		case MANVEH_KEY:					/* get out of manula zone mode */
			end_man_veh = 1;
			return(DONE);

		case NEXT_PG_KEY:					/* try to display the previuhous 17 exceptions */
			*piu_ptr = 1;	/* move to "Line" prompt */
			if ((temp_int = get_excepts(NO,FORWARD,ptr_ptr)) == 0) {
				prt_error(UI_NOEXCEPTS,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_18, "<no exceptions meeting selection criteria>"));
				return(DONE);
			}
			else {							/* got some excepts */
				pg_no++;
				*got_excepts = temp_int;
				move(START_SUM_ROW,0);
				clrtobot();					/* clear the summary area */
				*except_line = 0;				/* since new pg */
				for (i=0; i<SEARCHES_ON_PG; i++)		/* set all exceptions to not resloved */
					resolved_except[i] = 0;
				clear_detail_area();				/* CLEAR THE DETAIL AREA */
				disp_except_sum(*got_excepts,ptr_ptr);
			}
			return(DONE);

		case PREV_PG_KEY:						/* try to display the previuhous 17 exceptions */
			*piu_ptr = 1;						/* move to "Line" prompt */
			if ((temp_int = get_excepts(NO,BACKWARD,ptr_ptr)) == 0) {
				prt_error(UI_NOEXCEPTS,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_18, "<no exceptions meeting selection criteria>"));
				pg_no = 1;					/* got to 1st pg */
				return(DONE);
			}
			else {							/* got some excepts */
				pg_no--;
				*got_excepts = temp_int;
				move(START_SUM_ROW,0);
				clrtobot();					/* clear the summary area */
				*except_line = 0;				/* since prev pg */
				for (i=0; i<SEARCHES_ON_PG; i++)		/* set all exceptions to not resloved */
					resolved_except[i] = 0;
				clear_detail_area();				/* CLEAR THE DETAIL AREA */
				disp_except_sum(*got_excepts,ptr_ptr);
			}
			return(DONE);

		case REFRESH_KEY:
			if (msg_on_display) {		/* check if message window should be displayed or erased */
				msg_on_display = 0;	/* there is no message window on display */
				delete_message_window();
			}
			pg_no = 1;
			*piu_ptr = 1;						/* move to "Line" prompt */
			move(START_SUM_ROW,0);
			clrtobot();					/* clear the summary area; and fill it with new info */
			*except_line = 0;				/* since new start */
			for (i=0; i<SEARCHES_ON_PG; i++)		/* set all exceptions to not resloved */
				resolved_except[i] = 0;
			clear_detail_area();				/* CLEAR THE DETAIL AREA */

			/* initialize array of srch_calls[] structures */
			for(i = 0; i < SEARCHES_ON_PG; i++) 
				memset((char *)&srch_calls[i], '\0', sizeof(struct cisam_cl));

			if (!(*got_excepts = get_excepts(YES,FORWARD,ptr_ptr))) {		/* no exceptions found */
				end_man_zone = 1;						/* go back to check_except_line() */
				end_man_veh = 1;						/* go back to check_except_line() */
				prt_error(UI_NOEXCEPTS,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_1, "<meeting selection criteria>"));
			}
			else 									/* got some exceptions */
				disp_except_sum(*got_excepts,ptr_ptr);

			return(DONE);


		case HISTORY_KEY: 		/* display call history record(s) for the call/exception which is highlighted */
			if ((*(ptr_ptr + *except_line - 1 ))->call_ptr == (struct calls *)0 &&
			    (*(ptr_ptr + *except_line - 1 ))->call_nbr == (long)0 ) { 			/* no call ? */
				prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_20, "<call history is allowed only for exceptions associated with a call>"));
				return(DONE);
			}
			call_history(srch_calls[*except_line-1].nbr);		/* exception is with a call */
			return(DONE);

		default:							/* function key entered */
			prt_error(UI_INV_KEY,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_46, "<you must get out of MAN_VEH or MAN_ZONE mode first>"));
			return(DONE);
	}
}


int ck_except_line(ptr_scr_fld,piu_ptr,read_buf,key_val,field_entered,except_line,ptr_ptr,got_excepts,z_or_v)
	struct scr_flds *ptr_scr_fld;
	int *piu_ptr;
	char *read_buf;
	int key_val;
	int *field_entered;
	int *except_line;	/* exception line # highlighted */
	struct excepts **ptr_ptr;	/* pointers to exception slots */
	int *got_excepts;	/* # of exceptions on screen */
	char z_or_v;	/* C_ZONE/C_VEHICLE */
{
	struct scr_flds *ptr_to_fld;	/* pointer to approp. field */
	struct scr_flds *ptr_new_fld;	/* pointer to the zone reading field */
	int i;
	int line_num;	/* line # entered by user */
	int old_num;	/* old line # highlighted */
	int temp_int;	/* count of exceptions found */
	int	choice;

	ptr_to_fld = ptr_scr_fld + (*piu_ptr);
	ptr_new_fld = ptr_scr_fld + 2;	/* for reading the zone number */

	if ( *field_entered )  {
		/* user entered a line #; process this; do not process any functions;
		 * since that ends the manual zone assignment mode
		 */
		i = 0;
		while ( i < ptr_to_fld->max_chars  &&  read_buf[i] != '\0' ) {
			if(!isdigit(read_buf[i]))  {
				prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_15, "<digits only>"));
				return(DONE);
			} 
			i++;
		}

		if((line_num = atoi(read_buf)) <= 0 || line_num > *got_excepts)  {
			prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_16, "<invalid line # entered>"));
			return(DONE);
		}
		/* valid line # entered;  if exception number highlighted is other
		 * than the # entered, then display the detail of that exception
		 * and move the highlight
		 */
		if(line_num != *except_line) {
			change_line_attr(*except_line,0,line_num,A_REVERSE|A_BOLD);
			*except_line = line_num;
			disp_except_detail(ptr_ptr,*except_line); 		/* display the detail */
			/*  change the row for reading the zone # appropriately */
			*piu_ptr = 2;	/* lets go to the zone col */
			ptr_new_fld->prompt_row = START_SUM_ROW + (*except_line) - 1;
		}
		return(DONE);
	}
	else {

		/* user must have entered a function key, etc */
		old_num = *except_line;
		switch ( key_val )  {
			case '\n':
			case '\r':
			case '\t':
				*piu_ptr = 2;	/* lets go to zone col */
				/*  change the row for reading the zone # appropriately */
				ptr_new_fld->prompt_row = START_SUM_ROW + (*except_line) - 1;
				/* display the call detail */
				disp_except_detail(ptr_ptr,*except_line); 
				return(DONE);

			case KEY_DOWN:

				*piu_ptr = 2;	/* lets go to the zone col */
				/* move the highlight down by one count */
				if ( *except_line == *got_excepts )
					*except_line = 1;
				else 
					*except_line  += 1;
				change_line_attr(old_num,0,*except_line,A_REVERSE|A_BOLD);

				/*  change the row for reading the zone # appropriately */
				ptr_new_fld->prompt_row = START_SUM_ROW + (*except_line) - 1;
				/* display the call detail */
				disp_except_detail(ptr_ptr,*except_line); 
				return(DONE);

			case KEY_UP:

				*piu_ptr = 2;	/* lets go to the zone col */
				/* move highlight up by 1 line */
				if ( *except_line == 1 )
					*except_line = *got_excepts;
				else
					*except_line -= 1;
				change_line_attr(old_num,0,*except_line,A_REVERSE|A_BOLD);

				/*  change the row for reading the zone # appropriately */
				ptr_new_fld->prompt_row = START_SUM_ROW + (*except_line) - 1;
				/* display the call detail */
				disp_except_detail(ptr_ptr,*except_line); 
				return(DONE);
				
			case MANZONE_KEY:
				/* get out of manula zone mode */
				end_man_zone = 1;
				return(DONE);

			case MANVEH_KEY:
				/* get out of manula zone mode */
				end_man_veh = 1;
				return(DONE);

			case NEXT_PG_KEY:

				*piu_ptr = 1;	/* move to "Line" prompt */
				/* try to display the previuhous 17 exceptions */
				if ( (temp_int = get_excepts(NO,FORWARD,ptr_ptr)) == 0 )  {
					prt_error(UI_NOEXCEPTS,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_18, "<no exceptions meeting selection criteria>"));
					return(DONE);
				}
				else {
					/* got some excepts */
					pg_no++;
					*got_excepts = temp_int;
					/* clear the summary area */
					move(START_SUM_ROW,0);
					clrtobot();
					*except_line = 1;  /* since prev pg */
					/* set all exceptions to not resloved */
					for ( i=0; i < SEARCHES_ON_PG; i++ )
						resolved_except[i] = 0;
					disp_except_sum(*got_excepts,ptr_ptr);
					/* display detail of 1 st line */
					change_line_attr(0,0,1,A_REVERSE|A_BOLD);
					disp_except_detail(ptr_ptr,*except_line); 
					
				}  /* end else */

				return(DONE);

			case PREV_PG_KEY:
				
				*piu_ptr = 1;	/* move to "Line" prompt */
				/* try to display the previuhous 17 exceptions */
				if((temp_int = get_excepts(NO,BACKWARD,ptr_ptr)) == 0) {
					prt_error(UI_NOEXCEPTS,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_18, "<no exceptions meeting selection criteria>"));
					pg_no = 1;	/* got to 1st pg */
					return(DONE);
				} 
				else {
					/* got some excepts */
					pg_no--;
					*got_excepts = temp_int;
					/* clear the summary area */
					move(START_SUM_ROW,0);
					clrtobot();
					*except_line = 1;  /* since prev pg */
					/* set all exceptions to not resloved */
					for(i=0; i < SEARCHES_ON_PG; i++)
						resolved_except[i] = 0;
					disp_except_sum(*got_excepts,ptr_ptr);
					/* display detail of 1 st line */
					change_line_attr(0,0,1,A_REVERSE|A_BOLD);
					disp_except_detail(ptr_ptr,*except_line);
				} 

				return(DONE);

			case REFRESH_KEY:
				if (msg_on_display) { 		/* check if message window should be displayed or erased */
					msg_on_display = 0;	/* there is no message window on display */
					delete_message_window();
				}
				pg_no = 1;
				*piu_ptr = 1;			/* move to "Line" prompt */
				move(START_SUM_ROW,0);		/* clear the summary area; and fill it with new info */
				clrtobot();
				*except_line = 0;		/* since new start */
				for (i=0; i < SEARCHES_ON_PG; i++)		/* set all exceptions to not resloved */
					resolved_except[i] = 0;
				clear_detail_area();				/* clear the detail area */

				/* initialize array of srch_calls[] structures */
				for(i = 0; i < SEARCHES_ON_PG; i++) 
					memset((char *)&srch_calls[i], '\0', sizeof(struct cisam_cl));

				if (!(*got_excepts = get_excepts(YES,FORWARD,ptr_ptr))) {		/* no exceptions found */
					end_man_zone = 1;						/* get out of this mode */
					end_man_veh = 1;						/* get out of this mode */
					prt_error(UI_NOEXCEPTS,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_1, "<meeting selection criteria>"));
				}
				else {									/* got some exceptions */
					*except_line = 1;
					disp_except_sum(*got_excepts,ptr_ptr);
					change_line_attr(0,0,1,A_REVERSE|A_BOLD);		/* display detail of 1 st line */
					disp_except_detail(ptr_ptr,*except_line);
				}
				return(DONE);

			case HISTORY_KEY: 	/* display call history record(s) for the call/exception which is highlighted */
				if (srch_calls[*except_line - 1].nbr == 0) {
				  prt_error(UI_NOFIELD,
					    catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_20,
						    "<call history is allowed only for exceptions associated with a call>"));
				  return(DONE);
				}
				call_history(srch_calls[*except_line - 1].nbr); 	/* exception is with a call */
				return(DONE);

			case APPROVE_KEY:
			case DISAPPROVE_KEY:
				/* not allowed for manual zone assignment */
				prt_error(UI_NOFIELD,
					  catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_47,
						  "<APPROVE/DISAPPROVE not allowed while in manual zone/vehicle assignment mode>"));
				return(DONE);

			case DETAIL_KEY :
			        if (srch_calls[*except_line - 1].nbr == 0)
				  {
				    prt_error(UI_NOFIELD,
					      catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_34,
						      "<detail is allowed only for exceptions associated with a call>"));
				    return(DONE);
			          }
			        New_screen_from_command = 0;
				if( *except_line <= 0)  {
					prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_19,
								     "<no exception highlighted>"));
					return(DONE);
				}
				/* calls_on_page is a global */
				calls_on_page = *got_excepts;
				choice = *except_line - 1;
				detail_key(&choice);
				*except_line = choice + 1;

				/* Only redisplay screen if no COMMAND_KEY */
				if (!New_screen_from_command) {
				    scr_name = EXCEPTIONS_SCR;
				    disp_except_scr(); 
				    disp_except_sum(got_excepts,ptr_ptr);
				    change_line_attr(*except_line,0,*except_line,A_BOLD|A_REVERSE);
				    disp_except_detail(ptr_ptr,*except_line);
				}
				return(DONE);
				break;

			default:
				/* function key entered */
				return(NOT_DONE);

		}
	} 
}

int 
ck_except_dummy(ptr_scr_fld,piu_ptr,read_buf,key_val,field_entered,except_line,ptr_ptr,got_excepts,z_or_v)
struct scr_flds	*ptr_scr_fld;
int 	*piu_ptr;
char 	*read_buf;
int 	key_val;
int 	*field_entered;
int 	*except_line;		/* exception line # highlighted */
struct excepts 	**ptr_ptr;	/* pointers to exception slots */
int 	*got_excepts;		/* # of exceptions on screen */
char 	z_or_v;			/* C_ZONE or C_VEHICLE */
{
	return(DONE);
}

/********************************************************************************************************/
/* resolve_except: tries to resolve an exception. Updates the calls file on disk; the call		*/
/*		   history file, and formats and writes a request to disptach. This function		*/
/*		   also set the inuser byte in shared memory						*/
/********************************************************************************************************/

resolve_except(except_ptr,except_index,zv_num,ch_call_flag,ch_hist_flag,resolve_code,action_code,except_type)
struct excepts *except_ptr[];
int except_index;				/* index of the exception highlighted */
int zv_num;					/* zone or vehicle # entered */
short ch_call_flag;				/* 0=do not update call file on disk */
short  ch_hist_flag;				/* 0=do not update call history rec */
int resolve_code;				/* exception resolution code */
char *action_code;				/* action code description */
int except_type;				/* type of exception */
{
	struct excpt_rec except_fifo;		/* for writing except resolution record to FIFO */
	struct cisam_cl *temp_cl_ptr;
	int i,j;
	char temp_comment[41];
	char temp_zone_nbr[4];			/* temporary zone number holder */
	char temp_veh_nbr[5];			/* temporary vehicle number holder */
	char temp_str[7];			/* temporary string holder */
	char exdesc[9];				/* description of exception from shared memory */
	struct excpt_desc *exptr;		/* pointer to exception desc slot */
	char date_str[10];			/* for formatting date */
	char time_str[10];			/* for formatting time */
	char cancel_action;			/* cancel action byte from fleet struct in shared memory; for CANCEL key */
	char call_except_num;			/* '*' = call has more than 1 exception; ' ' = call has 1 exception */
	int	call_nbr;			/* Counter for canceling multiple calls */
	int	call_save;			/* Stores grouped with call for next search */
	int	indx;				/* Index into srch screen array */
	short   back_to_original_call;
	struct	excepts	*ex_ptr;
	char	*last_six();
	int	line_num;
	int	cancelling, loops;


	
	if(except_ptr[except_index]->inuse == '1' || except_ptr[except_index]->nbr == 0) {	/* already resolved ? */
		prt_error(UI_EXPT_RESOLVED,"<%s %d>", catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_48, "except line ="), except_index+1);
		resolved_except[except_index] = 1;
		change_line_attr(0,0,except_index+1,A_REVERSE);
		return;
	}

	if(except_ptr[except_index]->fl_ptr != (struct fleets *)0)		/* increment stats */
		bump_stat(except_ptr[except_index]->fl_ptr->fleet_id,RESOLVING_EXCEPTION);

        if (resolve_code == (short)R_CANCEL) {                                  /* is user cancelling a call ? */
	        cancelling = 1;
                for (i = 0; i < FLEET_MAX; i++) {
                        if (fleet[i]->fleet_id == srch_calls[except_index].fleet) {
                                cancel_action = fleet[i]->cancel_action;        /* get cancel action for this fleet */
                                        break;
                        }
                }
                if (cancel_action == '3')
                        ch_call_flag = 1;                                       /* update call rec on disk */
                else {
                        ch_call_flag = 0;                                       /* do not update call rec on disk */
                        prt_error(UI_TO_DISPATCH,catgets(UI_m_catd, UI_1, UI_UI_FTNKEY_26, "<proceed>"));
                }
        }
	else
	  cancelling = 0;

/*
 *      linked multiple calls
 */
	if(except_type == MULT && srch_calls[except_index].grouped_with) {
		mult_call = 0;

                switch (resolve_code) {
                    case R_APPROVE:
			temp_cl_ptr = (struct cisam_cl *)req_buf;
			temp_cl_ptr->nbr = srch_calls[except_index].nbr;		/* Get call # to read in */

			if((db(CALL_FILE_ID, READ_KEYS, &cl_key1, ISEQUAL, 0)) < 0)  {	/* read in call */
				prt_error(UI_DSKREAD,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_49, "<call record>"));
				return;							/* Get out */
			}

			memcpy(&call_struct, req_buf, sizeof(struct cisam_cl));
			own_isrecnum = isrecnum;
                        
			fill_call_rec();

                        if ( !strncmp( temp_cl_ptr->status, PENDING, 5 ) )
                          call_fifo.call_type.time = 1;
                        
			/** old_due_time carries TC activation time for MULT time calls **/
			if ( call_fifo.call_type.multiple && call_fifo.call_type.time )
			  call_fifo.old_due_time = time(0);			
			if(write(fifo_fd, &call_fifo, sizeof(CALL_IPC_REC)) == -1) { 	/* write on fifo failed */
				prt_error(UI_WRITE, catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_50, "FIFO"));
				return;
			}

			if(read_callh_rec(srch_calls[except_index].nbr, MULT)) 
 				update_callh_rec(ACT_APPROVE, srch_calls[except_index].pckup_zone);	/* update hist rec */

			except_fifo.u_prcs_id = pid;
			except_fifo.rec_type = EXCEPT_REQ;
			except_fifo.veh_nbr = 0;
			except_fifo.zone_nbr = 0;	
			except_fifo.old_zone_nbr = 0;	
			except_fifo.old_due_time = 0;	

			call_nbr = srch_calls[except_index].grouped_with;

			except_fifo.resolve_code = (short)R_MULT;
			loops = 0;
			while(call_nbr != srch_calls[except_index].nbr) {
			        ++loops;
				if ( loops > 34 )
				  break;
				ex_ptr = offset->first_except;
				indx = 0;
				while(ex_ptr != NULL) {
					if(call_nbr == ex_ptr->call_nbr)
						break;
/*
 *                              fix for highlighting correct multiple call exceptions
 */
                                        if (except_matches(ex_ptr))
                                                indx++;
                                        ex_ptr = ex_ptr->next;
                                }

				except_fifo.call_nbr = call_nbr;

 				if(read_callh_rec(call_nbr, MULT)) 
 					update_callh_rec(ACT_APPROVE, srch_calls[except_index].pckup_zone);  /* update hist rec */

				if(ex_ptr != NULL) {
					ex_ptr->inuse = '1';
					/* indx only points into the exception list. If there are previously
					   cancelled calls, they will still be on the screen in srch_calls,
					   but no longer in the exception list. Find the place in the srch_calls: */
					line_num = -1;
					for (i = 0; i < SEARCHES_ON_PG; i ++)
					  if (srch_calls[i].nbr == ex_ptr->call_nbr)
					    {
					      line_num = i;
					      break;
					    }
					if (line_num >= 0)
					  resolved_except[line_num] = 1;

					except_fifo.except_ptr = ex_ptr;
					except_fifo.except_nbr = ex_ptr->nbr;
				}
				else
				  continue;

				temp_cl_ptr = (struct cisam_cl *)req_buf;
				temp_cl_ptr->nbr = call_nbr;
				if(db(CALL_FILE_ID, READ_KEYS, &cl_key1, ISEQUAL, 0) < 0) {
					prt_error(UI_DSKREAD,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_51, "<calls file .... FATAL ERROR>"));
					return;
				}

				call_nbr = except_fifo.grouped_with = temp_cl_ptr->grouped_with;

				if(write(fifo_fd, &except_fifo, sizeof(struct excpt_rec)) == -1)
					prt_error(UI_WRITE, catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_52, "<except rec to FIFO>"));

				if (line_num >= 0)
					change_line_attr(0, 0, line_num + 1, A_REVERSE);
			}

			except_ptr[except_index]->inuse = '1';
			resolved_except[except_index] = 1;

/*
 *                      the highlighted call gets sent to dispatch with a resolve code of approve
 *                              so that it gets put into shared memory
 *                              all other members of the group get sent with R_MULT so that they
 *                              don't get put into shared memory
 */
			except_fifo.resolve_code = (short)resolve_code;

			except_fifo.except_ptr = except_ptr[except_index];
			except_fifo.except_nbr = except_ptr[except_index]->nbr;
			except_fifo.call_nbr = srch_calls[except_index].nbr;
			except_fifo.grouped_with = srch_calls[except_index].grouped_with;
			if(write(fifo_fd, &except_fifo, sizeof(struct excpt_rec)) == -1)
				prt_error(UI_WRITE, catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_52, "<except rec to FIFO>"));

			change_line_attr(0, 0, except_index + 1, A_REVERSE);

			prt_error(UI_SENT, catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_53, "<Multiple Call>"));
			return;
			break;

                    case R_DISSAPROVE:
			call_nbr = srch_calls[except_index].nbr;

			while(call_nbr != 0) {
				for(indx = 0; indx < 17; indx++)
					if(call_nbr == srch_calls[indx].nbr)
						srch_calls[indx].grouped_with = 0;
/*
 *                              fix for multiple call
 *                              call history not updated on unlink of calls
 
 				if(read_callh_rec(call_nbr, MULT)) 
 					update_callh_rec(ACT_DISAPPROVE, 0);		/* update hist rec *
 */

				temp_cl_ptr = (struct cisam_cl *)req_buf;
				temp_cl_ptr->nbr = call_nbr;
				if(db(CALL_FILE_ID, READ_KEYS, &cl_key1, ISEQUAL+ISLOCK, 0) < 0) {
					prt_error(UI_DSKREAD,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_51, "<calls file .... FATAL ERROR>"));
					return;
				}

				call_save = temp_cl_ptr->grouped_with;
				temp_cl_ptr->grouped_with = 0L;

				if(db(CALL_FILE_ID,UPDATE_REC,&cl_key1,0,0) < 0) {
					prt_error(UI_WRITE,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_54, "<error updating calls FATAL ERROR>"));
					return;
				}

				if(db(CALL_FILE_ID,RELEASE,&cl_key1,0,0) < 0)
					prt_error(UI_WRITE,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_55, "<error releasing lock calls record>"));

				call_nbr = call_save;
				if(call_nbr == srch_calls[except_index].nbr)
					call_nbr = 0L;
			}
			dsp_cl_detail(except_index + 1);
			prt_error(UI_UNLINKED, "");
			return;
			break;

                    case R_CANCEL:
/*
 *                      add call history for each call in group
 *                              and make exception
 *                      then continue to process single call
 */
                        except_fifo.u_prcs_id = pid;
                        except_fifo.rec_type = EXCEPT_REQ;
                        except_fifo.veh_nbr = 0;
                        except_fifo.zone_nbr = 0;
			except_fifo.old_zone_nbr = 0;	
			except_fifo.old_due_time = 0;	

                        call_nbr = srch_calls[except_index].grouped_with;
                        while (call_nbr != srch_calls[except_index].nbr)
                          {
/*
 *                              find exception index for call number
 */
                                ex_ptr = offset->first_except;
                                indx = 0;
                                while(ex_ptr != NULL) {
                                        if(call_nbr == ex_ptr->call_nbr)
                                                break;
                                        if (except_matches(ex_ptr))
                                                indx++;
                                        ex_ptr = ex_ptr->next;
                                }

/*
 *                              add to call history
 */
                                add_noexcpt_callh(indx, resolve_code, action_code, 0);

/*
 *                              make exception
 */
                                if(ex_ptr != NULL)
                                  {
                                        ex_ptr->inuse = '1';
					/* indx only points into the exception list. If there are previously
					   cancelled calls, they will still be on the screen in srch_calls,
					   but no longer in the exception list. Find the place in the srch_calls: */
					line_num = -1;
					for (i = 0; i < SEARCHES_ON_PG; i ++)
					  if (srch_calls[i].nbr == ex_ptr->call_nbr)
					    {
					      line_num = i;
					      break;
					    }
					if (line_num >= 0)
					  resolved_except[line_num] = 1;

                                        except_fifo.except_ptr = ex_ptr;
                                        except_fifo.except_nbr = ex_ptr->nbr;
                                  }


                                temp_cl_ptr = (struct cisam_cl *)req_buf;
                                temp_cl_ptr->nbr = call_nbr;
                                if(db(CALL_FILE_ID, READ_KEYS, &cl_key1, ISEQUAL, 0) < 0)
				  {
                                  
                                        prt_error(UI_DSKREAD,"<calls file .... FATAL ERROR>");
                                        return;
                                  }

                                call_nbr = except_fifo.grouped_with = temp_cl_ptr->grouped_with;

                                except_fifo.resolve_code = (short)resolve_code;
                                except_fifo.call_nbr = temp_cl_ptr->nbr;

                                if(write(fifo_fd, &except_fifo, sizeof(struct excpt_rec)) == -1)
                                        prt_error(UI_WRITE, "<except rec to FIFO>");

                                if (line_num >= 0)
                                        change_line_attr(0, 0, line_num + 1, A_REVERSE);
                          }
                        break;

                    default:
                        break;
                  }
          }

/*
 *      unlinked multiple call
 */
/*
 *      fix for unlinked multiple call APPROVE
 *              error, multiple call must be linked for approve
 */
        if (except_ptr[except_index]->type == MULT &&
            !srch_calls[except_index].grouped_with &&
            !strcmp(action_code, ACT_APPROVE))
          {
                prt_error(UI_UNLINKED, catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_68, 
						"and cannot be approved as a Multiple Call"));
                return;
          }
/*
 *      non-multiple call processing
 */

	except_ptr[except_index]->inuse = '1';					/* we can resolve this exception */
	resolved_except[except_index] = 1;
	
	if (except_ptr[except_index]->type == REQ_TO_TLK && except_ptr[except_index]->call_ptr == NULL && 
	    except_ptr[except_index]->call_nbr == NULL)	/* if REQ_TO_TALK excep; no need to add/read call history record; */
		ch_hist_flag = 0;				/*  so set flag to 0 */

	if(ch_call_flag && resolve_code != (short)R_CANCEL && resolve_code != R_M_MAN_VEH) {  /* update call record on disk */
		if (resolve_code == (short)R_MAN_VEH) {		/* MAN VEH assignment ? */
			srch_calls[except_index].veh_nbr = (short)zv_num;
			if (zv_num > 0) {
				strcpy(srch_calls[except_index].status,ASSIGNED);
				srch_calls[except_index].pri_status = 64;		/* call is now assigned */
				srch_calls[except_index].dispd_date_time = (int)time(NULL);
				get_pkup_date(date_str);
				get_pkup_time(time_str);
				strcpy(srch_calls[except_index].dispd_date,date_str);
				strcpy(srch_calls[except_index].dispd_time,time_str);
				srch_calls[except_index].dispatched_by = (short)uid;
			}
			else {
				strcpy(srch_calls[except_index].status,UNASSGND);	/* unassignment */
				srch_calls[except_index].pri_status = srch_calls[except_index].priority;
				srch_calls[except_index].dispd_date_time = NULL;
				srch_calls[except_index].dispd_date[0] = NULL;
				srch_calls[except_index].dispd_time[0] = NULL;
				srch_calls[except_index].dispatched_by = 0;
				srch_calls[except_index].pckup_date_time = NULL;
				srch_calls[except_index].pckup_time[0] = NULL;
				srch_calls[except_index].pckup_date[0] = NULL;
				srch_calls[except_index].close_date_time = NULL;
				srch_calls[except_index].close_time[0] = NULL;
				srch_calls[except_index].close_date[0] = NULL;
				srch_calls[except_index].veh_nbr = 0;
				srch_calls[except_index].drv_id = 0;
			}
		}
		else if (resolve_code == (short)R_DISPATCH) {		/* this is an action, to dispach to starat the call again */
			strcpy(srch_calls[except_index].status,UNASSGND);
			srch_calls[except_index].pri_status = srch_calls[except_index].priority;
			srch_calls[except_index].dispd_date_time = NULL;
			srch_calls[except_index].dispd_time[0] = NULL;
			srch_calls[except_index].dispd_date[0] = NULL;
			srch_calls[except_index].pckup_date_time = NULL;
			srch_calls[except_index].pckup_time[0] = NULL;
			srch_calls[except_index].pckup_date[0] = NULL;
			srch_calls[except_index].close_date_time = NULL;
			srch_calls[except_index].close_time[0] = NULL;
			srch_calls[except_index].close_date[0] = NULL;
			srch_calls[except_index].veh_nbr = 0;
			srch_calls[except_index].dispatched_by = 0;
			srch_calls[except_index].drv_id = 0;
		}
		else {				/* depending upon the type of exception, update the call record accordingly */
			switch (except_type)  {
				case UNZONED:						/* update this record with new zone # */
					if (srch_calls[except_index].pckup_zone <= 0)
						update_unzoned(srch_calls[except_index].fleet, srch_calls[except_index].nbr,
							       zv_num, uid);
					srch_calls[except_index].pckup_zone = zv_num;
					srch_calls[except_index].last_changed_by = uid;
					break;

								/* update status to completed, add priority = 64, and add the */
				case WAKE_UP:			/* various times ... for APPROVE key entered */
					strcpy(srch_calls[except_index].status,COMPLETE);
					srch_calls[except_index].pri_status = (short)64;
					srch_calls[except_index].dispd_date_time = (int)time(NULL);
					srch_calls[except_index].date_time_in = srch_calls[except_index].dispd_date_time;
					srch_calls[except_index].pckup_date_time = srch_calls[except_index].dispd_date_time;
					srch_calls[except_index].close_date_time = srch_calls[except_index].dispd_date_time;
					get_pkup_date(date_str);
					get_pkup_time(time_str);
					strcpy(srch_calls[except_index].dispd_date,date_str);
					strcpy(srch_calls[except_index].dispd_time,time_str);
					strcpy(srch_calls[except_index].pckup_date,date_str);
					strcpy(srch_calls[except_index].pckup_time,time_str);
					strcpy(srch_calls[except_index].close_date,date_str);
					strcpy(srch_calls[except_index].close_time,time_str);
					break;

/*
 *                              fix for disapproving unlinked multiple call
 *                                      set extended to not multiple
 */
                                case MULT:
                                        set_multiple_call_type(srch_calls[except_index].extended_type, 0);
                                        break;

				case CALL_NO_MSG:
				case CALL_W_MSG:
				case HOL_TM_CALL:
				case EXTRA_TAXIS:
					if (resolve_code == R_DISSAPROVE){
						strcpy(srch_calls[except_index].status,CANCELD);
						srch_calls[except_index].pri_status = (short)64;
					}
					break;

				case CALL_CANCEL:		/* for approve key */
					if (resolve_code == R_APPROVE){
						strcpy(srch_calls[except_index].status,CANCELD);
						srch_calls[except_index].pri_status = (short)64;
						call_save = srch_calls[except_index].grouped_with;
						srch_calls[except_index].grouped_with = 0;
					}
					break;
				
				default:
				break;
		
			}

		}

		temp_cl_ptr = (struct cisam_cl *)req_buf;
		temp_cl_ptr->nbr = srch_calls[except_index].nbr;
		if(db(CALL_FILE_ID,READ_KEYS,&cl_key1,ISEQUAL+ISLOCK,0) < 0) {			/* try to read rec by call # */
			prt_error(UI_DSKREAD,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_51, "<calls file .... FATAL ERROR>"));			/* db failed, no record found */
			except_ptr[except_index]->inuse = '0';
			resolved_except[except_index] = 0;
			change_line_attr(0,0,except_index+1,A_REVERSE|A_BOLD);
			return;
		}

		memcpy(req_buf,(char *)&srch_calls[except_index],sizeof(struct cisam_cl));	/* make changes on disk record */
		if (db(CALL_FILE_ID,UPDATE_REC,&cl_key1,0,0) < 0) {
			prt_error(UI_WRITE,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_57, "<error updating calls record... FATAL ERROR>"));		/* db failed */
			except_ptr[except_index]->inuse = '0';
			resolved_except[except_index] = 0;
			change_line_attr(0,0,except_index+1,A_REVERSE|A_BOLD);
			return;
		}

		if(db(CALL_FILE_ID,RELEASE,&cl_key1,0,0) < 0)
			prt_error(UI_WRITE,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_58, "<error releasing lock on calls record>"));			/* db failed */
	}

/*
 *      multiple call vehicle assignment
 */

	if(resolve_code == R_M_MAN_VEH) {		/* MAN VEH assignment ? */
		call_nbr = srch_calls[except_index].nbr;

		except_fifo.except_ptr = except_ptr[except_index];
		except_fifo.except_nbr = except_ptr[except_index]->nbr;

		while(call_nbr != 0) {
			for(indx = 0; indx < 17; indx++) {
				if(call_nbr == srch_calls[indx].nbr)
					break;
			}

/*
 *                      if call is on screen ==> it is in memory
 */

			if(indx < 17) {
				srch_calls[except_index].veh_nbr = (short)zv_num;

                                change_line_attr(0, 0, indx + 1, A_REVERSE);
 
				if(zv_num > 0) {
					strcpy(srch_calls[indx].status,ASSIGNED);
					srch_calls[indx].pri_status = 64;		/* call is now assigned */
					srch_calls[indx].dispd_date_time = (int)time(NULL);
					get_pkup_date(date_str);
					get_pkup_time(time_str);
					strcpy(srch_calls[indx].dispd_date,date_str);
					strcpy(srch_calls[indx].dispd_time,time_str);
					srch_calls[indx].dispatched_by = (short)uid;
				}
				else {
					strcpy(srch_calls[indx].status,UNASSGND);	/* unassignment */
					srch_calls[indx].pri_status = srch_calls[indx].priority;
					srch_calls[indx].dispd_date_time = NULL;
					srch_calls[indx].dispd_date[0] = NULL;
					srch_calls[indx].dispd_time[0] = NULL;
					srch_calls[indx].dispatched_by = 0;
					srch_calls[indx].pckup_date_time = NULL;
					srch_calls[indx].pckup_time[0] = NULL;
					srch_calls[indx].pckup_date[0] = NULL;
					srch_calls[indx].close_date_time = NULL;
					srch_calls[indx].close_time[0] = NULL;
					srch_calls[indx].close_date[0] = NULL;
					srch_calls[indx].veh_nbr = 0;
					srch_calls[indx].drv_id = 0;
				} 
			}

/*
 *                      call is not on screen ==> bring it into memory
 */

			except_fifo.u_prcs_id = pid;							/* write to dispatch */
			except_fifo.rec_type = EXCEPT_REQ;
			except_fifo.resolve_code = (short)resolve_code;
			except_fifo.grouped_with = 0;
			except_fifo.veh_nbr = (short)zv_num;
			except_fifo.zone_nbr = 0;	
			except_fifo.old_zone_nbr = 0;	
			except_fifo.old_due_time = 0;	

			temp_cl_ptr = (struct cisam_cl *)req_buf;
			temp_cl_ptr->nbr = call_nbr;
			if(db(CALL_FILE_ID, READ_KEYS, &cl_key1, ISEQUAL+ISLOCK, 0) < 0) {	/* try to read rec by call # */
				prt_error(UI_DSKREAD,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_51, "<calls file .... FATAL ERROR>"));		/* db failed, no record found */
				except_ptr[except_index]->inuse = '0';
				resolved_except[except_index] = 0;
				change_line_attr(0,0,except_index+1,A_REVERSE|A_BOLD);
				return;
			}

			except_fifo.call_nbr = call_nbr;			/* exception is not associated with a call */
			call_nbr = temp_cl_ptr->grouped_with;

			temp_cl_ptr->veh_nbr = (short)zv_num;

			if(zv_num > 0) {
				strcpy(temp_cl_ptr->status,ASSIGNED);
				temp_cl_ptr->pri_status = 64;		/* call is now assigned */
				temp_cl_ptr->dispd_date_time = (int)time(NULL);
				get_pkup_date(date_str);
				get_pkup_time(time_str);
				strcpy(temp_cl_ptr->dispd_date,date_str);
				strcpy(temp_cl_ptr->dispd_time,time_str);
				temp_cl_ptr->dispatched_by = (short)uid;
			}
			else {
				strcpy(temp_cl_ptr->status,UNASSGND);	/* unassignment */
				temp_cl_ptr->pri_status = temp_cl_ptr->priority;
				temp_cl_ptr->dispd_date_time = NULL;
				temp_cl_ptr->dispd_date[0] = NULL;
				temp_cl_ptr->dispd_time[0] = NULL;
				temp_cl_ptr->dispatched_by = 0;
				temp_cl_ptr->pckup_date_time = NULL;
				temp_cl_ptr->pckup_time[0] = NULL;
				temp_cl_ptr->pckup_date[0] = NULL;
				temp_cl_ptr->close_date_time = NULL;
				temp_cl_ptr->close_time[0] = NULL;
				temp_cl_ptr->close_date[0] = NULL;
				temp_cl_ptr->veh_nbr = 0;
				temp_cl_ptr->drv_id = 0;
			} 

			if(db(CALL_FILE_ID,UPDATE_REC,&cl_key1,0,0) < 0) {
				prt_error(UI_WRITE,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_57, "<error updating calls record... FATAL ERROR>"));		/* db failed */
				except_ptr[except_index]->inuse = '0';
				resolved_except[except_index] = 0;
				change_line_attr(0,0,except_index+1,A_REVERSE|A_BOLD);
				return;
			}

			if(db(CALL_FILE_ID,RELEASE,&cl_key1,0,0) < 0)
				prt_error(UI_WRITE,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_58, "<error releasing lock on calls record>"));	/* db failed */

                        if(read_callh_rec(call_nbr, MULT))
                                update_callh_rec(action_code, srch_calls[except_index].pckup_zone);  /* update hist rec */

			if(write(fifo_fd, &except_fifo, sizeof(struct excpt_rec)) == -1) 	/* write to FIFO */
				prt_error(UI_WRITE, catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_52, "<except rec to FIFO>"));

			except_fifo.except_ptr = NULL;
			except_fifo.except_nbr = 0;

			if(call_nbr == srch_calls[except_index].nbr) 				/* Don't do first call again */
				call_nbr = 0L;
 
                        prt_error(UI_SENT, "<Multiple Call>");
		}
		goto done;
	}

/*
 *      approving a linked multiple call CANCEL
 */
	/* APPROVE CANCEL MULT CALL or cancel call if cancel_action='3' (for automatic) */
	if(resolve_code == R_CAN_MULT || (resolve_code == R_CANCEL && cancel_action == '3')) {
		call_nbr = srch_calls[except_index].nbr;

		except_fifo.except_ptr = except_ptr[except_index];
		except_fifo.except_nbr = except_ptr[except_index]->nbr;

		while(call_nbr != 0) {
			ex_ptr = offset->first_except;
			indx = 0;
			while(ex_ptr != NULL) {
				if(call_nbr == ex_ptr->call_nbr)
					break;
/*
 *                      fix for highlighting correct multiple call exceptions
 */
                                if (except_matches(ex_ptr))
					indx++;
				ex_ptr = ex_ptr->next;
			}

			if(ex_ptr != NULL) {
			        except_fifo.except_ptr = ex_ptr;
				except_fifo.except_nbr = ex_ptr->nbr;
				ex_ptr->inuse = '1';
				/* indx only points into the exception list. If there are previously
				   cancelled calls, they will still be on the screen in srch_calls,
				   but no longer in the exception list. Find the place in the srch_calls: */
				line_num = -1;
				for (i = 0; i < SEARCHES_ON_PG; i ++)
				  if (srch_calls[i].nbr == ex_ptr->call_nbr)
				    {
				      line_num = i;
				      break;
				    }
				if (line_num >= 0)
				  resolved_except[line_num] = 1;
			}

/*
 *                      fix for updating call history on a mult cancel
 *                      approve the cancel
 */
			/* update hist rec */

			/* For mult cancel */
                        if (resolve_code == R_CAN_MULT)
			  {
			    if (read_callh_rec(call_nbr, CALL_CANCEL))
			      update_callh_rec(ACT_APPROVE, srch_calls[except_index].pckup_zone);
			  }
			/* For normal cancel */
			else
			  add_noexcpt_callh(except_index, resolve_code, action_code, zv_num);

			if(line_num >= 0) {
				strcpy(srch_calls[line_num].status, CANCELD);
				srch_calls[line_num].pri_status = (short)64;
				srch_calls[line_num].grouped_with = 0;
				change_line_attr(0, 0, line_num + 1, A_REVERSE);
			}

			except_fifo.u_prcs_id = pid;							/* write to dispatch */
			except_fifo.rec_type = EXCEPT_REQ;
			except_fifo.resolve_code = (short)resolve_code;
			except_fifo.grouped_with = 0;
			except_fifo.veh_nbr = 0;
			except_fifo.zone_nbr = 0;	
			except_fifo.old_zone_nbr = 0;	
			except_fifo.old_due_time = 0;	
			except_fifo.call_nbr = call_nbr;

			temp_cl_ptr = (struct cisam_cl *)req_buf;
			temp_cl_ptr->nbr = call_nbr;
			if(db(CALL_FILE_ID, READ_KEYS, &cl_key1, ISEQUAL+ISLOCK, 0) < 0) {	/* try to read rec by call # */
				prt_error(UI_DSKREAD,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_51, "<calls file .... FATAL ERROR>"));		/* db failed, no record found */
				except_ptr[except_index]->inuse = '0';
				resolved_except[except_index] = 0;
				change_line_attr(0,0,except_index+1,A_REVERSE|A_BOLD);
				return;
			}

			call_nbr = temp_cl_ptr->grouped_with;
			temp_cl_ptr->grouped_with = 0L;
			strcpy(temp_cl_ptr->status, CANCELD);
			temp_cl_ptr->pri_status = (short)64;

			if(db(CALL_FILE_ID,UPDATE_REC,&cl_key1,0,0) < 0) {
				prt_error(UI_WRITE,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_57, "<error updating calls record... FATAL ERROR>"));		/* db failed */
				except_ptr[except_index]->inuse = '0';
				resolved_except[except_index] = 0;
				change_line_attr(0,0,except_index+1,A_REVERSE|A_BOLD);
				return;
			}

			if(db(CALL_FILE_ID,RELEASE,&cl_key1,0,0) < 0)
				prt_error(UI_WRITE,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_58, "<error releasing lock on calls record>"));	/* db failed */

			if ( (0) )
			  {
			    DeleteTimeCall( temp_cl_ptr->nbr );
			  }
			
			if(write(fifo_fd, &except_fifo, sizeof(struct excpt_rec)) == -1) 	/* write to FIFO */
				prt_error(UI_WRITE, catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_52, "<except rec to FIFO>"));

			except_fifo.except_ptr = NULL;
			except_fifo.except_nbr = 0;

			if(call_nbr == srch_calls[except_index].nbr) 				/* Don't do first call again */
				call_nbr = 0L;
		}
		goto done;
	}

/*
 *      disapproving a linked multiple call CANCEL
 */
        if (except_type == CALL_CANCEL &&
            resolve_code == R_DISSAPROVE &&
            srch_calls[except_index].grouped_with)
 
          {
                except_fifo.u_prcs_id = pid;
                except_fifo.rec_type = EXCEPT_REQ;
                except_fifo.veh_nbr = 0;
                except_fifo.zone_nbr = 0;
		except_fifo.old_zone_nbr = 0;	
		except_fifo.old_due_time = 0;	
 
                back_to_original_call = 0;
                call_nbr = srch_calls[except_index].grouped_with;
                while (!back_to_original_call)
                  {
                        ex_ptr = offset->first_except;
                        indx = 0;
                        while(ex_ptr != NULL) {
                                if(call_nbr == ex_ptr->call_nbr)
                                        break;
                                if (except_matches(ex_ptr))
                                        indx++;
                                ex_ptr = ex_ptr->next;
                        }


                        if(read_callh_rec(call_nbr,except_ptr[except_index]->type))
                                update_callh_rec(action_code, 0);

                        if(ex_ptr != NULL)
                          {
                                ex_ptr->inuse = '1';
				/* indx only points into the exception list. If there are previously
				   cancelled calls, they will still be on the screen in srch_calls,
				   but no longer in the exception list. Find the place in the srch_calls: */
				line_num = -1;
				for (i = 0; i < SEARCHES_ON_PG; i ++)
				  if (srch_calls[i].nbr == ex_ptr->call_nbr)
				    {
				      line_num = i;
				      break;
				    }
				if (line_num >= 0)
				  resolved_except[line_num] = 1;

                                except_fifo.except_ptr = ex_ptr;
                                except_fifo.except_nbr = ex_ptr->nbr;
                          }

                        temp_cl_ptr = (struct cisam_cl *)req_buf;
                        temp_cl_ptr->nbr = call_nbr;
                        if(db(CALL_FILE_ID, READ_KEYS, &cl_key1, ISEQUAL, 0) < 0)
                          {
                                prt_error(UI_DSKREAD,"<calls file .... FATAL ERROR>");
                                return;
                          }

                        if (call_nbr == srch_calls[except_index].nbr)
                                back_to_original_call = 1;

                        call_nbr = except_fifo.grouped_with = temp_cl_ptr->grouped_with;

                        except_fifo.resolve_code = (short)resolve_code;
                        except_fifo.call_nbr = temp_cl_ptr->nbr;

                        if(write(fifo_fd, &except_fifo, sizeof(struct excpt_rec)) == -1)
                                prt_error(UI_WRITE, "<except rec to FIFO>");

                        if(line_num >= 0)
                                change_line_attr(0, 0, line_num + 1, A_REVERSE);
                  }

                goto done;
          }



	if (ch_hist_flag) {						/* updat the call history file for this exception */
		if(resolve_code ==R_CALLBACK || resolve_code ==R_RESEND || resolve_code ==R_DISPATCH || resolve_code ==R_CANCEL ||
		   (resolve_code == R_SUP_HOLD && except_ptr[except_index]->call_ptr != (struct calls *)0 &&
		   except_ptr[except_index]->call_ptr->status.sup_hold == 0)) {
			add_noexcpt_callh(except_index,resolve_code,action_code,zv_num);
		}
 		else if (resolve_code == R_SUP_HOLD  && except_ptr[except_index]->call_ptr != (struct calls *)0 &&
 			except_ptr[except_index]->call_ptr->status.sup_hold == 1) {		/* already on HOLD ? */
 			if(read_callh_rec(srch_calls[except_index].nbr,SUP_HOLD)) 
 				update_callh_rec(action_code,0);				/* take it off HOLD */
 		}
		else if(read_callh_rec(srch_calls[except_index].nbr,except_ptr[except_index]->type)) {	/* got rec from disk */
			switch (resolve_code)
			{
				case R_MAN_VEH:
				case R_M_MAN_VEH:
				case R_MAN_ZONE:
				update_callh_rec(action_code,zv_num);			/* should update veh # in call hist rec */
				break;
			default:
				update_callh_rec(action_code,0);
				break;
			}
				
			if( (srch_calls[except_index].grouped_with && except_ptr[except_index]->type != NO_SHOW) &&
			    (srch_calls[except_index].grouped_with && except_ptr[except_index]->type != NS_SHORT) )
			{
				call_nbr = srch_calls[except_index].grouped_with;

				while(call_nbr != srch_calls[except_index].nbr) {	
					temp_cl_ptr = (struct cisam_cl *)req_buf;
					temp_cl_ptr->nbr = call_nbr;
					if(db(CALL_FILE_ID, READ_KEYS, &cl_key1, ISEQUAL+ISLOCK, 0) < 0) {
						prt_error(UI_DSKREAD,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_51, "<calls file .... FATAL ERROR>"));		
						return;
					}

					call_save = temp_cl_ptr->grouped_with;

					read_callh_rec(call_nbr, except_ptr[except_index]->type);
					update_callh_rec(action_code, 0);

					if(db(CALL_FILE_ID,RELEASE,&cl_key1,0,0) < 0)
						prt_error(UI_WRITE,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_58, "<error releasing lock on calls record>"));		

					call_nbr = call_save;
				}
			}
		}
	}

							/* for emergency exception, need to update the vehicle record on disk; */
	if (except_type == EMERGENCY) {			/* also upadte the emergency log file and the driver file */
		if (update_veh_rec(except_ptr,except_index) == -1) {				/* cannot update vehicle rec */
			prt_error(UI_WRITE,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_59, "<error updating vehicle record... exception resolved>"));		/* db failed */
		}
	}
	
	if (except_type == DRV_MSG) {		/* for DRV_MSG ( driver_msg) exception, need to update the messagae log file */
		if (msg_on_display) {	 			/* check if message window should be displayed or erased */
			msg_on_display = 0; 			/* there is a message window on display */
			delete_message_window();
		}
		if (resolve_code == (short)R_APPROVE)  
			message_apprvd_deny((int)except_ptr[except_index]->info, SUPER_APPROVED);
		else 
			message_apprvd_deny((int)except_ptr[except_index]->info, SUPER_DENIED);	/* disapproved */
	}

	except_fifo.u_prcs_id = pid;							/* write to dispatch */
	except_fifo.rec_type = EXCEPT_REQ;
	except_fifo.except_ptr = except_ptr[except_index];
	except_fifo.resolve_code = (short) resolve_code;
	except_fifo.except_nbr = except_ptr[except_index]->nbr;

	if (except_ptr[except_index]->call_ptr != (struct calls *)0 || except_ptr[except_index]->call_nbr > (long)0) {
		except_fifo.call_nbr = srch_calls[except_index].nbr;		/* exception is associated with a call */

		if ( srch_calls[except_index].grouped_with > 1 )
		  except_fifo.grouped_with = srch_calls[except_index].grouped_with;
		else
		  except_fifo.grouped_with = 0;
	}
	else {
		except_fifo.call_nbr = 0;					/* exception is not associated with a call */
		except_fifo.grouped_with = 0;
	}

	if (resolve_code == (short)R_MAN_VEH)					/* set the vehicle # */
		except_fifo.veh_nbr = zv_num;					/* its manual veh assignment */
	else 
		except_fifo.veh_nbr = 0;

	if (resolve_code == (short)R_MAN_ZONE)					/* set the zone # */
		except_fifo.zone_nbr = zv_num;
	else 
		except_fifo.zone_nbr = 0;	

	except_fifo.old_zone_nbr = 0;	
	except_fifo.old_due_time = 0;	

	if(write(fifo_fd, &except_fifo, sizeof(struct excpt_rec)) == -1) 	/* write to FIFO */
		prt_error(UI_WRITE, catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_52, "<except rec to FIFO>"));
									/* change this exception line to reverse dim mode */
									/* reformat line with new zone # in sum_ptr[except_index] */
done:
	i = except_index;
	if (except_ptr[except_index]->type != ETA && except_ptr[except_index]->type != MESSAGE &&
	    except_ptr[except_index]->call_ptr != (struct calls *)0 ) { 
		strncpy(temp_comment,srch_calls[i].pckup_adr_cmnt,20);
		temp_comment[20] = '\0';
		exptr = offset->except_desc + except_ptr[i]->type - 1;

		/* Cancelled is a special case */
		if (cancelling)
		  strcpy(exdesc, CANCELD);
		else
		  strcpy(exdesc, exptr->short_desc);

		if (srch_calls[i].pckup_zone)
			sprintf(temp_zone_nbr,"%-03d",srch_calls[i].pckup_zone);
		else
			sprintf(temp_zone_nbr,"   ");

		if (srch_calls[i].veh_nbr)
			sprintf(temp_veh_nbr,"%-03d",srch_calls[i].veh_nbr);
		else
			sprintf(temp_veh_nbr,"   ");

		/* If call has more than one exception, or call has been cancelled and the fleet
		   does not do auto cancelling (cancel_action != '3') and the extra exception has
		   not yet appeared in the status bits (but it will as soon as dispatch processes
		   the message from the FIFO) then put a '*' */
		if ( call_with_more_excepts(except_ptr[i]) || (cancelling && cancel_action != '3'))
			call_except_num = '*';	/* call has more than 1 exception */
		else 
			call_except_num = ' ';	/* call has 1 exception */

#ifdef SCANDINAVIA
		temp_comment[18] = '\0';
		if (0)
			sprintf(sum_ptr[i],
			" %-2d %c  %-8s%c%-6s  %-5s %-4s %-3s %-3s %20s %-6d               %-20s %-18s  %6s",
			i + 1, srch_calls[i].fleet, exdesc, call_except_num, srch_calls[i].due_time, srch_calls[i].extended_type,
			temp_veh_nbr, temp_zone_nbr, srch_calls[i].pckup_city,
			srch_calls[i].pckup_str_name, srch_calls[i].pckup_str_nbr,
			srch_calls[i].passenger_name, temp_comment, last_six(srch_calls[i].nbr));
		else
			sprintf(sum_ptr[i],
			" %-2d %c  %-8s%c%-6s  %-5s %-4s %-3s %-3s %20s %-6d%c              %-20s %-18s  %6s",
			i + 1, srch_calls[i].fleet, exdesc, call_except_num, srch_calls[i].due_time, srch_calls[i].extended_type,
			temp_veh_nbr, temp_zone_nbr, srch_calls[i].pckup_city,
			srch_calls[i].pckup_str_name, srch_calls[i].pckup_str_nbr, srch_calls[i].pckup_str_suf,
			srch_calls[i].passenger_name, temp_comment, last_six(srch_calls[i].nbr));
#else
		if (0)
			sprintf(sum_ptr[i]," %-2d %c  %-8s%c%-6s  %-5s %-4s %-3s %-3s %-6d  %-2s %-20s %-3s %-2s    %-20s %-20s%6s",
			i + 1, srch_calls[i].fleet, exdesc, call_except_num, srch_calls[i].due_time, srch_calls[i].extended_type,
			temp_veh_nbr, temp_zone_nbr, srch_calls[i].pckup_city,
			srch_calls[i].pckup_str_nbr, srch_calls[i].pckup_str_suf, srch_calls[i].pckup_pre_dir,
			srch_calls[i].pckup_str_name, srch_calls[i].pckup_str_type, srch_calls[i].pckup_post_dir,
			srch_calls[i].passenger_name, temp_comment, last_six(srch_calls[i].nbr));
		else
			sprintf(sum_ptr[i]," %-2d %c  %-8s%c%-6s  %-5s %-4s %-3s %-3s %-6d%c %-2s %-20s %-3s %-2s    %-20s %-20s%6s",
			i + 1, srch_calls[i].fleet, exdesc, call_except_num, srch_calls[i].due_time, srch_calls[i].extended_type,
			temp_veh_nbr, temp_zone_nbr, srch_calls[i].pckup_city,
			srch_calls[i].pckup_str_nbr, srch_calls[i].pckup_str_suf, srch_calls[i].pckup_pre_dir,
			srch_calls[i].pckup_str_name, srch_calls[i].pckup_str_type, srch_calls[i].pckup_post_dir,
			srch_calls[i].passenger_name, temp_comment, last_six(srch_calls[i].nbr));
#endif
		sum_ptr[i][strlen(sum_ptr[i])] = ' ';	/* strip off null char since we want a 132 byte string */
		sum_ptr[i][COLS] = '\0';
	}

	change_line_attr(0,0,except_index+1,A_REVERSE);

	return(DONE);
}

/******************************************************************************************/
/* clear_detail_area: clears the detail area  						  */
/******************************************************************************************/
clear_detail_area()
{

	attrset(A_REVERSE | A_BOLD);

	mvaddstr(1,0,Spaces);
	mvaddstr(2,0,Spaces);
	mvaddstr(3,0,Spaces);
	mvaddstr(4,0,Spaces);
	attrset(A_REVERSE);

	/* print the labels */
	mvaddstr(3,1,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_2, " Line:   "));
    	mvaddstr(3,DUE_EXCEPT_COL-1,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_3, " Due  Disp'd Veh# Pickup Close  Pri    Date     Attr "));

	attrset(0);

	/* do not refresh */

}  /* end clear_detail_area() */

/********************************************************************************************************/
/* update_veh_rec: updates the vehicle file for emergency exceptions; by setting the			*/
/*		last_emerg_off time to the time the exception is resolved				*/
/********************************************************************************************************/
int update_veh_rec(except_ptr,except_index)
struct excepts *except_ptr[];
int except_index;
{
	struct cisam_vh *vh_ptr;	

	vh_ptr = (struct cisam_vh *)req_buf;
	vh_ptr->fleet = except_ptr[except_index]->fl_ptr->fleet_id;
	vh_ptr->nbr = (short)except_ptr[except_index]->veh_nbr;

	if(db(VEHICLE_FILE_ID, READ_KEYS, &veh_key2, ISEQUAL+ISLOCK, 0) < 0) {
		prt_error(UI_DSKREAD,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_60, "<invalid vehicle number>"));
		return(-1);
	}

	/* update emergency time off field in the vehicle file */
	vh_ptr->last_emerg_off = (int)time(NULL);

	if(db(VEHICLE_FILE_ID, UPDATE_REC, &veh_key2, 0, 0) < 0) {
		prt_error(UI_WRITE,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_61, "<error updating vehicle record>"));
		return(-1);
	}

	if(db(VEHICLE_FILE_ID,RELEASE,&veh_key2,0,0) < 0) 
		prt_error(UI_WRITE,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_62, "<error releasing lock on vehicle record>"));
	
	/* update emergency log file and the driver file */
	writer_emer_off(vh_ptr->fleet, vh_ptr->last_emerg_on, (short)uid, vh_ptr->last_emerg_off, except_index);
}

/****************************************************************************************/
/* display_mesg: displays the message window for call with msg exception; also		*/
/*		clears the msg window while moving the highlight from one		*/
/*		exception line to the other						*/
/****************************************************************************************/
display_mesg(msg_num)
int msg_num;
{
  struct cisam_ml ml;
  
  if (message_log_read(msg_num, &ml) < 0) 
      prt_error(UI_FATAL,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_63, "<problems with MSGLOG file>"));
  
  /* got a valid message */
  else
  {
      
      mvwprintw(mesg_wind, 1, 1, "%s %d %s %-6s %-8s %s %c %s", 
		catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_64, "From"),
		ml.from, catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_65, "at"),
		ml.date_sent, ml.time_sent, catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_66, "To:"), ml.to_type,
		ml.to_selection);
      mvwprintw(mesg_wind, 2, 1, "%.100s", ml.msg_line1);
      mvwprintw(mesg_wind, 3, 1, "%.100s", ml.msg_line2);
      
      wrefresh(mesg_wind);
  }
}

display_emerg_mesg( EXCEPT_EMERG *pExceptEmerg )
{
      
  mvwprintw(mesg_wind, 1, 1, "%s ",
            "-Current Broadcast Settings-");
            
  mvwprintw(mesg_wind, 2, 1, "%s %c", "Active:  ", pExceptEmerg->active );
  mvwprintw(mesg_wind, 3, 1, "%s %d", "Radius:  ", pExceptEmerg->radius );
      
  wrefresh(mesg_wind);

}


/************************************************************************************************/
/* reset_sel_crit: resets the exception selection criteria as entered by the user.		*/
/*		Returns - 0   if string entered is incorrect 					*/
/*			  1   if exception criteria is chanegd					*/
/************************************************************************************************/
int reset_sel_crit(s_or_i,ptr)
char s_or_i;				/* 'S' = group #'s entered; 'I' = type #'s entered */
char *ptr;				/* pointerv to char after 'S'/'I' in read_buf[] */
{
	int num_ary[10];		/* for saving group/type #'s entered */
	char *wptr;			/* walk ptr */
	int i;				/* scratch variable */
	int type_ary[EXC_TYPES];	/* for saving exception types selected  */
	char *tptr;

	wptr = ptr;				/* if string entered has a syntax error, then return 0 */
	while(*wptr) {
		if (!isdigit(*wptr) && *wptr != ' ' && *wptr != ',') {
			prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_67, "<exception criteria unchanged>"));
			return(0);
		}
		wptr++;
	}

	while(*ptr) {				/* increment ptr so that it points to the 1st digit */
		if(*ptr == ' ' || *ptr == ',')
			ptr++;
		else 
			break;			/* got to 1st digit */
	}

	for(i = 0; i < EXC_TYPES; i++)		/* string entered is valid; check that exception group/type #'s entered are valid */
		type_ary[i] = -1;
	for(i = 0; i < 10; i++)
		num_ary[i] = -1;
	
	i = 0;
	tptr = strtok(ptr,", ");
	num_ary[i] = atoi(tptr);
	i++;
	while((tptr = strtok(NULL,", ")) != NULL) {
		num_ary[i] = atoi(tptr);
		i++;
	}

	i = 0;
	while(num_ary[i] != -1) {
		if(num_ary[i] <= 0) {	
			prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_67, "<exception criteria unchanged>"));
			return(0);
		}
		else if(num_ary[i] > EXC_TYPES && s_or_i == EXC_TYPE_SEL) {
			prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_67, "<exception criteria unchanged>"));
			return(0);
		}
		else if(num_ary[i] > EXC_GROUPS && s_or_i == EXC_GRP_SEL) {
			prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_67, "<exception criteria unchanged>"));
			return(0);
		}
		i++;
	}

	if(s_or_i == EXC_TYPE_SEL) {				/* exception type #'s entered */
		i = 0;
		while(num_ary[i] != -1) {
			type_ary[i] = num_ary[i] - 1;
			i++;
		}
	}
	else {						/* exception group #'s entered */
		i = 0;
		while (num_ary[i] != -1) {
			get_types_from_group(type_ary,num_ary[i]);
			i++;
		}
	}

	memset((char *)&temp_selected_except,'\0',sizeof(struct except_bits));	/* set up exception types in temp_selected_except */
	i = 0;
	while(type_ary[i] != -1 && i < EXC_TYPES) {
		set_bit(type_ary[i],sizeof(struct except_bits),(unsigned char *)&temp_selected_except);
		i++;
	}
	
	return(1);								/* selection criteria has been reset */
}

/* call_with_more_excepts: for an exception associated with a call, it determines if the call has
 * more than 1 exception associated with it. 
 *	Returns		0	if call does not have more than 1 exception
 *	Returns		1	if call has more than 1 exception associated with it
 */
int 
call_with_more_excepts(ptr_to_except)
struct excepts *ptr_to_except;	/* pointer to exception slot in shared memory */
{
	int except_count = 0;	/* for determining # of exception with this call */
	int i;

	if(ptr_to_except->call_ptr == (struct calls *)0)
		return(0);	/* this exception is not with a call */

	for(i = 0; i < MANUAL; i++)  {
		if(test_bit(i,sizeof(struct except_bits),(unsigned char *)&ptr_to_except->call_ptr->status)) {
			except_count++;
			if(except_count > 1) 
				return(1);	/* call has more than 1 exception */
		}
	}
	return(0);	/* call has only 1 exception */
}

/************************************************************************************************************************/
/* writer_emer_off - Called by USER_INTERFACE when a driver presses the emergency (M) key.  				*/
/* It updates DRIVER record with the time of the emergency.  								*/
/* It updates the emergency record that was created by DISPATCH when the emergency exception was generated.		*/
/************************************************************************************************************************/
writer_emer_off(fleet, date_time, by, resolve_time, except_index)
char fleet;					/* fleet in which exception is present */
long date_time;					/* date/time the exception was generated */
short by;					/* id of user who resolved the exception */
long resolve_time;				/* time exception is resolved */
int except_index;
{
	struct cisam_dr *dr_ptr;
	struct cisam_em *em_ptr;
	char cur_dt[10];
	char cur_tm[10];
	char comment[79];
	int  dummy1, dummy2;
	int  driver_id;
	static struct scr_flds emerg_screen[] = {
	  { 0, 0, 1, 1},
	  {3, 26, 78, 1}
	};

	bzero( comment, 79 );
	create_message_window(except_index);
	center_msg(mesg_wind, 1, catgets(UI_m_catd, UI_1, UI_TEXT_21,
					 "Enter a comment to go with EMERGENCY approval (Hit <RETURN> when done):"));
	while ( !strlen(comment) )
	  {
	    read_field(mesg_wind, emerg_screen, 1, comment, &dummy1, &dummy2);
	    if ( !strncmp(comment,"                                                  ", 50) )
	      bzero(comment, 79 );
	  }
	delete_message_window();


	if (db(EMERG_FILE_ID,OPEN,&em_key1,ISINOUT+ISMANULOCK,0) < 0) {		/* open EMERG file */
		db(EMERG_FILE_ID,CLOSE,&em_key1,0,0);				/* close EMERG file */
		return(-1);							/* if error, return */
	}

	em_ptr = (struct cisam_em *)req_buf;
	bzero((void *)em_ptr, sizeof(struct cisam_em));

	em_ptr->fleet = fleet;
	em_ptr->date_time = date_time;

	if (db(EMERG_FILE_ID,READ_KEYS,&em_key1,ISEQUAL+ISLOCK,0) < 0) {	/* read EMERG record */
	        prt_error(UI_NO_MSG, "Error reading Emergency file");
		db(EMERG_FILE_ID,RELEASE,&em_key1,0,0);				/* unlock EMERG record */
		db(EMERG_FILE_ID,CLOSE,&em_key1,0,0);				/* close EMERG file */
		return(-1);							/* if error, return */
	}

	get_asc_time(resolve_time, cur_dt, cur_tm);				/* get current time */
	strcpy(em_ptr->resolved_date,cur_dt);
	strcpy(em_ptr->resolved_time,cur_tm);
	em_ptr->resolved_dt_tm = resolve_time;
	em_ptr->resolved_by = by;
	strcpy(em_ptr->comment, comment);

	/* Save since it will be wiped out being stored in req_buf */
	driver_id = em_ptr->drv_id;

	if (db(EMERG_FILE_ID,UPDATE_REC,&em_key1,0,0) < 0) {			/* update EMERG record */
		db(EMERG_FILE_ID,RELEASE,&em_key1,0,0);				/* unlock EMERG record */
		db(EMERG_FILE_ID,CLOSE,&em_key1,0,0);				/* close EMERG file */
		return(-1);							/* if error, return */
	}

	if (db(EMERG_FILE_ID,RELEASE,&em_key1,0,0) < 0) {			/* unlock EMERG record */
		db(EMERG_FILE_ID,CLOSE,&em_key1,0,0);				/* close EMERG file */
		return(-1);							/* if error, return */
	}

	if(db(EMERG_FILE_ID,CLOSE,&em_key1,0,0) < 0) 				/* close EMERG file */
		return(-1);							/* if error, return */

	if (em_ptr->drv_id <= 0)
		return(0);							/* driver not signed on yet */

	dr_ptr = (struct cisam_dr *)req_buf;
	bzero((void *)dr_ptr, sizeof(struct cisam_dr));

	dr_ptr->fleet = fleet;
	dr_ptr->id = driver_id;

	if (db(DRIVER_FILE_ID,READ_KEYS,&dr_key1,ISEQUAL+ISLOCK,0) < 0) {	/* read DRIVER record */
		db(DRIVER_FILE_ID,RELEASE,&dr_key1,0,0);			/* unlock DRIVER record */
		return(-1);							/* if error, return */
	}

	dr_ptr->emerg_of_dt_tm = resolve_time;

	if (db(DRIVER_FILE_ID,UPDATE_REC,&dr_key1,0,0) < 0) {			/* update DRIVER record */
		db(DRIVER_FILE_ID,RELEASE,&dr_key1,0,0);			/* unlock DRIVER record */
		return(-1);							/* if error, return */
	}

	if(db(DRIVER_FILE_ID,RELEASE,&dr_key1,0,0) < 0) 			/* unlock DRIVER record */
		return(-1);							/* if error, return */
}

void
delete_message_window()
{
  if (mesg_wind)
    {
      delwin(mesg_wind);
      touchwin(stdscr);
      refresh();
      mesg_wind = NULL;
    }
}

void
create_message_window(line_num)
     int line_num;
{
  int start_row;

  delete_message_window();

  if (line_num > 10)
    start_row = 7;
  else
    start_row = 18;
  
  mesg_wind = newwin(5, COLS, start_row, 0);
  if (mesg_wind != NULL)
    tpak_box(mesg_wind, 0, 0);
}
