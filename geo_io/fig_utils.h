#ifndef FIG_UTILS_H
#define FIG_UTILS_H

#include "fig.h"
#include "../utils/rect_crop.h"


namespace fig {

  void rect_crop(const Rect<int> & cutter, fig_world & W){
    fig_world::iterator o = W.begin();
    while (o!=W.end()){
      Line<int> l = *o;
      ::rect_crop(cutter, l, o->is_closed() || (o->area_fill != -1));
      o->set_points(l);
      if (o->size() == 0) o=W.erase(o);
      else o++;
    }
  }


}
#endif
