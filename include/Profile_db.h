/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Profile_db.h,v $
*  @(#)  $Revision: 1.3 $
*  @(#)  $Date: 2001/03/12 16:42:13 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Profile_db.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ifndef _PROFILE_DB_H
#define _PROFILE_DB_H
#include <isam.h>
typedef struct   cisam_up  {
       short   user_id;                     /* User ID.  Displayed on all screens and used for transaction logging.        */
       char    user_name[9];               /* Full user name.             */
       char    fullname[23];
       char    greeting_name[13];           /* User name to be displayed on the greeting line.       */
       char    menu_perms[57];              /* A flag for each menu selectable item identifying whether this */
                                            /* user is allowed to perform this particular function.   */
       char    fleet_perms[9];              /* Flag for each fleet  that this user may work on.      */
       char    default_fleet;               /* Default fleet id.           */
       short   default_screen;              /* Default screen for this user in navigation format.  */
       short   default_search;              /* Default search for this user in navigation format.  */
       short   default_pos;                 /* Default user position # user type "C"alltaker,"S"upervisor */
                                            /* or "M"anager is derived from menu item permission.       */
       char    default_call_tp;             /* Default call type.               */
       char    default_city[4];             /* Default city code.             */
       char    can_zone_calls;              /* "Y"/"N".  Can this user zone calls.                 */
       short   high_call_pri;               /* 1-62.  The highest priority this user may assign to a call.           */
       short  superzone_1;               /* Superzone 1 default selection.    */
       short  superzone_2;               /* Superzone 2 default selection.    */
       short  superzone_3;               /* Superzone 3 default selection.    */
       short  superzone_4;               /* Superzone 4 default selection.    */
       short  superzone_5;               /* Superzone 5 default selection.    */
       short  superzone_6;               /* Superzone 6 default selection.    */
       short  superzone_7;               /* Superzone 7 default selection.    */
       short  superzone_8;               /* Superzone 8 default selection.    */
       short  superzone_9;               /* Superzone 9 default selection.    */
       short  superzone_10;              /* Superzone 10 default selection.   */
       short  superzone_11;              /* Superzone 11 default selection.   */
       short  superzone_12;              /* Superzone 12 default selection.   */
       short  superzone_13;              /* Superzone 13 default selection.   */
       short  superzone_14;              /* Superzone 14 default selection.   */
       short  superzone_15;              /* Superzone 15 default selection.   */
       short  superzone_16;              /* Superzone 16 default selection.   */
       char   logo_file[21];             /* A pathname of an ascii file to be displayed when the user logs off */
                                         /* (exits) the MADS system.          */
       char   group_1;                    /* "Y"/"N". whether exception group 1 is includ in this users selection */
       char   group_2;                    /* "Y"/"N". whether exception group 2 is includ in this users selection */
       char   group_3;                    /* "Y"/"N". whether exception group 3 is includ in this users selection */
       char   group_4;                    /* "Y"/"N". whether exception group 4 is includ in this users selection */
       char   group_5;                    /* "Y"/"N". whether exception group 5 is includ in this users selection */
       char   group_6;                    /* "Y"/"N". whether exception group 6 is includ in this users selection */
       char   group_7;                    /* "Y"/"N". whether exception group 7 is includ in this users selection */
       char   group_8;                    /* "Y"/"N". whether exception group 8 is includ in this users selection */
       char   group_9;                    /* "Y"/"N". whether exception group 9 is includ in this users selection */
       char   group_10;                   /* "Y"/"N". whether exception group 10 is includ in this users selection */
       char   group_11;                   /* "Y"/"N". whether exception group 11 is includ in this users selection */
       char   group_12;                   /* "Y"/"N". whether exception group 12 is includ in this users selection */
       char   type_1;                    /* "Y"/"N". whether exception type 1 is includ in this users selection */
       char   type_2;                    /* "Y"/"N". whether exception type 2 is includ in this users selection */
       char   type_3;                    /* "Y"/"N". whether exception type 3 is includ in this users selection */
       char   type_4;                    /* "Y"/"N". whether exception type 4 is includ in this users selection */
       char   type_5;                    /* "Y"/"N". whether exception type 5 is includ in this users selection */
       char   type_6;                    /* "Y"/"N". whether exception type 6 is includ in this users selection */
       char   type_7;                    /* "Y"/"N". whether exception type 7 is includ in this users selection */
       char   type_8;                    /* "Y"/"N". whether exception type 8 is includ in this users selection */
       char   type_9;                    /* "Y"/"N". whether exception type 9 is includ in this users selection */
       char   type_10;                   /* "Y"/"N". whether exception type 10 is includ in this users selection */
       char   type_11;                   /* "Y"/"N". whether exception type 11 is includ in this users selection */
       char   type_12;                   /* "Y"/"N". whether exception type 12 is includ in this users selection */
       char   type_13;                   /* "Y"/"N". whether exception type 13 is includ in this users selection */
       char   type_14;                   /* "Y"/"N". whether exception type 14 is includ in this users selection */
       char   type_15;                   /* "Y"/"N". whether exception type 15 is includ in this users selection */
       char   type_16;                   /* "Y"/"N". whether exception type 16 is includ in this users selection */
       char   type_17;                   /* "Y"/"N". whether exception type 17 is includ in this users selection */
       char   type_18;                   /* "Y"/"N". whether exception type 18 is includ in this users selection */
       char   type_19;                   /* "Y"/"N". whether exception type 19 is includ in this users selection */
       char   type_20;                   /* "Y"/"N". whether exception type 20 is includ in this users selection */
       char   type_21;                   /* "Y"/"N". whether exception type 21 is includ in this users selection */
       char   type_22;                   /* "Y"/"N". whether exception type 22 is includ in this users selection */
       char   type_23;                   /* "Y"/"N". whether exception type 23 is includ in this users selection */
       char   type_24;                   /* "Y"/"N". whether exception type 24 is includ in this users selection */
       char   type_25;                  /* "Y"/"N". whether exception type 25 is includ in this users selection */
       char   type_26;                  /* "Y"/"N". whether exception type 26 is includ in this users selection */
       char   type_27;                   /* "Y"/"N". whether exception type 27 is includ in this users selection */
       char   type_28;                   /* "Y"/"N". whether exception type 28 is includ in this users selection */
       char   type_29;                   /* "Y"/"N". whether exception type 29 is includ in this users selection */
       char   type_30;                  /* "Y"/"N". whether exception type30 is includ in this users selection */
       char   type_31;                   /* "Y"/"N". whether exception type 31 is includ in this users selection */
       char   type_32;                   /* "Y"/"N". whether exception type32 is includ in this users selection */
       char   auto_free_text;             /** whether to auto display free text message window for this user after
					     call entry **/
       char   hia_1;
       char   hia_2;
       char   hia_3;
       char   hia_4;
       char   hia_5;
       char   hia_6;
       char   hia_7;
       char   hia_8;
       char   hia_9;
       char   hia_10;
       char   hia_11;
       char   hia_12;
       char   hia_13;
       char   hia_14;
       char   hia_15;
       char   hia_16;
       char   hia_17;
       char   hia_18;
 }USER_REC;
#ifdef KEY_SRC
struct keydesc up_key1= {
         ISNODUPS,
         1,
         {0,INTSIZE,INTTYPE},
};
struct keydesc up_key2= {
         ISDUPS,
         1,
         {10,22,CHARTYPE},
};
#else
extern struct keydesc up_key1;
extern struct keydesc up_key2;
#endif
#endif
