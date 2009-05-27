#include "geo_nom.h"
#include <iostream>
#include <cstring>

// command line interface to convs::nom_name/nom_range

using namespace std;
using namespace convs;

void usage(){
  cerr << "\n"
       << "Command line interface to convs::nom_name/nom_range conversions.\n"
       << "usage: echo <points> | convs_nom -n <rscale>\n"
       << "       echo <names>  | convs_nom -r\n"
       << "\n"
  ;
  exit(1);
}

int main(int argc, char** argv){

  if ((argc == 3) && (strcmp(argv[1], "-n") == 0)){
    while (!cin.eof()){
      Point<double> p;
      cin >> p;
      cout << nom_name(p, atoi(argv[2])) << "\n";
    }
  }
  else if ((argc == 2) && (strcmp(argv[1], "-r") == 0)){
    while (!cin.eof()){
      string s;
      cin >> s;
      if (s!="") cout << nom_range(s) << "\n";
    }
  }
  else usage();
}
