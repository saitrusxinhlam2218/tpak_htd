/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Scrn_choice.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1998/05/12 19:20:42 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Scrn_choice.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ifndef Scrn_choice_H
#define Scrn_choice_H
#define	SCRN_CW_MAX_STRING_LEN	124
#define	SCRN_CW_MAX_ROWS		4
#define	SCRN_BAD_ARG			err_encode(SCRN_OBJECT_ID, INVALID_ARG)
#define	SCRN_MALLOC			err_encode(SCRN_OBJECT_ID, NO_MEMORY)
 
typedef struct screen_choice 
{
        char            string[SCRN_CW_MAX_STRING_LEN + 1];
        HNDL            data;
}       SCRN_CHOICE;
 
 
typedef struct screen_choice_list 
{
        HNDL                    data;
        int                     display_len,
                                max_choices,
                                nbr_choices;
        SCRN_CHOICE           choices[SCRN_CW_MAX_ROWS];
}       SCRN_CHOICE_LIST, *SCRN_CHOICE_LIST_HANDLE;
#define CHOICE_SCREEN			200   /* 
					       * Do not know what this 
				               * should be TEK
					       */	
/* Scrn_choice.c */
RET_STATUS Scrn_choice ARGS((int max_string_len, RET_STATUS (*next_page )(), RET_STATUS (*prev_page )(), RET_STATUS (*clean_up )(), HNDL user_data_handle, HNDL *choice));
/* Scrn_choice_alloc.c */
RET_STATUS Scrn_choice_alloc ARGS((int max_string_len, SCRN_CHOICE_LIST **choice_list));
/* Scrn_choice_free.c */
RET_STATUS Scrn_choice_free ARGS((SCRN_CHOICE_LIST **choice_list));
/* Scrn_choice_window.c */
RET_STATUS Scrn_choice_window ARGS((SCRN_CHOICE_LIST *choice_list, RET_STATUS (*next_page )(), RET_STATUS (*prev_page )(), HNDL *choice));
#endif	// Scrn_choice_H
