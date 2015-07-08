static char sz__FILE__[]="@(#)ab_zone.c	10.1(ver) Date Release 12/2/94" ;
 /****************************************************************************
 *                                                                           *
 *                           S C C S   I N F O                               *
 *                                                                           *
 * @(#)  ab_zone.c; Rel: 10.1.0.0; Get date: 4/18/95 at 16:32:17
 * @(#)  Last delta: 12/2/94 at 12:11:15
 * @(#)  SCCS File: /taxi/sccs/s.ab_zone.c
 *                                                                           *
 * Copyright (c) 1990 - MDI Seattle                                          *
 *                                                                           *
 *****************************************************************************/
#ident "@(#) head:ab_zone.c	10.1.0.0"

#include <stdio.h>
#include <isam.h>
#include <sys/types.h>
#include "mads_types.h"
#include "mads_isam.h"
#include "df_maint.h"
#include "mad_error.h"
#include "ex_key.h"
#include "zgeo.h"
#include "call_entry.h"
#include "screens.h"
#include "ui_def.h"
#include "language.h"
#include "ui_strdefs.h"

#include "pi_account_defines.h"


extern struct fleets *fleet[FLEET_MAX];						/* array of pointers to point to fleets */
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
zone_addr(call_pt,pck_dst,zip)
struct cisam_cl *call_pt;							/* call structure filled in by user interface */
int pck_dst;									/* flag to specify pickup or destination address */
int *zip;
{
    int j;									/* temporary place holder for intersection */
    int rc;									/* return code */

    if (call_pt == NULL)							/* did we receive a NULL pointer? */
	return(-1);								/* yes */

    if (pck_dst == PICKUP_ADDR) {						/* is this pickup address? */
	st_nbr = &(call_pt->pckup_str_nbr);					/* yes, save pickup street number */
	st_name = call_pt->pckup_str_name;					/* save pickup street name */
	st_type = call_pt->pckup_str_type;					/* save pickup street type */
	st_predir = call_pt->pckup_pre_dir;					/* save pickup street pre_dir */
	st_postdir = call_pt->pckup_post_dir;					/* save pickup street post_dir */
	st_city = call_pt->pckup_city;						/* save pickup street city */
	st_zone = &(call_pt->pckup_zone);					/* save pickup zone */
	st_comment = call_pt->pckup_adr_cmnt;					/* save pickup comment */
    }
    else if (pck_dst == DEST_ADDR) {						/* is this destination address? */
	st_nbr = &(call_pt->dest_str_nbr);					/* yes, save destination street number */
	st_name = call_pt->dest_str_name;					/* save destination street name */
	st_type = call_pt->dest_str_type;					/* save destination street type */
	st_predir = call_pt->dest_pre_dir;					/* save destination street pre_dir */
	st_postdir = call_pt->dest_post_dir;					/* save destination street post_dir */
	st_city = call_pt->dest_city;						/* save destination street city */
	st_zone = &(call_pt->dest_zone);					/* save destination zone */
	st_comment = call_pt->dest_adr_cmnt;					/* save destination comment */
    }
    else
	return(-1);								/* not specified */

    strncpy(street_key,st_name,20);						/* store first part as key */
    st_pick = call_pt->pckup_pmap;

    if (*st_nbr > 0)								/* was there a street number? */
	rc = read_address(call_pt,pck_dst,zip);					/* yes, try to zone address */
    else if (((j=call_pt->intr_str_name[0]) == '_') || (j == '\0'))		/* was there a real intersection? */
	rc = read_placename(call_pt,pck_dst,zip);				/* no, try to zone place name */
    else if (pck_dst == PICKUP_ADDR)						/* for pickup address only */
	rc = read_intersection(call_pt,zip);					/* try to zone intersection */
    else
	rc = -1;

    return(rc);									/* return code */
}



/***********************************************************************************************************************************
zone_alt - 
This routine is called by user interface to supply alternate choices to the location supplied by the user.  This routine may be
called repeatedly to supply next and previous pages of alternate choices.
***********************************************************************************************************************************/
zone_alt(file_flag,key_flag,call_pt,count,nxt_prv,alt_array,pck_dst)
int *file_flag;									/* flag to determine which file to check */
int *key_flag;									/* flag to determine which key to check */
struct cisam_cl *call_pt;							/* pointer to call structure with location info */
int *count;									/* count of alternate choices found */
int nxt_prv;									/* next/previous flag to determine direction */
struct alt_choice alt_array[];							/* array to store alternate choices */
int pck_dst;									/* flag to specify pickup or destination address */
{
    int rc;									/* return code */

    if (call_pt == NULL) {							/* is pointer to call structure valid? */
	*count = 0;								/* show no alternates */
	return(-1);								/* no */
    }

    if (pck_dst == PICKUP_ADDR) {						/* is this pickup address? */
	st_nbr = &(call_pt->pckup_str_nbr);					/* yes, save pickup street number */
	st_name = call_pt->pckup_str_name;					/* save pickup street name */
	st_type = call_pt->pckup_str_type;					/* save pickup street type */
	st_predir = call_pt->pckup_pre_dir;					/* save pickup street pre_dir */
	st_postdir = call_pt->pckup_post_dir;					/* save pickup street post_dir */
	st_city = call_pt->pckup_city;						/* save pickup street city */
	st_zone = &(call_pt->pckup_zone);					/* save pickup zone */
	st_comment = call_pt->pckup_adr_cmnt;					/* save pickup comment */
    }
    else if (pck_dst == DEST_ADDR) {						/* is this destination address? */
	st_nbr = &(call_pt->dest_str_nbr);					/* yes, save destination street number */
	st_name = call_pt->dest_str_name;					/* save destination street name */
	st_type = call_pt->dest_str_type;					/* save destination street type */
	st_predir = call_pt->dest_pre_dir;					/* save destination street pre_dir */
	st_postdir = call_pt->dest_post_dir;					/* save destination street post_dir */
	st_city = call_pt->dest_city;						/* save destination street city */
	st_zone = &(call_pt->dest_zone);					/* save destination zone */
	st_comment = call_pt->dest_adr_cmnt;					/* save destination comment */
    }
    else {
	*count = 0;								/* show no alternates */
	return(-1);								/* pickup or dest not specified */
    }

    if (*file_flag == -1)							/* are we starting a new alternate choice? */
	strncpy(street_key,st_name,20);						/* yes, store first part as key */

    if (*st_nbr > 0 || (*st_name && *st_type))						/* is this address with house number? */
	rc= get_alt_address(file_flag,key_flag,call_pt,count,nxt_prv,alt_array);	/* yes, get alternates for address */

    else if (call_pt->intr_str_name[0] == '_' || call_pt->intr_str_name[0] == '\0')	/* is this place name? */
	rc = get_alt_placename(file_flag,key_flag,call_pt,count,nxt_prv,alt_array);	/* yes, get alternates for place name */

    else if (pck_dst == PICKUP_ADDR)							/* intersection only valid for pickup */
	rc = get_alt_intersection(file_flag,key_flag,call_pt,count,nxt_prv,alt_array);	/* get alternates for intersection */

    else {
	*count = 0;									/* no alternates available */
	rc = -1;
    }

    return(rc);										/* return code */
}



/***********************************************************************************************************************************
get_alt_placename -
This function finds alternate choices for a placename entered by the user.  Records from the Placename file are read from disk
and placed in the alternate choice array for return to the calling procedure.  The first time this function is called, the file
flag parameter should point to an integer containing -1.  This indicates that the first page of alternates are to be displayed.
The key flag and count fields will be set by this function and should not be changed by the calling procedure.  The call_pt
parameter should be a pointer to a call structure containing all user supplied data.  The nxt_prv field should be positive to
get the next page of alternates, and negative to get the previous page.  Finally, the alt_array parameter should point to an
array of alternate_choice structures to store the alternates found.
***********************************************************************************************************************************/
get_alt_placename(file_flag,key_flag,call_pt,count,nxt_prv,alt_array)
int *file_flag;								/* flag to determine which file to access */
int *key_flag;								/* flag to determine which key to use */
struct cisam_cl *call_pt;						/* pointer to call structure with user supplied data */
int *count;								/* count of alternates found */
int nxt_prv;								/* next/previous flag to specify direction of search */
struct alt_choice alt_array[];						/* alternate choice array for return */
{
    int i;								/* temporary variable for loops */
    int temperr;							/* error returned from db call */

    pl_ptr = (struct cisam_pl *)req_buf;				/* pointer to placename structure */

    if (*file_flag == -1) {						/* is this the first time this was called? */
	*count = 0;							/* yes, start count at 0 */
	*file_flag = PLACENM_FILE_ID;					/* set file to Placename file */
	*key_flag = 1;							/* use primary key */
	strncpy(pl_ptr->name,street_key,20);				/* fill key to retrieve placename record */
	if (db(PLACENM_FILE_ID,READ_KEYS,&pl_key1,ISGTEQ,0) < 0) {	/* read a record */
	    *count = 0;							/* no record found */
	    return(-1);							/* get out now */
	}
	*count = next_plc_rec(call_pt,alt_array,MAXALT);		/* read as many as match up to a page full */
    }
    else if (nxt_prv >= 0) {						/* is this a request for next page of alternates? */
	temperr = iserrno;						/* yes, save error value */
	*count = next_plc_rec(call_pt,alt_array,MAXALT);		/* read as many as match up to a page full */
	if ((temperr == 110) && (*count == 1) && (iserrno == 110))	/* if we reached end of file and only had 1 match */
	    *count = 0;							/* it doesn't really exist, next_plc_rec adds 1 more */
    }
    else {								/* this is a request for previous page of alternates */
	if (iserrno == 110)						/* did we reach end/beginning of file last time? */
	    *count -= 1;						/* yes, count is 1 too many */
	for(i= *count; i>0; i--) {					/* go back over the records on the present page */
	    if (db(PLACENM_FILE_ID,READ_KEYS,&pl_key1,ISPREV,0) < 0) {	/* read each one */
		*count = 0;						/* if no record found, there is no previous page */
		return(-1);						/* get out here */
	    }
	    if (strncmp(street_key,pl_ptr->name,strlen(street_key)) != 0) {	/* is this still a possible duplicat? */
		db(PLACENM_FILE_ID,READ_KEYS,&pl_key1,ISNEXT,0);		/* no, go back to one that was */
		*count = 0;							/* no previous page */
		return(-1);							/* get out here */
	    }
	}

	for(i=0; i<MAXALT; i++) {					/* try to go back for a page of alternates */
	    if (db(PLACENM_FILE_ID,READ_KEYS,&pl_key1,ISPREV,0) < 0)	/* read another record backwards */
		break;							/* if end of file, stop here */
	    else if (strncmp(street_key,pl_ptr->name,strlen(street_key)) != 0) {	/* is it a possible duplicate? */
		db(PLACENM_FILE_ID,READ_KEYS,&pl_key1,ISNEXT,0);	/* no, go forward to one that was */
		break;							/* stop search here */
	    }
	}
	*count = next_plc_rec(call_pt,alt_array,i);			/* read forward as many records as we just found */
    }
    return(-1);
}



/***********************************************************************************************************************************
get_alt_intersection-
This function finds alternate choices for an intersection entered by the user.  Records from the intersection file are read
from disk and placed in the alternate choice array for return to the calling procedure.  The first time this function is called,
the file flag parameter should point to an integer containing -1.  This indicates that the first page of alternates are to
be displayed.  The key flag and count fields will be set by this function and should not be changed by the calling procedure.
The call_pt parameter should be a pointer to a call structure containing all user supplied data.  The nxt_prv field should be
positive to get the next page of alternates, and negative to get the previous page.  Finally, the alt_array parameter should
point to an array of alternate_choice structures to store the alternates found.
***********************************************************************************************************************************/
get_alt_intersection(file_flag,key_flag,call_pt,count,nxt_prv,alt_array)
int *file_flag;								/* flag to determine which file to access */
int *key_flag;								/* flag to determine which key to use */
struct cisam_cl *call_pt;						/* pointer to call structure with user supplied data */
int *count;								/* count of alternates found */
int nxt_prv;								/* next/previous flag to specify direction of search */
struct alt_choice alt_array[];						/* alternate choice array for return */
{
    int i;								/* temporary variable for loops */

    in_ptr = (struct cisam_in *)req_buf;				/* pointer to intersection structure */
    sn_ptr = (struct cisam_sn *)req_buf;				/* pointer to street name structure */

    if (*file_flag == -1) {						/* is this the first time in for alternates? */
	s_idx1 = s_idx2 = -1;						/* initialize street indexes */
	x1 = x2 = 0;							/* again */
	*count = 0;							/* start with no duplicates */
	*file_flag = STRNAME_FILE_ID;					/* start with street name file */
	*key_flag = 1;							/* start with primary key */
	strncpy(sn_ptr->name,st_name,20);				/* fill street name key */
	strncpy(sn_ptr->str_type,st_type,3);
	if (db(STRNAME_FILE_ID,READ_KEYS,&sn_key2,ISEQUAL,0) < 0) {	/* read first street name */
	    strncpy(sn_ptr->name,street_key,20);			/* not found, try key for alternates */
	    if (db(STRNAME_FILE_ID,READ_KEYS,&sn_key2,ISGTEQ,0) >= 0)	/* read alternates */
		*count = next_str_rec(call_pt,alt_array,MAXALT);	/* go forward and try to find a page full */
	}
	else								/* first street was found */
	    s_idx1 = sn_ptr->index;					/* save index for it */

	if (*count == 0) {						/* have we got duplicates yet? */
	    *key_flag = 2;						/* no, search on intersecting street name */
	    strncpy(sn_ptr->name,call_pt->intr_str_name,20);		/* fill in key for search */
	    strncpy(sn_ptr->str_type,call_pt->intr_str_type,3);
	    if (db(STRNAME_FILE_ID,READ_KEYS,&sn_key2,ISEQUAL,0) < 0) {		/* try to read street name */
		strncpy(street_key,call_pt->intr_str_name,20);			/* not found, use key for alternates */
		strncpy(sn_ptr->name,street_key,20);
		if (db(STRNAME_FILE_ID,READ_KEYS,&sn_key2,ISGTEQ,0) >= 0)	/* read alternates based on intersection key */
		    *count = next_str_rec(call_pt,alt_array,MAXALT);		/* if one found, try to get more alternates */
	    }
	    else
		s_idx2 = sn_ptr->index;						/* intersecting street found, save index */
	}

	if (*count == 0) {						/* have we got any possible duplicates yet? */
	    if ((s_idx1 == -1) && (s_idx2 == -1))			/* no, do we have any valid streets yet? */
		return(-1);						/* no, return without a duplicate */
	    else if (s_idx1 == -1) {					/* was first street missing? */
		x1 = in_ptr->index1 = s_idx2;				/* yes, save second street as first index in key */
		x2 = in_ptr->index2 = 0;				/* no second index */
	    }
	    else if (s_idx2 == -1) {					/* was second street missing? */
		x1 = in_ptr->index1 = s_idx1;				/* yes, save first street as first index in key */
		x2 = in_ptr->index2 = 0;				/* no second index */
	    }
	    else {
		if (s_idx1 < s_idx2) {					/* is first index the smaller one? */
		    x1 = in_ptr->index1 = s_idx1;			/* yes, save it first */
		    x2 = in_ptr->index2 = s_idx2;			/* second one is larger */
		}
		else {							/* second index is smaller */
		    x1 = in_ptr->index1 = s_idx2;			/* save it first */
		    x2 = in_ptr->index2 = s_idx1;			/* first one is larger */
		}
	    }
	    strncpy(in_ptr->pre_dir1,"",2);				/* dummy up the rest of the key fields */
	    strncpy(in_ptr->post_dir1,"",2);
	    strncpy(in_ptr->pre_dir2,"",2);
	    strncpy(in_ptr->post_dir2,"",2);
	    strncpy(in_ptr->city,"",3);
	    *file_flag = INT_FILE_ID;					/* searching intersection file now */
	    *key_flag = 1;						/* primary key */
	    if (db(INT_FILE_ID,READ_KEYS,&in_key1,ISGTEQ,0) < 0)	/* try to read intersection records */
		return(-1);						/* nothing found, stop here */
	    *count = next_intr_rec(alt_array,MAXALT);			/* try to read a page full of alternates */
	}
	return(-1);							/* return with number of alternates in *count */
    }
    else if (*file_flag == STRNAME_FILE_ID) {				/* continuing search on street name */
	if (nxt_prv >= 0)						/* is this a request to go forward a page? */
	    *count = next_str_rec(call_pt,alt_array,MAXALT);		/* yes, try to get a page full of alternates */
	else {								/* must be request to go back to previous page */
	    for(i= *count; i>0; i--) {					/* go back past records on current page */
		if (db(STRNAME_FILE_ID,READ_KEYS,&sn_key2,ISPREV,0) < 0) {	/* read previous */
		    *count =0;						/* if not found, then there is no previous page */
		    return(-1);						/* stop here */
		}
	    }
	    for(i=0; i<MAXALT; i++) {					/* now try to get a full page of previous records */
		if (db(STRNAME_FILE_ID,READ_KEYS,&sn_key2,ISPREV,0) < 0)	/* read previous */
		    break;						/* if nothing found, this is all we found */
		else if (strncmp(street_key,sn_ptr->name,strlen(street_key)) != 0) {	/* does this match key? */
		    db(STRNAME_FILE_ID,READ_KEYS,&sn_key2,ISNEXT,0);	/* no, go forward to one that does */
		    break;						/* this is all */
		}
	    }
	    *count = next_str_rec(call_pt,alt_array,i);			/* get as many as we found in above loop (i) */
	}
	return(-1);							/* stop here */
    }
    else if (*file_flag == INT_FILE_ID) {				/* searching intersection file */
	if (nxt_prv >= 0)						/* is this a request to go to next page? */
	    *count = next_intr_rec(alt_array,MAXALT);			/* yes, try to get a page full of alternates */
	else {								/* must be a request to go back a page */
	    for(i= *count; i>0; i--) {					/* go back over records on current page */
		if (db(INT_FILE_ID,READ_KEYS,&in_key1,ISPREV,0) < 0) {	/* read previous */
		    *count = 0;						/* if nothing found, there is no previous page */
		    return(-1);						/* stop here */
		}
	    }
	    for(i=0; i>MAXALT; i++) {					/* try to find a full page of alternates */
		if (db(INT_FILE_ID,READ_KEYS,&in_key1,ISPREV,0) < 0)	/* read previous */
		    break;						/* if no record, this is all we got */
		else if ((x1 > 0) && (x2 > 0)) {			/* are we searching with both street indexes? */
		    if ((x1 != in_ptr->index1) || (x2 != in_ptr->index2))	/* yes, are either of them differetn? */
			break;						/* yes, this is all we found */
		}
		else if (x1 != in_ptr->index1)				/* only using one index, is it the same? */
		    break;						/* no, this is all we found */
	    }
	    *count = next_intr_rec(alt_array,i);			/* count up all we found in above loop (i) */
	}
	return(-1);							/* that's it */
    }
}



/***********************************************************************************************************************************
get_alt_address-
This function finds alternate choices for an address entered by the user.  Records from the address file are read
from disk and placed in the alternate choice array for return to the calling procedure.  The first time this function is called,
the file flag parameter should point to an integer containing -1.  This indicates that the first page of alternates are to
be displayed.  The key flag and count fields will be set by this function and should not be changed by the calling procedure.
The call_pt parameter should be a pointer to a call structure containing all user supplied data.  The nxt_prv field should be
positive to get the next page of alternates, and negative to get the previous page.  Finally, the alt_array parameter should
point to an array of alternate_choice structures to store the alternates found.
***********************************************************************************************************************************/
get_alt_address(file_flag,key_flag,call_pt,count,nxt_prv,alt_array)
int *file_flag;								/* flag to determine which file to access */
int *key_flag;								/* flag to determine which key to use */
struct cisam_cl *call_pt;						/* pointer to call structure with user supplied data */
int *count;								/* count of alternates found */
int nxt_prv;								/* next/previous flag to specify direction of search */
struct alt_choice alt_array[];						/* alternate choice array for return */
{
    int i,j,x;								/* temporary variables for loops, etc. */

    ad_ptr = (struct cisam_ad *)req_buf;				/* pointer to address structure */
    sn_ptr = (struct cisam_sn *)req_buf;				/* pointer to street name structure */

    if (*file_flag == -1) {						/* is this the first time for alternates here? */
	*count = 0;							/* yes, nothing found yet */
	*file_flag = STRNAME_FILE_ID;					/* start with street name file */
	*key_flag = 1;							/* start with primary key */
	strncpy(sn_ptr->name,st_name,20);				/* fill key with street name info */
	strncpy(sn_ptr->str_type,st_type,3);

	if (db(STRNAME_FILE_ID,READ_KEYS,&sn_key2,ISEQUAL,0) < 0) {	/* read street name record */
	    strncpy(sn_ptr->name,street_key,20);			/* if not found, search with key */
	    if (db(STRNAME_FILE_ID,READ_KEYS,&sn_key2,ISGTEQ,0) < 0) {	/* try to get an alternate */
		*count = 0;						/* if nothing found, no possible alternates */
		return(-1);						/* stop here */
	    }
	    *count = next_str_rec(call_pt,alt_array,MAXALT);		/* else try to get a full page of alternates */
	}
	else {								/* street name is ok, try to match rest of address */
	    *file_flag = ADDRESS_FILE_ID;				/* searching on address file now */
	    *key_flag = 1;						/* with primary key */
	    s_idx = sn_ptr->index;					/* save street index */
	    if ((i = read_first_key1(call_pt)) >= 0)			/* read address record with primary key */
		*count = next_adr_rec1(call_pt,alt_array,MAXALT);	/* found one, try to get full page of alternates */
	    if (*count < MAXALT) {					/* do we still have room for more alternate choices? */
		*key_flag = 2;						/* yes, search on secondary key now */
		ad_ptr->str_index = s_idx;				/* fill secondary key */
		ad_ptr->high_nbr = 0;					/*** THIS SHOULD BE st_nbr WHEN FULL DATABASE */
		if (db(ADDRESS_FILE_ID,READ_KEYS,&ad_key2,ISGTEQ,0) < 0) {	/* try to read alternates */
		    *count += 0;					/* nothing found, count stays the same */
		    return(-1);						/* that's all, stop here */
		}
		*count = next_adr_rec2(call_pt,alt_array,*count,MAXALT);	/* try to get rest of page filled up */
	    }
	}
	return(-1);							/* stop here */
    }
    else if (*file_flag == STRNAME_FILE_ID) {				/* searching on street name file */
	if (nxt_prv >= 0)						/* is this a request to go forward to next page? */
	    *count = next_str_rec(call_pt,alt_array,MAXALT);		/* yes, try to fill page with alternate choices */
	else {								/* must be request to go back to previous page */
	    for(i= *count; i>0; i--) {					/* back up over records on current page */
		if (db(STRNAME_FILE_ID,READ_KEYS,&sn_key2,ISPREV,0) < 0) {	/* read previous */
		    *count = 0;							/* if nothing found, no previous page */
		    return(-1);							/* stop here */
		}
		if (strncmp(street_key,sn_ptr->name,strlen(street_key)) != 0) {	/* is it still within range? */
		    db(STRNAME_FILE_ID,READ_KEYS,&sn_key2,ISNEXT,0);		/* no, go forward to record that was */
		    *count = 0;							/* no previous page */
		    return(-1);							/* stop here */
		}
	    }
	    for(i=0; i<MAXALT; i++) {						/* try to get full page of previous alternates */
		if (db(STRNAME_FILE_ID,READ_KEYS,&sn_key2,ISPREV,0) < 0)	/* read previous */
		    break;							/* if nothing found, that's all the alternates */
		else if (strncmp(street_key,sn_ptr->name,strlen(street_key)) != 0) {	/* is it still within range? */
		    db(STRNAME_FILE_ID,READ_KEYS,&sn_key2,ISNEXT,0);		/* no, go forward to record that was */
		    break;							/* enough */
		}
	    }
	    *count = next_str_rec(call_pt,alt_array,i);			/* get all the possible alternates from the loop above */
	}
	return(-1);							/* stop here */
    }
    else if (*file_flag == ADDRESS_FILE_ID) {					/* searching the address file */
	if (nxt_prv >= 0) {							/* is this request for next page of alternates? */
	    if (*key_flag == 1) {						/* yes, is this the primary key? */
		if ((*count = next_adr_rec1(call_pt,alt_array,MAXALT)) < MAXALT) {	/* yes, try to get full page of alts */
		    ad_ptr->str_index = s_idx;					/* page not full, save index for secondary key */
		    ad_ptr->high_nbr = 0;
		    if (db(ADDRESS_FILE_ID,READ_KEYS,&ad_key2,ISGTEQ,0) < 0) {	/* try to read alternates for address with key 2 */
			*count += 0;						/* no more alternates */
			return(-1);						/* stop here */
		    }
		    *key_flag = 2;						/* show we are on second key */
		    *count = next_adr_rec2(call_pt,alt_array,*count,MAXALT);	/* try to fill up page with secondary alternates */
		}
	    }
	    else if (*key_flag == 2)						/* are we on secondary key? */
		*count = next_adr_rec2(call_pt,alt_array,0,MAXALT);		/* yes, try to get next page of alternates */
	}
	else {									/* must be request for prev page of alternates */
	    if (*key_flag == 1) {						/* is this primary key? */
		for(i= *count; i>0; i--) {					/* yes, go back over records on current page */
		    if (db(ADDRESS_FILE_ID,READ_KEYS,&ad_key1,ISPREV,0) < 0) {	/* read previous record */
			*count = 0;						/* if not found, no previous page */
			return(-1);						/* stop here */
		    }
		    if ((s_idx != ad_ptr->str_index) || (strncmp(st_predir,ad_ptr->str_pre_dir,2) != 0) ||	/* does it have */
				(strncmp(st_postdir,ad_ptr->str_post_dir,2) != 0) ||				/* the same key? */
				(strncmp(st_city,ad_ptr->str_city,3) != 0)) {
			db(ADDRESS_FILE_ID,READ_KEYS,&ad_key1,ISNEXT,0);	/* no, go forward to one that does */
			*count = 0;						/* no previous page */
			return(-1);						/* stop here */
		    }
		}
		i = prev_key1(call_pt);						/* get count of page of previous record */
		*count = next_adr_rec1(call_pt,alt_array,i);			/* read those records */
	    }
	    else if (*key_flag == 2) {						/* is this secondary key? */
		for(i= *count; i>0; i--) {					/* yes, go back over current page of records */
		    if ((db(ADDRESS_FILE_ID,READ_KEYS,&ad_key2,ISPREV,0) < 0) || (s_idx != ad_ptr->str_index)) {
			/* || (call_pt->pckup_str_nbr < ad_ptr->low_nbr) || (call_pt->pckup_str_nbr > ad_ptr->high_nbr)) { */
			read_last_key1();					/* gone back over all secondary key records */
			*key_flag = 1;						/* go back to primary key */
			for(;i>0;i--) {						/* continue to read back */
			    if (db(ADDRESS_FILE_ID,READ_KEYS,&ad_key1,ISPREV,0) < 0) {	/* read previous record */
				*count = 0;					/* if not found, no previous page */
				return(-1);					/* stop here */
			    }
			    if ((s_idx != ad_ptr->str_index) || (strncmp(st_predir,ad_ptr->str_pre_dir,2) != 0) ||
					(strncmp(st_postdir,ad_ptr->str_post_dir,2) != 0) ||
					(strncmp(st_city,ad_ptr->str_city,3) != 0)) {		/* is it within range? */
				db(ADDRESS_FILE_ID,READ_KEYS,&ad_key1,ISNEXT,0);		/* no, go forward to one that is */
				*count = 0;							/* no previous page */
				return(-1);							/* stop here */
			    }
			}
			break;
		    }
		}
		if (*key_flag == 1) {						/* is this primary key? */
		    i = prev_key1(call_pt);					/* get previous records with this key */
		    if ((*count = next_adr_rec1(call_pt,alt_array,i)) < i) {	/* save them for return */
			*key_flag = 2;						/* change to secondary key */
			ad_ptr->str_index = s_idx;				/* if entire page not found, go to next key */
			ad_ptr->high_nbr = 0;					/* set up secondary key values */
			if (db(ADDRESS_FILE_ID,READ_KEYS,&ad_key2,ISGTEQ,0) < 0) {	/* read new record */
			    *count += 0;					/* if none found don't increase count */
			    return(-1);						/* stop here */
			}
			*count = next_adr_rec2(call_pt,alt_array,*count,i);	/* get count on second key also */
		    }
		}
		else for(i=0; i<MAXALT; i++) {					/* must be on secondary key */
/* || (call_pt->pckup_str_nbr < ad_ptr->low_nbr) || (call_pt->pckup_str_nbr > ad_ptr->high_nbr)) { */
		    if ((db(ADDRESS_FILE_ID,READ_KEYS,&ad_key2,ISPREV,0) < 0) || (s_idx != ad_ptr->str_index)) {
			read_last_key1();					/* if no previous record on this key */
			*key_flag = 1;						/* change to primary key */
			for(;i<MAXALT;i++) {					/* repeat while not a page full */
			    if (db(ADDRESS_FILE_ID,READ_KEYS,&ad_key1,ISPREV,0) < 0)	/* read previous record */
				break;							/* if nothing found break out here */
			    if ((s_idx != ad_ptr->str_index) || (strncmp(st_predir,ad_ptr->str_pre_dir,2) != 0) ||
					(strncmp(st_postdir,ad_ptr->str_post_dir,2) != 0) ||
					(strncmp(st_city,ad_ptr->str_city,3) != 0)) {		/* is it within proper range? */
				if (db(ADDRESS_FILE_ID,READ_KEYS,&ad_key1,ISNEXT,0) < 0)	/* no, go forward to one that is */
				break;
			    }
			}
			break;
		    }
		}
		if (*key_flag == 1) {		/* have gone back to get previous page of alternates starting at present record */
		    if ((*count = next_adr_rec1(call_pt,alt_array,i)) < i) {	/* if primary key get count for this key */
			ad_ptr->str_index = s_idx;				/* if not enough, then search secondary key too */
			ad_ptr->high_nbr = 0;
			*key_flag = 2;							/* change to secondary key */
			if (db(ADDRESS_FILE_ID,READ_KEYS,&ad_key2,ISGTEQ,0) < 0) {	/* read record from secodary key */
			    *count += 0;						/* if not found, don't increase count */
			    return(-1);							/* stop here */
			}
			*count = next_adr_rec2(call_pt,alt_array,*count,MAXALT);	/* add count from secondary key search */
		    }
		}
		else
		    *count = next_adr_rec2(call_pt,alt_array,0,i);			/* just get count from secondary key */
	    }
	}
	return(-1);
    }
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
    int l,x,pl_length,cl_length;				/* temporary variables */

    pl_ptr = (struct cisam_pl *)req_buf;			/* pointer to placename record */

    strncpy(pl_ptr->name,st_name,20);				/* fill placename key with info user supplied in call pointer */
    strncpy(pl_ptr->str_city,st_city,3);

    if (db(PLACENM_FILE_ID,READ_KEYS,&pl_key1,ISEQUAL,0) < 0)	/* try to read exact record */
	return(-1);						/* if not found, stop here */

    pl_length = strlen(pl_ptr->str_comment);			/* string length of place name comment */
    cl_length = strlen(cp->general_cmnt);			/* string length of general comment */
    if (pl_length > 0 && (cl_length + pl_length) <= 112) {	/* do we need to or can we shuffle comments around? */
	if (pck_dst == PICKUP_ADDR) {
	    strcat(cp->general_cmnt,catgets(UI_m_catd, UI_1, UI_ZONE_ADDR_1, " PCKUP: "));		/* place name comment for pickup address */
	    strcat(cp->general_cmnt,pl_ptr->str_comment);
	}
	else {
	    strcat(cp->general_cmnt,catgets(UI_m_catd, UI_1, UI_ZONE_ADDR_2, " DEST: "));			/* place name comment for destinantion address */
	    strcat(cp->general_cmnt,pl_ptr->str_comment);
	}
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
    	sprintf(st_pick,"%d %4s",pl_ptr->map_ref_page,pl_ptr->map_ref_block);
    else
	*st_pick = '\0';


    return(*st_zone);						/* return zone */
}


/***********************************************************************************************************************************
read_intersection-
This routine is used to zone an intersection contained in the call pointer structure.  If the intersectin can be zoned, the zone
number is placed in the structure and returned to the calling procedure. If the intersection cannont be zoned, a -1 is returned.
***********************************************************************************************************************************/
read_intersection(cp,zip)
struct cisam_cl *cp;						/* pointer to call structure with user supplied data */
int *zip;
{
    int ind1,ind2;						/* variables for storing street indexes */

    sn_ptr = (struct cisam_sn *)req_buf;			/* pointer to street name structure */
    in_ptr = (struct cisam_in *)req_buf;			/* pointer to intersection structure */

    strncpy(sn_ptr->name,cp->pckup_str_name,20);		/* fill key with data from user */
    strncpy(sn_ptr->str_type,cp->pckup_str_type,3);
    if (db(STRNAME_FILE_ID,READ_KEYS,&sn_key2,ISEQUAL,0) < 0)	/* read first street name */
	return(-1);						/* if not found, intersection not zoned */
    ind1 = sn_ptr->index;					/* save first index */

    strncpy(sn_ptr->name,cp->intr_str_name,20);			/* fill key with second street name */
    strncpy(sn_ptr->str_type,cp->intr_str_type,3);
    if (db(STRNAME_FILE_ID,READ_KEYS,&sn_key2,ISEQUAL,0) < 0)	/* read second street name */
	return(-1);						/* if not found, intersection not zoned */
    ind2 = sn_ptr->index;					/* save second index */

    if (ind1 < ind2) {						/* if first index less than second */
	in_ptr->index1 = ind1;					/* search for record with first street first */
	in_ptr->index2 = ind2;
    	strncpy(in_ptr->pre_dir1,cp->pckup_pre_dir,2);		/* fill in rest of key to search on */
    	strncpy(in_ptr->post_dir1,cp->pckup_post_dir,2);
    	strncpy(in_ptr->pre_dir2,cp->intr_pre_dir,2);
    	strncpy(in_ptr->post_dir2,cp->intr_post_dir,2);
    }
    else {							/* second index must be less than first */
	in_ptr->index1 = ind2;					/* search for record with second street first */
	in_ptr->index2 = ind1;
    	strncpy(in_ptr->pre_dir1,cp->intr_pre_dir,2);		/* fill in rest of key to search on */
    	strncpy(in_ptr->post_dir1,cp->intr_post_dir,2);
    	strncpy(in_ptr->pre_dir2,cp->pckup_pre_dir,2);
    	strncpy(in_ptr->post_dir2,cp->pckup_post_dir,2);
    }

    strncpy(in_ptr->city,cp->pckup_city,3);

    if (db(INT_FILE_ID,READ_KEYS,&in_key1,ISEQUAL,0) < 0)	/* read intersection record */
	return(-1);						/* if not found, intersection not zoned */

    *zip = in_ptr->zip;
    *st_zone = sz_to_zone(cp->fleet,in_ptr->subzone);		/* transform subzone to zone */
    return(*st_zone);							/* return zone */
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

    	sn_ptr = (struct cisam_sn *)req_buf;			/* pointer to street name structure */
    	ad_ptr = (struct cisam_ad *)req_buf;			/* pointer to address structure */

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
					sprintf(cp->dest_dmap,"%3d %s",ad_ptr->map_ref_page,ad_ptr->map_ref_block);
				else
					cp->dest_dmap[0] = '\0';
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
next_str_rec-
This function gets the next records in key sequence from the street name file.  The records are compared against the street_key
field to determine whether or not they fall in range.  The max parameter determines how many alternates we attempt to read.
***********************************************************************************************************************************/
next_str_rec(call_pt,alt_array,max)
struct cisam_cl *call_pt;						/* pointer to call structure with user supplied data */
struct alt_choice alt_array[];						/* alternate choice array for return */
int max;								/* max number of records to read */
{
    int i;								/* temporary variable for loops */

    for(i=0; ((strncmp(street_key,sn_ptr->name,strlen(street_key)) == 0) && i<max); i++) {	/* repeat for max while in range */
	strncpy(alt_array[i].str_name,sn_ptr->name,20);			/* save in alternate choice array */
	strncpy(alt_array[i].str_type,sn_ptr->str_type,3);
	if (db(STRNAME_FILE_ID,READ_KEYS,&sn_key2,ISNEXT,0) < 0) {	/* read next record */
	    i++;							/* increment count if at end */
	    break;
	}
    }
    return(i);								/* return count of alternates */
}



/***********************************************************************************************************************************
next_plc_rec-
This function gets the next records in key sequence from the placename file.  The records are compared against the street_key
field to determine whether or not they fall in range.  The max parameter determines how many alternates we attempt to read.
***********************************************************************************************************************************/
next_plc_rec(call_pt,alt_array,max)
struct cisam_cl *call_pt;						/* pointer to call structure with user supplied data */
struct alt_choice alt_array[];						/* alternate choice array for return */
int max;								/* max number of records to read */
{
    int i;								/* temporary variable for loops */

    for(i=0; ((strncmp(street_key,pl_ptr->name,strlen(street_key)) == 0) && i<max); i++) {	/* for max while in range */
	alt_array[i].low = pl_ptr->street_nbr;				/* fill up alternate array */
	strncpy(alt_array[i].plc_name,pl_ptr->name,20);
	strncpy(alt_array[i].str_name,pl_ptr->str_name,20);
	strncpy(alt_array[i].str_type,pl_ptr->str_type,3);
	strncpy(alt_array[i].str_pre,pl_ptr->str_pre_dir,2);
	strncpy(alt_array[i].str_post,pl_ptr->str_post_dir,2);
	strncpy(alt_array[i].str_city,pl_ptr->str_city,3);
	strncpy(alt_array[i].comment,pl_ptr->name,30);
	if (db(PLACENM_FILE_ID,READ_KEYS,&pl_key1,ISNEXT,0) < 0) {	/* read next record */
	    i++;							/* if end of file, add one more to count */
	    break;
	}
    }
    return(i);								/* return count */
}



/***********************************************************************************************************************************
next_intr_rec-
This function gets the next records in key sequence from the intersection file.  The records are compared against the street_key
field to determine whether or not they fall in range.  The max parameter determines how many alternates we attempt to read.
***********************************************************************************************************************************/
next_intr_rec(alt_array,max)
struct alt_choice alt_array[];						/* alternate choice array for return */
int max;								/* max number of records to read */
{
    int i;								/* temporary variable for loops */

    for(i=0; i<max; i++) {						/* repeat for max */
	if ((x1 > 0) && (x2 > 0)) {					/* have both indexes been supplied? */
	    if ((x1 != in_ptr->index1) || (x2 != in_ptr->index2))	/* yes, see if they both match */
		break;							/* they don't, stop here */
	}
	else if (x1 != in_ptr->index1)					/* index 1 in range? */
	    break;							/* no */
	fill_intr_buf(i,alt_array);					/* fill up alternate array */
	if (db(INT_FILE_ID,READ_KEYS,&in_key1,ISNEXT,0) < 0) {		/* read next record */
	    i++;							/* if end of file, add 1 to count */
	    break;
	}
    }
    return(i);								/* return count */
}



/***********************************************************************************************************************************
fill_intr_buf-
This function fills in the alternate array from the intersection structure.
***********************************************************************************************************************************/
fill_intr_buf(idx,alt_array)
int idx;							/* index into array to fill */
struct alt_choice alt_array[];					/* alternate choice array */
{
    strncpy(alt_array[idx].str_pre,in_ptr->pre_dir1,2);		/* fill it up */
    strncpy(alt_array[idx].str_name,in_ptr->name1,20);
    strncpy(alt_array[idx].str_type,in_ptr->type1,3);
    strncpy(alt_array[idx].str_post,in_ptr->post_dir1,2);
    strncpy(alt_array[idx].str_city,in_ptr->city,3);
    strncpy(alt_array[idx].int_pre,in_ptr->pre_dir2,2);
    strncpy(alt_array[idx].int_name,in_ptr->name2,20);
    strncpy(alt_array[idx].int_type,in_ptr->type2,3);
    strncpy(alt_array[idx].int_post,in_ptr->post_dir2,2);
    strncpy(alt_array[idx].comment,in_ptr->comment,30);
}



/***********************************************************************************************************************************
fill_alt_buf-
This function fills in the alternate array from the address structure.
***********************************************************************************************************************************/
fill_alt_buf(idx,alt_array)
int idx;							/* index into alternate array to fill */
struct alt_choice alt_array[];					/* alternate choice array */
{
    alt_array[idx].low = ad_ptr->low_nbr;			/* fill it up */
    alt_array[idx].high = ad_ptr->high_nbr;
    strncpy(alt_array[idx].str_pre,ad_ptr->str_pre_dir,2);
    strncpy(alt_array[idx].str_name,ad_ptr->str_name,20);
    strncpy(alt_array[idx].str_type,ad_ptr->str_type,3);
    strncpy(alt_array[idx].str_post,ad_ptr->str_post_dir,2);
    strncpy(alt_array[idx].str_city,ad_ptr->str_city,3);
    strncpy(alt_array[idx].comment,ad_ptr->str_comment,30);
}



/***********************************************************************************************************************************
next_adr_rec1-
This function gets the next records in key sequence from the address file.  The records are compared against the key fields to
determine whether or not they fall in range.  The max parameter determines how many alternates we attempt to read.
***********************************************************************************************************************************/
next_adr_rec1(call_pt,alt_array,max)
struct cisam_cl *call_pt;						/* pointer to call structure with user supplied data */
struct alt_choice alt_array[];						/* alternate choice array for return */
int max;								/* max number of records to read */
{
    int i,j;								/* temporary variables for loops */

    for(i=0; ((s_idx == ad_ptr->str_index) && (strncmp(st_predir,ad_ptr->str_pre_dir,2) == 0) &&	/* for max while in range */
		(strncmp(st_postdir,ad_ptr->str_post_dir,2) == 0) &&
		(strncmp(st_city,ad_ptr->str_city,3) == 0)) && i<max; i++) {
	fill_alt_buf(i,alt_array);					/* fill in alternate array */
	if (db(ADDRESS_FILE_ID,READ_KEYS,&ad_key1,ISNEXT,0) < 0) {	/* read next record */
	    i++;							/* if end of file, add 1 */
	    break;
	}
    }
    if (i > 0) {							/* if found, save last data for primary key */
	j = i - 1;
	s_high = alt_array[j].high;
	strncpy(s_pre,alt_array[j].str_pre,2);
	strncpy(s_post,alt_array[j].str_post,2);
	strncpy(s_city,alt_array[j].str_city,3);
    }
    return(i);								/* return count */
}



/***********************************************************************************************************************************
next_adr_rec2-
This function gets the next records in key sequence from the address file.  The records are compared against the key fields to
determine whether or not they fall in range.  The max parameter determines how many alternates we attempt to read.
***********************************************************************************************************************************/
next_adr_rec2(call_pt,alt_array,min,max)
struct cisam_cl *call_pt;						/* pointer to call structure with user supplied data */
struct alt_choice alt_array[];						/* alternate choice array for return */
int min,max;								/* starting and max count of records to read */
{
    int i;								/* temporary variable for loops */

/* && (call_pt->pckup_str_nbr >= ad_ptr->low_nbr) && (call_pt->pckup_str_nbr <= ad_ptr->high_nbr)) { */

    for(i=min; (s_idx == ad_ptr->str_index) && i<max; i++) {		/* while in range up to max */
	fill_alt_buf(i,alt_array);					/* fill alternate array with data */
	if (db(ADDRESS_FILE_ID,READ_KEYS,&ad_key2,ISNEXT,0) < 0) {	/* read next record */
	    i++;							/* if end of file, add 1 to count */
	    break;
	}
    }
    return(i);								/* return count */
}




/***********************************************************************************************************************************
read_first_key1-
This function is used to read the addres record with primary key.
***********************************************************************************************************************************/
read_first_key1(call_pt)
struct cisam_cl *call_pt;						/* pointer to call structure with user supplied data */
{
    ad_ptr->str_index = s_idx;						/* fill in key for address file */
    strncpy(ad_ptr->str_pre_dir,st_predir,2);
    strncpy(ad_ptr->str_post_dir,st_postdir,2);
    strncpy(ad_ptr->str_city,st_city,3);
    ad_ptr->high_nbr = 0;
    if (db(ADDRESS_FILE_ID,READ_KEYS,&ad_key1,ISGTEQ,0) < 0) {		/* try to read a record */
	return(-1);							/* if none found, stop here */
    }
    if ((s_idx != ad_ptr->str_index) || (strncmp(st_predir,ad_ptr->str_pre_dir,2) != 0) ||
		(strncmp(st_postdir,ad_ptr->str_post_dir,2) != 0) ||
		(strncmp(st_city,ad_ptr->str_city,3) != 0)) {		/* is it in range? */
	return(-1);							/* no */
    }
    else {
	return(0);							/* yes */
    }
}



/***********************************************************************************************************************************
prev_key1-
This function reads previous records for the primary key for the address file.
***********************************************************************************************************************************/
prev_key1(call_pt)
struct cisam_cl *call_pt;						/* pointer to call structure with user supplied data */
{
    int i;								/* temporary variable for loops */

    for(i=0; i<MAXALT; i++) {						/* try to read a full page */
	if (db(ADDRESS_FILE_ID,READ_KEYS,&ad_key1,ISPREV,0) < 0)	/* read previous record */
	    break;							/* if not found, stop here */
	else if ((s_idx != ad_ptr->str_index) || (strncmp(st_predir,ad_ptr->str_pre_dir,2) != 0) ||
		    (strncmp(st_postdir,ad_ptr->str_post_dir,2) != 0) ||
		    (strncmp(st_city,ad_ptr->str_city,3) != 0)) {	/* is it in range? */
	    db(ADDRESS_FILE_ID,READ_KEYS,&ad_key1,ISNEXT,0);		/* no, go forward to one that is */
	    break;
	}
    }
    return(i);								/* return count */
}




/***********************************************************************************************************************************
read_last_key1-
This function is called when reading previous records from the address file on the secondary key and we want to revert back to
the primary key.  The data for the primary key was stored when we went from key 1 to key 2.
***********************************************************************************************************************************/
read_last_key1()
{
    ad_ptr->str_index = s_idx;					/* fill key up with saved data */
    strncpy(ad_ptr->str_pre_dir,s_pre,2);
    strncpy(ad_ptr->str_post_dir,s_post,2);
    strncpy(ad_ptr->str_city,s_city,3);
    ad_ptr->high_nbr = s_high;
    if (db(ADDRESS_FILE_ID,READ_KEYS,&ad_key1,ISEQUAL,0) < 0)	/* read the record */
	return(-1);						/* not found */
    else
	return(0);						/* found it */
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
#ifdef DEBUG
			sprintf(trace_str,"zone_addr: Fleet in call structure not valid: %c \n",flt);
			TRACE(trace_str);
#endif
			x = -1;
    }
    return(x);						/* return zone */
}



/***********************************************************************************************************************************
choose_alt-
This function is called when a user wants to choose one of the alternates.  The information from the choice is then placed
back into the call pointer.
***********************************************************************************************************************************/
choose_alt(file_flag,key_flag,call_pt,alt_array,indx,pck_dst,zip)
int *file_flag;								/* flag to determine which file to access */
int *key_flag;								/* flag to determine which key to use */
struct cisam_cl *call_pt;						/* pointer to call structure with user supplied data */
struct alt_choice alt_array[];						/* alternate choice array for return */
int indx;								/* index into alternate array */
int pck_dst;								/* pickup or destination flag */
int *zip;
{
    int x;								/* temporary variable */

    if (*file_flag == STRNAME_FILE_ID) {				/* is this street name choice? */
	if (*key_flag == 1) {						/* yes, is it first street? */
	    strncpy(st_name,alt_array[indx].str_name,20);		/* yes, fill call structure */
	    strncpy(st_type,alt_array[indx].str_type,3);
	}
	else if (*key_flag == 2) {						/* is it second street? */
	    strncpy(call_pt->intr_str_name,alt_array[indx].str_name,20);	/* yes, fill call structure */
	    strncpy(call_pt->intr_str_type,alt_array[indx].str_type,3);
	}
    }
    else if (*file_flag == ADDRESS_FILE_ID) {				/* is this address file? */
/**
/*	st_nbr = &alt_array[indx].low;
/*	strncpy(st_predir,alt_array[indx].str_pre,2);			/* yes, fill call structure from alternate array */
/*	strncpy(st_postdir,alt_array[indx].str_post,2);
/*	strncpy(st_city,alt_array[indx].str_city,3);
/*	strncpy(st_name,alt_array[indx].str_name,20);
/*	strncpy(st_type,alt_array[indx].str_type,3);
/*	strncpy(st_comment,alt_array[indx].comment,30);
**/
	if (pck_dst == PICKUP_ADDR) {
	    call_pt->pckup_str_nbr = alt_array[indx].low;
	    strncpy(call_pt->pckup_city, alt_array[indx].str_city,3);
	    strncpy(call_pt->pckup_pre_dir,alt_array[indx].str_pre,2);
	    strncpy(call_pt->pckup_post_dir,alt_array[indx].str_post,2);
	    strncpy(call_pt->pckup_str_name,alt_array[indx].str_name,20);
	    strncpy(call_pt->pckup_str_type,alt_array[indx].str_type,3);
	    strncpy(call_pt->pckup_adr_cmnt,alt_array[indx].comment,30);
	}
	else {
	    call_pt->dest_str_nbr = alt_array[indx].low;
	    strncpy(call_pt->dest_city, alt_array[indx].str_city,3);
	    strncpy(call_pt->dest_pre_dir,alt_array[indx].str_pre,2);
	    strncpy(call_pt->dest_post_dir,alt_array[indx].str_post,2);
	    strncpy(call_pt->dest_str_name,alt_array[indx].str_name,20);
	    strncpy(call_pt->dest_str_type,alt_array[indx].str_type,3);
	    strncpy(call_pt->dest_adr_cmnt,alt_array[indx].comment,30);
	}
	    
    }
    else if (*file_flag == PLACENM_FILE_ID) {				/* is this placename file? */
	strncpy(st_name,alt_array[indx].plc_name,20);			/* yes, fill call structure from alternate array */
	strncpy(st_city,alt_array[indx].str_city,3);
    }
    else if (*file_flag == INT_FILE_ID) {				/* is this intersection file? */
	strncpy(st_predir,alt_array[indx].str_pre,2);			/* yes, fill call structure from alternate array */
	strncpy(st_name,alt_array[indx].str_name,20);
	strncpy(st_type,alt_array[indx].str_type,3);
	strncpy(st_postdir,alt_array[indx].str_post,2);
	strncpy(st_city,alt_array[indx].str_city,3);
	strncpy(st_comment,alt_array[indx].comment,30);
	strncpy(call_pt->intr_pre_dir,alt_array[indx].int_pre,2);
	strncpy(call_pt->intr_str_name,alt_array[indx].int_name,20);
	strncpy(call_pt->intr_str_type,alt_array[indx].int_type,3);
	strncpy(call_pt->intr_post_dir,alt_array[indx].int_post,2);
    }

    x = zone_addr(call_pt,pck_dst,zip);						/* return zone if zoneable */
    return(x);
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


