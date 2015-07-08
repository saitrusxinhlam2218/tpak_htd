/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: ShapeMatch.c,v $
*  @(#)  $Revision: 1.1 $
*  @(#)  $Date: 2002/03/25 17:17:29 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/tools/ShapeMatch.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: ShapeMatch.c,v $	$Revision: 1.1 $"

static char rcsid[] = "$Id: ShapeMatch.c,v 1.1 2002/03/25 17:17:29 jwelch Exp $";


#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <locale.h>
#include <ctype.h>
#include "mads_types.h"
#include "mads_isam.h"
#include "Address_db.h"
#include "Street_db.h"
#include "language.h"
#include "key.h"
#include "mad_error.h"
#include "path.h"

char tmp_req_buf[5120];

int main( int argc, char **argv )
{
  FILE         *fpShapeSeg;
  int          rc = 0, i, do_update;
  char           txt_buf[512];
  ADDRESS_REC    addr_rec;
  STREET_NAME_REC    str_rec;
  char           *pAddressTxt;
  int            low_nbr,hi_nbr,hi_sav,city_code;
  char           odd_even;
  char           strName[64];
  char           strCity[16];
  int            ptX, ptY;
  
  if (( fpShapeSeg = fopen("./output.txt", "r+")) == NULL )
    {
      fprintf( stderr, "Unable to open file\n" );
      exit(1);
    }

  db_open(ADDRESS_FILE_ID, DB_READ_WRITE|DB_MANUAL_LOCK);
  db_open(STRNAME_FILE_ID, DB_READ_WRITE|DB_MANUAL_LOCK);
  while (1)
    {
      fgets( txt_buf, 200, fpShapeSeg );
      
      pAddressTxt = (char *) strtok( txt_buf, "," );  // ODD/EVEN
      odd_even = *pAddressTxt;
      pAddressTxt = (char *) strtok( NULL, "," );    // NbrLow
      low_nbr = atoi( pAddressTxt );
      pAddressTxt = (char *) strtok( NULL, "," );   // NbrHigh
      hi_nbr = atoi( pAddressTxt );
      if ( hi_nbr < low_nbr )
        {
          hi_sav = hi_nbr;
          hi_nbr = low_nbr;
          low_nbr = hi_sav;
        }
      pAddressTxt = (char *) strtok( NULL, "," );   // street name
      strcpy( strName, pAddressTxt );
      pAddressTxt = (char *) strtok( NULL, "," );   // city code
      city_code = atoi( pAddressTxt );
      pAddressTxt = (char *) strtok( NULL, "," );   // X coord
      ptX = atoi( pAddressTxt );
      pAddressTxt = (char *) strtok( NULL, "," );   // Y coord
      ptY = atoi( pAddressTxt );

      switch (city_code) {
      case 180:
        strcpy( strCity, "STO" );
        break;
      case 182:
        strcpy( strCity, "NAC" );
        break;
      case 114:
        strcpy( strCity, "UPP" );
        break;
      case 115:
        strcpy( strCity, "VAL" );
        break;
      case 117:
        strcpy( strCity, "ÖAK" );
        break;
      case 120:
        strcpy( strCity, "VÄR" );
        break;
      case 123:
        strcpy( strCity, "JÄR" );
        break;
      case 125:
        strcpy( strCity, "EKÖ" );
        break;
      case 126:
        strcpy( strCity, "HUD" );
        break;
      case 127:
        strcpy( strCity, "BOT" );
        break;
      case 128:
        strcpy( strCity, "SAL" );
        break;
      case 136:
        strcpy( strCity, "HAN" );
        break;
      case 138:
        strcpy( strCity, "TYR" );
        break;
      case 139:
        strcpy( strCity, "BRO" );
        break;
      case 160:
        strcpy( strCity, "TÄB" );
        break;
      case 183:
        strcpy( strCity, "SUN" );
        break;
      case 184:
        strcpy( strCity, "SOL" );
        break;
      case 186:
        strcpy( strCity, "LID" );
        break;
      case 187:
        strcpy( strCity, "VAX" );
        break;
      case 191:
        strcpy( strCity, "SIG" );
        break;
      case 163:
        strcpy( strCity, "SOT" );
        break;
      case 162:
        strcpy( strCity, "DAN" );
        break;
      default:
        strcpy( strCity, "NON" );
        break;
      }

      printf( "%c %d %d %s %s %d %d\n", odd_even, low_nbr, hi_nbr, strName, strCity, ptX, ptY );
      bzero( &str_rec, sizeof( str_rec ) );
      strcpy( str_rec.name, strName );

      if ( ( rc = db_read_key( STRNAME_FILE_ID, &str_rec, &sn_key2, ISEQUAL ) ) == SUCCESS )
        {
          addr_rec.str_index = str_rec.index;
          addr_rec.high_nbr = 0;
          if ( ( rc = db_read_key( ADDRESS_FILE_ID, &addr_rec, &ad_key2, ISGTEQ ) ) == SUCCESS )
            {

              while ( str_rec.index == addr_rec.str_index )
                {
                  do_update = 0;                  
                  if ( !strcmp(addr_rec.str_city, strCity) )
                    {
                      // handle all cases where there is an overlap
                      // in the address range
                      if ( (addr_rec.low_nbr >= low_nbr) &&
                           (addr_rec.low_nbr <= hi_nbr) &&
                           (addr_rec.high_nbr <= hi_nbr) )
                        do_update = 1;
                      else if ( (addr_rec.low_nbr >= low_nbr) &&
                                (addr_rec.low_nbr <= hi_nbr ) )
                        do_update = 1;
                      else if ( (addr_rec.high_nbr <= hi_nbr) &&
                                (addr_rec.high_nbr >= low_nbr) )
                        do_update = 1;
                      else if ( (low_nbr >= addr_rec.low_nbr) &&
                                (low_nbr <= addr_rec.high_nbr) &&
                                (hi_nbr <= addr_rec.high_nbr) )
                        do_update = 1;
                      else if ( (low_nbr >= addr_rec.low_nbr) &&
                                (low_nbr <= addr_rec.high_nbr) )
                        do_update = 1;
                      else if ( (hi_nbr <= addr_rec.high_nbr) &&
                                (hi_nbr >= addr_rec.low_nbr) )
                        do_update = 1;
                      else if ( addr_rec.low_nbr == 999 && addr_rec.high_nbr == 999 )
                        do_update = 1;

                      if ( (do_update == 1 ) && (addr_rec.str_x_coord == 0) )
                        {
                          
                          addr_rec.str_x_coord = ptX;
                          addr_rec.str_y_coord = ptY;
                          db_update( ADDRESS_FILE_ID, &addr_rec );
                          addr_rec.high_nbr = 0;
                          rc = db_read_key( ADDRESS_FILE_ID, &addr_rec, &ad_key2, ISGTEQ );
                        }
                      else
                        rc = db_read_key( ADDRESS_FILE_ID, &addr_rec, &ad_key2, ISNEXT );
                    }
                  else
                    rc = db_read_key( ADDRESS_FILE_ID, &addr_rec, &ad_key2, ISNEXT );
                  if ( rc != SUCCESS )
                    break;
                }
            }
        }
      
    }
}  

