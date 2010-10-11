// Получение fig-картинки со всеми знаками

#include <string>
#include <fstream>
#include "vmap/zn_lists.h"

using namespace std;

// изготовление текстовой таблички с названиями знаков

void usage(){
    cerr << "usage: get_text <style> > out.txt\n";
    exit(0);
}


main(int argc, char** argv){

  if (argc != 2) usage();
  string style = argv[1];

  zn::zn_conv zconverter(style);
  cout << zn::make_text(zconverter);
}
