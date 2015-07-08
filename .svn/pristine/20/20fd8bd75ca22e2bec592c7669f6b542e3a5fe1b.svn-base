static char sz__FILE__[]="@(#)in_cl_fld.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  in_cl_fld.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:48:00
 * @(#)  Last delta: 12/2/94 at 12:01:51
 * @(#)  SCCS File: /taxi/sccs/s.in_cl_fld.c
 *                                                                           *
 *****************************************************************************/

#include <cursesX.h>
#include <setjmp.h>

#include "in_maint.h"
#include "in_m_strdefs.h"

/******************************************************************************
* clear_field - clear a data field on the screen.
*******************************************************************************/
void
clear_field(field_num)
     int field_num;
{
  int i;

  if (field_num < 0 || field_num > Num_fields)
    return;

  /* move the cursor to the 1st char of the field to be cleared */
  move(In_scr_flds[field_num].prompt_row, In_scr_flds[field_num].prompt_col);

  /* clear the field */
  for (i = 0; i < In_scr_flds[field_num].max_chars; i++ )  
    addch(' ');
}

void
print_field(field_num)
     int field_num;
{
  int i;

  if (field_num < 0 || field_num >= Num_fields)
    return;

  if (In_scr_flds[field_num].type == STRING_TYPE)
    mvprintw(In_scr_flds[field_num].prompt_row,
	     In_scr_flds[field_num].prompt_col,
	     In_scr_flds[field_num].format,
	     (char *) In_scr_flds[field_num].data);

  else if (In_scr_flds[field_num].type == INT_TYPE)
    mvprintw(In_scr_flds[field_num].prompt_row,
	     In_scr_flds[field_num].prompt_col,
	     In_scr_flds[field_num].format,
	     *In_scr_flds[field_num].data);

  else if (In_scr_flds[field_num].type == SHORT_TYPE)
    mvprintw(In_scr_flds[field_num].prompt_row,
	     In_scr_flds[field_num].prompt_col,
	     In_scr_flds[field_num].format,
	     *((short *)In_scr_flds[field_num].data));

  else if (In_scr_flds[field_num].type == CHAR_TYPE)
    mvprintw(In_scr_flds[field_num].prompt_row,
	     In_scr_flds[field_num].prompt_col,
	     In_scr_flds[field_num].format,
	     *((char *)In_scr_flds[field_num].data));

}

void
create_field_formats()
{
  int  i;
  char *format;
  char max_chars[FORMAT_LEN];

  for (i = 0; i < Num_fields; i ++)
    {

      format = (char *) malloc(FORMAT_LEN);
      In_scr_flds[i].format = format;

      strcpy(format, "%");

      if (In_scr_flds[i].justification == LEFT)
	strcat(format, "-");
      else if (In_scr_flds[i].justification != RIGHT)
	{
	  fprintf(stderr, "Error in initialization of fields (justification)\n");
	  abort();
	}

  
      sprintf(max_chars, "%d", In_scr_flds[i].max_chars);
      strcat(format, max_chars);

      switch (In_scr_flds[i].type)
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

	default:
	  fprintf(stderr, "Error in initialization of fields (field type)\n");
	  abort();
	}

    }
}

init_field_descrip()
{
  int i;

  for (i = 0; i < Num_fields; i ++)
    {
      switch(i)
	{
	case F_INDEX1:
	case F_INDEX2:
	  In_scr_flds[i].descrip = catgets(IN_MAINT_m_catd, IN_MAINT_SET, IN_MAINT_27, " Index1 from street name file");
	  break;
	  
	case F_PRE_DIR1:
	case F_PRE_DIR2:
	  In_scr_flds[i].descrip = catgets(IN_MAINT_m_catd, IN_MAINT_SET, IN_MAINT_28, "Intersection pre_direction");
	  break;
	  
	case F_NAME1:
	case F_NAME2:
	  In_scr_flds[i].descrip = catgets(IN_MAINT_m_catd, IN_MAINT_SET, IN_MAINT_29, "Intersection street name");
	  break;
	  
	case F_TYPE1:
	case F_TYPE2:
	  In_scr_flds[i].descrip = catgets(IN_MAINT_m_catd, IN_MAINT_SET, IN_MAINT_30, "Intersection street type");
	  break;
	  
	case F_POST_DIR1:
	case F_POST_DIR2:
	  In_scr_flds[i].descrip = catgets(IN_MAINT_m_catd, IN_MAINT_SET, IN_MAINT_31, "Intersection post_direction");
	  break;
	  
	case F_ZIP:
	  In_scr_flds[i].descrip = catgets(IN_MAINT_m_catd, IN_MAINT_SET, IN_MAINT_32, "Intersection zip code");
	  break;
	  
	case F_SUBZONE:
	  In_scr_flds[i].descrip = catgets(IN_MAINT_m_catd, IN_MAINT_SET, IN_MAINT_33, "Intersection subzone.");
	  break;
	  
	case F_CITY:
	  In_scr_flds[i].descrip = catgets(IN_MAINT_m_catd, IN_MAINT_SET, IN_MAINT_34, "Intersection city");
	  break;
	  
	case F_X_COORD:
	  In_scr_flds[i].descrip = catgets(IN_MAINT_m_catd, IN_MAINT_SET, IN_MAINT_35, "X coordinate of the intersection");
	  break;
	  
	case F_Y_COORD:
	  In_scr_flds[i].descrip = catgets(IN_MAINT_m_catd, IN_MAINT_SET, IN_MAINT_36, "Y coordinate of the intersection");
	  break;
	  
	case F_MAP_PAGE:
	  In_scr_flds[i].descrip = catgets(IN_MAINT_m_catd, IN_MAINT_SET, IN_MAINT_37, "Map page reference number");
	  break;
	  
	case F_MAP_BLOCK:
	  In_scr_flds[i].descrip = catgets(IN_MAINT_m_catd, IN_MAINT_SET, IN_MAINT_38, "Map block designation");
	  break;
	  
	case F_COMMENT:
	  In_scr_flds[i].descrip = catgets(IN_MAINT_m_catd, IN_MAINT_SET, IN_MAINT_39, "Intersection location comments");
	  break;

	default:
	  fprintf(stderr, "Error in initialization of field description\n");
	  abort();
	}
    }
}

init_fields()
{
  int  i;

  for (i = 0; In_scr_flds[i].prompt_row != -1; i ++);
  Num_fields = i;

  create_field_formats();
  init_field_descrip();
}
