#include <srtm/map_tracer.h>
#include <loaders/image_png.h>
#include <2d/rainbow.h>

/* srtm3 example: map river tracer */

#define DMS(x,y,z) ((x)*3600+(y)*60+z)/3

int
main(){
  const int lat1 = DMS(53,45,00); // picture corners
  const int lat2 = DMS(54,15,00);
  const int lon1 = DMS(95,30,00);
  const int lon2 = DMS(96,30,00);


  map_tracer m(lat1, lon1, lat2, lon2);
  m.set_dirs(1000,1000);
  m.set_areas();

  simple_rainbow R(500,3000);
  iImage img(lon2-lon1, lat2-lat1);

  iPoint p = iPoint(lon1+100, lat1+100);

  for (int lat=m.lat2-1; lat>=m.lat1; lat--){
    for (int lon=m.lon1; lon<m.lon2; lon++){

      short h  = m.geth(lat,lon);
      int c = (h > srtm_min)? R.get(h) : 0;

      if (m.pt(lat,lon)->marea > 0.5) c=0;
      if (m.pt(lat,lon)->rarea > 0.5) c=0xff0000;

      img.set(lon-lon1, lat2-lat-1, c);
    }
  }
  image_png::save(img, "map_trace.png");
}

