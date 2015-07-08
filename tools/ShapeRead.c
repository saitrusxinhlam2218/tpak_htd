/***********************************************************************
*                 RCS INFO
*
*  @(#)  $RCSfile: ShapeRead.c,v $
*  @(#)  $Revision: 1.1 $
*  @(#)  $Date: 2002/03/25 17:17:29 $
*  @(#)  $Author: jwelch $
*  @(#)  $Source: /taxi-scan/taxi_proj/cvs/taxi-scan/src/tools/ShapeRead.c,v $
*
*  Copyright (c) 1994 - MobileSoft Consulting, Inc. Woodinville WA
*
***********************************************************************/
#ident "@(#) head:$RCSfile: ShapeRead.c,v $	$Revision: 1.1 $"

static char rcsid[] = "$Id: ShapeRead.c,v 1.1 2002/03/25 17:17:29 jwelch Exp $";

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include "mads_types.h"
#include "mads_isam.h"
#include "language.h"
#include "key.h"
#include "mad_error.h"
#include "path.h"
#include "Zone_private.h"
#include "Zonepoly_db.h"

char tmp_req_buf[5120];

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

main( int argc, char **argv )
{
  FILE           *fpShape, *fpData, *fpOutput;
  int            rc = 0, i;
  char           in_buf[8192], out_buf[8192];
  char           txt_buf[512];
  MAIN_FILE_HDR  *pMainHdr;
  int            rec_length = 0;
  double         *pXMax, *pXMin, *pYMax, *pYMin;
  RECORD_HDR     *pRecHdr;
  ARC_HDR        *pArc;
  int            *NumParts;
  int            *NumPoints;
  int            *pPart1;
  int            offset = 0;
  double         *ptX;
  double         *ptY;
  double         avgX, avgY;
  int            FirstPoint;
  ZONE_POLY_REC  zone_poly;
  char           *pZone;
  short          zn_nbr, seq_nbr;
  
  if (( fpShape = fopen("./zon_irregular.shp", "r+")) == NULL )
    {
      fprintf( stderr, "Unable to open SHAPE file\n" );
      exit(1);
    }

  if ( ( rc = db_open( ZONEPOLY_FILE_ID, ISINOUT + ISMANULOCK ) ) != SUCCESS )
    {
      fprintf( stderr, "Unable to open ZONEPOLY file\n" );
      exit(1);
    }
  
  if (( fpOutput = fopen("./output.txt", "a")) == NULL )
    {
      fprintf( stderr, "Unable to open OUTPUT file\n" );
      exit(1);
    }

  if (( fpData = fopen("./zon_irregular.txt", "r")) == NULL )
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



  while ( rc == 1 )
    {
      bzero( in_buf, sizeof( in_buf ) );
      /* 8 byte record headers */
      rc = fread( in_buf, 8, 1, fpShape );
      
      pRecHdr = ( RECORD_HDR * ) in_buf;
      
      fprintf( stderr, "Record #: %d\tRecord Length: %d\n",
	      ntohl( pRecHdr->RecordNbr ), ntohl( pRecHdr->Length ) );

      rec_length = ntohl( pRecHdr->Length ) * 2;
      bzero( in_buf, sizeof( in_buf ) );

      /* read in ARC_HDR       */
      fread( in_buf, rec_length, 1, fpShape );


      fgets( txt_buf, 100, fpData );
      if ( txt_buf[0] == '\r' )
	exit(1);
      
      zn_nbr = atoi( txt_buf );
      
      pXMin = (double *)(in_buf + 4);
      pYMin = (double *)(in_buf + 12);
      pXMax = (double *)(in_buf + 20);
      pYMax = (double *)(in_buf + 28);
      NumParts = (int *)(in_buf + 36);
      NumPoints = (int *)(in_buf + 40);
      pPart1 = (int *)(in_buf + 44);

      offset = 48;
      FirstPoint = 1;
      seq_nbr = 1;
      for ( i = 0; i < *NumPoints; i++ )
	{
	  ptX = (double *)(in_buf + offset);
	  offset += 8;
	  ptY = (double *)(in_buf + offset);
	  offset += 8;
	  fprintf( stderr, "X: %lf\tY: %lf\n", *ptX, *ptY );
	  zone_poly.fleet = 'A';
	  zone_poly.nbr = zn_nbr;
	  zone_poly.seq_nbr = seq_nbr;
	  zone_poly.pointx = *ptX;
	  zone_poly.pointy = *ptY;
	  db_write( ZONEPOLY_FILE_ID, &zone_poly );
	  ++seq_nbr;
	}

    }
  fclose( fpShape );
}
  
  
