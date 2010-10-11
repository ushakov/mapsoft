#include "mp/mp.h"
#include <iomanip>
#include <set>

double area(const dRect &r){return r.w*r.h;}

/*****************************************************/
int main(int argc, char** argv){
  if (argc < 4){
    std::cerr << "usage: mp_bboxes <padding, deg> <max_size, deg> <mp files> ...\n";
    exit(0);
  }

  double padd=atof(argv[1]);
  double maxs=atof(argv[2]);

  if ((padd<=0) || (padd>=180)) {
      std::cerr << "ERR: wrong padding: " << argv[1] << "\n";
      return 1;
  }
  if ((maxs<=0) || (maxs>=180)) {
      std::cerr << "ERR: wrong max size: " << argv[2] << "\n";
      return 1;
  }

  // read files
  mp::mp_world M;
  for (int i=3; i<argc; i++){
    if (!mp::read(argv[i], M)) {
      std::cerr << "ERR: bad mp file" << argv[i] << "\n";
      return 1;
    }
  }

  std::set<dRect> rects;
  for (mp::mp_world::iterator i=M.begin(); i!=M.end(); i++)
    rects.insert(rect_pump(i->range(),padd));

  int oldsize;
  do {
    oldsize=rects.size();
    std::set<dRect>::iterator i,j,im=rects.end(),jm=rects.end();
    double min=1e99;
    for (i=rects.begin(); i!=rects.end(); i++){
      for (j=i; j!=rects.end(); j++){
        if (i==j) continue;
        dRect r=rect_bounding_box(*i,*j);
        if ((r.w>maxs)||(r.h>maxs)) continue;
        double a=area(rect_intersect(*i,*j));
        if (a<min) {a=min; im=i; jm=j;}
      }
    }
    if ((im!=rects.end()) && (jm!=rects.end())){
      dRect r=rect_bounding_box(*im,*jm);
      rects.erase(im);
      rects.erase(jm);
      rects.insert(r);
    }
    std::cerr << rects.size() << " ";
  } while (oldsize!=rects.size());

  double a=0;
  std::set<dRect>::const_iterator i;
  for (i=rects.begin(); i!=rects.end(); i++){

    std::cout << std::fixed << std::setw(4)
              << *i << " " << area(*i) << "\n";
    a+=area(*i);
  }
  std::cout << "padding: " << padd
            << " maxs: " << maxs
            << " count: " << rects.size()
            << " area: " << a << "\n";
}
