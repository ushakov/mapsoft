#include <vector>
#include <cmath>

#include "layer_trk.h"
#include "utils/cairo_wrapper.h"
#include "2d/line_utils.h"

using namespace std;

LayerTRK::LayerTRK(g_track * _data):
      data(_data), mymap(get_myref()),
      cnv(get_myref(), Datum("wgs84"), Proj("lonlat")),
      myrange(rect_pump(cnv.bb_bck(data->range()), 1.0)){
#ifdef DEBUG_LAYER_TRK
  cerr  << "LayerTRK: set_ref range: " << myrange << "\n";
#endif
}

void
LayerTRK::refresh(){
  myrange = rect_pump(cnv.bb_bck(data->range()), 1.0);
}

g_map
LayerTRK::get_ref() const {
  return mymap;
}

convs::map2pt
LayerTRK::get_cnv() const{
  return cnv;
}

g_map
LayerTRK::get_myref() const {
  g_map ret;
  ret.map_proj = Proj("lonlat");
  ret.push_back(g_refpoint(0,  45, 0, 45*3600));
  ret.push_back(g_refpoint(180, 0, 180*3600,0));
  ret.push_back(g_refpoint(0,   0, 0, 90*3600));
  return ret;
}

void
LayerTRK::set_ref(const g_map & map){
  mymap=map;
  cnv = convs::map2pt(mymap, Datum("wgs84"), Proj("lonlat"));
  refresh();
#ifdef DEBUG_LAYER_TRK
  cerr  << "LayerTRK: set_ref range: " << myrange << "\n";
#endif
}

iImage
LayerTRK::get_image (iRect src){
  if (rect_intersect(myrange, src).empty()) return iImage(0,0);
  iImage ret(src.w, src.h, 0);
  draw(src.TLC(), ret);
  return ret;
}

void
LayerTRK::draw(const iPoint origin, iImage & image){
  iRect src_rect = image.range() + origin;
#ifdef DEBUG_LAYER_TRK
  cerr  << "LayerTRK: draw " << src_rect <<  " my: " << myrange << "\n";
#endif
  // FIXME - use correct range instead of +110
  if (rect_intersect(myrange, rect_pump(src_rect,110)).empty()) return;

  CairoWrapper cr(image);
  cr->set_line_cap(Cairo::LINE_CAP_ROUND);

  int w = data->width;
  int arr_w = w * 2.0;
  int dot_w = w * 0.5;
  int arr_dist = w * 10; // distance between arrows
  Color color = data->color;

  cr->set_color(color.value);
  cr->set_line_width(w);

  dPoint po;
  vector<g_trackpoint>::const_iterator pt;
  double len_cnt;
  for (pt = data->begin(); pt!= data->end(); pt++){
    dPoint p(*pt); cnv.bck(p);  p-=origin;
    if (pt == data->begin()) po=p;

    iRect r = rect_pump(iRect(p,po), w);

    if (!rect_intersect(r, image.range()).empty()){
      if ((pt == data->begin()) || pt->start){
        cr->circle(p, dot_w);
        len_cnt=0;
      }
      else {
        cr->move_to(po);
        cr->line_to(p);

        if (len_cnt < arr_dist){ // draw dot
          cr->circle(p, dot_w);
          len_cnt+=pdist(po-p);
        }
        else { // draw arrow
          dPoint dp = pnorm(po-p) * arr_w;
          dPoint p1 = p + dp + dPoint(dp.y, -dp.x) * 0.5;
          dPoint p2 = p + dp - dPoint(dp.y, -dp.x) * 0.5;
          cr->move_to(p);
          cr->line_to(p1);
          cr->line_to(p2);
          cr->line_to(p);
          len_cnt=0;
        }
      }
    }
    po=p;
  }
  cr->stroke();
}

int
LayerTRK::find_trackpoint (iPoint pt, int radius){
  for (int n = 0; n < data->size(); ++n) {
    dPoint p((*data)[n]);
    cnv.bck(p);
    if (pdist(p,dPoint(pt))<radius) return n;
  }
  return -1;
}

int
LayerTRK::find_track (iPoint pt, int radius){
  int ts=data->size();

  if (ts<1) return -1;
  if (ts<2){
    if (pdist((*data)[0],dPoint(pt))<radius) return 0;
    else return -1;
  }

  for (int n = 0; n < ts-1; ++n) {
    dPoint p1((*data)[n]), p2((*data)[n+1]);
    cnv.bck(p1); cnv.bck(p2);
    dPoint v1 = pnorm(p2-p1) * pscal(dPoint(pt)-p1, p2-p1)/pdist(p2-p1);

    if ((pdist(v1) < 0)||(pdist(v1)>pdist(p2-p1))) continue;
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
