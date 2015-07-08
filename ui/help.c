static char sz__FILE__[]="@(#)help.c	10.1.2.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  help.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:48:50
 * @(#)  Last delta: 12/2/94 at 18:22:24
 * @(#)  SCCS File: /taxi/sccs/s.help.c
 *                                                                           *
 *****************************************************************************/


#include <cursesX.h>
#include <memory.h>
#include <sys/types.h>
#include <ctype.h>

#include "call_entry.h"
#include "screens.h"
#include "mads_types.h"
#include "mads_isam.h"
#include "ui_def.h"
#include "ui_ipc.h"
#include "user.h"
#include "ui_error.h"
#include "df_maint.h"
#include "func_keys.h"
#include "ui_strdefs.h"

extern int scr_name;
extern short clear_err_flag;
extern struct keydesc	hp_key1;
extern char req_buf[];
extern WINDOW *help_win;
struct scr_flds hp_scr_fld[] = {
	{0,0,1,1},
	{0,0,1,1},
};



/* help: processes the HELP_KEY function key */
help(scr_name,fld_nbr)
int scr_name;	/* name of screen or window for which help is reqd */
int fld_nbr;	/* # of field for which help is requested */
{
	struct cisam_hp	*hp_ptr;
	struct	scr_flds  *ptr_scr_fld;
	int key_val;
	int end_help = 0;
	int field_entered = 0;
	char read_buf[READ_BUF_SIZE];
	int  i,m,n;
	int  k = 0,j = 1;
	int  piu = 1;
	int  check_flags = 0;
	int  line_flags = 1;
	int  prev_flags = 0;
	int should_clear = 0;


	ptr_scr_fld = hp_scr_fld;

	werase(help_win);

	/* curses needs touch_win() to really erase window.
	   For some reason it doesn't work without it. */
	touchwin(help_win);

	hp_ptr = (struct cisam_hp *)req_buf;

	if (db(HELP_FILE_ID,OPEN,&hp_key1,ISINOUT+ISMANULOCK,0) < 0)	{
		prt_error(UI_OPEN,"<%s %d>", catgets(UI_m_catd, UI_1, UI_HELP_1, "help file: error ="), iserrno);
		return;
	}

	hp_ptr->nav_string = (short)scr_name;
	hp_ptr->field_nbr = (short)fld_nbr;

	for(i = 1; i <= 4; ++i)	{
		hp_ptr->line_nbr = i;
		if (db(HELP_FILE_ID,READ_KEYS,&hp_key1,ISEQUAL,0) < 0)	{
			if (i == 1)	{
				prt_error(UI_NO_HP_LN," ");
				if (db(HELP_FILE_ID,CLOSE,&hp_key1,0,0) < 0)	
					prt_error(UI_CLOSE,"<%s %d>",catgets(UI_m_catd, UI_1, UI_HELP_1, "help file: error ="), iserrno);
				return;
			}	/* end if i */
			line_flags = 0;
			break;
		}	/* end if .. db */
		if (hp_ptr->like_nav_string != 0)	{
			hp_ptr->nav_string = hp_ptr->like_nav_string;
			hp_ptr->field_nbr = hp_ptr->like_field_nbr;
			--i;
			--k;
		}	/* end if ... hp_ptr */
		mvwprintw(help_win,k,0,"%-130s",hp_ptr->help_line);
		++k;
	}	/* end for .. i */

	wrefresh(help_win);

	while ( !end_help )	{

		key_val = read_field(help_win,ptr_scr_fld,piu,read_buf,&field_entered,&should_clear);

		if (key_val == -1)
			continue;

		if (clear_err_flag)	{
			clear_error();
			refresh();
		}	/* end if */

		switch(key_val)	{

			case	NEXT_PG_KEY:
				if (!line_flags)	{
					prt_error(UI_NO_HP_LN," ");
					break;
				}	/*end if .. */
				if (prev_flags)
					i = i + 4;
				if( i == 1)
					i = 5;
				k = 0;
				if (( i % 4) != 1)
					prt_error(UI_NO_HP_LN," ");
				else	{
					++j;
					if ((i <= (j * 4)) && (check_flags != 0))	
						werase(help_win);
					while (i <= (j * 4))	{
						hp_ptr->line_nbr = i;
						if (db(HELP_FILE_ID,READ_KEYS,&hp_key1,ISEQUAL,0) < 0)	{
							if (check_flags == 0)	{
								prt_error(UI_NO_HP_LN," ");
							}
							break;
						}	/* end if .. db */		
						if (i == 5)
							werase(help_win);
						mvwprintw(help_win,k,0,"%-130s",hp_ptr->help_line);
						++k;
						++i;
						++check_flags;
						prev_flags = 0;
					}	/* end while */
					wrefresh(help_win);
				}	/* end else  */
				break;

			case	PREV_PG_KEY:
				while(1)	{
					if (line_flags == 0)	{
						prt_error(UI_NOPREV," ");
						break;
					}	/* end if */
					if ((i == 1))	{
						prt_error(UI_NOPREV," ");
						break;
					}	/* end if */
					k = 3;
					if ((i % 4) == 1)	{
						if ((i == 5) && (check_flags == 0))	{
							prt_error(UI_NOPREV," ");
							break;
						}
						else
							i = i - 5;
					}	/* end if */
					else	{
						n = i % 4;
						if (n == 0)	
							i = i - 4;
						else
							i = i - n;
					}	/* end else */
					if ( i == 0)
						i = 4;
					m = i - 4;
					if (i > m)	
						werase(help_win);
					while (i > m)	{
						hp_ptr->line_nbr = i;

						if (db(HELP_FILE_ID,READ_KEYS,&hp_key1,ISEQUAL,0) < 0)	{
							break;
						}		
						mvwprintw(help_win,k,0,"%-130s",hp_ptr->help_line);
						--k;
						--i;
					}	/* end while */
					wrefresh(help_win);
					++i;	
					--j;
					prev_flags = 1;
					break;
				} /* while (1) */
				break;		

			case	HELP_KEY:
				end_help = 1;
				werase(help_win);
				wrefresh(help_win);
				break;

			case	COMMAND_KEY:
				end_help = 1;
				werase(help_win);
				wrefresh(help_win);
				command_line();
				break;


			case PRINT_SCR_KEY:	
				print_scr(help_win);
				break;

			default:
				prt_error(UI_INV_KEY," ");
				break;
			
		}	/* end switch */

	} /* while */				

	if (db(HELP_FILE_ID,CLOSE,&hp_key1,0,0) < 0)	{
		prt_error(UI_CLOSE,"<%s %d>",catgets(UI_m_catd, UI_1, UI_HELP_1, "help file: error ="), iserrno);
		return;
	}	/* end if .. db */

	touchwin(stdscr);
	refresh();

}	/* end help()  */
