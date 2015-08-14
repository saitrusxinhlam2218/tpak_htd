static char sz__FILE__[]= "@(#)pi_action.c 10.1.2.5(ver) Date Release 95/02/23 16:26:09";

 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  pi_action.c; Rel: 10.1.2.5; Get date: 4/19/95 at 08:45:37
 * @(#)  Last delta: 2/23/95 at 16:26:09
 * @(#)  SCCS File: /taxi/sccs/s.pi_action.c
 *                                                                           *
 *****************************************************************************/
/* -------------------------------------------------------------------------------
   Copyright 1993 - TaxiCom Copenhagen, Denmark

   System	:	PLANET - LINK 
   Filename	:	pi_action.c
   Purpose	:	This module contains the handling of all the PI-functions
			interfacing TaxiPak.

Modification history

Author  Date  Revision  Description
------  ------- --------------- -----------------------------------------------
LG      931125  0.0		Function-scheletons and pseudo-code.
LG	931215  0.1		Data-block handling routines.
LG	931227  0.2		Error-handling routine.
LG	931228  0.3		Check of input-data-block.
LG	940128  0.4		Zoning address/dispatching call.
------------------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <isam.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>
#include <math.h>
#include <locale.h>
#include <langinfo.h>

#include "taxipak.h"
#include "mem_mgmt.h"
#include "Object.h"
#include "df_maint.h"
#include "except.h"
#include "mads_types.h"
#include "veh_driv.h"
#include "mads_isam.h"
#include "ex_key.h"
#include "ui_ipc.h"
#include "pi.h"
#include "pi_struc.h"
#include "language.h"
#include "enhance.h"
#include "switch_ext.h"
#include "Call_private.h"
#include "Line_Manager_private.h"
#include "cnapi.h"
#include "writer_strdefs.h"
#include "Account_Address_db.h"
#include "Indv_db.h"
#include "ui_strdefs.h"
#include "Fleet_public.h"
#include "Fleet_private.h"
#include "Zone_public.h"
#include "Zone_private.h"
#include "Zone_db.h"

#define               MIN(x,y)        ( (x) < (y) ? (x):(y) )
#define         MAX(x,y)        ( (x) > (y) ? (x):(y) )

#define deg2rad (M_PI/180.0)
#define rad2deg (180.0/M_PI)

#define TO_DIGIT(c) ((int) (c - '0'))
#ifndef NDEBUG
#define MakeLogEntry( string ) NULL
#endif

void LatLong_to_UTM( double, double, double *, double * );

int EnterCallinTaxiPak( struct call_ext *cur_call, int nCallIndex,struct cisam_cl *call_ptr,
			int nType,char cRecType, struct call_ipc_rec *spCallRec );
int EnterNonExtCallinTaxiPak( struct call *cur_call, int nCallIndex,struct cisam_cl *call_ptr,
			int nType,char cRecType, struct call_ipc_rec *spCallRec );
int MakeResponseMsg( return_type *return_data, short nMsgResponse, char *szErrDesc );

void alarm_noop();

time_t get_internal_time(char *, char *);
time_t conversion(char *);
short GetCustomerZone( int, double, double );
extern MEM_ZONE *fleetMemZones[FLEET_MAX][ZONE_MAX];

/* Global structures and variables. */
/************************************/

struct excep {					
  int excep_type;				/* Internal exception-type enumrations.		*/
  long last_nbr;				/* Number of the last exception reported.	*/
  char on_off;					/* ON=1/OFF=0.					*/
  } ; 

struct excep_mess {
  long last_nbr;				/* Number of the last message-exception reported.*/
  char on_off;					/* ON=1/OFF=0.					*/
  } ; 

extern struct excep on_off_excep[ NUM_OF_EXCEP_TYPE ];		/* Table of exceptions to report.		*/
extern struct excep_mess mess_on_off[ NUM_OF_EXCEP_MESS ];	/* Message to report (ON) or not (OFF).		*/
extern struct excep_mess drv_mess_on_off[ NUM_OF_EXCEP_MESS ];	/* Driver-message to report (ON) or not (OFF).	*/

extern struct fleets *fleet[];						/* pointer to fleet array 		*/
struct cisam_cl srch_calls[ NUM_OF_EXCEPTIONS * 10 ];			/* array of calls struct's */
extern int fifo_fd;							/* Filedesc. for the USER_PIPE 		*/
extern int uid;								/* users id as in /etc/passwd 		*/
extern int pid;								/* process id of this interface process	*/
extern char req_buf[ BUFSIZ ];						/* Request-buffer (DB).			*/

/* System global structures       . */
/************************************/
struct cisam_dr driver, *drv_ptr;       /* The call-record for the dispatched call.         */
struct cisam_vh vehicle, *veh_ptr;      /* The call-record for the dispatched call.         */
struct call_ipc_rec disp_call, *dispc_ptr;  /* The call record to send to dispatch */
struct cisam_cl call, *call_ptr;

/* Local structures and variables. */
/************************************/

struct excepts *except_ptr[ NUM_OF_EXCEPTIONS * 10 ];/* Main pointer to the exceptions.         */
struct cisam_rm message;      /* The message-record for repeated messages.        */
char tmp_req_buf[ BUFSIZ ];     /* Saved Request-buffer (DB).           */
short call_status_conv(char *, size_t); 
struct cisam_ch call_history, *callh_ptr;

extern int msgkey;
extern int main_msgkey;

extern char szLogMsg[];
extern BOOLEAN callout_active;
extern BOOLEAN callback_active;


/* this is used to make sure that inbound data is in an 8-bit format
   instead of the old 7-bit format. Routines in dispatch will ensure
   that data is in 7-bit form prior to sending to MDT    */
int	ValidMMPmessage( char *cpMessageData, int nLength )
{
	int 	i=0;
	char	*cpData = cpMessageData;

    for(; i < nLength; i++, cpData++ )
	{
		if ( *cpData == 0x00 )
			break;
		switch ( *cpData )
		  {
		  case '[':
		  case '{':
		    if ( !strcmp( LANGUAGE, "Danish" ) )
		      *cpData = 'Æ';
		    else
		      *cpData = 'Ä';
		    break;
		  case ']':
		  case '}':
		    *cpData = 'Å';
		    break;
		  case '\\':
		  case '|':
		    if ( !strcmp( LANGUAGE, "Danish" ) )
		      *cpData = 'Ø';
		    else
		      *cpData = 'Ö';
		    break;
		  case '^':
		  case '~':
		    if ( !strcmp( LANGUAGE, "Swedish" ) )
		      *cpData = 'Ü';
		    break;
		  }
		

	}

	/* Leftover size has to be greater than one byte */	
	/* memset() at NULL location, of leftover bytes-1 */

	if ( i < nLength-1 )
	  memset( &cpMessageData[i], 0x00, nLength-i-1 ); 

	return 1;
}

/* ----------------------------------------------------------------------------
/* FUNCTION: PI_ZONE_ADDRESS
/*
/* DESCRIPTION: Gets a zone to an address
/*
/* ORDER-DATA: CALL-structure
/* 
/* RETURN-DATA:  if return-value = 1: return_data.data = PI_OK, 
/*                                                       CALL-structure
/*                                    OR
/*                                    return_data.data = PI_FAIL
/*	                              return_data.len  = length of data
/*
/*               if return-value = 0: return_data is not defined (close PI).
/* 
------------------------------------------------------------------------------ */

int pi_zone_address( struct order_struc *order_data, struct return_struc *return_data ) 
  {
    short pi_fail_ok,		/* Return-value (PI_OK/PI_FAIL).	*/
          counter;		/* Common loop counter.			*/
    int return_code = 1;	/* Internal func. return-code.(OK=1)	*/
    char *position;		/* Current position in data-block. 	*/
    int zip, zone;		/* Zip-code and temporary zone-nbr.	*/
		struct cisam_cl call, *call_ptr;        /* The call-structure similar to the one used in TaxiPak.     */
		struct call cur_call;

    /* Checking block size... */

		if ( order_data->data_len != CALL_STRUC_SIZE && order_data->data_len != CALL_STRUC_GPS_SIZE )

			return( MakeResponseMsg( return_data, (short) PI_INVALID_LEN,
													"Invalid msg length : PI_ZONE_CALL" ) );


		/* Check if valid data in struct to zone */

		memcpy( &cur_call, order_data->data, sizeof( struct call ) );
		if ( !ValidCallRecord( &cur_call, return_data ) )
			return 1;

    /***********************************/
    /* NULL-terminating ASCII-strings. */
    /***********************************/

    cur_call.from_addr_street[ 20 ] = '\0';
    cur_call.from_addr_apart[ 5 ] = '\0';
    cur_call.from_addr_city[ 3 ] = '\0';
    cur_call.from_addr_cmnt[ 30 ] = '\0';
    cur_call.passenger[ 20 ] = '\0';
    cur_call.phone[ 10 ] = '\0';
    cur_call.to_addr_street[ 20 ] = '\0';
    cur_call.to_addr_apart[ 5 ] = '\0';
    cur_call.to_addr_city[ 3 ] = '\0';
    cur_call.to_addr_cmnt[ 30 ] = '\0';
    cur_call.driver_id[ 5 ] = '\0';
    cur_call.creation_date[ 6 ] = '\0';
	 cur_call.creation_time[ 4 ] = '\0';
    cur_call.due_date[ 6 ] = '\0';
    cur_call.due_time[ 4 ] = '\0';
    cur_call.pickup_time[ 4 ] = '\0';
    cur_call.close_time[ 4 ] = '\0';
    cur_call.call_comment[ 64 ] = '\0';


    /***********************************************/
    /* Execute TaxiPak-handling og PI_ZONE_ADRESS. */
    /***********************************************/

    call_ptr = &call;

		call_ptr->fleet = cur_call.fleet;
		call_ptr->pckup_str_nbr = cur_call.from_addr_number;
		call_ptr->pckup_str_suf = cur_call.from_addr_suffix;
    call_ptr->pckup_pre_dir[ 0 ] = '\0';
		strcpy( call_ptr->pckup_str_name, cur_call.from_addr_street );
    call_ptr->pckup_str_key[ 0 ] = '\0';
    call_ptr->pckup_str_type[ 0 ] = '\0';
    call_ptr->pckup_post_dir[ 0 ] = '\0';
		strcpy( call_ptr->pckup_apt_room, cur_call.from_addr_apart );
		strcpy( call_ptr->pckup_city, cur_call.from_addr_city );
		strcpy( call_ptr->pckup_adr_cmnt, cur_call.from_addr_cmnt );
		strcpy( call_ptr->general_cmnt, cur_call.call_comment);

    cur_call.call_status = NOEXIST;
    if ( glEnhSwitch & ENH0002 )
      cur_call.creator = 9999;
    else
      cur_call.creator = 8888;
    
    zone = zone_addr( &call, PICKUP_ADDR, &zip );

		if ( zone <= 0 )
		{
			return( MakeResponseMsg( return_data, (short) PI_INV_ZONE,
														 "Not able to zone pickup-address : PI_INV_ZONE" ) );
		}
    call_ptr->pckup_zone = zone;
    cur_call.from_addr_zone = call_ptr->pckup_zone;
    cur_call.from_addr_number = call_ptr->pckup_str_nbr;
    strcpy( cur_call.from_addr_street, call_ptr->pckup_str_name );
    strcpy( cur_call.from_addr_cmnt, call_ptr->pckup_adr_cmnt );
    strcpy( cur_call.call_comment, call_ptr->general_cmnt );

    /*******************************************/
    /* If destination-address exists, zone it. */
    /*******************************************/

    if ( cur_call.to_addr_street[ 0 ] != '\0' )
      {
        call_ptr->dest_str_nbr = cur_call.to_addr_number;
        call_ptr->dest_str_suf = cur_call.to_addr_suffix;
        call_ptr->dest_pre_dir[ 0 ] = '\0';
        strcpy( call_ptr->dest_str_name, cur_call.to_addr_street );
        call_ptr->dest_str_type[ 0 ] = '\0';
        call_ptr->dest_post_dir[ 0 ] = '\0';
        strcpy( call_ptr->dest_apt_room, cur_call.to_addr_apart );
        strcpy( call_ptr->dest_city, cur_call.to_addr_city );
        strcpy( call_ptr->dest_adr_cmnt, cur_call.to_addr_cmnt );
        strcpy( call_ptr->general_cmnt, cur_call.call_comment);

        zone = zone_addr( &call, DEST_ADDR, &zip );

        call_ptr->dest_zone = zone;
        cur_call.to_addr_zone = call_ptr->dest_zone;
        cur_call.to_addr_number = call_ptr->dest_str_nbr;
        strcpy( cur_call.to_addr_street, call_ptr->dest_str_name );
        strcpy( cur_call.to_addr_cmnt, call_ptr->dest_adr_cmnt );
        strcpy( cur_call.call_comment, call_ptr->general_cmnt );

      }

    /******************************/
    /* Build return-data-block.   */
    /******************************/

    return_data->data_len = 0;
    position = return_data->data;
    pi_fail_ok = PI_OK;

    return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
    if ( return_code < 1 )
      {
        return_data->data_len = 0;
        position = return_data->data;

        ERROR( "Invalid block length (too long)" );	/* Log the error... */

        pi_fail_ok = PI_INVALID_VALUE;
        return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
        return( return_code ); 
      }

    return_code = ins_to_data_blk( sizeof( struct call ), &position, (char *) &cur_call, &return_data->data_len );
    if ( return_code < 1 )
      {
        return_data->data_len = 0;
        position = return_data->data;

        ERROR( "Invalid block length (too long)" );	/* Log the error... */

        pi_fail_ok = PI_INVALID_VALUE;
        return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
        return( return_code ); 
      }

    return( return_code ); 
  } /* end PI_ZONE_ADDRESS */

int pi_coord_transform( struct order_struc *order_data, struct return_struc *return_data )
{
  short pi_fail_ok, counter, i;
  int  return_code;
  COORD_TRANSFORM in_mess;
  double inx, iny, outx, outy;
  char  *position;

  if ( order_data->data_len != sizeof(COORD_TRANSFORM) )
    {
      return_data->data_len = 0;
      position = return_data->data;
      pi_fail_ok = PI_INVALID_LEN;
      return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
      return( return_code );
    }

  position = order_data->data;
  return_code = get_from_data_blk( sizeof( COORD_TRANSFORM ), &position, (char *) &in_mess, &order_data->data_len );

  if ( return_code < 1 )
    {
      return_data->data_len = 0;
      position = return_data->data;

      pi_fail_ok = PI_INVALID_VALUE;
      return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
      return( return_code );
    }
  return_data->data_len = 0;
  position = return_data->data;


  if ( in_mess.direction == '1' )
    {
      inx = (double)atof(in_mess.coord_x);
      iny = (double)atof(in_mess.coord_y);
      //      ST74ToWGS84( inx, iny, &outx, &outy );
      sprintf(in_mess.latitude, "%8.6f", outy);
      sprintf(in_mess.longitude, "%8.6f", outx);      
    }
  else
    {
      inx = (double)atof(in_mess.longitude);
      iny = (double)atof(in_mess.latitude);
      //      WGS84ToST74( inx, iny, &outx, &outy );
      sprintf(in_mess.coord_x, "%9.0f", outx);
      sprintf(in_mess.coord_y, "%9.0f", outy);
    }

                     
  return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
  
  if ( return_code < 1 )
    {
      return_data->data_len = 0;
      position = return_data->data;
      
      ERROR( "Invalid block length (too long)" );	/* Log the error... */
      
      pi_fail_ok = PI_INVALID_VALUE;
      return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
      return( return_code ); 
    }
  
  if ( pi_fail_ok == PI_OK )
    {
      return_code = ins_to_data_blk( sizeof(COORD_TRANSFORM), &position, (char *) &in_mess, &return_data->data_len ); 
      if ( return_code < 1 )
        {
          return_data->data_len = 0;
          position = return_data->data;
          
          ERROR( "Invalid block length (too long)" );	/* Log the error... */
          
          pi_fail_ok = PI_INVALID_VALUE;
          return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
          return( return_code ); 
        }
    }
  
  return( return_code );
}

int pi_dispatch_account_call( struct order_struc *order_data, struct return_struc *return_data )
{
  short pi_fail_ok, counter, i;
  int  return_code;
  int  cl_nbr;
  char  *position;

  return_code = d1000_open_db_files();

  return_code = m2000_process_message( 0, order_data, &cl_nbr );

  d1100_close_db_files();

  if ( return_code != 0 )
    {
      return_data->data_len = 0;
      position = return_data->data;
      pi_fail_ok = return_code;
      return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
      return( return_code );
    }

  return_data->data_len = 0;
  position = return_data->data;
  pi_fail_ok = return_code;
  

  return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
  if ( return_code < 1 )
    {
      return_data->data_len = 0;
      position = return_data->data;
      
      ERROR( "Invalid block length (too long)" );	/* Log the error... */
      
      pi_fail_ok = PI_INVALID_VALUE;
      return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
      return( return_code ); 
    }

  memcpy( return_data->data, &cl_nbr, PI_INT_TYPE );
  return_data->data_len += PI_INT_TYPE;
  return(1);

  //  return_code = ins_to_data_blk( PI_INT_TYPE, &position, (char *) &cl_nbr, &return_data->data_len );
  //  if ( return_code < 1 )
  // {
  //  return_data->data_len = 0;
  //  position = return_data->data;
  //  
  //  ERROR( "Invalid block length (too long)" );	/* Log the error... */
  //  
  //  pi_fail_ok = PI_INVALID_VALUE;
  //  return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
  //  return( return_code ); 
  // }
  
  
}

  
/* ----------------------------------------------------------------------------
/* FUNCTION: PI_DISPATCH_CALL
/*
/* DESCRIPTION: Dispatch a zoned call
/*
/* ORDER-DATA: CALL-structure
/* 
/* RETURN-DATA:  if return-value = 1: return_data.data = PI_OK, 
/*                                                       Call_Number
/*                                    OR
/*                                    return_data.data = PI_FAIL
/*	                              return_data.len  = length of data
/*
/*               if return-value = 0: return_data is not defined (close PI).
/* 
------------------------------------------------------------------------------ */

int pi_dispatch_call( struct order_struc *order_data, struct return_struc *return_data ) 
  {
		short pi_fail_ok,     /* Return-value (PI_OK/PI_FAIL).    */
					counter, i;     /* Common loop counters.      */
    int return_code = 1;	/* Internal func. return-code.(OK=1)		*/
		int db_call_num;      /* The call-number assigned to the call_ptr.  */
		struct call *spCurCall;
		char *position;   /* Current position in data-block.    */
		struct call_ipc_rec sCallRec;

    /**********************/
    /* FORMAT-check part. */
    /**********************/

		/* Checking client's msg size ... */

		if ( order_data->data_len != CALL_STRUC_SIZE && order_data->data_len != CALL_STRUC_GPS_SIZE )

			return( MakeResponseMsg( return_data, (short) PI_INVALID_LEN,
													"Invalid msg length : PI_DISPATCH_CALL" ) );


                  
                


    /**********************/
    /* FORMAT-check part. */
    /**********************/

		spCurCall = ( struct call *) order_data->data;
		//                spCurCall->priority = (short)(order_data->data[8]);
		//		spCurCall->from_addr_number = (short)(order_data->data[48]);
                // Remove any NULL's from GPS positions
                for ( i = 0; i < 8; i++ ) {
                  if ( spCurCall->gpsx[i] == '\0' )
                    spCurCall->gpsx[i] = '0';
                  if ( spCurCall->gpsy[i] == '\0' )
                    spCurCall->gpsy[i] = '0';
                }
                
		if ( !ValidZonedCallRecord( spCurCall, return_data ) )
			return 1;


    /*****************************************************/
    /* Execute the TaxiPak-handling of PI_DISPATCH_CALL. */
    /*****************************************************/


		call_ptr = &call;

		if ( spCurCall->number_of_vehicles == 0 )
		  spCurCall->number_of_vehicles = 1;
		for ( i = 1; i <= spCurCall->number_of_vehicles; i++ )
		{
				/* Updating Call and Call history databases */
                                LogMsg( PI_DISPATCH_CALL, EXTP_TO_PI, (void *)spCurCall );
                                if ( glEnhSwitch & ENH0002 )
                                  db_call_num = EnterNonExtCallinTaxiPak( (struct call_ext *)spCurCall, i, call_ptr, NON_EXT_CALL, CALL_REQ, &sCallRec );
                                else
                                  db_call_num = EnterCallinTaxiPak( (struct call_ext *)spCurCall, i, call_ptr, NON_EXT_CALL, CALL_REQ, &sCallRec );


				EnterCallInSharedMem( &sCallRec );
		}
    
    /******************************/
    /* Build return-data-block.   */
    /******************************/

    return_data->data_len = 0;
    position = return_data->data;
    pi_fail_ok = PI_OK;


    return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
    if ( return_code < 1 )
      {
        return_data->data_len = 0;
        position = return_data->data;

        ERROR( "Invalid block length (too long)" );	/* Log the error... */

        pi_fail_ok = PI_INVALID_VALUE;
        return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
        return( return_code ); 
			}

		return_code = ins_to_data_blk( PI_INT_TYPE, &position, (char *) &call_ptr->nbr, &return_data->data_len );
		if ( return_code < 1 )
		{
        return_data->data_len = 0;
        position = return_data->data;

        ERROR( "Invalid block length (too long)" );	/* Log the error... */

        pi_fail_ok = PI_INVALID_VALUE;
        return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
        return( return_code ); 
		}

    return( return_code ); 
  } /* end PI_DISPATCH_CALL */


	void DumpBuf( char *szBuf, int nSize )
	{
		 int nCount=0;
		 char  szMsg[10];

		 szLogMsg[0] = 0x00;

		 for ( ; nCount < nSize ; nCount++ )
		 {
			 if ( szBuf[nCount] < ' ' || szBuf[nCount] >= 128 )
				sprintf( szMsg, "(??)[%x]", szBuf[nCount] );
			 else
				sprintf( szMsg, "(%c)[%x]", szBuf[nCount], szBuf[nCount] );
			 strcat( szLogMsg, szMsg );
			 if ( nCount%10 == 0 )
			 {
					MakeLogEntry( szLogMsg );
					MakeLogEntry( "\n" );
					szLogMsg[0] = 0x00;
			 }
		 }
	}


	/* ----------------------------------------------------------------------------
	/* FUNCTION: PI_DISPATCH_EXT_CALL
	/*
	/* DESCRIPTION: Dispatch a external zoned call
	/*
	/* ORDER-DATA: CALL-structure
	/*
	/* RETURN-DATA:  if return-value = 1: return_data.data = PI_OK,
	/*                                                       Call_Number
	/*                                    OR
	/*                                    return_data.data = PI_FAIL
	/*                                    return_data.len  = length of data
	/*
	/*               if return-value = 0: return_data is not defined (close PI).
	/*
	------------------------------------------------------------------------------ */

	int pi_dispatch_ext_call( struct order_struc *order_data, struct return_struc *return_data )
		{
			short pi_fail_ok,   /* Return-value (PI_OK/PI_FAIL).    */
						counter, i;   /* Common loop counters.      */
			int return_code = 1;  /* Internal func. return-code.(OK=1)    */
			int db_call_num;    /* The call-number assigned to the call_ptr.  */
			char *position;   /* Current position in data-block.    */

			short   bEndOfMsg = 0;
			short   nCallMsgs = 1;
			short   x,y;
			int nNumberOfCalls = 0;
			int nMsgSize = 0;
			int nTempCallNbr;
			int NumVeh;

			struct call_ext *spCurCall; /* The actual call (structure).     */
			struct cisam_cl call, *call_ptr;        /* The call-structure similar to the one used in TaxiPak.     */
			struct cisam_cmsg *ci_cmsg;
			CallInfo saCallInfo[MAX_MULTI_CALLS];  /* track/process incoming calls to API from Ftj */
			struct call_ipc_rec  saCallRecData[MAX_MULTI_CALLS]; /* for keeping call_rec to send to dispatch later*/
			char *ptr;

			/**************************************/
			/* Check if minimum one call received */

			memset( saCallInfo, 0, sizeof(saCallInfo) );
			saCallInfo[0].call_nbr = INVALID_CALL;
			saCallInfo[1].call_nbr = INVALID_CALL;
			saCallInfo[2].call_nbr = INVALID_CALL;

			if ( order_data->data_len < sizeof(struct call_ext) )
				 return( MakeResponseMsg( return_data, (short) PI_INVALID_LEN,
																	"Invalid PI_DISPATCH_EXT_CALL msg size" ) );



			/*******************************************************
			 * VALIDATION PHASE
			 * Validate each multi-call and call_msg               */

			spCurCall = ( struct call_ext *)order_data->data;

			while( !bEndOfMsg )
			{

				/* Validate call structure */
				if ( !ValidZonedCallRecord((struct call*) spCurCall, return_data ) )
					return 1;

				/* Check if call zoned yet */
				if ( spCurCall->from_addr_zone < MIN_ADDR_ZONE  ||
						 spCurCall->from_addr_zone > MAX_ADDR_ZONE )
				{
						MakeResponseMsg( return_data, (short) PI_INV_ZONE,
												"Invalid pickup-zone: PI_DISPATCH_EXT_CALL" );
						return 1;
				}


                                //				MakeLogEntry( "Valid Call record\n" );

				if ( !ValidFtjInfoCallRec( spCurCall, return_data ) )
					return 1;

                                //				MakeLogEntry("Valid ftj info\n" );

				/* Check if all call_msg records are here */
				nMsgSize += (spCurCall->msg_nbr-'0')*sizeof(struct cisam_cmsg)
																			+ sizeof(struct call_ext) ;
                                //				MakeLogEntry( szLogMsg );


				if ( nMsgSize > order_data->data_len )
					return( MakeResponseMsg( return_data, (short) PI_INVALID_LEN,
																	"Invalid PI_DISPATCH_EXT_CALL msg size" ) );

				/* Check validity of all call_msg records */
				ptr = (char *)spCurCall;

				ci_cmsg = ( struct cisam_cmsg*) ( ptr + sizeof(struct call_ext) );
				for ( i=0; i<spCurCall->msg_nbr-'0'; i++, ci_cmsg++ )
					if ( !ValidFtjCallRec( ci_cmsg, return_data ) )
						return 1;

                                //				MakeLogEntry( "Valid call_msg records\n" );

				/* Update statistics on all records received! */
				saCallInfo[nCallMsgs-1].nbr_call_msg = spCurCall->msg_nbr-'0';
				saCallInfo[nCallMsgs-1].call_nbr = VALID_CALL;
				++nCallMsgs;

				sprintf( szLogMsg, "Multi call (%d), call_nbr (%d)\n", nCallMsgs-1,
																		spCurCall->msg_nbr-'0' );
                                //				MakeLogEntry( szLogMsg );

				/* Check if any more data to process */
				if ( order_data->data_len < nMsgSize + sizeof(struct call_ext) )
				{
					sprintf( szLogMsg, "No more msgs: buf(%d), prcsd(%d) ,min reqd (%d)\n",
													order_data->data_len, nMsgSize, sizeof( struct call_ext));
                                        //					MakeLogEntry( szLogMsg );
					bEndOfMsg = 1;
				}

				/* Look for next call structure */
				spCurCall = ( struct call_ext *) ( ci_cmsg );

			}

                        //			MakeLogEntry( "VAlIDATION COMPLETE\n" );

			/*********************************************************
			 * CALL-ENTRY PHASE
			 * Enter call(s) and call_msg(s) into TaxiPak.
			 * M.O. fill in array of call information
			 *      1.  save call# of every call put into taxiPak
			 *********************************************************/

			/* Remember to put loop here for linking calls for mult-car trips */

			--nCallMsgs;
			spCurCall = ( struct call_ext *)order_data->data;

			NumVeh = spCurCall->number_of_vehicles; /* Save number of vehicles for this call */
			for ( i = 1; i <= NumVeh; i++ )
			{
				/*******************************/
				/* Loop through all mult-calls */

				for ( x=0; x<nCallMsgs; x++ )
				{
					if ( saCallInfo[x].call_nbr == INVALID_CALL )
						break;

					sprintf( szLogMsg, "Entering mult-call(%d) into TaxiPak\n", x+1 );
                                        //					MakeLogEntry( szLogMsg );

					if ( nCallMsgs > 1 ) /* Ie. a multi-call */
						spCurCall->call_type[MULT_TYPE_OFF] = MULT_CALL; /* Multi-call type */



                                        LogMsg( PI_DISP_EXT_CALL, EXTP_TO_PI, (void *)spCurCall );                                                          

					nTempCallNbr = EnterCallinTaxiPak( spCurCall, i, call_ptr,
																						 EXT_CALL, SP_CALL_REQ, &saCallRecData[x] );

					
					/* Assign number only if valid */
					if ( nTempCallNbr > 0 )
						saCallInfo[x].call_nbr = nTempCallNbr;


					if ( saCallInfo[x].call_nbr == 0 )
						return( MakeResponseMsg( return_data, (short) PI_DB_OP_FAIL,
																	"Invalid PI_DISPATCH_EXT_CALL msg size" ) );


					ci_cmsg = ( struct cisam_cmsg*) (++spCurCall);

					/*******************************/
					/* Loop through all call-msgs  */

					for ( y=0; y< saCallInfo[x].nbr_call_msg; y++, ci_cmsg++ )
					{
						sprintf( szLogMsg, "Entering call_msg record (%d) into TaxiPak\n", y+1 );
                                                //						MakeLogEntry( szLogMsg );
						sprintf( szLogMsg,"Call_msg comment (%s)\n", ci_cmsg->msg );
                                                //						MakeLogEntry( szLogMsg );

						/* Pointer to the Request-buffer. */
						ci_cmsg->call_nbr = saCallInfo[x].call_nbr;
						ci_cmsg->ack = NO;  /* initially no ack on device */
						memcpy(req_buf, (char *)ci_cmsg, sizeof(struct cisam_cmsg));
						sprintf( szLogMsg, "Laying call_msg record(%d) \n", y+1 );
                                                //						MakeLogEntry( szLogMsg );

						if ( db(CALLMSG_FILE_ID, ADD_REC, &cl_key1, ISEQUAL ) < 0 )
								ERROR( "Cannot add call_msg from PI (DB)." );
					} /* call-msg loop */

					/* Point to next call in buffer */
					spCurCall = ( struct call_ext *) ci_cmsg;

				} /* End loop for mult-call entry */

			} /* End-loop for number of vehicles */

			/* Leftover x index is number of calls */
			nNumberOfCalls = x;


			/*******************************************
			 * LINK PHASE
			 * Link all if more than one calls
			 *
			 * 1. Use prev-saved call#'s to link calls
			 *
			 *******************************************/

                        //			MakeLogEntry( "DATABASE RECORDS ARE CREATED\n" );
			sprintf( szLogMsg, "Linking (%d) calls(%d), (%d), (%d)\n" ,
														nNumberOfCalls, saCallInfo[0].call_nbr,
														saCallInfo[1].call_nbr, saCallInfo[2].call_nbr );
                        //			MakeLogEntry( szLogMsg );

			if ( nNumberOfCalls > 1 ) /* If a multi-call */
				for ( y=0; y<nNumberOfCalls; y++ )
				{
					if ( saCallInfo[y].call_nbr == INVALID_CALL )
							break;

					/******************************/
					/* Find the multi call record */
					call_ptr = ( struct cisam_cl *)req_buf;
					call_ptr->nbr = saCallInfo[y].call_nbr;

					if ( db(CALL_FILE_ID, READ_KEYS , &cl_key1, ISEQUAL+ISLOCK, 0 ) < 0 )
						ERROR( "Can't find record with isam offset" );


					sprintf( szLogMsg, "Found call(%d)\n" , saCallInfo[y].call_nbr );
                                        //					MakeLogEntry( szLogMsg );

					/* update the record with the next record in the link */
					/* also update the saved call_recs to send to dispatch later */
					if ( (y+1) == nNumberOfCalls )
					{
						call_ptr->grouped_with = saCallInfo[0].call_nbr;
						sprintf( szLogMsg, "Linking call (%d) with next(%d)\n",
																					saCallInfo[y].call_nbr,saCallInfo[0].call_nbr );
						saCallRecData[y].grouped_with = saCallInfo[0].call_nbr;
					}
					else
					{
						call_ptr->grouped_with = saCallInfo[y+1].call_nbr;
						saCallRecData[y].grouped_with = saCallInfo[y+1].call_nbr;
						sprintf( szLogMsg, "Linking call (%d) with next(%d)\n",
																					saCallInfo[y].call_nbr,saCallInfo[y+1].call_nbr );
					}
                                        //					MakeLogEntry( szLogMsg );

					/********************************/
					/* Update the multi call record */


					if ((db(CALL_FILE_ID,UPDATE_REC,0,0,0)) < 0)
							ERROR("Error updating <call record>" );

					if ((db(CALL_FILE_ID,RELEASE,0,0,0)) < 0)
							ERROR("Error releasing <call record>");

					/* Multi-call loop for link */

				} /* End loop multi-trips */


				/********************************************
				 *  DISPATCH call entry Phase
				 *
				 *
				 ********************************************/

                        //			MakeLogEntry( "LINKING COMPLETE\n" );

/*      for ( y=0; y<nNumberOfCalls; y++ ) */

				EnterCallInSharedMem( &saCallRecData[0] );


                                //			MakeLogEntry( "CALL RECS TO DISPATCH\n" );


			/***********************************/
			/* Return call number of last call */
			/* if (2-9)cabs requested for trip */

			/******************************/
			/* Build return-data-block.   */
			/******************************/

			return_data->data_len = 0;
			position = return_data->data;
			pi_fail_ok = PI_OK;

			return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
			if ( return_code < 1 )
				{
					return_data->data_len = 0;
					position = return_data->data;

					ERROR( "Invalid block length (too long)" ); /* Log the error... */

					pi_fail_ok = PI_INVALID_VALUE;
					return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
					return( return_code );
				}

			sprintf( szLogMsg, "Returning call number(%d) of(%d) calls to client\n",
													saCallInfo[nNumberOfCalls-1].call_nbr, nNumberOfCalls );
                        //			MakeLogEntry( szLogMsg );

			return_code = ins_to_data_blk( PI_INT_TYPE, &position,
																			(char *) &saCallInfo[0].call_nbr,
																															&return_data->data_len );
			if ( return_code < 1 )
				{
					return_data->data_len = 0;
					position = return_data->data;

					ERROR( "Invalid block length (too long)" ); /* Log the error... */

					pi_fail_ok = PI_INVALID_VALUE;
					return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
					return( return_code );
				}

			return( return_code );
		} /* end PI_DISPATCH_EXT_CALL */

/* ----------------------------------------------------------------------------
 * FUNCTION: EnterCallInSharedMem
 *
 * DESCRIPTION: Send the call physically to TaxiPak
 *
 * M.O.   Send over the populated call_rec for dispatch to
 *        place in shared memory
 *
 * RETURN-DATA:  if return-value = 1: fields OK,
 *               if return-value = 0: invalid data.
 *
------------------------------------------------------------------------------ */

int   EnterCallInSharedMem( struct call_ipc_rec *dispc_ptr )
{

  //	MakeLogEntry( "call sent to dispatch\n" );
	/*
	sprintf( szLogMsg, "Call (%d), due-time(%s): sent to dispatch\n", dispc_ptr->call_nbr, dispc_ptr->due_time );
	MakeLogEntry( szLogMsg );
	*/

	if ( write( fifo_fd, dispc_ptr, sizeof( struct call_ipc_rec )) == -1 )
	{
		ERROR("Error sending call to FIFO.");
		return FALSE;
	}

	return TRUE;
}



		/* ----------------------------------------------------------------------------
		 * FUNCTION: EnterCallinTaxiPak
		 *
		 * DESCRIPTION: Add records and send info to dispatch process
		 *              Initiate call to be dispatched
		 *
		 * PARAMETERS: struct *call       - local API struct from client on call!
		 *             int    nCallIndex  - the call index out of total calls
		 *                                  (ie. total = #cabs for trip )
		 *             cisam_cl call_ptr  - isam structure for call
		 *             char cRecType      - source of dispatch call
		 *
		 * M.O.:      Fill in cisam calls struct and add new record!
		 *            Fill in cisam call history struct and add new record !
		 *            Populate call_rec structs (
		 *
		 * RETURN-DATA:  if return-value = 1: fields OK,
		 *
		 *               if return-value = 0: invalid data.
		 *
		------------------------------------------------------------------------------ */

int EnterCallinTaxiPak( struct call_ext *cur_call, int nCallIndex,struct cisam_cl *call_ptr,
			int nType,char cRecType, struct call_ipc_rec *spCallRec )
{
			int db_call_num;    /* The call-number assigned to the call_ptr.  */
			char *position;   /* Current position in data-block.    */
			char future_call;   /* Indicating whether call is a future-call.  */
			char now_ascii_date[ 12 ],  /* Times to compare for deciding    */
					 now_ascii_time[ 7 ], /* whether to dispatch a future call    */
					 min_ascii_date[ 12 ],  /* or now call.         */
					 min_ascii_time[ 7 ],
					 max_ascii_date[ 12 ],
					 max_ascii_time[ 7 ];
			time_t now_time,
						 min_time,
						 max_time;
			int counter;
			int max_attr;
                        int zip = 0;
                        double x_coord = 0.0, y_coord = 0.0;        

			struct cisam_ch call_history, *callh_ptr; /* The call-history-structure similar to the one used in TaxiPak.   */
			struct call *pMPKCall;
			double dLat, dLong, dNorth, dEast;

			callh_ptr = &call_history;

			/***********************************/
			/* NULL-terminating ASCII-strings. */
			/***********************************/

			cur_call->from_addr_street[ 20 ] = '\0';
			cur_call->from_addr_apart[ 5 ] = '\0';
			cur_call->from_addr_city[ 3 ] = '\0';
			cur_call->from_addr_cmnt[ 30 ] = '\0';
			cur_call->passenger[ 20 ] = '\0';
			cur_call->phone[ 10 ] = '\0';
			cur_call->to_addr_street[ 20 ] = '\0';
			cur_call->to_addr_apart[ 5 ] = '\0';
			cur_call->to_addr_city[ 3 ] = '\0';
			cur_call->to_addr_cmnt[ 30 ] = '\0';
			cur_call->driver_id[ 5 ] = '\0';
			cur_call->creation_date[ 6 ] = '\0';
			cur_call->creation_time[ 4 ] = '\0';
			cur_call->due_date[ 6 ] = '\0';
			cur_call->due_time[ 4 ] = '\0';
			cur_call->pickup_time[ 4 ] = '\0';
			cur_call->close_time[ 4 ] = '\0';
			cur_call->call_comment[ 64 ] = '\0';

			/******************************/
			/* Prepare call for db->       */
			/******************************/

                        //			MakeLogEntry("Preparing call for TaxiPak\n" );
			call_ptr = &call;
			memset((char *)call_ptr, '\0', sizeof( struct cisam_cl ));

			/* Initialize call index related information */
			call_ptr->nbr_of_vehs = '1';
			call_ptr->pos_in_group = nCallIndex + '0';
			call_ptr->nbr_in_group = cur_call->number_of_vehicles + '0';
			if ( cur_call->number_of_vehicles == 1 )
			{
					call_ptr->pos_in_group = ' ';
					call_ptr->nbr_in_group = ' ';
			}

                        //			MakeLogEntry("Init call index\n" );

			/* Creator of call is the API process! */
			/*
			cur_call->creator = API_DISPATCHER_ID;
			*/
			if ( cur_call->creator <= 0 )
			  call_ptr->entered_by = API_DISPATCHER_ID;
			else
			  call_ptr->entered_by = cur_call->creator;
			
			call_ptr->dispatched_by = API_DISPATCHER_ID;

			/* Copy information from client's struct into a cisam struct for database */
			call_ptr->fleet = cur_call->fleet;
			cur_call->number_of_calls = cur_call->number_of_vehicles; /* Num vehicles = num of calls in TP */
			cur_call->call_group = 0;
			strncpy( call_ptr->extended_type, cur_call->call_type, 5 );


			  
			call_ptr->pckup_str_nbr = cur_call->from_addr_number;
			call_ptr->dest_str_nbr = cur_call->to_addr_number;
			call_ptr->pckup_str_suf = cur_call->from_addr_suffix;
			call_ptr->dest_str_suf = cur_call->to_addr_suffix;
			strcpy( call_ptr->pckup_str_name, cur_call->from_addr_street );
			strncpy( call_ptr->pckup_str_key, cur_call->from_addr_street,4 );
			call_ptr->pckup_str_key[4] = 0;
			strcpy( call_ptr->dest_str_name, cur_call->to_addr_street );
			call_ptr->pckup_zone = cur_call->from_addr_zone;
			call_ptr->dest_zone = cur_call->to_addr_zone;
			strcpy( call_ptr->pckup_apt_room, cur_call->from_addr_apart );
			strcpy( call_ptr->dest_apt_room, cur_call->to_addr_apart );
			strcpy( call_ptr->pckup_city, cur_call->from_addr_city );
			strcpy( call_ptr->dest_city, cur_call->to_addr_city );
			strcpy( call_ptr->pckup_adr_cmnt, cur_call->from_addr_cmnt );
			strcpy( call_ptr->dest_adr_cmnt, cur_call->to_addr_cmnt );
			strcpy( call_ptr->passenger_name, cur_call->passenger );
			strncpy( call_ptr->pass_name_key, cur_call->passenger, 4 );
			call_ptr->pass_name_key[4] = 0;
			strcpy( call_ptr->phone_number, cur_call->phone );
			strcpy( call_ptr->general_cmnt, cur_call->call_comment );
			call_ptr->priority = cur_call->priority;
			call_ptr->veh_attr_flag = NO;
			call_ptr->drv_attr_flag = NO;


			max_attr = 32;

			for ( counter = 0; counter < max_attr; counter++ )
				{
					if ( cur_call->car_attrib[ counter ] == YES )
						{
							call_ptr->veh_attr[ counter ] = YES;
							call_ptr->veh_attr_flag = YES;
						}
					else
						call_ptr->veh_attr[ counter ] = NO;
				}

			for ( counter = 0; counter < max_attr; counter++ )
				{
					if ( cur_call->driver_attrib[ counter ] == YES )
						{
							call_ptr->drv_attr[ counter ] = YES;
							call_ptr->drv_attr_flag = YES;
						}
					else
						call_ptr->drv_attr[ counter ] = NO;
				}

			if (cur_call->car_number > 0)
			  {
			    // automatic assignment to specifica car
			    call_ptr->personal_veh = cur_call->car_number;
			    call_ptr->personal_rqst = ONLY_PERS_REQ;
			  }
			
			// Set an MPK attribute for HTD if originating there
			if ( ( 1 ) && ( ! strncmp(call_ptr->extended_type, "MPK ", 4) ) )
			  {
			    call_ptr->drv_attr[21] = YES; // MPK driver attribute

			    if ( call_ptr->veh_attr[30] != YES ) //INVA Taksi
			      call_ptr->veh_attr[7] = YES;  // EB vehicle attribute

			    // Use coordinates passed by MPK system if they are present
			    pMPKCall = (struct call *)cur_call;
			    pMPKCall->gpsy[8] = '\0';
			    
			    dLat = (double)atof(pMPKCall->gpsy)/1000000.0;
			    pMPKCall->gpsx[8] = '\0';
			    dLong = (double)atof(pMPKCall->gpsx)/1000000.0;
			    LatLong_to_UTM( dLat, dLong, &dNorth, &dEast );
			    printf( "Zone - %d\n", GetCustomerZone(7, dEast, dNorth ) );
			    call_ptr->pckup_zone = GetCustomerZone(7, dEast, dNorth);
			  }
			else if ( ! strncmp(call_ptr->extended_type, "KE", 2) )
			  {
			    pMPKCall = (struct call *)cur_call;
			    pMPKCall->gpsy[8] = '\0';
			    pMPKCall->gpsy[2] = *nl_langinfo(RADIXCHAR);
			    dLat = (double)atof(pMPKCall->gpsy);
			    pMPKCall->gpsx[8] = '\0';
			    pMPKCall->gpsx[2] = *nl_langinfo(RADIXCHAR);
			    dLong = (double)atof(pMPKCall->gpsx);
			    LatLong_to_UTM( dLat, dLong, &dNorth, &dEast );
			    call_ptr->pckup_zone = GetCustomerZone(7, dEast, dNorth);
			    cur_call->from_addr_zone = call_ptr->pckup_zone;
			  }
			else if ( ! strncmp(call_ptr->extended_type, "KV", 2) )
			  {
			    pMPKCall = (struct call *)cur_call;
			    pMPKCall->gpsy[8] = '\0';
			    pMPKCall->gpsy[2] = *nl_langinfo(RADIXCHAR);
			    dLat = (double)atof(pMPKCall->gpsy);
			    pMPKCall->gpsx[8] = '\0';
			    pMPKCall->gpsx[2] = *nl_langinfo(RADIXCHAR);
			    dLong = (double)atof(pMPKCall->gpsx);
			    LatLong_to_UTM( dLat, dLong, &dNorth, &dEast );
			    call_ptr->pckup_zone = GetCustomerZone(7, dEast, dNorth);
			    cur_call->from_addr_zone = call_ptr->pckup_zone;
			  }			

			    // Check to see if trip is zoned
			if ( cur_call->from_addr_zone == 0 )
			  zone_addr( call_ptr, PICKUP_ADDR, &zip );

			// zone_addr also added coordinates to the trip if present in geo tables
			// if this is an MPK trip, we should override with what we receive from MPK
			if ( ( 1 ) && ( ! strncmp(call_ptr->extended_type, "MPK ", 4) ) )
			  {
			    if ( dNorth > 0.0 && dEast > 0.0 )
			      {
				call_ptr->gps_lat = dNorth;
				call_ptr->gps_long = dEast;
			      }
			  }

                        if ( cur_call->to_addr_zone == 0 )
                          zone_addr( call_ptr, DEST_ADDR, &zip );

                        
			/* Make creation/date information for new call */
			get_pkup_date( call_ptr->date_in );
			TP_to_PI_date( call_ptr->date_in, cur_call->creation_date );
			get_pkup_time( call_ptr->time_in );
			TP_to_PI_time( call_ptr->time_in, cur_call->creation_time );
			call_ptr->date_time_in = time(NULL);

                        //			sprintf( szLogMsg, "fleet indx (%d), fleet_ptr(%ld)\n", (int)cur_call->fleet -'A', fleet);
                        //			MakeLogEntry( szLogMsg );
                        //			sprintf( szLogMsg, "cur_call->fleet (%c)\n", cur_call->fleet );
                        //			MakeLogEntry( szLogMsg );

			/* Get min/maximum values from fleet parameter
				 to determine if call is future call or not */
			now_time = time(NULL);
			max_time = now_time + (time_t) (fleet[ cur_call->fleet - 'A' ]->future_call_maximum);
			min_time = now_time + (time_t) (fleet[ cur_call->fleet - 'A' ]->future_call_minimum);
			strftime( now_ascii_time, 7, "%H\%M", localtime( &now_time ));   /* The \ is added to prevent interpretaion of the SCCS system */
			strftime( now_ascii_date, 10, "%y%m%d", localtime( &now_time ));
			strftime( max_ascii_time, 7, "%H\%M", localtime( &max_time ));
			strftime( max_ascii_date, 10, "%y%m%d", localtime( &max_time ));
			strftime( min_ascii_time, 7, "%H\%M", localtime( &min_time ));
			strftime( min_ascii_date, 10, "%d%m%y", localtime( &min_time ));

			/* determine if call is a future call */
			if ( !strncmp(call_ptr->extended_type, "KE", 2) )
			  {

			    if ( get_alt_tm_from_date_time( cur_call->due_date, cur_call->due_time ) <
				get_alt_tm_from_date_time( min_ascii_date, min_ascii_time ) )
			     future_call = 0;
			    else
			      future_call = 1;    /* Call is a legal future-call... */
			  }
			else if ( !strncmp(call_ptr->extended_type, "KV", 2) )
			  {

			    if ( get_alt_tm_from_date_time( cur_call->due_date, cur_call->due_time ) <
				get_alt_tm_from_date_time( min_ascii_date, min_ascii_time ) )
			     future_call = 0;
			    else
			      future_call = 1;    /* Call is a legal future-call... */
			  }
			else
			  future_call = 0;

                        //		sprintf( szLogMsg, "Call (%s)(%s) is (%d) future_call\n", cur_call->due_time,
                        //																										cur_call->due_date, future_call );
                        //			MakeLogEntry( szLogMsg );

                        //			sprintf( szLogMsg, "Fleet min(%s)(%s) and max(%s)(%s)\n", min_ascii_time, min_ascii_date,
                        //																										max_ascii_time, max_ascii_date );
//			MakeLogEntry( szLogMsg );

			/* Call is not a future call. (dispatch now) */
			if ( !future_call )
			{
                          if ( cur_call->call_type[ TIMEC_TYPE_OFF ] != 'K' )
                            {
                              cur_call->call_type[ TIMEC_TYPE_OFF ] = ' ';
                              call_ptr->extended_type[ TIMEC_TYPE_OFF ] = ' ';
                            }
					call_ptr->pri_status = call_ptr->priority;
					cur_call->call_status = UNASSIGNED;
					strcpy( call_ptr->status, catgets(WRITER_catd, 1, 39, "UNASSGND" ) );
					get_pkup_date( call_ptr->due_date );
					get_pkup_time( call_ptr->due_time );
                                        call_ptr->due_date_time = get_local_time();

                                        if ( strlen( cur_call->due_date ) )
                                          PI_to_TP_date( cur_call->due_date, call_ptr->due_date );
                                        if ( strlen( cur_call->due_time ) )
                                          PI_to_TP_time( cur_call->due_time, call_ptr->due_time );
				}
			else        /* Call is a legal time-call... */
				{
                                  if ( cur_call->call_type[ TIMEC_TYPE_OFF ] != 'K' )
                                    {
                                      cur_call->call_type[ TIMEC_TYPE_OFF ] = TIMEC;
                                      call_ptr->extended_type[ TIMEC_TYPE_OFF ] = TIMEC;
                                    }


				  /*Set premium attributes */
				  call_ptr->drv_attr_flag = YES;
				  call_ptr->drv_attr[25] = YES;
				  call_ptr->veh_attr_flag = YES;
				  call_ptr->veh_attr[25] = YES;
				  
					call_ptr->pri_status = 63;
					cur_call->call_status = PENDING;
					strcpy( call_ptr->status, catgets(WRITER_catd, 1, 41, "PENDING" ) );
					call_ptr->due_date_time = (time_t)get_internal_time( (char *)cur_call->due_date, (char *)cur_call->due_time );

					PI_to_TP_date( cur_call->due_date, call_ptr->due_date );
					PI_to_TP_time( cur_call->due_time, call_ptr->due_time );
				}


			if ( nType == EXT_CALL )
			{
				call_ptr->ext_nbr = cur_call->ftj_number;
				call_ptr->offer_type = cur_call->offer_type;
				call_ptr->detail_type = cur_call->detail_type;
				call_ptr->call_msg_nbr = cur_call->msg_nbr;
			}
			else
				call_ptr->ext_nbr = NOT_EXTERNAL;

			if ( glEnhSwitch & ENH0002 )
			  {
			    CityNavGetCoords( call_ptr );

			  }

                       call_ptr->dest_gpsx = 0.0;
                       call_ptr->dest_gpsy = 0.0;
			/******************************/
			/* Write call(s) to DB.       */
			/******************************/
//			MakeLogEntry( "Laying call into database\n" );

			memcpy(req_buf, (char *)call_ptr, sizeof( struct cisam_cl )); /* Pointer to the Request-buffer. */

			if ( db(CALL_FILE_ID, ADD_REC, &cl_key1, ISEQUAL ) < 0 )
				{
					ERROR( "Cannot add call from PI (DB)." );
					return FALSE;
				}

			call_ptr->nbr = ((struct cisam_cl *)req_buf)->nbr;
			db_call_num = isrecnum;


			sprintf( szLogMsg, "call(%d) in database\n", db_call_num  );
//			MakeLogEntry( szLogMsg );

			/******************************/
			/* Prepare call-history.      */
			/******************************/

			memset((char *)callh_ptr, '\0', sizeof( struct cisam_ch ));

			callh_ptr->call_nbr = call_ptr->nbr;
			callh_ptr->exception_nbr = 0;
			callh_ptr->fleet = call_ptr->fleet;
			callh_ptr->event_type = REGULAR_EVENT;
			callh_ptr->status_dt_tm = (int)time(NULL);
			callh_ptr->veh_nbr = 0;
			callh_ptr->drv_id = 0;
			callh_ptr->user_id = call_ptr->entered_by;

			get_pkup_date( callh_ptr->status_date );
			get_pkup_time( callh_ptr->status_time );

			strcpy( callh_ptr->event_desc, catgets(WRITER_catd, 1, 42, "ENTERED" ) );

			/******************************/
			/* Write to calls-history.    */
			/******************************/

			memcpy(req_buf, (char *)callh_ptr, sizeof( struct cisam_ch ));  /* Pointer to the Request-buffer. */
			if ( db(CALLH_FILE_ID, ADD_REC, &ch_key1, ISEQUAL ) < 0 )
				{
					ERROR( "Cannot add call-history-record for PI-call (DB)." );
					return FALSE;
				}
//			MakeLogEntry( "Laying call_history rec into database\n" );

			/******************************/
			/* Prepare call for dispatch. */
			/******************************/

			memset((char *)spCallRec, '\0', sizeof( struct call_ipc_rec ));

			spCallRec->u_prcs_id = pid;
			spCallRec->rec_type = cRecType;  /* call type ( Samplan/Normal from client ) */
			spCallRec->c_isam_nbr = db_call_num;
			spCallRec->call_nbr = call_ptr->nbr;
			spCallRec->fleet = call_ptr->fleet;
			spCallRec->pickup_zone = call_ptr->pckup_zone;
			spCallRec->dest_zone = call_ptr->dest_zone;
			spCallRec->priority = call_ptr->priority;
			spCallRec->due_time = call_ptr->due_date_time;
			spCallRec->msg_nbr = 0;
			spCallRec->updated_call = 0;
			spCallRec->old_due_time = 0;
			strcpy(spCallRec->pick_pmap, call_ptr->pckup_pmap);

			spCallRec->FTJ_nbr = -1; //cur_call->ftj_number;
			spCallRec->offer_type = cur_call->offer_type;
			spCallRec->detail_type = cur_call->detail_type;
			spCallRec->call_msg_nbr = cur_call->msg_nbr;


			if (cur_call->car_number > 0)
			  {
			    spCallRec->personal_veh = cur_call->car_number;
			    spCallRec->personal_rqst = ONLY_PERS_REQ;
			  }
			
			/* Setup specific call-types ( Passenger, Delivery, Timecall... ). */

			memset((char *)&spCallRec->call_type, '\0', sizeof(struct call_types));

			if ( cur_call->call_type[ 0 ] == PASS )
				spCallRec->call_type.passenger = TRUE;
			else
				if ( cur_call->call_type[ 0 ] == DELIV )
					spCallRec->call_type.delivery = TRUE;
				else
					if ( cur_call->call_type[ 0 ] == TYPEX )
						spCallRec->call_type.xtype = TRUE;
					else
						if ( cur_call->call_type[ 0 ] == WAKEUP )
							spCallRec->call_type.wakeup = TRUE;

			if ( cur_call->call_type[ 1 ] == AUTOBOOK_CALL )
				spCallRec->call_type.passenger = TRUE;

			if ( cur_call->call_type[ 2 ] == TIMEC )
				spCallRec->call_type.time = TRUE;
			else
				if ( cur_call->call_type[ 2 ] == SUBS )
					spCallRec->call_type.subscription = TRUE;

			if ( cur_call->call_type[ 3 ] == CHARGE )
				spCallRec->call_type.charge = TRUE;

			if ( cur_call->call_type[ 4 ] == MULT_CALL )
				spCallRec->call_type.multiple = TRUE;

			//Interlink trips
			if ( ( cur_call->call_type[0] == 'M' ) && ( cur_call->call_type[1] == '8' ) )
			  {
			    spCallRec->call_type.ilink = TRUE;

			  }

			/* Setup the attributes for vehicle and driver. */

			memset((char *)&spCallRec->veh_attr, '\0', sizeof(struct veh_attributes));
			memset((char *)&spCallRec->drv_attr, '\0', sizeof(struct drv_attributes));

			for ( counter = 0; counter < max_attr; counter++ )
				{
					if ( call_ptr->veh_attr[ counter ] == YES )
						set_attr_bit( counter, sizeof( struct veh_attributes ), &spCallRec->veh_attr );

					if ( call_ptr->drv_attr[ counter ] == YES )
						set_attr_bit( counter, sizeof( struct drv_attributes ), &spCallRec->drv_attr );
				}
			
			spCallRec->gps_lat = call_ptr->gps_lat;
			spCallRec->gps_long = call_ptr->gps_long;

			return spCallRec->call_nbr;

		} /* end  EnterCallinTaxiPak() */

int EnterNonExtCallinTaxiPak( struct call *cur_call, int nCallIndex,struct cisam_cl *call_ptr,
			int nType,char cRecType, struct call_ipc_rec *spCallRec )
{
			int db_call_num;    /* The call-number assigned to the call_ptr.  */
			char *position;   /* Current position in data-block.    */
			char future_call;   /* Indicating whether call is a future-call.  */
			char now_ascii_date[ 12 ],  /* Times to compare for deciding    */
					 now_ascii_time[ 7 ], /* whether to dispatch a future call    */
					 min_ascii_date[ 12 ],  /* or now call.         */
					 min_ascii_time[ 7 ],
					 max_ascii_date[ 12 ],
					 max_ascii_time[ 7 ];
			time_t now_time,
						 min_time,
						 max_time;
			int counter;
			int max_attr;
                        int zip = 0;
                        struct call_ext ext_call;

			struct cisam_ch call_history, *callh_ptr; /* The call-history-structure similar to the one used in TaxiPak.   */


                        bzero( &ext_call, sizeof( struct call_ext ) );
			callh_ptr = &call_history;

			/***********************************/
			/* NULL-terminating ASCII-strings. */
			/***********************************/

			cur_call->from_addr_street[ 20 ] = '\0';
			cur_call->from_addr_apart[ 5 ] = '\0';
			cur_call->from_addr_city[ 3 ] = '\0';
			cur_call->from_addr_cmnt[ 30 ] = '\0';
			cur_call->passenger[ 20 ] = '\0';
			cur_call->phone[ 10 ] = '\0';
			cur_call->to_addr_street[ 20 ] = '\0';
			cur_call->to_addr_apart[ 5 ] = '\0';
			cur_call->to_addr_city[ 3 ] = '\0';
			cur_call->to_addr_cmnt[ 30 ] = '\0';
			cur_call->driver_id[ 5 ] = '\0';
			cur_call->creation_date[ 6 ] = '\0';
			cur_call->creation_time[ 4 ] = '\0';
			cur_call->due_date[ 6 ] = '\0';
			cur_call->due_time[ 4 ] = '\0';
			cur_call->pickup_time[ 4 ] = '\0';
			cur_call->close_time[ 4 ] = '\0';
			cur_call->call_comment[ 64 ] = '\0';

			/******************************/
			/* Prepare call for db->       */
			/******************************/

                        //			MakeLogEntry("Preparing call for TaxiPak\n" );
			call_ptr = &call;
			memset((char *)call_ptr, '\0', sizeof( struct cisam_cl ));

			/* Initialize call index related information */
			call_ptr->nbr_of_vehs = '1';
			call_ptr->pos_in_group = nCallIndex + '0';
			call_ptr->nbr_in_group = cur_call->number_of_vehicles + '0';
			if ( cur_call->number_of_vehicles == 1 )
			{
					call_ptr->pos_in_group = ' ';
					call_ptr->nbr_in_group = ' ';
			}

                        //			MakeLogEntry("Init call index\n" );

			/* Creator of call is the API process! */
			/*
			cur_call->creator = API_DISPATCHER_ID;
			*/
			if ( cur_call->creator <= 0 )
			  call_ptr->entered_by = API_DISPATCHER_ID;
			else
			  call_ptr->entered_by = cur_call->creator;
			
			call_ptr->dispatched_by = API_DISPATCHER_ID;

			/* Copy information from client's struct into a cisam struct for database */
			call_ptr->fleet = cur_call->fleet;
			cur_call->number_of_calls = cur_call->number_of_vehicles; /* Num vehicles = num of calls in TP */
			cur_call->call_group = 0;
			strncpy( call_ptr->extended_type, cur_call->call_type, 5 );
			call_ptr->pckup_str_nbr = cur_call->from_addr_number;
			call_ptr->dest_str_nbr = cur_call->to_addr_number;
			call_ptr->pckup_str_suf = cur_call->from_addr_suffix;
			call_ptr->dest_str_suf = cur_call->to_addr_suffix;
			strcpy( call_ptr->pckup_str_name, cur_call->from_addr_street );
			strncpy( call_ptr->pckup_str_key, cur_call->from_addr_street,4 );
			call_ptr->pckup_str_key[4] = 0;
			strcpy( call_ptr->dest_str_name, cur_call->to_addr_street );
			call_ptr->pckup_zone = cur_call->from_addr_zone;
			call_ptr->dest_zone = cur_call->to_addr_zone;
			strcpy( call_ptr->pckup_apt_room, cur_call->from_addr_apart );
			strcpy( call_ptr->dest_apt_room, cur_call->to_addr_apart );
			strcpy( call_ptr->pckup_city, cur_call->from_addr_city );
			strcpy( call_ptr->dest_city, cur_call->to_addr_city );
			strcpy( call_ptr->pckup_adr_cmnt, cur_call->from_addr_cmnt );
			strcpy( call_ptr->dest_adr_cmnt, cur_call->to_addr_cmnt );
			strcpy( call_ptr->passenger_name, cur_call->passenger );
			strncpy( call_ptr->pass_name_key, cur_call->passenger, 4 );
			call_ptr->pass_name_key[4] = 0;
			strcpy( call_ptr->phone_number, cur_call->phone );
			strcpy( call_ptr->general_cmnt, cur_call->call_comment );
			call_ptr->priority = cur_call->priority;
			call_ptr->veh_attr_flag = NO;
			call_ptr->drv_attr_flag = NO;


			max_attr = 32;

			for ( counter = 0; counter < max_attr; counter++ )
				{
					if ( cur_call->car_attrib[ counter ] == YES )
						{
							call_ptr->veh_attr[ counter ] = YES;
							call_ptr->veh_attr_flag = YES;
						}
					else
						call_ptr->veh_attr[ counter ] = NO;
				}

			for ( counter = 0; counter < max_attr; counter++ )
				{
					if ( cur_call->driver_attrib[ counter ] == YES )
						{
							call_ptr->drv_attr[ counter ] = YES;
							call_ptr->drv_attr_flag = YES;
						}
					else
						call_ptr->drv_attr[ counter ] = NO;
				}

                        // If we've got GPS position on the call set it here
                        // This will prevent 'zone_addr' from setting the GPS
                        // position using Tatort2000

                        if ( ( atoi( cur_call->gpsx ) > 0 ) &&
                             ( atoi( cur_call->gpsy ) > 0 ) ) {
                          
                          call_ptr->gps_long = (double)atoi( cur_call->gpsx );
                          call_ptr->gps_lat = (double)atoi( cur_call->gpsy );
                          
                        }
                          

                        // Check to see if trip is zoned
                        if ( cur_call->from_addr_zone == 0 )
                          zone_addr( call_ptr, PICKUP_ADDR, &zip );

                        if ( cur_call->to_addr_zone == 0 )
                          zone_addr( call_ptr, DEST_ADDR, &zip );                        
                        
			/* Make creation/date information for new call */
			get_pkup_date( call_ptr->date_in );
			TP_to_PI_date( call_ptr->date_in, cur_call->creation_date );
			get_pkup_time( call_ptr->time_in );
			TP_to_PI_time( call_ptr->time_in, cur_call->creation_time );
			call_ptr->date_time_in = time(NULL);

                        //			sprintf( szLogMsg, "fleet indx (%d), fleet_ptr(%ld)\n", (int)cur_call->fleet -'A', fleet);
                        //			MakeLogEntry( szLogMsg );
                        //			sprintf( szLogMsg, "cur_call->fleet (%c)\n", cur_call->fleet );
                        //			MakeLogEntry( szLogMsg );

			/* Get min/maximum values from fleet parameter
				 to determine if call is future call or not */
			now_time = time(NULL);
			max_time = now_time + (long) (fleet[ cur_call->fleet - 'A' ]->future_call_maximum);
			min_time = now_time + (long) (fleet[ cur_call->fleet - 'A' ]->future_call_minimum);
			strftime( now_ascii_time, 7, "%H\%M", localtime( &now_time ));   /* The \ is added to prevent interpretaion of the SCCS system */
			strftime( now_ascii_date, 10, "%y%m%d", localtime( &now_time ));
			strftime( max_ascii_time, 7, "%H\%M", localtime( &max_time ));
			strftime( max_ascii_date, 10, "%y%m%d", localtime( &max_time ));
			strftime( min_ascii_time, 7, "%H\%M", localtime( &min_time ));
			strftime( min_ascii_date, 10, "%y%m%d", localtime( &min_time ));

			/* determine if call is a future call */
			if ( glEnhSwitch & ENH0002 )
			  {                        
                            if ( get_tm_from_date_time( cur_call->due_date, cur_call->due_time ) <
                                 get_tm_from_date_time( min_ascii_date, min_ascii_time ) )
                              future_call = 0;
                            else
                              future_call = 1;    /* Call is a legal future-call... */
                          }
                        else
                          future_call = 0;
                        
                            

			/* Call is not a future call. (dispatch now) */
			if ( !future_call )
			{
					cur_call->call_type[ TIMEC_TYPE_OFF ] = ' ';
					call_ptr->extended_type[ TIMEC_TYPE_OFF ] = ' ';
					call_ptr->pri_status = call_ptr->priority;
					cur_call->call_status = UNASSIGNED;
					strcpy( call_ptr->status, "EJ TILLD" );
					get_pkup_date( call_ptr->due_date );
					get_pkup_time( call_ptr->due_time );
					call_ptr->due_date_time = get_local_time();

					PI_to_TP_date( cur_call->due_date, call_ptr->due_date );
					PI_to_TP_time( cur_call->due_time, call_ptr->due_time );
				}
			else        /* Call is a legal time-call... */
				{
					cur_call->call_type[ TIMEC_TYPE_OFF ] = TIMEC;
					call_ptr->extended_type[ TIMEC_TYPE_OFF ] = TIMEC;
					call_ptr->pri_status = 63;
					cur_call->call_status = PENDING;
					strcpy( call_ptr->status, "VÄNTAR" );
					call_ptr->due_date_time = (time_t)get_internal_time( cur_call->due_date, cur_call->due_time );

					PI_to_TP_date( cur_call->due_date, call_ptr->due_date );
					PI_to_TP_time( cur_call->due_time, call_ptr->due_time );
				}

                        
                        call_ptr->ext_nbr = NOT_EXTERNAL;

			if ( glEnhSwitch & ENH0002 )
			  {
			    CityNavGetCoords( call_ptr );

			  }

                       call_ptr->dest_gpsx = 0.0;
                       call_ptr->dest_gpsy = 0.0;
			/******************************/
			/* Write call(s) to DB.       */
			/******************************/
//			MakeLogEntry( "Laying call into database\n" );

			memcpy(req_buf, (char *)call_ptr, sizeof( struct cisam_cl )); /* Pointer to the Request-buffer. */

			if ( db(CALL_FILE_ID, ADD_REC, &cl_key1, ISEQUAL ) < 0 )
				{
					ERROR( "Cannot add call from PI (DB)." );
					return FALSE;
				}

			call_ptr->nbr = ((struct cisam_cl *)req_buf)->nbr;
			db_call_num = isrecnum;


			sprintf( szLogMsg, "call(%d) in database\n", db_call_num  );
//			MakeLogEntry( szLogMsg );

			/******************************/
			/* Prepare call-history.      */
			/******************************/

			memset((char *)callh_ptr, '\0', sizeof( struct cisam_ch ));

			callh_ptr->call_nbr = call_ptr->nbr;
			callh_ptr->exception_nbr = 0;
			callh_ptr->fleet = call_ptr->fleet;
			callh_ptr->event_type = REGULAR_EVENT;
			callh_ptr->status_dt_tm = (int)time(NULL);
			callh_ptr->veh_nbr = 0;
			callh_ptr->drv_id = 0;
			callh_ptr->user_id = call_ptr->entered_by;

			get_pkup_date( callh_ptr->status_date );
			get_pkup_time( callh_ptr->status_time );

			strcpy( callh_ptr->event_desc, "INSKR   " );

			/******************************/
			/* Write to calls-history.    */
			/******************************/

			memcpy(req_buf, (char *)callh_ptr, sizeof( struct cisam_ch ));  /* Pointer to the Request-buffer. */
			if ( db(CALLH_FILE_ID, ADD_REC, &ch_key1, ISEQUAL ) < 0 )
				{
					ERROR( "Cannot add call-history-record for PI-call (DB)." );
					return FALSE;
				}
//			MakeLogEntry( "Laying call_history rec into database\n" );

			/******************************/
			/* Prepare call for dispatch. */
			/******************************/

			memset((char *)spCallRec, '\0', sizeof( struct call_ipc_rec ));

			spCallRec->u_prcs_id = pid;
			spCallRec->rec_type = cRecType;  /* call type ( Samplan/Normal from client ) */
			spCallRec->c_isam_nbr = db_call_num;
			spCallRec->call_nbr = call_ptr->nbr;
			spCallRec->fleet = call_ptr->fleet;
			spCallRec->pickup_zone = call_ptr->pckup_zone;
			spCallRec->dest_zone = call_ptr->dest_zone;
			spCallRec->priority = call_ptr->priority;
			spCallRec->due_time = call_ptr->due_date_time;
			spCallRec->msg_nbr = 0;
			spCallRec->updated_call = 0;
			spCallRec->old_due_time = 0;
			strcpy(spCallRec->pick_pmap, call_ptr->pckup_pmap);

			spCallRec->FTJ_nbr = 0;
			spCallRec->offer_type = 0;
			spCallRec->detail_type = 0;
			spCallRec->call_msg_nbr = 0;


			/* Setup specific call-types ( Passenger, Delivery, Timecall... ). */

			memset((char *)&spCallRec->call_type, '\0', sizeof(struct call_types));

			if ( cur_call->call_type[ 0 ] == PASS )
				spCallRec->call_type.passenger = TRUE;
			else
				if ( cur_call->call_type[ 0 ] == DELIV )
					spCallRec->call_type.delivery = TRUE;
				else
					if ( cur_call->call_type[ 0 ] == TYPEX )
						spCallRec->call_type.xtype = TRUE;
					else
						if ( cur_call->call_type[ 0 ] == WAKEUP )
							spCallRec->call_type.wakeup = TRUE;

			if ( cur_call->call_type[ 1 ] == AUTOBOOK_CALL )
				spCallRec->call_type.passenger = TRUE;

			if ( cur_call->call_type[ 2 ] == TIMEC )
				spCallRec->call_type.time = TRUE;
			else
				if ( cur_call->call_type[ 2 ] == SUBS )
					spCallRec->call_type.subscription = TRUE;

			if ( cur_call->call_type[ 3 ] == CHARGE )
				spCallRec->call_type.charge = TRUE;

			if ( cur_call->call_type[ 4 ] == MULT_CALL )
				spCallRec->call_type.multiple = TRUE;

			/* Setup the attributes for vehicle and driver. */

			memset((char *)&spCallRec->veh_attr, '\0', sizeof(struct veh_attributes));
			memset((char *)&spCallRec->drv_attr, '\0', sizeof(struct drv_attributes));

			for ( counter = 0; counter < max_attr; counter++ )
				{
					if ( call_ptr->veh_attr[ counter ] == YES )
						set_attr_bit( counter, sizeof( struct veh_attributes ), &spCallRec->veh_attr );

					if ( call_ptr->drv_attr[ counter ] == YES )
						set_attr_bit( counter, sizeof( struct drv_attributes ), &spCallRec->drv_attr );
				}
			
			spCallRec->gps_lat = call_ptr->gps_lat;
			spCallRec->gps_long = call_ptr->gps_long;

			return spCallRec->call_nbr;

		} /* end  EnterNonExtCallinTaxiPak() */

/* ----------------------------------------------------------------------------
/* FUNCTION: ValidZonedCallRec
/*
/* DESCRIPTION: Determine if fields are valid for zoned call data for TaxiPak
/*
/* PARAMETER: struct *call_rec
/*
/* RETURN-DATA:  if return-value = 1: fields OK,
/*
/*               if return-value = 0: invalid data.
/*
------------------------------------------------------------------------------ */


int ValidZonedCallRecord( struct call *cur_call, return_type *return_data )
{

	/* Check if call zoned yet */
	if ( cur_call->from_addr_zone < MIN_ADDR_ZONE  ||
			cur_call->from_addr_zone > MAX_ADDR_ZONE )
	{
		 printf( "Invalid zone(%d) on call dest(%s)", cur_call->from_addr_zone,
																									cur_call->from_addr_street );
		 MakeResponseMsg( return_data, (short) PI_INV_ZONE,
								 "Invalid pickup-zone: PI_DISPATCH_EXT_CALL" );
		 return FALSE;
	}

	return ( ValidCallRecord( cur_call, return_data ) );
}


void  MakePiExcept( int nPid, int  nType )
{
	struct sp_excpt_rec   sException;

	sException.u_prcs_id = nPid;
	sException.rec_type = SP_EXCEPT_REQ;
	sException.call_nbr = 0;
	sException.except_info = SYS_ERR ;  /* this not used currently in dispatch process */
	sException.except_type = nType;

	if (write(fifo_fd,&sException,sizeof(struct sp_excpt_rec)) == -1) /* Write FIFO to dispatch */
		ERROR("Error writing to FIFO.");
}

void  MakeSysExcept( int nPid, int  nInfo )
{
	struct sp_excpt_rec   sException;

	sException.u_prcs_id = nPid;
	sException.rec_type = SP_EXCEPT_REQ;
	sException.call_nbr = 0;
	sException.except_info = nInfo ;  /* this not used currently in dispatch process */
	sException.except_type = SYS_ERR;

	if (write(fifo_fd,&sException,sizeof(struct sp_excpt_rec)) == -1) /* Write FIFO to dispatch */
		ERROR("Error writing to FIFO.");
}

		/* ----------------------------------------------------------------------------
		/* FUNCTION: ValidCallRec
		/*
		/* DESCRIPTION: Determine if fields are valid data for TaxiPak
		/*
		/* PARAMETER: struct *call_rec
		/*
		/* RETURN-DATA:  if return-value = 1: fields OK,
		/*
		/*               if return-value = 0: invalid data.
		/*
		------------------------------------------------------------------------------ */


 int ValidCallRecord( struct call *cur_call, return_type *return_data )
 {
		short counter;
		short fl_index = 0;
		int  nOff,x;
		char  *ptr;

#ifdef KEVIN
		for ( x=0,ptr = (char*)cur_call; x < sizeof( struct call ) ; x++,ptr++ )
		{
			sprintf( szLogMsg, "(%d)Line(%d)\n", *ptr, x );
			MakeLogEntry( szLogMsg );
		}
		nOff = (int)  ( (char*)cur_call->from_addr_street
							- (char*)cur_call );
		sprintf( szLogMsg, "Offset in struct (%d), ptr1(%d), ptr2(%d)\n", nOff,
															(int) cur_call->from_addr_street,
															(int) cur_call);
		MakeLogEntry( szLogMsg );
		sprintf( szLogMsg, "(%c)(%d)(%s)(%s)(%s)\n", cur_call->fleet,
													cur_call->priority,
													cur_call->from_addr_street, cur_call->from_addr_apart,
													cur_call->from_addr_city );

		MakeLogEntry( szLogMsg );
#endif

                //		MakeLogEntry( "Validating fields\n" );
		if( (!ValidMMPmessage( cur_call->from_addr_street, 20 ) ) ||
			(!ValidMMPmessage( cur_call->from_addr_apart, 5 ) ) ||
			(!ValidMMPmessage( cur_call->from_addr_city, 3 ) ) ||
			(!ValidMMPmessage( cur_call->from_addr_cmnt, 30 ) ) ||
			(!ValidMMPmessage( cur_call->passenger, 20 ) ) ||
			(!ValidMMPmessage( cur_call->to_addr_street, 20 ) ) ||
			(!ValidMMPmessage( cur_call->to_addr_apart, 5 ) ) ||
			(!ValidMMPmessage( cur_call->to_addr_city, 3 ) ) ||
			(!ValidMMPmessage( cur_call->to_addr_cmnt, 30 ) ) ||
			(!ValidMMPmessage( cur_call->call_comment, 64 ) )     )
		{
			MakeResponseMsg( return_data, (short) PI_INVALID_MMP_CHAR,
															"Invalid text data for MMP: PI_DISPATCH_EXT_CALL" );
			return FALSE;
		}

                //		MakeLogEntry( "Validating fields finished\n" );

		if ( cur_call->fleet < MIN_FLEET || cur_call->fleet > MAX_FLEET )   /* Is fleet valid??? (A-H) */
		{
			MakeResponseMsg( return_data, (short) PI_INVALID_FLEET,
															"Invalid fleet: PI_DISPATCH_EXT_CALL" );
			return FALSE;
		}
    else
		{
		  for ( fl_index = 0; fl_index < FLEET_MAX; fl_index++ )
		    if ( fleet[fl_index]->fleet_id == cur_call->fleet )
		      break;

		  if (fleet[fl_index]->active != YES)
		    return( MakeResponseMsg( return_data, (short) PI_INVALID_FLEET,
					     "Invalid fleet: PI_DISPATCH_EXT_CALL" ) );
		}


		if ( cur_call->priority < MIN_PRIORITY ||
				 cur_call->priority > MAX_PRIORITY )  /* Is priority valid??? (6-63) */
		{
			MakeResponseMsg( return_data, (short) PI_INVALID_PRIO,
															"Invalid priority: PI_DISPATCH_EXT_CALL" );
			return FALSE;
		}


#ifdef FOO
		if ( atoi(cur_call->number_of_vehicles)!= 0 &&
		     cur_call->number_of_vehicles != 0 )
		  {
		    if ( cur_call->number_of_vehicles < MIN_NUM_VEHICLES ||
			 cur_call->number_of_vehicles > MAX_NUM_VEHICLES )  /* # of vehicles (1 - 9) */
		      {
			MakeResponseMsg( return_data, (short) PI_INVALID_VEH,
					 "Invalid num of vehicles: PI_DISPATCH_EXT_CALL" );
			return FALSE;
		      }
		  }
#endif

#ifdef FOO
		if ( cur_call->call_type[ ATTRIB_SLOT_0 ] != ' ' )        /* Check of call_type - 1st column... */
			if (( cur_call->call_type[ ATTRIB_SLOT_0 ] != PASS && cur_call->call_type[ ATTRIB_SLOT_0 ] != DELIV ) &&
					( cur_call->call_type[ ATTRIB_SLOT_0 ] != TYPEX && cur_call->call_type[ ATTRIB_SLOT_0 ] != WAKEUP ))
			{
				MakeResponseMsg( return_data, (short) PI_INVALID_CALL_1,
												"Invalid call type(Pass,Deliv,Extra,Wakeup): PI_DISPATCH_EXT_CALL" );
				return FALSE;
			}


		if ( cur_call->call_type[ ATTRIB_SLOT_1 ] != ' ' )        /* Check of call_type - 2nd column... */
			if ( ( cur_call->call_type[ ATTRIB_SLOT_1 ] != AUTOBOOK_CALL ) &&
			     ( cur_call->call_type[ ATTRIB_SLOT_1 ] != 'P' ) &&
                             ( cur_call->call_type[ ATTRIB_SLOT_1 ] != 'Y' ) &&
			     ( cur_call->call_type[ ATTRIB_SLOT_1 ] != '8' ))
			{
				MakeResponseMsg( return_data, (short) PI_INVALID_CALL_2,
										"Invalid call type(Autobooker): PI_DISPATCH_EXT_CALL" );
				return FALSE;
			}


		if ( cur_call->call_type[ ATTRIB_SLOT_2 ] != ' ' )        /* Check of call_type - 3rd column... */
			if ( ( cur_call->call_type[ ATTRIB_SLOT_2 ] != TIMEC ) &&
                             ( cur_call->call_type[ ATTRIB_SLOT_2 ] != SUBS ) &&
                             ( cur_call->call_type[ ATTRIB_SLOT_2 ] != 'K' ) &&
			     ( cur_call->call_type[ ATTRIB_SLOT_2 ] != '0' )) /* HTD has MYK type trips designated */
			{
				MakeResponseMsg( return_data, (short) PI_INVALID_CALL_3,
										"Invalid call type(Timecall,Subscr): PI_DISPATCH_EXT_CALL" );
				return FALSE;
			}


		if ( cur_call->call_type[ ATTRIB_SLOT_3 ] != ' ' )        /* Check of call_type - 4th column... */
			if ( ( cur_call->call_type[ ATTRIB_SLOT_3 ] != CHARGE ) &&
			     ( cur_call->call_type[ ATTRIB_SLOT_3 ] != '0' ) )
			{
				MakeResponseMsg( return_data, (short) PI_INVALID_CALL_4,
										"Invalid call-type(Charge): PI_DISPATCH_EXT_CALL" );
				return FALSE;
			}

#endif



		if (( cur_call->from_addr_number < MIN_ADDR_NBR ) ||
				( cur_call->from_addr_number > MAX_ADDR_NBR ))  /* Any kind of pickup-address#? */
		{
				MakeResponseMsg( return_data, (short) PI_INVALID_ADDR_NUM,
										"Invalid street-number(pickup-address): PI_DISPATCH_EXT_CALL" );
				return FALSE;
		}


    for ( counter = 0; counter < 32; counter++ )			/* Vehicle-attributes must be Y/N */
			if (( cur_call->car_attrib[ counter ] != YES ) &&
			    ( cur_call->car_attrib[ counter ] != NO ) &&
			    ( cur_call->car_attrib[ counter ] != NULL ))
			{
				MakeResponseMsg( return_data, (short) PI_INV_VEH_ATTRIB,
										"Invalid attributes (vehicle): PI_DISPATCH_EXT_CALL" );
				return FALSE;
			}


    for ( counter = 0; counter < 32; counter++ )			/* Driver-attributes must be Y/N */
			if (( cur_call->driver_attrib[ counter ] != YES ) &&
			    ( cur_call->driver_attrib[ counter ] != NO ) &&
			    ( cur_call->driver_attrib[ counter ] != NULL ))
			{
				MakeResponseMsg( return_data, (short) PI_INV_DRV_ATTRIB,
										"Invalid attributes (driver): PI_DISPATCH_EXT_CALL" );
					return FALSE;
			}



		
		if (( fleet[ fl_index ]->zone_ptr[ cur_call->from_addr_zone ] == NULL )  && ( cur_call->from_addr_zone != 0 ))
		{
				MakeResponseMsg( return_data, (short) PI_INV_ZONE,
									"Invalid pickup-zone(array): PI_DISPATCH_EXT_CALL" );
				return FALSE;
		}

                //		MakeLogEntry( "Validated call\n" );

		return 1;
 }


 /* ----------------------------------------------------------------------------
 /* FUNCTION: ValidFtjInfoCallRec
 /*
 /* DESCRIPTION: Determine ( call_msg record for Ftj call)
 /*                          fields are valid data for TaxiPak
 /*
 /* PARAMETER: struct *call_rec
 /*
 /* RETURN-DATA:  if return-value = 1: fields OK,
 /*
 /*               if return-value = 0: invalid data.
 /*
 ------------------------------------------------------------------------------ */
 int ValidFtjInfoCallRec( struct call_ext *cur_call, return_type *return_data  )
 {
	 sprintf( szLogMsg, " Ftj#(%d), Offer(%d), Type(%d), Msgs(%d)\n",
							cur_call->ftj_number, (int)cur_call->offer_type-'0',
							(int)cur_call->detail_type-'0', (int)cur_call->msg_nbr-'0' );
	 MakeLogEntry( szLogMsg );

#ifdef NDEBUG
	 printf("ValidFtjInfoCallRec values:\n");
	 printf("    cur_call->ftj_number=%d, (%d)\n",cur_call->ftj_number,MIN_FTJ_NBR);
	 printf("    cur_call->offer_type=%d, (%d,%d)\n",cur_call->offer_type,MIN_SAMPLAN_OFFER_TYPE,MAX_SAMPLAN_OFFER_TYPE);
	 printf("    cur_call->=detail_type%d, (%d,%d)\n",cur_call->detail_type,MIN_SAMPLAN_DETAIL_TYPE,MAX_SAMPLAN_DETAIL_TYPE);
	 printf("    cur_call->=msg_nbr%d (%d,%d)\n",cur_call->msg_nbr,MIN_SAMPLAN_CMSGS,MAX_SAMPLAN_CMSGS);
	 printf("\n\n");
#endif

	 if ( cur_call->ftj_number < MIN_FTJ_NBR )
	 {
			MakeResponseMsg( return_data, (short) PI_INV_FTJ_NBR,
								 "Invalid Ftj number: PI_DISPATCH_EXT_CALL" );
			return FALSE;
	 }

	 if (( cur_call->offer_type-'0' < MIN_SAMPLAN_OFFER_TYPE ) ||
			 ( cur_call->offer_type-'0' > MAX_SAMPLAN_OFFER_TYPE ))
	 {
			MakeResponseMsg( return_data, (short) PI_INV_OFFER_TYPE,
								 "Invalid offer type: PI_DISPATCH_EXT_CALL" );
			return FALSE;
	 }

	 if (( cur_call->detail_type-'0' < MIN_SAMPLAN_DETAIL_TYPE ) ||
			 ( cur_call->detail_type-'0' > MAX_SAMPLAN_DETAIL_TYPE ))
	 {
			MakeResponseMsg( return_data, (short) PI_INV_DETAIL_TYPE,
								"Invalid detail type: PI_DISPATCH_EXT_CALL" );
			return FALSE;
	 }

	 if (( (int)cur_call->msg_nbr-'0' < MIN_SAMPLAN_CMSGS ) ||
			 ( (int)cur_call->msg_nbr-'0' > MAX_SAMPLAN_CMSGS ))
	 {
			MakeResponseMsg( return_data, (short) PI_INV_NBR_CALL_MSGS,
								"Invalid num call msgs type: PI_DISPATCH_EXT_CALL" );
			return FALSE;
	 }

	 return 1;
 }


 /* ----------------------------------------------------------------------------
 /* FUNCTION: ValidFtjCallRec
 /*
 /* DESCRIPTION: Determine if call_msg record fields
 /*                 are valid data for TaxiPak
 /*
 /* PARAMETER: struct *call_rec
 /*
 /* RETURN-DATA:  if return-value = 1: fields OK,
 /*
 /*               if return-value = 0: invalid data.
 /*
 ------------------------------------------------------------------------------ */

 int ValidFtjCallRec( struct cisam_cmsg *call_msg, return_type *return_data )
 {

#ifdef NDEBUG
	printf("ValidFtjCallRec values:\n");
	printf("    call_msg->device=%d, (%d,%d)\n",call_msg->device,MIN_SAMPLAN_DEVICE,MAX_SAMPLAN_DEVICE);
	printf("    call_msg->msg_nbr=%d, (%d,%d)\n",call_msg->msg_nbr,0,MAX_MULTI_CALLS);
	printf("\n\n");
#endif

	if (( call_msg->device-'0' < MIN_SAMPLAN_DEVICE ) ||
			 ( call_msg->device-'0' > MAX_SAMPLAN_DEVICE ))
	{
			MakeResponseMsg( return_data, (short) PI_INV_CMSG_DEVICE,
							"Invalid call_msg device: PI_DISPATCH_EXT_CALL" );
			return FALSE;
	}

	if (( call_msg->msg_nbr-'0' < 0  ) ||
			( call_msg->msg_nbr-'0' > MAX_MULTI_CALLS ))
	{
			MakeResponseMsg( return_data, (short) PI_INV_CMSG_MULTI_NUM,
						 "Invalid multi_num: PI_DISPATCH_EXT_CALL" );
			return FALSE;
	}

	return (1);
 }


/* ----------------------------------------------------------------------------
/* FUNCTION: PI_CANCEL_CALL
/*
/* DESCRIPTION: Cancel a dispatched call 
/*
/* ORDER-DATA: Call_Number
/* 
/* RETURN-DATA:  if return-value = 1: return_data.data = PI_OK, 
/*                                                       Call_Number,
/*                                                       Call_Status
/*                                    OR
/*                                    return_data.data = PI_FAIL,
/*                                                       Call_Number
/*	                              return_data.len  = length of data
/*
/*               if return-value = 0: return_data is not defined (close PI).
/*
------------------------------------------------------------------------------ */

int pi_cancel_call( struct order_struc *order_data, struct return_struc *return_data ) 
  {
    short pi_fail_ok,		/* Return-value (PI_OK/PI_FAIL).	*/
          counter;		/* Common loop counter.			*/
    int return_code = 1;	/* Internal func. return-code.(OK=1)	*/
		struct cisam_cl call, *call_ptr;        /* The call-structure similar to the one used in TaxiPak.     */
		struct call cur_call;
    int cur_call_number;	/* Number of call to be cancelled.	*/
    int call_nbr;		/* Temporary call#			*/
    short call_status = 7;	/* Status of the current call.		*/
    char *position;		/* Current position in data-block. 	*/
    char cncl_act;
    int i;
    struct excpt_rec except_fifo;	/* for writing except resolution record to FIFO */
    char loop_count;		/* # of requetss to be written to FIFO */
    int	call_save;		/* Stores grouped with call for next search */
    int	indx;			/* Index for calls in memory */

    /**********************/
    /* FORMAT-check part. */
    /**********************/

    /* Checking block size... */

    if ( order_data->data_len != PI_INT_TYPE )
      {
        return_data->data_len = 0;
        position = return_data->data;

        ERROR( "Invalid block length" );	/* Log the error... */

        pi_fail_ok = PI_INVALID_LEN;
        cur_call_number = 0;
        return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
        return( return_code ); 
      }

    position = order_data->data;
    return_code = get_from_data_blk( PI_INT_TYPE, &position, (char *) &cur_call_number, &order_data->data_len );
    if ( return_code < 1 )
      {
        return_data->data_len = 0;
        position = return_data->data;

        ERROR( "Invalid parameter (too short)" );	/* Log the error... */

        pi_fail_ok = PI_INVALID_VALUE;
        cur_call_number = 0;
        return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
        return( return_code ); 
      }

    /***********************************************/
    /* Execute TaxiPak-handling of PI_CANCEL_CALL. */
    /***********************************************/


    return_data->data_len = 0;
    position = return_data->data;
    pi_fail_ok = PI_OK;
    call_nbr = cur_call_number;

		sprintf( szLogMsg, "Canceling call (%d)\n", call_nbr );
		MakeLogEntry( szLogMsg );

    if (call_nbr != 0)
      /* Check if call has already been cancelled. 						*/
      /* Note that a cancelled call can be revived, so look at the last entry in call history 	*/
      if (get_last_exception_type_from_callh(call_nbr) == CALL_CANCEL)
        {
          ERROR("Call allready cancelled");
          pi_fail_ok = PI_NO_CALL;
        }
      else
        while(call_nbr != 0)
          {
            call_ptr = (struct cisam_cl *)req_buf;
            call_ptr->nbr = call_nbr;
            
            if((db(CALL_FILE_ID,READ_KEYS,&cl_key1,ISEQUAL+ISLOCK,0)) < 0)
							{
								printf( "Failed with isam error (%d)\n", iserrno );
                ERROR( "Call does not exist" );		/* Log the error... */
                pi_fail_ok = PI_NO_CALL;
                break;
              }

            call_save = call_ptr->grouped_with;
            
            strcpy(call_ptr->status,catgets(WRITER_catd, 1, 40, "AVBEST  " ) );
            call_status = call_status_conv(catgets(WRITER_catd, 1, 40, "AVBEST  "), 6); 

            call_ptr->pri_status = (short)64;
            call_ptr->grouped_with = 0L;
            
            if ((db(CALL_FILE_ID, UPDATE_REC, &cl_key1, 0, 0)) < 0)		/* R2 */
							{
								printf( "Failed with isam error (%d)\n", iserrno );
                ERROR("Error updating <call record>");
              }

            if ((db(CALL_FILE_ID,RELEASE,&cl_key1,0,0)) < 0)
              {
								printf( "Failed with isam error (%d)\n", iserrno );
								ERROR("Error releasing <call record>");
              }

	    if ( glEnhSwitch & ENH0008 )
	      {
		DeleteTimeCall( call_ptr->nbr );
	      }
	    
            add_mult_h(call_nbr, call_ptr->fleet );

            /* Build an exception record to send to dispatch, so the call can be assigned */
            except_fifo.u_prcs_id = pid;
            except_fifo.rec_type = EXCEPT_REQ;
            except_fifo.except_ptr = '\0';
            except_fifo.except_nbr = 0;
            except_fifo.resolve_code = R_CANCEL;
            except_fifo.call_nbr = call_nbr;
            except_fifo.grouped_with = call_save;
            except_fifo.veh_nbr = 0;
            except_fifo.zone_nbr = 0;
            except_fifo.old_zone_nbr = 0;
            except_fifo.old_due_time = 0;
            
            if (write(fifo_fd,&except_fifo,sizeof(struct excpt_rec)) == -1)	/* Write FIFO to dispatch */
              {
                ERROR("Error writing to FIFO.");
              }

            call_nbr = call_save;
            if ( call_nbr == cur_call_number )		/* Don't do first call again */
              call_nbr = 0L;
          }

    /******************************/
    /* Build return-data-block.   */
    /******************************/

    return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
    if ( return_code < 1 )
      {
        return_data->data_len = 0;
        position = return_data->data;

        ERROR( "Invalid block length (too long)" );	/* Log the error... */

        pi_fail_ok = PI_INVALID_VALUE;
        return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
        return( return_code ); 
      }

    if ( pi_fail_ok == PI_OK )
      {
        return_code = ins_to_data_blk( PI_INT_TYPE, &position, (char *) &cur_call_number, &return_data->data_len );
        if ( return_code < 1 )
          {
            return_data->data_len = 0;
            position = return_data->data;
    
            ERROR( "Invalid block length (too long)" );	/* Log the error... */
    
            pi_fail_ok = PI_INVALID_VALUE;
            return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
            return( return_code ); 
          }

        return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &call_status, &return_data->data_len );
        if ( return_code < 1 )
          {
            return_data->data_len = 0;
            position = return_data->data;
    
            ERROR( "Invalid block length (too long)" );	/* Log the error... */
    
            pi_fail_ok = PI_INVALID_VALUE;
            return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
            return( return_code ); 
          }
      }

    return( return_code ); 
  } /* end PI_DISPATCH_CALL */

/* ----------------------------------------------------------------------------
/* FUNCTION: PI_GET_CALL
/*
/* DESCRIPTION: Get current data for a call
/*
/* ORDER-DATA: Call_Number
/* 
/* RETURN-DATA:  if return-value = 1: return_data.data = PI_OK, 
/*                                                       CALL-structure
/*                                    OR
/*                                    return_data.data = PI_FAIL,
/*                                                       Call_Number
/*	                              return_data.len  = length of data
/*
/*               if return-value = 0: return_data is not defined (close PI).
/* 
------------------------------------------------------------------------------ */

int pi_get_call( struct order_struc *order_data, struct return_struc *return_data ) 
  {
    short pi_fail_ok,		/* Return-value (PI_OK/PI_FAIL).	*/
          counter,		/* Common loop counter.			*/
          i;			/* Common loop counter.			*/
    int return_code = 1;	/* Internal func. return-code.(OK=1)	*/
		struct cisam_cl call, *call_ptr;        /* The call-structure similar to the one used in TaxiPak.     */
		struct call cur_call;
    int cur_call_number;	/* Number of call to get.		*/
    char *position;		/* Current position in data-block. 	*/
    char TraceBuf[512];

    /**********************/
    /* FORMAT-check part. */
    /**********************/

    bzero( &cur_call, sizeof(struct call) );
    
    /* Checking block size... */

    if ( order_data->data_len != PI_INT_TYPE )
      {
        return_data->data_len = 0;
        position = return_data->data;

        ERROR( "Invalid block length" );	/* Log the error... */

        pi_fail_ok = PI_INVALID_LEN;
        return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
        return( return_code ); 
      }

    position = order_data->data;
    return_code = get_from_data_blk( PI_INT_TYPE, &position, (char *) &cur_call_number, &order_data->data_len );
    if ( return_code < 1 )
      {
        return_data->data_len = 0;
        position = return_data->data;

        ERROR( "Invalid parameter (too short)" );	/* Log the error... */

        pi_fail_ok = PI_INVALID_VALUE;
        return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
        return( return_code ); 
      }

    /********************************************/
    /* Execute TaxiPak-handling of PI_GET_CALL. */
    /********************************************/

    call_ptr = (struct cisam_cl *)req_buf;
    call_ptr->nbr = cur_call_number;

    sprintf( TraceBuf, "EXTP --> PI PI_GET_CALL call_nbr %d\n", cur_call_number );
    MakeLogEntry( TraceBuf );
    
    if((db(CALL_FILE_ID,READ_KEYS,&cl_key1,ISEQUAL,0)) < 0)
      {
        return_data->data_len = 0;
        position = return_data->data;

        ERROR( "Call does not exist" );		/* Log the error... */

        pi_fail_ok = PI_NO_CALL;
        return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
        return( return_code ); 
      }

    pi_fail_ok = PI_OK;

    cur_call.fleet = call_ptr->fleet;
    cur_call.call_number = call_ptr->nbr;
    cur_call.priority = call_ptr->pri_status;
    cur_call.call_group = call_ptr->grouped_with;
    if ( call_ptr->pos_in_group == ' ' )
      cur_call.call_position = 0;
    else
      cur_call.call_position = call_ptr->pos_in_group - '0';
    cur_call.number_of_calls = call_ptr->nbr_in_group;
    if ( call_ptr->nbr_in_group == ' ' )
      cur_call.number_of_calls = 0;
    else
      cur_call.number_of_calls = call_ptr->nbr_in_group - '0';
    cur_call.number_of_vehicles = call_ptr->nbr_of_vehs - '0';
    strncpy( cur_call.call_type, call_ptr->extended_type, 5 );
    strcpy( cur_call.from_addr_street, call_ptr->pckup_str_name );
    cur_call.from_addr_number = call_ptr->pckup_str_nbr;
    cur_call.from_addr_suffix = call_ptr->pckup_str_suf;
    strcpy( cur_call.from_addr_apart, call_ptr->pckup_apt_room );
    strcpy( cur_call.from_addr_city, call_ptr->pckup_city );
    cur_call.from_addr_zone = call_ptr->pckup_zone;
    strcpy( cur_call.from_addr_cmnt, call_ptr->pckup_adr_cmnt );
    strcpy( cur_call.passenger, call_ptr->passenger_name );
    strncpy( cur_call.phone, call_ptr->phone_number, 10 );
    strcpy( cur_call.to_addr_street, call_ptr->dest_str_name );
    cur_call.to_addr_number = call_ptr->dest_str_nbr;
    cur_call.to_addr_suffix = call_ptr->dest_str_suf;
    strcpy( cur_call.to_addr_apart, call_ptr->dest_apt_room );
    strcpy( cur_call.to_addr_city, call_ptr->dest_city );
    cur_call.to_addr_zone = call_ptr->dest_zone;
    strcpy( cur_call.to_addr_cmnt, call_ptr->dest_adr_cmnt );
    cur_call.car_number = call_ptr->veh_nbr;
    strncpy( cur_call.car_attrib, call_ptr->veh_attr, 8 );
    sprintf( cur_call.driver_id, "%5.5d", call_ptr->drv_id );
    strncpy( cur_call.driver_attrib, call_ptr->drv_attr, 8 );
    cur_call.creator = call_ptr->entered_by;
    TP_to_PI_date( call_ptr->date_in, cur_call.creation_date );
    TP_to_PI_time( call_ptr->time_in, cur_call.creation_time );
    TP_to_PI_date( call_ptr->due_date, cur_call.due_date );
    TP_to_PI_time( call_ptr->due_time, cur_call.due_time );
    TP_to_PI_time( call_ptr->pckup_time, cur_call.pickup_time);
    TP_to_PI_time( call_ptr->close_time, cur_call.close_time );
    strcpy( cur_call.call_comment, call_ptr->general_cmnt );
    cur_call.call_status = call_status_conv( call_ptr->status, strlen( call_ptr->status ));


    /******************************/
    /* Build return-data-block.   */
    /******************************/

    return_data->data_len = 0;
    position = return_data->data;

    return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
    if ( return_code < 1 )
      {
        return_data->data_len = 0;
        position = return_data->data;

        ERROR( "Invalid block length (too long)" );	/* Log the error... */

        pi_fail_ok = PI_INVALID_VALUE;
        return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
        return( return_code ); 
      }

    if ( pi_fail_ok == PI_OK )
      {
        return_code = ins_to_data_blk( sizeof( struct call ), &position, (char *) &cur_call, &return_data->data_len ); 
        if ( return_code < 1 )
          {
            return_data->data_len = 0;
            position = return_data->data;
    
            ERROR( "Invalid block length (too long)" );	/* Log the error... */
    
            pi_fail_ok = PI_INVALID_VALUE;
            return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
            return( return_code ); 
          }
      }

    return( return_code ); 
  } /* end PI_GET_CALL */



pi_get_call_message()
{
}

/* ----------------------------------------------------------------------------
/* FUNCTION: PI_GET_CAR
/*
/* DESCRIPTION: Get car connected to a call
/*
/* ORDER-DATA: Call_Number
/* 
/* RETURN-DATA:  if return-value = 1: return_data.data = PI_OK, 
/*                                                       Call_Number,
/*                                                       Car_Number
/*                                    OR
/*                                    return_data.data = PI_FAIL,
/*                                                       Call_Number
/*	                              return_data.len  = length of data
/*
/*               if return-value = 0: return_data is not defined (close PI).
/*
------------------------------------------------------------------------------ */

int pi_get_car( struct order_struc *order_data, struct return_struc *return_data )
  {
    short pi_fail_ok,		/* Return-value (PI_OK/PI_FAIL).	*/
          counter;		/* Common loop counter.			*/
    int return_code = 1;	/* Internal func. return-code.(OK=1)	*/
		struct cisam_cl call, *call_ptr;        /* The call-structure similar to the one used in TaxiPak.     */
		struct call cur_call;
    int cur_call_number;	/* Number of call to be cancelled.	*/
    char *position;		/* Current position in data-block. 	*/

    /**********************/
    /* FORMAT-check part. */
    /**********************/

    /* Checking block size... */

    if ( order_data->data_len != PI_INT_TYPE )
      {
        return_data->data_len = 0;
        position = return_data->data;

        ERROR( "Invalid block length" );	/* Log the error... */

        cur_call_number = 0;
        pi_fail_ok = PI_INVALID_LEN;
        return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
        return( return_code ); 
      }

    position = order_data->data;
    return_code = get_from_data_blk( PI_INT_TYPE, &position, (char *) &cur_call_number, &order_data->data_len );
    if ( return_code < 1 )
      {
        return_data->data_len = 0;
        position = return_data->data;

        ERROR( "Invalid parameter (too short)" );	/* Log the error... */

        cur_call_number = 0;
        pi_fail_ok = PI_INVALID_VALUE;
        return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
        return( return_code ); 
      }

    /******************************************/
    /* Execute TaxiPak-handling of PI_GET_CAR */
    /******************************************/

    call_ptr = (struct cisam_cl *)req_buf;
    call_ptr->nbr = cur_call_number;
    
    if((db(CALL_FILE_ID,READ_KEYS,&cl_key1,ISEQUAL,0)) < 0)
      {
        return_data->data_len = 0;
        position = return_data->data;


        ERROR( "Call does not exist" );		/* Log the error... */

        pi_fail_ok = PI_NO_CALL;
        return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
        return( return_code ); 
      }

    pi_fail_ok = PI_OK;

    cur_call.call_number = call_ptr->nbr;
    cur_call.car_number = call_ptr->veh_nbr;


    /******************************/
    /* Build return-data-block.   */
    /******************************/

    return_data->data_len = 0;
    position = return_data->data;

    return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
    if ( return_code < 1 )
      {
        return_data->data_len = 0;
        position = return_data->data;

        ERROR( "Invalid block length (too long)" );	/* Log the error... */

        pi_fail_ok = PI_INVALID_VALUE;
        return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
        return( return_code ); 
      }

    if ( pi_fail_ok == PI_OK )
      {
        return_code = ins_to_data_blk( PI_INT_TYPE, &position, (char *) &cur_call.call_number, &return_data->data_len );
        if ( return_code < 1 )
          {
            return_data->data_len = 0;
            position = return_data->data;
    
            ERROR( "Invalid block length (too long)" );	/* Log the error... */
    
            pi_fail_ok = PI_INVALID_VALUE;
            return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
            return( return_code ); 
          }

        return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &cur_call.car_number, &return_data->data_len ); 
        if ( return_code < 1 )
          {
            return_data->data_len = 0;
            position = return_data->data;
    
            ERROR( "Invalid block length (too long)" );	/* Log the error... */
    
            pi_fail_ok = PI_INVALID_VALUE;
            return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
            return( return_code ); 
          }
      }

    return( return_code ); 
  } /* end PI_GET_CAR */

int MakeExceptRecord( PI_EXCEPT_REC *spExcptRepStruct, struct excepts *spSharedMemExcpt )
{
	int   return_code;

	spExcptRepStruct->exception_number = spSharedMemExcpt->nbr;
	if ( spSharedMemExcpt->fl_ptr != NULL )
		spExcptRepStruct->fleet = spSharedMemExcpt->fl_ptr->fleet_id;
	else
		spExcptRepStruct->fleet = ' ';

	spExcptRepStruct->exception_type = spSharedMemExcpt->type;
	spExcptRepStruct->approval = ' ';
	spExcptRepStruct->zone_number = spSharedMemExcpt->zone_nbr;
	spExcptRepStruct->call_number = spSharedMemExcpt->call_nbr;
	spExcptRepStruct->car_number = spSharedMemExcpt->veh_nbr;
	spExcptRepStruct->message_number = spSharedMemExcpt->info;
	spExcptRepStruct->outstanding = YES;
	return_code = strftime( spExcptRepStruct->creation_date, 7, "%y%m%d", localtime(&spSharedMemExcpt->time_occur));
	return_code = strftime( spExcptRepStruct->creation_time, 5, "%H%M", localtime(&spSharedMemExcpt->time_occur));

	return return_code;

}


	/* ----------------------------------------------------------------------------
	/* FUNCTION: PI_GENERATE_EXCEPTION
	/*
	/* DESCRIPTION: Generate exceptions externally
	/*
	/* ORDER-DATA: Exception_Type, Message_Number
	/*
	/* RETURN-DATA:  if return-value = 1: return_data.data = PI_OK,
	/*                                                       Number_Of_Exceptions,
	/*                                                       {EXCEPTION}-structure(s)
	/*                                    OR
	/*                                    return_data.data = PI_FAIL,
	/*                                                       Exception_Type
	/*                                return_data.len  = length of data
	/*
	/*               if return-value = 0: return_data is not defined (close PI).
	/*
	------------------------------------------------------------------------------ */

int pi_generate_exception( struct order_struc *order_data, struct return_struc *return_data )
{

			GenExcept           *spGenExcept;

			/**********************/
			/* FORMAT-check part. */
			/**********************/

			/* Checking Message length from client */

			if ( order_data->data_len != sizeof(GenExcept) )
				return( MakeResponseMsg( return_data, (short) PI_INVALID_LEN,
																	"Invalid PI_GENERATE_EXCEPTION msg size" ) );


			/**************************************/
			/* Check exception-type-validness... */
			/*************************************/

			spGenExcept = ( GenExcept *) order_data->data;

			if ( spGenExcept->nGroup != SAMPLAN_GRP )
				return( MakeResponseMsg( return_data, (short) PI_INV_EXCP_GROUP,
																				"Invalid exception group" ));

			if ( spGenExcept->nNumber < SP_EXCPT_01 ||
					 spGenExcept->nNumber > SP_EXCPT_15 )
				return( MakeResponseMsg( return_data, (short) PI_INV_EXCP_NBR,
																	"Invalid exception number" ));


			/*************************************************/
			/* Generate exception through user-pipe          */
			/*************************************************/

			MakePiExcept( pid, spGenExcept->nNumber );


			/******************************/
			/* Build return-data-block.   */
			/******************************/

			return( MakeResponseMsg( return_data, (short) PI_OK,"" ));


		} /* end PI_GENERATE_EXCEPTION */



/* ----------------------------------------------------------------------------
/* FUNCTION: PI_GET_EXCEPTIONS
/*
/* DESCRIPTION: Get exceptions of a certain type right now
/*
/* ORDER-DATA: Exception_Type, Message_Number
/* 
/* RETURN-DATA:  if return-value = 1: return_data.data = PI_OK, 
/*                                                       Number_Of_Exceptions,
/*                                                       {EXCEPTION}-structure(s)
/*                                    OR
/*                                    return_data.data = PI_FAIL,
/*                                                       Exception_Type
/*	                              return_data.len  = length of data
/*
/*               if return-value = 0: return_data is not defined (close PI).
/*
------------------------------------------------------------------------------ */

int pi_get_exceptions( struct order_struc *order_data, struct return_struc *return_data )
  {
    short pi_fail_ok,		/* Return-value (PI_OK/PI_FAIL).	*/
          counter, exc_of_this_type,		/* Common loop counter.			*/
          i;			/* Common loop counter.			*/
    int return_code = 1;	/* Internal func. return-code.(OK=1)	*/
    int cur_excep_type;		/* Type of exception to get.		*/
    int cur_excep_mess;	/* Type of message to get.		*/
    PI_EXCEPT_REC picked_excep[ NUM_OF_EXCEPTIONS + 1 ];	/* Table of picked exceptions. */
    int num_of_pck_exc = 0;	/* number_of_exceptions picked.		*/
    char *position;		/* Current position in data-block. 	*/

    /******************************************/
    /* Initialized table of picked excetions. */
    /******************************************/

    for ( counter = 0; counter < NUM_OF_EXCEPTIONS; counter++ )
      memset((char *)&picked_excep[ counter ], '\0', sizeof( PI_EXCEPT_REC ));

    /**********************/
    /* FORMAT-check part. */
    /**********************/

    /* Checking block size... */

    if ( order_data->data_len != ( PI_INT_TYPE + PI_LONG_TYPE ))
      {
        return_data->data_len = 0;
        position = return_data->data;

        sprintf( szLogMsg, "(%s:%d) Invalid block length", __FILE__, __LINE__ );
        MakeLogEntry( szLogMsg );

        cur_excep_type = 0;
        pi_fail_ok = PI_INVALID_LEN;
        return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
        return( return_code ); 
      }

    position = order_data->data;

    return_code = get_from_data_blk( PI_INT_TYPE, &position, (char *) &cur_excep_type, &order_data->data_len );
    if ( return_code < 1 )
      {
        return_data->data_len = 0;
        position = return_data->data;

        sprintf( szLogMsg, "(%s:%d) Invalid parameter (too short)", __FILE__, __LINE__ );
        MakeLogEntry( szLogMsg );

        cur_excep_type = 0;
        pi_fail_ok = PI_INVALID_VALUE;
        return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
        return( return_code ); 
      }

    /*************************************/
    /* Check exception-type-validness... */
    /*************************************/

    sprintf( szLogMsg, "PI_GET_EXCEPTIONS checking for exception type %d", cur_excep_type );
    MakeLogEntry( szLogMsg );

    num_of_pck_exc = 0;
    if ( cur_excep_type == 0 ) // get all exceptions
      {
	for ( i = 0; i < NUM_OF_EXCEP_TYPE; i++ )
	  {
	    exc_of_this_type = get_excepts(except_ptr, i, 0, 0 );
	    for (counter=0; counter < exc_of_this_type; counter++ )
	      MakeExceptRecord( &picked_excep[num_of_pck_exc+counter], except_ptr[ counter ] );
	    num_of_pck_exc += exc_of_this_type;
	  }

	/******************************/
	/* Build return-data-block.   */
	/******************************/
	
	return_data->data_len = 0;
	position = return_data->data;
	pi_fail_ok = PI_OK;
	
	return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );


	if ( pi_fail_ok == PI_OK )
	  {
	    return_code = ins_to_data_blk( PI_INT_TYPE, &position, (char *) &num_of_pck_exc, &return_data->data_len );
	    for ( counter = 0; counter < num_of_pck_exc; counter++ )
	      {
		return_code = ins_to_data_blk( sizeof(PI_EXCEPT_REC), &position, (char *) &picked_excep[ counter ], &return_data->data_len );
	      }
	  }
	return(return_code);
      }

    i = 0;
  
    while ( i < NUM_OF_EXCEP_TYPE )
      if ( cur_excep_type == on_off_excep[ i++ ].excep_type )
        break;

    if ( cur_excep_type != on_off_excep[ --i ].excep_type )
      {
        return_data->data_len = 0;
        position = return_data->data;

        sprintf( szLogMsg, "(%s:%d) Exception-type does not exist", __FILE__, __LINE__ );
        MakeLogEntry( szLogMsg );
        
        pi_fail_ok = PI_INV_EXCEP_TYPE;
        return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
        return( return_code ); 
      }
    else
      pi_fail_ok = PI_OK;

    return_code = get_from_data_blk( PI_LONG_TYPE, &position, (char *) &cur_excep_mess, &order_data->data_len );
    if ( return_code < 1 )
      {
        return_data->data_len = 0;
        position = return_data->data;

        ERROR( "Invalid parameter (too short)" );	/* Log the error... */

        pi_fail_ok = PI_INVALID_VALUE;
        return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
        return( return_code ); 
      }

    /****************************************/
    /* Check exception-message-validness... */
    /****************************************/

    if (( cur_excep_type == MESSAGE ) || ( cur_excep_type == DRV_MSG ))
      {
        if (( cur_excep_mess < 0 ) || ( cur_excep_mess > 99 ))
          {
            return_data->data_len = 0;
            position = return_data->data;
  
            ERROR( "Exception-message does not exist" );		/* Log the error... */
  
            pi_fail_ok = PI_INV_EXCEP_MESS;
            return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
            return( return_code ); 
          }
      }
    else
      cur_excep_mess = 0;

    /*************************************************/
    /* Execute TaxiPak-handling of PI_GET_EXCEPTIONS */
    /*************************************************/

		sprintf( szLogMsg, "Making lookup for excp type(%d)\n",cur_excep_type );
		MakeLogEntry( szLogMsg );

    num_of_pck_exc = get_excepts(except_ptr, cur_excep_type, cur_excep_mess, 0 );
    if ( num_of_pck_exc > NUM_OF_EXCEPTIONS )
      num_of_pck_exc = NUM_OF_EXCEPTIONS;

		sprintf( szLogMsg, "Found (%d) of this type\n",num_of_pck_exc );
		MakeLogEntry( szLogMsg );

    for (counter = 0; counter < num_of_pck_exc; counter++ )
				MakeExceptRecord( &picked_excep[counter], except_ptr[ counter ] );


		MakeLogEntry( "Sending Exception structs to client\n" );

    /******************************/
    /* Build return-data-block.   */
    /******************************/

    return_data->data_len = 0;
    position = return_data->data;
    pi_fail_ok = PI_OK;

    return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
    if ( return_code < 1 )
      {
        return_data->data_len = 0;
        position = return_data->data;

        ERROR( "Invalid block length (too long)" );	/* Log the error... */

        pi_fail_ok = PI_INVALID_VALUE;
        return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
        return( return_code ); 
      }

    if ( pi_fail_ok == PI_OK )
      {
        return_code = ins_to_data_blk( PI_INT_TYPE, &position, (char *) &num_of_pck_exc, &return_data->data_len );
        if ( return_code < 1 )
          {
            return_data->data_len = 0;
            position = return_data->data;
    
            ERROR( "Invalid block length (too long)" );	/* Log the error... */
    
            pi_fail_ok = PI_INVALID_VALUE;
            return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
            return( return_code ); 
          }

        for ( counter = 0; counter < num_of_pck_exc; counter++ )
          {
            return_code = 
              ins_to_data_blk( sizeof(PI_EXCEPT_REC), &position, (char *) &picked_excep[ counter ], &return_data->data_len ); 
            if ( return_code < 1 )
              {
                return_data->data_len = 0;
                position = return_data->data;
        
                ERROR( "Invalid block length (too long)" );	/* Log the error... */
        
                pi_fail_ok = PI_INVALID_VALUE;
                return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
                return( return_code ); 
              }
          }
      }
    else
      {
        return_code = ins_to_data_blk( PI_INT_TYPE, &position, (char *) &cur_excep_type, &return_data->data_len );
        if ( return_code < 1 )
          {
            return_data->data_len = 0;
            position = return_data->data;
    
            ERROR( "Invalid block length (too long)" );	/* Log the error... */
    
            pi_fail_ok = PI_INVALID_VALUE;
            return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
            return( return_code ); 
          }
        return( return_code );
      }

    return( return_code ); 
  } /* end PI_GET_EXCEPTIONS */

/* ----------------------------------------------------------------------------
/* FUNCTION: PI_REPORT_EXCEPTIONS
/*
/* DESCRIPTION: Get exceptions of a certain type when they occurs
/*
/* ORDER-DATA: Exception_Type, Message_Number, On_Off_Switch
/* 
/* RETURN-DATA:  if return-value = 1: return_data.data = PI_OK, 
/*                                                       0,
/*                                                       Exception_Type
/*                                    OR
/*                                    return_data.data = PI_FAIL,
/*                                                       Exception_Type
/*                                    OR
/*                                    return_data.data = PI_OK,
/*                                                       1,
/*                                                       EXCEPTION-structure
/*	                              return_data.len  = length of data
/*
/*               if return-value = 0: return_data is not defined (close PI).
/*                                    OR
/*               if return-value = 0: no exception were found (BROADCAST).
/*
------------------------------------------------------------------------------ */

int pi_report_exceptions( struct order_struc *order_data, struct return_struc *return_data, char broadcast )
  {
    short pi_fail_ok,		/* Return-value (PI_OK/PI_FAIL).	*/
          counter,		/* Common loop counter.			*/
          i,			/* Common loop counter.			*/
          j;			/* Common loop counter.			*/
    int return_code = 1;	/* Internal func. return-code.(OK=1)	*/
    int cur_excep_type;		/* Type of the actual exception.	*/
    int cur_excep_idx;		/* Current index in the exception-table */
    int exceptions_got;		/* Number of exceptions that matches.	*/
    int cur_excep_mess;	/* Evt. message connected to it.	*/
    int cur_excep_num;		/* Number of the excep. to report.	*/
    PI_EXCEPT_REC excep;	/* Exception to report.			*/
    char on_off_switch;		/* The actual switch (1=ON,0=OFF)	*/
    char *position;		/* Current position in data-block. 	*/

    /************************************/
    /* Report exception if BROADCAST... */
    /************************************/


    // problems with this function....just bag it!
    return(0);
    
    if ( broadcast )
      {
        /************************************************************************/
        /*                                             				*/
        /*          Searching criteria for exception-reporting: 		*/
        /*                                             				*/
        /* 1) Exception-types that are ON.             				*/
        /*                                             				*/
        /* 2) if exception-type = message => Message-number that are ON. 	*/
        /*                                             				*/
        /* 3) Exception from exception-list that fullfills criteria 1 & 2. 	*/
        /*                                             				*/
        /************************************************************************/

        cur_excep_num = 0;
        cur_excep_type = 0;
          
        while (( cur_excep_type < NUM_OF_EXCEP_TYPE ) && ( cur_excep_num == 0 ))
          {
            if ( on_off_excep[ cur_excep_type ].on_off == SWITCH_ON )
              {
                if ( on_off_excep[ cur_excep_type ].excep_type == MESSAGE ) 
                  {
                    cur_excep_mess = 0;
                    while (( cur_excep_mess <= NUM_OF_EXCEP_MESS ) && ( cur_excep_num == 0 ))
                      {
                        if ( mess_on_off[ cur_excep_mess ].on_off == SWITCH_ON )
                          {
                              exceptions_got = 
                                get_excepts(except_ptr, on_off_excep[cur_excep_type].excep_type, cur_excep_mess, 0 );
                              if ( exceptions_got > 0 )
                                {
                                  counter = 0;
                                  while (( counter < exceptions_got ) && ( cur_excep_num == 0 )) 
                                    {
                                      if ( except_ptr[ counter ]->nbr > mess_on_off[cur_excep_mess].last_nbr )
                                        {
                                          cur_excep_num = except_ptr[ counter ]->nbr;
                                          mess_on_off[cur_excep_mess].last_nbr = cur_excep_num;
                                        }
                                       counter++;
                                     }
                                 }
                            } 
                        cur_excep_mess++; 
                      }
                  }
                else
                  if ( on_off_excep[ cur_excep_type ].excep_type == DRV_MSG ) 
                    {
                      cur_excep_mess = 0;
                      while (( cur_excep_mess <= NUM_OF_EXCEP_MESS ) && ( cur_excep_num == 0 ))
                        {
                          if ( drv_mess_on_off[ cur_excep_mess ].on_off == SWITCH_ON )
                            {
                              exceptions_got = 
                                get_excepts(except_ptr, on_off_excep[cur_excep_type].excep_type, cur_excep_mess, 0 );
                              if ( exceptions_got > 0 )
                                {
                                  counter = 0;
                                  while (( counter < exceptions_got ) && ( cur_excep_num == 0 )) 
                                    {
                                      if ( except_ptr[ counter ]->nbr > drv_mess_on_off[cur_excep_mess].last_nbr )
                                        {
                                          cur_excep_num = except_ptr[ counter ]->nbr;
                                          drv_mess_on_off[cur_excep_mess].last_nbr = cur_excep_num;
                                        }
                                       counter++;
                                     }
                                 }
                            } 
                          cur_excep_mess++; 
                        }
                    }
                else
                  {
                    exceptions_got = get_excepts(except_ptr, on_off_excep[cur_excep_type].excep_type, 0, 0 );
                    if ( exceptions_got > 0 )
                      {
                        counter = 0;
                        while (( counter < exceptions_got ) && ( cur_excep_num == 0 ))
                          {
                            if ( except_ptr[ counter ]->nbr > on_off_excep[cur_excep_type].last_nbr )
                              {
                                cur_excep_num = except_ptr[ counter ]->nbr;
                                on_off_excep[cur_excep_type].last_nbr = cur_excep_num;
                              }
                            counter++;
                          }
                      }
                  }
              }
            cur_excep_type++;
          }

        counter--;

        /****************************************************/
        /* Build return-data-block, if exception was found. */
        /****************************************************/
    
        if ( cur_excep_num == 0 )
          return( 0 );
        else
          {
            broadcast = 1;
            return_data->data_len = 0;
            position = return_data->data;
            pi_fail_ok = PI_OK;

            /**********************************/
            /* Build report-exception record. */        
            /**********************************/
            
            MakeExceptRecord( &excep, except_ptr[ counter ] );

            sprintf( szLogMsg, "(%s:%d) Broadcast PI_REPORT_EXCEPTION", __FILE__, __LINE__ );
            MakeLogEntry( szLogMsg );
                                
            return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
            if ( return_code < 1 )
              {
                return_data->data_len = 0;
                position = return_data->data;
        
                ERROR( "Invalid block length (too long)" );	/* Log the error... */
        
                pi_fail_ok = PI_INVALID_VALUE;
                return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
                return( return_code ); 
              }
    
            return_code = ins_to_data_blk( PI_CHAR_TYPE, &position, (char *) &broadcast, &return_data->data_len );
            if ( return_code < 1 )
              {
                return_data->data_len = 0;
                position = return_data->data;
        
                ERROR( "Invalid block length (too long)" );	/* Log the error... */
        
                pi_fail_ok = PI_INVALID_VALUE;
                return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
                return( return_code ); 
              }

            sprintf( szLogMsg, "(%s:%d) Exception details: Nbr %d\t Fleet %c\t Date %s \tTime %s\nType %d\tApproval %c\tTaxi %d\tCall Number %d\tMsg Nbr %d\n",
                     __FILE__, __LINE__, excep.exception_number, excep.fleet, excep.creation_date,
                     excep.creation_time, excep.exception_type, excep.approval, excep.car_number, excep.call_number, excep.message_number );
            MakeLogEntry( szLogMsg );
            
            return_code = 
              ins_to_data_blk( sizeof( PI_EXCEPT_REC ), &position, (char *) &excep, &return_data->data_len );

            if ( return_code < 1 )
              {
                return_data->data_len = 0;
                position = return_data->data;
        
                ERROR( "Invalid block length (too long)" );	/* Log the error... */
        
                pi_fail_ok = PI_INVALID_VALUE;
                return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
                return( return_code ); 
              }
            }
          return_code = 1;
        } /* end BROADCAST */ 
      else
        { /* Set/unset exception... */
          /**********************/
          /* FORMAT-check part. */
          /**********************/
      
          /* Checking block size... */
          sprintf(szLogMsg, "(%s:%d) Entering PI_REPORT_EXCEPTIONS set/unset", __FILE__, __LINE__);
          MakeLogEntry( szLogMsg );
          
          if ( order_data->data_len != ( PI_INT_TYPE + PI_INT_TYPE + PI_CHAR_TYPE ))
            {
              return_data->data_len = 0;
              position = return_data->data;

              sprintf( szLogMsg, "(%s:%d) Invalid block length", __FILE__, __LINE__ );
              MakeLogEntry( szLogMsg );
              
              cur_excep_type = 0;
              pi_fail_ok = PI_INVALID_LEN;
              return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
              return( return_code ); 
            }
      
          position = order_data->data;
      
          return_code = get_from_data_blk( PI_INT_TYPE, &position, (char *) &cur_excep_type, &order_data->data_len );
          if ( return_code < 1 )
            {
              return_data->data_len = 0;
              position = return_data->data;

              sprintf( szLogMsg, "(%s:%d) Invalid parameter (too short)", __FILE__, __LINE__ );
              MakeLogEntry( szLogMsg );
              
              cur_excep_type = 0;
              pi_fail_ok = PI_INVALID_VALUE;
              return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
              return( return_code ); 
            }
          else
            {
              sprintf( szLogMsg, "(%s:%d) Received exception type (%d)", __FILE__, __LINE__, cur_excep_type );
              MakeLogEntry( szLogMsg );
            }
      
          /*************************************/
          /* Check exception-type-validness... */
          /*************************************/
          
          i = 0;
        
          while ( i < NUM_OF_EXCEP_TYPE )
            if ( cur_excep_type == on_off_excep[ i++ ].excep_type )
              break;

          if ( cur_excep_type != on_off_excep[ --i ].excep_type )
            {
              return_data->data_len = 0;
              position = return_data->data;

              sprintf( szLogMsg, "(%s:%d) Exception type does not exist", __FILE__, __LINE__ );
              MakeLogEntry( szLogMsg );
      
              pi_fail_ok = PI_INV_EXCEP_TYPE;
              return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
              return( return_code ); 
            }
          else
            cur_excep_idx = i;
      
          return_code = get_from_data_blk( PI_INT_TYPE, &position, (char *) &cur_excep_mess, &order_data->data_len );
          if ( return_code < 1 )
            {
              return_data->data_len = 0;
              position = return_data->data;

              sprintf( szLogMsg, "(%s:%d) Invalid parameter (too short)", __FILE__, __LINE__ );
              MakeLogEntry( szLogMsg );
              
      
              pi_fail_ok = PI_INVALID_VALUE;
              return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
              return( return_code ); 
            }
          else
            {
              sprintf( szLogMsg, "(%s:%d) Received message from driver #%d", __FILE__, __LINE__, cur_excep_mess );
              MakeLogEntry( szLogMsg );
            }
      
          /****************************************/
          /* Check exception-message-validness... */
          /****************************************/
      
          if (( cur_excep_type == MESSAGE ) || ( cur_excep_type == DRV_MSG ))
            {
              if (( cur_excep_mess < 0 ) || ( cur_excep_mess > NUM_OF_EXCEP_MESS ))
                {
                  return_data->data_len = 0;
                  position = return_data->data;

                  sprintf( szLogMsg, "(%s:%d) Exception message does not exist", __FILE__, __LINE__ );
                  MakeLogEntry( szLogMsg );
        
                  pi_fail_ok = PI_INV_EXCEP_MESS;
                  return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
                  return( return_code ); 
                }
            }
          else
            cur_excep_mess = 0;
      
          return_code = get_from_data_blk( PI_CHAR_TYPE, &position, (char *) &on_off_switch, &order_data->data_len );
          if ( return_code < 1 )
            {
              return_data->data_len = 0;
              position = return_data->data;

              sprintf( szLogMsg, "(%s:%d) Invalid on/off parameter", __FILE__, __LINE__ );
              MakeLogEntry( szLogMsg );

              cur_excep_num = 0;
              pi_fail_ok = PI_INVALID_VALUE;
              return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
              return( return_code ); 
            }
      

          /***********************************************/
          /* Check valid on_off_switch: ON/OFF/BROADCAST */
          /***********************************************/
      
          if (( on_off_switch != SWITCH_ON ) && ( on_off_switch != SWITCH_OFF ))
            {
              return_data->data_len = 0;
              position = return_data->data;

              sprintf( szLogMsg, "(%s:%d) Invalid approval type switch", __FILE__, __LINE__ );
              MakeLogEntry( szLogMsg );
      
              pi_fail_ok = PI_INV_SWITCH;
              return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
              return( return_code ); 
            }

          if (( cur_excep_type != MESSAGE ) && ( cur_excep_type != DRV_MSG ))
            on_off_excep[ cur_excep_idx ].on_off = on_off_switch; 

          if ( cur_excep_type == MESSAGE )
            {
              mess_on_off[ cur_excep_mess ].on_off = on_off_switch;
              if ( on_off_switch == SWITCH_ON )
                {
                  on_off_excep[ cur_excep_idx ].on_off = on_off_switch; 
                } 
              else
                {
                  i = 1;
                
                  while ( i < NUM_OF_EXCEP_MESS)
                    if ( mess_on_off[ i++ ].on_off == SWITCH_ON )
                      break;
        
                  if (  mess_on_off[ --i ].on_off != YES )
                    on_off_excep[ cur_excep_idx ].on_off = SWITCH_OFF; 
                }
            }
          else
            if ( cur_excep_type == MESSAGE )
              {
                drv_mess_on_off[ cur_excep_mess ].on_off = on_off_switch;
                if ( on_off_switch ==SWITCH_ON )
                  {
                    on_off_excep[ cur_excep_idx ].on_off = on_off_switch; 
                  } 
                else
                  {
                    i = 1;
                  
                    while ( i < NUM_OF_EXCEP_MESS)
                      if ( drv_mess_on_off[ i++ ].on_off == SWITCH_ON )
                        break;
          
                    if (  drv_mess_on_off[ --i ].on_off != SWITCH_ON )
                      on_off_excep[ cur_excep_idx ].on_off = SWITCH_OFF; 
                  }
              }

          /****************************/
          /* Build return-data-block. */
          /****************************/

          broadcast = 0;
          return_data->data_len = 0;
          position = return_data->data;
          pi_fail_ok = PI_OK;
      
          return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
          if ( return_code < 1 )
            {
              return_data->data_len = 0;
              position = return_data->data;
      
              ERROR( "Invalid block length (too long)" );	/* Log the error... */
      
              pi_fail_ok = PI_INVALID_VALUE;
              return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
              return( return_code ); 
            }
  
          return_code = ins_to_data_blk( PI_CHAR_TYPE, &position, (char *) &broadcast, &return_data->data_len );
          if ( return_code < 1 )
            {
              return_data->data_len = 0;
              position = return_data->data;
      
              ERROR( "Invalid block length (too long)" );	/* Log the error... */
      
              pi_fail_ok = PI_INVALID_VALUE;
              return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
              return( return_code ); 
            }
  
          return_code = ins_to_data_blk( PI_INT_TYPE, &position, (char *) &cur_excep_type, &return_data->data_len );
          if ( return_code < 1 )
            {
              return_data->data_len = 0;
              position = return_data->data;
      
              ERROR( "Invalid block length (too long)" );	/* Log the error... */
      
              pi_fail_ok = PI_INVALID_VALUE;
              return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
              return( return_code ); 
            }
  
          return_code = 1;
        } /* end setting */

    return( return_code ); 

  } /* end PI_REPORT_EXCEPTIONS */



/* ----------------------------------------------------------------------------
/* FUNCTION: PI_ACCEPT_EXCEPTION
/*
/* DESCRIPTION: Approval or disapproval of an exception
/*
/* ORDER-DATA: Exception_Number + Approval.
/* 
/* RETURN-DATA:  if return-value = 1: return_data.data = PI_OK, 
/*                                                       Exception_Number
/*                                    OR
/*                                    return_data.data = PI_FAIL,
/*                                                       Exception_Number
/*	                              return_data.len  = length of data
/*
/*               if return-value = 0: return_data is not defined (close PI).
/* 
------------------------------------------------------------------------------ */

int pi_accept_exception( struct order_struc *order_data, struct return_struc *return_data )
  {
    short pi_fail_ok,		/* Return-value (PI_OK/PI_FAIL).	*/
          counter,		/* Common loop counter.			*/
          i;			/* Common loop counter.			*/
    int return_code = 1;	/* Internal func. return-code.(OK=1)	*/
    int update_call_history;	/* Set (1) if call-history is updated.  */
    int cur_excep_num;		/* Number of the actual exception.	*/
    char cur_approval;		/* The actual approval-character. (Y/N) */
    char *position;		/* Current position in data-block. 	*/

    /**********************/
    /* FORMAT-check part. */
    /**********************/

    /* Checking block size... */

    MakeLogEntry( "Entering PI_ACCEPT_EXCEPTION" );
    
    if ( order_data->data_len != ( PI_INT_TYPE + PI_CHAR_TYPE ))
      {
        return_data->data_len = 0;
        position = return_data->data;

        sprintf( szLogMsg, "(%s:%d) Invalid block length", __FILE__, __LINE__ );
        MakeLogEntry( szLogMsg );

        cur_excep_num = 0;
        pi_fail_ok = PI_INVALID_LEN;
        return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
        return( return_code ); 
      }

    position = order_data->data;

    return_code = get_from_data_blk( PI_INT_TYPE, &position, (char *) &cur_excep_num, &order_data->data_len ); 
    if ( return_code < 1 )       
      {
        return_data->data_len = 0;
        position = return_data->data;

        sprintf( szLogMsg, "(%s:%d) Invalid parameter (too short)", __FILE__, __LINE__ );
        MakeLogEntry( szLogMsg );        

        cur_excep_num = 0;
        pi_fail_ok = PI_INVALID_VALUE;
        return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
        return( return_code ); 
      }

    /********************************************/
    /* Check exception-number-type-validness... */
    /********************************************/
    
    return_code = get_from_data_blk( PI_CHAR_TYPE, &position, (char *) &cur_approval, &order_data->data_len );
    if ( return_code < 1 )
      {
        return_data->data_len = 0;
        position = return_data->data;

        sprintf( szLogMsg, "(%s:%d) Invalid parameter (too short)", __FILE__, __LINE__ );
        MakeLogEntry( szLogMsg );

        cur_excep_num = 0;
        pi_fail_ok = PI_INVALID_VALUE;
        return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
        return( return_code ); 
      }


    sprintf( szLogMsg, "Except nbr %d Approval %c", cur_excep_num, cur_approval );
    MakeLogEntry( szLogMsg );
    
    cur_approval = YES;
    if (( cur_approval != YES ) && ( cur_approval != NO ))
      {
        return_data->data_len = 0;
        position = return_data->data;

        sprintf( szLogMsg, "(%s:%d) Invalid approval-type", __FILE__, __LINE__ );
        MakeLogEntry( szLogMsg );

        pi_fail_ok = PI_INV_APPROVAL;
        return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
        return( return_code ); 
      }
   
    /***************************************************/
    /* Execute TaxiPak-handling of PI_ACCEPT_EXCEPTION */
    /***************************************************/

    if ( get_excepts(except_ptr, 0, 0, cur_excep_num ) != 1 )
      {
        return_data->data_len = 0;
        position = return_data->data;

        sprintf( szLogMsg, "(%s:%d) Exception does not exist in TaxiPak", __FILE__, __LINE__ );
        MakeLogEntry( szLogMsg );

        pi_fail_ok = PI_INV_EXCEPTION;
        return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
        return( return_code ); 
      }
    else
      {				/* Resolve exception... (approve/disapprove) */
        pi_fail_ok = PI_OK;
        if ( cur_approval == YES || cur_approval == 'Y' )	/* Approve exception... */
        /* check type of exception being resolved and process accordingly */
          switch(except_ptr[ 0 ]->type)
            {
              case ETA:
              case MESSAGE:
              case REQ_TO_TLK:
                if (( except_ptr[ 0 ]->call_nbr == 0 ) && ( except_ptr[ 0 ]->call_ptr == NULL ))
                  update_call_history = 0;
                else
                  update_call_history = 1;

                resolve_except(except_ptr,0,0,0,update_call_history,R_APPROVE,catgets(WRITER_catd, 1, 25, "APPROVED" ),except_ptr[ 0 ]->type); 
              break;
  
              case MISSING_RECEIPT_RETRY_LIMIT:
              case CALL_TIMEOUT:
              case LATE_METER:
              case SHORT_METER:
              case CALLBACK:
              case CALLOUT:
              case REJECT:
              case FLAG_REQ:
              case BID:
              case SUP_HOLD:
              case EXTRA_TAXIS:
              case CALL_NO_MSG:
              case HOL_TM_CALL:
              case NO_SHOW:
              case CALL_W_MSG:
              case MULT:
              case PERSONAL:
              case MANUAL:	/* manual exception, when cab's kdt is not working */
              			/* update call history record, and write request to FIFO; do not update call record */
                resolve_except(except_ptr,0,0,0,1,R_APPROVE,catgets(WRITER_catd, 1, 25, "APPROVED" ),except_ptr[ 0 ]->type ); 
              break;

							case SP_EXCPT_01:
							case SP_EXCPT_02:
							case SP_EXCPT_03:
							case SP_EXCPT_04:
							case SP_EXCPT_05:
							case SP_EXCPT_06:
							case SP_EXCPT_07:
							case SP_EXCPT_08:
							case SP_EXCPT_09:
							case SP_EXCPT_10:
							case SP_EXCPT_11:
							case SP_EXCPT_12:
							case SP_EXCPT_13:
							case SP_EXCPT_14:
							case SP_EXCPT_15:

              case SYS_ERR:	/* system errors; do not update call or callh rec */
              case EMERGENCY:
              case MFAULT:
              resolve_except(except_ptr,0,0,0,0,R_APPROVE,catgets(WRITER_catd, 1, 25, "APPROVED" ),except_ptr[ 0 ]->type ); 
              break;
  
              case CALL_CANCEL:
              case WAKE_UP:
                resolve_except(except_ptr,0,0,1,1,R_APPROVE,catgets(WRITER_catd, 1, 25, "APPROVED" ),except_ptr[ 0 ]->type ); 
              break;
  
              case UNZONED:
                ERROR("<APPROVE not allowed for unzoned exceptions>");
              break;
  
              case DRV_MSG:
              			/* update msg log file, do not update call file or call history file */
                resolve_except(except_ptr,0,0,0,0,R_APPROVE,catgets(WRITER_catd, 1, 25, "APPROVED" ),except_ptr[ 0 ]->type ); 
              break;
              
              default:
                ERROR("DEBUG MSG: this exception is not coded yet!!!");
            }
        else			/* Disapprove exception... check type of exception being resolved and process accordingly */
          switch(except_ptr[ 0 ]->type)
            {
              case ETA:
              case MESSAGE:
              case REQ_TO_TLK:
                if (( except_ptr[ 0 ]->call_nbr == 0 ) && ( except_ptr[ 0 ]->call_ptr == NULL ))
                  update_call_history = 0;
                else 
                  update_call_history = 1;
								MakeLogEntry( "Disapproving exception\n" );
                resolve_except(except_ptr,0,0,0,update_call_history, R_DISSAPROVE,catgets(WRITER_catd, 1, 26, "DENIED" ),except_ptr[ 0 ]->type ); 
              break;

              case MISSING_RECEIPT_RETRY_LIMIT:
              case CALL_TIMEOUT:
              case LATE_METER:
              case SHORT_METER:
              case CALLBACK:
              case CALLOUT:
              case REJECT:
              case FLAG_REQ:
              case BID:
              case SUP_HOLD:
              case WAKE_UP:
              case NO_SHOW:
              case PERSONAL:
              case MANUAL:	/* when cab's kdt is not working */
				/* update call history record, and write request to FIFO; do not update call record */
                resolve_except(except_ptr,0,0,0,1,R_DISSAPROVE,catgets(WRITER_catd, 1, 26, "DENIED" ),except_ptr[ 0 ]->type ); 
              break;

              case EXTRA_TAXIS:
              case CALL_NO_MSG:
              case CALL_CANCEL:
              case MULT:
              case HOL_TM_CALL:
              case CALL_W_MSG:
				/* mark call as cancelled, update call & callh recs */
                resolve_except(except_ptr,0,0,1,1,R_DISSAPROVE,catgets(WRITER_catd, 1, 26, "DENIED" ),except_ptr[ 0 ]->type ); 
              break;

              case SYS_ERR:	/* system errors; do not update call or callh rec */
              case EMERGENCY:
              case MFAULT:
                resolve_except(except_ptr,0,0,0,0,R_DISSAPROVE,catgets(WRITER_catd, 1, 26, "DENIED" ),except_ptr[ 0 ]->type ); 
              break;

              case UNZONED:
                ERROR("<DISAPPROVE not allowed for unzoned exceptions>");
              break;

              case DRV_MSG:
			/* update msg log file, do not update call file or call history file */
                resolve_except(except_ptr,0,0,0,0,R_DISSAPROVE,catgets(WRITER_catd, 1, 26, "DENIED" ), except_ptr[ 0 ]->type ); 
              break;

              default:
                ERROR("DEBUG MSG: this exception is not coded yet!!!");
           }
      }

    /******************************/
    /* Build return-data-block.   */
    /******************************/

    return_data->data_len = 0;
    position = return_data->data;
    pi_fail_ok = PI_OK;

    return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
    if ( return_code < 1 )
      {
        return_data->data_len = 0;
        position = return_data->data;

        ERROR( "Invalid block length (too long)" );	/* Log the error... */

        pi_fail_ok = PI_INVALID_VALUE;
        return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
        return( return_code ); 
      }

    return_code = ins_to_data_blk( PI_LONG_TYPE, &position, (char *) &cur_excep_num, &return_data->data_len );
    if ( return_code < 1 )
      {
        return_data->data_len = 0;
        position = return_data->data;

        ERROR( "Invalid block length (too long)" );	/* Log the error... */

        pi_fail_ok = PI_INVALID_VALUE;
        return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
        return( return_code ); 
      }

    return( return_code ); 
  } /* end PI_ACCEPT_EXCEPTION */

pi_monitor_callback( struct return_struc *return_data )
{
  short pi_fail_ok;
  int return_code = 1;
  PI_PHONE_DEMAND_REC in_mess;
  char *position;

  in_mess.frame_type = CALLOUT_FRAME;
  strcpy(in_mess.agent_id, "00005");
  strcpy(in_mess.phone_number, "7170745");
  strcpy(in_mess.demand_data, "EXTRA STUFF HERE");
  return_data->data_len = 0;
  position = return_data->data;  
  pi_fail_ok = PI_OK;
  return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
  return_code = ins_to_data_blk( sizeof( PI_PHONE_DEMAND_REC ), &position, (char *) &in_mess, &return_data->data_len );
  return( return_code );
}

pi_monitor_callout( struct return_struc *return_data )
{
  short pi_fail_ok;
  int return_code = 1;
  PI_PHONE_DEMAND_REC in_mess;
  char *position;

  in_mess.frame_type = CALLOUT_FRAME;
  strcpy(in_mess.agent_id, "00005");
  sprintf(in_mess.phone_number, "%-20s", "7170745");
  strcpy(in_mess.demand_data, "EXTRA STUFF HERE");
  return_data->data_len = 0;
  position = return_data->data;  
  pi_fail_ok = PI_OK;
  return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
  return_code = ins_to_data_blk( sizeof( PI_PHONE_DEMAND_REC ), &position, (char *) &in_mess, &return_data->data_len );
  return( return_code );  
}

pi_cback_cout_order( struct order_struc *order_data, struct return_struc *return_data )
{
  short pi_fail_ok;
  int return_code = 1;
  PI_CBACK_COUT_REC  in_mess;
  char    *position;

  // check block size
  if ( order_data->data_len != sizeof(PI_CBACK_COUT_REC) )
    {
      return_data->data_len = 0;
      position = return_data->data;
      
      ERROR( "Invalid block length" );	/* Log the error... */
      
      pi_fail_ok = PI_INVALID_LEN;
      return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
      return( return_code ); 
    }

  position = order_data->data;
  return_code = get_from_data_blk( sizeof( PI_CBACK_COUT_REC ), &position, (char *) &in_mess, &order_data->data_len );
  if ( return_code < 1 )
    {
      return_data->data_len = 0;
      position = return_data->data;
      
      ERROR( "Invalid parameter (too short)" );	/* Log the error... */
      
      pi_fail_ok = PI_INVALID_VALUE;
      return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
      return( return_code ); 
    }

  if ( in_mess.frame_type == CALLBACK_FRAME ) {
    if ( in_mess.active == YES )
      callback_active = TRUE;
    else
      callback_active = FALSE;
  }
  else if ( in_mess.frame_type == CALLOUT_FRAME ) {
    if ( in_mess.active == YES )
      callout_active = TRUE;
    else
      callout_active = FALSE;
  }

  return_data->data_len = 0;
  position = return_data->data;
  
  pi_fail_ok = PI_OK;
  return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );        

  return( return_code );
  
}
    
  
pi_line_mgr_order( struct order_struc *order_data, struct return_struc *return_data )
{
  short pi_fail_ok,		/* Return-value (PI_OK/PI_FAIL).	*/
          counter;		/* Common loop counter.			*/
    int return_code = 1;	/* Internal func. return-code.(OK=1)	*/
    PI_LINE_MGR_REC  in_mess;
    char *position;		/* Current position in data-block. 	*/
    char	*ptr;			/* general ptr for char testing */
    int  line_nbr = 0;
    int cl_nbr = 0;
    int i = 0;
    struct order_struc new_order_struc;
    ACCT_DATA_REC   *pAcctData;
    char sAgentID[6];

    /* Checking block size... */

    if ( order_data->data_len != sizeof(PI_LINE_MGR_REC) )
      {
        return_data->data_len = 0;
        position = return_data->data;

        ERROR( "Invalid block length" );	/* Log the error... */

        pi_fail_ok = PI_INVALID_LEN;
        return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
        return( return_code ); 
      }

    position = order_data->data;
    return_code = get_from_data_blk( sizeof( PI_LINE_MGR_REC ), &position, (char *) &in_mess, &order_data->data_len );
    if ( return_code < 1 )
      {
        return_data->data_len = 0;
        position = return_data->data;

        ERROR( "Invalid parameter (too short)" );	/* Log the error... */

        pi_fail_ok = PI_INVALID_VALUE;
        return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
        return( return_code ); 
      }

    strncpy( sAgentID, in_mess.agent_id, 6 );
    line_nbr = atoi(sAgentID);

    for ( i = 0; i < 20; i++ )
      {
        if ( in_mess.a_number[i] == ' ' )
          {
            in_mess.a_number[i] = '\0';
            break;
          }
      }
    
    switch ( in_mess.frame_type )
      {
      case AUTOBOOKING:        
      case STANDARD_BOOKING:
        if ( line_nbr >= 0 && Lines[line_nbr] != NULL )
          {
            strcpy( Lines[line_nbr]->EventPending, in_mess.a_number );
            strcpy( Lines[line_nbr]->TelephoneNbr, in_mess.a_number );        
            Lines[line_nbr]->NewCall = '1';
            Lines[line_nbr]->MonitorName[0] = in_mess.customer_id;
            for ( i = 0; i < MAX_PIDS; i++ )
              {
                if ( Lines[line_nbr]->UIPid[i] != 0 )
                  kill( Lines[line_nbr]->UIPid[i], SIGCONT );
              }
          }
        return_data->data_len = 0;
        position = return_data->data;
        
        pi_fail_ok = PI_OK;
        return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );        
        return_code = ins_to_data_blk( sizeof( PI_LINE_MGR_REC ), &position, (char *) &in_mess, &return_data->data_len );        
        break;

#ifdef FOO
      case AUTOBOOKING:
        bzero( &new_order_struc, sizeof(struct order_struc) );
        pAcctData = (ACCT_DATA_REC *)new_order_struc.data;
        pAcctData->fleet = in_mess.customer_id;
        strcpy( pAcctData->acct_nbr, in_mess.pin_number );
        return_code = d1000_open_db_files();
        return_code = m2000_process_message( 0, &new_order_struc, &cl_nbr );
        d1100_close_db_files();
        
        sprintf(in_mess.call_nbr, "%-10d", cl_nbr);
        return_data->data_len = 0;
        position = return_data->data;        
        return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );        
        return_code = ins_to_data_blk( sizeof( PI_LINE_MGR_REC ), &position, (char *) &in_mess, &return_data->data_len );                
        break;
#endif
      default:
        break;
      }
    

    return( return_code ); 
}


int pi_veh_status( struct order_struc *order_data, struct return_struc *return_data )
{
    short pi_fail_ok,		/* Return-value (PI_OK/PI_FAIL).	*/
          counter;		/* Common loop counter.			*/
    int return_code = 1;	/* Internal func. return-code.(OK=1)	*/
    int fl_nbr = 0, vh_nbr = 0;
    int times_thru, rc;
    VEH_STATUS in_mess;
    struct veh_rqst vrq;						/* structure for requesting vehicle detail info */
    VEH_RESP_REC *vrp;					/* structure for vehicle detail responses */
    VEH_RESP_HNDL  hVehResp;    
    char *position;
    char *ptr;
    unsigned char *tbpt;						/* pointer for looping through bit structures */
    char st_buf[64];							/* buffer for storing current taxi status text */
    char stp[8];								/* pointer for filling in buffer */
    unsigned char mask;							/* mask for looping through bit structures */
    unsigned char *vbpt;						/* pointer for looping through bit structures */
    unsigned char *dbpt;						/* pointer for looping through bit structures */
    int i;

        /* Checking block size... */

    pi_fail_ok = PI_OK;
    if ( order_data->data_len != sizeof(VEH_STATUS) )
      {
        return_data->data_len = 0;
        position = return_data->data;

        ERROR( "Invalid block length" );	/* Log the error... */

        pi_fail_ok = PI_INVALID_LEN;
        return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
        return( return_code ); 
      }

    position = order_data->data;
    return_code = get_from_data_blk( sizeof( VEH_STATUS ), &position, (char *) &in_mess, &order_data->data_len );
    if ( return_code < 1 )
      {
        return_data->data_len = 0;
        position = return_data->data;

        ERROR( "Invalid parameter (too short)" );	/* Log the error... */

        pi_fail_ok = PI_INVALID_VALUE;
        return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
        return( return_code ); 
      }

    return_data->data_len = 0;
    position = return_data->data;

    vh_nbr = atoi(in_mess.veh_id);

	vrq.d_prcs_id = DISP_DEST;	
	vrq.u_prcs_id = pid;	
	vrq.rec_type = VEH_REQ;
	vrq.fleet = in_mess.fleet;
	vrq.veh_nbr = vh_nbr;

	msgsnd(main_msgkey,
	       (struct msgbuf *)&vrq,sizeof(struct veh_rqst), IPC_NOWAIT);
	hVehResp = TMALLOC( VEH_RESP_REC, 1 );
	vrp = ( VEH_RESP_REC * )hVehResp;
	
	times_thru = 0;    

	while (times_thru <= 2)
	  {
	    rc = msgrcv(main_msgkey,
			(void *)vrp,sizeof(VEH_RESP_REC),(long)pid,0);
	    
	    if (rc == -1) 
	      {
		if (errno != EINTR) 
		  {
		    TFREE( hVehResp );
		    pi_fail_ok = PI_INVALID_VALUE;
		    return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
		    return( return_code ); 		    

		  }
		if (++times_thru == 2) 
		  {

		    TFREE( hVehResp );
		    pi_fail_ok = PI_INVALID_VALUE;
		    return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
		    return( return_code ); 		    
		  }
	      }
	    else 
	      if (vrp->rec_type == VEH_REQ)
		break;
	      else 
		{

		  TFREE( hVehResp );
		  pi_fail_ok = PI_INVALID_VALUE;
		  return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
		  return( return_code ); 		    		  
	    	}
	  }

    	tbpt = (unsigned char *)&vrp->cur_status;
    	memset(st_buf,0,64);
	memset(stp,0,8);
	
    	if (vrp->cur_status.emer) 
	  strcat(st_buf,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_57, "DISAB/"));
	
    	if (vrp->cur_status.posted) {
	  strcat(st_buf,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_58, "BOOKED "));
	  sprintf(stp,"%3.3d/",vrp->zone_num);
	  strcat(st_buf, stp);
    	}

       if (vrp->cur_status.crse_annul)
	 {
	   strcat(st_buf,"ANNUL/");
	 }
	
    	if (vrp->cur_status.e_posted) {
	  strcat(st_buf,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_59, "EBOOKED "));
	  sprintf(stp,"%3.3d/",vrp->zone_num);
	  strcat(st_buf, stp);
    	}
	
    	if (vrp->cur_status.on_break)
	  strcat(st_buf,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_60, "BREAK/"));
	
    	if (vrp->cur_status.pckup)
	  strcat(st_buf,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_61, "PICKUP/"));
	
    	if (vrp->cur_status.accepted)
	  strcat(st_buf,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_62, "ACCEPT/"));
	
    	if (vrp->cur_status.offered) 
	  strcat(st_buf,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_63, "OFFER/"));
	
    	if (vrp->cur_status.idle)
			strcat(st_buf,catgets(UI_m_catd, UI_1, UI_UI_SSTAT_64, "IDLE/"));


	strcpy( in_mess.current_status, st_buf );

	if (vrp->cur_status.meter_on)
	  strcpy(st_buf, catgets(UI_m_catd, UI_1, UI_UI_SSTAT_76, "UP "));
	else
	  strcpy(st_buf, catgets(UI_m_catd, UI_1, UI_UI_SSTAT_77, "DWN"));

	strcpy( in_mess.meter_status, st_buf );

	// copy statistics
	in_mess.assigns = vrp->assigns;
	in_mess.calls = vrp->calls;
	in_mess.flags = vrp->flags;
	in_mess.posts = vrp->posts;
	in_mess.cond_posts = vrp->cond_posts;
	in_mess.enroute_posts = vrp->enroute_posts;
	in_mess.short_meters = vrp->short_meter;
	in_mess.late_meters = vrp->late_meter;
	in_mess.no_accepts = vrp->no_accepts;
	in_mess.rejects = vrp->rejects;
	in_mess.breaks = vrp->breaks;
	in_mess.req_to_talk = vrp->req_to_talk;
	in_mess.messages = vrp->messages;
	in_mess.callbacks = vrp->callbacks;
	in_mess.callouts = vrp->callouts;
	in_mess.no_shows = vrp->no_shows;

	// Return attributes for vehicle and driver
	mask = 0x01;
    	vbpt = (unsigned char *)&vrp->veh_attr;
    	dbpt = (unsigned char *)&vrp->drv_attr;
    	for(i = 0; i < ATTR_MAX; i++) { /* MRB */
	  if (*vbpt & mask)
	    {
	      sprintf(st_buf, "%.2s ", fleet[0]->vd_attr_info[i].veh_attr);
	      strcat(in_mess.veh_attr, st_buf);
	    }
	  if (*dbpt & mask)
	    {
	      sprintf(st_buf, "%.2s ", fleet[0]->vd_attr_info[i].drv_attr);
	      strcat(in_mess.drv_attr, st_buf);
	    }
	  if (( mask <<= 1 ) == 0x00 )
	    {
	      mask = 0x01;
	      ++dbpt;
	      ++vbpt;
	    }
	}
	
	if ( pi_fail_ok == PI_OK )
	  {
	    return_code = ins_to_data_blk( sizeof(VEH_STATUS), &position, (char *) &in_mess, &return_data->data_len ); 
	    if ( return_code < 1 )
	      {
		return_data->data_len = 0;
		position = return_data->data;
		
		ERROR( "Invalid block length (too long)" );	/* Log the error... */
		
		pi_fail_ok = PI_INVALID_VALUE;
		return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
		return( return_code ); 
	      }
	  }

	return( return_code ); 	    
	
}
    

int pi_fleet_status( struct order_struc *order_data, struct return_struc *return_data )
{
    short pi_fail_ok,		/* Return-value (PI_OK/PI_FAIL).	*/
          counter;		/* Common loop counter.			*/
    int return_code = 1;	/* Internal func. return-code.(OK=1)	*/
    int fl_nbr = 0;
    FLEET_STATUS  in_mess;
    char *position;		/* Current position in data-block. 	*/
    char	*ptr;			/* general ptr for char testing */

    /* Checking block size... */

    if ( order_data->data_len != sizeof(FLEET_STATUS) )
      {
        return_data->data_len = 0;
        position = return_data->data;

        ERROR( "Invalid block length" );	/* Log the error... */

        pi_fail_ok = PI_INVALID_LEN;
        return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
        return( return_code ); 
      }

    position = order_data->data;
    return_code = get_from_data_blk( sizeof( FLEET_STATUS ), &position, (char *) &in_mess, &order_data->data_len );
    if ( return_code < 1 )
      {
        return_data->data_len = 0;
        position = return_data->data;

        ERROR( "Invalid parameter (too short)" );	/* Log the error... */

        pi_fail_ok = PI_INVALID_VALUE;
        return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
        return( return_code ); 
      }

    return_data->data_len = 0;
    position = return_data->data;

    if ( in_mess.fleet_id == 'H' )
      fl_nbr = 0;
    else
      fl_nbr = in_mess.fleet_id - 'A';

    if ( ( in_mess.fleet_id == 'A' ) || ( in_mess.fleet_id == 'H' ) ) {
      in_mess.signed_on = fleet[fl_nbr]->signed_on;
      in_mess.calls = fleet[fl_nbr]->calls;
      strcpy(in_mess.fleet_name, fleet[fl_nbr]->fleet_name);
      in_mess.signed_off = fleet[fl_nbr]->signed_off;
      in_mess.flags = fleet[fl_nbr]->flags;
      in_mess.cond_posts = fleet[fl_nbr]->cond_posts;
      in_mess.enroute_posts = fleet[fl_nbr]->enroute_posts;
      in_mess.short_meters = fleet[fl_nbr]->short_meters;
      in_mess.late_meters = fleet[fl_nbr]->late_meters;
      in_mess.no_accepts = fleet[fl_nbr]->no_accepts;
      in_mess.asigns = fleet[fl_nbr]->asigns;
      in_mess.rejects = fleet[fl_nbr]->rejects;
      in_mess.breaks = fleet[fl_nbr]->breaks;
      in_mess.req_to_talk = fleet[fl_nbr]->req_to_talk;
      in_mess.messages = fleet[fl_nbr]->messages;
      in_mess.callbacks = fleet[fl_nbr]->callbacks;
      in_mess.callouts = fleet[fl_nbr]->callouts;
      in_mess.no_shows = fleet[fl_nbr]->no_shows;
      in_mess.suspended = fleet[fl_nbr]->suspended;      
      in_mess.emergencies = fleet[fl_nbr]->emergencies;
      in_mess.wakeups  = fleet[fl_nbr]->wakeups;
      in_mess.cancels = fleet[fl_nbr]->cancels;
      in_mess.now_signed_on = fleet[fl_nbr]->now_signed_on;
      in_mess.now_signed_off = fleet[fl_nbr]->now_signed_off;
      in_mess.now_in_fleet = fleet[fl_nbr]->now_in_fleet;
      in_mess.now_calls = fleet[fl_nbr]->now_calls;
      in_mess.now_flags = fleet[fl_nbr]->now_flags;
      in_mess.now_posts = fleet[fl_nbr]->now_posts;
      in_mess.now_enroute_posts = fleet[fl_nbr]->now_enroute_posts;
      in_mess.now_late_meters = fleet[fl_nbr]->now_late_meters;
      in_mess.now_breaks = fleet[fl_nbr]->now_breaks;
      in_mess.now_req_to_talk = fleet[fl_nbr]->now_req_to_talk;
      in_mess.now_messages = fleet[fl_nbr]->now_messages;
      in_mess.now_callbacks = fleet[fl_nbr]->now_callbacks;
      in_mess.now_callouts = fleet[fl_nbr]->now_callouts;
      in_mess.now_no_shows = fleet[fl_nbr]->now_no_shows;
      in_mess.now_idle = fleet[fl_nbr]->now_idle;
      in_mess.now_suspended = fleet[fl_nbr]->now_suspended;
      in_mess.now_emergencies = fleet[fl_nbr]->now_emergencies;
    }
    

    return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );

    if ( return_code < 1 )
      {
        return_data->data_len = 0;
        position = return_data->data;

        ERROR( "Invalid block length (too long)" );	/* Log the error... */

        pi_fail_ok = PI_INVALID_VALUE;
        return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
        return( return_code ); 
      }

    if ( pi_fail_ok == PI_OK )
      {
        return_code = ins_to_data_blk( sizeof(FLEET_STATUS), &position, (char *) &in_mess, &return_data->data_len ); 
        if ( return_code < 1 )
          {
            return_data->data_len = 0;
            position = return_data->data;
    
            ERROR( "Invalid block length (too long)" );	/* Log the error... */
    
            pi_fail_ok = PI_INVALID_VALUE;
            return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
            return( return_code ); 
          }
      }

    return( return_code ); 
} /* end PI_FLEET_STATUS */    
    
/* ----------------------------------------------------------------------------
/* FUNCTION: PI_SEND_MESSAGE
/*
/* DESCRIPTION: Send a message to TaxiPak
/*
/* ORDER-DATA: MESSAGE-structure
/* 
/* RETURN-DATA:  if return-value = 1: return_data.data = PI_OK, 
/*                                                       Message_Number
/*                                    OR
/*                                    return_data.data = PI_FAIL,
/*                                                       Message_Number
/*	                              return_data.len  = length of data
/*
/*               if return-value = 0: return_data is not defined (close PI).
/*
------------------------------------------------------------------------------ */

int pi_send_message( struct order_struc *order_data, struct return_struc *return_data )
  {
    short pi_fail_ok,		/* Return-value (PI_OK/PI_FAIL).	*/
          counter;		/* Common loop counter.			*/
    int return_code = 1;	/* Internal func. return-code.(OK=1)	*/
    MSG_REC  in_mess;
    struct message cur_mess;	/* The actual message (structure).	*/
    char *position;		/* Current position in data-block. 	*/
    char received[ 3 ];		/* Message received (YES/NO)		*/
	char	*ptr;			/* general ptr for char testing */


    /* Checking block size... */


        MakeLogEntry("START --> PI_SEND_MESSAGE");
        
        if ( order_data->data_len != MESSAGE_STRUC_SIZE )
      {
        return_data->data_len = 0;
        position = return_data->data;


        sprintf( szLogMsg, "(%s:%d)(Invalid block length = %d should be %d", __FILE__, __LINE__, order_data->data_len, sizeof(MESSAGE_STRUC_SIZE) );
        MakeLogEntry( szLogMsg );	/* Log the error... */        


        pi_fail_ok = PI_INVALID_LEN;
        return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
        return( return_code ); 
      }

    position = order_data->data;
    return_code = get_from_data_blk( MESSAGE_STRUC_SIZE, &position, (char *) &in_mess, &order_data->data_len );
    if ( return_code < 1 )
      {
        return_data->data_len = 0;
        position = return_data->data;

        sprintf( szLogMsg, "(%s:%d)Invalid parameter(too short)", __FILE__, __LINE__ );
        MakeLogEntry( szLogMsg );

        pi_fail_ok = PI_INVALID_VALUE;
        return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
        MakeLogEntry("END --> PI_SEND_MESSAGE");        
        return( return_code ); 
      }

    // log the message to trace file
    sprintf( szLogMsg, "(%s:%d) Message received\n\tFleet: %c\n\tMsgNbr: %d\n\tCreator: %d\n\tDate: %s\tTime: %s\n\tGroup: %c\n\tReceiveID: %s\n\tText: %s", __FILE__, __LINE__, in_mess.fleet, in_mess.message_number, in_mess.creator, in_mess.creation_date, in_mess.creation_time, in_mess.receive_group, in_mess.receive_id, in_mess.message_text );
    MakeLogEntry( szLogMsg );
    
    return_data->data_len = 0;
    position = return_data->data;

    /***********************************/
    /* NULL-terminating ASCII-strings. */
    /***********************************/
    cur_mess.fleet = in_mess.fleet;

    strcpy(cur_mess.receive_id, in_mess.receive_id);
    strcpy( cur_mess.message_text, in_mess.message_text );
    get_pkup_date(cur_mess.creation_date);
    get_pkup_time(cur_mess.creation_time);
    cur_mess.creation_date[ 6 ] = '\0';
    cur_mess.creation_time[ 4 ] = '\0';
    cur_mess.receive_id[ 6 ] = '\0';
    //    cur_mess.message_text[ 200 ] = '\0';
    cur_mess.receive_group = in_mess.receive_group;	
	/* Check for valid MMP message in text buffer */
	if ( !ValidMMPmessage( cur_mess.message_text, 512 ) )
	{
       	return_data->data_len = 0;
       	position = return_data->data;

       	ERROR( "Invalid text data for MMP" );	/* Log the error... */

       	pi_fail_ok = PI_INVALID_MMP_CHAR;
       	return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
        MakeLogEntry("END --> PI_SEND_MESSAGE");                
       	return( return_code ); 
	}
    /**********************/
    /* FORMAT-check part. */
    /**********************/

    if ( cur_mess.fleet < 'A' || cur_mess.fleet > 'H' ) 	/* Is fleet valid??? (A-H) */
      {
       	return_data->data_len = 0;
       	position = return_data->data;

        MakeLogEntry( "Invalid fleet" );	/* Log the error... */
  
        pi_fail_ok = PI_INVALID_FLEET;
        return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
        MakeLogEntry("END --> PI_SEND_MESSAGE");                
        return( return_code ); 
      }
    else
      {
        counter = cur_mess.fleet - 'A';
        //        if (fleet[counter]->active != YES)
        //          {
        //            ERROR( "Invalid fleet" );	/* Log the error... */
        //       		return_data->data_len = 0;
        //       		position = return_data->data;
        //    
        //            pi_fail_ok = PI_INVALID_FLEET;
        //            return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
        //            return( return_code ); 
        //          }
      }

    /*******************************************/
    /* Valid receive-group? driver, vehicle... */
    /*******************************************/

    if (( cur_mess.receive_group != '*' && cur_mess.receive_group != MSG_FLEET && cur_mess.receive_group != MSG_SUPERVISOR ) &&
        ( cur_mess.receive_group != MSG_ZONE && cur_mess.receive_group != MSG_USER && cur_mess.receive_group != MSG_CALL_TAKER ) &&
        ( cur_mess.receive_group != MSG_MANAGER && cur_mess.receive_group != 'Q' && cur_mess.receive_group != MSG_VEHICLE && cur_mess.receive_group != MSG_DRIVER ))
      { 
        return_data->data_len = 0;
        position = return_data->data;

        MakeLogEntry( "Invalid receive-group (message)" );				/* Log the error... */ 
        pi_fail_ok = PI_INVALID_GROUP;
        return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
        MakeLogEntry("END --> PI_SEND_MESSAGE");                
        return( return_code ); 
      }
    
    /***********************************************/
    /* Execute TaxiPak-handling of PI_SEND_MESSAGE */
    /***********************************************/

    if ( glEnhSwitch & ENH0002 )
      cur_mess.creator = 9999;
    else
      cur_mess.creator = 8888;
    
    //    memset((char *)&message, '\0', sizeof( struct cisam_rm ));
    bzero(&message, sizeof(struct cisam_ml));
    message.created_by = cur_mess.creator;
    message.created_dt_tm = get_local_time();
    get_pkup_date( message.cr_date );
    get_pkup_time( message.cr_time );
    TP_to_PI_date( message.cr_date, cur_mess.creation_date );
    TP_to_PI_time( message.cr_time, cur_mess.creation_time );
    message.repeat = NO;
    message.fleet = cur_mess.fleet;
    message.to_type = cur_mess.receive_group;
    strncpy( message.to_selection, cur_mess.receive_id, 6 );
    strncpy( message.line_1, cur_mess.message_text, 100 );
    message.line_1[ 100 ] = '\0';
	memcpy( message.line_2, &(cur_mess.message_text[100]), 100 ); 
	for ( counter =0 ; counter < 101; counter++ )
		message.line_2[ counter ] = cur_mess.message_text[ counter + 100 ];
	
	message.line_2[100] = 0x00;

	if ( strstr(cur_mess.message_text, "%.L") )
	  cur_mess.message_text[strstr(cur_mess.message_text, "%.L") - cur_mess.message_text + 10] = '\0';

	strcpy(message.msgtext, cur_mess.message_text);

    cur_mess.message_number = message_send(&message);
    
    /*********************************/
    /* Check if message was received */
    /*********************************/

    if ( cur_mess.message_number < -90 ) 	/* Msg has not been recieved */
    {
      pi_fail_ok = abs( cur_mess.message_number);
      return_data->data_len = 0;
      position = return_data->data;
      
      return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
      if ( return_code < 1 )
      {
        return_data->data_len = 0;
        position = return_data->data;

        MakeLogEntry( "Invalid block length (too long)" );	/* Log the error... */

        pi_fail_ok = PI_INVALID_VALUE;
        return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
        MakeLogEntry("END --> PI_SEND_MESSAGE");                
        return( return_code );
      }
      return( return_code ); 
    }
    else /* PI_OK */
    {
      pi_fail_ok = PI_OK;
      return_data->data_len = 0;
      position = return_data->data;

      return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
      if ( return_code < 1 )
      {
        return_data->data_len = 0;
        position = return_data->data;

        MakeLogEntry( "Invalid block length (too long)" );	/* Log the error... */

        pi_fail_ok = PI_INVALID_VALUE;
        return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
        MakeLogEntry("END --> PI_SEND_MESSAGE");                
        return( return_code ); 
      }

      return_code = ins_to_data_blk( PI_INT_TYPE, &position, (char *) &cur_mess.message_number, &return_data->data_len );
      if ( return_code < 1 )
      {
        return_data->data_len = 0;
        position = return_data->data;

        ERROR( "Invalid block length (too long)" );	/* Log the error... */

        pi_fail_ok = PI_INVALID_VALUE;
        return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
        return( return_code ); 
      }
      return( return_code ); 
    }
  } /* end PI_SEND_MESSAGE */

/* ----------------------------------------------------------------------------
/* FUNCTION: PI_GET_MESSAGE
/*
/* DESCRIPTION: Get a message to TaxiPak
/*
/* ORDER-DATA: Message_Number
/* 
/* RETURN-DATA:  if return-value = 1: return_data.data = PI_OK, 
/*                                                       MESSAGE-structure
/*                                    OR
/*                                    return_data.data = PI_FAIL,
/*                                                       Message_Number
/*	                              return_data.len  = length of data
/*
/*               if return-value = 0: return_data is not defined (close PI).
/*
------------------------------------------------------------------------------ */

int pi_get_message( struct order_struc *order_data, struct return_struc *return_data )
  {
    short pi_fail_ok,		/* Return-value (PI_OK/PI_FAIL).	*/
          counter,		/* Common loop counter.			*/
          i;			/* Common loop counter.			*/
    int return_code = 1;	/* Internal func. return-code.(OK=1)	*/
    struct message cur_mess;	/* The actual message (structure).	*/
    struct cisam_ml *mess_ptr;	/* The actual message (DB).		*/
    int cur_mess_number;	/* Number of message to get.		*/
    char *position;		/* Current position in data-block. 	*/

    /**********************/
    /* FORMAT-check part. */
    /**********************/

    /* Checking block size... */

    if ( order_data->data_len != PI_INT_TYPE )
      {
        return_data->data_len = 0;
        position = return_data->data;

        ERROR( "Invalid block length" );	/* Log the error... */

        cur_mess_number = 0;
        pi_fail_ok = PI_INVALID_LEN;
        return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
        return( return_code ); 
      }

    position = order_data->data;
    return_code = get_from_data_blk( PI_INT_TYPE, &position, (char *) &cur_mess_number, &order_data->data_len );
    if ( return_code < 1 )
      {
        return_data->data_len = 0;
        position = return_data->data;

        ERROR( "Invalid parameter (too short)" );	/* Log the error... */

        cur_mess_number = 0;
        pi_fail_ok = PI_INVALID_VALUE;
        return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
        return( return_code ); 
      }

    /**********************************************/
    /* Execute TaxiPak-handling of PI_GET_MESSAGE */
    /**********************************************/

    mess_ptr = (struct cisam_ml *)req_buf;
    mess_ptr->nbr = cur_mess_number;
    
    if (db(MSGLOG_FILE_ID, OPEN, 0, ISINOUT+ISMANULOCK, 0) < 0)
      {
        ERROR("Error opening file - message-log");	
      }
   
    if ((db(MSGLOG_FILE_ID,READ_KEYS,&ml_key1,ISEQUAL,0)) < 0)
      {
        return_data->data_len = 0;
        position = return_data->data;

        ERROR( "Message does not exist" );		/* Log the error... */

        pi_fail_ok = PI_NO_MESS;
        return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
        return( return_code ); 
      }

    db(MSGLOG_FILE_ID, CLOSE, 0, 0, 0); 

    pi_fail_ok = PI_OK;

    cur_mess.fleet = mess_ptr->fleet;
    cur_mess.message_number = mess_ptr->nbr;
    cur_mess.creator = mess_ptr->from;
    TP_to_PI_date( mess_ptr->date_sent, cur_mess.creation_date );
    TP_to_PI_time( mess_ptr->time_sent, cur_mess.creation_time );
    cur_mess.receive_group = mess_ptr->to_type;
    strcpy( cur_mess.receive_id, mess_ptr->to_selection );
    strcpy( cur_mess.message_text, mess_ptr->msg_line1 );
    strcat( cur_mess.message_text, mess_ptr->msg_line2 );

    /******************************/
    /* Build return-data-block.   */
    /******************************/

    return_data->data_len = 0;
    position = return_data->data;

    return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
    if ( return_code < 1 )
      {
        return_data->data_len = 0;
        position = return_data->data;

        ERROR( "Invalid block length (too long)" );	/* Log the error... */

        pi_fail_ok = PI_INVALID_VALUE;
        return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
        return( return_code ); 
      }

    if ( pi_fail_ok == PI_OK )
      {
        return_code = ins_to_data_blk( sizeof(struct message), &position, (char *) &cur_mess, &return_data->data_len ); 
        if ( return_code < 1 )
          {
            return_data->data_len = 0;
            position = return_data->data;
    
            ERROR( "Invalid block length (too long)" );	/* Log the error... */
    
            pi_fail_ok = PI_INVALID_VALUE;
            return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
            return( return_code ); 
          }
      }

    return( return_code ); 
  } /* end PI_GET_MESSAGE */

/* ----------------------------------------------------------------------------
/* FUNCTION: PI_ALIVE
/*
/* DESCRIPTION: Check if PI is alive
/*
/* ORDER-DATA: None
/* 
/* RETURN-DATA:  if return-value = 1: return_data.data = PI_OK
/*                                    OR
/*                                    return_data.data = PI_FAIL
/*	                              return_data.len  = length of data
/*
/*               if return-value = 0: return_data is not defined (close PI).
/*
------------------------------------------------------------------------------ */

int pi_alive( struct order_struc *order_data, struct return_struc *return_data )
  {
    int return_code;
    short ret_val = PI_OK;

    char *position;

    return_data->data_len = 0;
    position = return_data->data;

    return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &ret_val, &return_data->data_len );
    return ( return_code ); 
  } /* end PI_ALIVE */

/* ----------------------------------------------------------------------------
/* FUNCTION: PI_QUIT
/*
/* DESCRIPTION: Disconnect from PI
/*
/* ORDER-DATA: None
/* 
/* RETURN-DATA:  if return-value = 1: return_data.data = PI_OK
/*                                    OR
/*                                    return_data.data = PI_FAIL
/*	                              return_data.len  = length of data
/*
/*               if return-value = 0: return_data is not defined (close PI).
/*
------------------------------------------------------------------------------ */

int pi_quit( struct order_struc *order_data, struct return_struc *return_data )
  {
    int return_code;
    short ret_val = PI_OK;

    char *position;

    return_data->data_len = 0;
    position = return_data->data;

    return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &ret_val, &return_data->data_len );
    return ( return_code ); 
  } /* end PI_QUIT */



	/* ----------------------------------------------------------------------------
	/* FUNCTION: MakeResponseMsg
	/*
	/* DESCRIPTION: Create a response to client for received msg.
	/*
	/* PARAMETER-LIST:  return_data  ->   Buffer for message sent back to client
	/*                  nMsgResponse ->   Response code for message
	/*                  szErrDesc    ->   Text description for error file
	/*
	/* RETURN-VALUE: return value always OK
	/*
	------------------------------------------------------------------------------ */


int MakeResponseMsg( return_type *return_data, short nMsgResponse, char *szErrDesc )
{
		short shData = nMsgResponse;

		if ( nMsgResponse != PI_OK )
			ERROR( szErrDesc );
		memcpy( return_data->data, &shData, PI_SHORT_TYPE );
		return_data->data_len = PI_SHORT_TYPE;
		return(1);
}

/* ----------------------------------------------------------------------------
/* FUNCTION: GET_FROM_DATA_BLK
/*
/* DESCRIPTION: Gets a value from the order data-block.
/*
/* PARAMETER-LIST: 	Value-length,
/*			Data-block-position,
/*			Value,
/*			Data-blk-length.
/* 
/* RETURN-VALUE: 0 if length < 0/1 if OK.
/*
------------------------------------------------------------------------------ */

int get_from_data_blk( int temp_data_len, char **current_pos, char *temp_data, int *data_blk_len ) 
  {
    if ( *data_blk_len - temp_data_len < 0 )
      return( 0 );
    else 
      {
        memcpy( temp_data, *current_pos, temp_data_len );
        *data_blk_len -= temp_data_len;
        *current_pos += temp_data_len;
        return( 1 );
      }
  } /* end GET_FROM_DATA_BLK */

/* ----------------------------------------------------------------------------
/* FUNCTION: INS_TO_DATA_BLK
/*
/* DESCRIPTION: Inserts a value into the return data-block.
/*
/* PARAMETER-LIST: 	Value-length,
/*			Data-block-position,
/*			Value,
/*			Data-block-length.
/* 
/* RETURN-VALUE: 0 if length > MAX_DATA_LEN/1 if OK.
/*
------------------------------------------------------------------------------ */

int ins_to_data_blk( int temp_data_len, char **current_pos, char *temp_data, int *data_blk_len ) 
  {
    if ( *data_blk_len + temp_data_len > MAX_DATA_LEN )
      return( 0 );
    else 
      {
        memcpy( *current_pos, temp_data, temp_data_len );
        *data_blk_len += temp_data_len;
        *current_pos += temp_data_len;
        return( 1 );
      }
  } /* end INS_TO_DATA_BLK */

/* ----------------------------------------------------------------------------
/* FUNCTION: CALL_STATUS_CONV
/*
/* DESCRIPTION: Converts the call-status to an enumeration.
/*
/* PARAMETER-LIST: Status as language-dependant string.
/* 
/* RETURN-VALUE: Call-status enumeration.
/*
------------------------------------------------------------------------------ */

short call_status_conv( char *status, size_t stat_len ) 
  {
    short lookup_idx = 0;

    while (( lookup_idx < STATUS_TABLE_MAX ) && ( strncmp( status, call_status[ lookup_idx ], stat_len ) != 0 ))
      lookup_idx++;

    if ( lookup_idx == STATUS_TABLE_MAX )
      lookup_idx = NOEXIST; 		 	/* return call_status NOEXIST if status not found. */
    else					/* ----------------------------------------------- */
      lookup_idx = ( lookup_idx % NUM_OF_CALL_STATUS ) + 1;

    return( lookup_idx );
  } /* end CALL_STATUS_CONV */

/* ----------------------------------------------------------------------------
/* FUNCTION: GET_DATE_TIME
/*
/* DESCRIPTION: Gets an ASCII-format of the current date/time.
/*
/* PARAMETER-LIST: 	Current Date "YYMMDD",
/*			Current Time "HHMM".
/* 
/* RETURN-VALUE: 0 if failure/1 if OK.
/*
------------------------------------------------------------------------------ */

int get_ascii_date_time( char *cur_date, char *cur_time )
  {
    int return_code;
    time_t tp;

    tp = time(NULL); 
    return_code = strftime( cur_date, 7, "%y%m%d", localtime( &tp )); 
    if ( return_code != 6 )
      return( -1 );
    return_code = strftime( cur_time, 5, "11/3/94M", localtime( &tp )); 
    if ( return_code != 4 )
      return( -2 );
    return( 1 );
  } /* end GET_DATE_TIME */

/* ----------------------------------------------------------------------------
/* FUNCTION: WRITE_ERROR
/*
/* DESCRIPTION: Writes all the errors into a specified error-log.
/*
/* PARAMETER-LIST: <MODULE>, <LINENUMBER>, <MESSAGE>.
/* 
/* RETURN-VALUE: 0 if failure/1 if OK.
/*
------------------------------------------------------------------------------ */

int write_error( char *module, int linenumber, char *message )
  {
    int return_code;
    char now_date[ 7 ],
         now_time[ 5 ];
    FILE *err_log;

    return_code = get_ascii_date_time( now_date, now_time );

    if (( err_log = fopen( "pi_err.log", "a" )) == NULL )
      {
        ERROR( "Error opening errorlog-file" );
      }

    if ( return_code == 1 )
      fprintf( err_log, "%s - %s\tFILE: %s\t\t#%d\tERR.: %s\n", now_date, now_time, module, linenumber, message );  
#ifdef	NDEBUG
      printf( "%s - %s\tFILE: %s\t\t#%d\tERR.: %s\n", now_date, now_time, module, linenumber, message );  
#endif
	 
    fclose( err_log );
    return ( return_code );
  } /* end WRITE_ERROR */

/* ----------------------------------------------------------------------------
/* FUNCTION: SET_ATTR_BIT
/*
/* DESCRIPTION: Sets up the attribute-match bits for vehicle/driver.
/*
/* PARAMETER-LIST: bit_num, size, struct_ptr.
/* 
/* RETURN-VALUE: TRUE.
/*
------------------------------------------------------------------------------ */

set_attr_bit(bit_num, size, struct_ptr)
	int			 	 bit_num,
				 	 size;
	unsigned char	*struct_ptr;
{
	unsigned char	*bit_ptr,
					 mask;
	int				 shift_by;


	mask = 0x80;
	bit_ptr = (unsigned char *)struct_ptr + bit_num/8;
	shift_by = bit_num % 8;
	mask >>= shift_by;

	*bit_ptr = *bit_ptr | mask;

	return(TRUE);
}

/**************************************************************************************/
/* get_internal_time: gets the time in internal C format; from the display formats    */
/*	of date & time								      */
/*	Returns 	0  if invalid date/time arg is passed to function	      */
/*			time in unix format 					      */
/**************************************************************************************/
time_t get_internal_time(date_copy,time_copy)
	char *date_copy;			/* assumed to be in yy/mm/dd format */
	char *time_copy;			/* assumed to be in 24hr or 12hr format; hh:mm{a/p} */
{
	char date_time[13];			/* date & time formatted in YYMMDDhhmmss format */
	char scratch_ptr[3];			/* scratch area */
	int hours=0;

  if (DATE_FORMAT == YEAR_FIRST_IN_DATE)
    {	
        date_time[0] = date_copy[0];
        date_time[1] = date_copy[1];
        date_time[2] = date_copy[2];
        date_time[3] = date_copy[3];
        date_time[4] = date_copy[4];
        date_time[5] = date_copy[5];
    }
  else
    {
        date_time[0] = date_copy[4];
        date_time[1] = date_copy[5];
        date_time[2] = date_copy[2];
        date_time[3] = date_copy[3];
        date_time[4] = date_copy[0];
        date_time[5] = date_copy[1];
    }

	/* copy the hour */
	date_time[6] = time_copy[0];
	date_time[7] = time_copy[1];


	date_time[8] = time_copy[2]; 			/* copy the min from time */
	date_time[9] = time_copy[3];
	
	date_time[10] = '0'; 				/* copy sec's to be 0 secs */
	date_time[11] = '0';
	date_time[12] = '\0';
	
	return(conversion(date_time)); 			/* now get the internal time */

}  							/* end get_internal_time() */

/******************************************************************************
* convert YYMMDDhhmmss to a long.
* returns - 0 if invalid argument (date/time format is passed)
*	  - the time in internal C format as strored by Unix
******************************************************************************/
time_t conversion(date_time) 
char date_time[13];
{
    
    struct tm t;
    struct tm *t_curr;
    extern struct tm *localtime();    
    long tl;
    int  i;
    int  dst_save;
    time_t nowtime;

    nowtime = time(0);
    t_curr = localtime(&nowtime);

    memcpy( &t, t_curr, sizeof( struct tm ) );
    
    if (strlen(date_time) != 12) 
	/* invalid format for date/time passed to function */
	return(0);
    
    for(i=0; i<12; i++)
	if (date_time[i] < '0' || date_time[i] > '9')	
	    /* char passed should all be digits; are not */
	    return(0);

    if (TO_DIGIT(date_time[0]) * 10 + TO_DIGIT(date_time[1]) < 80 )
      t.tm_year = (TO_DIGIT(date_time[0]) * 10 + TO_DIGIT(date_time[1]) + 100 );
    else
      t.tm_year = (TO_DIGIT(date_time[0]) * 10 + TO_DIGIT(date_time[1]));          

    t.tm_mon = TO_DIGIT(date_time[2]) * 10 + TO_DIGIT(date_time[3]) - 1;
    t.tm_mday = TO_DIGIT(date_time[4]) * 10 + TO_DIGIT(date_time[5]);
    t.tm_hour = TO_DIGIT(date_time[6]) * 10 + TO_DIGIT(date_time[7]);
    t.tm_min = TO_DIGIT(date_time[8]) * 10 + TO_DIGIT(date_time[9]);
    t.tm_sec = TO_DIGIT(date_time[10]) * 10 + TO_DIGIT(date_time[11]);
    
    if (t.tm_year < 70 ||
	t.tm_mon < 0 || t.tm_mon > 11 ||
	t.tm_mday < 1 || t.tm_mday > days_in_month(t.tm_mon + 1, t.tm_year) ||
	t.tm_hour > 23 ||
	t.tm_min > 59 ||
	t.tm_sec > 59)
	return(0);

    dst_save = t_curr->tm_isdst;
    t.tm_isdst = -1;
    
    tl = timelocal(&t);

    if ( ( dst_save != t.tm_isdst ) &&   // time change between time of call entry and due time
	 ( dst_save == 1 ) &&            // time of call entry is during DST
	 ( t.tm_hour == 2 ) )            // due time of call is 02:00 to 02:59
      {
	// go back to summertime reference
	t.tm_isdst = 1;
	tl = timelocal( &t );
      }
    
    return(tl);
    
}  /* End of conversion()  */

/* days_in_month: returns the # of days in month mm, in year yy */
int days_in_month(mm,yy)
int mm;	/* month */
int yy;	/* year */
{
    
    static int days[] = {31,28,31,30,31,30,31,31,30,31,30,31};
    
    /* if month is not FEB, return # of days */
    if ( mm != 2 )
	return(days[mm-1]);
    else {
	/* month is FEB; if its a leap yaer, then return 29,
	 * else return 28
	 */
	if ( ( yy % 4 == 0  && yy % 100 != 0 ) || ( yy % 400 == 0 ) || ( yy == 100 )  ) 
	    /* its a  leap year */
	    return(29);
	else 
	    /* its not a leap year */
	    return(28);
    }  /* end else ... FEB */
    
}  /* end days_in_month() */

int
AddTimeCall( pCall )
     struct cisam_cl *pCall;
{
  TIME_CALL_REC  *pTimeCall;
  int rc;

  if ( pCall == NULL )
    return ( -1 );
  
  pTimeCall = ( TIME_CALL_REC * )tmp_req_buf;

  pTimeCall->fleet = pCall->fleet;
  pTimeCall->zone = pCall->pckup_zone;
  pTimeCall->due_date_time = pCall->due_date_time;
  pTimeCall->cl_nbr = pCall->nbr;
  strcpy( pTimeCall->drv_attr, pCall->drv_attr );
  strcpy( pTimeCall->veh_attr, pCall->veh_attr );

  if ( ( rc = db( TIME_CALL_FILE_ID, ADD_REC, &time_call_key1, ISEQUAL, 0 ) ) < 0 )
    {
      return( -1 );
    }
  else
    return ( 1 );
}

int
CheckTimeCallCriteria( pCall )
     struct cisam_cl  *pCall;
{
  /** Checks whether CALL meets the criteria for tracking **/
  /** as a time_call in the TIME_CALL table               **/

  if ( ( pCall->pri_status == 63 ) &&
       ( pCall->personal_rqst != ONLY_PERS_REQ ) &&
       ( ( pCall->grouped_with == 0 ) || ( pCall->grouped_with == pCall->nbr ) ) )
    return( 1 );
  else
    return( -1 );
}

int
DeleteTimeCall( CallNbr )
     int CallNbr;
{
  TIME_CALL_REC *pTimeCall;
  int  rc;

  pTimeCall = ( TIME_CALL_REC * ) tmp_req_buf;
  pTimeCall->cl_nbr = CallNbr;

  if ( ( rc = db( TIME_CALL_FILE_ID, READ_KEYS, &time_call_key1, ISEQUAL, 0 ) ) < 0 )
    return( -1 );

  if ( ( rc = db( TIME_CALL_FILE_ID, DELETE, &time_call_key1, 0, 0 ) ) < 0 )
    return( -1 );

  return( 1 );
}

int pi_pre_book_info( struct order_struc *order_data, struct return_struc *return_data )
{
  int       y = 0, return_code;
  char      *position;
  short     pi_fail_ok;
  PRE_BOOK_INFO *pPreBookInfo;
  int rc, times_thru, i;  

  position = return_data->data;
  return_data->data_len = 0;  
  pi_fail_ok = PI_OK;
  return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );  
  pPreBookInfo = ( PRE_BOOK_INFO * ) order_data->data;

  /* check if fleet is active and zone exists */
  for ( y = 0; y < FLEET_MAX; y++ )
    if ( fleet[y]->fleet_id == pPreBookInfo->fleet )
      break;

  if ( ( fleet[y]->active != YES ) || ( fleet[y]->zone_ptr[pPreBookInfo->zone_nbr] == NULL ) )
    {
      return_data->data_len = 0;
      position = return_data->data;
      pi_fail_ok = PI_INV_ZONE;
      return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
      return( return_code );
    }

  pi_tcr_check_time_call_restriction( pPreBookInfo, y );

  return_code = ins_to_data_blk( sizeof( PRE_BOOK_INFO ), &position, (char *)pPreBookInfo, &return_data->data_len );

  return( return_code );  
}

int pi_rqst_gps_pos ( struct order_struc *order_data, struct return_struc *return_data )
{
  int       return_code;
  char      *position;
  short     pi_fail_ok;
  PI_RQST_GPS_POS *pRqstGPSPos;
  struct excpt_rec except_fifo;

  position = return_data->data;
  return_data->data_len = 0;
  
  pRqstGPSPos = (PI_RQST_GPS_POS *) order_data->data;

  LogMsg( PI_RQST_GPS, EXTP_TO_PI, (void *)pRqstGPSPos );
  // Notify Dispatch of the Request
  except_fifo.u_prcs_id = pid;
  except_fifo.rec_type = GPS_POS_REQ;
  except_fifo.except_ptr = '\0';
  except_fifo.except_nbr = 0;
  except_fifo.resolve_code = R_EMERG;
  except_fifo.call_nbr = 0;
  except_fifo.grouped_with = 0;
  except_fifo.veh_nbr = pRqstGPSPos->veh_nbr;
  except_fifo.zone_nbr = 0;
  except_fifo.old_zone_nbr = 0;
  except_fifo.old_due_time = 0;
  
  if (write(fifo_fd,&except_fifo,sizeof(struct excpt_rec)) == -1)	/* Write FIFO to dispatch */
    {
      ERROR("Error writing to FIFO.");
    }

  pi_fail_ok = PI_OK;
  return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );  
  return( return_code );  
}


  
int pi_get_callh( struct order_struc *order_data, struct return_struc *return_data )
{
  int                cur_call_number;
  struct cisam_ch callh;
  PI_CALLH_REC   callh_rec[20];
  
  char *position = NULL;		/* Current position in data-block. 	*/
  int return_code = 1;	/* Internal func. return-code.(OK=1)	*/
  short pi_fail_ok;
  int mode, i;
  int nbr_callh = 0;

  bzero( &callh, sizeof( struct cisam_ch ) );
  
  if ( order_data->data_len != PI_INT_TYPE )
    {
      return_data->data_len = 0;
      position = return_data->data;
      
      ERROR( "Invalid block length" );	/* Log the error... */
      
      pi_fail_ok = PI_INVALID_LEN;
      return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
      return( return_code ); 
    }

  position = order_data->data;  

  return_code = get_from_data_blk( PI_INT_TYPE, &position, (char *) &cur_call_number, &order_data->data_len );

  callh.call_nbr = cur_call_number;
  callh.status_dt_tm = 0;
  mode = ISGTEQ;
  nbr_callh = 0;
  position = return_data->data;  
  while ( ( db_read_key( CALLH_FILE_ID, &callh, &ch_key3, mode ) == SUCCESS ) )
    {
      mode = ISNEXT;
      if ( callh.call_nbr != cur_call_number )
	break;

      strcpy( callh_rec[nbr_callh].event_desc, callh.event_desc );
      strcpy( callh_rec[nbr_callh].status_date, callh.status_date );
      strcpy( callh_rec[nbr_callh].status_time, callh.status_time );
      callh_rec[nbr_callh].veh_nbr = callh.veh_nbr;
      callh_rec[nbr_callh].drv_id = callh.drv_id;
      callh_rec[nbr_callh].user_id = callh.user_id;
      strcpy( callh_rec[nbr_callh].action_taken, callh.action_taken );
      strcpy( callh_rec[nbr_callh].action_date, callh.action_date );
      strcpy( callh_rec[nbr_callh].action_time, callh.action_time );
      callh_rec[nbr_callh].ch_call_number = callh.call_nbr;

      if (nbr_callh > 19)
	break;
	    
      ++nbr_callh;
    }

  pi_fail_ok = PI_OK;
  return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );

  return_code = ins_to_data_blk( sizeof(int), &position, (char *) &nbr_callh, &return_data->data_len );

  for ( i = 0; i < nbr_callh; i++ )
    {
      return_code = ins_to_data_blk( sizeof(struct cisam_ch), &position, (char *) &callh_rec[i], &return_data->data_len );
    }

  return( return_code );
}

int pi_update_call( struct order_struc *order_data, struct return_struc *return_data ) 
  {
    short pi_fail_ok,     /* Return-value (PI_OK/PI_FAIL).    */
      counter, i;     /* Common loop counters.      */
    int call_nbr;
    int return_code = 1;	/* Internal func. return-code.(OK=1)		*/
    int db_call_num;      /* The call-number assigned to the call_ptr.  */
    ACCT_DATA_REC *spCurCall;
    char *position;   /* Current position in data-block.    */
    struct call_ipc_rec sCallRec;
    struct cisam_cl *call_ptr;
    struct cisam_ch call_history, *callh_ptr;
    time_t min_time, now_time;
    char min_ascii_time[7];char min_ascii_date[12];
    char future_call;   /* Indicating whether call is a future-call.  */
    struct call_ipc_rec cipc;
    
    /**********************/
    /* FORMAT-check part. */
    /**********************/
    if ( order_data->data_len != ACCT_DATA_REC_SIZE )
      return( MakeResponseMsg( return_data, (short) PI_INVALID_LEN,
			       "Invalid msg length : PI_UPDATE_CALL" ) );    
    
    
    /**********************/
    /* FORMAT-check part. */
    /**********************/
    bzero(&cipc, sizeof(struct call_ipc_rec));
    callh_ptr = &call_history;
    spCurCall = ( ACCT_DATA_REC *) order_data->data;
    
    call_nbr = atoi(spCurCall->call_number);
    cipc.call_nbr = call_nbr;
    
    if (call_nbr > 0)
      {
	call_ptr = (struct cisam_cl *)req_buf;
	call_ptr->nbr = call_nbr;

	if((db(CALL_FILE_ID,READ_KEYS,&cl_key1,ISEQUAL+ISLOCK,0)) < 0)
	  {
	    printf( "Failed with isam error (%d)\n", iserrno );
	    ERROR( "Call does not exist" );		/* Log the error... */
	    pi_fail_ok = PI_NO_CALL;
	    return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
	    return(return_code);
	  }

	// determine if a future call
	now_time = time(NULL);
	min_time = now_time + (time_t) (fleet[ spCurCall->fleet - 'A' ]->future_call_minimum);	
	strftime( min_ascii_time, 7, "%H\%M", localtime( &min_time ));
	strftime( min_ascii_date, 10, "%d%m%y", localtime( &min_time ));	
	if ( get_alt_tm_from_date_time( spCurCall->due_date, spCurCall->due_time ) <
	     get_alt_tm_from_date_time( min_ascii_date, min_ascii_time ) )
	  future_call = 0;
	else
	  future_call = 1;    /* Call is a legal future-call... */

	cipc.old_due_time = call_ptr->due_date_time;	
	call_ptr->due_date_time = (time_t)get_internal_time((char *)spCurCall->due_date, (char *)spCurCall->due_time);
	PI_to_TP_date(spCurCall->due_date, call_ptr->due_date);
	PI_to_TP_time(spCurCall->due_time, call_ptr->due_time);
	strcpy(call_ptr->pckup_str_name, spCurCall->from_addr_street);
	call_ptr->pckup_str_nbr = atoi(spCurCall->from_addr_number);
	strcpy(call_ptr->pckup_city, spCurCall->from_addr_city);

	cipc.u_prcs_id = pid;
	cipc.rec_type = CALL_REQ;
	cipc.fleet = call_ptr->fleet;
	cipc.pickup_zone = call_ptr->pckup_zone;
	cipc.priority = call_ptr->priority;
	cipc.due_time = call_ptr->due_date_time;
	cipc.msg_nbr = 0;
	cipc.updated_call = 1;
	cipc.FTJ_nbr = -1;
	cipc.call_type.passenger = TRUE;

	if (future_call)
	  {
	    call_ptr->extended_type[TIMEC_TYPE_OFF] = TIMEC;
	    call_ptr->extended_type[TIMEC_TYPE_OFF+1] = ' ';
	    strcpy( call_ptr->status, catgets(WRITER_catd, 1, 41, "PENDING" ) );
	    cipc.call_type.time = TRUE;
	  }
	else
	  {
	    call_ptr->extended_type[TIMEC_TYPE_OFF] = ' ';
	    call_ptr->extended_type[TIMEC_TYPE_OFF+1] = ' ';
	    //	    strcpy(call_ptr->status, catgets(WRITER_catd, 1, 39, "UNASSGND" ));
	    cipc.call_type.time = FALSE;
	  }
	bzero(&cipc.veh_attr, sizeof(struct veh_attributes));
	bzero(&cipc.drv_attr, sizeof(struct drv_attributes));
	for (counter = 0; counter < 32; counter ++)
	  {
	    if (call_ptr->veh_attr[counter] == YES)
	      set_attr_bit(counter, sizeof(struct veh_attributes), &cipc.veh_attr);
	    if (call_ptr->drv_attr[counter] == YES)
	      set_attr_bit(counter, sizeof(struct drv_attributes), &cipc.drv_attr);
	  }
	
	if ((db(CALL_FILE_ID, UPDATE_REC, &cl_key1, 0, 0)) < 0)		/* R2 */
	  {
	    printf( "Failed with isam error (%d)\n", iserrno );
	    ERROR("Error updating <call record>");
	  }
	if ((db(CALL_FILE_ID,RELEASE,&cl_key1,0,0)) < 0)
	  {
	    printf( "Failed with isam error (%d)\n", iserrno );
	    ERROR("Error releasing <call record>");
	  }

	//Add Call history rec
	memset((char *)callh_ptr, '\0', sizeof( struct cisam_ch ));
	
	callh_ptr->call_nbr = call_ptr->nbr;
	callh_ptr->exception_nbr = 0;
	callh_ptr->fleet = call_ptr->fleet;
	callh_ptr->event_type = REGULAR_EVENT;
	callh_ptr->status_dt_tm = (int)time(NULL);
	callh_ptr->veh_nbr = 0;
	callh_ptr->drv_id = 0;
	callh_ptr->user_id = call_ptr->entered_by;
	
	get_pkup_date( callh_ptr->status_date );
	get_pkup_time( callh_ptr->status_time );
	
	strcpy( callh_ptr->event_desc, "KELA" );
	memcpy(req_buf, (char *)callh_ptr, sizeof( struct cisam_ch ));  /* Pointer to the Request-buffer. */
	if ( db(CALLH_FILE_ID, ADD_REC, &ch_key1, ISEQUAL ) < 0 )
	  {
	    ERROR( "Cannot add call-history-record for PI-call (DB)." );
	    return FALSE;
	  }

	// Tell dispatch

	EnterCallInSharedMem(&cipc);
      }
    /******************************/
    /* Build return-data-block.   */
    /******************************/

    return_data->data_len = 0;
    position = return_data->data;
    pi_fail_ok = PI_OK;


    return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
    return( return_code ); 
  }


int pi_get_account_addr( struct order_struc *order_data, struct return_struc *return_data )
{
  PI_GET_ACCOUNT_ADDR_REC *pGetAcctAddr;
  ACAD_REC  acad_rec;
  int       account_number;
  struct cisam_ac	                 account;
  struct cisam_cl call, *call_ptr;        /* The call-structure similar to the one used in TaxiPak.     */
  struct call cur_addr[10];
  char *position = NULL;		/* Current position in data-block. 	*/
  int return_code = 1;	/* Internal func. return-code.(OK=1)	*/
  short pi_fail_ok;
  int mode, i;
  int nbr_addr = 0;

  bzero( cur_addr, sizeof(struct call)*10 );
  
  position = (char *)return_data->data;
  return_data->data_len = 0;

  pGetAcctAddr = (PI_GET_ACCOUNT_ADDR_REC *)order_data->data;

  return_code = d1000_open_db_files();

  account_number = atoi(pGetAcctAddr->ID);

  if ( d2000_get_account(account_number, &account, pGetAcctAddr->Fleet) == FAIL )
    {
      return_data->data_len = 0;
      position = return_data->data;
      pi_fail_ok = PI_INV_ZONE;
      return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
      return( return_code );
    }

  acad_rec.id.account_id.fleet = account.fleet;
  strcpy(acad_rec.id.account_id.nbr, account.nbr);
  acad_rec.id.index = 1;
  mode = ISEQUAL;

  db_open( ACAD_FILE_ID, ISINOUT | ISMANULOCK );
  while( ( db_read_key( ACAD_FILE_ID, &acad_rec, &ACAD_key, mode ) == SUCCESS ) )
    {
      mode = ISNEXT;      
      if ( strncmp( acad_rec.id.account_id.nbr, account.nbr, strlen(acad_rec.id.account_id.nbr) ) )
	break;

      cur_addr[nbr_addr].fleet = acad_rec.id.account_id.fleet;
      strcpy(cur_addr[nbr_addr].from_addr_street, acad_rec.address.street_name);
      strncpy(cur_addr[nbr_addr].from_addr_city, acad_rec.address.city_name, 3);
      cur_addr[nbr_addr].from_addr_number = acad_rec.address.street_nbr;

      if (nbr_addr > 9)
	break;
      
      ++nbr_addr;      
    }
  
  pi_fail_ok = PI_OK;
  return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );

  pGetAcctAddr->nbr_of_addr = nbr_addr;
  return_code = ins_to_data_blk( sizeof(PI_GET_ACCOUNT_ADDR_REC), &position, (char *)pGetAcctAddr, &return_data->data_len );
  
  for ( i = nbr_addr; i--; i >= 0 )
    {
      return_code = ins_to_data_blk( sizeof( struct call ), &position, (char *) &cur_addr[i], &return_data->data_len );
    }

  return(return_code);
}

int pi_get_account_indv( struct order_struc *order_data, struct return_struc *return_data )
{
  PI_GET_ACCOUNT_INDV_REC *pGetAcctIndv;
  INDV_REC  indv_rec;
  int       account_number;
  struct cisam_ac	                 account;
  struct cisam_cl call, *call_ptr;        /* The call-structure similar to the one used in TaxiPak.     */
  char   cur_indv[10][25];
  char *position = NULL;		/* Current position in data-block. 	*/
  int return_code = 1;	/* Internal func. return-code.(OK=1)	*/
  short pi_fail_ok;
  int mode, i;
  int nbr_indv = 0;

  position = (char *)return_data->data;
  return_data->data_len = 0;

  pGetAcctIndv = (PI_GET_ACCOUNT_INDV_REC *)order_data->data;

  return_code = d1000_open_db_files();

  account_number = atoi(pGetAcctIndv->ID);

  if ( d2000_get_account(account_number, &account, pGetAcctIndv->Fleet) == FAIL )
    {
      return_data->data_len = 0;
      position = return_data->data;
      pi_fail_ok = PI_INV_ACCOUNT;
      return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
      return( return_code );
    }

  indv_rec.individual_id.acct_id.fleet = account.fleet;
  strcpy(indv_rec.individual_id.acct_id.nbr, account.nbr);

  mode = ISEQUAL;

  db_open( INDV_FILE_ID, ISINOUT | ISMANULOCK );
  while( ( db_read_key( INDV_FILE_ID, &indv_rec, &indv_key1, mode ) == SUCCESS ) )
    {
      mode = ISNEXT;      
      if ( strncmp( indv_rec.individual_id.acct_id.nbr, account.nbr, strlen(indv_rec.individual_id.acct_id.nbr) ) )
	break;

      strcpy(cur_indv[nbr_indv], indv_rec.individual_id.name);

      if (nbr_indv > 9)
	break;
      
      ++nbr_indv;      
    }
  
  pi_fail_ok = PI_OK;
  return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );

  pGetAcctIndv->nbr_of_indv = nbr_indv;
  return_code = ins_to_data_blk( sizeof(PI_GET_ACCOUNT_INDV_REC), &position, (char *)pGetAcctIndv, &return_data->data_len );
  
  for ( i = nbr_indv; i--; i >= 0 )
    {
      return_code = ins_to_data_blk( 25, &position, cur_indv[i], &return_data->data_len );
    }

  return(return_code);
}
  
int pi_get_account_calls( struct order_struc *order_data, struct return_struc *return_data )
{
  PI_GET_CALLS_REC *pGetCalls;
  int       account_number;
  struct cisam_ac	                 account;
  struct cisam_cl call, *call_ptr;        /* The call-structure similar to the one used in TaxiPak.     */
  struct call cur_call[50];
  char *position = NULL;		/* Current position in data-block. 	*/
  int return_code = 1;	/* Internal func. return-code.(OK=1)	*/
  short pi_fail_ok;
  int mode, i;
  int nbr_calls = 0;

  bzero( cur_call, sizeof(struct call)*10 );
  
  position = (char *)return_data->data;
  return_data->data_len = 0;

  pGetCalls = (PI_GET_CALLS_REC *)order_data->data;

  return_code = d1000_open_db_files();

  account_number = atoi(pGetCalls->ID);

  if ( d2000_get_account(account_number, &account, pGetCalls->Fleet) == FAIL )
    {
      return_data->data_len = 0;
      position = return_data->data;
      pi_fail_ok = PI_INV_ZONE;
      return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
      return( return_code );
    }
  
  pi_fail_ok = PI_OK;
  return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );



  call_ptr = (struct cisam_cl *)req_buf;

  call_ptr->fleet = pGetCalls->Fleet;
  strcpy(call_ptr->charge_nbr, pGetCalls->ID);
  call_ptr->due_date_time = 0; //time(0) - (24*60*60);
  mode = ISGTEQ;
  while ( (db(CALL_FILE_ID, READ_KEYS, &cl_key8, mode, 0)) == 0 )
    {
      mode = ISNEXT;
      if ( strncmp( call_ptr->charge_nbr, pGetCalls->ID, strlen(call_ptr->charge_nbr) ) )
	break;

      if ( call_ptr->pri_status <= pGetCalls->status )
	{
	  cur_call[nbr_calls].fleet = call_ptr->fleet;
	  cur_call[nbr_calls].call_number = call_ptr->nbr;
	  cur_call[nbr_calls].priority = call_ptr->pri_status;
	  cur_call[nbr_calls].call_group = call_ptr->grouped_with;
	  if ( call_ptr->pos_in_group == ' ' )
	    cur_call[nbr_calls].call_position = 0;
	  else
	    cur_call[nbr_calls].call_position = call_ptr->pos_in_group - '0';
	  cur_call[nbr_calls].number_of_calls = call_ptr->nbr_in_group;
	  if ( call_ptr->nbr_in_group == ' ' )
	    cur_call[nbr_calls].number_of_calls = 0;
	  else
	    cur_call[nbr_calls].number_of_calls = call_ptr->nbr_in_group - '0';
	  cur_call[nbr_calls].number_of_vehicles = call_ptr->nbr_of_vehs - '0';
	  strncpy( cur_call[nbr_calls].call_type, call_ptr->extended_type, 5 );
	  strcpy( cur_call[nbr_calls].from_addr_street, call_ptr->pckup_str_name );
	  cur_call[nbr_calls].from_addr_number = call_ptr->pckup_str_nbr;
	  cur_call[nbr_calls].from_addr_suffix = call_ptr->pckup_str_suf;
	  strcpy( cur_call[nbr_calls].from_addr_apart, call_ptr->pckup_apt_room );
	  strcpy( cur_call[nbr_calls].from_addr_city, call_ptr->pckup_city );
	  cur_call[nbr_calls].from_addr_zone = call_ptr->pckup_zone;
	  strcpy( cur_call[nbr_calls].from_addr_cmnt, call_ptr->pckup_adr_cmnt );
	  strcpy( cur_call[nbr_calls].passenger, call_ptr->passenger_name );
	  strncpy( cur_call[nbr_calls].phone, call_ptr->phone_number, 10 );
	  strcpy( cur_call[nbr_calls].to_addr_street, call_ptr->dest_str_name );
	  cur_call[nbr_calls].to_addr_number = call_ptr->dest_str_nbr;
	  cur_call[nbr_calls].to_addr_suffix = call_ptr->dest_str_suf;
	  strcpy( cur_call[nbr_calls].to_addr_apart, call_ptr->dest_apt_room );
	  strcpy( cur_call[nbr_calls].to_addr_city, call_ptr->dest_city );
	  cur_call[nbr_calls].to_addr_zone = call_ptr->dest_zone;
	  strcpy( cur_call[nbr_calls].to_addr_cmnt, call_ptr->dest_adr_cmnt );
	  cur_call[nbr_calls].car_number = call_ptr->veh_nbr;
	  strncpy( cur_call[nbr_calls].car_attrib, call_ptr->veh_attr, 8 );
	  sprintf( cur_call[nbr_calls].driver_id, "%5.5d", call_ptr->drv_id );
	  strncpy( cur_call[nbr_calls].driver_attrib, call_ptr->drv_attr, 8 );
	  cur_call[nbr_calls].creator = call_ptr->entered_by;
	  strncpy( cur_call[nbr_calls].creation_date, call_ptr->date_in, 5 );
	  strncpy( cur_call[nbr_calls].creation_time, call_ptr->time_in, 6 );
	  strncpy( cur_call[nbr_calls].due_date, call_ptr->due_date, 5 );
	  strncpy( cur_call[nbr_calls].due_time, call_ptr->due_time, 6 );
	  strncpy( cur_call[nbr_calls].pickup_time, call_ptr->pckup_time, 6 );
	  strncpy( cur_call[nbr_calls].close_time, call_ptr->close_time, 6 );
	  
	  strcpy( cur_call[nbr_calls].call_comment, call_ptr->general_cmnt );
	  cur_call[nbr_calls].call_status = call_status_conv( call_ptr->status, strlen( call_ptr->status ));
	  if (nbr_calls > 49)
	    break;
	  ++nbr_calls;
	}

    }
  pGetCalls->nbr_of_calls = nbr_calls;
  return_code = ins_to_data_blk( sizeof(PI_GET_CALLS_REC), &position, (char *)pGetCalls, &return_data->data_len );

  for ( i = nbr_calls; i--; i >= 0 )
    {
      return_code = ins_to_data_blk( sizeof( struct call ), &position, (char *) &cur_call[i], &return_data->data_len );
    }

  return(return_code);
}

  
int pi_account_validate( struct order_struc *order_data, struct return_struc *return_data )
{
  int       y = 0, return_code;
  char      *position;
  short     pi_fail_ok;
  PI_ACCOUNT_VALIDATE_REC *pAccountValid;
  time_t    now_time;
  struct tm *tm_ptr;
  int       account_number;
  struct cisam_ac	                 account;
  
  bzero( &account, sizeof( struct cisam_ac ) );
  
  position = return_data->data;
  return_data->data_len = 0;

  pAccountValid = (PI_ACCOUNT_VALIDATE_REC *)order_data->data;

  return_code = d1000_open_db_files();

  account_number = atoi(pAccountValid->ID);

  if ( d2000_get_account(account_number, &account, pAccountValid->Fleet) == FALSE )
    {
      return_data->data_len = 0;
      position = return_data->data;
      pi_fail_ok = PI_INV_ACCOUNT;
      return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
      return( return_code );
    }      

  strcpy(pAccountValid->Password, account.password);
  strcpy(pAccountValid->Name, account.name);
  sprintf(pAccountValid->StreetNbr, "%d", account.str_nbr);
  pAccountValid->StreetSuffix = account.str_suf;
  strcpy(pAccountValid->StreetName, account.str_name);
  strcpy(pAccountValid->Apart, account.apt_room);
  strcpy(pAccountValid->City, account.city);
  strcpy(pAccountValid->Phone, account.phone_number);
  strcpy(pAccountValid->Attention, account.attention);
  strcpy(pAccountValid->Comment, account.comment);
  strcpy(pAccountValid->DriverMsg, account.driver_msg);
  
  return_code = ins_to_data_blk( sizeof( PI_ACCOUNT_VALIDATE_REC ), &position, (char *)pAccountValid, &return_data->data_len );

  return( return_code );
}


  
  
int pi_emerg_picture( struct order_struc *order_data, struct return_struc *return_data )
{
  int       y = 0, return_code;
  char      *position;
  short     pi_fail_ok;
  char      sPath[1024];
  FILE      *fpEmergPicture;  
  PI_EMERG_PICTURE_REC *pEmergPicture;
  time_t    nowtime;
  struct tm *tm_ptr;
  double    dLat = 0.0;
  double    dLong = 0.0;
  int       iMobileID = 0, iTime = 0;
  int       iMsgLength = 0;
  char      cEmerg, cCamTrigger, cGPSStatus, cYear, cMonth, cDay;
  struct excpt_rec except_fifo;	/* for writing except resolution record to FIFO */
  VEH_REC  veh_rec;
  TL_STRUCT tl_struct;
  int       iVehNbr = 0;
  char      sVehNbr[16];
  char      sBufMsg[1024];
  double    TaxiX = 0.0, TaxiY = 0.0;
  struct excepts *except_ptr[ NUM_OF_EXCEPTIONS * 10 ];
  int num_of_pck_exc = 0;	/* number_of_exceptions picked.		*/
  short counter;
  
  bzero( &except_fifo, sizeof(struct excpt_rec) );
  position = return_data->data;
  return_data->data_len = 0;  

  MakeLogEntry("entering pi_emerg_picture()");
  memcpy( &iMobileID, (char *)(order_data->data+2), sizeof(int) );
  memcpy( &iTime, (char *)(order_data->data+22), sizeof(int) );
  memcpy( &dLat, (char *)(order_data->data+6), sizeof(double) );
  memcpy( &dLong, (char *)(order_data->data+14), sizeof(double) );
  cEmerg = order_data->data[0];
  cCamTrigger = order_data->data[1];
  cGPSStatus = order_data->data[26];
  strncpy( sVehNbr, (char *)(order_data->data + 30), 11 );
  iVehNbr = atoi(sVehNbr);
  cYear = order_data->data[27];
  cMonth = order_data->data[28];
  cDay = order_data->data[29];

  //  sprintf(sBufMsg,"%d %f %f %c %c %s %c-%c-%c", iTime, dLat, dLong, cEmerg, cCamTrigger, cGPSStatus, sVehNbr, cYear, cMonth, cDay);
  //  MakeLogEntry(sBufMsg);
  
  LatLong_to_UTM( dLat, dLong, &TaxiY, &TaxiX );
  
  pEmergPicture = (PI_EMERG_PICTURE_REC *) order_data->data;

  // Build image filename
  nowtime = (time_t)time((time_t *)0);
  tm_ptr = localtime(&nowtime);
  sprintf( sPath, "/usr/taxi/run/images/%d-%02d%02d%02d-%02d%02d%02d.jpg",
           iMobileID, tm_ptr->tm_year % 100, tm_ptr->tm_mon + 1, tm_ptr->tm_mday,
           tm_ptr->tm_hour, tm_ptr->tm_min, tm_ptr->tm_sec);

  sprintf(sBufMsg,"Image filename: %s", sPath);
  MakeLogEntry(sBufMsg);
  
  if ( ( fpEmergPicture = fopen( sPath, "w" ) ) == NULL )
    {
      printf("Unable to open Emergency Picture File\n");
      MakeLogEntry("Unable to open Emegency Picture File");
      pi_fail_ok = PI_DB_OP_FAIL;
      return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );            
    }
  else
    {
      fwrite( (void *)(order_data->data+41), sizeof(char), order_data->data_len, fpEmergPicture ); //order_data->data_len , fpEmergPicture );
      fclose( fpEmergPicture );
      pi_fail_ok = PI_OK;
      return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
    }
  
  //Build text file to store other data
  sprintf( sPath, "/usr/taxi/run/images/%d-%02d%02d%02d-%02d%02d%02d.txt",
           iMobileID, tm_ptr->tm_year % 100, tm_ptr->tm_mon + 1, tm_ptr->tm_mday,
           tm_ptr->tm_hour, tm_ptr->tm_min, tm_ptr->tm_sec);

  if ( ( fpEmergPicture = fopen( sPath, "w" ) ) == NULL )
    {
      printf("Unable to open Emergency Data File\n");
      pi_fail_ok = PI_DB_OP_FAIL;
      return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );            
    }
  else
    {
//      fprintf( fpEmergPicture, "MobileID %d\nVehicle ID %d\nEmerg. State %d\nCamera Trigger %d\nGPS Status %d\nLatitude %4.5f (%8.2f)\nLongitude %4.5f (%8.2f)\nGPS Time %d\n\Date %d-%d-%d\n",
//	       0, 0, 0, 0,
//	       0, 0.0, 0.0, 0.0, 0.0, 0, 0, 0, 0 );
      
            fprintf( fpEmergPicture, "MobileID %d\nVehicle ID %d\nEmerg. State %d\nCamera Trigger %d\nGPS Status %d\nLatitude %4.5f (%8.2f)\nLongitude %4.5f (%8.2f)\nGPS Time %d\n\Date %d-%d-%d\n",
                     iMobileID, iVehNbr, cEmerg, cCamTrigger,
                     cGPSStatus, dLat, TaxiY, dLong, TaxiX, iTime, (int)cYear, (int)cMonth, (int)cDay );
      fclose( fpEmergPicture );
    }

  // Update Vehicle database record

  bzero(&veh_rec, sizeof(VEH_REC));
  veh_rec.fleet = 'H';
  sprintf(veh_rec.mobile_rf_id, "%05x", iMobileID);
  if ( db_read_key(VEHICLE_FILE_ID, &veh_rec, &veh_key1, ISEQUAL) == SUCCESS )
    {
            veh_rec.gps_lat = TaxiY;
            veh_rec.gps_long = TaxiX;
            db_update( VEHICLE_FILE_ID, &veh_rec );
    }  

  db_open( TL_FILE_ID, ISINOUT + ISMANULOCK );
  bzero( &tl_struct, sizeof( TL_STRUCT ) );
  tl_struct.date_time = time(0);
  tl_struct.fleet = 'H';
  tl_struct.lat_y = TaxiY;
  tl_struct.long_x = TaxiX;
  tl_struct.veh_nbr = veh_rec.nbr;
  get_pkup_date( tl_struct.date );
  get_pkup_time( tl_struct.time );
  tl_struct.receiver_status = 'G';
  
  db_write(TL_FILE_ID, &tl_struct);
  
  db_close(TL_FILE_ID);
  
  // If emergency status is set to '0' don't alert TaxiPak
  if ( cEmerg == 0 )
    return( return_code );

  num_of_pck_exc = get_excepts(except_ptr, 1, 0, 0);
  if ( num_of_pck_exc > 0 ) // got some emergencies to look at
    {
      for ( counter = 0; counter < num_of_pck_exc; counter++ )
	{
	  if ( except_ptr[counter]->veh_nbr == veh_rec.nbr )
	    return(return_code);
	}
    }
  
  // Notify Dispatch of the Emergency
  except_fifo.u_prcs_id = pid;
  except_fifo.rec_type = EXCEPT_REQ;
  except_fifo.except_ptr = '\0';
  except_fifo.except_nbr = 0;
  except_fifo.resolve_code = R_EMERG;
  except_fifo.call_nbr = 0;
  except_fifo.grouped_with = 0;
  except_fifo.veh_nbr = veh_rec.nbr;
  except_fifo.zone_nbr = 0;
  except_fifo.old_zone_nbr = 0;
  except_fifo.old_due_time = 0;
  
  if (write(fifo_fd,&except_fifo,sizeof(struct excpt_rec)) == -1)	/* Write FIFO to dispatch */
    {
      ERROR("Error writing to FIFO.");
    }

  MakeLogEntry("exiting pi_emerg_picture()");
  
  return( return_code );
}

int pi_zone_info( struct order_struc *order_data, struct return_struc *return_data )
{
  int       y = 0, return_code;
  char      *position;
  short     pi_fail_ok;
  ZONE_INFO_REC *pZoneInfo;
  struct zone_rqst zrq;						/* structure for requesting zone detail info */
  ZONE_RESP  *zrp;
  ZONE_RESP_HNDL hZoneResp;
  int rc, times_thru, i;  

  bzero( &zrq, sizeof( struct zone_rqst ) );
  position = return_data->data;
  return_data->data_len = 0;  
  pi_fail_ok = PI_OK;
  return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );  
  pZoneInfo = ( ZONE_INFO_REC * ) order_data->data;

  
  /* check if fleet is active and zone exists */
  for ( y = 0; y < FLEET_MAX; y++ )
    if ( fleet[y]->fleet_id == pZoneInfo->fleet )
      break;

  if ( y == FLEET_MAX )
    {
      return_data->data_len = 0;
      position = return_data->data;      
      pi_fail_ok = PI_INVALID_FLEET;
      return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
      return( return_code );
    }

  if ( ( fleet[y]->active != YES ) || ( fleet[y]->zone_ptr[pZoneInfo->zone_nbr] == NULL ) )
    {
      return_data->data_len = 0;
      position = return_data->data;
      ERROR( "Bad fleet/zone combo" );
      pi_fail_ok = PI_INV_ZONE;
      return_code = ins_to_data_blk( PI_SHORT_TYPE, &position, (char *) &pi_fail_ok, &return_data->data_len );
      return( return_code );
    }

  zrq.d_prcs_id = DISP_DEST;						/* send ipc to Dispatch */
  zrq.u_prcs_id = pid;						/* user's process id */
  zrq.rec_type = PI_ZONE_INFO_REQ;						/* zone request type */
  zrq.fleet = pZoneInfo->fleet;
  zrq.zone_nbr = pZoneInfo->zone_nbr;
  for ( i = 0; i < ATTR_MAX; i++ )
    {
      if ( pZoneInfo->veh_attr[i] == YES )
	set_bit( i, sizeof(zrq.veh_attr), &zrq.veh_attr );
    }
  for ( i = 0; i < ATTR_MAX; i++ )
    {
      if ( pZoneInfo->drv_attr[i] == YES )
	set_bit( i, sizeof(zrq.drv_attr), &zrq.drv_attr );
    }    

  msgsnd(msgkey,(struct msgbuf *)&zrq,sizeof(struct zone_rqst), IPC_NOWAIT);
  
  signal(SIGALRM, alarm_noop );
  
  hZoneResp = (ZONE_RESP_HNDL)malloc( sizeof(ZONE_RESP) );
  zrp = ( ZONE_RESP * )hZoneResp;
  times_thru = 0;
  while (times_thru <=2 ) {

    alarm(5);
    
    rc = msgrcv(msgkey,(void *)zrp,sizeof(ZONE_RESP),(long) pid,0);	/* receive responses until correct one */
    
    if (rc == -1) {
      
      /* If error not due to caught signal */
      if (errno != EINTR) {
	free( hZoneResp );
	alarm(0);
	return(0);
      }
      
      /* if error was a caught signal (expecting alarm for status line display) allow
       * two such errors (meaning a timeout of 1 to 2 minutes since the alarm comes
       * once a minute.) */
      
      if (++times_thru == 2) {
	alarm(0);
	free( hZoneResp );
	return(0);
      }
      
    }
    
    /* Found right response, exit loop */
    else if (zrp->rec_type == ZONE_REQ)
      break;
    
    /* Invalid response */
    else {
      free( hZoneResp );
      alarm(0);
      return(0);
    }
  }
  
  alarm(0);
  
  pZoneInfo->veh_primary_zone = zrp->veh_posted;
  pZoneInfo->veh_backup_zones = 0;
  pZoneInfo->calls_unassigned = 0;
  for ( i = 0; i < BCK_IN_ZN_RESP; i++ )
    {
      pZoneInfo->veh_backup_zones += zrp->bkup_zones[i][2];
      pZoneInfo->calls_unassigned += zrp->bkup_zones[i][1];
    }
  
  pZoneInfo->calls_unassigned += zrp->unassn_calls;
  
  return_code = ins_to_data_blk( sizeof( ZONE_INFO_REC ), &position, (char *)pZoneInfo, &return_data->data_len );

  free( hZoneResp );
  
  return( return_code );
}

CityNavGetCoords( struct cisam_cl *pCall )
{
  CNargptr        mydata;
  int             PckupStrIndex;
  char            sCity[16];
  int             i, err_code;
  int             adr_index = 0;  


  /* coordinates were set previously in Tatort2000 tables */
  if ( pCall->gps_lat > 0.0 && pCall->gps_long > 0.0 )
    return(0);  
  
  PckupStrIndex = AddressIndexFromStreet( &pCall->pckup_str_nbr,
					  pCall->pckup_str_type,
					  pCall->pckup_str_name,
					  pCall->pckup_pre_dir,
					  pCall->pckup_post_dir,
					  pCall->pckup_city,
					  pCall->pckup_pmap,
					  &pCall->gps_long,
					  &pCall->gps_lat, &adr_index);



  return( 0 );
}

/**
  Takes data from the global 'call_struct' and returns the
  street index from the Address Table
**/
int
AddressIndexFromStreet( int  *str_nbr, char *str_type, char *str_name,
		        char *pre_dir, char *post_dir,
		        char *city, char *map_page_coords,
			double *pGPS_x, double *pGPS_y, int *adr_index )
{
  int  x;
  short ccode = 0;
  short odd_address = 0;
  char  sn_type = ' ';
  struct cisam_sn   strname;
  struct cisam_ad   address;
  int   s_idx;

  bzero( &strname, sizeof( struct cisam_sn ) );
  bzero( &address, sizeof( struct cisam_ad ) );
  
  strncpy(strname.name,str_name,20);
  strncpy(strname.str_type,str_type,3);
  if (db_read_key(STRNAME_FILE_ID, &strname, &sn_key2,ISEQUAL)  == SUCCESS) {		/* read street name record */
    sn_type = strname.rec_type;					/* save record type */
    s_idx = strname.index;						/* save index */
    address.str_index = s_idx;					/* fill in address key */
    strncpy(address.str_pre_dir,pre_dir,2);
    strncpy(address.str_post_dir,post_dir,2);
    strncpy(address.str_city,city,3);
    address.high_nbr = *str_nbr;
    
    if (*str_nbr/2 == (*str_nbr+1)/2)					/* even address ? */
      odd_address = 0;
    else
      odd_address = 1;					/* address is odd */
    
    ccode = db_read_key(ADDRESS_FILE_ID,&address,&ad_key1,ISGTEQ); 	/* read an address record */
    while (ccode == SUCCESS){						/* more records to read */
      if ((s_idx != address.str_index) || (strncmp(pre_dir,address.str_pre_dir,2) != 0) ||	/* in range? */
	  (strncmp(post_dir,address.str_post_dir,2) != 0) || (strncmp(city,address.str_city,3) != 0)) 
	return(-1);
      
      if (odd_address && (address.odd_even == ADDRESS_ODD || address.odd_even == ADDRESS_BOTH))
	break;						/* found appropriate range */
      
      if (!odd_address && (address.odd_even == ADDRESS_EVEN || address.odd_even == ADDRESS_BOTH))
	break;						/* found appropriate range */
      
      ccode = db_read_key(ADDRESS_FILE_ID,&address,&ad_key1,ISNEXT); 	/* read next ddress record */
    }
    
    if (ccode != SUCCESS)
      return(-1);						/* no record found */
    
    if ((address.low_nbr <= *str_nbr) && (address.high_nbr >= *str_nbr))
      /* is it in range? */
      {
	sprintf( map_page_coords, "%3d %4s", address.map_ref_page, address.map_ref_block );
	if ( (address.str_x_coord > 0 ) && (address.str_y_coord > 0) )
	  {
	    *pGPS_x = (double)address.str_x_coord;
	    *pGPS_y = (double)address.str_y_coord;
	  }
	else
	  {
	    *pGPS_x = 0;
	    *pGPS_y = 0;
	  }
	return( address.str_index );
      }
      
    return(-1);								/* address not in range */
  }
  return(-1);									/* no street name record */
}

void alarm_noop()
{
}

void LatLong_to_UTM( double Lat, double Long, double *pNorthing, double *pEasting )
{
  int   iLong;
  double a = 6378137.0;
  double eccSquared = 0.00669439;
  double k0 = 0.9996013;

  double LongOrigin;
  double eccPrimeSquared;
  double N, T, C, A, M;



  if ( !strcmp(LANGUAGE, "Danish") )
    LongOrigin = 9.0;
  else
    LongOrigin = 27; // HTD map setting 
  //LongOrigin = (int)(Long/6)*6 + 3*(int)(Long/6)/abs((int)(Long/6));
  
  eccPrimeSquared = (eccSquared)/(1.0-eccSquared);

  N = a/sqrt(1.0-eccSquared*sin(Lat*deg2rad)*sin(Lat*deg2rad));
  T = tan(Lat*deg2rad)*tan(Lat*deg2rad);
  C = eccPrimeSquared*cos(Lat*deg2rad)*cos(Lat*deg2rad);
  A = cos(Lat*deg2rad)*(Long-LongOrigin)*deg2rad;

  M = 111132.9520 * Lat - 16038.5200*sin(2.000*Lat*deg2rad) +
      16.8300*sin(4.0*Lat*deg2rad) - 0.0200*sin(6.000*Lat*deg2rad);

  *pEasting = k0*N*(A+(1.0-T+C)*A*A*A/6.0
	            + (5.0-18.0*T+T*T+72.0*C-58.0*eccPrimeSquared)*A*A*A*A*A/120.0)
                    + 500000.0;
  *pNorthing = k0*(M+N*tan(Lat*deg2rad)*(A*A/2.0+(5.0-T+9.0*C+4.0*C*C)*A*A*A*A/24.0
					 +(61.0-58.0*T+T*T+600.0*C-330.0*eccPrimeSquared)*A*A*A*A*A*A/720.0));
}


short GetCustomerZone( int fl_nbr, double cust_x, double cust_y )
{
  MEM_ZONE *pZone;
  POINT_POLY cust_point, p1, p2;
  POINT_POLY *zone_poly;
  int  nbr_points, i, j;
  double xinters;
  int counter = 0;

  cust_point.x = cust_x;
  cust_point.y = cust_y;
  
  for ( j = 1; j < ZONE_MAX; j++ )
    {
      if ( ( pZone = fleetMemZones[fl_nbr][j] ) != HNDL_NULL )
	{
	  zone_poly = (POINT_POLY *)pZone->zone_poly;
	  p1 = zone_poly[0];
	  nbr_points = pZone->poly_points;
	  
	  for ( i = 1; i <= nbr_points; i++ )
	    {
	      p2 = zone_poly[i % nbr_points];
	      if ( cust_point.y > MIN(p1.y,p2.y) )
		{
		  if ( cust_point.y <= MAX(p1.y,p2.y) )
		    {
		      if ( cust_point.x <= MAX(p1.x,p2.x) )
			{
			  if ( p1.y != p2.y )
			    {
			      xinters = (cust_point.y - p1.y)*(p2.x-p1.x)/(p2.y-p1.y)+p1.x;
			      if ( p1.x == p2.x || cust_point.x <= xinters )
				counter++;
			    }
			}
		    }
		}
	      p1 = p2;
	    }
	  if ( counter % 2 == 0 )
	    continue;
	  else
	    return(j); 
	}
    }
      
  return 0;
}
ZonePolyCalculateCentroid( MEM_ZONE *zone_ptr )
{
  int   N, i;
  double x, y;
  double running_sumx, running_sumy;
  POINT_POLY  *zone_poly;

  N = zone_ptr->poly_points;
  zone_poly = (POINT_POLY *) zone_ptr->zone_poly;
  running_sumx = 0.0;
  running_sumy = 0.0;
  
  for ( i = 0; i < N; i++ )
    {
      running_sumx += zone_poly[i].x;
      running_sumy += zone_poly[i].y;
    }

  x = running_sumx / N;
  y = running_sumy / N;

  zone_ptr->poly_centroid_x = x;
  zone_ptr->poly_centroid_y = y;
  
}


