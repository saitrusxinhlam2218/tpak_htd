/*
 *                 RCS INFO
 *
 *  @(#)  $RCSfile: Scrn_read.c,v $
 *  @(#)  $Revision: 1.2 $
 *  @(#)  $Date: 2002/03/29 14:53:45 $
 *  @(#)  $Author: jwelch $
 *  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/lib/SCRN/Scrn_read.c,v $
 *
 *  Copyright (c) 1991 - Motorola Mobile Data Division, Seattle, WA
 *
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>

#include "taxipak.h"
#include "Object.h"
#include "Scrn.h"

void
fix_spaces(char *buf)
{
  int len;
  int i;
  int blanks;

  len = strlen(buf);

  /* Strip trailing spaces */
  for (i = len - 1; i >= 0; i --)
  {
    if (buf[i] == ' ')
    {
      buf[i] = EOS;
      len--;
    }
    else
      break;
  }

  /* Strip leading blanks */

  /* Count leading blanks */
  for (blanks = 0; blanks < len; blanks ++)
  {
    if (buf[blanks] != ' ')
      break;
  }

  if (blanks > 0)
  {    
    for (i = 0; i <= len - blanks; i ++)
      buf[i] = buf[i + blanks];
  }

}


 

RET_STATUS
Scrn_read_field(SCRN *screen,
		char *read_buf,
		BOOLEAN *field_changed,
		int *key_entered,
		BOOLEAN error_last_time)
{
  int c;	/* for reading a char */
  int cur_col;	/* columnar cur_col of the cursor */
  int min_col;	/* min columnar position in field */
  int max_col;
  int cur_len;
  int r_index;	/* count of number of char's read into 
			 * the field */
  BOOLEAN end_read_field;	/* flag; 0/1; set if end the function */
  int row;	/* row of field */
  int col;	/* col of field */
  int max_chars;		/* max chars in field */
  int len;
  FIELD *ptr_to_fld;	/* pointer to appropriate slot in 
				 * scr_flds structure in  the array
				 * of field_num structures */
  int  i;
  int  diff;
  int saved_highlight;
  int char_count;
  
  
  char_count = 0;
  *field_changed = FALSE;
  
  /* initialize ptr_to_fld to point to the right structure in the 
   * array of structures scr_flds
   */
  ptr_to_fld = &(screen->field[screen->curr_field_num]);
  
  /* Initilaize field boundaries of the field for entry. */
  row = ptr_to_fld->row;
  col = ptr_to_fld->col;
  max_chars = ptr_to_fld->max_chars;
  
  min_col = col;
  max_col = col + max_chars - 1;
  cur_col = min_col;

  /* Move the cursor to the appropriate position to read the field. */
  wmove(screen->window, row, cur_col);
  
  /* Initilaize counters and pointers to read field. */
  r_index = 0;
  
  /* Clear out the entry storage area */
  bzero(read_buf, max_chars + 1);

  if (screen->field[screen->curr_field_num].field_use != SILENT &&
      screen->field[screen->curr_field_num].data != NULL)
  {
    switch (screen->field[screen->curr_field_num].type)
    {
      case STRING_TYPE:
      strcpy(read_buf, (char *)screen->field[screen->curr_field_num].data);
      break;
      
      case CHAR_TYPE:
      read_buf[0] = *((char *) screen->field[screen->curr_field_num].data);
      read_buf[1] = EOS;
      break;
      
      case INT_TYPE:
      sprintf(read_buf, "%d", *((int *) screen->field[screen->curr_field_num].data));
      break;
      
      case SHORT_TYPE:
      sprintf(read_buf, "%d", *((short *) screen->field[screen->curr_field_num].data));
      break;
      
      case FLOAT_TYPE:
      sprintf(read_buf, "%f", *((float *) screen->field[screen->curr_field_num].data));
      break;
    }
  }

  len = strlen(read_buf);

  /*   Enter a while loop which executes as long as the field is 
   *   being entered. If the user enters a <CR> or a newline,
   *   or a function key, the field entry exits
   */
  
  /* Getting the current highlight type at input field */
  saved_highlight = (int)mvwinch(screen->window, row, min_col) & A_ATTRIBUTES;
  
  /* Highlight entry field */
  Scrn_alter_highlight(screen->window, row,
		       min_col, max_col, A_REVERSE | A_UNDERLINE);
  
  
  /* Start data entry at end of field */
  if (ptr_to_fld->flags & START_AT_END)
  {
    r_index = len;
    cur_col += len;
    wmove(screen->window, row, cur_col);
  }
  wrefresh(screen->window);

  end_read_field = FALSE;
  
  cur_len = len;

  /* In case error in data */
  if (cur_len > max_chars)
  {
    cur_len = max_chars;
    read_buf[cur_len] = EOS;
  }

  /* Single character mode read: Each key entered
     overwrites the previous one. */
  if (ptr_to_fld->flags & SINGLE_CHAR)
  {
    while (1)
    {
      c = *key_entered = wgetkey(screen->window);
      if (isascii(c) && isprint(c))
      {
	/* Upcase if flag set */
	if (ptr_to_fld->flags & UPPER) 
	  c = Toupper(c);

	if (screen->field[screen->curr_field_num].field_use != SILENT)
	{
	  /* Keep on setting attribute becuase they
	     can be reset on call to status line */
	  wattrset(screen->window, A_REVERSE | A_UNDERLINE);
	  waddch(screen->window, c);  /* echo char on screen */
	}
	wmove(screen->window, row, cur_col);
	wrefresh(screen->window);

	read_buf[0] = c;
      }
      else if (c == '\n' ||
	       c == '\r' ||
	       c == KEY_UP ||
	       c == KEY_DOWN ||
	       (c >= KEY_F(0) && c <= KEY_F(64)))
	break;

      /* ^L: refresh screen */
      else if (c == '\014')
	wrefresh(curscr);
      else
	beep();
    }
    goto finish_up;
  }

  while (!end_read_field)
  {
    
    /* If field entry exits when last character is entered */
    if (r_index == max_chars &&
	(ptr_to_fld->flags & EXIT_AT_END))
    {
      /* Must tell caller function that user
	 wants to go to the next field */
      *key_entered = '\n';
      break;
    }
    
    c = *key_entered = wgetkey(screen->window);
    char_count++;

    if (c == ERR)
      continue;
    
    
    /* Chars entered are printable, function keys, or screen editing keys
       (KEY_RIGHT at end of entered line is same as space) */

    if ( c == KEY_RIGHT && screen->name == 99 )
      {
        c = '\011';
        *key_entered = '\011';
      }

    if (isascii(c) && isprint(c) ||
	(c == KEY_RIGHT && r_index == cur_len))
    {	

      if (c == KEY_RIGHT && r_index == cur_len)
	c = (int) ' ';

      /* Only clear on entry on very first char entered */
      if (char_count == 1 && (ptr_to_fld->flags & CLEAR_ON_ENTRY))
      {
	bzero(read_buf, max_chars + 1);
	Scrn_clear_current_field(screen);
	/* Highlight entry field */
	Scrn_alter_highlight(screen->window, row,
			     min_col, max_col, A_REVERSE | A_UNDERLINE);
	wmove(screen->window, row, cur_col);
	cur_len = 0;
	r_index = 0;
      }
      *field_changed = TRUE;

      /* If field is full, must wait until return/arrow key/function key */
      if (r_index == max_chars)
	continue;

      /* It's a printable char */

      /* Upcase appropiate characters */
      if (ptr_to_fld->flags & UPPER) 
	c = Toupper(c);

      /* At end of entered data */
      if ( ptr_to_fld->flags & PHONE_FIELD )
	{
	  if ( c!= '.' &&
	      ( r_index == 2
	       || r_index == 5
	       || r_index == 8
	       || r_index == 11 )
	    )
	    {
	      read_buf[r_index] = '.';
	      waddch(screen->window, '.');
	      r_index++;
	      cur_len++;
	      cur_col++;
	      read_buf[r_index] = c;
	      r_index++;
	      cur_len++;
	      cur_col++;
	    }
	  else
	    {
	      read_buf[r_index] = c;
	      r_index++;
	      cur_col++;
	      cur_len++;
	    }
	}
      else if ( ptr_to_fld->flags & CREDIT_CARD_FIELD )
	{
	  if ( c != ' ' &&
	      ( r_index == 2 ||
	        r_index == 5 ||
	        r_index == 8 ||
	        r_index == 11 ||
	        r_index == 14 ||
	        r_index == 17 ||
	        r_index == 20 ||
	        r_index == 23 ) )
	    {
	      waddch(screen->window, ' ');
	      read_buf[r_index] = ' ';
	      r_index++;
	      cur_len++;
	      cur_col++;
	      read_buf[r_index] = c;
	      cur_len++;
	      cur_col++;
	      r_index++;
	    }
	  else
	    {
	      read_buf[r_index] = c;
	      r_index++;
	      cur_col++;
	      cur_len++;
	    }
	}
      else
	{
	  if (r_index == cur_len)
	    {
	      read_buf[r_index] = c;
	      r_index++;
	      read_buf[r_index] = EOS;
	      cur_len++;
	      cur_col++;
	    }

	  /* Somewhere in the middle */
	  else 
	    {
	      read_buf[r_index] = c;
	      r_index++;
	      cur_col++;
	    }
	}

      if ( screen->field[screen->curr_field_num].field_use == SILENT )
	wmove(screen->window, row, cur_col);
      else
      {
	/* Keep on setting attribute becuase they
	   can be reset on call to status line */
	wattrset(screen->window, A_REVERSE | A_UNDERLINE);

	waddch(screen->window, c);  /* echo char on screen */
      }

      wrefresh(screen->window);
      
    }  /* end if */

    /* Non-printable characters */
    else
    {
      switch(c) {
	case '\r':
	case '\n':
	  end_read_field = TRUE;
	  break;
	
	/* Left arrow key: non-destructive move towards left */
	case KEY_LEFT:
	  /* Cursor at start of field, nowhere to go */
	  if (r_index == 0)
	    beep();

	  /* Move cursor to the left */
	  else
	  {
	    r_index--;
	    cur_col--;
	    wmove(screen->window, row, cur_col);
	    wrefresh(screen->window);
	  }
	
	  break;
	
	/* Right arrow key: non destructive move towards right */
	case KEY_RIGHT:

	  /* cursor is on the last position in the field */
	  if (r_index == cur_len)
	    beep();
	
	  /* Move cursor to the right */
	  else
	  {
	    r_index++;
	    cur_col++;
	    wmove(screen->window, row, cur_col);
	    wrefresh(screen->window);
	  }
	  break;
	
	
	/* ^A: go to start of field */
	case '\01':
	  if (r_index != 0)
	  {
	    cur_col = min_col;
	    r_index = 0;
	    wmove(screen->window, row, cur_col);
	    wrefresh(screen->window);
	  }
	  break;
	  
	/* ^E: go to end of characters in field,
	   (not counting trailing spaces) */
	case '\05':
	  /* Skip trailing spaces */
	  for (i = cur_len - 1; i >= 0; i--)
	    if (read_buf[i] != ' ')
	      break;
	  r_index = i + 1;
	  cur_col = min_col + i + 1;
	  wmove(screen->window, row, cur_col);
	  wrefresh(screen->window);
	  break;
	  

	/* ^U: clear field */
	case '\025':
	  *field_changed = TRUE;
	  Scrn_clear_current_field(screen);

	  /* Re-highlight the field */
	  Scrn_alter_highlight(screen->window, row,
			       min_col, max_col, A_REVERSE | A_UNDERLINE);

	  /* Initilaize counters and pointers to read field. */
	  cur_col = min_col;
	  r_index = 0;
	  cur_len = 0;
	  bzero(read_buf, max_chars);
	  wmove(screen->window, row, cur_col);
	  wrefresh(screen->window);
	  break;
	
	/* Delete the previous character */
	case KEY_BACKSPACE:
	case KEY_DL:
	case '\b':

	  /* cursor is on the last char entered by user+1 */
	  if (r_index == cur_len && cur_len > 0)
	  {
	    /* Erase the last character */
	    mvwaddch(screen->window, row, cur_col - 1, ' ');
	    r_index--;
	    cur_col--;
	    cur_len--;
	    read_buf[r_index] = EOS;
	    mvwaddch(screen->window, row, cur_col, ' ');
	    *field_changed = TRUE;
	  }

	  /* At start of line -> erase first char and
	     shift the following to the left */
	  else if (r_index == 0 && cur_len > 0)
	  {
	    for (i = 1; i < cur_len; i++)
	    {
	      read_buf[i - 1] = read_buf[i];
	      mvwaddch(screen->window, row, min_col + i - 1, read_buf[i]);
	    }
	    read_buf[i - 1] = EOS;
	    mvwaddch(screen->window, row, min_col + i - 1, ' ');
	    cur_len--;
	    *field_changed = TRUE;
	  }
	  
	  /* In middle of entered chars */
	  else if (r_index > 0 && cur_len > 0)
	  {
	    for (i = r_index; i < cur_len; i ++)
	    {
	      read_buf[i - 1] = read_buf[i];
	      mvwaddch(screen->window, row, min_col + i - 1, read_buf[i]);
	    }
	    read_buf[i - 1] = EOS;
	    mvwaddch(screen->window, row, min_col + i - 1, ' ');
	    r_index--;
	    cur_col--;
	    cur_len--;
	    *field_changed = TRUE;
	  }

	  else if (cur_len == 0)
	    beep();

	  /* reset cursor position on screen */
	  wmove(screen->window, row, cur_col);
	  wrefresh(screen->window);

	  break;
	
	/* ^L: refresh screen */
	case '\014':
	  wrefresh(curscr);
	  break;
	
	/* ^W: This is a kludge to interupt during debugging without
	   being in a read (which causes dbx to get stuck in a read
	   forever */
	case '\027':
	sleep(5);
	cbreak();
	noecho();
	break;

	/* Function key entered */
	default:
	  end_read_field = TRUE;
	  break;
      }
      
      
    }  /* end else */
    
  }  /* end while */
  
finish_up:
  wattrset(screen->window, A_NORMAL);
  
  /* Restore highlighting of field */
  Scrn_alter_highlight(screen->window, row,
		       min_col, max_col, saved_highlight);

#ifdef FOO
  fix_spaces(read_buf);
#endif

  /* If there was an error last time through for the same
     field #, set field_changed to TRUE even if it hasn't
     changed because the data entered in the field is still
     wrong and field_changed = TRUE will force the verification
     function to validate the data again */
  if (error_last_time && !(*field_changed))
    *field_changed = TRUE;

  return(SUCCESS);
}
