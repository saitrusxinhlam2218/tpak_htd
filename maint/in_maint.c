static char sz__FILE__[]="@(#)in_maint.c	10.1(ver) Date Release 12/2/94" ;
/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  in_maint.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:48:02
* @(#)  Last delta: 12/2/94 at 11:16:44
* @(#)  SCCS File: /taxi/sccs/s.in_maint.c
*                                                                           *
* Copyright (c) 1990 - MDI Seattle                                          *
*                                                                           *
*****************************************************************************/

/* Make sure location constants are defined. Create a compile error if they
   are not defined or both are defined. */
#ifdef SCANDINAVIA
#ifdef NAMERICA
ERROR: "Both compile time constants SCANDINAVIA and NAMERICA are defined."
#endif
#else
#ifdef NAMERICA
#else
ERROR: Compile time constant SCANDINAVIA or NAMERICA undefined.
#endif
#endif

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <isam.h>
#include <cursesX.h>
#include <signal.h>
#include <setjmp.h>
#include <ctype.h>

#define DECLARING
#define IN_MAINT_SRC
#include "comm_strdefs.h"
#include "in_m_strdefs.h"
#include "mad_error.h"
#include "mads_types.h"
#include "key.h"
#include "in_maint.h"
#include "in_msg.h"
#include "path.h"
#include "language.h"
#include "tokenize.h"


/*  Declare an array of pointers to functions which check the field entered
 *  for validity.  
 */

/* Externs for this file only */
int Displaying_a_record;
int Record_num;
int Prev_next_allowed;

void init();
void cleanup();
void print_query_header();
void print_add_header();
void print_screenx();
void process_interupt();
void process_next();
void process_previous();
void process_add();
void process_remove();
void print_header();
void print_field_names();
char read_request();
char prompt_confirm_of_remove();
void prompt_for_file_name();
char *prompt_for_file_mode();
void print_update_header();
void do_input();
void print_error();
void zero_in();

/* This is to keep compilation happy. It's not used by
   anything here */
struct offsets *offset;

/*****************************************************************************
*  in_maint - main for the intersection file maintenance program.
*******************************************************************************/


main()
{
  int  rc;
  char request;		/*  User request : Q, N, P, A, U, R, O, or E */



  init();
  
  print_header();
  print_field_names();

  /*  Get the first record from the intersection file */
  zero_in(&In);
  rc = db(INT_FILE_ID, READ_KEYS, &in_key1, ISGTEQ, 0);
  if (rc < 0)
    {
      print_error(catgets(IN_MAINT_m_catd, IN_MAINT_SET, IN_MAINT_3, "No record found, ADD first!"));
      Prev_next_allowed = 0;
    }
  else
    {
      
      /*  Display the data from the first record */
      memcpy((char *)&In, req_buf, sizeof(struct cisam_in));
      print_data();
    
      /* save record number for future read */
      Record_num = isrecnum;
      
      Displaying_a_record = 1;
      Prev_next_allowed = 1;
    }

  /*  Input the users request (Query, Next, Previous, Add, Update,
      Remove, Output, Exit).  					*/
  
  while (request = read_request())
    {
      clear_error_line();
      request = Toupper(request);
      switch(request)  {
      case M_QUERY:
	process_query();
	break;
	
      case M_NEXT:
	process_next();
	break;
	
      case M_PREVIOUS:
	process_previous();
	break;
	
      case M_ADD:
	process_add();
	break;
	
      case M_UPDATE:
	process_update();
	break;
	
      case M_REMOVE:
	process_remove();
	break;
	
      case M_OUTPUT:
	process_output();
	break;
	
      case M_EXIT:
	cleanup();
	break;
	
      default:
	cleanup();
	break;
      } /* switch */
      
    } /* while */
  
  cleanup();

}  /* End of main */

void
fill_and_print_in()
{
  Displaying_a_record = 1;
  memcpy((char *)&In, req_buf, sizeof(struct cisam_in));
  print_data();  		
  print_header();
}

int
search_by_index(index)
     int index;
{
  int  rc;
  struct cisam_in *in_ptr;

  in_ptr = (struct cisam_in *) req_buf;
  zero_in(in_ptr);
  in_ptr->index1 = index;
  rc = db(INT_FILE_ID, READ_KEYS, &in_key1, ISGTEQ, 0);
  if (rc < 0)
    {
      if (iserrno == ENOREC)
	{
	  print_error(NO_ROWS);
	  clear_all_fields();
	  Displaying_a_record = 0;
	  print_header();
	  return(-1);
	}
      else
	{
	  print_error(READ_INTERST_ERR);
	  sleep(3);
	  ERROR(' ',INTERST_FILE,READ_INTERST_ERR);
	  cleanup();
	}
    }
  Record_num = isrecnum;
  return(0);
}

int
find_by_both_indexes(index1, index2, action)
     int index1, index2;
     int action;
{
  int  rc;
  struct cisam_in *in_ptr;

  if (action != ISGTEQ && action != ISEQUAL)
    return(-1);

  in_ptr = (struct cisam_in *) req_buf;
  zero_in(in_ptr);
  in_ptr->index1 = index1;
  in_ptr->index2 = index2;
  rc = db(INT_FILE_ID, READ_KEYS, &in_key1, action, 0);
  if (rc < 0)
    {
      if (iserrno == ENOREC)
	{
	  print_error(NO_ROWS);
	  clear_all_fields();
	  Displaying_a_record = 0;
	  print_header();
	  return(-1);
	}
      else
	{
	  print_error(READ_INTERST_ERR);
	  sleep(3);
	  ERROR(' ',INTERST_FILE,READ_INTERST_ERR);
	  cleanup();
	}
    }
  Record_num = isrecnum;
  return(0);
}

void
zero_in(in_ptr)
     struct cisam_in *in_ptr;
{
  bzero((char *)in_ptr, sizeof(struct cisam_in));
}

/******************************************************************************
* process_query - search for specific record in the intersection file
*******************************************************************************/

process_query()
{
  int field_num;
  int should_clear = 0;
  int entry_error;
  int key_entered;
  int query_by;
  int field_entered;
  int rc;
  char read_buf[BUFFER_SIZE];
  struct cisam_sn *sn_ptr;
  struct cisam_sn sn;


  clear_error_line();
  clear_all_fields();
  print_query_header();

  zero_in(&In);

  /* Initialize indexes to invalid values */
  In.index1 = -1;
  In.index2 = -1;

  query_by = 0;

  /* Process input in index or streetname fields */
  field_num = F_INDEX1;
  entry_error = 0;
  while (1)
    {
      field_desc(field_num);
      Interupts_allowed = 1;
      key_entered = read_field(stdscr, In_scr_flds, field_num, read_buf, &field_entered, &should_clear);

      if (entry_error)
	clear_error_line();
      Interupts_allowed = 0;

      if (Saw_interupt)  {
	Saw_interupt = 0;
	Displaying_a_record = 0;
	clear_all_fields();
	clear_error_line();
	print_header();
	zero_in(&In);
	return;
      }

      switch (field_num)
	{
	case F_INDEX1:
	  if (!is_empty(read_buf))
	    {
	      if (!string_is_number(read_buf))
		{
		  print_error(catgets(IN_MAINT_m_catd, IN_MAINT_SET, IN_MAINT_50, "Must enter a number for index"));
		  entry_error = 1;
		  continue;
		}

	      /* If an index is entered only indexes can be entered for queries */
	      query_by = INDEX;
	      In.index1 = atoi(read_buf);
	      field_num = F_INDEX2;
	      entry_error = 0;
	    }
	  else
	    {
	      In.index1 = -1;
	      In.index2 = -1;
	      query_by = 0;
	      field_num = F_NAME1;
	      entry_error = 0;
	    }
	  break;
		
	case F_INDEX2:
	  if (!is_empty(read_buf))
	    {
	      if (!string_is_number(read_buf))
		{
		  print_error(catgets(IN_MAINT_m_catd, IN_MAINT_SET, IN_MAINT_50, "Must enter a number for index"));
		  entry_error = 1;
		  continue;
		}

	      /* If an index is entered only indexes can be entered for queries */
	      In.index2 = atoi(read_buf);
	      field_num = F_INDEX1;
	      entry_error = 0;
	    }
	  else
	    {
	      In.index2 = -1;
	      field_num = F_INDEX1;
	      clear_field(F_INDEX1);
	      entry_error = 0;
	    }
	  break;
		
	case F_NAME1:
	  entry_error = 0;
	  if (!is_empty(read_buf))
	    {
	      query_by = STREET_NAME;
	      strcpy(In.name1, read_buf);
	      field_num = F_NAME2;
	    }
	  else
	    {
	      query_by = 0;
	      In.name1[0] = '\0';
	      In.name2[0] = '\0';

	      /* Erase street_name 2 */
	      print_field(F_NAME2);
	      field_num = F_INDEX1;
	    }
	  break;
		  
	case F_NAME2:
	  entry_error = 0;
	  if (!is_empty(read_buf))
	    {
	      strcpy(In.name2, read_buf);
	      field_num = F_NAME1;
	    }
	  else
	    {
	      In.name2[0] = '\0';
	      field_num = F_NAME1;
	    }
	  break;
		  
	default:
	  print_error("@Should not get this");
	  break;
	} /* switch */

      if (key_entered == ESC)
	{
	  clear_error_line();
	  break;
	}
	  
    } /* while */

	  
  /* Default to query by index */
  if (query_by == 0)
    {
      query_by = INDEX;
      In.index1 = -1;
      In.index2 = -1;
    }

  Prev_next_allowed = 0;
  if (query_by == INDEX)
    {
      /* Searching by only one index */
      if (In.index2 == -1)
	{
	  rc = search_by_index(In.index1);
	  if (rc == -1)
	    {
	      zero_in(&In);
	      return;
	    }
	  fill_and_print_in();
	  Prev_next_allowed = 1;
	  return;
	}

      /* Searching off two records */
      else
	{
	  rc = search_by_index(In.index1);
	  if (rc == -1)
	    {
	      zero_in(&In);
	      return;
	    }
	  rc = find_by_both_indexes(In.index1, In.index2, ISGTEQ);
	  if (rc == -1)
	    {
	      zero_in(&In);
	      return;
	    }
	  fill_and_print_in();
	  return;
	}
    }

  else if (query_by == STREET_NAME)
    {
      /* Search off one street name */
      if (In.name2[0] == '\0')
	{
	  sn_ptr = (struct cisam_sn *) req_buf;
	  bzero(sn_ptr, sizeof(struct cisam_sn));
	  strcpy(sn_ptr->name, In.name1);
	  rc = db(STRNAME_FILE_ID, READ_KEYS, &sn_key2, ISGTEQ, 0);
	  if (rc < 0)
	    {
	      print_error(catgets(IN_MAINT_m_catd, IN_MAINT_SET, IN_MAINT_45, "Street name is not in database"));
	      zero_in(&In);
	      return;
	    }
	  memcpy(&sn, req_buf, sizeof(struct cisam_sn));
	  rc = search_by_index(sn.index);
	  if (rc == -1)
	    {
	      zero_in(&In);
	      return;
	    }
	  fill_and_print_in();
	  Prev_next_allowed = 1;
	  return;
	}
      
      /* Searching off of both street names */
      else
	{
	  sn_ptr = (struct cisam_sn *) req_buf;
	  bzero(sn_ptr, sizeof(struct cisam_sn));
	  strcpy(sn_ptr->name, In.name1);
	  rc = db(STRNAME_FILE_ID, READ_KEYS, &sn_key2, ISGTEQ, 0);
	  if (rc < 0)
	    {
	      print_error(catgets(IN_MAINT_m_catd, IN_MAINT_SET, IN_MAINT_51, "Street name 1 not in database"));
	      zero_in(&In);
	      return;
	    }
	  memcpy(&sn, req_buf, sizeof(struct cisam_sn));
	  In.index1 = sn.index;
	  rc = search_by_index(sn.index);
	  if (rc == -1)
	    {
	      zero_in(&In);
	      return;
	    }
	  
	  bzero(sn_ptr, sizeof(struct cisam_sn));
	  strcpy(sn_ptr->name, In.name2);
	  rc = db(STRNAME_FILE_ID, READ_KEYS, &sn_key2, ISGTEQ, 0);
	  if (rc < 0)
	    {
	      print_error(catgets(IN_MAINT_m_catd, IN_MAINT_SET, IN_MAINT_52, "Street name 2 not in database"));
	      zero_in(&In);
	      return;
	    }

	  memcpy(&sn, req_buf, sizeof(struct cisam_sn));
	  In.index2 = sn.index;
	  rc = find_by_both_indexes(In.index1, sn.index, ISGTEQ);
	  if (rc == -1)
	    {
	      zero_in(&In);
	      return;
	    }
	  fill_and_print_in();

	} /* else */

    } /* if STREET_NAME */

  else
    print_error("@Should not fall through to here");

}  /* End of process_query()  */

/******************************************************************************
* process_next - read the next record in the intersection file and display it.
*******************************************************************************/
void
process_next()
{
  int  rc;

  clear_error_line();
  if (!Displaying_a_record)
    {
      print_error(NO_CUR_LIST);
      return;
    }

  if (!Prev_next_allowed)
    {
      print_error(catgets(IN_MAINT_m_catd, IN_MAINT_SET, IN_MAINT_53,
			  "Previous and Next not allowed when querying by two indexes or street names"));
      return;
    }

  /*  Call the C-Isam routine to get the next entry.   */
  rc = db(INT_FILE_ID, READ_KEYS, &in_key1, ISNEXT, 0);
  if (rc < 0)
    {
      if (iserrno == EENDFILE)
	{
	  print_error(NO_ROWS_DIR);
	  return;
	}
      else
	{
	  print_error(RD_NXT_INTERST_ERR);
	  sleep(3);
	  ERROR(' ',INTERST_FILE,RD_NXT_INTERST_ERR);
	  cleanup();
	}
    }

  memcpy((char *)&In, req_buf, sizeof(struct cisam_in));

  /* save the record number for future read */
  Record_num = isrecnum;

  print_data();

}  /* End of process_next()  */

/******************************************************************************
* process_previous - read the previous record in the intersection file and display it.
*******************************************************************************/
void
process_previous()
{
  int  rc;
  
  clear_error_line();
  if(!Displaying_a_record)
    {
      print_error(NO_CUR_LIST);
      return;
    }
  
  if (!Prev_next_allowed)
    {
      print_error(catgets(IN_MAINT_m_catd, IN_MAINT_SET, IN_MAINT_53,
			  "Previous and Next not allowed when querying by two indexes or street names"));
      return;
    }

  /* Get the previous entry.  */
  rc = db(INT_FILE_ID, READ_KEYS, &in_key1, ISPREV, 0);
  if (rc < 0)
    {
      if(iserrno == EENDFILE)
	{
	  print_error(NO_ROWS_DIR);
	  return;
	}
      else
	{
	  print_error(RD_PREV_INTERST_ERR);
	  sleep(3);
	  ERROR(' ',INTERST_FILE,RD_PREV_INTERST_ERR);
	  cleanup();
	}
    }

  memcpy((char *)&In, req_buf, sizeof(struct cisam_in));

  /* save the record number for future read */
  Record_num = isrecnum;

  print_data();

}  /* End of process_previous()  */

int
in_struct_is_empty()
{
  if (In.index1 == 0 || In.index2 == 0 ||
      In.name1[0] == '\0' || In.name1[0] == '\0')
    return(1);
  return(0);
}


/******************************************************************************
* process_add - input data fields and add the new record to the intersection file 
*******************************************************************************/
void
process_add()
{
  int  rc;
  int  field_num;

  zero_in(&In);
  clear_error_line();
  clear_all_fields();
  print_add_header();
  print_field_names();
#ifdef NAMERICA
  field_num = F_PRE_DIR1;
#else
  field_num = F_NAME1;
#endif
  do_input(&field_num, &In);
  clear_error_line();
  
  if (Saw_interupt)
    {
      Saw_interupt = 0;
      Displaying_a_record = 0;
      clear_all_fields();
      clear_error_line();
      print_header();
      return;
    }
  
  if (in_struct_is_empty())
    {
      print_error("@Error: empty structure");
      return;
    }

  /* Add the new record.  */
  memcpy(req_buf, (char *) &In, sizeof(struct cisam_in));
  rc = db(INT_FILE_ID, ADD_REC, 0, 0, 0);
  if (rc < 0)
    {
      print_error(ADD_INTERST_ERR);
      print_header();
      return;
    }
  
  Record_num = isrecnum;

  /* Swap the index, streetname, etc, and add again */
  swap_1_and_2((struct cisam_in *) req_buf);
  rc = db(INT_FILE_ID, ADD_REC, 0, 0, 0);
  if (rc < 0)
    {
      print_error(ADD_INTERST_ERR);
      print_header();
      return;
    }

  print_error(catgets(IN_MAINT_m_catd, IN_MAINT_SET, IN_MAINT_54, "Record added"));
  Displaying_a_record = 1;
  print_header();

}  /* End of process_add()  */

/******************************************************************************
* process_update - input data fields and update the intersection file record
*******************************************************************************/

process_update()
{
  int field_num;
  int rc;
  int record_num1;
  int record_num2;
  struct cisam_in orig_in;
  
  clear_error_line();
  if (!Displaying_a_record)
    {
      print_error(NO_CUR_LIST);
      return;
    }
  
  print_update_header();
  

  orig_in = In;

#ifdef NAMERICA
  field_num = F_PRE_DIR1;
#else
  field_num = F_NAME1;
#endif
  do_input(&field_num, &In);
  clear_error_line();

  if (Saw_interupt)
    {
      Saw_interupt = 0;
      Displaying_a_record = 0;
      clear_all_fields();
      clear_error_line();
      print_header();
      return;
    }
  
  if (in_struct_is_empty())
    {
      print_error("@Error: empty structure");
      return;
    }

  /* Save the current record number */
  record_num1 = Record_num;

  /* Find the record with the swapped street names */
  swap_1_and_2(&orig_in);

  rc = find_by_both_indexes(orig_in.index1, orig_in.index2, ISEQUAL);
  if (rc < 0)
    {
      print_error(UPD_INTERST_ERR);
      print_header();
      return;
    }

  /* Save the record number for the matching duplicate record
     which has the index, strname, etc swapped */
  record_num2 = Record_num;

  /* Lock and update record #1. In contains the new data */

  /* Lock record # 1 */
  rc = db(INT_FILE_ID, READ_NUM, &in_key1, ISEQUAL + ISLOCK, record_num1);
  if (rc < 0)
    {
      print_error(LOCK_INTERST_ERR);
      print_header();
      return;
    }
  
  /* Update record # 1 */
  memcpy(req_buf, (char *)&In, sizeof(struct cisam_in));
  rc = db(INT_FILE_ID, UPDATE_REC, 0, 0, 0);
  if (rc < 0)
    {
      rc = db(INT_FILE_ID, RELEASE, 0, 0, 0);
      print_error(UPD_INTERST_ERR);
      print_header();
      return;
    }

  /* Lock and update record #2. The street name data in In must be swapped first */

  swap_1_and_2(&In);
  /* Lock record # 2 */
  rc = db(INT_FILE_ID, READ_NUM, &in_key1, ISEQUAL + ISLOCK, record_num2);
  if (rc < 0)
    {
      print_error(LOCK_INTERST_ERR);
      print_header();
      return;
    }
  
  /* Update record # 2 */
  memcpy(req_buf, (char *)&In, sizeof(struct cisam_in));
  rc = db(INT_FILE_ID, UPDATE_REC, 0, 0, 0);
  if (rc < 0)
    {
      rc = db(INT_FILE_ID, RELEASE, 0, 0, 0);
      print_error(UPD_INTERST_ERR);
      print_header();
      return;
    }


  print_error(REC_UPDATED);

  /* Release the locks */
  rc = db(INT_FILE_ID, RELEASE, 0, 0, 0);
  if (rc < 0)
    {
      print_error(RLS_INTERST_ERR);
      print_header();
      return;
    }
  
  Record_num = isrecnum;
  Displaying_a_record = 1;
  print_header();
  
}  /*  End of process_update()  */

/******************************************************************************
* process_remove - remove the current intersection file record.
*******************************************************************************/
void
process_remove()
{
  char answer;			/* response from user */
  int  rc;


  clear_error_line();
  if (!Displaying_a_record)
    {
      print_error(NO_CUR_LIST);
      return;
    }

  answer = prompt_confirm_of_remove();

  if (answer != YES)
    {
      print_header();
      return;
    }

  /*  If the user responded affirmatively, call C-Isam routine 
      to delete the record.  */

  /* We must read the right record before we remove it */
  rc = db(INT_FILE_ID, READ_NUM, &in_key1, ISEQUAL, Record_num);
  if (rc < 0)
    {
      print_error(DEL_INTERST_ERR);
      print_header();
      return;
    }

  rc = db(INT_FILE_ID, DELETE, 0, 0, 0);
  if (rc < 0)
    {
      print_error(DEL_INTERST_ERR);
      print_header();
      return;
    }

  /* Find the record's duplcate */
  rc = find_by_both_indexes(In.index2, In.index1, ISEQUAL);
  if (rc < 0)
    {
      print_error(DEL_INTERST_ERR);
      print_header();
      return;
    }

  /* We must read the right record before we remove it */
  rc = db(INT_FILE_ID, READ_NUM, &in_key1, ISEQUAL, Record_num);
  if (rc < 0)
    {
      print_error(DEL_INTERST_ERR);
      print_header();
      return;
    }
  
  rc = db(INT_FILE_ID, DELETE, 0, 0, 0);
  if (rc < 0)
    {
      print_error(DEL_INTERST_ERR);
      print_header();
      return;
    }

  zero_in(&In);

  Displaying_a_record = 0;
  clear_all_fields();
  print_header();

}  /* End of process_remove()  */

/******************************************************************************
* process_output - open an output file and call print_screen.
*******************************************************************************/

process_output()
{
  
  FILE *o_file_fp;		/* output file pointer */
  char output_file_name[FILE_NAME_SIZE + 1];
  char *mode;

  
  Interupts_allowed = 1;
  clear_error_line();
  echo();
  prompt_for_file_name(output_file_name);
  if (Saw_interupt)
    {
      noecho();
      Saw_interupt = 0;
      Interupts_allowed = 0;
      print_header();
      return;
    }
  
  mode = prompt_for_file_mode(output_file_name);
  
  noecho();
  if (Saw_interupt)
    {
      Saw_interupt = 0;
      Interupts_allowed = 0;
      print_header();
      return;
    }
  
  o_file_fp = fopen(output_file_name, mode);
  if (o_file_fp == NULL)
    {
      print_error(OPEN_OUT_ERR);
      print_header();
      return;
    }
  
  print_screenx(o_file_fp);
  fclose(o_file_fp);
  print_header();
  
}  /* End of process_output() */

/******************************************************************************
* print_field_names - print the field labels on the screen.
*******************************************************************************/

void print_field_names()
{
  char	 open_brace[2],  close_brace[2];
  
  
  strcpy(open_brace, catgets(IN_MAINT_m_catd, IN_MAINT_SET, IN_MAINT_42, "[" ));
  strcpy(close_brace, catgets(IN_MAINT_m_catd, IN_MAINT_SET, IN_MAINT_43, "]" ));
  
  /* print the screen labels  */
  attrset(A_BOLD);
  mvaddstr(L_INDEX1_ROW,L_INDEX1_COL,catgets(IN_MAINT_m_catd, IN_MAINT_SET, IN_MAINT_5, "Street 1 Index"));
#ifdef NAMERICA
  mvaddstr(L_INDEX1_ROW,L_PRE_DIR1_COL,catgets(IN_MAINT_m_catd, IN_MAINT_SET, IN_MAINT_6, "Pre-direction"));
#endif
  mvaddstr(L_INDEX1_ROW,L_NAME1_COL,catgets(IN_MAINT_m_catd, IN_MAINT_SET, IN_MAINT_7, "Name"));
#ifdef NAMERICA
  mvaddstr(L_INDEX1_ROW,L_TYPE1_COL,catgets(IN_MAINT_m_catd, IN_MAINT_SET, IN_MAINT_8, "Type"));
  mvaddstr(L_INDEX1_ROW,L_POST_DIR1_COL,catgets(IN_MAINT_m_catd, IN_MAINT_SET, IN_MAINT_9, "Post-direction"));
#endif
  attrset(0);
  mvaddstr(L_INDEX1_ROW+1,L_INDEX1_COL+9,open_brace);
  mvaddstr(L_INDEX1_ROW+1,L_INDEX1_COL+28,close_brace);
#ifdef NAMERICA
  mvaddstr(L_INDEX1_ROW+1,L_PRE_DIR1_COL,open_brace);
  mvaddstr(L_INDEX1_ROW+1,L_PRE_DIR1_COL+CHARS_PRE_DIR1+1,close_brace);
#endif
  mvaddstr(L_INDEX1_ROW+1,L_NAME1_COL,open_brace);
  mvaddstr(L_INDEX1_ROW+1,L_NAME1_COL+CHARS_NAME1+1,close_brace);
#ifdef NAMERICA
  mvaddstr(L_INDEX1_ROW+1,L_TYPE1_COL,open_brace);
  mvaddstr(L_INDEX1_ROW+1,L_TYPE1_COL+CHARS_TYPE1+1,close_brace);
  mvaddstr(L_INDEX1_ROW+1,L_POST_DIR1_COL,open_brace);
  mvaddstr(L_INDEX1_ROW+1,L_POST_DIR1_COL+CHARS_POST_DIR1+1,close_brace);
#endif
  attrset(A_BOLD);
  mvaddstr(L_INDEX2_ROW,L_INDEX2_COL,catgets(IN_MAINT_m_catd, IN_MAINT_SET, IN_MAINT_10, "Street 2 Index"));
#ifdef NAMERICA
  mvaddstr(L_INDEX2_ROW,L_PRE_DIR2_COL,catgets(IN_MAINT_m_catd, IN_MAINT_SET, IN_MAINT_6, "Pre-direction"));
#endif
  mvaddstr(L_INDEX2_ROW,L_NAME2_COL,catgets(IN_MAINT_m_catd, IN_MAINT_SET, IN_MAINT_7, "Name"));
#ifdef NAMERICA
  mvaddstr(L_INDEX2_ROW,L_TYPE2_COL,catgets(IN_MAINT_m_catd, IN_MAINT_SET, IN_MAINT_8, "Type"));
  mvaddstr(L_INDEX2_ROW,L_POST_DIR2_COL,catgets(IN_MAINT_m_catd, IN_MAINT_SET, IN_MAINT_9, "Post-direction"));
#endif
  attrset(0);
  mvaddstr(L_INDEX2_ROW+1,L_INDEX2_COL+9,open_brace);
  mvaddstr(L_INDEX2_ROW+1,L_INDEX2_COL+28,close_brace);
#ifdef NAMERICA
  mvaddstr(L_INDEX2_ROW+1,L_PRE_DIR2_COL,open_brace);
  mvaddstr(L_INDEX2_ROW+1,L_PRE_DIR2_COL+CHARS_PRE_DIR2+1,close_brace);
#endif
  mvaddstr(L_INDEX2_ROW+1,L_NAME2_COL,open_brace);
  mvaddstr(L_INDEX2_ROW+1,L_NAME2_COL+CHARS_NAME2+1,close_brace);
  attrset(A_BOLD);
#ifdef NAMERICA
  mvaddstr(L_INDEX2_ROW+1,L_TYPE2_COL,open_brace);
  mvaddstr(L_INDEX2_ROW+1,L_TYPE2_COL+CHARS_TYPE2+1,close_brace);
  mvaddstr(L_INDEX2_ROW+1,L_POST_DIR2_COL,open_brace);
  mvaddstr(L_INDEX2_ROW+1,L_POST_DIR2_COL+CHARS_POST_DIR1+2,close_brace);
  mvaddstr(L_ZIP_ROW,L_ZIP_COL,catgets(IN_MAINT_m_catd, IN_MAINT_SET, IN_MAINT_11, "Zip Code"));
#endif
  mvaddstr(L_ZIP_ROW,L_SUBZONE_COL,catgets(IN_MAINT_m_catd, IN_MAINT_SET, IN_MAINT_12, "Subzone"));
  mvaddstr(L_ZIP_ROW,L_CITY_COL,catgets(IN_MAINT_m_catd, IN_MAINT_SET, IN_MAINT_13, "City"));
  mvaddstr(L_ZIP_ROW,L_X_COORD_COL,catgets(IN_MAINT_m_catd, IN_MAINT_SET, IN_MAINT_14, "X-coord"));
  mvaddstr(L_ZIP_ROW,L_Y_COORD_COL,catgets(IN_MAINT_m_catd, IN_MAINT_SET, IN_MAINT_15, "Y-coord"));
  attrset(0);
#ifdef NAMERICA
  mvaddstr(L_ZIP_ROW+1,L_ZIP_COL,open_brace);
  mvaddstr(L_ZIP_ROW+1,L_ZIP_COL+CHARS_ZIP+1,close_brace);
#endif
  mvaddstr(L_ZIP_ROW+1,L_SUBZONE_COL,open_brace);
  mvaddstr(L_ZIP_ROW+1,L_SUBZONE_COL+CHARS_SUBZONE+1,close_brace);
  mvaddstr(L_ZIP_ROW+1,L_CITY_COL,open_brace);
  mvaddstr(L_ZIP_ROW+1,L_CITY_COL+CHARS_CITY+1,close_brace);
  mvaddstr(L_ZIP_ROW+1,L_X_COORD_COL,open_brace);
  mvaddstr(L_ZIP_ROW+1,L_X_COORD_COL+CHARS_X_COORD+1,close_brace);
  mvaddstr(L_ZIP_ROW+1,L_Y_COORD_COL,open_brace);
  mvaddstr(L_ZIP_ROW+1,L_Y_COORD_COL+CHARS_Y_COORD+1,close_brace);
  attrset(A_BOLD);
  mvaddstr(L_MAP_PAGE_ROW,L_MAP_PAGE_COL,catgets(IN_MAINT_m_catd, IN_MAINT_SET, IN_MAINT_16, "Map Ref Page"));
  mvaddstr(L_MAP_PAGE_ROW,L_MAP_BLK_COL,catgets(IN_MAINT_m_catd, IN_MAINT_SET, IN_MAINT_17, "Map Ref Block"));
  mvaddstr(L_MAP_PAGE_ROW,L_CMT_COL,catgets(IN_MAINT_m_catd, IN_MAINT_SET, IN_MAINT_18, "Comments"));
  attrset(0);
  mvaddstr(L_MAP_PAGE_ROW+1,L_MAP_PAGE_COL,open_brace);
  mvaddstr(L_MAP_PAGE_ROW+1,L_MAP_PAGE_COL+CHARS_MAP_PAGE+1,close_brace);
  mvaddstr(L_MAP_PAGE_ROW+1,L_MAP_BLK_COL,open_brace);
  mvaddstr(L_MAP_PAGE_ROW+1,L_MAP_BLK_COL+CHARS_MAP_BLK+1,close_brace);
  mvaddstr(L_MAP_PAGE_ROW+1,L_CMT_COL,open_brace);
  mvaddstr(L_MAP_PAGE_ROW+1,L_CMT_COL+CHARS_CMT+1,close_brace);
  refresh();
  

}  /* End of print_field_names  */

/******************************************************************************
* print_data - print the data on the screen.
*******************************************************************************/

print_data()
{
  int  i;
  
  /* Fill in the data items on the screen */
  
  for (i = 0; i < Num_fields; i ++)
    {
#ifdef SCANDINAVIA
      /* Skip over non-Scandinavian fields */
      if (i != F_PRE_DIR1 &&
	  i != F_TYPE1 &&
	  i != F_POST_DIR1 &&
	  i != F_PRE_DIR2 &&
	  i != F_TYPE2 &&
	  i != F_POST_DIR2 &&
	  i != F_ZIP)
#endif
	print_field(i);
    }

  refresh();

}  /*  End of print_data  */


/******************************************************************************
 * field_desc - key descriptions 
*******************************************************************************/
field_desc(field_num)
     int   field_num;
{
  move(ERROR_ROW - 1, ERROR_COL);
  clrtoeol();
  mvaddstr(ERROR_ROW-1, ERROR_COL, In_scr_flds[field_num].descrip);

}

/******************************************************************************
 * swap_1_and_2
*******************************************************************************/
swap_1_and_2(in_ptr)
     struct cisam_in *in_ptr;
{
  struct cisam_in int_temp;

  int_temp = *in_ptr;
  
  in_ptr->index1 = in_ptr->index2;
  in_ptr->index2 = int_temp.index1;
  
  strcpy(in_ptr->pre_dir1, in_ptr->pre_dir2);
  strcpy(in_ptr->pre_dir2, int_temp.pre_dir1);
  
  strcpy(in_ptr->name1, in_ptr->name2);
  strcpy(in_ptr->name2, int_temp.name1);
  
  strcpy(in_ptr->type1, in_ptr->type2);
  strcpy(in_ptr->type2, int_temp.type1);
  
  strcpy(in_ptr->post_dir1, in_ptr->post_dir2);
  strcpy(in_ptr->post_dir2, int_temp.post_dir1);
}

/******************************************************************************
* print_screen - read the screen and output characters to a file.
*******************************************************************************/
void
print_screenx(o_file_fp)
     FILE *o_file_fp;
{
  int  col, line;
  char c;

  col = 0;
  for (line = 0; line < LINES ; line++)
    {
      for (col = 0; col < COLS ;col++)
	{
	  move(line, col);
	  c = inch() & A_CHARTEXT;
	  fputc(c, o_file_fp);	
	}
      if (c != '\n')
	fputc('\n', o_file_fp);
    }

}  /*  End of print_screen() */



/******************************************************************************
* read_request - read users request for options.
*******************************************************************************/
char
read_request()
{
  /*  The main header, the labels and the data from the first
   *  record in the intersection file have been displayed.  At this
   *  point the user must request one of the eight options,
   *  they are : Q, q, N, n, P, p, A, a, U, u, R, r, O, o, E, and e.
   */
  
  int c;		/* For reading characters */
  
  
  move(REQ_ROW,REQ_COL);
  refresh();
  
  while(c = getkey())
    {
      c = Toupper(c);
      if (c == M_QUERY ||
	  c == M_NEXT ||
	  c == M_PREVIOUS ||
	  c == M_ADD ||
	  c == M_UPDATE ||
	  c == M_REMOVE ||
	  c == M_OUTPUT ||
	  c == M_EXIT)
	break;
      else
	{
	  /* ^L for redrawing screen */
	  if (c == '\014')
	    wrefresh(curscr);
	  else
	    {
	      move(REQ_ROW, REQ_COL);
	      beep();
	      refresh();
	    }
	}
    }
  
  return(c);

} /*  End of read_request() */

/******************************************************************************
* print_query_header - print the header line for the query option.
*******************************************************************************/
void
print_query_header()
{
  clear_header_line();

  /* Print the Query screen header */
  mvprintw(HDR_ROW_1,HDR_COL_1,"%s. %s", QUERY_HDR,
	   catgets(IN_MAINT_m_catd, IN_MAINT_SET, IN_MAINT_55, "Can search only by indexes OR by street names"));
  refresh();

  clear_all_fields();

}  /*  End of print_query_header()  */

/******************************************************************************
* print_add_header - print the header line for the add option.
*******************************************************************************/
void
print_add_header()
{
  clear_header_line();
  mvaddstr(HDR_ROW_1,HDR_COL_1,ADD_HDR);
  refresh();
  clear_all_fields();

}  /* End of print_add_header() */

/******************************************************************************
* print_header - print the main screen header.
*******************************************************************************/

void
print_header()
{
  clear_header_line();
  mvaddstr(HDR_ROW_1,HDR_COL_1,MAIN_HDR);
  attrset(A_BOLD);
  mvaddstr(HDR_ROW_2,HDR_COL_2,INTERST_TITLE);
  attrset(0);
  refresh();

}  /* End of print_header()  */

/******************************************************************************
* print_update_header - print the header line for the indate option.
*******************************************************************************/

void
print_update_header()
{
  clear_header_line();
  mvaddstr(HDR_ROW_1,HDR_COL_1,UPDATE_HDR);
  refresh();

}  /*  End of prt_ind_hdr() */

/******************************************************************************
* prompt_confirm_of_remove - confirm that the user wants to remove this record.
*******************************************************************************/

char
prompt_confirm_of_remove()
{
  int c;

  clear_header_line();
  mvaddstr(DEL_ROW, DEL_COL, DELETE_HDR);
  refresh();
  
  while (c = getkey())
    {
      c = Toupper(c);
      if (c == YES || c == NO) 
	break;
      else
	beep();
    }
  
  return(c);

}  /*  End of prompt_confirm_of_remove()  */

/******************************************************************************
* prompt_for_file_name - prompt the user for a file name to output the screen image to.
*******************************************************************************/
void
prompt_for_file_name(output_file_name)
     char *output_file_name;
{
  clear_header_line();
  mvaddstr(HDR_ROW_1, HDR_COL_1, INTERST_FNAME_PROMPT);
  refresh();
  fgets(output_file_name, FILE_NAME_SIZE, stdin);
  if (output_file_name[0] == '\n' || output_file_name[0] == '\0')
    strcpy(output_file_name, "in_scr.out");

}  /*  End of prompt_for_file_name()  */

/******************************************************************************
* prompt_for_file_mode - prompt the user for file mode Create or Append.
*******************************************************************************/
char *
prompt_for_file_mode(output_file_name)
     char *output_file_name;
{
  int have_valid_response;
  static char mode[3];


  have_valid_response = 0;
  while (!have_valid_response)
    {
      clear_header_line();
      mvprintw(HDR_ROW_1, HDR_COL_1, FMODE_PROMPT, output_file_name);
      
      fgets(mode, 2, stdin);
      /*
       *      default to create
       */
      if (mode[0] == '\0')
	mode[0] = M_CREATE;
      
      mode[0] = Toupper(mode[0]);
      
      switch (mode[0])
	{
	case M_APPEND:
	  strcpy(mode, "a");
	  have_valid_response = 1;
	  break;

	case M_CREATE:
	  strcpy(mode, "w");
	  have_valid_response = 1;
	  break;

	default:
	  print_error(catgets(IN_MAINT_m_catd, IN_MAINT_SET, IN_MAINT_44, 
			      "Invalid choice - Please enter (A/C) append or create"));
	  break;
	}
    }
  
  clear_error_line();

}  /*  End of prompt_for_file_mode()  */


/******************************************************************************
* print_error - print error message.
*******************************************************************************/
void
print_error(err_msg)
     char *err_msg;
{
  static long last_error_time = 0;

  /* Wait at least one second between errors */
  if (time(0) - last_error_time <= 1)
    sleep(1);

  beep();

  attrset(A_REVERSE);

  mvprintw(ERROR_ROW,ERROR_COL," %s ",err_msg);
  clrtoeol();
  refresh();

  attrset(0);
  last_error_time = time(0);
}  /*  End of print_error()  */


/******************************************************************************
* init - open files, initialize variables, setin signals, prepare curses.
*******************************************************************************/
void
init()
{
  int  rc;
  char nlspath[80];
  static char put_nlspath[80];
  int  putenv();    
  
  /*
   * Set up for internationalization by setting the locale and opening the catalog
   */
  strcpy(nlspath, "/usr/taxi/catalogs/finnish/%N.cat");
  sprintf(put_nlspath, "NLSPATH=%s", nlspath);
  putenv(put_nlspath);	  
  setlocale(LC_ALL, "");
  open_catalog(&COMMON_m_catd, "common", 0, 0);
  open_catalog(&IN_MAINT_m_catd, "in_maint", 0, 0);

  Interupts_allowed = 0;
  Saw_interupt = 0;
  Displaying_a_record = 0;
  initscr();
  noecho();

  term_setup(getenv("TERM"));
  config_tty();
  
  cbreak();
  
  signal(SIGQUIT, SIG_IGN);
  signal(SIGINT, process_interupt);
  signal(SIGTERM, cleanup);
  signal(SIGHUP, cleanup);
  signal(SIGPIPE, cleanup);
  signal(SIGTSTP, SIG_IGN);
  
  rc = db(INT_FILE_ID, OPEN, 0, ISINOUT + ISMANULOCK, 0);
  if (rc < 0)
    {
      print_error(INTERST_OPEN_ERR);
      sleep(3);
      ERROR(' ',INTERST_FILE,INTERST_OPEN_ERR);
      cleanup();
    }

  rc = db(STRNAME_FILE_ID, OPEN, 0, ISINPUT + ISMANULOCK, 0);
  if (rc < 0)
    {
      print_error(OPEN_ST_ERR);
      sleep(3);
      ERROR(' ',STRNAME_FILE,OPEN_ST_ERR);
      cleanup();
    }

  rc = db(STRTYPE_FILE_ID, OPEN, 0, ISINPUT + ISMANULOCK, 0);
  if (rc < 0)
    {
      print_error(OPEN_ST_ERR);
      sleep(3);
      ERROR(' ',STRNAME_FILE,OPEN_ST_ERR);
      cleanup();
    }

  rc = db(SUBZONE_FILE_ID, OPEN, 0, ISINPUT + ISMANULOCK, 0);
  if (rc < 0)
    {
      print_error(OPEN_SUBZ_ERR);
      sleep(3);
      ERROR(' ',STRNAME_FILE,OPEN_SUBZ_ERR);
      cleanup();
    }

  rc = db(LOCALTY_FILE_ID, OPEN, 0, ISINPUT + ISMANULOCK, 0);
  if (rc < 0)
    {
      print_error(OPEN_LO_ERR);
      sleep(3);
      ERROR(' ',STRNAME_FILE,OPEN_LO_ERR);
      cleanup();
    }

  zero_in(&In);

  init_fields();

}  /*  End of init()  */

/******************************************************************************
* cleanup - if an error has occured, close files and terminate curses.
*******************************************************************************/
void
cleanup()
{
  refresh();
  endwin();

  catclose(COMMON_m_catd);
  catclose(IN_MAINT_m_catd);

  db(INT_FILE_ID, CLOSE, 0, 0, 0);
  db(STRNAME_FILE_ID, CLOSE, 0, 0, 0);
  db(LOCALTY_FILE_ID, CLOSE, 0, 0, 0);
  db(SUBZONE_FILE_ID, CLOSE, 0, 0, 0);

  exit(0);

}  /*  End of cleanup()  */

/******************************************************************************
* Clear the data fields on the screen. 
*******************************************************************************/

clear_all_fields()
{
  int i;

  for(i = 0; i < Num_fields; i++) 
    clear_field(i); 

}

/******************************************************************************
* clear_error_line - clear the error line on the screen.
*******************************************************************************/

clear_error_line()
{
  move(ERROR_ROW - 1, ERROR_COL);
  clrtoeol();
  refresh();  
  move(ERROR_ROW,ERROR_COL);
  clrtoeol();

}

/******************************************************************************
* clear_header_line - clear the header line on the screen.
*******************************************************************************/

clear_header_line()
{
  move(HDR_ROW_1,HDR_COL_1);
  clrtoeol();
  refresh();
}

/******************************************************************************
* process_interupt - process the interrupt signal.
*******************************************************************************/
void
process_interupt()
{
  /* This routine is called when an interrupt signal
     is received.  A flag is set to indicate that this
     signal was received. */

  signal(SIGINT, process_interupt);
  if (Interupts_allowed)
    {
      Saw_interupt = 1;
    }

  if (Do_jump)
    {
      Do_jump = 0;
      longjmp(Jump_env, 1);
    }
}

void
do_input(field_num, in_ptr)
     int *field_num;
     struct cisam_in *in_ptr;
{
  int should_clear;
  int entry_error;
  int key_entered;
  int rc;
  int street_index;
  int field_entered;
  char read_buf[BUFFER_SIZE];
  

  should_clear = 0;
  entry_error = 0;

enter_data:  

  while (1)
    {
      field_desc(*field_num);
      Interupts_allowed = 1;
      
      key_entered = read_field(stdscr, In_scr_flds, *field_num, read_buf, &field_entered, &should_clear); 

      Interupts_allowed = 0;
      if (Saw_interupt)
	return;
  
      /* Erase previous error message */
      if (entry_error)
	clear_error_line();

      rc = (In_scr_flds[*field_num].check_func)(*field_num, read_buf, field_entered, in_ptr);

      if (rc == VALID)
	{
	  print_field(*field_num);

	  if (*field_num == F_NAME1 || *field_num == F_TYPE1)
	    {
	      street_index = get_street_index(in_ptr->name1, in_ptr->type1);
	      if (street_index == -1 && 
		  /* In NAMERICA don't give error when stepping from name to type if
		     no index found yet */
#ifdef NAMERICA
		  *field_num != F_NAME1
#else
		  1
#endif
		  )
		{
		  print_error(catgets(IN_MAINT_m_catd, IN_MAINT_SET, IN_MAINT_56,
				      "No matching index for given street name and type"));
		  in_ptr->index1 = 0;
		  update_field_num(field_num, key_entered);
		  continue;
		}
	      if (street_index != -1)
		{
		  in_ptr->index1 = street_index;
		  print_field(F_INDEX1);
		}
	    }
	  else if (*field_num == F_NAME2 || *field_num == F_TYPE2)
	    {
	      street_index = get_street_index(in_ptr->name2, in_ptr->type2);
	      if (street_index == -1 &&
		  /* In NAMERICA don't give error when stepping from name to type if
		     no index found yet */
#ifdef NAMERICA
		  *field_num != F_NAME2
#else
		  1
#endif
		  )
		{
		  print_error(catgets(IN_MAINT_m_catd, IN_MAINT_SET, IN_MAINT_56,
				      "No matching index for given street name and type"));
		  in_ptr->index2 = 0;
		  update_field_num(field_num, key_entered);
		  continue;
		}
	      if (street_index != -1)
		{
		  in_ptr->index2 = street_index;
		  print_field(F_INDEX2);
		}
	    }

	  if (key_entered == ESC)
	    break;

	  update_field_num(field_num, key_entered);
	  entry_error = 0;
	} /* if (rc == VALID) */

      /* rc != VALID */
      else
	entry_error = 1;

    } /* while (1) */

  if (in_ptr->index1 == 0)
    {
      print_error("First street is not filled in");
      *field_num = F_NAME1;
      goto enter_data;
    }
  
  if (in_ptr->index2 == 0)
    {
      print_error("Second street is not filled in");
      *field_num = F_NAME2;
      goto enter_data;
    }

  if (is_empty(in_ptr->subzone))
    {
      print_error("Subzone field is empty");
      *field_num = F_SUBZONE;
      goto enter_data;
    }

  if (is_empty(in_ptr->city))
    {
      print_error("City field is empty");
      *field_num = F_CITY;
      goto enter_data;
    }

}

/* Return 1 if string is all spaces or empty, 0 otherwise */
int
is_empty(str)
     char *str;
{
  while (*str)
    {
      if (!isspace(*str))
	return(0);
      str ++;
    }
  
  return(1);
}
