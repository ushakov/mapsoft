#include <iostream>
#include <utils/mapsoft_options.h>

int main(){
  Options o;
  o["o1"] = "a";
  o["o2"] = "b";
  o["o3"] = "2006-10-10";

  std::cerr << o;

  std::cerr << "o1 = " << o.get_string("o1", "not found") << "\n";
  std::cerr << "o3 = " << o.get_time("o3") << "\n";

  std::cerr << o;


  GenericAccessor * ga = CreateGenericAccessor(&o);
  std::vector<std::string> names = ga->get_names();
  for (int i = 0; i < names.size(); ++i) {
      std::cerr << i << ": " << names[i] << " -> " << ga->get(names[i]) << std::endl;
  }
  ga->set("o1", "abc");

  std::cerr << o;

}


