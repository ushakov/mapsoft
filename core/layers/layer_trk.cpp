#include <vector>
#include <cmath>

#include "layer_trk.h"
#include "layers/draw_trk.h"

using namespace std;

LayerTRK::LayerTRK(g_track * _data, const Options & o):
      data(_data), opt(o) { }

void
LayerTRK::set_opt(const Options & o){
  opt = o;
}

Options
LayerTRK::get_opt(void) const{
  return opt;
}

void
LayerTRK::refresh(){
  myrange = cnv? iRect(rect_pump(cnv->bb_bck(data->range()), 1.0)) :
                 iRect();
}

g_map
LayerTRK::get_myref() const {
  g_map ret;
  ret.map_proj = Proj("lonlat");
  ret.push_back(g_refpoint(0,  45, 0, 45*3600));
  ret.push_back(g_refpoint(180, 0, 180*3600,90*3600));
  ret.push_back(g_refpoint(0,   0, 0, 90*3600));
  return ret;
}

int
LayerTRK::draw(iImage & image, const iPoint & origin){
  iRect src_rect = image.range() + origin;
#ifdef DEBUG_LAYER_TRK
  cerr  << "LayerTRK: draw " << src_rect <<  " my: " << myrange << "\n";
#endif
  // FIXME - use correct range instead of +110
  if (rect_intersect(myrange, rect_pump(src_rect,110)).empty())
    return GOBJ_FILL_NONE;

  draw_trk(image, origin, *cnv, *data, opt);
  return GOBJ_FILL_PART;
}

int
LayerTRK::find_trackpoint (iPoint pt, int radius){
  for (int n = 0; n < data->size(); ++n) {
    dPoint p((*data)[n]);
    cnv->bck(p);
    if (pdist(p,dPoint(pt))<radius) return n;
  }
  return -1;
}

vector<int>
LayerTRK::find_trackpoints (const iRect & r){
  vector<int> ret;
  for (int n = 0; n < data->size(); ++n) {
    dPoint p((*data)[n]);
    cnv->bck(p);
    if (point_in_rect(p, dRect(r))) ret.push_back(n);
  }
  return ret;
}


int
LayerTRK::find_track (iPoint pt, int radius){
  int ts=data->size();

  if (ts<1) return -1;
  if (ts<2){
    dPoint p((*data)[0]);
    cnv->bck(p);
    if (pdist(p,dPoint(pt))<radius) return 0;
    else return -1;
  }

  for (int n = 0; n < ts-1; ++n) {
    dPoint p1((*data)[n]), p2((*data)[n+1]);
    cnv->bck(p1); cnv->bck(p2);
    double vn = pscal(dPoint(pt)-p1, p2-p1)/pdist(p2-p1);
    dPoint v1 = pnorm(p2-p1) * vn;

    if (vn < -radius || vn > pdist(p2-p1) + radius) continue;
    if (pdist(dPoint(pt)-p1, v1) < radius) return n;
  }
  return -1;
}

g_track *
LayerTRK::get_data() const{
  return data;
}

g_trackpoint *
LayerTRK::get_pt(const int n) const{
  return &(*data)[n];
}

iRect
LayerTRK::range() const{
  return myrange;
}

