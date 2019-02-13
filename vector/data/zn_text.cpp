// Получение fig-картинки со всеми знаками

#include <string>
#include <fstream>
#include "vmap/zn_lists.h"

using namespace std;

// изготовление текстовой таблички с названиями знаков
int
main(int argc, char** argv){

  if (argc != 2){
    cerr << "usage: get_text <style> > out.txt\n";
    return 1;
  }
  string style = argv[1];

  zn::zn_conv zconverter(style);
  cout << zn::make_text(zconverter);
  return 0;
}
