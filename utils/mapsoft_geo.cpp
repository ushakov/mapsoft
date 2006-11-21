// Геодезические проекции и системы координат

#include <cmath>
#include "../jeeps/gpsdatum.h"
#include "../jeeps/gpsproj.h"
#include "../jeeps/gpsmath.h"
#include "mapsoft_geo.h"

// преобразование проекции lonlat из любой СК в wgs84
void conv_anydatum_to_wgs84(Point<double> & p, const Datum & D){
    double x,y,h;
    GPS_Math_Known_Datum_To_WGS84_M(p.y, p.x, 0, &y, &x, &h, D.n);
    p.x=x; p.y=y;
}

// преобразование проекции lonlat из wgs84 в любую СК
void conv_wgs84_to_anydatum(Point<double> & p, const Datum & D){
    double x,y,h;
    GPS_Math_WGS84_To_Known_Datum_M(p.y, p.x, 0, &y, &x, &h, D.n);
    p.x=x; p.y=y;
}

// преобразование любой проекции к lonlat для заданной СК.
void conv_anyproj_to_lonlat(Point<double> & p, const Datum & D, const Proj & P, const Options & Po){
    double x,y;

    // значения по умолчанию для разных параметров:
    double lat0,lon0,E0,N0,k;
    int zone;
    char zc;

    double a = GPS_Ellipse[GPS_Datum[D.n].ellipse].a;
    double f = 1/GPS_Ellipse[GPS_Datum[D.n].ellipse].invf;
    switch (P.n){
	case 0: return;
	case 1: // tmerc
	    // Мы можем использовать префикс в координате X, как на советских картах.
	    lon0=0;
            if (p.x>999999){
              lon0=((int)(p.x/1e6)-1)*6+3;
              p.x -= floor(p.x/1e6)*1e6;
            }
            // А можем явно указать осевой меридиан.
	    lon0 = Po.get_double("lon0", lon0);
            lat0 = Po.get_double("lat0",0);  
            N0   = Po.get_double("N0",0);  
            E0   = Po.get_double("E0",500000);  
            k    = Po.get_double("k",1);  
	    GPS_Math_TMerc_EN_To_LatLon(p.x, p.y, &y, &x, lat0, lon0, E0, N0, k, a, a*(1-f));
            p.x = x; p.y = y;
	    return;
	case 2: // UTM
            // Я не знаю, какие здесь нужны параметры... Разберемся потом.
	    zone = Po.get_int("zone",0);
	    zc   = Po.get_char("zc",'C');
	    GPS_Math_UTM_EN_To_WGS84(&y, &x, p.x, p.y, zone, zc);
            p.x = x; p.y = y;
	    return;
	case 3: // merc
            // Я не знаю, какие здесь нужны параметры... Разберемся потом.
	    lon0 = Po.get_double("lon0",0);
            lat0 = Po.get_double("lat0",0);  
            N0   = Po.get_double("N0",0);  
            E0   = Po.get_double("E0",0);  

	    GPS_Math_Mercator_EN_To_LatLon(p.x, p.y, &y, &x, lat0, lon0, E0, N0, a, a*(1-f));
            p.x = x; p.y = y;
	    return;
	default: 
	    std::cerr << "unknown proj: " << P.n << "\n";
            return;
    }
}

// построение любой проекции для заданной СК.
void conv_lonlat_to_anyproj(Point<double> & p, const Datum & D, const Proj & P, const Options & Po){
    double x,y;
    double lon0, lat0, E0, N0, k;
    int zone;
    char zc;
    double a = GPS_Ellipse[GPS_Datum[D.n].ellipse].a;
    double f = 1/GPS_Ellipse[GPS_Datum[D.n].ellipse].invf;
    switch (P.n){
	case 0: return;
	case 1: // tmerc
            // осевой меридиан определим автоматически:
            lon0 = floor( lon0/6.0 ) * 6 + 3;  
            // А, может быть, его указали явно
	    lon0 = Po.get_double("lon0", lon0);

            // Остальные параметры взятые по умолчанию обычно хороши
            lat0 = Po.get_double("lat0",0);  
            N0   = Po.get_double("N0",0);  
            E0   = Po.get_double("E0",500000);  
            k    = Po.get_double("k",1);  
	    GPS_Math_TMerc_LatLon_To_EN(p.y, p.x, &x, &y, lat0, lon0, E0, N0, k, a, a*(1-f));
            // Добавим к координате префикс - как на советских картах:
            x += 1e6 * (floor((lon0-3)/6)+1);
            p.x = x; p.y = y;
	    return;
	case 2: // UTM
	    std::cerr << "conversion latlon -> utm is not supported. fixme!\n";
	    return;
	case 3: // merc
	    std::cerr << "conversion latlon -> utm is not supported. fixme!\n";
	    return;
	default: 
	    std::cerr << "unknown proj: " << P.n << "\n";
            return;
    }
}

// преобразование любых координат к latlon wgs84
void conv_any_to_std(Point<double> & p, const Datum & D, const Proj & P, const Options & Po){
    conv_anyproj_to_lonlat(p,D,P,Po);
    conv_anydatum_to_wgs84(p,D);
}

// преобразование из latlon wgs84 в любые координаты
void conv_std_to_any(Point<double> & p, const Datum & D, const Proj & P, const Options & Po){
    conv_wgs84_to_anydatum(p,D);
    conv_lonlat_to_anyproj(p,D,P,Po);
}

void conv_any_to_any( Point<double> & p,
                      const Datum & sD, const Proj & sP, const Options & sPo,
                      const Datum & dD, const Proj & dP, const Options & dPo){
    if ((sP.n==dP.n) && (sD.n==dD.n)) return;
    conv_any_to_std(p, sD, sP, sPo);
    conv_std_to_any(p, dD, dP, dPo);
}

