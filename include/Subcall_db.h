/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Subcall_db.h,v $
*  @(#)  $Revision: 1.3 $
*  @(#)  $Date: 1998/05/12 19:17:40 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Subcall_db.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ifndef _SUBCALL_DB_H
#define _SUBCALL_DB_H
#include <isam.h>
typedef struct       cisam_sc  {
          int     nbr;                  /* System assigned subscription call number.                            */
          char   fleet;                 /* Fleet indicator.                   */
          char   extended_type[6];      /* Extended call type.                */
          char   pckup_adr_cmnt[31];    /* Pickup address comment from the geo file.                              */
          char  local;                       /* "Y"/"N".  Whether this call is a local run or not*/
          int   pckup_str_nbr;          /* A six digit street number.         */
          char  pckup_str_suf;          /* An alpha street number suffix.     */
          char  pckup_pre_dir[3];       /* Pickup address pre-direction(E.g."W" in 120 W.Main St.).                */
          char  pckup_str_name[21];     /* Pickup street name.                */
          char  pckup_str_type[4];      /* E.g. "Ave" in 120 W. North Ave.    */
          char  pckup_post_dir[3];      /* E.g. "NW" in 120 29th St.          */
          char  intr_pre_dir[3];        /* Intersection street pre-direction  */
          char  intr_str_name[21];      /* Intersection street name.          */
          char  intr_str_type[4];       /* Intersection street type.          */
          char  intr_post_dir[3];       /* Intersection post direction        */
          char  pckup_apt_room[6];      /* Pickup apartment or room number    */
          char  pckup_city[4];          /* Abbreviation of the pickup city    */
          char  passenger_name[21];     /* Name of passenger.                 */
          short pckup_zone;             /* 1-999.  User entered or computer generated pickup zone number.      */
          char  phone_area[4];          /* Telephone area code.               */ 
	  char  phone_nbr[8];           /* Telephone number.                  */
          char  phone_ext[6];           /* Telephone extension.               */
          char  general_cmnt[65];      /* General comment area.              */
          char  dest_adr_cmnt[31];      /* Destination address comment from geo file.                              */
          int   dest_str_nbr;           /* A six digit street number.         */
          char  dest_str_suf;           /* An alpha street number suffix.     */
          char  dest_pre_dir[3];        /* Destination address pre-direction (E.g. "W" in 120 W.Main St.).       */
          char  dest_str_name[21];      /* Destination street name.           */
          char  dest_str_type[4];       /* E.g. "Ave" in 120 W. North Ave.    */
          char  dest_post_dir[3];       /* E.g. "NW" in 120 29th St.          */
          char  dest_apt_room[6];       /* Destination apartment or room nbr  */
          char  dest_city[4];           /* Abbrev of the destination city.    */
          short dest_zone;              /* 1-999.  User entered or computer generated destination zone nbr.    */
          short personal_veh;           /* 1-9999.  Vehicle number of the veh specific requested by the customer */
          char  personal_rqst;          /* Defines whether "O"nly the vehicle specified may pickup the customer or the */
                                        /* customer would like to use that vehicle "I"f it is available, otherwise */
                                        /* another vehicle is accept */
          char  veh_attr_flag;          /* "Y"/"N".  Whether a vehicle or vehicles with specific characteristics */
                                        /* are required to carry out this call.                */
					char  veh_attr[65];           /* 32 flags.  Defines which vehicle attributes are required to carry out this */
                                        /* call. (See ATTRIBUTEfifle)*/
          char  drv_attr_flag;          /* "Y"/"N".  Whether a driver or drivers with specific characteristics are */
                                        /* required to carry out this call.                     */
					char  drv_attr[65];           /* 32 flags.  Defines which driver attributes are required to carry out this call. */
					/* (See ATTRIBUTEFILE).*/
          char  nbr_of_vehs;            /* 1-9.  Number of vehicles needed to service call.   */
          short priority;               /*  Call priority (See FLEET file).                   */
          char  charge_nbr[19];         /* Customer account number or charge card.              */
          char  requested_by[21];       /* Name of person requesting service.                  */
          char  person_to_see[21];      /* Person to see at the destination address.      */
          char  pod_required;           /* "Y"/"N".  Whether proof of delivery is required.     */
          float call_rate;              /* Rate, from the rate table, given to customer.        */
          int  from_date;                /* Beginning date of subscription service.     */ 
          int  to_date;                  /* Ending date of subscription service.  No ending date implies service */
                                         /* until further notice.     */
          short mon_time;                    /* Pickup time for each Monday of the week.       */
          short mon_time_change;             /* A one time Monday time change.                   */
          short tue_time;                     /* Pickup time for each Tuesday of the week.     */
          short tue_time_change;              /* A one time Tuesday time change.                  */
          short wed_time;                     /* Pickup time for each Wednesday of the week.   */
          short wed_time_change;              /* A one time Wednesday time change.                  */
          short thu_time;                     /* Pickup time for each Thursday of the week.    */
          short thu_time_change;              /* A one time Thursday time change.                  */
          short fri_time;                     /* Pickup time for each Friday of the week.      */
          short fri_time_change;              /* A one time Friday time change.                  */
          short sat_time;                     /* Pickup time for each Saturday of the week.    */
          short sat_time_change;              /* A one time Saturday time change.                  */
          short sun_time;                    /* Pickup time for each Sunday of the week.      */
          short sun_time_change;              /* A one time Sunday time change.                  */
          int   date_time_in;                 /* Date and time this subscription call was entered.  "C" format.    */
          short entered_by;                   /* User ID of person who entered this call.       */
          short	last_changed_by;             /* User ID of the last person who changed this subscription call.        */
          int   call_nbr1;                   /* Call number of the call subscription call for the next 24 hours.(from cl_nbr)*/
          int   call_nbr2;                   /* Call number of the call generated from this subscription call for the */
                                             /* 25-48 hours into the future*/
          char  date_in[10];                 /* Date this call was entered.  Display format.*/
          char  time_in[7];                  /* Time this call was entered.  disp format.   */
          char  ac_addt1_info1[26];          /* Information entered in response to the prompt from the account file(ac_header_3)  */
          char  ac_addt1_info2[26];          /* .... (ac_header_4).           */
          char  status_bits[5];              /* 16 flags.                     */
          char  company_name[26];            /* Company name if  a charge account call.                          */
          char  driver_msg[37];              /* Message to be sent to the driver from the account file if this is a charge call. */
          char  pckup_pmap[9];               /* Formated pickup address map info for the driver.                */
          char  dest_dmap[9];                /* Formatted destination address map info for the driver.           */
	  char 	pckup_str_key[5];		/* First 4 chars of pickup street for search purposes */
	  char	pass_name[5];			/* First 4 chars of passenger name for search purposes */
	  char   phone_number[19];          /* internationalization phone number */
	  char	advise_arrival;
#ifdef NAMERICA
	  char  send_dest;
	  char  send_rate;
#endif
}SUBCALL_REC;
#define SC_ADD_ATTR_OFFSET   112
#define SC_NBR                                      0
#define SC_FLEET             SC_NBR               + 4
#define SC_PHONE_NBR         SC_FLEET             + 129
#define SC_CHARGE_NBR        SC_PHONE_NBR         + 172 + SC_ADD_ATTR_OFFSET
#define SC_TO_DATE           SC_CHARGE_NBR        + 67
#define SC_PCKUP_STR_KEY     SC_TO_DATE           + 194
#define SC_PASS_NAME         SC_PCKUP_STR_KEY     + 4
#ifdef KEY_SRC
struct keydesc sc_key1= {
         ISNODUPS,
         1,
         {SC_NBR, LONGSIZE, LONGTYPE}
};
/* Composite of (fleet,to_date) */
struct keydesc sc_key2= {
         ISDUPS,
         2,
				 { {SC_FLEET, CHARSIZE, CHARTYPE},
					 {SC_TO_DATE, LONGSIZE, LONGTYPE} }
};
/* Composite of (fleet,pckup_str_key) */
struct keydesc sc_key3= {
         ISDUPS,
         2,
				 { {SC_FLEET, CHARSIZE, CHARTYPE},
					 {SC_PCKUP_STR_KEY, 4, CHARTYPE} }
};
/* Composite of (fleet,pass_name) */
struct keydesc sc_key4= {
         ISDUPS,
         2,
				 { {SC_FLEET, CHARSIZE, CHARTYPE},
					 {SC_PASS_NAME, 4, CHARTYPE} }
};
/* Composite of (fleet,phone_nbr) */
struct keydesc sc_key5=  {
	 ISDUPS,
	 2,
	 {     {SC_FLEET, CHARSIZE, CHARTYPE},
				 {SC_PHONE_NBR, 7, CHARTYPE} }
};
/* Composite of (fleet,charge_nbr) */
struct keydesc sc_key6=  {
	 ISDUPS,
	 2,
	 {     {SC_FLEET, CHARSIZE, CHARTYPE},
				 {SC_CHARGE_NBR, 18, CHARTYPE} }
};
#else
extern struct keydesc sc_key1;
extern struct keydesc sc_key2;
extern struct keydesc sc_key3;
extern struct keydesc sc_key4;
extern struct keydesc sc_key5;
extern struct keydesc sc_key6;
#endif
#endif
