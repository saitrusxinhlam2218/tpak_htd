/*
 *                 RCS INFO
 *
 *  @(#)  $RCSfile: Scrn.c,v $
 *  @(#)  $Revision: 1.2 $
 *  @(#)  $Date: 2002/03/29 14:53:44 $
 *  @(#)  $Author: jwelch $
 *  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/lib/SCRN/Scrn.c,v $
 *
 *  Copyright (c) 1991 - Motorola Mobile Data Division, Seattle, WA
 *
 */

#include <strings.h>
#include <sys/types.h>

#include "taxipak.h"
#include "Object.h"
#define SCREEN_SRC
#include "Scrn.h"
#include "tokenize.h"

/*
  Scrn_start(): initialize curses, setup terminal. Must be
  first screen routine called.
*/
void
Scrn_start()
{
  initscr();
  term_setup(getenv("TERM"));
  config_tty();
  noecho();
  cbreak();
  clear();

  Scrn_started = TRUE;
}

/*
  Scrn_end(): End curses. Should be called before program exits.
*/
void
Scrn_end()
{
  if (Scrn_started)
  {
    nocbreak();
    echo();
    endwin();
    Scrn_started = FALSE;
  }
}

/*
Scrn_update_field_num: Update the current field number for a screen.

The key_entered value will determine where the field. To go to the
previous field in the screen array, use KEY_UP or PREV_FIELD (which is
not a key, but a constant that exists just to tell Scrn_update_field
to go to the previous field). To go to the next field, use KEY_DOWN,
return, line feed or NEXT_FIELD. To jump to the field on the other
column of the screen use the TAB key. Normally, this function is
called in the exit functions of a field. In most cases the exit
function passes key_entered blindly, with Scrn_update_field_num,
satisfying the user's input for the current field.  If key_entered is
not a key that will change the current field, the current field number
is left unchanged.

Arguments:
	SCRN *screen;		pointer to screen struct.
	int  key_entered;	key used to change field position

Returns: None.
*/

void
Scrn_update_field_num(SCRN *screen, int key_entered)
{
  int start_field_num;
  RET_STATUS rs;

  start_field_num = screen->curr_field_num;

  do
  {

    if (key_entered == KEY_UP || key_entered == PREV_FIELD)
    {
      screen->curr_field_num--;
      if (screen->curr_field_num < 0)
	screen->curr_field_num = screen->num_fields - 1;
    }
    
    else if (key_entered == KEY_DOWN ||
	     key_entered == '\n' ||
	     key_entered == '\r' ||
	     key_entered == NEXT_FIELD)
    {
      screen->curr_field_num++;
      if (screen->curr_field_num >= screen->num_fields)
	screen->curr_field_num = 0;
    }

    /* TAB entered */
    else if (key_entered == '\011')
    {
      /* -1 at TAB field means TAB has no effect on field */
      if (screen->field[screen->curr_field_num].jump_field_id == -1)
	return;

      rs = Scrn_get_field_num_from_id(screen, &(screen->curr_field_num),
				      screen->field[screen->curr_field_num].jump_field_id);
      /* Invalid TAB field, don't change current field number */
      if (rs != SUCCESS)
	return;

      /* In case there's a problem with the jump field (e.g. set to a
	 display only field), search down the fields
	 until a good one is found */
      key_entered = KEY_DOWN;
      
    }
  }
  while ((screen->field[screen->curr_field_num].field_use == DISPLAY_ONLY ||
	  screen->field[screen->curr_field_num].field_use == TURNED_OFF) &&
	 screen->curr_field_num != start_field_num);
}

/*

Scrn_get_field_num_from_id:
	Given a field id, find its location in a screen structure.

Arguments:
	SCRN *screen;		pointer to screen struct.
	int *field_num_ptr;	Where the passed in field_id is found.
	int field_id;		The field id to find in the screen.

Returns:
	SUCCESS if field_id is found in the screen.
	FAIL is the field_is is not found.
*/

RET_STATUS
Scrn_get_field_num_from_id(SCRN *screen, int *field_num_ptr, int field_id)
{
  int  i;
  static int previous_field_num = -1;
  static SCRN *previous_screen = NULL;

  /* If same screen as previous search, start where
     you left off, which will the case often enough
     to make it worthwhile to do so */
  if (previous_screen == screen)
  {

    /* Search from previous field, to last one */
    i = previous_field_num;
    while (i < screen->num_fields)
    {
      if (FIELD_ID(screen, i) == field_id)
      {
	*field_num_ptr = i;
	previous_field_num = i;
	return(SUCCESS);
      }
      i++;
    }

    /* Search from start to previous field */
    i = 0;
    while (i < previous_field_num)
    {
      if (FIELD_ID(screen, i) == field_id)
      {
	*field_num_ptr = i;
	previous_field_num = i;
	return(SUCCESS);
      }
      i++;
    }
    previous_field_num = -1;
    previous_screen = NULL;
    return(FAIL);
  }

  /* Search the entire screen array */

  i = 0;
  while (i < screen->num_fields)
  {
    if (FIELD_ID(screen, i) == field_id)
    {
      *field_num_ptr = i;
      previous_field_num = i;
      previous_screen = screen;
      return(SUCCESS);
    }
    i++;
  }
  previous_field_num = -1;
  previous_screen = NULL;
  return(FAIL);
}

/*
Scrn_print_field_by_num:

Print the contents on a field specified by its number.

Arguments:
	SCRN *screen;		pointer to screen struct.
	int field_num;		The field number to print.

Returns: None.

Warnings:
Does not refresh the screen.

*/

void
Scrn_print_field_by_num(SCRN *screen, int field_num)
{
  BOOLEAN clear_it;

  if (screen->field[field_num].field_use == TURNED_OFF)
    return;

  /* Clear fields that are blank if zero */
  if (screen->field[field_num].flags & DONT_DISPLAY_ZERO)
  {
    clear_it = FALSE;
    switch (screen->field[field_num].type)
    {
      case INT_TYPE:
      if (*((int *) screen->field[field_num].data) == 0)
	clear_it = TRUE;
      break;
      
      case SHORT_TYPE:
      if (*((short *) screen->field[field_num].data) == 0)
	clear_it = TRUE;
      break;
      
      case FLOAT_TYPE:
      if (*((float *) screen->field[field_num].data) == 0.0)
	clear_it = TRUE;
      break;
      
      default:
      break;
    }
    if (clear_it)
    {
      Scrn_clear_field_by_num(screen, field_num);
      return;
    }
  }

  /* Display only fields are not printed with underlines */
  if ( (screen->field[field_num].field_use != DISPLAY_ONLY) &&
       !((screen->field[field_num].flags & NO_UNDERLINE)) )
    wattrset(screen->window, A_UNDERLINE);
  else
    wattrset(screen->window, A_NORMAL);

  if ( screen->field[field_num].flags & BOLD_TEXT )
    wattrset( screen->window, A_BOLD );

  if ( screen->field[field_num].flags & REVERSE_TEXT )
    wattrset( screen->window, A_REVERSE );

  if (screen->field[field_num].data)
  {
    switch (screen->field[field_num].type)
    {
      case STRING_TYPE:
      mvwprintw(screen->window,
		screen->field[field_num].row,
		screen->field[field_num].col,
		screen->field[field_num].format,
		(char *) screen->field[field_num].data);
      break;

      case CHAR_TYPE:
      mvwprintw(screen->window,
		screen->field[field_num].row,
		screen->field[field_num].col,
		screen->field[field_num].format,
		*((char *) screen->field[field_num].data));
      break;

      case INT_TYPE:
      mvwprintw(screen->window,
		screen->field[field_num].row,
		screen->field[field_num].col,
		screen->field[field_num].format,
		*((int *) screen->field[field_num].data));
      break;

      case SHORT_TYPE:
      mvwprintw(screen->window,
		screen->field[field_num].row,
		screen->field[field_num].col,
		screen->field[field_num].format,
		*((short *) screen->field[field_num].data));
      break;

      case FLOAT_TYPE:
      mvwprintw(screen->window,
		screen->field[field_num].row,
		screen->field[field_num].col,
		screen->field[field_num].format,
		*((float *) screen->field[field_num].data));
      break;

      default:
      break;
    }
  }
}

/*
Scrn_print_current_field:

Print the data in the current field.

Arguments:
        SCRN *screen;           pointer to screen struct.

Returns: None.

Warning: Does not refresh the screen.
*/

void
Scrn_print_current_field(SCRN *screen)
{
  Scrn_print_field_by_num(screen, screen->curr_field_num);
}

/*
Scrn_print_field_by_id:

Given a field id, print it value on the screen.

Arguments:
        SCRN *screen;           pointer to screen struct.
	int field_id;		field id to print.

Returns:
	SUCCESS if field_id exists in screen.
	FAIL if field_id is not found on screen.

Warning: Does not refresh the screen.
*/

RET_STATUS
Scrn_print_field_by_id(SCRN *screen, int field_id)
{
  int field_num;
  RET_STATUS rs;

  rs = Scrn_get_field_num_from_id(screen, &field_num, field_id);
  if (rs != SUCCESS)
    return(FAIL);

  Scrn_print_field_by_num(screen, field_num);

  return(SUCCESS);
}

/*

Scrn_highlight_field_by_num:

Highlight a field referenced by its number with a certain attribute.

Arguments:
	SCRN *screen;		pointer to screen struct.
	int field_num;		The field number to highlight.
	int attr;		The curses attribute.
Returns: None.

Warnings:
Does not refresh the screen.

*/

void
Scrn_highlight_field_by_num(SCRN *screen, int field_num, int attr)
{
  int  i;
  int  c;

  wmove(screen->window,
	screen->field[field_num].row,
	screen->field[field_num].col);
  wattrset(screen->window, A_UNDERLINE | attr);
  for (i = 0; i < screen->field[field_num].max_chars; i ++)
  {
    c = winch(screen->window) & A_CHARTEXT;
    waddch(screen->window, c);
  }
}
  
/*

Scrn_highlight_current_field:

Highlight the current field with a certain attribute.

Arguments:
	SCRN *screen;		pointer to screen struct.
	int attr;		The curses attribute.

Returns: None.

Warnings:
Does not refresh the screen.

*/
void
Scrn_highlight_current_field(SCRN *screen, int attr)
{
  Scrn_highlight_field_by_num(screen, screen->curr_field_num, attr);
}

/*
Scrn_highlight_field_by_id

Print the contents of a field specified by its id.

Arguments:
        SCRN *screen;           pointer to screen struct.
	int field_id;		field id to print.
	int attr;		The curses attribute.

Returns:
	SUCCESS if field_id exists in screen.
	FAIL if field_id is not found on screen.

Warning: Does not refresh the screen.
*/


RET_STATUS
Scrn_highlight_field_by_id(SCRN *screen, int field_id, int attr)
{
  int field_num;
  RET_STATUS rs;

  rs = Scrn_get_field_num_from_id(screen, &field_num, field_id);
  if (rs != SUCCESS)
    return(FAIL);

  Scrn_highlight_field_by_num(screen, field_num, attr);

  return(SUCCESS);
}


/*
Scrn_clear_field_by_num:

Clear the contents of a field specified by its number.
Display-only fields are cleared with no attributes, other fields
keep their underlining.

Arguments:
	SCRN *screen;		pointer to screen struct.
	int field_num;		The field number to clear.

Returns: None.

Warnings:
Does not refresh the screen.

*/
void
Scrn_clear_field_by_num(SCRN *screen, int field_num)
{
  int  i;

    if (screen->field[field_num].field_use == TURNED_OFF)
      {
        wmove(screen->window, screen->field[field_num].row, screen->field[field_num].col);
        for (i = 0; i < screen->field[field_num].max_chars; i ++)
          waddch(screen->window, ' ');        
        return;
      }

  if (screen->field[field_num].field_use == DISPLAY_ONLY)
    wattrset(screen->window, A_NORMAL);
  else
    wattrset(screen->window, A_UNDERLINE);

  wmove(screen->window, screen->field[field_num].row, screen->field[field_num].col);
  for (i = 0; i < screen->field[field_num].max_chars; i ++)
    waddch(screen->window, ' ');
}

/*
Scrn_clear_current_field:

Clear the contents on a field specified by its number.
Display- only fields are cleared with no attributes, other fields
keep their underlining.

Arguments:
	SCRN *screen;		pointer to screen struct.

Returns: None.

Warnings:
Does not refresh the screen.

*/
void
Scrn_clear_current_field(SCRN *screen)
{
  Scrn_clear_field_by_num(screen, screen->curr_field_num);
}

/*
Scrn_clear_field_by_id:

Clear the contents of a field specified by its number.
Display-only fields are cleared with no attributes, other fields
keep their underlining.

Arguments:
	SCRN *screen;		pointer to screen struct.
	int field_id;		The field id to clear.

Returns:
	SUCCESS if field_id exists in screen.
	FAIL if field_id is not found on screen.

Warnings:
Does not refresh the screen.

*/
RET_STATUS
Scrn_clear_field_by_id(SCRN *screen, int field_id)
{
  int field_num;
  RET_STATUS rs;

  rs = Scrn_get_field_num_from_id(screen, &field_num, field_id);
  if (rs != SUCCESS)
    return(FAIL);

  Scrn_clear_field_by_num(screen, field_num);

  return(SUCCESS);
}

/*
Scrn_print_all_fields:

Print the data for all the fields defined for a screen.

Arguments:
	SCRN *screen;		pointer to screen struct.

Returns: None
*/
void
Scrn_print_all_fields(SCRN *screen)
{
  int  i;

  for (i = 0; i < screen->num_fields; i ++)
    Scrn_print_field_by_num(screen, i);
}

/*
Scrn_clear_all_fields:

Clear the data for all the fields defined for a screen.

Arguments:
	SCRN *screen;		pointer to screen struct.

Returns: None
*/
void
Scrn_clear_all_fields(SCRN *screen)
{
  int  i;

  for (i = 0; i < screen->num_fields; i ++)
    Scrn_clear_field_by_num(screen, i);
}

/*
Scrn_set_field_use_by_num:

Change the field use.

Arguments:
	SCRN *screen;		pointer to screen struct.
	int field_num;		The field number to set.
	int field_use;		SILENT, ENTERED, DISPLAY_ONLY, TURNED_OFF.

Returns: None.

Warnings:
Does not refresh the screen.
*/
void
Scrn_set_field_use_by_num(SCRN *screen, int field_num, int field_use)
{
  int  i;

  if (screen->field[field_num].field_use == field_use)
    return;

  if (field_use == TURNED_OFF)
  {
    /* Set temporarly to DISPLAY_ONLY so clear_field does not underline */
    screen->field[field_num].field_use = DISPLAY_ONLY;
    Scrn_clear_field_by_num(screen, field_num);
    screen->field[field_num].field_use = TURNED_OFF;
  }

  else
  {
    screen->field[field_num].field_use = field_use;
    Scrn_print_field_by_num(screen, field_num);
  }
}

/*
Scrn_set_current_field_use:

Set the field use on the current field.

Arguments:
	SCRN *screen;		pointer to screen struct.
	int field_use;		SILENT, ENTERED, DISPLAY_ONLY, TURNED_OFF.

Returns: None.

Warnings:
Does not refresh the screen.
*/
void
Scrn_set_current_field_use(SCRN *screen, int field_use)
{
  Scrn_set_field_use_by_num(screen, screen->curr_field_num, field_use);
}

/*
Scrn_set_field_use_by_id:

Set a field use referenced by id.

Arguments:
	SCRN *screen;		pointer to screen struct.
	int field_id;		The field id to set.
	int field_use;		SILENT, ENTERED, DISPLAY_ONLY, TURNED_OFF.

Returns:
	SUCCESS if field_id exists in screen.
	FAIL if field_id is not found on screen.

Warnings:
Does not refresh the screen.

*/
RET_STATUS
Scrn_set_field_use_by_id(SCRN *screen, int field_id, int field_use)
{
  int field_num;
  RET_STATUS rs;

  rs = Scrn_get_field_num_from_id(screen, &field_num, field_id);
  if (rs != SUCCESS)
    return(FAIL);

  Scrn_set_field_use_by_num(screen, field_num, field_use);

  return(SUCCESS);
}

/*
Scrn_default_field_use_all:

Set the field use of all the fields on a screen to the default

Arguments:
	SCRN *screen;		pointer to screen struct.

Returns: None.

Warnings:
Does not change anything on the screen.

*/
void
Scrn_default_field_use_all(SCRN *screen)
{
  int  i;

  for (i = 0; i < screen->num_fields; i ++)
    screen->field[i].field_use = screen->field[i].default_field_use;
}
/*
Scrn_init_screen:

Initialize parts of the screen data structure not specified in the standard
initilaization. These parts are the number of fields (screen->num_fields),
The current field number (screen->curr_field_num), the printing format
for the data for each field (unless the format is initialized already), and the
curses window where the screen is displayed.

Arguments:
	SCRN *screen;		pointer to screen struct.
	WINDOW *window;		pointer to curses window.

Returns: None.

Warnings: If there is any bogus information in the screen structure, an
abort will occur, and the screen definition must be recoded properly.
*/
void
Scrn_init_screen(SCRN *screen, WINDOW *window)
{
  int  i;
  char *format;
  char max_chars[FORMAT_LEN];

  for (i = 0; screen->field[i].field_id != -1; i ++)
  {
    
    screen->field[i].field_use = screen->field[i].default_field_use;

    /* Format has already been initialized */
    if (screen->field[i].format != NULL)
      continue;

    format = (char *) malloc(FORMAT_LEN);
    screen->field[i].format = format;
    
    strcpy(format, "%");
    
    if (screen->field[i].justification == LEFT)
      strcat(format, "-");
    else if (screen->field[i].justification != RIGHT)
    {
      fprintf(stderr, "Error in initialization of fields (justification)\n");
      abort();
    }
    
    sprintf(max_chars, "%d", screen->field[i].max_chars);
    strcat(format, max_chars);
    
    switch (screen->field[i].type)
    {
      case CHAR_TYPE:
        strcat(format, "c");
	break;
      
      case STRING_TYPE:
	strcat(format, "s");
	break;
      
      case INT_TYPE:
      case SHORT_TYPE:
	strcat(format, "d");
	break;
      
      case FLOAT_TYPE:
	strcat(format, "f");
	break;

      default:
	FATAL();
      }
    
  }

  screen->num_fields = i;
  screen->curr_field_num = screen->starting_field_num;
  screen->window = window;
}

/*
Scrn_print_error:

Print an error message (and beep) on the error message line of the stdscr.
Will guarantee at least one second between error messages to avoid having
message appear and dissappear too fast to read.

Arguments: (This has fake printf like variable arguments)
	char *format;		printing format (e.g. "%s", "%d")
	long p1, p2 .... p7;	arguments for the printing format.

Returns: None.
*/
void
Scrn_print_error(char *format,
		 long p1, long p2, long p3, long p4,
		 long p5, long p6, long p7, long p8)
{
  int start_col;
  char buf[READ_BUF_LEN + 1];
  time_t now_time;
  static char last_buf[READ_BUF_LEN + 1];
  static time_t last_time = 0;

  sprintf(buf, format, p1, p2, p3, p4, p5, p6, p7, p8);

  /* Just print to stderr if curses not initialized yet */
  //  if (!Scrn_started)
  //  {
  //    fprintf(stderr, buf);
  //    return;
  //  }

  move(ERROR_ROW, 0);
  clrtoeol();
  attrset(A_REVERSE);
  addstr(buf);
  Scrn_error_displayed = TRUE;
  attrset(A_NORMAL);

  /* If last time an error was displayed was between
     0 and 1 second, and not the same error as before,
     wait 1 second for next error display. The checking of
     the same error as before is to avoid the annoying
     problem of a whole bunch of the same errors appearing
     after another, with 1 second between each. */
  now_time = time(0);
  if (now_time - last_time <= 1 &&
      strcmp(buf, last_buf) != 0)
    sleep(1);

  beep();
  refresh();
  last_time = now_time;
  strcpy(last_buf, buf);
}

/*
Scrn_clear_error:

Clear the message from the error line.

Arguments: None

Returns: None
*/
void
Scrn_clear_error()
{
  if (Scrn_error_displayed)
  {
    move(ERROR_ROW, 0);
    clrtoeol();
    Scrn_error_displayed = FALSE;
    refresh();
  }
}

#if 0
This is skipped now since it's really overloading the data argument

RET_STATUS
Scrn_init_field(SCRN *screen, int field_id, void *ptr, void *data)
{
  RET_STATUS rs;
  int field_num;

  rs = Scrn_get_field_num_from_id(screen, &field_num, field_id);
  if (rs != SUCCESS)
    return(FAIL);

  switch (screen->field[field_num].type)
  {
    case CHAR_TYPE:
    *((char *) ptr) = (char) data;
    *((char *) screen->field[field_num].data) = (char) data;
    break;

    case STRING_TYPE:
    strcpy((char *) ptr, (char *) data);
    strcpy((char *) screen->field[field_num].data, (char *) data);
    break;

    case INT_TYPE:
    *((int *) ptr) = (int) data;
    *((int *) screen->field[field_num].data) = (int) data;
    break;

    case SHORT_TYPE:
    *((short *) ptr) = (short) data;
    *((short *) screen->field[field_num].data) = (short) data;
    break;

    case FLOAT_TYPE:
    *((float *) ptr) = *((float *) data);
    *((float *) screen->field[field_num].data) = *((float *) data);
    break;
  }

  return(SUCCESS);
}
*/
#endif

RET_STATUS
Scrn_init_char_field(SCRN *screen, int field_id, char data)
{
  RET_STATUS rs;
  int field_num;

  rs = Scrn_get_field_num_from_id(screen, &field_num, field_id);
  if (rs != SUCCESS)
    return(FAIL);

  if (screen->field[field_num].type != CHAR_TYPE)
    FATAL();

  *((char *) screen->field[field_num].data) = data;

  return(SUCCESS);
}

RET_STATUS
Scrn_init_string_field(SCRN *screen, int field_id, char *data)
{
  RET_STATUS rs;
  int field_num;

  rs = Scrn_get_field_num_from_id(screen, &field_num, field_id);
  if (rs != SUCCESS)
    return(FAIL);

  if (screen->field[field_num].type != STRING_TYPE)
    FATAL();
      
  strcpy((char *) screen->field[field_num].data, data);

  return(SUCCESS);
}

RET_STATUS
Scrn_init_int_field(SCRN *screen, int field_id, int data)
{
  RET_STATUS rs;
  int field_num;

  rs = Scrn_get_field_num_from_id(screen, &field_num, field_id);
  if (rs != SUCCESS)
    return(FAIL);

  if (screen->field[field_num].type != INT_TYPE)
    FATAL();

  *((int *) screen->field[field_num].data) = data;

  return(SUCCESS);
}

RET_STATUS
Scrn_init_short_field(SCRN *screen, int field_id, short data)
{
  RET_STATUS rs;
  int field_num;

  rs = Scrn_get_field_num_from_id(screen, &field_num, field_id);
  if (rs != SUCCESS)
    return(FAIL);

  if (screen->field[field_num].type != SHORT_TYPE)
    FATAL();

  *((short *) screen->field[field_num].data) = data;

  return(SUCCESS);
}

RET_STATUS
Scrn_init_float_field(SCRN *screen, int field_id, float data)
{
  RET_STATUS rs;
  int field_num;

  rs = Scrn_get_field_num_from_id(screen, &field_num, field_id);
  if (rs != SUCCESS)
    return(FAIL);

  if (screen->field[field_num].type != FLOAT_TYPE)
    FATAL();

  *((float *) screen->field[field_num].data) = (float) data;

  return(SUCCESS);
}


int
Scrn_process(SCRN *screen)
{
  char read_buf[READ_BUF_LEN + 1];
  BOOLEAN field_changed;
  BOOLEAN done_with_screen;
  BOOLEAN exit_screen;
  BOOLEAN error_last_time;
  int key_entered;
  RET_STATUS  rs;
  int curr_field_num;

  wclear(screen->window);

  if (screen->background_text_func)
    (*screen->background_text_func)(screen);

  if (screen->init_func)
    (*screen->init_func)(screen);

  Scrn_print_all_fields(screen);

  if (screen->post_init_func)
    (*screen->post_init_func)(screen);

  field_changed = FALSE;
  done_with_screen = FALSE;
  exit_screen = FALSE;
  error_last_time = FALSE;

  while (!exit_screen)
  {
    while (!done_with_screen)
    {
      wrefresh(screen->window);
      read_buf[0] = EOS;
      Scrn_read_field(screen, read_buf, &field_changed, &key_entered, error_last_time);
      error_last_time = FALSE;
      Scrn_clear_error();
      if (screen->pre_key_func)
      {
	rs = (*screen->pre_key_func)(screen, key_entered, &done_with_screen);
	if (rs == SUCCESS)
	  continue;
      }

      /* Save current field since the check_func will likely change it */
      curr_field_num = screen->curr_field_num;

      if (screen->field[screen->curr_field_num].check_func)
      {
      	rs = (*screen->field[screen->curr_field_num].check_func)(screen, read_buf,
							       key_entered,
							       field_changed);
	if (rs != SUCCESS)
	{
	  /* On an error that stays on the same field
	     let Scrn_read_field() know that there was
	     an error last time through */
	  if (curr_field_num == screen->curr_field_num)
	    error_last_time = TRUE;
	  continue;
	}
      }

      /* Print field as set by the exit/check function */
      if (screen->field[curr_field_num].field_use == ENTRY)
	Scrn_print_field_by_num(screen, curr_field_num);
      
      if (screen->post_key_func)
	(*screen->post_key_func)(screen, key_entered, &done_with_screen);
    }
    
    if (screen->exit_func)
      (*screen->exit_func)(screen, key_entered, &exit_screen);
  }
  return(key_entered);
}

/*
  Scrn_direction(): return -1 if up arrow, 1 if return, line feed or
  down arrow. 0 otherwise.
*/
int
Scrn_direction(int key_entered)
{
  if (key_entered == KEY_UP)
    return(-1);

  if (key_entered == '\n' ||
      key_entered == '\r' ||
      key_entered == KEY_DOWN)
    return(1);

  return(0);
}

/*
  Scrn_center(): Print a message centered on a window line.
*/
void
Scrn_center(WINDOW *window, int row, char *format,
	    long p1, long p2, long p3, long p4,
	    long p5, long p6, long p7, long p8)
{
  int start_col;
  char buf[READ_BUF_LEN + 1];

  sprintf(buf, format, p1, p2, p3, p4, p5, p6, p7, p8);

  start_col = (window->_maxx - strlen(buf)) / 2;
  if (start_col < 0)
    start_col = 0;

  mvwaddstr(window, row, start_col, buf);
}

/*
  Scrn_left(): Print a message flushed left on a window line.
*/
void
Scrn_left(WINDOW *window, int row, char *format,
     long p1, long p2, long p3, long p4, long p5, long p6, long p7, long p8)
{
  int start_col;
  char buf[READ_BUF_LEN + 1];

  sprintf(buf, format, p1, p2, p3, p4, p5, p6, p7, p8);

  start_col = window->_maxx - strlen(buf);
  if (start_col < 0)
    start_col = 0;

  mvwaddstr(window, row, start_col, buf);
}

/*
  Scrn_set_current_field_num(): Set the current field number for a screen.
*/
void
Scrn_set_current_field_num(SCRN *screen, int field_num)
{
  screen->curr_field_num = field_num;
}

/*
  Scrn_set_current_field_id(): Set the current field id for a screen.
*/
void
Scrn_set_current_field_id(SCRN *screen, int field_id)
{
  RET_STATUS rs;
  int field_num;

  rs = Scrn_get_field_num_from_id(screen, &field_num, field_id);
  if (rs == SUCCESS)
    Scrn_set_current_field_num(screen, field_num);
}


/*
  Scrn_alter_highlight(): Highlight a portion of a window line.
*/
void
Scrn_alter_highlight(WINDOW *window,
		     int y,
		     int min_x,
		     int max_x,
		     int attr)
{
  int  i;
  
  wmove(window, y, min_x);
  wattrset(window, attr);
  for (i = min_x; i <= max_x; i ++)
    /* The "(winch(window) & A_CHARTEXT) | attr" is because
       some Ultrix versions of curses are screwed up and
       this forces the printing the chars with the proper
       attributes */
    waddch(window, (winch(window) & A_CHARTEXT) | attr);
  
  /* Move cursor to start of field */
  wmove(window, y, min_x);
  
  wrefresh(window);
}

/*
  Scrn_status_line(): Print a message on the status line of
  a terminal (if it has one).
*/
void
Scrn_status_line(message)
char *message;
{
  static char last_message[READ_BUF_LEN + 1];
  
  /* Don't do anything if status line has not changed */
  if (strcmp(message, last_message) == 0)
    return;
  
  if (message[0])
    printf("%s%s%s", Status_line_start, message, Status_line_end);
  
  /* Empty message, wipe out status line */
  else
    printf("%s%130s%s", Status_line_start, "", Status_line_end);
  
  /* Force display */
  fflush(stdout);
  
  strcpy(last_message, message);
  
  return;
}  /* end Scrn_status_line */

/*
  Scrn_clear_lines(): Clear specifield lines in a window.
*/
void
Scrn_clear_lines(WINDOW *window, int start_line, int end_line)
{
  int i;

  for (i = start_line; i <= end_line; i ++)
  {
    /* There is a problem in curses that
       requires some non-space character
       printed on a line before a line is erased */
    mvwaddch(window, i, 0, '.');
    wmove(window, i, 0);
    wclrtoeol(window);
  }
}

/*
  Scrn_clear: Clear all but the error line. This is used often
  to preserve the error message for the user to read.
*/
void
Scrn_clear()
{
  Scrn_clear_lines(stdscr, 0, ERROR_ROW - 1);
}
