#include <iostream>
#include <string>
#include <cassert>

#include <list>

#include "2d/line.h"
#include "2d/rect.h"
#include "2d/line_utils.h"

#include "geo/geo_refs.h"
#include "geo/geo_convs.h"

dRect l1r(36.0,55.0, 0.1, 0.1);
dRect l2r(35.9,55.0, 0.1, 0.1);

dLine l1 = rect2line(l1r);
dLine l2 = rect2line(l2r);

int
main(){
  std::cerr << l1 << "\n";
  std::cerr << l2 << "\n";

  for (int i=0; i<8; i++){
    dLine l = i%2? l1:l2;
    bool swap = (i/2)%2;
    int  unit = (i/4)%2? 1:100;

    g_map m1 = mk_tmerc_ref (l, unit, swap);
    convs::map2wgs cnv(m1);
    std::cout << "lon0: " << convs::lon2lon0(l.center().x) << " "
              << "unit: " << unit << " m "
              << "spap y: " << swap << "\n";
    for (double a=-90; a<90; a+=45){
      double ac  = cnv.angd_frw(l.center(), a, 10);
      double acc = cnv.angd_bck(l.center(), ac, 0.001);

      std::cout << "  " << a << " " <<  ac << " " << acc << "\n";
    }
  }

}
