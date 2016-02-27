/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: out_msg.c,v $
*  @(#)  $Revision: 2.0 $
*  @(#)  $Date: 2012/08/28 13:29:52 $
*  @(#)  $Author: taxiprod $
*  @(#)  $Source: /usr/users/jwelch/cvs_g7/taxi-g7/lib/MMP/out_msg.c,v $
*
*  Copyright (c) 1992 - Motorola Mobile Data Division, Seattle, WA
*
***********************************************************************/

static char rcsid[] = "$Id: out_msg.c,v 2.0 2012/08/28 13:29:52 taxiprod Exp $";

/* Based in 5.4 */

/****************************************************************************/
/* file out_msg.c 							    */
/* routines for handling predefined outbound text messages                  */
/* taxi application 						            */
/* support for the MPP protocol                                             */
/* does NOT know about protocal per se - see  msg_mmp.c                     */
/* started 24 Jan 90 							    */
/* last update 15 Feb 90 			                            */
/* M. R. Bradley                                                            */
/****************************************************************************/

/****************************************************************************/
/* int init_outb_msg_text() - initialize the message predefined text fields */
/* char get_outb_type(msg_id) - return the type of the message requested    */
/* char * get_outb_txt(msg_id) - return the text if any to the message      */
/* char * get_outb_p_fields(msg_id) - return the p_fields                   */
/* char * get_outb_header(msg_id) - return the type and p_fieldss           */
/* int is_valid_text_id(msg_id) - do we have a valid message id             */
/****************************************************************************/

#include <string.h>
#include <stdlib.h>
#include <memory.h>
#include "dispatch_strdefs.h"
#include "out_msg.h"

#ifndef FALSE
#define FALSE (0)
#define TRUE (1)
#endif

int
init_outb_msg_text()
/* initialize the message predefined text fields */
{
   int   ii;
   /* storage for the actual messages */
   msg_info[0].txt_ptr = (char *) malloc((MAX_MSG_ID + 1) * MAX_MSG_LENGTH);
   for (ii = 0; ii <= MAX_MSG_ID; ii++)
      if (msg_info[ii].is_canned)
	 msg_info[ii].txt_ptr = NULL;
      else
	 msg_info[ii].txt_ptr = msg_info[0].txt_ptr + (ii * MAX_MSG_LENGTH);

#include "msg_text.h"		/* actual message text */
}				/* end init_msg_text */

char
get_outb_type(msg_id)
   int   msg_id;
/* return the type of the message requested */
/* currently 1 = display */
/* 2 = prompt/error */
/* 3 = status */
/* 0 = illegal message id */
{
   return (msg_info[msg_id].type);
}				/* end get_outb_type */

char *
get_outb_txt(msg_id)
   int   msg_id;
/* return the text if any to the message */
/* return null on error */
{
   return (msg_info[msg_id].txt_ptr);
}				/* end get_outb_txt */

char *
get_outb_p_fields(msg_id)
   int   msg_id;
/* return the p_fields */
/* return null on error */
{
   return (msg_info[msg_id].p_fields);
}				/* end get_outb_p_fields */

char *
get_outb_header(msg_id)
   int   msg_id;
/* return the type and p_fieldss */
{
   static char header[P_FLD_SIZE + 2];

   header[0] = get_outb_type(msg_id);
   memcpy(&header[1], get_outb_p_fields(msg_id), P_FLD_SIZE);
   return (header);
}				/* end get_outb_header */

int
is_valid_text_id(msg_id)
   int   msg_id;
/* do we have a vaild message msg_id */
{
   if (msg_id < 0 || msg_id > MAX_MSG_ID)
      return (FALSE);
   else
      return (TRUE);
}				/* end is_valid_text_id */

#ifdef TEST_ONLY
int
get_max_msgs()
/* return the number of predefined outbound text messages */
{
   return (MAX_MSG_ID + 1);
}
#endif
