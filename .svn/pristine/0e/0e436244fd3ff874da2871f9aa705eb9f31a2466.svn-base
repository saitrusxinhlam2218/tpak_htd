/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Menu_db.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1998/02/23 21:43:53 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Menu_db.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ifndef _MENU_DB_H
#define _MENU_DB_H
#include <isam.h>
typedef struct   cisam_mf   {
       int     nbr;                     /* Sequential number assigned used for permission cross-reference. */
       short   command;                /* A command string assigned to each menu item.  Used by the user with  */
                                        /* the <CMD> function key to invoke menu items.                     */
       char   last_menu_item;           /* "Y"/"N".  Whether this menu item is the last one on the menu or not*/
       char   type;                     /* A menu item may be a "M"enu, or an "E"xternal executable, or an */
                                        /* "I"nternal executable.  External executable implies that a UNIX executable string is */
                                        /* provided in mf_unix_string.  Internal executable implies a function provided by MADS */
                                        /* user interface.                */ 
      char    desc[34];                 /* The actual character description that will be displayed on the menu.           */
      char    unix_string[41];          /* If this menu item is an External executable then this field defines the */
                                        /*  UNIX command sequence to be executed.                     */
      char   prev_scr_flag;           
   } MENU_REC;
#ifdef KEY_SRC
struct keydesc mf_key1= {
         ISNODUPS,
         1,
         {0,LONGSIZE,LONGTYPE},
};
/* command */
struct keydesc mf_key2= {
         ISNODUPS,
         1,
         {4,INTSIZE,INTTYPE},
};
#else
extern struct keydesc mf_key1;
extern struct keydesc mf_key2;
#endif
#endif
