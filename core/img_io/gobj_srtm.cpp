#include "gobj_srtm.h"
#include <sstream>
#include <fstream>

#include "2d/rainbow.h"

using namespace std;

GObjSRTM::GObjSRTM(srtm3 *srtm):S(srtm){
  opt.put<string>("srtm_mode", "normal");
  opt.put<bool>("srtm_on",  "false");
  opt.put<double>("srtm_cnt_step", 50);
  opt.put<double>("srtm_hmin", 0.0);
  opt.put<double>("srtm_hmax", 5000.0);
  opt.put<double>("srtm_smin", 30.0);
  opt.put<double>("srtm_smax", 55.0);
}

void
GObjSRTM::set_opt(const Options & o){
  opt = o;
}

Options
GObjSRTM::get_opt(void) const{
  return opt;
}

g_map
GObjSRTM::get_myref() const {
//  return ref_ll(180*1200); // 1200pt/degree
  g_map ret;
  ret.map_proj = Proj("lonlat");
  ret.push_back(g_refpoint(0,  45, 0, 45*1200));
  ret.push_back(g_refpoint(180, 0, 180*1200,90*1200));
  ret.push_back(g_refpoint(0,   0, 0, 90*1200));
  return ret;
}

int
GObjSRTM::draw(iImage & image, const iPoint & origin){
  if (S==NULL) return GOBJ_FILL_NONE;
  iRect src_rect = image.range() + origin;

  string mode  = opt.get<string>("srtm_mode", "normal");
  int cnt_step = opt.get<int>("srtm_cnt_step",  50);

  // avoid string comparing strings inside the loop:
  int m=0;
  if      (mode == "normal") m=1;
  else if (mode == "slopes") m=2;

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

  for (int j=0; j<image.h; j++){
    for (int i=0; i<image.w; i++){
      int c=0xffffff;
      dPoint p0,px,py;
      int h0,hx,hy;

      p0 = origin + iPoint(i,j);
      cnv->frw(p0);

      h0 = S->geth4(p0, false);
      if (h0 < srtm_min){
        c=0xC8C8C8; goto print_colors;
      }

      if (cnt_step>0){ // contours
        px = origin + iPoint(i+1,j);
        py = origin + iPoint(i,j+1);
        cnv->frw(px);
        cnv->frw(py);
        hx = S->geth4(px, false);
        hy = S->geth4(py, false);

        // holes
        if (hx < srtm_min || hy < srtm_min){
          c=0xC8C8C8; goto print_colors;
        }

        if ((hx/cnt_step - h0/cnt_step) ||(hy/cnt_step - h0/cnt_step)){
          c=0; goto print_colors;
        }
      }

      if (m == 1){
        double a = S->slope4(p0, false);
        if (a>90.0) a=90.0;
        c = R.get(h0);
        c = color_shade(c, 1-a/90.0);
        goto print_colors;
      }

      if (m == 2){ // slopes
        c=R.get(S->slope4(p0, false));
        goto print_colors;
      }

      print_colors:
      image.set_na(i,j, c);
    }
  }
  return GOBJ_FILL_ALL;
}

iRect
GObjSRTM::range() const{
  return iRect(
    iPoint(INT_MIN, INT_MIN),
    iPoint(INT_MAX, INT_MAX));
}
