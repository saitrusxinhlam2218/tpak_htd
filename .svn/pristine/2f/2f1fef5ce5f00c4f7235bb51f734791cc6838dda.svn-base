static char sz__FILE__[]="@(#)kdt_ck_fld.c	10.1.2.1(ver) Date Release 12/2/94" ;
/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  kdt_ck_fld.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:49:04
* @(#)  Last delta: 12/2/94 at 18:27:57
* @(#)  SCCS File: /taxi/sccs/s.kdt_ck_fld.c
*                                                                           *
* Copyright (c) 1990 - MDI Seattle                                          *
*                                                                           *
*****************************************************************************/

#include <cursesX.h>
#include "ui_def.h"
#include "ui_error.h"
#include <sys/types.h>
#include "mads_types.h"
#include "mads_isam.h"
#include "ui_ipc.h"
#include "kdt_ary.h"
#include <ctype.h>
#include "screens.h"
#include "user.h"
#include "df_maint.h"
#include "func_keys.h"
#include <time.h>
#include "ex_key.h"
#include "mad_error.h"
#include "path.h"
#include "language.h"
#include "ui_strdefs.h"

extern struct cisam_vh *vh_ptr;					/* for disk i/o */
extern struct cisam_dr *dr_ptr;					/* for disk i/o */
extern struct cisam_zn *zn_ptr;					/* for disk i/o */
extern struct kdt_emulate Kdt;
extern int scr_name;						/* name of screen on display */
extern int cur_fl_index;					/* index of the fleet the user is currently working with */
extern int pid;
extern int end_kdt;
extern WINDOW *srch_win;
extern int valid_flags;
extern int dr_flags;
extern int vh_flags;
extern int zn_flags;
extern int pr_flags;
extern int act_flags;
extern int fifo_fd;
extern	struct scr_flds kdt_scr_flds[];
extern int prt_entry_areas();
extern int find_findex();
extern int user_index;							/* index into user's struct in shared mem */
extern struct fleets *fleet[];						/* array of pointers to fleets struct */
extern struct users *uiuser[];						/* array of pointers to users struct */
extern struct veh_drv_attrs veh_drv_attr[];				/* struct's for veh/drv attrs*/
extern char req_buf[];
extern short inters_flag;						/* flag set if user wants to enter inters address */
extern struct keydesc dr_key1, veh_key2, zn_key1;
extern int def_fl_index;						/* index for the users default fleet */
extern char   ft_id;
extern int uid;
int    act_code;
int    pty;
int    post;

									/* declare functions for checking fleet permissions */
 extern int ui_fleet_perms0(), ui_fleet_perms1(), ui_fleet_perms2(), ui_fleet_perms3(), ui_fleet_perms4(), ui_fleet_perms5(), 
			ui_fleet_perms6(), ui_fleet_perms7(); 

extern int (*ui_fleet_perms[])(); 

/**********************************************************************************************/
/* updt1_piu: updates the value of piu, for the next field which is 			      */
/*	      is to be read, called by field checking routines				      */
/**********************************************************************************************/
int updt1_piu(key_val,piu_ptr) 
int key_val; 
int *piu_ptr;
{ 
switch(key_val) { 
		case OK:					/* max char's were entered by user */ 
		case '\r': 
		case '\n': 
		case '\t':
	        case KEY_DOWN:
			++*piu_ptr;				/* move to next field */
			return(DONE);

		case KEY_UP:
			--*piu_ptr;					/* move up 1 field */
			return(DONE);

		case SEND_KEY:
			write_to_pipe(piu_ptr); 
			return(DONE);

		case KDT_EMULATE_KEY:
			werase(srch_win);
			wrefresh(srch_win);
			touchwin(stdscr);
			refresh();
			return(2);

		case COMMAND_KEY:
			werase(srch_win);
			wrefresh(srch_win);
			touchwin(stdscr);
			refresh();
			command_line();
			return(2);

		case PRINT_SCR_KEY:
			print_scr(srch_win);
			return(DONE);

		case HELP_KEY:
			help(KDT_EMULATION_WIN_SCR,piu_ptr);
			return(DONE);

		default:					/* some other function key entered */
			return(DONE);
	}
}

/*****************************************************************************************/
/* check_dummy2 - dummy verification routine						 */
/*****************************************************************************************/
int check_dummy2(piu_ptr,read_buf,key_val,field_entered)
int *piu_ptr;					/* pointer to piu */
char *read_buf;					/* pointer to buffer for reading field */
int key_val;					/* value of key pressed by user */
int *field_entered;				/* whether field entered =1; or not=0 */
{
  return(DONE);
}

/*****************************************************************************************/
/* check_fleet2 - verify fleet entered.     						 */
/*****************************************************************************************/
int check_fleet2(piu_ptr,read_buf,key_val,field_entered)
int *piu_ptr;					/* pointer to piu */
char *read_buf;					/* pointer to buffer for reading field */
int key_val;					/* value of key pressed by user */
int *field_entered;				/* whether field entered =1; or not=0 */
{
	int old_piu,ret_code;
	int in_fleet;

	if (*field_entered) {
		if ((in_fleet = find_findex(read_buf[0])) == -1) {
			prt_error(UI_NOFIELD," ");
			valid_flags = 0;
			old_piu = *piu_ptr;
			ret_code = updt1_piu(key_val,piu_ptr);	
			*piu_ptr = old_piu;
			return(ret_code);
		 }
		if ((*ui_fleet_perms[in_fleet])() == NOT_ALLOWED) {
			prt_error(UI_NOT_ALLOWED," ");
			valid_flags = 0;
			old_piu = *piu_ptr;
			ret_code = updt1_piu(key_val,piu_ptr);	
			*piu_ptr = old_piu;
			return(ret_code);
		}
		if (fleet[in_fleet]->active != YES) {
			prt_error(UI_NOT_ACTIVE," ");
			valid_flags = 0;
			old_piu = *piu_ptr;
			ret_code = updt1_piu(key_val,piu_ptr);	
			*piu_ptr = old_piu;
			return(ret_code);
		}
		ft_id = read_buf[0];
		wattrset(srch_win,A_UNDERLINE);
		mvwprintw(srch_win,D_FLEET_ROW,D_FLEET_COL,"%c",ft_id);
		wattrset(srch_win,0);
		valid_flags = 1;
		return(updt1_piu(key_val,piu_ptr));	
	}
	else {
		if (!valid_flags) {
			prt_error(UI_NOFIELD," ");
			old_piu = *piu_ptr;
			ret_code = updt1_piu(key_val,piu_ptr);	
			*piu_ptr = old_piu;
			return(ret_code);
		}
		return(updt1_piu(key_val,piu_ptr));	
	}
}

/*****************************************************************************************/
/* check_veh1 - verify vehicle number entered						 */
/*****************************************************************************************/
int check_veh1(piu_ptr,read_buf,key_val,field_entered)
int *piu_ptr;					/* pointer to piu */
char *read_buf;					/* pointer to buffer for reading field */
int key_val;					/* value of key pressed by user */
int *field_entered;				/* whether field entered =1; or not=0 */
{
	int i,number,old_piu,ret_code;

	if (*field_entered) {
		vh_ptr->fleet = ft_id; 
		Kdt.fleet_id = ft_id;
		if (!numeric1(read_buf)) {
			vh_flags = 0;
			prt_error(UI_NOFIELD," ");
			old_piu = *piu_ptr;
			ret_code = updt1_piu(key_val,piu_ptr);	
			*piu_ptr = old_piu;
			return(ret_code);
		}
		number = atoi(read_buf);
		vh_ptr->nbr = (short)number;
		Kdt.veh_nbr = (short)number;
		if ((db(VEHICLE_FILE_ID,READ_KEYS,&veh_key2,ISEQUAL,0)) < 0) {
			vh_flags = 0;
			prt_error(UI_NO_MSG,catgets(UI_m_catd, UI_1, UI_TEXT_6, "Vehicle id is not in the VEHICLE file"));
			old_piu = *piu_ptr;
			ret_code = updt1_piu(key_val,piu_ptr);	
			*piu_ptr = old_piu;
			return(ret_code);
		}
		vh_flags = 1;
		wattrset(srch_win,A_UNDERLINE);
		mvwprintw(srch_win,D_FLEET_ROW,D_VEH_COL,"%4d",vh_ptr->nbr);
		wattrset(srch_win,0);
		return(updt1_piu(key_val,piu_ptr));	
	}
	else {
		if (key_val == COMMAND_KEY)	{
			werase(srch_win);
			wrefresh(srch_win);
			touchwin(stdscr);
			refresh();
			command_line();
			return(2);
		}
		if (key_val == KDT_EMULATE_KEY)	{
			werase(srch_win);
			wrefresh(srch_win);
			touchwin(stdscr);
			refresh();
			return(2);
		}

		if((!vh_flags) && (key_val != KEY_UP)) {
			prt_error(UI_NOFIELD," ");
			old_piu = *piu_ptr;
			ret_code = updt1_piu(key_val,piu_ptr);	
			*piu_ptr = old_piu;
			return(ret_code);
		}
		wattrset(srch_win,A_UNDERLINE);
		wattrset(srch_win,0);
		return(updt1_piu(key_val,piu_ptr));	
	}
} 

/*****************************************************************************************/
/* check_action - verify action code entered						 */
/*****************************************************************************************/
int check_action(piu_ptr,read_buf,key_val,field_entered)
int *piu_ptr;					/* pointer to piu */
char *read_buf;					/* pointer to buffer for reading field */
int key_val;					/* value of key pressed by user */
int *field_entered;				/* whether field entered =1; or not=0 */
{
	int old_piu,ret_code,act_cd;
	int grp;

	if (*field_entered) {
		act_cd = atoi(read_buf);
		act_code = (short)act_cd;
		grp = getgid();			/* allow programmers to use all commands in kdt_emulation */
		if (((EMUL_POST <= act_code) && (act_code <= EMUL_MSG99)) || grp == 400) {
			act_flags = 1;
			wattrset(srch_win,A_UNDERLINE);
			mvwprintw(srch_win,D_FLEET_ROW,D_ACTION_COL,"%-1d",act_code);
			wattrset(srch_win,0);

			switch(act_code) {
				case	EMUL_POST:
					post = 1;

					/* Driver id */
					cl_fld(D_DRV_ROW, D_DRV_COL, CHARS_DRV); 
					return(updt1_piu(key_val,piu_ptr));

				case	EMUL_CPOST:
				case	EMUL_EPOST:
					post = 0;
					/* Taxi queue priority */
					cl_fld(D_PRIORITY_ROW, D_PRIORITY_COL, CHARS_PRIORITY);

					/* Driver id */
					cl_fld(D_DRV_ROW, D_DRV_COL, CHARS_DRV);
					return(updt1_piu(key_val,piu_ptr));

				case	EMUL_S_IN:
					post = 0;
					
					/* Zone # */
					cl_fld(D_ZONE_ROW, D_ZONE_COL, CHARS_ZONE);

					/* Taxi queue priority */
					cl_fld(D_PRIORITY_ROW, D_PRIORITY_COL, CHARS_PRIORITY);
					++*piu_ptr;
					return(updt1_piu(key_val,piu_ptr));

				default:
					post = 0;
					
					/* Zone # */
					cl_fld(D_ZONE_ROW, D_ZONE_COL, CHARS_ZONE);

					/* Taxi queue priority */
					cl_fld(D_PRIORITY_ROW, D_PRIORITY_COL, CHARS_PRIORITY);

					/* Driver id */
					cl_fld(D_DRV_ROW, D_DRV_COL, CHARS_DRV); 

					/* map the dispalyed options to the real options */
					if ( act_code == EMUL_NOSHOW )		/* really want meter on */
						act_code = EMUL_PCKUP;
					else if ( act_code == EMUL_FLAG )	/* really want meter off */
						act_code = EMUL_CLOSE;
					
					if( key_val == SEND_KEY) {
						write_to_pipe(piu_ptr); 
						return(DONE);
					}
					*piu_ptr = 2;
					return(DONE);
			}
		}
		else {
			act_flags = 0;
			prt_error(UI_NOFIELD," ");
		}
	}
	else {
		if (key_val == COMMAND_KEY)	{
			werase(srch_win);
			wrefresh(srch_win);
			touchwin(stdscr);
			refresh();
			command_line();
			return(2);
		}
		if (key_val == KDT_EMULATE_KEY)	{
			werase(srch_win);
			wrefresh(srch_win);
			touchwin(stdscr);
			refresh();
			return(2);
		}

		if( key_val == SEND_KEY) {
			write_to_pipe(piu_ptr); 
			return(DONE);
		}

		if (key_val != KEY_UP) {
			if (act_flags) {
				switch(act_code) {
					case	EMUL_POST:
					case	EMUL_CPOST:
					case	EMUL_EPOST:
						return(updt1_piu(key_val,piu_ptr));

					case	EMUL_S_IN:
						++*piu_ptr;
						return(updt1_piu(key_val,piu_ptr));

					default:
						*piu_ptr = 2;
						return(DONE);
				}
			}
			else {
				prt_error(UI_NOFIELD," ");
				old_piu = *piu_ptr;
				ret_code = updt1_piu(key_val,piu_ptr);	
				*piu_ptr = old_piu;
				return(ret_code);
			}
		}
		else	
			return(updt1_piu(key_val,piu_ptr));
	} 
}

/*****************************************************************************************/
/* check_zone1 - verify zone number entered						 */
/*****************************************************************************************/
int check_zone1(piu_ptr,read_buf,key_val,field_entered)
int *piu_ptr;					/* pointer to piu */
char *read_buf;					/* pointer to buffer for reading field */
int key_val;					/* value of key pressed by user */
int *field_entered;				/* whether field entered =1; or not=0 */
{
	int i,number,old_piu,ret_code;

	if (*field_entered) {
		if (!numeric1(read_buf)) {
			zn_flags = 0;
			prt_error(UI_NOFIELD," ");
			old_piu = *piu_ptr;
			ret_code = updt1_piu(key_val,piu_ptr);	
			*piu_ptr = old_piu;
			return(ret_code);
		}

		number = atoi(read_buf);
		zn_ptr->nbr = (short)number;
		Kdt.zone_nbr = (short)number;
		zn_ptr->fleet = ft_id; 
		if(number != 0){					/* allow zero posting */
			if ((db(ZONE_FILE_ID,READ_KEYS,&zn_key1,ISEQUAL,0)) < 0) {
				zn_flags = 0;
				prt_error(UI_NO_MSG,catgets(UI_m_catd, UI_1, UI_TEXT_4, "Zone is not in ZONE file"));
				old_piu = *piu_ptr;
				ret_code = updt1_piu(key_val,piu_ptr);	
				*piu_ptr = old_piu;
				return(ret_code);
			}
		}
		zn_flags = 1;
		wattrset(srch_win,A_UNDERLINE);
		mvwprintw(srch_win,D_FLEET_ROW,D_DRV_COL,"%3d",zn_ptr->nbr);
		wattrset(srch_win,0);
		if ((post) && (key_val != KEY_UP)) {
			++*piu_ptr;
			return(updt1_piu(key_val,piu_ptr));	
		}
		if (!post) {
			if( key_val == SEND_KEY) {
				write_to_pipe(piu_ptr); 
				return(DONE);
			}
			--*piu_ptr;
			return(DONE);
		}
	}
	else  {
		if (key_val == COMMAND_KEY)	{
			werase(srch_win);
			wrefresh(srch_win);
			touchwin(stdscr);
			refresh();
			command_line();
			return(2);
		}
		if (key_val == KDT_EMULATE_KEY)	{
			werase(srch_win);
			wrefresh(srch_win);
			touchwin(stdscr);
			refresh();
			return(2);
		}
		if( key_val == SEND_KEY) {
			write_to_pipe(piu_ptr); 
			return(DONE);
		}
		if (zn_flags) {
			if ((post) && (key_val != KEY_UP)) {
				++*piu_ptr;
				return(updt1_piu(key_val,piu_ptr));	
			}
			else {
				--*piu_ptr;
				return(DONE);
			}
		}
		else {
			if (key_val != KEY_UP)	{
				prt_error(UI_NOFIELD," ");
				old_piu = *piu_ptr;
				ret_code = updt1_piu(key_val,piu_ptr);	
				*piu_ptr = old_piu;
				return(ret_code);
			}
			else {
				--*piu_ptr;
				return(DONE);
			}
		}
	}
} 

/*****************************************************************************************/
/* check_driver - verify driver number entered						 */
/*****************************************************************************************/
int check_driver(piu_ptr,read_buf,key_val,field_entered)
int *piu_ptr;				/* pointer to piu */
char *read_buf;				/* pointer to buffer for reading field */
int key_val;				/* value of key pressed by user */
int *field_entered;			/* whether field entered =1; or not=0 */
{
	int i,id_number,old_piu,ret_code;
	char ft_id;

	if (*field_entered) {
		dr_ptr->fleet = vh_ptr->fleet;
		if (!numeric1(read_buf)) {
			dr_flags = 0;
			prt_error(UI_NOFIELD," ");
			old_piu = *piu_ptr;
			ret_code = updt1_piu(key_val,piu_ptr);	
			*piu_ptr = old_piu;
			return(ret_code);
		}
		dr_ptr->id = atoi(read_buf);
		Kdt.drv_id = dr_ptr->id;
		if ((db(DRIVER_FILE_ID,READ_KEYS,&dr_key1,ISEQUAL,0)) < 0) {
			dr_flags = 0;
			prt_error(UI_NO_MSG,catgets(UI_m_catd, UI_1, UI_TEXT_5, "Driver id is not in DRIVER file"));
			dr_ptr->id = 0;
			old_piu = *piu_ptr;
			ret_code = updt1_piu(key_val,piu_ptr);	
			*piu_ptr = old_piu;
			return(ret_code);
		}
		dr_flags = 1;
		wattrset(srch_win,A_UNDERLINE);
		mvwprintw(srch_win,D_DRV_ROW,D_DRV_COL,"%5d",dr_ptr->id);
		wattrset(srch_win,0);
		if( key_val == SEND_KEY) {
			write_to_pipe(piu_ptr);
			return(DONE);
		}
		*piu_ptr = 3;
		return(DONE);
	}
	else {
		if (key_val == COMMAND_KEY)	{
			werase(srch_win);
			wrefresh(srch_win);
			touchwin(stdscr);
			refresh();
			command_line();
			return(2);
		}
		if (key_val == KDT_EMULATE_KEY)	{
			werase(srch_win);
			wrefresh(srch_win);
			touchwin(stdscr);
			refresh();
			return(2);
		}
		if (key_val == SEND_KEY) {
			write_to_pipe(piu_ptr);
			return(DONE);
		}
		*piu_ptr = 3;
		return(DONE);
	}
}

/*****************************************************************************************/
/* check_pri2 - verify queue priority entered						 */
/*****************************************************************************************/
int check_pri2(piu_ptr,read_buf,key_val,field_entered)
int *piu_ptr;					/* pointer to piu */
char *read_buf;					/* pointer to buffer for reading field */
int key_val;					/* value of key pressed by user */
int *field_entered;				/* whether field entered =1; or not=0 */
{
	int old_piu,ret_code,priority;

	if (*field_entered) {
		if (!numeric1(read_buf)) {
			pr_flags = 0;
			prt_error(UI_NOFIELD," ");
			old_piu = *piu_ptr;
			ret_code = updt1_piu(key_val,piu_ptr);	
			*piu_ptr = old_piu;
			return(ret_code);
		}
		priority = atoi(read_buf);
		pty = (short)priority;
		if ((1 <= pty) && (pty <= 9)) {
			pr_flags = 1;
			wattrset(srch_win,A_UNDERLINE);
			mvwprintw(srch_win,D_FLEET_ROW,D_PRIORITY_COL,"%-2d",pty);
			wattrset(srch_win,0);
			if (key_val == KEY_UP)
				--*piu_ptr;
			return(updt1_piu(key_val,piu_ptr));
		}
		else {
			pr_flags = 0;
			prt_error(UI_NOFIELD," ");
			old_piu = *piu_ptr;
			ret_code = updt1_piu(key_val,piu_ptr);	
			*piu_ptr = old_piu;
			return(ret_code);
		}
	}
	else {
		if (key_val == COMMAND_KEY)	{
			werase(srch_win);
			wrefresh(srch_win);
			touchwin(stdscr);
			refresh();
			command_line();
			return(2);
		}
		if (key_val == KDT_EMULATE_KEY)	{
			werase(srch_win);
			wrefresh(srch_win);
			touchwin(stdscr);
			refresh();
			return(2);
		}
		if (key_val == KEY_UP)
			--*piu_ptr;
		return(updt1_piu(key_val,piu_ptr));
	} 
}
		
/*****************************************************************************************/
/* write_to_pipe - write info to dispatch    						 */
/*****************************************************************************************/
write_to_pipe(piu_ptr)
int  *piu_ptr;
{
	switch(act_code) {
		case EMUL_POST:	
			if ((vh_flags) && (act_flags) &&(zn_flags) && (pr_flags))	
				break;
			else {
				prt_error(UI_WRITE,catgets(UI_m_catd, UI_1, UI_KDT_CK_FLD_3, "Veh_nbr,Act_code,Zone_nbr,Priority all fields requied to enter"));
				return;
			}

		case EMUL_CPOST:	
		case EMUL_EPOST:	
			if ((vh_flags) && (act_flags) &&(zn_flags))	
				break;
			else {
				prt_error(UI_WRITE,catgets(UI_m_catd, UI_1, UI_KDT_CK_FLD_4, "Veh_nbr,Act_code,Zone_nbr all fields required to enter"));
				return;
			}

		case EMUL_S_IN:	
			if ((vh_flags) && (act_flags) &&(dr_flags))	
				break;
			else {
				prt_error(UI_WRITE,catgets(UI_m_catd, UI_1, UI_KDT_CK_FLD_5, "Veh_nbr,Act_code,Driver_id all fields required to enter"));
				return;
			}

		case EMUL_S_OUT:	
		case EMUL_RESET:	
		case EMUL_DWNLD:	
		case  EMUL_ACPT:
		case  EMUL_REJ:
		case  EMUL_NOSHOW:
		case  EMUL_FLAG	:
		case  EMUL_PCKUP:
		case  EMUL_CLOSE:
	case EMUL_MSG97:
	case EMUL_MSG98:
	case EMUL_MSG99:
			if ((vh_flags) && (act_flags))	
				break;
			else {
				prt_error(UI_WRITE,catgets(UI_m_catd, UI_1, UI_KDT_CK_FLD_6, "Veh_nbr,Act_code both fields required to enter"));
				return;
			}

		default:
			prt_error(UI_WRITE," ");
			return;
	}

	Kdt.u_prcs_id = (long)pid;
	Kdt.rec_type = KDT_REQ;
	Kdt.action_code = act_code;
	Kdt.priority = pty;
	log_kdt_message(act_code);
	if (write(fifo_fd,&Kdt,sizeof(struct kdt_emulate)) < 0)	{
		prt_error(UI_WRITE," ");
		return(DONE);
	}
	prt_entry_areas(srch_win,kdt_scr_flds,6);
	wattrset(srch_win,A_UNDERLINE);
	wattrset(srch_win,0);
	vh_flags = 0;
	zn_flags = 0;
	dr_flags = 0;
	pr_flags = 0;
	act_flags = 0;
	*piu_ptr = 2;

	Kdt.veh_nbr = 0;
	Kdt.zone_nbr = 0;
	Kdt.priority = 0;
	Kdt.drv_id = 0;
	if (def_fl_index == -1)
		mvwprintw(srch_win,D_FLEET_ROW,D_FLEET_COL,"%c",' ');
	else {
		valid_flags = 1;
		ft_id = fleet[def_fl_index]->fleet_id;
		wattrset(srch_win,A_UNDERLINE);
		mvwprintw(srch_win,D_FLEET_ROW,D_FLEET_COL,"%c",ft_id);
		wattrset(srch_win,0);
	}
}


/*****************************************************************************************/
/* cl_fld - clear field                      						 */
/*****************************************************************************************/
cl_fld(row1,col1,nums)
int  row1;
int  col1;
int  nums;
{
	int  i;

	wattrset(srch_win,A_UNDERLINE);
	wmove(srch_win,row1,col1);
	for (i = 0; i < nums; ++i)
		waddch(srch_win,' ');
	wattrset(srch_win,0);
}

/*****************************************************************************************/
/* numeric1 - is a field numeric             						 */
/*****************************************************************************************/
numeric1(in_str)
char  *in_str;
{
	int i;
	int valid;

	i = 0;
	valid = 1;
	while(*in_str) {
		if (!isdigit(*in_str))
			valid = 0;
		in_str++;
	}
	return(valid);
}
log_kdt_message(type)
int type;
{
    	struct cisam_ml	*ml_ptr;					/* pointer to message log buffer*/ 
	char act_str[32];
 	int time_now;	
	int pri_val;

	act_str[0] = '\0';

	switch(type) {

		case 1:	strcpy(act_str, catgets(UI_m_catd, UI_1, UI_KDT_CK_FLD_7, "BOOK"));
			break;		/* BOOK used to be POST 	*/

		case 2:	strcpy(act_str, catgets(UI_m_catd, UI_1, UI_KDT_CK_FLD_8, "CBOOK"));
			break;		/* CBOOK used to be CPOST	*/

		case 3:	strcpy(act_str, catgets(UI_m_catd, UI_1, UI_KDT_CK_FLD_9, "EBOOK"));
			break;		/* EBOOK used to be EPOST	*/

		case 4:	strcpy(act_str, catgets(UI_m_catd, UI_1, UI_KDT_CK_FLD_10, "SIGN-IN"));
			break;

		case 5:	strcpy(act_str, catgets(UI_m_catd, UI_1, UI_KDT_CK_FLD_11, "SIGN-OUT"));
			break;

		case 6:	strcpy(act_str, catgets(UI_m_catd, UI_1, UI_KDT_CK_FLD_12, "RESET MDT"));
			break;		/* RESET MDT used ot be RESET KDT */

		case 7:	strcpy(act_str, catgets(UI_m_catd, UI_1, UI_KDT_CK_FLD_13, "MDT DOWNLOAD"));
			break;
		case 8:	strcpy(act_str, catgets(UI_m_catd, UI_1, UI_KDT_CK_FLD_22, "ACCEPT TRIP"));
			break;
		case 9:	strcpy(act_str, catgets(UI_m_catd, UI_1, UI_KDT_CK_FLD_23, "REJECT TRIP"));
			break;
		case 10:	strcpy(act_str, catgets(UI_m_catd, UI_1, UI_KDT_CK_FLD_26, "NO SHOW"));
			break;
		case 11:	strcpy(act_str, catgets(UI_m_catd, UI_1, UI_KDT_CK_FLD_27, "FLAG"));
			break;
		case 12:	strcpy(act_str, catgets(UI_m_catd, UI_1, UI_KDT_CK_FLD_24, "METER ON"));
			break;
		case 13:	strcpy(act_str, catgets(UI_m_catd, UI_1, UI_KDT_CK_FLD_25, "METER OFF"));
			break;
	}

	if(type != 1)
		pri_val = 0;
	else
		pri_val = Kdt.priority;

    	ml_ptr = (struct cisam_ml *)req_buf;				/* initialize pointer to request buffer */
	if(db(MSGLOG_FILE_ID,OPEN,&ml_key1,ISINOUT+ISMANULOCK,0) < 0) {	/* open message log file */
    		ERROR(' ',MSGLOG_FILE,"Error opening file");		/* if problem, ERROR */
    		return(-1);
	}

	/* fill in message log info for new record to add */
	ml_ptr->from = uid;
	time_now = get_time();
	ml_ptr->date_time_sent = time_now;				
	get_asc_time(time_now,ml_ptr->date_sent,ml_ptr->time_sent);	
	ml_ptr->date_sent[9] = '\0';					
	ml_ptr->time_sent[6] = '\0';					
	ml_ptr->supervisor_id = 0;					
	ml_ptr->supervisor_saw = '\0';					
	ml_ptr->repeat = '2';					
	ml_ptr->fleet = Kdt.fleet_id;					
	ml_ptr->to_type = MSG_VEHICLE;
	ml_ptr->to_selection[0] = '\0';
	sprintf(ml_ptr->msg_line1,"%s %d  %s %s  %s %d  %s %d  %s %d ",
		catgets(UI_m_catd, UI_1, UI_KDT_CK_FLD_15, "Vehicle number:"), Kdt.veh_nbr, catgets(UI_m_catd, UI_1, UI_KDT_CK_FLD_16, "Action:"), act_str, catgets(UI_m_catd, UI_1, UI_KDT_CK_FLD_17, "Zone Number:"), Kdt.zone_nbr,
		catgets(UI_m_catd, UI_1, UI_KDT_CK_FLD_18, "Taxi Queue Priority:"), pri_val, catgets(UI_m_catd, UI_1, UI_KDT_CK_FLD_19, "Driver ID"), Kdt.drv_id);
	sprintf(ml_ptr->msg_line2," ");

	if(db(MSGLOG_FILE_ID,ADD_REC,&ml_key1,0,0) < 0) {		/* add message log record */
    		ERROR(' ',MSGLOG_FILE,"Error adding record to file");	/* if problem, ERROR */
    		db(MSGLOG_FILE_ID,CLOSE,&ml_key1,0,0);
    		return(-1);
	}
	if(db(MSGLOG_FILE_ID,CLOSE,&ml_key1,0,0) < 0) {			/* close message log file */
    		ERROR(' ',MSGLOG_FILE,"Error closing file");		/* if problem, ERROR */
    		return(-1);
	}
}
