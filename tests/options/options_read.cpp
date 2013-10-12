#include <iostream>
#include <fstream>
#include <options/options.h>

#include <options/m_time.h>

using namespace std;



main(int argc, char **argv){

  Time T("2013-10-04T06:50:26Z");
  std::cerr << T << "\n";

  if (argc==1) cerr << "usage: " << argv[0] << " <option files>\n";

  Options O;

  for (int i=1; i<argc; i++){
    ifstream s(argv[i]);
    s >> O;
  }
  cout << O;


}
