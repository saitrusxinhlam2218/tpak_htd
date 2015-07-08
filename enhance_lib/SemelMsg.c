static char sz__FILE__[]="@(#)SemelMsg.c	10.1(ver) Date Release 12/2/94" ;
/****************************************************************************
*                                                                           *
*                           S C C S   I N F O                               *
*                                                                           *
* @(#)  SemelMsg.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:36:07		            *			
* @(#)  Last delta: 12/2/94 at 12:16:30						    *	
* @(#)  SCCS File: /taxi/sccs/s.SemelMsg.c							    *
*                                                                           *
* Copyright (c) 1994 - TaxiCom ApS					    *
*                                                                           *
*****************************************************************************/

#include <string.h>
#include "enhance_com.h"
#include "ccp_com.h"
#include "ccp_var.h"


 /**************************************************************************
  * Name: field                                			           *
  * Type: Function                                                         *
  *                                                                        * 
  * Purpose: Retrieve the field number 'num' from string, where the fields *
  *          are separated by the tab character				   *
  *                                                                        *
  * Returnvalue: On success : Valid pointer to text string.                *
  *              On failure : NULL                                         *
  **************************************************************************/
char *field(UCHAR *string, int num)
{
   static char tmp_str[512];
   static char *ret;
   int ant;
   
   ant = 1;
   strncpy((char *)tmp_str,(char *)string,512);
   ret = strtok(tmp_str,"\t");
   while ((ret != NULL) && (num > ant++))
      ret = strtok(NULL,"\t");
   return(ret);
}


 /**************************************************************************
  * Name: CheckMsg		                                           *
  * Type: Function                                                         *
  *                                                                        * 
  * Purpose: Checks if first 4 characters of 'buf' contains a valid Semel  *
  *          message.			 				   *
  *                                                                        *
  * Returnvalue: Not complete message : INCOMPLETE                         *
  *              Not valid message    : SEMEL_INVALID                      *
  *              Valid message        : Number of message                  *
  **************************************************************************/
int	CheckMsg( UCHAR *buf, int nBufSize)
{
	int	x;
	
	x = 0;
	if ( nBufSize < 5 )
		return INCOMPLETE;

	/* Get msg nuber from Semel Message array 	*/
	for ( x=0; x < NUM_CCP_MSGS; x++ ) 
		if ( strncmp( (char *)buf, (char *)MinutusMsg[x].szMsgName, 4 ) == 0 )
			break;

	if ( x == NUM_CCP_MSGS )
		return SEMEL_INVALID;
	
	return(MinutusMsg[x].nMsgNum);
}


  /************************************************************************* 
  * Name: GetSemelMsgInfo	                                           *
  * Type: Function                                                         *
  *                                                                        * 
  * Purpose: To retrieve full information about a Semel message.	   *
  *									   *
  * Input:	*buf		: String holding Semel message.		   *
  *		nBufSize	: Size of string.			   *
  *		*RetMsgInfo	: Return structure holding information     *
  *				  about the Semel message.		   *
  *                                                                        *
  *									   * 
  * Returnvalue: Success	: bTRUE			                   *
  *              Not success	: bFalse	                           *
  **************************************************************************/
cBOOL GetSemelMsgInfo(UCHAR *buf, int nBufSize, MsgInfo *RetMsgInfo)
{
	int i;
	int oldpos;
	int tab_num;					/* number of tab characters read */
	int msg_length;
	int ret;
	int LengthOfMessageField;
#ifdef PDEBUG
	for( i = 0; i < nBufSize; i++)
		printf("%c",*(buf + i));
	printf("\n");
#endif

	
	if ((ret = CheckMsg(buf, nBufSize)) < 0)	/* Check Message */
	{
		RetMsgInfo->state = ret;
		return(bFALSE);
	}

	RetMsgInfo->nField[0] = 0;			/* Set start of first field */
	oldpos = 0;
	RetMsgInfo->nMsgNum = ret;			/* Set message number */

	if (MinutusMsg[ret].nFieldNum > 4)
		LengthOfMessageField = bTRUE;		/* Message contains a length field */
	else
		LengthOfMessageField = bFALSE;		/* Message doesn't contain a length field */
	tab_num = 0;
	i = 4;						/* Start reading message from position 'i' */
	while ((i < nBufSize) && (tab_num < (MinutusMsg[ret].nFieldNum - 1)))
		if (buf[i++] == '\t')			/* Separation character found ? */	
		{		
			tab_num++;
			RetMsgInfo->nField[tab_num] = i; /* Set start of field 'tab_num' */
			RetMsgInfo->nFieldSize[tab_num - 1] = i - oldpos - 1;
			oldpos = i;
		}

	if (tab_num < (MinutusMsg[ret].nFieldNum - 1))
	{
		RetMsgInfo->state = INCOMPLETE;		/* Message is incomplete */
		return bFALSE;
	}

	/* if message contains length field, check remaining message */
	if (LengthOfMessageField == bTRUE)
	{
		if((msg_length = atoi(field(buf,MSG_LENGTH_FIELD))) <= 0)
		{
			RetMsgInfo->state = SEMEL_INVALID;    /* Message length field invalid */
			return bFALSE;
		}
		if (( i + msg_length + 1) > nBufSize)	
		{
			RetMsgInfo->state = INCOMPLETE;	/* Buf doen't contail full message */
			return bFALSE;
		}
		if (buf[i + msg_length] != LF)
		{
			RetMsgInfo->state = SEMEL_INVALID;    /* Message is not terminated with LF character */
			return bFALSE;
		}
		RetMsgInfo->state = bOK;
		RetMsgInfo->nSize = i + msg_length + 1;	/* Set length of message */
		RetMsgInfo->nFieldSize[tab_num] =  msg_length;	/* Set length of message field */
		return bTRUE;
	}
	/* Check message for termination character */
	else
	{
		while((i < nBufSize) && (buf[i] != LF))
			i++;
		if (buf[i] == LF)
		{
			RetMsgInfo->state = bOK;
			RetMsgInfo->nSize = i+1;	/* Set length of message */
			RetMsgInfo->nFieldSize[tab_num] =  i - oldpos;	/* Set length of message field */
			return bTRUE;
		}
		RetMsgInfo->state = INCOMPLETE;		/* Termination character not found */
		return bFALSE;
	}
}


 /**************************************************************************
  * Name: GetSemelMsg(int msg, char szBuf)                                 *
  * Type: Function                                                         *
  *                                                                        * 
  * Purpose: Gets a Semel message string				   *
  *                                                                        *
  * Returnvalue: Success: Pointer to text string                           *
  *		 Failure: NULL						   *
  **************************************************************************/
char *GetSemelMsg(int msg, char *szBuf)
{
	if ((msg < 0) || (msg > NUM_CCP_MSGS))
		return('\0');
	return(strcpy(szBuf,(char *)MinutusMsg[msg].szMsgName));
}	
