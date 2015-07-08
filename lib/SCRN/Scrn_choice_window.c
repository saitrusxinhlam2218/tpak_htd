/*
 *                 RCS INFO
 *
 *  @(#)  $RCSfile: Scrn_choice_window.c,v $
 *  @(#)  $Revision: 1.3 $
 *  @(#)  $Date: 2003/01/28 16:58:23 $
 *  @(#)  $Author: jwelch $
 *  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/lib/SCRN/Scrn_choice_window.c,v $
 *
 *  Copyright (c) 1991 - Motorola Mobile Data Division, Seattle, WA
 *
 */


/* 
 * Scrn_do_choice
 *
 * This is a common window that is used to display up to 12 choices (at a time) 
 * of a list of information. The choices are numbered and the user is selects 
 * that appropriate choice via a single data entry field. The choices can be 
 * paged forward and backward.
 */
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
 

#include "taxipak.h"
#include "Object.h"
#include "func_keys.h"
#include "Scrn.h"
#include "Scrn_private.h"
#include "Individual_private.h"

/*************************************************

		Logic

Display the specified window with the specified 
choices from the choice_list structure. Store the 
handle corresponding to the user's choice in the 
choice variable. If the user didn't select an option, 
the choice is set to NULL.

invoke the next_page function


loop
	display window
	read response
	if numeric
		if in range
			*choice = choice_list->choice[i].data.
			return SUCCESS;
		else
			invalid range
	if previous_page key
		invoke prev_page function
		if NO_DATA
			display beginning of list message
	else if next_page key
		invoke next_page function
		if NO_DATA
			display end of list message
	else if exit key hit
		*choice = NULL;
		return SUCCESS;
ENDLOOP
 

**************************************************/

FIELD Choice_number_fields[] =
{
{1, 1, 15, 2, UPPER | DONT_DISPLAY_ZERO,
   NULL, SHORT_TYPE, LEFT, ENTRY, 0, -1, NULL, NULL, NULL},
{-1, -1, -1, -1, FALSE, NULL, 0, 0, 0, NULL, NULL}
};

SCRN Choice_number_screen =
{
  0, 0, 0, CHOICE_SCREEN, NULL,
  NULL, NULL, NULL,
  NULL, NULL,
  Choice_number_fields, NULL, NULL
};

FIELD Choice_filter_number_fields[] =
{
{1, 1, 15, 8, UPPER | DONT_DISPLAY_ZERO,
   NULL, STRING_TYPE, LEFT, ENTRY, 0, -1, NULL, NULL, NULL},
{-1, -1, -1, -1, FALSE, NULL, 0, 0, 0, NULL, NULL}
};

SCRN Choice_filter_number_screen =
{
  0, 0, 0, CHOICE_FILTER_SCREEN, NULL,
  NULL, NULL, NULL,
  NULL, NULL,
  Choice_filter_number_fields, NULL, NULL
};



static
RET_STATUS
Scrn_choices_to_window
(
	WINDOW				*window,
	SCRN_CHOICE_LIST		*choice_list
)

{
  int   x,y,nb_columns,c,i,digit_width;

	char			 string[SCRN_CW_MAX_STRING_LEN + 1];


    	wattrset(window, A_NORMAL);

	/* Clear choice area (but not top line) */
	for (y = 2; y < window->_maxy - 1; y ++)
	{
	  /* Ultrix curses is screwed up. It is necessary to put
	     something on a line before it will erase it */
	  mvwaddch(window, y, 1, '.');
	  wmove(window, y, 1);
	  wclrtoeol(window);
	}

	box( window, 0, 0 );
	
        if ( choice_list->display_len > SCRN_TWO_COL_MAX_STRING_LEN )
        {
	 	nb_columns = 1;
		
	  	/* Only single digit numbers for choices (up to 4) */
		digit_width = 1;
        }
        else
        {
                if ( choice_list->display_len > SCRN_THREE_COL_MAX_STRING_LEN )
                {
	 		nb_columns = 2;	

			/* Only single digit numbers for choices (up to 8) */
			digit_width = 1;
                }
                else
                {
	 		nb_columns = 3;	

			/* Can have double digit numbers for choices (up to 12) */
			digit_width = 2;
                }
        }

 	for (i = 1; i < choice_list->nbr_choices+1; i++)
        {
		if (choice_list->choices[i-1].string != NULL)
		  {
		    if ( choice_list->noncharge_list_flag != 1 )
                        sprintf(string, "<%*d> %s", digit_width, i, 
				choice_list->choices[i-1].string);
		    else
		      sprintf(string, "<%2d> %s", choice_list->choices[i-1].noncharge_type,
			                          choice_list->choices[i-1].string);
		}	
                else
		  {
		    if ( choice_list->noncharge_list_flag != 1 )
		      sprintf(string, "<%d>", i);
		  }
 

		switch (c = ((i-1) / (SCRN_CW_ROWS - 3)))
		{
		default:
		case 0:
			x = SCRN_CW_COL_1;
			break;
		case 1:
			x=((nb_columns == 2)?SCRN_CW_COL2_2 :  SCRN_CW_COL3_2);			break;
		case 2:
			x = SCRN_CW_COL_3;
			break;
		}

		y = i - ((SCRN_CW_ROWS - 3) * c) + 1;
                mvwaddstr(window,y,x, string);
        }

        return(SUCCESS);
}

RET_STATUS
Scrn_choice_window
(
	SCRN_CHOICE_LIST		*choice_list,
	RET_STATUS			(*next_page)(),
	RET_STATUS			(*prev_page)(),
	HNDL				*choice,
        int                         entry_required
)

{
	RET_STATUS		 rs;
	WINDOW * 		 choice_window;
	char			 string[SCRN_CW_MAX_STRING_LEN + 1],
				 read_buf[3];
	int			 count = 1,
				 i,
				 field_changed,
				 key_entered;
	BOOLEAN			 done = FALSE;
	short			 entered_choice;
	


 	if (SCRN_NOT_INITIALIZED(&Choice_number_screen))
	{
        	choice_window  = newwin
			(
			SCRN_CW_ROWS,
			SCRN_CW_COLUMNS, 
                	SCRN_CW_TOPLINE, 
			SCRN_CW_LEFTCOL
			);
    		Scrn_init_screen(&Choice_number_screen, choice_window);
	}

    	wattrset(Choice_number_screen.window, A_NORMAL);

	if (((rs = (*next_page)(choice_list))!=SUCCESS)||
		(choice_list->nbr_choices == 0))
	{
	     	*choice = NULL;
		touchwin(stdscr);
		Scrn_print_error(SCRN_MSG_NO_CHOICES);
		return(SCRN_NO_CHOICES);
	  
	}

	mvwaddstr(Choice_number_screen.window, 1, 1, SCRN_MSG_ENTER_CHOICE);

	do
	{
 
	/*
 	 *      display window and get user response
 	 */
	        Scrn_choices_to_window(Choice_number_screen.window,
					 choice_list);

    		Scrn_read_field(&Choice_number_screen, read_buf, &field_changed, &key_entered);

		/* Don't let stray characters from before mess up field */
		Scrn_clear_current_field(&Choice_number_screen);

    		Scrn_clear_error();

		switch (key_entered)
		{
		case SEND_KEY:
		case '\n':
		        /* If no choice entered, exit */
		        if (read_buf[0] == EOS)
			{
			  *choice = NULL;
			  done = TRUE;
		  	  rs = SCRN_NO_CHOICES;
			  break;
			}

			entered_choice = atoi(read_buf);
			if ( choice_list->noncharge_list_flag )
			  {
			    for ( i = 0; i< choice_list->nbr_choices; i++ )
			      {
				if ( choice_list->choices[i].noncharge_type == entered_choice )
				  {
				    done = TRUE;
				    rs = SUCCESS;
				    *choice = choice_list->choices[i].data;
				  }
			      }
			    if ( rs != SUCCESS )
			      {
				Scrn_print_error( SCRN_MSG_BAD_CHOICE );
				*choice = NULL;
			      }
			  }
			else if (entered_choice > 0 && 
			    entered_choice <= choice_list->nbr_choices)
		  	{
				done = TRUE;
				rs = SUCCESS;
				*choice = 
				     choice_list->choices[entered_choice-1].data;
	
			}
			else
			{
				
				Scrn_print_error( SCRN_MSG_BAD_CHOICE  );
				*choice = NULL;
			}
			break;
		case NEXT_PG_KEY:
			rs = (*next_page)(choice_list);
			if (rs != SUCCESS)
			  Scrn_print_error( SCRN_MSG_NO_NEXT_PAGE );
			break;
		case PREV_PG_KEY:
			rs = (*prev_page)(choice_list);
			if (rs != SUCCESS)
			  Scrn_print_error( SCRN_MSG_NO_PREV_PAGE );
			break;
		case COMMAND_KEY:
			if (command_screen() == SUCCESS)
			{
				*choice = NULL;
				done = TRUE;
				rs = SCRN_NO_CHOICES;
			}
			break;
		case PRINT_SCR_KEY:
			print_scr(Choice_number_screen.window);
		default:
			break;
		
		}
	} while (!done);	

	werase(Choice_number_screen.window);
	wrefresh(Choice_number_screen.window);
	touchwin(stdscr);
	refresh();
	

	return(rs);
}

RET_STATUS
Scrn_choice_filter_window
(
	SCRN_CHOICE_LIST		*choice_list,
	RET_STATUS			(*next_page)(),
	RET_STATUS			(*prev_page)(),
	HNDL				*choice,
        int                             entry_required
 
)

{
	RET_STATUS		 rs;
	WINDOW * 		 choice_window;
	char			 string[SCRN_CW_MAX_STRING_LEN + 1],
				 read_buf[3];
	int			 count = 1,
				 i,
				 field_changed,
				 key_entered;
	BOOLEAN			 done = FALSE;
	short			 entered_choice;
	INDV_LIST_CONTEXT        *list_info;	
	


 	if (SCRN_NOT_INITIALIZED(&Choice_filter_number_screen))
	{
        	choice_window  = newwin
			(
			SCRN_CW_ROWS,
			SCRN_CW_COLUMNS, 
                	SCRN_CW_TOPLINE, 
			SCRN_CW_LEFTCOL
			);
    		Scrn_init_screen(&Choice_filter_number_screen, choice_window);
	}

    	wattrset(Choice_filter_number_screen.window, A_NORMAL);

	list_info = (INDV_LIST_CONTEXT *)choice_list->data;
	if (((rs = (*next_page)(choice_list))!=SUCCESS)||
		(choice_list->nbr_choices == 0))
	{
	     	*choice = NULL;
		touchwin(stdscr);
		Scrn_print_error(SCRN_MSG_NO_CHOICES);
		return(SCRN_NO_CHOICES);
	  
	}

	mvwaddstr(Choice_filter_number_screen.window, 1, 1, SCRN_MSG_ENTER_CHOICE);

	do
	{
 
	/*
 	 *      display window and get user response
 	 */
	        Scrn_choices_to_window(Choice_filter_number_screen.window,
					 choice_list);

    		Scrn_read_field(&Choice_filter_number_screen, read_buf, &field_changed, &key_entered);

		/* Don't let stray characters from before mess up field */
		Scrn_clear_current_field(&Choice_filter_number_screen);

    		Scrn_clear_error();

		switch (key_entered)
		{
		case SEND_KEY:
		case '\n':
		        /* If no choice entered, exit */
		        if ( (read_buf[0] == EOS) &&
			     ( entry_required == 0 ) )
			{
			  *choice = NULL;
			  done = TRUE;
		  	  rs = SCRN_NO_CHOICES;
			  break;
			}

			entered_choice = atoi(read_buf);
			if (entered_choice > 0 && 
			    entered_choice <= choice_list->nbr_choices &&
			    read_buf[strlen(read_buf) - 1] != '*')
		  	{
				done = TRUE;
				rs = SUCCESS;
				*choice = 
				     choice_list->choices[entered_choice-1].data;
	
			}
			else
			{
			        if ( read_buf[strlen(read_buf)-1] == '*' )
				  read_buf[strlen(read_buf)-1] = '\0';
                                //				strcpy(list_info->filter, read_buf);
				done = TRUE;
				rs = SCRN_FILTER_CHOICES;
			}
			break;
			
		      case ALTERNATE_KEY:
			read_buf[0] = '\0';
                        //			strcpy(list_info->filter, read_buf);
			done = TRUE;
			rs = SCRN_FILTER_CHOICES;
			break;
			
		case NEXT_PG_KEY:
			rs = (*next_page)(choice_list);
			if (rs != SUCCESS)
			  Scrn_print_error( SCRN_MSG_NO_NEXT_PAGE );
			break;
		case PREV_PG_KEY:
			rs = (*prev_page)(choice_list);
			if (rs != SUCCESS)
			  Scrn_print_error( SCRN_MSG_NO_PREV_PAGE );
			break;
		case COMMAND_KEY:
			if (command_screen() == SUCCESS)
			{
				*choice = NULL;
				done = TRUE;
				rs = SCRN_NO_CHOICES;
			}
			break;
		case PRINT_SCR_KEY:
			print_scr(Choice_filter_number_screen.window);
		default:
			break;
		
		}
	} while (!done);	

	werase(Choice_filter_number_screen.window);
	wrefresh(Choice_filter_number_screen.window);
	touchwin(stdscr);
	refresh();
	

	return(rs);
}

