#include <fstream>
#include "../core/libfig/fig_utils.h"
#include "../core/libfig/fig_io.h"

// Обрезка fig-файла по объекту-прямоугольнику с комментарием CUTTER
// (из нескольких - по последнему)

main(int argc, char **argv){
  if (argc<2) {
      std::cerr << "usage: " << argv[0] << " <fig>\n";
      exit(0);
  }
  fig::fig_world W;
  if (! fig::read(argv[1], W)){
    std::cerr << "File is not modified.\n";
    exit(1);
  }

  fig::fig_world::iterator o = W.begin();

  iRect cutter;

  fig::fig_world W1;

  while (o!=W.end()){
    if ((o->comment.size() > 0) && 
        (o->comment[0].compare(0,6,"CUTTER")==0) &&
        (o->type == 2) &&
        ((o->sub_type == 2) || (o->sub_type == 5)) &&
        (o->size() > 3)){
      cutter = iRect((*o)[0], (*o)[2]);
      W1.push_back(*o);
      o=W.erase(o);
      o++;
    }
    else if (o->depth<40){ // FIXHACK!
      W1.push_back(*o);
      o=W.erase(o);
    }
    else o++;
  }

  fig::rect_crop(cutter, W);
  W.insert(W.end(), W1.begin(), W1.end());
  std::ofstream f(argv[1]);
  fig::write(f, W);
}
