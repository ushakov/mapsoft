// Геодезические проекции и системы координат

// нам нужны следующие вещи:
// - прочитав из файла название проекции и системы координат, преобразовать правильно точки
// - записать в файл правильное название проекции и СК (разное для разных форматов)


#include <string>
#include "../jeeps/gpsproj.h"
#include "../jeeps/gpsmath.h"
#include "../point.h"

struct Datum{
  int n; // номера систем координат из ../jeeps/gpsdatum.c. Надо ли их все сюда перенести?..

  Datum(const std::string & name = "wgs84"){
    if ((name == "wgs84")||
        (name == "wgs 84")||
        (name == "WGS84")||
        (name == "WGS 84")) {n=118; return;}
    if ((name == "Pulkovo 1942")||
        (name == "Pulkovo 1942 (1)")||
        (name == "Pulkovo 1942 (2)")||
        (name == "pulk")||
        (name == "pulkovo")||
        (name == "pulk42")) {n=92; return;}
    std::cerr << "Unknown datum: " << name << ". Using WGS 84.\n";
    n=118;
    return;

  }

  std::string xml_str{
    switch (n){
      case 118: return "wgs84";
      case  92: return "pulkovo";
      default:
	std::cerr << "xml_str: unknown datum: " << n << "\n";
	return "unknown";
    }
  }

  std::string oe_str{
    switch (n){
      case 118: return "WGS 84";
      case  92: return "Pulkovo 1942";
      default:
	std::cerr << "oe_str: unknown datum: " << n << "\n";
	return "Unknown";
    }
  }

};

struct Proj{
    int n;

    Proj(const std::string & name = "lonlat"){
	if ((name == "lonlat")||
	    (name == "Latitude/Longitude")){n=0; return;}
        if ((name == "tmerc")||
    	    (name == "Transverse Mercator")){n=1; return;}
        if ((name == "utm")||
    	    (name == "UTM")){n=2; return;}
	if ((name == "merc")||
    	    (name == "Mercator")){n=3; return;}
	std::cerr << "Unknown proj: " << name << ". Using Latitude/Longitude.\n";
	n=0;
    }

    std::string xml_str{
	switch (n){
    	    case  0: return "lonlat";
    	    case  1: return "tmerc";
    	    case  2: return "utm";
    	    case  3: return "merc";
    	    default:
    	    std::cerr << "xml_str: unknown datum: " << n << "\n";
    	    return "unknown";
	}
    }

    std::string oe_str{
	switch (n){
    	    case  0: return "Latitude/Longitude";
    	    case  1: return "Transverse Mercator";
    	    case  2: return "UTM";      // проверить, что в OE оно так называется
    	    case  3: return "Mercator"; // проверить, что в OE оно так называется
    	    default:
    	    std::cerr << "oe_str: unknown datum: " << n << "\n";
    	    return "Unknown";
	}
    }

};

Point<double> conv_to_std(const Point<double> & p, const Datum & D, const Proj & P){
    Point<double> p1,p2;
    p1=conv_to_lonlat(p,D,P);
    p2=conv_lonlat_to_wgs(p1,D);
    return p2;
}

// преобразование проекции к lonlat для заданной СК.
Point<double> conv_to_lonlat(const Point<double> & p, const Datum & D, const Proj & P){
    switch (P.n){
	case 0: return p;
	case 1: 
            lon0?
	    Point<double> p1;
    	    double a = GPS_Ellipse[GPS_Datum[datum].ellipse].a;
	    double f = 1/GPS_Ellipse[GPS_Datum[datum].ellipse].invf;
	    GPS_Math_TMerc_EN_To_LatLon(p.x, p.y, &p1.x, &p1.y, 0, lon0, 0, 500000, 1, a, a*(1-f))
	case 2:
    }
}

//
/*
 void GPS_Math_Known_Datum_To_Known_Datum_M(double Sphi, double Slam, double SH,
                                           double *Dphi, double *Dlam,
                                           double *DH, int32 n1, int32 n2);
void GPS_Math_Mercator_LatLon_To_EN(double phi, double lambda, double *E,
                                    double *N, double phi0, double lambda0,
                                    double E0, double N0, double a, double b);
void GPS_Math_Mercator_EN_To_LatLon(double E, double N, double *phi,
                                    double *lambda, double phi0,
                                    double lambda0, double E0, double N0,
                                    double a, double b);
void GPS_Math_TMerc_LatLon_To_EN(double phi, double lambda, double *E,
                                 double *N, double phi0, double lambda0,
                                 double E0, double N0, double F0,
                                 double a, double b);
void GPS_Math_TMerc_EN_To_LatLon(double E, double N, double *phi,
                                 double *lambda, double phi0, double lambda0,
                                 double E0, double N0, double F0,
                                 double a, double b);

*/