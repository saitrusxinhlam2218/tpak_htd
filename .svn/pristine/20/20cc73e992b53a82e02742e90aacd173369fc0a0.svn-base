#ifdef ANSIC
/*
#endif
//--------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------//
// Toppfunktioner f–r transformation mellan G–teborg-WGS84 och ST74-WGS84		//
//											//	
// Copyright © 1999, SWEGIS. All rights reserved.					//
//											//
// Date: 1999-09-02									//
//											//
// Author: Mikael Elmquist, mikael@swegis.com						//
//--------------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------------//
#ifdef ANSIC
*/
#endif



/* #include "wgstrans.h" */
#include <string.h>
#include <stdio.h>
#include "transform.h"

#ifdef ANSIC
void ST74ToWGS84(double inX,double inY, double *utX, double *utY)
#else
void __declspec( dllexport ) ST74ToWGS84(double inX,double inY, double *utX, double *utY)
#endif
{
  KoordSystem inSystem,utSystem;

#ifdef ANSIC
/*
#endif
  //-----------------------------------------------------------
  //      Tilldela  koordinat till”gg
  //-----------------------------------------------------------
	
  // ST74
#ifdef ANSIC
*/
#endif
  inSystem.xTillagg = -6500000;
  inSystem.yTillagg = 100000;
#ifdef ANSIC
/*
#endif
  // WGS 84
#ifdef ANSIC
*/
#endif
  utSystem.xTillagg = 0;
  utSystem.yTillagg = 0;


#ifdef ANSIC
/*
#endif
  //-----------------------------------------------------------
  //		Tilldela trans. konstanter
  //-----------------------------------------------------------

  // ST74
#ifdef ANSIC
*/
#endif
  inSystem.pSystemTyp = "Lokala System";
  inSystem.pKoordSystem = "ST 74";
  inSystem.meridian = (20.064753086/400)*2*3.141592653589793238462643383280;
  inSystem.A=1.000011118407698;
  inSystem.B=-2.370812488181253e-005;
  inSystem.C=2.370812488181253e-005;
  inSystem.D=1.000011118407698;
  inSystem.XTo=6500016.0304197040;	 
  inSystem.YTo=-100009.0805348641;
  inSystem.inSystemXTillagg=-6500000;
  inSystem.inSystemYTillagg=100000;
  inSystem.inSystemMedelmeridian=(20.064753086/400)*2*3.141592653589793238462643383280;
  inSystem.utSystemXTillagg=0;
  inSystem.utSystemYTillagg=0;
  inSystem.utSystemMedelmeridian=(20.064753086/400)*2*3.141592653589793238462643383280;
	

#ifdef ANSIC
/*
#endif
  // WGS 84
#ifdef ANSIC
*/
#endif
  utSystem.pSystemTyp = "Rikets System";
  utSystem.pKoordSystem = "Lat/Long (WGS 84)";
  utSystem.meridian = 0;
  utSystem.A=0;
  utSystem.B=0;
  utSystem.C=0;
  utSystem.D=0;
  utSystem.XTo=0;
  utSystem.YTo=0;
  utSystem.inSystemXTillagg=0;
  utSystem.inSystemYTillagg=0;
  utSystem.inSystemMedelmeridian=0;
  utSystem.utSystemXTillagg=0;
  utSystem.utSystemYTillagg=0;
  utSystem.utSystemMedelmeridian=0;

	
	
#ifdef ANSIC
/*
#endif
  //------------------------------------------------------------
  // Transformera sj”va datan
  //------------------------------------------------------------
#ifdef ANSIC
*/
#endif

  Transform(inX,
	    inY,
	    inSystem,
	    utX,
	    utY,
	    utSystem);

}

#ifdef ANSIC
void WGS84ToST74(double inX,double inY, double *utX, double *utY)
#else
void __declspec( dllexport ) WGS84ToST74(double inX,double inY, double *utX, double *utY)
#endif
{
  KoordSystem inSystem,utSystem;

#ifdef ANSIC
/*
#endif
  //-----------------------------------------------------------
  //      Tilldela  till”ggsvariablerna
  //-----------------------------------------------------------
	
  // WGS 84
#ifdef ANSIC
*/
#endif

  inSystem.xTillagg = 0;
  inSystem.yTillagg = 0;
#ifdef ANSIC
/*
#endif
 	// ST74
#ifdef ANSIC
*/
#endif
  utSystem.xTillagg = -6500000;
  utSystem.yTillagg = 100000;


#ifdef ANSIC
/*
#endif
  //-----------------------------------------------------------
  //		Tilldela trans. konstanter
  //-----------------------------------------------------------

  // WGS 84
#ifdef ANSIC
*/
#endif

  inSystem.pSystemTyp = "Rikets System";
  inSystem.pKoordSystem = "Lat/Long (WGS 84)";
  inSystem.meridian = 0;
  inSystem.A=0;
  inSystem.B=0;
  inSystem.C=0;
  inSystem.D=0;
  inSystem.XTo=0;
  inSystem.YTo=0;
  inSystem.inSystemXTillagg=0;
  inSystem.inSystemYTillagg=0;
  inSystem.inSystemMedelmeridian=0;
  inSystem.utSystemXTillagg=0;
  inSystem.utSystemYTillagg=0;
  inSystem.utSystemMedelmeridian=0;

#ifdef ANSIC
/*
#endif
  // ST74
#ifdef ANSIC
*/
#endif

  utSystem.pSystemTyp = "Lokala System";
  utSystem.pKoordSystem = "ST 74";
  utSystem.meridian = (20.064753086/400)*2*3.141592653589793238462643383280;
  utSystem.A=1.000011118407698;
  utSystem.B=-2.370812488181253e-005;
  utSystem.C=2.370812488181253e-005;
  utSystem.D=1.000011118407698;
  utSystem.XTo=6500016.0304197040;	 
  utSystem.YTo=-100009.0805348641;
  utSystem.inSystemXTillagg=0;
  utSystem.inSystemYTillagg=0;
  utSystem.inSystemMedelmeridian=(20.064753086/400)*2*3.141592653589793238462643383280;
  utSystem.utSystemXTillagg=-6500000;
  utSystem.utSystemYTillagg=100000;
  utSystem.utSystemMedelmeridian=(20.064753086/400)*2*3.141592653589793238462643383280;

#ifdef ANSIC
/*
#endif
  //------------------------------------------------------------
  // Transformera sj”va datan
  //------------------------------------------------------------
#ifdef ANSIC
*/
#endif

  Transform(inX,
	    inY,
	    inSystem,
	    utX,
	    utY,
	    utSystem);
	
}

#ifdef ANSIC
void WGS84ToGoteborg(double inX,double inY, double *utX, double *utY)
#else
void __declspec( dllexport ) WGS84ToGoteborg(double inX,double inY, double *utX, double *utY)
#endif
{
  KoordSystem inSystem,utSystem;

#ifdef ANSIC
/*
#endif
  //-----------------------------------------------------------
  //      Tilldela  till”ggsvariablerna
  //-----------------------------------------------------------
	
  // G–teborg
#ifdef ANSIC
*/
#endif

  inSystem.xTillagg = 0;
  inSystem.yTillagg = 0;
#ifdef ANSIC
/*
#endif
  // WGS 84
#ifdef ANSIC
*/
#endif
  utSystem.xTillagg = 0;
  utSystem.yTillagg = 0;


#ifdef ANSIC
/*
#endif
  //-----------------------------------------------------------
  //		Tilldela trans. konstanter
  //-----------------------------------------------------------

  // G–teborg
#ifdef ANSIC
*/
#endif

  utSystem.pSystemTyp = "Lokala System";
  utSystem.pKoordSystem = "G–teborg";
  utSystem.meridian = (12.564753086/400)*2*3.141592653589793238462643383280;
  utSystem.A=0.999998071635;
  utSystem.B=2.687607870674e-005;
  utSystem.C=-2.68760787067e-005;
  utSystem.D=0.999998071635;
  utSystem.XTo=6369995.98317;	 
  utSystem.YTo=-3.53094498254;
  utSystem.inSystemXTillagg=0;
  utSystem.inSystemYTillagg=0;
  utSystem.inSystemMedelmeridian=(12.564753086/400)*2*3.141592653589793238462643383280;
  utSystem.utSystemXTillagg=0;
  utSystem.utSystemYTillagg=0;
  utSystem.utSystemMedelmeridian=(12.564753086/400)*2*3.141592653589793238462643383280;
	

#ifdef ANSIC
/*
#endif
  // WGS 84
#ifdef ANSIC
*/
#endif

  inSystem.pSystemTyp = "Rikets System";
  inSystem.pKoordSystem = "Lat/Long (WGS 84)";
  inSystem.meridian = 0;
  inSystem.A=0;
  inSystem.B=0;
  inSystem.C=0;
  inSystem.D=0;
  inSystem.XTo=0;
  inSystem.YTo=0;
  inSystem.inSystemXTillagg=0;
  inSystem.inSystemYTillagg=0;
  inSystem.inSystemMedelmeridian=0;
  inSystem.utSystemXTillagg=0;
  inSystem.utSystemYTillagg=0;
  inSystem.utSystemMedelmeridian=0;

#ifdef ANSIC
/*
#endif
  //------------------------------------------------------------
  // Transformera sj”va datan
  //------------------------------------------------------------
#ifdef ANSIC
*/
#endif

  Transform(inX,
	    inY,
	    inSystem,
	    utX,
	    utY,
	    utSystem);
	
}

#ifdef ANSIC
void GoteborgToWGS84(double inX,double inY, double *utX, double *utY)
#else
void __declspec( dllexport ) GoteborgToWGS84(double inX,double inY, double *utX, double *utY)
#endif
{
  KoordSystem inSystem,utSystem;

#ifdef ANSIC
/*
#endif
  //-----------------------------------------------------------
  //      Tilldela  till”ggsvariablerna
  //-----------------------------------------------------------
	
  // G–teborg
#ifdef ANSIC
*/
#endif

  inSystem.xTillagg = 0;
  inSystem.yTillagg = 0;
#ifdef ANSIC
/*
#endif
  // WGS 84
#ifdef ANSIC
*/
#endif
  utSystem.xTillagg = 0;
  utSystem.yTillagg = 0;


#ifdef ANSIC
/*
#endif
  //-----------------------------------------------------------
  //		Tilldela trans. konstanter
  //-----------------------------------------------------------

  // G–teborg
#ifdef ANSIC
*/
#endif

  inSystem.pSystemTyp = "Lokala System";
  inSystem.pKoordSystem = "G–teborg";
  inSystem.meridian = (12.564753086/400)*2*3.141592653589793238462643383280;
  inSystem.A=0.999998071635;
  inSystem.B=2.687607870674e-005;
  inSystem.C=-2.68760787067e-005;
  inSystem.D=0.999998071635;
  inSystem.XTo=6369995.98317;	 
  inSystem.YTo=-3.53094498254;
  inSystem.inSystemXTillagg=0;
  inSystem.inSystemYTillagg=0;
  inSystem.inSystemMedelmeridian=(12.564753086/400)*2*3.141592653589793238462643383280;
  inSystem.utSystemXTillagg=0;
  inSystem.utSystemYTillagg=0;
  inSystem.utSystemMedelmeridian=(12.564753086/400)*2*3.141592653589793238462643383280;
	

#ifdef ANSIC
/*
#endif
  // WGS 84
#ifdef ANSIC
*/
#endif

  utSystem.pSystemTyp = "Rikets System";
  utSystem.pKoordSystem = "Lat/Long (WGS 84)";
  utSystem.meridian = 0;
  utSystem.A=0;
  utSystem.B=0;
  utSystem.C=0;
  utSystem.D=0;
  utSystem.XTo=0;
  utSystem.YTo=0;
  utSystem.inSystemXTillagg=0;
  utSystem.inSystemYTillagg=0;
  utSystem.inSystemMedelmeridian=0;
  utSystem.utSystemXTillagg=0;
  utSystem.utSystemYTillagg=0;
  utSystem.utSystemMedelmeridian=0;

#ifdef ANSIC
/*
#endif
  //------------------------------------------------------------
  // Transformera sj”va datan
  //------------------------------------------------------------
#ifdef ANSIC
*/
#endif

  Transform(inX,
	    inY,
	    inSystem,
	    utX,
	    utY,
	    utSystem);

}
