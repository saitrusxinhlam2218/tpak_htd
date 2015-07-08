/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: acad_indv_import.c,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 1998/12/15 15:01:52 $
*  @(#)  $Author: taxiadm $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/auto_call/acad_indv_import.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/

static char rcsid[] = "$Id: acad_indv_import.c,v 1.2 1998/12/15 15:01:52 taxiadm Exp $";


#include <stdio.h>
#include <time.h>
#include <cursesX.h>
#include <string.h>
#include <sys/types.h>
#include "mads_types.h"
#include "mads_isam.h"
#include "language.h"
#include "key.h"
#include "mad_error.h"
#include "path.h"

extern char req_buf[];
char tmp_req_buf[BUFSIZ];

struct keydesc null_key = {
  ISNODUPS,
  0
};


main( int argc, char **argv[] )
{
  int   rc = 0;
  int   done = 0;
  int   mode;
  ACCOUNT_REC   Account;
  ACAD_REC      Acad;
  INDV_REC      Indv;
  int   rec_count = 0;

  db_open( ACCOUNT_FILE_ID, ISINOUT | ISMANULOCK );
  db_open( ACAD_FILE_ID, ISINOUT | ISMANULOCK );
  db_open( INDV_FILE_ID, ISINOUT | ISMANULOCK );

  bzero( &Account, sizeof( ACCOUNT_REC ) );
  mode = ISFIRST;
  Account.nbr[0] = '\0';
  while ( rc == 0 )
    {
      bzero( &Acad, sizeof( ACAD_REC ) );
      bzero( &Indv, sizeof( INDV_REC ) );
      rc = db_read_key( ACCOUNT_FILE_ID, &Account, &null_key, mode );
      if ( rc == SUCCESS )
	{
	  if ( Account.autobook_enabled == YES )
	    {
	      ++rec_count;
	      Acad.id.account_id.fleet = Account.fleet;
	      strcpy(Acad.id.account_id.nbr, Account.nbr);
	      Acad.id.index = 1;
	      Acad.address.street_nbr = Account.str_nbr;
	      strcpy(Acad.address.street_name, Account.str_name);
	      strcpy(Acad.address.apt, Account.apt_room);
	      strcpy(Acad.address.city_name, Account.city);
	      strcpy(Acad.phone.phone_nbr, Account.phone);
	      
	      /* Add the new ACAD */
	      db_write( ACAD_FILE_ID, &Acad );
	      
	      Indv.individual_id.acct_id.fleet = Account.fleet;
	      strcpy(Indv.individual_id.acct_id.nbr, Account.nbr);
	      strcpy(Indv.individual_id.name, Account.name);
	      strcpy(Indv.subacct_nbr ,"01");
	      Indv.can_charge = YES;
	      
	      /* Add the new INDV */
	      db_write( INDV_FILE_ID, &Indv );
	    }
	}
      mode = ISNEXT;      
    }
  printf( "Account records searched and converted -- %d\n", rec_count );
  db_close( ACAD_FILE_ID );
  db_close( ACCOUNT_FILE_ID );
  db_close( INDV_FILE_ID );
}
	  
	  
