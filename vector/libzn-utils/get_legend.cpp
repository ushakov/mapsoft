// Получение fig-картинки со всеми знаками

#include <string>
#include <fstream>
#include "../libzn/zn_lists.h"

using namespace std;

void usage(){
    cerr << "usage: get_legend <style> > out.fig\n";
    exit(0);
}


main(int argc, char** argv){

  if (argc != 2) usage();
  string style = argv[1];

  zn::zn_conv zconverter(style);
  fig::fig_world F = make_legend(zconverter);
  fig::write(cout, F);
}
