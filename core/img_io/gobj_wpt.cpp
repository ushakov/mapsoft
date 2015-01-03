#include <vector>
#include <cmath>

#include "gobj_wpt.h"
#include "img_io/draw_wpt.h"

using namespace std;

int
GObjWPT::draw(iImage & image, const iPoint & origin){
  iRect src_rect = image.range() + origin;
#ifdef DEBUG_LAYER_WPT
  cerr  << "GObjWPT: draw " << src_rect <<  " my: " << myrange << "\n";
#endif
  if (rect_intersect(rect_pump(myrange,110), src_rect).empty())
    return GOBJ_FILL_NONE;

  draw_wpt(image, origin, cnv, *data, Options());
  return GOBJ_FILL_PART;
}

void
GObjWPT::refresh(){ /// Refresh layer -- update myrange
  myrange = wpts_range(cnv, *data, Options());
}


int
GObjWPT::find_waypoint (iPoint pt, int radius) {
  iRect target_rect (pt,pt);
  target_rect = rect_pump(target_rect, radius);
  for (int wpt = 0; wpt < data->size(); ++wpt) {
    dPoint p((*data)[wpt]);
    cnv.bck(p);
    if (pdist(dPoint(pt),p)<radius) return wpt;
  }
  return -1;
}

vector<int>
GObjWPT::find_waypoints (const iRect & r){
  vector<int> ret;
  for (int n = 0; n < data->size(); ++n) {
    dPoint p((*data)[n]);
    cnv.bck(p);
    if (point_in_rect(p, dRect(r))) ret.push_back(n);
  }
  return ret;
}

