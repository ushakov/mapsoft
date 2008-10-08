#ifndef GEO_NAMES_H
#define GEO_NAMES_H

// Геодезические проекции и системы координат

// нам нужны следующие вещи:
// - прочитав из файла (текстовое) название проекции и системы координат, преобразовать правильно точки
// - записать в файл правильное (текстовое) название проекции и СК (разное для разных форматов)


#include <string>
#include "../utils/mapsoft_options.h"
#include <boost/operators.hpp>


struct Datum :
    public boost::equality_comparable<Datum>
{
  int n; // номера систем координат из ../jeeps/gpsdatum.c. Надо ли их все сюда перенести?..

  bool operator== (const Datum & d) const { return (n==d.n);}

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

  std::string xml_str() const {
    switch (n){
      case 118: return "wgs84";
      case  92: return "pulkovo";
      default:
	std::cerr << "xml_str: unknown datum: " << n << "\n";
	return "unknown";
    }
  }

  std::string oe_str() const {
    switch (n){
      case 118: return "WGS 84";
      case  92: return "Pulkovo 1942";
      default:
	std::cerr << "oe_str: unknown datum: " << n << "\n";
	return "Unknown";
    }
  }

};

struct Proj :
public boost::equality_comparable<Proj>
{
    int n;

    bool operator== (const Proj & p) const { return (n==p.n);}

    Proj(const std::string & name = "lonlat"){
	if ((name == "lonlat")||
	    (name == "Latitude/Longitude")){n=0; return;}
        if ((name == "tmerc")||
    	    (name == "Transverse Mercator")){n=1; return;}
        if ((name == "utm")||
    	    (name == "UTM")){n=2; return;}
	if ((name == "merc")||
    	    (name == "Mercator")){n=3; return;}
	if  (name == "google"){n=4; return;}
	if  (name == "ks"){n=5; return;}
	std::cerr << "Unknown proj: " << name << ". Using Latitude/Longitude.\n";
	n=0;
    }

    std::string xml_str() const {
	switch (n){
    	    case  0: return "lonlat";
    	    case  1: return "tmerc";
    	    case  2: return "utm";
    	    case  3: return "merc";
    	    case  4: return "google";
    	    case  5: return "ks";
    	    default:
    	    std::cerr << "xml_str: unknown proj: " << n << "\n";
    	    return "unknown";
	}
    }

    std::string oe_str() const {
	switch (n){
    	    case  0: return "Longitude/Latitude";
    	    case  1: return "Transverse Mercator";
    	    case  2: return "UTM";      // проверить, что в OE оно так называется!
    	    case  3: return "Mercator"; // проверить, что в OE оно так называется!
    	    case  4: return "Google";   // Как это называется в Ozi???
    	    default:
    	    std::cerr << "oe_str: unknown proj: " << n << "\n";
    	    return "Unknown";
	}
    }

};
#endif
