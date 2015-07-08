/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Account_db.h,v $
*  @(#)  $Revision: 1.5 $
*  @(#)  $Date: 2005/01/11 14:07:59 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Account_db.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ifndef Account_db_H
#define Account_db_H
#include <isam.h>
typedef struct    cisam_ac 
{
      char   fleet;                     /* Fleet indicator.                */
      char   nbr[19];                   /* An account or credit card number*/
      char   name[26];                  /* Name of the company or individual holding this account. */
      int    str_nbr;                   /* Street No of client's location. */
      char   str_suf;                   /* Street number suffix.           */
      char   pre_dir[3];                /* Street pre-direction.           */
      char   str_name[21];              /* Street name.                    */
      char   str_type[4];               /* Street type.                    */
      char   post_dir[3];               /* Street post-direction.          */
      char   apt_room[6];               /* Suite or room number.           */
      char   city[4];                   /* City.                           */
      char   state[3];                  /* State code.                     */
      char   zip[6];                    /* Zip code.                       */
      char   zip_suf[5];                /* Zip code suffix.                */
      char   phone_area[4];             /* Phone area code.                */
      char   phone[8];                  /* Phone number.                   */
      char   phone_ext[6];              /* Phone number extension.         */
      char   phone_contact[21];         /* Phone contact name.             */
      char   attention[21];             /* Account contact name.           */
      char   comment[41];               /* Comment.                        */
      char   header_1[31];              /* Field header of first field to be displayed if enable by account entry clerk. */ 
      char   header_2[31];              /* Field header of second field to be displayed if enable by account entry clerk. */
      char   header_3[16];              /* Header for an entry field          */
      char   header_4[16];              /* Header for an entry field          */
      char   driver_msg[37];            /* A message to be relayed to the driver. */
      short  priority;                  /* A priority given to this account.  will override default priority, if higher */
      char   show_pckup_adr;            /*"Y"/"N".  "Y"=place account pickup address on the call entry screen automatically */
      char   phone_number[19];          /* internationalization phone number */
			char   drv_attr[65];
			char   veh_attr[65];
      char   autobook_enabled;
      char   account_charge_enabled;
      float  factor_value;
      int    extra_value;
  char   cash_account;
  char password[19];
    } ACCOUNT_REC, ACCOUNT;
#ifdef KEY_SRC
struct keydesc ac_key1= {
         ISNODUPS,
         2,
				 { {0,CHARSIZE,CHARTYPE},
					 {1,18,CHARTYPE}  }
};
/*  account nbr only key */
struct keydesc ac_key2 = {
	ISNODUPS,
	1,
	1,18, CHARTYPE,
};
struct keydesc *Account_keys[ ] =
{
  &ac_key1,
  &ac_key2
};
#else
extern struct keydesc ac_key1;
extern struct keydesc ac_key2;
extern struct keydesc *Account_keys[];
#endif
#endif
