#include "fig/fig_utils.h"
#include "fig/fig_io.h"

// Сортировка fig-файла (для удобства хранения в git, просмотра diff'a и т.п.)
// Реально так и не использовалась, кажется.

int
main(int argc, char **argv){
  if (argc<2) {
      std::cerr << "usage: " << argv[0] << " <fig>\n";
      exit(0);
  }
  fig::fig_world W;

  if (! fig::read(argv[1], W)){
    std::cerr << "Read error. File was not modified.\n";
    exit(1);
  }

  fig::fig_world::iterator i=W.begin();
  while (i!=W.end()){
    if ((i->type==6) || (i->type==-6)) i=W.erase(i);
    else i++;
  }

  W.sort();

  fig::write(argv[1], W);
  return 0;
}
