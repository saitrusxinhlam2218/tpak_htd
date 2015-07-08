#ifdef ANSIC
/*
#endif
//----------------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------------//
// Denna fil innehÂller en funktioner f–r transformation frÂn valfritt koordinatsystem till		    //
// valfritt koordinatsystem. Funktionen (Transform) tar sex st parametrar. xF och yF ”r in koordinaterna    //
// och pXT, pYT ut- koordinaterna. Vidare skall ocksÂ tvÂ st. variabler av typen KoordSystem 		    //
// skickas med. Dessa anger alla parametrar om in- resp- ut- system					    //
// 													    //
//													    //	
// Copyright © 1999, SWEGIS. All rights reserved.							    //
//													    //
// Date: 1999-09-02											    //
//													    //
// Author: Mikael Elmquist, mikael@swegis.com								    //
//----------------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------------//
#ifdef ANSIC
*/
#endif



#include "transform.h"
#include <string.h>
#include "datumTrans.h"
#include "bessel.h"
#include "wgs84.h"



#ifdef ANSIC
void Transform(double xF,
	       double yF,
	       KoordSystem inSystem,
	       double *pXT,
	       double *pYT,
	       KoordSystem utSystem)
#else
void __declspec( dllexport ) Transform(double xF,
				       double yF,
				       KoordSystem inSystem,
				       double *pXT,
				       double *pYT,
				       KoordSystem utSystem)
#endif
{

#ifdef ANSIC
/*
#endif
  //-----------------------------------------------------
  //	Declare variables for this function
  //-----------------------------------------------------
#ifdef ANSIC
*/
#endif
	
  double xMid,yMid,yT,xT;
  
#ifdef ANSIC
/*
#endif
  //-----------------------------------------------------
  //	Add x and y shift to the in-coordinates, Exceptions;
  //	if the in-system is a long/lat system
  //-----------------------------------------------------
#ifdef ANSIC
*/
#endif
	
    if ((strcmp(inSystem.pSystemTyp,"Rikets System") == 0) &&
	((strcmp(inSystem.pKoordSystem,"Lat/Long (Bessel)") == 0)||
	 (strcmp(inSystem.pKoordSystem,"Lat/Long (WGS 84)") == 0)
	 ))
      {
#ifdef ANSIC
/*
#endif
	//yF = inpadXVertices[j];
	//xF = inpadYVertices[j];
#ifdec ANSIC
*/
#endif
      }
    else
      {
	yF = yF-inSystem.yTillagg;
	xF = xF-inSystem.xTillagg;
      }
	
#ifdef ANSIC
/*
#endif
  //---------------------------------------------------
  //	Transform coordinates RT 90 2.5 g V.
  //---------------------------------------------------
#ifdef ANSIC
*/
#endif
	
	
    if (strcmp(inSystem.pSystemTyp,"Rikets System") == 0)
      {
	if (strcmp(inSystem.pKoordSystem,"Lat/Long (Bessel)") == 0)
	  {
#ifdef ANSIC
/*
#endif
    //---------------------------------------------------
    // The in-system is long/lat on Bessel's ellipsoid. All
    // we have to do is to un-project the coordinates.
    // We also change the meridianDouble parameter to
    // reflect the projection that the coordinates now are.			
    // 
    //---------------------------------------------------

    // Change units from degrees to radians
#ifdef ANSIC
*/
#endif

	    xF = ((xF/360)*2*3.141592653589793238462643383280);
	    yF = ((yF/360)*2*3.141592653589793238462643383280);
			
#ifdef ANSIC
/*
#endif
	    // Project lat/long to 2.5 gon V
#ifdef ANSIC
*/
#endif
			
	    besselProject(xF,yF,&xMid,&yMid);
			
#ifdef ANSIC
/*
#endif
            // Change the meridianDouble
#ifdef ANSIC
*/
#endif
	    inSystem.meridian = ((17.564753086/400)*2*3.141592653589793238462643383280);
	  }
	else if (strcmp(inSystem.pKoordSystem,"Lat/Long (WGS 84)") == 0)
	  {
	    double geoXF,geoYF,geoZF;
	    double tempH;
#ifdef ANSIC
/*
#endif
	    //---------------------------------------------------
	    // The in-system is long/lat on the WGS 84 elipsiod. 
	    // We need to first get the three-deimesional x,y,z
	    // and transform these to the Bessel elipsoid. Once
	    // done we convert the x,y,z on Bessel to lat/long
	    // and project these to 2.5 g V. 
	    // We also chaneg the meridianDouble parameter to
	    // reclect the projection that the coordiantes now is.			
	    // 
	    //---------------------------------------------------

	    // Chaneg units from degrees to radians

#ifdef ANSIC
*/
#endif
	    xF = ((xF/360)*2*3.141592653589793238462643383280);
	    yF = ((yF/360)*2*3.141592653589793238462643383280);
			
#ifdef ANSIC
/*
#endif
	    // Get geocentric XYZ from WGS84 long/lat
	    // (We do work with any heights, so we send
	    // a height of 0)
#ifdef ANSIC
*/
#endif
			
	    wgs84GetXYZ(xF,yF,(double)0.00000,&geoXF,&geoYF,&geoZF);
			
			
#ifdef ANSIC
/*
#endif
	    // Transform from WGS 84 X,Y,Z to Bessel X,Y,Z
#ifdef ANSIC
*/
#endif

	    wgs84ToBessel (geoXF,geoYF,geoZF,&geoXF,&geoYF,&geoZF);
			
			
#ifdef ANSIC
/*
#endif
	    // Bet Bessel long/lat/h from geocentric XYZ
#ifdef ANSIC
*/
#endif
			
	    besselGetLongLat(geoXF,geoYF,geoZF,&xF,&yF,&tempH);
			
#ifdef ANSIC
/*
#endif
	    // Projects Bessel lat/long to 2.5 gon V
#ifdef ANSIC
*/
#endif
	    besselProject(xF,yF,&xMid,&yMid);
			
			
	    inSystem.meridian = ((17.564753086/400)*2*3.141592653589793238462643383280);
	  }
	else
	  {
#ifdef ANSIC
/*
#endif
	     //-----------------------------------------
	     // The coordinates is already in RT 90, we
	     // just copy the x and y.
	     //-----------------------------------------
#ifdef ANSIC
*/
#endif

	    xMid = xF;
	    yMid = yF;
	  }
      }
    else if (strcmp(inSystem.pSystemTyp,"Regionala System") == 0)
      {
#ifdef ANSIC
/*
#endif
		//-----------------------------------------
		// This section is not needed and therefore 
		// not included in this code version
		//-----------------------------------------
#ifdef ANSIC
*/
#endif
      }
    else if (strcmp(inSystem.pSystemTyp,"Lokala System") == 0)
      {
#ifdef ANSIC
/*
#endif
		//-------------------------------------------------------------
		// If there is a projection within the local system, change
		// the meridian to the meridian where the transformation
		// shall be done.
		//-------------------------------------------------------------
#ifdef ANSIC
*/
#endif

		
	if (inSystem.meridian != inSystem.inSystemMedelmeridian)
	  {			
	    besselChangeMeridian(&xF,&yF,inSystem.meridian,inSystem.inSystemMedelmeridian);
	    inSystem.meridian = inSystem.inSystemMedelmeridian;			
	  }
		
#ifdef ANSIC
/*
#endif
		//-----------------------------------------
		//	Add x,y shift
		//-----------------------------------------
#ifdef ANSIC
*/
#endif
		
	yF = yF+inSystem.inSystemYTillagg;
	xF = xF+inSystem.inSystemXTillagg;
		
#ifdef ANSIC
/*
#endif
	// Transform the coordinates		
#ifdef ANSIC
*/
#endif
		
		
	xMid =  inSystem.XTo +  inSystem.A*xF  +  inSystem.C*yF;
	yMid =	inSystem.YTo +  inSystem.B*xF  +  inSystem.D*yF;
		

	yMid = yMid-inSystem.utSystemYTillagg;
	xMid = xMid-inSystem.utSystemXTillagg;
		
#ifdef ANSIC
/*
#endif
	//-----------------------------------------
	//	If the out-coordinates from the transformation
	//	changed, then we use the new one from here.
	//-----------------------------------------
#ifdef ANSIC
*/
#endif

		
	inSystem.meridian = inSystem.utSystemMedelmeridian;
		
		
      }
	
#ifdef ANSIC
/*
#endif
	//---------------------------------------------
	//	The coordinates is now always in RT 90. The 
	// meridian can however be any of the standard
	// meridians
	//---------------------------------------------

	
	
	//---------------------------------------------
	//	Now we take our RT 90 coordinates (yMid, xMid
	//	and transform them to the out-system.
	//---------------------------------------------
#ifdef ANSIC
*/
#endif
	

  if (strcmp(utSystem.pSystemTyp,"Rikets System") == 0)
    {
		
      if (strcmp(utSystem.pKoordSystem,"Lat/Long (Bessel)") == 0)
	{
#ifdef ANSIC
/*
#endif
	  //-----------------------------------------
	  // The out-system is long/lat on the Bessel
	  // ellipsoid. All we have to do is to
	  // un-project the coordinates
	  //-----------------------------------------

	  //-----------------------------------------
	  //	Change meridian to 2.5 gon V
	  //-----------------------------------------
#ifdef ANSIC
*/
#endif

			
	  if (inSystem.meridian != ((17.564753086/400)*2*3.141592653589793238462643383280))
	    {
	      besselChangeMeridian(&xMid,&yMid,inSystem.meridian,((17.564753086/400)*2*3.141592653589793238462643383280));
	      inSystem.meridian = ((17.564753086/400)*2*3.141592653589793238462643383280);
	    }

#ifdef ANSIC
/*
#endif
	  // Un-project Bessel 2.5 gon V to lat/long
#ifdef ANSIC
*/
#endif
	  besselUnproject(xMid,yMid,&xT,&yT);
			
			
#ifdef ANSIC
/*
#endif
	  // Change units from radians to degrees
#ifdef ANSIC
*/
#endif
	  xT = ((xT/(2*3.141592653589793238462643383280))*360);
	  yT = ((yT/(2*3.141592653589793238462643383280))*360);

#ifdef ANSIC
/*
#endif
	  // Change out-meridian to in-meridian
#ifdef ANSIC
*/
#endif
	  utSystem.meridian = inSystem.meridian;
			
	}
      else if (strcmp(utSystem.pKoordSystem,"Lat/Long (WGS 84)") == 0)
	{
	  double geoXT,geoYT,geoZT, geoXF, geoYF, geoZF;
	  double tempH;
#ifdef ANSIC
/*
#endif
	  //-----------------------------------------
	  // The out-system is long/lat on the WGS 84
	  // ellipsoid. We need to first un-project
	  // the coordinates, get the geocentric
	  // x,y,z on the Bessel ellipsoid. We then
	  // transform these to x,y, z on WGS 84 and
	  // convert to long/lat.
	  //-----------------------------------------

	  //-----------------------------------------
	  //	Change in meridian to 2.5 gon V
	  //-----------------------------------------
#ifdef ANSIC
*/
#endif

			
	  if (inSystem.meridian != ((17.564753086/400)*2*3.141592653589793238462643383280))
	    {
	      besselChangeMeridian(&xMid,&yMid,inSystem.meridian,((17.564753086/400)*2*3.141592653589793238462643383280));
	      inSystem.meridian = ((17.564753086/400)*2*3.141592653589793238462643383280);
	    }
			
			
#ifdef ANSIC
/*
#endif
	  // Un-project Bessel 2.5 gon V to lat/long
#ifdef ANSIC
*/
#endif
	  besselUnproject(xMid,yMid,&xT,&yT);
			
#ifdef ANSIC
/*
#endif
	  // Get geocentriska XYZ from Bessel long/lat/height
#ifdef ANSIC
*/
#endif
	  besselGetXYZ(xT,yT,(double)0.000000,&geoXT,&geoYT,&geoZT);
			
#ifdef ANSIC
/*
#endif
	  // Transform Bessel XYZ to WGS84 XYZ
#ifdef ANSIC
*/
#endif
	  besselToWgs84 (geoXT,geoYT,geoZT,&geoXF,&geoYF,&geoZF);
			
			
#ifdef ANSIC
/*
#endif
	  // Get WGS 84 long/lat from geocentric XYZ
#ifdef ANSIC
*/
#endif
	  wgs84GetLongLat(geoXF,geoYF,geoZF,&xT,&yT,&tempH);
			
			
#ifdef ANSIC
/*
#endif
	  // Change units from radians to degrees			
#ifdef ANSIC
*/
#endif
	  xT = ((xT/(2*3.141592653589793238462643383280))*360);
	  yT = ((yT/(2*3.141592653589793238462643383280))*360);
			
			
#ifdef ANSIC
/*
#endif
	  // Change out-meridian to in-meridian
#ifdef ANSIC
*/
#endif
	  utSystem.meridian = inSystem.meridian;
	}
      else
	{
#ifdef ANSIC
/*
#endif
	  //-----------------------------------------
	  //	The out-system is RT 90, just copy the 
	  // coordinates
	  //-----------------------------------------
#ifdef ANSIC
*/
#endif
			
	  xT = xMid;
	  yT = yMid;
	}
    }
  else if (strcmp(utSystem.pSystemTyp,"Regionala System") == 0)
    {
#ifdef ANSIC
/*
#endif
      //-----------------------------------------
      // This section is not needed and therefore 
      // not included in this code version
      //-----------------------------------------
#ifdef ANSIC
*/
#endif

    }
  else if (strcmp(utSystem.pSystemTyp,"Lokala System") == 0)
    {
		
#ifdef ANSIC
/*
#endif
      //-------------------------------------------------------------
      //	Change meridian to where the transformation shall be done
      //-------------------------------------------------------------
#ifdef ANSIC
*/
#endif
		

      if (inSystem.meridian != utSystem.inSystemMedelmeridian)
	{			
	  besselChangeMeridian(&xMid,&yMid,inSystem.meridian,utSystem.inSystemMedelmeridian);
	  inSystem.meridian = utSystem.inSystemMedelmeridian;
	}
		
#ifdef ANSIC
/*
#endif
      //-----------------------------------------
      //	Add x-, y-shift
      //-----------------------------------------
#ifdef ANSIC
*/
#endif
		
      yMid = yMid+utSystem.inSystemYTillagg;
      xMid = xMid+utSystem.inSystemXTillagg;
		
#ifdef ANSIC
/*
#endif
      //-----------------------------------------
      //	Make the transformation
      //-----------------------------------------		
#ifdef ANSIC
*/
#endif
		
      yT =  (utSystem.A*yMid-utSystem.A*utSystem.YTo-utSystem.B*xMid+utSystem.B*utSystem.XTo)/(utSystem.A*utSystem.D-utSystem.B*utSystem.C);
      xT = (xMid-utSystem.XTo-(utSystem.C*yT))/(utSystem.A);
		
#ifdef ANSIC
/*
#endif
      //-----------------------------------------
      //	Remove x,y shift
      //-----------------------------------------
#ifdef ANSIC
*/
#endif
		
      yT = yT-utSystem.utSystemYTillagg;
      xT = xT-utSystem.utSystemXTillagg;
		
#ifdef ANSIC
/*
#endif
      //-----------------------------------------
      //	Change in meridian to the out meridian
      // after the transformation
      //-----------------------------------------
#ifdef ANSIC
*/
#endif
		
      inSystem.meridian = utSystem.utSystemMedelmeridian;
		
		
    }
	
#ifdef ANSIC
/*
#endif
  //--------------------------------------------
  //  Finally, change meridian to the out-
  //	meridian
  //--------------------------------------------
#ifdef ANSIC
*/
#endif
	

	
  if (inSystem.meridian != utSystem.meridian)
    {
      besselChangeMeridian(&xT,&yT,inSystem.meridian,utSystem.meridian);
    }
	
#ifdef ANSIC
/*
#endif
  //----------------------------------------------------------------------
  //  If the out system is other then long/lat, we add the out x,y-shift
  //----------------------------------------------------------------------
#ifdef ANSIC
*/
#endif
  if ((strcmp(utSystem.pSystemTyp,"Rikets System") == 0) &&
      (  (strcmp(utSystem.pKoordSystem,"Lat/Long (Bessel)") == 0)
       || (strcmp(utSystem.pKoordSystem,"Lat/Long (WGS 84)") == 0)))
    {
#ifdef ANSIC
/*
#endif
      //outpadYVertices[j] = yT;
      //outpadXVertices[j] = xT;
#ifdef ANSIC
*/
#endif
    }
  else
    {
      yT = yT+utSystem.yTillagg;
      xT = xT+utSystem.xTillagg;
    }

	
  memcpy(pXT, &xT, sizeof(double) );
  memcpy(pYT, &yT, sizeof(double) );

}
