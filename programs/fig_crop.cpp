#include <fstream>
#include "../geo_io/fig_utils.h"

// Обрезка fig-файла по объекту-прямоугольнику с комментарием CUTTER
// (из нескольких - по последнему)

main(int argc, char **argv){
  if (argc<2) {
      std::cerr << "usage: " << argv[0] << " fig\n";
      exit(0);
  }
  fig::fig_world W;
  if (! fig::read(argv[1], W)){
    std::cerr << "File is not modified.\n";
    exit(1);
  }

  fig::fig_world::iterator o = W.begin();

  Rect<int> cutter;

  while (o!=W.end()){
    if ((o->comment.size() > 0) && 
        (o->comment[0].compare(0,6,"CUTTER")==0) &&
        (o->type == 2) &&
        ((o->sub_type == 2) || (o->sub_type == 5)) &&
        (o->size() > 3)){
      cutter = Rect<int>((*o)[0], (*o)[2]);
      o=W.erase(o);
    }
    else o++;
  }

  fig::rect_crop(cutter, W);
  std::ofstream f(argv[1]);
  fig::write(f, W);
}
