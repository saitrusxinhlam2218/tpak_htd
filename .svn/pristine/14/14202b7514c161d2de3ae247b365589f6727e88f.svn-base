/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: fill_acad.c,v $
*  @(#)  $Revision: 1.4 $
*  @(#)  $Date: 1998/05/12 19:16:12 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/fill/fill_acad.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: fill_acad.c,v 1.4 1998/05/12 19:16:12 taxiadm Exp $";

#define KEY_SRC

#include <isam.h>
#include "Account_Address_db.h"

void
fill_acad_key( char *from, char *to )
{
  register int  offset = 0;
  ACCOUNT_ADDRESS	*acad = (ACCOUNT_ADDRESS *)from;
  
  /* ACCOUNT_ADDRESS struct */
  /* ACCOUNT_ADDRESS.ACCOUNT_ID struct */
  
  stchar(&acad->id.account_id.fleet,
	 &to[offset],sizeof(acad->id.account_id.fleet));	  /* fleet indicator */
  offset += sizeof(acad->id.account_id.fleet);
  
  stchar(
	 acad->id.account_id.nbr,&to[offset],sizeof(acad->id.account_id.nbr)-1);
  offset += sizeof(acad->id.account_id.nbr)-1;
  
  /* end ACCOUNT_ADDRESS.ACCOUNT_ID struct */
  
  stlong(acad->id.index,&to[offset]);
  offset += LONGSIZE;
  
  /* .... */
  
  /* end ACCOUNT_ADDRESS struct */  
  
}

fill_acad( char *from, char *to, int action )
{
	register int  offset = 0;
	int i;
	ACCOUNT_ADDRESS	*acad = (ACCOUNT_ADDRESS *)from;

	/* ACCOUNT_ADDRESS struct */


	/* ACCOUNT_ADDRESS.ACCOUNT_ID struct */

	stchar(&acad->id.account_id.fleet,
	 &to[offset],sizeof(acad->id.account_id.fleet));	  /* fleet indicator */
	offset += sizeof(acad->id.account_id.fleet);

	stchar(
	 acad->id.account_id.nbr,&to[offset],sizeof(acad->id.account_id.nbr)-1);
	offset += sizeof(acad->id.account_id.nbr)-1;

	/* end ACCOUNT_ADDRESS.ACCOUNT_ID struct */

	stlong(acad->id.index,&to[offset]);
	offset += LONGSIZE;

	/* ACCOUNT_ADDRESS.ADDRESS struct */

	stlong(acad->address.street_nbr, &to[offset]);
	offset += LONGSIZE;

	stchar(&acad->address.street_nbr_suffix,
	 &to[offset], sizeof(acad->address.street_nbr_suffix));
	offset += sizeof(acad->address.street_nbr_suffix);

	stchar(acad->address.street_pre_dir,
	 &to[offset], sizeof(acad->address.street_pre_dir)-1);
	offset += sizeof(acad->address.street_pre_dir)-1;

	stchar(acad->address.street_name,
	 &to[offset], sizeof(acad->address.street_name)-1);
	offset += sizeof(acad->address.street_name)-1;

	stchar(acad->address.street_type,
	 &to[offset], sizeof(acad->address.street_type)-1);
	offset += sizeof(acad->address.street_type)-1;

	stchar(acad->address.street_post_dir,
	 &to[offset], sizeof(acad->address.street_post_dir)-1);
	offset += sizeof(acad->address.street_post_dir)-1;

	stchar(acad->address.apt, &to[offset], sizeof(acad->address.apt)-1);
	offset += sizeof(acad->address.apt)-1;

	stchar(acad->address.city_name,
	 &to[offset], sizeof(acad->address.city_name)-1);
	offset += sizeof(acad->address.city_name)-1;

	stchar(acad->address.zip, &to[offset], sizeof(acad->address.zip)-1);
	offset += sizeof(acad->address.zip)-1;

	/* end ACCOUNT_ADDRESS.ADDRESS struct */


	/* ACCOUNT_ADDRESS.PHONE struct */

	stchar(acad->phone.phone_nbr,
	 &to[offset], sizeof(acad->phone.phone_nbr)-1);
	offset += sizeof(acad->phone.phone_nbr)-1;

	stchar(acad->phone.phone_ext,
	 &to[offset], sizeof(acad->phone.phone_ext)-1);
	offset += sizeof(acad->phone.phone_ext)-1;

	/* end ACCOUNT_ADDRESS.PHONE struct */

	/* end ACCOUNT_ADDRESS struct */  
}

unfill_acad( char *to, char *from )
{
	int i;
	register int  offset = 0;
	ACCOUNT_ADDRESS	*acad = (ACCOUNT_ADDRESS *)to;

	/* ACCOUNT_ADDRESS struct */


	/* ACCOUNT_ADDRESS.ACCOUNT_ID struct */

	acad->id.account_id.fleet = from[offset];
	offset += sizeof(acad->id.account_id.fleet);

	ldchar(
	 &from[offset],sizeof(acad->id.account_id.nbr)-1,acad->id.account_id.nbr);
	offset += sizeof(acad->id.account_id.nbr)-1;

	/* end ACCOUNT_ADDRESS.ACCOUNT_ID struct */

	acad->id.index = ldlong(&from[offset]);
	offset += LONGSIZE;

	/* ACCOUNT_ADDRESS.ADDRESS struct */

	acad->address.street_nbr = ldlong(&from[offset]);
	offset += LONGSIZE;

	acad->address.street_nbr_suffix = from[offset];
	offset += sizeof(acad->address.street_nbr_suffix);

	ldchar(
	 &from[offset],sizeof(acad->address.street_pre_dir)-1,
	 acad->address.street_pre_dir);
	offset += sizeof(acad->address.street_pre_dir)-1;

	ldchar(
	 &from[offset],sizeof(acad->address.street_name)-1,
	 acad->address.street_name);
	offset += sizeof(acad->address.street_name)-1;

	ldchar(
	 &from[offset],sizeof(acad->address.street_type)-1,
	 acad->address.street_type);
	offset += sizeof(acad->address.street_type)-1;

	ldchar(
	 &from[offset],sizeof(acad->address.street_post_dir)-1,
	 acad->address.street_post_dir);
	offset += sizeof(acad->address.street_post_dir)-1;

	ldchar(
	 &from[offset],sizeof(acad->address.apt)-1, acad->address.apt);
	offset += sizeof(acad->address.apt)-1;

	ldchar(
	 &from[offset],sizeof(acad->address.city_name)-1, acad->address.city_name);
	offset += sizeof(acad->address.city_name)-1;

	ldchar(
	 &from[offset],sizeof(acad->address.zip)-1, acad->address.zip);
	offset += sizeof(acad->address.zip)-1;

	/* end ACCOUNT_ADDRESS.ADDRESS struct */


	/* ACCOUNT_ADDRESS.PHONE struct */

	ldchar(
	 &from[offset],sizeof(acad->phone.phone_nbr)-1, acad->phone.phone_nbr);
	offset += sizeof(acad->phone.phone_nbr)-1;

	ldchar(
	 &from[offset],sizeof(acad->phone.phone_ext)-1, acad->phone.phone_ext);
	offset += sizeof(acad->phone.phone_ext)-1;

	/* end ACCOUNT_ADDRESS.PHONE struct */

	/* end ACCOUNT_ADDRESS struct */  

}
