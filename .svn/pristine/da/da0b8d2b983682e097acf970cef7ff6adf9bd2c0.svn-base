/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: ShapetoStreet.c,v $
*  @(#)  $Revision: 1.2 $
*  @(#)  $Date: 2004/02/02 19:12:09 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/tools/ShapetoStreet.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: ShapetoStreet.c,v $	$Revision: 1.2 $"

static char rcsid[] = "$Id: ShapetoStreet.c,v 1.2 2004/02/02 19:12:09 jwelch Exp $";

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <locale.h>
#include <ctype.h>
#include <unistd.h>
#include <math.h>
#include "mads_types.h"
#include "mads_isam.h"
#include "language.h"
#include "key.h"
#include "mad_error.h"
#include "path.h"
#include "Zone_private.h"
#include "Zonepoly_db.h"
#include "GeoAddr_Addr_db.h"
#include "Street_db.h"
#include "Address_db.h"

char tmp_req_buf[5120];
char sCity[16];
char sDirPath[64];
char sFileName[32];

typedef struct _main_file_hdr_
{
  int         FileCode;
  int         Unused1;
  int         Unused2;
  int         Unused3;
  int         Unused4;
  int         Unused5;
  int         FileLength;
  int         Version;
  int         ShapeType;
  int         unused;
  double        Xmin;
  double        Ymin;
  double        Xmax;
  double        Ymax;
  int         Unused6;
} MAIN_FILE_HDR;

typedef struct _RECORD_HDR_
{
  int         RecordNbr;
  int         Length;
} RECORD_HDR;

typedef struct _ARC_HDR_
{
  int         ShapeType;
  double      Xmin;
  double      Ymin;
  double      Xmax;
  double      Ymax;
  int         NumParts;
  int         NumPoints;
} ARC_HDR;

typedef struct _POINT_DEF_
{
  int        ptX;
  int        ptY;
} POINT_DEF;

POINT_DEF  PointsColl[100];

main( int argc, char **argv )
{
  FILE           *fpShape, *fpData, *fpOutput;
  int            rc = 0, i, rs;
  char           in_buf[8192], out_buf[8192];
  char           txt_buf[1024];
  MAIN_FILE_HDR  *pMainHdr;
  int            rec_length = 0;
  double         *pXMax, *pXMin, *pYMax, *pYMin;
  int            iFromLeft, iToLeft, iFromRight, iToRight;
  RECORD_HDR     *pRecHdr;
  ARC_HDR        *pArc;
  int            *NumParts;
  int            *NumPoints;
  int            *pPart1;
  int            offset = 0;
  double         *ptX;
  double         *ptY;
  double         avgX, avgY, StartX, StartY, EndX, EndY;
  int            FirstPoint;
  char           *pAddressTxt;
  unsigned char           strName[64];
  int            nbrFromLeft,nbrFromRight,nbrToLeft,nbrToRight, iZone;
  int            c, strname_counter;
  char sLocale[80];
  GEOADDR_ADDR_REC geo_addr;
  GEOADDR_LINE_REC geo_line;
  GEOADDR_POINT_REC geo_point;
  STREET_NAME_REC  sname;
  ADDRESS_REC      adr;
  LOC_REC          loc;
  char           sLoc[64];
  char           sSubZone[16];
  char           sComment[64];
  char           FilePath[64];
  
  parse_cmd( argc, argv );
  
    init_mgr_locale();

    db_open(GEOADDR_ADDR_FILE_ID, DB_READ_WRITE|DB_MANUAL_LOCK);
    db_open(GEOADDR_LINE_FILE_ID, DB_READ_WRITE|DB_MANUAL_LOCK);
    db_open(GEOADDR_POINT_FILE_ID, DB_READ_WRITE|DB_MANUAL_LOCK);
    db_open(STRNAME_FILE_ID, DB_READ_WRITE|DB_MANUAL_LOCK);
    db_open(SUBZONE_FILE_ID, DB_READ_WRITE|DB_MANUAL_LOCK);
    db_open(ADDRESS_FILE_ID, DB_READ_WRITE|DB_MANUAL_LOCK);
    db_open(LOCALITY_FILE_ID, DB_READ_WRITE|DB_MANUAL_LOCK);
    

  sprintf(FilePath, "%s/%s.txt", sDirPath, sFileName );
  if (( fpData = fopen(FilePath, "r")) == NULL )
    {
      fprintf( stderr, "Unable to open DATA file\n" );
      exit(1);
    }    

  sprintf(FilePath, "%s/%s.out", sDirPath, sFileName );
  if (( fpOutput = fopen(FilePath, "a")) == NULL )
    {
      fprintf( stderr, "Unable to open OPUTPUT file\n");
      exit(1);
    }
      
  while ( 1 )
    {

  
      bzero( &sname, sizeof(STREET_NAME_REC) );
      bzero( &adr, sizeof(ADDRESS_REC) );
      bzero( &loc, sizeof(LOC_REC) );
      
      bzero(txt_buf, 1024);

      fgets( txt_buf, 200, fpData );
      
      if ( txt_buf[0] == '\0' )
	exit(1);
      
      if ( strlen(txt_buf) == 0 )
	exit(2);
      
      iFromLeft = 0;
      iToLeft = 0;
      iZone = 0;
      bzero( sLoc, 64 );
      bzero( sComment, 64 );
      bzero( sSubZone, 16 );
      
      AddressTokens( txt_buf, sname.name, &iFromLeft, &iToLeft, sComment, sLoc, &iZone );

      //      printf( "Str %s From %d To %d Comment %s\tCity %s Zone %d\n",
      //	      sname.name, iFromLeft, iToLeft, sComment, sLoc, iZone );
      strcpy( adr.str_name, sname.name );
      adr.str_index = sname.index;
      
      if ( !ValidLocality( sLoc ) )
	{
	  // generate exception to file
	  fprintf(fpOutput, "Locality not found for %s\n", sLoc);
	  continue; // keep going
	}

      if ( !ValidZone( iZone, sSubZone ) )
	{
	  // generate exception to file
	  fprintf(fpOutput,"Subzone not found for %d\n", iZone);
	  continue;
	}

      //      printf("City %s\t Subzone %s\n", sLoc, sSubZone);
      if ( !ValidAddress( sname.name, iFromLeft, iToLeft, sSubZone, sLoc, sComment ) )
	{
	  // generate exception to file - duplicate entry, overlapping entry
	  fprintf(fpOutput, "Entry exists already %s\n", sname.name);
	  continue;
	}

      fprintf(fpOutput, "Inserted %s\n", sname.name);

    }
  fclose( fpData );
  fclose( fpOutput);
}

ValidZone( int Zone, char *sSubZone )
{
  SUBZONE_REC sz_rec;
  int rc = 0;
  
  bzero( &sz_rec, sizeof(SUBZONE_REC) );

  rc = db_read_key( SUBZONE_FILE_ID, &sz_rec, &sz_key1, ISFIRST );
  while( rc == SUCCESS )
    {
      if ( sz_rec.zone_fleet8 == Zone )
	{
	  strcpy( sSubZone, sz_rec.nbr );
	  return(1); // found subzone for zone
	}
      else
	rc = db_read_key( SUBZONE_FILE_ID, &sz_rec, &sz_key1, ISNEXT );
    }

  return(0); // didn't find subzone for zone
}

ValidAddress( char *StreetName, int NbrStart, int NbrEnd, char *sSubZone, char *sCity, char *sComment )
{
  STREET_NAME_REC str_rec;
  ADDRESS_REC addr_rec;
  int         db_mode;
  bzero( &str_rec, sizeof(STREET_NAME_REC) );
  bzero( &addr_rec, sizeof(ADDRESS_REC) );
  
  strcpy( str_rec.name, StreetName );

  if ( db_read_key( STRNAME_FILE_ID, &str_rec, &sn_key2, ISEQUAL ) != SUCCESS )
    {
      // Add streetname
      str_rec.rec_type = '1'; // main record
      
      db_write( STRNAME_FILE_ID, &str_rec );

      db_read_key( STRNAME_FILE_ID, &str_rec, &sn_key2, ISEQUAL );
      
      str_rec.index = str_rec.record_nbr;
      
      db_update( STRNAME_FILE_ID, &str_rec );

      addr_rec.str_index = str_rec.record_nbr;
      strcpy( addr_rec.str_name, str_rec.name );
      addr_rec.odd_even = ADDRESS_BOTH;
      addr_rec.low_nbr = NbrStart;
      addr_rec.high_nbr = NbrEnd;
      strcpy( addr_rec.str_subzone, sSubZone );
      addr_rec.str_x_coord = 0;
      addr_rec.str_y_coord = 0;
      strcpy(addr_rec.str_city, sCity );
      strcpy(addr_rec.str_comment, sComment);      

      // Add the first address segment for this street name
      db_write( ADDRESS_FILE_ID, &addr_rec );
      
    }
  else
    {
      // streetname exists
      addr_rec.str_index = str_rec.index;
      db_mode = ISGTEQ;
      while ( db_read_key( ADDRESS_FILE_ID, &addr_rec, &ad_key2, db_mode ) == SUCCESS )
	{
	  // possible matching address segment
	  if ( strcmp( addr_rec.str_name, str_rec.name ) )
	    break;
	  
	  if ( !strncmp( addr_rec.str_city, sCity, 3 ) )
	    {
	      // Cities match
	      //	      printf( "Found existing address segment for %s %s\n",
	      //		      addr_rec.str_name, addr_rec.str_city );
	      return(0);
	    }
	  else
	    db_mode = ISNEXT;
	}
      
      // no matching address segment so add it
      bzero( &addr_rec, sizeof(ADDRESS_REC) );
      addr_rec.str_index = str_rec.index;
      strcpy( addr_rec.str_name, str_rec.name );
      addr_rec.odd_even = ADDRESS_BOTH;
      addr_rec.low_nbr = NbrStart;
      addr_rec.high_nbr = NbrEnd;
      strcpy( addr_rec.str_subzone, sSubZone );
      addr_rec.str_x_coord = 0;
      addr_rec.str_y_coord = 0;
      strcpy(addr_rec.str_city, sCity );
      strcpy(addr_rec.str_comment, sComment);

      // Add the first address segment for this street name
      db_write( ADDRESS_FILE_ID, &addr_rec );      
    }
  
  return(1);
}


ValidLocality( char *Locality )
{
  LOC_REC loc_rec;

  bzero( &loc_rec, sizeof( LOC_REC ) );

  strcpy( loc_rec.code, Locality );

  if ( db_read_key( LOCALITY_FILE_ID, &loc_rec, &lo_key1, ISEQUAL ) == 0 )
    return(1); // locality found
  else
    return(0); // not found
}

  
AddressTokens( char *pAddress, char *Strname, int *FromLeft, int *ToLeft, char *comment,
	       char *loc, int *Zone )
{
  char *p;
  int i;

  // File format is as follows:
  // <streetname>\t<zone_nbr>\t<start_nbr>\t<end_nbr>\t<comment>\t<locality>
  
  p = (char *)strtok( pAddress, "\t" ); // Street name

  strcpy( Strname, p );

  p = (char *)strtok( NULL, "\t" ); // zone_nbr
  *Zone = atoi( p );

  p = (char *)strtok( NULL, "\t" ); // FromLeft
  *FromLeft = atoi( p );

  p = (char *)strtok( NULL, "\t" ); // ToLeft
  *ToLeft = atoi( p );

  p = (char *)strtok( NULL, "\t" ); // Comment

  strcpy( comment, p );
  
  p = (char *)strtok( NULL, "\t" ); // Locality
  if ( p == NULL )
    {
      strncpy( loc, comment, 3 );
      *(loc + 4) = '\0';
      bzero(comment, 64);
    }
  else
    {
      strncpy( loc, p, 3 );
      *(loc + 4) = '\0';
    }
}
  

FindLocality( char *pLoc, LOC_REC *pLocRec )
{
  int rs = 0;

  db_open(LOCALITY_FILE_ID, DB_READ_WRITE|DB_MANUAL_LOCK);      
  rs = db_read_key( LOCALITY_FILE_ID, &pLocRec, &lo_key1, ISFIRST );
  while ( rs == 0 )
    {
      if ( !strcmp( pLocRec->subzone, pLoc ) )
        break;  // found locality record
      else
        rs = db_read_key( LOCALITY_FILE_ID, &pLocRec, &lo_key1, DB_NEXT );
    }
  db_close(LOCALITY_FILE_ID);

  if ( rs != 0 )
    return(-1);
  else
    return(1);
}

parse_cmd(argc,argv)
int argc;
char **argv;
{
	int c;
	char version[80];
	extern char *optarg;

	if ( argc < 3 )
	  exit(-1);
	
	while ((c = getopt(argc, argv, "f:p:h")) != -1 )
	  {
	    switch (c){
	    case 'f':
	      strcpy( sFileName, optarg );
	      break;
	    case 'p':
	      strcpy( sDirPath, optarg );
	      break;
	    case '?':
	    case 'h':
	      printf("Usage: StreetImport -f<filename> -p<path>\n");
	      exit;
	      break;
	    default:
	      break;
	    }
	  }
}
