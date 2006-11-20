#ifndef GEO_CONVS_H
#define GEO_CONVS_H

// Геодезические проекции и системы координат

// нам нужны следующие вещи:
// - прочитав из файла (текстовое) название проекции и системы координат, преобразовать правильно точки
// - записать в файл правильное (текстовое) название проекции и СК (разное для разных форматов)


#include <string>
#include "../jeeps/gpsdatum.h"
#include "../jeeps/gpsproj.h"
#include "../jeeps/gpsmath.h"
#include "point.h"
#include "mapsoft_options.h"

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

  std::string xml_str(){
    switch (n){
      case 118: return "wgs84";
      case  92: return "pulkovo";
      default:
	std::cerr << "xml_str: unknown datum: " << n << "\n";
	return "unknown";
    }
  }

  std::string oe_str(){
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

    std::string xml_str(){
	switch (n){
    	    case  0: return "lonlat";
    	    case  1: return "tmerc";
    	    case  2: return "utm";
    	    case  3: return "merc";
    	    default:
    	    std::cerr << "xml_str: unknown proj: " << n << "\n";
    	    return "unknown";
	}
    }

    std::string oe_str(){
	switch (n){
    	    case  0: return "Latitude/Longitude";
    	    case  1: return "Transverse Mercator";
    	    case  2: return "UTM";      // проверить, что в OE оно так называется!
    	    case  3: return "Mercator"; // проверить, что в OE оно так называется!
    	    default:
    	    std::cerr << "oe_str: unknown proj: " << n << "\n";
    	    return "Unknown";
	}
    }

};

// преобразование любой проекции к lonlat для заданной СК.
Point<double> conv_anyproj_to_lonlat(Point<double> p, const Datum & D, const Proj & P, Options & opts){
    Point<double> p1;
    double lat0,lon0,E0,N0,k;
    int zone;
    char zc;
    double a = GPS_Ellipse[GPS_Datum[D.n].ellipse].a;
    double f = 1/GPS_Ellipse[GPS_Datum[D.n].ellipse].invf;
    switch (P.n){
	case 0: return p;
	case 1: // tmerc
	    // Мы можем использовать префикс в координате X, как на советских картах.
	    lon0=0;
            if (p.x>999999){
              lon0=((int)(p.x/1e6)-1)*6+3;
              p.x -= floor(p.x/1e6)*1e6;
            }
            // А можем явно указать осевой меридиан.
	    lon0 = opts.get_double("lon0", lon0);
            // Остальные параметры взятые по умолчанию обычно хороши
            lat0=opts.get_double("lat0",0);  
            N0=opts.get_double("N0",0);  
            E0=opts.get_double("E0",500000);  
            k=opts.get_double("k",1);  
	    GPS_Math_TMerc_EN_To_LatLon(p.x, p.y, &p1.y, &p1.x, lat0, lon0, E0, N0, k, a, a*(1-f));
	    return p1;
	case 2: // UTM
            // Я не знаю, какие здесь нужны параметры... Разберемся потом.
	    zone = opts.get_int("zone",0);
	    zc = opts.get_char("zc",'C');
	    GPS_Math_UTM_EN_To_WGS84(&p1.y, &p1.x, p.x, p.y, zone, zc);
	    return p1;
	case 3: // merc
            // Я не знаю, какие здесь нужны параметры... Разберемся потом.
	    lon0 = opts.get_double("lon0", 0);
            lat0=opts.get_double("lat0",0);  
            N0=opts.get_double("N0",0);  
            E0=opts.get_double("E0",0);  

	    GPS_Math_Mercator_EN_To_LatLon(p.x, p.y, &p1.y, &p1.x, lat0, lon0, E0, N0, a, a*(1-f));
	    return p1;
	default: 
	    std::cerr << "unknown proj: " << P.n << "\n";
            return p;
    }
}

// построение любой проекции для заданной СК.
Point<double> conv_lonlat_to_anyproj(const Point<double> & p, const Datum & D, const Proj & P, Options & opts){
    Point<double> p1;
    double lon0, lat0, E0, N0, k;
    int zone;
    char zc;
    double a = GPS_Ellipse[GPS_Datum[D.n].ellipse].a;
    double f = 1/GPS_Ellipse[GPS_Datum[D.n].ellipse].invf;
    switch (P.n){
	case 0: return p;
	case 1: // tmerc
            // осевой меридиан определим автоматически:
            lon0 = floor( lon0/6.0 ) * 6 + 3;  
            // А, может быть, его указали явно
	    lon0 = opts.get_double("lon0", lon0);

            // Остальные параметры взятые по умолчанию обычно хороши
            lat0=opts.get_double("lat0",0);  
            N0=opts.get_double("N0",0);  
            E0=opts.get_double("E0",500000);  
            k=opts.get_double("k",1);  
	    GPS_Math_TMerc_LatLon_To_EN(p.y, p.x, &p1.x, &p1.y, lat0, lon0, E0, N0, k, a, a*(1-f));
            // Добавим к координате префикс - как на советских картах:
            p1.x += 1e6 * (floor((lon0-3)/6)+1);
	    return p1;
	case 2: // UTM
	    std::cerr << "conversion latlon -> utm is not supported. fixme!\n";
	    return p1;
	case 3: // merc
	    std::cerr << "conversion latlon -> utm is not supported. fixme!\n";
	    return p;
	default: 
	    std::cerr << "unknown proj: " << P.n << "\n";
            return p;
    }
}

// преобразование любых координат к latlon wgs84
Point<double> conv_any_to_std(const Point<double> & p, const Datum & D, const Proj & P, Options & O){
    Point<double> p1,p2;
    p1=conv_anyproj_to_lonlat(p,D,P,O);

    double h;
    GPS_Math_Known_Datum_To_WGS84_M(p1.y, p1.x, 0, &p2.y, &p2.x, &h, D.n);

    return p2;
}

// преобразование из latlon wgs84 в любые координаты
Point<double> conv_std_to_any(const Point<double> & p, const Datum & D, const Proj & P, Options & O){
    Point<double> p1,p2;
    double h;
    GPS_Math_WGS84_To_Known_Datum_M(p.y, p.x, 0, &p1.y, &p1.x, &h, D.n);

    p2=conv_lonlat_to_anyproj(p1,D,P,O);
    return p2;
}

Point<double> conv_any_to_any(const Point<double> & p, 
	const Datum & SD, const Proj & SP, 
	const Datum & DD, const Proj & DP, Options & O){
    if ((SP.n==DP.n) && (SD.n==DD.n)) return p;
    return conv_std_to_any(conv_any_to_std(p, SD, SP, O), DD, DP, O);
}

#endif
