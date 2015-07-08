static char sz__FILE__[]="@(#)msg_mmp.c	10.1.2.1(ver) Date Release 12/2/94" ;
/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  msg_mmp.c; Rel: 10.1.2.1; Get date: 5/11/95 at 10:15:37
* @(#)  Last delta: 12/2/94 at 17:55:14
* @(#)  SCCS File: /taxi/sccs/s.msg_mmp.c
*                                                                           *
* Copyright (c) 1990 - MDI Seattle                                          *
*                                                                           *
*****************************************************************************/

/* Based on R3 version 5.12 */

/****************************************************************************/
/* file msg_mmp.c 			                                    */
/* routines for formatting actual mmp messages 			            */
/* taxi application 			                                    */
/* support for the MPP protocol 			                    */
/* encapsulates all the mmp format information 			            */
/* started 26 Jan 90 			                                    */
/* last update 12 Nov 1990                                                  */
/* M. R. Bradley 			                                    */
/****************************************************************************/

/****************************************************************************/
/* GENERAL USER ROUTINES                                                    */
/* void init_outb_msg() - initialization - one time stuff                   */
/* int mk_outb_text(text) - make the text field of the mmp header          */
/* int add_outb_text(text) - add the text to an existing text field         */
/* int add_outb_space() - add a space to an existing text field             */
/* void delete_outb_char() - delete the last character in the buffer        */
/* config_outb_msg(<msg content indepenednt params>) - set some msg fields  */
/* int get_outb_line_size() - get the size of the last line in the buffer   */
/* int is_outb_msg(msg) - is this an itm message                            */
/* int get_mmp_msg_field(str1, str2, nbr) - return the nbrbered field in msg */
/* kdt_4.c AND INTERNAL ROUTINES                                            */
/* void mk_outb_header(term_id, msg_hdr) - fill in the outb msg header      */
/* int get_outb_text_size() - return the size of the text field             */
/* int is_text_outb() - return indication of outbound text in buffer        */
/* void terminate_outb_text() - terminate and outbound text message         */
/* void mk_clear_mmp(term_id, device) - clear the indicated display device  */
/* void mk_msg_priority(priority) - set the message priority field          */
/* set_mdtmap(optarg) - sets up the mdt character mapping string            */
/* INTERNAL ROUTINES                                                        */
/* static void mk_id_mmp(term_id)  - make the id field of the mmp header    */
/* static void mk_type_mmp(type) - make the type field of the mmp header    */
/* static void mk_p_fields_mmp(p_fields) - make the p_fields of the mmp hdr */
/* static int is_canned() - returns TRUE if message is canned else FALSE    */
/* static int is_outb_text_too_long(text) - is the text field too long      */
/* static char mdtmap(char) - maps outbound mdt text			    */
/****************************************************************************/

#ifndef TEST_ONLY
#include "acc.h"
#endif

#include <string.h>
#include <ctype.h>
#include <memory.h>
#include <sys/types.h>
#include "STD.h"
#include "taxipak.h"
#include "Object.h"

#include "msg.h"
#include "msg_mmp.h"
#include "mad_error.h"
#include "mads_types.h"
#include "except.h"
#include "calls.h"
#include "veh_driv.h"
#include "tmplen.h"

	
void 	nak_handler(MSG_QUEUE_HDR *, struct veh_driv *);			/* Processes NAK errors */

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

extern MSG_QUEUE_HDR outb;
extern *scratch_ptr;

/* NOTE these defines are also defined in dispatch.c, if they are changed	*/
/* here they also need to be changed there.					*/
#define	MDT_MAP_LENGTH	128
#define	E_SUCCESS	0
#define	E_FAILURE	1
extern	int mdt_char_mapping;
extern	char mdt_map[];
static	char mdtmap();

static int is_outb_text_too_long();

void
config_outb_msg(prio, pkt_id, d_prio, d_disp, multicast, persist)
   int   prio;
   char  pkt_id[];
char  d_prio, d_disp, multicast, persist;
/* set up the message content independent fields */
{
#ifndef PIPES
   outb.prio = prio;
#endif
   memcpy(outb.pkt_id, pkt_id, sizeof(outb.pkt_id));
   if (d_prio == ' ' || d_prio == 'E')
      outb.delvry_prio = d_prio;
#ifdef DEBUG
   else
      printf("BAD  3 parameter to config_outb_msg %c\n", d_prio);
#endif
   if (d_disp == ' ' || d_disp == 'N')
      outb.delvry_disp = d_disp;
#ifdef DEBUG
   else
      printf("BAD  4 parameter to config_outb_msg %c\n", d_disp);
#endif
   if (multicast == ' ' || multicast == 'M')
      outb.multicast = multicast;
#ifdef DEBUG
   else
      printf("BAD  5 parameter to config_outb_msg %c\n", multicast);
#endif
   if (persist == ' ' || persist == 'L')
      outb.persist = persist;
#ifdef DEBUG
   else
      printf("BAD  6 parameter to config_outb_msg %c\n", persist);
#endif
}				/* end config_outb_msg */

void
init_outb_msg()
/* outbound message initialization stuff */
{
   init_outb_msg_text();
   config_outb_msg(10, "00", ' ', ' ', ' ', ' ');

}				/* end init_outb_msg */

static void
mk_id_mmp(term_id)
   char  term_id[];
/* make the id field of the mmp header UPPER CASE */
{
   int   ii;

   for (ii = 0; ii < MID_SIZE; ii++)
   {
      if (islower(term_id[ii]))
	 term_id[ii] = Toupper(term_id[ii]);
   }

   /* check for valid field characters */
   memcpy(outb.route_id, term_id, MID_SIZE);
}				/* end mk_id_mmp */

static void
mk_type_mmp(type)
   char  type;
/* make the type field of the mmp header */
{
   /* check for valid type character */
   memcpy(&outb.type, &type, 1);
}				/* end mk_type_mmp */

static void
mk_p_fields_mmp(p_fields)
   char  p_fields[];
/* make the p_fields field of the mmp header */
{
   /* check for valid field characters */
   memcpy(outb.p_fields, p_fields, P_FLD_SIZE);
}				/* end mk_p_fields_mmp */

void
mk_outb_header(term_id, msg_hdr)
   char  term_id[];
char  msg_hdr[];
/* fill in the outbound message header */
{
   mk_id_mmp(term_id);
   mk_type_mmp(msg_hdr[0]);
   mk_p_fields_mmp(&msg_hdr[1]);
}				/* end mk_outb_header */

void
mk_outb_type(type)
     char type;
{
  memcpy(&outb.type, &type, 1);
}

int
mk_outb_text(text)
   char  text[];
/* make the text field of the mmp header */
/* return text length */
{
   int   ii;
   int   add_outb_text();

   outb.text[0] = '\0';
   return (add_outb_text(text));
}				/* end mk_outb_text */

int
get_outb_text_size()
/* return the size of the text field */
{
   enum {
     T1, T2 } tstate;
   int ii;
   
   if ( outb.type == '2'  && outb.p_fields[0] == '0' && outb.text[0] == '7' && outb.text[1] == '0' )
     {
       ii = 0;
       for ( tstate = T1;; )
	 {
	   if ( tstate == T1 )
	     {
	       if ( outb.text[ii] == '%' )
		 tstate = T2;
	       ii++;
	     }
	   else
	     {
	       if ( outb.text[ii] == 'E' )
		 {
		   ++ii;
		   break;
		 }
	       else
		 {
		   ++ii;
		   tstate = T1;
		 }
	     }
	 }
       return(ii+1);
     }
   else
     return (strlen(outb.text));
}				/* end get_outb_text_size */

int
get_outb_line_size()
/* get the size of the last line in the outbound text buffer */
{
   int   len;
   char *str_ptr = outb.text;


   len = strlen(outb.text);
   do
   {
      str_ptr = (char *) strpbrk(str_ptr, CR);
      if (str_ptr)
      {
	 if (str_ptr[0] == CTRL_START && str_ptr[1] == CR_END)
	    len = strlen(str_ptr + 2);
	 str_ptr++;
      }
   }
   while (str_ptr != 0);
   return (len);
}				/* end get_outb_line_size */

int
add_outb_text(text)
   char  text[];
/* add the text to an existing text field */
/* return length added or 0 in NULL */
{
	int ii;
	int length;

	if (text != NULL)
	{
#ifdef MRB_DEBUG
		if (is_outb_text_too_long(text))
			printf("ERROR - text to long - add_outb_text '%s'\n", text);
#endif
		if (strcmp(text, CR) == 0)   
			if ((get_outb_line_size() % MAX_DISPLAY_LENGTH) == 0)
				return(0);                             /* DONT NEED A CR */
	
		length = strlen(outb.text);
		strcat(outb.text, text);
		for (ii = 0; ii < strlen(text); ii++)
		  {
		    outb.text[ii + length] = Toupper(outb.text[ii + length]);

		    switch ( outb.text[ii + length ] )
		      {
		      case 'ä':
		      case 'Ä':
		      case 'æ':
		      case 'Æ':
			outb.text[ii + length] = '[';
			break;
		      case 'Å':
		      case 'å':
			outb.text[ii + length] = ']';
			break;
		      case 'ö':
		      case 'Ö':
		      case 'ø':
		      case 'Ø':
			outb.text[ii + length] = '\\';
			break;
		      case 'Ü':
		      case 'ü':
			outb.text[ii + length] = '^';
			break;
		      default:
			break;
		      }


		  }

		/* if mapping of MDT characters enabled go to it */
		/*						 */
		if ( mdt_char_mapping )
			for (ii = 0; ii < strlen(text); ii++)
				outb.text[ii + length] = mdtmap(outb.text[ii + length]);

		return (strlen(text));
	}
	return (0);
}				/* add_outb_text */

void
terminate_outb_msg()
{
  int   ii;
  enum {
    T1, T2 } tstate;

  if ( outb.type == '2'  && outb.p_fields[0] == '0' && outb.text[0] == '7' && outb.text[1] == '0' )
    {
      ii = 0;
       for ( tstate = T1;; )
	 {
	   if ( tstate == T1 )
	     {
	       if ( outb.text[ii] == '%' )
		 tstate = T2;
	       ii++;
	     }
	   else
	     {
	       if ( outb.text[ii] == 'E' )
		 {
		   ++ii;
		   break;
		 }
	       else
		 {
		   ++ii;
		   tstate = T1;
		 }
	     }
	 }
       memcpy(&outb.text[ii],"|", 1);
     }
  else
    strcat(outb.text, CHECK_CHAR_STR);
}                              /* end terminate_outb_msg() */

void
delete_outb_char()
/* delete the last outbound character */
{
   int   len = strlen(outb.text);

   if (len)
      outb.text[len - 1] = '\0';
}				/* end delete_outb_char */

int
add_outb_space()
/* add a space to an existing text field */
{

#ifdef MRB_DEBUG
   if (is_outb_text_too_long(" "))
      printf("ERROR - text to long - add_outb_space\n");
#endif
   strcat(outb.text, " ");
   return (1);
}				/* add_outb_space */

static int
is_canned()
/* returns TRUE if message is canned else FALSE */
{
   if (outb.type == CANNED)
      return (TRUE);
   else
      return (FALSE);
}				/* end is_canned */

int
is_itm_msg(msg)
   MSG_QUEUE_HDR *msg;
/* returns true if this message is from the ITM */
{
   if (strncmp(&msg->type, FROM_ITM, 3) == 0)
      return (TRUE);
   else
      return (FALSE);
}				/* end is_itm_msg */

int
is_text_outb()
/* return true if there is any unterminated outbound text */
{
   int   ii;

   // These messages contain binary data...don't use this text termination code
   if ( outb.type == '2'  && outb.p_fields[0] == '0' && outb.text[0] == '7' && outb.text[1] == '0' )
     return(FALSE);
   
   ii = strlen(outb.text);
   if (ii)
   {
      if (outb.text[ii - 1] == EOM_END && outb.text[ii - 2] == CTRL_START)
	 return (FALSE);
      else
	 return (TRUE);
   } else
      return (FALSE);

}				/* end is_text_outb */

void
terminate_outb_text()
/* terminate and outbound text message */
{
   int   ii;
   enum {
     T1, T2 } tstate;

   if ( outb.type == '2'  && outb.p_fields[0] == '0' && outb.text[0] == '7' && outb.text[1] == '0' )
     {
       ii = 0;
       for ( tstate = T1;; )
	 {
	   if ( tstate == T1 )
	     {
	       if ( outb.text[ii] == '%' )
		 tstate = T2;
	       ii++;
	     }
	   else
	     {
	       if ( outb.text[ii] == 'E' )
		 {
		   ++ii;
		   break;
		 }
	       else
		 {
		   ++ii;
		   tstate = T1;
		 }
	     }
	 }
     }
   else
     {
       ii = strlen(outb.text);
       
       if (!is_canned() && is_text_outb())
	 {
	   if (outb.text[ii - 1] == CR_END && outb.text[ii - 2] == CTRL_START)	/* un-needed carriage
										 * return ? */
	     outb.text[ii - 1] = EOM_END;
	   else
	     strcat(outb.text, EOM);
	 }
     }
}				/* end terminate_outb_text */

static int
is_outb_text_too_long(text)
   char  text[];
/* is the text field too long */
{
   int   len;
   char *text_ptr = text;
   char *str_ptr = outb.text;
   char  device = outb.p_fields[0];

   if (text == NULL || !strcmp(text, CR))
      return (FALSE);
   len = strlen(outb.text) + strlen(text);
   switch (device)
   {
   case PROMPT:
      if (len > MAX_PROMPT_LENGTH)
	 return (TRUE);
      break;
   case DISPLAY:

      len = strlen(outb.text);
      /* find the length of the last line in outb.text */
      /* dont know of an  easyh way to generalize CR stuff ?? */
      str_ptr = (char *) strrchr(outb.text, CTRL_START);
      if (str_ptr)
	 if (str_ptr[1] == CR_END)
	    len = strlen(str_ptr + 2);

      /* no fine the lines in text */
      str_ptr = text;
      do
      {
	 str_ptr = (char *) strpbrk(text_ptr, CR);
	 if (str_ptr)
	 {
	    if (str_ptr[0] == CTRL_START && str_ptr[1] == CR_END)
	    {
	       if ((len + (str_ptr - text_ptr)) > MAX_DISPLAY_LENGTH)
		  return (TRUE);
	       text_ptr = str_ptr + strlen(CR);
	       len = 0;
	    } else
	    {
	       len += (str_ptr - text_ptr);
	       text_ptr = str_ptr + 1;
	    }
	 }
      }
      while (str_ptr != 0);

      len += strlen(text_ptr);
      if (len > MAX_DISPLAY_LENGTH)
	 return (TRUE);
      break;
   case STATUS:
      if (len > MAX_STATUS_LENGTH)
	 return (TRUE);
      break;
   default:
#ifdef MRB_DEBUG
      printf("ERROR - bad device %c\n", outb.p_fields[0]);
#endif
      break;
   }
   return (FALSE);
}				/* end is_outb_text_too_long */

void
mk_msg_priority(priority)
   char  priority;
{
   if (priority >= PRIORITY_0 && priority <= PRIORITY_3)
      outb.p_fields[2] = priority;
}				/* end mk_msg_priority */

void
mk_clear_mmp(term_id, device)
   char  term_id[];
char  device;
/* clear the indicated screen area */
{
   mk_id_mmp(term_id);
   mk_type_mmp('2');
   strcpy(outb.text, EOM);
   switch (device)
   {
   case STATUS:
      mk_p_fields_mmp("10200");
      break;
   default:
   case DISPLAY:
      mk_p_fields_mmp("00200");
      break;
   }
}				/* mk_clear_mmp */

int
get_mmp_msg_field(str_ptr, dest_str, field_nbr)
   char *str_ptr;
   char *dest_str;
   int   field_nbr;
/* copy the indicated field in str_ptr to dest_str */
/* null termiante dest_str */
/* return nbrber of characters copied */
{
   char *ptr_field_start = str_ptr;
   char *ptr_field_end;
   int   ii;
   int   str_len;

   if (field_nbr <= 0)
      return (0);
   if (dest_str == NULL)
      return (0);
   if (str_ptr == NULL)
      return (0);
   if (strlen(str_ptr) == 0)
      return (0);
   for (ii = 0; ii < field_nbr; ii++)
   {
      ptr_field_end = (char *) strchr(ptr_field_start, CTRL_START);
      if (ptr_field_end == NULL)
	 return (0);
      if (ptr_field_end[1] != 'D')
      {
#ifdef DEBUG_R4
	 /* fprintf(stderr,"Illegal field termination %s\n",str_ptr); */
#endif
	 /* must not be this many fields */
	 return (0);
      }
      if (ii < field_nbr - 1)
	 ptr_field_start = ptr_field_end + 2;
   }
   ptr_field_end--;

   str_len = ptr_field_end - ptr_field_start + 1;
   if (str_len == 0)
      return (0);
   strncpy(dest_str, ptr_field_start, str_len);
   dest_str[str_len] = '\0';
   return (str_len);
}				/* end get_mmp_msg_field */

#ifdef TEST_ONLY
void
print_msg()
/* print the current message */
{
   int   ii;
   printf("Message to :");
   for (ii = 0; ii < 3; ii++)
      printf("%c", outb.route_id[ii]);
   printf(" type is %c\n", outb.type);
   printf(" p_fields ");
   switch (outb.p_fields[0])
   {
   case DISPLAY:
      printf(" D ");
      break;
   case STATUS:
      printf(" S ");
      break;
   case PROMPT:
      printf(" P ");
      break;
   case DISPLAY_BUF:
      printf(" B ");
      break;
   default:
      printf(" ?%x ", outb.p_fields[0]);
   }				/* end switch */
   for (ii = 1; ii < 5; ii++)
      printf(" %c ", outb.p_fields[ii]);
   if (outb.text[0] == '\0')
   {
      if (outb.p_fields[2] == 0)
	 printf("NO CANNED MESSAGE NUMBER\n");
      if (outb.type != '3')
	 printf("WORNG MESSAGE TYPE\n");
      printf("\n text:-<CANNED>-\n \n");
   } else
      printf("\n text:-%s-\n\n", outb.text);
}				/* end print_msg */
#endif

#ifndef TEST_ONLY
void
dump_outb_msg()
/* dump the outb message */
{
   int   ii;
	 static char temp_str[TMP_STR_LEN];
	 static char trace_str[TMP_STR_LEN];  /* should use mads_error.h */

#ifndef PIPES
   sprintf(trace_str, " ipc priority = %d\n", outb.prio);
   TRACE(trace_str);
#endif

   sprintf(trace_str, " pkt_id = %c\n", outb.pkt_id);
   TRACE(trace_str);

   sprintf(trace_str, " delvry_prio = %c\n", outb.delvry_prio);
   TRACE(trace_str);

   sprintf(trace_str, " delvry_disp = %c\n", outb.delvry_disp);
   TRACE(trace_str);

   sprintf(trace_str, " mulitcast = %c\n", outb.multicast);
   TRACE(trace_str);

   sprintf(trace_str, " persist = %c\n", outb.persist);
   TRACE(trace_str);

   strcpy(trace_str, " Addressed to :");
   for (ii = 0; ii < 3; ii++)
   {
      sprintf(trace_str, "%c", outb.route_id[ii]);
      strcat(trace_str, temp_str);
   }
   sprintf(temp_str, " type is %c\n", outb.type);
   strcat(trace_str, temp_str);
   TRACE(trace_str);

   strcpy(trace_str, " p_fields ");
   switch (outb.p_fields[0])
   {
   case DISPLAY:
      strcat(trace_str, " D ");
      break;
   case STATUS:
      strcat(trace_str, " S ");
      break;
   case PROMPT:
      strcat(trace_str, " P ");
      break;
   case DISPLAY_BUF:
      strcat(trace_str, " B ");
      break;
   default:
      sprintf(temp_str, " ?%x ", outb.p_fields[0]);
      strcat(trace_str, temp_str);
   }				/* end switch */
   for (ii = 1; ii < 5; ii++)
   {
      sprintf(temp_str, " %c ", outb.p_fields[ii]);
      strcat(trace_str, temp_str);
   }
   strcat(trace_str, "\n");
   TRACE(trace_str);

   trace_str[0] = '\0';
   if (outb.text[0] == '\0')
   {
      if (outb.p_fields[2] == 0)
	 strcat(trace_str, "NO CANNED MESSAGE NUMBER\n");
      if (outb.type != '3')
	 strcat(trace_str, "WORNG MESSAGE TYPE\n");
      sprintf(temp_str, " text:-<CANNED>-\n \n");
   } else
      sprintf(temp_str, " text:-%s-\n\n", outb.text);
   strcat(trace_str, temp_str);
   TRACE(trace_str);
}				/* end dump_outb_msg */
#endif
#ifdef DEBUG
count_msg()
{
   char *str_ptr = outb.text;
   int   char_cnt = 0;
   int   line_cnt = 1;

   do
   {
      if (str_ptr[0] == CTRL_START && str_ptr[1] == CR_END)
      {
	 printf("ch = %d line # %d\n", char_cnt, line_cnt++);
	 char_cnt = 0;
	 str_ptr++;
      } else
	 char_cnt++;
      str_ptr++;
   }
   while (str_ptr[0] != 0);
   if (char_cnt)
      printf("ch = %d line # %d\n", char_cnt, line_cnt);
   printf("\n");
}
#endif


int set_mdtmap( optarg )
char *optarg;
{ 
	int 	length;
	char	*start, *end, *local_str;

	mdt_char_mapping = 0;

	/* check for NULL character map	*/
	/*				*/
	if ( optarg == NULL )
		return( E_FAILURE );

	local_str = (char *)malloc(strlen(optarg) + 1);
	strcpy(local_str, optarg);
	start = local_str;

	/* check to see if the map string is quoted, if so do not	*/
	/* include the quotes in the mapping string			*/
	/*								*/
	if ( *start == '\'' )
	{
		++start;

		/* check for ending quote	*/
		end = strrchr(start, '\'');
		if ( end == NULL || end == optarg )
			return( E_FAILURE );

		*end = '\0';
	}
	else	/* check for double quote	*/
		if ( *start == '\"' )
		{
			++start;

			/* check for ending quote	*/
			end = strrchr(start, '\"');
			if ( end == NULL || end == optarg )
				return( E_FAILURE );

			*end = '\0';
		}

	
	length = strlen(start);

	/* exit if map is too short or too long	*/
	if ( length <= 0 || length >= MDT_MAP_LENGTH )
		return( E_FAILURE );

	/* exit if map does not contain even number of characters	*/
	if ( length % 2 )
		return( E_FAILURE );


	/* set the first character of mdt_map to the half the length of	*/
	/* the map string and copy the string in after it		*/
	/*								*/
	mdt_char_mapping = length / 2;
	mdt_map[0] 	 = (char)(mdt_char_mapping);
	strncpy( &mdt_map[1], start, length );

	return( E_SUCCESS );
}

/* Map the character, if there is no match return the character */
/*								*/
static char mdtmap( c )
{ 
	int i, map_length;

	map_length = (int)mdt_map[0];

	for ( i = 1; i <= map_length; i++ )
		if ( c == mdt_map[i] )
			return( mdt_map[i + map_length] );
	return( c );
}
