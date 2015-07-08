#ifdef ANSIC
/*
#endif
//----------------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------------//
// Denna fil innehåller funktioner för Datumbyte mellan Bessel och WGS 84. Detta sker med			//
// en 3-D Helmerttransformation med 7 parametrar (Bursa-Wolf).							//
// 														//
// void wgs84ToBessel (double geoXF,double geoYF,double geoZF,double *geoXT,double *geoYT,double *geoZT);	//
// Transformerar geocentriska koordinater på WGS84 till Bessel							//
//														//
// void besselToWgs84 (double geoXF,double geoYF,double geoZF,double *geoXT,double *geoYT,double *geoZT);	//
// Transformerar geocentriska koordinater på Bessel till WGS84							//
//														//
// Copyright © 1999, SWEGIS. All rights reserved.								//
//														//
// Date: 1999-09-02												//
//														//
// Author: Mikael Elmquist, mikael@swegis.com									//
//														//
// Adapted to ANSI-C by Tommy Ericson, TEIT AB, tommy.ericson@teit.se						//
//														//
//----------------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------------//
#ifdef ANSIC
*/
#endif

#include "bessel.h"
#include <math.h>


#ifdef ANSIC
/*
#endif
//--------------------------------------------------------------------//
//	Byter datum från WGS 84 (geoXF, geoYF, geoZF) till Bessel
//	(geoXT,geoYT,geoZT)
//--------------------------------------------------------------------//
#ifdef ANSIC
*/
#endif


const long double wgs84ToBesseldX = -419.375;
const long double wgs84ToBesseldY =  -99.352;
const long double wgs84ToBesseldZ = -591.349;

const long double wgs84ToBesselFiX =   (((0.850458/(3600))/360)*2*3.141592653589793238462643383280);
const long double wgs84ToBesselFiY =   (((1.817245/(3600))/360)*2*3.141592653589793238462643383280);
const long double wgs84ToBesselFiZ =   (((-7.862245/(3600))/360)*2*3.141592653589793238462643383280);
const long double wgs84ToBesselDelta = 1.00000099496; /* // Skalkorrektion */

long double wgs84ToBesselR11;

long double wgs84ToBesselR12;


long double wgs84ToBesselR13;


long double wgs84ToBesselR21;

long double wgs84ToBesselR22;

long double wgs84ToBesselR23;

long double wgs84ToBesselR31;

long double wgs84ToBesselR32;

long double wgs84ToBesselR33;


void wgs84ToBessel (double geoXF,double geoYF,double geoZF,double *geoXT,double *geoYT,double *geoZT)
{
  /* init "constants" */

  wgs84ToBesselR11 = wgs84ToBesselDelta*(cos(wgs84ToBesselFiY)*cos(wgs84ToBesselFiZ));

  wgs84ToBesselR12 = wgs84ToBesselDelta*((cos(wgs84ToBesselFiX)*sin(wgs84ToBesselFiZ)) +
					 (sin(wgs84ToBesselFiX)*sin(wgs84ToBesselFiY)*cos(wgs84ToBesselFiZ)));

  wgs84ToBesselR13 = wgs84ToBesselDelta*(((double)-1.0* cos(wgs84ToBesselFiX)*sin(wgs84ToBesselFiY)*cos(wgs84ToBesselFiZ))+
					 (sin(wgs84ToBesselFiX)*sin(wgs84ToBesselFiZ)));

  wgs84ToBesselR21 = wgs84ToBesselDelta*((double)-1.0*cos(wgs84ToBesselFiY)*sin(wgs84ToBesselFiZ));

  wgs84ToBesselR22 = wgs84ToBesselDelta*((cos(wgs84ToBesselFiX)*cos(wgs84ToBesselFiZ)) -
					 (sin(wgs84ToBesselFiX)*sin(wgs84ToBesselFiY)*sin(wgs84ToBesselFiZ)));

  wgs84ToBesselR23 = wgs84ToBesselDelta*((sin(wgs84ToBesselFiX)*cos(wgs84ToBesselFiZ))+
					 (cos(wgs84ToBesselFiX)*sin(wgs84ToBesselFiY)*sin(wgs84ToBesselFiZ)));

  wgs84ToBesselR31 = wgs84ToBesselDelta*(sin(wgs84ToBesselFiY));

  wgs84ToBesselR32 = wgs84ToBesselDelta*((double)-1.0*sin(wgs84ToBesselFiX)*cos(wgs84ToBesselFiY));

  wgs84ToBesselR33 = wgs84ToBesselDelta*(cos(wgs84ToBesselFiX)*cos(wgs84ToBesselFiY));

  /* work */

  *geoXT = wgs84ToBesseldX + (wgs84ToBesselR11*geoXF) + (wgs84ToBesselR12*geoYF) + (wgs84ToBesselR13*geoZF);

  *geoYT = wgs84ToBesseldY + (wgs84ToBesselR21*geoXF) + (wgs84ToBesselR22*geoYF) + (wgs84ToBesselR23*geoZF);

  *geoZT = wgs84ToBesseldZ + (wgs84ToBesselR31*geoXF) + (wgs84ToBesselR32*geoYF) + (wgs84ToBesselR33*geoZF);

}

#ifdef ANSIC
/*
#endif
//--------------------------------------------------------------------//
//	Byter datum från Bessel (geoXF,geoYFgeoZF) till WGS 84
//	(geoXT,geoyT,geoZT)
//--------------------------------------------------------------------//
#ifdef ANSIC
*/
#endif


const long double besselToWgs84dX = 419.3836;
const long double besselToWgs84dY =  99.3335;
const long double besselToWgs84dZ = 591.3451;

long double besselToWgs84FiX;
long double besselToWgs84FiY;
long double besselToWgs84FiZ;
long double besselToWgs84Delta; /* // Skalkorrektion */

long double besselToWgs84R11;

long double besselToWgs84R12;


long double besselToWgs84R13;


long double besselToWgs84R21;

long double besselToWgs84R22;

long double besselToWgs84R23;

long double besselToWgs84R31;

long double besselToWgs84R32;

long double besselToWgs84R33;


void besselToWgs84 (double geoXF,double geoYF,double geoZF,double *geoXT,double *geoYT,double *geoZT)
{
  /* init "constants" */

  besselToWgs84FiX =   (((-0.850389/(3600))/360)*2*3.141592653589793238462643383280);
  besselToWgs84FiY =   (((-1.817277/(3600))/360)*2*3.141592653589793238462643383280);
  besselToWgs84FiZ =   (((7.862238/(3600))/360)*2*3.141592653589793238462643383280);
  besselToWgs84Delta = 0.99999900504;

  besselToWgs84R11 = besselToWgs84Delta*(cos(besselToWgs84FiY)*cos(besselToWgs84FiZ));

  besselToWgs84R12 = besselToWgs84Delta*((cos(besselToWgs84FiX)*sin(besselToWgs84FiZ)) +
					 (sin(besselToWgs84FiX)*sin(besselToWgs84FiY)*cos(besselToWgs84FiZ)));


  besselToWgs84R13 = besselToWgs84Delta*(((double)-1.0* cos(besselToWgs84FiX)*sin(besselToWgs84FiY)*cos(besselToWgs84FiZ))+
					 (sin(besselToWgs84FiX)*sin(besselToWgs84FiZ)));


  besselToWgs84R21 = besselToWgs84Delta*((double)-1.0*cos(besselToWgs84FiY)*sin(besselToWgs84FiZ));

  besselToWgs84R22 = besselToWgs84Delta*((cos(besselToWgs84FiX)*cos(besselToWgs84FiZ)) -
					 (sin(besselToWgs84FiX)*sin(besselToWgs84FiY)*sin(besselToWgs84FiZ)));

  besselToWgs84R23 = besselToWgs84Delta*((sin(besselToWgs84FiX)*cos(besselToWgs84FiZ))+
					 (cos(besselToWgs84FiX)*sin(besselToWgs84FiY)*sin(besselToWgs84FiZ)));

  besselToWgs84R31 = besselToWgs84Delta*(sin(besselToWgs84FiY));

  besselToWgs84R32 = besselToWgs84Delta*((double)-1.0*sin(besselToWgs84FiX)*cos(besselToWgs84FiY));

  besselToWgs84R33 = besselToWgs84Delta*(cos(besselToWgs84FiX)*cos(besselToWgs84FiY));

  /* work */

  *geoXT = besselToWgs84dX + (besselToWgs84R11*geoXF) + (besselToWgs84R12*geoYF) + (besselToWgs84R13*geoZF);

  *geoYT = besselToWgs84dY + (besselToWgs84R21*geoXF) + (besselToWgs84R22*geoYF) + (besselToWgs84R23*geoZF);

  *geoZT = besselToWgs84dZ + (besselToWgs84R31*geoXF) + (besselToWgs84R32*geoYF) + (besselToWgs84R33*geoZF);

}
