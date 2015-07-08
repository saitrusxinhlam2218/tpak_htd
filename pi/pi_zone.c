static char sz__FILE__[]="@(#)pi_zone.c	10.1.2.1(ver) Date Release 12/2/94";
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  pi_zone.c; Rel: 10.1.2.1; Get date: 4/19/95 at 08:46:26
 * @(#)  Last delta: 12/2/94 at 18:52:59
 * @(#)  SCCS File: /taxi/sccs/s.pi_zone.c
 *                                                                           *
 *****************************************************************************/
/* -------------------------------------------------------------------------------
   Copyright 1993 - TaxiCom Copenhagen, Denmark

   System	:	PLANET - LINK
   Filename	:	pi_zone.c
   Purpose	:	This module zones an address from the PLANET - LINK

Modification history

Author	Date	Revision	Description
------  ------- --------------- -----------------------------------------------
LG      931127  0.0		Function-scheletons and pseudo-code.

------------------------------------------------------------------------------- */

#include <stdio.h>
#include <isam.h>
#include <sys/types.h>
#include <string.h>
#include <math.h>

#include "mads_types.h"
#include "mads_isam.h"
#include "df_maint.h"
#include "ex_key.h"
#include "zgeo.h"
#include "call_entry.h"
#include "screens.h"
#include "ui_def.h"
#include "language.h"
#include "ui_strdefs.h"

#include "pi_account_defines.h"
#include "enhance.h"
#include "switch_ext.h"


extern struct fleets *fleet[FLEET_MAX];						/* array of pointers to point to fleets */
extern char buf[];								/* buffer for db */
extern char req_buf[];								/* request buffer for db */

extern db();									/* routine to access data files */

struct cisam_sn *sn_ptr;							/* pointer to street name buffer */
struct cisam_ad *ad_ptr;							/* pointer to address buffer */
struct cisam_pl *pl_ptr;							/* pointer to place name buffer */
struct cisam_sz *sz_ptr;							/* pointer to sub-zone buffer */

int s_idx;									/* saved index for address verification */

/* the following fields are used for validating intersection addresses */
int s_idx1;									/* saved index for intersection verification */
int s_idx2;									/* saved index for intersection verification */
int x1;										/* temporary space for index for intersection */
int x2;										/* temporary space for index for intersection */

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

typedef struct { double X, Y; } ADDR_POINT;
/***********************************************************************************************************************************
zone_addr -
This routine is called from pi_zone_address.  The call structure being pointed to contains all the
address information as supplied by the user.  There are three ways to supply a location:
    a) Actual address - house #, street name, etc.
    b) Place Name - No house #.
If the location given is zoneable, the zone is returned, otherwise -1 is returned.
***********************************************************************************************************************************/
zone_addr(call_pt,pck_dst,zip)
struct cisam_cl *call_pt;							/* call structure filled in by user interface */
int pck_dst;									/* flag to specify pickup or destination address */
int *zip;
{
    int j;									/* temporary place holder for intersection */
    int rc;
    double x_coord = 0.0, y_coord = 0.0;

    if (call_pt == NULL)							/* did we receive a NULL pointer? */
	return(-1);								/* yes */

    if (pck_dst == PICKUP_ADDR) {				    		/* is this pickup address? */
	st_nbr     = &(call_pt->pckup_str_nbr);					/* yes, save pickup street number */
	st_name    = call_pt->pckup_str_name;					/* save pickup street name */
	st_type    = call_pt->pckup_str_type;					/* save pickup street type */
	st_predir  = call_pt->pckup_pre_dir;					/* save pickup street pre_dir */
	st_postdir = call_pt->pckup_post_dir;					/* save pickup street post_dir */
	st_city    = call_pt->pckup_city;						/* save pickup street city */
	st_zone    = &(call_pt->pckup_zone);					/* save pickup zone */
	st_comment = call_pt->pckup_adr_cmnt;					/* save pickup comment */
    st_pick    = call_pt->pckup_pmap;
    }
    else if (pck_dst == DEST_ADDR) {						/* is this destination address? */
	st_nbr     = &(call_pt->dest_str_nbr);					/* yes, save destination street number */
	st_name    = call_pt->dest_str_name;					/* save destination street name */
	st_type    = call_pt->dest_str_type;					/* save destination street type */
	st_predir  = call_pt->dest_pre_dir;						/* save destination street pre_dir */
	st_postdir = call_pt->dest_post_dir;					/* save destination street post_dir */
	st_city    = call_pt->dest_city;						/* save destination street city */
	st_zone    = &(call_pt->dest_zone);						/* save destination zone */
	st_comment = call_pt->dest_adr_cmnt;					/* save destination comment */
    st_pick    = call_pt->dest_dmap;
    }
    else
	return(-1);								/* not specified */

    strncpy(street_key,st_name,20);						/* store first part as key */

    if (*st_nbr > 0)								/* was there a street number? */
      rc = read_address(call_pt,pck_dst,zip);					/* yes, try to zone address */
    else 
    if (pck_dst == PICKUP_ADDR)
      rc = read_placename(call_pt,pck_dst,zip);				/* no, try to zone place name */

    /* DEST_ADDR */
    else
      rc = read_placename(call_pt,pck_dst,zip);				/* no, try to zone place name */
    return(rc);									/* return code */
}

/* Check if one string is in another and return the pointer in
   the string where it's located. Return NULL if not found.  */
char * in_string(s, in)
char *s;
char *in;
{
  int  i, in_len, no_match;

  in_len = strlen(in);

  while (*s)
    {
      no_match = 0;
      for (i = 0; i < in_len; i ++)
	{
	  if (s[i] != in[i])
	    {
	      no_match = 1;
	      break;
	    }
	}
      if (no_match)
	s ++;
      else
	return(s);
    }
  return(NULL);
}

/***********************************************************************************************************************************
read_placename -
This routine is used to zone a placename contained in the call pointer structure.  If the placename can be zoned, the zone number
is places in the structure and returned to the calling procedure.  If the placename cannont be zoned, a -1 is returned.
***********************************************************************************************************************************/
read_placename(cp,pck_dst,zip)
struct cisam_cl *cp;						/* pointer to call structure with user supplied data */
int pck_dst;							/* pickup or destination address */
int *zip;
{
    int  i, j;
    int  len, old_len;
    char p_or_d_str[32];
    char placename_comment[40];
    char *start;
    char *end_comment;
    char old_str[65];
    char new_str[130];
    char *spacing;

    pl_ptr = (struct cisam_pl *)req_buf;			/* pointer to placename record */

    strncpy(pl_ptr->name,st_name,20);				/* fill placename key with info user supplied in call pointer */
    strncpy(pl_ptr->str_city,st_city,3);

    if (db(PLACENM_FILE_ID,READ_KEYS,&pl_key1,ISEQUAL,0) < 0)	/* try to read exact record */
	return(-1);						/* if not found, stop here */

    if (pck_dst == PICKUP_ADDR)
      strcpy(p_or_d_str, "(HTID:");
    else
      strcpy(p_or_d_str, "(DEST:");

    if (*pl_ptr->str_comment)
      sprintf(placename_comment, "%s%s)", p_or_d_str, pl_ptr->str_comment);
    else
      placename_comment[0] = '\0';

    if ((start = in_string(cp->general_cmnt, p_or_d_str)) != NULL)
      {
	/* Find close paren (end of placename comment) */
	if ((end_comment = strchr(start, ')')) == NULL)
	  /* If not found go to complete end of string */
	  end_comment = start + strlen(start);
	else
	  /* Go past ')' */
	  end_comment ++;
	    
	/* Get part before placename comment */
	old_len = (int) (start - cp->general_cmnt);
	strncpy(old_str, cp->general_cmnt, old_len);
	old_str[old_len] = '\0';
	
	/* Remove trailing spaces */
	for (i = old_len - 1; i >= 0; i --)
	  {
	    if (old_str[i] != ' ')
	      break;
	    else
	      {
		old_str[i] = '\0';
		old_len --;
	      }
	  }

	/* Only put space at start of new comment if it's not at start of comment */
	if (old_len != 0)
	  spacing = " ";
	else
	  spacing = "";
	
	/* New general comment is part before old placename comment, new placename comment, and then
	   stuff after old placename comment (this might get longer than the room we have for the
	   general comment) */
	sprintf(new_str, "%s%s%s%s", old_str, spacing, placename_comment, end_comment);
	
	/* Max length is 64 characters, so cut it off it got too long */
	new_str[65] = '\0';
	
	strcpy(cp->general_cmnt, new_str);
      }

    /* If couldn't find old placename comment then only add placename comment if there's room for it (+1 is for space) */
    else if (strlen(cp->general_cmnt) + strlen(placename_comment) + 1 <= 64)
      {
	if (cp->general_cmnt[0] == '\0' || cp->general_cmnt[0] == ' ')
	  strcpy(cp->general_cmnt, placename_comment);
	else
	  {
	    /* Add a space between old comment and new placename comment */
	    strcat(cp->general_cmnt, " ");
	    strcat(cp->general_cmnt, placename_comment);
	  }
      }

    /* Remove leading spaces */
    len = strlen(cp->general_cmnt);
    for (i = 0; i < len && cp->general_cmnt[i] == ' '; i ++)
      {
	for (j = 0; j < len; j ++)
	  cp->general_cmnt[i + j] = cp->general_cmnt[i + j + 1];
	len --;
      }
    
    strncpy(st_comment,pl_ptr->name,20);			/* now copy place name to address comment */
    *st_nbr = pl_ptr->street_nbr;				/* save street number */
    *zip = pl_ptr->str_zip;
    strncpy(st_name,pl_ptr->str_name,20);			/* copy placename info to call structure */
    strncpy(st_type,pl_ptr->str_type,3);
    strncpy(st_predir,pl_ptr->str_pre_dir,2);
    strncpy(st_postdir,pl_ptr->str_post_dir,2);
    strncpy(st_city,pl_ptr->str_city,3);
    *st_zone = sz_to_zone(cp->fleet,pl_ptr->str_subzone);	/* transform subzone into zone number */
    if(pl_ptr->map_ref_page > 0)
    	sprintf(st_pick,"%3d %4s",pl_ptr->map_ref_page,pl_ptr->map_ref_block);
    else
	*st_pick = '\0';

    return(*st_zone);						/* return zone */
}

/***********************************************************************************************************************************
read_address-
This routine is used to zone an address contained in the call pointer structure.  If the address can be zoned, the zone
number is placed in the structure and returned to the calling procedure. If the address cannont be zoned, a -1 is returned.
***********************************************************************************************************************************/
read_address(cp,pkup_or_dest,zip)
struct cisam_cl *cp;						/* pointer to call structure with user supplied data */
int pkup_or_dest;						/* whether pickup or dest address is being zoned */
int *zip;
{
    	int x;
	short ccode=0;						/* C-ISAM return code */
	short odd_address=0;					/* 0=even address; 1=odd address */
    	char sn_type=' ';
        double x_coord = 0.0, y_coord = 0.0;        
	char *end;
	
    	sn_ptr = (struct cisam_sn *)req_buf;			/* pointer to street name structure */
    	ad_ptr = (struct cisam_ad *)req_buf;			/* pointer to address structure */

	// Need to trim whitespace from end of streetname...
	end = st_name + strlen(st_name) - 1;
	while (end > st_name && isspace(*end)) end --;
	*(end+1) = 0;
	
    	strncpy(sn_ptr->name,st_name,20);			/* fill street name key with info from call structure */
    	strncpy(sn_ptr->str_type,st_type,3);
    	if (db(STRNAME_FILE_ID,READ_KEYS,&sn_key2,ISEQUAL,0) >= 0) {		/* read street name record */
		sn_type = sn_ptr->rec_type;					/* save record type */
		s_idx = sn_ptr->index;						/* save index */
		ad_ptr->str_index = s_idx;					/* fill in address key */
		strncpy(ad_ptr->str_pre_dir,st_predir,2);
		strncpy(ad_ptr->str_post_dir,st_postdir,2);
		strncpy(ad_ptr->str_city,st_city,3);
		ad_ptr->high_nbr = *st_nbr;

		if (*st_nbr/2 == (*st_nbr+1)/2)					/* even address ? */
			odd_address = 0;
		else
			odd_address = 1;					/* address is odd */

		ccode = db(ADDRESS_FILE_ID,READ_KEYS,&ad_key1,ISGTEQ,0); 	/* read an address record */
		while (ccode >= 0){						/* more records to read */
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

		if ((ad_ptr->low_nbr <= *st_nbr) && (ad_ptr->high_nbr >= *st_nbr)) {		/* is it in range? */
	    		if (pkup_or_dest == PICKUP_ADDR){
	        		if(sn_type == '2' || sn_type == '3'){			/* alias or abbreviation */
		   			strcpy(cp->pckup_str_name,ad_ptr->str_name);	/* copy real street name */
					strcpy(cp->pckup_str_type,ad_ptr->str_type);	/* copy real street type */
	        		}
				else {
					if (strlen(ad_ptr->str_comment) > 0) {
						strcpy(cp->pckup_adr_cmnt,ad_ptr->str_comment);
					}
				}
				if (ad_ptr->map_ref_page > 0)
					sprintf(cp->pckup_pmap,"%3d %s",ad_ptr->map_ref_page,ad_ptr->map_ref_block);
				else
					cp->pckup_pmap[0] = '\0';
	    		}
	    		else{
	        		if(sn_type == '2' || sn_type == '3'){			/* alias or abbreviation */
		   			strcpy(cp->dest_str_name,ad_ptr->str_name);	/* copy real street name */
		   			strcpy(cp->dest_str_type,ad_ptr->str_type);	/* copy real street type */
				}
				else {
					if (strlen(ad_ptr->str_comment) > 0) {
						strcpy(cp->dest_adr_cmnt,ad_ptr->str_comment);
					}
				}
				if (ad_ptr->map_ref_page > 0)
					sprintf(cp->dest_dmap,"%3d %4s",ad_ptr->map_ref_page,ad_ptr->map_ref_block);
				else
					cp->dest_dmap[0] = '\0';
	        	}

			if ( pkup_or_dest == PICKUP_ADDR )
			  {
                            // TSAB uses Tatort 2000 database for gps coordinates
                            if ( ( glEnhSwitch & ENH0024 ) &&
                                 ( ( cp->gps_long <= 0.0 ) || ( cp->gps_lat <= 0.0 ) ) )
                              {
                                x_coord = y_coord = 0.0;
                                if ( GetTatort2000( &x_coord, &y_coord, cp, PICKUP_ADDR ) == SUCCESS )
                                  {
                                    if ( x_coord > 0.01 )
                                      {
                                        cp->gps_long = x_coord;
                                        cp->gps_lat = y_coord;
                                      }
                                    else
                                      {
                                        cp->gps_long = (double)ad_ptr->str_x_coord;
                                        cp->gps_lat  = (double)ad_ptr->str_y_coord;
                                      }
                                  }
                                else 
                                  {
                                    cp->gps_long = (double)ad_ptr->str_x_coord;
                                    cp->gps_lat  = (double)ad_ptr->str_y_coord;
                                  }
                              }
                            else if ( ( cp->gps_long <= 0.0 ) || ( cp->gps_lat <= 0.0 ) )
                              {
                                cp->gps_long = (double)ad_ptr->str_x_coord;
                                cp->gps_lat  = (double)ad_ptr->str_y_coord;
                              }                                
			  }
                        else
                          {
                            if ( ( glEnhSwitch & ENH0024 ) &&
                                 ( ( cp->gps_long <= 0.0 ) || ( cp->gps_lat <= 0.0 ) ) )
                              {
                                x_coord = y_coord = 0.0;
                                if ( GetTatort2000( &x_coord, &y_coord, cp, DEST_ADDR ) == SUCCESS )
                                  {
                                    if ( x_coord > 0.01 )
                                      {
                                        cp->dest_gpsx = x_coord;
                                        cp->dest_gpsy = y_coord;
                                      }
                                    else
                                      {
                                        cp->dest_gpsx = (double)ad_ptr->str_x_coord;
                                        cp->dest_gpsy = (double)ad_ptr->str_y_coord;
                                      }
                                  }
                                else
                                  {
                                    cp->dest_gpsx = (double)ad_ptr->str_x_coord;
                                    cp->dest_gpsy = (double)ad_ptr->str_y_coord;
                                  }
                              }
                          }

	    		x = *st_zone = sz_to_zone(cp->fleet,ad_ptr->str_subzone);	/* transform subzone to zone */
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
	return(x);							/* return zone */
    }

    for(y=0; y<FLEET_MAX; y++)
	if (fleet[y]->fleet_id == flt)
	    break;

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
			x = -1;
    }
    return(x);						/* return zone */
}




/*-------------------------------------------------
 *==>	z1000 get zone desc
 *-------------------------------------------------
 */

z1000_get_zone_desc(call)
	struct cisam_cl	*call;
{
	struct cisam_zn *zone;


	memset((char *)call->zone_desc, '\0', sizeof(call->zone_desc));

	zone = (struct cisam_zn *)req_buf;
	zone->fleet = call->fleet;
	zone->nbr = call->pckup_zone;

	if (db(ZONE_FILE_ID, READ_KEYS, &zn_key1, ISEQUAL, 0) < 0)	
		return(FALSE);

	if (strlen(zone->desc))
	  {
		strcpy(call->zone_desc, zone->desc);
		return(TRUE);
	  }

	return(FALSE);
}

GetTatort2000( pXCoord, pYCoord, cp, pckup_or_dest )
  double *pXCoord;
  double *pYCoord;
  struct cisam_cl *cp;
{
  char sname[21];
  int  rc = 0, done = 0, mode, i;
  short sTmp;
  GEOADDR_ADDR_REC   geo_addr;
  GEOADDR_LINE_REC   geo_line;
  GEOADDR_POINT_REC  geo_point;
  double avgX = 0.0, avgY = 0.0;
  int    nbrPoints = 0;
  double dx, dy, InterpDist, Fraction;
  double Dist[512];
  ADDR_POINT geo_points[512];
  bool bOnLeft = 0;
  double FromAddr; double ToAddr; double MatchAddr;
  int Number, WhichOne;
  long    lFrom,lTo, lMatch;

 
  if ( pckup_or_dest == PICKUP_ADDR )
    {
      strcpy( sname, cp->pckup_str_name );
      if (cp->pckup_str_nbr%2 == 0)
	bOnLeft = 1;
    }
  else
    {
      strcpy( sname, cp->dest_str_name );
      if (cp->dest_str_nbr%2 == 0)
	bOnLeft = 1;
    }

  geo_mod( sname );

  bzero( &geo_addr, sizeof(GEOADDR_ADDR_REC) );
  strcpy( geo_addr.sname, sname );
  strcpy( geo_addr.city, cp->pckup_city );

  mode = ISEQUAL;
  while ( !done )
    {
      if ( db_read_key( GEOADDR_ADDR_FILE_ID, &geo_addr, &geoaddr_addr_key1, mode ) != SUCCESS )
        return(FAIL);

      if ( strcmp( geo_addr.sname, sname ) )
        return(FAIL);
      
      mode = ISNEXT;

      if (bOnLeft)
	{
	  if ( geo_addr.from_left > geo_addr.to_left )
	    {
	      sTmp = geo_addr.to_left;
	      geo_addr.to_left = geo_addr.from_left;
	      geo_addr.from_left = sTmp;
	    }
	}
      else
	{

	  if ( geo_addr.from_right > geo_addr.to_right )
	    {
	      sTmp = geo_addr.to_right;
	      geo_addr.to_right = geo_addr.from_right;
	      geo_addr.from_right = sTmp;
	    }      
	}
      
      if ( pckup_or_dest == PICKUP_ADDR )
        {
	  MatchAddr = (double)cp->pckup_str_nbr;
          if ( ( cp->pckup_str_nbr >= geo_addr.from_left ) &&
                 ( cp->pckup_str_nbr <= geo_addr.to_left ) &&
                 ( !strncmp( cp->pckup_city, geo_addr.city, 3 ) ) &&
		 ( bOnLeft ))
	    done = 1;
	  else if ( ( cp->pckup_str_nbr >= geo_addr.from_right ) &&
		    ( cp->pckup_str_nbr <= geo_addr.to_right )  &&
		    ( !strncmp( cp->pckup_city, geo_addr.city, 3 ) ) &&
		    ( !bOnLeft ) )
	    done = 1;
        }
      else
        {
          if ( ( ( cp->dest_str_nbr >= geo_addr.from_left ) &&
                 ( cp->dest_str_nbr <= geo_addr.to_left ) &&
                 ( !strncmp( cp->dest_city, geo_addr.city, 3 ) ) )
               ||
               ( ( cp->dest_str_nbr >= geo_addr.from_right ) &&
                 ( cp->dest_str_nbr <= geo_addr.to_right )  &&
                 ( !strncmp( cp->dest_city, geo_addr.city, 3 ) ) ) )
	    {
	      MatchAddr = (double)cp->dest_str_nbr;
	      done = 1;
	    }
        }          
    }

  
  if (bOnLeft)
    {
      FromAddr = geo_addr.from_left;
      ToAddr = geo_addr.to_left;
    }
  else
    {
      FromAddr = geo_addr.from_right;
      ToAddr = geo_addr.to_right;
    }
  
  lFrom = (long)FromAddr;
  lTo = (long)ToAddr;
  lMatch = (long)MatchAddr;
  if ((lFrom % 2) == (lTo % 2))
    {
      // same parity - every other address in range
      Number = 2 + labs(lTo - lFrom);
      WhichOne = 1 + labs(lMatch - lFrom);
    }
  else
    {
      // opposite parity - every address in range
      Number = 2 + 2 * labs(lTo - lFrom);
      WhichOne = 1 + 2 * labs( lMatch - lFrom);
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
  bzero(Dist, 512*sizeof(double));
  bzero( geo_points, 512*sizeof(ADDR_POINT) );
  mode = ISGTEQ;
  done = 0;
  while ( !done )
    {
      if ( db_read_key( GEOADDR_POINT_FILE_ID, &geo_point, &geo_addr_point_key1, mode ) != SUCCESS )
        break;

      if ( geo_point.line_idx != geo_line.line_idx )
        break;

      geo_points[nbrPoints].X = geo_point.point_x;
      geo_points[nbrPoints].Y = geo_point.point_y;
      
      ++nbrPoints;
      if ( nbrPoints == 1 )
        {
          avgX = geo_point.point_x;
          avgY = geo_point.point_y;
        }
      else
        {
          avgX = ( avgX + geo_point.point_x ) / 2.0;
          avgY = ( avgY + geo_point.point_y ) / 2.0;
        }
      
      mode = ISNEXT;
    }

  if ( nbrPoints == 0 )
    return(FAIL);


  // Start linear interpolation
  for (i=1; i<nbrPoints; ++i)
    {
      dx = (geo_points[i].X-geo_points[i-1].X);
      dy = (geo_points[i].Y-geo_points[i-1].Y);
      Dist[i] = (double)Dist[i-1] + (double)sqrt( dx * dx + dy * dy );
    }

  InterpDist = (Dist[nbrPoints-1]) * WhichOne/Number; 

  //which segment contains the interpolated distance
  for (i=1;i<nbrPoints;++i)
    {
      if (InterpDist < Dist[i])
	break;
    }

  Fraction = (InterpDist - Dist[i-1])/(Dist[i]-Dist[i-1]);
  dx = geo_points[i].X-geo_points[i-1].X;
  dy = geo_points[i].Y-geo_points[i-1].Y;

  *pXCoord = geo_points[i-1].X + dx*Fraction;
  *pYCoord = geo_points[i-1].Y + dy*Fraction;
  
  //  *pXCoord = avgX;
  //  *pYCoord = avgY;

  return(SUCCESS);
}


