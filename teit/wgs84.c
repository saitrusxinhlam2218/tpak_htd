#ifdef ANSIC
/*
#endif
//----------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------//
// Denna fil innehÂller funktioner f–r WGS84 Ellipsoiden. Funktionerna ”r:			//
// 												//
//	void wgs84Project(double xF,double yF,double *xMid,double *yMid);			//
//	Projicerar long/lat koordinaterna xF och xF till Gauss konforma 			//
//	transformation 2.5 gon V								//
//												//
//	void wgs84ChangeMeridian (double *x, double *y,double inMer,double outMer);		//
//  Byter medel meridian pÂ koordinaterna x,y projicerade i Gauss. FrÂn in meridian 		//
//	till ut- meridian.									//
// 												//
//	void wgs84GetXYZ(double xF,double yF,double h,double *geoXF,double *geoYF,double *geoZF);//
//  Konverterar long/lat/h–jd –ver ellipsoiden (xF,yF,h) till geocentriska X,Y,Z		//
//												//
//	void wgs84GetLongLat(double geoXF,double geoYF,double geoZF,double *xF,double *yF,double *h);//
//  Konverterar geocentriska X,Y,Z till long/lat/h–jd –ver ellipsoiden (xF,yF,h)		//
//												//
//	void wgs84Unproject(double xMid,double yMid,double *xT,double *yT);			//
//	"Avprojicerar"- koordinater projicerade i Gauss till lat/long				//
//												//
// Copyright © 1999, SWEGIS. All rights reserved.						//
//												//
// Date: 1999-09-02										//
//												//
// Author: Mikael Elmquist, mikael@swegis.com							//
//----------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------//
#ifdef ANSIC
*/
#endif



#include "wgs84.h"
#include <math.h>


#ifdef ANSIC
/*
#endif
//--------------------------------------------------------------//
//	Konstanter f–r Gauss Projicering
//--------------------------------------------------------------//


//-----------------------------------------------------
// Elipsoid specifica konstanter
//-----------------------------------------------------
#ifdef ANSIC
*/
#endif

int InitedP = 0;

long double wgs84f;
long double wgs84a;

long double wgs84b;
long double wgs84e;
long double wgs84n;
long double wgs84takA;
long double wgs84delta1;

long double wgs84delta2;

long double wgs84delta3;

long double wgs84delta4;

long double wgs84AStar;
long double wgs84BStar;
long double wgs84CStar;
long double wgs84DStar;

long double wgs84A;
long double wgs84B;
long double wgs84C;
long double wgs84D;

long double wgs84beta1;

long double wgs84beta2;

long double wgs84beta3;

long double wgs84beta4;


void initlocals()
{
  if (InitedP)
    return;
  wgs84f = 1.0/298.257222101;
  wgs84a = 6378137.0;

  wgs84b = wgs84a-(wgs84f*wgs84a);
  wgs84e = sqrt(wgs84f*(2-wgs84f));
  wgs84n = wgs84f/(2-wgs84f);
  wgs84takA = (wgs84a/(1.0+wgs84n)) * (1.0+ (pow(wgs84n,2)/4.0)+(pow(wgs84n,4)/64.0));
  wgs84delta1 = ((1.0/2.0)*wgs84n)-
    ((2.0/3.0)*pow(wgs84n,2))+
      ((37.0/96.0)*pow(wgs84n,3))-
	((1.0/360.0)*pow(wgs84n,4));

  wgs84delta2 = ((1.0/48.0)*pow(wgs84n,2))+
    ((1.0/15.0)*pow(wgs84n,3))-
      ((437.0/1440.0)*pow(wgs84n,4));

  wgs84delta3 = ((17.0/480.0)*pow(wgs84n,3))-
    ((37.0/840.0)*pow(wgs84n,4));

  wgs84delta4 = ((4397.0/161280.0)*pow(wgs84n,4));

  wgs84AStar = (pow(wgs84e,2)+pow(wgs84e,4)+pow(wgs84e,6)+pow(wgs84e,8));
  wgs84BStar = -(1.0/6.0)*((7.0*pow(wgs84e,4)+17.0*pow(wgs84e,6)+30.0*pow(wgs84e,8)));
  wgs84CStar = (1.0/120.0)*((224.0*pow(wgs84e,6)+889.0*pow(wgs84e,8)));
  wgs84DStar = -(1.0/1260.0)*((4279.0*pow(wgs84e,8)));

  wgs84A = pow(wgs84e,2);
  wgs84B = (1.0/6.0)*(5.0*pow(wgs84e,4) - pow(wgs84e,6));
  wgs84C = (1.0/120.0)*(104.0*pow(wgs84e,6) - 45.0*pow(wgs84e,8));
  wgs84D = (1.0/1260.0)*(1237.0*pow(wgs84e,8));

  wgs84beta1 = ((1.0/2.0)*wgs84n -
		(2.0/3.0)*pow(wgs84n,2) +
		(5.0/16.0)*pow(wgs84n,3) +
		(41.0/180.0)*pow(wgs84n,4));

  wgs84beta2 = ((13.0/48.0)*pow(wgs84n,2) -
		(3.0/5.0)*pow(wgs84n,3) +
		(557.0/1440.0)*pow(wgs84n,4));

  wgs84beta3 = ((61.0/240.0)*pow(wgs84n,3) -
		(103.0/140.0)*pow(wgs84n,4));

  wgs84beta4 = ((49561.0/161280.0)*pow(wgs84n,4));

  InitedP = 1;
}


#ifdef ANSIC
/*
#endif
//--------------------------------------------------------------//
//	Byter medelmeridian pÂ wgs84s elipsoid
//--------------------------------------------------------------//
#ifdef ANSIC
*/
#endif

void wgs84ChangeMeridian (double *x, double *y,double inMer,double outMer)
{
  long double epsilon;
  long double eta;
  long double primEpsilon;
  long double primEta;
  long double fiStar;
  long double deltaLambda;
  
  initlocals();
  /* init locals */
  epsilon = (x[0]/wgs84takA);
  eta = (y[0]/wgs84takA);
  primEpsilon = (epsilon-
		 (wgs84delta1*sin(2.0*epsilon)*cosh(2.0*eta))-
		 (wgs84delta2*sin(4.0*epsilon)*cosh(4.0*eta))-
		 (wgs84delta3*sin(6.0*epsilon)*cosh(6.0*eta))-
		 (wgs84delta4*sin(8.0*epsilon)*cosh(8.0*eta)));
  
  primEta = (eta-
	     (wgs84delta1*cos(2*epsilon)*sinh(2*eta))-
	     (wgs84delta2*cos(4*epsilon)*sinh(4*eta))-
	     (wgs84delta3*cos(6*epsilon)*sinh(6*eta))-
	     (wgs84delta4*cos(8*epsilon)*sinh(8*eta)));
  
  fiStar = asin((sin(primEpsilon)/cosh(primEta)));
  
  deltaLambda = atan((sinh(primEta)/cos(primEpsilon)));
  
  
  deltaLambda = deltaLambda + inMer - outMer;
  
  
  primEpsilon = atan((tan(fiStar)/cos(deltaLambda)));
  primEta = wgs84atanh((cos(fiStar)*sin(deltaLambda)));
  
  x[0] = wgs84takA*((primEpsilon +
		     (wgs84beta1*sin(2*primEpsilon)*cosh(2*primEta))+
		     (wgs84beta2*sin(4*primEpsilon)*cosh(4*primEta))+
		     (wgs84beta3*sin(6*primEpsilon)*cosh(6*primEta))+
		     (wgs84beta4*sin(8*primEpsilon)*cosh(8*primEta))));
  
  y[0] = wgs84takA*((primEta +
		     (wgs84beta1*cos(2*primEpsilon)*sinh(2*primEta))+
		     (wgs84beta2*cos(4*primEpsilon)*sinh(4*primEta))+
		     (wgs84beta3*cos(6*primEpsilon)*sinh(6*primEta))+
		     (wgs84beta4*cos(8*primEpsilon)*sinh(8*primEta))));
  
  
}

#ifdef ANSIC
/*
#endif
//--------------------------------------------------------------------//
//	Projicerar lat/long (xF,yF) frÂn WGS 84 elipsoid till Gauss 2.5 V
//--------------------------------------------------------------------//
#ifdef ANSIC
*/
#endif

void wgs84Project(double xF,double yF,double *xMid,double *yMid)
{
#ifdef ANSIC
/*
#endif
  // L”gg till 2.5 gon V
#ifdef ANSIC
*/
#endif
  long double deltaLambda;
  long double fiStar;
  long double primEpsilon;
  long double primEta;
  
  initlocals();
  /* init locals */
  deltaLambda = yF - ((17.564753086/400)*2*3.141592653589793238462643383280);
  
  fiStar = xF-sin(xF)*cos(xF)*(wgs84A +
			       wgs84B*pow(sin(xF),2) +
			       wgs84C*pow(sin(xF),4) +
			       wgs84D*pow(sin(xF),6));


  primEpsilon = atan((tan(fiStar)/cos(deltaLambda)));
  primEta = wgs84atanh((cos(fiStar)*sin(deltaLambda)));
  
  *xMid = wgs84takA*((primEpsilon +
		      (wgs84beta1*sin(2*primEpsilon)*cosh(2*primEta))+
		      (wgs84beta2*sin(4*primEpsilon)*cosh(4*primEta))+
		      (wgs84beta3*sin(6*primEpsilon)*cosh(6*primEta))+
		      (wgs84beta4*sin(8*primEpsilon)*cosh(8*primEta))));
  
  *yMid = wgs84takA*((primEta +
		      (wgs84beta1*cos(2*primEpsilon)*sinh(2*primEta))+
		      (wgs84beta2*cos(4*primEpsilon)*sinh(4*primEta))+
		      (wgs84beta3*cos(6*primEpsilon)*sinh(6*primEta))+
		      (wgs84beta4*cos(8*primEpsilon)*sinh(8*primEta))));
  
  
}

#ifdef ANSIC
/*
#endif
//--------------------------------------------------------------------//
//	Avprojicerar wgs84 2.5 V till lat/long (xF,yF)
//--------------------------------------------------------------------//
#ifdef ANSIC
*/
#endif

void wgs84Unproject(double xMid,double yMid,double *xT,double *yT)
{
  long double epsilon;
  long double eta;
  long double primEpsilon;
  long double primEta;
  long double fiStar;
  long double deltaLambda;
  
  initlocals();
  /* init locals */
  epsilon = (xMid/wgs84takA);
  eta = (yMid/wgs84takA);

  primEpsilon = (epsilon-
		 (wgs84delta1*sin(2.0*epsilon)*cosh(2.0*eta))-
		 (wgs84delta2*sin(4.0*epsilon)*cosh(4.0*eta))-
		 (wgs84delta3*sin(6.0*epsilon)*cosh(6.0*eta))-
		 (wgs84delta4*sin(8.0*epsilon)*cosh(8.0*eta)));

  primEta = (eta-
	     (wgs84delta1*cos(2*epsilon)*sinh(2*eta))-
	     (wgs84delta2*cos(4*epsilon)*sinh(4*eta))-
	     (wgs84delta3*cos(6*epsilon)*sinh(6*eta))-
	     (wgs84delta4*cos(8*epsilon)*sinh(8*eta)));
  
  fiStar = asin((sin(primEpsilon)/cosh(primEta)));
  
  deltaLambda = atan((sinh(primEta)/cos(primEpsilon)));
  
  *yT = deltaLambda + ((17.564753086/400)*2*3.141592653589793238462643383280);
  
  
  
  *xT = fiStar+(sin(fiStar)*cos(fiStar)*(wgs84AStar +
					 wgs84BStar*pow(sin(fiStar),2.0) +
					 wgs84CStar*pow(sin(fiStar),4.0) +
					 wgs84DStar*pow(sin(fiStar),6.0)));
  
  
}

#ifdef ANSIC
/*
#endif
//------------------------------------------------------------------------------------
// Tar fram Geocentriska X,Y,Z frÂn long/lat/hojd
//------------------------------------------------------------------------------------
#ifdef ANSIC
*/
#endif

void wgs84GetXYZ(double xF,double yF,double h,double *geoXF,double *geoYF,double *geoZF)
{
	
  long double Nprim;

  initlocals();
  Nprim = wgs84a/(sqrt(1-(wgs84f*(2-wgs84f)*pow(sin(xF),2))));
  
  *geoXF = (Nprim+h)*cos(xF)*cos(yF);
  
  *geoYF = (Nprim+h)*cos(xF)*sin(yF);
  
  *geoZF = (Nprim*(1-(wgs84f*(2-wgs84f)))+h)*sin(xF);
  
}

#ifdef ANSIC
/*
#endif
//------------------------------------------------------------------------------------
// Tar fram long/lat/hojd frÂn geocentriska. 
//------------------------------------------------------------------------------------
#ifdef ANSIC
*/
#endif

void wgs84GetLongLat(double geoXF,double geoYF,double geoZF,double *xF,double *yF,double *h)
{
  long double localLambda;
  long double localP;
  long double teta;
  long double localFi;
  long double Nprim;
  
  initlocals();
  /* init locals */
  localLambda = atan(geoYF/geoXF);
  
  localP = sqrt(pow(geoXF,2)+pow(geoYF,2));
  
  teta = atan(geoZF/(localP*sqrt(1-(wgs84f*(2-wgs84f)))));
  
  localFi =
    atan(
	 (geoZF+(  (wgs84a*(wgs84f*(2-wgs84f)))/(   sqrt(1- ((wgs84f*(2-wgs84f))) )     )) * pow(sin(teta),3))
	 /
	 (localP-( (wgs84a*(wgs84f*(2-wgs84f))) *  pow(cos(teta),3)))
	 );
  
  *xF = localFi;
  *yF = localLambda;
  
  Nprim = wgs84a/(sqrt(1-(wgs84f*(2-wgs84f)*pow(sin(localFi),2))));
  
  *h = (localP/cos(localFi))-Nprim;

}

#ifdef ANSIC
/*
#endif
//------------------------------------------------------------------------------------
//						Funktion f–r att r”kna ut atanh
//------------------------------------------------------------------------------------
#ifdef ANSIC
*/
#endif

long double wgs84atanh(long double x)
{
  return ((log((1+x)/(1-x)))/2);

}
