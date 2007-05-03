#include <iostream>
#include "../geo_io/fig.h"
#include "../utils/poly.h"

main(int argc, char **argv){
  if (argc!=2){
    std::cerr << "usage: " << argv[0] << " file.fig\n";
    exit(0);
  }
  fig::fig_world W = fig::read(argv[1]);
  std::cerr << W.size() << " objects\n";

  fig::fig_world W1;

  Polygon<int> P;

  // выбрать все объекты такого вида:
  for (fig::fig_world::const_iterator i=W.begin(); i!=W.end(); i++){
    if (test_object(*i, "2 * * * * * * * * * * * * * * *")){
      Line<int> l;
      for (fig::fig_object::const_iterator p = i->begin(); p!=i->end(); p++){
        l.push_back(*p);
      }
      P.push_back(l);
    }
  }
  for (Polygon<int>::const_iterator i = P.begin(); i!=P.end(); i++){
    fig::fig_object o = fig::make_object("2 3 0 0 28 28 200 0 20 2.000 0 0 7 0 0 *");
    o.clear();
    for (Line<int>::const_iterator p = i->begin(); p!=i->end(); p++){
      o.push_back(*p);
    }
    W.push_back(o);
  }

  fig::write(std::cout, W);
}
