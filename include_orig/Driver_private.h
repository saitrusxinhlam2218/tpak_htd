/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Driver_private.h,v $
*  @(#)  $Revision: 1.4 $
*  @(#)  $Date: 2001/10/03 17:54:12 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Driver_private.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

#ifndef _DRIVER_PRIVATE_
#define _DRIVER_PRIVATE_

typedef struct    cisam_dr {
        char   fleet;                   /* Fleet Indicator.                */
        char   class;                   /* Driver classification.  Choices: 1 - Regular 2 - Rookie             */
        int    id;                      /* Driver ID. 1 - 99999.           */
        char   name[26];                /* Driver name.                    */
        char   address[26];             /* Driver's home address.          */
        char   city[21];                /* Driver's home city.             */
        char   zip[6];                  /* Driver's home zip code.         */
        char   zip_suf[5];              /* Driver's home zip code suffix.  */
        char   phone[13];               /* Driver's phone number.          */
        char   license_nbr[16];         /* Driver's license number.        */
        char   taxi_license[9];         /* Driver's taxi license No.       */
        char   seals[17];               /* 16 flags for seals.  See vehicle file.                   */
				char   attributes[65];          /* 32 flags.  Define any restrictions on type of service. These flags */
                                        /* correspond to entries in the attribute file and define whether the driver */
                                        /* possesses the related attribute */
        int    susp_from_dt_tm;         /* Date and time this driver was last suspended.             */
        int    susp_to_dt_tm;           /* Date and time this driver was last restored to service.   */
        char   susp_pass_call;          /* "Y/N" whether the driver is restricted from servicing passenger calls.                */
        char   susp_del_call;           /* ....... delivery calls.         */
        char   susp_typex_call;         /* ........dial-a-ride calls.      */
        char   susp_reason[21];         /* reason for drivers suspension.  (internal).                     */
        char   susp_message[97];       /* suspension message to be sent to the driver when he/she signs on while still suspended.*/
        char   comments[81];            /* General driver comments.        */
        char   lic_renew[10];            /* Driver license renewal date.    */
        char   taxi_lic_renew[10];       /* Driver's taxi license renewal date.                           */
        int    emerg_on_dt_tm;          /* Last date and time this driver invoked emergency.       */
        int    emerg_of_dt_tm;          /* Date and time the last emergency was taken care of     */
        int    signed_on_dt_tm;         /* Last date and time this driver was on the street.       */
        int    signed_of_dt_tm;         /* Last date and time this driver became idle.             */
        short  last_veh_nbr;            /* Last vehicle this driver drove  */
        int    last_call;               /* Last call handled by this driver*/

/*         The following counters apply to last shift only.                   */
        short  assigns;                 /* Number of calls manually assigned to this driver on his/her last shift.             */
        short  calls;                   /* Number of dispatched calls      */
        short  flags;                   /* Number of flags picked up on the street.                  */
        short  bids;                    /* Number of bids awarded to this driver.                    */
        short  posts;                   /* Number of times this driver posted.                  */
        short  cond_posts;              /* Number of times this driver conditionally posted.    */
        short  enroute_posts;           /* Number of times this driver enroute posted.          */
        short  short_meters;            /* Number of short meters.         */
        short  late_meters;             /* Number of late meters.          */
        short  no_accepts;              /* Number of no-accepts.           */
        short  rejects;                 /* Number of rejects.              */
        short  breaks;                  /* Number of breaks taken.         */
        short  req_to_talk;             /* Number of times this driver requested to talk.       */
        short  messages;                /* Number of messages sent to supervisors.                    */
        short  callbacks;               /* Number of callbacks.            */
        short  callouts;                /* Number of callouts.             */
        short  no_shows;                /* Number of no-shows.             */
	char	susp_from_date[10];
	char	susp_from_time[7];	
	char	susp_to_date[10];
	char	susp_to_time[7];
      char   phone_number[19];          /* internationalization phone number */
        /* suspended from delivery calls information.             */
        int    susp_d_from_dt_tm;         /* Date and time this driver was last suspended.             */
        int    susp_d_to_dt_tm;           /* Date and time this driver was last restored to service.   */
	char	susp_d_from_date[10];
	char	susp_d_from_time[7];	
	char	susp_d_to_date[10];
	char	susp_d_to_time[7];
        /* suspended from 'X' type calls information.             */
        int    susp_x_from_dt_tm;         /* Date and time this driver was last suspended.             */
        int    susp_x_to_dt_tm;           /* Date and time this driver was last restored to service.   */
	char	susp_x_from_date[10];
	char	susp_x_from_time[7];	
	char	susp_x_to_date[10];
	char	susp_x_to_time[7];
	char    telephone[11];
    char    serial_nbr;    // holds an incremental serial number for a driver ID card (0-9)
  } DRIVER_REC;

#endif
