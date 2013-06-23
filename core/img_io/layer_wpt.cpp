#include <vector>
#include <cmath>

#include "layer_wpt.h"
#include "layers/draw_wpt.h"

using namespace std;

LayerWPT::LayerWPT (g_waypoint_list * _data) : data(_data) {}

void
LayerWPT::refresh(){
  myrange = cnv? iRect(rect_pump(cnv->bb_bck(data->range()), 1.0)) :
                 iRect();
}

g_map
LayerWPT::get_myref() const {
  g_map ret;
  ret.map_proj = Proj("lonlat");
  ret.push_back(g_refpoint(0,  45, 0, 45*3600));
  ret.push_back(g_refpoint(180, 0, 180*3600,90*3600));
  ret.push_back(g_refpoint(0,   0, 0, 90*3600));
  return ret;
}

int
LayerWPT::draw(iImage & image, const iPoint & origin){
  iRect src_rect = image.range() + origin;
#ifdef DEBUG_LAYER_WPT
  cerr  << "LayerWPT: draw " << src_rect <<  " my: " << myrange << "\n";
#endif
  if (rect_intersect(rect_pump(myrange,110), src_rect).empty())
    return GOBJ_FILL_NONE;

  draw_wpt(image, origin, *cnv, *data, Options());
  return GOBJ_FILL_PART;
}

int
LayerWPT::find_waypoint (iPoint pt, int radius) {
  iRect target_rect (pt,pt);
  target_rect = rect_pump(target_rect, radius);
  for (int wpt = 0; wpt < data->size(); ++wpt) {
    dPoint p((*data)[wpt]);
    cnv->bck(p);
    if (pdist(dPoint(pt),p)<radius) return wpt;
  }
  return -1;
}

vector<int>
LayerWPT::find_waypoints (const iRect & r){
  vector<int> ret;
  for (int n = 0; n < data->size(); ++n) {
    dPoint p((*data)[n]);
    cnv->bck(p);
    if (point_in_rect(p, dRect(r))) ret.push_back(n);
  }
  return ret;
}

g_waypoint_list *
LayerWPT::get_data() const{
  return data;
}

g_waypoint *
LayerWPT::get_pt(const int n) const{
  return &(*data)[n];
}

iRect
LayerWPT::range() const{
  return myrange;
}
