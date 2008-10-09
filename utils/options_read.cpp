#include <iostream>
#include <fstream>
#include "options.h"

//unit-test for Options object
using namespace std;
using namespace mapsoft;


main(int argc, char **argv){
  if (argc==1) cerr << "usage: " << argv[0] << " <option files>\n";

  Options O;

  for (int i=1; i<argc; i++){
    ifstream s(argv[i]);
    s >> O;
  }
  cout << O;
}
