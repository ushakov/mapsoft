#include <vector>
#include <cmath>

#include "gobj_trk.h"
#include "draw_trk.h"

using namespace std;

int
GObjTRK::draw(iImage & image, const iPoint & origin){
  iRect src_rect = image.range() + origin;
#ifdef DEBUG_LAYER_TRK
  cerr  << "GObjTRK: draw " << src_rect <<  " my: " << myrange << "\n";
#endif
  // FIXME - use correct range instead of +110
  if (rect_intersect(myrange, rect_pump(src_rect,110)).empty())
    return GOBJ_FILL_NONE;

  draw_trk(image, origin, cnv, *data, opt);
  return GOBJ_FILL_PART;
}

int
GObjTRK::find_trackpoint (dPoint pt, double radius){
  for (int n = 0; n < data->size(); ++n) {
    dPoint p((*data)[n]);
    cnv.bck(p);
    if (pdist(p,pt)<radius) return n;
  }
  return -1;
}

vector<int>
GObjTRK::find_trackpoints (const dRect & r){
  vector<int> ret;
  for (int n = 0; n < data->size(); ++n) {
    dPoint p((*data)[n]);
    cnv.bck(p);
    if (point_in_rect(p, r)) ret.push_back(n);
  }
  return ret;
}


int
GObjTRK::find_track (dPoint pt, double radius){
  int ts=data->size();

  if (ts<1) return -1;
  if (ts<2){
    dPoint p((*data)[0]);
    cnv.bck(p);
    if (pdist(p,pt)<radius) return 0;
    else return -1;
  }

  for (int n = 0; n < ts-1; ++n) {
    dPoint p1((*data)[n]), p2((*data)[n+1]);
    cnv.bck(p1); cnv.bck(p2);
    double vn = pscal(pt-p1, p2-p1)/pdist(p2-p1);
    dPoint v1 = pnorm(p2-p1) * vn;

    if (vn < -radius || vn > pdist(p2-p1) + radius) continue;
    if (pdist(pt-p1, v1) < radius) return n;
  }
  return -1;
}


