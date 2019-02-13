// Получение fig-картинки со всеми знаками

#include <string>
#include <fstream>
#include <cstring>
#include "vmap/zn_lists.h"

using namespace std;

int
main(int argc, char** argv){

  bool cmap=false;
  if ((argc >=2 ) && (strcmp("-c", argv[1])==0)){
    cmap=true;
    argc--;
    argv++;
  }

  if (argc !=2 ){
    cerr << "usage: [-c] get_legend <style> > out.fig\n";
    return 1;
  }

  string style = argv[1];

  zn::zn_conv zconverter(style);
  fig::fig_world F = make_legend(zconverter, cmap);
  fig::write(cout, F);
  return 0;
}
