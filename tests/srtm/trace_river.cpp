#include <srtm/tracer.h>
#include <loaders/image_png.h>
#include <2d/rainbow.h>

/* srtm3 example - trace one river */


main(){

  tracer T;
//  T.trace_river(dPoint(95.9389167, 53.9771778));
  T.trace_river(dPoint(95.1750444, 54.1042528));

  iRect r=T.get_range(); r = rect_pump(r,1);
  std::set<iPoint> brd = T.get_border();

  simple_rainbow R(500,3000);
  iImage img(r.w, r.h);
  iPoint p;

  for (int lat=r.y+r.h-1; lat>=r.y; lat--){
    for (int lon=r.x; lon<r.x+r.w; lon++){

      iPoint p(lon,lat);
      short h  = T.S.geth(p, true);
      int c = 0xffffff;

      if (h > srtm_min){
         if (T.done.count(p)) c = R.get(h);
         if (brd.count(p))  c = 0;
         if (T.res.count(p))  c = 0xff0000;
      }

      img.set(lon-r.x, lat-r.y, c);
    }
  }
  image_png::save(img, "trace_river.png");
}

