static char sz__FILE__[]="@(#)fl_rd_fld.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  fl_rd_fld.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:47:37
 * @(#)  Last delta: 12/2/94 at 11:33:53
 * @(#)  SCCS File: /taxi/sccs/s.fl_rd_fld.c
 *                                                                           *
 *****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <curses.h>
#include <ctype.h>
#include <sys/types.h>
#include <setjmp.h>

#include "mads_types.h"
#include "user.h"
#include "ui_def.h"
#include "fl_scr.h"


/* global variables */
extern int saw_intr;
int upper_case = 1;	/* should chars to changed to upper case */
extern char *getenv();
short silent = 0;	/* read_field() is silent or not ? */
short skip_return = 0;	/* should read_field skip return befre returning on max chars entered */

jmp_buf jmp_env;
int	do_jmp;

void initialize_read_buf();

alter_highlight(win_fld, y, min_x, max_x, attr)
WINDOW *win_fld;
int y;
int min_x;
int max_x;
int attr;
{
    int  i;
 
    wmove(win_fld, y, min_x);
#ifdef FOO
    wattrset(win_fld, attr);
#endif
    for (i = min_x; i <= max_x; i ++)
        waddch(win_fld, winch(win_fld));
 
    /* Move cursor to start of field */
    wmove(win_fld, y, min_x);
 
    wrefresh(win_fld);
}
 

/* read_field: This function takes the number of the field to be 
 * read (=piu), and  a pointer to the buffer which is used to store 
 * the chars being read. The function is used to read the various 
 * fields on the data entry screens. 
 * The function returns:
 *	OK  if the user enters a field; and field entry is completed
 *	by reaching the max # of chars.
 *	ret_val (= value of the function key pressed; if the user
 *	enters a function key, <CR>, or newline)
 * 	-1 is returned if the user does not enter a char within
 *	BLOCK_TIME
 */
int read_field(win_fld, ptr_scr_fld, piu, read_buf, field_entered,should_clear_ptr)
	WINDOW *win_fld;   /* window in which read_field() is to operate */
	struct scr_flds *ptr_scr_fld;	/* pointer to 1st element of array
					 * of piu structures */
	int piu;	/* prompt in use */
	char *read_buf;	/* pointer to buffer for reading input */
	int *field_entered;	/* pointer to 0 or 1; field entered or not */
	int *should_clear_ptr;	/* pointer to flag; 0= clear the field upon entry of 1 st char ; 1= do not clear */
{
	int c;	/* for reading a char */
	int position;	/* columnar position of the cursor */
	int max_position;	/* max columnar position in field */
	int min_position;	/* min columnar position in field */
	short read_count;	/* count of number of char's read into 
				 * the field */
	char *read_ptr;	/* pointer to walk thru the buffer for reading char's */
	char *end_ptr;	/* pointer to last char+1 entered in buffer */
	int ret_val;	/* value returned by function */
	int prompt_row;	/* row of field */
	int prompt_col;	/* col of field */
	int max_chars;		/* max chars in field */
	int end_read_field;	/* flag; 0/1; set if end the function */
	char *temp_ptr;		/* temp pointer  */
	struct scr_flds *ptr_to_fld;	/* pointer to appropriate slot in 
					 * scr_flds structure in  the array
					 * of piu structures */
	int cy,cx;		/* cursor position saved for write to 25th line */
	int old_entry_exists = 0;	/* does old entry exist in this field */
	int entry_changed = 0;	/* has old entry benn changed .. used for timing out read_field() */
	int  i;
	int  diff;
	int saved_highlight;


	do_jmp = 0;

	/* initialize ptr_to_fld to point to the right structure in the 
	 * array of structures scr_flds
	 */
	ptr_to_fld = ptr_scr_fld + piu;


	/* Initilaize field boundaries of the field for entry. */
	prompt_row = ptr_to_fld->prompt_row;
	prompt_col = ptr_to_fld->prompt_col;
	max_chars = ptr_to_fld->max_chars;

	min_position = prompt_col;
	max_position = prompt_col + max_chars -1;
	position = min_position;

	
	/* Move the cursor to the appropriate position to read the field. */
	wmove(win_fld,prompt_row,position);
	wrefresh(win_fld);

	/* Initilaize counters and pointers to read field. */
	read_count = 0;
	read_ptr = read_buf;
	end_ptr = read_ptr;
	memset(read_buf, ' ', max_chars);
	read_buf[max_chars] = '\0';

	/* initialize pointers, and count of char's from an old entry which might exist in the field */
	initialize_read_buf(win_fld,ptr_scr_fld,piu,read_buf,&read_ptr,&end_ptr,&read_count,&old_entry_exists);

	/*   Enter a while loop which executes as long as the field is 
	 *   being entered. If the user enters a <CR> or a newline,
	 *   or a function key, or if the max number of chars for the
	 *   field have been entered, then it is assumed that the user
	 *   has finished entering the field.
	 */

	/* Getting the current highlight type at input field */
	saved_highlight = mvwinch(win_fld, prompt_row, min_position) & A_ATTRIBUTES;

        /* Highlight entry field */
        alter_highlight(win_fld, prompt_row, min_position, max_position, A_REVERSE | A_UNDERLINE);


	end_read_field = 0;	/* flag set if while loop is to end */

	while ( (!end_read_field) && (read_count <= max_chars) )  {
		
		 /* Read the char from the user. This is in halfdelay()
		  * mode. The routine halfdelay() causes getch() to block for 
		  * BLOCK_TIME (=5 seconds, say). If a char is read 
		  *  then it will return immediately.
		  */
		if ( read_ptr == read_buf + ptr_to_fld->max_chars && skip_return == 1 )
			break;	/* leave while loop */

		if(saw_intr)  /* if CTRL-X and we start entering data remove this record */
		{
			wattrset(win_fld,0);	/* set attributes back to normal mode */
			return(-1);
		}

		do_jmp = 1;

		if ( setjmp(jmp_env) == 0 ) {
			if ( ( c= wgetkey(win_fld) ) == ERR )   { 					/* char not available */
				if ( (!read_count)  ||  (old_entry_exists  &&  !entry_changed) )  {		/* times out */ 
					silent = 0;	/* read_field() is to echo char's */

                              	/* Unhighlight entry field */
                                	alter_highlight(win_fld, prompt_row, min_position, max_position, saved_highlight);
 
					wattrset(win_fld,0);	/* set attributes back to normal mode */
					*should_clear_ptr = 0;	/* set flag to not clear field on entry of 1st char */
					return(-1);
				}
				continue;								/* back to while loop */
			}
		} else
		{
			wattrset(win_fld,0);	/* set attributes back to normal mode */
			return(-1);
		}

		do_jmp = 0;

		/*  A char has been read. If it's a valid char, then it 
		 *  should be echoed on the screen. If its a function 
		 *  key then it should not be echoed on the screen.
		 */

		if ( c < 255 && isprint(c) )  {	
			/* should field be cleared upon entry of 1st char in field */
			entry_changed = 1;

			if ( *should_clear_ptr )  {
				*should_clear_ptr = 0;
				read_count = 0;
				read_ptr = read_buf;
				end_ptr = read_ptr;
				memset(read_buf, ' ', max_chars);
				read_buf[max_chars] = '\0';
			}  /* end if .. should clear ptr */
			/* if max chars have already been entered, then should terminate entry by <CR> or function key */
			if ( read_count == max_chars && read_ptr == end_ptr )
				continue;	/* back to while loop */
			/* its a printable char */
			if ( upper_case ) 
				/* change char entered to upper case */
				c = Toupper(c);
			if ( !read_count )  {
				/* this is the first char read on this
				 * field; so clear the entry field */
				clear_field(win_fld, ptr_scr_fld,piu);
				/* move cursor back to its position */
				wmove(win_fld,prompt_row,position);
			}  /* end if */
			if ( read_ptr >= end_ptr )  {
				*read_ptr++ = c;	/* save char */
				end_ptr = read_ptr;
				read_count++;
			}
			else 
				*read_ptr++ = c;

			position++;	
			ret_val = OK;
			if ( silent )
				wmove(win_fld,prompt_row,position);
				/* read_field() is silent.. do not echo char's */
			else 
				waddch(win_fld,c);  /* echo char on screen */
			wrefresh(win_fld);

		}  /* end if */
		else  {
		    *should_clear_ptr = 0;
		    /* c is not a printable char */
		    switch(c) {
			case '\r':					/* <CR> */
			case '\n':					/* newline */
				ret_val = c;
				end_read_field =1;
				break;					/* leave switch */

			case KEY_LEFT:					/* left arrow key non desructive move towards left
*/
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
				
			case KEY_RIGHT:					/* right arrow key non destructive move towards
right */
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
				clear_field(win_fld, ptr_scr_fld,piu); 		/* field; so clear the entry field */
				position = min_position;
				read_count = 0;				/* Initilaize counters and pointers to read field.
*/
				read_ptr = read_buf;
				end_ptr = read_ptr;
				memset(read_buf, ' ', max_chars);
				read_buf[max_chars] = '\0';
				wmove(win_fld,prompt_row,position); 		/* move cursor back to its position */
				wrefresh(win_fld);
				break;
				
			case KEY_BACKSPACE:
			case KEY_DL:			/* delete key.. should not be set up to generate the interrupt
signal */
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
					if (position == min_position) 	/* cursor is on the first chara of the field, & no
*/
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
				    while (*temp_ptr && temp_ptr < end_ptr) { 	/* delete char under the cursor, and move
all the */
						 			/* char's which are towards the right of the cursor,
*/
									/* 1 slot to the left */
					     *temp_ptr = *(temp_ptr+1);
					     waddch(win_fld,*temp_ptr);
					     temp_ptr++;
					 }

				    if ( position > min_position) {
					position--;
					read_ptr--;
				    }
				    read_count--;
				    *end_ptr = ' ';
				    end_ptr--;

				    wmove(win_fld,prompt_row,position); 		/* reset cursor position on screen
*/
				    wrefresh(win_fld);
				}

				break;					/* leave switch */
			
			case '\014':					/* ^L, refresh screen */
				wrefresh(curscr);
				break;
			
			default:					/* function key entered */
				end_read_field = 1;			/* flag to leave while loop */
				ret_val = c;
				break;					/* leave switch */
		   }


		}  /* end else */

	}  /* end while */

	
	 /* Either the field has been entered or it has not been entered. */
	wattrset(win_fld,0);	/* set attributes back to normal mode */

        if (!read_count) {
                if (old_entry_exists) {                                 /* an old entry existed in this field, so to wipe it
out,*/
                        read_buf[0] = ' ';                              /* set read_buf[0] = ' '; this happens when the user
*/
			if ( entry_changed )
                        	*field_entered = 1;                             /* clears out an old entry from the screen field */
                }
                else {                                                  /* set local int, field_entered to 0, */
                        *field_entered = 0;                             /* since field not entered */
                }
                                                /* no char's entered; user entered <CR> or function key pressed w/o entering
char */
                silent = 0;                     /* read_field() is to echo char's */
                *should_clear_ptr = 0;          /* set flag to not clear field on entry of 1st char */
 
                /* Unhighlight entry field */
                alter_highlight(win_fld, prompt_row, min_position, max_position, saved_highlight);
                wattrset(win_fld, 0);                           /* set attributes back to normal mode */
                return(ret_val);
        }
        else {                          /* field has been entered; if it is a 1 char field; then do not null terminate; */
                                        /* otherwise null terminate it */
		if ( entry_changed )
                	*field_entered = 1;                             /* since field has been entered */
                if (max_chars == 1) {                           /* it's a 1 char field */
                        silent = 0;                             /* read_field() is to echo char's */
                        *should_clear_ptr = 0;                  /* set flag to not clear field on entry of 1st char */
 
                        /* Unhighlight entry field */
                        alter_highlight(win_fld, prompt_row, min_position, max_position, saved_highlight);
                        wattrset(win_fld, 0);                           /* set attributes back to normal mode */
                        return(ret_val);
                }
                else {                                          /* more than 1 char field; null terminate */
                        *end_ptr++ = '\0';
                        strip_blanks(read_buf,max_chars,read_count);
                        silent = 0;                             /* read_field() is to echo char's */
                        *should_clear_ptr = 0;                  /* set flag to not clear field on entry of 1st char */
 
                        /* Unhighlight entry field */
                        alter_highlight(win_fld, prompt_row, min_position, max_position, saved_highlight);
                        wattrset(win_fld, 0);                           /* set attributes back to normal mode */
                        return(ret_val);
                } 
        }
}
 

/* clear_field: clears the field area (called by read_field() ) */
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
void initialize_read_buf(win_fld,ptr_scr_fld,piu,read_buf,ptr_read_ptr,ptr_end_ptr,ptr_count,ptr_old_entry_exists)
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
	int c;	/* for raeding curses internal buffers */
	struct scr_flds *ptr_to_fld;	/* pointer to piu struct of field being read */
	char *walk_ptr;	/* to walk thru read_buf[] */
	int read_count;	/* count of char's in field */


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
	int index;
	char *ptr;	/* to walk thru read_buf[] */

	index = ( max_chars  < chars_entered ) ? (max_chars-1):(chars_entered-1);

	ptr = &read_buf[index];

	/* strip off trailing blanks - if any */
	while ( *ptr == ' '  && index != 0 )  {
		*ptr-- = '\0';
		index--;
	}  /* end while */

}  /* end strip_blanks()  */

