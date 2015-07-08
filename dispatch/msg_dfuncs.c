static char sz__FILE__[]="@(#)msg_dfuncs.c	10.1.2.1(ver) Date Release 12/2/94" ;
/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  msg_dfuncs.c; Rel: 10.1.2.1; Get date: 5/11/95 at 10:15:33
* @(#)  Last delta: 12/2/94 at 18:29:07
* @(#)  SCCS File: /taxi/sccs/s.msg_dfuncs.c
*                                                                           *
*****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <isam.h>
#include <math.h>
#include <sys/types.h>
#include <memory.h>

#include "taxipak.h"
#include "Object.h"
#include "List_public.h"
#include "dispatch_strdefs.h"
#include "sprintf.h"
#include "path.h"
#include "mads_types.h"
#include "ex_key.h"
#include "mads_isam.h"
#include "mad_error.h"
#include "ui_ipc.h"
#include "kdt.h"
#include "veh_driv.h"
#include "df_maint.h"
#include "except.h"
#include "mesg.h"
#include "timers.h"
#include "Timer_public.h"
#include "Zone_lists_public.h"
#include "msg_mmp.h"
#include "language.h"
#include "tmplen.h"
#include "enhance.h"
#include "switch_ext.h"
#include "itm.h"

extern struct fleets *fleet[];							/* pointer to fleets */
extern int veh_used;								/* number of vehicles in use */
extern char req_buf[];								/* request buffer for db */
extern int iserrno;                                                             /* c-isam global errno */
extern void local_date_time();

char frmt_outpt[MMP_TEXT_SIZE];								/* formatted output for taxis */
struct cisam_ml ml;								/* message log structure to copy data to */

struct veh_driv *get_veh_ptr();						/* get pointer to vehicle structure */
char *ctime();


/***********************************************************************************************************************************
disp_send_mesg -
This procedure is called by dispatch after a message has been read from the USER INTERFACE pipe and it has been determined that
it was a driver message notification.  The repeat flag is checked to see if this is a normal outbound message, a repeat message,
an update to a repeat message, or a deletion of a repeat message.  The proper function is called to perform the task.
***********************************************************************************************************************************/
disp_send_mesg(dmp)
struct drv_msg *dmp;								/* driver message structure from User Interface */
{
  struct veh_driv *veh_ptr;
  short fleet_nbr;
  
  switch (dmp->msg_flag)
    {
    case MSG_LOG:
      disp_drv_mesg(dmp->msg_nbr, dmp->dispatch_flag);/* normal message to driver */
      break;
      
    case MSG_RPT:
      disp_rpt_msg(dmp->msg_nbr);			/* new repeat message */
      break;
      
    case MSG_UPD:
      disp_rpt_upd(dmp->msg_nbr);			/* update repeat message */
      break;
      
    case MSG_DEL:
      disp_rpt_del(dmp->msg_nbr);			/* delete repeat message */
      break;
      
    default:
      ERROR(' ',"","Invalid repeat code frou UI");	/* unknown value for repeat code */
      break;
    }
}


/***********************************************************************************************************************************
disp_drv_mesg -
This procedure is called to send a message to a driver or group of drivers.  The message log record is read and the message is
sent to all vehicles that match the destination specifications.
***********************************************************************************************************************************/
disp_drv_mesg(msg_nbr, from_call_taker)
int msg_nbr;									/* driver message structure from User Interface */
int from_call_taker;
{
    struct cisam_ml *ml_ptr;							/* pointer to message log buffer */
    FLEET_HNDL fleet_hndl;
    int    veh_num;
    struct veh_driv *veh_ptr;

    ml_ptr = (struct cisam_ml *)req_buf;					/* initialize pointer to request buffer */

    bzero( req_buf, BUFSIZ );
    ml_ptr->nbr = msg_nbr;							/* fill in key with message number */
    if (db(MSGLOG_FILE_ID,READ_KEYS,&ml_key1,ISEQUAL,0) < 0) {			/* read message log record */
	ERROR(' ',MSGLOG_FILE,"Error reading record from msg_nbr");		/* if problem, ERROR */
	return;
    }

    if (from_call_taker)
    {
	/* Determine if message to drivers from call takers are automatically sent,
	   or generate an exception for approval */
      (void) Fleet_get_hndl(BY_ID, ml_ptr->fleet, &fleet_hndl);
      
      if ((fleet_hndl != HNDL_ERR) &&
          (short) Fleet_get_value(fleet_hndl, FLEET_APPROVE_MSGS_TO_DRIVERS) == 1)
	{
	  veh_num = atoi(ml_ptr->to_selection);
	  Veh_get_hndl(BY_NBR, veh_num, fleet_hndl->fleet_nbr,(VEH_HNDL *)&veh_ptr);
	  mk_except((struct calls *) NULL, (struct veh_driv *) NULL, DRV_MSG, msg_nbr, 0);
	  return;
	}
    }

    printf("Reached send_msg call %s\n", ml_ptr->msg_line1);
    memcpy(&ml,req_buf,sizeof(struct cisam_ml));				/* copy info from request buffer to structure */
    send_msg(&ml);								/* send message to destinations */
}



/***********************************************************************************************************************************
disp_rpt_msg -
This procedure is called to send a repeat message to a driver or group of drivers.  The repeat message record is read and the
message is sent to all vehicles that match the destination specifications.  A timer event is also created to expire when the
message is scheduled to be broadcast again.
***********************************************************************************************************************************/
disp_rpt_msg(msg_nbr)
int msg_nbr;									/* driver message structure from User Interface */
{
    int x,y;									/* temp values for time */
    struct cisam_rm *rm_ptr;							/* pointer to repeat message pointer */
    short fleet_nbr;
    char the_fleet;

    rm_ptr = (struct cisam_rm *)req_buf;					/* initialize pointer to request buffer */
    if (read_rptmsg(msg_nbr) < 0)						/* did we get valid repeat message? */
	return;
    x = (rm_ptr->how_often * 60);						/* get next time message should be sent */
    y = rm_ptr->until_dt_tm;							/* get expiration time of message */

    /* Save fleet since we'll lose it due to global req_buf */
    the_fleet = rm_ptr->fleet;

    rm_to_ml(rm_ptr);								/* copy data to message log structure */
    ml.nbr = msg_nbr;
    send_msg(&ml);								/* send message to destination */
    if ((get_time() + x) < y){
	  (void) Timer_create(
			      TIMER_TYPE, T_REP_MSG,
			      TIMER_EXPIRE_TIME, x,
			      TIMER_NOTIFY_FUNC, disp_rpt_msg,
			      TIMER_DATA, msg_nbr,
			      ATTRIBUTE_END);
    }
}



/***********************************************************************************************************************************
disp_rpt_upd -
This procedure is called to update a repeat message to a driver or group of drivers.  The repeat message chain is scanned to 
delete the old timer event.  The message is sent out to the appropriate destination(s) and a new timer event is submitted
for the next broadcast for this message.
***********************************************************************************************************************************/
disp_rpt_upd(msg_nbr)
int msg_nbr;									/* driver message structure from User Interface */
{
    Timer_delete_rptmsg( msg_nbr );
    disp_rpt_msg(msg_nbr);							/* send this message and re-submit timer entry */
    return(1);
}



/***********************************************************************************************************************************
disp_rpt_del -
This procedure is called to delete a repeat message to a driver or group of drivers.  The repeat message chain is scanned to 
find and delete the old timer event.
***********************************************************************************************************************************/
disp_rpt_del(msg_nbr)
int msg_nbr;									/* driver message structure from User Interface */
{
    Timer_delete_rptmsg( msg_nbr );
    return(1);
}



/***********************************************************************************************************************************
read_rptmsg -
This procedure is called to read a repeat message record from disk.  The message number to be used as the key is passed as a
parameter.   The message is read into the request buffer.
***********************************************************************************************************************************/
read_rptmsg(mnbr)
int mnbr;									/* repeat message number */
{
    struct cisam_rm *rm_ptr;							/* pointer to repeat message buffer */

    rm_ptr = (struct cisam_rm *)req_buf;					/* initialize pointer to request buffer */
    rm_ptr->msg_nbr = mnbr;							/* fill in key with message number */
    if (db(RPTMSG_FILE_ID,OPEN,&rm_key1,ISINOUT+ISMANULOCK,0) < 0) {		/* open repeat message file */
	ERROR(' ',RPTMSG_FILE,"Error opening file");				/* if problem, ERROR */
	return(-1);
    }
    if (db(RPTMSG_FILE_ID,READ_KEYS,&rm_key1,ISEQUAL,0) < 0) {			/* read repeat message record */
	ERROR(' ',RPTMSG_FILE,"Error reading record from msg_nbr");		/* if problem, ERROR */
	db(RPTMSG_FILE_ID,CLOSE,0,0);
	return(-1);
    }
    db(RPTMSG_FILE_ID,CLOSE,0,0);
    return(0);
}



/***********************************************************************************************************************************
rm_to_ml -
This procedure is called to copy data from a repeat message buffer to the message log structure which is defined globally.
***********************************************************************************************************************************/
rm_to_ml(rm_ptr)
struct cisam_rm *rm_ptr;							/* pointer to repeat message buffer */
{
    get_asc_time(rm_ptr->created_dt_tm,ml.date_sent,ml.time_sent);		/* convert created time to ascii */
    ml.date_time_sent = time(0);
    ml.from = rm_ptr->created_by;						/* copy info from repeat message to messag log */
    ml.to_type = rm_ptr->to_type;						/* "    "    "    "      "       "  "      "   */
    ml.fleet = rm_ptr->fleet;							/* "    "    "    "      "       "  "      "   */
    strncpy(ml.to_selection,rm_ptr->to_selection,6);				/* "    "    "    "      "       "  "      "   */
    sprintf(ml.msg_line1,"%s", rm_ptr->line_1);					/* "    "    "    "      "       "  "      "   */
    sprintf(ml.msg_line2,"%s", rm_ptr->line_2);					/* "    "    "    "      "       "  "      "   */
    strncpy(ml.veh_attr, rm_ptr->veh_attr, 32);
    strncpy(ml.drv_attr, rm_ptr->drv_attr, 32);
    ml.veh_attr_set = rm_ptr->veh_attr_set;
    ml.drv_attr_set = rm_ptr->drv_attr_set;
}



/***********************************************************************************************************************************
send_msg -
This procedure is called to send the message text to the proper destination(s).  The to_type and to_selection fields are
checked to determine the proper recipients for this message.
***********************************************************************************************************************************/
send_msg(mlp)
struct cisam_ml *mlp;								/* pointer to message log buffer */
{
    switch (Toupper(mlp->to_type)) {							/* what kind of message is this? */
	case '*':	disp_msg_allveh(mlp);					/* to all */
			break;

	case MSG_VEHICLE:
	  disp_msg_taxi(mlp);
	  break;
	  
    case 'Q': // MPK generated message
	  disp_msg_taxi_mpk(mlp);					/* to specific taxi */
			break;

	case MSG_DRIVER:	disp_msg_driver(mlp);					/* to specific driver */
			break;

	case MSG_ZONE:	disp_msg_zone(mlp);					/* to specific zone */
			break;

	default:	ERROR(' ',"","Send msg - invalid type code from UI");		/* invalid type code */
			break;
    }
}



/***********************************************************************************************************************************
stcp -
This procedure is called to copy strings without copying a '\0' character at the end to pad.   The number of characters actually
copied is returned.
***********************************************************************************************************************************/
stcp(cp1,cp2,clen)
char *cp1;									/* destination string */
char *cp2;									/* source string */
int clen;									/* max length to copy */
{
    int i;

    for(i=0; i<clen; i++) {							/* repeat for all characters */
	if (cp2[i] == '\0')							/* is source string finished? */
	    break;								/* yes, stop here */
	else
	    cp1[i] = Toupper(cp2[i]);							/* copy this character */
    }
    return(i);
}

/***********************************************************************************************************************************
find_str-
This procedure is called to find a string of character in another string.  The pointer to character where the substring locates
in the source string is returned.
***********************************************************************************************************************************/
char *
find_str(s1,s2)
char *s1,*s2;
{
int len;
char *p;

len = strlen(s2);
for (p=s1;*p!='\0';p++) {
    if (!strncmp(p,s2,len))
        return(p);      /* found the substring */
    }

return((char *)0);      /* substring not present in the source string */
}

char * find_last_str(s1,s2)
char *s1,*s2;
{
	int len;
	char *p;

	len = strlen(s2);
	for (p=s1+strlen(s1)-1;p != s1; p--)
	{
		if (!strncmp(p,s2,len))
			return(p);
	}
	return((char *)0);
}

/***********************************************************************************************************************************
format_msg -
This procedure is called to format a message from a message log structure into a single character string terminated by a '\0'
character.
***********************************************************************************************************************************/
format_msg(mlp)
struct cisam_ml *mlp;								/* pointer to message log buffer */
{
    int x;									/* offset from start of formatted output */
    char *outchp;								/* roving pointer to output buffer */
		static char tmp[TMP_STR_LEN];               /* temporary buffer for printing */
		static char tmp_msg[MMP_TEXT_SIZE+TMP_STR_LEN],*p;
    char * word_break();
    char * find_str();
    char *p2;
    int count = 0;
    long t = 0;
    char msg_date[16];
    char msg_time[16];
    char *stime;
    char  cur_dt[10];
    char  cur_tm[7];
    char *CR_ptr;

    bzero( frmt_outpt, MMP_TEXT_SIZE );
    outchp = frmt_outpt;							/* initialize pointer to start of output buffer */

    x = stcp(outchp,FLEET_HDR,12);						/* put string */
    outchp += x;

    sprintf(tmp,"%c",mlp->fleet);                                               /* get fleet id */

    x = stcp(outchp,tmp,8);                                                     /* print fleet id */
    outchp += x;

    x = stcp(outchp,FROM_HDR,12);						/* put string */
    outchp += x;

    sprintf(tmp,"%d",mlp->from);						/* get originators id */

    x = stcp(outchp,tmp,8);							/* print originators id */
    outchp += x;

    x = stcp(outchp,TO_HDR,12);
    outchp += x;

    /* warning note - apparently to_selection has been known  */
    /* on some occasions to be unterminated - may want to add */
    /* code to check for this                                 */

    if ( strlen( mlp->to_selection ) == 6 )
      sprintf(tmp,"%c%s%s",mlp->to_type,mlp->to_selection, CR);
    else
      sprintf(tmp,"%c %s%s",mlp->to_type,mlp->to_selection, CR);
    

    x = stcp(outchp,tmp,13);                                                    /* print originators id */
    outchp += x;

    strcpy(tmp_msg,mlp->msg_line1);						/* Removed space between line to fix TC0081 */
    strcat(tmp_msg," ");
    strcat(tmp_msg,mlp->msg_line2);
    
    strcpy(tmp_msg,word_break(tmp_msg,MAX_DISPLAY_LENGTH,CR));

    
    p = tmp_msg;
    while ((p2 = find_str(p, "%R")) != NULL)   /*this find_str command was originally strstr() but 3.1 ultrix doesn't have it*/
						/* ultrix 4.2a does.*/
    {
	p2++;				/*bump past found string to look for next*/
	p = p2; 
	count++;
    }
    if (count > 8)
    {
	count = 0;
	do
	{

		if ((p = find_last_str(tmp_msg, "%R"))!=NULL)
		{
			*p++= '\0';
			*p++= ' ';
			count++;
			strcat(tmp_msg, p);
		}
		else
			break;
	} while(strlen(find_last_str(tmp_msg, "%R")) > (MAX_DISPLAY_LENGTH*count));
    }


    x = stcp(outchp,tmp_msg,strlen(tmp_msg));					/* put line 1 */
    outchp += x;
    
    /** This section will add a date/time stamp to each message **/
    /** Add this in when authorized to do so **/
    CR_ptr = (outchp -2);
    t = mlp->date_time_sent;
    get_srch_date(t, msg_date);
    get_srch_time(t, msg_time);

    if (strncmp(CR_ptr, CR, 2) == 0)
      sprintf(tmp, "%s %s %s ", msg_date, msg_time, CR);
    else
      sprintf(tmp, "%s%s %s %s ", CR, msg_date, msg_time, CR);
    strcpy(outchp, tmp);
    outchp += 17;

    *outchp = '\0';								/* end with '\0' */
}

format_mpk(mlp)
struct cisam_ml *mlp;								/* pointer to message log buffer */
{
    int x;									/* offset from start of formatted output */
    char *outchp;								/* roving pointer to output buffer */
    static char tmp[TMP_STR_LEN];               /* temporary buffer for printing */
    static char tmp_msg[MMP_TEXT_SIZE+TMP_STR_LEN];


    bzero( frmt_outpt, MMP_TEXT_SIZE );    
    outchp = frmt_outpt;							/* initialize pointer to start of output buffer */

    bzero(tmp_msg, MMP_TEXT_SIZE+TMP_STR_LEN);
    strcpy(tmp_msg,mlp->msgtext);						/* Removed space between line to fix TC0081 */

    if ( strstr(tmp_msg, "%.L") )
      tmp_msg[strstr(tmp_msg, "%.L") - tmp_msg + 10] = '\0';
    printf("In format_mpk #0: %s\n", tmp_msg);
    x = stcp(outchp,tmp_msg,strlen(tmp_msg));					/* put line 1 */
    outchp += x;
    printf("In format_mpk #1: %s\n", frmt_outpt);
    *outchp = '\0';								/* end with '\0' */
    printf("In format_mpk #2: %s\n", frmt_outpt);    
}

/***********************************************************************************************************************************
disp_msg_allveh -
This procedure is called to send a message to all vehicles.  All fleets send out the message in a fleetwide broadcast.
***********************************************************************************************************************************/
disp_msg_allveh(mlp)
struct cisam_ml *mlp;								/* pointer to message log buffer */
{
    int i, len ,attributes_set = 0;
    char                scratch_buf[1024];
    char                scratch2_buf[1024];
    char                attr_buf[96];

    unsigned char       attribute_bits[11];
    unsigned char       *s, *d, ii;
    

    bzero( attribute_bits, sizeof(attribute_bits) );
    bzero( scratch_buf, sizeof( scratch_buf ) );
    bzero( attr_buf, 96 );
    
    attributes_set = 0;

    strcpy( attr_buf, mlp->veh_attr );
    strcat( attr_buf, mlp->drv_attr );
    

    format_msg(mlp);

    if (1)
      {
	d = attribute_bits;
	s = (unsigned char *)attr_buf;

	for (i = 0, *d = 0; i<64; s += 1 )
	  {
	    *d = *d<<1;
	    if ( *s == YES )
	      {
		*d |= 0x01;
		attributes_set = 1;
	      }
	    if (!(++i % 6)) *++d=0;
	  }
	*d = *d<<2;
      }

    for (i = 0; i < 11; i++)
      {
	ii = (unsigned char)attribute_bits[i];
	attribute_bits[i] = ii + ' ';
      }
    attribute_bits[11] = '\0';
    if ( ( attributes_set ) && (1) )
      {
	// EscapePercent( dest_buf );

	sprintf( scratch_buf, "%s%s%s%s%s  ", INVISIBLE, attribute_bits, "***", INVISIBLE, SCREEN_COORD );
	/* INVISIBLE = %I SCREEN_COORD = %C */

	/* save the formatted output already created */
	strcpy( scratch2_buf, frmt_outpt );

	/* add the AMD header to the message */
	strcpy( frmt_outpt, scratch_buf );

	/* put back the formatted output previously saved */
	strcat( frmt_outpt, scratch2_buf );
	
      }

    
    for(i = 0; i < FLEET_MAX; i++)
      {
	if (fleet[i] != NULL && fleet[i]->active == YES &&
	    (mlp->fleet == '*' || mlp->fleet == fleet[i]->fleet_id))
	  send_fleetwide(i, frmt_outpt);
      }
}


/***********************************************************************************************************************************
disp_msg_taxi -
This procedure is called to send a message to a specific taxi.
***********************************************************************************************************************************/
disp_msg_taxi(mlp)
struct cisam_ml *mlp;								/* pointer to message log buffer */
{
  int  i;
  int  taxi_num;
  struct veh_driv *veh_ptr;
  
  if (mlp->to_selection[0] == '*')
    {
      disp_msg_allveh(mlp);
      return;
    }

  if ( mlp->from == 8888 )
    format_mpk(mlp);
  else
    format_msg(mlp);

  taxi_num = atoi(mlp->to_selection);

  /*
    Find the fleet for the taxi. No messages can be sent to
    taxi #n in all fleets that have a taxi #n.
    */
  for(i = 0; i < FLEET_MAX; i++)
    {
      if (fleet[i] != NULL && fleet[i]->active == YES &&
	  mlp->fleet == fleet[i]->fleet_id)
	{
	  veh_ptr = get_veh_ptr(taxi_num, i);

	  if (veh_ptr != NULL)
	    {

	      if (SIGNED_ON(veh_ptr))
		{

		  if ( !strcmp( mlp->msg_line1, catgets(DISPATCH_catd, 1, 45, "SWITCH TO VOICE") ) )
		    {
		      send_text_predefined(veh_ptr->mid, GO_TO_VOICE, veh_ptr);
		      return;
		    }
		  send_pkt_id(MSG_PKT_TYPE, mlp->nbr,
			      (int)Veh_get_value((VEH_HNDL)veh_ptr, VEH_NBR));
		  if (send_text(veh_ptr->mid, frmt_outpt, veh_ptr, PRIORITY_3) != 0)
		    if (mlp->repeat == NO)
		      save_undeliv(mlp->nbr); /* didnt get out */
		  clr_pkt_id();

		  // If message originated in MPK we will also direct to ITM printer
#ifdef WAIT_FOR_DDS
		  if ( mlp->to_type == 'Q' )
		    itm_send_msg(veh_ptr->mid, veh_ptr, PRINT_TEXT, frmt_outpt);
#endif
		}
	      else if (mlp->repeat == NO)
		save_undeliv(mlp->nbr);

	    } /* end if (veh_ptr != NULL) */

	  /* Only one fleet to send a message to, so exit now */
	  break;

	} /* end if (fleet[i] != NULL ... ) */

    } /* end for */
}

/***********************************************************************************************************************************
disp_msg_taxi_mpk -
This procedure is called to send a message to a specific taxi.
***********************************************************************************************************************************/
disp_msg_taxi_mpk(mlp)
struct cisam_ml *mlp;								/* pointer to message log buffer */
{
  int  i;
  int  taxi_num;
  struct veh_driv *veh_ptr;
  
  if (mlp->to_selection[0] == '*')
    {
      disp_msg_allveh(mlp);
      return;
    }

  if ( mlp->from == 8888 )
    format_mpk(mlp);
  else
    format_msg(mlp);

  taxi_num = atoi(mlp->to_selection);

  /*
    Find the fleet for the taxi. No messages can be sent to
    taxi #n in all fleets that have a taxi #n.
    */
  for(i = 0; i < FLEET_MAX; i++)
    {
      if (fleet[i] != NULL && fleet[i]->active == YES &&
	  mlp->fleet == fleet[i]->fleet_id)
	{
	  veh_ptr = get_veh_ptr(taxi_num, i);
	  if (veh_ptr != NULL)
	    {
	      if (SIGNED_ON(veh_ptr))
		{
		  if ( !strcmp( mlp->msg_line1, catgets(DISPATCH_catd, 1, 45, "SWITCH TO VOICE") ) )
		    {
		      send_text_predefined(veh_ptr->mid, GO_TO_VOICE, veh_ptr);
		      return;
		    }
		  send_pkt_id(MSG_PKT_TYPE, mlp->nbr,
			      (int)Veh_get_value((VEH_HNDL)veh_ptr, VEH_NBR));
		  printf("before send_text frmt_outpt=%s\n", frmt_outpt);
		  if (send_text(veh_ptr->mid, frmt_outpt, veh_ptr, PRIORITY_3) != 0)
		    if (mlp->repeat == NO)
		      save_undeliv(mlp->nbr); /* didnt get out */
		  clr_pkt_id();

		  // If message originated in MPK we will also direct to ITM printer
#ifdef WAIT_FOR_DDS
		  if ( mlp->to_type == 'Q' )
		    itm_send_msg(veh_ptr->mid, veh_ptr, PRINT_TEXT, frmt_outpt);
#endif
		}
	      else if (mlp->repeat == NO)
		save_undeliv(mlp->nbr);

	    } /* end if (veh_ptr != NULL) */

	  /* Only one fleet to send a message to, so exit now */
	  break;

	} /* end if (fleet[i] != NULL ... ) */

    } /* end for */
}



/***********************************************************************************************************************************
disp_msg_driver -
This procedure is called to send a message to a specific driver.
***********************************************************************************************************************************/
disp_msg_driver(mlp)
struct cisam_ml *mlp;								/* pointer to message log buffer */
{
  int i;
  int driver_num;
  int rc;
  struct cisam_dr *dr_ptr;
  struct veh_driv *veh_ptr;
  
  if (mlp->to_selection[0] == '*')
    {
      disp_msg_allveh(mlp);
      return;
    }
  
  dr_ptr = (struct cisam_dr *)req_buf;						/* initialize pointer to request buffer */
  format_msg(mlp);								/* format message for output to kdts */
  driver_num = atoi(mlp->to_selection);

  /*
    Find the fleet for the driver. No messages can be sent to
    driver #n in all fleets that have a driver #n.
    */
  for (i = 0; i < FLEET_MAX; i++)
    {
      if (fleet[i] != NULL && fleet[i]->active == YES &&
	  mlp->fleet == fleet[i]->fleet_id)
	{
	  dr_ptr->fleet = fleet[i]->fleet_id;
	  dr_ptr->id = driver_num;

	  /* Find the driver record */
	  rc = db(DRIVER_FILE_ID, READ_KEYS, &dr_key1, ISEQUAL, 0);
	  if (rc >= 0)
	    {
	      /* is this driver signed on? */
	      if (dr_ptr->signed_on_dt_tm > dr_ptr->signed_of_dt_tm)
		{
		  veh_ptr = get_veh_ptr(dr_ptr->last_veh_nbr, i);
		  /* We do SIGNED_ON here since the above test on the driver record is not
		     valid if dispatch dies before a driver signs off */
		  if (SIGNED_ON(veh_ptr))
		    {
		      send_pkt_id(MSG_PKT_TYPE, mlp->nbr,
				  (int)Veh_get_value((VEH_HNDL)veh_ptr, VEH_NBR));
		      if (send_text(veh_ptr->mid, frmt_outpt, veh_ptr, PRIORITY_3) != 0)
			if (mlp->repeat == NO)
			  save_undeliv(mlp->nbr);   /* didnt get out */
		      clr_pkt_id();
		    }

		  /* No vehicle assigned to driver now */
		  else if (mlp->repeat == NO)
		    save_undeliv(mlp->nbr);
		}

	      /* Driver not signed on now */
	      else if (mlp->repeat == NO)
		save_undeliv(mlp->nbr);
	    }

	  /* Only one fleet to send a message to, so exit now */
	  break;

	} /* end if (fleet[i] != NULL ...) */

    } /* end for */
}



/***********************************************************************************************************************************
disp_msg_zone -
This procedure is called to send a message to a specific zone.
***********************************************************************************************************************************/
disp_msg_zone(mlp)
struct cisam_ml *mlp;								/* pointer to message log buffer */
{
  int  i, len;
  int  zone_num;

  char           scratch_buf[1024];
  char           scratch2_buf[1024];
  char           attr_buf[96];
  unsigned char  attribute_bits[11];
  unsigned char       *s, *d, ii;  

  bzero( attribute_bits, sizeof(attribute_bits) );  
  bzero( scratch_buf, sizeof( scratch_buf ) );
  bzero( attr_buf, 96 );

  if ( !(1) )
    {
      zone_num = atoi(mlp->to_selection);
      if (zone_num <= 0 || zone_num >= ZONE_MAX)
	return;
    }

  if (1)
    {
      strcpy( attr_buf, mlp->veh_attr );
      strcat( attr_buf, mlp->drv_attr );

      d = attribute_bits;
      s = (unsigned char *)attr_buf;

      for ( i = 0, *d = 0; i<64; s += 1 )
	{
	  *d = *d>>1;
	  if ( *s == YES )
	      *d |= 0x01;
	  if (!(++i % 6)) *++d=0;
	}
      *d = *d>>2;

      for ( i = 0; i < 11; i++ )
	attribute_bits[i] += ' ';
      
      // EscapePercent( dest_buf );
      sprintf( scratch_buf, "%s%s%3s%s%s  ", INVISIBLE, attribute_bits, mlp->to_selection, INVISIBLE, SCREEN_COORD );
    }

  format_msg(mlp);

  if (1)
  {
    strcpy( scratch2_buf, frmt_outpt );
    strcpy( frmt_outpt, scratch_buf );
    strcat( frmt_outpt, scratch2_buf ); 
  }

  for (i = 0; i < FLEET_MAX; i++)
    {
      if (fleet[i] != NULL && fleet[i]->active == YES &&
	  (mlp->fleet == '*' || mlp->fleet == fleet[i]->fleet_id))
	{
	  if (1)
	    send_fleetwide( i, frmt_outpt );
	  else
	    {
	      if (fleet[i]->zone_ptr[zone_num] != NULL)
		send_zonewide(fleet[i]->zone_ptr[zone_num]);
	    }
	}
    }
}



/***********************************************************************************************************************************
send_zonewide -
This procedure is called to output a message to a specific zone.
***********************************************************************************************************************************/
send_zonewide(zp)
struct zones *zp;								/* pointer to zone */
{
    struct veh_driv     *veh_ptr;
    ZONE_HNDL           hZone;
    VEH_HNDL            hVehicle;
    ZONE_VEH_LIST_HNDL  veh_list_hndl;

    if( zp == NULL )
      return;

    
    (void) Zone_get_hndl(zp->zone_nbr, zp->fleet_nbr, &hZone);
    veh_list_hndl = (ZONE_VEH_LIST_HNDL) Zone_get_value(hZone, ZONE_AVAIL_VEH_LIST);
    Zone_veh_list_each_get( veh_list_hndl, hVehicle )
      {
	veh_ptr = ( struct veh_driv * ) hVehicle;
	send_text(veh_ptr->mid,frmt_outpt,veh_ptr,PRIORITY_1);
      } Zone_veh_list_each_end(veh_list_hndl)
}


send_undeliv(veh_ptr)
struct veh_driv *veh_ptr;
{
struct cisam_mb *mb_ptr;
struct cisam_ml *ml_ptr;
int cc;
short msg_ctr;
char temp_buf[BUFSIZ];

    mb_ptr = (struct cisam_mb*)req_buf;

    /* counter for messages - driver and vehicle combined */
    msg_ctr = 0;

    /* Assume driver has undeliverable message first */
    mb_ptr->address = veh_ptr->driver_id + PREFIX_DRIV;
    if ((cc=db(MAILBOX_FILE_ID,READ_KEYS,&mb_key1,ISEQUAL,0)) < 0) {
	if ((iserrno != ENOREC) && (iserrno != EENDFILE)) {
	    ERROR(' ',MAILBOX_FILE,"Error reading file");
	    return;
	}
    }
    else {
	for (;((cc >= 0)&&(msg_ctr < MAX_NO_MSG));) {
	    if ((mb_ptr->to_type == MSG_DRIVER)
	      &&(mb_ptr->fleet == veh_ptr->fleet)
	      &&(atoi(mb_ptr->to_selection) == veh_ptr->driver_id)) {
		ml_ptr = (struct cisam_ml *)temp_buf;
		if(log_undeliv(veh_ptr,mb_ptr,ml_ptr) != 0)
		    return;
		msg_ctr++;
       	    }
	    if ((cc=db(MAILBOX_FILE_ID,READ_KEYS,&mb_key1,ISNEXT,0)) < 0)
	        if ((iserrno != ENOREC) && (iserrno != EENDFILE)) {
		    ERROR(' ',MAILBOX_FILE,"Error reading file");
		    return;
	        }
	} /* end for loop */

	if (msg_ctr >= MAX_NO_MSG) {
	    send_text_predefined(veh_ptr->mid,MORE_MSG,veh_ptr);
	    return;
        }
    } /* end of else */

    /* Look for undeliverable message intended for this vehicle */
    mb_ptr->address = veh_ptr->veh_nbr + PREFIX_VEH;
    if ((cc=db(MAILBOX_FILE_ID,READ_KEYS,&mb_key1,ISEQUAL,0)) < 0) {
	if ((iserrno != ENOREC) && (iserrno != EENDFILE)) {
	    ERROR(' ',MAILBOX_FILE,"Error reading file");
	    return;
	}
    }
    else {
	for (;(cc >= 0)&&(msg_ctr < MAX_NO_MSG);) {
	    if ((mb_ptr->to_type == MSG_VEHICLE)
	      &&(mb_ptr->fleet == veh_ptr->fleet)
	      &&(atoi(mb_ptr->to_selection) == veh_ptr->veh_nbr)) {
		ml_ptr = (struct cisam_ml *)temp_buf;
		if(log_undeliv(veh_ptr,mb_ptr,ml_ptr) != 0)
		    return;
		msg_ctr++;
	    }
	    if ((cc=db(MAILBOX_FILE_ID,READ_KEYS,&mb_key1,ISNEXT,0)) < 0)
	        if ((iserrno != ENOREC) && (iserrno != EENDFILE)) {
		    ERROR(' ',MAILBOX_FILE,"Error reading file");
		    return;
	        }
	} /* end for loop */

	if (msg_ctr >= MAX_NO_MSG) {
	    send_text_predefined(veh_ptr->mid,MORE_MSG,veh_ptr);
	    return;
	}
    } /* end else */

}


log_undeliv(vhptr,mbptr,mlptr)
struct veh_driv *vhptr;
struct cisam_mb *mbptr;
struct cisam_ml *mlptr;
{
short save_key;

    /* format the message for delivery */
    mlptr->from = mbptr->from;
    mlptr->date_time_sent = mbptr->date_time_sent;
    mlptr->supervisor_id = 0;
    strcpy(mlptr->date_sent,mbptr->date_sent);
    strcpy(mlptr->time_sent,mbptr->time_sent);
    mlptr->date_sent[9] = '\0';
    mlptr->time_sent[6] = '\0';
    mlptr->supervisor_saw = SUPER_APPROVED;
    mlptr->repeat = NO;
    mlptr->fleet = mbptr->fleet;
    mlptr->to_type = mbptr->to_type;
    strncpy(mlptr->to_selection,mbptr->to_selection,6);
    mlptr->to_selection[6] = '\0';
    strcpy(mlptr->msg_line1,mbptr->msg_line1);
    strcpy(mlptr->msg_line2,mbptr->msg_line2);
    mlptr->msg_line1[100] = '\0';
    mlptr->msg_line2[100] = '\0';

    /* send the message to the driver */
    format_msg(mlptr);
    send_text(vhptr->mid,frmt_outpt,vhptr,PRIORITY_1);

    save_key = mbptr->address;
    if (db(MAILBOX_FILE_ID,DELETE,&mb_key1,0,0) < 0) {
        ERROR(' ',MAILBOX_FILE,"Error deleting record");
        return(-1);
    }

    memcpy(mbptr,mlptr,sizeof(struct cisam_ml));

    if (db(MSGLOG_FILE_ID,ADD_REC,&ml_key1,0,0) < 0) {
        ERROR(' ',MSGLOG_FILE,"Error adding record");
        return(-1);
    }

    mbptr->address = save_key;

    return(0);
}

ml_to_mb(struct cisam_ml *ml_ptr,struct cisam_mb *mb_ptr)
{

    mb_ptr->from = ml_ptr->from;
    mb_ptr->date_time_sent = ml_ptr->date_time_sent;
    strcpy(mb_ptr->date_sent,ml_ptr->date_sent);
    strcpy(mb_ptr->time_sent,ml_ptr->time_sent);
    mb_ptr->date_sent[9] = '\0';
    mb_ptr->time_sent[6] = '\0';
    mb_ptr->status = NEW_MSG;
    mb_ptr->fleet = ml_ptr->fleet;
    mb_ptr->to_type = ml_ptr->to_type;
    strncpy(mb_ptr->to_selection,ml_ptr->to_selection, 6);
    mb_ptr->to_selection[6] = '\0';
    strcpy(mb_ptr->msg_line1,ml_ptr->msg_line1);
    strcpy(mb_ptr->msg_line2,ml_ptr->msg_line2);

    mb_ptr->msg_type = MTYPETEXT;  /* General status text */

    mb_ptr->address=atoi(mb_ptr->to_selection);
    if (mb_ptr->to_type == MSG_DRIVER)
	mb_ptr->address += PREFIX_DRIV;
    else if (mb_ptr->to_type == MSG_VEHICLE)
	mb_ptr->address += PREFIX_VEH;
}



save_undeliv(num)
long	num;
{
    struct cisam_ml *ml_ptr;                                                    /* pointer to message log buffer */
    struct cisam_mb *mb_ptr;                                                    /* pointer to message log buffer */
    char temp_buf[1500];

    ml_ptr = (struct cisam_ml *)req_buf;                                        /* initialize pointer to request buffer */
    mb_ptr = (struct cisam_mb *)temp_buf;                                        /* initialize pointer to request buffer */

    ml_ptr->nbr = num;                                                      /* fill in key with message number */
    if (db(MSGLOG_FILE_ID,READ_KEYS,&ml_key1,ISEQUAL,0) < 0) {                  /* read message log record */
        ERROR(' ',MSGLOG_FILE,"Error reading record from msg_nbr");             /* if problem, ERROR */
        return;
    }

    /* Don't save repeat messages */
    if (ml_ptr->repeat == YES)
      return;

    ml_to_mb(ml_ptr, mb_ptr);
    memcpy(req_buf,temp_buf,BUFSIZ);

   if (db(MAILBOX_FILE_ID, ADD_REC, 0, 0, 0) < 0) {
	ERROR(' ',MAILBOX_FILE,"Error adding record");				/* if problem, ERROR */
   }
}

/* set_bit: sets the bit specified by bit_num to 1. The size of the bit structure and its address 
 *		should be passed as arguments
 */
set_bit(bit_num,size,struct_ptr)
	int bit_num;	/* # of the bit to be tested; leaset significant = 0 */
	int size;	/* size of bit struct in bytes */
	unsigned char *struct_ptr;	/* pointer to the bit struct */
{
	unsigned char *bit_ptr;	/* poinetr to bit struct */
	unsigned char mask;
	int shift_by;	/* shift the mask by */

	mask = 0x80;
	bit_ptr = (unsigned char *)struct_ptr + bit_num/8;
	shift_by = bit_num % 8;
	mask >>= shift_by;

	*bit_ptr = *bit_ptr | mask;

}  /* end set_bit() */

int test_bit(bit_num,size,struct_ptr)
	int bit_num;	/* # of the bit to be tested; leaset significant = 0 */
	int size;	/* size of bit struct in bytes */
	unsigned char *struct_ptr;	/* pointer to the bit struct */
{
	unsigned char *bit_ptr;	/* poinetr to bit struct */
	unsigned char mask;
	int shift_by;	/* shift the mask by */

	bit_ptr = (unsigned char *)struct_ptr + bit_num/8;
	mask = 0x80;	/* start from bit 0 */
	shift_by = bit_num % 8;
	mask >>= shift_by;

	if ( *bit_ptr & mask )
		/* bit is set */
		return(1);
	else 
		/* bit is not set */
		return(0);

}  /* end test_bit() */
