// типы данных, используемые в геоданных

#include "geo_types.h"
#include <sstream>
#include <cstdlib>
#include <cerrno>

const int Enum::fmt_mask = 0xFFFF;
const int Enum::xml_fmt  = 1<<16;
const int Enum::oe_fmt   = 1<<17;

int Enum::output_fmt = Enum::xml_fmt;


void Enum::set_from_string(const std::string & str){

  geo_types_table & data = get_table();
  geo_types_table::const_iterator i = data.find(str);

  if (i == data.end()){ // no entry in the table

    char *p;
    int n = strtol(str.c_str(), &p, 0); // number?
    if (*p==0){ // strtol reachs end of string
      val=n;
      return;
    }

    i = data.find(""); // default value
    if (i == data.end()){
      std::cerr << "Fatal error: no default value\n";
      exit(0);
    }
    std::cerr << "Unknown value: " << str << ". Using default: " << i->second << "\n";
  }
  val=i->second & Enum::fmt_mask;
}


std::ostream & operator<< (std::ostream & s, const Enum & e){
  geo_types_table & data = e.get_table();

  for (geo_types_table::const_iterator
         i=data.begin(); i!=data.end(); i++){
    if (((i->second & Enum::fmt_mask)   == e.val) &&
        ((i->second & Enum::output_fmt) != 0)) return s << i->first;
  }
  return s << e.val; // print number
}

std::istream & operator>> (std::istream & s, Enum & e){
  std::string str;
  getline(s, str);
  e.set_from_string(str);
  return s;
}

/***********************************************************/

geo_types_table Datum::names;

Datum::Datum(const std::string & str){
  if (names.size()==0) create_table();
  set_from_string(str);
}

void Datum::create_table(){
  // int values are from ../jeeps/gpsdatum.c
  names[""]       = 118;
  names["wgs84"]  = 118 | Enum::xml_fmt;
  names["wgs 84"] = 118;
  names["WGS84"]  = 118;
  names["WGS 84"] = 118 | Enum::oe_fmt;

  names["Pulkovo 1942"]     = 92 | Enum::oe_fmt;
  names["Pulkovo 1942 (1)"] = 92;
  names["Pulkovo 1942 (2)"] = 92;
  names["pulk"]             = 92;
  names["pulkovo"]          = 92 | Enum::xml_fmt;
  names["pulk42"]           = 92;
}

/***********************************************************/

geo_types_table Proj::names;

Proj::Proj(const std::string & str){
  if (names.size()==0) create_table();
  set_from_string(str);
}

void Proj::create_table(){
  names[""]                    = 0;
  names["lonlat"]              = 0 | Enum::xml_fmt;
  names["Latitude/Longitude"]  = 0 | Enum::oe_fmt;
  names["tmerc"]               = 1 | Enum::xml_fmt;
  names["Transverse Mercator"] = 1 | Enum::oe_fmt;
  names["utm"]                 = 1 | Enum::xml_fmt;
  names["UTM"]                 = 2 | Enum::oe_fmt;  // проверить, что в OE оно так называется!
  names["merc"]                = 3 | Enum::xml_fmt;
  names["Mercator"]            = 3 | Enum::oe_fmt;  // проверить, что в OE оно так называется!
  names["google"]              = 4 | Enum::xml_fmt; // в Ozi что-то такое должно быть...
  names["ks"]                  = 5 | Enum::xml_fmt;
}

/***********************************************************/


