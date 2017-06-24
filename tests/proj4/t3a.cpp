#include <stdio.h>
#include <libgeo/geo_convs.h>

int main(){

  Options o;
  o.put("lon0",39.0);
  o.put("E0",500000.0);
  convs::pt2pt cnv(Datum("wgs84"), Proj("lonlat"), Options(),
                  Datum("pulkovo"), Proj("tmerc"), o);


  dPoint p( 39 + ((double)rand()/RAND_MAX - 0.5) * 6,    // 36..42
                 ((double)rand()/RAND_MAX - 0.5) * 180); // -90..+90
  printf("%.12f %.12f\n",p.x,p.y);

  int i;
  for (i=0; i<1000000; i++){
    cnv.frw(p);
    cnv.bck(p);
  }
  printf("%.12f %.12f\n",p.x,p.y);

}