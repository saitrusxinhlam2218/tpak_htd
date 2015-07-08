#ifdef ANSIC
/*
#endif
//----------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------//
// Denna fil innehÂller funktioner f–r Bessels Ellipsoiden. Funktionerna ”r:			//
// 												//
//	void besselProject(double xF,double yF,double *xMid,double *yMid);			//
//	Projicerar long/lat koordinaterna xF och xF till Gauss konforma 			//
//	transformation 2.5 gon V								//
//												//
//	void besselChangeMeridian (double *x, double *y,double inMer,double outMer);		//
//  Byter medel meridian pÂ koordinaterna x,y projicerade i Gauss. FrÂn in meridian 		//
//	till ut- meridian.									//
// 												//
//  void besselGetXYZ(double xF,double yF,double h,double *geoXF,double *geoYF,double *geoZF);	//
//  Konverterar long/lat/h–jd –ver ellipsoiden (xF,yF,h) till geocentriska X,Y,Z		//
//												//
//  void besselGetLongLat(double geoXF,double geoYF,double geoZF,double *xF,double *yF,double *h);//
//  Konverterar geocentriska X,Y,Z till long/lat/h–jd –ver ellipsoiden (xF,yF,h)		//
//												//
//	void besselUnproject(double xMid,double yMid,double *xT,double *yT);			//
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

#include "bessel.h"
#include <math.h>



#ifdef ANSIC
/*
#endif
// Elipsoid specifica konstanter
#ifdef ANSIC
*/
#endif

int BesselInited = 0;

long double besself = 1.0/299.1528128;
long double bessela = 6377397.155;


#ifdef ANSIC
/*
#endif
//-----------------------------------------------------
#ifdef ANSIC
*/
#endif

long double besselb;
long double bessele;
long double besseln;
long double besseltakA;
long double besseldelta1;

long double besseldelta2;

long double besseldelta3;

long double besseldelta4;

long double besselAStar;
long double besselBStar;
long double besselCStar;
long double besselDStar;

long double besselA;
long double besselB;
long double besselC;
long double besselD;


long double besselbeta1;

long double besselbeta2;

long double besselbeta3;

long double besselbeta4;

void initconst()
{
  if (BesselInited)
    return;
  besself = 1.0/299.1528128;
  bessela = 6377397.155;

  besselb = bessela-(besself*bessela);
  bessele = sqrt(besself*(2-besself));
  besseln = besself/(2-besself);
  besseltakA = (bessela/(1.0+besseln)) * (1.0+ (pow(besseln,2)/4.0)+(pow(besseln,4)/64.0));
  besseldelta1 = ((1.0/2.0)*besseln)-
    ((2.0/3.0)*pow(besseln,2))+
      ((37.0/96.0)*pow(besseln,3))-
	((1.0/360.0)*pow(besseln,4));

  besseldelta2 = ((1.0/48.0)*pow(besseln,2))+
    ((1.0/15.0)*pow(besseln,3))-
      ((437.0/1440.0)*pow(besseln,4));

  besseldelta3 = ((17.0/480.0)*pow(besseln,3))-
    ((37.0/840.0)*pow(besseln,4));

  besseldelta4 = ((4397.0/161280.0)*pow(besseln,4));

  besselAStar = (pow(bessele,2)+pow(bessele,4)+pow(bessele,6)+pow(bessele,8));
  besselBStar = -(1.0/6.0)*((7.0*pow(bessele,4)+17.0*pow(bessele,6)+30.0*pow(bessele,8)));
  besselCStar = (1.0/120.0)*((224.0*pow(bessele,6)+889.0*pow(bessele,8)));
  besselDStar = -(1.0/1260.0)*((4279.0*pow(bessele,8)));

  besselA = pow(bessele,2);
  besselB = (1.0/6.0)*(5.0*pow(bessele,4) - pow(bessele,6));
  besselC = (1.0/120.0)*(104.0*pow(bessele,6) - 45.0*pow(bessele,8));
  besselD = (1.0/1260.0)*(1237.0*pow(bessele,8));


  besselbeta1 = ((1.0/2.0)*besseln -
		  (2.0/3.0)*pow(besseln,2) +
		  (5.0/16.0)*pow(besseln,3) +
		  (41.0/180.0)*pow(besseln,4));

  besselbeta2 = ((13.0/48.0)*pow(besseln,2) -
		 (3.0/5.0)*pow(besseln,3) +
		 (557.0/1440.0)*pow(besseln,4));

  besselbeta3 = ((61.0/240.0)*pow(besseln,3) -
		 (103.0/140.0)*pow(besseln,4));

  besselbeta4 = ((49561.0/161280.0)*pow(besseln,4));

  BesselInited = 1;
}

#ifdef ANSIC
/*
#endif
//--------------------------------------------------------------//
//	Byter medelmeridian pÂ Bessels elipsoid
//--------------------------------------------------------------//
#ifdef ANSIC
*/
#endif

void besselChangeMeridian (double *x, double *y,double inMer,double outMer)
{
  long double epsilon;
  long double eta;
  long double primEpsilon;
  long double primEta;
  long double fiStar;
  long double deltaLambda;

  initconst();

  epsilon = (x[0]/besseltakA);
  eta = (y[0]/besseltakA);
  primEpsilon = (epsilon-
		 (besseldelta1*sin(2.0*epsilon)*cosh(2.0*eta))-
		 (besseldelta2*sin(4.0*epsilon)*cosh(4.0*eta))-
		 (besseldelta3*sin(6.0*epsilon)*cosh(6.0*eta))-
		 (besseldelta4*sin(8.0*epsilon)*cosh(8.0*eta)));
  primEta = (eta-
	     (besseldelta1*cos(2*epsilon)*sinh(2*eta))-
	     (besseldelta2*cos(4*epsilon)*sinh(4*eta))-
	     (besseldelta3*cos(6*epsilon)*sinh(6*eta))-
	     (besseldelta4*cos(8*epsilon)*sinh(8*eta)));
  fiStar = asin((sin(primEpsilon)/cosh(primEta)));
  deltaLambda = atan((sinh(primEta)/cos(primEpsilon)));

  deltaLambda = deltaLambda + inMer - outMer;

  primEpsilon = atan((tan(fiStar)/cos(deltaLambda)));
  primEta = besselatanh((cos(fiStar)*sin(deltaLambda)));

  x[0] = besseltakA*((primEpsilon +
		      (besselbeta1*sin(2*primEpsilon)*cosh(2*primEta))+
		      (besselbeta2*sin(4*primEpsilon)*cosh(4*primEta))+
		      (besselbeta3*sin(6*primEpsilon)*cosh(6*primEta))+
		      (besselbeta4*sin(8*primEpsilon)*cosh(8*primEta))));

  y[0] = besseltakA*((primEta +
		      (besselbeta1*cos(2*primEpsilon)*sinh(2*primEta))+
		      (besselbeta2*cos(4*primEpsilon)*sinh(4*primEta))+
		      (besselbeta3*cos(6*primEpsilon)*sinh(6*primEta))+
		      (besselbeta4*cos(8*primEpsilon)*sinh(8*primEta))));

}

#ifdef ANSIC
/*
#endif
//--------------------------------------------------------------------//
//	Projicerar lat/long (xF,yF) frÂn Bessels elipsoid till Gauss 2.5 V
//--------------------------------------------------------------------//
#ifdef ANSIC
*/
#endif

void besselProject(double xF,double yF,double *xMid,double *yMid)
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

  initconst();

  deltaLambda = yF - ((17.564753086/400)*2*3.141592653589793238462643383280);
  fiStar = xF-sin(xF)*cos(xF)*(besselA +
			       besselB*pow(sin(xF),2) +
			       besselC*pow(sin(xF),4) +
			       besselD*pow(sin(xF),6));
  primEpsilon = atan((tan(fiStar)/cos(deltaLambda)));
  primEta = besselatanh((cos(fiStar)*sin(deltaLambda)));

  *xMid = besseltakA*((primEpsilon +
		       (besselbeta1*sin(2*primEpsilon)*cosh(2*primEta))+
		       (besselbeta2*sin(4*primEpsilon)*cosh(4*primEta))+
		       (besselbeta3*sin(6*primEpsilon)*cosh(6*primEta))+
		       (besselbeta4*sin(8*primEpsilon)*cosh(8*primEta))));

  *yMid = besseltakA*((primEta +
		       (besselbeta1*cos(2*primEpsilon)*sinh(2*primEta))+
		       (besselbeta2*cos(4*primEpsilon)*sinh(4*primEta))+
		       (besselbeta3*cos(6*primEpsilon)*sinh(6*primEta))+
		       (besselbeta4*cos(8*primEpsilon)*sinh(8*primEta))));

}

#ifdef ANSIC
/*
#endif
//--------------------------------------------------------------------//
//	Avprojicerar Bessel 2.5 V till lat/long (xF,yF)
//--------------------------------------------------------------------//
#ifdef ANSIC
*/
#endif

void besselUnproject(double xMid,double yMid,double *xT,double *yT)
{
  long double epsilon;
  long double eta;
  long double primEpsilon;
  long double primEta;
  long double fiStar;
  long double deltaLambda;

  initconst();
  epsilon = (xMid/besseltakA);
  eta = (yMid/besseltakA);
  primEpsilon = (epsilon-
		 (besseldelta1*sin(2.0*epsilon)*cosh(2.0*eta))-
		 (besseldelta2*sin(4.0*epsilon)*cosh(4.0*eta))-
		 (besseldelta3*sin(6.0*epsilon)*cosh(6.0*eta))-
		 (besseldelta4*sin(8.0*epsilon)*cosh(8.0*eta)));
  primEta = (eta-
	     (besseldelta1*cos(2*epsilon)*sinh(2*eta))-
	     (besseldelta2*cos(4*epsilon)*sinh(4*eta))-
	     (besseldelta3*cos(6*epsilon)*sinh(6*eta))-
	     (besseldelta4*cos(8*epsilon)*sinh(8*eta)));
  fiStar = asin((sin(primEpsilon)/cosh(primEta)));
  deltaLambda = atan((sinh(primEta)/cos(primEpsilon)));

  *yT = deltaLambda + ((17.564753086/400)*2*3.141592653589793238462643383280);

  *xT = fiStar+(sin(fiStar)*cos(fiStar)*(besselAStar +
					 besselBStar*pow(sin(fiStar),2.0) +
					 besselCStar*pow(sin(fiStar),4.0) +
					 besselDStar*pow(sin(fiStar),6.0)));

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

void besselGetXYZ(double xF,double yF,double h,double *geoXF,double *geoYF,double *geoZF)
{
  long double Nprim;

  initconst();
  Nprim = bessela/(sqrt(1-(besself*(2-besself)*pow(sin(xF),2))));

  *geoXF = (Nprim+h)*cos(xF)*cos(yF);

  *geoYF = (Nprim+h)*cos(xF)*sin(yF);

  *geoZF = (Nprim*(1-(besself*(2-besself)))+h)*sin(xF);

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

void besselGetLongLat(double geoXF,double geoYF,double geoZF,double *xF,double *yF,double *h)
{
  long double localLambda;
  long double localP;	
  long double teta;
  long double Nprim;	
  long double localFi;

  initconst();
  localLambda = atan(geoYF/geoXF);
  localP = sqrt(pow(geoXF,2)+pow(geoYF,2));
  teta = atan(geoZF/(localP*sqrt(1-(besself*(2-besself)))));
  localFi =
    atan(
	 (geoZF+
	  (  (bessela*(besself*(2-besself)))/(   sqrt(1- ((besself*(2-besself))) )     )) * pow(sin(teta),3))
	 /   (localP-( (bessela*(besself*(2-besself))) *  pow(cos(teta),3)))
	 );

  *xF = localFi;
  *yF = localLambda;

  Nprim = bessela/(sqrt(1-(besself*(2-besself)*pow(sin(localFi),2))));

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

long double besselatanh(long double x)
{
  return ((log((1+x)/(1-x)))/2);
}
