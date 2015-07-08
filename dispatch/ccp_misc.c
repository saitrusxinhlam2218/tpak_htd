static char sz__FILE__[]="@(#)ccp_misc.c	10.1.2.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  ccp_misc.c; Rel: 10.1.2.1; Get date: 5/11/95 at 10:13:28
 * @(#)  Last delta: 12/2/94 at 18:53:08
 * @(#)  SCCS File: /taxi/sccs/s.ccp_misc.c
 *                                                                           *
 *****************************************************************************/
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/time.h>
#include "STD.h"
#include "acc.h"
#include "msg.h"
#include "mads_types.h"
#include "veh_driv.h"
#include "mad_error.h"
#include "mads_isam.h"
#include "except.h"
#include "language.h"
#include "itm.h"
#include "ccp_strdefs.h"
#define  LIBRARY 1
#include "enhance.h"
#include "ccp_enh.h"

extern struct veh_driv *get_veh_id();
void BrdcstMsg(int nEvent);

int nEncrypt = 0;		/* Kdt Stats counters */
int nCCValidRq = 0;		
int nCCDataTrans = 0;
int ccp2tp_fd;			/* Pipe file descriptors between Credit Card process and TaxiPak */
int tp2ccp_fd;

 /**************************************************************************
  * Name: Open_Pipe(char *szPipeName) 			                   *
  * Type:                                                          *
  *                                                                        * 
  * Purpose: Open the named pipe 'szPipeName' 			           * 
  *           		   		 				   *
  *                                                                        *
  * Returnvalue: On success : Valid file descriptor.	                   *
  *              On failure : No return. Function cleans up.               *
  **************************************************************************/
int Open_Pipe(char *szPipeName)
{
	int nFileDesc;
	char buf[MAX_CCP_DATA];

	if((nFileDesc = open(szPipeName, O_RDWR | O_NDELAY)) < 0 )	/* open pipe */ 
	{
		ERROR(' '," ",szPipeName);
		cleanup();
	}
	while( read(nFileDesc,buf,MAX_CCP_DATA) > 0);			/* Flush pipe */
	return(nFileDesc);
}


 /**************************************************************************
  * Name: void OpenCCPPipes(void)                                          *
  * Type: 	                                                           *
  *                                                                        * 
  * Purpose: Opens communication pipes to and from the Credit Card process *
  *                                                                        *
  * Returnvalue: None   				                   *
  **************************************************************************/
void OpenCCPPipes(void)
{
	ccp2tp_fd = Open_Pipe(CCP2TP_PIPE);
	tp2ccp_fd = Open_Pipe(TP2CCP_PIPE);
}

 /**************************************************************************
  * Name: void read_ccp_pipe(void)                                         *
  * Type:                                                         *
  *                                                                        * 
  * Purpose: Read messages from the credit card process and pass them on   * 
  *          to the ITM. Messages are validated. Invalid messages are      *
  *	     discarded. Incomplete messages are stored until next call of  *
  * 	     the function.    		 				   *
  *                                                                        *
  * Returnvalue: None   				                   *
  **************************************************************************/
void read_ccp_pipe(void)
{
	static UCHAR buf[MAX_CCP_DATA];			/* credit_card_pipe buffer */
        //  static char buf[MAX_CCP_DATA];			/* credit_card_pipe buffer */  
	static int poffset;				/* offset into the pipe buffer */
	static int bytes_remaining;			/* number of bytes remaining to be processed */
	int bytes_read;					/* bytes read from the ccp_pipe */
	int i;
	int tab_num;					/* number of tab characters read */
	int itm_msg_type;
	int msg_length;
	char term_id[ROUTE_ID_SIZE];			/* RFID in ASCI format */
	struct veh_driv *veh_ptr;
	UCHAR msg[MAX_CCP_DATA];
	extern int ccp2tp_fd;
	MsgInfo SemelMessage;
	char cMsgSubType[10];

	if(bytes_remaining != 0){				/* Bytes unprocessed from previous read ? */
		memcpy(buf,&buf[poffset],bytes_remaining);
		poffset = 0;					/* reset the offset */
	}

	if((bytes_read = read(ccp2tp_fd,&buf[bytes_remaining],MAX_CCP_DATA-bytes_remaining)) <= 0)		/* read the credit card process pipe */
	{
		bytes_remaining = 0;
		return;
	}
#ifdef PDEBUG
	printf("ccp message: %s\n",buf);
#endif

	bytes_read += bytes_remaining;							/* take into account stuff from previous read */
	bytes_remaining = bytes_read;

	while(poffset < bytes_read )
	{		
		if(GetSemelMsgInfo(&buf[poffset], bytes_read-poffset, &SemelMessage) == bFALSE)
			if (SemelMessage.state == INCOMPLETE)
				return;
			else
			{
				bytes_remaining = 0;			/* Discard buffer contents */
				poffset = 0;
				return;
			}
		switch(SemelMessage.nMsgNum)
		{
			case AS_UP:
				BrdcstMsg(AS_UP);
				poffset += SemelMessage.nSize;
				bytes_remaining -= SemelMessage.nSize;
				break;
			case AS_DWN:
				mk_except((struct calls *) NULL, (struct veh_driv *) NULL, 
					   SYS_ERR, AS_DOWN, -1);
				BrdcstMsg(AS_DWN);
				poffset += SemelMessage.nSize;
				bytes_remaining -= SemelMessage.nSize;
				break;
			case MSG_TO_CAR:
				strncpy(term_id, (char *)&buf[poffset + SemelMessage.nField[2]],SemelMessage.nFieldSize[2] );
				term_id[SemelMessage.nFieldSize[2]] = '\0';
				memcpy(msg, &buf[poffset + SemelMessage.nField[4]],SemelMessage.nFieldSize[4] );
				memcpy(msg + SemelMessage.nFieldSize[4], "%D%E", 4);
				msg[SemelMessage.nFieldSize[4]+4] = '\0';
				strncpy(cMsgSubType, (char *)&buf[poffset + SemelMessage.nField[1]],SemelMessage.nFieldSize[1] );
				cMsgSubType[SemelMessage.nFieldSize[1]] = '\0';
				itm_msg_type = hex2int(cMsgSubType,ITM_SUB_TYPE_LENGTH);
				if (itm_msg_type == NEW_KEYS)	/* Increment Stats-counter */
					nEncrypt++;
				veh_ptr = get_veh_id(term_id);
				itm_send_msg(term_id, veh_ptr, itm_msg_type, msg);
				poffset += SemelMessage.nSize;
				bytes_remaining -= SemelMessage.nSize;
				break;
			default:
				poffset += SemelMessage.nSize;
				bytes_remaining -= SemelMessage.nSize;
				break;
		} /* End switch */
	} /* end while */
	poffset = 0;						/* All messages processed */
	bytes_remaining = 0;
}


 /**************************************************************************
  * Name: int send_ccp_msg(MSG_QUEUE_HDR *msg)                             *
  * Type:                                                          *
  *                                                                        * 
  * Purpose: Message read from ITM is converted into the Semel format      * 
  *          and passed on to the credit card process.			   *
  *                                                                        *
  * Returnvalue: All characters written     : 1		                   *
  *              Not all characters written : -1         	           *
  **************************************************************************/
int send_ccp_msg(MSG_QUEUE_HDR *msg)
{
   char buf[MAX_CCP_DATA], tmp_str[MAX_CCP_DATA];
   int nMsgType;
   extern int tp2ccp_fd;
   int text_len = 0;
   int msg_len = 0;
   enum {
     T1, T2 } tstate;

   text_len = 0;
   bzero(buf, MAX_CCP_DATA);
   bzero(tmp_str, MAX_CCP_DATA);
   for ( tstate = T1;; )
     {
       if ( tstate == T1 )
	 {
	   if ( msg->text[text_len] == '%' )
	     tstate = T2;
	   ++text_len;
	 }
       else
	 {
	   if ( ( msg->text[text_len] == 'E' ) &&
		( msg->text[text_len + 1] == '\0' ) &&
		( msg->text[text_len + 2] == '\0' ) )
	     {
	       ++text_len;
	       break;
	     }
	   else
	     {
	       ++text_len;
	       tstate = T1;
	     }
	 }
     }
   
   memcpy(buf,"CARQ", 4);					/* Message type */
   memcpy(buf+4,"\t", 1);
   memcpy(buf+5,msg->text,ITM_SUB_TYPE_LENGTH);		/* Message subtype */
   memcpy(buf+7,"\t", 1);
   memcpy(buf+8, msg->route_id,ROUTE_ID_SIZE);		/* Terminal ID */
   memcpy(buf+13,"\t", 1);
   sprintf(tmp_str,"%04d",text_len - NUM_ITM_CTRL_CHAR);
   memcpy(buf+14,tmp_str, 4);					/* Length of Message */
   memcpy(buf+18,"\t", 1);
   memcpy(tmp_str, (msg->text)+ITM_SUB_TYPE_LENGTH, text_len - NUM_ITM_CTRL_CHAR);
   text_len -= NUM_ITM_CTRL_CHAR;
   //   UnEscapeMsg(tmp_str, &text_len);
   memcpy(buf+19,tmp_str,text_len);	/* Message */
   memcpy(buf+19 + text_len,"\n", 1);
   msg_len = 19 + (text_len) + 1;
   strncpy(tmp_str,msg->text,ITM_SUB_TYPE_LENGTH);	/* Get Message Type */
   tmp_str[ITM_SUB_TYPE_LENGTH] = '\0';
   nMsgType = hex2int(tmp_str,ITM_SUB_TYPE_LENGTH);
   if (nMsgType == TRIPFIFO_DATA)			/* Increment Kdt Stats counter */
	nCCDataTrans++;
   else if (nMsgType == RQ_CARD)			/* Increment Kdt Stats counter */
	nCCValidRq++;

#ifdef PDEBUG
	 printf("Message from car: %s\n",buf);
#endif

   if ((write(tp2ccp_fd,buf,msg_len)) != msg_len)
      return(-1);					/* Not all characters of message written */
   else
      return(1);					/* All characters of message written */
} /* end send_ccp_msg */


int UnEscapeMsg( char *tmp_str, int *text_len )
{
  int i,j;
  char tmp_msg[4096];
  
  bzero(tmp_msg, 4096);

  j = 0;
  for ( i = 0; i < *text_len; i++ )
    {
      switch(*(tmp_str+i))
	{
	case '%' :
	  if ( *(tmp_str+i+1) != '%' )
	    *(tmp_msg+j++) = *(tmp_str+i);
	  break;
	default: *(tmp_msg+j++) = *(tmp_str+i);
	  break;
	}
    }
  memcpy(tmp_str, tmp_msg, j);
  *text_len = j;
}

 /**************************************************************************
  * Name: void AddCreditStats(FILE *nStatsFileDesc)                        *
  * Type:                                                          *
  *                                                                        * 
  * Purpose: Writes information concerning creditcard transactions into    * 
  *          file represented by 'nStatsFileDesc'                          * 
  *                                                                        *
  * Returnvalue: All characters written     : 1		                   *
  *              Not all characters written : -1         	           *
  **************************************************************************/
void AddCreditStats(FILE *nStatsFileDesc)
{
	extern int nEncrypt;
	extern int nCCValidRq;
	extern int nCCDataTrans;

	fprintf(nStatsFileDesc,"      Encryption Key Changes: %d, ",nEncrypt);
	fprintf(nStatsFileDesc,"Credit Card Validation Requests: %d, ", nCCValidRq);
	fprintf(nStatsFileDesc,"Credit Card Data Transmissions: %d\n", nCCDataTrans);
	nEncrypt = nCCValidRq = nCCDataTrans = 0;
}


 /**************************************************************************
  * Name: void BrdcstMsg(int nEvent)					   *
  * Type:                                                          *
  *                                                                        * 
  * Purpose: Broadcast a message to all vehicles, saying either that       * 
  *	     Credit card transactions are possible or not possible.        * 
  *                                                                        *
  * Returnvalue: None					                   *
  **************************************************************************/
void BrdcstMsg(int nEvent)
{
	struct cisam_ml mlp;

	mlp.from = 9999;	
	mlp.supervisor_id = 9999;
	mlp.to_type = MSG_VEHICLE;
	mlp.to_selection[0] = '*';	/* All vehicles */
	mlp.to_selection[1] = '\0';
	mlp.date_time_sent = time(0);
	mlp.fleet = '*';		/* All fleets */
	switch(nEvent)
	{
		case AS_UP:
			strcpy(mlp.msg_line1,catgets(CCP_catd,CCP_SET,AS_UP_MSG,AS_UP_DEFAULT));
			disp_msg_allveh(&mlp);
			break;
		case AS_DWN:
			strcpy(mlp.msg_line1,catgets(CCP_catd,CCP_SET,AS_DOWN_MSG,AS_DOWN_DEFAULT));
			disp_msg_allveh(&mlp);
			break;
	}
}
