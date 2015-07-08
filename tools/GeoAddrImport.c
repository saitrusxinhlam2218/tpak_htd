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


main( int argc, char **argv )
{
  FILE           *fpShape, *fpData, *fpOutput;
  int            rc = 0, i, rs;
  char           in_buf[8192], out_buf[8192];
  char           txt_buf[1024];

  int            rec_length = 0;
  double         *pXMax, *pXMin, *pYMax, *pYMin;
  int            iFromLeft, iToLeft, iFromRight, iToRight;

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
  int            addr_index;
  int            nbrFromLeft,nbrFromRight,nbrToLeft,nbrToRight;
  int            c, strname_counter;
  int            copyidx;  
  char sLocale[80];
  GEOADDR_ADDR_REC geo_addr;
  GEOADDR_LINE_REC geo_line;
  GEOADDR_POINT_REC geo_point;
  STREET_NAME_REC  sname;
  ADDRESS_REC      adr;
  LOC_REC          loc;
  char           sLoc[64];
  char           FilePath[64];
  char           *p = NULL;
  char           Strname[32];
  char           Locality[32];
  
  parse_cmd( argc, argv );
  
    init_mgr_locale();

    db_open(GEOADDR_ADDR_FILE_ID, DB_READ_WRITE|DB_MANUAL_LOCK);
    db_open(GEOADDR_LINE_FILE_ID, DB_READ_WRITE|DB_MANUAL_LOCK);
    db_open(GEOADDR_POINT_FILE_ID, DB_READ_WRITE|DB_MANUAL_LOCK);

    sprintf( FilePath, "./geo_addr.txt" );
    if (( fpData = fopen(FilePath, "r")) == NULL )
    {
      fprintf(stderr, "Unable to open DATA file\n");
      exit(1);
    }

    while (  1 )
      {
	bzero(txt_buf, 1024);
	fgets( txt_buf, 200, fpData );
	if ( txt_buf[0] == '\0')
	  exit(1);
	if ( strlen(txt_buf) == 0 )
	  exit;

	p = (char *)strtok(txt_buf, "|");
	addr_index = atoi( p);

	p = (char *)strtok(NULL, "|");
	iFromLeft = atoi(p);

	p = (char *)strtok(NULL, "|");
	iToLeft = atoi(p);

	p = (char *)strtok(NULL, "|");
	iFromRight = atoi(p);

	p = (char *)strtok(NULL, "|");
	iToRight = atoi(p);

	p = (char *)strtok(NULL, "|");
	strcpy(sname.name, p);

	// munge up streetname
	bzero(Strname, 32);
	copyidx = 0;
	for (i = 0; i < strlen(sname.name); i++)
	  {
	    if ( (unsigned char)(*(sname.name + i)) == 132 )
	      *(Strname + copyidx - 1) = 0xc4; // a umlaut
	    else if ( (unsigned char)(*(sname.name + i)) == 150 )
	      *(Strname + copyidx - 1) = 0xd6;  // o umlaut
	    else // copy regular character
	      {
		*(Strname + copyidx) = *(sname.name + i);
		++copyidx;
	      }
	  }
	Strname[copyidx] = '\0';
	
	p = (char *)strtok(NULL, "|");
	strcpy(sLoc, p);

	// munge up locality
	bzero(Locality, 32);
	copyidx = 0;
	for (i = 0; i < strlen(sLoc); i++)
	  {
	    if ( (unsigned char)(*(sLoc + i)) == 132 )
	      *(Locality + copyidx - 1) = 0xc4; // a umlaut
	    else if ( (unsigned char)(*(sLoc + i)) == 150 )
	      *(Locality + copyidx - 1) = 0xd6;  // o umlaut
	    else // copy regular character
	      {
		*(Locality + copyidx) = *(sLoc + i);
		++copyidx;
	      }
	  }
	Locality[copyidx] = '\0';	    

	bzero(&geo_addr, sizeof(GEOADDR_ADDR_REC));
	geo_addr.addr_idx = addr_index;
	geo_addr.from_left = iFromLeft;
	geo_addr.to_left = iToLeft;
	geo_addr.from_right = iFromRight;
	geo_addr.to_right = iToRight;
	strcpy(geo_addr.sname, Strname);
	strcpy(geo_addr.city, Locality);

	db_write(GEOADDR_ADDR_FILE_ID, &geo_addr);
      }
	

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
