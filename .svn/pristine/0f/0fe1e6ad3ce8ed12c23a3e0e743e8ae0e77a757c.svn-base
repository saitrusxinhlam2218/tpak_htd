#ifdef ANSIC

typedef struct TKoordSystem {
  double meridian;
  double xTillagg;
  double yTillagg;
  char *pSystemTyp;
  char *pKoordSystem;
  double inSystemMedelmeridian;
  double inSystemYTillagg;
  double inSystemXTillagg;
  double XTo;
  double YTo;
  double A;
  double B;
  double C;
  double D;
  double utSystemYTillagg;
  double utSystemXTillagg;
  double utSystemMedelmeridian;
} KoordSystem;


void Transform(double xF,
	       double yF,
	       KoordSystem inSystem,
	       double *pXT,
	       double *pYT,
	       KoordSystem utSystem);
#else

struct KoordSystem
{
	double meridian;
	double xTillagg;
	double yTillagg;
	char *pSystemTyp;
	char *pKoordSystem;
	double inSystemMedelmeridian;
	double inSystemYTillagg;
	double inSystemXTillagg;
	double XTo;
	double YTo;
	double A;
	double B;
	double C;
	double D;
	double utSystemYTillagg;
	double utSystemXTillagg;
	double utSystemMedelmeridian;
};


void __declspec( dllexport ) Transform(double xF,
				       double yF,
				       KoordSystem inSystem,
				       double *pXT,
				       double *pYT,
				       KoordSystem utSystem);
#endif
