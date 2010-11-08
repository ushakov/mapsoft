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


// Для каждой пары соседних точек из l1 ищем минимальное
// расстояние от точек l2 до прямой, задаваемой парой.
// Ищем максимум этой величины (но не менее maxdist) по всем парам.
// Раастояние считается положительным для точек снаружи от l1
// (если l1 ориентирована по часовой стрелке) и отрицательно внутри.
// maxdist, p1,p2 выставляются в соответствии с найденным максимумом.
void
test_pairs(const dLine & l1, const dLine & l2,
           double & maxdist, dPoint & p1, dPoint & p2){
  dLine::const_iterator i,j;
  for (i=l1.begin(); i!=l1.end(); i++){
    j=i+1; if (j==l1.end()) j=l1.begin();
    if (*i==*j) continue;
    double mindist=INFINITY;
    dLine::const_iterator k;
    for (k=l2.begin(); k!=l2.end(); k++){
      dPoint v1(*j-*i);
      dPoint v2(*k-*i);
      double K = pscal(v1,v2)/pscal(v1,v1);
      double d = pdist(K*v1-v2);
      if ((v1.x*v2.y-v2.x*v1.y)<0) d*=-1;
      if (d<mindist) mindist=d;
    }
    if (maxdist < mindist){
      maxdist = mindist;
      p1=*i; p2=*j;
    }
  }
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
  // find border
  l1=convex_border(l1);
  l2=convex_border(l2);

  std::cerr << " border points: " << l1.size() << " " << l2.size() << "\n";

  // Support vectors (http://en.wikipedia.org/wiki/Support_vector_machine)
  // лежат на выпуклой границе множства точек. При этом по крайней мере
  // в одном из множеств их две штуки и они - соседи в выпуклой границе.

  // Пары соседних точек границ ищем минимальное
  // расстояние от точек чужой границы до прямой, заданной этой парой.
  // Ищем максимум этой величины по всем парам.

  // Границы, полученные из convex_border() обходятся по часовой стрелке,
  // так что расстояния положительны снаружи от границы.

  double maxdist=-INFINITY;
  dPoint p1,p2;
  test_pairs(l1,l2,maxdist,p1,p2);
  test_pairs(l2,l1,maxdist,p1,p2);

  // Мы нашли два support vector'а: p1 и p2 в одной из границ и
  // расстояние maxdist - ширину зазора между множествами
  // (возможно, отрицательную).

  // Сместим линию, задаваемую p1 и p2 в сторону положительных расстояний
  // на maxdist/2.

  dPoint t = pnorm(p2-p1);
  dPoint n(t.y, -t.x);
  dPoint p0 = p1 + n*maxdist/2.0;

  // Нужная линия задается точкой p0 и направлением t!

std::cerr << p0 << " " << t << " " << maxdist << "\n";


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


