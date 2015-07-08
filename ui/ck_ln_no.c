static char sz__FILE__[]="@(#)ck_ln_no.c	10.1.2.1(ver) Date Release 12/2/94" ;
/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  ck_ln_no.c; Rel: 6.6.0.0; Get date: 2/7/92 at 16:35:15
* @(#)  Last delta: 4/8/91 at 10:42:49
* @(#)  SCCS File: /taxi/sccs/s.ck_ln_no.c
*                                                                           *
* Copyright (c) 1990 - MDI Seattle                                          *
*                                                                           *
*****************************************************************************/

#include <cursesX.h>
#include <sys/types.h>
#include <ctype.h>
#include <sys/ipc.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>

#include "taxipak.h"
#include "mem_mgmt.h"
#include "Object.h"
#include "ui_strdefs.h"
#include "mads_types.h"
#include "mads_isam.h"
#include "mad_error.h"
#include "df_maint.h"
#include "call_entry.h"
#define SRCH_EXTERNS
#include "srch.h"
#include "ui_def.h"
#include "func_keys.h"
#include "user.h"
#include "except.h"
#include "veh_driv.h"
#include "writer.h"
#include "ui_ipc.h"
#include "ui_error.h"
#include "screens.h"
#include "language.h"
#include "enhance.h"
#include "switch_ext.h"
#include "GPS_private.h"

#define 	FORWARD 		1
#define 	BACKWARD 		0
#define		SEARCHES_ON_PG 		17
#define		LINE_NUM_ROW		3
#define		LINE_NUM_COL		8
#define		LINE_NUM_MAX		2
#define		CALL_LINE_CHARS		9
#define		ZONE_COL		35
#define		VEHNO_COL		30
#define		CHARS_VEH		4

extern struct users *uiuser[];					/* user profile shared memory structure */
extern int user_index;						/* index into the user structure */
extern int uid;							/* user id number */
extern int pid;							/* ui process id number */
extern int fifo_fd;						/* IPC queue file descriptor */
extern int gps_pipe_fd;
extern int    GPS_guy;
extern int page_num;						/* current page number */
extern short end_man_zone;					/* flag for ending manual zone */
extern short end_man_veh;					/* flag for ending manual vehicle */
extern int (*fnptr_srches)();
extern struct scr_flds srch_flds[];
extern struct keydesc *ptr_srch_key;
extern int zn_or_vh_on[];
extern int matches_index;					/* for matches function */
extern short clear_err_flag;					/* for matches function */
extern struct keydesc zn_key1;
extern struct keydesc veh_key2;
extern struct keydesc cl_key1;
extern int bottom_of_page;
extern int disp_calls_scrn();
extern int scr_name;
extern int New_screen_from_command;
extern struct cisam_sc 	subs_struct;
extern int calls_on_page;
extern int msgkey;
extern struct fleets *fleet[];
extern char sGPS_host[];
extern struct call_ipc_rec call_fifo;		/* for writing call rec's to FIFO */
int    multi_selected = 0;
extern	struct cisam_cl srch_calls[];
extern	struct cisam_sc subs_calls[];	 			/* for reading records from disk */
extern char req_buf[];				/* buffer for r/w records from/to disk */

/* check_line_no: reads the line number entered by the call taker. Displays 
 *			the detail of the line # entered in the detail area
 *			(lines 2-6). This routine also processes the 
 *			<DETAIL>, <NEXT_PG>, <PREV_PG>, etc. function keys,
 *			which are specific to this searches
 */
check_line_no(read_buf,key_ent_val, kb_field_entered,end_srch,calls_on_pg, ptr_struct,pass_line_no)
char *read_buf;					/* buffer in which field entered is stored */
int key_ent_val;				/* value of "function key" entered by call taker */
int kb_field_entered;				/* 0=field not entered; 1=field entered */
int *end_srch;					/* 1=end srch; 0=continue srch */
int *calls_on_pg;				/* # of calls which are on current page display */
struct select_criterias ptr_struct;		/* structure in which searches selection criteria is saved */
int *pass_line_no;				/* used to pass the current line number back to search */
{
	static int detail_call_no = NOT_AVAILABLE;
	struct scr_flds line_num;			/* for piu structure for line # entry */
	struct scr_flds *ptr_scr_fld;

	struct cisam_cl *ptr;				/* pointer to be assigned to req_buf[] */
        struct cisam_cl tmp_call_rec;
	struct cisam_zn *zn_ptr;			/* pointer to be assigned to req_buf[] */
	struct cisam_vh *vh_ptr;			/* pointer to be assigned to req_buf[] */

	int got_calls = 0;
	int zv_fld_entered;				/* whether zone or vehicle # entered */
	int fld_entered;	
	int piu;					/* prompt in use */
        int line_no;
        int ret_ck_fld;
	int key_val;
	int loop, i;
	WINDOW *wind;
        int start_row, c, start_cl_nbr = 0,next_cl_nbr = 0;

	/* Due to stupidity of the original programmers, calls_on_page is a global AND used many places as
	   a local variable. This of course is very confusing. Here I am assigning the global to the local
	   so I can to next page properly. */
	calls_on_page = *calls_on_pg;

	if ( kb_field_entered ){				/* if field was entered by digit */

		if ( ((line_no = atoi(read_buf)) < 1 ) || line_no > *calls_on_pg) { 	/* wrong line # entered */
			prt_error(UI_WRONG_NUM," "); 					/* "Wrong number was entered" 	*/
		        mvaddstr(3,8,"    ");
			attrset(A_REVERSE);
		        mvaddstr(3,8,"    ");
			attrset(0);
			*pass_line_no = detail_call_no;
			return(DONE);
		}

		clear_detail_area();

		if (detail_call_no!=NOT_AVAILABLE) { 				/*unhilite the line matched on summary area */
			if (scr_name == SUBS_FUTURE_CLS_SCR){
				unhilite_line(detail_call_no); 
			}
			else {
				if (!zn_or_vh_on[detail_call_no])
					unhilite_line(detail_call_no); 
				else
					dim_line(detail_call_no); 
			}
		}
		detail_call_no = line_no - 1;
                
                for ( i = 0; i < SEARCHES_ON_PG; i++ )
                  {
                    if ( srch_calls[i].nbr > 0 )
                      unhilite_line(i);
                    else
                      break;
                  }
                
		hilite_line(detail_call_no); 					/*hilite the line on summary area */

                if ( srch_calls[detail_call_no].grouped_with > 0 )
                  {
                    multi_selected = detail_call_no;
                    hilite_multis(detail_call_no);                            
                  }
                
		dsp_cl_detail(line_no); 				/* line # entered is valid; display detail of this call */
				
		*pass_line_no = detail_call_no;
		return(DONE);
	}

	switch (key_ent_val)  { 				/* user entered a function key; check and process type of key */
		case '\r':
		case '\n': 					/* <CR> entered w/o entering line #; do nothing */
			*pass_line_no = detail_call_no;
			return(DONE);

		case KEY_UP:					/* display detail of previous call */
			if (detail_call_no==NOT_AVAILABLE)  { 	/* try to assign a line# first there is no detail on display */
								/* or line is on the top of the page already */
				prt_error(UI_NO_DTL," "); 	/* "No detail line number available" 	*/
				*pass_line_no = detail_call_no;
				return(DONE);
			}

			if (scr_name == SUBS_FUTURE_CLS_SCR){
				unhilite_line(detail_call_no); 
			}
			else {
				if (zn_or_vh_on[detail_call_no]) 	/*unhilite the last hilited line */
					dim_line(detail_call_no); 
                                else
                                  {
                                    for ( i = 0; i < SEARCHES_ON_PG; i++ )
                                      {
                                        if ( srch_calls[i].nbr > 0 )
                                          unhilite_line(i);
                                        else
                                          break;
                                      }
                                  }                                  
			}

			if (detail_call_no > 0)  
				detail_call_no--;
			else
				detail_call_no = (*calls_on_pg) - 1;

			hilite_line(detail_call_no); 			/*hilite the line matched on summary area */
                        if ( srch_calls[detail_call_no].grouped_with > 0 )
                          {
                            multi_selected = detail_call_no;
                            hilite_multis(detail_call_no);
                          }
			clear_detail_area();
			dsp_cl_detail(detail_call_no+1); 
			*pass_line_no = detail_call_no;
			return(DONE);

		case KEY_DOWN: 						/* display detail of next call */
			if (detail_call_no==NOT_AVAILABLE)  { 		/* try to assign a line# first there is no detail on
									/* display or line is on the top of the page already */
				prt_error(UI_NO_DTL," "); 		/* "No detail line number available" 	*/
				*pass_line_no = detail_call_no;
				return(DONE);
			}

			if (scr_name == SUBS_FUTURE_CLS_SCR){
				unhilite_line(detail_call_no); 
			}
			else {
				if (zn_or_vh_on[detail_call_no]) 		/* should this line be dimmed ? */
					dim_line(detail_call_no); 
				else
                                  {
                                    for ( i = 0; i < SEARCHES_ON_PG; i++ )
                                      {
                                        if ( srch_calls[i].nbr > 0 )
                                          unhilite_line(i);
                                        else
                                          break;
                                      }
                                    //unhilite_line(detail_call_no);
                                  }
			}
			
			if (detail_call_no >= (*calls_on_pg-1))  
				detail_call_no = 0;
			else
				detail_call_no++;

                        if ( srch_calls[detail_call_no].grouped_with > 0 )
                          {
                            multi_selected = detail_call_no;
                            hilite_multis(detail_call_no);                            
                          }
                        
			hilite_line(detail_call_no); 			/*hilite the line matched on summary area */
	
			clear_detail_area();
			dsp_cl_detail(detail_call_no+1); 
			*pass_line_no = detail_call_no;
			return(DONE);

		case NEXT_PG_KEY: 	 					/* Next Page request */
			got_calls = 0;
			if ((*fnptr_srches)(YES, &got_calls, FORWARD, NO)) {  	/* display summary of the next 17 calls */
				if (got_calls)
					*calls_on_pg = got_calls;
				detail_call_no = NOT_AVAILABLE; 
				clear_detail_area();

				erase_area(6,17); 				/* erase_sum_area */
				disp_call_sum(*calls_on_pg);			/* from srch_calls[] */

				if (scr_name != SUBS_FUTURE_CLS_SCR){
					for (loop=0; loop<SEARCHES_ON_PG; loop++)
						zn_or_vh_on[loop] = 0;
				}
			}
			else {
				prt_error(UI_NO_RECORD,catgets(UI_m_catd, UI_1, UI_CK_LN_NO_1, "<in the direction you are going>")); 	/* No more records found */
			}

			*pass_line_no = detail_call_no;
			return(DONE);

		case PREV_PG_KEY:  					/* Previous Page request */
			got_calls = 0;
			if (page_num > 1 && (*fnptr_srches)(YES, &got_calls, BACKWARD, NO) )  {  
				if (got_calls)
					*calls_on_pg = got_calls;
				detail_call_no = NOT_AVAILABLE; 
				clear_detail_area();

				erase_area(6,17); 			/* erase sum area */
				disp_call_sum(*calls_on_pg);		/* from srch_calls[] */

				if (scr_name != SUBS_FUTURE_CLS_SCR){
					for (loop=0; loop<SEARCHES_ON_PG; loop++)
						zn_or_vh_on[loop] = 0;
				}
			}
			else {
			        /* No more records found */
			        prt_error(UI_NO_RECORD,catgets(UI_m_catd, UI_1, UI_CK_LN_NO_1, "<in the direction you are going>"));
				/* If no previous records, then must be on page 1 (occasionally the page number gets
				   out of alignment, so this needs to be done */
				page_num = 1;
			}
			*pass_line_no = detail_call_no;
			return(DONE);

			case MANZONE_KEY:
				if (scr_name == SUBS_FUTURE_CLS_SCR){
					prt_error(UI_INV_KEY," ");
					return(DONE);
				}
				else {
					if (detail_call_no == NOT_AVAILABLE)  {	/* check if any detail is available on display */
						prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_CK_LN_NO_2, "<no call highlighted>"));
						return(DONE);
					}
					if (uiuser[user_index]->can_zone_calls != YES) {		/* can user zone calls */
						prt_error(UI_NOT_ALLOWED,catgets(UI_m_catd, UI_1, UI_CK_LN_NO_3, "<to zone calls>"));
						return(DONE);
					}
					srch_zone_veh(&detail_call_no,read_buf,calls_on_pg,C_ZONE);
					return(DONE);
				}
			

			case MANVEH_KEY:
				if (scr_name == SUBS_FUTURE_CLS_SCR){
					prt_error(UI_INV_KEY," ");
					return(DONE);
				}
				else {
					if (detail_call_no == NOT_AVAILABLE)  { /* check if any detail is available on display */
						prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_CK_LN_NO_2, "<no call highlighted>"));
						return(DONE);
					}
					if (uiuser[user_index]->menu_perms.exceptions !=1){		/* is user a dispatcher ?*/
						prt_error(UI_NOT_ALLOWED,catgets(UI_m_catd, UI_1, UI_CK_LN_NO_4, "<to manually assign vehicles>"));
						return(DONE);
					}
					srch_zone_veh(&detail_call_no,read_buf,calls_on_pg,C_VEHICLE);
					return(DONE);
				}

			case HISTORY_KEY:
				if (scr_name == SUBS_FUTURE_CLS_SCR)
				  {
				    subcall_history(subs_calls[detail_call_no].nbr);
				    *pass_line_no = detail_call_no;
				    return(DONE);
				  }
				else {
					if ( detail_call_no < 0 || detail_call_no > *calls_on_pg) {
						prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_CK_LN_NO_5, "<invalid call # highlighted>"));
						*pass_line_no = detail_call_no;
						return(DONE);
					}
					call_history(srch_calls[detail_call_no].nbr);
					*pass_line_no = detail_call_no;
					return(DONE);
				}

			case REFRESH_KEY:	/* REFRESH */
				got_calls = 0;
				bottom_of_page = 1;
				if ( (*fnptr_srches)(YES, &got_calls, FORWARD, YES) )  {  
					if (got_calls)
						*calls_on_pg = got_calls;
					detail_call_no = NOT_AVAILABLE; 
					clear_detail_area();

					/* erase_sum_area */
					erase_area(6,17);
					disp_call_sum(*calls_on_pg);	/* from srch_calls[] */

					if (scr_name != SUBS_FUTURE_CLS_SCR){
						for (loop=0; loop<SEARCHES_ON_PG; loop++)
							zn_or_vh_on[loop] = 0;
					}
				}
				else  
					prt_error(UI_NO_RECORD," "); /* "No record found in search" 	*/

				*pass_line_no = detail_call_no;
				return(DONE);

			case DETAIL_KEY:
			if (detail_call_no==NOT_AVAILABLE)
			  {
			    prt_error(UI_NO_DTL," ");
			    *pass_line_no = detail_call_no;
			    return(DONE);
			  }
			
			/* Copy subs calls to search calls */
			if (scr_name == SUBS_FUTURE_CLS_SCR)
			  {
			    subs_to_call(&subs_calls[detail_call_no], &srch_calls[detail_call_no]);
			    subs_struct = subs_calls[detail_call_no];
			  }

			New_screen_from_command = 0;
			
			detail_key(&detail_call_no);
			
			/* Redisplay screen if COMMAND_KEY didn't change it */
			if (!New_screen_from_command) {
			  disp_calls_scrn();
			  disp_call_sum(*calls_on_pg);
			  if (!zn_or_vh_on[detail_call_no])
			    hilite_line(detail_call_no); 
			  else
			    dim_line(detail_call_no); 
			  dsp_cl_detail(detail_call_no+1);
			}
			return(DONE);


			case PREV_SCR_KEY:
				detail_call_no = NOT_AVAILABLE;
				return(NOT_DONE);

        case MULTIPLE_KEY:
          if ( detail_call_no == NOT_AVAILABLE )
            return(NOT_DONE);
          else if ( scr_name != UNASSIGNED_CALLS_SCR )
            {
              prt_error(UI_NO_MSG,"NOT PERMITTED ON THIS SCREEN. USE SCR21");
              return(NOT_DONE);
            }
          else if ( srch_calls[detail_call_no].extended_type[4] != MULT_CALL )
            {
              prt_error(UI_NO_MSG,"NOT A MULTI-TRIP");
              return(DONE);
            }
          else if ( srch_calls[detail_call_no].grouped_with != 0 )
           {
             // Get confirmation from user
             if ( detail_call_no > 8 )
               start_row = 7;
             else
               start_row = 16;
             wind = newwin(3,82,start_row,22);
             if (wind == NULL)
               return(DONE);
             tpak_box(wind, 0, 0);
             mvwaddstr(wind, 1, 1, "Are you sure you want to UNLINK these Multi-trips (J/N)? ");
             while (1)
               {
                 c = wgetkey(wind);
                 c = Toupper(c);
                 if ( c == NO )
                   {
                     wclear(wind);
                     delwin(wind);
                     touchwin(stdscr);
                     refresh();
                     return(DONE);
                   }
                 if ( c == YES )
                   {
                     wclear(wind);
                     delwin(wind);
                     touchwin(stdscr);
                     refresh();
                     break;
                   }
                 beep();
               }
             
             // need to unlink trip by clearing the 'grouped_with' field

             memcpy( &tmp_call_rec, &srch_calls[detail_call_no], sizeof( struct cisam_cl ) );
             start_cl_nbr = tmp_call_rec.nbr;
             next_cl_nbr = tmp_call_rec.grouped_with;
             while ( next_cl_nbr != start_cl_nbr )
               {
                 if ( db_read_key( CALL_FILE_ID, &tmp_call_rec, &cl_key1, DB_EQUAL ) == SUCCESS )
                   {
                     next_cl_nbr = tmp_call_rec.grouped_with;
                     tmp_call_rec.grouped_with = 0;                 
                     db_update( CALL_FILE_ID, &tmp_call_rec );
                     // generate a MULT exception...                     
                     fill_mult_call_rec(&tmp_call_rec);
                     call_fifo.grouped_with = 0;
                     write(fifo_fd, &call_fifo, sizeof(struct call_ipc_rec));                     
                   }
                 tmp_call_rec.nbr = next_cl_nbr;
               }
                 


              prt_error(UI_NO_MSG, catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_53, "MULT-exception created"));             
             return(DONE);
          }
          else
            {
              // generate a MULT exception...
              fill_mult_call_rec(&srch_calls[detail_call_no]);
              write(fifo_fd, &call_fifo, sizeof(struct call_ipc_rec));
              prt_error(UI_NO_MSG, catgets(UI_m_catd, UI_1, UI_UI_EXCEPTS_53, "MULT-exception created"));
              return(DONE);
            }
          break;
		      case SEARCH_KEY:
			if (detail_call_no==NOT_AVAILABLE)
			  {
			    detail_call_no = NOT_AVAILABLE;
			    return(NOT_DONE);
			  }
			else
			  {
			    if ( ( GPS_guy ) &&
				 ( srch_calls[detail_call_no].gps_long != 0.0 ) &&
				 ( srch_calls[detail_call_no].gps_lat != 0.0 ) )
			      {
				if ( GPS_client_display( &srch_calls[detail_call_no] ) )
				  prt_error(UI_NO_MSG, "TAXIGEO VISUALIZATION IN PROGRESS");
				return(DONE);
			      }
			    else if ( ( GPS_guy ) )
			      {
				prt_error(UI_NO_MSG,"GPS COORDINATES NOT AVAILABLE FOR SELECTED ADDRESS");
				return(DONE);
			      }
			  }
			break;

		      case 6:
			if ( GPS_guy )
			  GPS_zoom( MAP_APP_RQST_TYPE_ZOOM_IN );
			break;

		      case 7:
			if ( GPS_guy )
			  GPS_zoom( MAP_APP_RQST_TYPE_ZOOM_OUT );
			break;
			    
			case COMMAND_KEY:
				if (detail_call_no==NOT_AVAILABLE)  { 	
					detail_call_no = NOT_AVAILABLE;
					return(NOT_DONE);
				}

				if (scr_name == SUBS_FUTURE_CLS_SCR){
					unhilite_line(detail_call_no); 
				}
				else {
					if (zn_or_vh_on[detail_call_no]) 		/* should this line be dimmed ? */
						dim_line(detail_call_no); 
					else
						unhilite_line(detail_call_no); 
				}
				detail_call_no = NOT_AVAILABLE;
				return(NOT_DONE);
	}  /* end switch */
	return(NOT_DONE);
}  /* end check_line_no */

/*********************************************************************************************/
/* srch_zone_veh: processes the manual zone/vehicle assignment mode on all search screens    */
/*		Returns  	DONE		if processing is complete                    */
/*				NOT_DONE	if processing is not completed 		     */
/*********************************************************************************************/
int srch_zone_veh(detail_call_no,read_buf,calls_on_page,z_or_v)
	int *detail_call_no;					/* call line # highlighted */
	char *read_buf;						/* buffer for input */
	int *calls_on_page;					/* # of calls on the page */
	char z_or_v;						/* whether manual zone or manual_veh assign */
{
	static struct scr_flds man_zone_fld[] = { {0,0,1,1},
			{LINE_NUM_ROW,LINE_NUM_COL,CALL_LINE_CHARS,2},
			{6,ZONE_COL,CHARS_ZONE,1},
	};

	static struct scr_flds man_veh_fld[] = { {0,0,1,1},
			{LINE_NUM_ROW,LINE_NUM_COL,CALL_LINE_CHARS,2},
			{6,VEHNO_COL,CHARS_VEH,1},
	};

	struct scr_flds *ptr_scr_fld;
	struct scr_flds *ptr_to_fld;
	int piu;
	int field_entered;
	extern int end_unassigned_srch;
	short error_flag = 0;
	int key_val;
	int ret_ck_fld;
	short *end_man_zv;				/* pointer to end_man_zone or end_man_veh field */
	int should_clear = 0;

	attrset(A_REVERSE); 				/* on the first line show that we are on manual zone assignment mode */
	if ( z_or_v == C_ZONE ) {
		mvaddstr(0,23,catgets(UI_m_catd, UI_1, UI_CK_LN_NO_6, "MODE=MAN_ZONE")); 	/* its manual zone assignment */
		ptr_scr_fld = man_zone_fld;
	}
	else  {
		mvaddstr(0,23,catgets(UI_m_catd, UI_1, UI_CK_LN_NO_7, "MODE=MAN_VEH")); 		/* its manual veh assignment */
		ptr_scr_fld = man_veh_fld;
	}

	attrset(0);

	piu = 2;					/* for zoning calls */
	ptr_to_fld = ptr_scr_fld + piu;
	ptr_to_fld->prompt_row = 6 + *detail_call_no;	/* initialize the row of entry */

	 end_man_zone = 0;	  			/* flag for ending manual_zone mode */
	 end_man_veh = 0;				/* flag for ending manual vehicle mode */

	 if ( z_or_v == C_ZONE )
		 end_man_zv =  &end_man_zone;
	else
		end_man_zv = &end_man_veh;

	while (!*end_man_zv && !end_unassigned_srch) {
		check_mail();
		key_val = read_field(stdscr,ptr_scr_fld,piu,read_buf,&field_entered,&should_clear);

		if (key_val == -1) 
			continue;							/* read_field() timed out */
		
		if (clear_err_flag)
			clear_error();							/* clear error msg from last line */
											/* check the field entered by the user */
		ret_ck_fld = ck_srch_zone_veh(ptr_scr_fld,&piu,read_buf,key_val,&field_entered,detail_call_no,calls_on_page,z_or_v);

		if (ret_ck_fld == NOT_DONE) {
			*end_man_zv = 1; 			/* function key entered; should end manual zone assignment mode  */
			ftnkey_handler(&piu,key_val,0);
		}
	}

	attrset(A_REVERSE);			/* on the first line show that we are out of manual zone/vehcile assignment mode */
	mvaddstr(0,23,"             ");
	attrset(0);
	return(DONE);
}
/*****************************************************************************************************/
/* ck_except_zone_veh: checks if the zone # entered is valid for manual zone assignment              */
/*	or vehicle # entered is valid for manual vehicle assignment mode on  the exceptions screen   */
/*****************************************************************************************************/
int ck_srch_zone_veh(ptr_scr_fld,piu_ptr,read_buf,key_val,field_entered,detail_call_no,calls_on_page,z_or_v)
struct scr_flds *ptr_scr_fld;
int *piu_ptr;
char *read_buf;
int key_val;
int *field_entered;
int *detail_call_no;			/* exception line # highlighted */
int *calls_on_page;
char z_or_v;				/* C_ZONE/C_VEHICLE for manual zone or veh assignmnet */
{

	short i;
	short line_no;
	struct scr_flds *ptr_to_fld;
	short error_flag=0;
	short zone_veh_num;
	struct cisam_zn *zn_ptr;
	int got_calls=0;
	int temp_int;
	int times_thru, rc;
	char zv_desc[7];			/* for error msgs etc */
	struct cisam_vh *vh_ptr;
	struct veh_rqst  vrq;
	struct veh_drv_info_resp vrp;
	struct veh_driv *veh_ptr;
	short fleet_nbr_index;
	int   driver_id;

	ptr_to_fld = ptr_scr_fld + (*piu_ptr);

	if (*field_entered && *piu_ptr == 2) {					/* user entered the zone #, verify it */
		i = 0;
		while (i < ptr_to_fld->max_chars  && read_buf[i] != '\0') {	
			if (!isdigit(read_buf[i])) {
				prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_CK_LN_NO_8, "<digits only>"));
				error_flag = 1;
				break;						/* leave while loop */
			}
			i++;
		}

		if (z_or_v == C_ZONE)  						/* check if this call is unzoned or not */
			strcpy(zv_desc, catgets(UI_m_catd, UI_1, UI_CK_LN_NO_9, "zone"));
		else
			strcpy(zv_desc, catgets(UI_m_catd, UI_1, UI_CK_LN_NO_10, "veh"));

		if (!error_flag) {
			if ((zone_veh_num = atoi(read_buf)) <= 0) {		/* veh # of 0 is allowed to unassign a call */
				if (zone_veh_num == 0  &&  z_or_v == C_VEHICLE) 
					;					/* do nothing */
				else {						/* invalid zone or vehicle # entered */
					prt_error(UI_NOFIELD,"<%s %s %s>", catgets(UI_m_catd, UI_1, UI_CK_LN_NO_11, "invalid"), zv_desc, catgets(UI_m_catd, UI_1, UI_CK_LN_NO_12, "# entered") );
					error_flag = 1;
				}
			}
		}
		
		if (!error_flag) {
			if (z_or_v == C_ZONE && zone_veh_num != srch_calls[*detail_call_no].pckup_zone) {	/* its man zone assignment*/
				zn_ptr = (struct cisam_zn *)req_buf;
				zn_ptr->fleet = srch_calls[(*detail_call_no)].fleet;
				zn_ptr->nbr = zone_veh_num;
				if (db(ZONE_FILE_ID,READ_KEYS,&zn_key1,ISEQUAL,0) < 0) {
					prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_CK_LN_NO_13, "<zone #>")); 		/* invalid zone entered */
					error_flag = 1;
				}
				else 	if (zn_ptr->type == '3') {					/* superzone number ? */
						prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_CK_LN_NO_14, "<superzones are not allowed>"));	/* invalid zone entered */
						error_flag = 1;
					}
				else 	if (zn_ptr->type == '4') {					/* special backup zone number ? */
						prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_CK_LN_NO_24, "<special backup zones are not allowed>"));	/* invalid zone entered */
						error_flag = 1;
					}
				else {
					srch_calls[*detail_call_no].pckup_zone = zone_veh_num;
					strncpy(srch_calls[*detail_call_no].zone_desc,zn_ptr->desc,20);
					srch_upd_files(detail_call_no,zone_veh_num,C_ZONE);
					dim_line(*detail_call_no);
					zn_or_vh_on[(*detail_call_no)] = 1;
				}
			}
			/* It's man veh assignment*/
			else if (z_or_v == C_VEHICLE && zone_veh_num != srch_calls[*detail_call_no].veh_nbr)
			  {
			    if (srch_calls[*detail_call_no].pri_status == 63 &&
				strcmp(srch_calls[*detail_call_no].status, PENDING) == 0)
			      {
				prt_error(UI_NO_MSG, catgets(UI_m_catd, UI_1, UI_TEXT_7,
							     "Can't assign time calls. Change to NOW first."));
				error_flag = 1;
			      }
			    else
			      {
				if ( (man_veh_idle( zone_veh_num, srch_calls[*(detail_call_no)].fleet, &driver_id ) ) != SUCCESS )
				  {
				    prt_error(UI_VEH_STATUS, " ");
				    mvaddstr( ptr_to_fld->prompt_row, ptr_to_fld->prompt_col, "     " );
				    return(DONE);
				  }
				else
				  {
				    srch_calls[*detail_call_no].veh_nbr = zone_veh_num;
				    srch_calls[*detail_call_no].drv_id = driver_id;
				    
				    if(srch_calls[*detail_call_no].grouped_with)
				      man_veh_mult(detail_call_no, zone_veh_num);
				    else
				      srch_upd_files(detail_call_no,zone_veh_num,C_VEHICLE);
				    
				    dim_line(*detail_call_no);
				    zn_or_vh_on[(*detail_call_no)] = 1;
				  }
				
			      }
			  }
		      }
	      }
	else { 	if(*field_entered && *piu_ptr == 1) {					/* user entered a line number */
			i = 0;
			while (i < ptr_to_fld->max_chars  && read_buf[i] != '\0') {	
				if (!isdigit(read_buf[i])) {
					prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_CK_LN_NO_8, "<digits only>"));
					error_flag = 1;
					return(DONE);
				}
				i++;
			}

			if(((line_no = atoi(read_buf)) < 1) || (line_no > *calls_on_page)) {
				prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_CK_LN_NO_16, "<invalid line # entered>"));
				error_flag = 1;
				return(DONE);
			}

			if((line_no-1) != *detail_call_no){		/* different line number to display */
				if(*detail_call_no < *calls_on_page){
					if(zn_or_vh_on[(*detail_call_no)])
						dim_line(*detail_call_no);
					else
						unhilite_line(*detail_call_no);
				}
			}

			clear_detail_area();
			*detail_call_no = line_no - 1;
			hilite_line(*detail_call_no); 			/*hilite the line matched on summary area */
			dsp_cl_detail(*detail_call_no+1);

			*piu_ptr = 2;
			ptr_to_fld = ptr_scr_fld + (*piu_ptr);
			ptr_to_fld->prompt_row = 6 + *detail_call_no;
			return(DONE);
			
		}
	}

	switch ( key_val) {
		case OK:				/* field was entered */
		case '\n':
		case '\r':
		case '\t':
			*piu_ptr = 1;			/* go to "Line" prompt */
			return(DONE);

		case KEY_DOWN: 				/* move the highlight down by one count */
			if(*detail_call_no == NOT_AVAILABLE){
				prt_error(UI_NO_DTL," ");
				error_flag = 1;
				return(DONE);
			}
			*piu_ptr = 2;
			if(zn_or_vh_on[(*detail_call_no)])
				dim_line(*detail_call_no);
			else
				unhilite_line(*detail_call_no);

			if ((*detail_call_no) >= (*calls_on_page-1))
				*detail_call_no = 0;
			else 
				(*detail_call_no)++;

			clear_detail_area();
			hilite_line(*detail_call_no); 			/*hilite the line matched on summary area */
			dsp_cl_detail(*detail_call_no+1);

			ptr_to_fld = ptr_scr_fld + (*piu_ptr);
			ptr_to_fld->prompt_row = 6 + *detail_call_no;
			return(DONE);

		case KEY_UP:
			if(*detail_call_no == NOT_AVAILABLE){
				prt_error(UI_NO_DTL," ");
				error_flag = 1;
				return(DONE);
			}
			*piu_ptr = 2;
			if(zn_or_vh_on[(*detail_call_no)])
				dim_line(*detail_call_no);
			else
				unhilite_line(*detail_call_no);

			if ((*detail_call_no) <= 0)
				*detail_call_no = *calls_on_page - 1;
			else 
				(*detail_call_no)--;

			clear_detail_area();
			hilite_line(*detail_call_no); 			/*hilite the line matched on summary area */
			dsp_cl_detail(*detail_call_no+1);

			ptr_to_fld = ptr_scr_fld + (*piu_ptr);
			ptr_to_fld->prompt_row = 6 + *detail_call_no;
			return(DONE);
			
		case MANZONE_KEY:
			end_man_zone = 1; 				/* get out of manual zone mode */
			return(DONE);

		case MANVEH_KEY:
			end_man_veh = 1; 				/* get out of manual veh mode */
			return(DONE);

		case NEXT_PG_KEY:
			*piu_ptr = 1;					/* move to "Line:" prompt */
			if ((*fnptr_srches)(YES,&got_calls,FORWARD,NO)){
				page_num++;				/* got some calls */

				if(got_calls)
					(*calls_on_page) = got_calls;

				move(6,0);
				clrtobot();				/* clear summary area */
				clear_detail_area();
				for(i=0; i<SEARCHES_ON_PG; i++)		/* clear dimmed flags */
					zn_or_vh_on[i] = 0;
				*detail_call_no = NOT_AVAILABLE;
				disp_call_sum(*calls_on_page);		/* display summary area */
			}
			else {
				prt_error(UI_NO_RECORD,catgets(UI_m_catd, UI_1, UI_CK_LN_NO_1, "<in the direction you are going>"));
				return(DONE);
			}
			ptr_to_fld = ptr_scr_fld + (*piu_ptr);
			return(DONE);

		case PREV_PG_KEY:
			*piu_ptr = 1;					/* move to "Line:" prompt */
			if (page_num > 1 && (*fnptr_srches)(YES,&got_calls,BACKWARD,NO)){
				page_num--;				/* got some calls */

				if(got_calls)
					(*calls_on_page) = got_calls;

				move(6,0);
				clrtobot();				/* clear summary area */
				clear_detail_area();
				for(i=0; i<SEARCHES_ON_PG; i++)		/* clear dimmed flags */
					zn_or_vh_on[i] = 0;
				*detail_call_no = NOT_AVAILABLE;
				disp_call_sum(*calls_on_page);		/* display summary area */
			}
			else {
				prt_error(UI_NO_RECORD,catgets(UI_m_catd, UI_1, UI_CK_LN_NO_1, "<in the direction you are going>"));
				return(DONE);
			}
			ptr_to_fld = ptr_scr_fld + (*piu_ptr);
			return(DONE);

		case REFRESH_KEY:
			*piu_ptr = 1;					/* move to "Line" prompt */

			got_calls = 0;
			if ((*fnptr_srches)(YES,&got_calls,FORWARD,YES)){
				page_num = 1;
				if(got_calls)
					(*calls_on_page) = got_calls;
				for(i=0; i<SEARCHES_ON_PG; i++)			/* clear dimmed flags */
					zn_or_vh_on[i] = 0;
				move(6,0);	 				/* clear the summary area */
				clrtobot();
				clear_detail_area(); 				/* CLEAR THE DETAIL AREA */
				*detail_call_no = NOT_AVAILABLE; 		/* since new start */
				disp_call_sum(got_calls);
			}
			else {
				end_man_zone = 1;  			/* go back to searches */
				end_man_veh = 1;  			/* go back to searches */
				prt_error(UI_NO_RECORD," ");
				error_flag = 1;
			}
			return(DONE);


		case HISTORY_KEY:
			if ( *detail_call_no < 0 || *detail_call_no > *calls_on_page) {
				prt_error(UI_NOFIELD,catgets(UI_m_catd, UI_1, UI_CK_LN_NO_17, "<invalid call # highlighted>"));
				return(DONE);
			}
			call_history(srch_calls[(*detail_call_no)].nbr);
			return(DONE);

		default:						/* function key entered */
			prt_error(UI_INV_KEY,catgets(UI_m_catd, UI_1, UI_CK_LN_NO_18, "<you must get out of MAN_VEH or MAN_ZONE mode first>"));
			return(DONE);

	}

}

hilite_area(line_no, num_line)
int line_no, num_line;
{
	int loop;

	attrset(A_REVERSE | A_BOLD);
	for (loop=0; loop<num_line; loop++)
	{
		mvaddstr(loop+line_no,0,"                                                                                                                                    \0");
	}
	attrset(0);
}


erase_area(line_no, num_line)
int line_no, num_line;
{
	int loop;

	for (loop=0; loop<num_line; loop++)
	{
		move(line_no+loop, 0);
		clrtoeol();
	}
}


erase_sum_area()
{
}


check_srch_vehicle()
{
return 1;
}

check_srch_zone()
{
}
/*****************************************************************************************/
/* srch_upd_files - will update the call file and create call history records for the    */
/*                  manual zone assignment or manual vehicle assignment                  */
/*****************************************************************************************/
srch_upd_files(detail_call_no,zone_veh_num,action_type)
int *detail_call_no;				/* current line being manually assigned */
int zone_veh_num;				/* manually assigned zone or vehicle number */
char action_type;				/* type of action C_ZONE/C_VEHICLE */
{
	struct excpt_rec except_fifo;				/* for writing except resolution record to FIFO */
	struct cisam_cl *temp_cl_ptr;
	char date_str[10];					/* for formatting date */
	char time_str[10];					/* for formatting time */
	int old_zone_nbr;					/* old zone number to be sent to dispatch for updating appropriate*/
								/*F35 intervals it call is MAN ZONED*/

	if (action_type == C_VEHICLE)
	  {
	    if (zone_veh_num > 0)
	      {
		strcpy(srch_calls[*detail_call_no].status,ASSIGNED);
		srch_calls[*detail_call_no].dispd_date_time = (int)time(NULL);
		get_pkup_date(date_str);
		get_pkup_time(time_str);
		strcpy(srch_calls[*detail_call_no].dispd_date,date_str);
		strcpy(srch_calls[*detail_call_no].dispd_time,time_str);
		srch_calls[*detail_call_no].dispatched_by = (short)uid;
		srch_calls[*detail_call_no].pri_status = 64;			/* assigned call */
		srch_calls[*detail_call_no].pckup_date_time = NULL;
		srch_calls[*detail_call_no].pckup_time[0] = NULL;
		srch_calls[*detail_call_no].pckup_date[0] = NULL;
		srch_calls[*detail_call_no].close_date_time = NULL;
		srch_calls[*detail_call_no].close_time[0] = NULL;
		srch_calls[*detail_call_no].close_date[0] = NULL;
	      }
	    else {
	      strcpy(srch_calls[*detail_call_no].status,UNASSGND);	/* unassignment */
	      srch_calls[*detail_call_no].pri_status = srch_calls[*detail_call_no].priority;
	      srch_calls[*detail_call_no].dispd_date_time = NULL;
	      srch_calls[*detail_call_no].dispd_date[0] = NULL;
	      srch_calls[*detail_call_no].dispd_time[0] = NULL;
	      srch_calls[*detail_call_no].dispatched_by = 0;
	      srch_calls[*detail_call_no].pckup_date_time = NULL;
	      srch_calls[*detail_call_no].pckup_time[0] = NULL;
	      srch_calls[*detail_call_no].pckup_date[0] = NULL;
	      srch_calls[*detail_call_no].close_date_time = NULL;
	      srch_calls[*detail_call_no].close_time[0] = NULL;
	      srch_calls[*detail_call_no].close_date[0] = NULL;
	      srch_calls[*detail_call_no].veh_nbr = 0;
	      srch_calls[*detail_call_no].drv_id = 0;
	    }
	  }

	temp_cl_ptr = (struct cisam_cl *)req_buf; 				/* try to read rec by call # */
	temp_cl_ptr->nbr = srch_calls[*detail_call_no].nbr;
	if (db(CALL_FILE_ID,READ_KEYS,&cl_key1,ISEQUAL+ISLOCK,0) < 0){ 		/* db failed, no record found */
		prt_error(UI_DSKREAD,catgets(UI_m_catd, UI_1, UI_CK_LN_NO_19, "<calls file; can't LOCK call>"));
		return(NOT_DONE);
	}

	old_zone_nbr = temp_cl_ptr->pckup_zone;

	memcpy(req_buf,(char *)&srch_calls[*detail_call_no],sizeof(struct cisam_cl)); 	/* make these changes on disk record */


	if (db(CALL_FILE_ID,UPDATE_REC,&cl_key1,0,0) < 0) {
		prt_error(UI_WRITE,catgets(UI_m_catd, UI_1, UI_CK_LN_NO_20, "<error updating calls record... FATAL ERROR>"));
		return(NOT_DONE);
	}

	if ( db(CALL_FILE_ID,RELEASE,&cl_key1,0,0) < 0) {
		prt_error(UI_WRITE,catgets(UI_m_catd, UI_1, UI_CK_LN_NO_21, "<error releasing lock on calls record>"));
	}

	if ( ( 0 ) && ( CheckTimeCallCriteria( temp_cl_ptr ) ) )
	  {
	    AddTimeCall( temp_cl_ptr );
	  }
	
	if(action_type == C_ZONE)
		add_noexcpt_callh(*detail_call_no,R_MAN_ZONE,ACT_MANZONE,zone_veh_num);
	else
		add_noexcpt_callh(*detail_call_no,R_MAN_VEH,ACT_MANVEH,zone_veh_num);
		
	except_fifo.u_prcs_id = pid; 					/* write to dispatch */
	except_fifo.rec_type = EXCEPT_REQ;
	except_fifo.except_ptr = 0;
	except_fifo.except_nbr = 0;
	except_fifo.call_nbr = srch_calls[*detail_call_no].nbr;
	except_fifo.grouped_with = 0;

	if(action_type == C_VEHICLE){
		except_fifo.veh_nbr = zone_veh_num;
		except_fifo.zone_nbr = 0;
		except_fifo.resolve_code = (short)R_MAN_VEH;
	}
	else {
		except_fifo.zone_nbr = temp_cl_ptr->pckup_zone;
		except_fifo.old_zone_nbr = old_zone_nbr;
		except_fifo.old_due_time = temp_cl_ptr->due_date_time;
		except_fifo.veh_nbr = 0;
		except_fifo.resolve_code =(short)R_MAN_ZONE;
	}

	if (write(fifo_fd,&except_fifo,sizeof(struct excpt_rec)) == -1) 			/* write to FIFO */
		prt_error(UI_WRITE,catgets(UI_m_catd, UI_1, UI_CK_LN_NO_22, "<except rec to FIFO>"));

	return(DONE);
}

man_veh_mult(detail_call_no, zv_num)
int *detail_call_no;				/* current line being manually assigned */
int zv_num;					/* manually assigned zone or vehicle number */
{
	struct excpt_rec except_fifo;				/* for writing except resolution record to FIFO */
	struct cisam_cl *temp_cl_ptr;
	char date_str[10];					/* for formatting date */
	char time_str[10];					/* for formatting time */
	long	call_nbr;
	int	indx;

	call_nbr = srch_calls[*detail_call_no].nbr;

	while(call_nbr != 0) {
		for(indx = 0; indx < 17; indx++) {
			if(call_nbr == srch_calls[indx].nbr)
				break;
		}

		if(indx < 17) {
			srch_calls[*detail_call_no].veh_nbr = (short)zv_num;

			if(zv_num > 0) {
				strcpy(srch_calls[*detail_call_no].status,ASSIGNED);
				srch_calls[*detail_call_no].pri_status = 64;		/* call is now assigned */
				srch_calls[*detail_call_no].dispd_date_time = (int)time(NULL);
				get_pkup_date(date_str);
				get_pkup_time(time_str);
				strcpy(srch_calls[*detail_call_no].dispd_date,date_str);
				strcpy(srch_calls[*detail_call_no].dispd_time,time_str);
				srch_calls[*detail_call_no].dispatched_by = (short)uid;
			}
			else {
				strcpy(srch_calls[*detail_call_no].status,UNASSGND);	/* unassignment */
				srch_calls[*detail_call_no].pri_status = srch_calls[*detail_call_no].priority;
				srch_calls[*detail_call_no].dispd_date_time = NULL;
				srch_calls[*detail_call_no].dispd_date[0] = NULL;
				srch_calls[*detail_call_no].dispd_time[0] = NULL;
				srch_calls[*detail_call_no].dispatched_by = 0;
				srch_calls[*detail_call_no].pckup_date_time = NULL;
				srch_calls[*detail_call_no].pckup_time[0] = NULL;
				srch_calls[*detail_call_no].pckup_date[0] = NULL;
				srch_calls[*detail_call_no].close_date_time = NULL;
				srch_calls[*detail_call_no].close_time[0] = NULL;
				srch_calls[*detail_call_no].close_date[0] = NULL;
				srch_calls[*detail_call_no].veh_nbr = 0;
				srch_calls[*detail_call_no].drv_id = 0;
			} 
		}

		except_fifo.u_prcs_id = pid;							/* write to dispatch */
		except_fifo.rec_type = EXCEPT_REQ;
		except_fifo.except_ptr = NULL;
		except_fifo.resolve_code = (short)R_M_MAN_VEH;
		except_fifo.except_nbr = 0;
		except_fifo.grouped_with = 0;
		except_fifo.veh_nbr = (short)zv_num;
		except_fifo.zone_nbr = 0;	
		except_fifo.old_zone_nbr = 0;	
		except_fifo.old_due_time = 0;	

		temp_cl_ptr = (struct cisam_cl *)req_buf;
		temp_cl_ptr->nbr = call_nbr;
		if(db(CALL_FILE_ID, READ_KEYS, &cl_key1, ISEQUAL+ISLOCK, 0) < 0) {	/* try to read rec by call # */
			prt_error(UI_DSKREAD,catgets(UI_m_catd, UI_1, UI_CK_LN_NO_23, "<calls file .... FATAL ERROR>"));		/* db failed, no record found */
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
			prt_error(UI_WRITE,catgets(UI_m_catd, UI_1, UI_CK_LN_NO_20, "<error updating calls record... FATAL ERROR>"));		/* db failed */
			return;
		}

		if(db(CALL_FILE_ID,RELEASE,&cl_key1,0,0) < 0)
			prt_error(UI_WRITE,catgets(UI_m_catd, UI_1, UI_CK_LN_NO_21, "<error releasing lock on calls record>"));	/* db failed */

		if ( ( 0 ) && ( CheckTimeCallCriteria( temp_cl_ptr ) ) )
		  {
		    AddTimeCall( temp_cl_ptr );
		  }
		
		if(write(fifo_fd, &except_fifo, sizeof(struct excpt_rec)) == -1) 	/* write to FIFO */
			prt_error(UI_WRITE, catgets(UI_m_catd, UI_1, UI_CK_LN_NO_22, "<except rec to FIFO>"));

		if(call_nbr == srch_calls[*detail_call_no].nbr) 				/* Don't do first call again */
			call_nbr = 0L;
	}
}

man_veh_idle( veh_num, fleet_id, driver_id )
     short veh_num;
     char  fleet_id;
     int   *driver_id;
{
  struct veh_rqst vrq;
  VEH_RESP_REC *vrp;	
  VEH_RESP_HNDL  hVehResp;
    int  rc, times_thru;
    char error_str[80];  


  vrq.d_prcs_id = DISP_DEST;
  vrq.u_prcs_id = pid;
  vrq.rec_type = VEH_REQ;
  vrq.fleet = fleet_id;
  vrq.veh_nbr = veh_num;

  msgsnd( msgkey, (struct msgbuf *)&vrq, sizeof(struct veh_rqst), IPC_NOWAIT );
  hVehResp = TMALLOC( VEH_RESP_REC, 1 );
  vrp = ( VEH_RESP_REC * )hVehResp;

  times_thru = 0;
  while (times_thru <= 2) 
    {
      rc = msgrcv(msgkey,
		  (void *)vrp,sizeof(VEH_RESP_REC),(long)pid,0);
      
      if (rc == -1) 
	{
	  if (errno != EINTR) 
	    {
	      prt_error(UI_FATAL, "%s", 
			catgets(UI_m_catd, UI_1, UI_UI_SSTAT_74, 
				"Error reading from dispatch in msgrcv"));
	      sprintf(error_str,
		      "Error %d reading ipc message from DISPATCH", errno);
	      ERROR(' ',"User-IPC", error_str);
	      TFREE( hVehResp );
	      return( FAIL );
	    }
	  if (++times_thru == 2) 
	    {
	      prt_error(UI_SYSTEM_DOWN, "");
	      ERROR(' ',"User-IPC", 
		    "Timeout reading ipc message from DISPATCH");
	      TFREE( hVehResp );
	      return( FAIL );
	    }
	}
      else 
	if (vrp->rec_type == VEH_REQ)
	  break;
	else 
	  {
	    prt_error(UI_FATAL, "%s", 
		      catgets(UI_m_catd, UI_1, UI_UI_SSTAT_74, 
			      "Error reading from dispatch in msgrcv"));
	    ERROR(' ',"User-IPC", "Invalid msg from dispatch DISPATCH");
	    TFREE( hVehResp );
	    return( FAIL );
	  }
    }	    

  *driver_id = vrp->drv_id;
  if ( !vrp->cur_status.kdt_working )
    return ( SUCCESS );

  if ( !vrp->cur_status.idle ||
       vrp->cur_status.meter_on ||
       vrp->cur_status.pckup ||
       vrp->cur_status.accepted ||
       vrp->cur_status.on_break ||
       vrp->cur_status.offered )
    return ( FAIL );

  return ( SUCCESS );
}

int
GPS_client_display( CALL_REC *cl_ptr )
{
  int                 res, i, sockfd;
  char                send_buf[132];
  struct sockaddr_in  serv_addr, cli_addr;
  struct hostent      *hp;

  bzero( send_buf, sizeof( send_buf ) );

  if ( !strlen( sGPS_host ) )
      {
	prt_error(UI_NO_MSG, "TAXIGEO NOT AUTHORIZED FROM THIS TERMINAL");
	return(-1);
      }
      
  if ( cl_ptr->veh_nbr == 0 )
    sprintf( send_buf, "%c!%c!%d!0", MAP_APP_RQST_TYPE_CLIENT, cl_ptr->fleet, cl_ptr->nbr );
  else
    sprintf( send_buf, "%c!%c!%d!%d", MAP_APP_RQST_TYPE_CLIENT, cl_ptr->fleet, cl_ptr->nbr, cl_ptr->veh_nbr );    
  
  bzero((char *)&serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  hp = gethostbyname(sGPS_host);
  bcopy(hp->h_addr, (char *)&serv_addr.sin_addr, hp->h_length);
  serv_addr.sin_port = htons(6232);
  if ( ( sockfd = socket(AF_INET, SOCK_DGRAM, 0 ) ) != 0 )      
    {
      bzero((char *)&cli_addr, sizeof(cli_addr));
      cli_addr.sin_family = AF_INET;
      cli_addr.sin_addr.s_addr = htonl(INADDR_ANY);
      cli_addr.sin_port = htons(0);
      if ( bind( sockfd, (struct sockaddr *) &cli_addr, sizeof(cli_addr) ) >= 0 )
	res = sendto(sockfd, send_buf, strlen(send_buf), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
      
      close( sockfd );
    }
  return (1);
}

fill_mult_call_rec( CALL_REC  *pMultCall )
{
	int i;

	call_fifo.u_prcs_id = pid; 				/* copy the process id of the user */
	call_fifo.rec_type = CALL_REQ; 				/* copy the type of the request to be sent */
								/* copy the message # for the call to supervisor */
								/* copy the status bits ( for exceptions ) */
	call_fifo.c_isam_nbr = 0;
	call_fifo.call_nbr = pMultCall->nbr; 			/* copy the call # */
	call_fifo.grouped_with = pMultCall->grouped_with; 	/* copy the grouped with field */
	call_fifo.fleet = pMultCall->fleet; 			/* copy the fleet */
	memset((char *)&call_fifo.call_type,'\0', sizeof(struct call_types));

        // ??assume a time call when linking...??
        call_fifo.call_type.time = 1;
        
        switch ( pMultCall->extended_type[0] ) {
        case PASS:
          call_fifo.call_type.passenger = 1;
          break;
        case DELIV:
          call_fifo.call_type.delivery = 1;
          break;
        case TYPEX:
          call_fifo.call_type.xtype = 1;
          break;
        case WAKEUP:
          call_fifo.call_type.wakeup = 1;
          break;
	}  /* end switch */
	if(pMultCall->extended_type[4] == MULT_CALL)
		call_fifo.call_type.multiple = 1;

	if ( pMultCall->extended_type[1] == '\0' )
		return;

	/* check the other char's */
	switch ( pMultCall->extended_type[2] ) {
		case '\0':
			return;
		case SUBS:
			call_fifo.call_type.subscription = 1;
			break;
		case TIMEC:
			call_fifo.call_type.time = 1;
			break;
		case CHARGE:
			call_fifo.call_type.charge = 1;
			break;
	}  /* end switch */

	/* check the last char's */
	switch ( pMultCall->extended_type[3] ) {
		case '\0':
			return;
		case SUBS:
			call_fifo.call_type.subscription = 1;
			break;
		case TIMEC:
			call_fifo.call_type.time = 1;
			break;
		case CHARGE:
			call_fifo.call_type.charge = 1;
			break;

	}  /* end switch */
        
	call_fifo.personal_veh = pMultCall->personal_veh;	/* copy cab number if personal request */
	call_fifo.personal_rqst = pMultCall->personal_rqst;	/* copy O/I */
	call_fifo.pickup_zone = pMultCall->pckup_zone; 	/* copy the pickup zone */
	call_fifo.dest_zone = pMultCall->dest_zone; 		/* copy the destination zone */
	call_fifo.personal_veh = pMultCall->personal_veh; 	/* copy the personal veh fields */
	//call_fifo.msg_nbr = global_msg_num;			/* copy msg # for call with msg exception */
	strcpy(call_fifo.pick_pmap,pMultCall->pckup_pmap);	
	
	for ( i = 0; i < ATTR_MAX; i++ )  { 			/* copy the vehicle attributes */
		if ( pMultCall->veh_attr[i] == YES )  {
			set_bit(i,sizeof(struct veh_attributes),&call_fifo.veh_attr); 			/* set bit */
		}
	}

	for ( i = 0; i < ATTR_MAX; i++ )  { 			/* copy the driver attributes */
		if ( pMultCall->drv_attr[i] == YES )  {
			set_bit(i,sizeof(struct drv_attributes),&call_fifo.drv_attr); 			/* set bit */
		}
	}

	call_fifo.priority = pMultCall->priority; 		/* copy the call priority */
	call_fifo.due_time = pMultCall->due_date_time; 	/* copy the due date time */
	call_fifo.updated_call = 0; 				/* intialize to a new call */

	
	call_fifo.old_due_time = 0; 	/* and no old due time */
	
	call_fifo.call_msg_nbr = pMultCall->call_msg_nbr;
	call_fifo.gps_lat = pMultCall->gps_lat;
	call_fifo.gps_long = pMultCall->gps_long;
	call_fifo.FTJ_nbr = -1;
}  

hilite_multis( srch_index )
  int srch_index;
{
  int    i;

  for ( i = 0; i <= SEARCHES_ON_PG; i++ )
    {
      if ( srch_calls[i].nbr == srch_calls[srch_index].grouped_with )
        {
          dim_line(i);
          if ( ( srch_calls[i].grouped_with > 0 ) && ( srch_calls[i].grouped_with != srch_calls[multi_selected].nbr ) )
            hilite_multis(i);
        }
    }
}
