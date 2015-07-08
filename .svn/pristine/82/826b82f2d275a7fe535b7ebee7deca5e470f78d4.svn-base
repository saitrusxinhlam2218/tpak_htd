/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: udp_link.c,v $
*  @(#)  $Revision: 1.6 $
*  @(#)  $Date: 2004/02/02 18:22:33 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/async_dual/udp_link.c,v $
*
*  Copyright (c) 2000 - MobileSoft Consulting, Inc. Dublin OH
*
***********************************************************************/

static char rcsid[] = "$Id: udp_link.c,v 1.6 2004/02/02 18:22:33 jwelch Exp $";

#include <sys/types.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ipc.h>
#include <time.h>
#include <malloc.h>
#include <sys/param.h>
#include <errno.h>
#include "std.h"
#include "STDIO.h"
#include "string.h"
#include "fcntl.h"
#include "signal.h"
#include "ipcpriority.h"
#include <sys/msg.h>

#include "parman.h"
#include "acc.h"
#include "async.h"
#include "msg.h"
#include "mmp.h"
#include "language.h"
#define DEBUG_R4
#include "itm.h"
#include "path.h"
#define DECLARING
#include "comm_strdefs.h"
#include "udp_link.h"
#include "hash.h"

extern char trace_file[];
extern ALC_STATS *alc_ptr;
char * veh_lookup(int );
udp_gprs_receive( int sockfd, char *msg_buff, int *length )
{
  struct sockaddr_in  cli_addr;
  char *pUDPRxMsg;
  MSG_QUEUE_HDR tmpRxMsg;
  char udp_mesg[BUFSIZ];
  char hash_string[32];
  char MID[8];
  char *pVehIP = NULL;
  char VehIP[32];
  int rf_id = 0;

  FILE *trace_logfile;
  CHAR *ctime(), *stime;
  time_t tmtime;
  struct tm *tm_ptr;
  time_t nowtime;
  extern  struct tm *localtime();
  char   DateBuff[16];  

  bzero(VehIP, 32);
  pUDPRxMsg = (char *)msg_buff;
  dg_receive( sockfd, (struct sockaddr *) &cli_addr, sizeof(cli_addr), udp_mesg, length );
  BuildMMP( udp_mesg, &tmpRxMsg, *length );

  if ( *length > 4096 )
    {

     if ((trace_logfile = fopen(trace_file, "a")) == NULL)
       fprintf(stderr, "Error opening trace file \n");

     tmtime = time((long *) 0);
     get_arg_date(tmtime, DateBuff);
     stime = ctime(&tmtime);
     fprintf(trace_logfile, "%.2s", DateBuff);
     fprintf(trace_logfile, "%.2s", &DateBuff[3]);
     fprintf(trace_logfile, "%.2s ", &DateBuff[6]);
     fprintf(trace_logfile, "%.2s", &stime[11]);
     fprintf(trace_logfile, "%.2s", &stime[14]);
     fprintf(trace_logfile, "%.2s", &stime[17]);
     strcpy( VehIP, inet_ntoa( ((struct sockaddr_in *)&cli_addr)->sin_addr ) );      
     fprintf(trace_logfile, " %s %d %s\n", "**ALC** buffer overrun detected", *length, VehIP);
     fclose(trace_logfile);
     *length = 4000;
    }
  
  memcpy( pUDPRxMsg, (tmpRxMsg.route_id), sizeof(tmpRxMsg) - sizeof(int) );

  *length = *length - 10; // text body length returned

  tmpRxMsg.size = *length + ROUTE_ID_SIZE + PKT_ID_SIZE + 5 + P_SIZE + 2;

  *length = tmpRxMsg.size;
  
  strncpy( MID, tmpRxMsg.route_id, ROUTE_ID_SIZE );

  rf_id = (int)strtol(MID, (char **) 0, 16);
  
  pVehIP = ( char * )veh_lookup( rf_id );

  if ( pVehIP == NULL ) // no IP address in table
    {
      strcpy( VehIP, inet_ntoa( ((struct sockaddr_in *)&cli_addr)->sin_addr ) );      
      veh_insert( VehIP, rf_id );
    }
  else
    {
      strcpy( pVehIP, inet_ntoa( ((struct sockaddr_in *)&cli_addr)->sin_addr ) );
      veh_update( pVehIP, rf_id );
    }
  
}

udp_gprs_broadcast_write( int sockfd, LINK_PKT *pLinkPkt, int size )
{
  char udp_mesg[BUFSIZ];
  struct sockaddr_in serv_addr;
  int num;
  char MID[8];
  char MMP_text[BUFSIZ];
  char *pMMPtext;
  char *pVehIP = NULL;
  bucket *bucketVehIP;
  int i = 0;

  bzero( udp_mesg, BUFSIZ );
  bzero( MMP_text, BUFSIZ );
  strncpy( MID, pLinkPkt->route_id, ROUTE_ID_SIZE );
  MID[ROUTE_ID_SIZE] = '\0';

  serv_addr.sin_family = AF_INET;
 
  serv_addr.sin_port = htons(6242);

  pMMPtext = (char *)strstr( pLinkPkt->text, "%E" );
  if ( pMMPtext == NULL )
    {
      MMP_text[0] = '%';
      MMP_text[1] = 'E';
    }
  else
    {
  //      *pMMPtext = '\0';
      *(pMMPtext + 2) = '\0';
      strcpy( MMP_text, pLinkPkt->text );
    }

  for ( i = 0; i < 500; i++ )
    {
      if ( (char *)(alc_ptr->veh_table[i].VehIP) != NULL )
        {
          pVehIP = (char *)(alc_ptr->veh_table[i].VehIP);
          serv_addr.sin_addr.s_addr = inet_addr( pVehIP );
          if ( pLinkPkt->type == '1' )  // broadcast message
            {
              switch( pLinkPkt->p_fields[2] )
                {
                case '0':
                  sprintf( udp_mesg, "%03d%s00%s", TPAK_BCAST_F31, MID, MMP_text );
                  break;
                case '1':
                  sprintf( udp_mesg, "%03d%s00%s", TPAK_BCAST_F32, MID, MMP_text );
                  break;
                case '2':
                  sprintf( udp_mesg, "%03d%s00%s", TPAK_BCAST_F33, MID, MMP_text );
                  break;
                case '3':
                  sprintf( udp_mesg, "%03d%s00%s", TPAK_BCAST_F34, MID, MMP_text );
                  break;
                default:
                  sprintf( udp_mesg, "%03d%s00%s", TPAK_BCAST_F31, MID, MMP_text );
                  break;
                }
            }
          else if ( ( pLinkPkt->type == '2' ) && ( pLinkPkt->p_fields[0] != '2' ) )
            {
              switch ( pLinkPkt->p_fields[2] )
                {
                default:
                case '0':
                  sprintf( udp_mesg, "%03d%s00%s", TPAK_ITM_OUT, MID, pLinkPkt->text );
                  break;
                case '1':
                  sprintf( udp_mesg, "%03d%s00%s", TPAK_GPS_OUTPUT, MID, pLinkPkt->text );
                  break;
                }
            }
          else     
            sprintf( udp_mesg, "%03d%s00%s", TPAK_FILLED_MSG_DISPLAY, MID, MMP_text );
          
          num = sendto( sockfd, udp_mesg, ROUTE_ID_SIZE +strlen(MMP_text) + 8, 0, &serv_addr, sizeof(serv_addr) );
        }
    }
}

udp_gprs_write( int sockfd, LINK_PKT *pLinkPkt, int size )
{
  char udp_mesg[BUFSIZ];
  struct sockaddr_in serv_addr;
  int num, i;
  char MID[8];
  int rf_id;
  char hash_string[32];
  char *pVehIP = NULL;

  strncpy( MID, pLinkPkt->route_id, ROUTE_ID_SIZE );
  MID[ROUTE_ID_SIZE] = '\0';

  rf_id = (int)strtol(MID, (char **) 0, 16);  
  pVehIP = ( char * )veh_lookup( rf_id );

  if ( pVehIP == NULL )  // don't know IP address for this mobile
    return;

  // Convert 7bit INTL chars to 8bit
  if ( pLinkPkt->type == '2' && pLinkPkt->p_fields[0] == '0' )
    {
    }
  else
    {
      if ( strlen(pLinkPkt->text ) )
        {
          for ( i = 0; i < strlen(pLinkPkt->text); i++ )
            {
              switch( pLinkPkt->text[i] )
                {
                case '[':
                  pLinkPkt->text[i] = BIT_CHAR_LEFT_BRACKET;
                  break;
                case ']':
                  pLinkPkt->text[i] = BIT_CHAR_RIGHT_BRACKET;
                  break;
                case '\\':
                  pLinkPkt->text[i] = BIT_CHAR_BACKSLASH;
                  break;
                case '^':
                  pLinkPkt->text[i] = BIT_CHAR_CARET;
                  break;
                default:
                  break;
                }
            }
        }
    }
    
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr( pVehIP );
  serv_addr.sin_port = htons(6242);
  
  switch ( pLinkPkt->type )
    {
    case '0':
      TerminalControlCodes( sockfd, pLinkPkt, MID );
      break;
    case '1':
      NVRamControl( sockfd, pLinkPkt, MID );
      break;
    case '2':
      FilledForms( sockfd, pLinkPkt, MID );
      break;
    case '3':
      CannedMessages( sockfd, pLinkPkt, MID );
      break;
    default:
      break;
    }
  
}

BuildMMP( char *pUDPMsg, MSG_QUEUE_HDR *pUDPRxMsg, int msg_len )
{
  char MesgType[ 4 ];
  char *pMesgData;

  bzero( pUDPRxMsg, sizeof(MSG_QUEUE_HDR) );
  pUDPRxMsg->size = msg_len;
  pUDPRxMsg->delvry_prio = 'G';
  pUDPRxMsg->delvry_disp = ' ';
  pUDPRxMsg->multicast = ' ';
  
  pUDPRxMsg->persist = 'S';

  pMesgData = pUDPMsg;
  //  pMesgData = strtok( pUDPMsg, "!" );
  strncpy( MesgType, pMesgData, 3 );  // first token is message type
  
  //pMesgData = strtok( NULL, "!" );  // second token is MID
  pMesgData = (pUDPMsg + 3);
  memcpy( pUDPRxMsg->route_id, pMesgData, 5 );

  pUDPRxMsg->persist = 'S';
  
  //  if ( !strncmp( pUDPRxMsg->route_id, "00002", 5 ) )
  //    pUDPRxMsg->persist = 'S';
  //  else
  //    pUDPRxMsg->persist = ' ';
  
  switch ( atoi(MesgType) ) {

    // Forms
  case MDT_SIGN_IN:
    pUDPRxMsg->type = '2';
    memcpy(pUDPRxMsg->pkt_id, "00", 2);
    memcpy(pUDPRxMsg->p_fields, "10000", 5);
    pMesgData = (pUDPMsg+10);  // move to driver ID field
    //pMesgData = strtok( NULL, "!" ); // contains driver ID
    if ( pMesgData ) 
      strncpy(pUDPRxMsg->text, pMesgData, 6);
    //      sprintf(pUDPRxMsg->text, "%s", pMesgData);
    else
      sprintf( pUDPRxMsg->text, "" );      
    break;
  case MDT_BOOK_IN:
    pUDPRxMsg->type = '2';
    memcpy(pUDPRxMsg->pkt_id, "00", 2);
    memcpy(pUDPRxMsg->p_fields, "11000", 5);
    pMesgData = (pUDPMsg + 10);
    if ( pMesgData )
      strncpy( pUDPRxMsg->text, pMesgData, (msg_len - 10) );
    else
      sprintf( pUDPRxMsg->text, "" );
    break;

  case MDT_EBOOK:
    pUDPRxMsg->type = '2';
    memcpy(pUDPRxMsg->pkt_id, "00", 2);
    memcpy(pUDPRxMsg->p_fields, "12000", 5);
    pMesgData = (pUDPMsg + 10);
    if ( pMesgData )
      strncpy( pUDPRxMsg->text, pMesgData, (msg_len - 10) );    
    else
      sprintf( pUDPRxMsg->text, "" );
    break;

  case MDT_MSG_FROM_DRV:
    pUDPRxMsg->type = '2';
    memcpy(pUDPRxMsg->pkt_id, "00", 2);
    memcpy(pUDPRxMsg->p_fields, "13000", 5);
    pMesgData = (pUDPMsg + 10);
    if ( pMesgData )
      strncpy( pUDPRxMsg->text, pMesgData, (msg_len - 10) );    
    else
      sprintf( pUDPRxMsg->text, " %%D %%D%%E" );
    break;    

  case MDT_ETA:
    pUDPRxMsg->type = '2';
    memcpy(pUDPRxMsg->pkt_id, "00", 2);
    memcpy(pUDPRxMsg->p_fields, "17000", 5);
    pMesgData = (pUDPMsg + 10);
    if ( pMesgData )
      strncpy( pUDPRxMsg->text, pMesgData, (msg_len - 10) );    
    else
      sprintf( pUDPRxMsg->text, "" );
    break;
    
  case MDT_CBOOK:
    pUDPRxMsg->type = '2';
    memcpy(pUDPRxMsg->pkt_id, "00", 2);
    memcpy(pUDPRxMsg->p_fields, "15000", 5);
    pMesgData = (pUDPMsg + 10);
    if ( pMesgData )
      strncpy( pUDPRxMsg->text, pMesgData, (msg_len - 10) );        
    else
      sprintf( pUDPRxMsg->text, "%%D%%E" );
    break;

  case MDT_NOSHOW:
    pUDPRxMsg->type = '2';
    memcpy(pUDPRxMsg->pkt_id, "00", 2);
    memcpy(pUDPRxMsg->p_fields, "19000", 5);
    pMesgData = (pUDPMsg + 10);
    if ( pMesgData )
      strncpy( pUDPRxMsg->text, pMesgData, (msg_len - 10) );            
    else
      sprintf( pUDPRxMsg->text, "%%D%%E" );
    break;

  case MDT_STATS:
    pUDPRxMsg->type = '2';
    memcpy(pUDPRxMsg->pkt_id, "00", 2);
    memcpy(pUDPRxMsg->p_fields, "1A000", 5);
    pMesgData = (pUDPMsg + 10);
    if ( pMesgData )
      strncpy( pUDPRxMsg->text, pMesgData, (msg_len - 10) );                
    else
      sprintf( pUDPRxMsg->text, "%%D%%E" );
    break;
    
  case MDT_RELEVE:
    pUDPRxMsg->type = '2';
    memcpy(pUDPRxMsg->pkt_id, "00", 2);
    memcpy(pUDPRxMsg->p_fields, "1B000", 5);
    pMesgData = (pUDPMsg + 10);
    if ( pMesgData )
      strncpy( pUDPRxMsg->text, pMesgData, (msg_len - 10) );                    
    else
      sprintf( pUDPRxMsg->text, "%%D%%E" );
    break;

  case MDT_DUPE_RELEVE:
    pUDPRxMsg->type = '2';
    memcpy(pUDPRxMsg->pkt_id, "00", 2);
    memcpy(pUDPRxMsg->p_fields, "1C000", 5);
    pMesgData = (pUDPMsg + 10);
    if ( pMesgData )
      strncpy( pUDPRxMsg->text, pMesgData, (msg_len - 10) );                        
    else
      sprintf( pUDPRxMsg->text, "%%D%%E" );
    break;        
    
  case MDT_UNBOOK:
    pUDPRxMsg->type = '2';
    memcpy(pUDPRxMsg->pkt_id, "00", 2);
    memcpy(pUDPRxMsg->p_fields, "16000", 5);
    pUDPRxMsg->text[0] = '\0';
    break;

  case MDT_ZONE_INQ:
    pUDPRxMsg->type = '2';
    memcpy(pUDPRxMsg->pkt_id, "00", 2);
    memcpy(pUDPRxMsg->p_fields, "18000", 5);
    pMesgData = (pUDPMsg + 10);
    if ( pMesgData )
      strncpy( pUDPRxMsg->text, pMesgData, (msg_len - 10) );                            
    else
      sprintf( pUDPRxMsg->text, "%%D%%E" );
    break;

  case MDT_GPS_UPDATE:
    pUDPRxMsg->type = '2';
    memcpy(pUDPRxMsg->pkt_id, "00", 2);
    memcpy(pUDPRxMsg->p_fields, "31000", 5);
    pMesgData = (pUDPMsg + 10);
    if ( pMesgData )
      strncpy( pUDPRxMsg->text, pMesgData, (msg_len - 10) );                                
    else
      sprintf( pUDPRxMsg->text, "%%D%%E" );
    break;

  case MDT_ITM_FUNCTION:
    pUDPRxMsg->type = '2';
    memcpy(pUDPRxMsg->pkt_id, "00", 2);
    memcpy(pUDPRxMsg->p_fields, "30000", 5);
    pMesgData = (pUDPMsg + 10);
    //    if ( pMesgData )
    //      strncpy( pUDPRxMsg->text, pMesgData, (msg_len - 10) );                                    
    if ( pMesgData )
      {
	memcpy( pUDPRxMsg->text, pMesgData, msg_len - (pMesgData - pUDPMsg) ); 
	pUDPRxMsg->text[msg_len - 10] = '%';
	pUDPRxMsg->text[msg_len - 10 +1] = 'E'; 
	//	sprintf( pUDPRxMsg->text, "%s", pMesgData );
      }
    else
      sprintf( pUDPRxMsg->text, "%%D%%E" );
    break;    

    // Functions
  case MDT_TRIP_ACCEPT:
    pUDPRxMsg->type = '3';
    memcpy(pUDPRxMsg->pkt_id, "00", 2);
    memcpy(pUDPRxMsg->p_fields, "31000", 5);
    pUDPRxMsg->text[0] = '\0';
    break;

  case MDT_TRIP_REJECT:
    pUDPRxMsg->type = '3';
    memcpy(pUDPRxMsg->pkt_id, "00", 2);
    memcpy(pUDPRxMsg->p_fields, "32000", 5);
    pUDPRxMsg->text[0] = '\0';
    break;    

  case MDT_FLAG_REQ:
    pUDPRxMsg->type = '3';
    memcpy(pUDPRxMsg->pkt_id, "00", 2);
    memcpy(pUDPRxMsg->p_fields, "33000", 5);
    pUDPRxMsg->text[0] = '\0';
    break;

  case MDT_REQ_TALK:
    pUDPRxMsg->type = '3';
    memcpy(pUDPRxMsg->pkt_id, "00", 2);
    memcpy(pUDPRxMsg->p_fields, "34000", 5);
    pUDPRxMsg->text[0] = '\0';
    break;

  case MDT_SIGN_OFF:
    pUDPRxMsg->type = '3';
    memcpy(pUDPRxMsg->pkt_id, "00", 2);
    memcpy(pUDPRxMsg->p_fields, "35000", 5);
    pUDPRxMsg->text[0] = '\0';    
    break;    

  case MDT_QP_NEW:
    pUDPRxMsg->type = '2';
    memcpy(pUDPRxMsg->pkt_id, "00", 2);
    memcpy(pUDPRxMsg->p_fields, "1E000", 5);
    pMesgData = (pUDPMsg + 10);
    if ( pMesgData )
      strncpy( pUDPRxMsg->text, pMesgData, (msg_len - 10) );    
    else
      pUDPRxMsg->text[0] = '\0';          
    break;

  case MDT_QP:
    pUDPRxMsg->type = '3';
    memcpy(pUDPRxMsg->pkt_id, "00", 2);
    memcpy(pUDPRxMsg->p_fields, "36000", 5);
    pUDPRxMsg->text[0] = '\0';    
    break;

  case MDT_BREAK:
    pUDPRxMsg->type = '3';
    memcpy(pUDPRxMsg->pkt_id, "00", 2);
    memcpy(pUDPRxMsg->p_fields, "37000", 5);
    pUDPRxMsg->text[0] = '\0';    
    break;


    
  case MDT_LOAD:
    pUDPRxMsg->type = '3';
    memcpy(pUDPRxMsg->pkt_id, "00", 2);
    memcpy(pUDPRxMsg->p_fields, "38000", 5);
    pUDPRxMsg->text[0] = '\0';    
    break;

  case MDT_UNLOAD:
    pUDPRxMsg->type = '3';
    memcpy(pUDPRxMsg->pkt_id, "00", 2);
    memcpy(pUDPRxMsg->p_fields, "39000", 5);
    pUDPRxMsg->text[0] = '\0';    
    break;

  case MDT_TOGGLE:
    pUDPRxMsg->type = '3';
    memcpy(pUDPRxMsg->pkt_id, "00", 2);
    memcpy(pUDPRxMsg->p_fields, "3C000", 5);
    pUDPRxMsg->text[0] = '\0';    
    break;

  case MDT_EMERGENCY:
    pUDPRxMsg->type = '3';
    memcpy(pUDPRxMsg->pkt_id, "00", 2);
    memcpy(pUDPRxMsg->p_fields, "00000", 5);
    pUDPRxMsg->text[0] = '\0';
    break;

  case MDT_G7_SUR_PLACE:
    pUDPRxMsg->type = '3';
    memcpy(pUDPRxMsg->pkt_id, "00", 2);
    memcpy(pUDPRxMsg->p_fields, "3A000", 5);
    pUDPRxMsg->text[0] = '\0';    
    break;
    
  case MDT_ADV_ARRIVAL:
    pUDPRxMsg->type = '3';
    memcpy(pUDPRxMsg->pkt_id, "00", 2);
    memcpy(pUDPRxMsg->p_fields, "3B000", 5);
    pUDPRxMsg->text[0] = '\0';    
    break;

  case MDT_METER_ON:
    pUDPRxMsg->type = '2';
    memcpy(pUDPRxMsg->pkt_id, "00", 2);
    memcpy(pUDPRxMsg->p_fields, "1E000", 5);
    sprintf( pUDPRxMsg->text, "METER_ON%%T%%E" );
    break;

  case MDT_METER_OFF:
    pUDPRxMsg->type = '3';
    memcpy(pUDPRxMsg->pkt_id, "00", 2);
    memcpy(pUDPRxMsg->p_fields, "12000", 5);
    pUDPRxMsg->text[0] = '\0';
    break;

  case MDT_METER_OFF_NEW:
    pUDPRxMsg->type = '2';
    memcpy(pUDPRxMsg->pkt_id, "00", 2);
    memcpy(pUDPRxMsg->p_fields, "30000", 5);
    pMesgData = (pUDPMsg + 10);
    if ( pMesgData )
      strncpy( pUDPRxMsg->text, pMesgData, (msg_len - 10) );        
    else
      sprintf( pUDPRxMsg->text, "%%D%%E" );
    break;

  case MDT_ACK:
    pMesgData = (pUDPMsg + 8);
    pUDPRxMsg->type = '4';
    memcpy(pUDPRxMsg->pkt_id, pMesgData, 2);
    memcpy(pUDPRxMsg->p_fields, "00000", 5);
    pUDPRxMsg->text[0] = '\0';
    break;

  case MDT_NOACK:
    pMesgData = (pUDPMsg + 8);
    pUDPRxMsg->type = '4';
    memcpy(pUDPRxMsg->pkt_id, pMesgData, 2);
    memcpy(pUDPRxMsg->p_fields, "G4000", 5);
    pUDPRxMsg->text[0] = '\0';
    break;    
    
  }
  
}

dg_receive( int sockfd, struct sockaddr *pcli_addr, int maxclilen, char *msg_buff, int *msg_len )
{
  int n, clilen;
  char mesg[BUFSIZ];  
  char sIPAddr[ 24 ];
  bzero( mesg, BUFSIZ );  
  clilen = maxclilen;
  
  n = recvfrom( sockfd, msg_buff, BUFSIZ, 0, pcli_addr, &clilen );

  msg_buff[n] = '\0';
  *msg_len = n;
  sprintf(sIPAddr, "%s", inet_ntoa( ((struct sockaddr_in *)pcli_addr)->sin_addr ));
  // save  IP address - MID for outbound messaging
  
}

NVRamControl( int sockfd, LINK_PKT *pLinkPkt, char *MID )
{
  struct sockaddr_in serv_addr;
  char udp_mesg[BUFSIZ];  
  int num;
  char hash_string[32];
  int rf_id = 0;
  char *pVehIP = NULL;  

  sprintf( hash_string, "%s", MID );
  rf_id = (int)strtol(MID, (char **) 0, 16);  
  pVehIP = (char *)veh_lookup( rf_id );

  if ( pVehIP == NULL )  // don't know IP address for this mobile
    return;
  
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr( pVehIP );
  serv_addr.sin_port = htons(6242);

  sprintf( udp_mesg, "%03d%s00%s", TPAK_DEFOBJ_NVRAM, MID, pLinkPkt->text );
  num = sendto(sockfd, udp_mesg, strlen(udp_mesg), 0, &serv_addr, sizeof(serv_addr) );
  
}


TerminalControlCodes( int sockfd, LINK_PKT *pLinkPkt, char *MID )
{
  struct sockaddr_in serv_addr;
  char udp_mesg[BUFSIZ];  
  int num;
  char hash_string[32];
  int rf_id = 0;
  char *pVehIP = NULL;  

  sprintf( hash_string, "%s", MID );
  rf_id = (int)strtol(MID, (char **) 0, 16);  
  pVehIP = (char *)veh_lookup( rf_id );

  if ( pVehIP == NULL )  // don't know IP address for this mobile
    return;
  
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr( pVehIP );
  serv_addr.sin_port = htons(6242);

  switch ( pLinkPkt->p_fields[0] )
    {
    case '2':   // AUTHORIZE
      sprintf( udp_mesg, "%03d%s00%s", TPAK_AUTHORIZE, MID, pLinkPkt->text );
      num = sendto( sockfd, udp_mesg, strlen(udp_mesg), 0, &serv_addr, sizeof(serv_addr) );
      break;
    case '1':   // DEAUTHORIZE
      sprintf( udp_mesg, "%03d%s00", TPAK_DEAUTHORIZE, MID );
      num = sendto( sockfd, udp_mesg, ROUTE_ID_SIZE + 5, 0, &serv_addr, sizeof(serv_addr) );
      break;          
    default:
      break;
    }
}

FilledForms( int sockfd, LINK_PKT *pLinkPkt, char *MID )
{
  struct sockaddr_in serv_addr;
  char udp_mesg[BUFSIZ];  
  int num;
  char MMP_text[BUFSIZ];
  char *pMMPtext;
  char PKT_id[8];
  int rf_id = 0;
  char *pVehIP = NULL;  


  rf_id = (int)strtol(MID, (char **) 0, 16);    
  pVehIP = ( char * )veh_lookup( rf_id );

  if ( pVehIP == NULL )  // don't know IP address for this mobile
    return;
  
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr( pVehIP );
  serv_addr.sin_port = htons(6242);

  pMMPtext = (char *)strstr( pLinkPkt->text, "%E" );
  if ( pMMPtext == NULL )
    return;
  *(pMMPtext + 2)= '\0';

  strcpy( MMP_text, pLinkPkt->text );
  strncpy( PKT_id, pLinkPkt->pkt_id, 2 );
  PKT_id[2] = '\0';
  
  switch ( pLinkPkt->p_fields[0] )
    {
    case '0': // ITM or GPS message
      switch ( pLinkPkt->p_fields[2] )
        {
        case '0':  // ITM OUTPUT
          sprintf( udp_mesg, "%03d%s%-2s%s", TPAK_ITM_OUT, MID, PKT_id, pLinkPkt->text );
          break;
        case '1': // GPS OUTPUT
        default:
          sprintf( udp_mesg, "%03d%s00%s", TPAK_ITM_OUT, MID, pLinkPkt->text );
          break;
        }
      num = sendto( sockfd, udp_mesg, ROUTE_ID_SIZE + strlen(MMP_text)+8, 0, &serv_addr, sizeof(serv_addr) );
      break;
      
    case '1': // STATUS LINE
      sprintf( udp_mesg, "%03d%s00%s", TPAK_FILLED_STATUS_LINE, MID, MMP_text );
      num = sendto( sockfd, udp_mesg, ROUTE_ID_SIZE +strlen(MMP_text) + 5, 0, &serv_addr, sizeof(serv_addr) );
      break;
    case '2': // MSG DISPLAY
      if ( pLinkPkt->p_fields[1] == '1' )
        {
          sprintf( udp_mesg, "%03d%s%-2s%s", TPAK_TRIP_OFFER, MID, PKT_id, MMP_text );
          num = sendto( sockfd, udp_mesg, ROUTE_ID_SIZE +strlen(MMP_text) + 8, 0, &serv_addr, sizeof(serv_addr) );          
        }
      else if ( pLinkPkt->p_fields[1] == '2' )
        {
          sprintf( udp_mesg, "%03d%s%-2s%s", TPAK_TRIP_ASSIGN, MID, PKT_id,   MMP_text );
          num = sendto( sockfd, udp_mesg, ROUTE_ID_SIZE +strlen(MMP_text) + 8, 0, &serv_addr, sizeof(serv_addr) );          
        }
      else if ( pLinkPkt->p_fields[1] == '5' )
	{
	  sprintf( udp_mesg, "%03d%s%-2s%s", TPAK_TRIP_CANX, MID, PKT_id,   MMP_text );
          num = sendto( sockfd, udp_mesg, ROUTE_ID_SIZE +strlen(MMP_text) + 8, 0, &serv_addr, sizeof(serv_addr) );          
        }	  
      else
        {
          sprintf( udp_mesg, "%03d%s00%s", TPAK_FILLED_MSG_DISPLAY, MID, MMP_text );
          num = sendto( sockfd, udp_mesg, ROUTE_ID_SIZE +strlen(MMP_text) + 5, 0, &serv_addr, sizeof(serv_addr) );
        }
      break;          
    case '3': // PROMPT LINE
      sprintf( udp_mesg, "%03d%s00%s", TPAK_FILLED_PROMPT_LINE, MID, MMP_text );
      num = sendto( sockfd, udp_mesg, ROUTE_ID_SIZE +strlen(MMP_text) + 5, 0, &serv_addr, sizeof(serv_addr) );
      break;
    }
}

CannedMessages( int sockfd, LINK_PKT *pLinkPkt, char *MID )
{
  struct sockaddr_in serv_addr;
  char udp_mesg[BUFSIZ];  
  int num;
  char MMP_text[BUFSIZ];
  char *pMMPtext;
  int rf_id = 0;  
  char *pVehIP = NULL;  

  rf_id = (int)strtol(MID, (char **) 0, 16);  
  pVehIP = (char *)veh_lookup( rf_id );

  if ( pVehIP == NULL )  // don't know IP address for this mobile
    return;
  
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr( pVehIP );
  serv_addr.sin_port = htons(6242);

  if ( pLinkPkt->p_fields[0] != '0' )
    {
      sprintf( udp_mesg, "%03d%s00%c", TPAK_CANNED_MSG_DISPLAY, MID, pLinkPkt->p_fields[1] );
      num = sendto( sockfd, udp_mesg, ROUTE_ID_SIZE + 6, 0, &serv_addr, sizeof(serv_addr) );
    }
  else   // probably ITM or GPS output
    {

    }
  
}

char * veh_lookup(int rf_id)
{
  int i;
  
  for (i = 0; i < 500; i ++ )
    {
      if ( alc_ptr->veh_table[i].mid == rf_id )
	return (alc_ptr->veh_table[i].VehIP);
    }
  return NULL;
}

int veh_insert(char *IP_addr, int rf_id)
{
  int i;

  for ( i = 0; i < 500; i ++ )
    {
      if ( alc_ptr->veh_table[i].mid == 0 )
	{
	  alc_ptr->veh_table[i].mid = rf_id;
	  strcpy(alc_ptr->veh_table[i].VehIP, IP_addr);
	  break;
	}
    }
  // No entries available assert!!
}

int veh_update(char *IP_addr, int rf_id )
{
  int i;

  for ( i = 0; i < 500; i++ )
    {
      if ( alc_ptr->veh_table[i].mid == rf_id )
	{
	  strcpy(alc_ptr->veh_table[i].VehIP, IP_addr);
	  break;
	}
    }
}
	  
	
