#include "gobj_srtm.h"
#include <sstream>
#include <fstream>

#include "2d/rainbow.h"

using namespace std;

GObjSRTM::GObjSRTM(SRTM3 *srtm):S(srtm){
  opt.put<string>("srtm_mode", "normal");
  opt.put<double>("srtm_cnt_step", 50);
  opt.put<double>("srtm_hmin", 0.0);
  opt.put<double>("srtm_hmax", 5000.0);
  opt.put<double>("srtm_smin", 30.0);
  opt.put<double>("srtm_smax", 55.0);
  opt.put<string>("srtm_dir",  "");
}

int
GObjSRTM::draw(iImage & image, const iPoint & origin){
  if (S==NULL) return GObj::FILL_NONE;

  S->set_dir(opt.get<string>("srtm_dir", ""));

  string mode  = opt.get<string>("srtm_mode", "normal");
  int cnt_step = opt.get<int>("srtm_cnt_step",  50);

  // avoid string comparing strings inside the loop:
  int m=0;
  if      (mode == "none")   m=0;
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
      cnv.frw(p0);

      h0 = S->geth4(p0, false);
      if (h0 < srtm_min){
        continue;
      }
      if (cnt_step>0){ // contours
        px = origin + iPoint(i+1,j);
        py = origin + iPoint(i,j+1);
        cnv.frw(px);
        cnv.frw(py);
        hx = S->geth4(px, false);
        hy = S->geth4(py, false);

        // holes
        if (hx < srtm_min || hy < srtm_min){
          continue;
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
  return GObj::FILL_ALL;
}

