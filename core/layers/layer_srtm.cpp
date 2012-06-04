#include "layer_srtm.h"
#include <sstream>
#include <fstream>

#include "geo/geo_refs.h"
#include "2d/rainbow.h"

using namespace std;

LayerSRTM::LayerSRTM(srtm3 *srtm):S(srtm){
  mymap=get_myref();
  opt.put<string>("srtm_mode", "normal");
  opt.put<bool>("srtm_on",  "false");
  opt.put<double>("srtm_cnt_step", 50);
  opt.put<double>("srtm_hmin", 0.0);
  opt.put<double>("srtm_hmax", 5000.0);
  opt.put<double>("srtm_smin", 30.0);
  opt.put<double>("srtm_smax", 55.0);
}

void
LayerSRTM::set_opt(const Options & o){
  opt = o;
}

Options
LayerSRTM::get_opt(void) const{
  return opt;
}

g_map
LayerSRTM::get_ref() const {
  return mymap;
}

g_map
LayerSRTM::get_myref() const {
//  return ref_ll(180*1200); // 1200pt/degree
  g_map ret;
  ret.map_proj = Proj("lonlat");
  ret.push_back(g_refpoint(0,  45, 0, 45*1200));
  ret.push_back(g_refpoint(180, 0, 180*1200,90*1200));
  ret.push_back(g_refpoint(0,   0, 0, 90*1200));
  return ret;
}

void
LayerSRTM::set_ref(const g_map & map){
  mymap=map;
}

//iImage
//LayerSRTM::get_image (iRect src){
//  if (rect_intersect(myrange, src).empty()) return iImage(0,0);
//  iImage ret(src.w, src.h, 0);
//  draw(src.TLC(), ret);
//  return ret;
//}

void
LayerSRTM::draw(const iPoint origin, iImage & image){
  if (S==NULL) return;
  iRect src_rect = image.range() + origin;

  string mode  = opt.get<string>("srtm_mode", "normal");
  int cnt_step = opt.get<int>("srtm_cnt_step",  50);

  double min=0, max=0;
  rainbow_type type = RAINBOW_NORMAL;
  if (mode == "normal"){
    min = opt.get<double>("srtm_hmin", 0.0);
    max = opt.get<double>("srtm_hmax", 5000.0);
  }
  else if (mode == "slopes"){
    type=RAINBOW_BURNING;
    min = opt.get<double>("srtm_smin", 30.0);
    max = opt.get<double>("srtm_smax", 55.0);
  }

  simple_rainbow R(min,max,type), RG(0,90,0xffffff,0x00000);
  convs::map2pt cnv(mymap, Datum("wgs84"),Proj("lonlat"),Options());

  for (int j=0; j<image.h; j++){
    for (int i=0; i<image.w; i++){
      int c=0xffffff;
      dPoint p0 = origin + iPoint(i,j);
      dPoint px=p0 + dPoint(1,0);
      dPoint py=p0 + dPoint(0,1);

      cnv.frw(p0);
      cnv.frw(px);
      cnv.frw(py);
      int h0 = S->geth4(p0, false);
      int hx = S->geth4(px, false);
      int hy = S->geth4(py, false);

      // holes
      if ((h0 < srtm_min) || (hx < srtm_min) || (hy < srtm_min)){
        c=0xC8C8C8; goto print_colors;
      }

      // contours
      if ((cnt_step>0) && ((hx/cnt_step - h0/cnt_step) ||(hy/cnt_step - h0/cnt_step))){
        c=0; goto print_colors;
      }

      if (mode == "normal"){
        double a = S->slope4(p0, false);
        if (a>90.0) a=90.0;
        c = R.get(h0);
        c = color_shade(c, 1-a/90.0);
        goto print_colors;
      }

      if (mode == "slopes"){ // slopes
        c=R.get(S->slope4(p0, false));
        goto print_colors;
      }

      print_colors:
      image.set_na(i,j, c);
    }
  }
}

iRect
LayerSRTM::range() const{
  return iRect(
    iPoint(INT_MIN, INT_MIN),
    iPoint(INT_MAX, INT_MAX));
}

