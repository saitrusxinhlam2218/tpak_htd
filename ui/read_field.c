static char sz__FILE__[]="@(#)read_field.c	10.1.2.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  read_field.c; Rel: 6.1.0.0; Get date: 10/4/90 at 11:33:25
 * @(#)  Last delta: 9/21/90 at 16:21:09
 * @(#)  SCCS File: /taxi/sccs/s.read_field.c
 *                                                                           *
 *****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <cursesX.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/time.h>
#include <langinfo.h>
#include <signal.h>
#include <setjmp.h>

#include "mads_types.h"
#include "mads_isam.h"
#include "user.h"
#include "ui_def.h"
#include "call_entry.h"
#include "screens.h"
#include "sp_fld_name.h"
#include "func_keys.h"
#include "language.h"
#include "Line_Manager_private.h"
#include "enhance.h"
#include "switch_ext.h"
#define		PIU_ALL_CALLS_PHONENUM1		14

extern struct users *uiuser[USER_MAX];		/* array of pointers to point to users structure in shared memory */
extern int user_index;				/* index to be used by this user for the users structure in shared memory */
extern short da_flag;
						/* global variables */
int upper_case = 1;				/* should chars to changed to upper case */
long last_upd_25 = 0;				/* last time this process re-printed the 25th line */
extern char *getenv();
short silent = 0;				/* read_field() is silent or not ? */
short skip_return = 0;		/* should read_field skip return befre returning on max chars entered */
extern struct scr_flds basic_scr_flds[], extended_scr_fld[], wakeup_scr_fld[], srch_all_flds[];
extern int scr_name;
extern int No_status_line;
extern char *Status_line_start;
extern char *Status_line_end;
extern struct cisam_cl call_struct;
int  GotSignal = 0;
extern sigjmp_buf     jmpbuf;
extern int LineMgrIndex;
extern int pid;
int    EventFlag = 0;
char Telephone[ TELEPHONE_NBR_LEN + 1];
char AccountCode[ 19 ];
extern int bPhoneFieldVisited;
extern BOOLEAN msg_box_active, question_for_user_active ;
extern int doing_update;
int 	gkey_val = 0;

void
GetClientInfo()
{
  char      EventInfo[ 5 ];
  int       idx_shmem;
  char      *pEvent;

  if ( ( scr_name == BASIC_CALL_SCR ) &&
       ( !bPhoneFieldVisited )  &&
       ( !msg_box_active )  &&
       ( !question_for_user_active ) &&
       ( !doing_update ) )
    {
      idx_shmem = LineMgrIndex;

      if ( Lines[idx_shmem]->NewCall == '1' )
	{
          if ( ( Lines[idx_shmem]->MonitorName[0] != 'B' ) && ( 0 ) )// not a basic call so toggle to extended screen
               gkey_val = TOGGLE_KEY;
               
	  if ( strlen( Lines[idx_shmem]->TelephoneNbr ) )
	    {

              sprintf( Telephone, "%s", Lines[idx_shmem]->TelephoneNbr );

	      GotSignal = 1;
	      siglongjmp( jmpbuf, 1 );
	    }
	  else
	    siglongjmp( jmpbuf, 1 );
	}
      else
        siglongjmp( jmpbuf, 1 );          
    }
  else if ( ( scr_name == EXTENDED_CALL_SCR ) && ( !bPhoneFieldVisited ) && ( !doing_update ) )
    {
      idx_shmem = LineMgrIndex;
      if ( Lines[idx_shmem]->NewCall == '1' )
	{      
          if ( ( Lines[idx_shmem]->MonitorName[0] != 'A' ) && ( 0 ) )  // not an account customer so toggle to basic screen
            gkey_val = TOGGLE_KEY;
          else  // FTFA will always go to Basic Call Entry
            gkey_val = TOGGLE_KEY;
          
          if ( strlen( Lines[idx_shmem]->TelephoneNbr ) )
            {
              
              sprintf( Telephone, "%s", Lines[idx_shmem]->TelephoneNbr );
              
              GotSignal = 1;
              siglongjmp( jmpbuf, 1 );
            }
        }
      else
        siglongjmp( jmpbuf, 1 );
    }
  else if ( ( scr_name != BASIC_CALL_SCR ) ||
            ( msg_box_active ) ||
            ( question_for_user_active ) ||
            ( doing_update ) )
    EventFlag = TRUE;
}

alter_highlight(win_fld, y, min_x, max_x, attr)
WINDOW *win_fld;
int y;
int min_x;
int max_x;
int attr;
{
    int  i;

    wmove(win_fld, y, min_x);
    wattrset(win_fld, attr);
    for (i = min_x; i <= max_x; i ++)
	waddch(win_fld, winch(win_fld) & A_CHARTEXT);

    /* Move cursor to start of field */
    wmove(win_fld, y, min_x);

    wrefresh(win_fld);
}

/********************************************************************************/
/* read_field: This function takes the number of the field to be 		*/
/* read (=piu), and  a pointer to the buffer which is used to store 		*/
/* the chars being read. The function is used to read the various 		*/
/* fields on the data entry screens. 						*/
/* The function returns:							*/
/*	OK  if the user enters a field; and field entry is completed		*/
/*	by reaching the max # of chars.						*/
/*	ret_val (= value of the function key pressed; if the user		*/
/*	enters a function key, <CR>, or newline)				*/
/* 	-1 is returned if the user does not enter a char within			*/
/*	BLOCK_TIME 								*/
/********************************************************************************/
int read_field(win_fld, ptr_scr_fld, piu, read_buf, field_entered,should_clear_ptr)
WINDOW *win_fld;			/* window in which read_field() is to operate */
struct scr_flds *ptr_scr_fld;		/* pointer to 1st element of array of piu structures */
int piu;				/* prompt in use */
char *read_buf;				/* pointer to buffer for reading input */
int *field_entered;			/* pointer to 0 or 1; field entered or not */
int *should_clear_ptr;			/* pointer to flag; 0= clear the field upon entry of 1 st char ; 1= do not clear */
{
	int c;				/* for reading a char */
	int position;			/* columnar position of the cursor */
	int max_position;		/* max columnar position in field */
	int min_position;		/* min columnar position in field */
	short read_count;		/* count of number of char's read into the field */
	char *read_ptr;			/* pointer to walk thru the buffer for reading char's */
	char *end_ptr;			/* pointer to last char+1 entered in buffer */
	int ret_val;			/* value returned by function */
	int prompt_row;			/* row of field */
	int prompt_col;			/* col of field */
	int max_chars;			/* max chars in field */
	int end_read_field;		/* flag; 0/1; set if end the function */
	char *temp_ptr;			/* temp pointer  */
	struct scr_flds *ptr_to_fld;	/* pointer to appropriate slot in scr_flds structure in  the array of piu structures */
	int cy,cx;			/* cursor position saved for write to 25th line */
	int old_entry_exists = 0;	/* does old entry exist in this field */
	int entry_changed = 0;		/* has old entry benn changed .. used for timing out read_field() */
	short no_move = 0;

	int  i;
	int  diff;
	int  win_index;
	int saved_highlight;
	struct tm *pDateTime;
	char  *pDateDescript;

	skip_return = 0;
	ptr_to_fld = ptr_scr_fld + piu; 	/* init ptr_to_fld to point to right structure in array of structures scr_flds */

	prompt_row = ptr_to_fld->prompt_row;	/* Initilaize field boundaries of the field for entry. */
	prompt_col = ptr_to_fld->prompt_col;
	max_chars = ptr_to_fld->max_chars;

	min_position = prompt_col;
	max_position = prompt_col + max_chars -1;
	position = min_position;

	
	wmove(win_fld,prompt_row,position);		/* Move the cursor to the appropriate position to read the field. */
	wrefresh(win_fld);
	bump_resp(CMD_TIME_CHECK,END_TIME_CMD);		/* determine response time for action */

	read_count = 0;					/* Initilaize counters and pointers to read field. */
	read_ptr = read_buf;
	end_ptr = read_ptr;
	memset(read_buf, ' ', max_chars);
	read_buf[max_chars] = '\0';

				/* initialize pointers, and count of char's from an old entry which might exist in the field */
	initialize_read_buf(win_fld,ptr_scr_fld,piu,read_buf,&read_ptr,&end_ptr,&read_count,&old_entry_exists);

							/* Enter a while loop which executes as long as the field is */
	 						/* being entered. If the user enters a <CR> or a newline, */
	 						/* or a function key, or if the max number of chars for the */
	 						/* field have been entered, then it is assumed that the user */
	 						/* has finished entering the field. */


	/* Getting the current highlight type at input field */
	saved_highlight = mvwinch(win_fld, prompt_row, min_position) & A_ATTRIBUTES;
	
	/* Highlight entry field */
	alter_highlight(win_fld, prompt_row, min_position, max_position, A_REVERSE | A_UNDERLINE);

	end_read_field = 0;				/* flag set if while loop is to end */

						/* if phone num1 field is being eneterd, then skip_return should be set to 1 */
#ifdef NAMERICA
	if ((ptr_scr_fld == basic_scr_flds && piu == B_PH_NUM1) || (ptr_scr_fld == extended_scr_fld && piu == E_PH_NUM1) ||
	    (ptr_scr_fld == wakeup_scr_fld && piu == W_PH_NUM1) || (ptr_scr_fld == srch_all_flds && piu == PIU_ALL_CALLS_PHONENUM1)  )  
		skip_return = 1;			/* do not wait for <CR> */
#endif

        if ((ptr_scr_fld == basic_scr_flds && piu == B_CAR_NEEDS) ||
            (ptr_scr_fld == extended_scr_fld && piu == E_CAR_NEEDS))
          {
            skip_return = 1;
            silent = 1;
          }

	while ((!end_read_field) && (read_count <= max_chars)) { /* Read the char from the user. This is in halfdelay() */
		  						 /* mode. The routine halfdelay() causes getch() to block for */
		  						 /* BLOCK_TIME (=5 seconds, say). If a char is read */
		  						 /*  then it will return immediately.  */
          sigsetjmp( jmpbuf, 1);



          
          // do we have an A-number to use for call entry?
          if ( GotSignal == 1 )
            {
              GotSignal = 0;
              EventFlag = FALSE;
              strcpy( read_buf, Telephone );
              if ( ( scr_name == BASIC_CALL_SCR ) && ( piu == B_PH_AREA_CODE ) )
                {
                  *field_entered = 1;
                  return(OK);
                }
              else if ( ( scr_name == EXTENDED_CALL_SCR ) && ( piu == E_CHARGE ) )
                {
                  *field_entered = 1;
                  return(OK);
                }
            }
          

		if ( read_ptr == read_buf + ptr_to_fld->max_chars && skip_return == 1 )
			break;					/* leave while loop */
		if (( c= wgetkey(win_fld)) == ERR) {					/* char not available */


                  
			if ((!read_count) || (old_entry_exists && !entry_changed)) {	/* times out */ 
				silent = 0;						/* read_field() is to echo char's */
				wattrset(win_fld, 0);				/* set attributes back to normal mode */
				*should_clear_ptr = 0;					/* set flag to not clear field on */
											/* entry of 1st char */
				do_stats();						/* check if statusr records are to */
											/* be written ; if yes write 'em */
				/* Unhighlight entry field */
				alter_highlight(win_fld, prompt_row, min_position, max_position, saved_highlight);
				wattrset(win_fld, 0);				/* set attributes back to normal mode */

				/* Update status line */
				display_time();
    			
				return(-1);
			}
			continue;							/* back to while loop */
		}

								/*  A char has been read. If it's a valid char, then it */
		 						/*  should be echoed on the screen. If its a function  */
		 						/*  key then it should not be echoed on the screen. */
		entry_changed = 1;				/* old entry if it exists has been changed */
		if (c < 255 && isprint(c)) {	
			if (*should_clear_ptr) { 		/* should field be cleared upon entry of 1st char in field */
				*should_clear_ptr = 0;
				read_count = 0;
				read_ptr = read_buf;
				end_ptr = read_ptr;
				memset(read_buf, ' ', max_chars);
				read_buf[max_chars] = '\0';
			}
			if (read_count == max_chars && read_ptr == end_ptr)	/* if max chars have already been entered, then */
				continue; 					/* should terminate entry by <CR> or function key */

			if (upper_case)						/* its a printable char */
				c = Toupper(c);					/* change char entered to upper case */

			if (!read_count) { 					/* this is the first char read on this */
			        wattrset(win_fld, A_REVERSE|A_UNDERLINE); /* Set the attribute again since status can screw it up */
				if (!silent)
				  clear_field(win_fld, ptr_scr_fld,piu);	/* field; so clear the entry field */
				wmove(win_fld,prompt_row,position); 		/* move cursor back to its position */
			}
                                /* is it a date field being entered ? */
				if (
				    (
				     (
				      (scr_name == WAKEUP_CALL_SCR || scr_name == SUBS_WAKEUP_SCR) &&
				      (piu == W_DATE || piu == W_BEGIN_DATE || piu == W_END_DATE)) ||

				     (
				      (scr_name == EXTENDED_CALL_SCR || scr_name == SUBS_EXTENDED_SCR) &&
				      (piu == E_DATE || piu == E_BEGIN_DATE || piu == E_END_DATE)) ||

				     (
				      (scr_name == BASIC_CALL_SCR || scr_name == SUBS_BASIC_SCR) &&
				      (piu == B_DATE || piu == B_BEGIN_DATE || piu == B_END_DATE)) ||

				     (
				      (scr_name == SUSPEND_VEH_DR_SCR) &&
							(piu == F_SUSP_PASS_FROM_DATE || piu == F_SUSP_PASS_TO_DATE ||
							 piu == F_SUSP_DEL_FROM_DATE || piu == F_SUSP_DEL_TO_DATE ||
							 piu == F_SUSP_X_FROM_DATE || piu == F_SUSP_X_TO_DATE)) ||

				     (
				      (scr_name == ALL_CALLS_SCR || scr_name == ASSIGNED_CALLS_SCR ||
				       scr_name == UNASSIGNED_CALLS_SCR || scr_name == VEH_DR_CALLS_SCR || 
				       scr_name == INDIVIDUAL_CALLS_SCR) &&
				      piu == 9) ||
				     
				     (da_flag && piu == 7)))
				    {
				/* automatically enter slashes for user */
					if (c != DATE_SEPERATOR && (read_ptr == read_buf+2 || read_ptr == read_buf+5))
					{
						*read_ptr++ = DATE_SEPERATOR;
						position++;
						/* don't add to count if we are updating the field */
						if (!old_entry_exists) read_count++; 
						waddch(win_fld, DATE_SEPERATOR);
						wrefresh(win_fld);
					}
					else if(read_ptr == read_buf+1 && c == DATE_SEPERATOR)
					{
						position = prompt_col;
						read_count = 0;
						wattrset(win_fld, A_REVERSE|A_UNDERLINE); /* Set the attribute again since status can screw it up */
						clear_field(win_fld, ptr_scr_fld,piu); 		/* field; so clear the entry field */
						wmove(win_fld,prompt_row,prompt_col); 		/* move cursor back to its position */
						read_buf[1] = read_buf[0];
						position++;
						read_count++;
						waddch(win_fld,'0');
						read_buf[0] = '0';
						position++;
						read_count++;
						waddch(win_fld,read_buf[1]);
						read_ptr = read_buf+2;

					}
				}

			if (read_ptr >= end_ptr) {
				*read_ptr++ = c;				/* save char */
				end_ptr = read_ptr;
				read_count++;
			}
			else  {
				*read_ptr++ = c;
			}
			position++;	
			ret_val = OK;
			wattrset(win_fld, A_REVERSE | A_UNDERLINE); /* Set the attribute again since status can screw it up */
			if ( (silent) )
			  {
			    if (no_move)
			      {
				wmove(win_fld, prompt_row, position - 1);
				position = prompt_col;
				silent = 0;
			      }
			    else
				wmove(win_fld,prompt_row,position); 	/* read_field() is silent.. do not echo char's */
			  }
			else 
				mvwaddch(win_fld, prompt_row, position - 1, c);		/* echo char on screen */
			wrefresh(win_fld);
		}
		else {
		    *should_clear_ptr = 0;				/* c is not a printable char */
		    switch(c) {
			case '\r':					/* <CR> */
			case '\n':					/* newline */
				ret_val = c;
				end_read_field =1;
				break;					/* leave switch */

			case KEY_LEFT:					/* left arrow key non desructive move towards left */
				if (position == min_position)		/* cursor is on the first position in the field */
					beep();				/* ring bell */
				else  {					/* move cursor 1 slot towards left */
					if ( read_ptr > end_ptr)
						read_count--;
					read_ptr--;
					position--;
					wmove(win_fld,prompt_row,position);
					wrefresh(win_fld);
				}

				break;					/* leave switch */
				
			case KEY_RIGHT:					/* right arrow key non destructive move towards right */
				if (position > max_position)		/* cursor is on the last position in the field */
					beep();				/* ring bell */
				else  {					/* move cursor 1 slot to right*/
					if ( read_ptr >= end_ptr ) 
						read_count++;
					read_ptr++;
					position++;
					wmove(win_fld,prompt_row,position);
					wrefresh(win_fld);
				}
				break;					/* leave switch */

			
			case '\01': /* ^A for go to start of field */
				if (position != min_position) {
				    read_ptr = read_buf;
				    position = min_position;
				    wmove(win_fld,prompt_row,position);
				    wrefresh(win_fld);
				}
				break;
							
			case '\05': /* ^E for go to end of characters in field */
				if (read_ptr != end_ptr) {
				    diff = (int) (end_ptr - read_ptr);
				    read_ptr = end_ptr;
				    position += diff;
				    wmove(win_fld,prompt_row,position);
				    wrefresh(win_fld);
				}
				break;
							
			case '\025': /* ^U for clear field */
			        wattrset(win_fld, A_REVERSE|A_UNDERLINE); /* Set the attribute again since status can screw it up */
				clear_field(win_fld, ptr_scr_fld,piu); 		/* field; so clear the entry field */
				position = min_position;
				read_count = 0;				/* Initilaize counters and pointers to read field. */
				read_ptr = read_buf;
				end_ptr = read_ptr;
				memset(read_buf, ' ', max_chars);
				read_buf[max_chars] = '\0';
				wmove(win_fld,prompt_row,position); 		/* move cursor back to its position */
				wrefresh(win_fld);
				break;
				
			case KEY_BACKSPACE:
			case KEY_DL:			/* delete key.. should not be set up to generate the interrupt signal */
			case '\b':			/* backspace key delete char under cursor */
				if (read_ptr > end_ptr) { 	/* cursor is on the last char entered by user+1 */
					read_count--;
					read_ptr--;
					position--;
					*read_ptr = ' ';
					wmove(win_fld,prompt_row,position);
					wrefresh(win_fld);
				}
				else if (read_ptr == end_ptr) {
					if (position == min_position) 	/* cursor is on the first chara of the field, & no */
						beep(); 		/* charas have been entered; so just beep */
					else {				/* cursor is on the last+1 char entered by user */
						read_ptr--;
						position--;
						end_ptr --;
						*read_ptr = ' ';
						read_count --;
						mvwaddch(win_fld, prompt_row, position, ' '); /* Erase last char */
						wmove(win_fld,prompt_row,position);
						wrefresh(win_fld);
					}
				}
				else { 					/* read_ptr < end_ptr */
				    if (read_ptr == read_buf) /* At start of field */
					temp_ptr = read_ptr;
				    else {
					temp_ptr = read_ptr - 1;
					wmove(win_fld, prompt_row, position - 1);
				    }

				    /* delete char under the cursor, and move all the */
				    /* char's which are towards the right of the cursor, */
				    /* move 1 slot to the left */
				    while (*temp_ptr && temp_ptr < end_ptr)
				    {

				      *temp_ptr = *(temp_ptr+1);
		
				      /* When read_buf is full, temp_ptr
					 can point to a '\0' which you don't
					 want to print */
				      if (*temp_ptr != '\0')
					waddch(win_fld,*temp_ptr);
				      else
					waddch(win_fld, ' ');
				      temp_ptr++;
				    }


				    if ( position > min_position) {
					position--;
					read_ptr--;
				    }
				    read_count--;
				    *end_ptr = ' ';
				    end_ptr--;

				    /* make sure end of string stays 0 */
				    read_buf[max_chars] = '\0';

				    wmove(win_fld,prompt_row,position); 		/* reset cursor position on screen */
				    wrefresh(win_fld);
				}

				break;					/* leave switch */
			
			case '\014': /* ^L for redrawing screen */
				wrefresh(curscr);
				break;
				

			default:					/* function key entered */
				end_read_field = 1;			/* flag to leave while loop */
				ret_val = c;
				break;					/* leave switch */
		   }
		}
	}


	 						/* Either the field has been entered or it has not been entered. */
	wattrset(win_fld, 0);				/* set attributes back to normal mode */

	if (!read_count) {
		if (old_entry_exists) { 				/* an old entry existed in this field, so to wipe it out,*/
			read_buf[0] = ' '; 				/* set read_buf[0] = ' '; this happens when the user */
			*field_entered = 1; 				/* clears out an old entry from the screen field */
		}
		else { 							/* set local int, field_entered to 0, */
			*field_entered = 0; 				/* since field not entered */
		}
						/* no char's entered; user entered <CR> or function key pressed w/o entering char */
		silent = 0;			/* read_field() is to echo char's */
		*should_clear_ptr = 0;		/* set flag to not clear field on entry of 1st char */

		/* Unhighlight entry field */
		alter_highlight(win_fld, prompt_row, min_position, max_position, saved_highlight);
		wattrset(win_fld, 0);				/* set attributes back to normal mode */

		/* Update status line */
		display_time();
		return(ret_val);
	}
	else { 				/* field has been entered; if it is a 1 char field; then do not null terminate; */
					/* otherwise null terminate it */
	        *field_entered = 1;
		if (max_chars == 1) { 				/* it's a 1 char field */
			silent = 0;				/* read_field() is to echo char's */
			*should_clear_ptr = 0;			/* set flag to not clear field on entry of 1st char */

			/* Unhighlight entry field */
			alter_highlight(win_fld, prompt_row, min_position, max_position, saved_highlight);
			wattrset(win_fld, 0);				/* set attributes back to normal mode */

			/* Update status line */
			display_time();
			return(ret_val);
		}
		else { 						/* more than 1 char field; null terminate */
			*end_ptr++ = '\0';
			strip_blanks(read_buf,max_chars,read_count);
			silent = 0;				/* read_field() is to echo char's */
			*should_clear_ptr = 0;			/* set flag to not clear field on entry of 1st char */

			/* Unhighlight entry field */
			alter_highlight(win_fld, prompt_row, min_position, max_position, saved_highlight);
			wattrset(win_fld, 0);				/* set attributes back to normal mode */

			/* Update status line */
			display_time();
			return(ret_val);
		}  
	}
}

/****************************************************************/
/* clear_field: clears the field area (called by read_field() ) */
/****************************************************************/
clear_field(win_fld, ptr_scr_fld, piu)
	WINDOW *win_fld;	/* pointer to window to be used */
	struct scr_flds *ptr_scr_fld;	/* pointer to 1st element of the 
					 * array of piu structures */
	int piu;	/* prompt in use */
{

	struct scr_flds *ptr_to_fld;	/* pointer to appropriate slot in 
					 * scr-flds structure */
	int i;	/* scratch variable */

	/* initizlize ptr_to_fld to point to the right structure in the array
	 * of structures scr_flds 
	 */
	ptr_to_fld = ptr_scr_fld + piu;

	/* move the cursor to the 1st char of the field */
	wmove(win_fld, ptr_to_fld->prompt_row, ptr_to_fld->prompt_col);

	/* clear the field */
	for(i=0; i< ptr_to_fld->max_chars; i++)
		waddch(win_fld, ' ');
	
	/* do not refresh */

}  /* end clear_field */


/* initialize_read_buf: initializes the pointers and count of chars used by read_field(), from an old
 * entry which might exist in the field
 */
initialize_read_buf(win_fld,ptr_scr_fld,piu,read_buf,ptr_read_ptr,ptr_end_ptr,ptr_count,ptr_old_entry_exists)
	WINDOW *win_fld;	/* window in which read_field() is to operate */
	struct scr_flds *ptr_scr_fld;	/* pointer to array of piu structures for this window */
	int piu;	/* field # being read */
	char *read_buf;	/* pointer to buffer in which input is to be read */
	char **ptr_read_ptr;	/* pointer to read_ptr of read_field() */
	char **ptr_end_ptr;	/* pointer to end_ptr of read_field() */
	short *ptr_count;	/* pointer to # of chars in field */
	int *ptr_old_entry_exists;	/* does old entry exist */
{
	int i;	/* scratch variable */
	int c;	/* for reading curses internal buffers */
	struct scr_flds *ptr_to_fld;	/* pointer to piu struct of field being read */
	char *walk_ptr;	/* to walk thru read_buf[] */
	int read_count;	/* count of char's in field */


	if (silent)
	  return;

	ptr_to_fld = ptr_scr_fld + piu;


	/* read char's from curses internal buffers & save in read_buf[] */
	for ( i = 0; i < ptr_to_fld->max_chars; i++ )  {
		c = mvwinch(win_fld,ptr_to_fld->prompt_row,ptr_to_fld->prompt_col+i);
		read_buf[i] = c & 0x0ff;	/* strip off all attribute bits */
	}  /* end for */
	wmove(win_fld,ptr_to_fld->prompt_row,ptr_to_fld->prompt_col);

	/* if 1st char is a blank then assume that field is empty */
	if ( read_buf[0] == ' ' )  {
		*ptr_read_ptr = read_buf;
		*ptr_end_ptr = read_buf;
		*ptr_count = (short)0;
		*ptr_old_entry_exists = 0;	/* old entry does not exist */
		return;
	}  /* end if .. no old entry in field */

	/* should initialize pointers & count, depending upon how many char's the field has */
	if ( read_buf[ptr_to_fld->max_chars -1] != ' ' )  {
		/* the field contains max char's allowed */
		*ptr_read_ptr = read_buf;
		*ptr_end_ptr = read_buf + ptr_to_fld->max_chars;
		*ptr_count = (short)ptr_to_fld->max_chars;
		*ptr_old_entry_exists = 1;
		return;
	}  /* end if ... max chars in field */

	/* field contains less than max allowable char's */
	read_count = ptr_to_fld->max_chars - 1;
	walk_ptr = &read_buf[ptr_to_fld->max_chars -1];

	while ( *walk_ptr == ' '  &&  read_count > 0 )  {
		read_count--;
		walk_ptr--;
	}  /* end while */
	read_count++;
	*ptr_read_ptr = read_buf;
	*ptr_end_ptr = walk_ptr + 1;
	*ptr_count = (short)read_count;
	*ptr_old_entry_exists = 1;

}  /* end initialize_read_buf()  */


/* strip_blanks: strips off trailing blanks from an entry in read_buf[], it does not strip off
 * the first byte, even if it is a blank. It leaves the string null_terminated. For a 1 byte field
 * (max chars == 1), the function does nothing
 */
strip_blanks(read_buf,max_chars,chars_entered)
	char *read_buf;	/* pointer to buffer with input */
	int max_chars;	/* max allowable chars in field */
	int chars_entered;	/* how many chars were entered */
{
	int indx;
	char *ptr;	/* to walk thru read_buf[] */

	indx = ( max_chars  < chars_entered ) ? (max_chars-1):(chars_entered-1);

	ptr = &read_buf[indx];

	/* strip off trailing blanks - if any */
	while ( *ptr == ' '  && indx != 0 )  {
		*ptr-- = '\0';
		indx--;
	}  /* end while */

}  /* end strip_blanks()  */

/*====================================================================
= status_line() -
=	Identifies the terminal type.
=	If possible, prints the message on the 25th line.
=	Otherwise suppresses printing the message.
=	Who:	Bruce Young
=	What:	Written		When: 20 June 1988
=====================================================================*/
status_line(message)
char *message;
{
    static char last_message[133];

    /* For auto-pilot -nsl flag */
    if (No_status_line)
	return;

#if 0

Save this stuff for future reference

	termtype=getenv("TERM"); /* get the terminal type */

	/* The wyse60 uses two sting delimiter characters, that print spaces on the terminal. */
	/* Thus impose a limit for message length of 130 columns. */
	if ( strlen(message)>string_length ) message[string_length]=NULL;

	if (strcmp(termtype,"tm228w")==0 || strcmp(termtype,"tm228")==0)
		/* the motorolla tm3228 Kokasai automatically clears the status line */
		printf("\0337\033[99f%-130.130s\0338",message); 

	else if (strcmp(termtype,"vt100-w")==0 || strcmp(termtype,"vt100w")==0) 
		/* the wyse 60 is used as a vt100 in wide mode */
		printf("\033[>,%-130.130s",message);

	else if (strcmp(termtype,"tm220w")==0 || strcmp(termtype,"tm220w")==0) {
		/* the motorolla tm3220 AMPEX terminal */
		printf("\033[36h");	/* set the user line */
		printf("\033P1;0^%-130.130s\033\\",message); 
	}	/* end AMPEX */

	else if (strcmp(termtype,"wyse50")==0 || strcmp(termtype,"wy50")==0
		|| strcmp(termtype,"wyse50w")==0 || strcmp(termtype,"wy50w")==0
		|| strcmp(termtype,"wyse60")==0 || strcmp(termtype,"wy60")==0
		|| strcmp(termtype,"wyse60w")==0 || strcmp(termtype,"wy60w")==0) {
		/* the wyse50 and wyse60 write in protected modes */
		printf("\033z(%-130.130s\n",message); printf("\033z)");
	}	/* end wyse */
#endif

	/* Don't do anything if status line has not changed */
	if (strcmp(message, last_message) == 0)
	    return(0);
#ifdef FOO	
	if (message[0])
	  printf("%c]0;%s%c", '\033', message, '\007');
	else
	  printf("%c]0;%130s%c", '\033', "", '\007');
#endif
        if (message[0])
	  printf("%s%s%s", Status_line_start, message, Status_line_end);

        /* Empty message, wipe out status line */
        else
	  printf("%s%130s%s", Status_line_start, "", Status_line_end);


	/* Force display */
	fflush(stdout);

        strcpy(last_message, message);

	return(0);
}  /* end status_line */
