static char sz__FILE__[]="@(#)men_ck_fld.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  men_ck_fld.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:48:10
 * @(#)  Last delta: 12/2/94 at 11:38:00
 * @(#)  SCCS File: /taxi/sccs/s.men_ck_fld.c
 *                                                                           *
 *****************************************************************************/



#include <ctype.h>
#include <sys/types.h>
#include <cursesX.h>
#include "menu_maint.h"
#include "mad_error.h"
#include "mads_types.h"
#include "mads_isam.h"
#include "tokenize.h"
#include "language.h"
#include "menu_m_strdefs.h"

extern short ipc_required;		/* Set if a variable which resides */
					/* shared memory changes */
extern struct cisam_mf *mf_ptr;		/* pointer to menu file structure */
extern char read_buf[];			/* buffer for reading fields */
extern FILE *trace_fp;			/* trace file pointer */
extern char trace_str[];		/* trace string */
extern int piu;				/* prompt in use */
extern int key_val;			/* value of key entered */
extern int error_n_fld;			/* Set if an error in field condition exists  */
extern prt_error();
extern clear_err_line();
extern cmd_flag;
int update_piu();
int valid_cmd();

int check_dummy(), check_m_nbr(), check_cmd(), check_lst_menu();
int check_type(), check_desc(), check_unix_cmd(), check_pre_scr();

/*  Declare an array of pointers to functions which check the field entered
 *  for validity.  
 */

int (*check_field[])() = { check_dummy, check_m_nbr, check_cmd, check_lst_menu,
		       	   check_type, check_desc, check_unix_cmd, check_pre_scr};


/*****************************************************************************
* check_dummy - this routine is simply a place holder so the first fiels is 1.
*******************************************************************************/

int check_dummy (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
}

/******************************************************************************
* check_m_nbr - menu number assigned by system, not input by menu_maint.
*******************************************************************************/

int check_m_nbr (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
#ifdef DEBUG
	sprintf(trace_str,"Entering check_m_nbr\n");
	TRACE(trace_str);
#endif

#ifdef DEBUG
	sprintf(trace_str,"Leaving check_m_nbr\n");
	TRACE(trace_str);
#endif
}

/******************************************************************************
* check_cmd - check menu command field.
*******************************************************************************/

int check_cmd (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
	int cmd_nbr;			/* command number input by the user */

#ifdef DEBUG
	sprintf(trace_str,"Entering check_cmd\n");
	TRACE(trace_str);
#endif

	if(*field_entered)  {
		clear_err_line();

		if(!valid_cmd(read_buf))  {
			prt_error(catgets(MEN_MAINT_m_catd, MEN_MAINT_SET, MEN_MAINT_1, "Error in field"));
			error_n_fld = 1;
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_cmd - 1\n");
	TRACE(trace_str);
#endif
			return(NOT_VALID);
		}

		cmd_nbr = atoi(read_buf);

	/*	if(((1 <= cmd_nbr) && (cmd_nbr <= 9)) ||
	   	   ((11 <= cmd_nbr) && (cmd_nbr <= 19)) ||
	   	   ((21 <= cmd_nbr) && (cmd_nbr <= 29)) ||
	   	   ((31 <= cmd_nbr) && (cmd_nbr <= 39)) ||
	   	   ((61 <= cmd_nbr) && (cmd_nbr <= 69)) ||
	   	   ((281 <= cmd_nbr) && (cmd_nbr <= 289)))  {	*/
			clear_err_line();
			error_n_fld = 0;
			cmd_flag = 1;
			mf_ptr->command = (short)cmd_nbr;  /* move command number to isam buffer */
			mvprintw(D_M_NBR_ROW,D_CMD_COL,"%3d",mf_ptr->command);
			refresh();
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_cmd - 2\n");
	TRACE(trace_str);
#endif

			return(VALID);
	}
	else  { 	/* not entered */
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_cmd - 4\n");
	TRACE(trace_str);
#endif
		if((!cmd_flag) && (key_val != ESC))	{
			prt_error(catgets(MEN_MAINT_m_catd, MEN_MAINT_SET, MEN_MAINT_2, "You must enter value"));
			return(NOT_VALID);
		}

	}

}  /* End of check_cmd()  */

/******************************************************************************
* check_lst_menu - check last menu field.
*******************************************************************************/

int check_lst_menu (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
    char c;

#ifdef DEBUG
	sprintf(trace_str,"Entering check_lst_menu\n");
	TRACE(trace_str);
#endif
        c = Toupper(read_buf[0]);

	if(*field_entered)  {
		if(c == YES || c == NO) {
			ipc_required = 1;
			clear_err_line();
			error_n_fld = 0;
			mf_ptr->last_menu_item = c;
			mvprintw(D_M_NBR_ROW,D_LST_MENU_COL,"%c",mf_ptr->last_menu_item);
			refresh();
			update_piu();
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_lst_menu - 1\n");
	TRACE(trace_str);
#endif

			return(VALID);
		}
		else  {
			prt_error(catgets(MEN_MAINT_m_catd, MEN_MAINT_SET, MEN_MAINT_1, "Error in field"));
			error_n_fld = 1;
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_lst_menu - 2\n");
	TRACE(trace_str);
#endif

			return(NOT_VALID);
		}
	}
	if(!error_n_fld)
		update_piu();
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_lst_menu - 3\n");
	TRACE(trace_str);
#endif

	return(VALID);

}  /* End of check_lst_menu()  */

/******************************************************************************
* check_type - check menu type field.
*******************************************************************************/

int check_type (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
    char c;

#ifdef DEBUG
	sprintf(trace_str,"Entering check_type\n");
	TRACE(trace_str);
#endif
        c = Toupper(read_buf[0]);

	if(*field_entered)  {
	        if (c == UI_EXTERNAL ||
		    c == UI_MENU ||
		    c == UI_SCREEN) {
			ipc_required = 1;
			clear_err_line();
			error_n_fld = 0;
			mf_ptr->type = c;
			mvprintw(D_M_NBR_ROW,D_TYPE_COL,"%c",mf_ptr->type);
			refresh();
			update_piu();
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_type - 1\n");
	TRACE(trace_str);
#endif

			return(VALID);
		}
		else  {
			prt_error(catgets(MEN_MAINT_m_catd, MEN_MAINT_SET, MEN_MAINT_1, "Error in field"));
			error_n_fld = 1;
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_type - 2\n");
	TRACE(trace_str);
#endif

			return(NOT_VALID);
		}
	}
	if(!error_n_fld)
		update_piu();
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_type - 3\n");
	TRACE(trace_str);
#endif

	return(VALID);

}

/******************************************************************************
* check_desc - check menu description field.
*******************************************************************************/

int check_desc (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
#ifdef DEBUG
	sprintf(trace_str,"Entering check_desc\n");
	TRACE(trace_str);
#endif
	if(*field_entered)  {
		ipc_required = 1;
		clear_err_line();
		error_n_fld = 0;
		strcpy(mf_ptr->desc,read_buf);
		mvprintw(D_M_NBR_ROW,D_DESC_COL,"%s",mf_ptr->desc);
		refresh();
		update_piu();
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_desc - 1\n");
	TRACE(trace_str);
#endif

		return(VALID);
	}
	if(!error_n_fld)
		update_piu();
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_desc - 2\n");
	TRACE(trace_str);
#endif
	return(VALID);

}  /* End of check_desc()  */

/******************************************************************************
* check_unix_cmd - check user supplied unix command field.
*******************************************************************************/

int check_unix_cmd (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
#ifdef DEBUG
	sprintf(trace_str,"Entering check_unix_cmd\n");
	TRACE(trace_str);
#endif
	if(*field_entered)  {
		ipc_required = 1;
		clear_err_line();
		error_n_fld = 0;
		strcpy(mf_ptr->unix_string,read_buf);
		mvprintw(D_UNIX_CMD_ROW,D_UNIX_CMD_COL,"%s",mf_ptr->unix_string);
		refresh();
		update_piu();
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_unix_cmd - 1\n");
	TRACE(trace_str);
#endif

		return(VALID);
	}
	if(!error_n_fld)
		update_piu();
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_unix_cmd - 2\n");
	TRACE(trace_str);
#endif
	return(VALID);

}  /*  End of check_unix_cmd()  */

/******************************************************************************
* check_pre_scr - check previous screen field.
*******************************************************************************/

int check_pre_scr (piu_ptr, read_buf, key_val, field_entered)
	int *piu_ptr;	/* pointer to the number of the field read in */
	char *read_buf; /* pointer to the buffer in which the field was read in */
	int key_val;    /* value of the key pressed or entered */
	int *field_entered; /* indicates if data was entered or not */
{
    char c;

#ifdef DEBUG
	sprintf(trace_str,"Entering check_pre_scr\n");
	TRACE(trace_str);
#endif
        c = Toupper(read_buf[0]);

	if(*field_entered)  {
		if(c == YES || c == NO) {
			ipc_required = 1;
			clear_err_line();
			error_n_fld = 0;
			mf_ptr->prev_scr_flag = c;
			mvprintw(D_UNIX_CMD_ROW,D_PRE_SCR_COL,"%c",mf_ptr->prev_scr_flag);
			refresh();
			update_piu();
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_pre_scr - 1\n");
	TRACE(trace_str);
#endif

			return(VALID);
		}
		else  {
			prt_error(catgets(MEN_MAINT_m_catd, MEN_MAINT_SET, MEN_MAINT_1, "Error in field"));
			error_n_fld = 1;
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_pre_scr - 2\n");
	TRACE(trace_str);
#endif

			return(NOT_VALID);
		}
	}
	if(!error_n_fld)
		update_piu();
#ifdef DEBUG
	sprintf(trace_str,"Leaving check_pre_scr - 3\n");
	TRACE(trace_str);
#endif
	return(VALID);

} /*  End of check_pre_scr()  */

/******************************************************************************
* update_piu - update prompt in use variable.
*******************************************************************************/

update_piu()
{
#ifdef DEBUG
	sprintf(trace_str,"Entering update_piu\n");
	TRACE(trace_str);
#endif
	if(key_val == '\t' ||
	   key_val == '\n' ||
	   key_val == KEY_DOWN ||
	   key_val == VALID ||
	   key_val == KEY_RIGHT)
		if(piu == 7)
			piu = 3;
		else if(piu == F_LST_MENU)
			piu = F_DESC;
		else
			piu += 1;

	if(key_val == '\b' ||
	   key_val == KEY_LEFT ||
	   key_val == KEY_UP)
		if(piu == 3)
			piu = 7;
		else if(piu == F_DESC)
			piu = F_LST_MENU;
		else
			piu -= 1;
#ifdef DEBUG
	sprintf(trace_str,"Leaving update_piu \n");
	TRACE(trace_str);
#endif
	return;

} /* End of update_piu()  */

/****************************************************************************
* valid_cmd - make sure the command entered is numeric.
****************************************************************************/

valid_cmd(in_str)
char in_str[132];
{
	int i;		/* loop index */
	int validity;	/* = 0,1 for bad, good */

#ifdef DEBUG
	sprintf(trace_str,"Entering valid_cmd\n");
	TRACE(trace_str);
#endif
	i = 0;
	validity = 1;	/* assume the command is valid */

	while(in_str[i])
		if(!isdigit(in_str[i++]))
			validity = 0;
#ifdef DEBUG
	sprintf(trace_str,"Leaving valid_cmd \n");
	TRACE(trace_str);
#endif
	return(validity);
}
