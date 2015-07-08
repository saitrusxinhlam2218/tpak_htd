/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: Address.h,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1998/05/12 19:17:22 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/include/Address.h,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: Address.h,v $	$Revision: 1.2 $"


#ifndef Address_H
#define Address_H

 
#include "Object.h"
#include "taxi_error.h"

/* ATTRIBUTES */

#define ADR_INDEX	 	0
#define ADR_STREET_INDEX	1
#define ADR_STREET_ID		2
#define ADR_CITY_INDEX		3
#define ADR_CITY_ALIAS 		4
#define ADR_ODD_EVEN		5
#define ADR_STREET_HI_NBR	6
#define ADR_STREET_LO_NBR	7
#define ADR_ZIP			8
#define ADR_SUBZONE		9
#define ADR_MAP_INFO		10
#define ADR_PLACES		11
#define ADR_COMMENT		12
#define ADR_STREET_TYPE		13

/* key indicies */

#define ADR_INDEX_KEY 0			/* general key */
#define ADR_STR_NBR_LOC_KEY 1	/* address Completion */
#define ADR_STR_LOC_KEY 2		/* address listing <ALT-address> */
#define ADR_ZONE_ADDRESS_KEY 3	/* search for the exact address */
#define ADR_LOC_INDEX_KEY 4		/* find address in a specific city */
#define ADR_PRIMARY_KEY ADR_INDEX_KEY

/* length's */

#define CITY_CODE_LEN		3
#define CITY_NAME_LEN		3
#define CITY_ALIAS_LEN		6
#define ZIP_CODE_LEN		10
#define STREET_DIR_LEN		2
#define STREET_NAME_LEN		20
#define STREET_TYPE_LEN		3
#define APARTMENT_LEN		5
#define MAP_COORD_LEN		4
#define MAP_REF_BLOCK_LEN	4

#define PHONE_NBR_LEN		18
#define PHONE_EXT_LEN		6

#define ADR_COMMENT_LEN		30

/* errors */

#define NO_SUBZONE		ERR_OBJ_BEGIN+1

#define ADR_SUBZONE_NOT_FOUND	err_encode(ADR_OBJ, NO_SUBZONE)

#define ADR_END_OF_LIST		err_encode(ADR_OBJ, END_OF_LIST)
#define ADR_NULL_ARG		err_encode(ADR_OBJ,NULL_ARG)
#define ADR_INVALID_ARG		err_encode(ADR_OBJ, INVALID_ARG)


/* typedefs for ADDRESS struct */

typedef int INDEX;
typedef char APARTMENT[APARTMENT_LEN+1];
typedef char CITY_NAME[CITY_NAME_LEN+1];
typedef char CITY_CODE[CITY_CODE_LEN+1];
typedef char MAP_COORD[MAP_COORD_LEN+1];
typedef char STREET_DIRECTION[STREET_DIR_LEN+1];
typedef char STREET_NAME[STREET_NAME_LEN+1];
typedef char STREET_TYPE[STREET_TYPE_LEN+1];

typedef HNDL ADR_CUR_HNDL;
	

typedef struct _map_info_t
{
   char			coord[MAP_COORD_LEN+1];
   short		ref_page;
   char			ref_block[MAP_REF_BLOCK_LEN+1];
} MAP_INFO;

typedef struct _street_id_
{
	  char	pre_dir[STREET_DIR_LEN+1];
	  char  name[STREET_NAME_LEN+1];
	  char  type[STREET_TYPE_LEN+1];
	  char  post_dir[STREET_DIR_LEN+1];
} STREET_ID;

typedef int ADDRESS_PHONE;

typedef struct _street_address
{
        int street_nbr;
        char street_nbr_suffix;
/*
 * 	Next set of four elements 
 *      should be changed to STREET_ID
 *      until then to have a cast into 
 * 	STREET_ID street_pre_dir has moved
 *      from postion 3 ==> 1 in the group.
 *      WARNING: Keep this group together!
 */ 
        STREET_DIRECTION street_pre_dir;
        STREET_NAME street_name;
        STREET_TYPE street_type;
        STREET_DIRECTION street_post_dir;
/*
 *	STREET_ID street;
 */
        APARTMENT apt;
        CITY_NAME city_name;
        char zip[ZIP_CODE_LEN+1];
} STREET_ADDRESS;

typedef struct Phone_temp
{
	char phone_ext[PHONE_EXT_LEN+1];
	char phone_nbr[PHONE_NBR_LEN+1];
} PHONE;



/* public function prototypes */
RET_STATUS Address_open_list(
 int str_nbr, int str_idx, char *city_alias, ADR_CUR_HNDL *);
RET_STATUS Address_read_list(ADR_CUR_HNDL, ADR_REC_HNDL);
RET_STATUS Address_close_list(ADR_CUR_HNDL *cursor);
RET_STATUS Address_get_subzone(	/* subzone for a specific address */
 int str_nbr, int str_idx, int locality_idx, char * str_type,char *subzone_buf,ADR_REC_HNDL);
 

/* public maintenance function prototypes */
RET_STATUS Address_alloc(ADR_REC_HNDL *);
RET_STATUS Address_free(ADR_REC_HNDL *);
        /* for db records/objects */
RET_STATUS Address_set_rec_info(ADR_REC_HNDL, int attr, HNDL buf);
RET_STATUS Address_get_rec_info(ADR_REC_HNDL, int attr, HNDL buf);
RET_STATUS Address_release_locks(void);
RET_STATUS Address_read(ADR_REC_HNDL, int mode, int key_type);
RET_STATUS Address_add(ADR_REC_HNDL);
RET_STATUS Address_update(ADR_REC_HNDL); /* updt existing record */
RET_STATUS Address_delete(ADR_REC_HNDL);
 
#endif	// Address_H
