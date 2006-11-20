#include <iostream>
#include <geo_convs.h>

int main(){
  Options o;

  o["lon0"] = "39";
  Point<double> p1(38.888, 56.100);
  Point<double> p2 = conv_any_to_std(p1, Datum("pulk42"), Proj("lonlat"), o);
  Point<double> p3 = conv_std_to_any(p2, Datum("pulk42"), Proj("tmerc"), o);
  std::cerr << p1 << "\n"; 
  std::cerr << p2 << "\n"; 
  std::cerr << p3 << "\n"; 


}


