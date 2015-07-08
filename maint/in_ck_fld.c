static char sz__FILE__[]="@(#)in_ck_fld.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  in_ck_fld.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:47:58
 * @(#)  Last delta: 12/2/94 at 11:13:39
 * @(#)  SCCS File: /taxi/sccs/s.in_ck_fld.c
 *                                                                           *
 *****************************************************************************/

#include <cursesX.h>
#include <ctype.h>
#include <sys/types.h>
#include <setjmp.h>

#include "mad_error.h"
#include "mads_types.h"
#include "mads_isam.h"
#include "in_maint.h"
#include "df_maint.h"
#include "in_m_strdefs.h"

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

extern struct keydesc sn_key2, sz_key1, lo_key1, st_key1;

extern void print_error();
char *str_to_upper();
int check_if_valid_street_name();
int check_if_valid_city();


/*******************************************************************************
 * check_index - system assigned number according to street name file.
 ********************************************************************************/
int
check_index (field_num, read_buf, field_entered, in_ptr)
     int field_num;	/* pointer to the number of the field read in */
     char *read_buf; /* pointer to the buffer in which the field was read in */
     int field_entered; /* indicates if data was entered or not */
     struct cisam_in *in_ptr;
{
  return(VALID);
}



/*******************************************************************************
 * check intersection direction field.
*******************************************************************************/
int
check_dir(field_num, read_buf, field_entered, in_ptr)
     int field_num;	/* pointer to the number of the field read in */
     char *read_buf; /* pointer to the buffer in which the field was read in */
     int field_entered; /* indicates if data was entered or not */
     struct cisam_in *in_ptr;
{
#ifdef NAMERICA
  if (field_num != F_PRE_DIR1 && field_num != F_POST_DIR1 && field_num != F_PRE_DIR2 && field_num != F_POST_DIR2)
    {
      endwin();
      fprintf(stderr, "Fatal error: Wrong field check. Field = %d. File %s, line %d\n", field_num, __FILE__, __LINE__);
      abort();
    }

  if (!is_empty(read_buf) &&
      strcmp(read_buf, "N") != 0 &&
      strcmp(read_buf, "E") != 0 &&
      strcmp(read_buf, "W") != 0 &&
      strcmp(read_buf, "S") != 0 &&
      strcmp(read_buf, "NE") != 0 &&
      strcmp(read_buf, "NW") != 0 &&
      strcmp(read_buf, "SE") != 0 &&
      strcmp(read_buf, "SW") != 0)
    {
      print_error("Invalid direction. Use N, E, W, S, NE, NW, SE, or SW");
      return(NOT_VALID);
    }
  else
    {
      if (field_num == F_PRE_DIR1)
	strcpy(in_ptr->pre_dir1, read_buf);
      if (field_num == F_POST_DIR1)
	strcpy(in_ptr->post_dir1, read_buf);
      if (field_num == F_PRE_DIR2)
	strcpy(in_ptr->pre_dir2, read_buf);
      if (field_num == F_POST_DIR2)
	strcpy(in_ptr->post_dir2, read_buf);
    }
#endif
  return(VALID);
}


/*******************************************************************************
 * check intersection name field.
 ******************************************************************************/
int
check_street_name(field_num, read_buf, field_entered, in_ptr)
     int field_num;	/* pointer to the number of the field read in */
     char *read_buf; /* pointer to the buffer in which the field was read in */
     int field_entered; /* indicates if data was entered or not */
     struct cisam_in *in_ptr;
{
  int  rc;

  if (field_num != F_NAME1 && field_num != F_NAME2)
    {
      endwin();
      fprintf(stderr, "Fatal error: Wrong field check. Field = %d. File %s, line %d\n", field_num, __FILE__, __LINE__);
      abort();
    }

  if (is_empty(read_buf))
    return(VALID);

  str_to_upper(read_buf);

  rc = check_if_valid_street_name(read_buf);
  if (rc == -1)
    {
      print_error(catgets(IN_MAINT_m_catd, IN_MAINT_SET, IN_MAINT_45, "Street name is not in database"));
      return(NOT_VALID);
    }
  
  switch (field_num)
    {
    case F_NAME1:
      strcpy(in_ptr->name1, read_buf);
      break;
      
    case F_NAME2:
      strcpy(in_ptr->name2, read_buf);
      break;
      
    default:
      break;
    }
  
  return(VALID);
}



/*******************************************************************************
 * check intersection type field.
 ******************************************************************************/
int
check_street_type(field_num, read_buf, field_entered, in_ptr)
     int field_num;	/* pointer to the number of the field read in */
     char *read_buf; /* pointer to the buffer in which the field was read in */
     int field_entered; /* indicates if data was entered or not */
     struct cisam_in *in_ptr;
{
  int  rc;

  if (field_num != F_TYPE1 && field_num != F_TYPE2)
    {
      endwin();
      fprintf(stderr, "Fatal error: Wrong field check. Field = %d. File %s, line %d\n", field_num, __FILE__, __LINE__);
      abort();
    }

  if (read_buf[0] != ' ' && read_buf[0] != '\0')
    {

      rc = check_if_valid_street_type(read_buf);
      if (rc == -1)
	{
	  print_error(catgets(IN_MAINT_m_catd, IN_MAINT_SET, IN_MAINT_46, "Street type is not in database"));
	  return(NOT_VALID);
	}
    }
  
  switch (field_num)
    {
    case F_TYPE1:
      strcpy(in_ptr->type1, read_buf);
      break;
      
    case F_TYPE2:
      strcpy(in_ptr->type2, read_buf);
      break;
      
    default:
      break;
    }

  return(VALID);
}

/*******************************************************************************
 * check subzone  field
 ******************************************************************************/
int
check_subzone(field_num, read_buf, field_entered, in_ptr)
     int field_num;	/* pointer to the number of the field read in */
     char *read_buf; /* pointer to the buffer in which the field was read in */
     int field_entered; /* indicates if data was entered or not */
     struct cisam_in *in_ptr;
{
  int  rc;

  if (field_num != F_SUBZONE)
    {
      endwin();
      fprintf(stderr, "Fatal error: Wrong field check. Field = %d. File %s, line %d\n", field_num, __FILE__, __LINE__);
      abort();
    }

  rc = check_if_valid_zone(read_buf);
  if (rc == -1)
    {
      print_error(catgets(IN_MAINT_m_catd, IN_MAINT_SET, IN_MAINT_47, "Subzone is not in database"));
      return(NOT_VALID);
    }
  
  strcpy(in_ptr->subzone, read_buf);
  return(VALID);
}


/*******************************************************************************
 * check city  field
 ******************************************************************************/
int
check_city(field_num, read_buf, field_entered, in_ptr)
     int field_num;	/* pointer to the number of the field read in */
     char *read_buf; /* pointer to the buffer in which the field was read in */
     int field_entered; /* indicates if data was entered or not */
     struct cisam_in *in_ptr;
{
  int  rc;

  if (field_num != F_CITY)
    {
      endwin();
      fprintf(stderr, "Fatal error: Wrong field check. Field = %d. File %s, line %d\n", field_num, __FILE__, __LINE__);
      abort();
    }

  rc = check_if_valid_city(read_buf);
  if (rc == -1)
    {
      print_error(catgets(IN_MAINT_m_catd, IN_MAINT_SET, IN_MAINT_48, "City is not in database"));
      return(NOT_VALID);
    }

  strcpy(in_ptr->city, read_buf);
  return(VALID);
}




int
check_numeric_or_empty(field_num, read_buf, field_entered, in_ptr)
     int field_num;	/* pointer to the number of the field read in */
     char *read_buf; /* pointer to the buffer in which the field was read in */
     int field_entered; /* indicates if data was entered or not */
     struct cisam_in *in_ptr;
{
  int  rc;

  if (field_num != F_X_COORD && field_num != F_Y_COORD && field_num != F_MAP_PAGE && field_num != F_ZIP)
    {
      endwin();
      fprintf(stderr, "Fatal error: Wrong field check. Field = %d. File %s, line %d\n", field_num, __FILE__, __LINE__);
      abort();
    }
  
  /* Empty fields are valid here */
  if (!is_empty(read_buf))
    {
      rc = string_is_number(read_buf);
      if (rc == -1)
	{
	  print_error(catgets(IN_MAINT_m_catd, IN_MAINT_SET, IN_MAINT_49, "Must enter a number"));
	  return(NOT_VALID);
	}
    }

  switch (field_num)
    {
    case F_X_COORD:
      in_ptr->str_x_coord = atoi(read_buf);
      break;

    case F_Y_COORD:
      in_ptr->str_y_coord = atoi(read_buf);
      break;

    case F_MAP_PAGE:
      in_ptr->map_ref_page = atoi(read_buf);
      break;
      
    case F_ZIP:
      in_ptr->zip = atoi(read_buf);
      break;
      
    default:
      break;
    }

  return(VALID);
}


int
check_string(field_num, read_buf, field_entered, in_ptr)
     int field_num;	/* pointer to the number of the field read in */
     char *read_buf; /* pointer to the buffer in which the field was read in */
     int field_entered; /* indicates if data was entered or not */
     struct cisam_in *in_ptr;
{
  int  rc;

  if (field_num != F_COMMENT && field_num != F_MAP_BLOCK)
    {
      endwin();
      fprintf(stderr, "Fatal error: Wrong field check. Field = %d. File %s, line %d\n", field_num, __FILE__, __LINE__);
      abort();
    }

  str_to_upper(read_buf);

  switch (field_num)
    {
    case F_COMMENT:
      strcpy(in_ptr->comment, read_buf);
      break;

    case F_MAP_BLOCK:
      strcpy(in_ptr->map_ref_block, read_buf);
      break;

    default:
      break;
    }

  return(VALID);
}


void
keep_field_num_valid(field_num, direction)
     int *field_num;
     int direction;
{
  int bad_field;


  bad_field = 1;

  while (bad_field)
    {
      /* Keep everything in the proper range */
      if (*field_num < 0)
	*field_num = Num_fields - 1;
      else if (*field_num >= Num_fields)
	*field_num = 0;
      
      if (*field_num == F_INDEX1 || *field_num == F_INDEX2)
	*field_num += direction;
      else
	bad_field = 0;
    }
}



/******************************************************************************
* update_field_num - update prompt in use variable.
*******************************************************************************/

void
update_field_num(field_num, key_entered)
     int *field_num;
     int key_entered;
{
  int  direction;
  int  bad_field;
  
  
  if (key_entered == '\t' ||
      key_entered == '\n' ||
      key_entered == '\r' ||
      key_entered == KEY_DOWN ||
      key_entered == VALID)
    direction = 1;
  
  if (key_entered == KEY_UP)
    direction = -1;
  
  *field_num += direction;
  
  keep_field_num_valid(field_num, direction);
  
#ifdef SCANDINAVIA
  /* Make sure field_num doesn't fall on any unused fields */
  
  bad_field = 1;
  while (bad_field)
    {
      if (*field_num == F_PRE_DIR1 ||
	  *field_num == F_TYPE1 ||
	  *field_num == F_POST_DIR1 ||
	  *field_num == F_PRE_DIR2 ||
	  *field_num == F_TYPE2 ||
	  *field_num == F_POST_DIR2 ||
	  *field_num == F_ZIP)
	{
	  *field_num += direction;
	  keep_field_num_valid(field_num, direction);
	}
      else
	bad_field = 0;
    }
#endif /*SCANDINAVIA*/
  
} /* End of update_field_num()  */


int
string_is_number(in_str)
     char *in_str;
{
  /* Empty string is invalid */
  if (*in_str == '\0')
    return(0);

  while (*in_str)
    {
      if (!isdigit(*in_str))
	return(0);
      in_str ++;
    }
  return(1);
}

/*************************************************************
 * str_to_upper - Convert a string to upper case
 *************************************************************/

char *
str_to_upper(in_buf)
     char *in_buf;
{
  static char *original_in_buf;

  original_in_buf = in_buf;
  
  while (*in_buf)
    {
      *in_buf = Toupper(*in_buf);
      in_buf ++;
    }
  return(original_in_buf);
}

int
check_if_valid_street_name(street_name)
     char *street_name;
{
  int  rc;
  struct cisam_sn *sn_ptr;

  sn_ptr = (struct cisam_sn *) req_buf;
  bzero(sn_ptr, sizeof(struct cisam_sn));

  strcpy(sn_ptr->name, street_name);

  rc = db(STRNAME_FILE_ID, READ_KEYS, &sn_key2, ISGTEQ, 0);
  if (rc == -1)
    return(-1);

  /* Since we do a ISGTEQ, we must verify streetname */
  if (strcmp(sn_ptr->name, street_name) != 0)
    return(-1);

  return(0);
}

int
check_if_valid_street_type(street_type)
     char *street_type;
{
  int  rc;
  struct cisam_st *st_ptr;

  st_ptr = (struct cisam_st *) req_buf;
  bzero(st_ptr, sizeof(struct cisam_st));

  strcpy(st_ptr->abbrev, street_type);

  rc = db(STRTYPE_FILE_ID, READ_KEYS, &st_key1, ISEQUAL, 0);
  if (rc == -1)
    return(-1);
  return(0);
}

int
check_if_valid_zone(zone)
     char *zone;
{
  int  rc;
  struct cisam_sz *sz_ptr;

  sz_ptr = (struct cisam_sz *) req_buf;
  bzero(sz_ptr, sizeof(struct cisam_sz));

  strcpy(sz_ptr->nbr, zone);

  rc = db(SUBZONE_FILE_ID, READ_KEYS, &sz_key1, ISEQUAL, 0);
  if (rc == -1)
    return(-1);
  return(0);
}

int
check_if_valid_city(city)
     char *city;
{
  int  rc;
  struct cisam_lo *lo_ptr;

  lo_ptr = (struct cisam_lo *) req_buf;
  bzero(lo_ptr, sizeof(struct cisam_lo));

  strcpy(lo_ptr->code, city);

  rc = db(LOCALTY_FILE_ID, READ_KEYS, &lo_key1, ISEQUAL, 0);
  if (rc == -1)
    return(-1);
  return(0);
}

int
get_street_index(street_name, street_type)
     char *street_name;
     char *street_type;
{
  int  rc;
  struct cisam_sn *sn_ptr;

  sn_ptr = (struct cisam_sn *) req_buf;
  bzero(sn_ptr, sizeof(struct cisam_sn));

  strcpy(sn_ptr->name, street_name);
  strcpy(sn_ptr->str_type, street_type);

  rc = db(STRNAME_FILE_ID, READ_KEYS, &sn_key2, ISEQUAL, 0);
  if (rc == -1)
    return(-1);
  return(sn_ptr->index);
}
