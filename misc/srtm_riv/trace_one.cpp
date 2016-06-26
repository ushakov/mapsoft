#include <vector>
#include <2d/point_int.h>
#include <2d/rainbow.h>
#include <loaders/image_png.h>
#include <srtm/tracers.h>

// алгоритм нахождения русла реки
using namespace std;

main(){
  dPoint p0(95.786934, 54.057950);
  SRTM3 S;

  bool down=true;
  int nmax=1000; // максимальный размер бессточных областей
  int hmin=600;  // минимальная высота - критерий остановки

  size_t srtmw = S.get_width()-1;
  vector<iPoint> R = trace_river(S, p0*srtmw, nmax, hmin, down);
  if (!R.size()) exit(1);

  // find data range
  iRect r(*R.begin(),*R.begin());
  vector<iPoint>::const_iterator b;
  for (b = R.begin(); b!=R.end(); b++) r = rect_pump(r,*b);
  r = rect_pump(r,5);

  //
  iImage img(r.w, r.h);
  simple_rainbow rb(500,3000);

  for (int y=0; y<r.h; y++){
    for (int x=0; x<r.w; x++){
      iPoint p(x+r.x, y+r.y);
      short h  = S.geth(p, true);
      int c = rb.get(h);
      img.set_na(x,r.h-y-1,c);
    }
  }
  for (b = R.begin(); b!=R.end(); b++){
    img.set_na(b->x-r.x, r.y+r.h - b->y - 1, 0);
  }
  image_png::save(img, "trace_one.png");
}
