#include <iostream>
#include "utils/mapsoft_options.h"

int main(){
  Options o;
  o["o1"] = "a";
  o["o2"] = "b";
  o["o3"] = "2006-10-10";

  std::cerr << o;

  std::cerr << "o1 = " << o.get_string("o1", "not found") << "\n";
  std::cerr << "o3 = " << o.get_time("o3") << "\n";

  std::cerr << o;


}


