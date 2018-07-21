#include "geo/geo_nom.h"
#include <iostream>
#include <cstring>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace convs;

void usage(){
  cerr << "\n"
       << "Map nomenclature conversions.\n"
       << "usage: convs_nom -p <point> <rscale> -- maps at the point\n"
       << "       convs_nom -r <range> <rscale> -- maps at the range\n"
       << "       convs_nom -n <name> -- map range\n"
       << "       convs_nom -c <name> -- map center\n"
       << "       convs_nom -s <name> x_shift y_shift -- adjecent map\n"
       << "       convs_nom -S <name> <rscale> -- maps of different scale\n"
       << "       convs_nom -t <name> <range>  -- check if the map touches the range\n"
       << "\n"
  ;
}

int
main(int argc, char** argv){

  if ((argc == 4) && (strcmp(argv[1], "-p") == 0)){
    string name = pt_to_nom(
        boost::lexical_cast<dPoint>(argv[2]),
        boost::lexical_cast<double>(argv[3]) );
    cout << name << "\n";
    return 0;
  }
  if ((argc == 4) && (strcmp(argv[1], "-r") == 0)){
    vector<string> names = range_to_nomlist(
        boost::lexical_cast<dRect>(argv[2]),
        boost::lexical_cast<double>(argv[3]) );
    for (vector<string>::iterator i=names.begin(); i!=names.end(); i++){
      cout << *i << "\n";
    }
    return 0;
  }
  else if ((argc == 3) && (strcmp(argv[1], "-n") == 0)){
    cout << setprecision(9) << nom_to_range(argv[2]) << "\n";
    return 0;
  }
  else if ((argc == 3) && (strcmp(argv[1], "-c") == 0)){
    cout << setprecision(9) << nom_to_range(argv[2]).CNT() << "\n";
    return 0;
  }
  else if ((argc == 5) && (strcmp(argv[1], "-s") == 0)){
    iPoint sh(boost::lexical_cast<int>(argv[3]),
              boost::lexical_cast<int>(argv[4]));
    cout << nom_shift(argv[2], sh) << "\n";
    return 0;
  }
  else if ((argc == 4) && (strcmp(argv[1], "-S") == 0)){
    dRect r = rect_pump(nom_to_range(argv[2]), -0.001, -0.001);
    vector<string> names = range_to_nomlist( r,
        boost::lexical_cast<double>(argv[3]) );
    for (vector<string>::iterator i=names.begin(); i!=names.end(); i++){
      cout << *i << "\n";
    }
    return 0;
  }
  else if ((argc == 4) && (strcmp(argv[1], "-t") == 0)){
    dRect r1 = nom_to_range(argv[2]);
    dRect r2 = boost::lexical_cast<dRect>(argv[3]);
    return rect_intersect(r1,r2).empty();
  }
  usage();
  return 1;
}
