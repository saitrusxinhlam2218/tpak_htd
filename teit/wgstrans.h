#ifdef ANSIC
/*
#endif
//------------------------------------------------------------------------------//
// WGSTrans.h
//
// SWEGIS
// Mikael Elmquist
// Ÿlghagsgatan 4B
// 413 17 G–teborg
//
// E-mail: mikael@swegis.com
// Internet: www.swegis.com
//
//					Copyright (c) 1999, SWEGIS
//------------------------------------------------------------------------------//
#ifdef ANSIC
*/
#endif


#ifdef ANSIC
void  WGS84ToGoteborg(double inX,double inY, double *utX, double *utY);
void  GoteborgToWGS84(double inX,double inY, double *utX, double *utY);
void  WGS84ToST74(double inX,double inY, double *utX, double *utY);
void  ST74ToWGS84(double inX,double inY, double *utX, double *utY);
#else
void __declspec( dllexport ) WGS84ToGoteborg(double inX,double inY, double *utX, double *utY);
void __declspec( dllexport ) GoteborgToWGS84(double inX,double inY, double *utX, double *utY);
void __declspec( dllexport ) WGS84ToST74(double inX,double inY, double *utX, double *utY);
void __declspec( dllexport ) ST74ToWGS84(double inX,double inY, double *utX, double *utY);
#endif
