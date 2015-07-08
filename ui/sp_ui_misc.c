static char sz__FILE__[]= "@(#)sp_ui_misc.c 10.1.2.1(ver) Date Release 94/12/02 18:54:29";
/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  sp_ui_misc.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:49:50
* @(#)  Last delta: 12/2/94 at 18:54:29
* @(#)  SCCS File: /taxi/sccs/s.sp_ui_misc.c
*                                                                           *
*****************************************************************************/

#include <string.h>
#include <cursesX.h>
#include <sys/types.h>
#include "mads_types.h"
#include "df_maint.h"
#include "ex_key.h"
#include "mads_isam.h"
#include "path.h"
#include "except.h"
#include "ui_strdefs.h"

extern char req_buf[];

/**************************************************************************
 * Name: get_cl_nbr(int call_nbr, int exc_info)                           *
 * Type:                                                                  *
 *                                                                        *
 * Purpose:                                                               *
 *      Get call number associated with specific exception info number    *
 *      and first call number of a multi call                             *
 *                                                                        *
 *                                                                        *
 * Returnvalue: On success: Call number of call with exception            *
 *              On failure: -1                                            *
 **************************************************************************/
int get_cl_nbr(int call_nbr, int except_info)
{
  struct cisam_cl *call;
  int i, trip_nbr;


  call = (struct cisam_cl *)req_buf;
  call->nbr=call_nbr;

  switch(except_info) {
    case NO_ACK_MDT_1:
    case NO_ACK_PRT_1:
    case NO_ACK_ITM_1:
         trip_nbr = 0;
         break;
    case NO_ACK_MDT_2:
    case NO_ACK_PRT_2:
    case NO_ACK_ITM_2:
         trip_nbr = 1;
         break;
    case NO_ACK_MDT_3:
    case NO_ACK_PRT_3:
    case NO_ACK_ITM_3:
         trip_nbr = 2;
         break;
  }

  for (i = 0; i < trip_nbr; i++)
  {
    if(db(CALL_FILE_ID,READ_KEYS,&cl_key1,ISEQUAL,0) >= 0)
      call->nbr = call->grouped_with;
    else
    {
    /* ERROR(' '," ","Cannot find record"); */
    return(-1);
    }
  }

  return(call->nbr);
}

/**************************************************************************
 * Name: disp_call_msg(int call_nbr, int use_new_win)                     *
 * Type:                                                                  *
 *                                                                        *
 * Purpose:                                                               *
 *      Display call messages associated with a specific call number.     *
 *      If call messages should be displayed in a window of its own       *
 *      use_new_win should be set != 0                                    *
 *                                                                        *
 *                                                                        *
 * Returnvalue: On success:                                               *
 *              On failure:                                               *
 **************************************************************************/
disp_call_msg(int call_nbr, int use_new_win)
{

  struct cisam_cmsg *cmsg;
  FILE *MIN_HD;
  int i,j,line,start,end;
  int x,y,len;
  char cmsg_str[256],dummy1[101],dummy;
  char DEVICE[3][12];

  char spaces[256];
  WINDOW *cl_msg_win;

  
  if (use_new_win)
  {
        cl_msg_win = newwin(LINES, COLS,0,0);
        werase(cl_msg_win);
        touchwin(cl_msg_win);
  }
  else
  {
        clear();
        cl_msg_win = stdscr;
  }

  wattrset(cl_msg_win, 0);

  memset(spaces,' ',255); /* initialize space string */
  spaces[255] = '\0';

  strcpy(DEVICE[0],m_catgets(UI_m_catd, UI_1, UI_TEXT_49, "MDT"));
  strcpy(DEVICE[1],m_catgets(UI_m_catd, UI_1, UI_TEXT_48, "TAXIMETER"));
  strcpy(DEVICE[2],m_catgets(UI_m_catd, UI_1, UI_TEXT_47, "PRINTER"));

  cmsg=(struct cisam_cmsg *)req_buf;
  cmsg->call_nbr=call_nbr;

  /* Center message on 132 characters screen */
  strcpy(dummy1, m_catgets(UI_m_catd, UI_1, UI_TEXT_53, "Call Messages"));
  len = strlen(dummy1);
  memset(cmsg_str, ' ', (132-len)/2);
  cmsg_str[(132-len)/2] = '\0';
  strcat(cmsg_str,dummy1);
  mvwprintw(cl_msg_win, 1, 2, cmsg_str);

  /* Center string with 3 fields */
  sprintf(cmsg_str,"%-11s",m_catgets(UI_m_catd, UI_1, UI_TEXT_50, "Message Type"));
  strcpy(dummy1,m_catgets(UI_m_catd, UI_1, UI_TEXT_51, "Message"));
  len = strlen(dummy1);
  memset(&cmsg_str[11],' ',(132  - len)/2 - 11);

  cmsg_str[(132 - len)/2] = '\0';
  strcat(cmsg_str,dummy1);
  if((len = strlen(cmsg_str)) > 120)
  {
/*    ERROR(' '," ", "Display string too large, Check catalogs"); */
    len = 120;
  }
  memset(&cmsg_str[len],' ', 120-len);
  cmsg_str[120] = '\0';
  strcat(cmsg_str,m_catgets(UI_m_catd, UI_1, UI_TEXT_52, "ACK"));

  mvwprintw(cl_msg_win, 3, 2, cmsg_str);


  for (i = 0; i < 9; i++) /* for each device check the call handler database for calls to be displayed */
  {

    cmsg->msg_nbr = '1' + i;


    if(db(CALLMSG_FILE_ID,READ_KEYS,&cmsg_key1,ISEQUAL,0) >= 0)
    {
      line = 0;
      start = 0;
      end = 100;
      j = 0;

      sprintf(cmsg_str,"%-11s:  %-100.100s       %c",DEVICE[cmsg->device - '0']," ",cmsg->ack);
      mvwprintw( cl_msg_win,5+i*3, 2, cmsg_str);

      while (strlen( &(cmsg->msg[start]) ) > 100)
      {
        j = 0;

        while ((j < 25) && (cmsg->msg[end - j] != ' '))
          j++;

        if (j == 25)
        {
          strncpy(dummy1,&(cmsg->msg[start]),100);
          dummy1[100] = 0;
          start += 100;
          end = start + 100;
        }
        else
        {
          strncpy(dummy1,&(cmsg->msg[start]), 100 - j);
          dummy1[100-j] = 0;
          start += 101 - j;
          end = start + 100;
        }

        sprintf(cmsg_str,"%-100s", dummy1);
        mvwprintw( cl_msg_win,5+i*3+line, 15, cmsg_str);
        line++;

      } /* end while */

      strcpy(dummy1,&(cmsg->msg[start]));
      sprintf(cmsg_str,"%-100s", dummy1);
      mvwprintw( cl_msg_win,5+i*3+line, 15, cmsg_str);

    } /* end if */
  } /* end for */

  if (use_new_win)
  {
        mvwprintw(cl_msg_win,5 + 10,50,m_catgets(UI_m_catd, UI_1, UI_TEXT_55, "Press <RETURN> to continue"));
        wrefresh(cl_msg_win);
        dummy = getch();
        while (dummy != '\r' && dummy != '\n')
              dummy = getch();
        werase(cl_msg_win);
        wrefresh(cl_msg_win);
        touchwin(stdscr);
        refresh();
  }


}
