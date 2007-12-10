// Получение fig-картинки со всеми знаками

#include <string>
#include <fstream>
#include "zn_lists.h"

using namespace std;

void usage(){
    cerr << "usage: get_legend <conf_file> > out.fig\n";
    exit(0);
}


main(int argc, char** argv){

  if (argc != 2) usage();
  string conf_file = argv[1];

  zn::zn_conv zconverter(conf_file);
  fig::fig_world F = make_legend(zconverter);
  fig::write(cout, F);
}
