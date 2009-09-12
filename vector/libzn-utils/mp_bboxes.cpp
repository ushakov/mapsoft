#include "../../core/libmp/mp.h"
#include <iomanip>

/*****************************************************/
int main(int argc, char** argv){
  if (argc < 3){
    std::cerr << "usage: mp_bboxes <dist, deg> <mp files> ...\n";
    exit(0);
  }

  double dist=atof(argv[1]);
  if ((dist<=0) || (dist>=180)) {
      std::cerr << "ERR: wrong distance: " << argv[1] << "\n";
      return 1;
  }

  // read files
  mp::mp_world M;
  for (int i=2; i<argc; i++){
    if (!mp::read(argv[i], M)) {
      std::cerr << "ERR: bad mp file: " << argv[i] << "\n";
      return 1;
    }
  }
  double area=0;
  double count=0;

  while (M.size()!=0){
    bool new_range=true;
    dRect r0;
    int objects=0;
    int oldsize;
    do {
      oldsize = M.size();
      mp::mp_world::iterator i=M.begin();
      while (i!=M.end()){
        dRect r1=rect_pump(i->range(),1e-6);
        if (new_range) {
          r0=r1;
          new_range=false;
          i=M.erase(i);
          objects++;
          continue;
        }
        else {
          r1=rect_bounding_box(r1,r0);
          if ( (fabs(r1.w-r0.w)<dist) &&
               (fabs(r1.h-r0.h)<dist)){
            r0=r1;
            i=M.erase(i);
            objects++;
            continue;
          }
        }
        i++;
      }
    } while (oldsize!=M.size());
    std::cout << std::fixed << std::setw(4)
              << r0 << " "
              << r0.w*r0.h << " "
              << objects << "\n";
    count++;
    area+=r0.w*r0.h;
  }
  std::cout << "dist: " << dist 
            << " count: " << count 
            << " area: " << area << "\n";
}
