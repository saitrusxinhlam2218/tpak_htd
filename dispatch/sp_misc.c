static char sz__FILE__[]="@(#)sp_misc.c	10.1.2.8(ver) Date Release 5/24/95" ;
/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  sp_misc.c; Rel: 10.1.2.8; Get date: 5/24/95 at 11:41:14		            *			
* @(#)  Last delta: 5/24/95 at 11:41:10						    *	
* @(#)  SCCS File: /taxi/sccs/s.sp_misc.c							    *
*                                                                           *
* Copyright (c) 1994 - TaxiCom ApS					    *
*                                                                           *
*****************************************************************************/
#include <stdio.h>
#include <time.h>
#include <curses.h>
#include <string.h>
#include <sys/types.h>
#include "mads_types.h"
#include "mads_isam.h"
#include "language.h"
#include "kdt.h"
#include "acc.h"
#include "ui_ipc.h"
#include "except.h"
#include "calls.h"
#include "veh_driv.h"
#include "f_msg_defs.h"
#include "dispatch_strdefs.h"
#include "msg_mmp.h"
#include "writer.h"
#include "dispatch.h"
#include "mad_error.h"
#include "path.h"
#include "FTJ_bad_db.h"

extern time_t mads_time;
extern char cur_dt[];
extern char cur_tm[];
extern FILE *credit_fp;

long authorization_nbr;

static void
mk_general_comment(cl_ptr)
   struct cisam_cl *cl_ptr;
/* make the gerneal comment field of the assignment message */
{
   char  comment_buf[MAX_COMMENT_LENGTH + 1];
   char *cptr;
   int len;

   /* if we have more than 32 characters, less than 64 then and */
   /* the first line does not end with a space attempt to avoid word wrap  */

   len = strlen(cl_ptr->general_cmnt);
   if (len > MAX_DISPLAY_LENGTH
       && len < MAX_COMMENT_LENGTH
       && !(isspace(*(cl_ptr->general_cmnt + MAX_DISPLAY_LENGTH - 1))))
   {
      /* copy only the amount of text that can be displayed */
      /* */
      strncpy(comment_buf, cl_ptr->general_cmnt, MAX_COMMENT_LENGTH);
      comment_buf[MAX_COMMENT_LENGTH] = '\0';

      /* search backwards for a space to see if we can split the line	 */
      /* */
      cptr = &comment_buf[MAX_DISPLAY_LENGTH];

      while (!isspace(*cptr) && cptr > comment_buf)
	 --cptr;

      /* check to see if we can move a word from the first line to the second */
      /* if not then just output the comment and don't worry about the split  */
      /* */
      if ((len - (int) (cptr - comment_buf) + 1) <= MAX_DISPLAY_LENGTH)
      {
	 *cptr = '\0';
	 add_outb_text(comment_buf);

	 /* Don't write out the CR if the line is equal to the	 */
	 /* MAX_DISPLAY_LENGTH	it will be done by add_outb_text */
	 /* */
	 if ((int) (cptr - comment_buf) < MAX_DISPLAY_LENGTH)
	    add_outb_text(CR);

	 add_outb_text(++cptr);
	 if (strlen(cptr) < MAX_DISPLAY_LENGTH)
	    add_outb_text(CR);

      } else
      {
	 add_outb_text(comment_buf);
	 add_outb_text(CR);
      }
   } else
   {
      add_outb_text(cl_ptr->general_cmnt);
      if (len != MAX_DISPLAY_LENGTH && len != (2 * MAX_DISPLAY_LENGTH))
	 add_outb_text(CR);
   }

}				/* end mk_comment */

static int
is_this_a_time_call(call)
   struct cisam_cl *call;
/* does this trip require a due time for the assignment */
{
   int ii;
 
   for(ii=0;ii<sizeof(call->extended_type);ii++)
   {
 
    if (call->extended_type[ii]==TIMEC)
            return (TRUE);
    if (call->extended_type[ii]==SUBS)
            return (TRUE);
    if (call->extended_type[ii] == '\0' )					/* no more valid type chars */
	    break;
   }
   return (FALSE);
}

void ConvertMessage(char *message)
/**************************************************************************
 * Name: void ConvertMessage(char *message);                              *
 * Type:                                                                  *
 *                                                                        *
 * Purpose: Looks through character string for Sweedish 7 bit characters  *
 *          and converts them to 8 bit.                                   *
 *                                                                        *
 * Returnvalue: None                                                      *
 **************************************************************************/
{
  int i,len;

  len = strlen(message);

  for (i = 0; i < len; i++)
  {
    switch(*(message+i))
    {
      case ']' : *(message+i) = 143;
		 break;
      case '[' : *(message+i) = 142;
		 break;
      case '\\': *(message+i) = 153;
		 break;
      case '}' : *(message+i) = 134;
		 break;
      case '{' : *(message+i) = 132;
		 break;
      case '|' : *(message+i) = 148;
		 break;
    }
  }
}


void EscapePercent(char *message)
/**************************************************************************
 * Name: void EscapePercent(char *message);                               *
 * Type:                                                                  *
 *                                                                        *
 * Purpose: Looks through character string for the '%'-character          *
 *          and escapes it with another '%'-character.                    *
 *                                                                        *
 * Returnvalue: None                                                      *
 **************************************************************************/
{
  int i,j,len;
  char TmpString[512];

  len = strlen(message);

  j = 0;
  for (i = 0; i < len; i++)
  {
    switch(*(message+i))
    {
      case '%' : *(TmpString+j++) = *(message+i);
		 *(TmpString+j++) = '%';
		 break;
      default:   *(TmpString+j++) = *(message+i);
		 break;
    }
  }
  TmpString[j] = 0;
  strcpy(message,TmpString);
}



void WriteCardMessage(char *message)
/**************************************************************************
 * Name: void WriteCardMessage(char *message);                            *
 * Type:                                                                  *
 *                                                                        *
 * Purpose: Writes the characterstring "message to the credit_trans file  *
 *          (For debug purpose only)                                      *
 *                                                                        *
 * Returnvalue: None                                                      *
 **************************************************************************/
{
     if (credit_fp == NULL)
     {
	if ((credit_fp = fopen(CREDIT_TRANSACTION_FILE, "a")) == NULL)
	{
	   ERROR(' ', "", "Can't open CREDIT_TRANSACTION_FILE");
	   return;
	}
     }
     /* seek to the end of the file before doing the write  */
     /* This may seem like a stupid statement but it allows */
     /* the file to be zero by calling truncate on the file */
     /* Otherwise when the purge routines are run the file  */
     /* would have to be closed.                            */
     fseek(credit_fp, 0L, 2);

     fprintf(credit_fp, "%s\n", message);
     fflush(credit_fp);
}


int long2bcd(long num, char *bcd)
/**************************************************************************
 * Name: int long2bcd(long num, char bcd[BCD_LEN]);                       *
 * Type:                                                                  *
 *                                                                        *
 * Purpose: Converts the last 8 digits of a 9 digit long integer to its   *
 *          BCD representation.                                           *
 *                                                                        *
 *                                                                        *
 * Returnvalue: Success         1                                         *
 *              Failure         0                                         *
 **************************************************************************/
{
  char num_str[20];
  int length,x,a,b;

  sprintf(num_str,"%ld",num);
  length = strlen(num_str);
  if (length < 9)
  {
    ERROR(' ',"","Unable to convert to BCD");
    return(0);
  }
  else
  {
    for(x = 0; x < 4; x++)
    {
      bcd[x]=(char)((((char)num_str[x*2+1]-'0') >> 4) + ((char)num_str[x*2+2]-'0'));
/*      printf("x:%d, 1:%u, 2:%c, 3:%c\n",x,(char)bcd[x], num_str[x*2+1], num_str[x*2+2]); */
    }
    bcd[4] = 0;

  }

  return(1);
} /* end long2bcd */


/**************************************************************************
 * Name: char *get_card_validity_reply(char *dest, char *text,            *
 *                                     struct veh_driv *veh_ptr)          *
 * Type:                                                                  *
 *                                                                        *
 * Purpose: Handles a credit card validation request - checks the         *
 *          validity - and returns a reply that should be sent to         *
 *          the vehicle.                                                  *
 *                                                                        *
 *          Both input text and output dest are 6 byte ascii strings.     *
 *                                                                        *
 *                                                                        *
 * Returnvalue: Pointer to credit card validation request.                *
 **************************************************************************/
char *get_card_validity_reply(char *dest, char *text, struct veh_driv *veh_ptr)
{
  int i,j,res;
  char exp_str[512];
  char data_str[512];
  char card_validity_message[512];
  char card_name[512];
  char bcd[10];

  i = 0;
  while(text[i] != 0) /* find length of 6 bit string */
    i++;
  i = expand(exp_str,text,i-4); /* ignore the last 4 end of string characters */

  /******************
   * get BCD string *
   ******************/
  j = 0;
  while (i && j< 200)
  {
    sprintf(&data_str[2*j],"%c%c", (exp_str[j] & 0xF0)/16+'0',(exp_str[j] & 0x0F)+'0');
    i--;
    j++;
  }
  data_str[2*j]=0;       /* terminate string */

  /*********************************
   * Check validity of card string *
   *********************************/

  if ( veh_ptr->term_type == MDT_V03_ITM_VERS2 )
    card_validity_message[0] = exp_str[0];
  else
    card_validity_message[0] = exp_str[1]; /* get index */

  res = check_card_validity(veh_ptr, data_str, card_name);
  ConvertMessage(card_name);
  /***************************************
   * convert authorization number to bcd *
   ***************************************/

  if (res == -1)
  {
    /* Card validity not OK */
    if (res  = long2bcd(authorization_nbr,bcd))
      /************************************/
      /* Card Validity Reply to TaxiMeter */
      /* Extra 0 added to end of line as  */
      /* Dummy checksum                   */
      /************************************/
      sprintf(&card_validity_message[1],"%s:%c%c%c%c%c%c%c%c%c%c0",
	      card_name,0x4A,0x9A,0xAA,0xAA,0xAA,0xAA,bcd[0],bcd[1],bcd[2],bcd[3]);
  }
  else
  {
    /* Card validity OK */
    if (res  = long2bcd(authorization_nbr,bcd))
    {
      /************************************/
      /* Card Validity Reply to TaxiMeter */
      /* Extra 0 added to end of line as  */
      /* Dummy checksum                   */
      /************************************/
      sprintf(&card_validity_message[1],"%s:%c%c%c%c%c%c%c%c%c%c0",
	      card_name,0x4A,0x0A,0xAA,0xAA,0xAA,0xAA,bcd[0],bcd[1],bcd[2],bcd[3]);
    }
  }
  i = compress(dest,card_validity_message,strlen(card_name)+13); /* string length = index + cardname + validity codes + BCD number */
  dest[i] = 0;   /* terminate output string */
  EscapePercent(dest);
  WriteCardMessage(dest); /* for debug purpose only */
  return(dest);
}

char *get_ftj_card_validity_reply(char *dest, char *text, struct veh_driv *veh_ptr)
{
  int i,j,res;
  char exp_str[512];
  char data_str[512];
  char card_validity_message[512];
  char card_name[512];
  char bcd[10];
  char *ctime(), *stime;
  time_t tmtime;
  char   DateBuff[16];
   FILE   *trace_logfile;  

#ifdef FOO
  if ((trace_logfile = fopen("/usr/taxi/run/traces/ftj_valid_trace", "a")) != NULL)
      {
	  tmtime = time((long *) 0);
	  get_arg_date(tmtime, DateBuff);
	  stime = ctime(&tmtime);
	  fprintf(trace_logfile, "%.2s", DateBuff);
	  fprintf(trace_logfile, "%.2s", &DateBuff[3]);
	  fprintf(trace_logfile, "%.2s ", &DateBuff[6]);
	  fprintf(trace_logfile, "%.2s", &stime[11]);
	  fprintf(trace_logfile, "%.2s", &stime[14]);
	  fprintf(trace_logfile, "%.2s", &stime[17]);
	  fprintf(trace_logfile, " FTJ_VALID RECEIVED %s\n", text);
      }
#endif
  
  i = 0;
  while(text[i] != 0) /* find length of 6 bit string */
    i++;
  i = expand(exp_str,text,i-4); /* ignore the last 4 end of string characters */



  /******************
   * get BCD string *
   ******************/
  j = 0;
  while (i && j< 200)
  {
    sprintf(&data_str[2*j],"%c%c", (exp_str[j] & 0xF0)/16+'0',(exp_str[j] & 0x0F)+'0');
    i--;
    j++;
  }
  data_str[2*j]=0;       /* terminate string */

#ifdef FOO
  if ( trace_logfile != NULL )
      fprintf(trace_logfile, "               EXPANDED %s\n", data_str);
#endif

  /*********************************
   * Check validity of card string *
   *********************************/

  card_validity_message[0] = exp_str[0];

  res = 1;  
  res = check_ftj_card_validity(data_str);
  // ConvertMessage(card_name);
  strcpy(card_name, "FTJ");
  authorization_nbr = time(NULL);

  /***************************************
   * convert authorization number to bcd *
   ***************************************/

  if (res == -1)
  {
    /* Card validity not OK */
    if (res  = long2bcd(authorization_nbr,bcd))
      /************************************/
      /* Card Validity Reply to TaxiMeter */
      /* Extra 0 added to end of line as  */
      /* Dummy checksum                   */
      /************************************/
      sprintf(&card_validity_message[1],"%s:%c%c%c%c%c%c%c%c%c%c0",
	      card_name,0x4A,0x9A,0xAA,0xAA,0xAA,0xAA,bcd[0],bcd[1],bcd[2],bcd[3]);
  }
  else
  {
    /* Card validity OK */
    if (res  = long2bcd(authorization_nbr,bcd))
    {
      /************************************/
      /* Card Validity Reply to TaxiMeter */
      /* Extra 0 added to end of line as  */
      /* Dummy checksum                   */
      /************************************/
      sprintf(&card_validity_message[1],"%s:%c%c%c%c%c%c%c%c%c%c0",
	      card_name,0x4A,0x0A,0xAA,0xAA,0xAA,0xAA,bcd[0],bcd[1],bcd[2],bcd[3]);
    }
  }

#ifdef FOO
  if ( trace_logfile != NULL )
      {
	  fprintf(trace_logfile, "               REPLY %x%-3.3s%1x%1x%1x%1x%1x%1x%1x%1x%1x%1x\n",
		  card_validity_message[0],
		  &card_validity_message[1],
		  card_validity_message[2],
		  card_validity_message[3],
		  card_validity_message[4],
		  card_validity_message[5],
		  card_validity_message[6],
		  card_validity_message[7],
		  card_validity_message[8],
		  card_validity_message[9],
		  card_validity_message[10],
		  card_validity_message[11]);
      }
#endif
  
  i = compress(dest,card_validity_message,strlen(card_name)+13); /* string length = index + cardname + validity codes + BCD number */

  
  dest[i] = 0;   /* terminate output string */
  EscapePercent(dest);
  
#ifdef FOO
  if ( trace_logfile != NULL )
      {
	  fprintf(trace_logfile, "               COMPRESSED %s\n", dest);
	  fclose( trace_logfile );
      }
#endif
  
  return(dest);
}


/**************************************************************************
 * Name: void log_assign_nak(int except, struct calls *call_ptr)          *
 * Type:                                                                  *
 *                                                                        *
 * Purpose: Logs a trip detail NACK in calls history table                *
 *                                                                        *
 *                                                                        *
 * Returnvalue: NONE                                                      *
 **************************************************************************/
 void log_assign_nak(int except_nbr,struct calls *call_ptr)
{
	struct cisam_ch writer_buf;
	char str_temp[80];

	get_asc_time(mads_time,cur_dt,cur_tm);
	writer_buf.call_nbr = call_ptr->call_number;
	writer_buf.exception_nbr = (short)except_nbr;
	writer_buf.fleet = call_ptr->veh_ptr->fleet;
	writer_buf.event_type = EVNT_REG_TYPE;
	writer_buf.status_dt_tm = time(NULL);
	strcpy(writer_buf.status_date,cur_dt);
	strcpy(writer_buf.status_time,cur_tm);

	sprintf(str_temp,catgets(WRITER_catd, 1, 49, "NAK TRIP"));
	strcpy(writer_buf.event_desc, str_temp);

	writer_buf.veh_nbr = call_ptr->veh_ptr->veh_nbr;
	writer_buf.drv_id = call_ptr->veh_ptr->driver_id;
	writer_buf.user_id = SYSTEM_ID;

	strcpy(writer_buf.action_taken,"");
	strcpy(writer_buf.action_date,"");
	strcpy(writer_buf.action_time,"");
	writer_buf.action_dt_tm = 0;
	writer_buf.action_veh_nbr = 0;
	writer_buf.action_drv_id = 0;
	writer_buf.action_user_id = 0;
	writer_add_hist(&writer_buf);
}


/**************************************************************************
 * Name: void log_ch_nack(int except, struct calls *call_ptr)              *
 * Type:                                                                  *
 *                                                                        *
 * Purpose: Logs a call message NACK in calls history table               *
 *                                                                        *
 *                                                                        *
 * Returnvalue: NONE                                                      *
 **************************************************************************/
 void log_ch_nack(int except_nbr,struct calls *call_ptr, char cmsg_device, char cmsg_nbr)
{
	struct cisam_ch writer_buf;
	char str_temp[80];

	/* Add checks for NULL pointers to prevent */
	/* 	segmentation faults in dispatch */

	if (call_ptr == NULL)
	  {
	    ERROR(' ', "", "Null call_ptr in log_ch_nack");
	    return;
	  }
	if (call_ptr->veh_ptr == NULL)
	  {
	    ERROR(' ', "", "Null veh_ptr in log_ch_nack");
	    return;
	  }
	
	get_asc_time(mads_time,cur_dt,cur_tm);
	writer_buf.call_nbr = call_ptr->call_number;
	writer_buf.exception_nbr = (short)except_nbr;
	writer_buf.fleet = call_ptr->veh_ptr->fleet;
	writer_buf.event_type = EVNT_REG_TYPE;
	writer_buf.status_dt_tm = time(NULL);
	strcpy(writer_buf.status_date,cur_dt);
	strcpy(writer_buf.status_time,cur_tm);

	sprintf(str_temp,"NAK %c:%c",cmsg_nbr, cmsg_device);
	strcpy(writer_buf.event_desc, str_temp);

	writer_buf.veh_nbr = call_ptr->veh_ptr->veh_nbr;
	writer_buf.drv_id = call_ptr->veh_ptr->driver_id;
	writer_buf.user_id = SYSTEM_ID;

	strcpy(writer_buf.action_taken,"");
	strcpy(writer_buf.action_date,"");
	strcpy(writer_buf.action_time,"");
	writer_buf.action_dt_tm = 0;
	writer_buf.action_veh_nbr = 0;
	writer_buf.action_drv_id = 0;
	writer_buf.action_user_id = 0;
	writer_add_hist(&writer_buf);
}



/**************************************************************************
 * Name: int sp_itm_send_msg(struct sp_itm_msg *sp_itm_msg_ptr)           *
 * Type:                                                                  *
 *                                                                        *
 * Purpose: Sends an ITM message to car                                   *
 *                                                                        *
 *                                                                        *
 * Returnvalue: On success : 1                                            *
 *              On failure : 0                                            *
 **************************************************************************/
int sp_itm_send_msg(struct sp_itm_msg *sp_itm_msg_ptr)
{
	struct veh_driv *veh_ptr;

	if((veh_ptr = (struct veh_driv*) get_veh_id(sp_itm_msg_ptr->term_id)) == NULL)
	{
		return(0);
	}

	itm_send_msg(sp_itm_msg_ptr->term_id, veh_ptr, sp_itm_msg_ptr->itm_msg_type, sp_itm_msg_ptr->msg);
	return(1);
}


/**************************************************************************
 * Name: char *format_mdt_msg(char *dst_text, char *src_text)             *
 * Type:                                                                  *
 *                                                                        *
 * Purpose: formats a mdt message to be displayed as whole words on the   *
 *          mdt display.                                                  *
 *                                                                        *
 *                                                                        *
 * Returnvalue: Pointer to destination string                             *
 **************************************************************************/
char *format_mdt_msg(char *dst_text, char *src_text)
{
  int start,end,j;
  char dummy[320];

  start = 0;

  dummy[0] = 0;
  j = 0;

  while (strlen(src_text + start) > MAX_DISPLAY_LENGTH)
  {
    j = 0;

    end = start + MAX_DISPLAY_LENGTH - 1;

    while ((j < 7) && (src_text[end - j] != ' '))
      j++;

    if (j == 7)
    {
      strncat(dummy,src_text + start,MAX_DISPLAY_LENGTH);

      start += MAX_DISPLAY_LENGTH;
    }
    else
    {
      strncat(dummy,src_text + start, MAX_DISPLAY_LENGTH - j);
      if (j)
	strcat(dummy,"%R");
      start += MAX_DISPLAY_LENGTH - j;
    }
  } /* end while */

  strcat(dummy,src_text+start);
  strcpy(dst_text,dummy);
  return(dst_text);
}




/**************************************************************************
 * Name: int sp_mdt_send_msg(struct sp_mdt_msg *sp_mdt_msg_ptr)           *
 * Type:                                                                  *
 *                                                                        *
 * Purpose: Sends an MDT message to car                                   *
 *                                                                        *
 *                                                                        *
 * Returnvalue: On success : 1                                            *
 *              On failure : 0                                            *
 **************************************************************************/
int sp_mdt_send_msg(struct sp_mdt_msg *sp_mdt_msg_ptr)
{
	struct veh_driv *veh_ptr;
	char dummy[320];

	if((veh_ptr = (struct veh_driv*) get_veh_id(sp_mdt_msg_ptr->term_id)) == NULL)
	{
		return(0);
	}
	/* printf("MDT term_id %s\n",sp_mdt_msg_ptr->term_id); */
	send_pkt_id(SP_MDT_PKT_TYPE, 0,
		    (short)Veh_get_value((VEH_HNDL)veh_ptr, VEH_FLEET_NBR),
		    (int)Veh_get_value((VEH_HNDL)veh_ptr, VEH_NBR));
	format_mdt_msg(dummy, sp_mdt_msg_ptr->msg);
	send_text(sp_mdt_msg_ptr->term_id, dummy, veh_ptr,'60'); /* Priority 6 Highest, 60 Lowest */
	clr_pkt_id();
	return(1);
}


/**************************************************************************
 * Name: int generate_exception(struct sp_excpt_rec *sp_except_rec_ptr)   *
 * Type:                                                                  *
 *                                                                        *
 * Purpose: Generate an exception with no relation to a calll             *
 *          Function should later be expanded to handle all exception     *
 *          types                                                         *
 *                                                                        *
 *                                                                        *
 * Returnvalue: On success : 1                                            *
 *              On failure : 0                                            *
 **************************************************************************/
int generate_exception(struct sp_excpt_rec *sp_except_rec_ptr)
{
	short fleet_nbr;
	struct calls *cl_ptr;
	struct veh_driv *veh_ptr;
	int idx;

	cl_ptr = NULL;
	veh_ptr =NULL;
	fleet_nbr = 0;

	if (sp_except_rec_ptr->call_nbr > 0)
	{
	  cl_ptr = ( struct calls * ) Calls_list_find( sp_except_rec_ptr->call_nbr );
		if ( cl_ptr != NULL )
		{
				veh_ptr   = cl_ptr->veh_ptr;
				fleet_nbr = cl_ptr->fleet;
		}


	}


	/*printf("SP_MISC.C: Generate_exception:\n");
	printf("SP_MISC.C: sp_except_rec_ptr->call_nbr %d\n",sp_except_rec_ptr->call_nbr);
	printf("SP_MISC.C: type: %d\n",sp_except_rec_ptr->except_type);
	printf("SP_MISC.C: info: %d\n",sp_except_rec_ptr->except_info); */

	mk_except(cl_ptr, veh_ptr, sp_except_rec_ptr->except_type , sp_except_rec_ptr->except_info , fleet_nbr);
	if (cl_ptr != NULL &&
	    sp_except_rec_ptr->except_type == SYS_ERR &&
	    sp_except_rec_ptr->except_info >= NO_ACK_MDT_1 &&
	    sp_except_rec_ptr->except_info <= NO_ACK_ITM_3)
		log_ch_nack(sp_except_rec_ptr->except_type, cl_ptr,  sp_except_rec_ptr->cmsg_device, sp_except_rec_ptr->cmsg_nbr);
	return(1);
}


int mk_sp_details10( struct cisam_cl *cl_ptr)
{
  int ii, len;
  char    dummy[35];
  char    new_due_time[6];

  mk_outb_text("");

  /*********************** LINE 1 **********************/
  add_outb_text("KONTOBEST ");
  if (cl_ptr->grouped_with)
    sprintf(dummy,"%c/%c BN:%d", cl_ptr->pos_in_group, cl_ptr->nbr_in_group, cl_ptr->nbr);
  else
    sprintf(dummy,"BN:%d", cl_ptr->nbr);
  add_outb_text(dummy);
  if (is_this_a_time_call(cl_ptr))
    {
      add_outb_space();
      add_outb_text(cl_ptr->due_time);
    }
  add_outb_text(CR);

  /********************** LINE 2 ***********************/
  if (strlen(cl_ptr->passenger_name))
    {
      sprintf(dummy,"NAMN:%s FAKT.", cl_ptr->passenger_name);
      add_outb_text(dummy);
      add_outb_text(CR);
    }
  else
    {
      strcpy(dummy, "FAKT.");
      add_outb_text(dummy);
      add_outb_text(CR);
    }

  /********************** LINE 3 **********************/
  if (cl_ptr->call_rate >= 1.0)
    {
      sprintf(dummy,"H[MTA:%s %3s %3d FPR:%4.0f",
	      cl_ptr->pckup_pmap, cl_ptr->pckup_city, cl_ptr->pckup_zone, cl_ptr->call_rate);
    }
  else
    sprintf(dummy,"H[MTA:%s %s %3d ",
		  cl_ptr->pckup_pmap, cl_ptr->pckup_city, cl_ptr->pckup_zone);
  add_outb_text(dummy);
  add_outb_text(CR);
  
  /********************* LINE  4 **********************/
  sprintf(dummy, "%s %d %3s",
	  cl_ptr->pckup_str_name, cl_ptr->pckup_str_nbr, cl_ptr->pckup_apt_room);
  add_outb_text(dummy);
  add_outb_text(CR);

  /******************** LINE   5 *********************/
  if (strlen(cl_ptr->pckup_adr_cmnt))
    {
      add_outb_text(cl_ptr->pckup_adr_cmnt);
      add_outb_text(CR);
    }

  /******************** LINE   6 *********************/
  if (strlen(cl_ptr->dest_dmap) ||
      strlen(cl_ptr->dest_city) ||
      cl_ptr->dest_zone != 0 ||
      strlen(cl_ptr->dest_str_name))
    {
      if (cl_ptr->dest_zone > 0)
	sprintf(dummy, "L[MNA:%s %s %d END.SIGN",
		cl_ptr->dest_dmap, cl_ptr->dest_city, cl_ptr->dest_zone);
      else
	sprintf(dummy, "L[MNA: %s %s END.SIGN",
		cl_ptr->dest_dmap, cl_ptr->dest_city);
      add_outb_text(dummy);
      add_outb_text(CR);
    }
  else
    {
      sprintf(dummy, "END.SIGN");
      add_outb_text(dummy);
      add_outb_text(CR);
    }
  

  /******************* LINE    7 ********************/
  if ((strlen(cl_ptr->dest_str_name)) || (cl_ptr->dest_str_nbr > 0)
      || (strlen(cl_ptr->dest_apt_room)))
    {
      if (cl_ptr->dest_str_nbr > 0)
	{
	  sprintf(dummy, "%s %d %s", cl_ptr->dest_str_name,
		  cl_ptr->dest_str_nbr, cl_ptr->dest_apt_room);
	  add_outb_text(dummy);
	  add_outb_text(CR);
	}
      else if ((cl_ptr->dest_str_nbr <= 0) && (strlen(cl_ptr->dest_str_name)))
	{
	  sprintf(dummy, "%s %s", cl_ptr->dest_str_name, cl_ptr->dest_apt_room);
	  add_outb_text(dummy);
	  add_outb_text(CR);
	}
    }

  /****************** LINE     8 *********************/
  if (strlen(cl_ptr->dest_adr_cmnt))
    {
      add_outb_text(cl_ptr->dest_adr_cmnt);
      add_outb_text(CR);
    }

  /***************** LINE 9/10  *********************/
  mk_general_comment(cl_ptr);

}
  
/**************************************************************************
 * Name: int mk_sp_details2( struct cisam_cl *cl_ptr)                     *
 * Type:                                                                  *
 *                                                                        *
 * Purpose: Make detail screen number 2                                   *
 *                                                                        *
 *                                                                        *
 * Returnvalue: On success : 1                                            *
 *              On failure : 0                                            *
 **************************************************************************/
int mk_sp_details2( struct cisam_cl *cl_ptr, int mult_num, int total_mult_num)
{

	int ii,len;
	char dummy[35]; /* MDT terminal line length (32) */
	char new_due_time[6];


	if ( cl_ptr->grouped_with == 0 )
	  total_mult_num = 1;
	
	if (total_mult_num > 3)
		total_mult_num = 1;

	mk_outb_text("");

	/********** LINE 1 ******************/
	if (cl_ptr->ext_nbr > 0)
	{
		memcpy(new_due_time,cl_ptr->pckup_adr_cmnt+20,5);
		new_due_time[5] = 0;

		ii = 19;
		while (cl_ptr->pckup_adr_cmnt[ii] != ' ')
		  if (--ii < 0) break;
		cl_ptr->pckup_adr_cmnt[++ii] = '\0';


		  sprintf(dummy,"%.10s %1d:%1d %s%d %s%%R",cl_ptr->phone_number, mult_num, total_mult_num,
			 CALL_NUMBER_HDR, cl_ptr->nbr, new_due_time);

	}
	else
	  {
	    
	    ii = 19;
	    while (cl_ptr->pckup_adr_cmnt[ii] != ' ')
	      if (--ii < 0) break;
	    cl_ptr->pckup_adr_cmnt[++ii] = '\0';
	    
	    sprintf(dummy,"%.10s %1d:%1d %s%d %s%%R",cl_ptr->phone_number, mult_num, total_mult_num,
			 CALL_NUMBER_HDR, cl_ptr->nbr, cl_ptr->due_time);
	  }

	add_outb_text(dummy);


	/********** LINE 2 ******************/
	sprintf(dummy, "NAMN:%.20s/%c BILJ", cl_ptr->passenger_name, cl_ptr->pckup_str_suf);
	if (strlen(dummy) < MAX_DISPLAY_LENGTH)
	  strcat(dummy,"%R");

	add_outb_text(dummy);

	/********** LINE 3 ******************/
	sprintf(dummy,"H[MTA: %s ","");
	add_outb_text(dummy);

	if (cl_ptr->dest_str_suf == '1')
		add_outb_text("RETUR");
	add_outb_text("%R");

	/********** LINE 4 ******************/

	sprintf(dummy,"%s %d %s%%R",cl_ptr->pckup_str_name, cl_ptr->pckup_str_nbr, cl_ptr->pckup_apt_room);
	add_outb_text(dummy);

	/********** LINE 5 ******************/
	sprintf(dummy,"%s %d %s%%R",cl_ptr->pckup_city, cl_ptr->pckup_zone, cl_ptr->pckup_adr_cmnt);
	add_outb_text(dummy);

	/********** LINE 6 ******************/
	sprintf(dummy,"L[MNA: %s%%R","");
	add_outb_text(dummy);


	/********** LINE 7 ******************/

	sprintf(dummy,"%s %d %s%%R",cl_ptr->dest_str_name, cl_ptr->dest_str_nbr, cl_ptr->dest_apt_room);
	add_outb_text(dummy);

	/********** LINE 8 ******************/
	sprintf(dummy,"%s %d %s%%R",cl_ptr->dest_city, cl_ptr->dest_zone, cl_ptr->dest_adr_cmnt);
	add_outb_text(dummy);


	/********** LINE 9-10 ***************/
	add_outb_text(cl_ptr->general_cmnt);

}

decrypt_receipt_data( ReceiptBuf, ITM_text )
     char   *ReceiptBuf;
     char   *ITM_text;
{
  int i,j,res;
  char exp_str[512];
  char data_str[512];
  char text[512];
  char card_validity_message[512];
  char card_name[512];
  char bcd[10];

  strcpy( text, ITM_text );
  i = 0;
  while(text[i] != 0) /* find length of 6 bit string */
    i++;
  i = expand(ReceiptBuf,text,i-4); /* ignore the last 4 end of string characters */

  /******************
   * get BCD string *
   ******************/
  j = 0;
  while (i && j< 200)
  {
    sprintf(&data_str[2*j],"%c%c", (ReceiptBuf[j] & 0xF0)/16+'0',(ReceiptBuf[j] & 0x0F)+'0');
    i--;
    j++;
  }
  data_str[2*j]=0;       /* terminate string */
  strcpy(ITM_text, text);
  strcpy(ReceiptBuf, data_str);
}

// Receive FTJ validity string in the form
// <header_data>:<ftj_nbr>
// extract <ftj_nbr> and verify against database table
int check_ftj_card_validity( char *data_str )
{
    int rc = 0;
    FTJ_BAD_STRUCT  ftj_rec;
    char    *pFTJ, *pFill;
    char *ctime(), *stime;
    time_t tmtime;
    char   DateBuff[16];
    FILE   *trace_logfile;      
    time_t start_date_time = 0;

    // if we can't open FTJ table approve card
    if ((rc = db_open(FTJ_BAD_FILE_ID, ISINOUT + ISMANULOCK)) != SUCCESS )
	return(1);

#ifdef FOO
    if ((trace_logfile = fopen("/usr/taxi/run/traces/ftj_valid_trace", "a")) != NULL)
	{
	    tmtime = time((long *) 0);
	    get_arg_date(tmtime, DateBuff);
	    stime = ctime(&tmtime);
	    fprintf(trace_logfile, "%.2s", DateBuff);
	    fprintf(trace_logfile, "%.2s", &DateBuff[3]);
	    fprintf(trace_logfile, "%.2s ", &DateBuff[6]);
	    fprintf(trace_logfile, "%.2s", &stime[11]);
	    fprintf(trace_logfile, "%.2s", &stime[14]);
	    fprintf(trace_logfile, "%.2s", &stime[17]);
	    fprintf(trace_logfile, " VALIDATING FTJ DATA\tRECVD: %s\n", data_str);
	}
#endif
  
    bzero( &ftj_rec, sizeof( FTJ_BAD_STRUCT ) );

    pFTJ = (char *)strrchr( data_str, ':' );
    if ( pFTJ == NULL )  // bad format in request string
	{
	    db_close( FTJ_BAD_FILE_ID );
#ifdef FOO
	    if ( trace_logfile != NULL )
		{
		    fprintf(trace_logfile, "               BAD FORMAT IN REQUEST - REJECT!\n");
		    fclose( trace_logfile );
		}
#endif
	    return(-1);
	}
    else
	{
          pFTJ = (char *)(pFTJ + 1);  // skip the ':' delimiter

            
	    if ( pFTJ == NULL )
		{
		    db_close( FTJ_BAD_FILE_ID );
#ifdef FOO
		    if ( trace_logfile != NULL )
			{
			    fprintf(trace_logfile, "               BAD FORMAT IN REQUEST - REJECT!\n");
			    fclose( trace_logfile );
			}
#endif
		    return(-1);
		}
	    else
		{
                  pFill = (char *)strchr(pFTJ, '?');  // '?' is a filler character
                  if ( pFill != NULL )
                    *pFill = '\0';            

#ifdef FOO
		    if ( trace_logfile != NULL )
			{
			    fprintf( trace_logfile, "               VALIDATING FTJ NBR %s\n", pFTJ );
			}
#endif                    
                    
		    strcpy( ftj_rec.ftj_number, pFTJ );
		    if ( (rc = db_read_key( FTJ_BAD_FILE_ID, &ftj_rec, &ftj_bad_key1, ISEQUAL ) ) == SUCCESS )
			{
			    db_close( FTJ_BAD_FILE_ID );
#ifdef FOO
			    if ( trace_logfile != NULL )
				{
				    fprintf(trace_logfile, "               FTJ NBR FOUND START DATE %s\n", ftj_rec.ftj_start_date);
				}
#endif
			    // Check start date against current date
			    get_date_time_from_text( &start_date_time, ftj_rec.ftj_start_date, "00:01" );
			    if ( start_date_time < mads_time )
				{
#ifdef FOO
				    fprintf(trace_logfile, "               START DATE IS IN PAST - REJECT %s\n", ftj_rec.ftj_start_date);
				    fclose( trace_logfile );
#endif
				    return(-1);
				}
			    else
				{
#ifdef FOO
				    fprintf(trace_logfile, "               START DATE IS IN FUTURE - APPROVE %s\n", ftj_rec.ftj_start_date);
				    fclose( trace_logfile );
#endif
				    return(1);
				}				    
			}
#ifdef FOO
		    if ( trace_logfile != NULL )
			{
			    fprintf( trace_logfile, "               FTJ NBR not found in table - APPROVE\n" );
			    fclose( trace_logfile );
			}
#endif
		    db_close( FTJ_BAD_FILE_ID );
		    return(1);
		}
	}
    

}