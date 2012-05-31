#include <vector>
#include <cmath>

#include "layer_trk.h"
#include "utils/cairo_wrapper.h"
#include "2d/line_utils.h"
#include "2d/rainbow.h"

using namespace std;

LayerTRK::LayerTRK(g_track * _data, const Options & o):
      data(_data), mymap(get_myref()),
      cnv(get_myref(), Datum("wgs84"), Proj("lonlat")),
      myrange(rect_pump(cnv.bb_bck(data->range()), 1.0)),
      opt(o) {

#ifdef DEBUG_LAYER_TRK
  cerr  << "LayerTRK: set_ref range: " << myrange << "\n";
#endif
}

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
  ret.push_back(g_refpoint(180, 0, 180*3600,90*3600));
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
  int arr_dist = w * 10; // minimal segment with arrow

  string trk_draw_mode = opt.get<string>("trk_draw_mode", "normal");

  if (trk_draw_mode == "normal"){
    bool draw_dots   = opt.exists("trk_draw_dots");
    bool draw_arrows = opt.exists("trk_draw_arrows");
    cr->set_color(data->color.value);
    cr->set_line_width(w);
    for (int i=1; i<data->size(); i++){
      g_trackpoint p1((*data)[i-1]);
      g_trackpoint p2((*data)[i]);
      if (rect_intersect(
             rect_pump(iRect(p1,p2), w),
             image.range()).empty()) continue;
      if (p2.start) continue;

      cnv.bck(p1);  p1-=origin;
      cnv.bck(p2);  p2-=origin;

      cr->move_to(p1);
      cr->line_to(p2);

      if (draw_dots){
        if ((i==1) || p1.start) cr->circle(p1, dot_w);
        cr->circle(p2, dot_w);
      }

      if (draw_arrows && (pdist(p1-p2) > arr_dist)){
        dPoint p0=(p1+p2)/2;
        dPoint dp = pnorm(p1-p2) * arr_w;
        dPoint p3 = p0 + dp + dPoint(dp.y, -dp.x) * 0.5;
        dPoint p4 = p0 + dp - dPoint(dp.y, -dp.x) * 0.5;
        cr->move_to(p0);
        cr->line_to(p3);
        cr->line_to(p4);
        cr->line_to(p0);
      }

    }
    cr->stroke();
  }

  if (trk_draw_mode == "speed"){
    double trk_speed1 = opt.get<double>("trk_draw_v1", 0);
    double trk_speed2 = opt.get<double>("trk_draw_v2", 10);
    cr->set_line_width(2*w);
    simple_rainbow sr(trk_speed1, trk_speed2);
    int k = 1;
    for (int i=1; i<data->size(); i++){
      g_trackpoint p1((*data)[i-k]);
      g_trackpoint p2((*data)[i]);
      if (rect_intersect(
             rect_pump(iRect(p1,p2), w),
             image.range()).empty()) continue;
      if (p2.start) continue;

      double cc = cos((p1.y + p2.y)/2.0/180.0*M_PI);
      double dx = (p2.x - p1.x)*cc;
      double dy = (p2.y - p1.y);
      double d = sqrt(dx*dx + dy*dy) * 6380e3 * M_PI/180;

      cnv.bck(p1);  p1-=origin;
      cnv.bck(p2);  p2-=origin;
      if (pdist(p2-p1) < 2*w) { k++; continue;} // skip points

      double t = p2.t.value - p1.t.value;
      if (t>0 && t<3600*24) cr->set_color(sr.get(d/t * 3.6));
      else cr->set_color(0);

      cr->move_to(p1);
      cr->line_to(p2);
      cr->stroke();
      k=1;
    }
  }

  if (trk_draw_mode == "height"){
    double trk_height1 = opt.get<double>("trk_draw_h1", -200);
    double trk_height2 = opt.get<double>("trk_draw_h2", 8000);
    cr->set_line_width(2*w);
    simple_rainbow sr(trk_height1, trk_height2);
    for (int i=1; i<data->size(); i++){
      g_trackpoint p1((*data)[i-1]);
      g_trackpoint p2((*data)[i]);
      if (rect_intersect(
             rect_pump(iRect(p1,p2), w),
             image.range()).empty()) continue;
      if (p2.start) continue;

      cnv.bck(p1);  p1-=origin;
      cnv.bck(p2);  p2-=origin;


      if (!p1.have_alt() || !p2.have_alt()){
        cr->set_color(0);
        cr->move_to(p1);
        cr->line_to(p2);
        cr->stroke();
      }
      else {
        dPoint dp = pnorm(p2-p1)*2*w;
        int n = ceil(pdist(p2-p1)/2/w);

        for(int j=0; j< n; j++){
          cr->move_to(p1 + j*dp);
          cr->line_to(p1 + (j+1)*dp);
          cr->set_color(sr.get( p1.z + double(j*(p2.z - p1.z))/n ));
          cr->stroke();
        }
      }
    }
  }

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

vector<int>
LayerTRK::find_trackpoints (const iRect & r){
  vector<int> ret;
  for (int n = 0; n < data->size(); ++n) {
    dPoint p((*data)[n]);
    cnv.bck(p);
    if (point_in_rect(p, dRect(r))) ret.push_back(n);
  }
  return ret;
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
