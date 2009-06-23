#ifndef VMAP_PARTS_H
#define VMAP_PARTS_H

#include "../../core/libgeo/geo_convs.h"
#include "vmap.h"

namespace vmap {

dRect find_safe_region(world & w, const Rect<double> r, Datum D, Proj P, Options O){
  convs::pt2pt cnv (D, P, O, Datum("wgs84"), Proj("lonlat"), Options());
  dRect ret=r;
  for (std::map<id_t, object>::const_iterator i=w.objects.begin(); i!=w.objects.end(); i++){
    for (dMultiLine::const_iterator l=i->second.data.begin(); l!=i->second.data.end(); l++){
      for (dLine::const_iterator p=l->begin(); p!=l->end(); p++){
        dPoint P=*p;
        cnv.bck(P);
        if (!point_in_rect(P,r)) continue;
        if (p!=l->begin()){ P=*(p-1); cnv.bck(P); rect_pump(ret, P); }
        if (p+1!=l->end()){ P=*(p+1); cnv.bck(P); rect_pump(ret, P); }
      }
    }
  }
  return ret;
}

fig::fig_world get_fig(const world & w, const dRect r, Datum D, Proj P, Options O){
  dRect r2=find_safe_region(w, r, D, P, O);
  
}

} // namespace
#endif
