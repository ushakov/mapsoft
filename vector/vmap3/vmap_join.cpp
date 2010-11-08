#include <string>
#include <fstream>
#include <iostream>
#include "vmap/vmap.h"
#include "fig/fig.h"
#include "2d/line_utils.h"

using namespace std;

void usage(){
  const char * prog = "vmap_join";

  cerr
     << prog << " -- join two adjecent maps.\n"
     << "  usage: " << prog << " <in file1> <in file2> <out file>\n"
     << "\n"
  ;
  exit(1);
}

main(int argc, char **argv){

  if (argc!=4) usage();
  const char *in1 = argv[1];
  const char *in2 = argv[2];
  const char *out = argv[3];

  vmap::world V1 = vmap::read(in1);
  vmap::world V2 = vmap::read(in2);

  std::cerr << "Reading input files:\n";
  // Read files. It's faster to make convex_border also here
  dLine l1,l2;
  vmap::world::const_iterator o;
  dMultiLine::const_iterator l;
  for (o=V1.begin(); o!=V1.end(); o++){
    for (l=o->begin(); l!=o->end(); l++){
      dLine l0=convex_border(*l);
      l1.insert(l1.begin(), l0.begin(), l0.end());
    }
  }
  for (o=V2.begin(); o!=V2.end(); o++){
    for (l=o->begin(); l!=o->end(); l++){
      dLine l0=convex_border(*l);
      l2.insert(l2.begin(), l0.begin(), l0.end());
    }
  }

  dPoint p0;
  dPoint t;
  double m=margin_classifier(l1,l2,p0,t);

  // Теперь нам нужно найти, что мы можем склеить.
  // Нас интересуют во-первых, концы линий, а, во-вторых, стороны
  // многоугольников, лежащие близко к линии склейки. Тип и название
  // объекта тоже важны (чтоб Ока с Волгой случайно не склеилась).

std::cerr << p0 << " " << t << " " << m << "\n";



fig::fig_world F;
fig::fig_object f=fig::make_object("2 1 0 1  0 7 80 -1 -1 0.000 1 0 0 0 0 0");
f.iLine::operator=(l1*fig::cm2fig*10);
F.push_back(f); f.clear();
f.pen_color=0x0000FF;
f.iLine::operator=(l2*fig::cm2fig*10);
F.push_back(f); f.clear();
f.pen_color=0xFF0000; f.depth=50;

f.pen_color=0xFF0000;
f.push_back(p0*fig::cm2fig*10);
f.push_back((p0+t/10.0)*fig::cm2fig*10);
F.push_back(f); f.clear();

std::ofstream stream(out);
fig::write(stream,F);

//  if (!vmap::write(ofile, V, O)) exit(1);

  exit(0);
}


