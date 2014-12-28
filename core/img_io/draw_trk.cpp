#include <vector>
#include <cmath>

#include "draw_trk.h"
#include "utils/cairo_wrapper.h"
#include "2d/line_utils.h"
#include "2d/rainbow.h"

using namespace std;

void
draw_trk(iImage & image, const iPoint & origin,
         const Conv & cnv, const g_track & trk,
         const Options & opt){

  CairoWrapper cr(image);
  cr->set_line_cap(Cairo::LINE_CAP_ROUND);

  int w = trk.width;
  int arr_w = w * 2.0;
  int dot_w = w * 0.5;
  int arr_dist = w * 10; // minimal segment with arrow

  string trk_draw_mode = opt.get<string>("trk_draw_mode", "normal");

  if (trk_draw_mode == "normal"){
    bool draw_dots   = opt.exists("trk_draw_dots");
    bool draw_arrows = opt.exists("trk_draw_arrows");
    cr->set_color(trk.color);
    cr->set_line_width(w);

    if (trk.size()==1){ // draw 1pt tracks even in non-draw_dots mode
      g_trackpoint p1(trk[0]);
      cnv.bck(p1);  p1-=origin;
      cr->move_to(p1);
      cr->circle(p1, dot_w);
    }

    for (int i=1; i<trk.size(); i++){
      g_trackpoint p1(trk[i-1]);
      g_trackpoint p2(trk[i]);
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

    if (trk.size()==1){ // draw 1pt tracks even in non-draw_dots mode
      g_trackpoint p1(trk[0]);
      cnv.bck(p1);  p1-=origin;
      cr->set_color(0);
      cr->move_to(p1);
      cr->circle(p1, dot_w);
    }

    for (int i=1; i<trk.size(); i++){
      g_trackpoint p1(trk[i-1]);
      g_trackpoint p2(trk[i]);
      if (p2.start) continue;

      double cc = cos((p1.y + p2.y)/2.0/180.0*M_PI);
      double dx = (p2.x - p1.x)*cc;
      double dy = (p2.y - p1.y);
      double d = sqrt(dx*dx + dy*dy) * 6380e3 * M_PI/180;

      cnv.bck(p1);  p1-=origin;
      cnv.bck(p2);  p2-=origin;

      double t = p2.t.value - p1.t.value;
      if (t>0 && t<3600*24) cr->set_color(sr.get(d/t * 3.6));
      else cr->set_color(0);

      cr->move_to(p1);
      cr->line_to(p2);
      cr->stroke();
    }
  }

  if (trk_draw_mode == "height"){
    double trk_height1 = opt.get<double>("trk_draw_h1", -200);
    double trk_height2 = opt.get<double>("trk_draw_h2", 8000);
    cr->set_line_width(2*w);
    simple_rainbow sr(trk_height1, trk_height2);

    if (trk.size()==1){ // draw 1pt tracks even in non-draw_dots mode
      g_trackpoint p1(trk[0]);
      cnv.bck(p1);  p1-=origin;
      cr->set_color(0);
      cr->move_to(p1);
      cr->circle(p1, dot_w);
    }

    for (int i=1; i<trk.size(); i++){
      g_trackpoint p1(trk[i-1]);
      g_trackpoint p2(trk[i]);
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
        int n = ceil(pdist(p2-p1)/2/w);
        dPoint dp = (p2-p1)/n;

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

