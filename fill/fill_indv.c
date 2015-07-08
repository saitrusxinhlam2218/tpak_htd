/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: fill_indv.c,v $
*  @(#)  $Revision: 1.4 $
*  @(#)  $Date: 1998/05/12 19:16:13 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/fill/fill_indv.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: fill_indv.c,v 1.4 1998/05/12 19:16:13 taxiadm Exp $";

#define KEY_SRC
#include <stdio.h>
#include "taxipak.h"
#include "Indv_db.h"

void
fill_indv_key(char *from, char *to)
{
	register int  offset = 0;
	INDV_REC *indv = (INDV_REC *)from;

	/* struct INDIVIDUAL */
	/* struct INDIVIDUAL.INDIVIDUAL_ID */
	/* struct INDIVIDUAL.INDIVIDUAL_ID.ACCOUNT_ID */

    stchar(
	 &indv->individual_id.acct_id.fleet,
	 &to[offset],sizeof(indv->individual_id.acct_id.fleet));
	offset += sizeof(indv->individual_id.acct_id.fleet);

	stchar(indv->individual_id.acct_id.nbr,
	 &to[offset],sizeof(indv->individual_id.acct_id.nbr)-1);
	offset += sizeof(indv->individual_id.acct_id.nbr)-1;

	/* end struct INDIVIDUAL.INDIVIDUAL_ID.ACCOUNT_ID */

	stchar(
	 indv->individual_id.name, &to[offset], sizeof(indv->individual_id.name)-1);
	offset += sizeof(indv->individual_id.name)-1;

	stchar(
	 indv->subacct_nbr, &to[offset], sizeof(indv->subacct_nbr)-1);
	offset += sizeof(indv->subacct_nbr)-1;

	/* end struct INDIVIDUAL.INDIVIDUAL_ID */

	/* ... */
	/* end struct INDIVIDUAL */
}

void
fill_indv(char *from, char *to, int  inaction)
{
	register int  offset = 0;
	INDV_REC *indv = (INDV_REC *)from;

	/* struct INDIVIDUAL */
	/* struct INDIVIDUAL.INDIVIDUAL_ID */
	/* struct INDIVIDUAL.INDIVIDUAL_ID.ACCOUNT_ID */

    stchar(
	 &indv->individual_id.acct_id.fleet,
	 &to[offset],sizeof(indv->individual_id.acct_id.fleet));
	offset += sizeof(indv->individual_id.acct_id.fleet);

	stchar(
	 indv->individual_id.acct_id.nbr,
	 &to[offset],sizeof(indv->individual_id.acct_id.nbr)-1);
	offset += sizeof(indv->individual_id.acct_id.nbr)-1;

	/* end struct INDIVIDUAL.INDIVIDUAL_ID.ACCOUNT_ID */

	stchar(
	 indv->individual_id.name, &to[offset], sizeof(indv->individual_id.name)-1);
	offset += sizeof(indv->individual_id.name)-1;

	/* end struct INDIVIDUAL.INDIVIDUAL_ID */

	stchar(indv->subacct_nbr, &to[offset], sizeof(indv->subacct_nbr)-1);
	offset += sizeof(indv->subacct_nbr)-1;

    stchar(
	 &indv->can_charge, &to[offset], sizeof(indv->can_charge));
	offset += sizeof(indv->can_charge);
	
	/* end struct INDIVIDUAL */
}

void
unfill_indv(char *to, char *from)
{
	register int  offset = 0, i;
	INDV_REC *indv = (INDV_REC *)to;


	/* struct INDIVIDUAL */
	/* struct INDIVIDUAL.INDIVIDUAL_ID */
	/* struct INDIVIDUAL.INDIVIDUAL_ID.ACCOUNT_ID */

	indv->individual_id.acct_id.fleet = from[offset];
	offset += sizeof(indv->individual_id.acct_id.fleet);

	ldchar(
	 &from[offset],
	 sizeof(indv->individual_id.acct_id.nbr)-1,indv->individual_id.acct_id.nbr);
	offset += sizeof(indv->individual_id.acct_id.nbr)-1;

	/* end struct INDIVIDUAL.INDIVIDUAL_ID.ACCOUNT_ID */

	ldchar(
	 &from[offset],
	 sizeof(indv->individual_id.name)-1, indv->individual_id.name);
	offset += sizeof(indv->individual_id.name)-1;

	/* end struct INDIVIDUAL.INDIVIDUAL_ID */

	ldchar( 
	 &from[offset], sizeof(indv->subacct_nbr)-1, indv->subacct_nbr);
	offset += sizeof(indv->subacct_nbr)-1;

	indv->can_charge =  from[offset];
	offset += sizeof(indv->can_charge);
	
	/* end struct INDIVIDUAL */
}


