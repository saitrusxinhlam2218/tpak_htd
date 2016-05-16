static char sz__FILE__[]="@(#)ab_map.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  ab_map.c; Rel: 6.3.0.0; Get date: 10/9/92 at 13:28:34
 * @(#)  Last delta: 3/21/91 at 11:00:22
 * @(#)  SCCS File: /taxi/sccs/s.ab_map.c
 *                                                                           *
 * Copyright (c) 1990 - MDI Seattle                                          *
 *                                                                           *
 *****************************************************************************/

/*---------------------------------------------------------
 *	ab map.c
 *---------------------------------------------------------
 */

#include <sys/types.h>
#include <sys/ipc.h>
#include <time.h>
#include <pwd.h>
#include <signal.h>
#include <errno.h>
#include <nl_types.h>
#include <locale.h>
/*
 *	taxi includes
 */
#define DECLARING
#include "comm_strdefs.h"
#include "ui_strdefs.h"

#include "path.h"
#include "mads_types.h"
#include "mads_isam.h"
#include "mad_error.h"
#include "mad_ipc.h"
#include "zgeo.h"
#include "writer.h"
#include "ui_ipc.h"
#include "language.h"
#include "veh_driv.h"
#include "calls.h"
#include "Call_private.h"
#include "pi_struc.h"
#include "enhance.h"
#include "switch_ext.h"
#include "language.h"
/*
 *	ab includes
 */
#include "pi_account_defines.h"

struct order_struc {
  int  data_len;
  char data[ 4096 ];
};

typedef struct order_struc order;

/*
 *	globals
 */
int				 	pipe_fd,
					msgkey,
					pid,
					port_fd;
short				autobook_uid;
char				line_fleets[AB_LINE_MAX]; 
short				debug_level = 0;
short				check_sum = TRUE;
short				check_zone = TRUE;

/*
 *	externs
 */
extern int fifo_fd;
extern struct fleets *fleet[];						/* pointer to fleet array 		*/
extern char szLogMsg[];


/*--------------------------------------------------------
 *==>	m0100 cleanup autobook
 *--------------------------------------------------------
 */
/*--------------------------------------------------------
 *==>	m2000 process message
 *--------------------------------------------------------
 */

m2000_process_message(fd, ab_message, pcall_nbr)
	int				 fd;
	struct order_struc		 *ab_message;
        int                              *pcall_nbr;
{
	int				 line_number,
					 account_number;
	int	msg;
	struct cisam_ac	                 account, tmp_account;
	ACCT_DATA_REC                    *account_order;
	struct cisam_cust                CustRec;


        bzero( &account, sizeof( struct cisam_ac ) );
        
	account_order = ( ACCT_DATA_REC * ) ab_message->data;
	
	account_number = atoi(account_order->acct_nbr);

        if ( d2000_get_account(account_number, &account, 
			       account_order->fleet) == FALSE )
                  {
                    
                    return(PI_INV_ACCOUNT);
                  }


        memcpy( &tmp_account, &account, sizeof( struct cisam_ac ) );
	//bzero( &CustRec, sizeof( struct cisam_cust ) );
	//strcpy( CustRec.phone_nbr, account_order->phone_nbr );
	
	//if ( db_read_key(CUSTOMER_FILE_ID, &CustRec, &cust_key1, ISEQUAL ) != SUCCESS )
	//  return( FAIL );
        m4000_dispatch_call(fd, &account,account_order, pcall_nbr);
	if ( *pcall_nbr > 0 )
	  {
		return(0);
	  }


	return(PI_INV_ACCOUNT);
}




/*--------------------------------------------------------
 *==>	m4000 dispatch call
 *--------------------------------------------------------
 */
int
m4000_dispatch_call(fd, account, account_order, pdb_record_num)
  int				 fd;
struct cisam_ac *account;
ACCT_DATA_REC  *account_order;
int              *pdb_record_num;
{
	struct cisam_cl	 db_call;
	struct call_ipc_rec	 dispatch_call;
	struct cisam_ch	 call_history;
	struct zone_rqst zone_request;
	struct zone_resp zone_response;
	int rc,times_thru;
	int				 zip;


        LogMsg( 15, EXTP_TO_PI, (void *)account_order );
/*
 *	write call to db
 */
	if (!m4100_prepare_call_for_db(account, account_order, &db_call))
	  {
		ERROR(' ', "", "AB Error: zoning call record");
		return(AB_INVALID_ACCOUNT_MSG);
	  }


	
	if (!(*pdb_record_num = d3000_write_call_to_db(&db_call)))
	  {
		ERROR(' ', "", "AB Error: writing call record to DB");
		return(AB_INVALID_ACCOUNT_MSG);
	  }

	if (m4300_prepare_call_messages(&db_call) == SUCCESS )
	  {
	    db_call.call_msg_nbr = '1';
	    m4400_update_call_record( &db_call );
	  }
	else
	  db_call.call_msg_nbr = '0';
	
	if (!m4100_prepare_call_history_for_db(&db_call, &call_history))
	  {
		ERROR(' ', "", "AB Error: preparing call history record");
		return(AB_INVALID_ACCOUNT_MSG);
	  }


	
	if (!(d3100_write_call_history_to_db(&call_history)))
	  {
		ERROR(' ', "", "AB Error: writing call history record");
		return(AB_INVALID_ACCOUNT_MSG);
	  }

/*
 *	send call to dispatch
 */
	if (!m4200_prepare_call_for_dispatch(&db_call, &dispatch_call, *pdb_record_num, account_order))
	  {
		ERROR(' ', "", "AB Error: preparing call for dispatch");
		return(AB_INVALID_ACCOUNT_MSG);
	  }

	if ( ( rc = write( fifo_fd, &dispatch_call, sizeof( struct call_ipc_rec ) ) ) == -1 )
	  {
	    ERROR(' ', "", "Error writing user pipe");
	    return(FALSE);
	  }
	
	if (DEBUG(AB_DBX_MESSAGES))
		printf("--AB says: dispatched call\n");

	return(0);
}


/*--------------------------------------------------------
 *==>	m4100 prepare call for db
 *--------------------------------------------------------
 */

m4100_prepare_call_for_db(account, account_order, call)
  struct cisam_ac *account;
  ACCT_DATA_REC	*account_order;
  struct cisam_cl	*call;
{
	int				 zip,
					 zone,
					 i;
	long			 t1000_get_local_time();
	int			cl_length, dr_length;
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
        double x_coord = 0.0, y_coord = 0.0;
	char sLocale[32];
        
	memset((char *)call, '\0', sizeof(*call));

	call->fleet = account_order->fleet;
	call->nbr_of_vehs = '1';
	call->entered_by = atoi(account_order->creator);

        //        if ( atoi( account->nbr ) > 0 )
        //          m9200_set_ab_call_type(account->account_charge_enabled, call->extended_type);
        //        else
        //          sprintf(call->extended_type, "%c%c %c", PASS, ' ', ' ');

        strncpy( call->extended_type, account_order->call_type, 5 );
        call->extended_type[5] = '\0';
        
	call->ext_nbr = -1;
	
	strncpy(call->company_name, account->name, sizeof(call->company_name));

        if ( strlen(account_order->from_addr_number ) )
          {
            call->pckup_str_nbr = atoi(account_order->from_addr_number);
            call->pckup_str_suf = account_order->from_addr_suffix;
          }
        else
          {
            call->pckup_str_nbr = account->str_nbr;
            call->pckup_str_suf = account->str_suf;
          }

        if ( strlen(account_order->from_addr_street) )
          {
            strtoupper(account_order->from_addr_street);
            strcpy(call->pckup_str_name, (account_order->from_addr_street));
            strncpy(call->pckup_str_key, (account_order->from_addr_street), 4);
            call->pckup_str_key[4] = '\0';
            strtoupper(account_order->from_addr_apart);
            strcpy(call->pckup_apt_room, (account_order->from_addr_apart));
            strtoupper(account_order->from_addr_city);
            strcpy(call->pckup_city, (account_order->from_addr_city));
	    strtoupper(account_order->from_addr_cmnt);
	    strcpy(call->pckup_adr_cmnt, account_order->from_addr_cmnt);
            strtoupper(account_order->call_comment);
            strcpy(call->general_cmnt, (account_order->call_comment));
            strtoupper(account_order->passenger);
            strcpy(call->passenger_name, (account_order->passenger));
            strncpy(call->pass_name_key, (account_order->passenger), 4);
            call->pass_name_key[4] = '\0';
            strtoupper(account_order->requested_by);
            strcpy(call->requested_by, account_order->requested_by);
            strcpy(call->phone_area, account->phone_area);
            strncpy(call->phone_number, account_order->phone, 10);
            strcpy(call->phone_ext, account->phone_ext);            
          }
        else
          {
            strcpy(call->pckup_str_name, (account->str_name));
            strncpy(call->pckup_str_key, (account->str_name), 3);
            strcpy(call->pckup_apt_room, (account->apt_room));
            strcpy(call->pckup_city, (account->city));
            //            strcpy(call->passenger_name, (account->passenger));
            strcpy(call->phone_number, account->phone_contact);
          }

	call->pckup_str_key[4] = '\0';



        // destination address
        strtoupper(account_order->to_addr_street);
        strcpy(call->dest_str_name, account_order->to_addr_street);
        call->dest_str_nbr = atoi(account_order->to_addr_number);
        call->dest_str_suf = account_order->to_addr_suffix;
        strcpy(call->dest_city, (account_order->to_addr_city));

	account_order->priority[2] = '\0';
        if (strlen(account_order->priority))
          call->priority = atoi(account_order->priority);
        else
          call->priority = account->priority;
        
	strncpy(call->driver_msg, account->driver_msg, sizeof(call->driver_msg));
	/* Added the lines to add the driver_msg to the general comment for account trips to fix TC00034 */
        dr_length = strlen(call->driver_msg);                     /* string length of driver_msg comment */
        cl_length = strlen(call->general_cmnt);                   /* string length of general comment */
        if (cl_length > 0 && (cl_length + dr_length) <= 65)      /* do we need to or can we shuffle comments around? */
	{
            strcat(call->general_cmnt," /");
            strcat(call->general_cmnt,call->driver_msg);
            strcat(call->general_cmnt,"/");
        }

	strncpy(call->charge_nbr, account->nbr, sizeof(call->charge_nbr));
	call->veh_attr_flag = NO;
	call->drv_attr_flag = NO;

	for (i = 0; i < 32; i++)
	  {
		if (account_order->car_attrib[i] == YES)
		  {
			call->veh_attr[i] = YES;
			call->veh_attr_flag = YES;
		  }
		else
		  call->veh_attr[i] = NO;
	  }
	for (i = 0; i < 32; i++)
	  {
		if (account_order->driver_attrib[i] == YES)
		  {
			call->drv_attr[i] = YES;
			call->drv_attr_flag = YES;
		  }
		else
		  call->drv_attr[i] = NO;
	  }

	// Set an Internet Booker attribute for HTD if originating there
	if ( ( 1 ) && ( ! strncmp(call->extended_type, "M  LX", 5) ) )
	  call->drv_attr[20] = YES;
	
        // set TaxiGEO coordinates
        account_order->gpsx[7] = '\0';
        account_order->gpsy[7] = '\0';
        call->gps_long = (double)atoi(account_order->gpsx);
        call->gps_lat = (double)atoi(account_order->gpsy);

	account_order->fare_amount[7] = '\0';
	account_order->vat[7] = '\0';
	if (strlen(account_order->fare_amount))
	  {
	    strcpy( sLocale, TPAK_LOCALE );
	    setlocale(LC_ALL, sLocale);
	    call->fare_amount = atof(account_order->fare_amount);
	    call->call_rate = atof(account_order->fare_amount);	    
	    setlocale(LC_ALL, "");
	}
	if (strlen(account_order->vat))
	  {
	    strcpy( sLocale, TPAK_LOCALE );
	    setlocale(LC_ALL, sLocale);	    
	    call->vat = atof(account_order->vat);
	    setlocale(LC_ALL, "");
	  }
/*
 *	since call is a now call
 */
        /* Get min/maximum values from fleet parameter
           to determine if call is future call or not */
        now_time = time(NULL);
        max_time = now_time + (long) (fleet[ call->fleet - 'A' ]->future_call_maximum);
        min_time = now_time + (long) (fleet[ call->fleet - 'A' ]->future_call_minimum);
        strftime( now_ascii_time, 7, "%H\%M", localtime( &now_time ));   /* The \ is added to prevent interpretaion of the SCCS system */
        strftime( now_ascii_date, 10, "%y%m%d", localtime( &now_time ));
        strftime( max_ascii_time, 7, "%H\%M", localtime( &max_time ));
        strftime( max_ascii_date, 10, "%y%m%d", localtime( &max_time ));
        strftime( min_ascii_time, 7, "%H\%M", localtime( &min_time ));
        strftime( min_ascii_date, 10, "%y%m%d", localtime( &min_time ));
        
	call->pri_status = call->priority;
        if ( get_tm_from_date_time( account_order->due_date, account_order->due_time ) <
             get_tm_from_date_time( min_ascii_date, min_ascii_time ) )
          future_call = 0;
        else
          future_call = 1;

        get_pkup_date(call->date_in);
        get_pkup_time(call->time_in);
        if ( strlen( account_order->due_date ) == 0 )
          future_call = 0;
        
        if ( !future_call )
          {
            strcpy(call->status, catgets(WRITER_catd, 1, 39, "UNASSGND"));
            /*Set premium attributes */
            call->drv_attr_flag = YES;
            call->drv_attr[25] = YES;
            call->veh_attr_flag = YES;
            call->veh_attr[25] = YES;
            get_pkup_date(call->due_date);
            get_pkup_time(call->due_time);
            call->due_date_time = t1000_get_local_time();
          }
        else
          {
            call->extended_type[TIMEC_TYPE_OFF] = TIMEC;
            call->pri_status = 63;
            strcpy( call->status, catgets(WRITER_catd, 1, 41, "PENDING") );
            call->due_date_time = (time_t)get_internal_time( account_order->due_date, account_order->due_time );
	    
	    /*Set premium attributes */
	    call->drv_attr_flag = YES;
	    call->drv_attr[25] = YES;
	    call->veh_attr_flag = YES;
	    call->veh_attr[25] = YES;
	    
            PI_to_TP_date( account_order->due_date, call->due_date );
            PI_to_TP_time( account_order->due_time, call->due_time );            
          }

        call->pckup_zone = atoi( account_order->from_addr_zone );
        if ( call->pckup_zone == 0 )
          {
            zone = zone_addr(call, PICKUP_ADDR, &zip);
            if (zone <= 0)
              {
		if (DEBUG(AB_DBX_MESSAGES))
                  printf("--AB says: couldn't zone call\n");
		return(FALSE);
              }
            call->pckup_zone = zone;
          }
        if ( call->gps_lat <= 0 || call->gps_long <= 0 )
          {
            if ( GetTatort2000( &x_coord, &y_coord, call, PICKUP_ADDR ) == SUCCESS )
              {
                call->gps_lat = y_coord;
                call->gps_long = x_coord;
              }
          }
            
        zone = zone_addr(call, DEST_ADDR, &zip);
        if ( zone > 0 )
          call->dest_zone = zone;
        

	z1000_get_zone_desc(call);

	if (DEBUG(AB_DBX_MESSAGES))
		printf("--AB says: zone = %d\n", zone);
	
	return(TRUE);
}


/*--------------------------------------------------------
 *==>	m4110 prepare call history for db
 *--------------------------------------------------------
 */

m4100_prepare_call_history_for_db(call, call_history)
	struct cisam_cl	*call;
	struct cisam_ch	*call_history;
{
	char			 date_str[10],
					 time_str[10];


	memset((char *)call_history, '\0', sizeof(*call_history));
	
	call_history->call_nbr		= call->nbr;
	call_history->exception_nbr	= 0;
	call_history->fleet			= call->fleet;
	call_history->event_type	= REGULAR_EVENT;
	call_history->status_dt_tm	= (int)time(NULL);
	call_history->veh_nbr		= 0;
	call_history->drv_id		= 0;
	call_history->user_id		= call->entered_by;

	get_pkup_date(date_str);
	get_pkup_time(time_str);
	strcpy(call_history->status_date, date_str);
	strcpy(call_history->status_time, time_str);

	strcpy(call_history->event_desc, catgets(WRITER_catd, 1, 42, "ENTERED" ) );

	return(TRUE);
}


/*--------------------------------------------------------
 *==>	m4200 prepare call for dispatch
 *--------------------------------------------------------
 */

m4200_prepare_call_for_dispatch(db_call, dispatch_call, db_record_num, pAccountOrder)
	struct cisam_cl	*db_call;
	struct call_ipc_rec	*dispatch_call;
	int				 db_record_num;
ACCT_DATA_REC           *pAccountOrder;
{
	int				 i,
					 pid;


	memset((char *)dispatch_call, '\0', sizeof(*dispatch_call));

	pid = getpid();

	dispatch_call->u_prcs_id 	= pid;
	dispatch_call->rec_type 	= CALL_REQ;
	dispatch_call->c_isam_nbr 	= db_record_num;
	dispatch_call->call_nbr		= db_call->nbr;
	dispatch_call->grouped_with	= db_call->grouped_with;
	dispatch_call->fleet		= db_call->fleet;
	dispatch_call->personal_veh	= db_call->personal_veh;
	dispatch_call->personal_rqst= db_call->personal_rqst;
	dispatch_call->pickup_zone	= db_call->pckup_zone;
	dispatch_call->dest_zone	= db_call->dest_zone;
	dispatch_call->priority		= db_call->priority;
	dispatch_call->due_time		= db_call->due_date_time;
	dispatch_call->msg_nbr 		= 0;
        dispatch_call->call_msg_nbr     = '1';
	dispatch_call->updated_call	= 0; /*make sure this value is zero as this is a new call*/
	dispatch_call->old_due_time	= 0; /*make sure this value is zero as this is a new call*/
        dispatch_call->FTJ_nbr          = -1;

	memset((char *)&dispatch_call->call_type, '\0', sizeof(struct call_types));
	dispatch_call->call_type.passenger	= TRUE;
	if ( !strcmp(db_call->status, catgets(WRITER_catd, 1, 41, "PENDING" ) ) )
	  dispatch_call->call_type.time = 1;

        
	if (db_call->extended_type[1] == '4')
	  dispatch_call->call_type.charge = TRUE;

	strcpy(dispatch_call->pick_pmap, db_call->pckup_pmap);

	memset((char *)&dispatch_call->veh_attr, '\0', 
					sizeof(struct veh_attributes));
	memset((char *)&dispatch_call->drv_attr, '\0', 
					sizeof(struct drv_attributes));

	for (i = 0; i < 32; i++)
	  {
            if ( pAccountOrder->car_attrib[i] == YES )
              //		if (db_call->veh_attr[i] == YES)
			m9000_set_bit(i, sizeof(struct veh_attributes), 
							&dispatch_call->veh_attr);
            if ( pAccountOrder->driver_attrib[i] == YES )
              //		if (db_call->drv_attr[i] == YES)
			m9000_set_bit(i, sizeof(struct drv_attributes), 
							&dispatch_call->drv_attr);
	  }

	return(TRUE);
}


/*--------------------------------------------------------
 *==>	m9000 set bit
 *
 *			this is identical to the ui/set_bit.c
 *			ui/set_bit.c was not modular enough to link in
 *				- reference to global call_struct
 *--------------------------------------------------------
 */

m9000_set_bit(bit_num, size, struct_ptr)
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


/*--------------------------------------------------------
 *==>	m9100 get uid from uname
 *--------------------------------------------------------
 */

m9100_get_uid_from_uname(uname)
	char		 	 uname[];
{
	struct passwd	*passwd_entry,
					*getpwnam();
	

	if (!(passwd_entry = getpwnam(uname)))
		return(FALSE);

	return(passwd_entry->pw_uid);
}


/*--------------------------------------------------------
 *==>	m9200 set ab call type
 *--------------------------------------------------------
 */

m9200_set_ab_call_type(charge_allowed, type)
        char                     charge_allowed;
	char		 	 type[];
{
  sprintf(type, "%c%c %c", PASS, ' ', CHARGE);
  return(TRUE);
}


/*--------------------------------------------------------
 *==>	m9300 set options
 *--------------------------------------------------------
 */

m9300_set_options(argc, argv)
	int				 argc;
	char			*argv[];
{
	char			 option;
	extern char		*optarg;
	extern int		 optind;
	int				 i, j;


	while ((option = getopt(argc, argv, "zcd:f:")) != -1)
	  {
		switch(option)
		  {
			case 'z':
				check_zone = FALSE;
				break;
			case 'c':
				check_sum = FALSE;
				break;
			case 'f':
				memset(&line_fleets[0], '\0', sizeof(line_fleets));
				for (i = 0; i < AB_LINE_MAX; i++)
				  {
					if (optarg[i] < 'A' || optarg[i] > 'Z')
						break;
					
					line_fleets[i] = optarg[i]; 
				  }

				for (j = i; j < AB_LINE_MAX; j++)
				  {
					line_fleets[j] = line_fleets[j - 1];
				  }
				break;

			case 'd':
				debug_level = atoi(optarg);
				break;

			default:
				break;
		  }
	  }

	if (!strlen(line_fleets))
	  {
		if (DEBUG(AB_DBX_MESSAGES))
			printf("--AB says: error no fleet specified\n");
		return(FALSE);
	  }

	return(TRUE);
}

int
m4300_prepare_call_messages( struct cisam_cl *pCall )
{

  struct cisam_cmsg  cl_msg;
  char               *pHash;
  char               FlatRate, AccountCall;
  
  bzero( &cl_msg, sizeof( struct cisam_cmsg ) );
  
  
  /** No call message generated if there is no matching pair in the METER_ACTION
      table OR the meter action text is empty **/

  
  return( SUCCESS );
}

m4400_update_call_record( struct cisam_cl *pCall )
{
  struct cisam_cl tmp_call_rec;
  
  if ( pCall == NULL )
    return( FAIL );

  bzero( &tmp_call_rec, sizeof( struct cisam_cl ) );
  tmp_call_rec.nbr = pCall->nbr;
  
  if ( db_read_key( CALL_FILE_ID, &tmp_call_rec, &cl_key1, ISEQUAL ) == SUCCESS )
    {
      tmp_call_rec.call_msg_nbr = pCall->call_msg_nbr;
      db_update( CALL_FILE_ID, &tmp_call_rec );
    }

  return( SUCCESS );
}

int strtoupper( unsigned char *pBuf )
{
    int c;
    
    if ( pBuf == NULL )
	return( -1 );

    while ( *pBuf != '\0' ) {
	c = *pBuf;
	*pBuf = toupper( c );
	++pBuf;
    }
}
