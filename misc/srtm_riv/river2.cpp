#include <map>
#include <2d/rainbow.h>
#include <srtm/tracer.h>
#include "pnm.h"

// построение "водосбора" одной горы

main(){

  tracer T;
  T.trace_mount(dPoint(96.0076861, 53.8938806));

  iRect r=T.get_range(); r = rect_pump(r,1);
  std::set<iPoint> brd = T.get_border();

  print_pnm_head(r.w, r.h);
  simple_rainbow R(500,3000);

  for (int lat=r.y+r.h-1; lat>=r.y; lat--){
    for (int lon=r.x; lon<r.x+r.w; lon++){

      iPoint p(lon,lat);
      short h  = T.S.geth(p, true);
      int c = 0xffffff;

      if (h > srtm_min){
         if (T.done.count(p)) c = R.get(h);
         if (brd.count(p))  c = 0;
         if (T.res.count(p))  c = 0xff;
      }
      print_pnm_col(c);
    }
  }
}
