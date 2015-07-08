/*
 *                 RCS INFO
 *
 *  @(#)  $RCSfile: Minitel.h,v $
 *  @(#)  $Revision: 1.4 $
 *  @(#)  $Date: 2003/01/28 16:38:43 $
 *  @(#)  $Author: jwelch $
 *  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Minitel.h,v $
 *
 *  Copyright (c) 1991 - Motorola Mobile Data Division, Seattle, WA
 *
 */
#ident "@(#) head:$RCSfile: Minitel.h,v $	$Revision: 1.4 $"

#ifndef _MINITEL_H_
#define _MINITEL_H_


#include <sys/types.h>

#ifdef INTERNATIONAL

#ifndef INTER_INCLUDES
#include <nl_types.h>
#include <locale.h>
#define INTER_INCLUDES
#endif

#ifdef DECLARING
nl_catd MINITEL_catd;
#else
extern nl_catd MINITEL_catd;
#endif

nl_catd catopen();
char    *catgets();

#else
/*
 * no international text        
 */

#define catgets( catd, set_num, msg_num, s )    s
#define catclose( a )
#define setlocale( a, b )
#define open_catalog( a, b, c, d )

#endif

#include "taxipak.h"	/* Gives us such things as NULL, TRUE/FALSE, BOOLEAN, etc. */

#define	MINITEL_CITY_ALIAS_LEN	 6
#define	MINITEL_CITY_NAME_LEN	25
#define MINITEL_ID_LEN		 4
#define	MINITEL_DISP_DATE_LEN	 9
#define	MINITEL_DISP_TIME_LEN	 6
#define MINITEL_NAME_LEN	25
#define MINITEL_PASSWD_LEN	 4
#define MINITEL_PHONE_NBR_LEN	18
#define MINITEL_PHONE_EXT_LEN	 6
#define	MINITEL_PLACE_NAME_LEN	20
#define	MINITEL_STREET_NAME_LEN	20
#define	MINITEL_STREET_TYPE_LEN	 3
#define	MINITEL_ZIP_CODE_LEN	10
#define MINITEL_PICKUP_MAP_LEN 8
#define MINITEL_PICKUP_COMMENT_LEN 30
#define	G7_ACCOUNT_NBR_LEN	18


/**************************** Post_Address.h ***************************/
typedef struct _street
{
	  char  name[ MINITEL_STREET_NAME_LEN + 1 ];
	  char  type[ MINITEL_STREET_TYPE_LEN + 1 ];
} STREET;

typedef struct _post_address
{
        int	street_nbr;
        char	street_nbr_suffix;
	STREET	street;
        char	city_name[ MINITEL_CITY_NAME_LEN + 1 ];
        char	zip[ MINITEL_ZIP_CODE_LEN + 1 ];
	char    map_ref[ MINITEL_PICKUP_MAP_LEN + 1];
	char    comment[ MINITEL_PICKUP_COMMENT_LEN + 1];
	int     gps_x;
	int     gps_y;
	APARTMENT apt;
} POST_ADDRESS;

typedef struct _minitel_phone
{
	char	nbr[MINITEL_PHONE_NBR_LEN + 1];
	char	ext[MINITEL_PHONE_EXT_LEN + 1];
} MINITEL_PHONE;

/**************************** G7_Account_Id.h ***************************/
typedef struct _g7_account_id
{
  char		fleet;
  char		nbr[G7_ACCOUNT_NBR_LEN + 1];
} G7_ACCOUNT_ID;

/***
typedef struct _minitel_rec
{
    char  name[ AC_NAME_LEN + 1 ];
    char  tp_account[ 18 + 1 ];
    char  fleet;
    int   driver_id;
    char  passwd[ PASSWD_LEN + 1 ];
    int   zone;
}  MINITEL_REC;
/***/

#endif	
