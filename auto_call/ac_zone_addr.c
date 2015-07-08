/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: ac_zone_addr.c,v $
*  @(#)  $Revision: 1.3 $
*  @(#)  $Date: 2004/02/02 18:55:32 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/auto_call/ac_zone_addr.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: ac_zone_addr.c,v 1.3 2004/02/02 18:55:32 jwelch Exp $";


#include <stdio.h>
#include <isam.h>
#include <sys/types.h>
#include "mads_types.h"
#include "mads_isam.h"
#include "df_maint.h"
#include "mad_error.h"
#include "ex_key.h"
#include "zgeo.h"
#include "screens.h"
#include "language.h"
#include "ui_strdefs.h"
#include "taxipak.h"
#include "taxi_defs.h"
#include "mads_types.h"
#include "Object.h"
#include "Account_db.h"
#include "AutoCall_private.h"
#include "TES.h"


extern char buf[];								/* buffer for db */
extern char req_buf[];								/* request buffer for db */
extern char *strncpy();								/* string copy routine */
extern db();									/* routine to access data files */
extern int scr_name;								/* name of current screen */

struct cisam_sn *sn_ptr;							/* pointer to street name buffer */
struct cisam_ad *ad_ptr;							/* pointer to address buffer */
struct cisam_pl *pl_ptr;							/* pointer to place name buffer */
struct cisam_in *in_ptr;							/* pointer to intersection buffer */
struct cisam_sz *sz_ptr;							/* pointer to sub-zone buffer */

int s_idx;									/* saved index for address verification */

/* the following fields are used for validating intersection addresses */
int s_idx1;									/* saved index for intersection verification */
int s_idx2;									/* saved index for intersection verification */
int x1;										/* temporary space for index for intersection */
int x2;										/* temporary space for index for intersection */

/* the following fields are used for saving the last record read from address key 1 so previous searches will find it quickly */
char s_pre[2];									/* saved prefix direction */
char s_post[2];									/* saved postfix direction */
char s_city[3];									/* saved city */
int s_high;									/* saved high number of range */

/* the following fields are used to store both pickup and destination address fields so both can be zoned */
int *st_nbr;									/* street number */
char *st_name;									/* street name */
char *st_type;									/* street type */
char *st_predir;								/* street pre direction */
char *st_postdir;								/* street post direction */
char *st_city;									/* street city */
char *st_comment;								/* street comment */
short *st_zone;									/* street zone */
char street_key[21];								/* key used for alternate searches */
char *st_pick;


/***********************************************************************************************************************************
zone_addr -
This routine is called by user interface when a call is being entered.  The call structure being pointed to contains all the
address information as supplied by the user.  There are three ways to supply a location:
    a) Actual address - house #, street name, etc.
    b) Place Name - No house #  or  intersecting street.
    c) Intersection - No house #  and an intersecting street.
If the location given is zoneable, the zone is returned, otherwise -1 is returned.
***********************************************************************************************************************************/
ac_zone_addr(fleet, pickup, zip)
char fleet;
POST_ADDRESS *pickup;
int *zip;
{
    int j;									/* temporary place holder for intersection */
    int rc = 0;									/* return code */
    short zone;
    double x_coord = 0.0, y_coord = 0.0;
    
    st_nbr = &(pickup->street_nbr);					/* yes, save pickup street number */
    st_name = pickup->street.name;					/* save pickup street name */
#ifdef _ADD_FOR_NAMERICA_SUPPORT    
    st_predir = pickup->street.pre_dir;
    st_postdir = pickup->street.post_dir;
#endif
    st_type = pickup->street.type;					/* save pickup street type */
    st_city = pickup->city_name;						/* save pickup street city */
    st_zone = &zone;

    strncpy(street_key,st_name,20);						/* store first part as key */

    if (*st_nbr > 0)								/* was there a street number? */
      rc = read_address(fleet, pickup,zip);					/* yes, try to zone address */

    return(rc);									/* return code */
}


/***********************************************************************************************************************************
read_address-
This routine is used to zone an address contained in the call pointer structure.  If the address can be zoned, the zone
number is placed in the structure and returned to the calling procedure. If the address cannont be zoned, a -1 is returned.
***********************************************************************************************************************************/
read_address(fleet, pickup, zip)
     char fleet;
     POST_ADDRESS *pickup;
     int *zip;
{
    	int x;
	short ccode=0;						/* C-ISAM return code */
	short odd_address=0;					/* 0=even address; 1=odd address */
    	char sn_type=' ';
        double x_coord = 0.0, y_coord = 0.0;	

    	sn_ptr = (struct cisam_sn *)req_buf;			/* pointer to street name structure */
    	ad_ptr = (struct cisam_ad *)req_buf;			/* pointer to address structure */

    	strncpy(sn_ptr->name,st_name,20);			/* fill street name key with info from call structure */
    	strncpy(sn_ptr->str_type,st_type,3);
    	if (db(STRNAME_FILE_ID,READ_KEYS,&sn_key2,ISEQUAL,0) >= 0)
	  {
	    sn_type = sn_ptr->rec_type;					/* save record type */
	    s_idx = sn_ptr->index;						/* save index */
	    ad_ptr->str_index = s_idx;					/* fill in address key */
#ifdef _ADD_FOR_NAMERICA_SUPPORT
	    strncpy(ad_ptr->str_pre_dir,st_predir,2);
	    strncpy(ad_ptr->str_post_dir,st_postdir,2);
#else
	    bzero( ad_ptr->str_pre_dir, sizeof(ad_ptr->str_pre_dir) );
	    bzero( ad_ptr->str_post_dir, sizeof(ad_ptr->str_post_dir) );
	    st_predir = ad_ptr->str_pre_dir;
	    st_postdir = ad_ptr->str_post_dir;
#endif
	    strncpy(ad_ptr->str_city,st_city,3);
	    ad_ptr->high_nbr = *st_nbr;
	    
	    if (*st_nbr/2 == (*st_nbr+1)/2)					/* even address ? */
	      odd_address = 0;
	    else
	      odd_address = 1;					/* address is odd */
	    
	    ccode = db(ADDRESS_FILE_ID,READ_KEYS,&ad_key1,ISGTEQ,0); 	/* read an address record */
	    while (ccode >= 0)
	      {
		if ((s_idx != ad_ptr->str_index) || (strncmp(st_predir,ad_ptr->str_pre_dir,2) != 0) ||	/* in range? */
		    (strncmp(st_postdir,ad_ptr->str_post_dir,2) != 0) || (strncmp(st_city,ad_ptr->str_city,3) != 0)) 
		  return(-1);
		
		if (odd_address && (ad_ptr->odd_even == ADDRESS_ODD || ad_ptr->odd_even == ADDRESS_BOTH))
		  break;						/* found appropriate range */
		
		if (!odd_address && (ad_ptr->odd_even == ADDRESS_EVEN || ad_ptr->odd_even == ADDRESS_BOTH))
		  break;						/* found appropriate range */
		
		ccode = db(ADDRESS_FILE_ID,READ_KEYS,&ad_key1,ISNEXT,0); 	/* read next ddress record */
	      }
	    
	    if (ccode < 0)
	      return(-1);						/* no record found */
	    
	    if ((ad_ptr->low_nbr <= *st_nbr) && (ad_ptr->high_nbr >= *st_nbr))
	      {	
		if(sn_type == '2' || sn_type == '3')
		  {			/* alias or abbreviation */
		    strcpy(pickup->street.name,ad_ptr->str_name);	/* copy real street name */
		    strcpy(pickup->street.type,ad_ptr->str_type);	/* copy real street type */
		  }
		else
		  {

		    if (strlen(ad_ptr->str_comment) > 0)
		      strcpy(pickup->comment,ad_ptr->str_comment);

		  }
		
		if ( GetTatort2000( &x_coord, &y_coord, pickup, PICKUP_ADDR ) == SUCCESS )
		  {
		    if ( x_coord > 0.01 )
		      {
			pickup->gps_x = (int)x_coord;
			pickup->gps_y = (int)y_coord;
		      }
		  }
		
		if (ad_ptr->map_ref_page > 0)
		  sprintf(pickup->map_ref,"%3d %s",ad_ptr->map_ref_page,ad_ptr->map_ref_block);
		else
		  pickup->map_ref[0] = '\0';

		x = *st_zone = sz_to_zone(fleet, ad_ptr->str_subzone);	/* transform subzone to zone */
		*zip = ad_ptr->str_zip;
		return(x);							/* return zone */
	      }
	    return(-1);								/* address not in range */
	  }
    	return(-1);									/* no street name record */
}


/***********************************************************************************************************************************
sz_to_zone-
This function takes the subzone from a record and reads the corresponding record to determine the real zone.
***********************************************************************************************************************************/
sz_to_zone(flt,szp)
char flt;								/* fleet of subzone */
char *szp;								/* pointer to subzone */
{
    int x,y;								/* temporary variable */

    sz_ptr = (struct cisam_sz *)req_buf;				/* pointer to subzone record */
    strncpy(sz_ptr->nbr,szp,5);						/* fill key for search */
    if (db(SUBZONE_FILE_ID,READ_KEYS,&sz_key1,ISEQUAL,0) < 0) {		/* read record */
	x = -1;								/* if not found, no zone */
#ifdef DEBUG
	sprintf(trace_str,"zone_addr: Error reading SUBZONE file, no match on info from address record\n");
	TRACE(trace_str);
#endif
	return(x);							/* return zone */
    }

#ifdef LATER
    for(y=0; y<FLEET_MAX; y++)
	if (fleet[y]->fleet_id == flt)
	    break;
#endif

    y = 0; /* fleet A hardcode */

    switch (y) {
	case 0:		x = sz_ptr->zone_fleet1;
			break;
	case 1:		x = sz_ptr->zone_fleet2;
			break;
	case 2:		x = sz_ptr->zone_fleet3;
			break;
	case 3:		x = sz_ptr->zone_fleet4;
			break;
	case 4:		x = sz_ptr->zone_fleet5;
			break;
	case 5:		x = sz_ptr->zone_fleet6;
			break;
	case 6:		x = sz_ptr->zone_fleet7;
			break;
	case 7:		x = sz_ptr->zone_fleet8;
			break;
	default:
#ifdef DEBUG
			sprintf(trace_str,"zone_addr: Fleet in call structure not valid: %c \n",flt);
			TRACE(trace_str);
#endif
			x = -1;
    }
    return(x);						/* return zone */
}


GetTatort2000( pXCoord, pYCoord, cp, pckup_or_dest )
  double *pXCoord;
  double *pYCoord;
  POST_ADDRESS *cp;
{
  char sname[21];
  int  rc = 0, done = 0, mode;
  short sTmp;
  GEOADDR_ADDR_REC   geo_addr;
  GEOADDR_LINE_REC   geo_line;
  GEOADDR_POINT_REC  geo_point;
  double avgX = 0.0, avgY = 0.0;
  int    nbrPoints = 0;

  strcpy( sname, cp->street.name );

  geo_mod( sname );

  bzero( &geo_addr, sizeof(GEOADDR_ADDR_REC) );
  strcpy( geo_addr.sname, sname );
  strcpy( geo_addr.city, cp->city_name );

  mode = ISEQUAL;
  while ( !done )
    {
      if ( db_read_key( GEOADDR_ADDR_FILE_ID, &geo_addr, &geoaddr_addr_key1, mode ) != SUCCESS )
        return(FAIL);

      if ( strcmp( geo_addr.sname, sname ) )
        return(FAIL);
      
      mode = ISNEXT;

      if ( geo_addr.from_left > geo_addr.to_left )
        {
          sTmp = geo_addr.to_left;
          geo_addr.to_left = geo_addr.from_left;
          geo_addr.from_left = sTmp;
        }

      if ( geo_addr.from_right > geo_addr.to_right )
        {
          sTmp = geo_addr.to_right;
          geo_addr.to_right = geo_addr.from_right;
          geo_addr.from_right = sTmp;
        }      

      if ( pckup_or_dest == PICKUP_ADDR )
        {      
          if ( ( ( cp->street_nbr >= geo_addr.from_left ) &&
                 ( cp->street_nbr <= geo_addr.to_left ) &&
                 ( !strncmp( cp->city_name, geo_addr.city, 3 ) ) )
               ||
               ( ( cp->street_nbr >= geo_addr.from_right ) &&
                 ( cp->street_nbr <= geo_addr.to_right )  &&
                 ( !strncmp( cp->city_name, geo_addr.city, 3 ) ) ) )
            done = 1;
        }
    }

  bzero( &geo_line, sizeof(GEOADDR_LINE_REC) );
  geo_line.addr_idx = geo_addr.addr_idx;

  if ( db_read_key( GEOADDR_LINE_FILE_ID, &geo_line, &geoaddr_line_key1, ISEQUAL ) != SUCCESS )
    return(FAIL);

  bzero( &geo_point, sizeof(GEOADDR_POINT_REC) );
  geo_point.line_idx = geo_line.line_idx;

  avgX = 0.0;
  avgY = 0.0;
  nbrPoints = 0;
  
  mode = ISGTEQ;
  done = 0;
  while ( !done )
    {
      if ( db_read_key( GEOADDR_POINT_FILE_ID, &geo_point, &geo_addr_point_key1, mode ) != SUCCESS )
        break;

      if ( geo_point.line_idx != geo_line.line_idx )
        break;

      ++nbrPoints;
      if ( nbrPoints == 1 )
        {
          avgX = geo_point.point_x;
          avgY = geo_point.point_y;
        }
      else
        {
          avgX = ( avgX + geo_point.point_x ) / 2;
          avgY = ( avgY + geo_point.point_y ) / 2;
        }
      
      mode = ISNEXT;
    }

  if ( nbrPoints == 0 )
    return(FAIL);


  *pXCoord = avgX;
  *pYCoord = avgY;

  return(SUCCESS);
}




