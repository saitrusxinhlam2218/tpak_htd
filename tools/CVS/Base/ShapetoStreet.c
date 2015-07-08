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
  int            nbrFromLeft,nbrFromRight,nbrToLeft,nbrToRight;
  int            c, strname_counter;
  char sLocale[80];
  GEOADDR_ADDR_REC geo_addr;
  GEOADDR_LINE_REC geo_line;
  GEOADDR_POINT_REC geo_point;
  STREET_NAME_REC  sname;
  ADDRESS_REC      adr;
  LOC_REC          loc;
  char           sLoc[64];
  char           FilePath[64];
  
  parse_cmd( argc, argv );
  
    init_mgr_locale();

    db_open(GEOADDR_ADDR_FILE_ID, DB_READ_WRITE|DB_MANUAL_LOCK);
    db_open(GEOADDR_LINE_FILE_ID, DB_READ_WRITE|DB_MANUAL_LOCK);
    db_open(GEOADDR_POINT_FILE_ID, DB_READ_WRITE|DB_MANUAL_LOCK);

    sprintf( FilePath, "%s/%s.shp", sDirPath, sFileName );
    if (( fpShape = fopen(FilePath, "r")) == NULL )
    {
      fprintf( stderr, "Unable to open SHAPE file\n" );
      exit(1);
    }

  //  if ( ( rc = db_open( ZONEPOLY_FILE_ID, ISINOUT + ISMANULOCK ) ) != SUCCESS )
  //    {
  //      fprintf( stderr, "Unable to open ZONEPOLY file\n" );
  //      exit(1);
  //    }
  
  if (( fpOutput = fopen("./output.txt", "a")) == NULL )
    {
      fprintf( stderr, "Unable to open OUTPUT file\n" );
      exit(1);
    }

  sprintf(FilePath, "%s/%s.csv", sDirPath, sFileName );
  if (( fpData = fopen(FilePath, "r")) == NULL )
    {
      fprintf( stderr, "Unable to open DATA file\n" );
      exit(1);
    }    



  bzero( in_buf, sizeof( in_buf ) );
  rc = fread( in_buf, 100, 1, fpShape );

  pMainHdr = ( MAIN_FILE_HDR * ) in_buf;

  pXMin = (double *)(in_buf + 36);
  pYMin = (double *)(in_buf + 44);
  pXMax = (double *)(in_buf + 52);
  pYMax = (double *)(in_buf + 60);
  fprintf( stderr, "File Code: %d\tFile Length: %d\tVersion: %d\tShape Type: %d\t\n",
	            ntohl( pMainHdr->FileCode), ntohl( pMainHdr->FileLength ),
		    pMainHdr->Version, pMainHdr->ShapeType );

  strname_counter = 1;







      
  while ( rc == 1 )
    {

  
      bzero( &sname, sizeof(STREET_NAME_REC) );
      bzero( &adr, sizeof(ADDRESS_REC) );
      bzero( &loc, sizeof(LOC_REC) );
      bzero( &geo_addr, sizeof(GEOADDR_ADDR_REC) );
      bzero( &geo_line, sizeof(GEOADDR_LINE_REC) );
      bzero( &geo_point, sizeof(GEOADDR_POINT_REC) );
      bzero( sLoc, 64 );
      
      bzero( in_buf, sizeof( in_buf ) );
      /* 8 byte record headers */
      rc = fread( in_buf, 8, 1, fpShape );
      
      pRecHdr = ( RECORD_HDR * ) in_buf;
      
      //      fprintf( stderr, "Record #: %d\tRecord Length: %d\n",
      //	      ntohl( pRecHdr->RecordNbr ), ntohl( pRecHdr->Length ) );

      rec_length = ntohl( pRecHdr->Length ) * 2;
      bzero( in_buf, sizeof( in_buf ) );

      /* read in ARC_HDR       */
      fread( in_buf, rec_length, 1, fpShape );

      bzero(txt_buf, 1024);

      fgets( txt_buf, 200, fpData );
      if ( txt_buf[0] == '\0' )
	exit(1);
      
      
      pXMin = (double *)(in_buf + 4);
      pYMin = (double *)(in_buf + 12);
      pXMax = (double *)(in_buf + 20);
      pYMax = (double *)(in_buf + 28);
      NumParts = (int *)(in_buf + 36);
      NumPoints = (int *)(in_buf + 40);
      pPart1 = (int *)(in_buf + 44);

      offset = 48;
      FirstPoint = 1;
      for ( i = 0; i < *NumPoints; i++ )
	{
	  ptX = (double *)(in_buf + offset);
	  offset += 8;
	  ptY = (double *)(in_buf + offset);
	  offset += 8;
          PointsColl[i].ptX = rintf( *ptX );
          PointsColl[i].ptY = rintf( *ptY );
          if (FirstPoint)
            {
              StartX = *ptX;
              StartY = *ptY;
              avgX = *ptX;
              avgY = *ptY;
              FirstPoint = 0;
            }
          else
            {
              avgX = (avgX + *ptX)/2;
              avgY = (avgY + *ptY)/2;
              EndX = *ptX;
              EndY = *ptY;
            }
	}

      if ( strlen(txt_buf) == 0 )
	exit;
      
      AddressTokens( txt_buf, sname.name, &iFromLeft, &iToLeft, &iFromRight, &iToRight, sLoc );
      if ( !strncmp(sname.name, "0", 1) )
	continue;
      
      strcpy( adr.str_name, sname.name );
      adr.str_index = sname.index;

      if ( ( iFromLeft == 0 ) && ( iToLeft == 0 ) &&  // no address segments defined
           ( iFromRight == 0 ) && ( iToRight == 0 ) )
        {
          continue;
        }

      strcpy( adr.str_city, sCity );


          bzero( &geo_addr, sizeof(GEOADDR_ADDR_REC) );
          bzero( &geo_line, sizeof(GEOADDR_LINE_REC) );
          bzero( &geo_point, sizeof(GEOADDR_POINT_REC) );

          // Add the Address Record

          geo_addr.from_left = iFromLeft;
          geo_addr.to_left = iToLeft;
          geo_addr.from_right = iFromRight;
          geo_addr.to_right = iToRight;
          strcpy( geo_addr.sname, sname.name );
          strcpy( geo_addr.city, adr.str_city );

          db_write(GEOADDR_ADDR_FILE_ID, &geo_addr );

          // Add the Line Record

          geo_line.addr_idx = geo_addr.addr_idx;
          db_write(GEOADDR_LINE_FILE_ID, &geo_line );

          // Finally, add all the points that define the Line

          for ( i = 0; i < *NumPoints; i++ )
            {
              bzero(&geo_point, sizeof(GEOADDR_POINT_REC));
              geo_point.line_idx = geo_line.line_idx;
              geo_point.seq_nbr = i;
              geo_point.point_x = PointsColl[i].ptX;
              geo_point.point_y = PointsColl[i].ptY;
              db_write(GEOADDR_POINT_FILE_ID, &geo_point);
            }
    }
  fclose( fpShape );
}
  
  
AddressTokens( char *pAddress, char *Strname, int *FromLeft, int *ToLeft, int *FromRight, int *ToRight, char *loc )
{
  char *p;
  int i;

  //  p = (char *)strtok( pAddress, "," ); // Length
  //  p = (char *)strtok( NULL, "," ); // Type

  p = (char *)strtok( pAddress, "," ); // Street name

  strcpy( Strname, p );

  //  for ( i = 0; i < strlen(Strname); i++ )
  //    {
  //      *(Strname + i) = Toupper(*(Strname + i));
  //      if ( *(Strname + i) == 'æ' )
  //        *(Strname + i) = 'Æ';
  //      else if ( *(Strname + i) == 'ø' )
  //        *(Strname + i)= 'Ø';
  //      else if ( *(Strname + i) == 'å' )
  //        *(Strname + i) = 'Å';
  //    }

  p = (char *)strtok( NULL, "," ); // secondary street name

  p = (char *)strtok( NULL, "," ); // FromLeft
  *FromLeft = atoi( p );

  p = (char *)strtok( NULL, "," ); // ToLeft
  *ToLeft = atoi( p );

  p = (char *)strtok( NULL, "," ); // FromRight
  *FromRight = atoi( p );

  p = (char *)strtok( NULL, "," ); // ToRight
  *ToRight = atoi( p );

  //  p = (char *)strtok( NULL, "," );
  //  p = (char *)strtok( NULL, "," );
  //  p = (char *)strtok( NULL, "," );
  //  p = (char *)strtok( NULL, "," );
  //  p = (char *)strtok( NULL, "," ); // Kommunenr

  strcpy( loc, "91" );
  //  if ( p != NULL )
  //    strcpy( loc, p );

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

	while ((c = getopt(argc, argv, "f:p:c:h")) != -1 )
	  {
	    switch (c){
	    case 'f':
	      strcpy( sFileName, optarg );
	      break;
	    case 'p':
	      strcpy( sDirPath, optarg );
	      break;
	    case 'c':
	      strcpy( sCity, optarg );
	      break;
	    case '?':
	    case 'h':
	      printf("Usage: ShapetoStreet -f<filename> -p<path> -c<city_abbrev\n");
	      exit;
	      break;
	    default:
	      break;
	    }
	  }
}
