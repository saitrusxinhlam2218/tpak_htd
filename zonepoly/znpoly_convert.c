#include <stdio.h>
#include <math.h>
#include "mad_error.h"
#include "taxipak.h"
#include "Object.h"
#include "Zone_public.h"
#include "Zone_private.h"
#include "mads_isam.h"
#include "key.h"
#include "language.h"
#include "dispatch.h"
#include "enhance.h"
#include "switch_ext.h"
#include <locale.h>
#include <langinfo.h>

void UTM_to_LatLong( double, double, double *, double * );

main(argc,argv)
int argc;
char **argv;
{
  ZONE_POLY_REC zonepoly_rec;
  ZONE_REC  zone_rec;
  char szDebug[32];
  int i;
  short zn_nbr;
  int mode;
  int done = 0;
  double latitude = 0.0;
  double longitude = 0.0;
  double rF1, rF2, iLat, iLong;
  int sum1=0;
  unsigned long checksum = 0;
  double wgsLat, wgsLon;
  
  // Open zonepoly table
  db_open(ZONEPOLY_FILE_ID, ISINOUT + ISMANULOCK);
  db_open(ZONE_FILE_ID, ISINOUT + ISMANULOCK);    
  mode = ISEQUAL;
  zonepoly_rec.fleet = 'H';
  zonepoly_rec.nbr = 1;
  zonepoly_rec.seq_nbr = 1;
  zn_nbr = 1;
  for ( i = 1; i < 1000; i++ )
    {
      zonepoly_rec.nbr = i;
      done = 0;
      sum1 = 0;
      checksum = 0;
      mode = ISEQUAL;
      while ( !done )
	{
	  if ( db_read_key(ZONEPOLY_FILE_ID, &zonepoly_rec, &znpoly_key1, mode ) == SUCCESS )
	    {
	      mode = ISNEXT;
	      if ( zonepoly_rec.nbr != i )
		{
		  done = TRUE;
		  zone_rec.fleet = 'H';
		  zone_rec.nbr = i;
		  if ( db_read_key(ZONE_FILE_ID, &zone_rec, &zn_key1, ISEQUAL) == SUCCESS )
		    {
		      sprintf(zone_rec.poly_checksum, "%04X", (unsigned short)(checksum & 0xFFFF));
		      db_update( ZONE_FILE_ID, &zone_rec );
		    }		  
		}
	      else
		{
		  // running checksum total
		  UTM_to_LatLong( zonepoly_rec.pointy, zonepoly_rec.pointx, &latitude, &longitude );
		  sprintf(szDebug, "%03d.%05ld", (int)longitude, (int)(longitude * 100000L)% 100000L);
		  strcpy(zonepoly_rec.longitude,szDebug);
		  
		  sprintf(szDebug, "%02d.%05ld", (int)latitude, (int)(latitude * 100000L)% 100000L);
		  strcpy(zonepoly_rec.latitude, szDebug);

		  db_update( ZONEPOLY_FILE_ID, &zonepoly_rec );
		  zonepoly_rec.longitude[3] = *nl_langinfo(RADIXCHAR);
		  zonepoly_rec.latitude[2] = *nl_langinfo(RADIXCHAR);
		  wgsLat = atof(zonepoly_rec.latitude);
		  wgsLon = atof(zonepoly_rec.longitude);
		  checksum += ((unsigned long) (wgsLon * 100000.0)) % 10000L;
		  checksum += ((unsigned long) (wgsLat * 100000.0)) % 10000L;
		  printf("Zone %d point %d Latitude %s Longitude %s Checksum %d\n", zonepoly_rec.nbr, zonepoly_rec.seq_nbr, zonepoly_rec.latitude, zonepoly_rec.longitude, checksum);
		}
	    }
	  else
	    done = 1;
	}
    }
	    
    
}

#define deg2rad (M_PI/180.0)
#define rad2deg (180.0/M_PI)
void UTM_to_LatLong( double UTMNorthingMeter, double UTMEastingMeter, double *LatDegree, double *LongDegree )
{
  double equatorial_radius=6378137;
  double eccentricity_squared=0.00669437999013;
  double eccentricity_prime_squared=(eccentricity_squared)/(1-eccentricity_squared);
  double eccentricity_e1 = (1-sqrt(1-eccentricity_squared))/(1+sqrt(1-eccentricity_squared));
  double utm_long_origin = 27.0;
  
  	const double k0 = 0.9996;

	double x = UTMEastingMeter - 500000.0; // remove 500,000 meter offset for longitude
	double y = UTMNorthingMeter;

	double M = y / k0;
	double mu = M/(equatorial_radius*(1-eccentricity_squared/4-3*eccentricity_squared*eccentricity_squared/64-5*eccentricity_squared*eccentricity_squared*eccentricity_squared/256));

	double phi1Rad = mu	+ (3*eccentricity_e1/2-27*eccentricity_e1*eccentricity_e1*eccentricity_e1/32)*sin(2*mu) 
				              + (21*eccentricity_e1*eccentricity_e1/16-55*eccentricity_e1*eccentricity_e1*eccentricity_e1*eccentricity_e1/32)*sin(4*mu)
				              + (151*eccentricity_e1*eccentricity_e1*eccentricity_e1/96)*sin(6*mu);
	double phi1 = phi1Rad*rad2deg;

	double N1 = equatorial_radius/sqrt(1-eccentricity_squared*sin(phi1Rad)*sin(phi1Rad));
	double T1 = tan(phi1Rad)*tan(phi1Rad);
	double C1 = eccentricity_prime_squared*cos(phi1Rad)*cos(phi1Rad);
	double R1 = equatorial_radius*(1-eccentricity_squared)/pow(1-eccentricity_squared*sin(phi1Rad)*sin(phi1Rad), 1.5);
	double D = x/(N1*k0);

	*LatDegree = phi1Rad - (N1*tan(phi1Rad)/R1)*(D*D/2-(5+3*T1+10*C1-4*C1*C1-9*eccentricity_prime_squared)*D*D*D*D/24
		            + (61+90*T1+298*C1+45*T1*T1-252*eccentricity_prime_squared-3*C1*C1)*D*D*D*D*D*D/720);
	*LatDegree = *LatDegree * rad2deg;

	*LongDegree = (D-(1+2*T1+C1)*D*D*D/6+(5-2*C1+28*T1-3*C1*C1+8*eccentricity_prime_squared+24*T1*T1)*D*D*D*D*D/120)/cos(phi1Rad);
	*LongDegree = *LongDegree * rad2deg + utm_long_origin;

}

#ifdef FOO
void UTM_to_LatLong( double Northing, double Easting, double *pLat, double *pLong )
{
  double a = 6378137.0;
  double eccSquared = 0.00669439;
  double k0 = 0.9996013;
  double FE = 500000.0;  /* False Easting value */
  double LongOrigin;
  double eccPrimeSquared, e1, mew1, phi1, rho1, theta1, lambda = 0.0, phi0 = 0.0;
  double D, T, C, M;

  /** this is valid for UTM Zone 32 only...currently only HTD is using this code to support
      navigation messages **/
  LongOrigin = 27.0 * deg2rad;
  
  eccPrimeSquared = (eccSquared)/(1.0-eccSquared);  
  e1 = (1.0-(sqrt(1.0 - eccPrimeSquared)))/(1.0+(sqrt(1.0+eccPrimeSquared)));
  M = ( Northing )/k0;
  mew1 = M/(a*(1.0-eccSquared/4.0-3.0*(pow(eccSquared,2.0))/64.0-5.0*pow(eccSquared,3.0)/256.0));

  /** ouch! Math to support this is available upon request :) **/
  phi1 = mew1+(3.0*e1/2.0-27.0*pow(e1,3.0)/32.0)*sin(2.0*mew1)+
    (21.0*pow(e1,2.0)/16.0-55.0*pow(e1,4.0)/32.0)*sin(4.0*mew1)+
    (151.0*pow(e1,3.0)/96.0)*sin(6.0*mew1)+
    (1097.0*pow(e1,4.0)/512.0)*sin(8.0*mew1);

  rho1 = a*(1.0-eccSquared)/pow((1.0-eccSquared*pow(sin(phi1),2.0)),1.5);

  theta1 = a/(sqrt(1.0-eccSquared*pow(sin(phi1),2.0)));

  D = (Easting - FE)/(theta1*k0);
  
  T = pow(tan(phi1), 2.0);

  C = eccPrimeSquared*pow(cos(phi1),2.0);

  /** here we calculate the longitude in radians and then convert to decimal degrees **/
  lambda = LongOrigin+
    (D-(1.0+2.0*T+C)*pow(D,3.0)/6.0+(5.0-2.0*C+28.0*T-3.0*pow(C,2.0)+8.0*eccPrimeSquared+24.0*pow(T,2.0))*pow(D,5.0))/cos(phi1);

  *pLong = lambda * rad2deg;

  /** here we calculate the latitude in radians and then convert to decimal degrees **/
  phi0 = phi1 - theta1*tan(phi1)/rho1*
    (pow(D,2.0)/2.0-(5.0+3.0*T+10.0*C-4.0*pow(C,2.0)-9.0*eccPrimeSquared)*pow(D,4.0)/24.0 +
     (61.0+90.0*T+298.0*C+45.0*pow(T,2.0)-252.0*eccPrimeSquared-3.0*pow(C,2.0))*pow(D,6.0)/720);

  *pLat = phi0 * rad2deg;
  
}
#endif
