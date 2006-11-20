// ������������� �������� � ������� ���������

// ��� ����� ��������� ����:
// - �������� �� ����� (���������) �������� �������� � ������� ���������, ������������� ��������� �����
// - �������� � ���� ���������� (���������) �������� �������� � �� (������ ��� ������ ��������)


#include <cmath>
#include "../jeeps/gpsdatum.h"
#include "../jeeps/gpsproj.h"
#include "../jeeps/gpsmath.h"
#include "mapsoft_geo.h"

// �������������� ����� �������� � lonlat ��� �������� ��.
Point<double> conv_anyproj_to_lonlat(Point<double> p, const Datum & D, const Proj & P, Options & opts){
    Point<double> p1;

    // �������� �� ��������� ��� ������ ����������:
    double lat0,lon0,E0,N0,k;
    int zone;
    char zc;

    double a = GPS_Ellipse[GPS_Datum[D.n].ellipse].a;
    double f = 1/GPS_Ellipse[GPS_Datum[D.n].ellipse].invf;
    switch (P.n){
	case 0: return p;
	case 1: // tmerc
	    // �� ����� ������������ ������� � ���������� X, ��� �� ��������� ������.
	    lon0=0;
            if (p.x>999999){
              lon0=((int)(p.x/1e6)-1)*6+3;
              p.x -= floor(p.x/1e6)*1e6;
            }
            // � ����� ���� ������� ������ ��������.
	    lon0 = opts.get_double("lon0", lon0);
            lat0 = opts.get_double("lat0",0);  
            N0 = opts.get_double("N0",0);  
            E0 = opts.get_double("E0",500000);  
            k = opts.get_double("k",1);  
	    GPS_Math_TMerc_EN_To_LatLon(p.x, p.y, &p1.y, &p1.x, lat0, lon0, E0, N0, k, a, a*(1-f));
	    return p1;
	case 2: // UTM
            // � �� ����, ����� ����� ����� ���������... ���������� �����.
	    zone = opts.get_int("zone",0);
	    zc = opts.get_char("zc",'C');
	    GPS_Math_UTM_EN_To_WGS84(&p1.y, &p1.x, p.x, p.y, zone, zc);
	    return p1;
	case 3: // merc
            // � �� ����, ����� ����� ����� ���������... ���������� �����.
	    lon0 = opts.get_double("lon0",0);
            lat0 = opts.get_double("lat0",0);  
            N0 = opts.get_double("N0",0);  
            E0 = opts.get_double("E0",0);  

	    GPS_Math_Mercator_EN_To_LatLon(p.x, p.y, &p1.y, &p1.x, lat0, lon0, E0, N0, a, a*(1-f));
	    return p1;
	default: 
	    std::cerr << "unknown proj: " << P.n << "\n";
            return p;
    }
}

// ���������� ����� �������� ��� �������� ��.
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
            // ������ �������� ��������� �������������:
            lon0 = floor( lon0/6.0 ) * 6 + 3;  
            // �, ����� ����, ��� ������� ����
	    lon0 = opts.get_double("lon0", lon0);

            // ��������� ��������� ������ �� ��������� ������ ������
            lat0 = opts.get_double("lat0",0);  
            N0 = opts.get_double("N0",0);  
            E0 = opts.get_double("E0",500000);  
            k = opts.get_double("k",1);  
	    GPS_Math_TMerc_LatLon_To_EN(p.y, p.x, &p1.x, &p1.y, lat0, lon0, E0, N0, k, a, a*(1-f));
            // ������� � ���������� ������� - ��� �� ��������� ������:
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

// �������������� ����� ��������� � latlon wgs84
Point<double> conv_any_to_std(const Point<double> & p, const Datum & D, const Proj & P, Options & O){
    Point<double> p1,p2;
    p1=conv_anyproj_to_lonlat(p,D,P,O);

    double h;
    GPS_Math_Known_Datum_To_WGS84_M(p1.y, p1.x, 0, &p2.y, &p2.x, &h, D.n);

    return p2;
}

// �������������� �� latlon wgs84 � ����� ����������
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

