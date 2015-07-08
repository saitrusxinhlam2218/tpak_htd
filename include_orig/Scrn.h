/*
 *                 RCS INFO
 *
 *  @(#)  $RCSfile: Scrn.h,v $
 *  @(#)  $Revision: 1.4 $
 *  @(#)  $Date: 2003/01/28 16:53:50 $
 *  @(#)  $Author: jwelch $
 *  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Scrn.h,v $
 *
 *  Copyright (c) 1992 - Motorola Mobile Data Division, Bothell, WA
 *
 */
#ident "@(#) head:$RCSfile: Scrn.h,v $	$Revision: 1.4 $"

#ifndef _SCRN_H_
#define _SCRN_H_
#ifndef _SCROLLWIN
#include <cursesX.h>  
#endif  // #ifndef _SCROLLWIN
/* declare structure for saving field values */
struct scrn_flds
{
  int field_id;
  int row;		/* row on which prompt appears */
  int col;		/* col on which prompt appears */
  int max_chars;	/* maximum chars allowed on a field */
  int flags;		/* Current flags: EXIT_AT_END, UPPER, CLEAR_ON_ENTRY,
			   DONT_DISPLAY_ZERO, SINGLE_CHAR */
  char *format;	/* String format for printing (if inited
		   to NULL then it will be automatically
		   created) */
  int type;		/* CHAR_TYPE, STRING_TYPE, INT_TYPE, SHORT_TYPE, FLOAT_TYPE */
  int justification;	/* LEFT or RIGHT for justification
			   when printing field */
  int default_field_use;/* ENTRY, DISPLAY_ONLY, SILENT or OFF */
  int field_use;	/* current field use */
  int jump_field_id;	/* Field to jump to on TAB entry */
  char *descrip;	/* The field description */
  void *data;		/* Pointer to structure holding data */
  RET_STATUS (*check_func)();/* Pointer to verification function */
};
typedef struct scrn_flds FIELD;


struct screen_def
{
  int num_fields;
  int starting_field_num;
  int curr_field_num;
  int name;
  WINDOW *window;
  RET_STATUS (*background_text_func)();
  RET_STATUS (*init_func)();
  RET_STATUS (*pre_key_func)();
  RET_STATUS (*post_key_func)();
  RET_STATUS (*exit_func)();
  FIELD *field;
  void *record;
  RET_STATUS (*post_init_func)();
};

typedef struct screen_def SCRN;


/* For type */
#define CHAR_TYPE	1
#define STRING_TYPE	2
#define INT_TYPE	3
#define SHORT_TYPE	4
#define FLOAT_TYPE	5

/* For justification */
#define RIGHT		1
#define LEFT		2	

/* For field use */
#define ENTRY		1	/* User can enter data in field */
#define DISPLAY_ONLY	2	/* User cannot enter data in field */
#define SILENT		3	/* User can enter data, but it is not
				   shown in field */
#define TURNED_OFF	4	/* Field is not displayed or entered */

                                   

/* Flags for field that are ORed together */
#define EXIT_AT_END	0x01	/* Field exits at last char in field */
#define UPPER		0x02	/* All entry to field is in upper case */
#define CLEAR_ON_ENTRY	0x04	/* Field is cleared on first char entered */
#define DONT_DISPLAY_ZERO	0x08	/* Int/short/float fields of zero displayed
					   blank */
#define SINGLE_CHAR	0x10	/* Only single character input */
#define START_AT_END	0x20	/* Data entry is started at end of current data */
#define PHONE_FIELD     0x40    /* used to automatically add '.' into phone number */
#define CREDIT_CARD_FIELD 0x80  /* adds a space after every 2 digits  */
#define NO_UNDERLINE    0x100
#define BOLD_TEXT       0x200       /* print the field in bold text */
#define REVERSE_TEXT    0x400

/* Maximum length of string format "%-0123d" */
#define FORMAT_LEN	7

#define CURR_FIELD_ID(screen)	((screen)->field[(screen)->curr_field_num].field_id)
#define FIELD_ID(screen, field_num)	((screen)->field[(field_num)].field_id)
#define CURR_FIELD_DATA(screen) ((screen)->field[(screen)->curr_field_num].data)
#define FIELD_DATA(screen, field_num) ((screen)->field[(field_num)].data)

#define SCRN_NOT_INITIALIZED(screen) ((screen)->window == NULL)

#define ERROR_ROW	23
#define READ_BUF_LEN	132

#define FATAL()		{fprintf(stderr, "Abort in %s at line %d\n", __FILE__, __LINE__);abort();}

/* For field transversal */
#define NEXT_FIELD	1000
#define PREV_FIELD	1001


#define	SCRN_CW_MAX_STRING_LEN	124
#define	SCRN_CW_MAX_ROWS		4
#define	SCRN_BAD_ARG			err_encode(SCR_OBJ, INVALID_ARG)
#define	SCRN_MALLOC			err_encode(SCR_OBJ, NO_MEMORY)

 
typedef struct screen_choice 
{
        char            string[SCRN_CW_MAX_STRING_LEN + 1];
	int             noncharge_type;
        HNDL            data;
}       SCRN_CHOICE;
 
 
typedef struct screen_choice_list 
{
        HNDL                    data;
        int                     display_len,
                                max_choices,
                                nbr_choices,
	                        noncharge_list_flag;
        SCRN_CHOICE           choices[SCRN_CW_MAX_ROWS];
}       SCRN_CHOICE_LIST, *SCRN_CHOICE_LIST_HANDLE;

#define CHOICE_SCREEN			200   
#define CHOICE_FILTER_SCREEN            300

/* Scrn_choice.c */
RET_STATUS Scrn_choice ARGS((int max_string_len, RET_STATUS (*next_page )(), RET_STATUS (*prev_page )(), RET_STATUS (*clean_up )(), HNDL user_data_handle, HNDL *choice));

/* Scrn_choice_alloc.c */
RET_STATUS Scrn_choice_alloc ARGS((int max_string_len, SCRN_CHOICE_LIST **choice_list));

/* Scrn_choice_free.c */
RET_STATUS Scrn_choice_free ARGS((SCRN_CHOICE_LIST **choice_list));

/* Scrn_choice_window.c */
RET_STATUS Scrn_choice_window ARGS((SCRN_CHOICE_LIST *choice_list, RET_STATUS (*next_page )(), RET_STATUS (*prev_page )(), HNDL *choice, int entry_required));

extern void Scrn_update_field_num(SCRN *, int);
extern RET_STATUS Scrn_get_field_num_from_id(SCRN *, int *, int);
extern void Scrn_print_field_by_num(SCRN *, int);
extern void Scrn_print_current_field(SCRN *);
extern RET_STATUS Scrn_print_field_by_id(SCRN *, int);
extern void Scrn_highlight_field_by_num(SCRN *, int, int);
extern void Scrn_highlight_current_field(SCRN *, int);
extern RET_STATUS highlight_field_by_id(SCRN *, int, int);
extern void Scrn_clear_field_by_num(SCRN *, int);
extern void Scrn_clear_current_field(SCRN *);
extern RET_STATUS Scrn_clear_field_by_id(SCRN *, int);
extern void Scrn_print_all_fields(SCRN *);
extern void Scrn_clear_all_fields(SCRN *);
extern void Scrn_init_screen(SCRN *, WINDOW *);
extern RET_STATUS Scrn_init_field(SCRN *, int, void *, void *);
extern RET_STATUS Scrn_init_char_field(SCRN *, int, char);
extern RET_STATUS Scrn_init_string_field(SCRN *, int, char *);
extern RET_STATUS Scrn_init_int_field(SCRN *, int, int);
extern RET_STATUS Scrn_init_short_field(SCRN *, int, short);
extern RET_STATUS Scrn_init_float_field(SCRN *, int, float);
extern int Scrn_process(SCRN *);
extern int Scrn_direction(int);
extern void Scrn_set_current_field_num(SCRN *, int);
extern void Scrn_center();
extern void Scrn_left();
extern void Scrn_clear_lines(WINDOW *, int, int);
extern void Scrn_clear();

#ifdef SCREEN_SRC
BOOLEAN Scrn_error_displayed = FALSE;
BOOLEAN Scrn_started = FALSE;
#else
extern BOOLEAN Scrn_error_displayed;
BOOLEAN Scrn_started;
#endif

#define       SCRN_NO_CHOICES                 err_encode(SCR_OBJ, 101)
#define       SCRN_FILTER_CHOICES             err_encode(SCR_OBJ, 102)
#endif 
