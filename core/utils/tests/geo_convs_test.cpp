#include <iostream>
#include "geo/geo_convs.h"

int main(){
  Options o;

  o["lon0"] = "39";
  dPoint p1(38.888, 56.100);
  dPoint p2 = conv_any_to_std(p1, "pulk42", "lonlat", o);
  dPoint p3 = conv_std_to_any(p2, "pulk42", "tmerc", o);
  std::cerr << p1 << "\n"; 
  std::cerr << p2 << "\n"; 
  std::cerr << p3 << "\n"; 


}


